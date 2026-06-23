#ifndef PROGRESS_BAR_HH
#define PROGRESS_BAR_HH

#include <chrono>

class ProgressBar {
    public:
        ProgressBar() = delete;
        ProgressBar(size_t);
        virtual ~ProgressBar() {};
        void Update(size_t);
        void Display();
        void Done();

    private:
        static constexpr double width = 50.0;
        size_t nTasks, nTasksRemaining, nTasksComplete;
        double GetETA();
        double timeLeft, lastTimeLeft;
        std::chrono::system_clock::time_point start, current;
};

#endif
