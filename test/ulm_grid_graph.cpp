#include <lemon/network_simplex.h>
#include <ulmon/test/instance.h>
#include <ulmon/ulm_grid_graph.h>

using namespace lemon;
using namespace lemon::test;

using Graph = UlmGridGraph<>;
TEMPLATE_BPDIGRAPH_TYPEDEFS(Graph);
using Ref = NetworkSimplex<Graph, typename Graph::Value, typename Graph::Cost>;

void testCtor1() {
  fmt::printf("testCtor1:\t\t");

  // Dimensions of grid
  Int2Array muXdim = {8, 8};
  Int2Array muYdim = {8, 8};
  int nx = muXdim[0] * muXdim[1];
  int ny = muYdim[0] * muYdim[1];

  // Marginals
  ValueVector supply = test::getRandomSupply(nx, ny);
  Graph graph(muXdim, muYdim, supply, true);

  // Test costs
  Graph::CostArcMap costMap(graph);
  assert(costMap[graph.arcFromId(0)] == 0);
  assert(costMap[graph.arcFromId(1)] == 1);
  assert(costMap[graph.arcFromId(2)] == 4);
  assert(costMap[graph.arcFromId(3)] == 9);
  assert(costMap[graph.arcFromId(8)] == 1);
  assert(costMap[graph.arcFromId(9)] == 2);
  assert(costMap[graph.arcFromId(10)] == 5);

  fmt::printf("OK\n");
}

void testCtor2() {
  fmt::printf("testCtor2:\t\t");

  // Dimensions of grid
  Int2Array muXdim = {2, 2};
  Int2Array muYdim = {2, 2};
  int nx = muXdim[0] * muXdim[1];
  int ny = muYdim[0] * muYdim[1];

  // Marginals
  ValueVector supply = test::getRandomSupply(nx, ny);
  PosVector yMin = {{0, 0}, {0, 0}, {0, 0}, {0, 1}};
  PosVector yMax = {{2, 2}, {1, 1}, {1, 1}, {2, 2}};
  Graph graph(muXdim, muYdim, supply, yMin, yMax);

  // Test costs
  Graph::CostArcMap costMap(graph);
  assert(countArcs(graph) == 8);
  assert(costMap[graph.arcFromId(0)] == 0);
  assert(costMap[graph.arcFromId(1)] == 1);
  assert(costMap[graph.arcFromId(2)] == 1);
  assert(costMap[graph.arcFromId(3)] == 2);
  assert(costMap[graph.arcFromId(4)] == 1);
  assert(costMap[graph.arcFromId(5)] == 1);
  assert(costMap[graph.arcFromId(6)] == 1);
  assert(costMap[graph.arcFromId(7)] == 0);

  fmt::printf("OK\n");
}

void testCtor3AddArcs() {
  fmt::printf("testCtor3AddArcs:\t");

  // Dimensions of grid
  Int2Array muXdim = {5, 6};
  Int2Array muYdim = {7, 8};
  int nx = muXdim[0] * muXdim[1];
  int ny = muYdim[0] * muYdim[1];

  // Marginals
  ValueVector supply = test::getRandomSupply(nx, ny);
  Graph graph(muXdim, muYdim, supply, true);
  Graph coarse(graph, 2);

  assert(countArcs(graph) == muXdim[0] * muXdim[1] * muYdim[0] * muYdim[1]);
  assert(countArcs(coarse) == 0);
  assert(coarse._x_dim[0] == 3);
  assert(coarse._x_dim[1] == 3);
  assert(coarse._y_dim[0] == 4);
  assert(coarse._y_dim[1] == 4);

  Int2Array x_min{0, 0}, x_max{2, 2}, y_min{1, 1}, y_max{4, 4};
  coarse.addArcs(x_min, x_max, y_min, y_max);
  assert(countArcs(coarse) == 36);

  using RedNode = typename Graph::RedNode;
  assert(coarse.source(coarse.arcFromId(0), RedNode{}) == coarse.redNode(0));
  assert(coarse.source(coarse.arcFromId(1), RedNode{}) == coarse.redNode(0));
  assert(coarse.source(coarse.arcFromId(2), RedNode{}) == coarse.redNode(0));
  assert(coarse.source(coarse.arcFromId(9), RedNode{}) == coarse.redNode(1));

  using BlueNode = typename Graph::BlueNode;
  assert(coarse.target(coarse.arcFromId(0), BlueNode{}) == coarse.blueNode(5));
  assert(coarse.target(coarse.arcFromId(1), BlueNode{}) == coarse.blueNode(6));
  assert(coarse.target(coarse.arcFromId(2), BlueNode{}) == coarse.blueNode(7));
  assert(coarse.target(coarse.arcFromId(3), BlueNode{}) == coarse.blueNode(9));

  coarse.clearArcs();
  assert(countArcs(coarse) == 0);

  fmt::printf("OK\n");
}

void testCtor3Supply() {
  fmt::printf("testCtor3Supply:\t");
  // Dimensions of grid
  Int2Array muXdim = {8, 8};
  Int2Array muYdim = {8, 8};
  int nx = muXdim[0] * muXdim[1];
  int ny = muYdim[0] * muYdim[1];

  // Marginals
  ValueVector supply = test::getRandomSupply(nx, ny);
  Graph graph(muXdim, muYdim, supply, true);
  Graph coarse(graph, 2);
  coarse.addAllArcs();

  Ref solver(coarse);
  typename Graph::SupplyNodeMap supplyMap(coarse);
  typename Graph::CostArcMap costMap(coarse);
  typename Ref::ProblemType r =
      solver.supplyMap(supplyMap).costMap(costMap).run();

  assert(r == Ref::OPTIMAL);
  assert(solver.totalCost());

  fmt::printf("OK\n");
}

