#! /usr/bin/env bash
ls 
shopt -s expand_aliases

. /cvmfs/cms.cern.ch/cmsset_default.sh

# Override architecture for SCRAM
export SCRAM_ARCH=slc6_amd64_gcc53


scramv1 project CMSSW CMSSW_8_0_26_patch1
cd CMSSW_8_0_26_patch1/src && cmsenv && cd ../..
#cp runHZZanalysis HZZ2l2nu && cd HZZ2l2nu
cd HZZ2l2nu
wget http://mon.iihe.ac.be/~hbrun/filesForTests/$1
echo `pwd`"/$1" > localCata.txt
./runHZZanalysis catalogInputFile=localCata.txt histosOutputFile=$2 skip-files=0 max-files=25 isMC=$3 maxEvents=-1 doInstrMETAnalysis=0 doTnPTree=0
