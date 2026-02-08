#include "HistogramManager.h"
#include <iostream>

HistogramManager::HistogramManager() {}

HistogramManager::~HistogramManager() {
    // unique_ptr handles cleanup automatically
}

TH1F* HistogramManager::create1D(const std::string& name, const std::string& title,
                                  int nbins, double xmin, double xmax) {
    auto it = histograms1D_.find(name);
    if (it != histograms1D_.end()) {
        std::cerr << "Warning: Histogram '" << name << "' already exists" << std::endl;
        return it->second.get();
    }

    auto hist = std::make_unique<TH1F>(name.c_str(), title.c_str(), nbins, xmin, xmax);
    hist->SetDirectory(nullptr); // Prevent ROOT from taking ownership
    
    TH1F* ptr = hist.get();
    histograms1D_[name] = std::move(hist);
    return ptr;
}

TH2F* HistogramManager::create2D(const std::string& name, const std::string& title,
                                  int nbinsX, double xmin, double xmax,
                                  int nbinsY, double ymin, double ymax) {
    auto it = histograms2D_.find(name);
    if (it != histograms2D_.end()) {
        std::cerr << "Warning: Histogram '" << name << "' already exists" << std::endl;
        return it->second.get();
    }

    auto hist = std::make_unique<TH2F>(name.c_str(), title.c_str(), 
                                        nbinsX, xmin, xmax, nbinsY, ymin, ymax);
    hist->SetDirectory(nullptr);
    
    TH2F* ptr = hist.get();
    histograms2D_[name] = std::move(hist);
    return ptr;
}

TH1F* HistogramManager::get1D(const std::string& name) {
    auto it = histograms1D_.find(name);
    if (it != histograms1D_.end()) {
        return it->second.get();
    }
    return nullptr;
}

TH2F* HistogramManager::get2D(const std::string& name) {
    auto it = histograms2D_.find(name);
    if (it != histograms2D_.end()) {
        return it->second.get();
    }
    return nullptr;
}

void HistogramManager::writeToFile(const std::string& filename) {
    TFile file(filename.c_str(), "RECREATE");
    writeToFile(&file);
    file.Close();
}

void HistogramManager::writeToFile(TFile* file) {
    if (!file || !file->IsOpen()) {
        std::cerr << "Error: Invalid TFile in writeToFile" << std::endl;
        return;
    }

    file->cd();
    
    for (auto& pair : histograms1D_) {
        pair.second->Write();
    }
    
    for (auto& pair : histograms2D_) {
        pair.second->Write();
    }
}

void HistogramManager::print() const {
    std::cout << "HistogramManager contains:" << std::endl;
    std::cout << "  1D histograms: " << histograms1D_.size() << std::endl;
    std::cout << "  2D histograms: " << histograms2D_.size() << std::endl;
}
