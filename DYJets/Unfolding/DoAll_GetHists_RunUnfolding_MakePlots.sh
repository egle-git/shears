#!/bin/bash

# This script is designed to do everything all in one step
# include one argument, the name of the directory containing the results of the analysis
# It then retrieves unfolding histograms and saves them in a root file
# Then it runs unfolding on these histograms
# It then plots the results of unfolding along with other plots

# Setup environment
source /cvmfs/sft.cern.ch/lcg/views/setupViews.sh LCG_100 x86_64-centos7-gcc10-opt
export LD_PRELOAD=/usr/lib64/libopenblas.so.0

directory=$1
era=$2
chan=$3

mkdir -p plots
echo " "
echo "************************************"
echo "Processing directory $directory, era $era, and channel $chan"
echo "************************************"
echo "Now running getUnfoldingHistograms.C"
root -l << EOF
TString dir = "$directory"
TString era = "$era"
TString channel = "$chan"
.L macros/getUnfoldingHistograms.C 
getUnfoldingHistograms(dir,era,channel)
EOF

echo "************************************"
echo "Now running unfold.C"
root -l << EOF
TString dir = "$directory"
TString era = "$era"
TString channel = "$chan"
.L macros/unfold.C 
unfold(dir,era,channel) 
EOF

echo "************************************"
echo "Now running makePlots.C"
root -l << EOF
TString directory = "$directory"
TString era = "$era"
TString channel = "$chan"
.L macros/makePlots.C 
makePlots(directory,era,channel)
EOF
