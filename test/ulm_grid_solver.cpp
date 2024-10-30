#include <ulmon/ulm_grid_solver.h>
#include <ulmon/ulm_grid_graph.h>
#include <ulmon/test/instance.h>

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

using namespace lemon;
using namespace lemon::test;

using Graph = UlmGridGraph<>;
using Graph = UlmGridGraph<>;
using TestSolver = UlmGridSolver<Graph>;
using TestSubsolver = UlmNetworkSimplex<Graph>;

/// \brief Test subsolve method
void testSubsolve(const Int2Array dims) {
  // Grid dimensions
  const int n = dims[0] * dims[1];

  long double t_ref = 0, t_test = 0;
  bool ok = true;
  for (int it = 0; it < ULMON_CONST_IT; ++it) {
    fmt::printf(".");
    std::flush(std::cout);

    // Marginals
    ValueVector supply = getRandomSupply(n, n);

    // Reference
    Graph refG(dims, dims, supply, true);
    typename Graph::SupplyNodeMap supplyMap(refG);
    typename Graph::CostArcMap costMap(refG);
    Results r = lemonBS(refG, supplyMap, costMap);

    // Test
    Graph testG(dims, dims, supply, true);
    testG.reserveArcs(n);
    for (int i = 0; i < n; ++i) testG.addArc(testG.redNode(i), testG.blueNode(i));
    TestSolver testS(testG);
    TestSubsolver testSub(testG);
    Results t;
    t.tic();
    t.return_value = testS.subsolve(testG, testSub);
    t.toc();
    t.objective_value = testSub.totalCost();

    // Bookkeeping
    assert(r.objective_value == t.objective_value);
    assert(r.return_value == t.return_value);
    t_ref += r.t_ms;
    t_test += t.t_ms;
    ok &= r.objective_value == t.objective_value;
  }

  t_ref /= ULMON_CONST_IT, t_test /= ULMON_CONST_IT;
  fmt::printf("\r%7s%3dx%3d%7d%7.1f%7.1f%7d\n", "sub", dims[0], dims[1], ULMON_CONST_IT,
              t_ref, t_test, ok);
}

/// \brief Test run method
void testRun(const Int2Array dims) {
  // Grid dimensions
  const int n = dims[0] * dims[1];

  long double t_ref = 0, t_test = 0;
  bool ok = true;
  for (int it = 0; it < ULMON_CONST_IT; ++it) {
    fmt::printf(".");
    std::flush(std::cout);

    // Marginals
    ValueVector supply;
    setupSupply(n, n, supply);

    // Reference
    Graph refG(dims, dims, supply, true);
    typename Graph::SupplyNodeMap supplyMap(refG);
    typename Graph::CostArcMap costMap(refG);
    Results r = lemonBS(refG, supplyMap, costMap);

    // Test
    Graph graph(dims, dims, supply);
    TestSolver testS(graph);
    Results t;
    t.tic();
    t.return_value = testS.run();
    t.toc();
    t.objective_value = testS.totalCost();

    // Bookkeeping
    assert(r.objective_value == t.objective_value);
    t_ref += r.t_ms;
    t_test += t.t_ms;
    ok &= r.objective_value == t.objective_value;
  }

  t_ref /= ULMON_CONST_IT, t_test /= ULMON_CONST_IT;
  fmt::printf("\r%7s%3dx%3d%7d%7.1f%7.1f%7d\n", "run", dims[0], dims[1],
              ULMON_CONST_IT, t_ref, t_test, ok);
}

int main(int argc, char** argv) {
  fmt::printf("%7s%7s%7s%7s%7s%7s\n", "test", "dims", "iter", "t_ref", "t_test",
              "ok");
  for (int i = 0; i < 43; ++i) fmt::printf("-");
  fmt::printf("\n");

  Int2Array dims{};
  if (argc >= 2) {
    int d = std::atoi(argv[1]);
    dims = {d, d};
    testSubsolve(dims);
    testRun(dims);
  } else {
    for (int d = 8; d <= ULMON_CONST_D; d += ULMON_CONST_DELTA) {
      dims = {d, d};
      testSubsolve(dims);
    }
    for (int d = 8; d <= ULMON_CONST_D; d += ULMON_CONST_DELTA) {
      dims = {d, d};
      testRun(dims);
    }
  }
  return 0;
}
