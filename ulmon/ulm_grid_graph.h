#ifndef ULMON_ULM_GRID_GRAPH_H
#define ULMON_ULM_GRID_GRAPH_H

#include <ulmon/core.h>
#include <ulmon/smart_bpdigraph.h>
#include <ulmon/utils/grid.h>
#include <ulmon/utils/metric.h>

#include <array>
#include <cassert>
#include <functional>
#include <numeric>
#include <vector>

namespace lemon {

/// @brief Bipartite grid graph with functionality to modify itself given the
/// support of the current flow.
///
/// \tparam V The number type used for flow amounts and supply values.
/// By default, it is \c int.
/// \tparam C The number type used for arc costs.
/// By default, it is \c V.
/// \param int D Grid dimension
/// \tparam M The metric class used to compute distances.
/// By default, it is \c SquaredEuclidean<C, D>.
///
/// \warning \c V and \c C must be a signed number type.
template <typename V = int, typename C = V, int D = 2,
          typename M = SquaredEuclidean<C, D>>
class UlmGridGraph : public SmartBpDigraph {
  using Graph = UlmGridGraph<V, C, D, M>;

 public:
  // Type aliases
  using Value = V;
  using Cost = C;
  static constexpr int Dim = D;
  using Metric = M;

  using IntDimArray = std::array<int, Dim>;
  using ValueVector = std::vector<Value>;
  using CostVector = std::vector<Cost>;
  using PosVector = std::vector<IntDimArray>;

  using SupportVector = std::vector<std::pair<RedNode, BlueNode>>;
  using ArcVector = std::vector<Arc>;

  // Shielded tag
  using ShieldedTag = True;

  // Supply wrapper
  class SupplyNodeMap {
    const Graph& _g;

   public:
    SupplyNodeMap(const Graph& g) : _g(g) {}

    V operator[](const Node v) const {
      assert(_g.valid(v));
      const std::size_t i = _g.id(v);
      assert(i < _g._supply.size());
      return _g._supply[i];
    }
  };

  friend class SupplyNodeMap;

  // Cost wrapper
  class CostArcMap {
    const Graph& _g;

   public:
    CostArcMap(const Graph& g) : _g(g) {}

    C operator[](const Arc a) const {
      assert(_g.valid(a));
      const std::size_t i = _g.id(a);
      assert(i < _g._cost.size());
      return _g._cost[i];
    }
  };

  friend class CostArcMap;

 protected:
  // Protected type aliases
  using Parent = SmartBpDigraph;
  using CharVector = std::vector<signed char>;
  using IntVector = std::vector<int>;

  // Graph data
 public:
  const IntDimArray _x_dim, _y_dim;
  const IntDimArray _x_strides, _y_strides;

 protected:
  PosVector _y_min, _y_max;
  PosVector _old_y_min, _old_y_max;
  PosVector _x_pos, _y_pos;
  bool _fully;
  const int _merge_num;

  // Instance data
  ValueVector _supply;
  CostVector _cost;
  Metric _metric;

 public:
  /// \brief Constructor 1, creates an explicit empty bipartite graph
  ///
  /// \param x_dim Number of points per dimension in source grid
  /// \param y_dim Number of points per dimension in target grid
  /// \param supply Supply / demand of nodes
  UlmGridGraph(const IntDimArray& x_dim, const IntDimArray& y_dim,
               const ValueVector& supply, const bool fully = false)
      : Parent(utils::numNodes(x_dim), utils::numNodes(y_dim)),
        _x_dim(x_dim),
        _y_dim(y_dim),
        _x_strides(utils::getStrides(_x_dim)),
        _y_strides(utils::getStrides(_y_dim)),
        _y_min(_red_num),
        _y_max(_red_num, _y_dim),
        _old_y_min(_red_num),
        _old_y_max(_red_num, _y_dim),
        _x_pos(_red_num),
        _y_pos(_blue_num),
        _fully(fully),
        _merge_num(0),
        _supply(supply) /* Copy */ {
    initPos();
    if (_fully) {
      reserveArcs(_red_num * _blue_num);
      addArcs();
    } else {
      reserveArcs(ULMON_CONST_RESERVE * _node_num);
    }
  }

