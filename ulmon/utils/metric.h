#ifndef ULMON_UTILS_METRIC_H
#define ULMON_UTILS_METRIC_H

#include <array>

namespace lemon {

template <typename T, int D = 2>
class SquaredEuclidean {
  using IntDArray = std::array<int, D>;

 public:
  inline T operator()(const IntDArray& a, const IntDArray& b) const {
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
