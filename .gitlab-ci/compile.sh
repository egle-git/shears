#! /usr/bin/env bash

shopt -s expand_aliases

. /cvmfs/cms.cern.ch/cmsset_default.sh
cd CMSSW_8_0_27 && cmsenv && cd -

cd $1
make

