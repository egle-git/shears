How to use this unfolding package
=================================

This package is designed to carry out unfolding using the [TUnfold unfolding algorithm](https://www.desy.de/~sschmitt/tunfold.html). It was designed so that unfolding can be carried out with the user only needing to specify the required distributions. All other parameters are either hard-coded for this analysis or are calculated from the distributions. There is still some functionality to be added. For example: being able to specifiy a correlation matrix for the inputs. 

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

First, create the following directories in the Unfolding directory:
histograms/fromShears
histograms/unfolding
plots

From the Unfolding directory, execute the following commands:

This script will take all the histograms that were output by shears and will combine them using hadd into one script per sample type. These are saved in Unfolding/histograms/fromShears.
```
./combine-ROOT-files.sh ${directory_with_shears_output}
```

This script will take the invariant mass histograms used for unfolding from the files created in the last step. 
```
root macros/getUnfoldingHistograms.C
```    

This script will make the following plots: data vs. monte carlo, migration matrix, and a comparison between the migration matrix projections and the reco and true distributions.
```
root macros/makePlots.C
```

Now we can unfold the distributions. This script carries out the unfolding and outputs the following plots: unfolding closure, unfolding data, response matrix. The histograms will be saved in histograms/unfolding/unfolding_output.root
```
root macros/unfold.C
```
I also just added a new bash script called HitIt.sh, which will carry out all steps listed here to combine root files, retrieve histograms for unfolding, carry out unfolding, and make plots. HitIt.sh requires one arument, the name of the directory located in the DYJets directory, which contains the root files output from running the analysis via Condor.
