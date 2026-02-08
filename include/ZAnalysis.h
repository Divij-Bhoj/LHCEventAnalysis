#ifndef Z_ANALYSIS_H
#define Z_ANALYSIS_H

#include "Event.h"
#include "HistogramManager.h"
#include <vector>

/**
 * @brief Z → μ⁺μ⁻ analysis module
 * 
 * Reconstructs Z boson candidates from opposite-sign muon pairs.
 * Applies standard selection cuts and fills diagnostic histograms.
 */
class ZAnalysis {
public:
    ZAnalysis(HistogramManager& histMgr);

    // Analysis methods
    bool processEvent(const Event& event);
    void finalize();

    // Configuration
    void setMuonPtCut(double pt) { muonPtCut_ = pt; }
    void setMuonEtaCut(double eta) { muonEtaCut_ = eta; }
    void setMassWindowLow(double mass) { massWindowLow_ = mass; }
    void setMassWindowHigh(double mass) { massWindowHigh_ = mass; }

    // Statistics
    unsigned long nEventsProcessed() const { return nEventsProcessed_; }
    unsigned long nEventsPassed() const { return nEventsPassed_; }
    double efficiency() const;

private:
    HistogramManager& histMgr_;
    
    // Selection cuts
    double muonPtCut_;
    double muonEtaCut_;
    double massWindowLow_;
    double massWindowHigh_;

    // Statistics
    unsigned long nEventsProcessed_;
    unsigned long nEventsPassed_;

    // Helper methods
    void bookHistograms();
    std::vector<Particle> selectMuons(const Event& event) const;
    bool passesDimuonSelection(const std::vector<Particle>& muons) const;
    void fillHistograms(const Particle& mu1, const Particle& mu2, const Particle& Z);
};

#endif // Z_ANALYSIS_H
