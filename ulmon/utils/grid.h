// Adapted from Schmitzer's gridtools

#ifndef ULMON_UTILS_GRID_H
#define ULMON_UTILS_GRID_H

#include <cassert>
#include <limits>
#include <vector>

namespace lemon {

namespace utils {

//
// Arc and node numbers
//

// Returns number of nodes in grid
template <typename Array>
inline int numNodes(const Array& gridDim) {
  constexpr int dim = std::tuple_size<Array>{};
  int n = 1;
  for (int i = 0; i < dim; ++i) {
    n *= gridDim[i];
    assert(n >= 0);  // Overflow detection
  }
  return n;
}

// Returns number of nodes in rectangle
template <typename Array>
inline int numNodes(const Array& min, const Array& max) {
  constexpr int dim = std::tuple_size<Array>{};
  int n = 1;
  for (int i = 0; i < dim; ++i) {
    n *= std::max(max[i] - min[i], 0);
    assert(n >= 0);  // Overflow detection
  }
  return n;
}

template <typename PosVector>
inline int numArcs(const PosVector& yMin, const PosVector& yMax) {
  assert(yMin.size() == yMax.size());
  const int n = yMin.size();
  int m = 0;
  for (int i = 0; i < n; ++i) {
    m += numNodes(yMin[i], yMax[i]);
    assert(m >= 0);  // Overflow detection
  }
  return m;
}

//
// Strides
//

template <typename Array>
inline void setStrides(const Array& gridDim, Array& strides) {
  constexpr int dim = std::tuple_size<Array>{};
  strides[dim - 1] = 1;
  for (int i = dim - 2; i >= 0; --i) {
    strides[i] = strides[i + 1] * gridDim[i + 1];
    // Overflow detection
    assert(strides[i] >= 0);
  }
}

template <typename Array>
inline void setStrides(const Array& gridDimX, const Array& gridDimY,
                       Array& xStrides, Array& yStrides) {
  setStrides(gridDimX, xStrides);
  setStrides(gridDimY, yStrides);
}

template <typename Array>
inline Array getStrides(const Array& gridDim) {
  Array strides;
  setStrides(gridDim, strides);
  return strides;
}

//
// Position <-> ID
//

// Grid position from id
template <typename Array>
inline void posFromId(int id, const Array& strides, Array& pos) {
  constexpr int dim = std::tuple_size<Array>{};
  for (int i = 0; i < dim; ++i) {
    pos[i] = id / strides[i];
    id = id % strides[i];
  }
}

// Grid position coordinate pos[i] from id
template <typename Array>
inline int posIFromId(int id, const Array& strides, const int i) {
  if (i > 0) id = id % strides[i - 1];
  return id / strides[i];
}

// Grid id from position
template <typename Array>
int idFromPos(const Array& pos, const Array& strides) {
  constexpr int dim = std::tuple_size<Array>{};
  int id = 0;
  for (int i = 0; i < dim; ++i) {
    id += pos[i] * strides[i];
  }
  return id;
}

//
// Coarsening
//

template <typename Array>
inline void coarsenedGridDim(  //
    const int numMerge, const Array& gridDim, Array& newGridDim) {
  constexpr int dim = std::tuple_size<Array>{};
  for (int i = 0; i < dim; ++i) {
    int r = gridDim[i] / numMerge;
    int q = gridDim[i] % numMerge;
    newGridDim[i] = r + (q > 0);
  }
}

template <typename Array>
inline Array getCoarsenedGridDim(const int numMerge, const Array& gridDim) {
  Array newGridDim{};
  coarsenedGridDim(numMerge, gridDim, newGridDim);
  return newGridDim;
}

template <typename Array>
inline void coarsenedGridDim(                      //
    const int numMerge,                            //
    const Array& gridDimX, const Array& gridDimY,  //
    Array& newGridDimX, Array& newGridDimY) {
  coarsenedGridDim(numMerge, gridDimX, newGridDimX);
  coarsenedGridDim(numMerge, gridDimY, newGridDimY);
}

/// \brief Writes the multi-index of the representative of \c pos (after merging
/// \c numMerge points in each dimension) into \c newInd
template <typename Array>
inline void coarsenedIndex(  //
    const int numMerge, const Array& pos, Array& newInd) {
  constexpr int dim = std::tuple_size<Array>{};
  for (int i = 0; i < dim; ++i) {
    newInd[i] = pos[i] / numMerge;
  }
}

/// \brief Writes the multi-index of the representative of \c pos (after merging
/// \c numMerge points in each dimension) into \c newPos
template <typename Array>
inline void coarsenedPos(  //
    const int numMerge, const Array& pos, Array& newPos) {
  constexpr int dim = std::tuple_size<Array>{};
  for (int i = 0; i < dim; ++i) {
    newPos[i] = pos[i] / numMerge;
  }
}

//
// More position utils
//

/// \brief Advances \c pos in the hyperrectangle spanned by \c min and \c max
template <typename Array>
inline void advancePos(const Array& max, Array& pos) {
  int d = pos.size();
  assert(d > 0);
  do {
    --d;
    ++pos[d];
    if (pos[d] >= max[d]) pos[d] = 0;
  } while (pos[d] == 0 && d > 0);
}

/// \brief Advances \c pos in the hyperrectangle spanned by \c min and \c max
template <typename Array>
inline void advancePos(  //
    const Array& min, const Array& max, Array& pos) {
  int d = pos.size();
  assert(d > 0);
  do {
    --d;
    ++pos[d];
    if (pos[d] >= max[d]) pos[d] = min[d];
  } while (pos[d] == min[d] && d > 0);
}

//
// Hierarchy
//

inline int ceil_log(int n, int b) {
  if (n <= 1) return 0;  // log_b(1) is 0, and for n <= 0 it's undefined

  int res = 0;
  int power = 1;  // Start with b^0 = 1

  while (power < n) {
    power *= b;
    ++res;
  }

  return res;
}

/// \brief Returns a suitable hierarchical depth for the multiscale solver
template <typename Array>
inline int hierarchicalDepth(const Array& gridDimX, const Array& gridDimY,
                             const int merge_num) {
  constexpr int dim = std::tuple_size<Array>{};
  int min = std::numeric_limits<int>::max();
  for (const auto& d : gridDimX) min = std::min(min, d);
  for (const auto& d : gridDimY) min = std::min(min, d);
  return std::max(0, ceil_log(min, merge_num) - 1);
}

//
// Min, max, less
//

template <typename Array>
inline void min(const Array& a, Array& b) {
  constexpr int dim = std::tuple_size<Array>{};
  for (int i = 0; i < dim; ++i) b[i] = std::min(a[i], b[i]);
}

template <typename Array>
inline void max(const Array& a, Array& b) {
  constexpr int dim = std::tuple_size<Array>{};
  for (int i = 0; i < dim; ++i) b[i] = std::max(a[i], b[i]);
}

template <typename Array>
inline bool less(const Array& a, const Array& b) {
  constexpr int dim = std::tuple_size<Array>{};
  for (int i = 0; i < dim; ++i)
    if (a[i] >= b[i]) return false;
  return true;
}

template <typename Array>
inline bool leq(const Array& a, const Array& b) {
  constexpr int dim = std::tuple_size<Array>{};
  for (int i = 0; i < dim; ++i)
    if (a[i] > b[i]) return false;
  return true;
}

template <typename Array>
inline bool contains(const Array& min, const Array& max, const Array& mi,
                     const Array& ma) {
  return leq(min, mi) && less(ma, max);
}

template <typename Array>
inline bool contains(const Array& min, const Array& max, const Array& pos) {
  return leq(min, pos) && less(pos, max);
}

};  // namespace utils

};  // namespace lemon

#endif
