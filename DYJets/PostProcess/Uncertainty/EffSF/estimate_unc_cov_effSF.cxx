#include "Common/DYPath.h"
#include "Common/DYTool.h"
#include "Common/ShearsComparator.h"
#include "Common/DYUncertainty.h"

// -- estimate the uncertainty for a given set
// -- e.g. (mm, ID, data, stat) or (ee, TRIG_Leg1, sf, syst_altMC) ...
class SubUncEstimator_EffSF {
public:
  SubUncEstimator_EffSF(TString channel, TString effType, TString dataType, TString uncType):
  channel_(channel), effType_(effType), dataType_(dataType), uncType_(uncType) { 
    Init_UncEstMethod();
    Init_SystTag();
  }

  void Set_ShearsPath(TString path) { shearsPath_ = path; }
  void Set_CV(DYRun2Result* result_cv) { result_cv_ = result_cv; }

  TString EffType() const  { return effType_; }
  TString DataType() const { return dataType_; }
  TString UncType() const  { return uncType_; }
  TString UncEstMethod() const  { return uncEstMethod_; }

  vector<TString> GetVector_SystTag() const { return vec_systTag_; };

  void ProduceAndSave_DYRun2Result(TFile* f_output) {
    if( result_cv_ == nullptr ) 
      throw std::runtime_error("[ProduceAndSave_DYResult] result_cv_ is not initialized!");

    Init_DYRun2Result();
    ProducePlots_Validation();
    ProducePlots_DYRun2Result();
    Save_DYRun2Result(f_output);

    // -- release the memory
    for(auto& pair : map_result_ )
      delete pair.second;
  }

  void EstimateAndSave_UncAndCov(TString fileName_input, TFile* f_output) {
    if( result_cv_ == nullptr ) 
      throw std::runtime_error("[ProduceAndSave_DYResult] result_cv_ is not initialized!");

    Estimate_UncAndCov(fileName_input);
    Save_UncAndCov(f_output);
  }

  Uncertainty FinalUnc() { 
    // -- last element
    // -- smearing case: combined unc. for ell eras
    // -- other case: vec_unc_ has only 1 element anyway
    return vec_unc_.back();
  }

private:
  // -- number of smeared maps
  Int_t nStatMap_ = 100;

  TString channel_ = ""; // -- mm or ee
  TString effType_ = ""; // -- RECO, ID, ISO, TRIG ...
  TString dataType_ = ""; // -- data, mc, sf
  TString uncType_ = ""; // -- stat, syst_all, syst_altMC ...
  TString uncEstMethod_ = "";

  vector<TString> vec_systTag_; // -- e.g. { ID-data-syst_all_plus, ID-data-syst_all_minus }

  TString shearsPath_ = "";
  DYRun2Result* result_cv_ = nullptr;  

  Run2Output* output_;
  std::map<TString, DYRun2Result*> map_result_;

  vector<Uncertainty> vec_unc_;

  void Init_DYRun2Result() {
    output_ = new Run2Output(shearsPath_);

    // -- stat. uncertainty: separately estimated for each era
    if( uncType_.Contains("stat") ) {
      vector<TString> vec_era = {"16pre", "16post", "17", "18"};

      for(const auto& era :  vec_era ) {
        for(const auto& systTag : vec_systTag_ ) {
          DYRun2Result* result_stat = new DYRun2Result(output_);
          Update_HistName(result_stat, systTag, era); // -- change only one era
          result_stat->Produce();
          map_result_.insert( std::make_pair(systTag+"_"+era, result_stat) );
        } // -- iteration over systTag
      } // -- iteration over era
    }
    else { // -- syst. sources
      for(const auto& systTag : vec_systTag_ ) {
        DYRun2Result* result_syst = new DYRun2Result(output_);
        Update_HistName(result_syst, systTag, "all"); // -- change for all eras
        result_syst->Produce();
        map_result_.insert( std::make_pair(systTag, result_syst) );
      } // -- iteration over systTag
    }
  }

