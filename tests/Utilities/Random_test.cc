#include "Random_test.hh"
#include "gtest/gtest.h"

TEST_F(RandomTest, TestAll) {
    Random rand1;
    rand1.Seed(0);
    Random rand2(0);

    std::vector<double> randVec1 = rand1.Get(5), randVec2 = rand2.Get(5);
    EXPECT_EQ(randVec1,randVec2);

    double randNum1 = rand1.Get(), randNum2 = rand2.Get();
    EXPECT_EQ(randNum1, randNum2);
}

