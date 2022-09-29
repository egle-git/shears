#!/bin/bash

# This script is designed to do everything all in one step
# include one argument, the name of the directory containing the results of the analysis
# It then retrieves unfolding histograms and saves them in a root file
# Then it runs unfolding on these histograms
# It then plots the results of unfolding along with other plots

# Setup environment
source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_100 x86_64-centos7-gcc10-opt
export LD_PRELOAD=/usr/lib64/libopenblas.so.0

./combine-ROOT-files.sh $1

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
