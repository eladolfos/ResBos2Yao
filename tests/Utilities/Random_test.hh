#ifndef RANDOM_TEST_HH
#define RANDOM_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/Random.hh"

class MockRandom : public Random {
    public:
        MOCK_METHOD1(Seed,void(const size_t&));
        MOCK_METHOD1(Get,std::vector<double>(const size_t&));
        MOCK_METHOD2(Get,std::vector<std::vector<double>>(const size_t&, const size_t&));
        MOCK_METHOD0(Get,double());
};

class RandomTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {}

        // Clean-up work for each test
        virtual void TearDown() {}
};

#endif
