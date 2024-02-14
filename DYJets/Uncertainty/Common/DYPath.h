#pragma once
#include <TString.h>
#include <TSystem.h>

namespace DYTool {

// -- path: path to shears path
// -- the structure should be like this:
// -- path
// ---- path/ee
// ------ path/ee/16pre
// ------ path/ee/16post
// ------ path/ee/17
// ------ path/ee/18
// ---- path/mm
// ------ path/mm/16pre
// ------ path/mm/16post
// ------ path/mm/17
// ------ path/mm/18

// -- the purpose is to keep track of the latest result .root files
// ---- the .root files can be kept in the storage element (where all users can access) in T2_BE_IIHE
// ---- (the output is too large to directly add in shears repository)

// -- the codes under "Uncertainty" directory will read below paths
// -- paths can be modified for your test, investigation, etc

TString path_SE = "/pnfs/iihe/cms/store/user/kplee";
TString path_base = path_SE+"/DYFullRun2/dsigmadm/result_240214/input_postProcess";
// TString path_base = "/Users/kplee/Research/Analysis/Logbook/231124_update_latestSetup_v2/input_postProcess";

// -- path to your "Uncertainty" Directory
// TString path_uncDir = "/Users/kplee/Research/Analysis/Logbook/231124_update_latestSetup_v2/shears/DYJets/Uncertainty";
TString path_uncDir = gSystem->Getenv("DYUNCPATH");

//////////////////////////////////////////////////
// -- the path to the default shears output  -- //
// -- (i.e. result from dyjets-loop)         -- //
// -- used in the validation                 -- //
//////////////////////////////////////////////////
TString path_default = path_base+"/shears/default";

// -- simple struct that holds histogram path
struct DYHistInfo {
  TString channel;
  TString era;
  TString fileName;
  TString histName;
};

TString path_default_fake = path_base+"/shears/fromMarijus/FakeBkg";

vector<DYTool::DYHistInfo> vec_fakeLepBkgInfo = {
  DYTool::DYHistInfo{"ee", "16pre",  DYTool::path_default_fake+"/EE/2016preAPV/dyjets-Fakes.root",  "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"ee", "16post", DYTool::path_default_fake+"/EE/2016postAPV/dyjets-Fakes.root", "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"ee", "17",     DYTool::path_default_fake+"/EE/2017/dyjets-Fakes.root",        "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"ee", "18",     DYTool::path_default_fake+"/EE/2018/dyjets-Fakes.root",        "mass_wide_range_inc0jet"},

  DYTool::DYHistInfo{"mm", "16pre",  DYTool::path_default_fake+"/MuMu/2016preAPV/dyjets-Fakes.root",  "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"mm", "16post", DYTool::path_default_fake+"/MuMu/2016postAPV/dyjets-Fakes.root", "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"mm", "17",     DYTool::path_default_fake+"/MuMu/2017/dyjets-Fakes.root",        "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"mm", "18",     DYTool::path_default_fake+"/MuMu/2018/dyjets-Fakes.root",        "mass_wide_range_inc0jet"}
};

///////////////////////////////////////////////////////////////
// -- path_systVar_*: shears output from dyjets-loop-syst -- //
// -- for each uncertainty source                         -- //
///////////////////////////////////////////////////////////////
TString path_systVar_theory_pu_l1pref_muP_elE = path_base+"/shears/theory_pu_l1pref_muP_elE";

TString path_systVar_pileup = path_systVar_theory_pu_l1pref_muP_elE;

TString path_systVar_L1Pref = path_systVar_theory_pu_l1pref_muP_elE;

TString path_systVar_muP = path_systVar_theory_pu_l1pref_muP_elE;

// TString path_systVar_theory = path_systVar_theory_pu_l1pref_muP_elE;
TString path_systVar_theory = path_base+"/shears/theory_5sigma";

TString path_systVar_elE = path_systVar_theory_pu_l1pref_muP_elE;

TString path_systVar_effSF = path_base+"/shears/effSF";

////////////////////////////////////////////////////////
// -- path for the additional/intermediate results -- //
////////////////////////////////////////////////////////

// -- path to the efficiency (pt-eta) map for the central value + their uncertainties (e.g. values from POGs)
TString path_centralEffMap = path_SE+"/Shears/Uncertainty/EffSF/v230730/input";

// -- path to the efficiency map w/ systematic variation
// -- (output of Uncertainty/EffSF/EffMap/generate_systEffMap.cxx)
TString path_systEffMap = path_SE+"/Shears/Uncertainty/EffSF/v230730/LeptonEffMap_FullRun2.root";

// -- input for the acceptance results (output from miniAOD)
// TString path_inputForAcc = path_base+"/miniAOD/acc";
TString path_inputForAcc = path_uncDir+"/Acceptance/output";

// -- acceptance values
TString path_acc = path_uncDir+"/Acceptance/DYAcceptance.root";
TString path_theoryPred      = path_uncDir+"/Acceptance/TheoryPrediction_VariousPDF.root";
TString path_theoryPred_m200 = path_uncDir+"/Acceptance/TheoryPrediction_VariousPDF_aboveM200.root"; // -- theory predictions (m > 200 only)
};