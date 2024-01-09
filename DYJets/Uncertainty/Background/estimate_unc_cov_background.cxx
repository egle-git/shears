#include "TopBkgHist.h"

#include "Common/ShearsComparator.h"
#include "Common/DYUncertainty.h"

// -- path information for the alternative backgrounds
// -- need to be updated accordingly, if they are changed
namespace {

TString Get_ChannelTag(TString channel) {
  TString channelTag = "";
  if( channel == "ee" ) channelTag = "EE";
  else if( channel == "mm" ) channelTag = "MuMu";
  else
    throw std::invalid_argument("[Get_ChannelTag] channel = " + channel + " is not supported");

  return channelTag;
}

// -- systDirName: fakesMinus, fakesPlus, minus, plus
std::map<TString, TString> GetMap_AltTopBkgPath(TString channel, TString systDirName) {
  TString channelTag = Get_ChannelTag(channel);

  std::map<TString, TString> map_path = {
    {"16pre",  TString::Format("%s/shears/fromMarijus/TopBkgs/%s/ForSystematics/2016preAPV/%s",  DYTool::path_base.Data(), channelTag.Data(), systDirName.Data())},
    {"16post", TString::Format("%s/shears/fromMarijus/TopBkgs/%s/ForSystematics/2016postAPV/%s", DYTool::path_base.Data(), channelTag.Data(), systDirName.Data())},
    {"17",     TString::Format("%s/shears/fromMarijus/TopBkgs/%s/ForSystematics/2017/%s", DYTool::path_base.Data(), channelTag.Data(), systDirName.Data())},
    {"18",     TString::Format("%s/shears/fromMarijus/TopBkgs/%s/ForSystematics/2018/%s", DYTool::path_base.Data(), channelTag.Data(), systDirName.Data())},
  };

  return map_path;
}

// -- systDirName: constRatio, minus0 ... minus4, plus0 ... plus4
vector<DYTool::DYHistInfo> GetVector_FakeLepBkgInfo(TString channel, TString systDirName) {
  TString channelTag = Get_ChannelTag(channel);

  TString basePath_fake_syst = TString::Format("%s/%s/ForSystematics", DYTool::path_default_fake.Data(), channelTag.Data());
  TString fileName_syst = systDirName+"/dyjets-Fakes.root";

  vector<DYTool::DYHistInfo> vec_fakeLepBkgInfo = {
    DYTool::DYHistInfo{channel, "16pre",  basePath_fake_syst+"/2016preAPV/"+fileName_syst,  "mass_wide_range_inc0jet"},
    DYTool::DYHistInfo{channel, "16post", basePath_fake_syst+"/2016postAPV/"+fileName_syst, "mass_wide_range_inc0jet"},
    DYTool::DYHistInfo{channel, "17",     basePath_fake_syst+"/2017/"+fileName_syst,        "mass_wide_range_inc0jet"},
    DYTool::DYHistInfo{channel, "18",     basePath_fake_syst+"/2018/"+fileName_syst,        "mass_wide_range_inc0jet"}
  };

  return vec_fakeLepBkgInfo;
}

};

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

  map<TString, TH1D*> map_hist_altBkgMC_; // -- alternative bkg. MC with alt. top-quark bkg.
  map<TString, DYRun2Result*> map_result_;

  map<TString, Uncertainty> map_unc_;

  std::map<TString, TString> map_dirName_topBkg_ = {
    {"topBkg_fake_plus", "fakesPlus"},
    {"topBkg_fake_minus", "fakesMinus"},
    {"topBkg_fitParam_plus", "plus"},
    {"topBkg_fitParam_minus", "minus"}
  };

  std::map<TString, TString> map_dirName_fakeLepBkg_ee_ = {
    {"fakeLepBkg_constRatio", "constRatio"},
    {"fakeLepBkg_fitParam_minus0", "minus0"},
    {"fakeLepBkg_fitParam_minus1", "minus1"},
    {"fakeLepBkg_fitParam_minus2", "minus2"},
    {"fakeLepBkg_fitParam_minus3", "minus3"},
    {"fakeLepBkg_fitParam_plus0", "plus0"},
    {"fakeLepBkg_fitParam_plus1", "plus1"},
    {"fakeLepBkg_fitParam_plus2", "plus2"},
    {"fakeLepBkg_fitParam_plus3", "plus3"},
    {"fakeLepBkg_misID_plus",  "misidPlus"},
    {"fakeLepBkg_misID_minus", "misidMinus"},
    {"fakeLepBkg_altInterp", "altInterp"}
  };

  std::map<TString, TString> map_dirName_fakeLepBkg_mm_ = {
    {"fakeLepBkg_constRatio", "constRatio"},
    {"fakeLepBkg_fitParam_minus0", "minus0"},
    {"fakeLepBkg_fitParam_minus1", "minus1"},
    {"fakeLepBkg_fitParam_minus2", "minus2"},
    {"fakeLepBkg_fitParam_minus3", "minus3"},
    {"fakeLepBkg_fitParam_minus4", "minus4"},
    {"fakeLepBkg_fitParam_plus0", "plus0"},
    {"fakeLepBkg_fitParam_plus1", "plus1"},
    {"fakeLepBkg_fitParam_plus2", "plus2"},
    {"fakeLepBkg_fitParam_plus3", "plus3"},
    {"fakeLepBkg_fitParam_plus4", "plus4"}
  };

  void Init() {
    Insert_BkgMC_With_AltTopBkg();
    Init_Result();
  }

  void Init_Result() {
    DYRun2Result* result_cv = new DYRun2Result(DYTool::path_default+"/"+channel_);
    DYTool::Set_Fake(channel_, result_cv);
    // result_cv->Remove_NegativeBin_Fake();
    result_cv->Produce();
    map_result_.insert( std::make_pair("cv", result_cv) );

    for(const auto& pair : map_hist_altBkgMC_ ) {
      TString tag = pair.first;
      TH1D* h_bkgMC_alt = (TH1D*)pair.second->Clone();

      DYRun2Result* result_alt = new DYRun2Result(DYTool::path_default+"/"+channel_);
      DYTool::Set_Fake(channel_, result_alt);
      // result_alt->Remove_NegativeBin_Fake();
      result_alt->Add_CustomAllEraHist("reco", "bkgMC", h_bkgMC_alt);
      result_alt->Produce();

      map_result_.insert( std::make_pair(tag, result_alt) );
    }

    Init_Result_AltFakeLepBkg();
  }

  void Init_Result_AltFakeLepBkg() {
    std::map<TString, TString>& map_dirName = (channel_ == "ee") ? 
                                              map_dirName_fakeLepBkg_ee_ : 
                                              map_dirName_fakeLepBkg_mm_;

    for(const auto& pair : map_dirName ) {
      TString tag = pair.first;
      TString dirName = pair.second;

      vector<DYTool::DYHistInfo> vec_fakeLepBkgInfo = GetVector_FakeLepBkgInfo(channel_, dirName);

      DYRun2Result* result_alt = new DYRun2Result(DYTool::path_default+"/"+channel_);
      // result_alt->Remove_NegativeBin_Fake();
      DYTool::Set_Fake(channel_, result_alt, vec_fakeLepBkgInfo);
      result_alt->Produce();

      map_result_.insert( std::make_pair(tag, result_alt) );
    }
  }

  void Estimate_Unc() {
    Estimate_Unc_TopBkg();
    Estimate_Unc_FakeLepBkg();
  }

  void Estimate_Unc_TopBkg() {
    TH1D* h_cv = map_result_["cv"]->Get_AllEra("unfolded", "data");

    Uncertainty unc_fake("topBkg_fake", "alternative", "fullyCorr");
    unc_fake.Set_CentralHist( h_cv );
    unc_fake.Set_AltHist( { map_result_["topBkg_fake_plus"]->Get_AllEra("unfolded", "data"),
                            map_result_["topBkg_fake_minus"]->Get_AllEra("unfolded", "data") } );
    unc_fake.Estimate();

    map_unc_.insert( std::make_pair("topBkg_fake", unc_fake) );


    Uncertainty unc_fitParam("topBkg_fitParam", "alternative", "fullyCorr");
    unc_fitParam.Set_CentralHist( h_cv );
    unc_fitParam.Set_AltHist( { map_result_["topBkg_fitParam_plus"]->Get_AllEra("unfolded", "data"),
                                map_result_["topBkg_fitParam_minus"]->Get_AllEra("unfolded", "data") } );
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
    unc_constRatio.Set_AltHist( {map_result_["fakeLepBkg_constRatio"]->Get_AllEra("unfolded", "data")} );
    unc_constRatio.Estimate();
    map_unc_.insert( std::make_pair("fakeLepBkg_constRatio", unc_constRatio) );

    // -- uncertainty from the fit parameters
    Uncertainty unc_fitParam("fakeLepBkg_fitParam", "alternative", "fullyCorr");
    unc_fitParam.Set_CentralHist( h_cv );
    vector<TH1D*> vec_altHist_fitParam;
    for(const auto& pair : map_result_ ) {
      TString tag = pair.first;
      if( tag.Contains("fakeLepBkg_fitParam_") )
        vec_altHist_fitParam.push_back( pair.second->Get_AllEra("unfolded", "data") );
    }
    unc_fitParam.Set_AltHist( vec_altHist_fitParam );
    unc_fitParam.Estimate();
    map_unc_.insert( std::make_pair("fakeLepBkg_fitParam", unc_fitParam) );

    // -- electron channel: additional uncertainties - mis charge ID & alternative interpolation near Z peak
    if( channel_ == "ee" ) {
      // -- mis-charge ID
      Uncertainty unc_misID("fakeLepBkg_misID", "oneSigmaShift", "fullyCorr");
      unc_misID.Set_CentralHist( h_cv );
      unc_misID.Set_AltHist( { map_result_["fakeLepBkg_misID_plus"]->Get_AllEra("unfolded", "data"),
                               map_result_["fakeLepBkg_misID_minus"]->Get_AllEra("unfolded", "data") }  ); 
      unc_misID.Estimate();
      map_unc_.insert( std::make_pair("fakeLepBkg_misID", unc_misID) );

      // -- alt. interpolation
      Uncertainty unc_altInterp("fakeLepBkg_altInterp", "alternative", "fullyCorr");
      unc_altInterp.Set_CentralHist( h_cv );
      unc_altInterp.Set_AltHist( {map_result_["fakeLepBkg_altInterp"]->Get_AllEra("unfolded", "data")} );
      unc_altInterp.Estimate();

      map_unc_.insert( std::make_pair("fakeLepBkg_altInterp", unc_altInterp) );
    }

    // -- from the "Stat" directory (calculated by TUnfold)
    Uncertainty unc_stat = MakeUnc_FakeLepBkg_Stat();
    map_unc_.insert( std::make_pair("fakeLepBkg_stat", unc_stat) );

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
    TString fileName = "../Stat/Uncertainty_and_Covariance_Stat_"+channel_+".root";

    Uncertainty unc_stat("stat_bkgFakeLep", fileName);
    unc_stat.Set_Tag("fakeLepBkg_stat"); // -- change tag to have a consistent convention with the others
    return unc_stat;
  }

  void Compare() {
    Compare_AltTopBkg("topBkg_fake");
    Compare_AltTopBkg("topBkg_fitParam");

    Compare_AltFakeLepBkg("fakeLepBkg_constRatio");

    // -- too many plots; split to plus & minus (for uncertainty, they will be considered at once, as it should be)
    Compare_AltFakeLepBkg("fakeLepBkg_fitParam_minus");
    Compare_AltFakeLepBkg("fakeLepBkg_fitParam_plus");

    if( channel_ == "ee" ) {
      Compare_AltFakeLepBkg("fakeLepBkg_misID");
      Compare_AltFakeLepBkg("fakeLepBkg_altInterp");
    }
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

  void Compare_AltFakeLepBkg(TString tag) {
    ResultComparator comparator(channel_);
    comparator.Remove_RatioError();
    comparator.Set_Case(map_result_["cv"], "Central value");
    
    if( tag == "fakeLepBkg_constRatio" ) {
      comparator.Set_Case(map_result_[tag], "const. OS/SS ratio");
    }
    else if( tag.Contains("fakeLepBkg_fitParam") ) { // -- put all variations
      for(const auto& pair : map_result_) {
        TString theTag = pair.first;
        if( theTag.Contains(tag) )
          comparator.Set_Case(pair.second, theTag);
      }
    }
    else if( tag.Contains("fakeLepBkg_misID") ) { // -- up/down variation
      TString tag_plus  = tag+"_plus";
      TString tag_minus = tag+"_minus";

      TString legend_plus  = tag+", up variation";
      TString legend_minus = tag+", down variation";
      comparator.Set_Case(map_result_[tag_plus],  legend_plus);
      comparator.Set_Case(map_result_[tag_minus], legend_minus);
    }
    else if( tag == "fakeLepBkg_altInterp" ) { // -- 1 variation
      comparator.Set_Case(map_result_[tag],  "Alt. interpolation");
    }

    TString plotPath = TString::Format("DYRun2Result/%s/%s", tag.Data(), channel_.Data());
    comparator.Compare(plotPath);
  }

  void Insert_BkgMC_With_AltTopBkg() {

    for(const auto& pair : map_dirName_topBkg_ ) {
      TString tag = pair.first;
      TString dirName = pair.second;

      TopBkgHist topHist(channel_, GetMap_AltTopBkgPath(channel_, dirName) );
      TH1D* h_bkgMC = topHist.Get("bkgMC");
      map_hist_altBkgMC_.insert( std::make_pair(tag, h_bkgMC) );
    }
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