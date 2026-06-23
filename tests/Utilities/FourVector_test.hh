#ifndef FOURVECTOR_TEST_HH
#define FOURVECTOR_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/FourVector.hh"
#include "ResBos/ThreeVector.hh"

class MockFourVector : public FourVector {
    public:
        // Mock Getter functions
        MOCK_CONST_METHOD0(Px,double());
        MOCK_CONST_METHOD0(Py,double());
        MOCK_CONST_METHOD0(Pz,double());
        MOCK_CONST_METHOD0(E,double());
        MOCK_CONST_METHOD0(M2,double());
        MOCK_CONST_METHOD0(M,double());
        MOCK_CONST_METHOD0(Pt2,double());
        MOCK_CONST_METHOD0(Pt,double());
        MOCK_CONST_METHOD0(Rapidity,double());
        MOCK_CONST_METHOD0(GetVect,ThreeVector());

        // Mock Setter functions
        MOCK_METHOD4(SetPxPyPzE,void(const double&, const double&, const double&, const double&));
        MOCK_METHOD1(SetPxPyPzE,void(const std::vector<double>&));
        MOCK_METHOD1(SetPxPyPzE,void(const std::array<double,4>&));
        MOCK_METHOD1(SetPx,void(const double&));
        MOCK_METHOD1(SetPy,void(const double&));
        MOCK_METHOD1(SetPz,void(const double&));
        MOCK_METHOD1(SetE,void(const double&));
        MOCK_METHOD2(SetVectM,void(const ThreeVector&, const double&));

        // Mock Functions
        MOCK_METHOD1(Boost,void(const ThreeVector&));
        MOCK_METHOD3(Boost,void(const double&, const double&, const double&));
        MOCK_CONST_METHOD0(BoostVector,ThreeVector());
        MOCK_CONST_METHOD1(Dot,double(const FourVector&));
};

class FourVectorTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            p1.SetPxPyPzE(0,0,6,6);
            p2.SetPxPyPzE(0,0,-6,6);
            p3.SetPxPyPzE(1,2,3,6);
            p4.SetPxPyPzE(-1,-2,-3,6);
        }

        // Clean-up work for each test
        virtual void TearDown() {}

        FourVector p1, p2, p3, p4;
};

#endif
