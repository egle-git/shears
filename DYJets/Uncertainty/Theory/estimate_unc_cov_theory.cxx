#include "Common/ShearsComparator.h"
#include "Common/DYUncertainty.h"
#include "Common/DYPath.h"
#include "Common/DYTool.h"

class UncEstimator_Theory {
public:
  UncEstimator_Theory(TString channel): channel_(channel) {

  }

  void Use_Fake(Bool_t flag = kTRUE) { useFake_ = flag; }

  void FullPhaseSpace(Bool_t flag = kTRUE) { isFPS_ = flag; }

  void EstimateAndSave() {
    if( isFPS_ ) {
      fileName_unfolded_.ReplaceAll(".root", "_FPS.root");
      fileName_unc_.ReplaceAll(".root", "_FPS.root");
    }

    if( gSystem->AccessPathName(fileName_unfolded_) ) {
      cout << fileName_unfolded_ << " does not exist: make it ..." << endl;
      Init();
      ProducePlots_Validation();
      ProducePlots_DYRun2Result_PDFHessian();
      ProducePlots_DYRun2Result_Syst();
      Save_UnfoldedResults();
    }

    TH1::AddDirectory(kFALSE);
    Estimate_Unc_PDFHessian();
    Estimate_Unc_Syst();
    Save_Unc();

    Print_Summary();
  }
private:
  TString channel_ = "";
  Bool_t useFake_ = kTRUE;

  Bool_t isFPS_ = kFALSE; // -- true if the uncertainty is esimated for the full-phase space results

  TString inputPath_ = DYTool::path_systVar_theory;

  TString fileName_unfolded_ = "Unfolded_SystVar_Theory_"+channel_+".root";
  TString fileName_unc_ = "UncAndCov_Theory_"+channel_+".root";

  Run2Output* output_;
  DYRun2Result* result_cv_; // -- central value

  // -- PDF alpha_S variation, scale variation
  std::map<TString, DYRun2Result*> map_result_syst_;

  // // -- PDF stat. replica
  // const Int_t nStatVar_ = 100;
  // vector<DYRun2Result*> vec_result_stat_;

  // -- PDF hessian sets
  const Int_t nPDFHessian_ = 100;
  vector<DYRun2Result*> vec_result_PDFHessian_;

  // -- uncertainties
  std::map<TString, Uncertainty> map_unc_;

  void Init() {
    output_ = new Run2Output(DYTool::path_systVar_theory+"/"+channel_);

    // -- central value
    result_cv_ = new DYRun2Result(output_);
    if( useFake_ ) DYTool::Set_Fake(channel_, result_cv_);
    if( isFPS_ )   DYTool::Set_Acc(result_cv_, "cv");
    result_cv_->Produce();

    // -- systematics
    // -- alpha_S
    map_result_syst_.insert( std::make_pair("PDFVar_101", new DYRun2Result(output_)) );
    map_result_syst_.insert( std::make_pair("PDFVar_102", new DYRun2Result(output_)) );
    // -- scale
    map_result_syst_.insert( std::make_pair("scaleVar_000", new DYRun2Result(output_)) ); // -- (muR,muS) = (0.5, 0.5)
    map_result_syst_.insert( std::make_pair("scaleVar_001", new DYRun2Result(output_)) ); // -- (0.5, 1.0)
    map_result_syst_.insert( std::make_pair("scaleVar_003", new DYRun2Result(output_)) ); // -- (1.0, 0.5)
    // map_result_syst_.insert( std::make_pair("scaleVar_004", new DYRun2Result(output_)) ); // -- (1.0, 1.0) (central value)
    map_result_syst_.insert( std::make_pair("scaleVar_005", new DYRun2Result(output_)) ); // -- (1.0, 2.0)
    map_result_syst_.insert( std::make_pair("scaleVar_007", new DYRun2Result(output_)) ); // -- (2.0, 1.0)
    map_result_syst_.insert( std::make_pair("scaleVar_008", new DYRun2Result(output_)) ); // -- (2.0, 2.0)

    for(auto& pair : map_result_syst_ ) {
      TString tag = pair.first;
      if( useFake_ ) DYTool::Set_Fake(channel_, pair.second);
      // -- change the hist. names for all era at once (i.e. correlation between all eras)
      Update_HistName(pair.second, tag, "all");
      if( isFPS_ ) DYTool::Set_Acc(pair.second, tag); // -- change the acceptance accordingly
      pair.second->Produce();
    }

    // -- PDF hessian sets
    for(Int_t i_var=1; i_var<=nPDFHessian_; ++i_var) { // -- from 1 to 100
      TString tag = TString::Format("PDFVar_%03d", i_var);
      DYRun2Result* result_PDFHessian = new DYRun2Result(output_);
      if( useFake_ ) DYTool::Set_Fake(channel_, result_PDFHessian);
      // -- change the hist. names for all era at once (i.e. correlation between all eras)
      Update_HistName(result_PDFHessian, tag, "all");
      if( isFPS_ ) DYTool::Set_Acc(result_PDFHessian, tag); // -- change the acceptance accordingly
      result_PDFHessian->Produce();

      vec_result_PDFHessian_.push_back( result_PDFHessian );
    } // -- end of PDF hessian set iteration
  }

