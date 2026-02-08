#ifndef PERFORMANCE_TIMER_H
#define PERFORMANCE_TIMER_H

#include <chrono>
#include <string>
#include <map>
#include <iostream>

/**
 * @brief High-resolution timing for performance benchmarking
 * 
 * Provides microsecond-precision timing measurements for different
 * code regions, enabling detailed performance analysis.
 */
class PerformanceTimer {
public:
    PerformanceTimer();

    // Timing controls
    void start(const std::string& label = "default");
    void stop(const std::string& label = "default");
    void reset(const std::string& label = "default");
    void resetAll();

    // Results
    double elapsed(const std::string& label = "default") const; // in seconds
    double elapsedMs(const std::string& label = "default") const; // in milliseconds
    
    // Statistics
    void printSummary() const;
    void printSummary(const std::string& label) const;

    // Convenience class for RAII-style timing
    class ScopedTimer {
    public:
        ScopedTimer(PerformanceTimer& timer, const std::string& label)
            : timer_(timer), label_(label) {
            timer_.start(label_);
        }
        
        ~ScopedTimer() {
            timer_.stop(label_);
        }

    private:
        PerformanceTimer& timer_;
        std::string label_;
    };

private:
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    
    struct TimingData {
        TimePoint startTime;
        double totalTime;
        bool isRunning;
        int callCount;
        
        TimingData() : totalTime(0.0), isRunning(false), callCount(0) {}
    };

    std::map<std::string, TimingData> timers_;
};

#endif // PERFORMANCE_TIMER_H
