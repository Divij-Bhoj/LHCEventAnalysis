#include <TFile.h>
#include <TTree.h>
#include <TRandom3.h>
#include <iostream>
#include <cmath>

/**
 * @brief Generates simulated LHC collision events
 * 
 * Creates realistic-looking events for Z→μμ and tt̄→lepton+jets processes.
 * Uses simplified physics models for educational purposes.
 */

// Event structure matching EventReader
struct EventData {
    ULong64_t eventNumber;
    Int_t nParticles;
    static const int kMaxParticles = 100;
    Float_t particle_pt[kMaxParticles];
    Float_t particle_eta[kMaxParticles];
    Float_t particle_phi[kMaxParticles];
    Float_t particle_mass[kMaxParticles];
    Int_t particle_type[kMaxParticles];
    Int_t particle_charge[kMaxParticles];
    Bool_t particle_isBTagged[kMaxParticles];
};

class EventGenerator {
public:
    EventGenerator(unsigned int seed = 42) : rng_(seed) {}
    
    void generateZEvent(EventData& data, ULong64_t eventNum);
    void generateTopEvent(EventData& data, ULong64_t eventNum);
    void generateBackgroundEvent(EventData& data, ULong64_t eventNum);

private:
    TRandom3 rng_;
    
    void addMuon(EventData& data, double pt, double eta, double phi, int charge);
    void addJet(EventData& data, double pt, double eta, double phi, bool isBJet = false);
    void addElectron(EventData& data, double pt, double eta, double phi, int charge);
    
    double sampleZPt();
    double sampleTopPt();
};

void EventGenerator::addMuon(EventData& data, double pt, double eta, double phi, int charge) {
    int idx = data.nParticles++;
    data.particle_pt[idx] = pt;
    data.particle_eta[idx] = eta;
    data.particle_phi[idx] = phi;
    data.particle_mass[idx] = 0.105; // Muon mass in GeV
    data.particle_type[idx] = 0; // MUON
    data.particle_charge[idx] = charge;
    data.particle_isBTagged[idx] = false;
}

void EventGenerator::addElectron(EventData& data, double pt, double eta, double phi, int charge) {
    int idx = data.nParticles++;
    data.particle_pt[idx] = pt;
    data.particle_eta[idx] = eta;
    data.particle_phi[idx] = phi;
    data.particle_mass[idx] = 0.000511; // Electron mass in GeV
    data.particle_type[idx] = 1; // ELECTRON
    data.particle_charge[idx] = charge;
    data.particle_isBTagged[idx] = false;
}

void EventGenerator::addJet(EventData& data, double pt, double eta, double phi, bool isBJet) {
    int idx = data.nParticles++;
    data.particle_pt[idx] = pt;
    data.particle_eta[idx] = eta;
    data.particle_phi[idx] = phi;
    data.particle_mass[idx] = 5.0; // Approximate jet mass
    data.particle_type[idx] = isBJet ? 3 : 2; // BJET : JET
    data.particle_charge[idx] = 0;
    data.particle_isBTagged[idx] = isBJet;
}

double EventGenerator::sampleZPt() {
    // Simplified Z pT spectrum (peaked at low pT, falling exponentially)
    return rng_.Exp(50.0) + rng_.Gaus(30, 20);
}

double EventGenerator::sampleTopPt() {
    // Top quarks have harder pT spectrum
    return rng_.Exp(100.0) + rng_.Gaus(80, 40);
}

void EventGenerator::generateZEvent(EventData& data, ULong64_t eventNum) {
    data.eventNumber = eventNum;
    data.nParticles = 0;
    
    // Generate Z boson kinematics
    double zPt = sampleZPt();
    double zEta = rng_.Gaus(0, 1.5);
    double zPhi = rng_.Uniform(-M_PI, M_PI);
    // Note: Using simplified Z decay model (not using exact Z mass)
    
    // Decay Z → μ⁺μ⁻ (simplified - assume back-to-back in Z rest frame)
    double mu1Pt = zPt/2.0 + rng_.Gaus(0, 10);
    double mu2Pt = zPt/2.0 + rng_.Gaus(0, 10);
    double mu1Eta = zEta + rng_.Gaus(0, 0.5);
    double mu2Eta = zEta + rng_.Gaus(0, 0.5);
    double mu1Phi = zPhi + rng_.Gaus(0, 0.5);
    double mu2Phi = zPhi + M_PI + rng_.Gaus(0, 0.5);
    
    addMuon(data, mu1Pt, mu1Eta, mu1Phi, 1);
    addMuon(data, mu2Pt, mu2Eta, mu2Phi, -1);
    
    // Add some soft jets (underlying event)
    int nJets = rng_.Poisson(2);
    for (int i = 0; i < nJets && data.nParticles < EventData::kMaxParticles - 5; ++i) {
        double jetPt = rng_.Exp(20.0) + 15.0;
        double jetEta = rng_.Uniform(-4.0, 4.0);
        double jetPhi = rng_.Uniform(-M_PI, M_PI);
        addJet(data, jetPt, jetEta, jetPhi, false);
    }
}

