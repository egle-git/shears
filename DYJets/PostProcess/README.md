# DYJets/PostProcess

The collection of codes for **post-processing** the output `.root` files from `dyjets-loop` or relevant outputs

**Run `source setup.sh` first to use any codes here!**

* It will set a few environment variables used in the codes

## Analysis codes

* `Acceptance`
  * Input: root files from a custom `EDAnalyzer` running on miniAOD (details: `Acceptance/README.md`)
  * Output
    * a root file with the acceptance to the full phase space + associated systematic uncertainties
    * a root file with the theory prediction from Powheg-MiNNLO (fiducial & full phase space)
* `Uncertainty`
  * Input: `dyjets-loop` output root files for full Run-2 data
  * Output: various root files with systematic uncertainties from each source & cross section results
* `Combination`
  * Input: root files from `Uncertainty`
  * Output: root files with combined results between two channels for both fiducial & full phase space

## Utility codes

* `Common`
  * Set of functions & classes to read the output from `dyjets-loop`, analyze, and drawing plots
    (details: `Common/README.md`)

## Output directory

* `Output` (not included in repository)
  * The directory where all outputs from the codes in `PostProcess` are saved
    * root files
    * plots (saved under `Output/Plot/SubDir`)
  * **Set to be ignored in the commit** (in `.gitignore`)
    * The size of the output is too large
    * It can directly be copied to `/pnfs` directory as a backup



