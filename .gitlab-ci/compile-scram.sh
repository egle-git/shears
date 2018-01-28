#! /usr/bin/env bash

shopt -s expand_aliases

. /cvmfs/cms.cern.ch/cmsset_default.sh

#download the CMSSW area in local
wget http://mon.iihe.ac.be/~hbrun/filesForTests/aFullInstallationCompiled.tar.gz
tar xfvz aFullInstallationCompiled.tar.gz
cd CMSSW_8_0_26p1_testFullInstall/src
scramv1 b ProjectRename
scramv1 b -j 4
cmsenv && cd -

#now install the baobab producer
cp -r $1 CMSSW_8_0_26p1_testFullInstall/src/shears
cd CMSSW_8_0_26p1_testFullInstall/src/

# Override architecture for SCRAM
export SCRAM_ARCH=slc6_amd64_gcc530
scramv1 b

#get the result 
cd ../..
tar cfvz shearsCompilated.tar.gz CMSSW_8_0_26p1_testFullInstall


