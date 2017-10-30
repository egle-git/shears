#! /usr/bin/env bash

shopt -s expand_aliases

. /cvmfs/cms.cern.ch/cmsset_default.sh
cd CMSSW_8_0_27 && cmsenv && cd -

mkdir CMSSW_8_0_27/src/shears
cp -r $1 CMSSW_8_0_27/src/shears
cd CMSSW_8_0_27/src/shears/$1

# Override architecture for SCRAM
export SCRAM_ARCH=slc6_amd64_gcc530

scram b

