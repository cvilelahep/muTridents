#pragma once
// include/DetectorConstruction.hh
// A thin carbon slab in which muon-trident production takes place.

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    DetectorConstruction()  = default;
    ~DetectorConstruction() = default;

    G4VPhysicalVolume* Construct() override;

    void ConstructSDandField() override;    

    // Accessors (used by SteppingAction)
    G4LogicalVolume* GetTargetLogical() const { return fTargetLogical; }

private:
    G4LogicalVolume* fTargetLogical = nullptr;
};
