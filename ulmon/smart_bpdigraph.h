/* -*- mode: C++; indent-tabs-mode: nil; -*-
 *
 * This file is a part of LEMON, a generic C++ optimization library.
 *
 * Copyright (C) 2003-2013
 * Egervary Jeno Kombinatorikus Optimalizalasi Kutatocsoport
 * (Egervary Research Group on Combinatorial Optimization, EGRES).
 *
 * Permission to use, modify and distribute this software is granted
 * provided that this copyright notice appears in all copies. For
 * precise terms see the accompanying LICENSE file.
 *
 * This software is provided "AS IS" with no warranty of any kind,
 * express or implied, and with no claim as to its suitability for any
 * purpose.
 *
 */

#ifndef ULMON_SMART_BIDIGRAPH_H
#define ULMON_SMART_BIDIGRAPH_H

///\ingroup graphs
///\file
///\brief SmartDigraph and SmartGraph classes.

#include <ulmon/bits/bpdigraph_extender.h>

#include <cassert>
#include <vector>

namespace lemon {

class SmartBpDigraphBase {
 protected:
  struct RedNodeT {
    int first_out;
    RedNodeT() : first_out(-1) {}
  };

  struct BlueNodeT {
    int first_in;
    BlueNodeT() : first_in(-1) {}
  };

  struct ArcT {
    int source;
    int target;
    int next_out;
    int next_in;
    ArcT() {}
  };

  int _red_num;
  int _blue_num;
  int _node_num;

  std::vector<RedNodeT> _red_nodes;
  std::vector<BlueNodeT> _blue_nodes;
  std::vector<ArcT> _arcs;

 public:
  typedef SmartBpDigraphBase Graph;

  class Node;
  class Arc;

  class Node {
    friend class SmartBpDigraphBase;

   protected:
    int _id;
    explicit Node(int id) { _id = id; }

   public:
    Node() {}
    Node(Invalid) { _id = -1; }
    bool operator==(const Node& node) const { return _id == node._id; }
    bool operator!=(const Node& node) const { return _id != node._id; }
    bool operator<(const Node& node) const { return _id < node._id; }
  };

  class RedNode : public Node {
    friend class SmartBpDigraphBase;

   protected:
    explicit RedNode(int pid) : Node(pid) {}

   public:
    RedNode() {}
    RedNode(const RedNode& node) : Node(node) {}
    RedNode(Invalid) : Node(INVALID) {}
    const RedNode& operator=(const RedNode& node) {
      Node::operator=(node);
      return *this;
    }
  };

  class BlueNode : public Node {
    friend class SmartBpDigraphBase;

   protected:
    explicit BlueNode(int pid) : Node(pid) {}

   public:
    BlueNode() {}
    BlueNode(const BlueNode& node) : Node(node) {}
    BlueNode(Invalid) : Node(INVALID) {}
    const BlueNode& operator=(const BlueNode& node) {
      Node::operator=(node);
      return *this;
    }
  };

  class Arc {
    friend class SmartBpDigraphBase;

   protected:
    int _id;
    explicit Arc(int id) { _id = id; }

   public:
    Arc() {}
    Arc(Invalid) { _id = -1; }
    bool operator==(const Arc& arc) const { return _id == arc._id; }
    bool operator!=(const Arc& arc) const { return _id != arc._id; }
    bool operator<(const Arc& arc) const { return _id < arc._id; }
  };

  SmartBpDigraphBase() : _red_num(0), _blue_num(0), _node_num(0) {}

  void construct(const int red_num, const int blue_num) {
    _red_num = red_num;
    _blue_num = blue_num;
    _node_num = red_num + blue_num;

    _red_nodes.resize(red_num);
    _blue_nodes.resize(blue_num);
  }

  typedef True NodeNumTag;
  typedef True ArcNumTag;

  int nodeNum() const { return _node_num; }
  int redNum() const { return _red_num; }
  int blueNum() const { return _blue_num; }
  int arcNum() const { return _arcs.size(); }

  int maxNodeId() const { return _node_num - 1; }
  int maxRedId() const { return _red_num - 1; }
  int maxBlueId() const { return _blue_num - 1; }
  int maxArcId() const { return _arcs.capacity() - 1; }

  bool red(Node n) const { return n._id < _red_num; }
  bool blue(Node n) const { return n._id >= _red_num; }

  static RedNode asRedNodeUnsafe(Node n) { return RedNode(n._id); }
  static BlueNode asBlueNodeUnsafe(Node n) { return BlueNode(n._id); }

  Node source(Arc a) const { return Node(_arcs[a._id].source); }
  RedNode source(Arc a, RedNode) const {
    Node n = source(a);
    assert(red(n));
    return asRedNodeUnsafe(n);
  }

  Node target(Arc a) const { return Node(_arcs[a._id].target); }
  BlueNode target(Arc a, BlueNode) const {
    Node n = target(a);
    assert(blue(n));
    return asBlueNodeUnsafe(n);
  }

  void first(Node& node) const { node._id = _node_num - 1; }

  static void next(Node& node) { --node._id; }

  void first(RedNode& node) const { node._id = _red_num - 1; }

  void next(RedNode& node) const { --node._id; }

  void first(BlueNode& node) const {
    if (_blue_num == 0)
      node._id = -1;
    else
      node._id = _node_num - 1;
  }

