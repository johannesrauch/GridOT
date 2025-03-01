// dotmark.h

#ifndef DOTMARK_H
#define DOTMARK_H

#include <benchmark/solvers.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace benchmark {

class DOTmark {
 public:
  DOTmark(const std::string& data_path, int runs = 1, int dim = 0)
      : _data_path(data_path),
        _runs(runs),
        _dim(dim),
        _filename_pattern(R"(data(\d+)_1(\d+)\.csv)")  // Matches pattern like
                                                       // "data512_1006.csv") {}
  {
    fs::path output_directory = fs::path(_data_path).parent_path() / "Results";
    if (!fs::exists(output_directory)) fs::create_directory(output_directory);
    std::cout << "Results are stored into " << output_directory << std::endl;
    _output_file =
        std::ofstream(output_directory / "benchmark_output.txt", std::ios::app);
    _output_file_detailed = std::ofstream(
        output_directory / "benchmark_output_detailed.txt", std::ios::app);
    _statistics_file = std::ofstream(
        output_directory / "benchmark_statistics_gridOT.txt", std::ios::app);

    // Open the file stream in append mode to ensure we can add to it
    if (!_output_file.is_open() || !_output_file_detailed.is_open() ||
        !_statistics_file.is_open()) {
      throw std::runtime_error("Failed to open output file.");
    }
  }

  ~DOTmark() {
    if (_output_file.is_open()) _output_file.close();
    if (_output_file_detailed.is_open()) _output_file_detailed.close();
    if (_statistics_file.is_open()) _statistics_file.close();
  }

  // Load the class directories and their image file paths, organized by
  // resolution
  void loadData() {
    std::smatch match;

    for (const auto& class_entry : fs::directory_iterator(_data_path)) {
      if (class_entry.is_directory()) {
        std::string class_name = class_entry.path().filename().string();

        // Gather all CSV files within this class directory and organize by
        // resolution
        for (const auto& file_entry :
             fs::directory_iterator(class_entry.path())) {
          std::string filename = file_entry.path().filename().string();

          if (std::regex_match(filename, match, _filename_pattern)) {
            int resolution = std::stoi(
                match[1].str());  // Resolution extracted from filename
            // If _dim is given, only store images with resolution _dim
            if (!_dim || resolution == _dim) {
              std::string filepath = file_entry.path().string();

              // Store the image path under its class and resolution
              _class_images[class_name][resolution].push_back(filepath);
              _resolutions.insert(resolution);
            }
          }
        }
      }
    }
  }

  // Run benchmark for all pairs of images within each class
  void runBenchmark() {
    if (_dim)
      printOutput("%d runs per pair; resolution = %d\n", _runs, _dim);
    else
      printOutput("%d runs per pair; all resolutions\n", _runs);
    printOutput("%7s%17s%3s%3s%4s %9s%11s\n", "dim", "class", "i", "j", "opt",
                "obj", "time [ms]");
    for (int i = 0; i < 55; ++i) printOutput("-");
    printOutput("\n");
    for (const int res : _resolutions) {
      for (const auto& [class_name, class_resolutions] : _class_images) {
        auto it = class_resolutions.find(res);
        if (it != class_resolutions.end()) {
          const auto& images = it->second;
          long double t = 0, t_ref = 0;
          bool optimal = true, optimal_ref = true;
          int n = 0;
          for (size_t i = 0; i < images.size(); ++i) {
            for (size_t j = 0; j < images.size(); ++j) {
              if (i == j) continue;

              auto [opt, t_ij, opt_ref, t_ref_ij] =
                  benchmarkPair(class_name, res, images[i], images[j]);
              t += t_ij;
              t_ref += t_ref_ij;
              optimal &= opt;
              optimal_ref &= opt_ref;
              ++n;
            }
          }
          printOutput("%7d%17s%6s%4d %9s%11.1f   GridOT\n", res, class_name, "",
                      optimal, "", t / n);
          printOutput("%7d%17s%6s%4d %9s%11.1f   MultiScaleOT\n", res,
                      class_name, "", optimal_ref, "", t_ref / n);
        }
      }
    }
  }

 private:
  const std::string _data_path;  // Path to DOTmark data
  const int _runs;
  const int _dim;
  const std::regex _filename_pattern;
  std::set<int> _resolutions;
  std::map<std::string, std::map<int, std::vector<std::string>>>
      _class_images;  // class -> resolution -> list of image paths
  std::ofstream _output_file;
  std::ofstream _output_file_detailed;
  std::ofstream _statistics_file;

