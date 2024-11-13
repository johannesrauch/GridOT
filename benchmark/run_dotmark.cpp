// run_benchmark.cpp

#include <benchmark/dotmark.hpp>

// Main function to run the benchmark
int main(int argc, char** argv) {
  int runs = 5;        // runs per pair
  int resolution = 0;  // 0: run all resolutions

  const char* data_directory;
  int data_arg = 0;

#ifdef BENCHMARK_DATA_DIRECTORY
  data_directory = BENCHMARK_DATA_DIRECTORY;
#else
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <directory path to csv-data> ...\n";
    return 1;
  }
  data_directory = argv[1];
  data_arg = 1;
#endif

  if (argc >= 2 + data_arg) runs = std::atoi(argv[1 + data_arg]);
  if (argc >= 3 + data_arg) resolution = std::atoi(argv[2 + data_arg]);

  DOTmark benchmark(data_directory, runs, resolution);
  benchmark.loadData();
  benchmark.runBenchmark();

  return 0;
}