  void Update_HistName(DYRun2Result* result, TString tag, TString era = "all") {
    result->Update_HistName(era, "gen_DY",     "TUnfold1DTrue_inc0jet_"+tag);
    result->Update_HistName(era, "reco_DY",    "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "reco_bkgMC", "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "migM",       "TUnfold2DMig_inc0jet_"+tag);
  }

  void Init_SystTag() {
    if( uncEstMethod_ == "smearing" ) {
      for(Int_t i=0; i<nStatMap_; ++i) {
        // -- e.g. ID-data-stat_001
        TString tag = SystTag_Smearing(i);
        vec_systTag_.push_back( tag );
      }
    }
    else if( uncEstMethod_ == "oneSigmaShift" ) {
      // -- e.g. ID-data-syst_all_plus
      TString tag_plus = SystTag("plus");
      TString tag_minus = SystTag("minus");

      vec_systTag_.push_back(tag_plus);
      vec_systTag_.push_back(tag_minus);
    }
    else if( uncEstMethod_ == "alternative" ) {
      // -- e.g. TRIG_Leg1-sf-syst_altMC
      TString tag = SystTag();
      vec_systTag_.push_back(tag);
    }
  }

  TString SystTag(TString suffix = "") {
    TString tag = TString::Format("%s-%s-%s", effType_.Data(), dataType_.Data(), uncType_.Data());
    if( suffix != "" )
      tag = tag + "_" + suffix;

    return tag;
  }

  TString SystTag_Smearing(Int_t i_map) {
    TString suffix = TString::Format("%03d", i_map);
    return SystTag(suffix);
  }

  void Init_UncEstMethod() {
    if( uncType_.Contains("stat") ) 
      uncEstMethod_ = "smearing";
    else {
      if( channel_ == "ee" && effType_.Contains("TRIG")) uncEstMethod_ = "alternative";
      else                                               uncEstMethod_ = "oneSigmaShift";
    }
  }

  void ProducePlots_Validation() {
    TString path_plot = "DYRun2Result/validation/"+channel_+"/"+effType_+"/"+dataType_;
    Bool_t exist_dir = !gSystem->AccessPathName("./plot/"+path_plot);
    // printf("(path_plot, exist_dir) = (%s, %d)\n", path_plot.Data(), exist_dir);
    if( exist_dir ) return; // -- do not create duplicated plots

    Run2Output* output_default = new Run2Output(DYTool::path_default+"/"+channel_);
    DYRun2Result* result_default = new DYRun2Result(output_default);
    result_default->Produce();

    DYRun2Result* result_syst_cv = new DYRun2Result(output_);
    TString systTag_cv = TString::Format("%s-%s-cv", effType_.Data(), dataType_.Data());
    result_syst_cv->Produce();

    ResultComparator comparator(channel_);
    comparator.Remove_RatioError();
    comparator.Set_Case(result_default, "from dyjets-loop");
    comparator.Set_Case(result_cv_,     "from dyjets-loop-syst (default)");
    comparator.Set_Case(result_syst_cv, "from dyjets-loop-syst ("+systTag_cv+")");
    comparator.Expect_PerfectAgreement();
    comparator.Compare(path_plot);
  }

  void ProducePlots_DYRun2Result() {
    if( uncType_.Contains("stat") ) {
      ProducePlots_DYRun2Result_Stat("16pre");
      ProducePlots_DYRun2Result_Stat("16post");
      ProducePlots_DYRun2Result_Stat("17");
      ProducePlots_DYRun2Result_Stat("18");
    }
    else
     ProducePlots_DYRun2Result_Syst(); 
  }