  void next(BlueNode& node) const {
    if (node._id == _red_num)
      node._id = -1;
    else
      --node._id;
  }

  void first(Arc& arc) const { arc._id = _arcs.size() - 1; }

  static void next(Arc& arc) { --arc._id; }

  void firstOut(Arc& arc, const Node& v) const {
    if (v._id < _red_num)
      arc._id = _red_nodes[v._id].first_out;
    else
      arc._id = -1;
  }
  void nextOut(Arc& arc) const { arc._id = _arcs[arc._id].next_out; }

  void firstIn(Arc& arc, const Node& v) const {
    if (v._id >= _red_num)
      arc._id = _blue_nodes[v._id - _red_num].first_in;
    else
      arc._id = -1;
  }
  void nextIn(Arc& arc) const { arc._id = _arcs[arc._id].next_in; }

  static int id(Node v) { return v._id; }
  int id(RedNode v) const { return v._id; }
  int id(BlueNode v) const { return v._id - _red_num; }
  static int id(Arc e) { return e._id; }

  static Node nodeFromId(int id) { return Node(id); }
  static Arc arcFromId(int id) { return Arc(id); }

  bool valid(Node n) const { return n._id >= 0 && n._id < _node_num; }
  bool valid(RedNode n) const { return n._id >= 0 && n._id < _red_num; }
  bool valid(BlueNode n) const { return n._id >= _red_num && n._id < _node_num; }
  bool valid(Arc a) const {
    return a._id >= 0 && a._id < static_cast<int>(_arcs.size());
  }

  RedNode redNode(int index) const { return RedNode(index); }
  BlueNode blueNode(int index) const { return BlueNode(index + _red_num); }

  Arc addArc(RedNode u, BlueNode v) {
    int n = _arcs.size();
    _arcs.emplace_back();
    _arcs[n].source = u._id;
    _arcs[n].target = v._id;
    _arcs[n].next_out = _red_nodes[u._id].first_out;
    _arcs[n].next_in = _blue_nodes[v._id - _red_num].first_in;
    _red_nodes[u._id].first_out = n;
    _blue_nodes[v._id - _red_num].first_in = n;
    return Arc(n);
  }

  void clear() {
    _node_num = _red_num = _blue_num = 0;
    _red_nodes.clear();
    _blue_nodes.clear();
    _arcs.clear();
  }

  void clearArcs() {
    for (RedNodeT& r : _red_nodes) r.first_out = -1;
    for (BlueNodeT& b : _blue_nodes) b.first_in = -1;
    _arcs.clear();
  }
};

typedef BpDigraphExtender<SmartBpDigraphBase> ExtendedSmartBpDigraphBase;

/// \ingroup graphs
///
/// \brief A smart undirected bipartite graph class.
///
/// \ref SmartBpDigraph is a simple and fast directed bipartite graph
/// implementation. It is also quite memory efficient but at the price that it
/// does support neither node addition and deletion nor arc deletion.
///
/// This type fully conforms to the \ref concepts::BpDigraph "BpDigraph concept"
/// and it also provides some additional functionalities.
/// Most of its member functions and nested classes are documented
/// only in the concept class.
///
/// This class provides constant time counting for nodes and arcs.
///
/// The arcs are always directed from red nodes to blue nodes.
///
/// \sa concepts::BpDigraph
/// \sa SmartGraph
class SmartBpDigraph : public ExtendedSmartBpDigraphBase {
  typedef ExtendedSmartBpDigraphBase Parent;

 private:
  /// Graphs are \e not copy constructible. Use GraphCopy instead.
  SmartBpDigraph(const SmartBpDigraph&) : ExtendedSmartBpDigraphBase(){};
  /// \brief Assignment of a graph to another one is \e not allowed.
  /// Use GraphCopy instead.
  void operator=(const SmartBpDigraph&) {}

 public:
  /// Constructor.
  SmartBpDigraph() {}

  /// @brief Constructor.

  /// @param red_num Number of red nodes
  /// @param blue_num Number of blue nodes
  SmartBpDigraph(const int red_num, const int blue_num) {
    construct(red_num, blue_num);
  }

  /// \brief Add a new arc to the graph.
  ///
  /// This function adds a new arc to the graph from red node
  /// \c u to blue node \c v.
  /// \return The new arc.
  Arc addArc(RedNode u, BlueNode v) {
    assert(valid(u));
    assert(valid(v));
    Arc a = Parent::addArc(u, v);
    notifier(a).build();
    return a;
  }

  /// \warning Call notifier(Arc()).build() after adding all arcs
  Arc addArcLazily(RedNode u, BlueNode v) {
    return Parent::addArc(u, v);
  }

  /// Clear the graph.

  /// This function erases all nodes and arcs from the graph.
  void clear() { Parent::clear(); }

  void clearArcs() {
    notifier(Arc()).clear();
    Parent::clearArcs();
  }

  /// Reserve memory for arcs.

  /// Using this function, it is possible to avoid superfluous memory
  /// allocation: if you know that the graph you want to build will
  /// be large (e.g. it will contain millions of nodes and/or arcs),
  /// then it is worth reserving space for this amount before starting
  /// to build the graph.
  void reserveArcs(int m) { _arcs.reserve(m); };
};

}  // namespace lemon

#endif  // ULMON_SMART_BIDIGRAPH_H