  void Update_HistName(DYRun2Result* result, const TString tag, const TString era = "all") {
    result->Update_HistName(era, "gen_DY",  "TUnfold1DTrue_inc0jet_"+tag);
    result->Update_HistName(era, "reco_DY", "TUnfold1DReco_inc0jet_"+tag);
    // -- for bkg. MC: update ttbar, single-top and DY->tautau
    // ---- singleTop: except for ST_s-channel (doesn't have 101, 102 variation (alphaS))
    // -- VV, gg: small fraction & no weights are provided (pythia8 samples)
    // ---- their uncertainty will be estimated using the uncertainty on the cross sections
    result->Update_HistName(era, "reco_TT",        "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "reco_TauTau",    "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "reco_singleTop", "ST_t-channel_antitop", "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "reco_singleTop", "ST_t-channel_top",     "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "reco_singleTop", "ST_tW_antitop",        "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "reco_singleTop", "ST_tW_top",            "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "migM",    "TUnfold2DMig_inc0jet_"+tag);
  }

  void ProducePlots_Validation() {
    Run2Output* output_default = new Run2Output(DYTool::path_default+"/"+channel_);
    DYRun2Result* result_default = new DYRun2Result(output_default);
    if( useFake_ ) DYTool::Set_Fake(channel_, result_default);
    if( isFPS_ ) DYTool::Set_Acc(result_default, "cv");
    result_default->Produce();

    DYRun2Result* result_syst_PDF000 = new DYRun2Result(output_);
    if( useFake_ ) DYTool::Set_Fake(channel_, result_syst_PDF000);
    Update_HistName(result_syst_PDF000, "PDFVar_000", "all");
    if( isFPS_ ) DYTool::Set_Acc(result_syst_PDF000, "PDFVar_000");
    result_syst_PDF000->Produce();

    DYRun2Result* result_syst_scaleVar004 = new DYRun2Result(output_);
    if( useFake_ ) DYTool::Set_Fake(channel_, result_syst_scaleVar004);
    Update_HistName(result_syst_scaleVar004, "scaleVar_004", "all");
    if( isFPS_ ) DYTool::Set_Acc(result_syst_scaleVar004, "scaleVar_004");
    result_syst_scaleVar004->Produce();

    ResultComparator comparator(channel_);
    comparator.Remove_RatioError();
    comparator.Set_Case(result_default, "from dyjets-loop");
    comparator.Set_Case(result_cv_,     "from dyjets-loop-syst (default)");
    comparator.Set_Case(result_syst_PDF000,      "from dyjets-loop-syst (systVar, PDF-000)");
    comparator.Set_Case(result_syst_scaleVar004, "from dyjets-loop-syst (systVar, (1.0*#mu_{R}, 1.0*#mu_{F}))");
    comparator.Expect_PerfectAgreement();

    TString plotPath = "DYRun2Result/validation/"+channel_;
    if( isFPS_ ) plotPath.ReplaceAll("DYRun2Result", "DYRun2Result_FPS");
    comparator.Compare(plotPath);
  }

