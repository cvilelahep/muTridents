
#ifndef MuTridentXSbias_hh
#define MuTridentXSbias_hh 1

#include "G4VBiasingOperator.hh"
class G4BOptnChangeCrossSection;
class G4ParticleDefinition;
#include <map>

class MuTridentXSbias : public G4VBiasingOperator
{
  public:
    // ------------------------------------------------------------
    // -- Constructor: takes the name of the particle type to bias:
    // ------------------------------------------------------------
    MuTridentXSbias(G4String particleToBias);
    ~MuTridentXSbias() override;

    // -- method called at beginning of run:
    void StartRun() override;

  private:
    // -----------------------------
    // -- Mandatory from base class:
    // -----------------------------
    // -- This method returns the biasing operation that will bias the physics process occurence.
    G4VBiasingOperation*
    ProposeOccurenceBiasingOperation(const G4Track* track,
                                     const G4BiasingProcessInterface* callingProcess) override;
    // -- Methods not used:
    G4VBiasingOperation*
    ProposeFinalStateBiasingOperation(const G4Track*, const G4BiasingProcessInterface*) override
    {
      return nullptr;
    }
    G4VBiasingOperation*
    ProposeNonPhysicsBiasingOperation(const G4Track*, const G4BiasingProcessInterface*) override
    {
      return nullptr;
    }

  private:
    // -- ("using" is avoid compiler complaining against (false) method shadowing.)
    using G4VBiasingOperator::OperationApplied;

    // -- Optionnal base class method implementation.
    // -- This method is called to inform the operator that a proposed operation has been applied.
    // -- In the present case, it means that a physical interaction occured (interaction at
    // -- PostStepDoIt level):
    void OperationApplied(const G4BiasingProcessInterface* callingProcess,
                          G4BiasingAppliedCase biasingCase,
                          G4VBiasingOperation* occurenceOperationApplied,
                          G4double weightForOccurenceInteraction,
                          G4VBiasingOperation* finalStateOperationApplied,
                          const G4VParticleChange* particleChangeProduced) override;

  private:
    // -- List of associations between processes and biasing operations:
    std::map<const G4BiasingProcessInterface*, G4BOptnChangeCrossSection*>
      fChangeCrossSectionOperations;
    G4bool fSetup;
    const G4ParticleDefinition* fParticleToBias;
};

#endif
