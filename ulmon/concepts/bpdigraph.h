/*
-*- mode: C++; indent-tabs-mode: nil; -*-
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
 * ---
 *
 * This file is adapted from LEMON.
 */

///\ingroup graph_concepts
///\file
///\brief The concept of bipartite directed graphs.

#ifndef ULMON_CONCEPTS_BPDIGRAPH_H
#define ULMON_CONCEPTS_BPDIGRAPH_H

#include <lemon/concept_check.h>
#include <lemon/concepts/graph_components.h>
#include <lemon/concepts/maps.h>
#include <lemon/core.h>

namespace lemon {
namespace concepts {

/// \brief Base skeleton class for undirected bipartite graphs.
///
/// This class describes the base interface of undirected
/// bipartite graph types.  All bipartite graph %concepts have to
/// conform to this class.  It extends the interface of \ref
/// BaseDigraphComponent with an \c Arc type and functions to get
/// the end nodes of arcs, etc.
class BaseBpDigraphComponent : public BaseDigraphComponent {
 public:
  typedef BaseBpDigraphComponent BpGraph;

  typedef BaseDigraphComponent::Node Node;
  typedef BaseDigraphComponent::Arc Arc;

  /// \brief Class to represent red nodes.
  ///
  /// This class represents the red nodes of the graph. The red
  /// nodes can also be used as normal nodes.
  class RedNode : public Node {
    typedef Node Parent;

   public:
    /// \brief Default constructor.
    ///
    /// Default constructor.
    /// \warning The default constructor is not required to set
    /// the item to some well-defined value. So you should consider it
    /// as uninitialized.
    RedNode() {}

    /// \brief Copy constructor.
    ///
    /// Copy constructor.
    RedNode(const RedNode&) : Parent() {}

    /// \brief Constructor for conversion from \c INVALID.
    ///
    /// Constructor for conversion from \c INVALID.
    /// It initializes the item to be invalid.
    /// \sa Invalid for more details.
    RedNode(Invalid) {}
  };

  /// \brief Class to represent blue nodes.
  ///
  /// This class represents the blue nodes of the graph. The blue
  /// nodes can also be used as normal nodes.
  class BlueNode : public Node {
    typedef Node Parent;

   public:
    /// \brief Default constructor.
    ///
    /// Default constructor.
    /// \warning The default constructor is not required to set
    /// the item to some well-defined value. So you should consider it
    /// as uninitialized.
    BlueNode() {}

    /// \brief Copy constructor.
    ///
    /// Copy constructor.
    BlueNode(const BlueNode&) : Parent() {}

    /// \brief Constructor for conversion from \c INVALID.
    ///
    /// Constructor for conversion from \c INVALID.
    /// It initializes the item to be invalid.
    /// \sa Invalid for more details.
    BlueNode(Invalid) {}

    /// \brief Constructor for conversion from a node.
    ///
    /// Constructor for conversion from a node. The conversion can
    /// be invalid, since the Node can be member of the red
    /// set.
    BlueNode(const Node&) {}
  };

  /// \brief Gives back %true for red nodes.
  ///
  /// Gives back %true for red nodes.
  bool red(const Node&) const { return true; }

  /// \brief Gives back %true for blue nodes.
  ///
  /// Gives back %true for blue nodes.
  bool blue(const Node&) const { return true; }

  /// \brief Converts the node to red node object.
  ///
  /// This function converts unsafely the node to red node
  /// object. It should be called only if the node is from the red
  /// partition or INVALID.
  RedNode asRedNodeUnsafe(const Node&) const { return RedNode(); }

  /// \brief Converts the node to blue node object.
  ///
  /// This function converts unsafely the node to blue node
  /// object. It should be called only if the node is from the red
  /// partition or INVALID.
  BlueNode asBlueNodeUnsafe(const Node&) const { return BlueNode(); }

  /// \brief Converts the node to red node object.
  ///
  /// This function converts safely the node to red node
  /// object. If the node is not from the red partition, then it
  /// returns INVALID.
  RedNode asRedNode(const Node&) const { return RedNode(); }

