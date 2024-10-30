#include <ulmon/concepts/bpdigraph.h>
#include <ulmon/smart_bpdigraph.h>

#include <algorithm>
#include <cassert>
#include <vector>

using namespace lemon;

int main() {
  // If this compiles, then SmartBpDigraph fulfills the BpDigraph concept
  lemon::concepts::BpDigraph::Constraints<SmartBpDigraph> constraints;
  constraints.constraints();

  SmartBpDigraph graph(3, 4);
  assert(graph.redNum() == 3);
  assert(graph.blueNum() == 4);
  assert(graph.nodeNum() == 7);

  // Node iterator
  std::vector<bool> v(7);
  for (SmartBpDigraph::NodeIt n(graph); n != INVALID; ++n)
    v[graph.id(n)] = true;
  assert(std::all_of(v.begin(), v.end(), [](const bool& b) { return b; }));

  // RedNode iterator
  v.resize(3);
  std::fill(v.begin(), v.end(), false);
  for (SmartBpDigraph::RedNodeIt n(graph); n != INVALID; ++n)
    v[graph.id(n)] = true;
  assert(std::all_of(v.begin(), v.end(), [](const bool& b) { return b; }));

  // BlueNode iterator
  v.resize(4);
  std::fill(v.begin(), v.end(), false);
  for (SmartBpDigraph::BlueNodeIt n(graph); n != INVALID; ++n)
    v[graph.id(n)] = true;
  assert(std::all_of(v.begin(), v.end(), [](const bool& b) { return b; }));

  auto u1 = graph.redNode(0);
  auto u2 = graph.redNode(1);
  auto u3 = graph.redNode(2);
  auto v1 = graph.blueNode(0);
  auto v2 = graph.blueNode(1);
  auto v3 = graph.blueNode(2);
  auto v4 = graph.blueNode(3);
  graph.addArc(u1, v1);
  graph.addArc(u1, v2);
  graph.addArc(u1, v3);
  graph.addArc(u1, v4);
  graph.addArc(u3, v2);
  assert(graph.arcNum() == 5);

  // Arc iterator
  v.resize(5);
  std::fill(v.begin(), v.end(), false);
  for (SmartBpDigraph::ArcIt a(graph); a != INVALID; ++a) v[graph.id(a)] = true;
  assert(std::all_of(v.begin(), v.end(), [](const bool& b) { return b; }));

  // Out arc iterator
  v.resize(5);
  std::fill(v.begin(), v.end(), false);
  for (SmartBpDigraph::OutArcIt a(graph, u1); a != INVALID; ++a)
    v[graph.id(a)] = true;
  assert(v[0] && v[1] && v[2] && v[3] && !v[4]);

  for (SmartBpDigraph::OutArcIt a(graph, u2); a != INVALID; ++a) assert(false);

  std::fill(v.begin(), v.end(), false);
  for (SmartBpDigraph::OutArcIt a(graph, u3); a != INVALID; ++a)
    v[graph.id(a)] = true;
  assert(!v[0] && !v[1] && !v[2] && !v[3] && v[4]);

  for (SmartBpDigraph::OutArcIt a(graph, v1); a != INVALID; ++a) assert(false);

  // In arc iterator
  std::fill(v.begin(), v.end(), false);
  for (SmartBpDigraph::InArcIt a(graph, v1); a != INVALID; ++a)
    v[graph.id(a)] = true;
  assert(v[0] && !v[1] && !v[2] && !v[3] && !v[4]);

  std::fill(v.begin(), v.end(), false);
  for (SmartBpDigraph::InArcIt a(graph, v2); a != INVALID; ++a)
    v[graph.id(a)] = true;
  assert(!v[0] && v[1] && !v[2] && !v[3] && v[4]);

  for (SmartBpDigraph::InArcIt a(graph, u1); a != INVALID; ++a) assert(false);
}
