#include "jets.h"

#include <algorithm>

#include <boost/math/constants/constants.hpp>

#include "functions.h"

namespace physics
{

jets::jets(util::job::info &info, const util::options &opt)
    : JetAk04Pt(info.reader, "JetAk04Pt"),
      JetAk04Eta(info.reader, "JetAk04Eta"),
      JetAk04Phi(info.reader, "JetAk04Phi"),
      JetAk04E(info.reader, "JetAk04E"),
      JetAk04Id(info.reader, "JetAk04Id"),
      JetAk04PuMva(info.reader, "JetAk04PuMva"),
      JetAk04BDiscCisvV2(info.reader,"JetAk04BDiscCisvV2"),
      JetAk04HadFlav(info.reader,"JetAk04HadFlav")
{
    configure(opt);
}

void jets::configure(const util::options &opt)
{
    const YAML::Node node = opt.config["jets"];
    util::set_value_safe(node, _pt_cut, "pt", "jet pt cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _eta_cut, "eta", "jet eta cut", [](double val) { return val > 0; });
    util::set_value_safe(node, _pumva_cut, "pu mva", "jet PU MVA cut", [](double val) {
        return val >= -1 && val < 1;
    });
    util::set_value_safe(
        node, _deltar_cut, "lepton delta r", "jet-lepton Delta R cut", [](double val) {
            return val > 0;
        });
}

void jets::declare_histograms(util::histo_set &h)
{
    const double pi = boost::math::constants::pi<double>();

    h.declare("nJets", "Jet multiplicity (excl.)", 7, -0.5, 6.5);
    h.declare("nJetsIncl", "Jet multiplicity (incl.)", 7, -0.5, 6.5);
    h.declare("jetPuMva", "Jet PU variable from MVA", 40, -1, 1);
    h.declare("jetbdisc", "Jet bdisc variable ", 40, -1, 1);
    h.declare("jetPt", "Jet pt", 40, 0, 200);
    h.declare("jetEta", "Jet eta", 24, -2.4, 2.4);
    h.declare("jetPhi", "Jet phi", 24, -pi, pi);
}

std::vector<jet> jets::get()
{
    std::vector<jet> jets;
    for (unsigned i = 0; i < JetAk04Pt.GetSize(); ++i) {
        jet j;
        if (JetAk04Pt[i] < _pt_cut || std::abs(JetAk04Eta[i]) > _eta_cut ||
            JetAk04PuMva[i] < _pumva_cut || JetAk04Id[i] <= 0) {
            continue;
        }
        j.v.SetPtEtaPhiE(JetAk04Pt[i], JetAk04Eta[i], JetAk04Phi[i], JetAk04E[i]);
        j.id = JetAk04Id[i];
        j.puMva = JetAk04PuMva[i];
        j.bdisc = JetAk04BDiscCisvV2[i];
        j.hadflav = JetAk04HadFlav[i];
        jets.push_back(j);
    }
    return jets;
}

void jets::veto(std::vector<jet> &jets, const std::vector<lepton> &leptons) const
{
    jets.erase(std::remove_if(jets.begin(),
                              jets.end(),
                              [&](const jet &j) {
                                  for (const lepton &l : leptons) {
                                      if (deltaR(j.v, l.v) < _deltar_cut) {
                                          return true;
                                      }
                                  }
                                  return false;
                              }),
               jets.end());
}

void jets::fill(util::histo_set &h,
                const std::string &tag,
                const std::vector<jet> &jets,
                const weights &w)
{
    h.fill("nJets", tag, jets.size(), w.global_weight());
    for (std::size_t njets = 0; njets <= jets.size(); ++njets) {
        h.fill("nJetsIncl", tag, njets, w.global_weight());
    }
    for (const jet &j : jets) {
        h.fill("jetPt", tag, j.v.Pt(), w.global_weight());
        h.fill("jetEta", tag, j.v.Eta(), w.global_weight());
        h.fill("jetPhi", tag, j.v.Phi(), w.global_weight());
        h.fill("jetPuMva", tag, j.puMva, w.global_weight());
        h.fill("jetbdisc",tag, j.bdisc,w.global_weight());
    }
    if (jets.size() > 0) {
        const jet &j = jets[0];
        h.fill("jetPt", "leading_" + tag, j.v.Pt(), w.global_weight());
        h.fill("jetEta", "leading_" + tag, j.v.Eta(), w.global_weight());
        h.fill("jetPhi", "leading_" + tag, j.v.Phi(), w.global_weight());
    }
    if (jets.size() > 1) {
        const jet &j = jets[1];
        h.fill("jetPt", "subleading_" + tag, j.v.Pt(), w.global_weight());
        h.fill("jetEta", "subleading_" + tag, j.v.Eta(), w.global_weight());
        h.fill("jetPhi", "subleading_" + tag, j.v.Phi(), w.global_weight());
    }
    if (jets.size() > 2) {
        const jet &j = jets[2];
        h.fill("jetPt", "third_" + tag, j.v.Pt(), w.global_weight());
        h.fill("jetEta", "third_" + tag, j.v.Eta(), w.global_weight());
        h.fill("jetPhi", "third_" + tag, j.v.Phi(), w.global_weight());
    }
    if (jets.size() > 3) {
        const jet &j = jets[3];
        h.fill("jetPt", "fourth_" + tag, j.v.Pt(), w.global_weight());
        h.fill("jetEta", "fourth_" + tag, j.v.Eta(), w.global_weight());
        h.fill("jetPhi", "fourth_" + tag, j.v.Phi(), w.global_weight());
    }
}
}