  /// \brief Converts the node to blue node object.
  ///
  /// This function converts unsafely the node to blue node
  /// object. If the node is not from the blue partition, then it
  /// returns INVALID.
  BlueNode asBlueNode(const Node&) const { return BlueNode(); }

  template <typename _BpGraph>
  struct Constraints {
    typedef typename _BpGraph::Node Node;
    typedef typename _BpGraph::RedNode RedNode;
    typedef typename _BpGraph::BlueNode BlueNode;
    typedef typename _BpGraph::Arc Arc;

    void constraints() {
      checkConcept<BaseDigraphComponent, _BpGraph>();
      checkConcept<GraphItem<'n'>, RedNode>();
      checkConcept<GraphItem<'n'>, BlueNode>();
      {
        Node n;
        RedNode rn;
        BlueNode bn;
        Node rnan = rn;
        Node bnan = bn;
        bool b;
        b = bpgraph.red(rnan);
        b = bpgraph.blue(bnan);
        rn = bpgraph.asRedNodeUnsafe(rnan);
        bn = bpgraph.asBlueNodeUnsafe(bnan);
        rn = bpgraph.asRedNode(rnan);
        bn = bpgraph.asBlueNode(bnan);
        ::lemon::ignore_unused_variable_warning(b);
      }
    }

    const _BpGraph& bpgraph;
  };
};

/// \ingroup graph_concepts
///
/// \brief Class describing the concept of directed bipartite graphs.
///
/// This class describes the common interface of all directed
/// bipartite graphs.
///
/// Like all concept classes, it only provides an interface
/// without any sensible implementation. So any general algorithm for
/// undirected bipartite graphs should compile with this class,
/// but it will not run properly, of course.
/// An actual graph implementation like \ref SmartBpDigraph may have
/// additional functionality.
///
/// The bipartite graphs also fulfill the concept of \ref Graph
/// "undirected graphs". Bipartite graphs provide a bipartition of
/// the node set, namely a red and blue set of the nodes. The
/// nodes can be iterated with the RedNodeIt and BlueNodeIt in the
/// two node sets. With RedNodeMap and BlueNodeMap values can be
/// assigned to the nodes in the two sets.
///
/// The arcs of the graph cannot connect two nodes of the same
/// set. The arcs orientation is from the red nodes to
/// the blue nodes.
///
/// \sa Graph
class BpDigraph {
 private:
  /// BpDigraphs are \e not copy constructible. Use BpDigraphCopy instead.
  BpDigraph(const BpDigraph&) {}
  /// \brief Assignment of a graph to another one is \e not allowed.
  /// Use BpDigraphCopy instead.
  void operator=(const BpDigraph&) {}

 public:
  /// Default constructor.
  BpDigraph() {}

  /// \brief Undirected graphs should be tagged with \c UndirectedTag.
  ///
  /// Undirected graphs should be tagged with \c UndirectedTag.
  ///
  /// This tag helps the \c enable_if technics to make compile time
  /// specializations for undirected graphs.
  typedef True UndirectedTag;

  /// The node type of the graph

  /// This class identifies a node of the graph. It also serves
  /// as a base class of the node iterators,
  /// thus they convert to this type.
  class Node {
   public:
    /// Default constructor

    /// Default constructor.
    /// \warning It sets the object to an undefined value.
    Node() {}
    /// Copy constructor.

    /// Copy constructor.
    ///
    Node(const Node&) {}

    /// %Invalid constructor \& conversion.

    /// Initializes the object to be invalid.
    /// \sa Invalid for more details.
    Node(Invalid) {}
    /// Equality operator

    /// Equality operator.
    ///
    /// Two iterators are equal if and only if they point to the
    /// same object or both are \c INVALID.
    bool operator==(Node) const { return true; }

    /// Inequality operator

    /// Inequality operator.
    bool operator!=(Node) const { return true; }

    /// Artificial ordering operator.

