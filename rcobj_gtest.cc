// -*- mode: C++; tab-width: 4; coding: utf-8 -*-

#include "rcobj.h"

#include <gtest/gtest.h>

TEST(rcobj, default_obj) {
  ASSERT_EQ(true, rcobj().is_error());

  EXPECT_STREQ("(not initialized)", rcobj().str());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
