#!/bin/bash
input="datasets_2016preAPV_fakerateMarijus.txt"
echo "Previous Catalogs will be overwritten"
read -p "Are you sure? " -n 1 -r
echo    # (optional) move to a new line
if [[ $REPLY =~ ^[Yy]$ ]]
    then
    # do dangerous stuff
    echo "---------------------------"
    echo "Lets go!"
    while IFS= read -r line
    do
       ## take some action on $line
      [[ $line = \#* ]] && continue
      echo $line
      NAME=${line#/*} # le contenu après le premier /
      NAME=${NAME%"_TuneCP5"*} #le contenu avant le pattern
      echo $NAME
      xsec=${line#*:} #le contenu après le dernier :
      echo $xsec
      dataset=${line%:*}
      echo $dataset
      echo -e "# primary dataset: $dataset\n* data type: mc\n* primary events: 1\n* primary files: 1\n* lumi: -1\n* sample xsec: $xsec\n\n# ROOTFILES\n" > "Catalog2016_bonzai_$NAME.txt"
    done < "$input"
    #line="/QCD_Pt-15To20_MuEnrichedPt5_TuneCP5_13TeV-pythia8/RunIISummer20UL16NanoAODAPVv9-106X_mcRun2_asymptotic_preVFP_v11-v2/NANOAODSIM"
fi
