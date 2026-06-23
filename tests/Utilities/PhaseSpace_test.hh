#ifndef PHASESPACE_TEST_HH
#define PHASESPACE_TEST_HH

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "ResBos/PhaseSpace.hh"
#include "Random_test.hh"

class MockPhaseSpace : public PhaseSpace {
    public:
        // Mock Phase Space generators
        MOCK_CONST_METHOD2(PhaseGen,double(const std::vector<double>&, std::vector<FourVector>&));
        MOCK_METHOD4(PhaseSpace3,double(const std::vector<double>&, double&,
                                        double&, std::vector<FourVector>&));
        MOCK_CONST_METHOD6(x2Phase,double(double*, const double&, const double&, const double&,
                                          FourVector&, FourVector&));
        MOCK_METHOD4(Wresph,double(const std::vector<double>&, const double&, std::vector<double>&,
                                   std::vector<FourVector>&));

        // Mock Setters
        MOCK_METHOD1(SetECM,void(const double&));
        MOCK_METHOD1(SetNPrimaryPart,void(const size_t&));
        MOCK_METHOD1(SetIMonte,void(const size_t&));
        MOCK_METHOD1(SetIQt,void(const size_t&));
        MOCK_METHOD1(SetMass,void(const std::vector<double>&));
        MOCK_METHOD2(SetQRange,void(const double&, const double&));
        MOCK_METHOD2(SetQtRange,void(const double&, const double&));
};

using ::testing::TestWithParam;

class PhaseSpaceTest : public ::testing::TestWithParam<size_t> {
    public:
        // Set-up work for each test
        virtual void SetUp() {
            // This gets deleted by the PhaseSpace object
            rand = new MockRandom(); 
        }

        // Clean-up work for each test
        virtual void TearDown() {}
    
    protected:
        std::vector<FourVector> particles; 
        MockRandom *rand;
};

#endif
