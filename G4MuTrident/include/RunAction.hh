#pragma once
// include/RunAction.hh

#include "G4UserRunAction.hh"
#include "G4Run.hh"

class RunAction : public G4UserRunAction
{
public:
    RunAction()  = default;
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run* run) override;
    void EndOfRunAction  (const G4Run* run) override;
};
