#ifndef BASEINTEGRATOR_TEST_HH
#define BASEINTEGRATOR_TEST_HH

#include "gtest/gtest.h"
#include "ResBos/OgataQuad.hh"

class OgataQuadTest : public testing::TestWithParam<double> {
    public:
        // Set-up work for each test
        virtual void SetUp() {
            ogata = new Utility::OgataQuad(); 
        }

        virtual void TearDown() { 
            delete ogata; 
        }

    protected:
        Utility::OgataQuad *ogata;
};

#endif
