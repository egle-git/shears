#pragma once
#include <TString.h>

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

//////////////////////////////////////////////////
// -- the path to the default shears output  -- //
// -- (i.e. result from dyjets-loop)         -- //
// -- used in the validation                 -- //
//////////////////////////////////////////////////
// TString path_default = "/Users/kplee/Research/Analysis/Logbook/230608_unc_effSF/input/default";
TString path_default = "/Users/kplee/Research/Analysis/Logbook/231117_update_latestSetup/shearsOutput";

// -- default fake histograms
// TString path_default_fake_ee = "";
// TString path_default_fake_mm = "";

// -- simple struct that holds histogram path
struct DYHistInfo {
  TString channel;
  TString era;
  TString fileName;
  TString histName;
};


TString path_default_fake = "/Users/kplee/Research/Analysis/Logbook/231106_DileptonMassPlot_ForSMPV/shearsOutput_fromMarijus/fake_syncBin";
vector<DYTool::DYHistInfo> vec_fakeLepBkgInfo = {
  DYTool::DYHistInfo{"ee", "16pre",  DYTool::path_default_fake+"/dyjets-fake_ee_16pre.root",  "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"ee", "16post", DYTool::path_default_fake+"/dyjets-fake_ee_16post.root", "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"ee", "17",     DYTool::path_default_fake+"/dyjets-fake_ee_17.root",     "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"ee", "18",     DYTool::path_default_fake+"/dyjets-fake_ee_18.root",     "mass_wide_range_inc0jet"},

  DYTool::DYHistInfo{"mm", "16pre",  DYTool::path_default_fake+"/dyjets-fake_mm_16pre.root",  "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"mm", "16post", DYTool::path_default_fake+"/dyjets-fake_mm_16post.root", "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"mm", "17",     DYTool::path_default_fake+"/dyjets-fake_mm_17.root",     "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"mm", "18",     DYTool::path_default_fake+"/dyjets-fake_mm_18.root",     "mass_wide_range_inc0jet"}
};


///////////////////////////////////////////////////////////////
// -- path_systVar_*: shears output from dyjets-loop-syst -- //
// -- for each uncertainty source                         -- //
///////////////////////////////////////////////////////////////
TString path_systVar_effSF = "/Users/kplee/Research/Analysis/Logbook/230608_unc_effSF/input";

TString path_systVar_pileup = "/Users/kplee/Research/Analysis/Logbook/230729_update_Syst/Uncertainty/input_pileup_L1Pref";

TString path_systVar_L1Pref = "/Users/kplee/Research/Analysis/Logbook/230729_update_Syst/Uncertainty/input_pileup_L1Pref";

// TString path_systVar_muP = "/Users/kplee/Research/Analysis/Logbook/230729_update_Syst/Uncertainty/input_theory_muP/mm";
TString path_systVar_muP = "/Users/kplee/Research/Analysis/Logbook/230729_update_Syst/Uncertainty/input_muP/mm";

TString path_systVar_theory = "/Users/kplee/Research/Analysis/Logbook/230729_update_Syst/Uncertainty/input_theory_muP";

TString path_systVar_elE = "/Users/kplee/Research/Analysis/Logbook/230729_update_Syst/Uncertainty/input_elE/ee";

////////////////////////////////////////////////////////
// -- path for the additional/intermediate results -- //
////////////////////////////////////////////////////////

// -- path to the efficiency map for the central value + their uncertainties (e.g. values from POGs)
TString path_centralEffMap = path_SE+"/Shears/Uncertainty/EffSF/v230730/input";

// -- path to the efficiency map w/ systematic variation
// -- (output of Uncertainty/EffSF/EffMap/generate_systEffMap.cxx)
TString path_systEffMap = path_SE+"/Shears/Uncertainty/EffSF/v230730/LeptonEffMap_FullRun2.root";

// -- path to the DYRun2Result output from eff. SF variation
// -- (output of Uncertainty/EffSF/estimate_unc_cov.cxx)
TString path_DYResult_effSF_ee = path_SE+"/Shears/Uncertainty/EffSF/v230730/Unfolded_SystVar_EffSF_ee.root";
TString path_DYResult_effSF_mm = path_SE+"/Shears/Uncertainty/EffSF/v230730/Unfolded_SystVar_EffSF_mm.root";

// -- path to the Uncertainty output from eff. SF variation
// -- (output of Uncertainty/EffSF/estimate_unc_cov.cxx)
TString path_unc_effSF_ee = path_SE+"/Shears/Uncertainty/EffSF/v230730/Uncertainty_EffSF_ee.root";
TString path_unc_effSF_mm = path_SE+"/Shears/Uncertainty/EffSF/v230730/Uncertainty_EffSF_mm.root";

};