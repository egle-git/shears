# DYJets/Uncertainty

The collection of C++ codes to process the shears output (from `dyjets-loop-syst`) for the systematic uncertainty estimation and combination (i.e. all post-process steps)

To use the code,

1. *the paths in `Common/DYPath.h` should be updated with your paths accordingly!*
1. ```source setup.sh```

## Dependency

In short, you can run all codes here under your laptop if you have ROOT6

* Only `ROOT` is mandatory including the latest `TUnfold` version
  * `ROOT` version >= 6.XX

* `LCG 100` environment is **not** mandatory
  * To make the code be able to be run in a local machine as well (i.e. personal laptop)
    (no code under this directory requires high computing power)

* **Independent** with the source codes in shears
  * i.e. no need to do `make` when the codes under this directory is modified
  * The codes here are running without compile

## Directories

* Common
  * Collection of common classes used in the uncertainty estimation procedure
    * e.g. plotting, post-processing of shears output, uncertainty calculation, common paths, etc
* Validation
  * Technical validations (e.g. the distribution from `dyjets-loop` vs. `dyjets-loop-syst`, etc)
  * It **should be checked first** before proceeding the next steps

* CentralValue
  * Produce various plots from shears output
    * Reco-level distribution / unfolded distribution / etc ...

  * Check the distributions first from here before going to the uncertainty estimation!
* Directories for each uncertainty source (EffSF, elE, L1Prefiring ...)
  * Contains codes to estimate the uncertainty
* Summary
  * Collect all uncertainties & covariance matrix -> produce the final plots for each channel (uncertainty vs. mass, differential cross section)
* Combination
  * Perform the combination between two channels
  * Input: root files made in `Summary` directory


## Scripts

`pyscript_getOutput.py`: script to collect all root files in each directory (to save the results) or distribute all root files from a given input directory to individual directories (if you want to retrieve results)

* Collect all root files: `python3 pyscript_getOutput.py -m collect -o <output directory name>`
  * Default output directory name: `result_(date)`, e.g. `result_231212`
* Distributes root files: ``python3 pyscript_getOutput.py -m distribute -i <input directory path>`
