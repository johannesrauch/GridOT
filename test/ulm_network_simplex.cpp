#include <lemon/network_simplex.h>
#include <ulmon/test/instance.h>
#include <ulmon/ulm_grid_graph.h>
#include <ulmon/ulm_network_simplex.h>

#ifndef ULMON_GRID_DIM
#define ULMON_GRID_DIM 8
#endif

using namespace lemon;

using Value = int;
using Cost = int;
using ValueVector = std::vector<Value>;
using Int2Array = std::array<int, 2>;

using Graph = UlmGridGraph<Value, Cost>;
using SupplyNodeMap = typename Graph::SupplyNodeMap;
using CostArcMap = typename Graph::CostArcMap;

using Ref = NetworkSimplex<Graph, Value, Cost>;
using Test = UlmNetworkSimplex<Graph, Value, Cost>;

TEMPLATE_BPDIGRAPH_TYPEDEFS(Graph);

/// \brief Set LB and UB for feasible flow in the GEQ problem
void makeBoundsForGeq(Graph& graph, SupplyNodeMap& supply, IntArcMap& upper,
                      IntArcMap& lower) {
  for (RedNodeIt s(graph); s != INVALID; ++s) {
    OutArcIt a(graph, s);
    assert(a != INVALID);
    // Set lower bound of one out arc to 1 and hope that this is feasible, i.e.
    // no demand is oversatisfied
    lower[OutArcIt(graph, s)] = 1;
    // Set all outgoing upper bounds to supply/4 and hope that this is feasible,
    // i.e. all of the supply can be distributed
    Value sup = supply[s];
    for (; a != INVALID; ++a) {
      upper[a] = sup / 4;
    }
  }
}

/// \brief Set LB and UB for feasible flow in the LEQ problem
void makeBoundsForLeq(Graph& graph, SupplyNodeMap& supply, IntArcMap& upper,
                      IntArcMap& lower) {
  for (BlueNodeIt t(graph); t != INVALID; ++t) {
    InArcIt a(graph, t);
    assert(a != INVALID);
    // Set lower bound of one in arc to 1 and hope that this is feasible, i.e.
    // no supply is oversatisfied
    lower[a] = 1;
    // Set all incoming upper bounds to demand/2 and hope that this is feasible,
    // i.e. the demand can be satisfied
    Value dem = -supply[t];
    for (; a != INVALID; ++a) {
      upper[a] = dem / 2;
    }
  }
}

void testSolver(Graph& graph, SupplyNodeMap& supplyMap, CostArcMap& costMap,
                Ref::SupplyType refType, Test::SupplyType testType) {
  // Solver
  using Ref = NetworkSimplex<Graph, Value, Cost>;
  using Test = UlmNetworkSimplex<Graph, Value, Cost>;
  Ref ref(graph);
  Test test(graph);

  ref.supplyMap(supplyMap).supplyType(refType).costMap(costMap).run(
      Ref::FIRST_ELIGIBLE);
  test.supplyMap(supplyMap).supplyType(testType).costMap(costMap).run(
      Test::FIRST_ELIGIBLE);
  if (ref.totalCost() != test.totalCost()) {
    fmt::printf("Bug in FIRST_ELIGIBLE\n");
    std::abort();
  }

  ref.reset().supplyMap(supplyMap).supplyType(refType).costMap(costMap).run(
      Ref::BEST_ELIGIBLE);
  test.reset().supplyMap(supplyMap).supplyType(testType).costMap(costMap).run(
      Test::BEST_ELIGIBLE);
  if (ref.totalCost() != test.totalCost()) {
    fmt::printf("Bug in BEST_ELIGIBLE\n");
    std::abort();
  }

  ref.reset().supplyMap(supplyMap).supplyType(refType).costMap(costMap).run(
      Ref::BLOCK_SEARCH);
  test.reset().supplyMap(supplyMap).supplyType(testType).costMap(costMap).run(
      Test::BLOCK_SEARCH);
  if (ref.totalCost() != test.totalCost()) {
    fmt::printf("Bug in BLOCK_SEARCH\n");
    std::abort();
  }

  ref.reset().supplyMap(supplyMap).supplyType(refType).costMap(costMap).run(
      Ref::CANDIDATE_LIST);
  test.reset().supplyMap(supplyMap).supplyType(testType).costMap(costMap).run(
      Test::CANDIDATE_LIST);
  if (ref.totalCost() != test.totalCost()) {
    fmt::printf("Bug in CANDIDATE_LIST\n");
    std::abort();
  }

  ref.reset().supplyMap(supplyMap).supplyType(refType).costMap(costMap).run(
      Ref::ALTERING_LIST);
  test.reset().supplyMap(supplyMap).supplyType(testType).costMap(costMap).run(
      Test::ALTERING_LIST);
  if (ref.totalCost() != test.totalCost()) {
    fmt::printf("Bug in ALTERING_LIST\n");
    std::abort();
  }
}

