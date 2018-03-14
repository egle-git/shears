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
      _pileup(info, opt.config["year"].as<int>(), 0),
      _triggers(info),
      _mask_eraBG(info, opt.config["triggers B-F"].as<std::string>()),
      _mask_eraH(info, opt.config["triggers G-H"].as<std::string>()),
      _weights(info)
{
    if (opt.config["tables B-F"]) {
        _tables_eraBF = opt.config["tables B-F"].as<util::tables>();
    }
    if (opt.config["tables G-H"]) {
        _tables_eraBF = opt.config["tables G-H"].as<util::tables>();
    }

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
        if (pZ.M() < 71 || pZ.M() > 111) {
            return;
        }

        // Only read jets once we have a Z
        std::vector<jet> jets = _jets.get();
        _jets.veto(jets, {muons[0], muons[1]});

        _jets.fill(*this, "Zinc0jet_noweight", jets, _weights);
        _pileup.fill(*this, "Zinc0jet_noweight", _weights);
        _pileup.reweight(_weights);

        _muons.apply_sf(_weights, {muons[0], muons[1]}, _tables_eraBF);

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
