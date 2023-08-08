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