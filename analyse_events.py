import re
import sys
import csv
import argparse
from dataclasses import dataclass, asdict
from typing import Optional
import xml.etree.ElementTree as ET
import matplotlib.pyplot as plt

# ---------------------------------------------------------------------------
# Data structures
# ---------------------------------------------------------------------------

@dataclass
class Particle:
    pdg: int          # signed PDG code  (+13 = μ⁻, -13 = μ⁺)
    px:  float        # GeV
    py:  float        # GeV
    pz:  float        # GeV

    @property
    def pt(self) -> float:
        return (self.px**2 + self.py**2) ** 0.5

    @property
    def p(self) -> float:
        return (self.px**2 + self.py**2 + self.pz**2) ** 0.5

    @property
    def label(self) -> str:
        return {13: "mu-", -13: "mu+", 11: "e-", -11: "e+", 22: "gamma"}.get(
            self.pdg, str(self.pdg)
        )


@dataclass
class TridentEvent:
    event_number: int
    weight:       float            # first weight value on preamble line 1
    incoming_mu:  Particle         # always exactly one
    outgoing_mu:  list[Particle]   # 1 for elastic/Bhabha, 3 for trident
    electrons:    list[Particle]   # outgoing electrons/positrons
    photons:      list[Particle]   # NLO real photons

    @property
    def n_out_muons(self) -> int:
        return len(self.outgoing_mu)

    @property
    def n_photons(self) -> int:
        return len(self.photons)

    @property
    def topology(self) -> str:
        """Human-readable topology label."""
        if self.n_out_muons == 1:
            base = "elastic"
        elif self.n_out_muons == 3:
            base = "trident"
        else:
            base = f"{self.n_out_muons}mu"
        return base + (f"+{self.n_photons}γ" if self.n_photons else "")


# ---------------------------------------------------------------------------
# Parser
# ---------------------------------------------------------------------------

_EVENT_OPEN  = re.compile(r'<event>', re.IGNORECASE)
_EVENT_CLOSE = re.compile(r'</event>', re.IGNORECASE)

# A particle line starts with an integer (possibly negative) then exactly
# three floating-point numbers separated by whitespace.
_PARTICLE = re.compile(
    r'^\s*(-?\d+)\s+([-+eE\d.]+)\s+([-+eE\d.]+)\s+([-+eE\d.]+)\s*$'
)

# A pure-float line (preamble weights): one or more floats, no leading integer.
# We use this to distinguish preamble from particle lines robustly.
_FLOAT_ONLY = re.compile(
    r'^\s*([-+]?\d*\.?\d+(?:[eE][+-]?\d+)?)(\s+[-+]?\d*\.?\d+(?:[eE][+-]?\d+)?)*\s*$'
)


def _is_particle_line(line: str) -> Optional[tuple]:
    """Return (pdg, px, py, pz) if line is a particle line, else None."""
    m = _PARTICLE.match(line)
    if not m:
        return None
    pdg = int(m.group(1))
    # Guard: PDG codes are small integers; reject lines where the 'PDG' token
    # is actually a large float that happens to match (e.g. a weight value
    # that rounds to integer).  Real PDG codes |pdg| < 10000.
    if abs(pdg) > 9999:
        return None
    return pdg, float(m.group(2)), float(m.group(3)), float(m.group(4))


def _parse_event_block(block: list[str]) -> Optional[TridentEvent]:
    stripped = [l for l in block if l.strip()]
    if len(stripped) < 4:
        return None

    # Lines 0,1,2: seed, event_number, n_final_state  (all plain integers)
    try:
        event_number = int(stripped[1].strip())
    except (ValueError, IndexError):
        return None

    # Extract weight: first float on the first preamble line after line 2.
    weight = 0.0
    for line in stripped[3:]:
        parsed = _is_particle_line(line)
        if parsed is not None:
            break          # reached particle lines; no weight found (unusual)
        tokens = line.split()
        try:
            weight = float(tokens[0])
            break
        except (ValueError, IndexError):
            continue

    # Collect all particle lines
    all_muons:     list[Particle] = []
    all_electrons: list[Particle] = []
    all_photons:   list[Particle] = []

    for line in stripped:
        parsed = _is_particle_line(line)
        if parsed is None:
            continue
        pdg, px, py, pz = parsed
        p = Particle(pdg=pdg, px=px, py=py, pz=pz)
        if abs(pdg) == 13:
            all_muons.append(p)
        elif abs(pdg) == 11:
            all_electrons.append(p)
        elif pdg == 22:
            all_photons.append(p)

    if not all_muons:
        print(f"  [WARNING] Event {event_number}: no muon lines found; skipping.",
              file=sys.stderr)
        return None

    # Convention: first muon line is the beam muon (highest pz, pT ~ 0).
    incoming = all_muons[0]
    outgoing = all_muons[1:]

    return TridentEvent(
        event_number=event_number,
        weight=weight,
        incoming_mu=incoming,
        outgoing_mu=outgoing,
        electrons=all_electrons,
        photons=all_photons,
    )


