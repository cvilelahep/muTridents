# Muon-Trident Geant4 Simulation

Simulates **muon-trident** events — the rare QED process

```
μ + Z  →  μ + e⁺ + e⁻ + Z
```

using Geant4's built-in `G4MuPairProduction` process for a 100 GeV μ⁻
beam traversing a carbon (graphite) target.

## Physics background

At high energies, a muon can radiate a virtual photon that converts to an
electron–positron pair in the Coulomb field of a nucleus. The outgoing state
is therefore the original muon (degraded in energy) plus an e⁺e⁻ pair — a
"muon trident". The cross-section scales roughly as Z² and grows
logarithmically with muon energy.

`G4MuPairProduction` implements the Kelner–Kokoulin–Petrukhin differential
cross-section and handles both nuclear and atomic-electron contributions.

## Project layout

```
muon_trident/
├── CMakeLists.txt
├── build.sh                   # configure + build helper
├── run.mac                    # batch mode (1000 events)
├── vis.mac                    # interactive / OGL visualisation
├── analyze_tridents.py        # post-processing plots
├── muon_trident.cc            # main()
├── include/
│   ├── DetectorConstruction.hh
│   ├── PhysicsList.hh
│   ├── PrimaryGeneratorAction.hh
│   ├── EventAction.hh
│   ├── SteppingAction.hh
│   ├── RunAction.hh
│   └── AnalysisManager.hh
└── src/
    ├── DetectorConstruction.cc   – 10 X₀ carbon slab
    ├── PhysicsList.cc            – μ EM + G4MuPairProduction
    ├── PrimaryGeneratorAction.cc – 100 GeV μ- gun
    ├── EventAction.cc            – per-event bookkeeping
    ├── SteppingAction.cc         – trident vertex identification
    ├── RunAction.cc              – file open/close
    └── AnalysisManager.cc        – histograms + ntuple (CSV)
```

## Requirements

| Package | Version |
|---------|---------|
| Geant4  | ≥ 10.7  |
| CMake   | ≥ 3.16  |
| Python  | ≥ 3.8 (for analysis script) |
| matplotlib / pandas | optional, for plots |

## Build

```bash
# Source Geant4 environment
source /path/to/geant4-install/bin/geant4.sh

# Build
bash build.sh
cd build
```

## Run

**Batch mode (1000 events):**
```bash
./muon_trident run.mac
```

**Interactive / visualisation (requires OpenGL):**
```bash
./muon_trident
```

**Change energy or statistics in run.mac:**
```
/run/beamOn 5000
```

## Output

The simulation writes CSV files via `G4AnalysisManager`:

| File | Contents |
|------|----------|
| `muon_trident_output_h1_EpairKE.csv` | e⁺e⁻ pair total KE histogram |
| `muon_trident_output_h1_EmuFinal.csv` | surviving muon KE histogram |
| `muon_trident_output_h1_InvMass.csv` | e⁺e⁻ invariant mass histogram |
| `muon_trident_output_h1_Npairs.csv` | pairs per event histogram |
| `ntuple/muon_trident_output_t0_trident.csv` | per-event ntuple |

## Analysis

```bash
python3 ../analyze_tridents.py ntuple/
```

Produces `trident_analysis.png` with four panels:
- e⁺e⁻ pair kinetic energy distribution
- Surviving muon kinetic energy (energy loss spectrum)
- e⁺e⁻ invariant mass (peaked just above 2mₑ ≈ 1.02 MeV)
- Pair KE vs. muon energy loss scatter

## Key Geant4 classes

| Class | Role |
|-------|------|
| `G4MuPairProduction` | Core trident process (discrete EM) |
| `G4MuPairProductionModel` | Kelner–Kokoulin–Petrukhin differential XS |
| `G4MuIonisation` | Continuous muon energy loss |
| `G4MuBremsstrahlung` | Competing radiative process |

## Expected yield

At 100 GeV in 10 radiation lengths of carbon the probability of at least
one pair-production interaction is of order a few percent, so running
1000 events should give ~10–50 trident events depending on the exact cut
values and target thickness.

## Extending the simulation

- **μ-pair tridents** (μ⁺μ⁻ final state) become accessible above
  ~450 MeV/c² and require higher muon energies; increase `/gun/energy`.
- **Bethe–Heitler pair production** from bremsstrahlung photons is also
  included via `G4GammaConversion` and can be separated by checking the
  secondary's creator process name (`conv` vs `muPairProd`).
- Switch to a **reference physics list** (`FTFP_BERT_EMZ`) for more
  complete hadronic physics if studying nuclear recoil or hadronic backgrounds.
