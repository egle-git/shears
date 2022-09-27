#!/bin/bash

# This script combines all root files output from shears for use in unfolding
# The argument should be the name of the directory where all root files are saved

# categories which have many jobs and need to be combined
file_stem=(
    dyjets-data
    dyjets-DYJets
    dyjets-TauTau
    dyjets-TT
    dyjets-WJetsToLNu
    dyjets-ZZ
)

# Categories with single files that only need to be moved
single_files=(
    dyjets-ST_tW_antitop
    dyjets-ST_tW_top
    dyjets-ST_s-channel
    dyjets-ST_t-channel_top
    dyjets-ST_t-channel_antitop
    dyjets-WWTo2L2Nu
    dyjets-WZ
)

cd plots
rm *.png
cd -

saveLocation=histograms/fromShears

cd $saveLocation
rm *.root
cd -
cd ../$1/results

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
