#include "electrons.h"

#include <boost/math/constants/constants.hpp>

#include "logging.h"
#include "RoccoR.h"

namespace physics
{

electrons::electrons(util::job::info &info, const util::options &opt, util::histo_set &h)
    : ElPt(info.reader, "ElPt"),
      ElEta(info.reader, "ElEta"),
      ElPhi(info.reader, "ElPhi"),
      ElE(info.reader, "ElE"),
      ElCh(info.reader, "ElCh"),
      ElEtaSc(info.reader, "ElEtaSc"),
      ElPfIsoRho(info.reader, "ElPfIsoRho"),
      ElId(info.reader, "ElId")
{
    configure(opt);

    const double pi = boost::math::constants::pi<double>();

    h.declare("elPt", "Electron pt;Electron p_{T} [GeV]", 40, 0, 200);
    h.declare("elEta", "Electron eta;Electron #eta", 24, -2.4, 2.4);
    h.declare("elPhi", "Electron phi;Electron #phi", 24, -pi, pi);
}

void electrons::configure(const util::options &opt)
{
    const YAML::Node node = opt.config["electrons"];
    util::set_value_safe(node, _pt_cut, "pt", "electron pt cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _eta_cut, "eta", "electron eta cut", [](double val) { return val > 0; });
    util::set_value_safe(
        node, _iso_cut, "isolation", "electron isolation cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _id_sf_enabled, "use id scale factors", "id scale factors toggle");
    util::set_value_safe(
        node, _reco_sf_enabled, "use reconstruction scale factors", "reconstruction scale factors toggle");
}

std::vector<lepton> electrons::get()
{
    std::vector<lepton> electrons;
    for (unsigned i = 0; i < ElPt.GetSize(); ++i) {
        lepton l;
        if (std::abs(ElEtaSc[i]) > _eta_cut || ElPfIsoRho[i] > _iso_cut) {
            continue;
        } else if (std::abs(ElEtaSc[i]) > 1.4442 && std::abs(ElEtaSc[i]) < 1.566) {
            // Veto endcap-barrel transition
            continue;
        }
        l.v.SetPtEtaPhiE(ElPt[i], ElEta[i], ElPhi[i], ElE[i]);
        l.raw_v = l.v;
        l.charge = ElCh[i];
        l.iso = ElPfIsoRho[i];
        l.id = ElId[i];
        l.pdgid = 11;
        if (!(l.id & 2)) { // Loose ID
            continue;
        }
        if (l.v.Pt() < _pt_cut) {
            continue;
        }
        electrons.push_back(l);
    }
    std::sort(electrons.begin(), electrons.end(), [](const lepton &lhs, const lepton &rhs)
        {
            return lhs.v.Pt() > rhs.v.Pt();
        }
    );
    return electrons;
}

void electrons::apply_sf(weights &w,
                         const std::vector<lepton> &electrons,
                         const util::tables &tab) const
{
    if (w.ismc()) {
        for (const lepton &el : electrons) {
            if (_reco_sf_enabled) {
                w.use_weight(
                    tab.at("electron reco").getEfficiency(el.raw_v.Pt(), el.raw_v.Eta()));
            }
            if (_id_sf_enabled) {
                w.use_weight(tab.at("electron id")
                                .getEfficiency(el.raw_v.Pt(), el.raw_v.Eta()));
            }
        }
    }
}

void electrons::fill(util::histo_set &h,
                     const std::string &tag,
                     const std::vector<lepton> &electrons,
                     const weights &w)
{
    for (const lepton &el : electrons) {
        h.fill("elPt", tag, el.v.Pt(), w.global_weight());
        h.fill("elEta", tag, el.v.Eta(), w.global_weight());
        h.fill("elPhi", tag, el.v.Phi(), w.global_weight());
    }
    if (electrons.size() > 0) {
        const lepton &el = electrons[0];
        h.fill("elPt", "leading_" + tag, el.v.Pt(), w.global_weight());
        h.fill("elEta", "leading_" + tag, el.v.Eta(), w.global_weight());
        h.fill("elPhi", "leading_" + tag, el.v.Phi(), w.global_weight());
    }
    if (electrons.size() > 1) {
        const lepton &el = electrons[1];
        h.fill("elPt", "subleading_" + tag, el.v.Pt(), w.global_weight());
        h.fill("elEta", "subleading_" + tag, el.v.Eta(), w.global_weight());
        h.fill("elPhi", "subleading_" + tag, el.v.Phi(), w.global_weight());
    }
    if (electrons.size() > 2) {
        const lepton &el = electrons[2];
        h.fill("elPt", "third_" + tag, el.v.Pt(), w.global_weight());
        h.fill("elEta", "third_" + tag, el.v.Eta(), w.global_weight());
        h.fill("elPhi", "third_" + tag, el.v.Phi(), w.global_weight());
    }
    if (electrons.size() > 3) {
        const lepton &el = electrons[3];
        h.fill("elPt", "fourth_" + tag, el.v.Pt(), w.global_weight());
        h.fill("elEta", "fourth_" + tag, el.v.Eta(), w.global_weight());
        h.fill("elPhi", "fourth_" + tag, el.v.Phi(), w.global_weight());
    }
}
} // namespace physics
