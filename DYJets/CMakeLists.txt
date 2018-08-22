cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

# Use FindROOT from the repo, not the system one -- it doesn't have all the features we need
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Require C++ 14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
find_package(Boost REQUIRED COMPONENTS filesystem program_options system)
find_package(ROOT 6 REQUIRED COMPONENTS Table TreePlayer Unfold)
find_package(Threads REQUIRED)

# yaml-cpp requires special treatment
find_package(SCRAM)
if(${SCRAM_FOUND})
    SCRAM_FIND_PACKAGE(yaml-cpp 0.5)
else()
    FIND_PACKAGE(yaml-cpp 0.5 REQUIRED)
endif()

###################################################################################################
# RooUnfold
###################################################################################################

file(GLOB RooUnfold_src "RooUnfold/src/*.cxx")
add_library(RooUnfold ${RooUnfold_src} RooUnfoldDict.cxx)

target_include_directories(RooUnfold PUBLIC "RooUnfold/src")
target_include_directories(RooUnfold SYSTEM PUBLIC ${ROOT_INCLUDE_DIRS})

# Generate dictionary
ROOT_GENERATE_DICTIONARY(RooUnfold
    RooUnfoldDict
    "RooUnfold/src/*.h"
    LINKDEF "RooUnfold_LinkDef.h"
)

###################################################################################################
# New DYJets library
###################################################################################################

# Files added by hand to avoid importing too many static variables from the old code...
add_library(DYJets
    Sources/catalog.cc
    Sources/chains.cc
    Sources/comparison_entry.cc
    Sources/functions.cc
    Sources/histo_set.cc
    Sources/job.cc
    Sources/logging.cc
    Sources/mc_group.cc
    Sources/muons.cc
    Sources/options.cc
    Sources/sample.cc
    Sources/timer.cc
    Sources/triggers.cc
    Sources/weights.cc
)

# Preprocessor macro to disable incompatible parts of the old code
target_compile_definitions(DYJets PUBLIC DYJETS_NEW_API)

# Required headers
target_include_directories(DYJets PUBLIC Includes)
target_include_directories(DYJets SYSTEM PUBLIC ${ROOT_INCLUDE_DIRS})

# Dependencies
target_link_libraries(DYJets PUBLIC boost_filesystem)
target_link_libraries(DYJets PUBLIC boost_program_options)
target_link_libraries(DYJets PUBLIC boost_system)
target_link_libraries(DYJets PUBLIC ${ROOT_LIBRARIES})
target_link_libraries(DYJets PUBLIC ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(DYJets PUBLIC RooUnfold)
target_link_libraries(DYJets PUBLIC yaml-cpp)

###################################################################################################
# Higgs targets
###################################################################################################

add_executable(higgs-loop Main/higgs-loop.cc Sources/higgs_analyzer.cc)
target_link_libraries(higgs-loop PRIVATE DYJets)

add_executable(higgs-compare Main/higgs-compare.cc)
target_link_libraries(higgs-compare PRIVATE DYJets)