void testSolverWithBounds(Graph& graph, SupplyNodeMap& supplyMap,
                          CostArcMap& costMap, IntArcMap& lowerMap,
                          IntArcMap& upperMap, Ref::SupplyType refSupplyType,
                          Test::SupplyType testSupplyType) {
  // Solver
  Ref ref(graph);
  Test test(graph);

  auto retRef = ref.reset()
                    .supplyMap(supplyMap)
                    .supplyType(refSupplyType)
                    .costMap(costMap)
                    .upperMap(upperMap)
                    .lowerMap(lowerMap)
                    .run(Ref::FIRST_ELIGIBLE);
  if (retRef != Ref::OPTIMAL) {
    fmt::printf("Bug in setup of flow constraints \n");
    std::abort();
  }
  test.reset()
      .supplyMap(supplyMap)
      .supplyType(testSupplyType)
      .costMap(costMap)
      .upperMap(upperMap)
      .lowerMap(lowerMap)
      .run(Test::FIRST_ELIGIBLE);
  if (ref.totalCost() != test.totalCost()) {
    for (ArcIt a(graph); a != INVALID; ++a) {
      if (test.flow(a) > upperMap[a]) fmt::printf("Bad UB");
      if (test.flow(a) < lowerMap[a]) fmt::printf("Bad LB");
    }
    fmt::printf("Bug in FIRST_ELIGIBLE with flow constraints \n");
    std::abort();
  }

#ifndef NDEBUG
  int nL = 0, nU = 0;
  for (ArcIt a(graph); a != INVALID; ++a) {
    if (ref.flow(a) == upperMap[a]) nU++;
    if (ref.flow(a) == lowerMap[a] && lowerMap[a] != 0) nL++;
  }
  fmt::printf("LB / UB met for %s/%s out of %s arcs\n", nL, nU, graph.arcNum());
#endif

  ref.reset()
      .supplyMap(supplyMap)
      .supplyType(refSupplyType)
      .costMap(costMap)
      .upperMap(upperMap)
      .lowerMap(lowerMap)
      .run(Ref::BEST_ELIGIBLE);
  test.reset()
      .supplyMap(supplyMap)
      .supplyType(testSupplyType)
      .costMap(costMap)
      .upperMap(upperMap)
      .lowerMap(lowerMap)
      .run(Test::BEST_ELIGIBLE);
  if (ref.totalCost() != test.totalCost()) {
    fmt::printf("Bug in BEST_ELIGIBLE with flow constraints \n");
    std::abort();
  }

  ref.reset()
      .supplyMap(supplyMap)
      .supplyType(refSupplyType)
      .costMap(costMap)
      .upperMap(upperMap)
      .lowerMap(lowerMap)
      .run(Ref::BLOCK_SEARCH);
  test.reset()
      .supplyMap(supplyMap)
      .supplyType(testSupplyType)
      .costMap(costMap)
      .upperMap(upperMap)
      .lowerMap(lowerMap)
      .run(Test::BLOCK_SEARCH);
  if (ref.totalCost() != test.totalCost()) {
    fmt::printf("Bug in BLOCK_SEARCH with flow constraints \n");
    std::abort();
  }

  ref.reset()
      .supplyMap(supplyMap)
      .supplyType(refSupplyType)
      .costMap(costMap)
      .upperMap(upperMap)
      .lowerMap(lowerMap)
      .run(Ref::CANDIDATE_LIST);
  test.reset()
      .supplyMap(supplyMap)
      .supplyType(testSupplyType)
      .costMap(costMap)
      .upperMap(upperMap)
      .lowerMap(lowerMap)
      .run(Test::CANDIDATE_LIST);
  if (ref.totalCost() != test.totalCost()) {
    fmt::printf("Bug in CANDIDATE_LIST with flow constraints \n");
    std::abort();
  }

  ref.reset()
      .supplyMap(supplyMap)
      .supplyType(refSupplyType)
      .costMap(costMap)
      .upperMap(upperMap)
      .lowerMap(lowerMap)
      .run(Ref::ALTERING_LIST);
  test.reset()
      .supplyMap(supplyMap)
      .supplyType(testSupplyType)
      .costMap(costMap)
      .upperMap(upperMap)
      .lowerMap(lowerMap)
      .run(Test::ALTERING_LIST);
  if (ref.totalCost() != test.totalCost()) {
    fmt::printf("Bug in ALTERING_LIST with flow constraints \n");
    std::abort();
  }
}

