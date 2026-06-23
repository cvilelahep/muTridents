#pragma once
// include/PrimaryGeneratorAction.hh
// Fires a single 100 GeV mu- along +Z from upstream of the carbon target.

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include <memory>

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override = default;

    void GeneratePrimaries(G4Event* event) override;

private:
    std::unique_ptr<G4ParticleGun> fParticleGun;
};
