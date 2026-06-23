#include "SteppingAction.hh"
#include "AnalysisManager.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTypes.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    auto& rec = AnalysisManager::Instance()->CurrentRecord();

    G4Track* track = step->GetTrack();
    if (track->GetTrackID() == 1){
    
        if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "biasWrapper(muToMuonPairProd)") track->SetTrackStatus(fStopAndKill);

        rec.scattered = {track->GetParticleDefinition()->GetPDGEncoding(),
                         track->GetMomentum().x(),
                         track->GetMomentum().y(),
                         track->GetMomentum().z()};

    } else if ((track->GetParentID() == 1) and (track->GetCreatorProcess()->GetProcessName() == "biasWrapper(muToMuonPairProd)")){
        
        track->SetTrackStatus(fStopAndKill);

        if (track->GetParticleDefinition()->GetPDGEncoding() > 0){
            rec.pairMinus = {track->GetParticleDefinition()->GetPDGEncoding(),
                            track->GetMomentum().x(),
                            track->GetMomentum().y(),
                            track->GetMomentum().z()};
        } else {
            rec.pairPlus = {track->GetParticleDefinition()->GetPDGEncoding(),
                            track->GetMomentum().x(),
                            track->GetMomentum().y(),
                            track->GetMomentum().z()};
        }
    }   
}