  // Helper function to print to both the console and the file
  template <typename... Args>
  void printOutput(const char* format, Args... args) {
    fmt::printf(format, args...);
    fmt::printf(_output_file, format, args...);
    fmt::printf(_output_file_detailed, format, args...);
    _output_file.flush();
    _output_file_detailed.flush();
  }

  // Load CSV file as vector
  ValueVector loadCSV(const std::string& filename) {
    ValueVector data;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
      std::istringstream line_stream(line);
      std::string cell;

      // Read each cell in the line, split by comma
      while (std::getline(line_stream, cell, ',')) {
        data.push_back(std::stoi(cell));
      }
    }

    return data;
  }

  // Reads two csv files as signed supplies in a contiguous vector
  ValueVector loadSupply(const std::string& source, const std::string& target) {
    ValueVector supply = loadCSV(source);
    ValueVector demand = loadCSV(target);

    // Increment supply and demand by one to avoid zero entries 
    // which Schmitzer cannot handle
    for (auto& v : supply) ++v;
    for (auto& v : demand) ++v;

    ValueVector signed_supply = supply;
    for (Value v : demand) signed_supply.push_back(-v);

    return signed_supply;
  }

  void print_statistics(const int resolution, const std::string& class_name,
                        const int i, const int j,
                        const std::vector<std::vector<double>>& densities) {
    fmt::printf(_statistics_file, "%s (%dx%d): %d->%d:\n", class_name,
                resolution, resolution, i, j);
    for (const auto& level : densities) {
      for (const auto& density : level) {
        fmt::printf(_statistics_file, "%9.3g ", density);
      }
      fmt::printf(_statistics_file, "\n");
    }
    _statistics_file.flush();
  }

  // Benchmark function to run OT on an image pair
  std::tuple<bool, long double, bool, long double> benchmarkPair(
      const std::string& class_name, int resolution, const std::string& image1,
      const std::string& image2) {
    ValueVector supply = loadSupply(image1, image2);
    assert(supply.size() == 2 * resolution * resolution);

    // Measure time taken by the solver
    long double t = 0, t_ref = 0;
    TotalCost obj = 0, obj_ref = 0;
    bool optimal = true, optimal_ref = true;
    std::vector<std::vector<double>> densities;
    for (int it = 0; it < _runs; ++it) {
      std::array<int, 2> dim = {resolution, resolution};
      Graph graph(dim, dim, supply);
      auto [res, new_densities] = gridSolver(graph);
      t += res.t_ms;
      optimal &= res.return_value == GridSolver::NetSimplex::OPTIMAL;
      if (obj) assert(obj == res.objective_value);
      obj = res.objective_value;
      densities = new_densities;

      Results resRef = schmitzerMultiScale(
          dim.data(), supply, lemon::utils::hierarchicalDepth(dim, dim, 2) + 1);
      t_ref += resRef.t_ms;
      optimal_ref &= resRef.return_value;
      if (obj_ref) assert(obj_ref == resRef.objective_value);
      obj_ref = resRef.objective_value;
    }
    if (obj < 0) {
      fmt::printf("Integer overflow!!!\n");
    }

    // get image number
    int i, j;
    std::smatch match;
    std::filesystem::path path1(image1);
    std::filesystem::path path2(image2);
    std::string name1 = path1.filename().string();
    std::string name2 = path2.filename().string();
    std::regex_match(name1, match, _filename_pattern);
    i = std::stoi(match[2].str());
    std::regex_match(name2, match, _filename_pattern);
    j = std::stoi(match[2].str());

    fmt::printf(_output_file_detailed,
                "%7d%17s%3d%3d%4d %9.3g%11.1f   GridOT\n", resolution,
                class_name, i, j, optimal, (double)obj, t / _runs);
    fmt::printf(_output_file_detailed,
                "%7d%17s%3d%3d%4d %9.3g%11.1f   MultiScaleOT\n", resolution,
                class_name, i, j, optimal_ref, (double)obj_ref, t_ref / _runs);
    _output_file_detailed.flush();
    print_statistics(resolution, class_name, i, j, densities);
    return std::make_tuple(optimal, t / _runs, optimal_ref, t_ref / _runs);
  }
};

}  // namespace benchmark

#endif  // DOTMARK_H
