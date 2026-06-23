#pragma once
// include/EventAction.hh

#include "G4UserEventAction.hh"
#include "G4Event.hh"

class EventAction : public G4UserEventAction
{
public:
    EventAction()  = default;
    ~EventAction() override = default;

    void BeginOfEventAction(const G4Event* event) override;
    void EndOfEventAction  (const G4Event* event) override;
};
