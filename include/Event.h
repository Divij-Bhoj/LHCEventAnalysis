#ifndef EVENT_H
#define EVENT_H

#include "Particle.h"
#include <vector>
#include <memory>

/**
 * @brief Container for a single collision event
 * 
 * Stores reconstructed particles from a single proton-proton collision.
 * Provides methods for accessing particles by type and computing event-level quantities.
 */
class Event {
public:
    Event();
    explicit Event(unsigned long eventNumber);

    // Event identification
    unsigned long eventNumber() const { return eventNumber_; }
    void setEventNumber(unsigned long num) { eventNumber_ = num; }

    // Particle management
    void addParticle(const Particle& particle);
    void clear();

    // Accessors by particle type
    std::vector<Particle> getMuons() const;
    std::vector<Particle> getElectrons() const;
    std::vector<Particle> getJets() const;
    std::vector<Particle> getBJets() const;
    
    const std::vector<Particle>& getAllParticles() const { return particles_; }

    // Event-level quantities
    size_t nParticles() const { return particles_.size(); }
    double scalarHT() const;  // Scalar sum of jet pT
    double missingET() const; // Missing transverse energy magnitude
    double missingETPhi() const;

    // Utilities
    void print() const;

private:
    unsigned long eventNumber_;
    std::vector<Particle> particles_;
    
    // Cached MET calculation
    mutable bool metCalculated_;
    mutable double met_;
    mutable double metPhi_;
    
    void calculateMET() const;
};

#endif // EVENT_H