    /// Artificial ordering operator.
    ///
    /// \note This operator only has to define some strict ordering of
    /// the items; this order has nothing to do with the iteration
    /// ordering of the items.
    bool operator<(Node) const { return false; }
  };

  /// Class to represent red nodes.

  /// This class represents the red nodes of the graph. It does
  /// not supposed to be used directly, because the nodes can be
  /// represented as Node instances. This class can be used as
  /// template parameter for special map classes.
  class RedNode : public Node {
   public:
    /// Default constructor

    /// Default constructor.
    /// \warning It sets the object to an undefined value.
    RedNode() {}
    /// Copy constructor.

    /// Copy constructor.
    ///
    RedNode(const RedNode&) : Node() {}

    /// %Invalid constructor \& conversion.

    /// Initializes the object to be invalid.
    /// \sa Invalid for more details.
    RedNode(Invalid) {}
  };

  /// Class to represent blue nodes.

  /// This class represents the blue nodes of the graph. It does
  /// not supposed to be used directly, because the nodes can be
  /// represented as Node instances. This class can be used as
  /// template parameter for special map classes.
  class BlueNode : public Node {
   public:
    /// Default constructor

    /// Default constructor.
    /// \warning It sets the object to an undefined value.
    BlueNode() {}
    /// Copy constructor.

    /// Copy constructor.
    ///
    BlueNode(const BlueNode&) : Node() {}

    /// %Invalid constructor \& conversion.

    /// Initializes the object to be invalid.
    /// \sa Invalid for more details.
    BlueNode(Invalid) {}
  };

  /// Iterator class for the red nodes.

  /// This iterator goes through each red node of the graph.
  /// Its usage is quite simple, for example, you can count the number
  /// of red nodes in a graph \c g of type \c %BpDigraph like this:
  ///\code
  /// int count=0;
  /// for (BpDigraph::RedNodeIt n(g); n!=INVALID; ++n) ++count;
  ///\endcode
  class RedNodeIt : public RedNode {
   public:
    /// Default constructor

    /// Default constructor.
    /// \warning It sets the iterator to an undefined value.
    RedNodeIt() {}
    /// Copy constructor.

    /// Copy constructor.
    ///
    RedNodeIt(const RedNodeIt& n) : RedNode(n) {}
    /// %Invalid constructor \& conversion.

    /// Initializes the iterator to be invalid.
    /// \sa Invalid for more details.
    RedNodeIt(Invalid) {}
    /// Sets the iterator to the first red node.

    /// Sets the iterator to the first red node of the given
    /// digraph.
    explicit RedNodeIt(const BpDigraph&) {}
    /// Sets the iterator to the given red node.

    /// Sets the iterator to the given red node of the given
    /// digraph.
    RedNodeIt(const BpDigraph&, const RedNode&) {}
    /// Next node.

    /// Assign the iterator to the next red node.
    ///
    RedNodeIt& operator++() { return *this; }
  };

  /// Iterator class for the blue nodes.

  /// This iterator goes through each blue node of the graph.
  /// Its usage is quite simple, for example, you can count the number
  /// of blue nodes in a graph \c g of type \c %BpDigraph like this:
  ///\code
  /// int count=0;
  /// for (BpDigraph::BlueNodeIt n(g); n!=INVALID; ++n) ++count;
  ///\endcode
  class BlueNodeIt : public BlueNode {
   public:
    /// Default constructor

    /// Default constructor.
    /// \warning It sets the iterator to an undefined value.
    BlueNodeIt() {}
    /// Copy constructor.

    /// Copy constructor.
    ///
    BlueNodeIt(const BlueNodeIt& n) : BlueNode(n) {}
    /// %Invalid constructor \& conversion.

    /// Initializes the iterator to be invalid.
    /// \sa Invalid for more details.
    BlueNodeIt(Invalid) {}
    /// Sets the iterator to the first blue node.

    /// Sets the iterator to the first blue node of the given
    /// digraph.
    explicit BlueNodeIt(const BpDigraph&) {}
    /// Sets the iterator to the given blue node.

