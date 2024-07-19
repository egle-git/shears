#include "Common/ShearsComparator.h"

void Example_EraComparator();
void Example_ResultComparator();

void Example_ShearsComparator() {
  Example_EraComparator();
  Example_ResultComparator();
}

void Example_EraComparator() {
  TString commonPath = "/pnfs/iihe/cms/store/user/kplee/Shears/Example/v230731";
  TString mmPath_before_16pre = commonPath + "/mm_buggy/16pre";
  TString mmPath_after_16pre = commonPath + "/mm/16pre";

  // -- input arguments:
  // ---- 1) era (16pre, 16post, 17, 18)
  // ---- 2) channel (mm, ee)
  EraComparator comparator("16pre", "mm");
  comparator.Set_Case(mmPath_before_16pre, "before bug-fix"); // -- first case: reference (denominator in the ratio plot)
  comparator.Set_Case(mmPath_after_16pre,  "after bug-fix");
  // comparator.Set_Case(path_3rdCase,  "3rd case"); // -- you can add more cases

  // -- add fake if available
  // vector<TString> vec_fileName_fake = {"fileName_fake_case1", "fileName_fake_case2"};
  // vector<TString> vec_histName_fake = {"histName_fake_case1", "histName_fake_case2"};
  // comparator.Set_Fake(vec_fileName_fake, vec_histName_fake);

  // comparator.Add_Comparison( "mass_wide_range_inc0jet-gen" );
  comparator.Add_Comparison( "mass_wide_range_inc0jet" );
  comparator.Add_Comparison( "pt_inc0jet" );
  // -- you can use different histogram name for a comparison: give the name as vector in the order of case provided
  // comparator.Add_Comparison( {"mass_wide_range_inc0jet", "mass_wide_range_inc0jet"} );

  // comparator.OnlyMC(); // -- compare MC histogram only

  // comparator.Expect_PerfectAgreement(); // -- add test ratio == 1

  // -- it will produce comparison plots under the directory plot/16pre
  // -- plots (for each comparison)
  // ---- comparison between cases: DY, bkgMC, data (+bkgAll if fake bkg. is provided)
  // ---- data vs. MC stack histogram per each case
  comparator.Compare();
}

void Example_ResultComparator() {
  TString commonPath = "/pnfs/iihe/cms/store/user/kplee/Shears/Example/v230731";
  TString mmPath_before = commonPath + "/mm_buggy";
  TString mmPath_after  = commonPath + "/mm";

  Run2Output* output_before = new Run2Output(mmPath_before);
  Run2Output* output_after  = new Run2Output(mmPath_after);

  DYRun2Result* result_before = new DYRun2Result(output_before);
  DYRun2Result* result_after = new DYRun2Result(output_after);

  // -- change histogram name if necessary
  // TString tag_before = "";
  // result_before->Update_HistName("gen_DY",     "TUnfold1DTrue_inc0jet_"+tag_before);
  // result_before->Update_HistName("reco_DY",    "TUnfold1DReco_inc0jet_"+tag_before);
  // result_before->Update_HistName("reco_bkgMC", "TUnfold1DReco_inc0jet_"+tag_before);
  // result_before->Update_HistName("migM",       "TUnfold2DMig_inc0jet_"+tag_before);

  // TString tag_after = "";
  // result_after->Update_HistName("gen_DY",     "TUnfold1DTrue_inc0jet_"+tag_after);
  // result_after->Update_HistName("reco_DY",    "TUnfold1DReco_inc0jet_"+tag_after);
  // result_after->Update_HistName("reco_bkgMC", "TUnfold1DReco_inc0jet_"+tag_after);
  // result_after->Update_HistName("migM",       "TUnfold2DMig_inc0jet_"+tag_after);

  // -- TO-DO: method to include fake histograms

  result_before->Produce();
  result_after->Produce();

  ResultComparator comparator("mm");
  comparator.Set_Case(result_before, "before bug-fix"); // -- first case: reference (denominator of the ratio)
  comparator.Set_Case(result_after, "after bug-fix");
  // comparator.Set_Case(result_3rdCase, "3rd case"); // -- more case can be added

  // comparator.Expect_PerfectAgreement(); // -- if you want to check isRatio==1

  // comparator.Remove_RatioError(); // -- if you want to remove the error in the ratio plot

  TString subDir = "example";
  // -- produce comparison plots for all DY results under "./plot/subDir"
  // ---- h_allEra_gen_DY
  // ---- h_allEra_reco_DY, h_allEra_reco_data, h_allEra_reco_bkgMC, h_allEra_reco_data_bkgSub
  // ---- h_allEra_unfolded_DY, h_allEra_unfolded_data
  comparator.Compare(subDir);
}