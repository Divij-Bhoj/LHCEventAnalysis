#ifndef PARTICLE_H
#define PARTICLE_H

#include <cmath>

/**
 * @brief Represents a particle with 4-momentum and particle type
 * 
 * Lightweight class for representing particles in HEP analyses.
 * Uses standard (E, px, py, pz) representation with derived quantities.
 */
class Particle {
public:
    // Particle types
    enum class Type {
        MUON,
        ELECTRON,
        JET,
        BJET,
        PHOTON,
        NEUTRINO,
        UNKNOWN
    };

    // Constructors
    Particle();
    Particle(double px, double py, double pz, double e, Type type = Type::UNKNOWN);
    
    // Named constructor for pt-eta-phi-mass (static factory method)
    static Particle fromPtEtaPhiM(double pt, double eta, double phi, double mass, Type type = Type::UNKNOWN);

    // 4-momentum accessors
    double px() const { return px_; }
    double py() const { return py_; }
    double pz() const { return pz_; }
    double e() const { return e_; }

    // Derived kinematic quantities
    double pt() const;
    double eta() const;
    double phi() const;
    double mass() const;
    double rapidity() const;

    // Particle properties
    Type type() const { return type_; }
    int charge() const { return charge_; }
    bool isBTagged() const { return isBTagged_; }
    
    void setCharge(int charge) { charge_ = charge; }
    void setBTagged(bool tagged) { isBTagged_ = tagged; }
    void setType(Type type) { type_ = type; }

    // Physics operations
    double deltaR(const Particle& other) const;
    Particle operator+(const Particle& other) const;

    // Utilities
    void print() const;

private:
    double px_, py_, pz_, e_;
    Type type_;
    int charge_;
    bool isBTagged_;
};

#endif // PARTICLE_H