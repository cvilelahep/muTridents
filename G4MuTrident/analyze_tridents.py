#!/usr/bin/env python3
"""
analyze_tridents.py
────────────────────
Post-processing for the muon-trident Geant4 simulation.
Reads the CSV ntuple produced by G4AnalysisManager and produces
physics plots.

Usage
─────
    python3 analyze_tridents.py [ntuple_dir]

Output CSV columns (from AnalysisManager)
──────────────────────────────────────────
    eEplus    – positron total KE [GeV]
    eEminus   – electron total KE [GeV]
    eMuFinal  – surviving muon KE [GeV]
    invMass   – e+e- invariant mass [MeV/c²]
    nEplus    – number of positrons produced
    nEminus   – number of electrons produced
"""

import sys
import os
import glob
import numpy as np

try:
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    HAS_MPL = True
except ImportError:
    print("matplotlib not found – skipping plots (install with: pip install matplotlib)")
    HAS_MPL = False

try:
    import pandas as pd
    HAS_PD = True
except ImportError:
    HAS_PD = False

# ── Locate CSV ntuple ─────────────────────────────────────────────────────────
search_dirs = [sys.argv[1]] if len(sys.argv) > 1 else [".", "ntuple", "build/ntuple"]
csv_files = []
for d in search_dirs:
    csv_files += glob.glob(os.path.join(d, "*trident*.csv"))

if not csv_files:
    print("No trident ntuple CSV found. Run the simulation first.")
    print("Looking for files matching: *trident*.csv")
    sys.exit(1)

csv_file = csv_files[0]
print(f"Reading: {csv_file}")

# ── Load data ─────────────────────────────────────────────────────────────────
if HAS_PD:
    df = pd.read_csv(csv_file, comment="#")
    # G4AnalysisManager writes a header row
    if df.columns[0].startswith("#"):
        df.columns = ["eEplus", "eEminus", "eMuFinal", "invMass", "nEplus", "nEminus"]
    eEplus   = df["eEplus"].values
    eEminus  = df["eEminus"].values
    eMuFinal = df["eMuFinal"].values
    invMass  = df["invMass"].values
else:
    data = np.genfromtxt(csv_file, delimiter=",", comments="#")
    eEplus, eEminus, eMuFinal, invMass = data[:,0], data[:,1], data[:,2], data[:,3]

ePair = eEplus + eEminus
nEvents = len(eMuFinal)
print(f"\nTrident events loaded: {nEvents}")
print(f"  e+e- pair KE  : mean = {ePair.mean():.3f} GeV,  max = {ePair.max():.3f} GeV")
print(f"  Surviving μ KE: mean = {eMuFinal.mean():.3f} GeV,  min = {eMuFinal.min():.3f} GeV")
print(f"  e+e- inv. mass: mean = {invMass.mean():.2f} MeV,  max = {invMass.max():.2f} MeV")

if not HAS_MPL or nEvents == 0:
    sys.exit(0)

# ── Plots ─────────────────────────────────────────────────────────────────────
fig, axes = plt.subplots(2, 2, figsize=(10, 8))
fig.suptitle("Muon-Trident Events: 100 GeV μ⁻ on Carbon\n"
             r"$\mu + Z \rightarrow \mu + e^+ + e^- + Z$", fontsize=13)

# (a) e+e- pair KE
ax = axes[0, 0]
ax.hist(ePair, bins=30, color="steelblue", edgecolor="white", lw=0.5)
ax.set_xlabel("e⁺e⁻ pair total KE [GeV]")
ax.set_ylabel("Events")
ax.set_title("Pair kinetic energy")

# (b) surviving muon KE
ax = axes[0, 1]
ax.hist(eMuFinal, bins=30, color="darkorange", edgecolor="white", lw=0.5)
ax.set_xlabel("Surviving μ KE [GeV]")
ax.set_ylabel("Events")
ax.set_title("Muon energy loss")

# (c) e+e- invariant mass
ax = axes[1, 0]
ax.hist(invMass, bins=40, color="mediumseagreen", edgecolor="white", lw=0.5)
ax.set_xlabel("e⁺e⁻ invariant mass [MeV/c²]")
ax.set_ylabel("Events")
ax.set_title("Pair invariant mass")
ax.axvline(2 * 0.511, color="red", ls="--", lw=1, label="2mₑ threshold")
ax.legend(fontsize=9)

# (d) pair KE vs muon KE scatter
ax = axes[1, 1]
ax.scatter(eMuFinal, ePair, alpha=0.6, s=10, color="mediumpurple")
ax.set_xlabel("Surviving μ KE [GeV]")
ax.set_ylabel("e⁺e⁻ pair KE [GeV]")
ax.set_title("Pair KE vs muon energy loss")

plt.tight_layout()
out = "trident_analysis.png"
plt.savefig(out, dpi=150, bbox_inches="tight")
print(f"\nPlots saved to: {out}")
