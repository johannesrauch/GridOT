#ifndef ULMON_UTILS_METRIC_H
#define ULMON_UTILS_METRIC_H

#include <array>

namespace lemon {

template <typename T, int D = 2>
class SquaredEuclidean {
  using TDArray = std::array<T, D>;

 public:
  inline T operator()(const TDArray& a, const TDArray& b) const {
    T result{0};
    for (int i = 0; i < D; ++i) {
      T diff = a[i] - b[i];
      result += diff * diff;
    }
    return result;
  }
};

};  // namespace lemon

#endif
