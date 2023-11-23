# DYJets/Uncertainty

The collection of C++ codes to process the shears output (from `dyjets-loop-syst`) for the systematic uncertainty estimation

## Dependency

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
* CentralValue
  * Produce various plots from shears output
    * Reco-level distribution / unfolded distribution / etc ...

  * Check the distributions first from here before going to the uncertainty estimation!

* Directories for each uncertainty source (EffSF, elE, L1Prefiring ...)
  * Contains codes to estimate the uncertainty

* Summary
  * Collect all uncertainties & covariance matrix -> produce the final plots for each channel (uncertainty vs. mass, differential cross section)


