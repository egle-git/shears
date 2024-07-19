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

// -- the purpose is to keep track of the latest result .root files.
// ---- the .root files can be kept in the storage element (where all users can access) in T2_BE_IIHE.
// ---- (the output is too large to directly add in shears repository)

// -- the codes under "DYJets/PostProcess" directory will read below paths.
// -- paths can be modified for your test, investigation, etc.

TString path_SE = "/pnfs/iihe/cms/store/user/kplee";
TString path_base = path_SE+"/DYFullRun2/dsigmadm/result_240716/input_postProcess";
// TString path_base = "/Users/kplee/Research/Analysis/Logbook/240419_update_bVeto/data";

//////////////////////////////////////////////////
// -- the path to the default shears output  -- //
// -- (i.e. result from dyjets-loop)         -- //
// -- used in the validation                 -- //
//////////////////////////////////////////////////
TString path_default = path_base+"/shears/default";

/////////////////////////////////////////
// ---- for fake lepton backgrounds -- //
/////////////////////////////////////////
// -- simple struct that holds histogram path
struct DYHistInfo {
  TString channel;
  TString era;
  TString fileName;
  TString histName;
};

// TString path_default_fake = path_base+"/shears/fromMarijus/FakeBkg";
TString path_default_fake = path_default;

vector<DYTool::DYHistInfo> vec_fakeLepBkgInfo = {
  DYTool::DYHistInfo{"ee", "16pre",  DYTool::path_default_fake+"/ee/16pre/dyjets-Fakes.root",  "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"ee", "16post", DYTool::path_default_fake+"/ee/16post/dyjets-Fakes.root", "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"ee", "17",     DYTool::path_default_fake+"/ee/17/dyjets-Fakes.root",     "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"ee", "18",     DYTool::path_default_fake+"/ee/18/dyjets-Fakes.root",     "mass_wide_range_inc0jet"},

  DYTool::DYHistInfo{"mm", "16pre",  DYTool::path_default_fake+"/mm/16pre/dyjets-Fakes.root",  "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"mm", "16post", DYTool::path_default_fake+"/mm/16post/dyjets-Fakes.root", "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"mm", "17",     DYTool::path_default_fake+"/mm/17/dyjets-Fakes.root",     "mass_wide_range_inc0jet"},
  DYTool::DYHistInfo{"mm", "18",     DYTool::path_default_fake+"/mm/18/dyjets-Fakes.root",     "mass_wide_range_inc0jet"}
};

///////////////////////////////////////////////////////////////
// -- path_systVar_*: shears output from dyjets-loop-syst -- //
// -- for each uncertainty source                         -- //
///////////////////////////////////////////////////////////////
// TString path_systVar_theory_pu_l1pref_muP_elE = path_base+"/shears/theory_pu_l1pref_muP_elE";
TString path_systVar_lepE_bVeto_pileup_L1Pref = path_base+"/shears/lepE_bVeto_pileup_L1Pref";

TString path_systVar_pileup = path_systVar_lepE_bVeto_pileup_L1Pref;

TString path_systVar_L1Pref = path_systVar_lepE_bVeto_pileup_L1Pref;

TString path_systVar_muP = path_systVar_lepE_bVeto_pileup_L1Pref;

TString path_systVar_theory = path_base+"/shears/theory";

TString path_systVar_elE = path_systVar_lepE_bVeto_pileup_L1Pref;

TString path_systVar_effSF = path_base+"/shears/effSF";

TString path_systVar_bVeto = path_systVar_lepE_bVeto_pileup_L1Pref;

TString path_systVar_bkg_topRwgt = path_base+"/shears/bkg_topRwgt";

TString path_systVar_bkg_fakeLep = path_base+"/shears/bkg_fakeLep";

/////////////////////////////////////////////////////////////
// -- path for additional inputs outside of shears -- //
/////////////////////////////////////////////////////////////

// -- path to the efficiency (pt-eta) map for the central value + their uncertainties (e.g. values from POGs)
TString path_centralEffMap = path_SE+"/Shears/Uncertainty/EffSF/v230730/input";

// -- input for the acceptance results (output from miniAOD)
TString path_inputForAcc = path_base+"/miniAOD/acc";

///////////////////////////////////////////////
// -- path for the post-processed outputs -- //
///////////////////////////////////////////////

TString path_pp = gSystem->Getenv("DYPPPATH"); // -- pp = postProcess
TString path_output = path_pp+"/Output"; // -- where the output .root files are saved
TString path_plot   = path_pp+"/Output/Plot"; // -- where the output plots are saved

// -- acceptance values
TString path_acc             = path_output+"/DYAcceptance.root";
TString path_theoryPred      = path_output+"/TheoryPrediction_VariousPDF.root";
TString path_theoryPred_m200 = path_output+"/TheoryPrediction_VariousPDF_aboveM200.root"; // -- theory predictions (m > 200 only)

};