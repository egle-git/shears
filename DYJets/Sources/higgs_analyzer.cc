#include "higgs_analyzer.h"

#include <algorithm>

#include <TFile.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "lepton.h"

higgs_analyzer::higgs_analyzer(util::job::info &info, const util::options &opt)
    : muons_analyzer(info, opt),
      EvtRunNum(info.reader, "EvtRunNum"),
      triggers(info),
      mask_eraBG(info, "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL, HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL"),
      mask_eraH(info, "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ, HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ"),
      weights(info)
{
    declare("mass", "Dilepton mass", 100, 0, 200);
}

void higgs_analyzer::operator()()
{
    using namespace physics;

    weights.process_event();

    if (!passes_trigger()) {
        return;
    }

    std::vector<lepton> muons = get_muons();

    if (muons.size() >= 2) {
        TLorentzVector pZ = muons[0].v + muons[1].v;
        float charge_check = muons[0].charge * muons[1].charge;

        if (charge_check >= 0) {
            return;
        }
        if (pZ.M() < 55) {
            // We don't include the MC below M=50, adding 5 GeV to be sure
            return;
        }
        fill_muons(muons, weights, "Zinc0jet");
        fill("mass", "Zinc0jet", pZ.M(), weights.global_weight());
    }
}

bool higgs_analyzer::passes_trigger()
{
    const unsigned run_threshold = 278820u; // start of Run G

    if (weights.isdata() && *EvtRunNum < run_threshold) {
        return mask_eraBG.passes(triggers);
    } else {
        return mask_eraH.passes(triggers);
    }
}

void higgs_analyzer::write()
{
    weights.write(this);
    histo_set::write();
}

po::options_description higgs_analyzer::options()
{
    return po::options_description("Physics options");
}