    /// Sets the iterator to the given blue node of the given
    /// digraph.
    BlueNodeIt(const BpDigraph&, const BlueNode&) {}
    /// Next node.

    /// Assign the iterator to the next blue node.
    ///
    BlueNodeIt& operator++() { return *this; }
  };

  /// Iterator class for the nodes.

  /// This iterator goes through each node of the graph.
  /// Its usage is quite simple, for example, you can count the number
  /// of nodes in a graph \c g of type \c %BpDigraph like this:
  ///\code
  /// int count=0;
  /// for (BpDigraph::NodeIt n(g); n!=INVALID; ++n) ++count;
  ///\endcode
  class NodeIt : public Node {
   public:
    /// Default constructor

    /// Default constructor.
    /// \warning It sets the iterator to an undefined value.
    NodeIt() {}
    /// Copy constructor.

    /// Copy constructor.
    ///
    NodeIt(const NodeIt& n) : Node(n) {}
    /// %Invalid constructor \& conversion.

    /// Initializes the iterator to be invalid.
    /// \sa Invalid for more details.
    NodeIt(Invalid) {}
    /// Sets the iterator to the first node.

    /// Sets the iterator to the first node of the given digraph.
    ///
    explicit NodeIt(const BpDigraph&) {}
    /// Sets the iterator to the given node.

    /// Sets the iterator to the given node of the given digraph.
    ///
    NodeIt(const BpDigraph&, const Node&) {}
    /// Next node.

    /// Assign the iterator to the next node.
    ///
    NodeIt& operator++() { return *this; }
  };

  /// The arc type of the graph

  /// This class identifies a directed arc of the graph. It also serves
  /// as a base class of the arc iterators,
  /// thus they will convert to this type.
  class Arc {
   public:
    /// Default constructor

    /// Default constructor.
    /// \warning It sets the object to an undefined value.
    Arc() {}
    /// Copy constructor.

    /// Copy constructor.
    ///
    Arc(const Arc&) {}
    /// %Invalid constructor \& conversion.

    /// Initializes the object to be invalid.
    /// \sa Invalid for more details.
    Arc(Invalid) {}
    /// Equality operator

    /// Equality operator.
    ///
    /// Two iterators are equal if and only if they point to the
    /// same object or both are \c INVALID.
    bool operator==(Arc) const { return true; }
    /// Inequality operator

    /// Inequality operator.
    bool operator!=(Arc) const { return true; }

    /// Artificial ordering operator.

    /// Artificial ordering operator.
    ///
    /// \note This operator only has to define some strict ordering of
    /// the arcs; this order has nothing to do with the iteration
    /// ordering of the arcs.
    bool operator<(Arc) const { return false; }
  };

  /// Iterator class for the arcs.

  /// This iterator goes through each directed arc of the graph.
  /// Its usage is quite simple, for example, you can count the number
  /// of arcs in a graph \c g of type \c %BpDigraph as follows:
  ///\code
  /// int count=0;
  /// for(BpDigraph::ArcIt a(g); a!=INVALID; ++a) ++count;
  ///\endcode
  class ArcIt : public Arc {
   public:
    /// Default constructor

    /// Default constructor.
    /// \warning It sets the iterator to an undefined value.
    ArcIt() {}
    /// Copy constructor.

    /// Copy constructor.
    ///
    ArcIt(const ArcIt& e) : Arc(e) {}
    /// %Invalid constructor \& conversion.

    /// Initializes the iterator to be invalid.
    /// \sa Invalid for more details.
    ArcIt(Invalid) {}
    /// Sets the iterator to the first arc.

    /// Sets the iterator to the first arc of the given graph.
    ///
    explicit ArcIt(const BpDigraph& g) {
      ::lemon::ignore_unused_variable_warning(g);
    }
    /// Sets the iterator to the given arc.

    /// Sets the iterator to the given arc of the given graph.
    ///
    ArcIt(const BpDigraph&, const Arc&) {}
    /// Next arc

    /// Assign the iterator to the next arc.
    ///
    ArcIt& operator++() { return *this; }
  };

