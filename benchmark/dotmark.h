// dotmark.h

#ifndef DOTMARK_H
#define DOTMARK_H

#include <ulmon/ulm_grid_graph.h>
#include <ulmon/ulm_grid_solver.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <vector>

using namespace lemon;

namespace fs = std::filesystem;

using Value = int;  // Supply / demand type, signed
using Cost = int;   // Cost type, signed
using TotalCost = long int;
using ValueVector = std::vector<Value>;

struct Results {
  TotalCost objective_value{0};
  int return_value{0};
  long double t_ms{0};
  std::clock_t t0;

  Results() : t0(std::clock()) {}

  void tic() { t0 = std::clock(); }

  /**
   * @brief Returns and saves the cpu time in ms since the construction of the
   * object or the latest call to tic().
   *
   * @return long double Cpu time in ms
   */
  long double toc() {
    t_ms =
        static_cast<long double>(1000.) * (std::clock() - t0) / CLOCKS_PER_SEC;
    return t_ms;
  }
};

using Graph = UlmGridGraph<Value, Cost>;
using GridSolver = UlmGridSolver<Graph>;

class DOTmark {
 public:
  DOTmark(const std::string& data_path, int runs = 1, int dim = 0)
      : _data_path(data_path),
        _runs(runs),
        _dim(dim),
        _filename_pattern(R"(data(\d+)_1(\d+)\.csv)")  // Matches pattern like
                                                       // "data512_1006.csv") {}
  {}

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
  void runBenchmark(bool print_all_pairs = false) {
    if (_dim)
      fmt::printf("%d runs per pair; resolution = %d\n", _runs, _dim);
    else
      fmt::printf("%d runs per pair; all resolutions\n", _runs);
    fmt::printf("%7s%17s%3s%3s%4s %9s%11s\n", "dim", "class", "i", "j", "opt",
                "obj", "time [ms]");
    for (int i = 0; i < 55; ++i) fmt::printf("-");
    fmt::printf("\n");
    for (const int res : _resolutions) {
      for (const auto& [class_name, class_resolutions] : _class_images) {
        auto it = class_resolutions.find(res);
        if (it != class_resolutions.end()) {
          const auto& images = it->second;
          long double t = 0;
          bool optimal = true;
          int n = 0;
          for (size_t i = 0; i < images.size(); ++i) {
            for (size_t j = 0; j < images.size(); ++j) {
              auto [opt, t_ij] = benchmarkPair(class_name, res, images[i],
                                               images[j], print_all_pairs);
              t += t_ij;
              optimal &= opt;
              ++n;
            }
          }
          fmt::printf("%7d%17s%6s%4d %9s%11.1f\n", res, class_name, "", optimal,
                      "", t / n);
        }
      }
    }

    /// TODO: save results
  }

 private:
  const std::string _data_path;  // Path to DOTmark data
  const int _runs;
  const int _dim;
  const std::regex _filename_pattern;
  std::set<int> _resolutions;
  std::map<std::string, std::map<int, std::vector<std::string>>>
      _class_images;  // class -> resolution -> list of image paths

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

    ValueVector signed_supply = supply;
    for (Value v : demand) signed_supply.push_back(-v);

    return signed_supply;
  }

  // Benchmark function to run OT on an image pair
  std::pair<bool, long double> benchmarkPair(const std::string& class_name,
                                             int resolution,
                                             const std::string& image1,
                                             const std::string& image2,
                                             bool print_all_pairs = false) {
    ValueVector supply = loadSupply(image1, image2);
    assert(supply.size() == 2 * resolution * resolution);

    // Measure time taken by the solver
    long double t = 0;
    bool optimal = true;
    Results res;
    for (int it = 0; it < _runs; ++it) {
      std::array<int, 2> dim = {resolution, resolution};
      Graph graph(dim, dim, supply);
      GridSolver solver(graph);
      res.tic();
      res.return_value = solver.run();
      res.toc();
      t += res.t_ms;
      optimal &= res.return_value == GridSolver::NetSimplex::OPTIMAL;
      if (res.objective_value)
        assert(res.objective_value == solver.totalCost<TotalCost>());
      res.objective_value = solver.totalCost<TotalCost>();
    }
    if (res.objective_value < 0) {
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

    if (print_all_pairs) {
      fmt::printf("%7d%17s%3d%3d%4d %9.3g%11.1f\n", resolution, class_name, i,
                  j, optimal, (double)res.objective_value, t / _runs);
    }
    return std::make_pair(optimal, t / _runs);
  }
};

#endif  // DOTMARK_H
