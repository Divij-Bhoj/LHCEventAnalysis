#include "Particle.h"
#include <iostream>
#include <cmath>

Particle::Particle() 
    : px_(0), py_(0), pz_(0), e_(0), 
      type_(Type::UNKNOWN), charge_(0), isBTagged_(false) {}

Particle::Particle(double px, double py, double pz, double e, Type type)
    : px_(px), py_(py), pz_(pz), e_(e), 
      type_(type), charge_(0), isBTagged_(false) {}

Particle Particle::fromPtEtaPhiM(double pt, double eta, double phi, double mass, Type type) {
    Particle p;
    p.type_ = type;
    p.charge_ = 0;
    p.isBTagged_ = false;
    p.px_ = pt * std::cos(phi);
    p.py_ = pt * std::sin(phi);
    p.pz_ = pt * std::sinh(eta);
    p.e_ = std::sqrt(mass*mass + pt*pt*(1 + std::sinh(eta)*std::sinh(eta)));
    return p;
}

double Particle::pt() const {
    return std::sqrt(px_*px_ + py_*py_);
}

double Particle::eta() const {
    double p = std::sqrt(px_*px_ + py_*py_ + pz_*pz_);
    if (p == 0) return 0;
    return 0.5 * std::log((p + pz_) / (p - pz_));
}

double Particle::phi() const {
    return std::atan2(py_, px_);
}

double Particle::mass() const {
    double m2 = e_*e_ - px_*px_ - py_*py_ - pz_*pz_;
    return (m2 >= 0) ? std::sqrt(m2) : 0;
}

double Particle::rapidity() const {
    return 0.5 * std::log((e_ + pz_) / (e_ - pz_));
}

double Particle::deltaR(const Particle& other) const {
    double deta = eta() - other.eta();
    double dphi = phi() - other.phi();
    
    // Wrap phi to [-pi, pi]
    while (dphi > M_PI) dphi -= 2*M_PI;
    while (dphi < -M_PI) dphi += 2*M_PI;
    
    return std::sqrt(deta*deta + dphi*dphi);
}

Particle Particle::operator+(const Particle& other) const {
    return Particle(
        px_ + other.px_,
        py_ + other.py_,
        pz_ + other.pz_,
        e_ + other.e_,
        Type::UNKNOWN
    );
}

void Particle::print() const {
    std::cout << "Particle: pt=" << pt() 
              << " eta=" << eta() 
              << " phi=" << phi() 
              << " mass=" << mass() 
              << " E=" << e_ << std::endl;
}