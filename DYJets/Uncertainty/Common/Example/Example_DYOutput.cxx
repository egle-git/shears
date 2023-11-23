#include "Common/DYOutput.h"

void Example_SampleOutput_16pre(TString inputPath_16pre);
void Example_ProcessOutput_16pre(TString inputPath_16pre);
void Example_EraOutput_16pre(TString inputPath_16pre);
void Example_Run2Output(TString inputPath);
void Example_DYRun2Result(TString inputPath);

void Example_DYOutput() {
  // -- paths where the shears outputs exist
  TString commonPath = "/pnfs/iihe/cms/store/user/kplee/Shears/Example/v230731";
  TString mmPath = commonPath + "/mm";
  // TString eePath = commonPath + "/ee";

  TString mmPath_16pre = mmPath+"/16pre";

  Example_SampleOutput_16pre(mmPath_16pre);
  Example_ProcessOutput_16pre(mmPath_16pre);
  Example_EraOutput_16pre(mmPath_16pre);
  Example_Run2Output(mmPath);
  Example_DYRun2Result(mmPath);
}

void Example_SampleOutput_16pre(TString inputPath_16pre) {
  // -- tag: should be same with the tag used in the root file name
  // -- i.e. root file name = dyjets-[tag].root
  TString tag_DY_M50to100 = "DYJets_M-50to100";

  // -- input arguments:
  // ---- 1) tag (tag_DY_M50to100)
  // ---- 2) directory that contains the .root file
  // ---- 3) luminosity (for normalization)
  SampleOutput output_DY_M50to100(tag_DY_M50to100, inputPath_16pre, LUMI_16pre);

  // -- to get a 1D histogram: use "Get" function
  // -- it will return properly normalized histogram
  TH1D* h_mass = output_DY_M50to100.Get("mass_wide_range_in0jet");
  cout << "[Example_SampleOutput_16pre] dilepton mass histogram, sample = " << tag_DY_M50to100 << endl;
  PlotTool::Print_Histogram(h_mass);
  cout << endl;

  // -- to get a 2D histogram: use "Get2D" function
  // -- it will return properly normalized histogram
  // TH2D* h_migM = output_DY_M50to100.Get2D("mass_wide_range_in0jet-matrix");
  // PlotTool::Print_Histogram2D(h_migM);
}

void Example_ProcessOutput_16pre(TString inputPath_16pre) {
  vector<TString> vec_tag_DY = {
             "DYJets_M-10to50", "DYJets_M-50to100",  
             "DYJets_M-100to200", "DYJets_M-200to400", "DYJets_M-400to500", 
             "DYJets_M-500to700", "DYJets_M-700to800", "DYJets_M-800to1000", 
             "DYJets_M-1000to1500", "DYJets_M-1500to2000", "DYJets_M-2000toInf"};

  TString tag_process = "DY";

  // -- input arguments:
  // ---- 1) tag of the process
  // ---- 2) vector of TString: all tags corresponding to the process (e.g. tags for mass-binned DY samples)
  // ---- 3) directory that contains the .root files
  // ---- 4) luminosity (for normalization)
  ProcessOutput output_DY(tag_process, vec_tag_DY, inputPath_16pre, LUMI_16pre);

  // -- to get a merged 1D histogram: use "Get" function (same with SampleOutput)
  // -- it will return properly normalized histogram
  TH1D* h_mass = output_DY.Get("mass_wide_range_in0jet");
  cout << "[Example_SampleOutput_16pre] dilepton mass histogram, process = " << tag_process << endl;
  PlotTool::Print_Histogram(h_mass);
  cout << endl;

  // -- to get a merged 2D histogram: use "Get2D" function (same with SampleOutput)
  // -- it will return properly normalized histogram
  // TH2D* h_migM = output_DY.Get2D("mass_wide_range_in0jet-matrix");
  // PlotTool::Print_Histogram2D(h_migM);
}

void Example_EraOutput_16pre(TString inputPath_16pre) {
  // -- input arguments:
  // ---- 1) era (16pre, 16post, 17, 18) (to automatically set the luminosity in the class)
  // ---- 2) directory that contains the .root files
  EraOutput output_16pre("16pre", inputPath_16pre);

  // -- set the fake histogram as well
  // TString fileName_fake = "";
  // TString histName_fake = "";
  // output_16pre.Set_Fake(fileName_fake, histName_fake);

  // -- check whether it has the fake histogram
  // Bool_t hasFake = output_16pre.HasFake();

  // -- to get a merged 1D histogram from each process: use "Get" function (same with SampleOutput)
  // -- input arguments:
  // ---- 1) histogram name
  // ---- 2) process (pre-defined inside the class)
  // ------ list: data, DY, VV, singleTop, GG, TauTau, TT, bkgMC, fake, bkgAll
  TH1D* h_mass_DY    = output_16pre.Get("mass_wide_range_in0jet", "DY");
  TH1D* h_mass_data  = output_16pre.Get("mass_wide_range_in0jet", "data");
  TH1D* h_mass_bkgMC = output_16pre.Get("mass_wide_range_in0jet", "bkgMC"); // -- sum of all bkg. MC

  if( output_16pre.HasFake() ) {
    TH1D* h_mass_fake   = output_16pre.Get("mass_wide_range_in0jet", "fake"); // -- fake bkg.
    TH1D* h_mass_bkgAll = output_16pre.Get("mass_wide_range_in0jet", "bkgAll"); // -- sum of all bkg. MC + fake
  }

  // -- to get a merged 2D histogram: use "Get2D" function (same with SampleOutput)
  // -- it will return properly normalized histogram
  TH2D* h_migM = output_16pre.Get2D("mass_wide_range_in0jet-matrix", "DY");
  // PlotTool::Print_Histogram2D(h_migM);
}

