#pragma once

#include <cstdint>
#include <chrono>
#include <iostream>
#include <iomanip>

class PerfTimer
{
public:
    PerfTimer() : m_startTime(takeTimeStamp()) {}
    void restart() {m_startTime = takeTimeStamp();}
    double elapsedSec() const {
        return double(takeTimeStamp() - m_startTime) * 1e-9;
    }
    double elapsedMSec() const {
        return double(takeTimeStamp() - m_startTime) * 1e-6;
    }

    void printElapsedMSec(const char *message) const {
        std::cout << std::fixed << std::setprecision(2);
        std::cout << message << elapsedMSec() << " ms" << std::endl;
    }

    static std::uint64_t takeTimeStamp() {
//        return std::uint64_t(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        // On mingw steady clock seems higher resolution than high res clock.
        return std::uint64_t(std::chrono::steady_clock::now().time_since_epoch().count());
    }
    std::uint64_t m_startTime;
};