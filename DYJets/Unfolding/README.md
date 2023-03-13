How to use this unfolding package
=================================

This package is designed to carry out unfolding using the [TUnfold unfolding algorithm](https://www.desy.de/~sschmitt/tunfold.html). It was designed so that unfolding can be carried out with the user only needing to specify the required distributions. All other parameters are either hard-coded for this analysis or are calculated from the distributions. 

When you create an 'Unfolding' object, you specify the following distributions:

**reconstructed:** The reconstructed distribution must have more bins than the true distribution. Studies are ongoing to determine the binning to be used.

**true/gen:** This is the true, or gen-level distribution that the reconstructed distribution should be unfolded to. For this analysis, we are using dressed leptons for the true distribution.

**matrix of migrations:** The matrix of migrations represents the transformation between the true and reconstructed distributions. One must be careful when filling the weights for both true and reco. The general procedure is to do each fill twice: once with the full global weights, and then again in the underflow bin of the reconstructed distribution and with the weights 'gen_weight - global_weight'.

**backgrounds:** If one wants to include background subtraction, backgrounds can be added to the Unfolding object after the object is created.

# An example workflow showing how to create the Unfold object and carry out unfolding with background subtraction using TUnfold:

```
// First create the object with reco, true, and migration matrix 
// reco and true must be TH1F objects
// matrix must be a TH2F object
Unfold*unf = new Unfold(reco,true,matrix);

// For testing purposes, other unfolding methods can be implemented
// so the unfolding type must be specified 
// Unfold::TUNFOLD will be the method used in this analysis
Unfold::UnfoldType unfType = Unfold::TUNFOLD;

// After creating the Unfold object, the background must be set
// TUnfold will subtract this from the reco distribution before unfolding is carried out
// As with reco and true, background must be a TH1F type
unf->SetBackground(background);

// Here is where unfolding is carried out. One must simply specify what type of unfolding is to be used.
// All other needed quantites are determined from the distributions specified in the creation of the object Unfold
unf->EngageUnfolding(unfType);

// If you want to see a plot comparing the unfolded distribution to the true and reco distributions
// Simply use the following function which returns a TCanvas object
TCanvas*canvas = unf->plotUnfolded("canvas","Unfold Test",logplot); 
```

You can also use the macros I have put together to get the histograms output by Shears, plot them, and carry out unfolding on them.

First, create this directory in the Unfolding directory to hold plots:
plots

From the Unfolding directory, execute the following commands:

```
./combine-ROOT-files.sh ${directory_with_shears_output} ${directory_to_save_to} ${decay_channel}
```
combine-ROOT-files.sh doesn't create directories, so create the directory where you want to save the histograms before running it.

Next, you can run all steps from one script
```
DoAll_GetHists_RunUnfolding_MakePlots.sh ${directory_of_histograms} ${analysis_era} ${decay_channel}
```

You can also run any of the individual steps alone using the same three arguments as above for GetHists.sh, RunUnfolding.sh, or MakePlots.sh.

Here's a brief description of each of these scripts:
GetHists.sh opens the root files that were output by Shears and combined using combine-ROOT-files.sh and then extracts the histograms needed for unfolding. 

RunUnfolding.sh loads the histograms saved using GetHists.sh and uses them to carry out a closure test and unfolding on data with background subtraction.

MakePlots.sh then makes a variety of plots. At the moment, only the data vs. monte carlo plot is constructed.
