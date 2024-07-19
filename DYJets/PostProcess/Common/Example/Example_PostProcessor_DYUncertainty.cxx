#include "Common/PostProcessor.h"
#include "Common/DYUncertainty.h"

// -- example of using PostProcessor & Uncertainty class
// ---- situation: estimation of uncertainty from the efficiency SF in the muon channel
// ---- 1) ID efficiency, data, statistical uncertainty (smearing)
// ---- 2) ID efficiency, data, systematic uncertainty (+-1sigma shift)
void Example_PostProcessor_DYUncertainty() {
  TString inputPath = "/pnfs/iihe/cms/store/user/kplee/Shears/Example/v230731/mm_systVar_effSF";

  // -- input arguments:
  // -- 1) directory with the .root files
  // -- 2) uncertainty estimation method
  // ---- cv: central value
  // ---- smearing: varied multiple times & take the standard deviation of variations
  // ---- oneSigmaShift: shift by +-1 sigma for the systematic variation
  // ---- alternative: alternative value(s) with a different setup
  PostProcessor_EffSF pp_effSF_cv(inputPath, "cv", "ID-data-stat");
  PostProcessor_EffSF pp_effSF_stat(inputPath, "smearing", "ID-data-stat"); // -- 100 results from 100 maps varied within its stat. unc.
  PostProcessor_EffSF pp_effSF_syst(inputPath, "oneSigmaShift", "ID-data-syst_all");

  // -- fake histograms can be set
  // pp_effSF_cv.Set_Fake("16pre",  "fileName_fake_16pre",  "histName_fake_16pre");
  // pp_effSF_cv.Set_Fake("16post", "fileName_fake_16post", "histName_fake_16post");
  // pp_effSF_cv.Set_Fake("17",     "fileName_fake_17",     "histName_fake_17");
  // pp_effSF_cv.Set_Fake("18",     "fileName_fake_18",     "histName_fake_18");
  // -- do same for pp_effSF_stat and pp_effSF_syst as well

  // -- produce up to the final results & save in a file
  // -- it can take a few mins as it requires >100 unfolding
  TString fileName_result = "Example_PostProcessor_DYUncertainty_Result.root";
  if( gSystem->AccessPathName(fileName_result) ) { // -- if the file doesn't exist
    TFile *f_result = TFile::Open(fileName_result, "RECREATE");
    pp_effSF_cv.ProcessAndSave(f_result);
    pp_effSF_stat.ProcessAndSave(f_result);
    pp_effSF_syst.ProcessAndSave(f_result);
    f_result->Close();
  }

  // -- get the vector of histogram for the final result (h_allEra_unfolded_data)
  // -- "GetVector_Hist" method does not depend on "ProcessAndSave"
  TH1D* h_cv = pp_effSF_cv.GetVector_Hist(fileName_result)[0];
  // -- you can get a different type of histograms by providing its name
  // ---- e.g. pp_effSF_cv.GetVector_Hist(fileName_result, "h_allEra_unfolded_DY");

  // -- it will contain 100 unfolded results with stat. variation
  vector<TH1D*> vec_altHist_stat = pp_effSF_stat.GetVector_Hist(fileName_result);

  // -- it will contain 2 unfolded results with +1sigma & -1sigma variation
  vector<TH1D*> vec_altHist_syst = pp_effSF_syst.GetVector_Hist(fileName_result);

  // -- input arguments:
  // ---- 1) tag for the uncertainty
  // ---- 2) uncertainty estimation method
  // ------ oneSigmaShift, alternative, smearing, null
  // ---- 3) covariance estimation method
  // ------ fullyCorr, unCorr, smearing (only when 2) is also smearing), null
  Uncertainty unc_stat = Uncertainty("effSF-mu-ID-data-stat", "smearing", "smearing");
  unc_stat.Set_CentralHist( h_cv );
  unc_stat.Set_AltHist( vec_altHist_stat );
  unc_stat.Estimate(); // -- produce absUnc, relUnc, covM, corrM

  Uncertainty unc_syst = Uncertainty("effSF-mu-ID-data-syst", "oneSigmaShift", "fullyCorr");
  unc_syst.Set_CentralHist( h_cv );
  unc_syst.Set_AltHist( vec_altHist_syst );
  unc_syst.Estimate();

  vector<Uncertainty> vec_unc_subSource = { unc_stat, unc_syst };

  // -- total uncertainty
  // -- combine all uncertainties with the quadrature sum & add covariance matrix & re-calculate correlation matrix
  Uncertainty unc_total = Uncertainty("effSF-mu-ID-data-total");
  unc_total.Combine( vec_unc_subSource );

  // -- get results
  TH1D* h_absUnc_total = unc_total.AbsUnc();
  TH1D* h_relUnc_total = unc_total.RelUnc();
  TH2D* h_covM_total   = unc_total.CovM();
  TH2D* h_corrM_total  = unc_total.CorrM();

  // -- save all results: absUnc, relUnc, covM, corrM
  // -- you can draw their plots with the tools in SimplePlotTools.h
  TFile* f_unc = TFile::Open("Example_PostProcessor_DYUncertainty_Unc.root", "RECREATE");
  unc_stat.Save(f_unc);
  unc_syst.Save(f_unc);
  unc_total.Save(f_unc);
}