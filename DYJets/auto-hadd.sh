#! /usr/bin/env bash

echo $1
nn_results=$(ls $1/results/dyjets-*.root | wc -l )
nn_scripts=$(ls $1/scripts/???.sh | wc -l )
if [[ $nn_results == $nn_scripts ]]; then 
    echo "$nn_scripts script files"
    echo "$nn_results result files"
    echo "All results files are available"
fi
if [[ $nn_results -ne $nn_scripts ]]; then 
    echo "$(ls $1/logs/log-???.txt | wc -l ) log files"
    echo "$(ls $1/logs/log-???_con.txt | wc -l) log_con files"
    echo "$(ls $1/logs/log-???_conerr.txt | wc -l ) log_conerr files"
    echo "$(ls $1/logs/log-???_conlog.txt | wc -l ) log_conlog files"
    echo "$(ls $1/"jobs"/job-??? | wc -l ) jobs files"
    echo "$(ls $1/"jobs"/job-???-"command" | wc -l ) job command files"
    echo "$(ls $1/"jobs"/job-???-"exit-code" | wc -l ) job exit code files"
    echo "$(ls $1/results/dyjets-*.root | wc -l ) result files"
    echo "$(ls $1/scripts/???.sh | wc -l ) sh scripts files"
    echo "$(ls $1/scripts/???.sub | wc -l ) sub scripts files"
    echo "OUTPUT MISSING !!!!"
    exit 1
fi
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
    dyjets-DYJets_M-50to100
    dyjets-ST_t-channel_top
    dyjets-TauTau
    dyjets-TT
    dyjets-ZZ
)

# samples processed with a single job: no merging
sample_single=(
    dyjets-DYJets_M-100to200
    dyjets-DYJets_M-200to400
    #dyjets-DYJets_M-200to500
    dyjets-DYJets_M-400to500
    dyjets-DYJets_M-500to700
    dyjets-DYJets_M-700to800
    dyjets-DYJets_M-800to1000
    dyjets-DYJets_M-1000to1500
    dyjets-DYJets_M-1500to2000
    #dyjets-DYJets_M-2000to3000
    dyjets-DYJets_M-2000toInf
    dyjets-ST_s-channel
    dyjets-ST_t-channel_antitop
    dyjets-ST_tW_antitop
    dyjets-ST_tW_top
    dyjets-WWTo2L2Nu
    dyjets-WZ
    dyjets-GammaGamma
    dyjets-GammaGamma_M-5to50
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


