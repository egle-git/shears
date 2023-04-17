#include "genleps.h"

#include <boost/math/constants/constants.hpp>

#include "logging.h"
//#include "RoccoR.h"

namespace physics
{

genleps::genleps(util::job::info &info, const util::options &opt, util::histo_set &h)
    : GenDressedLepton_pt(info.init_optional_branch<decltype(GenDressedLepton_pt)>("GenDressedLepton_pt")),
      GenDressedLepton_eta(info.init_optional_branch<decltype(GenDressedLepton_eta)>("GenDressedLepton_eta")),
      GenDressedLepton_phi(info.init_optional_branch<decltype(GenDressedLepton_phi)>("GenDressedLepton_phi")),
      GenDressedLepton_mass(info.init_optional_branch<decltype(GenDressedLepton_mass)>("GenDressedLepton_mass")),
      GenDressedLepton_pdgId(info.init_optional_branch<decltype(GenDressedLepton_pdgId)>("GenDressedLepton_pdgId")),
      GenDressedLepton_hasTauAnc(info.init_optional_branch<decltype(GenDressedLepton_hasTauAnc)>("GenDressedLepton_hasTauAnc")),
      GenPart_status(info.init_optional_branch<decltype(GenPart_status)>("GenPart_status")),
      GenPart_statusFlags(info.init_optional_branch<decltype(GenPart_statusFlags)>("GenPart_statusFlags")),
      GenPart_pt(info.init_optional_branch<decltype(GenPart_pt)>("GenPart_pt")),
      GenPart_eta(info.init_optional_branch<decltype(GenPart_eta)>("GenPart_eta")),
      GenPart_phi(info.init_optional_branch<decltype(GenPart_phi)>("GenPart_phi")),
      GenPart_mass(info.init_optional_branch<decltype(GenPart_mass)>("GenPart_mass")),
      GenPart_pdgId(info.init_optional_branch<decltype(GenPart_pdgId)>("GenPart_pdgId")),
      LHEPart_pt(info.init_optional_branch<decltype(LHEPart_pt)>("LHEPart_pt")),
      LHEPart_eta(info.init_optional_branch<decltype(LHEPart_eta)>("LHEPart_eta")),
      LHEPart_phi(info.init_optional_branch<decltype(LHEPart_phi)>("LHEPart_phi")),
      LHEPart_mass(info.init_optional_branch<decltype(LHEPart_mass)>("LHEPart_mass")),
      LHEPart_pdgId(info.init_optional_branch<decltype(LHEPart_pdgId)>("LHEPart_pdgId"))
{
    configure(opt);

    const double pi = boost::math::constants::pi<double>();

    h.declare("genLepPt", "Gen lepton pt;Gen lepton p_{T} [GeV]", 50, 0, 200);
    h.declare("genLepEta", "Gen lepton eta;Gen lepton #eta", 24, -2.4, 2.4);
    h.declare("genLepPhi", "Gen lepton phi;Gen lepton #phi", 24, -pi, pi);
}

void genleps::configure(const util::options &opt)
{
    const YAML::Node node = opt.config["generator level"];
    util::set_value_safe(node, _pt_cut, "lepton pt", "gen lepton pt cut", [](double val) { return val >= 0; });
    util::set_value_safe(node, _eta_cut, "lepton eta", "gen lepton eta cut", [](double val) { return val > 0; });    
}

std::vector<lepton> genleps::get()
{
//    std::cout<<"________"<<std::endl;
    std::vector<lepton> genleps;

    for (unsigned i = 0; i < GenDressedLepton_pt->GetSize(); ++i) {
        lepton l;

        if (std::abs(GenDressedLepton_eta->At(i)) > _eta_cut || GenDressedLepton_hasTauAnc->At(i) ) {
            continue;
        }

        l.v.SetPtEtaPhiM(GenDressedLepton_pt->At(i), GenDressedLepton_eta->At(i), GenDressedLepton_phi->At(i), GenDressedLepton_mass->At(i));
        l.raw_v = l.v;
        l.charge = (-1)*GenDressedLepton_pdgId->At(i)/std::abs(GenDressedLepton_pdgId->At(i)); // true for leptons; may not for the other particles
        l.pdgid = GenDressedLepton_pdgId->At(i);

        if (l.v.Pt() < _pt_cut) {
           continue;
        }
        genleps.push_back(l);
    }
    std::sort(genleps.begin(), genleps.end(), [](const lepton &lhs, const lepton &rhs)
        {
            return lhs.v.Pt() > rhs.v.Pt();
        }
    );

    return genleps;
}

// return true if the event has "fromHardProcessFinalSate" two leptons with the given flavor
// useful for DY->ll sample to separate DY->ee, DY->mm and DY->tautau
bool genleps::IsGivenFlavorDileptonEvent(int pdgID) {
  int countLep_fromHardProcessFinalState = 0;

  for(unsigned i_par=0; i_par < GenPart_pdgId->GetSize(); ++i_par) {
    int i_pdgID = GenPart_pdgId->At(i_par);

    if( abs(i_pdgID) == pdgID ) {
      if( GenPart_statusFlags->At(i_par) & 256 && // -- fromHardProcess
          GenPart_status->At(i_par) == 1 ) // -- final state
        countLep_fromHardProcessFinalState++;
    }
  }

  return (countLep_fromHardProcessFinalState == 2);
}

std::vector<lepton> genleps::get_leptons_isLHE() {
    std::vector<lepton> genleps_isLHE;

    for(unsigned i=0; i < LHEPart_pt->GetSize(); ++i) {
        lepton l;
        int pdgID = LHEPart_pdgId->At(i);
        // (electron or muon)
        if(std::abs(pdgID) == 11 || std::abs(pdgID) == 13){
            l.v.SetPtEtaPhiM(LHEPart_pt->At(i), LHEPart_eta->At(i), LHEPart_phi->At(i), LHEPart_mass->At(i));
            l.raw_v = l.v;
            l.charge = (-1)*pdgID/std::abs(pdgID);
            l.pdgid = pdgID;

            genleps_isLHE.push_back(l);
        }
    }

    std::sort(genleps_isLHE.begin(), genleps_isLHE.end(),
            [](const lepton &lhs, const lepton &rhs) { return lhs.v.Pt() > rhs.v.Pt(); } );

    return genleps_isLHE;

}

// used for Rochester correction to find matched gen-muon in the final state (i.e. stable) to a given reco-muon. Dressed lepton should not be used here.
// it should be called in MC case: it will make seg. fault when it is called with data (at GenPart_pt->GetSize())
std::vector<lepton> genleps::get_leptons_finalState() {
  std::vector<lepton> genleps_finalState;

  for(unsigned i=0; i<GenPart_pt->GetSize(); ++i) {
    lepton l;

    int pdgID = GenPart_pdgId->At(i);

    // stable & (electron or muon)
    if( GenPart_status->At(i) == 1 && (std::abs(pdgID) == 11 || std::abs(pdgID) == 13) ) {
      l.v.SetPtEtaPhiM(GenPart_pt->At(i), GenPart_eta->At(i), GenPart_phi->At(i), GenPart_mass->At(i));
      l.raw_v = l.v;
      l.charge = (-1)*pdgID/std::abs(pdgID);
      l.pdgid = pdgID;

      genleps_finalState.push_back(l);
    }
  }

  std::sort(genleps_finalState.begin(), genleps_finalState.end(),
            [](const lepton &lhs, const lepton &rhs) { return lhs.v.Pt() > rhs.v.Pt(); }
  );

  return genleps_finalState;

}


void genleps::fill(util::histo_set &h,
                 const std::string &tag,
                 const std::vector<lepton> &genleps,
                 const weights &w)
{
    for (const lepton &mu : genleps) {
        h.fill("genLepPt", tag, mu.v.Pt(), w.gen_weight());
        h.fill("genLepEta", tag, mu.v.Eta(), w.gen_weight());
        h.fill("genLepPhi", tag, mu.v.Phi(), w.gen_weight());
    }
    if (genleps.size() > 0) {
        const lepton &mu = genleps[0];
        h.fill("genLepPt", "leading_" + tag, mu.v.Pt(), w.gen_weight());
        h.fill("genLepEta", "leading_" + tag, mu.v.Eta(), w.gen_weight());
        h.fill("genLepPhi", "leading_" + tag, mu.v.Phi(), w.gen_weight());
    }
    if (genleps.size() > 1) {
        const lepton &mu = genleps[1];
        h.fill("genLepPt", "subleading_" + tag, mu.v.Pt(), w.gen_weight());
        h.fill("genLepEta", "subleading_" + tag, mu.v.Eta(), w.gen_weight());
        h.fill("genLepPhi", "subleading_" + tag, mu.v.Phi(), w.gen_weight());
    }
    if (genleps.size() > 2) {
        const lepton &mu = genleps[2];
        h.fill("genLepPt", "third_" + tag, mu.v.Pt(), w.gen_weight());
        h.fill("genLepEta", "third_" + tag, mu.v.Eta(), w.gen_weight());
        h.fill("genLepPhi", "third_" + tag, mu.v.Phi(), w.gen_weight());
    }
    if (genleps.size() > 3) {
        const lepton &mu = genleps[3];
        h.fill("genLepPt", "fourth_" + tag, mu.v.Pt(), w.gen_weight());
        h.fill("genLepEta", "fourth_" + tag, mu.v.Eta(), w.gen_weight());
        h.fill("genLepPhi", "fourth_" + tag, mu.v.Phi(), w.gen_weight());
    }
}
} // namespace physics
