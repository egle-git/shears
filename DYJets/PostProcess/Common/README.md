

# DYJets/Uncertainty/Common

Collection of codes commonly used in the uncertainty estimation

* General code (not specific to shears)
  * `SimplePlotTools.h`: general plotting functions and classes
    * Dependency: just ROOT
  * `DYPath.h`: includes all paths to the results (e.g. shears output) used in Uncertainty estimation code
    * You can control which results will be used in the uncertainty estimation
    * Dependency: just ROOT
* Post-processing codes
  * `DYOutput.h`: handle the outputs from shears & make it easy to get normalized & merged histograms
    * Dependency: `SimplePlotTools.h`
  * `DYUncertainty.h`: a general class to estimate the uncertainty depending on its uncertainty estimation method (1 sigma shift, smearing, etc)
    * Dependency: `DYOutput.h`
  * `DYTool.h`: useful functions for DY analysis under the namespace `DYTool`
    * Dependency: `SimplePlotTools.h`', `DYPath.h`, `DYOutput.h`
* Plotting code
  * `ShearsComparator.h`: produce comparison plots from multiple shears outputs, including automatic calculation up to the unfolded results
    * Dependency: `DYOutput.h`

## SimplePlotTools.h

Collection of useful functions and classes to easily produce plots

### Namespace

 `PlotTool`

### Classes

NOTE: example codes to use these classes are available under `Example/SimplePlotTools` directory: 
you can copy & paste them whenever you need them

* `HistCanvas`
* `HistCanvaswRatio`
* `HistStackCanvaswRatio`
* `Hist2DCanvas`
* `GraphCanvas`
* `GraphCanvaswRatio`

For all above classes, basic structure is same:

```
TString canvasName = "c_variable";
PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
canvas->SetTitle("var", "# entry");

canvas->Register(h1, "1st hist", kBlack);
canvas->Register(h2, "2nd hist", kRed);

canvas->SetAutoRangeY();
// canvas->SetRangeX(minX, maxX);
// canvas->SetRangeY(minY, maxY);

canvas->Latex_CMSInternal();
canvas->Latex_LumiEnergy(10.0, 13);
canvas->RegisterLatex(0.16, 0.91, 42, 0.6, "Test");
// -- same with canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Test}}");

canvas->Draw();
```

### Functions

* `TH1D* Get_Hist(TString fileName, TString histName, TString histName_new = "")`
  * Get a 1D histogram in a .root file

* `TH2D* Get_Hist2D(TString fileName, TString histName, TString histName_new = "" )`
  * Get a 2D histogram in a .root file

* `TGraphAsymmErrors* Get_Graph(TString fileName, TString graphName, TString graphName_New = "" )`
  * Get a graph in a .root file

* `TH1D* DivideEachBin_ByBinWidth( TH1D* h, TString HistName = "" )`
  * Divide each bin content by its width
  * Useful when the histogram is converted from cross section -> differential cross section

* `TH1D* MultiplyEachBin_byBinWidth( TH1D* h, TString HistName = "" )`
  * Multiply each bin content by its width
  * Useful when the histogram is converted from differnetial cross section -> cross section

* `Bool_t IsRatio1( TH1D* h1, TH1D* h2)`
  * Print whether two histograms are exactly same (the ratio of two is exactly 1.0)

* `TH1D* Convert_GraphToHist( TGraphAsymmErrors *g )`
  * Convert a graph to histogram
  * Asymmetric error on a point: take the larger error when it is converted to the bin error in the histogram

* `void Print_Histogram( TH1D* h, Bool_t NegativeCheck = kFALSE )`
  * Print the histogram information bin by bin (bin content, error, relative error, etc)

* `void Print_Histogram2D( TH2D* h2D )`
  * Print the 2D histogram information bin by bin (bin content, error, relative error, etc)

* `TH1D* HistOperation(TString histName, TH1D* h1, TH1D* h2, TString operation)`
  * operation: `"+", "-", "*", "/"`
  * Warning: curerntly, the errors are not properly propagated now (the error is set to 0)

* `TH2D* Hist2DOperation(TString histName, TH2D* h1, TH2D* h2, TString operation)`
  * same with above, but for 2D
  * Warning: curerntly, the errors are not properly propagated now (the error is set to 0)

* `TH1D* MakeHist_fromVector(TString histName, vector<Double_t> vec_binEdge)`
  * make TH1D* using vector, not array for binning

* `TH2D* MakeHist2D_fromVector(TString histName, vector<Double_t> vec_binEdgeX, vector<Double_t> vec_binEdgeY)`
  * make TH2D* using vector, not array for binning
* `TH1D* QuadSum_Hist(vector<TH1D*> vec_hist)`
  * Return the histogram with the quadrature sum of each bin values in `vec_hist`
  * Useful to calculate the total uncertainty of uncorrelated sources

## DYOutput.h

Check `Example/Example_DYOutput.cxx` file for the technical details

### Global variables

* `LUMI_16pre`, `LUMI_16post`, `LUMI_17` and `LUMI_18` variables: total integrated luminosity of each era

### Classes 

* `SampleOutput`
  * Read a .root file from shears & Provide the hitsogram with the proper normalization
* `ProcessOutput`
  * collcection of `SampleOutput`: automatically merge all `SampleOutput` with the same physics process (e.g. DY mass-binned samples -> merged into one output)
* `EraOutput`
  * Collection of `ProcessOutput`: it has all `ProcessOuptut` in an era (data, DY, ttbar, etc)
  * You can add `fake` histogram as well
* `Run2Output`
  * Collection of 4 era's (16pre, 16post, 17 and 18) `EraOutput`
  * You can add `fake` histogram per each era as well

* `DYRun2Result`
  * Essential results (histograms) for this DY measurement (1D, vs. mass)
    * gen-level: DY
    * reco-level: data, DY, bkg. MC, fake, All bkg, data after bkg. subtraction
    * unfolded level: data, DY

## DYUncertainty.h

Check `Example/Example_DYUncertainty.cxx` file for the technical details

### Classes

* `Uncertainty`
  * General class to estimate the uncertainty & covariance between bins depending on the esitmation methods
  * Input: central value & alternative values with the systematic variation
  * Output: uncertainties & covariance/correlation matrices

