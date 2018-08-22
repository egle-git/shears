#! /usr/bin/env bash

eval $(.gitlab-ci/cmsenv.sh)

source /cvmfs/cms.cern.ch/slc6_amd64_gcc530/external/cmake/3.7.0-oenich/etc/profile.d/init.sh

SHEARS_BASE_DIR=$PWD

# Needed to use scram
mkdir $CMSSW_BASE/shears-build
cd $CMSSW_BASE/shears-build

# Out-of-source build
cmake $SHEARS_BASE_DIR/$1
make