int main(int argc, char** argv) {
  // Dimensions of grid
  Int2Array muXdim = {ULMON_GRID_DIM, ULMON_GRID_DIM};
  Int2Array muYdim = {ULMON_GRID_DIM, ULMON_GRID_DIM};
  if (argc >= 2) {
    int dim = std::atoi(argv[1]);
    for (int i = 0; i < 2; ++i) muXdim[i] = dim, muYdim[i] = dim;
  }
  int nx = muXdim[0] * muXdim[1];
  int ny = muYdim[0] * muYdim[1];

  fmt::printf("muXdim={%d,%d}, muYdim={%d,%d}\n",  //
              muXdim[0], muXdim[1], muYdim[0], muYdim[1]);

  // Supply
  ValueVector supply(test::getRandomSupply(nx, ny));

  // ---------------- sum supplies = 0 (GEQ) -----------------------------
  // Graph, and supply and cost map
  Graph graph(muXdim, muYdim, supply, true);
  SupplyNodeMap supplyMap(graph);
  CostArcMap costMap(graph);

  // test solvers without LB/UB
  testSolver(graph, supplyMap, costMap, Ref::GEQ, Test::GEQ);

  // test solvers with LB/UB
  IntArcMap lowerMap(graph);
  IntArcMap upperMap(graph);
  makeBoundsForGeq(graph, supplyMap, upperMap, lowerMap);
  testSolverWithBounds(graph, supplyMap, costMap, lowerMap, upperMap, Ref::GEQ,
                       Test::GEQ);

  // ---------------- sum supplies < 0 (GEQ) -----------------------------
  ValueVector supplyG = supply;
  for (auto& sup : supplyG) {
    if (sup > 2) sup--;
  }
  // Graph, and supply and cost map
  Graph graphG(muXdim, muYdim, supplyG, true);
  SupplyNodeMap supplyMapG(graphG);
  CostArcMap costMapG(graphG);

  // test solvers without LB/UB
  testSolver(graphG, supplyMapG, costMapG, Ref::GEQ, Test::GEQ);

  // test solvers with LB/UB
  IntArcMap lowerMapG(graphG);
  IntArcMap upperMapG(graphG);
  makeBoundsForGeq(graphG, supplyMapG, upperMapG, lowerMapG);
  testSolverWithBounds(graphG, supplyMapG, costMapG, lowerMapG, upperMapG,
                       Ref::GEQ, Test::GEQ);

  // ---------------- sum supplies > 0 (LEQ) -----------------------------
  ValueVector supplyL = supply;
  for (auto& sup : supplyL) {
    if (sup < -2) sup++;
  }
  // Graph, and supply and cost map
  Graph graphL(muXdim, muYdim, supplyL, true);
  SupplyNodeMap supplyMapL(graphL);
  CostArcMap costMapL(graphL);

  // test solvers without LB/UB
  testSolver(graphL, supplyMapL, costMapL, Ref::GEQ, Test::GEQ);

  // test solvers with LB/UB
  IntArcMap lowerMapL(graphL);
  IntArcMap upperMapL(graphL);
  makeBoundsForLeq(graphL, supplyMapL, upperMapL, lowerMapL);
  testSolverWithBounds(graphL, supplyMapL, costMapL, lowerMapL, upperMapL,
                       Ref::LEQ, Test::LEQ);

  fmt::printf("Okay\n");
  return 0;
}
