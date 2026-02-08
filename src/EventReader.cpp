#include "EventReader.h"
#include <iostream>

EventReader::EventReader(const std::string& filename)
    : tree_(nullptr), isValid_(false), currentEntry_(0) {
    
    file_ = std::make_unique<TFile>(filename.c_str(), "READ");
    
    if (!file_ || file_->IsZombie()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }

    tree_ = dynamic_cast<TTree*>(file_->Get("events"));
    if (!tree_) {
        std::cerr << "Error: Cannot find 'events' tree in file" << std::endl;
        return;
    }

    setupBranches();
    isValid_ = true;
}

EventReader::~EventReader() {
    // TFile owns the TTree, so we don't delete it
    if (file_ && file_->IsOpen()) {
        file_->Close();
    }
}

void EventReader::setupBranches() {
    tree_->SetBranchAddress("eventNumber", &eventNumber_);
    tree_->SetBranchAddress("nParticles", &nParticles_);
    tree_->SetBranchAddress("particle_pt", particle_pt_);
    tree_->SetBranchAddress("particle_eta", particle_eta_);
    tree_->SetBranchAddress("particle_phi", particle_phi_);
    tree_->SetBranchAddress("particle_mass", particle_mass_);
    tree_->SetBranchAddress("particle_type", particle_type_);
    tree_->SetBranchAddress("particle_charge", particle_charge_);
    tree_->SetBranchAddress("particle_isBTagged", particle_isBTagged_);
}

bool EventReader::nextEvent(Event& event) {
    if (!isValid_ || currentEntry_ >= static_cast<unsigned long>(tree_->GetEntries())) {
        return false;
    }

    tree_->GetEntry(currentEntry_++);
    fillEvent(event);
    return true;
}

bool EventReader::getEvent(unsigned long index, Event& event) {
    if (!isValid_ || index >= static_cast<unsigned long>(tree_->GetEntries())) {
        return false;
    }

    tree_->GetEntry(index);
    fillEvent(event);
    return true;
}

void EventReader::fillEvent(Event& event) {
    event.clear();
    event.setEventNumber(eventNumber_);

    for (int i = 0; i < nParticles_; ++i) {
        Particle p = Particle::fromPtEtaPhiM(
            particle_pt_[i], 
            particle_eta_[i], 
            particle_phi_[i], 
            particle_mass_[i]
        );
        
        p.setType(static_cast<Particle::Type>(particle_type_[i]));
        p.setCharge(particle_charge_[i]);
        p.setBTagged(particle_isBTagged_[i]);
        
        event.addParticle(p);
    }
}

unsigned long EventReader::nEvents() const {
    if (!isValid_) return 0;
    return tree_->GetEntries();
}

void EventReader::reset() {
    currentEntry_ = 0;
}