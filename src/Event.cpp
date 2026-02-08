#include "Event.h"
#include <iostream>
#include <cmath>

Event::Event() 
    : eventNumber_(0), metCalculated_(false), met_(0), metPhi_(0) {}

Event::Event(unsigned long eventNumber)
    : eventNumber_(eventNumber), metCalculated_(false), met_(0), metPhi_(0) {}

void Event::addParticle(const Particle& particle) {
    particles_.push_back(particle);
    metCalculated_ = false; // Invalidate MET cache
}

void Event::clear() {
    particles_.clear();
    metCalculated_ = false;
}

std::vector<Particle> Event::getMuons() const {
    std::vector<Particle> muons;
    for (const auto& p : particles_) {
        if (p.type() == Particle::Type::MUON) {
            muons.push_back(p);
        }
    }
    return muons;
}

std::vector<Particle> Event::getElectrons() const {
    std::vector<Particle> electrons;
    for (const auto& p : particles_) {
        if (p.type() == Particle::Type::ELECTRON) {
            electrons.push_back(p);
        }
    }
    return electrons;
}

std::vector<Particle> Event::getJets() const {
    std::vector<Particle> jets;
    for (const auto& p : particles_) {
        if (p.type() == Particle::Type::JET || p.type() == Particle::Type::BJET) {
            jets.push_back(p);
        }
    }
    return jets;
}

std::vector<Particle> Event::getBJets() const {
    std::vector<Particle> bjets;
    for (const auto& p : particles_) {
        if (p.isBTagged()) {
            bjets.push_back(p);
        }
    }
    return bjets;
}

double Event::scalarHT() const {
    double ht = 0;
    for (const auto& p : particles_) {
        if (p.type() == Particle::Type::JET || p.type() == Particle::Type::BJET) {
            ht += p.pt();
        }
    }
    return ht;
}

void Event::calculateMET() const {
    double metX = 0;
    double metY = 0;
    
    for (const auto& p : particles_) {
        // Include all visible particles in MET calculation
        if (p.type() != Particle::Type::NEUTRINO) {
            metX -= p.px();
            metY -= p.py();
        }
    }
    
    met_ = std::sqrt(metX*metX + metY*metY);
    metPhi_ = std::atan2(metY, metX);
    metCalculated_ = true;
}

double Event::missingET() const {
    if (!metCalculated_) {
        calculateMET();
    }
    return met_;
}

double Event::missingETPhi() const {
    if (!metCalculated_) {
        calculateMET();
    }
    return metPhi_;
}

void Event::print() const {
    std::cout << "Event #" << eventNumber_ << ": " 
              << particles_.size() << " particles, "
              << "MET=" << missingET() << " GeV" << std::endl;
}