  void ProducePlots_DYRun2Result_Syst() {
    // -- central value vs. alpha_S variation
    ResultComparator comparator_alphaS(channel_);
    comparator_alphaS.Remove_RatioError();
    comparator_alphaS.Set_Case(result_cv_, "Central value (#alpha_{s}=0.118)");
    comparator_alphaS.Set_Case(map_result_syst_["PDFVar_101"], "PDF with #alpha_{s}=0.116");
    comparator_alphaS.Set_Case(map_result_syst_["PDFVar_102"], "PDF with #alpha_{s}=0.120");

    TString plotPath_alphaS = "DYRun2Result/syst/alphaS/"+channel_;
    if( isFPS_ ) plotPath_alphaS.ReplaceAll("DYRun2Result", "DYRun2Result_FPS");
    comparator_alphaS.Compare(plotPath_alphaS);

    // -- central value vs. scale variation
    ResultComparator comparator_scale(channel_);
    comparator_scale.Remove_RatioError();
    comparator_scale.Set_Case(result_cv_, "Central value (1.0*#mu_{R}, 1.0*#mu_{F})");
    comparator_scale.Set_Case(map_result_syst_["scaleVar_000"], "(0.5*#mu_{R}, 0.5*#mu_{F})");
    comparator_scale.Set_Case(map_result_syst_["scaleVar_001"], "(0.5*#mu_{R}, 1.0*#mu_{F})");
    comparator_scale.Set_Case(map_result_syst_["scaleVar_003"], "(1.0*#mu_{R}, 0.5*#mu_{F})");
    comparator_scale.Set_Case(map_result_syst_["scaleVar_005"], "(1.0*#mu_{R}, 2.0*#mu_{F})");
    comparator_scale.Set_Case(map_result_syst_["scaleVar_007"], "(2.0*#mu_{R}, 1.0*#mu_{F})");
    comparator_scale.Set_Case(map_result_syst_["scaleVar_008"], "(2.0*#mu_{R}, 2.0*#mu_{F})");

    TString plotPath_scale = "DYRun2Result/syst/scale/"+channel_;
    if( isFPS_ ) plotPath_scale.ReplaceAll("DYRun2Result", "DYRun2Result_FPS");
    comparator_scale.Compare(plotPath_scale);
  }

  void ProducePlots_DYRun2Result_PDFHessian() {
    ResultComparator comparator(channel_);
    comparator.Remove_RatioError();
    comparator.Set_Case(result_cv_, "Central value");    
    comparator.Set_Case(vec_result_PDFHessian_[0], "PDF Hessian set 1");
    comparator.Set_Case(vec_result_PDFHessian_[1], "PDF Hessian set 2");
    comparator.Set_Case(vec_result_PDFHessian_[2], "PDF Hessian set 3");
    comparator.Set_Case(vec_result_PDFHessian_[3], "PDF Hessian set 4");

    TString plotPath = "DYRun2Result/PDFHessian/"+channel_;
    if( isFPS_ ) plotPath.ReplaceAll("DYRun2Result", "DYRun2Result_FPS");
    comparator.Compare(plotPath);
  }

  void Save_UnfoldedResults() {
    TFile *f_output = TFile::Open(fileName_unfolded_, "RECREATE");
    // -- central value
    result_cv_->Save(f_output, "cv");

    // -- syst. var
    for(auto& pair : map_result_syst_ )
      pair.second->Save(f_output, pair.first);

    // -- PDF hessian var
    // -- PDF_001 ... 100 -> PDFHessian_000 ... 099 (index shift by 1)
    for(Int_t i=0; i<nPDFHessian_; ++i) {
      TString tag = TString::Format("PDFHessian_%03d", i);
      vec_result_PDFHessian_[i]->Save(f_output, tag);
    } // -- end of hessian iteration

    f_output->Close();
  }

