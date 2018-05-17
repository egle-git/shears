#include "muons.h"

#include <boost/math/constants/constants.hpp>

#include "logging.h"
#include "RoccoR.h"

namespace physics
{

muons::muons(util::job::info &info, const util::options &opt, util::histo_set &h)
    : MuPt(info.reader, "MuPt"),
      MuEta(info.reader, "MuEta"),
      MuPhi(info.reader, "MuPhi"),
      MuE(info.reader, "MuE"),
      MuCh(info.reader, "MuCh"),
      MuPfIso(info.reader, "MuPfIso"),
      MuTkLayerCnt(info.reader, "MuTkLayerCnt"),
      MuIdTight(info.reader, "MuIdTight")
{
    configure(opt);

    const double pi = boost::math::constants::pi<double>();

    h.declare("muPt", "Muon pt;Muon p_{T} [GeV]", 40, 0, 200);
    h.declare("muEta", "Muon eta;Muon #eta", 24, -2.4, 2.4);
    h.declare("muPhi", "Muon phi;Muon #phi", 24, -pi, pi);
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
    util::set_value_safe(
        node, _roccor_enabled, "use rochester correction", "rochester correction toggle");
    if (_roccor_enabled) {
        std::string roccor_dir = "rcdata.2016.v3";
        if (node["rochester correction path"]) {
            roccor_dir = node["rochester correction path"].as<std::string>();
        }
        roccor_dir = "EfficiencyTables/" + roccor_dir;
        _roccor = std::make_shared<RoccoR>(roccor_dir);
    }
}

std::vector<lepton> muons::get(bool isdata)
{
    std::vector<lepton> muons;
    for (unsigned i = 0; i < MuPt.GetSize(); ++i) {
        lepton l;
        if (std::abs(MuEta[i]) > _eta_cut || MuPfIso[i] > _iso_cut) {
            continue;
        }
        l.v.SetPtEtaPhiE(MuPt[i], MuEta[i], MuPhi[i], MuE[i]);
        l.raw_v = l.v;
        l.charge = MuCh[i];
        l.iso = MuPfIso[i];
        l.id = MuIdTight[i];
        if (!(l.id & 1)) {
            continue;
        }
        if (_roccor_enabled) {
            if (isdata) {
                l.v *= _roccor->kScaleDT(l.charge, l.v.Pt(), l.v.Eta(), l.v.Phi(), 0, 0);
            } else {
                l.v *= _roccor->kScaleAndSmearMC(l.charge,
                                                 l.v.Pt(),
                                                 l.v.Eta(),
                                                 l.v.Phi(),
                                                 MuTkLayerCnt[i],
                                                 gRandom->Rndm(), // TODO kill gRandom
                                                 gRandom->Rndm(), // TODO kill gRandom
                                                 0,
                                                 0);
            }
        }
        if (l.v.Pt() < _pt_cut) {
            continue;
        }
        muons.push_back(l);
    }
    if (_roccor_enabled) {
        std::sort(muons.begin(), muons.end(), [](const lepton &lhs, const lepton &rhs)
            {
                return lhs.v.Pt() > rhs.v.Pt();
            }
        );
    }
    return muons;
}

void muons::apply_sf(weights &w, const std::vector<lepton> &muons, const util::tables &tab) const
{
    if (w.ismc()) {
        for (const lepton &mu : muons) {
            if (_id_sf_enabled) {
                w.use_weight(
                    tab.at("muon id").getEfficiency(mu.raw_v.Pt(), std::abs(mu.raw_v.Eta())));
            }
            if (_iso_sf_enabled) {
                w.use_weight(tab.at("muon isolation")
                                 .getEfficiency(mu.raw_v.Pt(), std::abs(mu.raw_v.Eta())));
            }
            if (_trk_sf_enabled) {
                w.use_weight(
                    tab.at("muon tracking").getEfficiency(mu.raw_v.Pt(), std::abs(mu.raw_v.Eta())));
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
} // namespace physics
