#include "gtest/gtest.h"
#include "Grid2D_test.hh"

TEST_F(Grid2DTest, FindersGetters) {
    for(int i = 1; i <= 10; i++)
        EXPECT_EQ(test -> GetX(static_cast<size_t>(std::distance(test -> FindX(1),test -> FindX(i)))),i);

    for(int i = 1; i <= 10; i++)
        EXPECT_EQ(test -> GetY(static_cast<size_t>(std::distance(test -> FindY(1),test -> FindY(i)))),i);

    for(size_t i = 0; i < 10; i++)
        for(size_t j = 0; j < 10; j++)
            EXPECT_EQ(test -> FindPoint(i,j),test -> FindPoint(test -> GetX(i), test -> GetY(j)));
}

TEST_F(Grid2DTest, Interpolate) {
    for(int i = 1; i < 100; i++) {
        for(int j = 1; j < 100; j++) {
            double x_ = i/100.;
            double y_ = j/100.;
            EXPECT_LT(fabs(test -> Interpolate(x_,y_) - TestFunc(x_,y_)),1E-13);
        }
    }
}
