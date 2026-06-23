#include "MuPlusMuMinusBias.hh"

#include "MuTridentXSbias.hh"

#include "G4BiasingProcessInterface.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MuPlusMuMinusBias::MuPlusMuMinusBias()
  : G4VBiasingOperator("MuPlusMuMinusTridentBias")
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MuPlusMuMinusBias::AddParticles()
{
    const G4ParticleDefinition* muPlus = G4ParticleTable::GetParticleTable()->FindParticle("mu+");
    auto optrMuPlus = new MuTridentXSbias("mu+");
    fParticlesToBias.push_back(muPlus);
    fBOptrForParticle[muPlus] = optrMuPlus;

    const G4ParticleDefinition* muMinus = G4ParticleTable::GetParticleTable()->FindParticle("mu-");
    auto optrMuMinus = new MuTridentXSbias("mu-");
    fParticlesToBias.push_back(muMinus);
    fBOptrForParticle[muMinus] = optrMuMinus;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VBiasingOperation* MuPlusMuMinusBias::ProposeOccurenceBiasingOperation(
  const G4Track* track, const G4BiasingProcessInterface* callingProcess)
{
  // -- limit application of biasing to primary particles only:
  if (track->GetParentID() != 0) return nullptr;

  if (fCurrentOperator)
    return fCurrentOperator->GetProposedOccurenceBiasingOperation(track, callingProcess);
  else
    return nullptr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MuPlusMuMinusBias::StartTracking(const G4Track* track)
{
  // -- fetch the underneath biasing operator, if any, for the current particle type:
  const G4ParticleDefinition* definition = track->GetParticleDefinition();
  
  auto it = fBOptrForParticle.find(definition);
  fCurrentOperator = nullptr;
  if (it != fBOptrForParticle.end()) fCurrentOperator = (*it).second;

  // -- reset count for number of biased interactions:
  fnInteractions = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MuPlusMuMinusBias::OperationApplied(
  const G4BiasingProcessInterface* callingProcess, G4BiasingAppliedCase biasingCase,
  G4VBiasingOperation* occurenceOperationApplied, G4double weightForOccurenceInteraction,
  G4VBiasingOperation* finalStateOperationApplied, const G4VParticleChange* particleChangeProduced)
{
  // -- count number of biased interactions:
  fnInteractions++;

  // -- inform the underneath biasing operator that a biased interaction occured:
  if (fCurrentOperator)
    fCurrentOperator->ReportOperationApplied(callingProcess, biasingCase, occurenceOperationApplied,
                                             weightForOccurenceInteraction,
                                             finalStateOperationApplied, particleChangeProduced);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
