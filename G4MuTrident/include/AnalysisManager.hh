#pragma once
// include/AnalysisManager.hh
// Singleton that owns the G4AnalysisManager (ROOT/CSV output) and the
// per-event trident bookkeeping structure.

#include "G4AnalysisManager.hh"
#include "G4ThreeVector.hh"
#include <vector>

// ── Per-event trident candidate record ───────────────────────────────────────
struct ParticleRecord {
    int pdg = 0;
    double px = 0.;
    double py = 0.;
    double pz = 0.;
};
struct TridentRecord {
    ParticleRecord incoming = {0, 0., 0., 0.};
    ParticleRecord scattered = {0, 0., 0., 0.};
    ParticleRecord pairPlus = {0, 0., 0., 0.};
    ParticleRecord pairMinus = {0, 0., 0., 0.};
};

// ─────────────────────────────────────────────────────────────────────────────
class AnalysisManager
{
public:
    static AnalysisManager* Instance();

    void BookHistograms();
    void OpenFile(const G4String& fname);
    void CloseFile();
    void FillEvent(const TridentRecord& rec);

    TridentRecord& CurrentRecord() { return fRecord; }
    void           ResetRecord()   { fRecord = TridentRecord{}; }

private:
    AnalysisManager();
    static AnalysisManager* fInstance;

    TridentRecord fRecord;

};
