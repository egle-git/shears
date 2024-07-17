#include "Common/DYTool.h"
#include "Common/ShearsComparator.h"
#include "Common/DYUncertainty.h"

class UncEstimator_Bkg {
public:

  UncEstimator_Bkg(TString channel):channel_(channel) {}

  void Use_Fake(Bool_t flag = kTRUE) { useFake_ = flag; }
  
  void EstimateAndSave() {
    Init();

    Compare();

    Estimate_Unc();

    Save();
  }

private:
  TString channel_ = "";
  Bool_t useFake_ = kTRUE;

  map<TString, DYRun2Result*> map_result_;
  map<TString, Uncertainty> map_unc_;

  vector<TString> vec_era_ = {"16pre", "16post", "17", "18"};
  vector<TString> vec_tag_topRwgt_ = {
    "emuMethodFakes_minus", 
    "emuMethodFakes_plus", 
    "emuMethodFit_minus", 
    "emuMethodFit_plus"
  };

  vector<TString> vec_tag_fakeLep_ee_ = {
    "fakeSameSignConstRatio",
    "fakeSameSignFit_minus1",
    "fakeSameSignFit_minus2",
    "fakeSameSignFit_minus3",
    "fakeSameSignFit_minus4",
    "fakeSameSignFit_plus1",
    "fakeSameSignFit_plus2",
    "fakeSameSignFit_plus3",
    "fakeSameSignFit_plus4",
    "fakeSameSignNoEmuMeth",
    "fakeSameSignElChMisid_plus",
    "fakeSameSignElChMisid_minus",
    "fakeSameSignInterp",
  };
  vector<TString> vec_tag_fakeLep_mm_ = {
    "fakeSameSignConstRatio",
    "fakeSameSignFit_minus1",
    "fakeSameSignFit_minus2",
    "fakeSameSignFit_minus3",
    "fakeSameSignFit_minus4",
    "fakeSameSignFit_minus5",
    "fakeSameSignFit_plus1",
    "fakeSameSignFit_plus2",
    "fakeSameSignFit_plus3",
    "fakeSameSignFit_plus4",
    "fakeSameSignFit_plus5",
    "fakeSameSignNoEmuMeth"
  };

  void Compare() {
    Compare_AltTopBkg("emuMethodFakes");
    Compare_AltTopBkg("emuMethodFit");

    Compare_AltFakeLepBkg("fakeSameSignConstRatio");

    // -- too many plots; split to plus & minus (for uncertainty, they will be considered at once, as it should be)
    Compare_AltFakeLepBkg("fakeSameSignFit_minus");
    Compare_AltFakeLepBkg("fakeSameSignFit_plus");

    Compare_AltFakeLepBkg("fakeSameSignNoEmuMeth");

    if( channel_ == "ee" ) {
      Compare_AltFakeLepBkg("fakeSameSignElChMisid");
      Compare_AltFakeLepBkg("fakeSameSignInterp");
    }
  }

  void Compare_AltFakeLepBkg(TString tag) {
    ResultComparator comparator(channel_);
    comparator.Remove_RatioError();
    comparator.Set_Case(map_result_["cv"], "Central value");
    
    if( tag == "fakeSameSignConstRatio" ) {
      comparator.Set_Case(map_result_[tag], "const. OS/SS ratio");
    }
    else if( tag.Contains("fakeSameSignFit") ) { // -- put all variations
      for(const auto& pair : map_result_) {
        TString theTag = pair.first;
        if( theTag.Contains(tag) )
          comparator.Set_Case(pair.second, theTag);
      }
    }
    else if( tag == "fakeSameSignNoEmuMeth" ) {
      comparator.Set_Case(map_result_[tag], "no e#mu rwgt. for SS top-quark bkg.");
    }
    else if( tag.Contains("fakeSameSignElChMisid") ) { // -- up/down variation
      TString tag_plus  = tag+"_plus";
      TString tag_minus = tag+"_minus";

      TString legend_plus  = "charge mis-ID, up variation";
      TString legend_minus = "charge mis-ID, down variation";
      comparator.Set_Case(map_result_[tag_plus],  legend_plus);
      comparator.Set_Case(map_result_[tag_minus], legend_minus);
    }
    else if( tag == "fakeSameSignInterp" ) { // -- 1 variation
      comparator.Set_Case(map_result_[tag],  "Alt. interpolation (Z peak)");
    }

    TString plotPath = TString::Format("DYRun2Result/%s/%s", tag.Data(), channel_.Data());
    comparator.Compare(plotPath);
  }

