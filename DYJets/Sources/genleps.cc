#include "genleps.h"

#include <boost/math/constants/constants.hpp>

#include "logging.h"
//#include "RoccoR.h"

namespace physics
{

genleps::genleps(util::job::info &info, const util::options &opt, util::histo_set &h)
    : GenLepPt(info.reader, "GLepDr01Pt"),
      GenLepEta(info.reader, "GLepDr01Eta"),
      GenLepPhi(info.reader, "GLepDr01Phi"),
      GenLepE(info.reader, "GLepDr01E"),
      GenLepId(info.reader, "GLepDr01Id"),
      GenLepPrompt(info.reader,"GLepDr01Prompt_"),
      GenLepTauProd(info.reader,"GLepDr01TauProd_")
{
    configure(opt);

    const double pi = boost::math::constants::pi<double>();

    h.declare("genLepPt", "Muon pt;Muon p_{T} [GeV]", 40, 0, 200);
    h.declare("genLepEta", "Muon eta;Muon #eta", 24, -2.4, 2.4);
    h.declare("genLepPhi", "Muon phi;Muon #phi", 24, -pi, pi);
}

void genleps::configure(const util::options &opt)
{
    const YAML::Node node = opt.config["muons"];
    util::set_value_safe(node, _pt_cut, "pt", "muon pt cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _eta_cut, "eta", "muon eta cut", [](double val) { return val > 0; });
}

std::vector<lepton> genleps::get()
{

    std::vector<lepton> genleps;
    for (unsigned i = 0; i < GenLepPt.GetSize(); ++i) {
        lepton l;
        if (std::abs(GenLepEta[i]) > _eta_cut || !GenLepPrompt[i]|| GenLepTauProd[i]) {
            continue;
        }
        l.v.SetPtEtaPhiE(GenLepPt[i], GenLepEta[i], GenLepPhi[i], GenLepE[i]);
        l.raw_v = l.v;
        l.charge = GenLepId[i]/std::abs(GenLepId[i]);
        l.pdgid = GenLepId[i];

        if (l.v.Pt() < _pt_cut) {
            continue;
        }
        genleps.push_back(l);
    }
    return genleps;
}
void genleps::fill(util::histo_set &h,
                 const std::string &tag,
                 const std::vector<lepton> &genleps,
                 const weights &w)
{
    for (const lepton &mu : genleps) {
        h.fill("genLepPt", tag, mu.v.Pt(), w.global_weight());
        h.fill("genLepEta", tag, mu.v.Eta(), w.global_weight());
        h.fill("genLepPhi", tag, mu.v.Phi(), w.global_weight());
    }
    if (genleps.size() > 0) {
        const lepton &mu = genleps[0];
        h.fill("genLepPt", "leading_" + tag, mu.v.Pt(), w.global_weight());
        h.fill("genLepEta", "leading_" + tag, mu.v.Eta(), w.global_weight());
        h.fill("genLepPhi", "leading_" + tag, mu.v.Phi(), w.global_weight());
    }
    if (genleps.size() > 1) {
        const lepton &mu = genleps[1];
        h.fill("genLepPt", "subleading_" + tag, mu.v.Pt(), w.global_weight());
        h.fill("genLepEta", "subleading_" + tag, mu.v.Eta(), w.global_weight());
        h.fill("genLepPhi", "subleading_" + tag, mu.v.Phi(), w.global_weight());
    }
    if (genleps.size() > 2) {
        const lepton &mu = genleps[2];
        h.fill("genLepPt", "third_" + tag, mu.v.Pt(), w.global_weight());
        h.fill("genLepEta", "third_" + tag, mu.v.Eta(), w.global_weight());
        h.fill("genLepPhi", "third_" + tag, mu.v.Phi(), w.global_weight());
    }
    if (genleps.size() > 3) {
        const lepton &mu = genleps[3];
        h.fill("genLepPt", "fourth_" + tag, mu.v.Pt(), w.global_weight());
        h.fill("genLepEta", "fourth_" + tag, mu.v.Eta(), w.global_weight());
        h.fill("genLepPhi", "fourth_" + tag, mu.v.Phi(), w.global_weight());
    }
}
} // namespace physics
