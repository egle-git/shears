#!/bin/bash

# Setup environment
source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_100 x86_64-centos7-gcc10-opt
export LD_PRELOAD=/usr/lib64/libopenblas.so.0

dir=$1
chan=$2

mkdir -p plots


root -l << EOF
TString directory = "$dir"
TString channel = "$chan"
.L macros/makePlots.C 
makePlots(directory,channel)
EOF
