#include "zfinder.h"

#include <cmath>

#include <boost/math/constants/constants.hpp>

#include "functions.h" // deltaPhi
#include "logging.h"

namespace physics
{

dilepton::dilepton(const lepton &a, const lepton &b)
    : v(a.v + b.v), charge_product(a.charge * b.charge), a(a), b(b)
{
}

double dilepton::phistar() const
{
    const double pi = boost::math::constants::pi<double>();

    double phi_acop = pi - deltaPhi(a.v, b.v);
    double costhetastar;
    if (a.charge < 0) {
        costhetastar = std::tanh((a.v.Eta() - b.v.Eta()) / 2.);
    } else {
        costhetastar = std::tanh((b.v.Eta() - a.v.Eta()) / 2.);
    }
    double sinthetastar = std::sqrt(1. - costhetastar * costhetastar);
    return std::tan(phi_acop / 2.) * sinthetastar;
}

double dilepton::DeltaPhi() const
 {
     const double pi = boost::math::constants::pi<double>();
 
     double dphi = deltaPhi(a.v, b.v);
     
     return dphi;
 }


zfinder::zfinder(const util::options &opt, const std::string &name)
{
    if (!opt.config[name] || !opt.config[name].IsMap()) {
        util::logging::warn << "Couldn't find configuration for Z finder \"" << name << "\". "
                            << "Will use the defaults." << std::endl;
        return;
    }
    const YAML::Node node = opt.config[name];

    if (node["charge mode"]) {
        std::string mode = node["charge mode"].as<std::string>();
        if (mode == "none") {
            _charge_mode = charge_mode::none;
        } else if (mode == "neutral") {
            _charge_mode = charge_mode::neutral;
        } else if (mode == "same sign") {
            _charge_mode = charge_mode::same_sign;
        } else {
            throw std::invalid_argument("Invalid charge mode for Z finder \"" + name + "\": " +
                                        mode);
        }
    }

    if (node["flavor mode"]) {
        std::string mode = node["flavor mode"].as<std::string>();
        util::logging::debug << "Setting Z finder \"" << name << "\" flavor mode to \"" << mode
                             << "\"" << std::endl;
        if (mode == "none") {
            _flavor_mode = flavor_mode::none;
        } else if (mode == "same") {
            _flavor_mode = flavor_mode::same;
        } else if (mode == "ee") {
            _flavor_mode = flavor_mode::ee;
        } else if (mode == "mumu") {
            _flavor_mode = flavor_mode::mumu;
        } else if (mode == "emu") {
            _flavor_mode = flavor_mode::emu;
        } else {
            throw std::invalid_argument("Invalid flavor mode for Z finder \"" + name + "\": " +
                                        mode);
        }
    }

    util::set_value_safe(node, _mass_low, "low mass", "low mass for Z finder \"" + name + "\"");
    util::set_value_safe(node, _mass_high, "high mass", "high mass for Z finder \"" + name + "\"");
    util::set_value_safe(node, _leadingLepPt, "leading lepton pt", "leading lepton pT for Z finder \"" + name + "\"");
    util::set_value_safe(node, _newcut, "mll/subleading lepton pT", "mll/subleading lepton pT \"" + name + "\"");
    

    const YAML::Node node_gen = opt.config["generator level"];
    util::set_value_safe(node_gen, _leadingGenLepPt, "leading lepton pt", "gen leading lepton pt cut", [](double val) { return val >= 0; });
}

std::vector<dilepton> zfinder::find(const std::vector<lepton> &inputs, bool isGEN) const
{
    std::vector<dilepton> list;
    for (auto ita = inputs.cbegin(); ita != inputs.cend(); ++ita) {
        for (auto itb = ita + 1; itb != inputs.cend(); ++itb) {
            dilepton candidate(*ita, *itb);
            if (valid(candidate, isGEN)) {
                list.push_back(candidate);
            }
        }
    }
    return list;
}

bool zfinder::valid(const dilepton &candidate, bool isGEN) const
{
    // Check charge
    if (_charge_mode == charge_mode::neutral && candidate.charge_product > 0) {
        return false;
    } else if (_charge_mode == charge_mode::same_sign && candidate.charge_product < 0) {
        return false;
    }
    //cout <<"Zfinder, charge prod: " <<candidate.charge_product << ", mass: " << candidate.v.M() <<endl;

    // Check flavor
    if (_flavor_mode == flavor_mode::same &&
        std::abs(candidate.a.pdgid) != std::abs(candidate.b.pdgid)) {
        return false;
    } else if (_flavor_mode == flavor_mode::ee &&
               (std::abs(candidate.a.pdgid) != 11 || std::abs(candidate.b.pdgid) != 11)) {
        return false;
    } else if (_flavor_mode == flavor_mode::mumu &&
               (std::abs(candidate.a.pdgid) != 13 || std::abs(candidate.b.pdgid) != 13)) {
        return false;
    } else if (_flavor_mode == flavor_mode::emu &&
               (std::abs(candidate.a.pdgid) != 13 || std::abs(candidate.b.pdgid) != 11) &&
               (std::abs(candidate.a.pdgid) != 11 || std::abs(candidate.b.pdgid) != 13)) {
        return false;
    }

    // Check leading lepton pT
    // Always "a" is a leading lepton in pT as the leptons are given after sorting in pT
    if( isGEN ) {
      if( candidate.a.v.Pt() < _leadingGenLepPt ) return false;
    }
    else {
      if( candidate.a.v.Pt() < _leadingLepPt ) return false;
    }
    if(candidate.v.M()/candidate.b.v.Pt() > _newcut ) return false;

    // Check if affected by the 2016 EMTF bug
    //if (std::abs(candidate.a.v.Eta()) > 1.2 && std::abs(candidate.b.v.Eta()) > 1.2
    //    && candidate.a.v.Eta() * candidate.b.v.Eta() > 0.
    //    && deltaPhi(candidate.a.v.Phi(), candidate.b.v.Phi()) < 1.396) return false; // 80 deg * pi/180deg = 1.396 rad

    // Check mass
    // double mass = candidate.v.M();
    // return _mass_low < mass && mass < _mass_high;

    // does not check the mass here
    // to avoid rejecting under/overflow events for response matrices
    return true;
}

} // namespace physics
