#!/bin/bash

# This script combines all root files output from shears for use in unfolding
# The argument should be the name of the directory where all root files are saved

loadDirectory=$1
saveDirectory=$2

echo "Combining output histograms from Shears from directory $loadDirectory"
echo "Saving the combined files to $saveDirectory"

# File stems of all files to be combined and copied
file_stem=(
    dyjets-data
    dyjets-DYJets_M-10to50
    dyjets-DYJets_M-50to100
    dyjets-TauTau
    dyjets-TT
    dyjets-ZZ
    dyjets-ST_t-channel_top
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

# location to save all combined output files
saveLocation=$saveDirectory

# creates directory for save location if it does not exist
mkdir -p $saveLocation

# move into save directory
cd $saveLocation
# create directory one level up for saving plots if it does not exist
mkdir -p ../plots
# remove old root files if they are already present
rm *.root
cd -
# move to location of saved root files from Shears so they can be loope over, combined, and
# copied
cd ../$loadDirectory/results

# Loop over each file that needs to be combined and combine with hadd
for index in ${!file_stem[*]}; do
    # see how many files with a given stem exist
    nFiles=$(ls -lh ${file_stem[$index]}* | wc -l)
    echo "$nFiles ${file_stem[$index]} files in the directory"

    # if there is one file, it just needs to be copied to the new location
    if [ $nFiles -eq 1 ]
    then
        echo "Copying file ${file_stem[$index]}.root to $saveLocation"
        cp ${file_stem[$index]}.root ../../Unfolding/${saveLocation}/
    # if there are multiple files, they need to be combined with hadd
    # and then copied to the new location
    elif [ $nFiles -gt 1 ]
    then
        echo "Combining files"
        hadd ${file_stem[$index]}.root ${file_stem[$index]}*
        echo "Copying file ${file_stem[$index]}.root to $saveLocation"
        cp ${file_stem[$index]}.root ../../Unfolding/${saveLocation}/
    # if there are zero files of this type, there is nothing to hadd or copy
    else 
        echo "There are no files with the stem ${file_stem[$index]} in the directory $loadDirectory"
    fi
    echo " "
done

cd ../../Unfolding
