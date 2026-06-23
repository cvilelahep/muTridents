// src/RunAction.cc

#include "RunAction.hh"
#include "AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include <iostream>

void RunAction::BeginOfRunAction(const G4Run* run)
{
    auto* am = AnalysisManager::Instance();
    am->BookHistograms();
    am->OpenFile("muon_trident_output");

    G4cout << "\n[RunAction] Starting run " << run->GetRunID()
           << " with " << run->GetNumberOfEventToBeProcessed()
           << " events.\n" << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
    AnalysisManager::Instance()->CloseFile();

    G4int nEvents   = run->GetNumberOfEvent();
    G4int nTridents = const_cast<G4Run*>(run)->GetNumberOfEventToBeProcessed();
    (void)nTridents;

    G4cout << "\n════════════════════════════════════════════════\n"
           << "  Run " << run->GetRunID() << " complete.\n"
           << "  Events processed : " << nEvents << "\n"
           << "  Output written   : muon_trident_output.csv\n"
           << "════════════════════════════════════════════════\n"
           << G4endl;
}
