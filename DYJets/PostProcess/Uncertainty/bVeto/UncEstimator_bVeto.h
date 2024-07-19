#include "Common/ShearsComparator.h"
#include "Common/DYUncertainty.h"
#include "Common/DYPath.h"
#include "Common/DYTool.h"

class UncEstimator_bVeto {
public:
  UncEstimator_bVeto(TString channel): channel_(channel) {}

  void Use_Fake(Bool_t flag = kTRUE) { useFake_ = flag; }

  void EstimateAndSave() {
    fileName_output_ = DYTool::Get_OutputPath("Unfolded_And_Uncertainty_bVeto_"+channel_+".root");
    if( !gSystem->AccessPathName(fileName_output_) )
      throw std::runtime_error(fileName_output_+ " already exist");

    Init();
    ProducePlot_DYRun2Result();

    Estimate_UncAndCov();

    Save();
  }

  TString OuputFileName() const { return fileName_output_; }


private:
  TString channel_ = "";
  Bool_t useFake_ = kTRUE; // -- default: true
  TString fileName_output_ = "";

  Run2Output* output_;
  std::map<TString, DYRun2Result*> map_result_;

  vector<TString> vec_era_ = {"16pre", "16post", "17", "18"};

  vector<TString> vec_uncType_mcEff_ = {"MC_eff_high", "MC_eff_low"};

  vector<TString> vec_flavor_bTagSF_ = {"light", "heavy"};
  vector<TString> vec_uncType_bTagSF_corr_   = {"up_correlated", "down_correlated"};
  vector<TString> vec_uncType_bTagSF_unCorr_ = {"up_uncorrelated", "down_uncorrelated"};

  // -- uncertainties
  std::map<TString, Uncertainty> map_unc_;

  void Init() {
    output_ = new Run2Output(DYTool::path_systVar_bVeto+"/"+channel_);
    
    // -- central value
    DYRun2Result* result_cv = new DYRun2Result(output_);
    if( useFake_ ) DYTool::Set_Fake(channel_, result_cv);
    result_cv->Produce();
    map_result_.insert( std::make_pair("cv", result_cv) );

    InitResult_MCEffStat();
    InitResult_bTagSF();
  }

  void InitResult_MCEffStat() {
    // -- stat unc.: uncorrelated between eras
    // -- should be varied individually for each era
    for(const auto& uncType : vec_uncType_mcEff_ ) {
      TString uncTag = "bVeto_"+uncType;

      for(const auto& era : vec_era_ )
        Insert_Result(uncTag, era);
    } // -- iteration over uncType
  }

  void InitResult_bTagSF() {
    // -- two kinds of uncertainties: correlated vs. uncorrelated between eras

    // -- correlated uncertainties: change all eras simultaneoustly
    for(const auto& uncType : vec_uncType_bTagSF_corr_ ) {
      for(const auto& flavor : vec_flavor_bTagSF_ ) {
        TString uncTag = "bVeto_"+uncType+"_"+flavor;
        Insert_Result(uncTag, "all");
      }
    }

    // -- uncorrelated uncertainties: change each era individually
    for(const auto& uncType : vec_uncType_bTagSF_unCorr_ ) {
      for(const auto& flavor : vec_flavor_bTagSF_ ) {
        TString uncTag = "bVeto_"+uncType+"_"+flavor;
        for(const auto& era : vec_era_ )
          Insert_Result(uncTag, era);
      }
    }
  }

  // -- varied results: all MC histograms
  void Update_HistName(DYRun2Result* result, TString uncTag, TString era="all") {
    result->Update_HistName(era, "gen_DY",     "TUnfold1DTrue_inc0jet_"+uncTag);
    result->Update_HistName(era, "reco_DY",    "TUnfold1DReco_inc0jet_"+uncTag);
    result->Update_HistName(era, "reco_bkgMC", "TUnfold1DReco_inc0jet_"+uncTag);
    result->Update_HistName(era, "migM",       "TUnfold2DMig_inc0jet_"+uncTag);
  }

  void Insert_Result(TString uncTag, TString era) {
    DYRun2Result* result = new DYRun2Result(output_);
    if( useFake_ ) DYTool::Set_Fake(channel_, result);
    Update_HistName(result, uncTag, era);
    result->Produce();

    TString finalTag = (era == "all") ? uncTag : uncTag+"_"+era;

    map_result_.insert( std::make_pair(finalTag, result) );
  }

  void ProducePlot_DYRun2Result() {
    for(const auto& era : vec_era_ ) {
      ProducePlot_DYRun2Result_MCEffStat(era);
      ProducePlot_DYRun2Result_bTagSF_UnCorr(era);
    }
    ProducePlot_DYRun2Result_bTagSF_Corr();
  }

