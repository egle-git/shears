Welcome to Shears, the Simple and Handy Event Analysis ROOT-based Suite
=======================================================================

Shears is the analysis framework developed within the CMS SMP-VJ group for Run-2 data analyses. It is an evolution of the W+jets and Z+jets analysis framework of 8 TeV analysis [1] originally developed by ULB (Tomislav Seva and Alexandre Leonard) and including contributions from METU (Bugra Bilin), Northeastern University (Apichart Hortiangthan) and CEA/Saclay (Philippe Gras).

More information can be found in the dedicated [Shears Twiki](https://twiki.cern.ch/twiki/bin/view/CMS/ShearsAnalysisFramework): 

Repository organisation
-----------------------

**Bonzais** Tools to make small ROOT trees ("skims") from genuine trees (so called Boababs)

**DYJets** Analysis code. Currently for W+jet and Z+jet analysis.

**WJets** W+jet analysis code.

**ntuple_production** Tools to produce the Boabab ROOT ntuple.

*Documentation can be found in the respective directories (README.md file displayed when browsing the directory with gitlab as for the one you are reading).*

*The code that produces the ntuple (Boabab) from CMS EDM dataset (MINIAOD) and used by the tools in ntuple\_production folder can be found in the [Tuple repository](https://github.com/UGent/Tupel/tree/Tupel_MiniAOD), under the git branch Tuple_MiniAOD.*

References
----------

[1] https://github.com/iihe-cms-sw/TreeAnalysis