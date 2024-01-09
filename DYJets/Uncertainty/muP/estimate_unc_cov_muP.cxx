#include "Common/ShearsComparator.h"
#include "Common/DYUncertainty.h"
#include "Common/DYPath.h"
#include "Common/DYTool.h"

class UncEstimator_MuP {
public:
  UncEstimator_MuP() { }

  void Use_Fake(Bool_t flag = kTRUE) { useFake_ = flag; }

  void EstimateAndSave() {
    if( gSystem->AccessPathName(fileName_unfolded_) ) {
      cout << fileName_unfolded_ << " does not exist: make it ..." << endl;
      Init();
      ProducePlots_Validation();

      for(const auto& era : vec_era_ )
        ProducePlots_DYRun2Result_Stat(era);
      ProducePlots_DYRun2Result_Syst();
      Save_UnfoldedResults();
    }
    else
      cout << "Use the existing file ("<< fileName_unfolded_ << ") ..." << endl;

    if( !gSystem->AccessPathName(fileName_unc_) ) {
      cout << fileName_unc_ << " already exists: move or remove it to re-estimate the uncertaintiy" << endl;
      return;
    }

    TH1::AddDirectory(kFALSE); // -- seg. fault without this (why? it is already used in PlotTool::Get_Hist...)
    Estimate_Unc_Stat();
    Estimate_Unc_Syst();
    Save_Unc();

    Print_Summary();
  }

private:
  Run2Output* output_;
  // -- (setX, DYRun2Result) pair
  std::map<TString, DYRun2Result*> map_result_syst_;

  Bool_t useFake_ = kTRUE; // -- default: true

  const Int_t nStatVar_ = 100;
  // -- (era, vector<DYRun2Result*>) pair
  std::map<TString, vector<DYRun2Result*>> map_result_stat_;

  std::map<TString, Uncertainty> map_unc_;
  vector<TString> vec_era_ = {"16pre", "16post", "17", "18"};

  TString fileName_unfolded_ = "Unfolded_SystVar_muP.root";
  TString fileName_unc_      = "UncAndCov_muP.root";

  void Init() {
    TString shearsPath = DYTool::path_systVar_muP+"/mm";
    cout << "Input shears results: " << shearsPath << endl;
    output_ = new Run2Output(shearsPath);
    
    // -- systematics
    map_result_syst_.insert( std::make_pair("set0", new DYRun2Result(output_)) );
    map_result_syst_.insert( std::make_pair("set2", new DYRun2Result(output_)) );
    map_result_syst_.insert( std::make_pair("set3", new DYRun2Result(output_)) );
    map_result_syst_.insert( std::make_pair("set4", new DYRun2Result(output_)) );
    map_result_syst_.insert( std::make_pair("set5", new DYRun2Result(output_)) );

    for(auto& pair : map_result_syst_ ) {
      TString tag = "muP_"+pair.first;
      if( useFake_ ) DYTool::Set_Fake("mm", pair.second);
      // -- change the hist. names for all era at once (i.e. correlation between all eras)
      Update_HistName(pair.second, tag, "all");
      pair.second->Produce();
    }

    // -- stat. replicas (set1_XXX) per era    
    for(const auto& era : vec_era_ ) {
      vector<DYRun2Result*> vec_result;

      for(Int_t i_var=0; i_var<nStatVar_; ++i_var) {
        TString tag = TString::Format("muP_set1_%03d", i_var);
        DYRun2Result* result_stat = new DYRun2Result(output_);
        if( useFake_ ) DYTool::Set_Fake("mm", result_stat);
        // -- change the hist. name for a given era only (i.e. no correlation between era)
        Update_HistName(result_stat, tag, era);
        result_stat->Produce();

        vec_result.push_back( result_stat );
      } // -- end of stat. replica iteration

      map_result_stat_.insert( std::make_pair(era, vec_result) );
    }// -- end of era iteration
  }

  void Update_HistName(DYRun2Result* result, const TString tag, const TString era = "all") {
    // -- data is also changed with the variation
    result->Update_HistName(era, "reco_data",  "TUnfold1DReco_inc0jet_"+tag);

    result->Update_HistName(era, "gen_DY",     "TUnfold1DTrue_inc0jet_"+tag);
    result->Update_HistName(era, "reco_DY",    "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "reco_bkgMC", "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "migM",       "TUnfold2DMig_inc0jet_"+tag);
  }

  void ProducePlots_Validation() {
    Run2Output* output_default = new Run2Output(DYTool::path_default+"/mm");
    DYRun2Result* result_default = new DYRun2Result(output_default);
    result_default->Produce();

    DYRun2Result* result_default_syst = new DYRun2Result(output_);
    result_default_syst->Produce();

    ResultComparator comparator("mm");
    comparator.Remove_RatioError();

    comparator.Set_Case(result_default,      "from dyjets-loop");
    comparator.Set_Case(result_default_syst, "from dyjets-loop-syst (default)");
    comparator.Set_Case(map_result_syst_["set5"], "from dyjets-loop-syst (systVar, set5)");
    comparator.Expect_PerfectAgreement();
    comparator.Compare("DYRun2Result/validation");
  }