  void ProducePlots_DYRun2Result_Stat(TString era) {
    ResultComparator comparator(channel_);
    comparator.Remove_RatioError();
    comparator.Set_Case(result_cv_, "Central value");
    Int_t i_case = 0;
    for(const auto& pair : map_result_ ) {
      if( pair.first.Contains(era) ) {
        comparator.Set_Case(pair.second, pair.first);
        if( i_case > 3 ) break;
        i_case++;
      }
    }
    comparator.Compare("DYRun2Result/"+channel_+"/"+effType_+"/"+dataType_+"/"+uncType_+"/"+era);
  }

  void ProducePlots_DYRun2Result_Syst() {
    ResultComparator comparator(channel_);
    comparator.Remove_RatioError();
    comparator.Set_Case(result_cv_, "Central value");
    for(const auto& pair : map_result_ )
      comparator.Set_Case(pair.second, pair.first);
    comparator.Compare("DYRun2Result/"+channel_+"/"+effType_+"/"+dataType_+"/"+uncType_);
  }

  void Save_DYRun2Result(TFile* f_output) {
    f_output->cd();

    for(auto& pair : map_result_ )
      pair.second->Save(f_output, pair.first);

    // f_output->Close(); // -- not close here
  }

  void Estimate_UncAndCov(TString fileName_input) {
    if( uncEstMethod_ == "smearing" ) {
      Estimate_UncAndCov_Smearing(fileName_input, "16pre");
      Estimate_UncAndCov_Smearing(fileName_input, "16post");
      Estimate_UncAndCov_Smearing(fileName_input, "17");
      Estimate_UncAndCov_Smearing(fileName_input, "18");

      TString tag = SystTag();
      Uncertainty unc_stat_allEra("effSF_"+tag);
      unc_stat_allEra.Combine(vec_unc_);

      vec_unc_.push_back(unc_stat_allEra);
    }
    else if( uncEstMethod_ == "oneSigmaShift" )
      Estimate_UncAndCov_OneSigmaShift(fileName_input);
    else if( uncEstMethod_ == "alternative" )
      Estimate_UncAndCov_Alternative(fileName_input);
  }

  void Estimate_UncAndCov_Smearing(TString fileName_input, TString era) {
    // TH1D* h_cv = result_cv_->Get_AllEra("unfoldeld", "data");
    TH1D* h_cv = PlotTool::Get_Hist(fileName_input, "h_allEra_unfolded_data_cv");

    vector<TH1D*> vec_altHist;
    for(Int_t i_map=0; i_map<nStatMap_; ++i_map) {
      TString tag_ith = SystTag_Smearing(i_map)+"_"+era;
      TString histName_ith = "h_allEra_unfolded_data_"+tag_ith;
      TH1D* h_alt = PlotTool::Get_Hist(fileName_input, histName_ith);
      vec_altHist.push_back( h_alt );
    }

    TString tag = SystTag(era);
    Uncertainty unc_stat("effSF_"+tag, "smearing", "smearing");
    unc_stat.Set_CentralHist(h_cv);
    unc_stat.Set_AltHist(vec_altHist);
    unc_stat.Estimate();

    vec_unc_.push_back(unc_stat);
  }

  void Estimate_UncAndCov_OneSigmaShift(TString fileName_input) {
    // TH1D* h_cv = result_cv_->Get_AllEra("unfoldeld", "data");
    TH1D* h_cv = PlotTool::Get_Hist(fileName_input, "h_allEra_unfolded_data_cv");

    vector<TH1D*> vec_altHist;
    TString tag_plus = SystTag("plus");
    TString tag_minus = SystTag("minus");
    TString histName_plus  = "h_allEra_unfolded_data_"+tag_plus;
    TString histName_minus = "h_allEra_unfolded_data_"+tag_minus;

    TH1D* h_plus  = PlotTool::Get_Hist(fileName_input, histName_plus);
    TH1D* h_minus = PlotTool::Get_Hist(fileName_input, histName_minus);
    vec_altHist.push_back( h_plus );
    vec_altHist.push_back( h_minus );

    TString tag = SystTag();
    Uncertainty unc("effSF_"+tag, "oneSigmaShift", "fullyCorr");
    unc.Set_CentralHist(h_cv);
    unc.Set_AltHist(vec_altHist);
    unc.Estimate();

    vec_unc_.push_back( unc );
  }

