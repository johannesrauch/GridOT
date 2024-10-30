/* -*- mode: C++; indent-tabs-mode: nil; -*-
 *
 * This file extends core.h of LEMON, a generic C++ optimization library.
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

#ifndef ULMON_CORE_H
#define ULMON_CORE_H

#include <lemon/core.h>

#ifndef ULMON_CONST_RESERVE
#define ULMON_CONST_RESERVE 4
#endif

///\file
///\brief ULMON core utilities.
///
///This header file contains core utilities for ULMON.
///It is automatically included by all graph types, therefore it usually
///do not have to be included directly.


namespace lemon {

  /// \addtogroup gutils
  /// @{

  ///Create convenience typedefs for the bipartite digraph types and iterators

  ///This \c \#define creates the same convenient type definitions as
  ///defined by \ref DIGRAPH_TYPEDEFS(BpDiGraph) and ten more, namely it
  ///creates \c RedNode, \c RedNodeIt, \c BoolRedNodeMap,
  ///\c IntRedNodeMap, \c DoubleRedNodeMap, \c BlueNode, \c BlueNodeIt,
  ///\c BoolBlueNodeMap, \c IntBlueNodeMap, \c DoubleBlueNodeMap.
  ///
  ///\note If the graph type is a dependent type, ie. the graph type depend
  ///on a template parameter, then use \c TEMPLATE_BPDIGRAPH_TYPEDEFS()
  ///macro.
#define BPDIGRAPH_TYPEDEFS(BpDiGraph)                                       \
  DIGRAPH_TYPEDEFS(BpDiGraph);                                              \
  typedef BpDiGraph::RedNode RedNode;                                     \
  typedef BpDiGraph::RedNodeIt RedNodeIt;                                 \
  typedef BpDiGraph::RedNodeMap<bool> BoolRedNodeMap;                     \
  typedef BpDiGraph::RedNodeMap<int> IntRedNodeMap;                       \
  typedef BpDiGraph::RedNodeMap<double> DoubleRedNodeMap;                 \
  typedef BpDiGraph::BlueNode BlueNode;                                   \
  typedef BpDiGraph::BlueNodeIt BlueNodeIt;                               \
  typedef BpDiGraph::BlueNodeMap<bool> BoolBlueNodeMap;                   \
  typedef BpDiGraph::BlueNodeMap<int> IntBlueNodeMap;                     \
  typedef BpDiGraph::BlueNodeMap<double> DoubleBlueNodeMap

  ///Create convenience typedefs for the bipartite digraph types and iterators

  ///\see BPGRAPH_TYPEDEFS
  ///
  ///\note Use this macro, if the graph type is a dependent type,
  ///ie. the graph type depend on a template parameter.
#define TEMPLATE_BPDIGRAPH_TYPEDEFS(BpDiGraph)                                  \
  TEMPLATE_DIGRAPH_TYPEDEFS(BpDiGraph);                                         \
  typedef typename BpDiGraph::RedNode RedNode;                                \
  typedef typename BpDiGraph::RedNodeIt RedNodeIt;                            \
  typedef typename BpDiGraph::template RedNodeMap<bool> BoolRedNodeMap;       \
  typedef typename BpDiGraph::template RedNodeMap<int> IntRedNodeMap;         \
  typedef typename BpDiGraph::template RedNodeMap<double> DoubleRedNodeMap;   \
  typedef typename BpDiGraph::BlueNode BlueNode;                              \
  typedef typename BpDiGraph::BlueNodeIt BlueNodeIt;                          \
  typedef typename BpDiGraph::template BlueNodeMap<bool> BoolBlueNodeMap;     \
  typedef typename BpDiGraph::template BlueNodeMap<int> IntBlueNodeMap;       \
  typedef typename BpDiGraph::template BlueNodeMap<double> DoubleBlueNodeMap

  /// @}
  
} //namespace lemon

#endif
