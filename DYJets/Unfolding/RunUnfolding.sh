#!/bin/bash

# Setup environment
source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_100 x86_64-centos7-gcc10-opt
export LD_PRELOAD=/usr/lib64/libopenblas.so.0

directory=$1
channel=$2

mkdir -p plots

root -l << EOF
TString dir = "$directory"
TString channel = "$channel"
.L macros/unfold.C 
unfold(dir,channel) 
EOF
