#ifndef THREEVECTOR_TEST_HH
#define THREEVECTOR_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/ThreeVector.hh"

class MockThreeVector : public ThreeVector {
    public:
        // Mock Getter functions
        MOCK_CONST_METHOD0(Px,double());
        MOCK_CONST_METHOD0(Py,double());
        MOCK_CONST_METHOD0(Pz,double());

        // Mock Setter functions
        MOCK_METHOD3(SetPxPyPz, void(const double&, const double&, const double&));
        MOCK_METHOD1(SetPxPyPz, void(const std::vector<double>&));
        MOCK_METHOD1(SetPxPyPz, void(const std::array<double,3>&));
        MOCK_METHOD1(SetPx, void(const double&));
        MOCK_METHOD1(SetPy, void(const double&));
        MOCK_METHOD1(SetPz, void(const double&));

        // Mock Functions
        MOCK_CONST_METHOD1(Dot, double(const ThreeVector&));
};

class ThreeVectorTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            p1.SetPxPyPz(1,2,3);
            p2.SetPxPyPz(-1,-2,-3);
            p0.SetPxPyPz(0,0,0);
            px.SetPxPyPz(1,0,0);
            py.SetPxPyPz(0,1,0);
            pz.SetPxPyPz(0,0,1);
        }

        // Clean-up work for each test
        virtual void TearDown() {}

        ThreeVector p1,p2,px,py,pz,p0;
};

#endif
