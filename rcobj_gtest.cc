// -*- mode: C++; tab-width: 4; coding: utf-8 -*-

#include <vector>
#include <gtest/gtest.h>

#include "rcobj.h"

TEST(rcobj, default_obj) {
  ASSERT_EQ(true, rcobj().is_error());

  EXPECT_STREQ("(not initialized)", rcobj().str());
}

TEST(rcobj, return_code_ok_done) {
  
  rcobj rc = [](){RET_OK_DONE;}();

  ASSERT_EQ(true,  !!rc);

  ASSERT_EQ(true,  rc.no_error());
  ASSERT_EQ(true,  rc.is_ok());
  ASSERT_EQ(true,  rc.has_results());
  ASSERT_EQ(true,  rc.is_done());
  ASSERT_EQ(true,  rc.is_finished());

  ASSERT_EQ(false, rc.is_error());
  ASSERT_EQ(false, rc.is_ng());
  ASSERT_EQ(false, rc.has_no_results());
  ASSERT_EQ(false, rc.is_blocked());
  ASSERT_EQ(false, rc.is_not_finished());
  
  ASSERT_EQ(0, rc.str());
  ASSERT_STREQ("OK_DONE", rc.code_str());
  
  {
    const char* msg = "uninitialized";
    ASSERT_EQ(false, rc.has_error(&msg));
    ASSERT_EQ(0, msg);
  }
}

TEST(rcobj, return_code_ok_cont) {
  
  std::vector<rcobj> rcvec;
  
  rcvec.push_back([](){RET_OK_CONT;}());
  rcvec.push_back([](){RET_OK_CONTINUE;}());
  
  for (auto rc : rcvec) {
    ASSERT_EQ(true,  !!rc);

    ASSERT_EQ(true,  rc.no_error());
    ASSERT_EQ(true,  rc.is_ok());
    ASSERT_EQ(true,  rc.has_results());
    ASSERT_EQ(false,  rc.is_done());
    ASSERT_EQ(false,  rc.is_finished());

    ASSERT_EQ(false, rc.is_error());
    ASSERT_EQ(false, rc.is_ng());
    ASSERT_EQ(false, rc.has_no_results());
    ASSERT_EQ(true, rc.is_blocked());
    ASSERT_EQ(true, rc.is_not_finished());
  
    ASSERT_EQ(0, rc.str());
    ASSERT_STREQ("OK_CONT", rc.code_str());
  
    {
      const char* msg = "uninitialized";
      ASSERT_EQ(false, rc.has_error(&msg));
      ASSERT_EQ(0, msg);
    }
  }
}

TEST(rcobj, return_code_ng_blocked) {
  
  std::vector<rcobj> rcvec;
  
  rcvec.push_back([](){RET_NG_CONT;}());
  rcvec.push_back([](){RET_NG_RETRY;}());
  rcvec.push_back([](){RET_NG_BLOCKED;}());
  
  for (auto rc : rcvec) {
    ASSERT_EQ(false,  !!rc);

    ASSERT_EQ(true,  rc.no_error());
    ASSERT_EQ(false,  rc.is_ok());
    ASSERT_EQ(false,  rc.has_results());
    ASSERT_EQ(false,  rc.is_done());
    ASSERT_EQ(false,  rc.is_finished());

    ASSERT_EQ(false, rc.is_error());
    ASSERT_EQ(true, rc.is_ng());
    ASSERT_EQ(true, rc.has_no_results());
    ASSERT_EQ(true, rc.is_blocked());
    ASSERT_EQ(true, rc.is_not_finished());
  
    ASSERT_EQ(0, rc.str());
    ASSERT_STREQ("NG_BLOCKED", rc.code_str());
  
    {
      const char* msg = "uninitialized";
      ASSERT_EQ(false, rc.has_error(&msg));
      ASSERT_EQ(0, msg);
    }
  }
}

TEST(rcobj, return_code_ng_eof) {
  
  std::vector<rcobj> rcvec;
  
  rcvec.push_back([](){RET_NG_DONE;}());
  rcvec.push_back([](){RET_EOF;}());
  
  for (auto rc : rcvec) {
    ASSERT_EQ(false,  !!rc);

    ASSERT_EQ(true,  rc.no_error());
    ASSERT_EQ(false,  rc.is_ok());
    ASSERT_EQ(false,  rc.has_results());
    ASSERT_EQ(true,  rc.is_done());
    ASSERT_EQ(true,  rc.is_finished());

    ASSERT_EQ(false, rc.is_error());
    ASSERT_EQ(true, rc.is_ng());
    ASSERT_EQ(true, rc.has_no_results());
    ASSERT_EQ(false, rc.is_blocked());
    ASSERT_EQ(false, rc.is_not_finished());
  
    ASSERT_EQ(0, rc.str());
    ASSERT_STREQ("NG_EOF", rc.code_str());
  
    {
      const char* msg = "uninitialized";
      ASSERT_EQ(false, rc.has_error(&msg));
      ASSERT_EQ(0, msg);
    }
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
