#include "estimate_unc_cov_effSF.cxx"

class ResultProducer {
public:
  ResultProducer(TString effType): effType_(effType) {}

  void Save_Default(Bool_t flag = kTRUE) { saveDefault_ = flag; }

  void ProduceAndSave() {
    Init();
    
    TFile* f_unfolded = TFile::Open(outputName_, "RECREATE");
    if( saveDefault_ )
      result_cv_->Save(f_unfolded, "cv");

    for(auto& subUncEstimator : vec_subUncEstimator_ )
      subUncEstimator.ProduceAndSave_DYRun2Result(f_unfolded);

    f_unfolded->Close();
  }

private:
  TString effType_ = "";
  TString outputName_base_ = "Unfolded_SystVar_EffSF_mm";
  TString outputName_ = "";

  Bool_t saveDefault_ = kFALSE;

  TString shearsPath_ = "";
  Run2Output* output_;
  DYRun2Result* result_cv_; // -- central value

  map<TString, vector<TString>> map_uncType_mm_ = {
    {"ID"    , {"stat", "syst_all"}},
    {"ISO"   , {"stat", "syst_all"}},
    {"STRIG" , {"stat", "syst_all"}},
    {"DTRIG" , {"stat", "syst_all"}}
  };

  vector<SubUncEstimator_EffSF> vec_subUncEstimator_;

  void Init() {
    outputName_ = TString::Format("%s_%s.root", outputName_base_.Data(), effType_.Data());
    outputName_ = DYTool::Get_OutputPath(outputName_);

    shearsPath_ = DYTool::path_systVar_effSF+"/mm";
    output_ = new Run2Output(shearsPath_);

    // -- central value
    result_cv_ = new DYRun2Result(output_);
    result_cv_->Produce();

    Init_SubUncEstimator();
  }

  void Init_SubUncEstimator() {
    vector<TString> vec_uncType = map_uncType_mm_[effType_];
    for(const auto& uncType : vec_uncType) {
      // -- two versions (data, mc)
      if( !(effType_ == "DTRIG") ) {
        Insert_SubUncEstimator("mm", effType_, "data", uncType);
        Insert_SubUncEstimator("mm", effType_, "mc",   uncType);
      }
      else // -- only one version (sf)
        Insert_SubUncEstimator("mm", effType_, "sf", uncType);
    }
  }

  void Insert_SubUncEstimator(TString channel, TString effType, TString dataType, TString uncType) {
    SubUncEstimator_EffSF estimator(channel, effType, dataType, uncType);
    estimator.Set_ShearsPath(shearsPath_);
    estimator.Set_CV(result_cv_);
    vec_subUncEstimator_.push_back( estimator );
  }

};

void produce_unfoldedResults_mm(TString effType) {

  ResultProducer producer_ID(effType);
  if( effType == "ID" )
    producer_ID.Save_Default();
  producer_ID.ProduceAndSave();
}