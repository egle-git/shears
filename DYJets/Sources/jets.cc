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
      JetAk04BDiscCisvV2(info.reader, "JetAk04BDiscCisvV2"),
      JetAk04HadFlav(info.reader, "JetAk04HadFlav"),
      EvtFastJetRho(info.reader, "EvtFastJetRho"),
      GJetAk04Pt(info.reader, "GJetAk04Pt"),
      GJetAk04Eta(info.reader, "GJetAk04Eta"),
      GJetAk04Phi(info.reader, "GJetAk04Phi"),
      GJetAk04E(info.reader, "GJetAk04E")

{
    configure(opt);
    m_JetResolution =
        new JME::JetResolution("EfficiencyTables/Spring16_25nsV10_MC_PtResolution_AK4PFchs.txt");
    m_JetResolutionScaleFactor =
        new JME::JetResolutionScaleFactor("EfficiencyTables/Spring16_25nsV10_MC_SF_AK4PFchs.txt");
    m_JetParameters = new JME::JetParameters();
}

void jets::configure(const util::options &opt)
{

    const YAML::Node node = opt.config["jets"];
    util::set_value_safe(node, _pt_cut, "pt", "jet pt cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _y_cut, "rapidity", "jet rapidity cut", [](double val) { return val > 0; });
    util::set_value_safe(node, _pumva_cut, "pu mva", "jet PU MVA cut", [](double val) {
        return val >= -1 && val < 1;
    });
    util::set_value_safe(
        node, _deltar_cut, "lepton delta r", "jet-lepton Delta R cut", [](double val) {
            return val > 0;
        });
    util::set_value_safe(node, _jer_smearing, "JER smearing", "JER smearing toggle");
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
    h.declare("jetDr_gen", "Jet DeltaR", 24, -pi, pi);
}

std::vector<jet> jets::getGen()
{
    std::vector<jet> Gjets;
    for (unsigned i = 0; i < GJetAk04Pt.GetSize(); ++i) {
        jet j;
        if (GJetAk04Pt[i] < _pt_cut) {
            continue;
        }
        j.v.SetPtEtaPhiE(GJetAk04Pt[i], GJetAk04Eta[i], GJetAk04Phi[i], GJetAk04E[i]);
        if (std::abs(j.v.Rapidity()) > _y_cut) {
            continue;
        }
        Gjets.push_back(j);
    }
    return Gjets;
}

std::vector<jet> jets::get(bool isdata)
{
    std::vector<jet> jets;
    for (unsigned i = 0; i < JetAk04Pt.GetSize(); ++i) {
        jet j;
        if (JetAk04PuMva[i] < _pumva_cut || JetAk04Id[i] <= 0) {
            continue;
        }
        j.v.SetPtEtaPhiE(JetAk04Pt[i], JetAk04Eta[i], JetAk04Phi[i], JetAk04E[i]);
        if (std::abs(j.v.Rapidity()) > _y_cut) {
            continue;
        }
        j.raw_v = j.v;
        j.id = JetAk04Id[i];
        j.puMva = JetAk04PuMva[i];
        j.bdisc = JetAk04BDiscCisvV2[i];
        j.hadflav = JetAk04HadFlav[i];

        m_JetParameters->setJetPt(j.v.Pt());
        m_JetParameters->setJetEta(j.v.Eta());
        m_JetParameters->setRho(*EvtFastJetRho);
        jetResolution = m_JetResolution->getResolution(*m_JetParameters);
        jetSF = m_JetResolutionScaleFactor->getScaleFactor(*m_JetParameters, m_Variation);

        // Jet energy resolution (JER) smearing
        if (!isdata && _jer_smearing) {
            float smearFactor = 1.0;
            if (GJetAk04Pt.GetSize() != 0) {
                for (unsigned i = 0; i < GJetAk04Pt.GetSize(); ++i) {
                    TLorentzVector jg;
                    jg.SetPtEtaPhiE(GJetAk04Pt[i], GJetAk04Eta[i], GJetAk04Phi[i], GJetAk04E[i]);

                    if (GJetAk04Pt[i] < 15. || std::abs(GJetAk04Eta[i]) > _eta_cut) {
                        float deltarjj = jg.DeltaR(j.v);
                        float dPt = abs(j.v.Pt() - jg.Pt());

                        if (deltarjj < 0.2 && dPt < (3 * j.v.Pt() * jetResolution)) {
                            smearFactor = 1.0 + (jetSF - 1.0) * (j.v.Pt() - jg.Pt()) / j.v.Pt();
                        }
                    }
                }
            }
            if (smearFactor == 1) {
                TRandom3 *random = new TRandom3(0);
                smearFactor =
                    1.0 +
                    random->Gaus(0.0, jetResolution) * sqrt(std::max(pow(jetSF, 2) - 1.0, 0.0));
                delete random;
            }
            float oldJetPt = j.v.Pt();
            float newJetPt = oldJetPt * smearFactor;
            j.v.SetPtEtaPhiE(newJetPt, j.v.Eta(), j.v.Phi(), j.v.E() * newJetPt / oldJetPt);
        }
        if (j.v.Pt() < _pt_cut) continue;

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
        h.fill("jetbdisc", tag, j.bdisc, w.global_weight());
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
