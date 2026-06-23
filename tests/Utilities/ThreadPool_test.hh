#ifndef THREADPOOL_TEST_HH
#define THREADPOOL_TEST_HH

#include "gmock/gmock.h"
#include "ResBos/ThreadPool.hh"

class ThreadPoolTest : public ::testing::Test {
    protected:
        // Set-up work for each test
        virtual void SetUp() {}

        // Clean-up work for each test
        virtual void TearDown() {}
};

#endif

