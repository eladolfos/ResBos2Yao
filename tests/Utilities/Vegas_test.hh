#ifndef VEGAS_TEST_HH
#define VEGAS_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/Vegas.hh"

class VegasTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {}

        // Clean-up work for each test
        virtual void TearDown() {}
};

#endif
