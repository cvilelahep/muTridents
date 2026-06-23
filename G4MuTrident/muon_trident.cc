// muon_trident.cc
// Main driver for muon-trident (μ→μ e+e-) simulation via G4MuPairProduction
//
// Physics channel:  μ + Z  →  μ + e+ + e-  (+ nuclear recoil)
// The pair is created by the virtual-photon exchange in G4MuPairProduction.
// ─────────────────────────────────────────────────────────────────────────────

#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"

#include "G4GenericBiasingPhysics.hh"

#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "RunAction.hh"
#include "AnalysisManager.hh"

int main(int argc, char** argv)
{
    // ── Random engine ────────────────────────────────────────────────────────
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    G4Random::setTheSeed(12345);

    // ── Run manager ──────────────────────────────────────────────────────────
    auto* runManager =
        G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);

    // ── Mandatory initialisation classes ─────────────────────────────────────
    runManager->SetUserInitialization(new DetectorConstruction());
    auto physicsList = new PhysicsList();

    auto biasingPhysics = new G4GenericBiasingPhysics();
    biasingPhysics->Bias("mu+");
    biasingPhysics->Bias("mu-");
    physicsList->RegisterPhysics(biasingPhysics);

    runManager->SetUserInitialization(physicsList);

    // ── Analysis manager (singleton) ─────────────────────────────────────────
    AnalysisManager::Instance();

    // ── User actions ─────────────────────────────────────────────────────────
    runManager->SetUserAction(new PrimaryGeneratorAction());
    runManager->SetUserAction(new RunAction());
    runManager->SetUserAction(new EventAction());
    runManager->SetUserAction(new SteppingAction());

    // ── Initialise G4 kernel ─────────────────────────────────────────────────
    runManager->Initialize();

    // ── UI / visualisation ───────────────────────────────────────────────────
    G4UIExecutive*  ui  = nullptr;
    G4VisManager*   vis = nullptr;

    if (argc == 1) {
        // Interactive mode
        ui  = new G4UIExecutive(argc, argv);
        vis = new G4VisExecutive("Quiet");
        vis->Initialize();
    }

    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    if (argc > 1) {
        // Batch mode: execute the macro given on the command line
        G4String macroFile = argv[1];
        UImanager->ApplyCommand("/control/execute " + macroFile);
    } else {
        UImanager->ApplyCommand("/control/execute run.mac");
        ui->SessionStart();
        delete ui;
    }

    delete vis;
    delete runManager;
    return 0;
}
