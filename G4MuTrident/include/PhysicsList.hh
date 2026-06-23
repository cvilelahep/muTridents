#pragma once
// include/PhysicsList.hh
// Minimal physics list centred on G4MuPairProduction.

#include "G4VModularPhysicsList.hh"

class PhysicsList : public G4VModularPhysicsList
{
public:
    PhysicsList();
    ~PhysicsList() override = default;

    void SetCuts() override;
};