  /// \brief Constructor 2, creates a specified bipartite graph
  ///
  /// \param x_dim Number of points per dimension in source grid
  /// \param y_dim Number of points per dimension in target grid
  /// \param supply Supply / demand of nodes
  /// \param y_min For each red node the blue node nbor with minimum pos
  /// \param y_max For each red node the blue node nbor with maximum pos
  UlmGridGraph(const IntDimArray& x_dim, const IntDimArray& y_dim,
               const ValueVector& supply,  //
               const PosVector& y_min, const PosVector& y_max)
      : Parent(utils::numNodes(x_dim), utils::numNodes(y_dim)),
        _x_dim(x_dim),
        _y_dim(y_dim),
        _x_strides(utils::getStrides(_x_dim)),
        _y_strides(utils::getStrides(_y_dim)),
        _y_min(y_min),  // Copy
        _y_max(y_max),  // Copy
        _old_y_min(_red_num),
        _old_y_max(_red_num, _y_dim),
        _x_pos(_red_num),
        _y_pos(_blue_num),
        _fully(false),
        _merge_num(0),
        _supply(supply) /* Copy */ {
    initPos();

    assert(_y_min.size() == _red_num);
    assert(_y_max.size() == _red_num);
    reserveArcs(ULMON_CONST_RESERVE * utils::numArcs(_y_min, _y_max));
    addArcs();
  }

  UlmGridGraph(const Graph& graph, const int merge_num)
      : Parent(utils::numNodes(
                   utils::getCoarsenedGridDim(merge_num, graph._x_dim)),
               utils::numNodes(
                   utils::getCoarsenedGridDim(merge_num, graph._y_dim))),
        _x_dim(utils::getCoarsenedGridDim(merge_num, graph._x_dim)),
        _y_dim(utils::getCoarsenedGridDim(merge_num, graph._y_dim)),
        _x_strides(utils::getStrides(_x_dim)),
        _y_strides(utils::getStrides(_y_dim)),
        _y_min(_red_num),
        _y_max(_red_num, _y_dim),
        _old_y_min(_red_num),
        _old_y_max(_red_num, _y_dim),
        _x_pos(_red_num),
        _y_pos(_blue_num),
        _fully(false),
        _merge_num(merge_num),
        _supply(_node_num) {
    initPos();

    IntDimArray pos{};
    for (int xx = 0; xx < graph._red_num; ++xx) {
      utils::coarsenedPos(merge_num, graph._x_pos[xx], pos);
      int x = utils::idFromPos(pos, _x_strides);
      _supply[x] += graph._supply[xx];
    }
    for (int yy = 0; yy < graph._blue_num; ++yy) {
      utils::coarsenedPos(merge_num, graph._y_pos[yy], pos);
      int y = utils::idFromPos(pos, _y_strides);
      _supply[y + _red_num] += graph._supply[yy + graph._red_num];
    }
    assert(std::reduce(_supply.begin(), _supply.end()) == 0);

    reserveArcs(ULMON_CONST_RESERVE * _node_num);
  }

  /// \brief Clears the graph, reserves space, resets shield, and adds all arcs
  void addAllArcs() {
    clearArcs();
    reserveArcs(_red_num * _blue_num);
    resetShield();
    addArcs();
    _fully = true;
  }

  /// \brief Adds the arc between x and y, computes cost
  Arc addArc(RedNode x, BlueNode y) {
    Arc a = Parent::addArc(x, y);
    _cost.emplace_back(_metric(_x_pos[id(x)], _y_pos[id(y)]));
    return a;
  }

  /// \brief Adds the arc between x and y w/o calling build, computes cost
  /// \warning Call buildArcs after adding all arcs
  inline Arc addArcLazily(RedNode x, BlueNode y) {
    Arc a = Parent::addArcLazily(x, y);
    _cost.emplace_back(_metric(_x_pos[id(x)], _y_pos[id(y)]));
    return a;
  }

