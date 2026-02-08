#ifndef HISTOGRAM_MANAGER_H
#define HISTOGRAM_MANAGER_H

#include <TH1F.h>
#include <TH2F.h>
#include <TFile.h>
#include <map>
#include <string>
#include <memory>

/**
 * @brief Manages histogram creation and storage
 * 
 * Centralized histogram management to avoid memory leaks and
 * provide consistent naming/binning across analyses.
 */
class HistogramManager {
public:
    HistogramManager();
    ~HistogramManager();

    // 1D histogram creation
    TH1F* create1D(const std::string& name, const std::string& title,
                   int nbins, double xmin, double xmax);

    // 2D histogram creation
    TH2F* create2D(const std::string& name, const std::string& title,
                   int nbinsX, double xmin, double xmax,
                   int nbinsY, double ymin, double ymax);

    // Histogram retrieval
    TH1F* get1D(const std::string& name);
    TH2F* get2D(const std::string& name);

    // File I/O
    void writeToFile(const std::string& filename);
    void writeToFile(TFile* file);

    // Utilities
    void print() const;
    size_t size() const { return histograms1D_.size() + histograms2D_.size(); }

private:
    std::map<std::string, std::unique_ptr<TH1F>> histograms1D_;
    std::map<std::string, std::unique_ptr<TH2F>> histograms2D_;
};

#endif // HISTOGRAM_MANAGER_H
