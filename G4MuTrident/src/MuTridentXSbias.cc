#include "MuTridentXSbias.hh"

#include "G4BOptnChangeCrossSection.hh"
#include "G4BiasingProcessInterface.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4VProcess.hh"

MuTridentXSbias::MuTridentXSbias(G4String particleToBias): G4VBiasingOperator("muTridentBias"), fSetup(true)
{
    fParticleToBias = G4ParticleTable::GetParticleTable()->FindParticle(particleToBias);

}
MuTridentXSbias::~MuTridentXSbias(){
    for (auto it = fChangeCrossSectionOperations.begin(); it != fChangeCrossSectionOperations.end();
           it++)
        delete (*it).second;

}
void MuTridentXSbias::StartRun(){
    if (fSetup){
        const G4ProcessManager* processManager = fParticleToBias->GetProcessManager();
        const G4BiasingProcessSharedData* sharedData = G4BiasingProcessInterface::GetSharedData(processManager);
        if (sharedData){
            for (size_t i = 0; i < (sharedData->GetPhysicsBiasingProcessInterfaces()).size(); i++) {
                const G4BiasingProcessInterface* wrapperProcess = (sharedData->GetPhysicsBiasingProcessInterfaces())[i];
                if (wrapperProcess->GetWrappedProcess()->GetProcessName() != "muToMuonPairProd") continue;
                G4String operationName = "XSchange-" + wrapperProcess->GetWrappedProcess()->GetProcessName();
                fChangeCrossSectionOperations[wrapperProcess] = new G4BOptnChangeCrossSection(operationName);
            }
        } 
        fSetup = false;
    }
}

G4VBiasingOperation* MuTridentXSbias::ProposeOccurenceBiasingOperation(
  const G4Track* track, const G4BiasingProcessInterface* callingProcess)
{

    if (fChangeCrossSectionOperations[callingProcess] == nullptr) return nullptr;
    
    if (track->GetDefinition() != fParticleToBias) return nullptr;
    G4double analogInteractionLength = callingProcess->GetWrappedProcess()->GetCurrentInteractionLength();
    if (analogInteractionLength > DBL_MAX / 10.) return nullptr;
    G4double analogXS = 1. / analogInteractionLength;
    G4double XStransformation = 1.e20;

    G4BOptnChangeCrossSection* operation = fChangeCrossSectionOperations[callingProcess];

    G4VBiasingOperation* previousOperation = callingProcess->GetPreviousOccurenceBiasingOperation();

    if (previousOperation == nullptr) {
        operation->SetBiasedCrossSection(XStransformation * analogXS);
        operation->Sample();
    } else {
        if (previousOperation != operation) {
        // -- should not happen !
        G4ExceptionDescription ed;
        ed << " Logic problem in operation handling !" << G4endl;
        G4Exception("GB01BOptrChangeCrossSection::ProposeOccurenceBiasingOperation(...)", "exGB01.02", JustWarning, ed);
        return nullptr;
    }
    
    if (operation->GetInteractionOccured()) {
      operation->SetBiasedCrossSection(XStransformation * analogXS);
      operation->Sample();
    } else {
      operation->UpdateForStep(callingProcess->GetPreviousStepSize());
      operation->SetBiasedCrossSection(XStransformation * analogXS);
      operation->UpdateForStep(0.0);
    }
  }
  return operation;
}
void MuTridentXSbias::OperationApplied(const G4BiasingProcessInterface* callingProcess,
                                                   G4BiasingAppliedCase,
                                                   G4VBiasingOperation* occurenceOperationApplied,
                                                   G4double, G4VBiasingOperation*,
                                                   const G4VParticleChange*)
{
  G4BOptnChangeCrossSection* operation = fChangeCrossSectionOperations[callingProcess];
  if (operation == occurenceOperationApplied) operation->SetInteractionOccured();
}

