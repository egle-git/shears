#! /usr/bin/env bash

echo $1
cd $1/results/ || exit 1

for s in data DYJets data-smu; do
#for s in data DYJets; do
    echo $s
    ls dyjets-$s-?.root dyjets-$s-??.root
    hadd -f ../dyjets-$s.root dyjets-$s-?.root dyjets-$s-??.root
done

for s in TT ZZ TauTau WJetsToLNu; do
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

