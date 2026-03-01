#include "EventReader.h"
#include "ZAnalysis.h"
#include "TopAnalysis.h"
#include "HistogramManager.h"
#include "PerformanceTimer.h"

#include <iostream>
#include <string>
#include <TFile.h>

int main(int argc, char** argv) {
    // Parse command line
    std::string inputFile = "data/simulated_events.root";
    std::string outputFile = "results/analysis_output.root";
    
    if (argc > 1) inputFile = argv[1];
    if (argc > 2) outputFile = argv[2];
    
    std::cout << "\n=== LHC Event Analysis Pipeline ===" << std::endl;
    std::cout << "Input:  " << inputFile << std::endl;
    std::cout << "Output: " << outputFile << std::endl;
    
    // Initialize performance timer
    PerformanceTimer timer;
    timer.start("total");
    
    // Open input file
    EventReader reader(inputFile);
    if (!reader.isValid()) {
        std::cerr << "Error: Failed to open input file" << std::endl;
        return 1;
    }
    
    std::cout << "Found " << reader.nEvents() << " events to process" << std::endl;
    
    // Initialize histogram manager and analyses
    HistogramManager histMgr;
    ZAnalysis zAnalysis(histMgr);
    TopAnalysis topAnalysis(histMgr);
    
    // Configure analyses
    zAnalysis.setMuonPtCut(20.0);
    zAnalysis.setMuonEtaCut(2.4);
    
    topAnalysis.setJetPtCut(30.0);
    topAnalysis.setLeptonPtCut(25.0);
    topAnalysis.setRequireBTag(true);
    
    // Event loop
    std::cout << "\nProcessing events..." << std::endl;
    timer.start("event_loop");
    
    Event event;
    unsigned long nProcessed = 0;
    
    while (reader.nextEvent(event)) {
        // Run both analyses on each event
        zAnalysis.processEvent(event);
        topAnalysis.processEvent(event);
        
        nProcessed++;
        if (nProcessed % 10000 == 0) {
            std::cout << "  Processed " << nProcessed << " events" << std::endl;
        }
    }
    
    timer.stop("event_loop");
    
    // Finalize analyses
    zAnalysis.finalize();
    topAnalysis.finalize();
    
    // Write output
    std::cout << "\nWriting results to " << outputFile << std::endl;
    timer.start("write_output");
    
    TFile outFile(outputFile.c_str(), "RECREATE");
    histMgr.writeToFile(&outFile);
    outFile.Close();
    
    timer.stop("write_output");
    timer.stop("total");
    
    // Print performance summary
    timer.printSummary();
    
    // Print analysis summary
    std::cout << "\n=== Z Analysis Summary ===" << std::endl;
    std::cout << "Events processed: " << zAnalysis.nEventsProcessed() << std::endl;
    std::cout << "Events passed:    " << zAnalysis.nEventsPassed() << std::endl;
    std::cout << "Efficiency:       " << zAnalysis.efficiency() * 100.0 << "%" << std::endl;
    
    std::cout << "\n=== Top Analysis Summary ===" << std::endl;
    std::cout << "Events processed: " << topAnalysis.nEventsProcessed() << std::endl;
    std::cout << "Events passed:    " << topAnalysis.nEventsPassed() << std::endl;
    std::cout << "Efficiency:       " << topAnalysis.efficiency() * 100.0 << "%" << std::endl;
    
    std::cout << "\nAnalysis complete!" << std::endl;
    std::cout << "Processing rate: " 
              << static_cast<double>(nProcessed) / timer.elapsed("event_loop")
              << " events/sec" << std::endl;
    
    return 0;
}