  /// Iterator class for the outgoing arcs of a node.

  /// This iterator goes trough the \e outgoing directed arcs of a
  /// certain node of a graph.
  /// Its usage is quite simple, for example, you can count the number
  /// of outgoing arcs of a node \c n
  /// in a graph \c g of type \c %BpDigraph as follows.
  ///\code
  /// int count=0;
  /// for (Digraph::OutArcIt a(g, n); a!=INVALID; ++a) ++count;
  ///\endcode
  class OutArcIt : public Arc {
   public:
    /// Default constructor

    /// Default constructor.
    /// \warning It sets the iterator to an undefined value.
    OutArcIt() {}
    /// Copy constructor.

    /// Copy constructor.
    ///
    OutArcIt(const OutArcIt& e) : Arc(e) {}
    /// %Invalid constructor \& conversion.

    /// Initializes the iterator to be invalid.
    /// \sa Invalid for more details.
    OutArcIt(Invalid) {}
    /// Sets the iterator to the first outgoing arc.

    /// Sets the iterator to the first outgoing arc of the given node.
    ///
    OutArcIt(const BpDigraph& n, const Node& g) {
      ::lemon::ignore_unused_variable_warning(n);
      ::lemon::ignore_unused_variable_warning(g);
    }
    /// Sets the iterator to the given arc.

    /// Sets the iterator to the given arc of the given graph.
    ///
    OutArcIt(const BpDigraph&, const Arc&) {}
    /// Next outgoing arc

    /// Assign the iterator to the next
    /// outgoing arc of the corresponding node.
    OutArcIt& operator++() { return *this; }
  };

  /// Iterator class for the incoming arcs of a node.

  /// This iterator goes trough the \e incoming directed arcs of a
  /// certain node of a graph.
  /// Its usage is quite simple, for example, you can count the number
  /// of incoming arcs of a node \c n
  /// in a graph \c g of type \c %BpDigraph as follows.
  ///\code
  /// int count=0;
  /// for (Digraph::InArcIt a(g, n); a!=INVALID; ++a) ++count;
  ///\endcode
  class InArcIt : public Arc {
   public:
    /// Default constructor

    /// Default constructor.
    /// \warning It sets the iterator to an undefined value.
    InArcIt() {}
    /// Copy constructor.

    /// Copy constructor.
    ///
    InArcIt(const InArcIt& e) : Arc(e) {}
    /// %Invalid constructor \& conversion.

    /// Initializes the iterator to be invalid.
    /// \sa Invalid for more details.
    InArcIt(Invalid) {}
    /// Sets the iterator to the first incoming arc.

    /// Sets the iterator to the first incoming arc of the given node.
    ///
    InArcIt(const BpDigraph& g, const Node& n) {
      ::lemon::ignore_unused_variable_warning(n);
      ::lemon::ignore_unused_variable_warning(g);
    }
    /// Sets the iterator to the given arc.

    /// Sets the iterator to the given arc of the given graph.
    ///
    InArcIt(const BpDigraph&, const Arc&) {}
    /// Next incoming arc

    /// Assign the iterator to the next
    /// incoming arc of the corresponding node.
    InArcIt& operator++() { return *this; }
  };

  /// \brief Standard graph map type for the nodes.
  ///
  /// Standard graph map type for the nodes.
  /// It conforms to the ReferenceMap concept.
  template <class T>
  class NodeMap : public ReferenceMap<Node, T, T&, const T&> {
   public:
    /// Constructor
    explicit NodeMap(const BpDigraph&) {}
    /// Constructor with given initial value
    NodeMap(const BpDigraph&, T) {}

   private:
    /// Copy constructor
    NodeMap(const NodeMap& nm) : ReferenceMap<Node, T, T&, const T&>(nm) {}
    /// Assignment operator
    template <typename CMap>
    NodeMap& operator=(const CMap&) {
      checkConcept<ReadMap<Node, T>, CMap>();
      return *this;
    }
  };