void Example_Run2Output(TString inputPath) {
  // -- it will hold EraOutput for all 4 eras in Run2 (16pre, 16post, 17 and 18)
  // -- the directory should have the structure like this:
  // ---- 2016, preAPV result:  inputPath/16pre
  // ---- 2016, postAPV result: inputPath/16post
  // ---- 2017 result:          inputPath/17
  // ---- 2018 result:          inputPath/18
  Run2Output output(inputPath);

  // -- histograms from each era: use EraOutput from Get_EraOutput(era)
  TH1D* h_mass_DY_16pre  = output.Get_EraOutput("16pre").Get("mass_wide_range_inc0jet", "DY");
  TH1D* h_mass_DY_16post = output.Get_EraOutput("16post").Get("mass_wide_range_inc0jet", "DY");
  TH1D* h_mass_DY_17     = output.Get_EraOutput("17").Get("mass_wide_range_inc0jet", "DY");
  TH1D* h_mass_DY_18     = output.Get_EraOutput("18").Get("mass_wide_range_inc0jet", "DY");

  TH2D* h_migM_DY_16pre  = output.Get_EraOutput("16pre").Get2D("mass_wide_range_inc0jet-matrix", "DY");
  TH2D* h_migM_DY_16post = output.Get_EraOutput("16post").Get2D("mass_wide_range_inc0jet-matrix", "DY");
  TH2D* h_migM_DY_17     = output.Get_EraOutput("17").Get2D("mass_wide_range_inc0jet-matrix", "DY");
  TH2D* h_migM_DY_18     = output.Get_EraOutput("18").Get2D("mass_wide_range_inc0jet-matrix", "DY");
}

void Example_DYRun2Result(TString inputPath) {
  Run2Output* output = new Run2Output(inputPath);

  // -- argument: Run2Output*
  DYRun2Result result(output);

  // -- set fake histograms
  // result.Set_Fake("16pre",  "fileName_fake_16pre",  "histName_fake_16pre");
  // result.Set_Fake("16post", "fileName_fake_16post", "histName_fake_16post");
  // result.Set_Fake("17",     "fileName_fake_17",     "histName_fake_17");
  // result.Set_Fake("18",     "fileName_fake_18",     "histName_fake_18");

  // -- change the histogram names from the default name
  // -- useful when the systematic variation is calculated
  // -- first argument: era (16pre, 16post, 17 and 18) or all (change all eras)
  // -- second argument: histType
  // ---- gen_DY, reco_data, reco_DY, reco_bkgMC, migM
  // result.Update_HistName("all", "gen_DY",     "TUnfold1DTrue_inc0jet-pileup_plus");
  // result.Update_HistName("all", "reco_DY",    "TUnfold1DReco_inc0jet-pileup_plus");
  // result.Update_HistName("all", "reco_bkgMC", "TUnfold1DReco_inc0jet-pileup_plus");
  // result.Update_HistName("all", "migM",       "TUnfold2DMig_inc0jet-pileup_plus");

  // -- produce all histograms for DY measurements
  // -- generator level
  // ---- DY: gen_DY
  // -- reconstruction level
  // ---- data: reco_data
  // ---- DY: reco_DY
  // ---- bkgMC: reco_bkgMC
  // ---- bkgAll: reco_bkgAll (bkgMC + fake, if fake is available)
  // -- unfolded level
  // ---- data: unfolded_data
  // ---- DY (closure test): unfolded_DY
  result.Produce();

  // -- "allEra" histograms: histogram with (era, mass) bin number in x-axis (input for TUnfold) or mass bin number (output of TUnfold)
  TH1D* h_allEra_reco_DY       = result.Get_AllEra("reco", "DY");
  TH1D* h_allEra_unfolded_data = result.Get_AllEra("unfolded", "data");
  TH2D* h_allEra_migM = result.Get_AllEra_MigM();
  TH2D* h_allEra_respM = result.Get_AllEra_RespM(); // -- normalized matrix

  TFile* f_result = TFile::Open("DYRun2Result.root", "RECREATE");
  // -- save all intermediate + final histograms
  // ---- h_allEra_gen_DY
  // ---- h_allEra_reco_DY, h_allEra_reco_data, h_allEra_reco_bkgMC, (if exists: h_allEra_reco_fake, h_allEra_reco_bkgAll)
  // ---- h_allEra_migM, h_allEra_respM
  // ---- h_unfolded_data, h_unfolded_DY
  result.Save(f_result);
  // result.Save(f_result, "example"); // -- 2nd argument: tag -> add the tag at the end of the histogram name
  f_result->Close();
}