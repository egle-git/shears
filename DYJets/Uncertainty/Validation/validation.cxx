#include "Common/SimplePlotTools.h"
#include "Common/DYPath.h"
#include "Common/DYOutput.h"
#include "Common/ShearsComparator.h"

// -- to check whether the results (central value) are same between:
// ---- the result from dyjets-loop (dyjets-analyzer)
// ---- the result from dyjets-loop-syst (dyjets-analyzer-syst)
// ---- the result from dyjets-loop-syst, muP systematics (where event selection is re-done)
// ---- the result from dyjets-loop-syst, elE systematics (where event selection is re-done)

class Validator {
public:
  Validator(TString channel): channel_(channel) {

  }

  void Validate_MuP(Bool_t flag = kTRUE) { do_muP_ = flag; }
  void Validate_ElE(Bool_t flag = kTRUE) { do_elE_ = flag; }

  void Validate() {
    Init();
    Comp();

    for(const auto& era : vec_era_ )
      Comp_Era(era);
  }

private:
  struct ResultInfo {
    DYRun2Result* result;
    TString legend;
  };

  TString channel_ = "";
  Bool_t do_muP_ = kTRUE;
  Bool_t do_elE_ = kTRUE;

  // std::map<TString, DYRun2Result*> map_result_;
  std::map<TString, ResultInfo> map_resultInfo_;

  vector<TString> vec_era_ = {"16pre", "16post", "17", "18"};

  void Init() {
    Insert("default", DYTool::path_default+"/"+channel_);
    Insert("syst",    DYTool::path_systVar_pileup+"/"+channel_);
    if( channel_ == "mm" && do_muP_ )
      Insert("syst_muP", DYTool::path_systVar_muP+"/"+channel_);
    if( channel_ == "ee" && do_elE_ )
      Insert("syst_elE", DYTool::path_systVar_elE+"/"+channel_);
  }

  // void Insert(TString type, TString path) {
  //   map_result_.insert( std::make_pair(type, new DYRun2Result(path)) );
  // }

  void Insert(TString type, TString path) {
    TString legend;
    if( type == "default" ) legend = "Default (dyjets-analyzer)";
    if( type == "syst" )    legend = "Syst. Code (dyjets-analyzer-syst)";
    if( type == "syst_muP") legend = "Syst. Code (dyjets-analyzer-syst), muP part";
    if( type == "syst_elE") legend = "Syst. Code (dyjets-analyzer-syst), elE part";

    ResultInfo info;
    info.result = new DYRun2Result(path);
    info.legend = legend;

    if( type == "syst_muP") Update_HistName(info.result, "muP_set5");
    if( type == "syst_elE") Update_HistName(info.result, "elE_set0");
    info.result->Produce();

    map_resultInfo_.insert( std::make_pair(type, info) );
  }

  void Update_HistName(DYRun2Result* result, const TString tag, const TString era = "all") {
    // -- data is also changed with the variation
    result->Update_HistName(era, "reco_data",  "TUnfold1DReco_inc0jet_"+tag);

    result->Update_HistName(era, "gen_DY",     "TUnfold1DTrue_inc0jet_"+tag);
    result->Update_HistName(era, "reco_DY",    "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "reco_bkgMC", "TUnfold1DReco_inc0jet_"+tag);
    result->Update_HistName(era, "migM",       "TUnfold2DMig_inc0jet_"+tag);
  }

  void Comp() {
    ResultComparator comparator(channel_);
    comparator.Set_Case(map_resultInfo_["default"].result, map_resultInfo_["default"].legend);
    comparator.Set_Case(map_resultInfo_["syst"].result,    map_resultInfo_["syst"].legend);

    if( channel_ == "mm" && do_muP_ )
      comparator.Set_Case(map_resultInfo_["syst_muP"].result, map_resultInfo_["syst_muP"].legend);

    if( channel_ == "ee" && do_elE_ )
      comparator.Set_Case(map_resultInfo_["syst_elE"].result, map_resultInfo_["syst_elE"].legend);

    comparator.Remove_RatioError();
    comparator.Expect_PerfectAgreement();
    comparator.Compare(channel_);
  }

  void Comp_Era(TString era) {
    EraComparator comparator(era, channel_);
    // comparator.Add_Comparison("mass_wide_range_inc0jet");

    cout << "default path = " << map_resultInfo_["default"].result->Get_Run2Output()->Get_EraOutput(era).Path() << endl;

    comparator.Set_Case(map_resultInfo_["default"].result->Get_Run2Output()->Get_EraOutput(era).Path(), map_resultInfo_["default"].legend);
    comparator.Set_Case(map_resultInfo_["syst"].result->Get_Run2Output()->Get_EraOutput(era).Path(),    map_resultInfo_["syst"].legend);
    if( channel_ == "mm" && do_muP_ )
      comparator.Set_Case(map_resultInfo_["syst_muP"].result->Get_Run2Output()->Get_EraOutput(era).Path(), map_resultInfo_["syst_muP"].legend);

    if( channel_ == "ee" && do_elE_ )
      comparator.Set_Case(map_resultInfo_["syst_elE"].result->Get_Run2Output()->Get_EraOutput(era).Path(), map_resultInfo_["syst_elE"].legend);

    if( channel_ == "mm" && do_muP_ ) {
      comparator.Add_Comparison( {"mass_wide_range_inc0jet", "mass_wide_range_inc0jet", "mass_wide_range_inc0jet_muP_set5"} );
      comparator.Add_Comparison( {"mass_wide_range_inc0jet-gen", "mass_wide_range_inc0jet-gen", "mass_wide_range_inc0jet_muP_set5-gen"} );
    }
    else if( channel_ == "ee" && do_elE_ ) {
      comparator.Add_Comparison( {"mass_wide_range_inc0jet", "mass_wide_range_inc0jet", "mass_wide_range_inc0jet_elE_set0"} );
      comparator.Add_Comparison( {"mass_wide_range_inc0jet-gen", "mass_wide_range_inc0jet-gen", "mass_wide_range_inc0jet_elE_set0-gen"} );
      // comparator.Add_Comparison( {"TUnfold1DReco_inc0jet", "TUnfold1DReco_inc0jet", "TUnfold1DReco_inc0jet_elE_set0"} );
    }
    else
      comparator.Add_Comparison( "mass_wide_range_inc0jet" );

    comparator.Remove_RatioError();
    comparator.Expect_PerfectAgreement();
    comparator.Compare(channel_);
  }
};

void validation() {
  Validator validator_ee("ee");
  // validator_ee.Validate_MuP(kFALSE);
  // validator_ee.Validate_ElE(kFALSE);
  validator_ee.Validate();

  Validator validator_mm("mm");
  validator_mm.Validate();
}