void testRebuildShield1() {
  fmt::printf("testRebuildShield1:\t");
  constexpr int n = 7;

  // Dimensions of grid
  Int2Array muXdim = {n, n};
  Int2Array muYdim = {n, n};
  int nx = muXdim[0] * muXdim[1];
  int ny = muYdim[0] * muYdim[1];
  assert(nx == ny);  // For this test

  // Marginals
  ValueVector supply = test::getRandomSupply(nx, ny);
  Graph graph(muXdim, muYdim, supply);

  assert(countArcs(graph) == 0);
  using SupportVector = typename Graph::SupportVector;
  SupportVector support;
  for (int i = 0; i < n * n; ++i)
    support.emplace_back(graph.redNode(i), graph.blueNode(i));

  graph.rebuildShield(support);
  assert(countOutArcs(graph, graph.redNode(0)) == 4);
  assert(countOutArcs(graph, graph.redNode(6)) == 4);
  assert(countOutArcs(graph, graph.redNode(42)) == 4);
  assert(countOutArcs(graph, graph.redNode(48)) == 4);
  for (int i = 1; i < n - 1; ++i)
    assert(countOutArcs(graph, graph.redNode(i)) == 6);
  for (int i = 7; i < (n - 1) * n; i += n)
    assert(countOutArcs(graph, graph.redNode(i)) == 6);
  for (int i = 13; i < n * n - 1; i += n)
    assert(countOutArcs(graph, graph.redNode(i)) == 6);
  for (int i = 43; i < n * n - 1; ++i)
    assert(countOutArcs(graph, graph.redNode(i)) == 6);
  for (int i = 8; i < 2 * n - 1; ++i)
    assert(countOutArcs(graph, graph.redNode(i)) == 9);
  assert(countArcs(graph) == 4 * 4 + 4 * (n - 2) * 6 + (n - 2) * (n - 2) * 9);

  graph.clearArcs();
  assert(countArcs(graph) == 0);
  fmt::printf("OK\n");
}

void testRebuildShield2() {
  fmt::printf("testRebuildShield2:\t");
  constexpr int n = 4;

  // Dimensions of grid
  Int2Array muXdim = {n, n};
  Int2Array muYdim = {n, n};
  int nx = muXdim[0] * muXdim[1];
  int ny = muYdim[0] * muYdim[1];
  assert(nx == ny);  // For this test

  // Marginals
  ValueVector supply = test::getRandomSupply(nx, ny);
  Graph graph(muXdim, muYdim, supply);

  // Support
  assert(countArcs(graph) == 0);
  using SupportVector = typename Graph::SupportVector;
  SupportVector support;
  for (int i = 0; i < n * n; ++i)
    support.emplace_back(graph.redNode(i), graph.blueNode(i));
  support.emplace_back(graph.redNode(5), graph.blueNode(7));

  graph.rebuildShield(support);
  using ArcIt = typename Graph::ArcIt;
  assert(graph.source(ArcIt{graph}) == graph.nodeFromId(5));
  assert(graph.target(ArcIt{graph}) == graph.nodeFromId(7 + n * n));

  assert(countArcs(graph) ==
         4 * 4 + 4 * (n - 2) * 6 + (n - 2) * (n - 2) * 9 - 6 + 2);
  fmt::printf("OK\n");
}

void testRebuildShield3() {
  fmt::printf("testRebuildShield3:\t");
  constexpr int n = 7;
  assert(n >= 2);  // For this test

  // Dimensions of grid
  Int2Array muXdim = {n, n};
  Int2Array muYdim = {n, n};
  int nx = muXdim[0] * muXdim[1];
  int ny = muYdim[0] * muYdim[1];
  assert(nx == ny);  // For this test

  // Marginals
  ValueVector supply = test::getRandomSupply(nx, ny);
  Graph graph(muXdim, muYdim, supply);

  // Support
  assert(countArcs(graph) == 0);
  using SupportVector = typename Graph::SupportVector;
  SupportVector support;
  for (int i = 0; i < n * n; ++i)
    support.emplace_back(graph.redNode(i), graph.blueNode(i));
  ValueVector support_flow(support.size(), 1);

  typename Graph::ArcVector support_arcs;
  graph.rebuildShield(support, support_flow, support_arcs);
  assert(support.size() == support_arcs.size());
  assert(countArcs(graph) == 4 * 4 + 4 * (n - 2) * 6 + (n - 2) * (n - 2) * 9);

  assert(support_arcs.size() == n * n);
  assert(support_arcs[0] == graph.arcFromId(0));
  assert(support_arcs[1] == graph.arcFromId(5));
  assert(support_arcs[2] == graph.arcFromId(11));
  assert(support_arcs[n] == graph.arcFromId(2 * 4 + (n - 2) * 6 + 2));

  for (std::size_t i = 0; i < support.size(); ++i) {
    assert(support[i].first == graph.source(support_arcs[i], RedNode{}));
    assert(support[i].second == graph.target(support_arcs[i], BlueNode{}));
  }

  fmt::printf("OK\n");
}

int main() {
  testCtor1();
  testCtor2();
  testCtor3AddArcs();
  testCtor3Supply();
  testRebuildShield1();
  testRebuildShield2();
  testRebuildShield3();
  return 0;
}