  void ProducePlot_DYRun2Result_MCEffStat(TString era) {
    ResultComparator comparator(channel_);
    comparator.Set_Case(map_result_["cv"], "Central value");
    // -- syst. var.
    for(const auto& uncType : vec_uncType_mcEff_ ) {
      TString uncTag = "bVeto_"+uncType+"_"+era;
      TString legend;
      if( uncType == "MC_eff_high" ) legend = "MC stat. ("+era+") +1#sigma";
      if( uncType == "MC_eff_low" )  legend = "MC stat. ("+era+") -1#sigma";
      comparator.Set_Case(map_result_[uncTag],  legend);
    }
    comparator.Remove_RatioError();
    comparator.Compare("DYRun2Result/"+channel_+"/MCEffStat_"+era);
  }

  void ProducePlot_DYRun2Result_bTagSF_UnCorr(TString era) {
    for(const auto& flavor : vec_flavor_bTagSF_ ) {

      ResultComparator comparator(channel_);
      comparator.Set_Case(map_result_["cv"], "Central value");
      // -- syst. var.
      for(const auto& uncType : vec_uncType_bTagSF_unCorr_ ) {
        TString uncTag = "bVeto_"+uncType+"_"+flavor+"_"+era;
        TString legend = GetLegend_bTagSF(uncType, flavor, era);
        comparator.Set_Case(map_result_[uncTag],  legend);
      } // -- uncType
      comparator.Remove_RatioError();
      comparator.Compare("DYRun2Result/"+channel_+"/bTagSF_unCorr_"+flavor+"_"+era);

    } // -- flavor
  }

  void ProducePlot_DYRun2Result_bTagSF_Corr() {
    for(const auto& flavor : vec_flavor_bTagSF_ ) {

      ResultComparator comparator(channel_);
      comparator.Set_Case(map_result_["cv"], "Central value");
      // -- syst. var.
      for(const auto& uncType : vec_uncType_bTagSF_corr_ ) {
        TString uncTag = "bVeto_"+uncType+"_"+flavor;
        TString legend = GetLegend_bTagSF(uncType, flavor, "all");
        comparator.Set_Case(map_result_[uncTag],  legend);
      } // -- uncType
      comparator.Remove_RatioError();
      comparator.Compare("DYRun2Result/"+channel_+"/bTagSF_corr_"+flavor);

    } // -- flavor
  }

  TString GetLegend_bTagSF(TString uncType, TString flavor, TString era) {
    TString legend = "undefined";
    if( uncType.Contains("uncorrelated") ) {
      if( uncType.BeginsWith("up_") )   legend = "b-tagging SF("+flavor+" jets, unCorr., "+era+"), +1#sigma";
      if( uncType.BeginsWith("down_") ) legend = "b-tagging SF("+flavor+" jets, unCorr., "+era+"), -1#sigma";
    }
    else { // -- correlated
      if( uncType.BeginsWith("up_") )   legend = "b-tagging SF("+flavor+" jets, corr.), +1#sigma";
      if( uncType.BeginsWith("down_") ) legend = "b-tagging SF("+flavor+" jets, corr.), -1#sigma";
    }

    return legend;
  }

  void Estimate_UncAndCov() {
    Uncertainty unc_mcEffStat     = Estimate_UncAndCov_MCEffStat();
    Uncertainty unc_bTagSF_unCorr = Estimate_UncAndCov_bTagSF_UnCorr();
    Uncertainty unc_bTagSF_corr   = Estimate_UncAndCov_bTagSF_Corr();

    // -- combine bTagSF: unCorr + corr
    Uncertainty unc_bTagSF("bVeto_bTagSF");
    unc_bTagSF.Combine( {unc_bTagSF_unCorr, unc_bTagSF_corr} );
    map_unc_.insert( std::make_pair(unc_bTagSF.Tag(), unc_bTagSF) );

    // -- total uncertainty
    Uncertainty unc_tot("bVeto_tot");
    unc_tot.Combine( {unc_mcEffStat, unc_bTagSF} );
    map_unc_.insert( std::make_pair(unc_tot.Tag(), unc_tot) );
  }

  Uncertainty Estimate_UncAndCov_bTagSF_Corr() {
    vector<Uncertainty> vec_unc;
    for(const auto& flavor : vec_flavor_bTagSF_ )
      vec_unc.push_back( MakeUnc_bTagSF_Corr(flavor) );

    Uncertainty unc_tot("bVeto_bTagSF_corr_tot");
    unc_tot.Combine( vec_unc );

    map_unc_.insert( std::make_pair(unc_tot.Tag(), unc_tot) );

    return unc_tot;
  }

