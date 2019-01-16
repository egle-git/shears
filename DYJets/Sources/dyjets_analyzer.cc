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

dyjets_analyzer::dyjets_analyzer(util::job::info &info, const util::options &opt)
    : boson_jets_analyzer(info, opt), _zfinder(opt, "Z")
{
    counter.declare("With two good leptons");
    counter.declare("With two good electrons");
    counter.declare("With two good muons");
    counter.declare("With a good Z boson");
    counter.declare("With two good gen leptons");
    counter.declare("With two good gen electrons");
    counter.declare("With two good gen muons");
    counter.declare("With a good gen Z boson");

    std::string binning_file = "dyjets-binnings.yml";
    if (opt.config["binning file"]) {
        binning_file = opt.config["binning file"].as<std::string>();
    }
    util::logging::info << "Taking binnings from file " << binning_file << std::endl;
    histo_set.set_style(util::style_list(YAML::LoadFile(binning_file)));
    histo_set2D.set_style(util::style_list(YAML::LoadFile(binning_file)));
}

namespace /* anonymous */
{

void apply_mu_trigger_sf(physics::weights &w,
                         const physics::lepton &mu1,
                         const physics::lepton &mu2,
                         const util::tables &tab)
{
    if (w.ismc()) {
        if (mu1.raw_v.Pt() > mu2.raw_v.Pt()) {
            w.use_weight(tab.at("dimu trigger")
                             .getEfficiency(std::abs(mu1.raw_v.Eta()), std::abs(mu2.raw_v.Eta())));
        } else {
            w.use_weight(tab.at("dimu trigger")
                             .getEfficiency(std::abs(mu2.raw_v.Eta()), std::abs(mu1.raw_v.Eta())));
        }
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
                           const util::matched<event_contents> &evt)
{
    boson_jets_analyzer::fill(tag, evt);

    auto mass = evt.apply(&event_contents::get_boson_p).apply(&TLorentzVector::M);
    fill_unfolded("mass", tag, mass);
    fill_unfolded("mass_wide_range", tag, mass);

    auto pt = evt.apply(&event_contents::get_boson_p)
                 .apply((double (TLorentzVector::*)() const) &TLorentzVector::Pt);
    fill_unfolded("pt", tag, pt);

    if (!evt.rec) {
        return;
    }

    const auto boson = evt.rec->leptons;
    const auto jets = evt.rec->jets;

    physics::dilepton Z(boson[0], boson[1]);

    histo_set.fill("phistar", tag, Z.phistar(), weights().global_weight());

    /*
     * Variables in Z rest frame
     */

    TLorentzVector pZ = Z.v;
    TLorentzVector pLep1 = Z.a.v;
    TLorentzVector pLep2 = Z.b.v;
    if (Z.a.charge > 0) {
        std::swap(pLep1, pLep2);
    }

    // Rotate to decay frame with Z axis parallel to boson momentum
    double phi = Z.v.Phi();
    double theta = Z.v.Theta();

    pZ.RotateZ(-phi);
    pZ.RotateY(-theta);
    pLep1.RotateZ(-phi);
    pLep1.RotateY(-theta);
    pLep2.RotateZ(-phi);
    pLep2.RotateY(-theta);

    // Boost to decay frame with boson at rest
    pLep1.Boost(-pZ.BoostVector());
    pLep2.Boost(-pZ.BoostVector());
    pZ.Boost(-pZ.BoostVector());

    histo_set.fill("decay_costheta", tag,
                   std::cos(pLep1.Theta()), weights().global_weight());
}

std::vector<physics::lepton>
dyjets_analyzer::find_boson(const std::vector<physics::lepton> &muons,
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
    return {Z.a, Z.b};
}

std::vector<physics::lepton>
dyjets_analyzer::find_gen_boson(const std::vector<physics::lepton> &genleps)
{
    if (genleps.size() < 2) {
        return {};
    }
    counter.count("With two good gen leptons", weights().global_weight());

    if (std::abs(genleps[0].pdgid) == 13) {
        counter.count("With two good gen muons", weights().global_weight());
    } else {
        counter.count("With two good gen electrons", weights().global_weight());
    }

    std::vector<physics::dilepton> candidates = _zfinder.find({genleps[0], genleps[1]});
    if (candidates.size() == 0) {
        return {};
    }
    counter.count("With a good gen Z boson", weights().global_weight());

    std::sort(candidates.begin(), candidates.end(), physics::dilepton::zmass_ordering);
    physics::dilepton Z = candidates[0];
    return {Z.a, Z.b};
}

po::options_description dyjets_analyzer::options()
{
    return po::options_description("Physics options");
}
