#include "FourVector_test.hh"
#include "ThreeVector_test.hh"
#include "gtest/gtest.h"
#include <sstream>

TEST_F(ThreeVectorTest, Constructors) {
    std::vector<double> pVec = {1,2,3};
    ThreeVector pTest(pVec);
    EXPECT_EQ(pTest,p1);

    std::array<double,3> pArray = {1,2,3};
    ThreeVector pTest2(pArray);
    EXPECT_EQ(pTest2,p1);

    ThreeVector pTest3(p1);
    EXPECT_EQ(pTest3, p1);
}

TEST_F(ThreeVectorTest, Setters) {
    ThreeVector pTest(0,0,0);
    std::vector<double> p = {p1[0],p1[1],p1[2]};
    pTest.SetPxPyPz(p);
    EXPECT_EQ(pTest,p1);

    std::array<double,3> pArray = {p1[0],p1[1],p1[2]};
    pTest.SetPxPyPz(pArray);
    EXPECT_EQ(pTest,p1);
}

TEST_F(ThreeVectorTest, Operators) {
    EXPECT_EQ(p1+p2,p0);
    EXPECT_EQ(p0-p1,p2);
    EXPECT_EQ(-p1,p2);
    EXPECT_EQ(+p1,p1);
    EXPECT_EQ(p1*p2,p1.Dot(p2));
    EXPECT_EQ(p1*p2,-14);
    EXPECT_EQ(0.5*p1,p1/2.0);
}

TEST_F(ThreeVectorTest, Access) {
    EXPECT_EQ(p1.Px(),p1[0]);
    EXPECT_EQ(p1.Px(),p1(0));
    EXPECT_EQ(p1.Py(),p1[1]);
    EXPECT_EQ(p1.Py(),p1(1));
    EXPECT_EQ(p1.Pz(),p1[2]);
    EXPECT_EQ(p1.Pz(),p1(2));
}

TEST_F(ThreeVectorTest, Stream) {
    std::stringstream ss;
    ss << p1;
    ThreeVector p;
    ss >> p;
    EXPECT_EQ(p1,p);
}
