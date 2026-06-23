#ifndef CONVOLUTION_TEST_HH
#define CONVOLUTION_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/Convolution.hh"
#include "ResBos/Enums.hh"
#include "../ResBos_test.hh"
#include "Pdf_test.hh"
#include "HoppetInterface_test.hh"

class MockConvolution : public Beam::Convolution {
    public:
        MOCK_METHOD4(GetConv,double(const int&, const double&, const double&, const Conv&));
        MOCK_METHOD3(GetConv,std::map<int,double>(const double&, const double&, const Conv&));
        MOCK_CONST_METHOD1(GetGrid,Beam::ConvGrid(const Conv&));
};

using ::testing::TestWithParam;

class ConvolutionTest : public ::testing::TestWithParam<double> {
    protected:
        // Set-up work for each test
        virtual void SetUp() {}

        // Clean-up work for each test
        virtual void TearDown() {}
};

#endif