void EventGenerator::generateTopEvent(EventData& data, ULong64_t eventNum) {
    data.eventNumber = eventNum;
    data.nParticles = 0;
    
    // Semi-leptonic ttbar: one top → bW→bℓν, other top → bW→bjj
    
    // Leptonic branch
    double leptonPt = rng_.Gaus(60, 25);
    if (leptonPt < 20) leptonPt = 20 + rng_.Uniform(0, 20);
    double leptonEta = rng_.Uniform(-2.5, 2.5);
    double leptonPhi = rng_.Uniform(-M_PI, M_PI);
    
    // 50/50 muon or electron
    if (rng_.Uniform() < 0.5) {
        addMuon(data, leptonPt, leptonEta, leptonPhi, rng_.Uniform() < 0.5 ? 1 : -1);
    } else {
        addElectron(data, leptonPt, leptonEta, leptonPhi, rng_.Uniform() < 0.5 ? 1 : -1);
    }
    
    // Leptonic b-jet
    double bLepPt = rng_.Gaus(80, 30);
    if (bLepPt < 30) bLepPt = 30 + rng_.Uniform(0, 30);
    double bLepEta = rng_.Uniform(-2.5, 2.5);
    double bLepPhi = rng_.Uniform(-M_PI, M_PI);
    addJet(data, bLepPt, bLepEta, bLepPhi, true);
    
    // Hadronic W → jj
    double w1Pt = rng_.Gaus(70, 25);
    double w2Pt = rng_.Gaus(60, 25);
    if (w1Pt < 30) w1Pt = 30 + rng_.Uniform(0, 20);
    if (w2Pt < 30) w2Pt = 30 + rng_.Uniform(0, 20);
    
    double w1Eta = rng_.Uniform(-2.5, 2.5);
    double w2Eta = rng_.Uniform(-2.5, 2.5);
    double w1Phi = rng_.Uniform(-M_PI, M_PI);
    double w2Phi = rng_.Uniform(-M_PI, M_PI);
    
    addJet(data, w1Pt, w1Eta, w1Phi, false);
    addJet(data, w2Pt, w2Eta, w2Phi, false);
    
    // Hadronic b-jet
    double bHadPt = rng_.Gaus(90, 35);
    if (bHadPt < 30) bHadPt = 30 + rng_.Uniform(0, 30);
    double bHadEta = rng_.Uniform(-2.5, 2.5);
    double bHadPhi = rng_.Uniform(-M_PI, M_PI);
    addJet(data, bHadPt, bHadEta, bHadPhi, true);
    
    // Additional jets (ISR/FSR)
    int nExtraJets = rng_.Poisson(1.5);
    for (int i = 0; i < nExtraJets && data.nParticles < EventData::kMaxParticles - 5; ++i) {
        double jetPt = rng_.Exp(30.0) + 25.0;
        double jetEta = rng_.Uniform(-4.0, 4.0);
        double jetPhi = rng_.Uniform(-M_PI, M_PI);
        // 10% chance of b-jet
        bool isBJet = rng_.Uniform() < 0.1;
        addJet(data, jetPt, jetEta, jetPhi, isBJet);
    }
}

void EventGenerator::generateBackgroundEvent(EventData& data, ULong64_t eventNum) {
    data.eventNumber = eventNum;
    data.nParticles = 0;
    
    // Multijet background - just jets
    int nJets = rng_.Poisson(4.0) + 2;
    for (int i = 0; i < nJets && data.nParticles < EventData::kMaxParticles - 5; ++i) {
        double jetPt = rng_.Exp(40.0) + 30.0;
        double jetEta = rng_.Uniform(-4.0, 4.0);
        double jetPhi = rng_.Uniform(-M_PI, M_PI);
        bool isBJet = rng_.Uniform() < 0.15; // 15% b-tagging rate
        addJet(data, jetPt, jetEta, jetPhi, isBJet);
    }
}

int main(int argc, char** argv) {
    // Parse command line
    int nEvents = 50000;
    std::string outputFile = "data/simulated_events.root";
    
    if (argc > 1) nEvents = std::atoi(argv[1]);
    if (argc > 2) outputFile = argv[2];
    
    std::cout << "Generating " << nEvents << " simulated events..." << std::endl;
    std::cout << "Output file: " << outputFile << std::endl;
    
    // Create output file
    TFile* file = new TFile(outputFile.c_str(), "RECREATE");
    TTree* tree = new TTree("events", "Simulated LHC Events");
    
    EventData data;
    tree->Branch("eventNumber", &data.eventNumber, "eventNumber/l");
    tree->Branch("nParticles", &data.nParticles, "nParticles/I");
    tree->Branch("particle_pt", data.particle_pt, "particle_pt[nParticles]/F");
    tree->Branch("particle_eta", data.particle_eta, "particle_eta[nParticles]/F");
    tree->Branch("particle_phi", data.particle_phi, "particle_phi[nParticles]/F");
    tree->Branch("particle_mass", data.particle_mass, "particle_mass[nParticles]/F");
    tree->Branch("particle_type", data.particle_type, "particle_type[nParticles]/I");
    tree->Branch("particle_charge", data.particle_charge, "particle_charge[nParticles]/I");
    tree->Branch("particle_isBTagged", data.particle_isBTagged, "particle_isBTagged[nParticles]/O");
    
    EventGenerator generator;
    
    // Event composition: 30% Z, 40% ttbar, 30% background
    for (int i = 0; i < nEvents; ++i) {
        double rand = static_cast<double>(i) / nEvents; // Deterministic split
        
        if (rand < 0.30) {
            generator.generateZEvent(data, i);
        } else if (rand < 0.70) {
            generator.generateTopEvent(data, i);
        } else {
            generator.generateBackgroundEvent(data, i);
        }
        
        tree->Fill();
        
        if ((i + 1) % 10000 == 0) {
            std::cout << "  Generated " << (i + 1) << " events" << std::endl;
        }
    }
    
    tree->Write();
    file->Close();
    
    std::cout << "Event generation complete!" << std::endl;
    std::cout << "File written: " << outputFile << std::endl;
    
    delete file;
    return 0;
}