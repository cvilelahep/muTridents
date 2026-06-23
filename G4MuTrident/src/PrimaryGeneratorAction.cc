// src/PrimaryGeneratorAction.cc
// 100 GeV μ- gun pointing along +Z, starting just upstream of the target.

#include "PrimaryGeneratorAction.hh"

#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
    fParticleGun = std::make_unique<G4ParticleGun>(1);   // 1 primary per event

    G4ParticleDefinition* muon =
        G4ParticleTable::GetParticleTable()->FindParticle("mu-");

    fParticleGun->SetParticleDefinition(muon);

    // Direction: along beam axis (+Z)
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));

    // 100 GeV kinetic energy
    // (muon rest mass = 105.66 MeV → effectively ultra-relativistic)
    fParticleGun->SetParticleEnergy(100. * GeV);

    // Start 1 mm upstream of the target face (target spans -41.5 → +41.5 cm)
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -41.6 * cm));
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    fParticleGun->GeneratePrimaryVertex(event);
}
