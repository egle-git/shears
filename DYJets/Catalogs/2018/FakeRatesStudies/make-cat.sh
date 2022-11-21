#!/bin/bash
input="datasets_2018_fakerateMarijus.txt"
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
      #echo $line
      NAME=${line#/*}
      NAME=${NAME%"_TuneCP5"*}
      echo $NAME
      echo -e "# primary dataset: $line\n* data type: mc\n* primary events: 1\n* primary files: 1\n* lumi: -1\n* sample xsec:\n\n# ROOTFILES\n" > "Catalog2018_bonzai_$NAME.txt"
    done < "$input"
    #line="/QCD_Pt-15To20_MuEnrichedPt5_TuneCP5_13TeV-pythia8/RunIISummer20UL16NanoAODAPVv9-106X_mcRun2_asymptotic_preVFP_v11-v2/NANOAODSIM"
fi
