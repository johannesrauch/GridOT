#include <lemon/network_simplex.h>
#include <ulmon/test/instance.h>
#include <ulmon/ulm_grid_graph.h>
#include <ulmon/ulm_network_simplex.h>

#ifndef ULMON_CONST_D
#ifndef NDEBUG
#define ULMON_CONST_D 16
#else
#define ULMON_CONST_D 32
#endif
#endif

#ifndef ULMON_CONST_DELTA
#ifndef NDEBUG
#define ULMON_CONST_DELTA 1
#else
#define ULMON_CONST_DELTA 4
#endif
#endif

#ifndef ULMON_CONST_IT
#define ULMON_CONST_IT 5
#endif

#ifndef ULMON_CONST_DENSITY
#define ULMON_CONST_DENSITY .4
#endif

using namespace lemon;
using namespace lemon::test;

using Graph = UlmGridGraph<Value, Cost>;
using SupplyNodeMap = typename Graph::SupplyNodeMap;
using CostArcMap = typename Graph::CostArcMap;

using Test = UlmNetworkSimplex<Graph, Value, Cost>;

/// \brief Test shielded pivot rule on fully bipartite graphs
void testShielded1(const int n) {
  fmt::printf("testShielded1(%d):\t", n);
  using Ref = NetworkSimplex<Graph>;

  // Dimensions of grid
  Int2Array muXdim = {n, n};
  Int2Array muYdim = {n, n};
  int nx = muXdim[0] * muXdim[1];
  int ny = muYdim[0] * muYdim[1];

  for (int it = 0; it < ULMON_CONST_IT; ++it) {
    fmt::printf(".");

    // Marginals
    ValueVector supply = getRandomSupply(nx, ny, ULMON_CONST_DENSITY);

    // Reference
    Graph refG(muXdim, muYdim, supply, true);
    CostArcMap refCost(refG);
    SupplyNodeMap refSupply(refG);
    Ref ref(refG);
    ref.supplyMap(refSupply).costMap(refCost);
    typename Ref::ProblemType r = ref.run();
    assert(r == Ref::OPTIMAL);

    // Test
    Graph testG(muXdim, muYdim, supply, true);
    CostArcMap testCost(testG);
    SupplyNodeMap testSupply(testG);
    Test test(testG, false);
    test.supplyMap(testSupply).costMap(testCost);
    typename Test::ProblemType t = test.runShielded();
    assert(t == Test::OPTIMAL);

    assert(ref.totalCost() == test.totalCost());
  }

  fmt::printf("OK\n");
}

/// \brief Test shielded pivot rule on incomplete bipartite graphs
void testShielded2(const int n) {
  fmt::printf("testShielded2(%d):\t", n);
  using Ref = NetworkSimplex<Graph>;

  // Dimensions of grid
  Int2Array muXdim = {n, n};
  Int2Array muYdim = {n, n};
  int nx = muXdim[0] * muXdim[1];
  int ny = muYdim[0] * muYdim[1];

  Int2Array pos{};
  PosVector yMin, yMax;
  for (int i = 0; i < nx; ++i) {
    yMin.push_back(pos);
    yMax.push_back({pos[0] + 1, pos[1] + 1});
    utils::advancePos(muYdim, pos);
  }

  for (int it = 0; it < ULMON_CONST_IT; ++it) {
    fmt::printf(".");
    std::flush(std::cout);

    // Marginals
    ValueVector supply;
    setupSupply(nx, ny, supply, ULMON_CONST_DENSITY);

    Graph refG(muXdim, muYdim, supply, true);
    SupplyNodeMap refSupply(refG);
    CostArcMap refCost(refG);
    Ref ref(refG);
    ref.supplyMap(refSupply).costMap(refCost);
    typename Ref::ProblemType r = ref.run();
    assert(r == Ref::OPTIMAL);

    Graph testG(muXdim, muYdim, supply, yMin, yMax);
    Test test(testG);
    SupplyNodeMap testSupply(testG);
    CostArcMap testCost(testG);
    test.supplyMap(testSupply).costMap(testCost);
    typename Test::ProblemType t = test.runShielded();
    assert(t == Test::OPTIMAL);

    assert(ref.totalCost() == test.totalCost());
  }

  fmt::printf("OK\n", n);
}

int main() {
  for (int d = ULMON_CONST_D / 4; d <= ULMON_CONST_D; d += ULMON_CONST_DELTA) {
    testShielded1(d);
  }
  for (int d = ULMON_CONST_D / 4; d <= ULMON_CONST_D; d += ULMON_CONST_DELTA) {
    testShielded2(d);
  }
  return 0;
}
