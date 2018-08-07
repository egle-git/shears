#include "dyjets_analyzer.h"

#include <algorithm>
#include <array>
#include <random>

#include <TFile.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "functions.h"
#include "lepton.h"

namespace /* anonymous */
{
    double zpt_binning[] = {0.1,  1.,   2.,   3.,   4.,   5.,   6.,   7.,   8.,   9.,
                            10.,  11.,  12.,  13.,  14.,  16.,  18.,  20.,  22.,  25.,
                            28.,  32.,  37.,  43.,  52.,  65.,  85.,  120., 160., 190.,
                            220., 250., 300., 350., 400., 450., 500., 1000.};
}

dyjets_analyzer::dyjets_analyzer(util::job::info &info, const util::options &opt)
    : boson_jets_analyzer(info, opt),
      _electrons(info, opt, histo_set),
      _jets(info, opt),
      _muons(info, opt, histo_set),
      _pileup(info, opt),
      _triggers(info),
      _mask_eraBG(info, opt.config["triggers B-F"].as<std::string>()),
      _mask_eraH(info, opt.config["triggers G-H"].as<std::string>()),
      _zfinder(opt, "Z")
{
    _jets.declare_histograms(histo_set);
    _pileup.declare_histograms(histo_set);

    _counter.declare("Total");
    _counter.declare("Passing the trigger");
    _counter.declare("With two good leptons");
    _counter.declare("With two good electrons");
    _counter.declare("With two good muons");
    _counter.declare("With a good Z boson");

    histo_set.declare("mass", "Dilepton mass;M(ll) [GeV]", 40, 71, 111);
    histo_set.declare("pt", "Dilepton p_{T};p_{T}(ll) [GeV]", sizeof(zpt_binning) / sizeof(double) - 1, zpt_binning);
}

namespace /* anonymous */
{

void apply_mu_trigger_sf(physics::weights &w,
                         const physics::lepton &mu1,
                         const physics::lepton &mu2,
                         const util::tables &tab)
{
    if (w.ismc()) {
        w.use_weight(tab.at("dimu trigger")
                        .getEfficiency(std::abs(mu1.raw_v.Eta()), std::abs(mu2.raw_v.Eta())));
    }
}

void apply_el_trigger_sf(physics::weights &w,
                         const physics::lepton &e1,
                         const physics::lepton &e2,
                         const util::tables &tab)
{
    if (w.ismc()) {
        w.use_weight(tab.at("diel trigger")
                        .getEfficiency(std::abs(e1.raw_v.Eta()), std::abs(e2.raw_v.Eta())));
    }
}
} // namespace anonymous

void dyjets_analyzer::analyze()
{
    using namespace physics;

    _counter.count("Total", weights().global_weight());

    if (!passes_trigger()) {
        return;
    }
    _counter.count("Passing the trigger", weights().global_weight());

    std::vector<lepton> muons = _muons.get(weights().isdata());
    std::vector<lepton> electrons = _electrons.get();

    std::vector<lepton> leptons = find_boson(muons, electrons);
    if (leptons.size() != 2) {
        return;
    }
    physics::dilepton Z(leptons[0], leptons[1]);

    // Only read jets once we have a Z
    std::vector<jet> jets = _jets.get();
    _jets.veto(jets, {Z.a, Z.b});

    _jets.fill(histo_set, "Zinc0jet_noweight", jets, weights());
    _pileup.fill(histo_set, "Zinc0jet_noweight", weights());
    _pileup.reweight(_weights);

    if (Z.a.pdgid == 13) {
        _muons.apply_sf(_weights, {Z.a, Z.b}, tables());
        apply_mu_trigger_sf(_weights, Z.a, Z.b, tables());
    } else {
        _electrons.apply_sf(_weights, {Z.a, Z.b}, tables());
        apply_el_trigger_sf(_weights, Z.a, Z.b, tables());
    }

    for (unsigned njets = 0; njets < 3; ++njets) {
        std::stringstream ss;
        ss << "Zinc" << njets << "jet";
        std::string tag = ss.str();

        _jets.fill(histo_set, tag, jets, weights());
        if (Z.a.pdgid == 13) {
            _muons.fill(histo_set, tag, {Z.a, Z.b}, weights());
        } else {
            _electrons.fill(histo_set, tag, {Z.a, Z.b}, weights());
        }
        _pileup.fill(histo_set, tag, weights());
        histo_set.fill("mass", tag, Z.v.M(), weights().global_weight());
        histo_set.fill("pt", tag, Z.v.Pt(), weights().global_weight());

        if (jets.size() == njets) {
            ss.str("");
            ss << "Zexc" << njets << "jet";
            std::string tag = ss.str();

            _jets.fill(histo_set, tag, jets, weights());
            if (Z.a.pdgid == 13) {
                _muons.fill(histo_set, tag, {Z.a, Z.b}, weights());
            } else {
                _electrons.fill(histo_set, tag, {Z.a, Z.b}, weights());
            }
            _pileup.fill(histo_set, tag, weights());
            histo_set.fill("mass", tag, Z.v.M(), weights().global_weight());
            histo_set.fill("pt", tag, Z.v.Pt(), weights().global_weight());

            break;
        }
    }
}

std::vector<physics::lepton> dyjets_analyzer::find_boson(
    const std::vector<physics::lepton> &muons,
    const std::vector<physics::lepton> &electrons)
{
    if (muons.size() < 2 && electrons.size() < 2) {
        return {};
    }
    _counter.count("With two good leptons", weights().global_weight());

    std::vector<physics::lepton> leptons;
    if (muons.size() >= 2) {
        _counter.count("With two good muons", weights().global_weight());
        leptons = muons;
    } else {
        _counter.count("With two good electrons", weights().global_weight());
        leptons = electrons;
    }

    std::vector<physics::dilepton> candidates = _zfinder.find({leptons[0], leptons[1]});
    if (candidates.size() == 0) {
        return {};
    }
    _counter.count("With a good Z boson", weights().global_weight());

    std::sort(candidates.begin(), candidates.end(), physics::dilepton::zmass_ordering);
    physics::dilepton Z = candidates[0];
    return { Z.a, Z.b };
}

bool dyjets_analyzer::passes_trigger() { return era_select(_mask_eraBG, _mask_eraH).passes(_triggers); }

void dyjets_analyzer::write()
{
    _counter.print();
    weights().write(&histo_set);
    histo_set.write();
}

po::options_description dyjets_analyzer::options()
{
    return po::options_description("Physics options");
}
