#ifndef BASEINTEGRATOR_TEST_HH
#define BASEINTEGRATOR_TEST_HH

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "ResBos/BaseIntegrator.hh"

class MockBaseIntegrator : public Utility::BaseIntegrator {
    public:
        // Mock Base Integrator
        MOCK_METHOD4(Integrate, double(const double&, const double&, double&, double&)); 
        MOCK_METHOD4(IntegrateVec, std::vector<double>(const double&, const double&,
                                                       double&, double&)); 
       
};

class IntegratorTest : public testing::TestWithParam<std::tuple<Utility::BaseIntegrator*, bool>> {
    public:
        // Set-up work for each test
        virtual void SetUp() {
            integrator = std::get<0>(GetParam());
            mode = std::get<1>(GetParam()); 
        }

        virtual void TearDown() {
            delete integrator;
        }

    protected:
        Utility::BaseIntegrator* integrator;
        bool mode;
};

#endif
