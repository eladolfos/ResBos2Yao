#include "ThreadPool_test.hh"
#include "gtest/gtest.h"

TEST_F(ThreadPoolTest,TestAll) {
    const int nThreads = 8;
    ThreadPool poolTest(nThreads);
    poolTest.GetIDs();

    for(size_t i = 0; i < nThreads; ++i) {
        for(size_t j = 0; j < nThreads; ++j) {
            if(i == j) continue;
            EXPECT_NE(poolTest.GetID(i),poolTest.GetID(j));
        }
    }

    std::vector<std::shared_future<void>> futures;
    for(size_t i = 0; i < 64; ++i) {
        futures.emplace_back(poolTest.enqueue([i] {
            for(size_t j = 0; j < i; ++j) {
            }
        }));
    }
    WaitAll(futures);
}
