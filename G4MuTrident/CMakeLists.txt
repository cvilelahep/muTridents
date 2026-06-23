cmake_minimum_required(VERSION 3.16)
project(MuonTrident VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Find Geant4 ──────────────────────────────────────────────────────────────
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# ── Sources ──────────────────────────────────────────────────────────────────
set(HEADERS
    include/DetectorConstruction.hh
    include/PhysicsList.hh
    include/PrimaryGeneratorAction.hh
    include/EventAction.hh
    include/SteppingAction.hh
    include/RunAction.hh
    include/AnalysisManager.hh
    include/MuPlusMuMinusBias.hh
    include/MuTridentXSbias.hh
)

set(SOURCES
    src/DetectorConstruction.cc
    src/PhysicsList.cc
    src/PrimaryGeneratorAction.cc
    src/EventAction.cc
    src/SteppingAction.cc
    src/RunAction.cc
    src/AnalysisManager.cc
    src/MuPlusMuMinusBias.cc
    src/MuTridentXSbias.cc
    muon_trident.cc
)

add_executable(muon_trident ${SOURCES} ${HEADERS})
target_include_directories(muon_trident PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(muon_trident ${Geant4_LIBRARIES})

# ── Copy macro files to build dir ────────────────────────────────────────────
configure_file(run.mac run.mac COPYONLY)
configure_file(vis.mac vis.mac COPYONLY)
