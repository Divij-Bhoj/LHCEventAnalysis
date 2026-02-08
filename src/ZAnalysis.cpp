#include "ZAnalysis.h"
#include <iostream>
#include <algorithm>

ZAnalysis::ZAnalysis(HistogramManager& histMgr)
    : histMgr_(histMgr),
      muonPtCut_(15.0),      // GeV
      muonEtaCut_(2.4),
      massWindowLow_(50.0),  // GeV
      massWindowHigh_(130.0), // GeV
      nEventsProcessed_(0),
      nEventsPassed_(0) {
    
    bookHistograms();
}

void ZAnalysis::bookHistograms() {
    // Z mass spectrum
    histMgr_.create1D("z_mass", "Z #rightarrow #mu^{+}#mu^{-} Invariant Mass;m_{#mu#mu} [GeV];Events / 2 GeV",
                      100, 0, 200);
    
    // Z kinematic distributions
    histMgr_.create1D("z_pt", "Z Boson p_{T};p_{T}^{Z} [GeV];Events / 5 GeV", 
                      100, 0, 500);
    histMgr_.create1D("z_eta", "Z Boson #eta;#eta^{Z};Events / 0.1", 
                      100, -5, 5);
    histMgr_.create1D("z_phi", "Z Boson #phi;#phi^{Z};Events / 0.1", 
                      64, -3.2, 3.2);
    
    // Muon kinematics
    histMgr_.create1D("muon_pt", "Muon p_{T};p_{T}^{#mu} [GeV];Muons / 2 GeV", 
                      150, 0, 300);
    histMgr_.create1D("muon_eta", "Muon #eta;#eta^{#mu};Muons / 0.1", 
                      100, -5, 5);
    
    // Di-muon system
    histMgr_.create2D("muon1_pt_vs_muon2_pt", 
                      "Leading vs Subleading Muon p_{T};p_{T}^{#mu1} [GeV];p_{T}^{#mu2} [GeV]",
                      50, 0, 250, 50, 0, 250);
    histMgr_.create1D("dimuon_deltaR", "#DeltaR(#mu^{+}, #mu^{-});#DeltaR;Events / 0.1",
                      50, 0, 5);
}

std::vector<Particle> ZAnalysis::selectMuons(const Event& event) const {
    std::vector<Particle> selectedMuons;
    
    auto muons = event.getMuons();
    for (const auto& muon : muons) {
        if (muon.pt() > muonPtCut_ && std::abs(muon.eta()) < muonEtaCut_) {
            selectedMuons.push_back(muon);
        }
    }
    
    return selectedMuons;
}

bool ZAnalysis::passesDimuonSelection(const std::vector<Particle>& muons) const {
    if (muons.size() < 2) return false;
    
    // Check for opposite-sign pair
    for (size_t i = 0; i < muons.size(); ++i) {
        for (size_t j = i + 1; j < muons.size(); ++j) {
            if (muons[i].charge() * muons[j].charge() < 0) {
                return true;
            }
        }
    }
    
    return false;
}

bool ZAnalysis::processEvent(const Event& event) {
    nEventsProcessed_++;
    
    // Select muons passing cuts
    auto selectedMuons = selectMuons(event);
    
    if (!passesDimuonSelection(selectedMuons)) {
        return false;
    }
    
    // Find best opposite-sign pair (highest pT sum)
    Particle bestMu1, bestMu2;
    double bestPtSum = 0;
    
    for (size_t i = 0; i < selectedMuons.size(); ++i) {
        for (size_t j = i + 1; j < selectedMuons.size(); ++j) {
            if (selectedMuons[i].charge() * selectedMuons[j].charge() < 0) {
                double ptSum = selectedMuons[i].pt() + selectedMuons[j].pt();
                if (ptSum > bestPtSum) {
                    bestPtSum = ptSum;
                    bestMu1 = selectedMuons[i];
                    bestMu2 = selectedMuons[j];
                }
            }
        }
    }
    
    // Reconstruct Z candidate
    Particle Z = bestMu1 + bestMu2;
    double zMass = Z.mass();
    
    // Apply mass window cut
    if (zMass < massWindowLow_ || zMass > massWindowHigh_) {
        return false;
    }
    
    nEventsPassed_++;
    
    // Fill histograms
    fillHistograms(bestMu1, bestMu2, Z);
    
    return true;
}

void ZAnalysis::fillHistograms(const Particle& mu1, const Particle& mu2, const Particle& Z) {
    // Z boson properties
    histMgr_.get1D("z_mass")->Fill(Z.mass());
    histMgr_.get1D("z_pt")->Fill(Z.pt());
    histMgr_.get1D("z_eta")->Fill(Z.eta());
    histMgr_.get1D("z_phi")->Fill(Z.phi());
    
    // Muon properties
    histMgr_.get1D("muon_pt")->Fill(mu1.pt());
    histMgr_.get1D("muon_pt")->Fill(mu2.pt());
    histMgr_.get1D("muon_eta")->Fill(mu1.eta());
    histMgr_.get1D("muon_eta")->Fill(mu2.eta());
    
    // Di-muon correlations
    double pt1 = std::max(mu1.pt(), mu2.pt());
    double pt2 = std::min(mu1.pt(), mu2.pt());
    histMgr_.get2D("muon1_pt_vs_muon2_pt")->Fill(pt1, pt2);
    histMgr_.get1D("dimuon_deltaR")->Fill(mu1.deltaR(mu2));
}

void ZAnalysis::finalize() {
    std::cout << "\n=== Z Analysis Summary ===" << std::endl;
    std::cout << "Events processed: " << nEventsProcessed_ << std::endl;
    std::cout << "Events passed:    " << nEventsPassed_ << std::endl;
    std::cout << "Efficiency:       " << efficiency() * 100 << "%" << std::endl;
}

double ZAnalysis::efficiency() const {
    if (nEventsProcessed_ == 0) return 0.0;
    return static_cast<double>(nEventsPassed_) / nEventsProcessed_;
}
