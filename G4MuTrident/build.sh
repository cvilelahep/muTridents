#!/usr/bin/env bash
# build.sh  –  Configure and build the muon-trident simulation
# Requires: Geant4 ≥ 10.7 with CMake, ROOT optional

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build"

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "  Muon-Trident Geant4 build"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"

# Detect Geant4 installation via geant4-config or GEANT4_DIR
if command -v geant4-config &>/dev/null; then
    G4_INSTALL="$(geant4-config --prefix)"
    echo "Geant4 found at: ${G4_INSTALL}"
elif [ -n "${GEANT4_DIR:-}" ]; then
    G4_INSTALL="${GEANT4_DIR}"
    echo "Geant4 from GEANT4_DIR: ${G4_INSTALL}"
else
    echo "ERROR: Geant4 not found."
    echo "  • Source the Geant4 setup: source /path/to/geant4/bin/geant4.sh"
    echo "  • Or set GEANT4_DIR=/path/to/geant4/install"
    exit 1
fi

mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

cmake "${SCRIPT_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DGeant4_DIR="${G4_INSTALL}/lib/Geant4-*/" 2>/dev/null || \
cmake "${SCRIPT_DIR}" \
    -DCMAKE_BUILD_TYPE=Release

make -j"$(nproc)"

echo ""
echo "Build successful!"
echo "Run the simulation with:"
echo "  cd ${BUILD_DIR} && ./muon_trident run.mac"
echo ""
echo "Or interactive mode (requires OpenGL):"
echo "  cd ${BUILD_DIR} && ./muon_trident"
