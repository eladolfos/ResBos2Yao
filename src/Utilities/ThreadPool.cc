#include "ResBos/ThreadPool.hh"
#include <iostream>
#include <algorithm>
#include <iomanip>

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }

    condition.notify_all();
    // Join the threads
    for(size_t i = 0; i < workers.size(); i++) 
        workers[i].join();
}

void ThreadPool::GetIDs() {
    for(size_t i = 0; i < workers.size(); i++) {
        ids.push_back(workers[i].get_id());
    }
}

// Wait and process futures in the order they complete
void WaitAll(std::vector<std::shared_future<void>> futures) {
    size_t cur = 0;
    size_t running = futures.size();
    size_t totalJobs = running;

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    while(running > 1) { //poll vector of futures for one that is ready
        std::shared_future<void> &f = futures[cur];

        auto status = f.wait_for(std::chrono::milliseconds(10));
        if(status == std::future_status::ready) {
            f.get();

            running--;
            futures.erase(futures.begin()+static_cast<long int>(cur));

            end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed = end-start;
            double timeRemaining = static_cast<double>(running)
                                 / static_cast<double>(totalJobs-running)*elapsed.count();
            double percentFinished = 100-static_cast<double>(running)/
                                         static_cast<double>(totalJobs)*100;
            std::cout << std::fixed << std::setprecision(1);
            std::cout << "\rEstimated time remaining: ";
            std::cout.width(6);
            std::cout << timeRemaining;
            std::cout << " seconds";
            std::cout << "  Percent complete: ";
            std::cout.width(6);
            std::cout << percentFinished << " %";
            std::cout << std::flush;
        }

        cur++; if(cur >= running) cur = 0;
    }

    futures[0].get(); //must wait for the last one
    std::cout << "\rFinished";
    std::cout.fill(' ');
    std::cout.width(100);
    std::cout << " " << std::endl;
}
