// src/DetectorConstruction.cc
// Geometry: world box  →  carbon target slab (20 X0 ≈ 4.27 cm × 20 = 85.4 cm)
// Using a thick target maximises trident yield per primary muon.

#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4LogicalVolumeStore.hh"

#include "MuPlusMuMinusBias.hh"

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4NistManager* nist = G4NistManager::Instance();

    // ── Materials ────────────────────────────────────────────────────────────
    G4Material* air    = nist->FindOrBuildMaterial("G4_AIR");
    G4Material* carbon = nist->FindOrBuildMaterial("G4_C");   // graphite

    // ── World ────────────────────────────────────────────────────────────────
    const G4double worldHalf = 2.0 * m;
    auto* worldSolid = new G4Box("World", worldHalf, worldHalf, worldHalf);
    auto* worldLogic = new G4LogicalVolume(worldSolid, air, "World");
    worldLogic->SetVisAttributes(G4VisAttributes::GetInvisible());

    auto* worldPhys = new G4PVPlacement(
        nullptr, G4ThreeVector(), worldLogic, "World", nullptr, false, 0, true);

    // ── Carbon target ────────────────────────────────────────────────────────
    // Carbon radiation length X0 = 18.8 g/cm² / 2.265 g/cm³ ≈ 8.3 cm
    // We use 10 X0 ≈ 83 cm so the muon has a good chance to radiate.
    const G4double targetHalfZ  = 41.5 * cm;   // 10 X0 in Z
    const G4double targetHalfXY = 20.0 * cm;

    auto* targetSolid = new G4Box("Target", targetHalfXY, targetHalfXY, targetHalfZ);
    fTargetLogical     = new G4LogicalVolume(targetSolid, carbon, "Target");

    G4VisAttributes* targetVis = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.6));
    targetVis->SetForceSolid(true);
    fTargetLogical->SetVisAttributes(targetVis);

    new G4PVPlacement(
        nullptr,
        G4ThreeVector(0, 0, 0),   // centred at origin
        fTargetLogical, "Target",
        worldLogic, false, 0, true);

    return worldPhys;
}

void DetectorConstruction::ConstructSDandField()
{
    // -- Fetch volume for biasing:
    G4LogicalVolume* logicTarget = G4LogicalVolumeStore::GetInstance()->GetVolume("Target");

    auto tridentBias = new MuPlusMuMinusBias();
    tridentBias->AddParticles();
    tridentBias->AttachTo(logicTarget);
}