  void Estimate_Unc_PDFHessian() {
    TString histName_base = "h_allEra_unfolded_data";
    if( isFPS_ )
      histName_base = "h_allEra_unfoldedFPS_data";

    TH1D* h_cv = PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_cv");

    // vector<TH1D*> vec_altHist;
    vector<Uncertainty> vec_unc_PDFHessian;
    for(Int_t i=0; i<nPDFHessian_; ++i) {
      TString tag = TString::Format("PDFHessian_%03d", i);
      TH1D* h_alt = PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_"+tag);

      Uncertainty unc_PDFHessian("theory_"+tag, "alternative", "fullyCorr");
      unc_PDFHessian.Set_CentralHist( h_cv );
      unc_PDFHessian.Set_AltHist( {h_alt} );
      unc_PDFHessian.Estimate();

      vec_unc_PDFHessian.push_back( unc_PDFHessian );
    }

    Uncertainty unc_PDFHessian_tot("theory_PDFHessian");
    unc_PDFHessian_tot.Combine( vec_unc_PDFHessian );

    map_unc_.insert( std::make_pair("PDFHessian", unc_PDFHessian_tot) );
  }

  void Estimate_Unc_Syst() {
    TString histName_base = "h_allEra_unfolded_data";
    if( isFPS_ )
      histName_base = "h_allEra_unfoldedFPS_data";

    TH1D* h_cv = PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_cv");

    // -- alphaS
    vector<TH1D*> vec_altHist_alphaS = {
      PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_PDFVar_101"),
      PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_PDFVar_102")
    };

    Uncertainty unc_syst_alphaS("theory_syst_alphaS", "alternative", "fullyCorr");
    unc_syst_alphaS.Set_CentralHist( h_cv );
    unc_syst_alphaS.Set_AltHist( vec_altHist_alphaS );
    unc_syst_alphaS.Estimate();

    // -- scale
    vector<TH1D*> vec_altHist_scale = {
      PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_scaleVar_000"),
      PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_scaleVar_001"),
      PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_scaleVar_003"),
      PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_scaleVar_005"),
      PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_scaleVar_007"),
      PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_scaleVar_008")
    };

    Uncertainty unc_syst_scale("theory_syst_scale", "alternative", "fullyCorr");
    unc_syst_scale.Set_CentralHist( h_cv );
    unc_syst_scale.Set_AltHist( vec_altHist_scale );
    unc_syst_scale.Estimate();

    vector<Uncertainty> vec_unc_syst = { unc_syst_alphaS, unc_syst_scale };
    Uncertainty unc_syst_tot("theory_syst_tot");
    unc_syst_tot.Combine( vec_unc_syst );

    map_unc_.insert( std::make_pair("syst_alphaS", unc_syst_alphaS) );
    map_unc_.insert( std::make_pair("syst_scale",  unc_syst_scale) );
    map_unc_.insert( std::make_pair("syst_tot",    unc_syst_tot) );
  }

  void Save_Unc() {
    TFile* f_output = TFile::Open(fileName_unc_, "RECREATE");
    for(auto& pair : map_unc_ ) {
      pair.second.Save(f_output);
    }

    vector<Uncertainty> vec_unc = { map_unc_["PDFHessian"], map_unc_["syst_tot"] };
    Uncertainty unc_tot("theory_tot");
    unc_tot.Combine( vec_unc );
    unc_tot.Save(f_output);

    f_output->Close();
  }

  void Print_Summary() {
    cout << "============ [summary] ============" << endl;
    cout << "[input]" << endl;
    cout << "  shears result: " << inputPath_ << endl;
    cout << "[output]" << endl;
    cout << "  Unfolded results with syst. var.: " << fileName_unfolded_ << endl;
    cout << "  Unc & cov. results: " << fileName_unc_ << endl;
    cout << "===================================" << endl;
  }
};

void estimate_unc_cov_theory_FPS() {
  UncEstimator_Theory estimator_ee("ee");
  estimator_ee.FullPhaseSpace();
  estimator_ee.EstimateAndSave();

  UncEstimator_Theory estimator_mm("mm");
  estimator_mm.FullPhaseSpace();
  estimator_mm.EstimateAndSave();
}

void estimate_unc_cov_theory() {
  UncEstimator_Theory estimator_ee("ee");
  estimator_ee.EstimateAndSave();

  UncEstimator_Theory estimator_mm("mm");
  estimator_mm.EstimateAndSave();

  estimate_unc_cov_theory_FPS();
}