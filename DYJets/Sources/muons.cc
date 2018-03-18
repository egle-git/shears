#include "muons.h"

#include <boost/math/constants/constants.hpp>

#include "logging.h"

namespace physics
{

muons::muons(util::job::info &info, const util::options &opt, util::histo_set &h)
    : MuPt(info.reader, "MuPt"),
      MuEta(info.reader, "MuEta"),
      MuPhi(info.reader, "MuPhi"),
      MuE(info.reader, "MuE"),
      MuCh(info.reader, "MuCh"),
      MuPfIso(info.reader, "MuPfIso"),
      MuIdTight(info.reader, "MuIdTight")
{
    configure(opt);

    const double pi = boost::math::constants::pi<double>();

    h.declare("muPt", "Muon pt", 40, 0, 200);
    h.declare("muEta", "Muon eta", 24, -2.4, 2.4);
    h.declare("muPhi", "Muon phi", 24, -pi, pi);
}

void muons::configure(const util::options &opt)
{
    const YAML::Node node = opt.config["muons"];
    util::set_value_safe(node, _pt_cut, "pt", "muon pt cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _eta_cut, "eta", "muon eta cut", [](double val) { return val > 0; });
    util::set_value_safe(
        node, _iso_cut, "isolation", "muon isolation cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _id_sf_enabled, "use id scale factors", "id scale factors toggle");
    util::set_value_safe(
        node, _iso_sf_enabled, "use isolation scale factors", "isolation scale factors toggle");
    util::set_value_safe(
        node, _trk_sf_enabled, "use tracking scale factors", "tracking scale factors toggle");
}

std::vector<lepton> muons::get()
{
    std::vector<lepton> muons;
    for (unsigned i = 0; i < MuPt.GetSize(); ++i) {
        lepton l;
        if (MuPt[i] < _pt_cut || std::abs(MuEta[i]) > _eta_cut || MuPfIso[i] > _iso_cut) {
            continue;
        }
        l.v.SetPtEtaPhiE(MuPt[i], MuEta[i], MuPhi[i], MuE[i]);
        l.charge = MuCh[i];
        l.iso = MuPfIso[i];
        l.id = MuIdTight[i];
        if (!(l.id & 1)) {
            continue;
        }
        muons.push_back(l);
    }
    return muons;
}

void muons::apply_sf(weights &w, const std::vector<lepton> &muons, const util::tables &tab) const
{
    if (w.ismc()) {
        for (const lepton &mu : muons) {
            if (_id_sf_enabled) {
                w.use_weight(tab.at("muon id").getEfficiency(mu.v.Pt(), std::abs(mu.v.Eta())));
            }
            if (_iso_sf_enabled) {
                w.use_weight(
                    tab.at("muon isolation").getEfficiency(mu.v.Pt(), std::abs(mu.v.Eta())));
            }
            if (_trk_sf_enabled) {
                w.use_weight(
                    tab.at("muon tracking").getEfficiency(mu.v.Pt(), std::abs(mu.v.Eta())));
            }
        }
    }
}

void muons::fill(util::histo_set &h,
                 const std::string &tag,
                 const std::vector<lepton> &muons,
                 const weights &w)
{
    for (const lepton &mu : muons) {
        h.fill("muPt", tag, mu.v.Pt(), w.global_weight());
        h.fill("muEta", tag, mu.v.Eta(), w.global_weight());
        h.fill("muPhi", tag, mu.v.Phi(), w.global_weight());
    }
    if (muons.size() > 0) {
        const lepton &mu = muons[0];
        h.fill("muPt", "leading_" + tag, mu.v.Pt(), w.global_weight());
        h.fill("muEta", "leading_" + tag, mu.v.Eta(), w.global_weight());
        h.fill("muPhi", "leading_" + tag, mu.v.Phi(), w.global_weight());
    }
    if (muons.size() > 1) {
        const lepton &mu = muons[1];
        h.fill("muPt", "subleading_" + tag, mu.v.Pt(), w.global_weight());
        h.fill("muEta", "subleading_" + tag, mu.v.Eta(), w.global_weight());
        h.fill("muPhi", "subleading_" + tag, mu.v.Phi(), w.global_weight());
    }
    if (muons.size() > 2) {
        const lepton &mu = muons[2];
        h.fill("muPt", "third_" + tag, mu.v.Pt(), w.global_weight());
        h.fill("muEta", "third_" + tag, mu.v.Eta(), w.global_weight());
        h.fill("muPhi", "third_" + tag, mu.v.Phi(), w.global_weight());
    }
    if (muons.size() > 3) {
        const lepton &mu = muons[3];
        h.fill("muPt", "fourth_" + tag, mu.v.Pt(), w.global_weight());
        h.fill("muEta", "fourth_" + tag, mu.v.Eta(), w.global_weight());
        h.fill("muPhi", "fourth_" + tag, mu.v.Phi(), w.global_weight());
    }
}
}
