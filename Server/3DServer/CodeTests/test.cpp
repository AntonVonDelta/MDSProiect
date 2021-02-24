#include "pch.h"

int test1(int,int);

TEST(TestCaseName, TestDummyFunction) {
  EXPECT_EQ(test1(10,12),10+12);
  EXPECT_TRUE(true);
}