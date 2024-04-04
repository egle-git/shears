#include "jets.h"
#include <algorithm>

#include <boost/math/constants/constants.hpp>

#include "functions.h"
#include "boson_jets_analyzer.h"

namespace physics
{

jets::jets(util::job::info &info, const util::options &opt)
    : Jet_pt(info.reader, "Jet_pt"),
      Jet_eta(info.reader, "Jet_eta"),
      Jet_phi(info.reader, "Jet_phi"),
      Jet_mass(info.reader, "Jet_mass"),
      Jet_jetId(info.reader, "Jet_jetId"),
      Jet_puId(info.reader, "Jet_puId"),//Max:check
      Jet_puIdDisc(info.reader, "Jet_puIdDisc"),//Iti:check
      Jet_btagCSVV2(info.reader, "Jet_btagCSVV2"),
      Jet_btagDeepB(info.reader, "Jet_btagDeepB"),
      Jet_btagDeepFlavB(info.reader, "Jet_btagDeepFlavB"),
      Jet_hadronFlavour(info.init_optional_branch<decltype(Jet_hadronFlavour)>("Jet_hadronFlavour")),//Max:check
      //EvtFastJetRho(info.reader, "EvtFastJetRho"),
      fixedGridRhoFastjetAll(info.reader, "fixedGridRhoFastjetAll"), //Iti:check
      GenJet_pt(info.init_optional_branch<decltype(GenJet_pt)>("GenJet_pt")),
      GenJet_eta(info.init_optional_branch<decltype(GenJet_eta)>("GenJet_eta")),
      GenJet_phi(info.init_optional_branch<decltype(GenJet_eta)>("GenJet_phi")),
      GenJet_mass(info.init_optional_branch<decltype(GenJet_eta)>("GenJet_mass"))
{
    configure(opt);
}

void jets::configure(const util::options &opt)
{

    const YAML::Node node = opt.config["jets"];
    util::set_value_safe(node, _pt_cut, "pt", "jet pt cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _eta_cut, "eta", "jet pseudorapidity cut", [](double val) { return val > 0; });
    util::set_value_safe(node, _pumva_cut, "pu mva", "jet PU MVA cut", [](double val) {
        return val >= -1 && val < 1;
    });
    util::set_value_safe(
        node, _deltar_cut, "lepton delta r", "jet-lepton Delta R cut", [](double val) {
            return val > 0;
        });
    std::string pileup_type = opt.config["pileup type"].as<std::string>();
    if( pileup_type == "2016_PreAPV" )       _era = 0;
    else if( pileup_type == "2016_PostAPV" ) _era = 1;
    else if( pileup_type == "2017" )         _era = 2;
    else if( pileup_type == "2018" )         _era = 3;

    // PU ID not needed if ptcut = 50 GeV
    //if( node["pu id"] ) {
    //    std::string pu_id = node["pu id"].as<std::string>();
    //    //std::string _year = node["pileup type"].as<std::string>();
    //    if(_era <= 1) { // 2016preAPV or postAPV
    //        if( pu_id == "loose" )       _pu_id_cut = 1;
    //        else if( pu_id == "medium" ) _pu_id_cut = 3;
    //        else if( pu_id == "tight" )  _pu_id_cut = 7;
    //        else                      throw std::invalid_argument("Unknown jet pu id: \"" + pu_id + "\"");
    //    }
    //    else { //2017 or 2018
    //        if( pu_id == "loose" )       _pu_id_cut = 4;
    //        else if( pu_id == "medium" ) _pu_id_cut = 6;
    //        else if( pu_id == "tight" )  _pu_id_cut = 7;
    //        else                      throw std::invalid_argument("Unknown jet pu id: \"" + pu_id + "\"");
    //    }
    //}
    util::set_value_safe(node, _jer_smearing, "JER smearing", "JER smearing toggle");
    if( _jer_smearing ) {
        if( node["Jet pt resolution"] ) {
            JER_pt_dir = node["Jet pt resolution"].as<std::string>();
            JER_pt_dir = "EfficiencyTables/JER/" + JER_pt_dir;
            m_JetResolution =
                new JME::JetResolution(JER_pt_dir);
        }
        if( node["Jet MC SF"] ) {
            JER_SF_dir = node["Jet MC SF"].as<std::string>();
            JER_SF_dir = "EfficiencyTables/JER/" + JER_SF_dir;
            m_JetResolutionScaleFactor =
                new JME::JetResolutionScaleFactor(JER_SF_dir);
        }
        m_JetParameters = new JME::JetParameters();
    }

}

void jets::declare_histograms(util::histo_set &h)
{
    const double pi = boost::math::constants::pi<double>();

    h.declare("nJets", "Jet multiplicity (excl.)", 7, -0.5, 6.5);
    h.declare("nJetsIncl", "Jet multiplicity (incl.)", 7, -0.5, 6.5);
    //h.declare("jetPuMva", "Jet PU variable from MVA", 40, -1, 1);
    h.declare("jetbdisc", "Jet bdisc variable ", 50, 0, 1);
    h.declare("jetPt", "Jet pt", 40, 0, 200);
    h.declare("jetPtforbveto_multiple", "Jet pt per eta region", 36, -0.5, 35.5); // (7 bins + UF + OF) * 4 eta regions
    h.declare("jetEta", "Jet eta", 25, -2.5, 2.5);
    h.declare("jetPhi", "Jet phi", 24, -pi, pi);
    h.declare("jetDr_gen", "Jet DeltaR", 24, -pi, pi);
    h.declare("jetHadFlav", "Jet HadFlav", 6, -0.5, 5.5);
}

std::vector<jet> jets::getGen()
{
    return getGen(_pt_cut, _eta_cut);
}

std::vector<jet> jets::getGen(double ptmin, double etamax)
{
    std::vector<jet> Gjets;
    for (unsigned i = 0; i < GenJet_pt->GetSize(); ++i) {
        jet j;
        if (GenJet_pt->At(i) < ptmin) {
            continue;
        }
        j.v.SetPtEtaPhiM(GenJet_pt->At(i), GenJet_eta->At(i), GenJet_phi->At(i), GenJet_mass->At(i));
        if (std::abs(j.v.Eta()) > etamax) {
            continue;
        }
        Gjets.push_back(j);
    }
    return Gjets;
}

std::vector<jet> jets::get(bool isdata, const std::vector<lepton> &leptons, double ptcut)
{
    if (ptcut < 0) {
        ptcut = _pt_cut;
    }

    std::vector<jet> jets;
    for (unsigned i = 0; i < Jet_pt.GetSize(); ++i) {
        jet j;
        //if (Jet_puIdDisc[i] < _pumva_cut || Jet_jetId[i] < 6) { // Tight ID + LepVeto + PU ID
        if (Jet_jetId[i] < 6) { // Tight ID + LepVeto
            continue;
        }
        j.v.SetPtEtaPhiM(Jet_pt[i], Jet_eta[i], Jet_phi[i], Jet_mass[i]);

        double deltarjjmin =0.2;
        bool matched =false;
        if (!isdata && _jer_smearing) {
            m_JetParameters->setJetPt(j.v.Pt());
            m_JetParameters->setJetEta(j.v.Eta());
            m_JetParameters->setRho(*fixedGridRhoFastjetAll);
            jetResolution = m_JetResolution->getResolution(*m_JetParameters);
            jetSF = m_JetResolutionScaleFactor->getScaleFactor(*m_JetParameters, m_Variation);
            float smearFactor = 1.0;
            std::vector<jet> gjet = getGen(10 , 5);
            veto(gjet, leptons);
            for (const auto &gj: gjet) {
                float deltarjj = gj.v.DeltaR(j.v);
                float dPt = abs(j.v.Pt() - gj.v.Pt());
                if (deltarjj < deltarjjmin && dPt < (3 * j.v.Pt() * jetResolution)) {
                    deltarjjmin = deltarjj;
                    smearFactor = 1.0 + (jetSF - 1.0) * (j.v.Pt() - gj.v.Pt()) / j.v.Pt();
                    matched = true;
                }
           }
           if (!matched) {
                TRandom3 *random = new TRandom3(0);
                smearFactor =
                    1.0 +
                    random->Gaus(0.0, jetResolution) * sqrt(std::max(pow(jetSF, 2) - 1.0, 0.0));
                delete random;
            }
            float oldJetPt = j.v.Pt();
            float newJetPt = oldJetPt * smearFactor;
            j.v.SetPtEtaPhiM(newJetPt, j.v.Eta(), j.v.Phi(), j.v.M() * newJetPt / oldJetPt);
        }

        if (std::abs(j.v.Eta()) > _eta_cut) {
            continue;
        }
        j.raw_v = j.v;
        j.id = Jet_jetId[i];
        //j.puMva = Jet_puIdDisc[i];
        //j.bdisc = Jet_btagDeepB[i]; //new DeepCSV
        j.bdisc = Jet_btagDeepFlavB[i]; //DeepJet (recommended)

        // Check if the optional contains a value before accessing it
        if (!isdata && Jet_hadronFlavour.has_value()) {
            // Access the value and then apply operator[]
            auto& _hadflav = *Jet_hadronFlavour;  // Dereference the optional to access the value
            j.hadflav = _hadflav[i];  // Accessing element at index i
        }
        if (j.v.Pt() < ptcut) continue;

        jets.push_back(j);
    }
        std::sort(jets.begin(), jets.end(), [](const jet &lhs, const jet &rhs)
            {
                return lhs.v.Pt() > rhs.v.Pt();
            }
);
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
        //h.fill("jetPuMva", tag, j.puMva, w.global_weight());
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