  void Compare_AltTopBkg(TString tag) {
    TString tag_plus  = tag+"_plus";
    TString tag_minus = tag+"_minus";

    TString legend_plus  = tag+", up variation";
    TString legend_minus = tag+", down variation";
    ResultComparator comparator(channel_);
    comparator.Remove_RatioError();
    comparator.Set_Case(map_result_["cv"],     "Central value");
    comparator.Set_Case(map_result_[tag_plus],  legend_plus);
    comparator.Set_Case(map_result_[tag_minus], legend_minus);
    // comparator.Expect_PerfectAgreement();

    TString plotPath = TString::Format("DYRun2Result/%s/%s", tag.Data(), channel_.Data());
    comparator.Compare(plotPath);
  }

  void Init() {
    // -- cv
    DYRun2Result* result_cv = new DYRun2Result(DYTool::path_default+"/"+channel_);
    DYTool::Set_Fake(channel_, result_cv);
    result_cv->Produce();
    map_result_.insert( std::make_pair("cv", result_cv) );

    // -- top bkg. variation
    // ---- emu fake (up / down)
    // ---- fit paramter (up / down)
    for(const auto& tag : vec_tag_topRwgt_ ) {
      DYRun2Result* result_alt = new DYRun2Result(DYTool::path_default+"/"+channel_);
      DYTool::Set_Fake(channel_, result_alt);
      Customize_TopBkg(tag, result_alt);
      result_alt->Produce();
      map_result_.insert( std::make_pair(tag, result_alt) );
    }

    // -- fake lepton bkg. variation
    // ---- fit parameter changes (8 variations for ee; 10 variations for mm)
    // ---- constant OS/SS ratio
    // ---- w/o emu scale factor on SS top quark bkg.
    // ---- charge mis-ID uncertainty (ee only)
    // ---- interporlation at Z peak region (ee only)
    vector<TString> vec_tag_fakeLep = (channel_ == "ee") ? vec_tag_fakeLep_ee_ : vec_tag_fakeLep_mm_;
    for(const auto& tag : vec_tag_fakeLep ) {
      DYRun2Result* result_alt = new DYRun2Result(DYTool::path_default+"/"+channel_);
      vector<DYTool::DYHistInfo> vec_fakeLepBkgInfo = GetVector_FakeLepBkgInfo(tag);
      DYTool::Set_Fake(channel_, result_alt, vec_fakeLepBkgInfo);
      // result_alt->Remove_NegativeBin_Fake();
      result_alt->Produce();
      map_result_.insert( std::make_pair(tag, result_alt) );
    }
  }

  vector<DYTool::DYHistInfo> GetVector_FakeLepBkgInfo(TString tag) {
    TString histName_alt = "mass_wide_range_inc0jet_"+tag;

    vector<DYTool::DYHistInfo> vec_fakeLepBkgInfo = {
      DYTool::DYHistInfo{channel_, "16pre",  DYTool::path_systVar_bkg_fakeLep+"/"+channel_+"/16pre/dyjets-Fakes.root",  histName_alt},
      DYTool::DYHistInfo{channel_, "16post", DYTool::path_systVar_bkg_fakeLep+"/"+channel_+"/16post/dyjets-Fakes.root", histName_alt},
      DYTool::DYHistInfo{channel_, "17",     DYTool::path_systVar_bkg_fakeLep+"/"+channel_+"/17/dyjets-Fakes.root",     histName_alt},
      DYTool::DYHistInfo{channel_, "18",     DYTool::path_systVar_bkg_fakeLep+"/"+channel_+"/18/dyjets-Fakes.root",     histName_alt}
    };

    return vec_fakeLepBkgInfo;
  }

