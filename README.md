Welcome to Shears, the Simple and Handy Event Analysis ROOT-based Suite
=======================================================================

Shears is the analysis framework developed within the CMS SMP-VJ group for Run-2 data analyses. It is an evolution of the W+jets and Z+jets analysis framework of 8 TeV analysis [1] originally developed by ULB (Tomislav Seva and Alexandre Leonard) and including contributions from METU (Bugra Bilin), Northeastern University (Apichart Hortiangthan) and CEA/Saclay (Philippe Gras).

More information can be found in the dedicated [Shears Twiki](https://twiki.cern.ch/twiki/bin/view/CMS/ShearsAnalysisFramework): 

Repository organisation
-----------------------

**Bonzais** Tools to make small ROOT trees ("skims") from genuine trees (so called Boababs)

**DYJets** Analysis code. Currently for W+jet and Z+jet analysis.

**HZZ2l2nu** Analysis code from the HZZ2l2nu group.

**TagAndProbe** Analysis code for tag and probe (also from the HZZ2l2nu group).

**WJets** W+jet analysis code (can run on both Baobab and Bonzai formats, currently set for Bonzai).

**ntuple_production** Tools to produce the Boabab ROOT ntuple.

**Baobabs** The code that produces the ntuple (Boabab) from CMS EDM dataset (MINIAOD) and used by the tools in ntuple\_production

*Documentation can be found in the respective directories (README.md file displayed when browsing the directory with gitlab as for the one you are reading).*

Installation recipe
------------------

**a) install the additionnal packages**

```
cmsrel CMSSW_8_0_26_patch1
cd CMSSW_8_0_26_patch1/src
cmsenv
git cms-init #add the repository with the updated Egamma packages
git cms-merge-topic HuguesBrun:METfiltersIn8027
git cms-merge-topic ikrav:egm_id_80X_v3_photons
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
scramv1 b -j 16
```

**c) load the environement**
```
cd shears/ntuple_production
PATH=$PATH:`pwd`
```

Compiling
---------

In addition to analysis' own `Makefile`s, the framework provides a `Makefile` at
the root of the source tree. It supports the following targets:

~~~{.sh}
make            # Will build everything for all analysis. Probably not what you want
make all        # Same as above
make clean      # Will clean everything (calls `make clean` in analysis folders)
make Pruners    # Will build all pruners (by running `make Pruners` in every
                # analysis folder)
make <Analysis> # Will build everything related to the given analysis (ie the pruner
                # executable, `make Pruners` and `make all` in the analysis folder)
~~~

Other targets should be considered *internal* and not relied on (althrough they
may work).

Old code
--------

Old code is pruned off the repo from time to time, but is still available in the
history. The table below lists the last commit at which such features were
available.

| Commit   | Comment                         |
|----------|---------------------------------|
| a0a6677b | Code for reading 8 TeV baobabs  |

References
----------

[1] https://github.com/iihe-cms-sw/TreeAnalysis