  // Add all arcs (x,y) with x in rectangle (x_min,x_max) and y in rectangle
  // (y_min,y_max)
  void addArcs(const IntDimArray& x_min, const IntDimArray& x_max,
               const IntDimArray& y_min, const IntDimArray& y_max) {
    reserveArcs(arcNum() +
                utils::numNodes(x_min, x_max) * utils::numNodes(y_min, y_max));

    IntDimArray x_pos = x_min;
    do {
      int x = utils::idFromPos(x_pos, _x_strides);
      IntDimArray y_pos = y_min;

      do {
        int y = utils::idFromPos(y_pos, _y_strides);
        addArcLazily(redNode(x), blueNode(y));
        utils::advancePos(y_min, y_max, y_pos);
      } while (y_pos != y_min);

      utils::advancePos(x_min, x_max, x_pos);
    } while (x_pos != x_min);

    buildArcs();
  }

  /// \brief Adds all arcs (x,y) for which _y_min <= y_pos < _y_max and
  /// \c cond(x,y) is true
  void addArcs(std::function<bool(int, int)> cond = [](int, int) {
    return true;
  }) {
    for (int x = 0; x < _red_num; ++x) {
      if (!isIsolated(x)) addArcs(x, cond);
    }
    buildArcs();
  }

  /// \brief Triggers build on all arc observers
  void buildArcs() { notifier(Arc()).build(); }

  /// \brief Deletes all arcs
  void clearArcs() {
    Parent::clearArcs();
    _cost.clear();
    _fully = false;
  }

  IntDimArray getPos(const RedNode x) const { return _x_pos[id(x)]; }

  IntDimArray getPos(const BlueNode y) const { return _y_pos[id(y)]; }

  /// \brief Recomputes the shield based on the given support, clears arcs,
  /// reserves space, adds all arcs in the shield, and then adds all missing
  /// arcs from the support
  void rebuildShield(const SupportVector& support) {
    // Recompute shield (_y_min, _y_max) and add all these arcs
    resetShield();
    for (const auto& [x, y] : support) updateShield(x, y);
    clearArcs();
    reserveArcs(utils::numArcs(_y_min, _y_max) + _node_num);
    addArcs();

    // Add missing support arcs
    for (const auto& [x, y] : support) {
      if (!utils::contains(_y_min[id(x)], _y_max[id(x)], _y_pos[id(y)])) {
        addArcLazily(x, y);
      }
    }
    buildArcs();
  }

  /// \brief Recomputes the shield based on the given support, clears arcs,
  /// reserves space, adds all arcs in the shield, and then adds all missing
  /// arcs from the support
  ///
  /// support_arcs[i] = a is the arc corresponding to support[i] = (x,y)
  void rebuildShield(                   //
      const SupportVector& support,     //
      const ValueVector& support_flow,  //
      ArcVector& support_arcs) {
    assert(std::is_sorted(support.begin(), support.end()));
    assert(support.size() == support_flow.size());
    support_arcs.clear();
    support_arcs.resize(support.size(), arcFromId(-1));

    // Recompute shield (_y_min, _y_max) and add all these arcs
    resetShield();
    int i = 0;
    for (const auto& [x, y] : support)
      if (support_flow[i++]) updateShield(x, y);
    clearArcs();
    reserveArcs(utils::numArcs(_y_min, _y_max) + _node_num);

    // Add shield arcs
    auto it = support.begin();
    i = 0;
    for (int x = 0; x < _red_num; ++x) {
      if (isIsolated(x)) continue;
      RedNode xn = redNode(x);

      IntDimArray y_pos = _y_min[x];
      do {
        const int y = utils::idFromPos(y_pos, _y_strides);
        BlueNode yn = blueNode(y);
        Arc a = addArcLazily(xn, yn);

        auto p = std::make_pair(xn, yn);
        while (it != support.end() && *it < p) ++it, ++i;
        if (it != support.end() && *it == p) support_arcs[i] = a;

        utils::advancePos(_y_min[x], _y_max[x], y_pos);
      } while (y_pos != _y_min[x]);
    }

    // Add missing support arcs
    i = 0;
    for (const auto& [x, y] : support) {
      if (!utils::contains(_y_min[id(x)], _y_max[id(x)], _y_pos[id(y)])) {
        Arc a = addArcLazily(x, y);
        assert(!valid(support_arcs[i]));
        support_arcs[i] = a;
      }
      ++i;
    }
    buildArcs();
  }

