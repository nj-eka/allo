#include <gtest/gtest.h>
#include <info/version.h>

TEST(version_unit_test, version_str_len) { ASSERT_GT(info::info_project_version().length(), 0) << "version() is empty"; }

TEST(version_unit_test, info_str_len) {
  // if use const * -> error: ==20224==ERROR: AddressSanitizer: heap-use-after-free on address 0x614000000040 at pc
  // 0x7f6304066942 bp 0x7ffff1ded780 sp 0x7ffff1decf28
  ASSERT_GT(info::info_project_full().length(), 0) << "version() is empty";
}
