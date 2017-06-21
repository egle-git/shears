Welcome to Shears, the Simple and Handy Event Analysis ROOT-based Suite
=======================================================================

Shears is the analysis framework developed within the CMS SMP-VJ group for Run-2 data analyses. It is an evolution of the W+jets and Z+jets analysis framework of 8 TeV analysis [1] originally developed by ULB (Tomislav Seva and Alexandre Leonard) and including contributions from METU (Bugra Bilin), Northeastern University (Apichart Hortiangthan) and CEA/Saclay (Philippe Gras).

More information can be found in the dedicated [Shears Twiki](https://twiki.cern.ch/twiki/bin/view/CMS/ShearsAnalysisFramework): 

Repository organisation
-----------------------

**Bonzais** Tools to make small ROOT trees ("skims") from genuine trees (so called Boababs)

**DYJets** Analysis code. Currently for W+jet and Z+jet analysis.

**WJets** W+jet analysis code (can run on both Baobab and Bonzai formats, currently set for Bonzai).

**ntuple_production** Tools to produce the Boabab ROOT ntuple.

*Documentation can be found in the respective directories (README.md file displayed when browsing the directory with gitlab as for the one you are reading).*

*The code that produces the ntuple (Boabab) from CMS EDM dataset (MINIAOD) and used by the tools in ntuple\_production folder can be found in the [Tuple repository](https://github.com/UGent/Tupel/tree/Tupel_MiniAOD), under the git branch Tuple_MiniAOD.*

Installation recipe
------------------

**a) install the additionnal packages**

```
cmsrel CMSSW_8_0_26_patch1 
cd CMSSW_8_0_26_patch1/src 
cmsenv 
git cms-init #add the repository with the updated Egamma package 
git cms-merge-topic cms-egamma:EGM_gain_v1 
cd EgammaAnalysis/ElectronTools/data  
git clone https://github.com/ECALELFS/ScalesSmearings.git
cd ScalesSmearings 
git checkout Moriond17_23Jan_v2 
cd $CMSSW_BASE/src 
scram b -j 8
```

**b) install and compile the Hzz code** 
```
git clone ssh://git@gitlab.cern.ch:7999/HZZ-IIHE/shears.git
git clone -b Tupel_MiniAOD ssh://git@gitlab.cern.ch:7999/HZZ-IIHE/NTuple-Producer.git
scramv1 b -j 16
```

**c) load the environement** 
```
cd shears/ntuple_production
PATH=$PATH:`pwd`
```

References
----------

[1] https://github.com/iihe-cms-sw/TreeAnalysis
