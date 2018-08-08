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
      _zfinder(opt, "Z")
{
    counter.declare("With two good leptons");
    counter.declare("With two good electrons");
    counter.declare("With two good muons");
    counter.declare("With a good Z boson");

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

void dyjets_analyzer::apply_trigger_sf(physics::weights &weights,
                                       const std::vector<physics::lepton> &leptons)
{
    if (leptons[0].pdgid == 13) {
        apply_mu_trigger_sf(weights, leptons[0], leptons[1], tables());
    } else {
        apply_el_trigger_sf(weights, leptons[0], leptons[1], tables());
    }
}

void dyjets_analyzer::fill(const std::string &tag,
                           const std::vector<physics::lepton> &boson,
                           const std::vector<physics::jet> &jets)
{
    physics::dilepton Z(boson[0], boson[1]);

    histo_set.fill("mass", tag, Z.v.M(), weights().global_weight());
    histo_set.fill("pt", tag, Z.v.Pt(), weights().global_weight());
}

std::vector<physics::lepton> dyjets_analyzer::find_boson(
    const std::vector<physics::lepton> &muons,
    const std::vector<physics::lepton> &electrons)
{
    if (muons.size() < 2 && electrons.size() < 2) {
        return {};
    }
    counter.count("With two good leptons", weights().global_weight());

    std::vector<physics::lepton> leptons;
    if (muons.size() >= 2) {
        counter.count("With two good muons", weights().global_weight());
        leptons = muons;
    } else {
        counter.count("With two good electrons", weights().global_weight());
        leptons = electrons;
    }

    std::vector<physics::dilepton> candidates = _zfinder.find({leptons[0], leptons[1]});
    if (candidates.size() == 0) {
        return {};
    }
    counter.count("With a good Z boson", weights().global_weight());

    std::sort(candidates.begin(), candidates.end(), physics::dilepton::zmass_ordering);
    physics::dilepton Z = candidates[0];
    return { Z.a, Z.b };
}

void dyjets_analyzer::write()
{
    counter.print();
    weights().write(&histo_set);
    histo_set.write();
}

po::options_description dyjets_analyzer::options()
{
    return po::options_description("Physics options");
}
