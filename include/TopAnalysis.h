#ifndef TOP_ANALYSIS_H
#define TOP_ANALYSIS_H

#include "Event.h"
#include "HistogramManager.h"
#include <vector>

/**
 * @brief tt̄ → lepton + jets analysis module
 * 
 * Reconstructs hadronic top quark from jets in semi-leptonic ttbar events.
 * Uses simplified b-tagging and W→jj reconstruction.
 */
class TopAnalysis {
public:
    TopAnalysis(HistogramManager& histMgr);

    // Analysis methods
    bool processEvent(const Event& event);
    void finalize();

    // Configuration
    void setJetPtCut(double pt) { jetPtCut_ = pt; }
    void setJetEtaCut(double eta) { jetEtaCut_ = eta; }
    void setLeptonPtCut(double pt) { leptonPtCut_ = pt; }
    void setRequireBTag(bool require) { requireBTag_ = require; }

    // Statistics
    unsigned long nEventsProcessed() const { return nEventsProcessed_; }
    unsigned long nEventsPassed() const { return nEventsPassed_; }
    double efficiency() const;

private:
    HistogramManager& histMgr_;
    
    // Selection cuts
    double jetPtCut_;
    double jetEtaCut_;
    double leptonPtCut_;
    double metCut_;
    bool requireBTag_;

    // Physics constants
    static constexpr double W_MASS = 80.4;   // GeV
    static constexpr double TOP_MASS = 173.0; // GeV

    // Statistics
    unsigned long nEventsProcessed_;
    unsigned long nEventsPassed_;

    // Helper methods
    void bookHistograms();
    std::vector<Particle> selectJets(const Event& event) const;
    std::vector<Particle> selectLeptons(const Event& event) const;
    bool passesSemileptonicSelection(const std::vector<Particle>& leptons,
                                      const std::vector<Particle>& jets,
                                      const Event& event) const;
    void reconstructHadronicTop(const std::vector<Particle>& jets);
};

#endif // TOP_ANALYSIS_H
