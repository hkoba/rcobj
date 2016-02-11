// -*- mode: C++; tab-width: 4; coding: utf-8 -*-

#include <vector>
#include <gtest/gtest.h>

#include "rcobj.h"

TEST(rcobj, return_unless) {// ================ Here is line 8. ==============
  {
    rcobj rc = [](){RET_UNLESS(true == false);}(); // line 10.
	const char* amsg = "Prereq 'true == false' failed in rcobj_errors_gtest.cc:10";
	
    ASSERT_EQ(true, rc.is_error());
    ASSERT_STREQ(amsg, rc.str());
	
	{
	  const char* msg = "uninitialized";
	  ASSERT_EQ(true, rc.has_error(&msg));
	  ASSERT_STREQ(amsg, msg);
	}

	EXPECT_EQ(false,  !!rc);

	EXPECT_EQ(false,  rc.no_error());
	EXPECT_EQ(false,  rc.is_ok());
	EXPECT_EQ(false,  rc.has_results());
	EXPECT_EQ(true,   rc.is_done()); // Has error means no remaining job. confusing?
	EXPECT_EQ(true,   rc.is_finished());

	EXPECT_EQ(true, rc.is_error());
	EXPECT_EQ(true, rc.is_ng());
	EXPECT_EQ(true, rc.has_no_results());
	EXPECT_EQ(false, rc.is_blocked());
	EXPECT_EQ(false, rc.is_not_finished());
	
  }
}


TEST(rcobj, return_error) {// ======== Here is line 40. ========
  {
    rcobj rc = [](){RET_ERROR("Hand crafted error");}(); // line 42.
	const char* amsg = "Hand crafted error in rcobj_errors_gtest.cc:42";
	
    ASSERT_EQ(true, rc.is_error());
    ASSERT_STREQ(amsg, rc.str());
	
	{
	  const char* msg = "uninitialized";
	  ASSERT_EQ(true, rc.has_error(&msg));
	  ASSERT_STREQ(amsg, msg);
	}

	EXPECT_EQ(false,  !!rc);

	EXPECT_EQ(false,  rc.no_error());
	EXPECT_EQ(false,  rc.is_ok());
	EXPECT_EQ(false,  rc.has_results());
	EXPECT_EQ(true,   rc.is_done()); // Has error means no remaining job. confusing?
	EXPECT_EQ(true,   rc.is_finished());

	EXPECT_EQ(true, rc.is_error());
	EXPECT_EQ(true, rc.is_ng());
	EXPECT_EQ(true, rc.has_no_results());
	EXPECT_EQ(false, rc.is_blocked());
	EXPECT_EQ(false, rc.is_not_finished());
	
  }
}


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
