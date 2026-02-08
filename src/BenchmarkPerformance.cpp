#include "EventReader.h"
#include "ZAnalysis.h"
#include "TopAnalysis.h"
#include "HistogramManager.h"
#include "PerformanceTimer.h"

#include <iostream>
#include <string>
#include <vector>

#ifdef USE_OPENMP
#include <omp.h>
#endif

void runSingleThreaded(const std::string& inputFile, HistogramManager& histMgr, 
                       PerformanceTimer& timer) {
    timer.start("single_threaded");
    
    EventReader reader(inputFile);
    ZAnalysis zAnalysis(histMgr);
    TopAnalysis topAnalysis(histMgr);
    
    Event event;
    while (reader.nextEvent(event)) {
        zAnalysis.processEvent(event);
        topAnalysis.processEvent(event);
    }
    
    timer.stop("single_threaded");
}

void runMultiThreaded(const std::string& inputFile, HistogramManager& histMgr,
                      PerformanceTimer& timer, int nThreads) {
#ifdef USE_OPENMP
    timer.start("multi_threaded");
    
    EventReader reader(inputFile);
    unsigned long nEvents = reader.nEvents();
    
    std::cout << "Using " << nThreads << " threads" << std::endl;
    
    // Each thread gets its own histogram manager
    std::vector<HistogramManager> threadHistMgrs(nThreads);
    std::vector<ZAnalysis*> zAnalyses;
    std::vector<TopAnalysis*> topAnalyses;
    
    for (int i = 0; i < nThreads; ++i) {
        zAnalyses.push_back(new ZAnalysis(threadHistMgrs[i]));
        topAnalyses.push_back(new TopAnalysis(threadHistMgrs[i]));
    }
    
    // Parallel event processing
    #pragma omp parallel num_threads(nThreads)
    {
        int tid = omp_get_thread_num();
        EventReader localReader(inputFile);
        Event event;
        
        #pragma omp for schedule(dynamic, 1000)
        for (unsigned long i = 0; i < nEvents; ++i) {
            localReader.getEvent(i, event);
            zAnalyses[tid]->processEvent(event);
            topAnalyses[tid]->processEvent(event);
        }
    }
    
    // Merge histograms from all threads
    for (int i = 1; i < nThreads; ++i) {
        // Get all histograms from thread i and add to main histogram manager
        auto* z_mass = threadHistMgrs[i].get1D("z_mass");
        if (z_mass) histMgr.get1D("z_mass")->Add(z_mass);
        
        auto* z_pt = threadHistMgrs[i].get1D("z_pt");
        if (z_pt) histMgr.get1D("z_pt")->Add(z_pt);
        
        auto* top_mass = threadHistMgrs[i].get1D("top_hadronic_mass");
        if (top_mass) histMgr.get1D("top_hadronic_mass")->Add(top_mass);
        
        // Add more histogram merging as needed
    }
    
    // Cleanup
    for (auto* analysis : zAnalyses) delete analysis;
    for (auto* analysis : topAnalyses) delete analysis;
    
    timer.stop("multi_threaded");
#else
    std::cout << "OpenMP not available - skipping multi-threaded benchmark" << std::endl;
#endif
}

int main(int argc, char** argv) {
    std::string inputFile = "data/simulated_events.root";
    
    if (argc > 1) inputFile = argv[1];
    
    std::cout << "\n=== Performance Benchmark ===" << std::endl;
    std::cout << "Input file: " << inputFile << std::endl;
    
    EventReader reader(inputFile);
    if (!reader.isValid()) {
        std::cerr << "Error: Cannot open input file" << std::endl;
        return 1;
    }
    
    unsigned long nEvents = reader.nEvents();
    std::cout << "Events to process: " << nEvents << std::endl;
    
    PerformanceTimer timer;
    
    // Baseline: Single-threaded
    std::cout << "\n--- Single-Threaded Baseline ---" << std::endl;
    HistogramManager histMgrSingle;
    ZAnalysis zAnalysisSingle(histMgrSingle);
    TopAnalysis topAnalysisSingle(histMgrSingle);
    runSingleThreaded(inputFile, histMgrSingle, timer);
    
    double singleTime = timer.elapsed("single_threaded");
    double singleRate = nEvents / singleTime;
    
    std::cout << "Time:       " << singleTime << " s" << std::endl;
    std::cout << "Rate:       " << singleRate << " events/s" << std::endl;
    
#ifdef USE_OPENMP
    // Multi-threaded benchmark
    std::cout << "\n--- Multi-Threaded (OpenMP) ---" << std::endl;
    
    int maxThreads = omp_get_max_threads();
    std::vector<int> threadCounts = {2, 4};
    if (maxThreads >= 8) threadCounts.push_back(8);
    
    for (int nThreads : threadCounts) {
        if (nThreads > maxThreads) continue;
        
        std::cout << "\nThreads: " << nThreads << std::endl;
        
        HistogramManager histMgrMulti;
        ZAnalysis zAnalysisInit(histMgrMulti); // Create base histograms
        TopAnalysis topAnalysisInit(histMgrMulti);
        
        std::string label = "threads_" + std::to_string(nThreads);
        timer.start(label);
        runMultiThreaded(inputFile, histMgrMulti, timer, nThreads);
        timer.stop(label);
        
        double multiTime = timer.elapsed(label);
        double multiRate = nEvents / multiTime;
        double speedup = singleTime / multiTime;
        double efficiency = speedup / nThreads * 100.0;
        
        std::cout << "Time:       " << multiTime << " s" << std::endl;
        std::cout << "Rate:       " << multiRate << " events/s" << std::endl;
        std::cout << "Speedup:    " << speedup << "x" << std::endl;
        std::cout << "Efficiency: " << efficiency << "%" << std::endl;
    }
    
    // Summary
    std::cout << "\n=== Performance Summary ===" << std::endl;
    timer.printSummary();
#else
    std::cout << "\nOpenMP not enabled - compile with -fopenmp for parallel benchmarks" << std::endl;
#endif
    
    return 0;
}