  void Estimate_UncAndCov_Alternative(TString fileName_input) {
    // TH1D* h_cv = result_cv_->Get_AllEra("unfoldeld", "data");
    TH1D* h_cv = PlotTool::Get_Hist(fileName_input, "h_allEra_unfolded_data_cv");

    TString tag = SystTag();
    TString histName_alt = "h_allEra_unfolded_data_"+tag;
    TH1D* h_alt = PlotTool::Get_Hist(fileName_input, histName_alt);
    Uncertainty unc("effSF_"+tag, "alternative", "fullyCorr");
    unc.Set_CentralHist(h_cv);
    unc.Set_AltHist( {h_alt} );
    unc.Estimate();

    vec_unc_.push_back( unc );
  }

  void Save_UncAndCov(TFile *f_output) {
    f_output->cd();

    for(auto& unc : vec_unc_ )
      unc.Save(f_output);
  }

};



class UncEstimator_EffSF {
public:
  UncEstimator_EffSF(TString channel): channel_(channel) { }

  void EstimateAndSave() {
    Init();

    if( gSystem->AccessPathName(fileName_unfolded_) ) {
      cout << fileName_unfolded_ << " does not exist: make it ..." << endl;

      TFile *f_unfolded = TFile::Open(fileName_unfolded_, "RECREATE");
      f_unfolded->cd();
      // -- save the central value
      result_cv_->Save(f_unfolded, "cv");
      // -- save the systematic variations
      for(auto& subUncEstimator : vec_subUncEstimator_ )
        subUncEstimator.ProduceAndSave_DYRun2Result(f_unfolded);
      f_unfolded->Close();
    }

    if( !gSystem->AccessPathName(fileName_unc_) )
      throw std::runtime_error(fileName_unc_+" already exists: remove or rename it");

    TH1::AddDirectory(kFALSE);

    // -- estimate individual uncertainties
    TFile* f_unc = TFile::Open(fileName_unc_, "RECREATE");
    // Init_UncMap();
    for(auto& subUncEstimator : vec_subUncEstimator_ ) {      
      subUncEstimator.EstimateAndSave_UncAndCov(fileName_unfolded_, f_unc);
      // TString effType = subUncEstimator.EffType();
      // map_unc_[effType].push_back( subUncEstimator.FinalUnc() );
    }

    // -- collect uncertainty per each effType (i.e. fill map_unc_)
    std::map<TString, vector<TString>> map_uncType = 
      (channel_ == "mm") ? map_uncType_mm_ : map_uncType_ee_;
    for(const auto& pair : map_uncType ) {
      TString effType = pair.first;

      vector<Uncertainty> vec_unc_effType;
      for(auto& subUncEstimator : vec_subUncEstimator_ ) {
        if( effType == subUncEstimator.EffType() )
          vec_unc_effType.push_back( subUncEstimator.FinalUnc() );
      }

      map_unc_.insert( std::make_pair(effType, vec_unc_effType) );
    }

    // -- combine the unc. & cov. for each effType
    for(auto& pair : map_unc_ ) {
      TString effType = pair.first;
      vector<Uncertainty> vec_unc = pair.second;

      Uncertainty unc_effType_tot("effSF_"+effType);
      unc_effType_tot.Combine(vec_unc);
      unc_effType_tot.Save(f_unc);
      vec_unc_effType_tot_.push_back( unc_effType_tot );
    }

    // -- final unc. & cov.
    Uncertainty unc_tot("effSF_tot");
    unc_tot.Combine(vec_unc_effType_tot_);
    unc_tot.Save(f_unc);

    f_unc->Close();

    Print_Summary();
  }

private:
  TString channel_ = "";

