#include "muons.h"

#include <boost/math/constants/constants.hpp>

namespace physics
{

muons_analyzer::muons_analyzer(TTreeReader &reader, const util::options &opt)
    : MuPt(reader, "MuPt"),
      MuEta(reader, "MuEta"),
      MuPhi(reader, "MuPhi"),
      MuE(reader, "MuE"),
      MuCh(reader, "MuCh"),
      MuPfIso(reader, "MuPfIso"),
      MuIdTight(reader, "MuIdTight")
{
    configure(opt);

    const double pi = boost::math::constants::pi<double>();

    declare("muPt", "Muon pt", 40, 0, 200);
    declare("muEta", "Muon eta", 24, -2.4, 2.4);
    declare("muPhi", "Muon phi", 24, -pi, pi);
}

void muons_analyzer::configure(const util::options &opt)
{
    const YAML::Node node = opt.config["muons"];
    util::set_value_safe(node, _pt_cut, "pt", "muon pt cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _eta_cut, "eta", "muon eta cut", [](double val) { return val > 0; });
    util::set_value_safe(
        node, _iso_cut, "isolation", "muon isolation cut", [](double val) { return val >= 0; });
}

std::vector<lepton> muons_analyzer::get_muons()
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
        muons.push_back(l);
    }
    return muons;
}

void muons_analyzer::fill_muons(const std::vector<lepton> &muons, const std::string &tag)
{
    for (const lepton &mu : muons) {
        fill("muPt", tag, mu.v.Pt());
        fill("muEta", tag, mu.v.Eta());
        fill("muPhi", tag, mu.v.Phi());
    }
    if (muons.size() > 0) {
        const lepton &mu = muons[0];
        fill("muPt", "leading_" + tag, mu.v.Pt());
        fill("muEta", "leading_" + tag, mu.v.Eta());
        fill("muPhi", "leading_" + tag, mu.v.Phi());
    }
    if (muons.size() > 1) {
        const lepton &mu = muons[1];
        fill("muPt", "subleading_" + tag, mu.v.Pt());
        fill("muEta", "subleading_" + tag, mu.v.Eta());
        fill("muPhi", "subleading_" + tag, mu.v.Phi());
    }
    if (muons.size() > 2) {
        const lepton &mu = muons[2];
        fill("muPt", "third_" + tag, mu.v.Pt());
        fill("muEta", "third_" + tag, mu.v.Eta());
        fill("muPhi", "third_" + tag, mu.v.Phi());
    }
    if (muons.size() > 3) {
        const lepton &mu = muons[3];
        fill("muPt", "fourth_" + tag, mu.v.Pt());
        fill("muEta", "fourth_" + tag, mu.v.Eta());
        fill("muPhi", "fourth_" + tag, mu.v.Phi());
    }
}

void muons_analyzer::write() { histo_set::write(); }
}
