#ifndef ULMON_TEST_INSTANCE_H
#define ULMON_TEST_INSTANCE_H

#include <lemon/network_simplex.h>

#include <cassert>
#include <fmt/printf.hpp>
#include <random>
#include <set>
#include <vector>

namespace lemon {

namespace test {

using Value = int;  // Supply / demand type, signed
using Cost = int;   // Cost type, signed
static constexpr int Dim = 2;
using Int2Array = std::array<int, Dim>;
using ValueVector = std::vector<Value>;
using CostVector = std::vector<Cost>;
using PosVector = std::vector<Int2Array>;

template <typename TotalCost = Cost>
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

std::mt19937 mt(0);  // Randomness engine

//
// Random instance
//

/**
 * @brief Computes total (integer) supply, should be quite larger than max(nx,
 * ny)
 */
inline Value upscaledTotalSupply(const int& nx, const int& ny) {
  return (nx + ny) * 1000;
}

/**
 * @brief Fills the space between iterators out_it and out_end uniformly with
 * random numbers between 1 and totalSupply-1 such that they sum up to
 * totalSupply.
 *
 * @tparam It Non-constant iterator
 * @param out_it Begin
 * @param out_end End
 */
template <typename It>
void fillSupply(Value totalSupply, It out_it, It out_end,
                const double density) {
  std::size_t n = std::distance(out_it, out_end);
  std::size_t npos = std::distance(out_it, out_end) * density;
  std::set<Value> set;  // To generate distinct values
  set.insert(0);
  set.insert(totalSupply);
  std::uniform_int_distribution<Value> dist(1, totalSupply - 1);
  while (set.size() <= npos) set.insert(dist(mt));

  std::vector<size_t> indices(n);  // generate distinct indices with supply > 0
  std::iota(indices.begin(), indices.end(), 0);
  std::shuffle(indices.begin(), indices.end(), mt);

  std::fill(out_it, out_end, Value{});
  Value sum = 0, sub = 0;
  auto it = set.begin();
  for (std::size_t i = 0; i < npos; ++i) {
    ++it;
    assert(it != set.end());
    std::size_t ind = indices[i];
    out_it[ind] = *it - sub;
    sum += out_it[ind];
    sub = *it;
    assert(out_it[ind] >= 0);
  }
  assert(sum == totalSupply);
}

/**
 * @brief Sets up the supplies. supply[0..nx-1] is filled with nonnegative
 * values (supply) that sum up to totalSupply = (nx + ny) * 1000,
 * supply[nx..nx+ny-1] is filled with nonpositive values (demand) that sum up to
 * -totalSupply.
 *
 * The idea is to first fill a vector with random integers in the range
 * 0..totalSupply. We understand these integers as indices that partition the
 * range 0..totalSupply; the desired random values are then the interval
 * lengths.
 *
 * @param nx Number of supply nodes
 * @param ny Number of demand nodes
 * @param supply Supply vector that gets resized and filled
 */
void setupSupply(const int nx, const int ny, std::vector<Value>& supply,
                 const double density) {
  const int n = nx + ny;
  assert(nx >= 0 && ny >= 0 && n >= 0);
  supply.resize(n);
  Value totalSupply = upscaledTotalSupply(nx, ny);
  fillSupply(totalSupply, supply.begin(), supply.begin() + nx, density);
  fillSupply(totalSupply, supply.begin() + nx, supply.end(), density);
  for (auto it = supply.begin() + nx; it != supply.end(); ++it) *it *= -1;
}

ValueVector getRandomSupply(const int nx, const int ny, const double density) {
  ValueVector supply;
  setupSupply(nx, ny, supply, density);
  return supply;
}

//
// Lemon reference
//

template <typename Graph, typename SupplyMap, typename CostMap>
Results<> lemonBS(const Graph& graph, const SupplyMap& supplyMap,
                  const CostMap& costMap) {
  using NetSimplex = NetworkSimplex<Graph, Value, Cost>;
  NetSimplex net(graph);
  net.supplyMap(supplyMap).costMap(costMap);

  Results results;
  results.tic();
  results.return_value = net.run(NetSimplex::BLOCK_SEARCH);
  assert(results.return_value == NetSimplex::OPTIMAL);
  results.toc();
  results.objective_value = net.totalCost();

  return results;
}

}  // namespace test

}  // namespace lemon

#endif
