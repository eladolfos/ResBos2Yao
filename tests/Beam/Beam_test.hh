#ifndef BEAM_TEST_HH
#define BEAM_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/Beam.hh"

using ::testing::Return;
using ::testing::_;

class MockBeam : public Beam::Beam {
    public:
        // Mock Getters
        MOCK_CONST_METHOD0(GetBeamType,::Beam::BeamType());
        MOCK_CONST_METHOD0(Energy,double());
        MOCK_CONST_METHOD0(GetQMin,double());

        // Mock Setters
        MOCK_METHOD1(SetBeamEnergy,void(const double&));
        MOCK_METHOD1(SetBeamType,void(const ::Beam::BeamType&));
        MOCK_METHOD1(SetBeamMomentum,void(const FourVector&));
        MOCK_METHOD1(SetPDF,void(Utility::PDF*));
        MOCK_METHOD1(SetHoppet,void(Utility::Hoppet*));
        MOCK_METHOD1(SetConvolution,void(::Beam::Convolution*));

        // Mock distribution function
        MOCK_CONST_METHOD4(PDF,double(const ::Beam::BDF, const int, const double, const double));
        MOCK_CONST_METHOD4(BeamMap,::Beam::BDFMap(const size_t&, const int&, 
                                                  const double&, const double&));
};

class BeamTest : public ::testing::TestWithParam<Beam::BeamType> {
    protected:
        // Set-up work for each test
        virtual void SetUp() {}

        // Clean-up work for each test
        virtual void TearDown() {}
};

#endif