  map<TString, vector<TString>> map_uncType_mm_ = {
    {"ID"    , {"stat", "syst_all"}},
    {"ISO"   , {"stat", "syst_all"}},
    {"STRIG" , {"stat", "syst_all"}},
    {"DTRIG" , {"stat", "syst_all"}}
  };

  map<TString, vector<TString>> map_uncType_ee_ = {
    {"RECO"      , {"stat_data", "stat_mc", "syst_altBkg", "syst_altSig", "syst_altMC", "syst_altTag"}},
    {"ID"        , {"stat_data", "stat_mc", "syst_altBkg", "syst_altSig", "syst_altMC", "syst_altTag"}},
    {"TRIG_Leg1" , {"stat", "syst_altMC", "syst_altTag", "syst_altSub"}},
    {"TRIG_Leg2" , {"stat", "syst_altMC", "syst_altTag", "syst_altSub"}}
  };

  TString fileName_unfolded_ = DYTool::Get_OutputPath("Unfolded_SystVar_EffSF_"+channel_+".root");
  TString fileName_unc_ = DYTool::Get_OutputPath("UncAndCov_EffSF_"+channel_+".root");
  TString shearsPath_ = "";

  Run2Output* output_;
  DYRun2Result* result_cv_; // -- central value

  // -- collection of DYRun2Result with systematic variations
  std::map<TString, DYRun2Result*> map_result_var_;

  vector<SubUncEstimator_EffSF> vec_subUncEstimator_;
  std::map<TString, vector<Uncertainty>> map_unc_;
  vector<Uncertainty> vec_unc_effType_tot_;

  void Init() {
    shearsPath_ = DYTool::path_systVar_effSF+"/"+channel_;
    output_ = new Run2Output(shearsPath_);

    // -- central value
    result_cv_ = new DYRun2Result(output_);
    result_cv_->Produce();

    Init_SubUncEstimator();
  }

  void Init_SubUncEstimator() {
    std::map<TString, vector<TString>> map_uncType = 
      (channel_ == "mm") ? map_uncType_mm_ : map_uncType_ee_;

    for(const auto& pair : map_uncType ) {
      TString effType = pair.first;
      vector<TString> vec_uncType = pair.second;

      for(const auto& uncType : vec_uncType) {
        // -- two versions (data, mc)
        if( channel_ == "mm" && !(effType == "DTRIG") ) {
          Insert_SubUncEstimator(channel_, effType, "data", uncType);
          Insert_SubUncEstimator(channel_, effType, "mc", uncType);
        }
        else // -- only one version (sf)
          Insert_SubUncEstimator(channel_, effType, "sf", uncType);
      }
    }
  }

  // void Init_UncMap() {
  //   std::map<TString, vector<TString>> map_uncType = 
  //     (channel_ == "mm") ? map_uncType_mm_ : map_uncType_ee_;

  //   for(const auto& pair : map_uncType ) {
  //     TString effType = pair.first;
  //     map_unc_.insert( std::make_pair(effType, {}) );
  //   }
  // }

  void Insert_SubUncEstimator(TString channel, TString effType, TString dataType, TString uncType) {
    SubUncEstimator_EffSF estimator(channel, effType, dataType, uncType);
    estimator.Set_ShearsPath(shearsPath_);
    estimator.Set_CV(result_cv_);
    vec_subUncEstimator_.push_back( estimator );
  }

  void Print_Summary() {
    cout << "============ [summary] ============" << endl;
    cout << "[input]" << endl;
    cout << "  shears result: " << shearsPath_ << endl;
    cout << "[output]" << endl;
    cout << "  Unfolded results with syst. var.: " << fileName_unfolded_ << endl;
    cout << "  Unc & cov. results: " << fileName_unc_ << endl;
    cout << "===================================" << endl;
  }
};

void estimate_unc_cov_effSF() {
  UncEstimator_EffSF estimator_ee("ee");
  estimator_ee.EstimateAndSave();

  UncEstimator_EffSF estimator_mm("mm");
  estimator_mm.EstimateAndSave();
}