  void ProducePlots_DYRun2Result_Syst() {
    ResultComparator comparator("mm");
    comparator.Remove_RatioError();
    comparator.Set_Case(map_result_syst_["set5"], "set5 (central value)");
    comparator.Set_Case(map_result_syst_["set2"], "set2 (w/o Z p_{T} reweighting)");
    comparator.Set_Case(map_result_syst_["set3"], "set3 (w/o ad-hoc EWK weights)");
    comparator.Set_Case(map_result_syst_["set4"], "set4 (alt. profile #DeltaM mass window)");
    comparator.Set_Case(map_result_syst_["set0"], "set0 (default correction, just for comparison)");

    comparator.Compare("DYRun2Result/syst");
  }

  void ProducePlots_DYRun2Result_Stat(TString era) {
    vector<DYRun2Result*>& vec_result = map_result_stat_[era];

    ResultComparator comparator("mm");
    comparator.Remove_RatioError();
    comparator.Set_Case(map_result_syst_["set5"], "set5 (central value)");    
    comparator.Set_Case(vec_result[0], "stat. replica 1 (era="+era+")");
    comparator.Set_Case(vec_result[1], "stat. replica 2 (era="+era+")");
    comparator.Set_Case(vec_result[2], "stat. replica 3 (era="+era+")");
    comparator.Set_Case(vec_result[3], "stat. replica 4 (era="+era+")");
    comparator.Compare("DYRun2Result/stat/"+era);
  }

  void Save_UnfoldedResults() {
    TFile *f_output = TFile::Open(fileName_unfolded_, "RECREATE");
    // -- syst. var
    for(auto& pair : map_result_syst_ )
      pair.second->Save(f_output, pair.first);

    // -- stat. var
    for(const auto& era : vec_era_ ) {
      vector<DYRun2Result*>& vec_result = map_result_stat_[era];

      for(Int_t i=0; i<nStatVar_; ++i) {
        TString tag = TString::Format("stat_%03d_%s", i, era.Data());
        vec_result[i]->Save(f_output, tag);
      } // -- end of stat. replica iteration
    }// -- end of era iteration

    f_output->Close();
  }

  void Estimate_Unc_Syst() {
    TString histName_base = "h_allEra_unfolded_data";

    // -- central value: set5
    TH1D* h_cv = PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_set5");
    // cout << "h_cv = " << h_cv << endl;
    // cout << "h_cv->GetBinContent(1) = " << h_cv->GetBinContent(1) << endl;
    // PlotTool::Print_Histogram(h_cv);

    // -- set0: not included
    vector<TString> vec_systTag = {"set2", "set3", "set4"};
    vector<Uncertainty> vec_unc_syst;

    for(const auto& systTag : vec_systTag ) {
      TH1D* h_alt = PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_"+systTag);  

      // PlotTool::Print_Histogram(h_alt);
      // break;

      Uncertainty unc("muP_syst_"+systTag, "alternative", "fullyCorr");
      unc.Set_CentralHist( h_cv );
      unc.Set_AltHist( {h_alt} );
      unc.Estimate();
      
      map_unc_.insert( std::make_pair(systTag, unc) );
      vec_unc_syst.push_back( unc );
    }

    // -- total systematic sources (quad. sum)
    Uncertainty unc_totSyst("muP_syst_tot");
    unc_totSyst.Combine( vec_unc_syst );

    map_unc_.insert( std::make_pair("syst", unc_totSyst) );
  }

  void Estimate_Unc_Stat() {
    TString histName_base = "h_allEra_unfolded_data";

    // -- central value: set5
    TH1D* h_cv = PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_set5");

    vector<Uncertainty> vec_unc_stat;
    for(const auto& era : vec_era_ ) {
      vector<TH1D*> vec_altHist;
      for(Int_t i=0; i<nStatVar_; ++i) {
        TString tag = TString::Format("stat_%03d_%s", i, era.Data());
        TH1D* h_alt = PlotTool::Get_Hist(fileName_unfolded_, histName_base+"_"+tag);
        vec_altHist.push_back( h_alt );
      }

      Uncertainty unc("muP_stat_"+era, "smearing", "smearing");
      unc.Set_CentralHist( h_cv );
      unc.Set_AltHist( vec_altHist );
      unc.Estimate();

      map_unc_.insert( std::make_pair("stat_"+era, unc) );
      vec_unc_stat.push_back( unc );
    }

    // -- total systematic sources (quad. sum)
    Uncertainty unc_totStat("muP_stat_tot");
    unc_totStat.Combine( vec_unc_stat );

    map_unc_.insert( std::make_pair("stat", unc_totStat) );
  }

  void Save_Unc() {
    TFile* f_output = TFile::Open(fileName_unc_, "RECREATE");
    for(auto& pair : map_unc_ ) {
      pair.second.Save(f_output);
    }

    vector<Uncertainty> vec_unc = { map_unc_["stat"], map_unc_["syst"] };
    // vector<Uncertainty> vec_unc;
    // vec_unc.push_back( map_unc_["stat"] );
    // vec_unc.push_back( map_unc_["syst"] );

    Uncertainty unc_tot("muP_tot");
    unc_tot.Combine( vec_unc );
    unc_tot.Save(f_output);

    f_output->Close();
  }

  void Print_Summary() {
    cout << "============ [summary] ============" << endl;
    cout << "[input]" << endl;
    cout << "  shears result: " << DYTool::path_systVar_muP << endl;
    cout << "[output]" << endl;
    cout << "  Unfolded results with syst. var.: " << fileName_unfolded_ << endl;
    cout << "  Unc & cov. results: " << fileName_unc_ << endl;
    cout << "===================================" << endl;
  }
};

void estimate_unc_cov_muP() {
  UncEstimator_MuP estimator;
  estimator.EstimateAndSave();
}