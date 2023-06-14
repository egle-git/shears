#!/bin/bash

# This script combines all root files output from shears for use in unfolding
# The argument should be the name of the directory where all root files are saved

loadDirectory=$1
saveDirectory=$2

echo "Combining output histograms from Shears from directory $loadDirectory"
echo "Saving the combined files to $saveDirectory"

# categori es which have many jobs and need to be combined
file_stem=(
    dyjets-data
    dyjets-DYJets_M-10to50
    dyjets-DYJets_M-50to100
    dyjets-TauTau
    dyjets-ST_t-channel_top
    dyjets-TT
    dyjets-ZZ
)

# Categories with single files that only need to be moved
single_files=(
    dyjets-DYJets_M-100to200
    dyjets-DYJets_M-200to400
    dyjets-DYJets_M-400to500
    dyjets-DYJets_M-500to700
    dyjets-DYJets_M-700to800
    dyjets-DYJets_M-800to1000
    dyjets-DYJets_M-1000to1500
    dyjets-DYJets_M-1500to2000
    dyjets-DYJets_M-2000toInf
    dyjets-ST_tW_antitop
    dyjets-ST_tW_top
    dyjets-ST_s-channel
    dyjets-ST_t-channel_antitop
    dyjets-WWTo2L2Nu
    dyjets-WZ
    dyjets-GammaGamma
)
saveLocation=$saveDirectory

mkdir -p $saveLocation

cd $saveLocation
rm *.root
cd -
cd ../$loadDirectory/results

# Loop over each file that needs to be combined and combine with hadd
for index in ${!file_stem[*]}; do
    echo "Combining ${file_stem[$index]}"
    hadd -f ${file_stem[$index]}.root ${file_stem[$index]}*
    mv ${file_stem[$index]}.root ../../Unfolding/${saveLocation}/
done

# loop over each individual file that nees to be moved and move it
for index in ${!single_files[*]}; do
    echo "Moving ${single_files[$index]}"
    cp ${single_files[$index]}.root ../../Unfolding/${saveLocation}/
done

cd ../../Unfolding