  void Customize_TopBkg(TString tag, DYRun2Result* result) {
    // -- get the bkg. histogram (all-era)
    TH1D* h_allEra_TT        = Get_AllEraHist(DYTool::path_systVar_bkg_topRwgt+"/"+channel_, "TT",        tag);
    TH1D* h_allEra_singleTop = Get_AllEraHist(DYTool::path_systVar_bkg_topRwgt+"/"+channel_, "singleTop", tag);
    result->Add_CustomAllEraHist("reco", "TT",        h_allEra_TT);
    result->Add_CustomAllEraHist("reco", "singleTop", h_allEra_singleTop);
  }

  TH1D* Get_AllEraHist(TString basePath, TString process, TString tag) {
    TString histName = "TUnfold1DReco_inc0jet";
    if( tag != "" )
      histName = histName + "_"+tag;

    vector<TH1D*> vec_eraHist_;
    for( const auto& era : vec_era_ ) {
      ProcessOutput output(process, DYTool::map_tag[process], basePath+"/"+era, DYTool::GetLumi(era));
      vec_eraHist_.push_back( output.Get(histName) );
    }

    return DYTool::MergeHist( vec_eraHist_ );
  }

  void Estimate_Unc() {
    Estimate_Unc_TopBkg();
    Estimate_Unc_FakeLepBkg();
  }

  void Estimate_Unc_TopBkg() {
    TH1D* h_cv = map_result_["cv"]->Get_AllEra("unfolded", "data");

    // -- unc. from the fake-lepton change in the emu method
    Uncertainty unc_fake("topBkg_fake", "alternative", "fullyCorr");
    unc_fake.Set_CentralHist( h_cv );
    unc_fake.Set_AltHist( { map_result_["emuMethodFakes_plus"]->Get_AllEra("unfolded", "data"),
                            map_result_["emuMethodFakes_minus"]->Get_AllEra("unfolded", "data") } );

    unc_fake.Estimate();

    map_unc_.insert( std::make_pair("topBkg_fake", unc_fake) );

    // -- unc. due to fit parameter change
    Uncertainty unc_fitParam("topBkg_fitParam", "alternative", "fullyCorr");
    unc_fitParam.Set_CentralHist( h_cv );
    unc_fitParam.Set_AltHist( { map_result_["emuMethodFit_plus"]->Get_AllEra("unfolded", "data"),
                                map_result_["emuMethodFit_minus"]->Get_AllEra("unfolded", "data") } );
    unc_fitParam.Estimate();

    map_unc_.insert( std::make_pair("topBkg_fitParam", unc_fitParam) );

    // -- quad. sum
    Uncertainty unc_topBkg("topBkg_tot");
    unc_topBkg.Combine( {unc_fake, unc_fitParam} );

    map_unc_.insert( std::make_pair("topBkg_tot", unc_topBkg) );
  }

