#!/bin/bash

# This script gets the histograms which have been combined using combine-ROOT-files.sh
# Then it runs unfolding on these histograms
# It then plots the results of unfolding along with other plots

# Setup environment
source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_100 x86_64-centos7-gcc10-opt
export LD_PRELOAD=/usr/lib64/libopenblas.so.0

root -l << EOF
.L macros/getUnfoldingHistograms.C
getUnfoldingHistograms()
EOF

root -l << EOF
.L macros/unfold.C
unfold()
EOF

root -l << EOF
.L macros/makePlots.C 
makePlots()
EOF
