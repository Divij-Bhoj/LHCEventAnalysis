#ifndef EVENT_READER_H
#define EVENT_READER_H

#include "Event.h"
#include <string>
#include <memory>
#include <TFile.h>
#include <TTree.h>

/**
 * @brief Reads events from ROOT files
 * 
 * Handles ROOT I/O for event data stored in TTree format.
 * Supports batch reading for performance-critical applications.
 */
class EventReader {
public:
    explicit EventReader(const std::string& filename);
    ~EventReader();

    // Prevent copying (ROOT objects are not copyable)
    EventReader(const EventReader&) = delete;
    EventReader& operator=(const EventReader&) = delete;

    // Event access
    bool nextEvent(Event& event);
    bool getEvent(unsigned long index, Event& event);
    
    // File information
    unsigned long nEvents() const;
    bool isValid() const { return isValid_; }
    void reset();

private:
    std::unique_ptr<TFile> file_;
    TTree* tree_;
    bool isValid_;
    unsigned long currentEntry_;

    // Branch variables (matching GenerateEvents.cpp structure)
    ULong64_t eventNumber_;
    Int_t nParticles_;
    static const int kMaxParticles = 100;
    Float_t particle_pt_[kMaxParticles];
    Float_t particle_eta_[kMaxParticles];
    Float_t particle_phi_[kMaxParticles];
    Float_t particle_mass_[kMaxParticles];
    Int_t particle_type_[kMaxParticles];
    Int_t particle_charge_[kMaxParticles];
    Bool_t particle_isBTagged_[kMaxParticles];

    void setupBranches();
    void fillEvent(Event& event);
};

#endif // EVENT_READER_H