def parse_mesmer_file(filepath: str) -> list[TridentEvent]:
    events: list[TridentEvent] = []
    with open(filepath) as fh:
        lines = fh.readlines()

    inside = False
    buf: list[str] = []
    for line in lines:
        if _EVENT_OPEN.search(line):
            inside = True
            buf = []
        elif _EVENT_CLOSE.search(line):
            inside = False
            ev = _parse_event_block(buf)
            if ev is not None:
                events.append(ev)
            buf = []
        elif inside:
            buf.append(line)

    return events

def parse_geant4_file(filepath: str) -> list[TridentEvent]:
    events: list[TridentEvent] = []
    with open(filepath) as fh:
        tree = ET.parse(fh)
        root = tree.getroot()

        tuple_elem = root.find("tuple")

        # Get column names
    columns = [col.attrib["name"] for col in tuple_elem.find("columns").findall("column")]

    # Parse rows
    for i_row, row in enumerate(tuple_elem.find("rows").findall("row")):
        values = [
            float(entry.attrib["value"])
            for entry in row.findall("entry")
        ]
        row = dict(zip(columns, values))
#        p_incoming = Particle(pdg=row["inc_pdg"], px=row["inc_px"], py=row["inc_py"], pz=row["inc_pz"])
        p_incoming = Particle(pdg=row["scat_pdg"], px=(row["scat_px"]+row["pair_neg_px"]+row["pair_pos_px"]), py=(row["scat_py"]+row["pair_neg_py"]+row["pair_pos_py"]), pz=(row["scat_pz"]+row["pair_neg_pz"]+row["pair_pos_pz"]))
        p_scatter = Particle(pdg=row["scat_pdg"], px=row["scat_px"], py=row["scat_py"], pz=row["scat_pz"])
        p_pair_minus = Particle(pdg=row["pair_neg_pdg"], px=row["pair_neg_px"], py=row["pair_neg_py"], pz=row["pair_neg_pz"])
        p_pair_plus = Particle(pdg=row["pair_pos_pdg"], px=row["pair_pos_px"], py=row["pair_pos_py"], pz=row["pair_pos_pz"])
    
        events.append(TridentEvent(
            event_number=i_row,
            weight=1.,
            incoming_mu=p_incoming,
            outgoing_mu=[p_scatter, p_pair_minus, p_pair_plus],
            electrons=[],
            photons=[])
        )
    
    return events


def makePlots(events, title = None, weighted = False):

    weight = []
    incoming_muon_p = []
    scatter_muon_p = []
    mupair_muon_p = []
    mupair_antimuon_p = []
    mupair_p_asym = []

    for ev in events:
        if ev.topology != "trident":
            continue

        if weighted:
            weight.append(ev.weight)
        else:
            weight.append(1.)
        
        incoming_muon_p.append(ev.incoming_mu.p)
        scatter_muon_p.append(ev.outgoing_mu[0].p)
        
        
        if ev.outgoing_mu[1].pdg == 13:
            mupair_p_asym.append(ev.outgoing_mu[1].p - ev.outgoing_mu[2].p)
            mupair_muon_p.append(ev.outgoing_mu[1].p)
            mupair_antimuon_p.append(ev.outgoing_mu[2].p)
            
        elif ev.outgoing_mu[1].pdg == -13:
            mupair_p_asym.append(ev.outgoing_mu[2].p - ev.outgoing_mu[1].p)
            mupair_muon_p.append(ev.outgoing_mu[2].p)
            mupair_antimuon_p.append(ev.outgoing_mu[1].p)

        else:
            raise RuntimeError("Expected muon!")
        mupair_p_asym[-1] /= ev.outgoing_mu[1].p + ev.outgoing_mu[2].p

    plt.figure()
    plt.hist(mupair_p_asym, weights = weight, range = (-1.2, 1.2), bins = 60, histtype = "step", label = "Muon pair")
    plt.xlabel(r"$\frac{|p(\mu^-)| - |p(\mu^+)|}{|p(\mu^-)| + |p(\mu^+)|}$") 
    plt.legend()
    if title:
        plt.title(title)
    plt.tight_layout()

    plt.figure()
    h_incoming, bins, _ = plt.hist(incoming_muon_p, weights = weight, range = (0, 110), bins = 60, histtype = "step", label = "Incoming muon", density = True)
    h_scatter, bins, _ = plt.hist(scatter_muon_p, weights = weight, range = (0, 110), bins = 60, histtype = "step", label = "Scattered muon", density = True)
    h_mupair_muon, bins, _ = plt.hist(mupair_muon_p, weights = weight, range = (0, 110), bins = 60, histtype = "step", label = r"$\mu^-$", density = True)
    h_mupair_antimuon, bins, _ = plt.hist(mupair_antimuon_p, weights = weight, range = (0, 110), bins = 60, histtype = "step", label = r"$\mu^+$", density = True)
    plt.xlabel(r"$|p|$ [GeV]") 
    plt.legend()
    if title:
        plt.title(title)

    plt.tight_layout()
    return {"bins": bins,
            "h_incoming": h_incoming,
            "h_scatter" : h_scatter,
            "h_mupair_muon": h_mupair_muon,
            "h_mupair_antimuon": h_mupair_antimuon}
            
# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------
def main():
    ap = argparse.ArgumentParser(
        description="Extract muon kinematics from a MESMER output file."
    )
    ap.add_argument("--mesmer", help="MESMER file")
    ap.add_argument("--geant4", help="Geant4 file")
    args = ap.parse_args()

    if args.mesmer is not None:
        events_mesmer = parse_mesmer_file(args.mesmer)
    if args.geant4 is not None:
        events_geant4 = parse_geant4_file(args.geant4)

    if (not events_mesmer) and (not events_geant4) :
        print("No events found. Check input file format.", file=sys.stderr)
        sys.exit(1)

    if events_mesmer:
        mesmer_hists_unweighted = makePlots(events_mesmer, title = "MESMER", weighted = False)
        mesmer_hists_weighted = makePlots(events_mesmer, title = "MESMER", weighted = True)
    if events_geant4:
        geant4_hists = makePlots(events_geant4, title = "Geant4", weighted = False)

    plt.figure()
    plt.stairs(edges = geant4_hists["bins"], values = geant4_hists["h_scatter"], label = r"Geant4 scattered $\mu$")
    plt.stairs(edges = geant4_hists["bins"], values = mesmer_hists_unweighted["h_scatter"], label = r"MESMER scattered $\mu$ (unweighted)")
    plt.stairs(edges = geant4_hists["bins"], values = mesmer_hists_weighted["h_scatter"], label = r"MESMER scattered $\mu$ (weighted)")
    plt.xlabel(r"$|p|$ [GeV]")     
    plt.legend()
    plt.ylim(bottom = 0)
    plt.tight_layout()

    plt.figure()
    plt.stairs(edges = geant4_hists["bins"], values = geant4_hists["h_mupair_muon"], label = r"Geant4 pair $\mu^-$")
    plt.stairs(edges = geant4_hists["bins"], values = mesmer_hists_unweighted["h_mupair_muon"], label = r"MESMER pair $\mu^-$ (unweighted)")
    plt.stairs(edges = geant4_hists["bins"], values = mesmer_hists_weighted["h_mupair_muon"], label = r"MESMER pair $\mu^-$ (weighted)")
    plt.xlabel(r"$|p|$ [GeV]")     
    plt.legend()
    plt.ylim(bottom = 0)
    plt.tight_layout()

    plt.figure()
    plt.stairs(edges = geant4_hists["bins"], values = geant4_hists["h_mupair_antimuon"], label = r"Geant4 pair $\mu^+$")
    plt.stairs(edges = geant4_hists["bins"], values = mesmer_hists_unweighted["h_mupair_antimuon"], label = r"MESMER pair $\mu^+$ (unweighted)")
    plt.stairs(edges = geant4_hists["bins"], values = mesmer_hists_weighted["h_mupair_antimuon"], label = r"MESMER pair $\mu^+$ (weighted)")
    plt.xlabel(r"$|p|$ [GeV]")     
    plt.legend()
    plt.ylim(bottom = 0)
    plt.tight_layout()

    plt.show()



if __name__ == "__main__":
    main()