  /// \brief Standard graph map type for the red nodes.
  ///
  /// Standard graph map type for the red nodes.
  /// It conforms to the ReferenceMap concept.
  template <class T>
  class RedNodeMap : public ReferenceMap<Node, T, T&, const T&> {
   public:
    /// Constructor
    explicit RedNodeMap(const BpDigraph&) {}
    /// Constructor with given initial value
    RedNodeMap(const BpDigraph&, T) {}

   private:
    /// Copy constructor
    RedNodeMap(const RedNodeMap& nm)
        : ReferenceMap<Node, T, T&, const T&>(nm) {}
    /// Assignment operator
    template <typename CMap>
    RedNodeMap& operator=(const CMap&) {
      checkConcept<ReadMap<Node, T>, CMap>();
      return *this;
    }
  };

  /// \brief Standard graph map type for the blue nodes.
  ///
  /// Standard graph map type for the blue nodes.
  /// It conforms to the ReferenceMap concept.
  template <class T>
  class BlueNodeMap : public ReferenceMap<Node, T, T&, const T&> {
   public:
    /// Constructor
    explicit BlueNodeMap(const BpDigraph&) {}
    /// Constructor with given initial value
    BlueNodeMap(const BpDigraph&, T) {}

   private:
    /// Copy constructor
    BlueNodeMap(const BlueNodeMap& nm)
        : ReferenceMap<Node, T, T&, const T&>(nm) {}
    /// Assignment operator
    template <typename CMap>
    BlueNodeMap& operator=(const CMap&) {
      checkConcept<ReadMap<Node, T>, CMap>();
      return *this;
    }
  };

  /// \brief Standard graph map type for the arcs.
  ///
  /// Standard graph map type for the arcs.
  /// It conforms to the ReferenceMap concept.
  template <class T>
  class ArcMap : public ReferenceMap<Arc, T, T&, const T&> {
   public:
    /// Constructor
    explicit ArcMap(const BpDigraph&) {}
    /// Constructor with given initial value
    ArcMap(const BpDigraph&, T) {}

   private:
    /// Copy constructor
    ArcMap(const ArcMap& em) : ReferenceMap<Arc, T, T&, const T&>(em) {}
    /// Assignment operator
    template <typename CMap>
    ArcMap& operator=(const CMap&) {
      checkConcept<ReadMap<Arc, T>, CMap>();
      return *this;
    }
  };

  /// \brief Gives back %true for red nodes.
  ///
  /// Gives back %true for red nodes.
  bool red(const Node&) const { return true; }

  /// \brief Gives back %true for blue nodes.
  ///
  /// Gives back %true for blue nodes.
  bool blue(const Node&) const { return true; }

  /// \brief Converts the node to red node object.
  ///
  /// This function converts unsafely the node to red node
  /// object. It should be called only if the node is from the red
  /// partition or INVALID.
  RedNode asRedNodeUnsafe(const Node&) const { return RedNode(); }

  /// \brief Converts the node to blue node object.
  ///
  /// This function converts unsafely the node to blue node
  /// object. It should be called only if the node is from the red
  /// partition or INVALID.
  BlueNode asBlueNodeUnsafe(const Node&) const { return BlueNode(); }

  /// \brief Converts the node to red node object.
  ///
  /// This function converts safely the node to red node
  /// object. If the node is not from the red partition, then it
  /// returns INVALID.
  RedNode asRedNode(const Node&) const { return RedNode(); }

  /// \brief Converts the node to blue node object.
  ///
  /// This function converts unsafely the node to blue node
  /// object. If the node is not from the blue partition, then it
  /// returns INVALID.
  BlueNode asBlueNode(const Node&) const { return BlueNode(); }

  /// \brief The source node of the arc.
  ///
  /// Returns the source node of the given arc.
  Node source(Arc) const { return INVALID; }

  /// \brief The target node of the arc.
  ///
  /// Returns the target node of the given arc.
  Node target(Arc) const { return INVALID; }

  /// \brief The ID of the node.
  ///
  /// Returns the ID of the given node.
  int id(Node) const { return -1; }

