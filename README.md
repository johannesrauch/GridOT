# GridOT - a discrete optimal transport solver on grids

We refer to this [article](https://github.com/johannesrauch/GridOT/blob/main/docs/main.pdf) for a description.

## Dependencies

- `g++` supporting `c++17`.
- `cmake`, version >= 3.15.
- [LEMON](https://lemon.cs.elte.hu/hg/lemon/) (Boost Software License, Version 1.0).
- [MultiScaleOT](https://github.com/bernhard-schmitzer/MultiScaleOT) (only for the benchmark, see `benchmark/README.md`).

## Building

Currently only linux builds are supported.

### Installing LEMON

Download [LEMON](https://lemon.cs.elte.hu/hg/lemon/) and install it where `cmake` can find it.

### Building the tests

Execute the following commands in the root source directory.

```
mkdir build
cd build
cmake ..
cmake --build .
```