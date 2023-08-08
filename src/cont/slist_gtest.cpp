#include <iostream>
#include <sstream>
#include <numeric>
#include <gtest/gtest.h>

#include "alloc/bpool_alloc.hpp"
#include "slist.hpp"

using namespace cont;

TEST(cont_unit_tests, slist_push) {
  slist<int> sl;
  sl.push_back(10);
  sl.push_back(20);
  sl.push_front(30);
  sl.push_front(40);
  std::stringstream ss;
  std::copy(sl.begin(), sl.end(), std::ostream_iterator<int>(ss, "-"));
  EXPECT_EQ(ss.str(), "40-30-10-20-");
}

TEST(cont_unit_tests, slist_ctor_T) {
  slist<int> sl(10);
  std::iota(sl.begin(), sl.end(), 0);
  std::stringstream ss;
  std::copy(sl.begin(), sl.end(), std::ostream_iterator<int>(ss, "-"));
  EXPECT_EQ(ss.str(), "0-1-2-3-4-5-6-7-8-9-");
}

TEST(cont_unit_tests, slist_bpool_push) {
  // std::set_terminate(&my_terminate_handler);
  slist<int, alloc::bpool_alloc<int, 10>> sl;
  sl.push_back(10);
  sl.push_back(20);
  sl.push_front(30);
  sl.push_front(40);
  std::stringstream ss;
  std::copy(sl.begin(), sl.end(), std::ostream_iterator<int>(ss, "-"));
  EXPECT_EQ(ss.str(), "40-30-10-20-");
}

TEST(cont_unit_tests, slist_bpool_hw3_p1) {
  slist<int> sl(10);
  std::iota(sl.begin(), sl.end(), 0);
  std::stringstream ss;
  std::copy(sl.begin(), sl.end(), std::ostream_iterator<int>(ss, " "));
  EXPECT_EQ(ss.str(), "0 1 2 3 4 5 6 7 8 9 ");
}

TEST(cont_unit_tests, slist_bpool_hw3_p2) {
  slist<int, alloc::bpool_alloc<int, 10>> sl(10);
  std::iota(sl.begin(), sl.end(), 0);
  std::stringstream ss;
  std::copy(sl.begin(), sl.end(), std::ostream_iterator<int>(ss, " "));
  EXPECT_EQ(ss.str(), "0 1 2 3 4 5 6 7 8 9 ");
}
