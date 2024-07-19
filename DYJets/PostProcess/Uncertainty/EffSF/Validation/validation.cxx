#include "Common/ShearsComparator.h"
#include "Common/DYPath.h"
#include "Common/DYTool.h"

void Update_HistName_DYRun2Result(DYRun2Result* result, TString tag) {
  result->Update_HistName("all", "gen_DY",     "TUnfold1DTrue_inc0jet_"+tag);
  result->Update_HistName("all", "reco_DY",    "TUnfold1DReco_inc0jet_"+tag);
  result->Update_HistName("all", "reco_bkgMC", "TUnfold1DReco_inc0jet_"+tag);
  result->Update_HistName("all", "migM",       "TUnfold2DMig_inc0jet_"+tag);
}

void validation(TString channel);

void validation() {
  validation("mm");
  validation("ee");
}

void validation(TString channel) {
  // -- path to the shears output from dyjets-loop
  TString inputPath_loop = DYTool::path_default+"/"+channel;
  Run2Output* output_loop = new Run2Output(inputPath_loop);
  DYRun2Result* result_loop = new DYRun2Result(output_loop);
  result_loop->Produce();

  // -- path to the shears output from dyjets-loop-syst
  TString inputPath_syst = DYTool::path_systVar_effSF+"/"+channel;
  Run2Output* output_syst = new Run2Output(inputPath_syst);
  DYRun2Result* result_syst_default = new DYRun2Result(output_syst);
  result_syst_default->Produce();

  TString plotDirPath = DYTool::Set_PlotPath("EffSF/Validation");

  // -- validation: default values (dyjets-loop vs. dyjet-loop-syst)
  ResultComparator comparator_default(channel);
  comparator_default.Set_Case(result_loop,      "dyjets-loop, default");
  comparator_default.Set_Case(result_syst_default, "dyjets-loop-syst, default");
  comparator_default.Expect_PerfectAgreement(); // -- if you want to check isRatio==1
  comparator_default.Compare(plotDirPath+"/"+channel+"/default");

  // -- validation: systematic variations (default vs. central value of systematic variations)
  vector<TString> vec_effType = {"ID", "ISO", "STRIG", "DTRIG"};
  if( channel == "ee" ) vec_effType = {"RECO", "ID", "TRIG_Leg1", "TRIG_Leg2"};

  for(const auto& effType : vec_effType) {
    ResultComparator comparator(channel);
    comparator.Set_Case(result_syst_default, "dyjets-loop-syst, default");
    if( channel == "mm" ) {
      if( effType == "DTRIG" ) {
        TString systTag = TString::Format("%s-sf-cv", effType.Data());
        DYRun2Result* result_syst_cv = new DYRun2Result(output_syst);
        Update_HistName_DYRun2Result(result_syst_cv, systTag);
        result_syst_cv->Produce();

        comparator.Set_Case(result_syst_cv, "dyjets-loop-syst, "+systTag);
      }
      else { // -- ID, ISO, STRIG: data & mc separately
        TString systTag_data = TString::Format("%s-data-cv", effType.Data());
        TString systTag_mc   = TString::Format("%s-mc-cv", effType.Data());

        DYRun2Result* result_syst_data_cv = new DYRun2Result(output_syst);
        Update_HistName_DYRun2Result(result_syst_data_cv, systTag_data);
        result_syst_data_cv->Produce();

        DYRun2Result* result_syst_mc_cv = new DYRun2Result(output_syst);
        Update_HistName_DYRun2Result(result_syst_mc_cv, systTag_mc);
        result_syst_mc_cv->Produce();

        comparator.Set_Case(result_syst_data_cv, "dyjets-loop-syst, "+systTag_data);
        comparator.Set_Case(result_syst_mc_cv, "dyjets-loop-syst, "+systTag_mc);
      }
    } // -- channel == "mm"
    else {
      TString systTag = TString::Format("%s-sf-cv", effType.Data());
      DYRun2Result* result_syst_cv = new DYRun2Result(output_syst);
      Update_HistName_DYRun2Result(result_syst_cv, systTag);
      result_syst_cv->Produce();

      comparator.Set_Case(result_syst_cv, "dyjets-loop-syst, "+systTag);
    } // -- channel == "ee"

    comparator.Expect_PerfectAgreement();
    comparator.Compare(plotDirPath+"/"+channel+"/"+effType);
  } // -- iteration over effType
}