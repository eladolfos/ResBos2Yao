#include "gtest/gtest.h"
#include "Grid3D_test.hh"

TEST_F(Grid3DTest, FindersGetters) {
    for(int i = 1; i <= 10; i++)
        EXPECT_EQ(test -> GetX(static_cast<size_t>(std::distance(test -> FindX(1),test -> FindX(i)))),i);

    for(int i = 1; i <= 10; i++)
        EXPECT_EQ(test -> GetY(static_cast<size_t>(std::distance(test -> FindY(1),test -> FindY(i)))),i);

    for(int i = 1; i <= 10; i++)
        EXPECT_EQ(test -> GetZ(static_cast<size_t>(std::distance(test -> FindZ(1),test -> FindZ(i)))),i);

    for(size_t i = 0; i < 10; i++)
        for(size_t j = 0; j < 10; j++)
            for(size_t k = 0; k < 10; k++)
                EXPECT_EQ(test -> FindPoint(i,j,k),test -> FindPoint(test -> GetX(i), test -> GetY(j), test -> GetZ(k)));
}

TEST_F(Grid3DTest, Interpolate) {
    for(int i = 1; i < 20; i++) {
        for(int j = 1; j < 20; j++) {
            for(int k = 1; k < 20; k++) {
                double x_ = i/20.;
                double y_ = j/20.;
                double z_ = k/20.;
                EXPECT_LT(fabs(test -> Interpolate(x_,y_,z_) - TestFunc(x_,y_,z_)), 1E-12);
            }
        }
    }
}

