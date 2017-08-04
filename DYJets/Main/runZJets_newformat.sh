#!/bin/bash
# -*- mode: sh -*-

#This script will allow for multiple threads to start for each job of runZJets. 
source /data/djarcaro/CMSSW_8_0_25/src/shears/DYJets/setup.sh
nJobs=$1

for (( iJob=1; iJob<=nJobs; iJob++ ))
do  
    echo "Trying job = $iJob"
    SAMPLE="DYJETS"
    date=$(date +%m_%d_%y)
    TRY=$2
    outputFile="ZJets_$(echo $SAMPLE)_JOB$(echo $iJob)_$(echo $TRY)_$date.log"
    echo $outputFile
    echo "runZJets_newformat doWhat=$SAMPLE nJobs=$nJobs jobNum=$iJob &> $outputFile &"
    runZJets_newformat doWhat=$SAMPLE nJobs=$nJobs jobNum=$iJob &> $outputFile &
done
