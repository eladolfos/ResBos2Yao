#include "FourVector_test.hh"
#include "gtest/gtest.h"
#include <sstream>

TEST_F(FourVectorTest, Constructors) {
    std::vector<double> pVec = {0,0,6,6};
    FourVector pTest(pVec);
    EXPECT_EQ(pTest,p1);

    std::array<double,4> pArray = {0,0,6,6};
    FourVector pTest2(pArray);
    EXPECT_EQ(pTest2,p1);

    FourVector pTest3(p1);
    EXPECT_EQ(pTest3,p1);
}

TEST_F(FourVectorTest, Rapidity) {
    EXPECT_NEAR(fabs(p3.Rapidity()),fabs(p4.Rapidity()),1E-8);
}

TEST_F(FourVectorTest, Setters) {
    FourVector pTest(0,0,0,0);
    std::vector<double> p = {p1[0],p1[1],p1[2],p1[3]};
    pTest.SetPxPyPzE(p);
    EXPECT_EQ(pTest,p1);

    std::array<double,4> pArray = {p1[0],p1[1],p1[2],p1[3]};
    pTest.SetPxPyPzE(pArray);
    EXPECT_EQ(pTest,p1);

    ThreeVector pVec = p1.GetVect();
    pTest.SetVectM(pVec,0);
    EXPECT_EQ(pTest,p1);
}

TEST_F(FourVectorTest, Boosts) {
    FourVector pTmp = p1;
    pTmp.Boost(p3.BoostVector());
    pTmp.Boost(-p3.BoostVector());
    EXPECT_LT(fabs(pTmp[0]-p1[0]),1E-13);
    EXPECT_LT(fabs(pTmp[1]-p1[1]),1E-13);
    EXPECT_LT(fabs(pTmp[2]-p1[2]),1E-13);
    EXPECT_LT(fabs(pTmp[3]-p1[3]),1E-13);
}

TEST_F(FourVectorTest, Operators) {
    EXPECT_EQ(p1+p2,p3+p4);
    EXPECT_EQ(p1-p3,p4-p2);
    EXPECT_EQ(p1*p1,0);
    EXPECT_EQ(0.5*p1,p1/2.0);
    EXPECT_EQ(-(-p1),p1);
    EXPECT_EQ(p1.Dot(p1),p1*p1);
    EXPECT_EQ(+p1,p1);
}

TEST_F(FourVectorTest, Access) {
    EXPECT_EQ(p1.Px(),p1[0]);
    EXPECT_EQ(p1.Px(),p1(0));
    EXPECT_EQ(p1.Py(),p1[1]);
    EXPECT_EQ(p1.Py(),p1(1));
    EXPECT_EQ(p1.Pz(),p1[2]);
    EXPECT_EQ(p1.Pz(),p1(2));
    EXPECT_EQ(p1.E(),p1[3]);
    EXPECT_EQ(p1.E(),p1(3));
    EXPECT_EQ(sqrt(p1.M2()),p1.M());

    EXPECT_EQ(p3.Pt2(),p4.Pt2());
    EXPECT_EQ(p3.Pt(),p4.Pt());

    ThreeVector v(0,0,6);
    EXPECT_EQ(p1.GetVect(),v);
}

TEST_F(FourVectorTest, Stream) {
    std::stringstream ss;
    ss << p1;
    FourVector p;
    ss >> p;
    EXPECT_EQ(p1,p);
}
