# allo
Course c++ developer: hw3 - Custom allocators

## task
- [pdf](task3.pdf)

## Install
```bash
> git clone https://github.com/nj-eka/allo.git
> cd allo

# config example
> cmake -B ./build -S . \
    -DLOGGING_ENABLE=ON \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTING=ON \
    -DENABLE_COVERAGE=ON \
    -DCMAKE_CXX_FLAGS="-g -O0 -fsanitize=undefined -fsanitize=leak -fno-omit-frame-pointer --coverage"

# build
> cmake --build ./build

# install (* skipped)
> cmake --build ./build --target install

# run tests
> ctest --test-dir build -T Test
...
Test project .../allo/build
    Start 1: alloc_cont_gtest
1/2 Test #1: alloc_cont_gtest .................   Passed    0.05 sec
    Start 2: info_unit_tests
2/2 Test #2: info_unit_tests ..................   Passed    0.01 sec

100% tests passed, 0 tests failed out of 2
# see tests coverage
> ctest --test-dir build -T Coverage
Internal ctest changing into directory: /home/jn/codes/cpp/otus23/allo/build
   Site: vmwgentoo1
   Build name: Linux-c++
Performing coverage
   Processing coverage (each . represents one file):
    ......
   Accumulating results (each . represents one file):
    ...............
        Covered LOC:         387
        Not covered LOC:     27
        Total LOC:           414
        Percentage Coverage: 93.48%
```

## Usage
```sh
> allo --help
Usage:
  -h [ --help ]                      Print this help message
  -v [ --version ]                   Print version
  --info                             Print project info
  -l [ --logging-level ] arg (=info) Logging level
  --logs arg                         Logging output file name [default:clog]
```

## Benchmarks
```sh
> cmake -B ./build -S . \
    -DENABLE_BENCHMARK=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_LOGGING=OFF \
    -DBUILD_TESTING=OFF \
    -DENABLE_COVERAGE=OFF \
    -DCMAKE_CXX_FLAGS="-g -O2"
> cmake --build build
> build/alloc_gbenchmark
Running build/alloc_gbenchmark
Run on (6 X 2494.24 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x6)
  L1 Instruction 32 KiB (x6)
  L2 Unified 256 KiB (x6)
  L3 Unified 6144 KiB (x3)
Load Average: 1.16, 1.23, 1.36
-----------------------------------------------------------------------------------------------
Benchmark                                                     Time             CPU   Iterations
-----------------------------------------------------------------------------------------------
BM_vector_std_alloc_optimum_linear_monotonic               1231 ns         1231 ns       503159
BM_vector_bpool_alloc_optimum_first_linear_monotonic       1119 ns         1119 ns       612835
BM_vector_bpool_alloc_optimum_last_linear_monotonic         996 ns          995 ns       665587
BM_vector_std_alloc_optimum_mix                            1908 ns         1904 ns       352235
BM_vector_bpool_alloc_optimum_first_mix                     763 ns          762 ns       825022
BM_vector_bpool_alloc_optimum_last_mix                      710 ns          710 ns       831771
BM_vector_std_alloc_freestyle_mix                         94234 ns        93954 ns        16736
BM_vector_bpool_alloc_freestyle_first_mix                 30013 ns        30011 ns        23364
BM_vector_bpool_alloc_freestyle_last_mix                  27251 ns        27249 ns        25454
```
