// Simple MATLAB-type timer

#ifndef _TIMER_H
#define _TIMER_H
#include <chrono>

class Timer {
public:
    time_t tic();
    time_t toc();
    double nsec() const;
private:
    std::chrono::time_point<std::chrono::system_clock> tStart, tEnd;
};


inline time_t Timer::tic() {
    tStart = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(tStart);
}

inline time_t Timer::toc() {
    tEnd = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(tEnd);
}

inline double Timer::nsec() const {
    std::chrono::duration<double> elapsed_seconds = tEnd - tStart;
    return elapsed_seconds.count();
}

#endif
