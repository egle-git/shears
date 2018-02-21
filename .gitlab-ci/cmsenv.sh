#! /usr/bin/env bash

shopt -s expand_aliases

. /cvmfs/cms.cern.ch/cmsset_default.sh

# Override architecture for SCRAM
export SCRAM_ARCH=slc6_amd64_gcc53

cmsrel CMSSW_8_0_26 >/dev/null
cd CMSSW_8_0_26
scram runtime -sh
