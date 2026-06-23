#ifndef HOPPETINTERFACE_TEST_HH
#define HOPPETINTERFACE_TEST_HH

#include "gmock/gmock.h"
#include "../Utilities/Settings_test.hh"
#include "ResBos/HoppetInterface.hh"
#include "ResBos/Enums.hh"

using ::testing::Return;
using ::testing::_;

class MockHoppet : public Utility::Hoppet {
    public:
        MOCK_METHOD6(GetConvolution,double(const int&, const double&, const double&,
                                           const int&, const int&, const int&));
        MOCK_METHOD4(GetConvolution,double(const int&, const double&, const double&,
                                           const Splitting&));
        MOCK_METHOD5(GetConvolution,std::map<int,double>(const double&, const double&,
                                                         const int&, const int&, const int&));
};

class HoppetTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {
            LHAPDF::PDF* pdf = LHAPDF::mkPDF("CT14nnlo",0);
            hoppet = new Utility::Hoppet(pdf,1.3,4.75,172);
        }

        // Clean-up work for each test
        virtual void TearDown() {
            delete hoppet; 
        }

        Utility::Hoppet* hoppet;
};

#endif


