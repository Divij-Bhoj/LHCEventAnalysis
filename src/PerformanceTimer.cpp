#include "PerformanceTimer.h"
#include <iomanip>

PerformanceTimer::PerformanceTimer() {}

void PerformanceTimer::start(const std::string& label) {
    auto& data = timers_[label];
    if (data.isRunning) {
        std::cerr << "Warning: Timer '" << label << "' already running" << std::endl;
        return;
    }
    
    data.startTime = std::chrono::high_resolution_clock::now();
    data.isRunning = true;
    data.callCount++;
}

void PerformanceTimer::stop(const std::string& label) {
    auto now = std::chrono::high_resolution_clock::now();
    
    auto it = timers_.find(label);
    if (it == timers_.end() || !it->second.isRunning) {
        std::cerr << "Warning: Timer '" << label << "' not running" << std::endl;
        return;
    }
    
    auto& data = it->second;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        now - data.startTime
    );
    
    data.totalTime += duration.count() / 1e6; // Convert to seconds
    data.isRunning = false;
}

void PerformanceTimer::reset(const std::string& label) {
    auto it = timers_.find(label);
    if (it != timers_.end()) {
        it->second = TimingData();
    }
}

void PerformanceTimer::resetAll() {
    timers_.clear();
}

double PerformanceTimer::elapsed(const std::string& label) const {
    auto it = timers_.find(label);
    if (it == timers_.end()) {
        return 0.0;
    }
    return it->second.totalTime;
}

double PerformanceTimer::elapsedMs(const std::string& label) const {
    return elapsed(label) * 1000.0;
}

void PerformanceTimer::printSummary() const {
    std::cout << "\n=== Performance Summary ===" << std::endl;
    std::cout << std::fixed << std::setprecision(3);
    
    for (const auto& pair : timers_) {
        printSummary(pair.first);
    }
}

void PerformanceTimer::printSummary(const std::string& label) const {
    auto it = timers_.find(label);
    if (it == timers_.end()) {
        std::cout << "Timer '" << label << "' not found" << std::endl;
        return;
    }
    
    const auto& data = it->second;
    
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "  " << label << ": " << data.totalTime << " s";
    
    if (data.callCount > 1) {
        double avgTime = data.totalTime / data.callCount;
        std::cout << " (" << data.callCount << " calls, " 
                  << avgTime << " s avg)";
    }
    
    std::cout << std::endl;
}
