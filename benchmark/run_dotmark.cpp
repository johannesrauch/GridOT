// run_benchmark.cpp

#include <benchmark/dotmark.hpp>

// Main function to run the benchmark
int main(int argc, char** argv) {
  const char* dataDirectory;
  if (argc >= 2) {
    dataDirectory = argv[1];
  } else {
#ifdef BENCHMARK_DATA_DIRECTORY
    dataDirectory = BENCHMARK_DATA_DIRECTORY;
#else
    std::cerr << "Usage: " << argv[0] << " <directory path to csv-data>\n";
    return 1;
#endif
  }

  DOTmark benchmark(dataDirectory);
  benchmark.loadData();
  benchmark.runBenchmark();

  return 0;
}
