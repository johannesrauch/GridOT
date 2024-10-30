#ifndef ULMON_CONCEPTS_SHIELD_GENERATOR_H
#define ULMON_CONCEPTS_SHIELD_GENERATOR_H

#include <lemon/concepts/bpgraph.h>

namespace lemon {

namespace concepts {

/// @brief This class comprises all methods we need to implement Schmitzer's
/// method "shield", cf. his article https://arxiv.org/abs/1510.05466
class ShieldGenerator : public BpGraph {
  /// @brief Computes a (not necessarily complete) shielding candidate set S for
  /// a supply node, cf. Schmitzer's set S(x_A); the shield is t(x) for all x in
  /// S, where (x, t(x)) is in the flow support
  void heuristicShield(const RedNode &, std::vector<RedNode> &) const {}

  /// @brief Computes a complete shield set S for a supply node, cf. Schmitzer's
  /// set hat{Y}
  void fullShield(const RedNode &, const std::vector<RedNode> &,
                  const RedNodeMap<BlueNode> &, std::vector<BlueNode> &) const {
  }
};

};  // namespace concepts

};  // namespace lemon

#endif
