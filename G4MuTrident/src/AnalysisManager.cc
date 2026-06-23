// src/AnalysisManager.cc

#include "AnalysisManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <cmath>

AnalysisManager* AnalysisManager::fInstance = nullptr;

AnalysisManager* AnalysisManager::Instance()
{
    if (!fInstance) fInstance = new AnalysisManager();
    return fInstance;
}

AnalysisManager::AnalysisManager()
{
    auto* am = G4AnalysisManager::Instance();
    am->SetDefaultFileType("xml");
    am->SetVerboseLevel(1);
    //am->SetNtupleDirectoryName("ntuple");
}

void AnalysisManager::BookHistograms()
{
    auto* am = G4AnalysisManager::Instance();
    am->CreateNtuple("MuTridentGeant4", "Track momenta");
    // Incoming muon
    am->CreateNtupleDColumn("inc_pdg");
    am->CreateNtupleDColumn("inc_px");
    am->CreateNtupleDColumn("inc_py");
    am->CreateNtupleDColumn("inc_pz");
    // Scattered muon
    am->CreateNtupleDColumn("scat_pdg");
    am->CreateNtupleDColumn("scat_px");
    am->CreateNtupleDColumn("scat_py");
    am->CreateNtupleDColumn("scat_pz");
    // Negative charge muon from pair
    am->CreateNtupleDColumn("pair_neg_pdg");
    am->CreateNtupleDColumn("pair_neg_px");
    am->CreateNtupleDColumn("pair_neg_py");
    am->CreateNtupleDColumn("pair_neg_pz");
    // Positive charge muon from pair
    am->CreateNtupleDColumn("pair_pos_pdg");
    am->CreateNtupleDColumn("pair_pos_px");
    am->CreateNtupleDColumn("pair_pos_py");
    am->CreateNtupleDColumn("pair_pos_pz");

    am->FinishNtuple();
}


void AnalysisManager::OpenFile(const G4String& fname)
{
    G4AnalysisManager::Instance()->OpenFile(fname);
}

void AnalysisManager::CloseFile()
{
    auto* am = G4AnalysisManager::Instance();
    am->Write();
    am->CloseFile();
}

void AnalysisManager::FillEvent(const TridentRecord& rec)
{
    auto* am = G4AnalysisManager::Instance();
    am->FillNtupleDColumn(0,  rec.incoming.pdg);
    am->FillNtupleDColumn(1,  rec.incoming.px / GeV);
    am->FillNtupleDColumn(2,  rec.incoming.py / GeV);
    am->FillNtupleDColumn(3,  rec.incoming.pz / GeV);
    am->FillNtupleDColumn(4,  rec.scattered.pdg);
    am->FillNtupleDColumn(5,  rec.scattered.px / GeV);
    am->FillNtupleDColumn(6,  rec.scattered.py / GeV);
    am->FillNtupleDColumn(7,  rec.scattered.pz / GeV);
    am->FillNtupleDColumn(8,  rec.pairMinus.pdg);
    am->FillNtupleDColumn(9,  rec.pairMinus.px / GeV);
    am->FillNtupleDColumn(10, rec.pairMinus.py / GeV);
    am->FillNtupleDColumn(11, rec.pairMinus.pz / GeV);
    am->FillNtupleDColumn(12, rec.pairPlus.pdg);
    am->FillNtupleDColumn(13, rec.pairPlus.px / GeV);
    am->FillNtupleDColumn(14, rec.pairPlus.py / GeV);
    am->FillNtupleDColumn(15, rec.pairPlus.pz / GeV);
    am->AddNtupleRow();
}
