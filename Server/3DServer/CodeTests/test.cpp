#include "pch.h"
#include "../3DServer/Grafica.h"

int test1(int a, int b) {
	return a + b;
}

TEST(TestCaseName, TestDummyFunction) {
	EXPECT_EQ(test1(10, 12), 10 + 12);
	EXPECT_TRUE(true);

	Grafica test;
	cout<<(unsigned int)test.getBuffer();
}
