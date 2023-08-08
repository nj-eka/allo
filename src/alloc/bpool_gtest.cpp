#include <gtest/gtest.h>

#include "memuse.hpp"
#include "bpool.hpp"

TEST(alloc_unit_tests, bpool_first){
    alloc::bpool<int, 4> bp(alloc::placement_policy::first);
    [[maybe_unused]] int *i = bp.allocate(1);
    EXPECT_STREQ(bp.repr().c_str(), "___*");
    [[maybe_unused]] int *j = bp.allocate(1);
    EXPECT_STREQ(bp.repr().c_str(), "__**");
    [[maybe_unused]] int *k = bp.allocate(1);
    EXPECT_STREQ(bp.repr().c_str(), "_***");
    bp.deallocate(i, 1);    
    EXPECT_STREQ(bp.repr().c_str(), "_**_");
    i = bp.allocate(1);
    EXPECT_STREQ(bp.repr().c_str(), "_***");
    bp.deallocate(j, 1);    
    EXPECT_STREQ(bp.repr().c_str(), "_*_*");
    j = bp.allocate(1);
    EXPECT_STREQ(bp.repr().c_str(), "_***");
}

TEST(alloc_unit_tests, bpool_last){
    alloc::bpool<int, 4> bp(alloc::placement_policy::last);
    [[maybe_unused]] int *i = bp.allocate(1);
    EXPECT_STREQ(bp.repr().c_str(), "___*");
    [[maybe_unused]] int *j = bp.allocate(1);
    EXPECT_STREQ(bp.repr().c_str(), "__**");
    bp.deallocate(i, 1);    
    EXPECT_STREQ(bp.repr().c_str(), "__*_");
    [[maybe_unused]] int *k = bp.allocate(1);
    EXPECT_STREQ(bp.repr().c_str(), "_**_");
    bp.allocate(1);    
    EXPECT_STREQ(bp.repr().c_str(), "***_");
    [[maybe_unused]] int *out = bp.allocate(1);
    EXPECT_STREQ(bp.repr().c_str(), "***_");
    EXPECT_TRUE(out == nullptr);
}

// int main(int argc, char **argv) {
//   ::testing::InitGoogleTest(&argc, argv);
//   return RUN_ALL_TESTS();
// }
