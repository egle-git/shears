#! /usr/bin/env bash

shopt -s expand_aliases

. /cvmfs/cms.cern.ch/cmsset_default.sh
tar xfvz shearsCompilated.tar.gz
cd CMSSW_8_0_26p1_testFullInstall && cmsenv && scramv1 b ProjectRename && scramv1 b -j 4 && cd -

cd $1
make
