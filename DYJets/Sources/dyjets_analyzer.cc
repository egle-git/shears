#include "dyjets_analyzer.h"

#include <algorithm>
#include <array>

#include <TFile.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "functions.h"
#include "lepton.h"

dyjets_analyzer::dyjets_analyzer(util::job::info &info, const util::options &opt)
    : EvtRunNum(info.reader, "EvtRunNum"),
      _jets(info, opt),
      _muons(info, opt, *this),
      _pileup(info, opt),
      _triggers(info),
      _mask_eraBG(info, opt.config["triggers B-F"].as<std::string>()),
      _mask_eraH(info, opt.config["triggers G-H"].as<std::string>()),
      _weights(info),
      _zfinder(opt, "Z")
{
    if (opt.config["tables B-F"]) {
        _tables_eraBF = opt.config["tables B-F"].as<util::tables>();
    }
    if (opt.config["tables G-H"]) {
        _tables_eraGH = opt.config["tables G-H"].as<util::tables>();
    }

    _jets.declare_histograms(*this);
    _pileup.declare_histograms(*this);
    declare("mass", "Dilepton mass", 40, 71, 111);
}

namespace /* anonymous */
{

void apply_trigger_sf(physics::weights &w,
                      const physics::lepton &mu1,
                      const physics::lepton &mu2,
                      const util::tables &tab)
{
    if (w.ismc()) {
        w.use_weight(tab.at("dimu trigger")
                         .getEfficiency(std::abs(mu1.raw_v.Eta()), std::abs(mu2.raw_v.Eta())));
    }
}
} // namespace anonymous

void dyjets_analyzer::operator()()
{
    using namespace physics;

    _weights.process_event();

    if (!passes_trigger()) {
        return;
    }

    std::vector<lepton> muons = _muons.get(_weights.isdata());
    if (muons.size() < 2) {
        return;
    }

    std::vector<dilepton> candidates = _zfinder.find({muons[0], muons[1]});
    if (candidates.size() == 0) {
        return;
    }

    std::sort(candidates.begin(), candidates.end(), dilepton::zmass_ordering);
    dilepton Z = candidates[0];

    // Only read jets once we have a Z
    std::vector<jet> jets = _jets.get();
    _jets.veto(jets, {Z.a, Z.b});

    _jets.fill(*this, "Zinc0jet_noweight", jets, _weights);
    _pileup.fill(*this, "Zinc0jet_noweight", _weights);
    _pileup.reweight(_weights);

    _muons.apply_sf(_weights, {Z.a, Z.b}, select(_tables_eraBF, _tables_eraGH));
    apply_trigger_sf(_weights, Z.a, Z.b, select(_tables_eraBF, _tables_eraGH));

    _jets.fill(*this, "Zinc0jet", jets, _weights);
    _muons.fill(*this, "Zinc0jet", {Z.a, Z.b}, _weights);
    _pileup.fill(*this, "Zinc0jet", _weights);
    fill("mass", "Zinc0jet", Z.v.M(), _weights.global_weight());
}

bool dyjets_analyzer::passes_trigger() { return select(_mask_eraBG, _mask_eraH).passes(_triggers); }

void dyjets_analyzer::write()
{
    _weights.write(this);
    histo_set::write();
}

po::options_description dyjets_analyzer::options()
{
    return po::options_description("Physics options");
}

template <class T> T &dyjets_analyzer::select(T &eraBG, T &eraGH)
{
    const unsigned run_threshold = 278820u; // start of Run G

    if (_weights.isdata() && *EvtRunNum < run_threshold) {
        return eraBG;
    } else {
        return eraGH;
    }
}
