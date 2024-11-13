// dotmark.hpp

#ifndef DOTMARK_HPP
#define DOTMARK_HPP

#include <ulmon/test/instance.h>
#include <ulmon/ulm_grid_graph.h>
#include <ulmon/ulm_grid_solver.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <vector>

using namespace lemon;
using namespace lemon::test;

namespace fs = std::filesystem;

using Graph = UlmGridGraph<Value, Cost>;
using GridSolver = UlmGridSolver<Graph>;

class DOTmark {
 public:
  DOTmark(const std::string& dataPath)
      : dataPath(dataPath),
        filenamePattern(R"(data(\d+)_1(\d+)\.csv)")  // Matches pattern like
                                                     // "data512_1006.csv") {}
  {}

  // Load the class directories and their image file paths, organized by
  // resolution
  void loadData() {
    std::smatch match;

    for (const auto& classEntry : fs::directory_iterator(dataPath)) {
      if (classEntry.is_directory()) {
        std::string className = classEntry.path().filename().string();

        // Gather all CSV files within this class directory and organize by
        // resolution
        for (const auto& fileEntry :
             fs::directory_iterator(classEntry.path())) {
          std::string filename = fileEntry.path().filename().string();

          if (std::regex_match(filename, match, filenamePattern)) {
            int resolution = std::stoi(
                match[1].str());  // Resolution extracted from filename
            std::string filepath = fileEntry.path().string();

            // Store the image path under its class and resolution
            classImages[className][resolution].push_back(filepath);
            resolutions.insert(resolution);
          }
        }
      }
    }
  }

  // Run benchmark for all pairs of images within each class
  void runBenchmark() {
    fmt::printf("%7s%15s%3s%3s%4s %9s%10s\n", "dim", "class", "i", "j", "opt",
                "obj", "time [ms]");
    for (const int res : resolutions) {
      for (const auto& [className, classResolutions] : classImages) {
        const auto& images = classResolutions.at(res);
        for (size_t i = 0; i < images.size(); ++i) {
          for (size_t j = 0; j < images.size(); ++j) {
            benchmarkPair(className, res, images[i], images[j]);
          }
        }
      }
    }

    /// TODO: save results
  }

 private:
  std::string dataPath;  // Path to DOTmark data
  std::regex filenamePattern;
  std::set<int> resolutions;
  std::map<std::string, std::map<int, std::vector<std::string>>>
      classImages;  // class -> resolution -> list of image paths

  // Load CSV file as vector
  ValueVector loadCSV(const std::string& filename) {
    ValueVector data;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
      std::istringstream lineStream(line);
      std::string cell;

      // Read each cell in the line, split by comma
      while (std::getline(lineStream, cell, ',')) {
        data.push_back(std::stoi(cell));
      }
    }

    return data;
  }

  // Reads two csv files as signed supplies in a contiguous vector
  ValueVector loadSupply(const std::string& source, const std::string& target) {
    ValueVector supply = loadCSV(source);
    ValueVector demand = loadCSV(target);

    ValueVector signedSupply = supply;
    for (Value v : demand) signedSupply.push_back(-v);

    return signedSupply;
  }

  // Benchmark function to run OT on each image pair
  void benchmarkPair(const std::string& className, int resolution,
                     const std::string& image1, const std::string& image2) {
    ValueVector supply = loadSupply(image1, image2);
    assert(supply.size() == 2 * resolution * resolution);

    // Measure time taken by the solver
    Int2Array dim = {resolution, resolution};
    Graph graph(dim, dim, supply);
    GridSolver solver(graph);
    Results res;
    res.tic();
    res.return_value = solver.run();
    res.toc();
    res.objective_value = solver.totalCost();
    bool optimal = res.return_value == GridSolver::NetSimplex::OPTIMAL;
    if (res.objective_value < 0) {
      fmt::printf("Integer overflow!!!\n");
    }

    // get image number
    int i, j;
    std::smatch match;
    std::filesystem::path pathObj1(image1);
    std::filesystem::path pathObj2(image2);
    std::string name1 = pathObj1.filename().string();
    std::string name2 = pathObj2.filename().string();
    std::regex_match(name1, match, filenamePattern);
    i = std::stoi(match[2].str());
    std::regex_match(name2, match, filenamePattern);
    j = std::stoi(match[2].str());

    fmt::printf("%7d%15s%3d%3d%4d %9.3g%10.1f\n", resolution, className, i, j,
                optimal, (double)res.objective_value, res.t_ms);
  }
};

#endif  // DOTMARK_HPP
