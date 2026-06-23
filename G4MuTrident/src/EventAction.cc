// src/EventAction.cc
// Resets the per-event trident record at the start of each event,
// and triggers histogram filling and optional printout at the end.

#include "EventAction.hh"
#include "AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
    AnalysisManager::Instance()->ResetRecord();
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    std::cout << "END OF EVENT" << std::endl;
    auto& rec = AnalysisManager::Instance()->CurrentRecord();
    std::cout << "Scattered muon:  " << rec.scattered.pdg << " " << rec.scattered.px << " " << rec.scattered.py << " " << rec.scattered.pz << std::endl;
    std::cout << "Pair -        :  " << rec.pairMinus.pdg << " " << rec.pairMinus.px << " " << rec.pairMinus.py << " " << rec.pairMinus.pz << std::endl;
    std::cout << "Pair +        :  " << rec.pairPlus.pdg << " " << rec.pairPlus.px << " " << rec.pairPlus.py << " " << rec.pairPlus.pz << std::endl;
    AnalysisManager::Instance()->FillEvent(rec); 
}
