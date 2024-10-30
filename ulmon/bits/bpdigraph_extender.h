/*
 * This file has been adapted from graph_extender.h from LEMON, a generic C++
 * optimization library.
 *
-*- mode: C++; indent-tabs-mode: nil; -*-
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

#ifndef ULMON_BITS_BPDIGRAPH_EXTENDER_H
#define ULMON_BITS_BPDIGRAPH_EXTENDER_H

#include <lemon/bits/default_map.h>
#include <lemon/bits/map_extender.h>
#include <lemon/bits/stl_iterators.h>
#include <lemon/concept_check.h>
#include <lemon/concepts/maps.h>
#include <lemon/core.h>

//\ingroup graphbits
//\file
//\brief Extenders for the graph types
namespace lemon {

// \ingroup _graphbits
//
// \brief Extender for the bipartite digraph implementations
template <typename Base>
class BpDigraphExtender : public Base {
  typedef Base Parent;

 public:
  typedef BpDigraphExtender BpDigraph;

  typedef typename Parent::Node Node;
  typedef typename Parent::RedNode RedNode;
  typedef typename Parent::BlueNode BlueNode;
  typedef typename Parent::Arc Arc;

  // BpDigraph extension

  using Parent::first;
  using Parent::id;
  using Parent::next;

  int maxId(Node) const { return Parent::maxNodeId(); }

  int maxId(RedNode) const { return Parent::maxRedId(); }

  int maxId(BlueNode) const { return Parent::maxBlueId(); }

  int maxId(Arc) const { return Parent::maxArcId(); }

  static Node fromId(int id, Node) { return Parent::nodeFromId(id); }

  static Arc fromId(int id, Arc) { return Parent::arcFromId(id); }

  Node oppositeNode(const Node& node, const Arc& arc) const {
    if (node == Parent::source(arc))
      return Parent::target(arc);
    else if (node == Parent::target(arc))
      return Parent::source(arc);
    else
      return INVALID;
  }

  RedNode asRedNode(const Node& node) const {
    if (node == INVALID || Parent::blue(node)) {
      return INVALID;
    } else {
      return Parent::asRedNodeUnsafe(node);
    }
  }

  BlueNode asBlueNode(const Node& node) const {
    if (node == INVALID || Parent::red(node)) {
      return INVALID;
    } else {
      return Parent::asBlueNodeUnsafe(node);
    }
  }

  // Alterable extension

  typedef AlterationNotifier<BpDigraphExtender, Node> NodeNotifier;
  typedef AlterationNotifier<BpDigraphExtender, RedNode> RedNodeNotifier;
  typedef AlterationNotifier<BpDigraphExtender, BlueNode> BlueNodeNotifier;
  typedef AlterationNotifier<BpDigraphExtender, Arc> ArcNotifier;

 protected:
  mutable NodeNotifier node_notifier;
  mutable RedNodeNotifier red_node_notifier;
  mutable BlueNodeNotifier blue_node_notifier;
  mutable ArcNotifier arc_notifier;

 public:
  NodeNotifier& notifier(Node) const { return node_notifier; }

  RedNodeNotifier& notifier(RedNode) const { return red_node_notifier; }

  BlueNodeNotifier& notifier(BlueNode) const { return blue_node_notifier; }

  ArcNotifier& notifier(Arc) const { return arc_notifier; }

  class NodeIt : public Node {
    const BpDigraph* _graph;

   public:
    NodeIt() {}

    NodeIt(Invalid i) : Node(i) {}

    explicit NodeIt(const BpDigraph& graph) : _graph(&graph) {
      _graph->first(static_cast<Node&>(*this));
    }

    NodeIt(const BpDigraph& graph, const Node& node)
        : Node(node), _graph(&graph) {}

    NodeIt& operator++() {
      _graph->next(*this);
      return *this;
    }
  };

  LemonRangeWrapper1<NodeIt, BpDigraph> nodes() const {
    return LemonRangeWrapper1<NodeIt, BpDigraph>(*this);
  }

  class RedNodeIt : public RedNode {
    const BpDigraph* _graph;

   public:
    RedNodeIt() {}

    RedNodeIt(Invalid i) : RedNode(i) {}

    explicit RedNodeIt(const BpDigraph& graph) : _graph(&graph) {
      _graph->first(static_cast<RedNode&>(*this));
    }

    RedNodeIt(const BpDigraph& graph, const RedNode& node)
        : RedNode(node), _graph(&graph) {}

    RedNodeIt& operator++() {
      _graph->next(static_cast<RedNode&>(*this));
      return *this;
    }
  };

  LemonRangeWrapper1<RedNodeIt, BpDigraph> redNodes() const {
    return LemonRangeWrapper1<RedNodeIt, BpDigraph>(*this);
  }

  class BlueNodeIt : public BlueNode {
    const BpDigraph* _graph;

   public:
    BlueNodeIt() {}

    BlueNodeIt(Invalid i) : BlueNode(i) {}

    explicit BlueNodeIt(const BpDigraph& graph) : _graph(&graph) {
      _graph->first(static_cast<BlueNode&>(*this));
    }

    BlueNodeIt(const BpDigraph& graph, const BlueNode& node)
        : BlueNode(node), _graph(&graph) {}

    BlueNodeIt& operator++() {
      _graph->next(static_cast<BlueNode&>(*this));
      return *this;
    }
  };

  LemonRangeWrapper1<BlueNodeIt, BpDigraph> blueNodes() const {
    return LemonRangeWrapper1<BlueNodeIt, BpDigraph>(*this);
  }

  class ArcIt : public Arc {
    const BpDigraph* _graph;

   public:
    ArcIt() {}

    ArcIt(Invalid i) : Arc(i) {}

    explicit ArcIt(const BpDigraph& graph) : _graph(&graph) {
      _graph->first(static_cast<Arc&>(*this));
    }

    ArcIt(const BpDigraph& graph, const Arc& arc) : Arc(arc), _graph(&graph) {}

    ArcIt& operator++() {
      _graph->next(*this);
      return *this;
    }
  };

  LemonRangeWrapper1<ArcIt, BpDigraph> arcs() const {
    return LemonRangeWrapper1<ArcIt, BpDigraph>(*this);
  }

  class OutArcIt : public Arc {
    const BpDigraph* _graph;

   public:
    OutArcIt() {}

    OutArcIt(Invalid i) : Arc(i) {}

    OutArcIt(const BpDigraph& graph, const Node& node) : _graph(&graph) {
      _graph->firstOut(*this, node);
    }

    OutArcIt(const BpDigraph& graph, const Arc& arc)
        : Arc(arc), _graph(&graph) {}

    OutArcIt& operator++() {
      _graph->nextOut(*this);
      return *this;
    }
  };

  LemonRangeWrapper2<OutArcIt, BpDigraph, Node> outArcs(const Node& u) const {
    return LemonRangeWrapper2<OutArcIt, BpDigraph, Node>(*this, u);
  }

  class InArcIt : public Arc {
    const BpDigraph* _graph;

   public:
    InArcIt() {}

    InArcIt(Invalid i) : Arc(i) {}

    InArcIt(const BpDigraph& graph, const Node& node) : _graph(&graph) {
      _graph->firstIn(*this, node);
    }

    InArcIt(const BpDigraph& graph, const Arc& arc)
        : Arc(arc), _graph(&graph) {}

    InArcIt& operator++() {
      _graph->nextIn(*this);
      return *this;
    }
  };

  LemonRangeWrapper2<InArcIt, BpDigraph, Node> inArcs(const Node& u) const {
    return LemonRangeWrapper2<InArcIt, BpDigraph, Node>(*this, u);
  }

  // \brief Base node of the iterator
  //
  // Returns the base node (ie. the source in this case) of the iterator
  Node baseNode(const OutArcIt& arc) const {
    return Parent::source(static_cast<const Arc&>(arc));
  }
  // \brief Running node of the iterator
  //
  // Returns the running node (ie. the target in this case) of the
  // iterator
  Node runningNode(const OutArcIt& arc) const {
    return Parent::target(static_cast<const Arc&>(arc));
  }

  // \brief Base node of the iterator
  //
  // Returns the base node (ie. the target in this case) of the iterator
  Node baseNode(const InArcIt& arc) const {
    return Parent::target(static_cast<const Arc&>(arc));
  }
  // \brief Running node of the iterator
  //
  // Returns the running node (ie. the source in this case) of the
  // iterator
  Node runningNode(const InArcIt& arc) const {
    return Parent::source(static_cast<const Arc&>(arc));
  }

  // Mappable extension

  template <typename _Value>
  class NodeMap : public MapExtender<DefaultMap<BpDigraph, Node, _Value> > {
    typedef MapExtender<DefaultMap<BpDigraph, Node, _Value> > Parent;

   public:
    explicit NodeMap(const BpDigraph& bpgraph) : Parent(bpgraph) {}
    NodeMap(const BpDigraph& bpgraph, const _Value& value)
        : Parent(bpgraph, value) {}

   private:
    NodeMap& operator=(const NodeMap& cmap) { return operator=<NodeMap>(cmap); }

    template <typename CMap>
    NodeMap& operator=(const CMap& cmap) {
      Parent::operator=(cmap);
      return *this;
    }
  };

  template <typename _Value>
  class RedNodeMap
      : public MapExtender<DefaultMap<BpDigraph, RedNode, _Value> > {
    typedef MapExtender<DefaultMap<BpDigraph, RedNode, _Value> > Parent;

   public:
    explicit RedNodeMap(const BpDigraph& bpgraph) : Parent(bpgraph) {}
    RedNodeMap(const BpDigraph& bpgraph, const _Value& value)
        : Parent(bpgraph, value) {}

   private:
    RedNodeMap& operator=(const RedNodeMap& cmap) {
      return operator=<RedNodeMap>(cmap);
    }

    template <typename CMap>
    RedNodeMap& operator=(const CMap& cmap) {
      Parent::operator=(cmap);
      return *this;
    }
  };

  template <typename _Value>
  class BlueNodeMap
      : public MapExtender<DefaultMap<BpDigraph, BlueNode, _Value> > {
    typedef MapExtender<DefaultMap<BpDigraph, BlueNode, _Value> > Parent;

   public:
    explicit BlueNodeMap(const BpDigraph& bpgraph) : Parent(bpgraph) {}
    BlueNodeMap(const BpDigraph& bpgraph, const _Value& value)
        : Parent(bpgraph, value) {}

   private:
    BlueNodeMap& operator=(const BlueNodeMap& cmap) {
      return operator=<BlueNodeMap>(cmap);
    }

    template <typename CMap>
    BlueNodeMap& operator=(const CMap& cmap) {
      Parent::operator=(cmap);
      return *this;
    }
  };

  template <typename _Value>
  class ArcMap : public MapExtender<DefaultMap<BpDigraph, Arc, _Value> > {
    typedef MapExtender<DefaultMap<BpDigraph, Arc, _Value> > Parent;

   public:
    explicit ArcMap(const BpDigraph& graph) : Parent(graph) {}
    ArcMap(const BpDigraph& graph, const _Value& value)
        : Parent(graph, value) {}

   private:
    ArcMap& operator=(const ArcMap& cmap) { return operator=<ArcMap>(cmap); }

    template <typename CMap>
    ArcMap& operator=(const CMap& cmap) {
      Parent::operator=(cmap);
      return *this;
    }
  };

  // Alteration extension

  void clear() {
    notifier(Arc()).clear();
    notifier(Node()).clear();
    notifier(BlueNode()).clear();
    notifier(RedNode()).clear();
    Parent::clear();
  }

  template <typename Digraph, typename NodeRefMap, typename ArcRefMap>
  void build(const Digraph& graph, NodeRefMap& nodeRef, ArcRefMap& arcRef) {
    Parent::build(graph, nodeRef, arcRef);
    notifier(RedNode()).build();
    notifier(BlueNode()).build();
    notifier(Node()).build();
    notifier(Arc()).build();
  }

  BpDigraphExtender() {
    red_node_notifier.setContainer(*this);
    blue_node_notifier.setContainer(*this);
    node_notifier.setContainer(*this);
    arc_notifier.setContainer(*this);
  }

  ~BpDigraphExtender() {
    arc_notifier.clear();
    node_notifier.clear();
    blue_node_notifier.clear();
    red_node_notifier.clear();
  }
};

}  // namespace lemon

#endif
