# Manual

1. Clone [Schmitzer's Multiscale Algorithm](https://github.com/bernhard-schmitzer/MultiScaleOT) into `MultiScaleOT`
2. Compile it (in the MultiScaleOT folder)
```
mkdir build
cd build
cmake -DUSE_LEMON=On ../src
cmake --build .
```
3. Download DOTmark tests from [here](https://www.stochastik.math.uni-goettingen.de/index.php?id=215/)
4. Compile `GridOT` (in the root source directory)
```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DULMON_COMPILE_BENCHMARK=On ..
cmake --build .
```
