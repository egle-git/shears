#! /usr/bin/env bash
ls 
shopt -s expand_aliases

. /cvmfs/cms.cern.ch/cmsset_default.sh

# Override architecture for SCRAM
export SCRAM_ARCH=slc6_amd64_gcc53
wget http://mon.iihe.ac.be/~hbrun/filesForTests/aFullInstallationCompiled.tar.gz
tar xfvz aFullInstallationCompiled.tar.gz
cd CMSSW_8_0_26p1_testFullInstall/src 
scramv1 b ProjectRename
scramv1 b -j 4
cd ../..
