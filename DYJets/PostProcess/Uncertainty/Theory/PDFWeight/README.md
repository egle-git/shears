# DYJets/Uncertainty/Theory/PDFWeight

Codes to produce the PDF weight (ratio of alt. weight to the nominal weight) distribution

* `WeightInfo.h`
  * Header file for the classes used to obtain the PDF weight distribution
  * Read the shears output defined in `DYTool::path_systVar_theory`
  * Used in the below ROOT marcos

* `CheckWeightInfo.cxx`
  * Produce `.root` files, which are the input for `dyjets-loop-syst` for the theory uncertainty estimation
  * Contains the histograms of mean and sigma of the weight distribution per each PDF hessian set
* `producePlot_weight.cxx`
  * Produce the plots for the mean and sigma distribution vs. each PDF set
    * For each sample
* `MakePyScript_WeightInfo.cxx`
  * Produce a .py file, which is the input for the acceptance calculation on miniAOD