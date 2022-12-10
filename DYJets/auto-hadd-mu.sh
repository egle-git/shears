#! /usr/bin/env bash

echo $1
if [[ $(grep -L "Done writing output" $1/logs/log-???.txt | wc -l) -ne "0" ]]; then
    echo "OUTPUT MISSING, look at the following files :"
    echo "$(grep -L "Done writing output" $1/logs/log-???.txt)"
    exit 1
fi
cd $1/results/ || exit 1

#for s in DYJets data-smu; do
for s in data DYJets data-smu; do
#for s in data data-smu DYJetsToMuMu DYJetsToLL; do
    echo $s
    ls dyjets-$s-?.root dyjets-$s-??.root
    hadd -f ../dyjets-$s.root dyjets-$s-?.root dyjets-$s-??.root
done

for s in TT ZZ TauTau; do
    echo $s
    hadd -f ../dyjets-$s.root dyjets-$s-?.root
done

for s in WZ WWTo2L2Nu ST_s-channel ST_t-channel_antitop ST_t-channel_top ST_tW_antitop ST_tW_top; do
    echo $s
    cp -p dyjets-$s.root ../
done

cd ../
ls
hadd -f tmp.root dyjets-data.root dyjets-data-smu.root
rm dyjets-data.root dyjets-data-smu.root
mv tmp.root dyjets-data.root
#mv dyjets-data-smu.root dyjets-data.root
ls