  /// \brief Reserves space for \c m arcs
  void reserveArcs(int m) {
    assert(m >= 0);
    Parent::reserveArcs(m);
    _cost.reserve(m);
  }

  /// \brief Resets shield to fully bipartite graph
  inline void resetShield() {
    _y_min.clear();
    _y_min.resize(_red_num, IntDimArray{});
    _y_max.clear();
    _y_max.reserve(_red_num);
    for (int x = 0; x < _red_num; ++x) {
      // Zero supply -> empty shield
      _y_max.emplace_back(_supply[x] == 0 ? IntDimArray{} : _y_dim);
    }
  }

  void updateShield(const SupportVector& support) {
    if (_fully) return;

    std::swap(_y_min, _old_y_min);
    std::swap(_y_max, _old_y_max);
    resetShield();
    for (const auto& [x, y] : support) updateShield(x, y);

    // _y_min and _y_max hold the best possible shield
    // Now we determine which arcs we have to add
    for (int x = 0; x < _red_num; ++x) {
      for (int i = 0; i < Dim; ++i) {
        _y_min[x][i] = std::min(_y_min[x][i], _old_y_min[x][i]);
        _y_max[x][i] = std::max(_y_max[x][i], _old_y_max[x][i]);
      }
    }

    // Now all (x,y) with y
    // (_y_min[x],_y_max[x]) \ (_old_y_min[x],_old_y_max[x]) are missing
    // The function cond is true iff (x,y) is not in this
    auto cond = [&](const int& x, const int& y) {
      return !utils::contains(_old_y_min[x], _old_y_max[x], _y_pos[y]);
    };
    addArcs(cond);
  }

 protected:
  /// \brief For \c x adds arcs (x,y) for which _y_min <= y_pos < _y_max and
  /// \c cond(x,y) is true
  inline void addArcs(
      const int& x,
      std::function<bool(int, int)> cond = [](int, int) { return true; }) {
    assert(0 <= x && x < _red_num);
    assert(utils::less(_y_min[x], _y_max[x]));

    IntDimArray y_pos = _y_min[x];
    do {
      const int y = utils::idFromPos(y_pos, _y_strides);

      if (cond(x, y)) {
#ifndef NDEBUG
        Arc a =
#endif
            addArcLazily(redNode(x), blueNode(y));
        assert(id(a) + 1 == _cost.size());
      }

      utils::advancePos(_y_min[x], _y_max[x], y_pos);
    } while (y_pos != _y_min[x]);
  }

  inline void initPos() {
    IntDimArray pos{};
    for (int x = 0; x < _red_num; ++x) {
      _x_pos[x] = pos;
      utils::advancePos(_x_dim, pos);
    }
    assert(pos == IntDimArray{});
    for (int y = 0; y < _blue_num; ++y) {
      _y_pos[y] = pos;
      utils::advancePos(_y_dim, pos);
    }
    assert(pos == IntDimArray{});
  }

  // If _y_min[x][i] >= _y_max[x][i] for some i, then x has no nbors
  inline bool isIsolated(const int& x) {
    return !utils::less(_y_min[x], _y_max[x]);
  }

  /// \brief Returns the positive neighbor in dimension i
  inline RedNode negNbor(const int& x, const int& i) const {
    return redNode(x - _x_strides[i]);
  }

  /// \brief Returns the negative neighbor in dimension i
  inline RedNode posNbor(const int& x, const int& i) const {
    return redNode(x + _x_strides[i]);
  }

  inline void updateShield(const RedNode& x, const BlueNode& y) {
    assert(valid(x));
    assert(valid(y));

    // Shrink shield
    for (int i = 0; i < Dim; ++i) {
      if (_x_pos[id(x)][i] > 0) {
        RedNode nx = negNbor(id(x), i);
        _y_max[id(nx)][i] = std::min(_y_max[id(nx)][i], _y_pos[id(y)][i] + 1);
      }
    }
    for (int i = 0; i < Dim; ++i) {
      if (_x_pos[id(x)][i] < _x_dim[i] - 1) {
        RedNode px = posNbor(id(x), i);
        _y_min[id(px)][i] = std::max(_y_min[id(px)][i], _y_pos[id(y)][i]);
      }
    }
  }
};

};  // namespace lemon

#endif
