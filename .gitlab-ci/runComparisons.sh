#! /usr/bin/env bash
ls
shopt -s expand_aliases

. /cvmfs/cms.cern.ch/cmsset_default.sh

# Override architecture for SCRAM
export SCRAM_ARCH=slc6_amd64_gcc53
cp .gitlab-ci/compareHistos.C HZZ2l2nu 

scramv1 project CMSSW CMSSW_8_0_26_patch1
cd CMSSW_8_0_26_patch1/src && cmsenv && cd ../..
#cp runHZZanalysis HZZ2l2nu && cd HZZ2l2nu
cd HZZ2l2nu
wget http://mon.iihe.ac.be/~hbrun/filesForTests/${1}_Ref.root 
mkdir result_${1}

root -b -q -l 'compareHistos.C("'${1}'_Ref.root", "'${1}'.root", "result_'${1}'")'