  Uncertainty MakeUnc_bTagSF_Corr(TString flavor) {
    TH1D* h_cv = map_result_["cv"]->Get_AllEra("unfolded", "data");
    vector<TH1D*> vec_altHist;
    for(const auto& uncType : vec_uncType_bTagSF_corr_ ) {
      TString finalTag = "bVeto_"+uncType+"_"+flavor;
      vec_altHist.push_back( map_result_[finalTag]->Get_AllEra("unfolded", "data") );
    }

    Uncertainty unc("bVeto_bTagSF_corr_"+flavor, "oneSigmaShift", "fullyCorr");
    unc.Set_CentralHist( h_cv );
    unc.Set_AltHist( vec_altHist );
    unc.Estimate();

    map_unc_.insert( std::make_pair(unc.Tag(), unc) );

    return unc;
  }

  Uncertainty Estimate_UncAndCov_bTagSF_UnCorr() {
    vector<Uncertainty> vec_unc;
    for(const auto& flavor : vec_flavor_bTagSF_ )
      vec_unc.push_back( MakeUnc_bTagSF_UnCorr(flavor) );

    Uncertainty unc_tot("bVeto_bTagSF_unCorr_tot");
    unc_tot.Combine( vec_unc );

    map_unc_.insert( std::make_pair(unc_tot.Tag(), unc_tot) );

    return unc_tot;
  }

  // -- estimate for all eras
  Uncertainty MakeUnc_bTagSF_UnCorr(TString flavor) {
    vector<Uncertainty> vec_unc_era;
    for(const auto& era: vec_era_) {
      Uncertainty unc_era = MakeUnc_bTagSF_UnCorr(flavor, era);
      vec_unc_era.push_back( unc_era );
    }

    Uncertainty unc_tot("bVeto_bTagSF_unCorr_"+flavor+"_tot");
    unc_tot.Combine( vec_unc_era );

    // -- register
    map_unc_.insert( std::make_pair(unc_tot.Tag(), unc_tot) );

    return unc_tot;
  }

  // -- make Uncertainty for a given flavor and era
  Uncertainty MakeUnc_bTagSF_UnCorr(TString flavor, TString era) {
    TH1D* h_cv = map_result_["cv"]->Get_AllEra("unfolded", "data");
    vector<TH1D*> vec_altHist;
    for(const auto& uncType : vec_uncType_bTagSF_unCorr_ ) {
      TString finalTag = "bVeto_"+uncType+"_"+flavor+"_"+era;
      vec_altHist.push_back( map_result_[finalTag]->Get_AllEra("unfolded", "data") );
    }

    Uncertainty unc("bVeto_bTagSF_unCorr_"+flavor+"_"+era, "oneSigmaShift", "fullyCorr");
    unc.Set_CentralHist( h_cv );
    unc.Set_AltHist( vec_altHist );
    unc.Estimate();

    // -- register
    map_unc_.insert( std::make_pair(unc.Tag(), unc) );

    return unc;
  }

  Uncertainty Estimate_UncAndCov_MCEffStat() {
    vector<Uncertainty> vec_unc_era;
    for(const auto& era: vec_era_) {
      Uncertainty unc_era = MakeUnc_MCEffStat(era);
      vec_unc_era.push_back( unc_era );
    }

    Uncertainty unc_tot("bVeto_mcEffStat_tot");
    unc_tot.Combine( vec_unc_era );

    map_unc_.insert( std::make_pair(unc_tot.Tag(), unc_tot) );

    return unc_tot;
  }

  Uncertainty MakeUnc_MCEffStat(TString era) {
    TH1D* h_cv = map_result_["cv"]->Get_AllEra("unfolded", "data");
    vector<TH1D*> vec_altHist;
    for(const auto& uncType : vec_uncType_mcEff_ ) {
      TString finalTag = "bVeto_"+uncType+"_"+era;
      vec_altHist.push_back( map_result_[finalTag]->Get_AllEra("unfolded", "data") );
    }

    Uncertainty unc("bVeto_mcEffStat_"+era, "oneSigmaShift", "fullyCorr");
    unc.Set_CentralHist( h_cv );
    unc.Set_AltHist( vec_altHist );
    unc.Estimate();

    // -- register
    map_unc_.insert( std::make_pair(unc.Tag(), unc) );

    return unc;
  }

  void Save() {
    TFile *f_output = TFile::Open(fileName_output_, "RECREATE");
    f_output->cd();
    for(auto& pair : map_result_ )
      pair.second->Save(f_output, pair.first);

    for(auto& pair : map_unc_ )
      pair.second.Save(f_output);

    f_output->Close();
  }

};

void estimate_unc_cov_bVeto() {
  UncEstimator_bVeto estimator_ee("ee");
  estimator_ee.EstimateAndSave();

  UncEstimator_bVeto estimator_mm("mm");
  estimator_mm.EstimateAndSave();
}