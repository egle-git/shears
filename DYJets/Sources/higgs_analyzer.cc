#include "higgs_analyzer.h"

#include <algorithm>

#include <TFile.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "lepton.h"

higgs_analyzer::higgs_analyzer(util::job::info &info, const util::options &opt)
    : EvtRunNum(info.reader, "EvtRunNum"),
      _jets(info, opt),
      _muons(info, opt, *this),
      _pileup(info, 2016, 0),
      _triggers(info),
      _mask_eraBG(info, "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL, HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL"),
      _mask_eraH(info, "HLT_Mu17_TrkIsoVVL_Mu8_TrkIsoVVL_DZ, HLT_Mu17_TrkIsoVVL_TkMu8_TrkIsoVVL_DZ"),
      _weights(info)
{
    _jets.declare_histograms(*this);
    _pileup.declare_histograms(*this);
    declare("mass", "Dilepton mass", 100, 0, 200);
}

void higgs_analyzer::operator()()
{
    using namespace physics;

    _weights.process_event();

    if (!passes_trigger()) {
        return;
    }

    std::vector<lepton> muons = _muons.get();

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

        // Only read jets once we have a Z
        std::vector<jet> jets = _jets.get();
        _jets.veto(jets, {muons[0], muons[1]});

        _jets.fill(*this, "Zinc0jet_noweight", jets, _weights);
        _pileup.fill(*this, "Zinc0jet_noweight", _weights);
        _pileup.reweight(_weights);

        _jets.fill(*this, "Zinc0jet", jets, _weights);
        _muons.fill(*this, "Zinc0jet", muons, _weights);
        _pileup.fill(*this, "Zinc0jet", _weights);
        fill("mass", "Zinc0jet", pZ.M(), _weights.global_weight());
    }
}

bool higgs_analyzer::passes_trigger()
{
    const unsigned run_threshold = 278820u; // start of Run G

    if (_weights.isdata() && *EvtRunNum < run_threshold) {
        return _mask_eraBG.passes(_triggers);
    } else {
        return _mask_eraH.passes(_triggers);
    }
}

void higgs_analyzer::write()
{
    _weights.write(this);
    histo_set::write();
}

po::options_description higgs_analyzer::options()
{
    return po::options_description("Physics options");
}
