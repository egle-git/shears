Quick start
============

Set up a CMSSW environment:

```
   $ cmsrel CMSSW_7_4_11
   $ cd CMSSW_7_4_11
   $ cmsenv
   $ cd ..
```

Clone the shears project to your working directory:

    git clone ssh://git@gitlab.cern.ch:7999/shears/shears.git

Once you have cloned the repository, you should move to the `shears/DYJets` directory and build the code:

```
    $ cd shears/DYJets
    $ make clean
    $ make
```

Unless you have already your own configuration file, copy the example:

```
cp example.cfg vjets.cfg
```

(vjets.cfg stored on git is not guaranteed to work out-of-the box and we recommend to use example.cfg).

To run the Z+jet analysis, execute:

```
    $ ./runZJets_newformat
```

With the default configuration file, the histograms will be store in the HistoFiles directory.

Note: if you have many "Warning: parameter XYZ was not found in configuration file!", it is most probably that you missed to copy the example.cfg file to vjets.cfg, made a typo in vjets.cfg when you copied it, or used an old version of vjets.cfg file.

To use as input ntuple in the Run 1 format, run `runZJets` instead of `runZJets_newformat`. To use Run 1 format you should use the code from the Run1_format branch, as the Run 1 format is not maintained on the master branch.

```
    $ ./runZJets
```

Important: we recommand to use Run 2 format. The instructions has been tested with this format.

Note: you can specify limit the number of events to analyze by adding to the above command line the
option maxEvents=XXXX. In such case the histogram directory will be named HistoFiles_XXXXevts instead
of HistoFiles.

Producing Data/MC comparison
============================

To produce data/MC comparison plot, you need to run on data, DY+jet and background simulation samples. This can be achieved by runnning the following commads within the DYJets directory. See Quick start to set up the code.

```
cd DYJets
make
./runZJets_newformat doWhat=data
./runZJets_newformat doWhat=dyjets
./runZJets_newformat doWhat=background
./runRecoComparison
```

The plots can then be found in RecoComparison.

The directory will contain a long list of file. The files can be reorganized in subdirectories with the organise_vjets_dir script:

```
cd RecoComparison/PAS_Comparison_DE_13TeV_Data_All_MC_JetPtMin_30_JetEtaMax_24
../../organise_vjets_dir.
```

The script will display errors which can be ignored.

If you are impatient you can run on a subset of events. This can be done by replacing the steps 5 to 8 by the following ones:

```
  ./runZJets_newformat doWhat=data maxEvents=10000
  ./runZJets_newformat doWhat=dyjets maxEvents=10000 mcYieldScale=0.0112369
  ./runZJets_newformat doWhat=background maxEvents=10000 mcYieldScale=0.0112369
  ./runRecoComparison histoDir=HistoFiles_10000evts/
```

Note 1: the mcYieldScale=0.0112369 is required to get the proper normalizations of the MC sample. It is the fraction of data events which has been processed. Use the value displayed during the first step when you run with the doWhat=data and maxEvent=XXXX option if it differs from 0.0112369. The histoDir=HistoFiles_10000evts/ is required because when limiting the number of events, the histogram are stored in a different directory than the default HistoFiles: it is to prevent to delete histograms produced from a long run, when running a short test.

Note 2: the first time you run make, do not use the parallel running option, -j N.  There is a problem when doing it the first time, when the RooUnfold library is not yet compiled. The following times you can use it without problem. Anyway compiling the code is fast enough to not require the -j N option.

Run the unfolding
=================

Before running the unfolding you need to produce the histogran for all the variations of the systematic sources. This can be achived by setting doSysRunning option to 1 either in the vjet.cfg configuration file or on the command line and runing runZJets_newformat:

```
./runZJets_newformat doSysRunning=1
```

Once this stop done the unfolding can be ran with the following command:

```
./runUnfoldingZJets
```

