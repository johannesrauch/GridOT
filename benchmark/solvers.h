#ifndef BENCHMARK_SOLVERS_H
#define BENCHMARK_SOLVERS_H

#include <Common.h>
#include <LP_Lemon.h>
#include <ShortCutSolver.h>
#include <ulmon/test/instance.h>
#include <ulmon/ulm_grid_graph.h>
#include <ulmon/ulm_grid_solver.h>

namespace benchmark {

using Value = int;  // Supply / demand type, signed
using Cost = int;   // Cost type, signed
using TotalCost = long int;
using ValueVector = std::vector<Value>;

using Results = lemon::test::Results<TotalCost>;

using Graph = UlmGridGraph<Value, Cost>;
using GridSolver = UlmGridSolver<Graph>;

// apply GridSolver
auto gridSolver(Graph& graph) {
  GridSolver solver(graph);
  Results res;
  res.tic();
  res.return_value = solver.run();
  res.toc();
  res.objective_value = solver.totalCost<TotalCost>();
  std::vector<std::vector<double>> densities = solver._densities;
  return std::make_pair(res, densities);
}

// Adapted from Schmitzer's MultiScaleOT/src/Examples/ShortCut.cpp
Results schmitzerMultiScale(int* dim, ValueVector& supply, int depth) {
  int msg;
  const int n = dim[0] * dim[1];

  std::vector<double> supplyNormalized(supply.size());
  Value totalSupply = 0;
  for (int i = 0; i < n; ++i) totalSupply += supply[i];
  for (int i = 0; i < n; ++i)
    supplyNormalized[i] = static_cast<double>(supply[i]) / totalSupply;
  for (int i = n; i < 2 * n; ++i)
    supplyNormalized[i] = -static_cast<double>(supply[i]) / totalSupply;
  double* muXdat = &supplyNormalized[0];
  double* muYdat = &supplyNormalized[n];

  // put raw marginal data into small container structs
  TDoubleMatrix muX, muY;

  muX.data = muXdat;
  muX.dimensions = dim;
  muX.depth = 2;

  muY.data = muYdat;
  muY.dimensions = dim;
  muY.depth = 2;

  // Lemon parameters
  double measureScale = 1E-9;  // scale to which marginal measures are truncated
  double cScale = 1E-7;        // scale to which cost function is truncated
  bool dualOffset =
      true;  // activate small trick with dual variables
             // that slightly accelerates the lemon network simplex

  ///////////////////////////////////////////////
  // total nr of points in each marginal
  int xres = GridToolsGetTotalPoints(muX.depth, muX.dimensions);
  int yres = GridToolsGetTotalPoints(muY.depth, muY.dimensions);

  // truncate marginals
  // both marginals are rounded to multiples of measureScale
  msg = MeasureToolsTruncateMeasures(muX.data, muY.data, xres, yres,
                                     measureScale);
  if (msg != 0) {
    fmt::printf("MeasureToolsTruncateMeasures msg=%d\n", msg);
    assert(msg == 0);
    Results results;
    results.return_value = 0;
    results.objective_value = 0;
    return results;
  }

  ///////////////////////////////////////////////
  // problem setup

  // generate multi-scale problem representation:
  // the MultiScaleSetup object will subsequently hold all
  // required data for the solver and other algorithms

  // the class TMultiScaleSetupGrid assumes that muX and muY describe measures
  // that live on regular Cartesian grids with edge lenghts 1

  // the more general base class TMultiScaleSetup uses marginals with support on
  // arbitrary point clouds
  TMultiScaleSetupGrid MultiScaleSetupX(&muX, depth, false, false, false);
  msg = MultiScaleSetupX.Setup();
  assert(msg == 0);

  TMultiScaleSetupGrid MultiScaleSetupY(&muY, depth, false, false, false);
  msg = MultiScaleSetupY.Setup();
  assert(msg == 0);

  // setup various aux components for actual solver algorithm
  TCostFunctionProvider_Dynamic costFunctionProvider(
      MultiScaleSetupX.resH, MultiScaleSetupY.resH, MultiScaleSetupX.posH,
      MultiScaleSetupY.posH, MultiScaleSetupX.nLayers, MultiScaleSetupX.dim);

  TShieldGeneratorGrid_SqrEuclidean shieldGenerator(
      MultiScaleSetupX.dim, MultiScaleSetupX.dimH, MultiScaleSetupY.dimH,
      MultiScaleSetupX.nLayers);

  TShortCutCouplingHandlerInterface couplingHandlerInterface(
      MultiScaleSetupX.resH, MultiScaleSetupY.resH, MultiScaleSetupX.nLayers);

  // LP subsolver: Lemon
  TShortCutSubSolverInterfaceLemon subSolverInterface(
      MultiScaleSetupX.nLayers, MultiScaleSetupX.muH, MultiScaleSetupY.muH,
      &couplingHandlerInterface, measureScale, cScale, dualOffset);

  static constexpr int VIOLATION_CHECKMODE = TShortCutSolver::VCHECK_PRIMAL;

  ///////////////////////////////////////////////
  // main solver object
  TShortCutMultiScaleSolver MultiScaleSolver(
      &costFunctionProvider, &couplingHandlerInterface, &subSolverInterface,
      &shieldGenerator, MultiScaleSetupX.HP, MultiScaleSetupY.HP,
      1,  // coarsest layer
      VIOLATION_CHECKMODE);

  MultiScaleSolver.autoDeletePointers = false;

  Results results;
  msg = MultiScaleSolver.solve();
  results.toc();
  results.objective_value = MultiScaleSolver.objective * totalSupply;
  results.return_value = msg == 0;
  assert(msg == 0);
  return results;
}

};  // namespace benchmark
#endif
