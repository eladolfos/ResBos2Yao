#ifndef THREADPOOL_HH
#define THREADPOOL_HH

// ThreadPool Class from github.com/progschj/ThreadPool
// Modified to work with the ResBos2 code

#include <future>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>

class ThreadPool {
    public:
        ThreadPool(size_t);
        template<class T, class... Args>
            auto enqueue(T&& t, Args&&... args) -> std::future<typename std::result_of<T(Args...)>::type>;
        ~ThreadPool();
        std::thread::id GetID(const size_t &i) const {return ids[i];}
        void GetIDs();
        size_t GetThreadCount() const {return workers.size();}
    private:
        std::vector<std::thread> workers;
        std::vector<std::thread::id> ids;
        std::queue<std::function<void()>> tasks;
        std::mutex queueMutex;
        std::condition_variable condition;
        bool stop;
};

// Constructor launches threads to start running
inline ThreadPool::ThreadPool(size_t threads) : stop(false) {
    for(size_t i = 0; i < threads; i++)
        workers.emplace_back([this] {
                for(;;) {
                std::function<void()> task;
                {
                std::unique_lock<std::mutex> lock(this->queueMutex);
                this->condition.wait(lock,[this] { return this->stop || !this->tasks.empty(); });
                if(this->stop && this->tasks.empty())
                return;
                task = std::move(this->tasks.front());
                this->tasks.pop();
                }

                task();
                }
                });
}

// Add new task
template<class T, class... Args>
auto ThreadPool::enqueue(T&& t, Args&&... args) -> std::future<typename std::result_of<T(Args...)>::type> {
    using return_type = typename std::result_of<T(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>> (
            std::bind(std::forward<T>(t), std::forward<Args>(args)...));

    std::future<return_type> result = task -> get_future();
    {
        std::unique_lock<std::mutex> lock(queueMutex);

        // Don't allow enqueueing after stopping the pool
        if(stop)
            throw std::runtime_error("ThreadPool: Unable to add tasks to finished pool");

        tasks.emplace([task](){ (*task)();});
    }
    condition.notify_one();
    return result;
}

extern ThreadPool *pool;
const std::thread::id mainThread = std::this_thread::get_id();

void WaitAll(std::vector<std::shared_future<void>> futures);

#endif
