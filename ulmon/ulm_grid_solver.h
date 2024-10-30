#ifndef ULMON_ULM_GRID_SOLVER_H
#define ULMON_ULM_GRID_SOLVER_H

#include <ulmon/core.h>
#include <ulmon/ulm_network_simplex.h>
#include <ulmon/utils/grid.h>

#include <fmt/printf.hpp>
#include <optional>

namespace lemon {

template <typename GR>
class UlmGridSolver {
  using Value = typename GR::Value;
  using Cost = typename GR::Cost;
  static constexpr int Dim = GR::Dim;

  using IntDimArray = typename GR::IntDimArray;
  using SupportVector = typename GR::SupportVector;

 public:
  using NetSimplex = UlmNetworkSimplex<GR, Value, Cost>;
  using ProblemType = typename NetSimplex::ProblemType;

 private:
  TEMPLATE_BPDIGRAPH_TYPEDEFS(GR);

  // Instance data
  GR& _graph;
  NetSimplex _net;
  SupportVector _support;

  // Solver settings
  const int _merge_num;
  const int _max_depth;
  bool _called_run{false};

 public:
  UlmGridSolver(GR& graph, const int merge_num = 2)
      : _graph(graph),
        _net(graph, false),
        _merge_num(merge_num),
        _max_depth(utils::hierarchicalDepth(_graph._x_dim, _graph._y_dim)) {
    _support.reserve(countNodes(_graph));
  }

  ProblemType run() {
    _called_run = true;

    if (_max_depth > 0) {
      ProblemType r = run(1, _graph);
      if (r != NetSimplex::OPTIMAL) return r;
    } else {
      _graph.addAllArcs();
    }

    _net.reset();
    return subsolve(_graph, _net);
  }

  ProblemType subsolve(GR& graph, NetSimplex& net) {
    typename GR::SupplyNodeMap supplyMap(graph);
    typename GR::CostArcMap costMap(graph);
    net.supplyMap(supplyMap).costMap(costMap);
    return net.runShielded();
  }

  ProblemType _subsolve(GR& graph, NetSimplex& net) {
    typename GR::SupplyNodeMap supplyMap(graph);
    typename GR::CostArcMap costMap(graph);

    Cost c = std::numeric_limits<Cost>::max(), cp = c;
    ProblemType r = NetSimplex::INFEASIBLE;

    for (;;) {
      net.supplyMap(supplyMap).costMap(costMap);

      if (r == NetSimplex::OPTIMAL) cp = c;
      r = net.run();
      c = net.totalCost();

      // If subsolve is called from run, the problem should always be feasible
      assert(!_called_run || r != NetSimplex::INFEASIBLE);

      if ((c >= cp && r == NetSimplex::OPTIMAL) || r == NetSimplex::UNBOUNDED)
        break;

      _support.clear();
      for (ArcIt a(graph); a != INVALID; ++a) {
        if (net.flow(a))
          _support.emplace_back(graph.source(a, RedNode{}),
                                graph.target(a, BlueNode{}));
      }
      graph.rebuildShield(_support);

      net.reset();
    }

    return r;
  }

  Cost totalCost() const { return _net.totalCost(); }

 private:
  ProblemType run(int depth, GR& parent) {
    ProblemType r;

    GR graph(parent, _merge_num);

    if (depth < _max_depth) {
      r = run(depth + 1, graph);
      if (r != NetSimplex::OPTIMAL) return r;
    } else {
      graph.addAllArcs();
    }

    NetSimplex net(graph, false);
    r = subsolve(graph, net);
    if (r != NetSimplex::OPTIMAL) return r;

    prepare(graph, net, parent);
    return r;
  }

  void prepare(const GR& graph, const NetSimplex& net, GR& parent) {
    parent.clearArcs();

    for (ArcIt a(graph); a != INVALID; ++a) {
      if (!net.flow(a)) continue;

      IntDimArray x_min = graph.getPos(graph.source(a, RedNode{}));
      IntDimArray y_min = graph.getPos(graph.target(a, BlueNode{}));

      IntDimArray x_max{}, y_max{};
      for (int i = 0; i < Dim; ++i) {
        x_min[i] *= _merge_num;
        x_max[i] = std::min(x_min[i] + _merge_num, parent._x_dim[i]);
        y_min[i] *= _merge_num;
        y_max[i] = std::min(y_min[i] + _merge_num, parent._y_dim[i]);
      }

      parent.addArcs(x_min, x_max, y_min, y_max);
    }
  }
};

};  // namespace lemon

#endif