The unfolded result can then be found in the UnfoldedFiles directory and the control plots in the UnfoldingCheck.

Running on the grid
===================

An example of script and configuration file to run the analysis can be found in the directory:
. `job_crab.sh`
. `crabConfig.py`

You need to set the config.Data.outLFNDirBase and config.Site.storageSite to indicate where to store the job output. With the script example, the output of each job is stored as a tarball.

In order to run runZJets_newformat on the grid, you will need to pack the EfficiencyTables directory in a tarball:
```tar czf EfficiencyTables.tgz EfficiencyTables```

Configuration file
==================

Configuration is stored in vjet.cfg. A commented example can be found in example.cfg


Code organization
=================

The DYJets directory contains several runXXX.cc files, which have been compiles in executable called runXXX. Each of these executable runs the code implemented in the class XXX with the corresponding name and defined in Includes/XXX.h and Sources/XXX.cc.  The list of runXXX applications is provided below.

List of applications
====================

Mots of the applications are not only in the untested branch. The master branch contains the application fully validatated for the run 2 format. These applications are runZJets_newformat, runRecoComparison and runUnfoldingZJets.


* Analysis up to reco-level distributions

| runDYJets.cc             | Runs W + jet analysis and Z+jet analysis. Replaced by runZJets for Z+jets. |
| runZJets_newformat.cc    | Runs Z+jet analysis 13TeV with run 2 format |
| runZJets.cc              | Runs Z+jet analysis 13TeV with run 1 format |
| runZJets_original.cc     | Runs Z+jet analysis 8TeV                    |
| runVJets.cc              | Similar to runZJets.cc. Deprecated. |

* Unfolding and channel combination

| runUnfoldingZJets.cc     | Runs unfolding for Z + jet analysis |
| runFinalUnfold.cc        | Performs data distributions unfolding. Deprecated for Z+jets? Used by W+jets? |
| runCombination.cc        | Combination of electron and muon channels. Produce final plots and tables |
| runMergeChannels.cc      | Perform electron and muon channel combination. Deprecated? |

| runFinalUnfoldAndCombination.cc | Performs simultaneous unfolding and combination of both electron and muon channels. Was not used for 8TeV. Unresolved problems with this method. Deprecated. |

* Validation plots and supporting studies

| runCompareUnfolding.cc   | Code missing from the repository. Deprecated?                                        |
| runFSRStudy.cc           | Runs study of effect of FSR of leptons and recovery of FSR with the lepton dressing. |

* Drawing plots and tables

| runFastPlots.cc          | To draw few plots. Requires editing runFastPlots.cc and Sources/FastPlots. Differences with runMyFastPlots.cc |
| runMyFastPlots.cc        | Another runFastPlot.cc. Differences with runFastPlot.cc?                             |

| runIndividual.cc | Produces final plots using runPlotting3RatiosCommon. Differences with runPlotter.cc, runPlotting.c, and runPlotting3Ratios.cc? |
| runPlotter.cc            | Makes final plots. Difference with runPlotting.cc, runIndividual.cc, and runPlotting3Ratios.cc? |
| runPlotting.cc           | Makes final plots. Difference with runPlotter.cc, runIndividual.cc, and runPlotting3Ratios.cc? Deprecated? |
| runPlotting3Ratios.cc    | Makes final plots. Difference with runPlotter.cc, runIndividual.cc, and runPlotting? |

| runMakeStatisticsTable.cc | Produces a latex table with the list of samples and their size. Difference with runStatistics.cc? |
| runStatistics.cc         | Produces a latex table with the list of samples and their size. Difference with runMakeStatisticsTable.cc |

| runRecoComparison.cc     | Produces reco-level comparison plots |

| runSystPlots.cc          | Produces SystematicsPlots_* plots |

* Utilities

| organise_vjets_dir       | Script to organize files in ReoComparison/XXX in a directory tree |
| dispatcher.cc            | Program to launch the analysis using several machine and ssh      |