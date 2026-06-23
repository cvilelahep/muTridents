#pragma once
// include/SteppingAction.hh
// Inspects each step to identify MuPairProduction secondaries and
// track the surviving muon's energy.

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"

class SteppingAction : public G4UserSteppingAction
{
public:
    SteppingAction()  = default;
    ~SteppingAction() override = default;

    void UserSteppingAction(const G4Step* step) override;
};
