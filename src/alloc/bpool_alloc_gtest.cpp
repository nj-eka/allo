#include <gtest/gtest.h>

#include "memuse.hpp"
#include "bpool_alloc.hpp"

using namespace alloc;

TEST(alloc_unit_tests, bpool_alloc_ctor){
    auto initial = memuse();    
    bpool_alloc<double, 4> ba(placement_policy::first);
    EXPECT_EQ(memuse(), initial);

    [[maybe_unused]] double* p1_1 = ba.allocate();
    [[maybe_unused]] double* p1_2 = ba.allocate();
    [[maybe_unused]] double* p1_3_2 = ba.allocate(2);
    EXPECT_EQ(memuse(), std::make_pair(initial.first+72, initial.second+2)); // 4 * sizeof(double) + bitset byte + list_node ptr + ...

    initial = memuse();
    [[maybe_unused]] double* p2_1 = ba.allocate();
    [[maybe_unused]] double* p2_2_3 = ba.allocate(5);
    EXPECT_EQ(ba.repr(), "__******\n****\n");
    EXPECT_EQ(memuse(), std::make_pair(initial.first+104, initial.second+2));    
    // memstat(std::cout);
    initial = memuse();

    ba.deallocate(p1_1);
    ba.allocate();
    EXPECT_EQ(ba.repr(), "_*******\n***_\n");

    ba.deallocate(p1_2);
    ba.deallocate(p1_3_2, 2);
    ba.allocate(4);
    EXPECT_EQ(ba.repr(), "_*******\n****\n");
    EXPECT_EQ(memuse(), initial);    
}


// TEST(alloc_unit_tests, bpool_alloc_2){
//     bpool_alloc<double, 10> ba(placement_policy::first);
// }