  /// \brief The red ID of the node.
  ///
  /// Returns the red ID of the given node.
  int id(RedNode) const { return -1; }

  /// \brief The blue ID of the node.
  ///
  /// Returns the blue ID of the given node.
  int id(BlueNode) const { return -1; }

  /// \brief The ID of the arc.
  ///
  /// Returns the ID of the given arc.
  int id(Arc) const { return -1; }

  /// \brief The node with the given ID.
  ///
  /// Returns the node with the given ID.
  /// \pre The argument should be a valid node ID in the graph.
  Node nodeFromId(int) const { return INVALID; }

  /// \brief The arc with the given ID.
  ///
  /// Returns the arc with the given ID.
  /// \pre The argument should be a valid arc ID in the graph.
  Arc arcFromId(int) const { return INVALID; }

  /// \brief An upper bound on the node IDs.
  ///
  /// Returns an upper bound on the node IDs.
  int maxNodeId() const { return -1; }

  /// \brief An upper bound on the red IDs.
  ///
  /// Returns an upper bound on the red IDs.
  int maxRedId() const { return -1; }

  /// \brief An upper bound on the blue IDs.
  ///
  /// Returns an upper bound on the blue IDs.
  int maxBlueId() const { return -1; }

  /// \brief An upper bound on the arc IDs.
  ///
  /// Returns an upper bound on the arc IDs.
  int maxArcId() const { return -1; }

  /// \brief The direction of the arc.
  ///
  /// Returns \c true if the given arc goes from a red node to a blue node.
  bool direction(Arc) const { return true; }

  void first(Node&) const {}
  void next(Node&) const {}

  void firstRed(RedNode&) const {}
  void nextRed(RedNode&) const {}

  void firstBlue(BlueNode&) const {}
  void nextBlue(BlueNode&) const {}

  void first(Arc&) const {}
  void next(Arc&) const {}

  void firstOut(Arc&, Node) const {}
  void nextOut(Arc&) const {}

  void firstIn(Arc&, Node) const {}
  void nextIn(Arc&) const {}

  // The second parameter is dummy.
  Node fromId(int, Node) const { return INVALID; }
  // The second parameter is dummy.
  Arc fromId(int, Arc) const { return INVALID; }

  // Dummy parameter.
  int maxId(Node) const { return -1; }
  // Dummy parameter.
  int maxId(RedNode) const { return -1; }
  // Dummy parameter.
  int maxId(BlueNode) const { return -1; }
  // Dummy parameter.
  int maxId(Arc) const { return -1; }

  /// \brief The opposite node on the arc.
  ///
  /// Returns the opposite node on the given arc.
  Node oppositeNode(Node, Arc) const { return INVALID; }

  /// \brief The base node of the iterator.
  ///
  /// Returns the base node of the given outgoing arc iterator
  /// (i.e. the source node of the corresponding arc).
  Node baseNode(OutArcIt) const { return INVALID; }

  /// \brief The running node of the iterator.
  ///
  /// Returns the running node of the given outgoing arc iterator
  /// (i.e. the target node of the corresponding arc).
  Node runningNode(OutArcIt) const { return INVALID; }

  /// \brief The base node of the iterator.
  ///
  /// Returns the base node of the given incoming arc iterator
  /// (i.e. the target node of the corresponding arc).
  Node baseNode(InArcIt) const { return INVALID; }

  /// \brief The running node of the iterator.
  ///
  /// Returns the running node of the given incoming arc iterator
  /// (i.e. the source node of the corresponding arc).
  Node runningNode(InArcIt) const { return INVALID; }

  template <typename _BpDigraph>
  struct Constraints {
    void constraints() {
      checkConcept<BaseBpDigraphComponent, _BpDigraph>();
      checkConcept<IterableDigraphComponent<>, _BpDigraph>();
      checkConcept<IDableDigraphComponent<>, _BpDigraph>();
      checkConcept<MappableDigraphComponent<>, _BpDigraph>();
    }
  };
};

}  // namespace concepts

}  // namespace lemon

#endif
