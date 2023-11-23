#include "Common/ShearsComparator.h"
#include "Common/DYUncertainty.h"
#include "Common/DYPath.h"

class UncEstimator_ElE {
public:
  UncEstimator_ElE() { }

  void EstimateAndSave() {
    TH1::AddDirectory(kFALSE);

    Init();

    Validation_ScaleUpDown();
    Validation_SmearingUpDown();

    EstimateUnc_Scale();
    EstimateUnc_Smearing();

    Save_All();
  }

private:
  Run2Output* output_;
  // -- (uncType, DYRun2Result) pair
  std::map<TString, DYRun2Result*> map_result_;

  // -- (uncType, Uncertainty) pair
  std::map<TString, Uncertainty> map_unc_;

  void Init() {
    cout << "Input shears results: " << DYTool::path_systVar_elE << endl;
    output_ = new Run2Output(DYTool::path_systVar_elE);

    // -- central value
    DYRun2Result* result_cv = new DYRun2Result(output_);
    result_cv->Produce();
    map_result_.insert( std::make_pair("cv", result_cv) );

    // -- scale up and down (data-only)
    vector<TString> vec_tag_scale = {"elE_scale_up", "elE_scale_down"};
    for( const auto& tag : vec_tag_scale ) {
      DYRun2Result* result = new DYRun2Result(output_);
      result->Update_HistName("all", "reco_data", "TUnfold1DReco_inc0jet_"+tag);
      result->Produce();
      map_result_.insert( std::make_pair(tag, result) );
    }

    // -- smearing up and down (MC-only)
    vector<TString> vec_tag_smearing = {"elE_smearing_up", "elE_smearing_down"};
    for( const auto& tag : vec_tag_smearing ) {
      DYRun2Result* result = new DYRun2Result(output_);
      result->Update_HistName("all", "gen_DY",     "TUnfold1DTrue_inc0jet_"+tag);
      result->Update_HistName("all", "reco_DY",    "TUnfold1DReco_inc0jet_"+tag);
      result->Update_HistName("all", "reco_bkgMC", "TUnfold1DReco_inc0jet_"+tag);
      result->Update_HistName("all", "migM",       "TUnfold2DMig_inc0jet_"+tag);
      result->Produce();
      map_result_.insert( std::make_pair(tag, result) );
    }
  }

  void Validation_ScaleUpDown() {
    ResultComparator comparator("ee");
    comparator.Remove_RatioError();
    comparator.Set_Case(map_result_["cv"],             "Central value");
    comparator.Set_Case(map_result_["elE_scale_up"],   "Scale +1#sigma (data-only)");
    comparator.Set_Case(map_result_["elE_scale_down"], "Scale -1#sigma (data-only)");
    comparator.Compare("validation/scale");
  }

  void Validation_SmearingUpDown() {
    ResultComparator comparator("ee");
    comparator.Remove_RatioError();
    comparator.Set_Case(map_result_["cv"],                "Central value");
    comparator.Set_Case(map_result_["elE_smearing_up"],   "Smearing +1#sigma (MC-only)");
    comparator.Set_Case(map_result_["elE_smearing_down"], "Smearing -1#sigma (MC-only)");
    comparator.Compare("validation/smearing");
  }

  void EstimateUnc_Scale() {
    TH1D* h_cv       = map_result_["cv"]->Get_AllEra("unfolded", "data");
    TH1D* h_alt_up   = map_result_["elE_scale_up"]->Get_AllEra("unfolded", "data");
    TH1D* h_alt_down = map_result_["elE_scale_down"]->Get_AllEra("unfolded", "data");
    
    Uncertainty unc("elE_scale", "oneSigmaShift", "fullyCorr");
    unc.Set_CentralHist( h_cv );
    unc.Set_AltHist( {h_alt_up, h_alt_down} );
    unc.Estimate();
    
    map_unc_.insert( std::make_pair("elE_scale", unc) );
  }

  void EstimateUnc_Smearing() {
    TH1D* h_cv       = map_result_["cv"]->Get_AllEra("unfolded", "data");
    TH1D* h_alt_up   = map_result_["elE_smearing_up"]->Get_AllEra("unfolded", "data");
    TH1D* h_alt_down = map_result_["elE_smearing_down"]->Get_AllEra("unfolded", "data");
    
    Uncertainty unc("elE_smearing", "oneSigmaShift", "fullyCorr");
    unc.Set_CentralHist( h_cv );
    unc.Set_AltHist( {h_alt_up, h_alt_down} );
    unc.Estimate();
    
    map_unc_.insert( std::make_pair("elE_smearing", unc) );
  }

  void Save_All() {
    TString fileName_output = "UncAndCov_elE.root";
    TFile *f_output = TFile::Open(fileName_output, "RECREATE");
    f_output->cd();
    // -- save results
    for(auto& pair : map_result_ )
      pair.second->Save(f_output, pair.first);

    // -- save uncertainties
    vector<Uncertainty> vec_unc;
    for(auto& pair : map_unc_ ) {
      pair.second.Save(f_output);

      vec_unc.push_back( pair.second );
    }

    // -- save total uncertainty
    Uncertainty unc_tot("elE_tot");
    unc_tot.Combine( vec_unc );
    unc_tot.Save(f_output);

    f_output->Close();
    cout << "All results are saved in " << fileName_output << endl;
  }

};

void estimate_unc_cov_elE() {
  UncEstimator_ElE uncEstimator;
  uncEstimator.EstimateAndSave();
}