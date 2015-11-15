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

To use as input ntuple in the Run I format, run `runZJets` instead of `runZJets_newformat`.

```
    $ ./runZJets
```

Note: you can specify limit the number of events to analyze by adding to the above command line the 
option maxEvents=XXXX. In such case the histogram directory will be named HistoFiles_XXXXevts instead
of HistoFiles.

Configuration file
==================

Configuration is stored in vjet.cfg. A commented example can be found in example.cfg


Code organization
=================

The DYJets directory contains several runXXX.cc files, which have been compiles in executable called runXXX. Each of these executable runs the code implemented in the class XXX with the corresponding name and defined in Includes/XXX.h and Sources/XXX.cc.  The list of runXXX applications is provided below.


List of applications
--------------------

. Analysis up to reco-level distributions

| runDYJets.cc             | Runs W + jet analysis and Z+jet analysis. Replaced by runZJets for Z+jets. |
| runZJets.cc              | Runs Z+jet analysis 13TeV           |
| runZJets_original.cc     | Runs Z+jet analysis 8TeV            |
| runVJets.cc              | Similar to runZJets.cc. Deprecated. |

. Unfolding and channel combination

| runUnfoldingZJets.cc     | Runs unfolding for Z + jet analysis |
| runFinalUnfold.cc        | Performs data distributions unfolding. Deprecated for Z+jets? Used by W+jets? |
| runCombination.cc        | Combination of electron and muon channels. Produce final plots and tables |
| runMergeChannels.cc      | Perform electron and muon channel combination. Deprecated? |

| runFinalUnfoldAndCombination.cc | Performs simultaneous unfolding and combination of both electron and muon channels. Was not used for 8TeV. Unresolved problems with this method. Deprecated. |

. Validation plots and supporting studies

| runCompareUnfolding.cc   | Code missing from the repository. Deprecated?  |
| runFSRStudy.cc           | Runs study of effect of FSR of leptons and recovery of FSR with the lepton dressing. |

. Drawing plots and tables

| runFastPlots.cc          | To draw few plots. Requires editing runFastPlots.cc and Sources/FastPlots. Differences with runMyFastPlots.cc |
| runMyFastPlots.cc        | Another runFastPlot.cc. Differences with runFastPlot.cc? |

| runIndividual.cc | Produces final plots using runPlotting3RatiosCommon. Differences with runPlotter.cc, runPlotting.c, and runPlotting3Ratios.cc? |
| runPlotter.cc            | Makes final plots. Difference with runPlotting.cc, runIndividual.cc, and runPlotting3Ratios.cc? |
| runPlotting.cc           | Makes final plots. Difference with runPlotter.cc, runIndividual.cc, and runPlotting3Ratios.cc? Deprecated? | 
| runPlotting3Ratios.cc    | Makes final plots. Difference with runPlotter.cc, runIndividual.cc, and runPlotting? |

| runMakeStatisticsTable.cc | Produces a latex table with the list of samples and their size. Difference with runStatistics.cc? |
| runStatistics.cc         | Produces a latex table with the list of samples and their size. Difference with runMakeStatisticsTable.cc |

| runRecoComparison.cc     | Produces reco-level comparison plots |

| runSystPlots.cc          | Produces SystematicsPlots_* plots | 


