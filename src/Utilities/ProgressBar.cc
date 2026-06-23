#include <cmath>
#include <iomanip>
#include <iostream>

#include "ResBos/ProgressBar.hh"

ProgressBar::ProgressBar(size_t tasks): nTasks(tasks) {
    nTasksRemaining = nTasks;
    nTasksComplete = 0;
    start = std::chrono::system_clock::now();
    current = std::chrono::system_clock::now();
    timeLeft = 0;
    lastTimeLeft = 0;
}

void ProgressBar::Update(size_t completed) {
    // Update tasks
    nTasksRemaining -= completed;
    nTasksComplete += completed;
}

void ProgressBar::Display() {
    // Only update display every two seconds
    auto current_ = std::chrono::system_clock::now();
    auto dur = std::chrono::duration<double>(current_ - current);
    if(dur.count() < 1) return;
    current = current_;

    // Estimate remaining time
    timeLeft = static_cast<double>(nTasksRemaining) / static_cast<double>(nTasksComplete) 
             * std::chrono::duration<double>(current-start).count();

    // Apply smoothing if the time change isn't too great
    if(lastTimeLeft == 0) lastTimeLeft = std::numeric_limits<double>::infinity();
    if(timeLeft > lastTimeLeft / 2.0) {
        double diff = timeLeft - lastTimeLeft;
        timeLeft = lastTimeLeft + (diff < 0 ? 0.3 : 0.1) * diff;

        double diffPct = diff / lastTimeLeft * 100;
        if(std::abs(diff) < 5 || std::abs(diffPct) < 5) 
            timeLeft = lastTimeLeft - (diff < 0 ? 0.4 : 0.2);
    }

    // Calculate which units to use for the time
    std::string unit1, unit2;
    int time1, time2;
    if(timeLeft > 60*60) {
        unit1 = " hr";
        time1 = static_cast<int>(timeLeft / 3600);
        unit2 = "min";
        time2 = static_cast<int>((timeLeft - 3600*time1) / 60);
    } else if(timeLeft > 60) {
        unit1 = "min";
        time1 = static_cast<int>(timeLeft / 60);
        unit2 = "sec";
        time2 = static_cast<int>(timeLeft - 60*time1);
    } else {
        unit1 = "sec";
        time1 = static_cast<int>(timeLeft);
        unit2 = "";
        time2 = 0;
    }

    // Display progress bar
    double progress = static_cast<double>(nTasksComplete) / static_cast<double>(nTasks);
    int loc = static_cast<int>(width * progress);
    std::cout << "[";
    for(int i = 0; i < width; ++i) {
        if(i < loc) std::cout << "=";
        else if(i == loc) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << static_cast<int>(progress*100);
    std::cout << "% ETA: " << time1 << " " << unit1 << " ";
    if(unit2 != "") std::cout << time2 << " " << unit2;
    else std::cout << std::setw(20) << " ";
    std::cout << std::setw(10) << " \r";
    std::cout.flush();
    
    // Update lastTimeLeft
    lastTimeLeft = timeLeft;
}

void ProgressBar::Done() {
    std::cout << "[";
    for(int i = 0; i < width; ++i) {
        std::cout << "=";
    }
    std::cout << "] " << 100;
    std::cout << "%" << std::setw(50) << " \n";
    std::cout.flush();
}
