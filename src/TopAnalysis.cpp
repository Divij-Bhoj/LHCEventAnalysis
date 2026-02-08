#include "TopAnalysis.h"
#include <iostream>
#include <algorithm>
#include <cmath>

TopAnalysis::TopAnalysis(HistogramManager& histMgr)
    : histMgr_(histMgr),
      jetPtCut_(30.0),       // GeV
      jetEtaCut_(2.5),
      leptonPtCut_(25.0),    // GeV
      metCut_(20.0),         // GeV
      requireBTag_(true),
      nEventsProcessed_(0),
      nEventsPassed_(0) {
    
    bookHistograms();
}

void TopAnalysis::bookHistograms() {
    // Hadronic W reconstruction
    histMgr_.create1D("w_hadronic_mass", 
                      "Hadronic W Mass;m_{jj} [GeV];Events / 2 GeV",
                      100, 0, 200);
    
    // Hadronic top reconstruction
    histMgr_.create1D("top_hadronic_mass", 
                      "Hadronic Top Mass;m_{bjj} [GeV];Events / 5 GeV",
                      100, 0, 500);
    histMgr_.create1D("top_hadronic_pt", 
                      "Hadronic Top p_{T};p_{T}^{top} [GeV];Events / 10 GeV",
                      100, 0, 1000);
    
    // Jet multiplicity
    histMgr_.create1D("n_jets", "Jet Multiplicity;N_{jets};Events",
                      15, 0, 15);
    histMgr_.create1D("n_bjets", "b-Jet Multiplicity;N_{b-jets};Events",
                      8, 0, 8);
    
    // Jet kinematics
    histMgr_.create1D("jet_pt", "Jet p_{T};p_{T}^{jet} [GeV];Jets / 5 GeV",
                      100, 0, 500);
    histMgr_.create1D("jet_eta", "Jet #eta;#eta^{jet};Jets / 0.1",
                      100, -5, 5);
    
    // Event-level
    histMgr_.create1D("met", "Missing E_{T};E_{T}^{miss} [GeV];Events / 5 GeV",
                      100, 0, 500);
    histMgr_.create1D("ht", "Scalar H_{T};H_{T} [GeV];Events / 20 GeV",
                      100, 0, 2000);
}

std::vector<Particle> TopAnalysis::selectJets(const Event& event) const {
    std::vector<Particle> selectedJets;
    
    auto jets = event.getJets();
    for (const auto& jet : jets) {
        if (jet.pt() > jetPtCut_ && std::abs(jet.eta()) < jetEtaCut_) {
            selectedJets.push_back(jet);
        }
    }
    
    // Sort by pT (descending)
    std::sort(selectedJets.begin(), selectedJets.end(),
              [](const Particle& a, const Particle& b) { return a.pt() > b.pt(); });
    
    return selectedJets;
}

std::vector<Particle> TopAnalysis::selectLeptons(const Event& event) const {
    std::vector<Particle> selectedLeptons;
    
    auto muons = event.getMuons();
    auto electrons = event.getElectrons();
    
    for (const auto& mu : muons) {
        if (mu.pt() > leptonPtCut_ && std::abs(mu.eta()) < 2.4) {
            selectedLeptons.push_back(mu);
        }
    }
    
    for (const auto& el : electrons) {
        if (el.pt() > leptonPtCut_ && std::abs(el.eta()) < 2.5) {
            selectedLeptons.push_back(el);
        }
    }
    
    return selectedLeptons;
}

bool TopAnalysis::passesSemileptonicSelection(const std::vector<Particle>& leptons,
                                                const std::vector<Particle>& jets,
                                                const Event& event) const {
    // Require exactly one lepton
    if (leptons.size() != 1) return false;
    
    // Require at least 4 jets
    if (jets.size() < 4) return false;
    
    // Require MET
    if (event.missingET() < metCut_) return false;
    
    // Require at least 2 b-tagged jets if b-tagging enabled
    if (requireBTag_) {
        auto bjets = event.getBJets();
        if (bjets.size() < 2) return false;
    }
    
    return true;
}

void TopAnalysis::reconstructHadronicTop(const std::vector<Particle>& jets) {
    // Find best W→jj candidate (closest to W mass)
    double bestWMassDiff = 1e9;
    Particle bestW;
    size_t bestJ1 = 0, bestJ2 = 0;
    
    for (size_t i = 0; i < jets.size(); ++i) {
        for (size_t j = i + 1; j < jets.size(); ++j) {
            // Skip if both are b-tagged (W decays to light quarks)
            if (jets[i].isBTagged() && jets[j].isBTagged()) continue;
            
            Particle W_candidate = jets[i] + jets[j];
            double massDiff = std::abs(W_candidate.mass() - W_MASS);
            
            if (massDiff < bestWMassDiff) {
                bestWMassDiff = massDiff;
                bestW = W_candidate;
                bestJ1 = i;
                bestJ2 = j;
            }
        }
    }
    
    histMgr_.get1D("w_hadronic_mass")->Fill(bestW.mass());
    
    // Find b-jet to combine with W (closest to top mass)
    double bestTopMassDiff = 1e9;
    Particle bestTop;
    
    for (size_t i = 0; i < jets.size(); ++i) {
        // Skip jets used in W
        if (i == bestJ1 || i == bestJ2) continue;
        
        // Prefer b-tagged jets
        if (requireBTag_ && !jets[i].isBTagged()) continue;
        
        Particle top_candidate = bestW + jets[i];
        double massDiff = std::abs(top_candidate.mass() - TOP_MASS);
        
        if (massDiff < bestTopMassDiff) {
            bestTopMassDiff = massDiff;
            bestTop = top_candidate;
        }
    }
    
    histMgr_.get1D("top_hadronic_mass")->Fill(bestTop.mass());
    histMgr_.get1D("top_hadronic_pt")->Fill(bestTop.pt());
}

bool TopAnalysis::processEvent(const Event& event) {
    nEventsProcessed_++;
    
    // Select physics objects
    auto selectedJets = selectJets(event);
    auto selectedLeptons = selectLeptons(event);
    
    // Fill pre-selection histograms
    histMgr_.get1D("n_jets")->Fill(selectedJets.size());
    histMgr_.get1D("n_bjets")->Fill(event.getBJets().size());
    histMgr_.get1D("met")->Fill(event.missingET());
    histMgr_.get1D("ht")->Fill(event.scalarHT());
    
    for (const auto& jet : selectedJets) {
        histMgr_.get1D("jet_pt")->Fill(jet.pt());
        histMgr_.get1D("jet_eta")->Fill(jet.eta());
    }
    
    // Apply selection
    if (!passesSemileptonicSelection(selectedLeptons, selectedJets, event)) {
        return false;
    }
    
    nEventsPassed_++;
    
    // Reconstruct hadronic top
    reconstructHadronicTop(selectedJets);
    
    return true;
}

void TopAnalysis::finalize() {
    std::cout << "\n=== Top Analysis Summary ===" << std::endl;
    std::cout << "Events processed: " << nEventsProcessed_ << std::endl;
    std::cout << "Events passed:    " << nEventsPassed_ << std::endl;
    std::cout << "Efficiency:       " << efficiency() * 100 << "%" << std::endl;
}

double TopAnalysis::efficiency() const {
    if (nEventsProcessed_ == 0) return 0.0;
    return static_cast<double>(nEventsPassed_) / nEventsProcessed_;
}
