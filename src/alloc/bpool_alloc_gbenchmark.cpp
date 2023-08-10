#include <benchmark/benchmark.h>
// #include <iostream>
#include "bpool_alloc.hpp"

/////////////////////////////////////////////////////////////////////////
constexpr static size_t optimal_max_count = alloc::OPTIMAL_BPOOL_SEGMENT_ELEMENTS_COUNT;

static void BM_vector_std_alloc_optimum_linear_monotonic(benchmark::State& state)
{
    std::vector<int*> vpi;
    vpi.reserve(optimal_max_count);
    for(auto _: state)
    {
        auto alloc = std::allocator<int>();
        for (size_t i = 0; i < optimal_max_count; i++)
            vpi.push_back(alloc.allocate(1));
        for (auto pi: vpi)
        {
            alloc.deallocate(pi, 1);
        }
        vpi.clear();
    }
}
BENCHMARK(BM_vector_std_alloc_optimum_linear_monotonic);

static void BM_vector_bpool_alloc_optimum_first_linear_monotonic(benchmark::State& state)
{
    std::vector<int*> vpi;
    vpi.reserve(optimal_max_count);
    for(auto _: state)
    {
        auto alloc = alloc::bpool_alloc<int, optimal_max_count>(alloc::placement_policy::first);
        for (size_t i = 0; i < optimal_max_count; i++)
            vpi.push_back(alloc.allocate(1));
        for (auto pi: vpi)
        {
            alloc.deallocate(pi, 1);
        }
        // std::cout << alloc.get_bpools_size() << "\n";
        vpi.clear();
    }
}
BENCHMARK(BM_vector_bpool_alloc_optimum_first_linear_monotonic);

static void BM_vector_bpool_alloc_optimum_last_linear_monotonic(benchmark::State& state)
{
    std::vector<int*> vpi;
    vpi.reserve(optimal_max_count);
    for(auto _: state)
    {
        auto alloc = alloc::bpool_alloc<int, optimal_max_count>(alloc::placement_policy::last);
        for (size_t i = 0; i < optimal_max_count; i++)
            vpi.push_back(alloc.allocate(1));
        for (auto pi: vpi)
        {
            alloc.deallocate(pi, 1);
        }
        // std::cout << alloc.get_bpools_size() << "\n";        
        vpi.clear();
    }
}
BENCHMARK(BM_vector_bpool_alloc_optimum_last_linear_monotonic);

///////////////////////////////////////////////////////////
static void BM_vector_std_alloc_optimum_mix(benchmark::State& state)
{
    std::vector<int*> vpi;
    vpi.reserve(optimal_max_count);
    for(auto _: state)
    {
        auto alloc = std::allocator<int>();
        for (size_t i = 0; i < optimal_max_count / 2; i++)
            vpi.push_back(alloc.allocate(1));
        for (size_t i = 0; i < optimal_max_count / 2; i += 2)
            alloc.deallocate(vpi[i], 1);
        for (size_t i = optimal_max_count / 2 + 1; i < optimal_max_count; i++)
            vpi.push_back(alloc.allocate(1));
        vpi.clear();
    }
}
BENCHMARK(BM_vector_std_alloc_optimum_mix);

static void BM_vector_bpool_alloc_optimum_first_mix(benchmark::State& state)
{
    std::vector<int*> vpi;
    vpi.reserve(optimal_max_count);
    for(auto _: state)
    {
        auto alloc = alloc::bpool_alloc<int, optimal_max_count>(alloc::placement_policy::first);
        for (size_t i = 0; i < optimal_max_count / 2; i++)
            vpi.push_back(alloc.allocate(1));
        for (size_t i = 0; i < optimal_max_count / 2; i += 2)
            alloc.deallocate(vpi[i], 1);
        for (size_t i = optimal_max_count / 2 + 1; i < optimal_max_count; i++)
            vpi.push_back(alloc.allocate(1));
        vpi.clear();
    }
}
BENCHMARK(BM_vector_bpool_alloc_optimum_first_mix);

static void BM_vector_bpool_alloc_optimum_last_mix(benchmark::State& state)
{
    std::vector<int*> vpi;
    vpi.reserve(optimal_max_count);
    for(auto _: state)
    {
        auto alloc = alloc::bpool_alloc<int, optimal_max_count>(alloc::placement_policy::last);
        for (size_t i = 0; i < optimal_max_count / 2; i++)
            vpi.push_back(alloc.allocate(1));
        for (size_t i = 0; i < optimal_max_count / 2; i += 2)
            alloc.deallocate(vpi[i], 1);
        for (size_t i = optimal_max_count / 2 + 1; i < optimal_max_count; i++)
            vpi.push_back(alloc.allocate(1));
        vpi.clear();
    }
}
BENCHMARK(BM_vector_bpool_alloc_optimum_last_mix);

///////////////////////////////////////////////////////////
constexpr static size_t freestyle_max_count = 1024;

static void BM_vector_std_alloc_freestyle_mix(benchmark::State& state)
{
    std::vector<int*> vpi;
    for(auto _: state)
    {
        auto alloc = std::allocator<int>();
        for (size_t i = 0; i < freestyle_max_count / 2; i++)
            vpi.push_back(alloc.allocate(1));
        for (size_t i = 0; i < freestyle_max_count / 2; i += 2)
            alloc.deallocate(vpi[i], 1);
        for (size_t i = freestyle_max_count / 2 + 1; i < freestyle_max_count; i++)
            vpi.push_back(alloc.allocate(1));
        vpi.clear();
    }
}
BENCHMARK(BM_vector_std_alloc_freestyle_mix);

static void BM_vector_bpool_alloc_freestyle_first_mix(benchmark::State& state)
{  
    std::vector<int*> vpi;
    for(auto _: state)
    {
        auto alloc = alloc::bpool_alloc<int>(alloc::placement_policy::first);
        for (size_t i = 0; i < freestyle_max_count / 2; i++)
            vpi.push_back(alloc.allocate(1));
        for (size_t i = 0; i < freestyle_max_count / 2; i += 2)
            alloc.deallocate(vpi[i], 1);
        for (size_t i = freestyle_max_count / 2 + 1; i < freestyle_max_count; i++)
            vpi.push_back(alloc.allocate(1));
        vpi.clear();
    }
}
BENCHMARK(BM_vector_bpool_alloc_freestyle_first_mix);

static void BM_vector_bpool_alloc_freestyle_last_mix(benchmark::State& state)
{
    std::vector<int*> vpi;
    for(auto _: state)
    {
        auto alloc = alloc::bpool_alloc<int>(alloc::placement_policy::last);
        for (size_t i = 0; i < freestyle_max_count / 2; i++)
            vpi.push_back(alloc.allocate(1));
        for (size_t i = 0; i < freestyle_max_count / 2; i += 2)
            alloc.deallocate(vpi[i], 1);
        for (size_t i = freestyle_max_count / 2 + 1; i < freestyle_max_count; i++)
            vpi.push_back(alloc.allocate(1));
        vpi.clear();
    }
}
BENCHMARK(BM_vector_bpool_alloc_freestyle_last_mix);

BENCHMARK_MAIN();

/*
cmake -B ./build -S . 
    -DENABLE_BENCHMARK=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_LOGGING=OFF \
    -DBUILD_TESTING=OFF \
    -DENABLE_COVERAGE=OFF \
    -DCMAKE_CXX_FLAGS="-g -O2"

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
*/