#!/bin/bash

# Setup environment
source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_100 x86_64-centos7-gcc10-opt
export LD_PRELOAD=/usr/lib64/libopenblas.so.0

root -l << EOF
.L macros/unfold.C 
unfold() 
EOF
