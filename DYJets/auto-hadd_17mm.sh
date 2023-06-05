#! /usr/bin/env bash

echo $1
if [[ $(grep -L "Done writing output" $1/logs/log-???.txt | wc -l) -ne "0" ]]; then
    echo "OUTPUT MISSING, look at the following files :"
    echo "$(grep -L "Done writing output" $1/logs/log-???.txt)"
    exit 1
fi
cd $1/results/ || exit 1

# samples processed with multiple jobs: need merging
sample_multiple=(
    dyjets-data # both for dyjets-data, dyjets-data-smu (mu channel)
    dyjets-DYJets_M-10to50
    # dyjets-DYJets_M-50to100
    dyjets-DYJets_M-50to200
    dyjets-ST_t-channel_top
    dyjets-TauTau
    dyjets-TT
    dyjets-ZZ
)

# samples processed with a single job: no merging
sample_single=(
    # dyjets-DYJets_M-100to200
    dyjets-DYJets_M-200to400
    dyjets-DYJets_M-400to500
    dyjets-DYJets_M-500to700
    dyjets-DYJets_M-700to800
    dyjets-DYJets_M-800to1000
    dyjets-DYJets_M-1000to1500
    dyjets-DYJets_M-1500to2000
    dyjets-DYJets_M-2000toInf
    dyjets-ST_s-channel
    dyjets-ST_t-channel_antitop
    dyjets-ST_tW_antitop
    dyjets-ST_tW_top
    dyjets-WWTo2L2Nu
    dyjets-WZ
    dyjets-GammaGamma
)

# merging samples in sample_multiple
for index in ${!sample_multiple[*]}; do
    echo "Combining ${sample_multiple[$index]}"
    ls ${sample_multiple[$index]}*.root
    hadd -f ../${sample_multiple[$index]}.root ${sample_multiple[$index]}*.root
done

# just copy the samples in sample_single
for index in ${!sample_single[*]}; do
    echo "Copying ${sample_single[$index]}"
    cp -p ${sample_single[$index]}.root ../
done

cd ../
echo "output: "
ls


