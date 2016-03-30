The directory contains the CMSSW patches required in addition to the Tupel code in order to produce Boabab ntuples.


patch-CMSSW_7_6_3_patch_2.sh
----------------------------

Boabab from miniaod 76x can be produced with CMSSW_7_6_3_patch2 with two additionnal patches: one for the electron and photon energy correction and one for the jet pileup id correction. The script patch-CMSSW_7_6_3_patch_2.sh, to be run from the src direcrtory copy from the release directory the two packages to patch and apply the patches. The script will fail to run if the current directory contains already the directories EgammaAnalysis and JetProducers.


