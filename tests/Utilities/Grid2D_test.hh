#ifndef GRID2D_TEST_HH
#define GRID2D_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/Grid2D.hh"

double TestFunc(const double& x, const double& y) {
    const int n = 10;
    return pow(x-n/2.0,2)+pow(y-n/2.0,2);
}

namespace Utility {
class MockGrid2D : public Grid2D {
    public:
        // Mock Add point
        MOCK_METHOD3(AddPoint,void(const size_t&, const size_t&, const double&));
        MOCK_METHOD3(AddPoint,void(const double&, const double&, const double&));
       
        // Mock Finders
        MOCK_CONST_METHOD1(FindX,std::vector<double>::const_iterator(const double&));
        MOCK_CONST_METHOD1(FindY,std::vector<double>::const_iterator(const double&));
        MOCK_CONST_METHOD2(FindPoint, double(const size_t&, const size_t&));
        MOCK_CONST_METHOD2(FindPoint, double(const double&, const double&));
        MOCK_CONST_METHOD2(FindPointLocation, size_t(const double&, const double&));

        // Mock Getters
        MOCK_CONST_METHOD1(GetX, double(const size_t&));
        MOCK_CONST_METHOD1(GetY, double(const size_t&));
        MOCK_CONST_METHOD0(GetXDim, size_t());
        MOCK_CONST_METHOD0(GetYDim, size_t());

        // Mock Interpolation
        MOCK_METHOD4(Interpolate, double(const double&, const double&,
                                         const size_t&, const size_t&));
};
}

class Grid2DTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            test = new Utility::Grid2D(x,y); 
            for(auto xVal : x) {
                for(auto yVal : y) {
                    test -> AddPoint(xVal,yVal,TestFunc(xVal,yVal));
                }
            }
        }

        // Clean-up work for each test
        virtual void TearDown() {
            delete test; 
        }

        const std::vector<double> x{1,2,3,4,5,6,7,8,9,10};
        const std::vector<double> y{1,2,3,4,5,6,7,8,9,10};
        Utility::Grid2D *test;
};

#endif
