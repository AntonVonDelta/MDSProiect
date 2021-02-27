#include "pch.h"

int test1(int a, int b) {
	return a + b;
}

void processConnections();
void meme();

TEST(TestCaseName, TestDummyFunction) {
  EXPECT_EQ(test1(10,12),10+12);
  EXPECT_TRUE(true);
  meme();
}

int main() { return 0; }