  void Estimate_Unc_FakeLepBkg() {
    TH1D* h_cv = map_result_["cv"]->Get_AllEra("unfolded", "data");

    // -- uncertainty from the alternative OS/SS ratio: constant ratio
    Uncertainty unc_constRatio("fakeLepBkg_constRatio", "alternative", "fullyCorr");
    unc_constRatio.Set_CentralHist( h_cv );
    unc_constRatio.Set_AltHist( {map_result_["fakeSameSignConstRatio"]->Get_AllEra("unfolded", "data")} );
    unc_constRatio.Estimate();
    map_unc_.insert( std::make_pair("fakeLepBkg_constRatio", unc_constRatio) );

    // -- uncertainty from the fit parameters
    Uncertainty unc_fitParam("fakeLepBkg_fitParam", "alternative", "fullyCorr");
    unc_fitParam.Set_CentralHist( h_cv );
    vector<TH1D*> vec_altHist_fitParam;
    for(const auto& pair : map_result_ ) {
      TString tag = pair.first;
      if( tag.Contains("fakeSameSignFit_") )
        vec_altHist_fitParam.push_back( pair.second->Get_AllEra("unfolded", "data") );
    }
    unc_fitParam.Set_AltHist( vec_altHist_fitParam );
    unc_fitParam.Estimate();
    map_unc_.insert( std::make_pair("fakeLepBkg_fitParam", unc_fitParam) );

    // -- uncertainty from variation w/o emu reweighting factor on SS top-bkg.
    Uncertainty unc_noSSEmuRwgt("fakeLepBkg_noSSEmuRwgt", "alternative", "fullyCorr");
    unc_noSSEmuRwgt.Set_CentralHist( h_cv );
    unc_noSSEmuRwgt.Set_AltHist( {map_result_["fakeSameSignNoEmuMeth"]->Get_AllEra("unfolded", "data")} );
    unc_noSSEmuRwgt.Estimate();
    map_unc_.insert( std::make_pair("fakeLepBkg_noSSEmuRwgt", unc_noSSEmuRwgt) );


    // -- electron channel: additional uncertainties - mis charge ID & alternative interpolation near Z peak
    if( channel_ == "ee" ) {
      // -- mis-charge ID
      Uncertainty unc_misID("fakeLepBkg_misID", "oneSigmaShift", "fullyCorr");
      unc_misID.Set_CentralHist( h_cv );
      unc_misID.Set_AltHist( { map_result_["fakeSameSignElChMisid_plus"]->Get_AllEra("unfolded", "data"),
                               map_result_["fakeSameSignElChMisid_minus"]->Get_AllEra("unfolded", "data") }  ); 
      unc_misID.Estimate();
      map_unc_.insert( std::make_pair("fakeLepBkg_misID", unc_misID) );

      // -- alt. interpolation
      Uncertainty unc_altInterp("fakeLepBkg_altInterp", "alternative", "fullyCorr");
      unc_altInterp.Set_CentralHist( h_cv );
      unc_altInterp.Set_AltHist( {map_result_["fakeSameSignInterp"]->Get_AllEra("unfolded", "data")} );
      unc_altInterp.Estimate();

      map_unc_.insert( std::make_pair("fakeLepBkg_altInterp", unc_altInterp) );
    }

    // -- from the "Stat" directory (calculated by TUnfold)
    Uncertainty unc_stat = MakeUnc_FakeLepBkg_Stat();
    map_unc_.insert( std::make_pair("fakeLepBkg_stat", unc_stat) );

    // -- total fake lepton unc.
    Uncertainty unc_fakeLepBkg("fakeLepBkg_tot");
    vector<Uncertainty> vec_unc_fakeLepBkg;
    for(const auto& pair : map_unc_ ) {
      if( pair.first.Contains("fakeLepBkg_") )
        vec_unc_fakeLepBkg.push_back( pair.second );
    }

    unc_fakeLepBkg.Combine( vec_unc_fakeLepBkg );

    map_unc_.insert( std::make_pair("fakeLepBkg_tot", unc_fakeLepBkg) );
  }

  Uncertainty MakeUnc_FakeLepBkg_Stat() {
    TString fileName = DYTool::path_uncDir+"/Stat/Uncertainty_and_Covariance_Stat_"+channel_+".root";

    Uncertainty unc_stat("stat_bkgFakeLep", fileName);
    unc_stat.Set_Tag("fakeLepBkg_stat"); // -- change tag to have a consistent convention with the others
    return unc_stat;
  }

  void Save() {
    TString fileName_output = TString::Format("Unfolded_And_Uncertainty_Bkg_%s.root", channel_.Data());

    TFile *f_output = TFile::Open(fileName_output, "RECREATE");
    f_output->cd();

    // -- unfolded results
    for(auto& pair : map_result_ )
      pair.second->Save(f_output, pair.first);

    // -- uncertainties
    // -- individual uncertainties
    for(auto& pair : map_unc_ )
      pair.second.Save(f_output);

    // -- total uncertainties
    vector<Uncertainty> vec_unc = { map_unc_["topBkg_tot"], map_unc_["fakeLepBkg_tot"] };
    Uncertainty unc_tot("bkg_tot");
    unc_tot.Combine( vec_unc );
    unc_tot.Save(f_output);

    f_output->Close();
  }
};

void estimate_unc_cov_background() {
  UncEstimator_Bkg estimator_ee("ee");
  estimator_ee.EstimateAndSave();

  UncEstimator_Bkg estimator_mm("mm");
  estimator_mm.EstimateAndSave();
}