#include "../Includes/ObjectSelection.h"




namespace objectSelection
{
  

  bool selectElectrons(std::vector<Electron> & tagElectrons, std::vector<Electron> & probeElectrons,std::vector<float> *ElCh, std::vector<float> *ElPt, std::vector<float> *ElEta, std::vector<float> *ElPhi, std::vector<float> *ElE, std::vector<unsigned int> *ElId, std::vector<float> *ElEtaSc, std::vector<float> *ElPfIsoRho)
  {
    for(int i = 0 ; i<ElPt->size() ; i++){
      bool passEta = false, passIso = false, passId = false, passPt = false, passLoosePt = false, passLooseId = false;
      Electron currentLepton; 
      currentLepton.lvector.SetPtEtaPhiE(ElPt->at(i),ElEta->at(i),ElPhi->at(i),ElE->at(i));
      currentLepton.Pt = ElPt->at(i);
      currentLepton.Phi = ElPhi->at(i);
      currentLepton.Eta = ElEta->at(i);
      currentLepton.E = ElE->at(i);
      currentLepton.PfIsoRho = ElPfIsoRho->at(i);
      currentLepton.EtaSc = ElEtaSc->at(i);
      currentLepton.Id = ElId->at(i);
      currentLepton.Ch = ElCh->at(i);
      currentLepton.Seq = i;
       //Id //Very temporary!!! Used without much cross-checking.
      passId = ElId->at(i) & (1<<17);
      passLooseId = ElId->at(i) & (1<<16);
      int eta = fabs(ElEtaSc->at(i));//I took the supercluster eta since it's really the geometry which is taken here.
      passEta = (eta<=2.5 && (eta>=1.5660 || eta<=1.4442));
      //passEta = (eta<=2.4);
      //Iso //We use ElPfIsoRho for now, we'll see after if it's mandatory to refine it. Iso is applied only for the "tight" selection, not for the extra lepton veto.
      if(eta>=1.5660 && ElPfIsoRho->at(i)<0.0646) passIso = true;
      if(eta<=1.4442 && ElPfIsoRho->at(i)<0.0354) passIso = true; //Numbers are taken from llvv_fwk and have not been checked.
      passPt = (currentLepton.lvector.Pt() >=25);
      passLoosePt = (currentLepton.lvector.Pt() >=10);
      //No iso criteria for extra leptons.
      if(passIso && passId ) tagElectrons.push_back(currentLepton);
      //if(passEta && passLooseId && passLoosePt && selElectrons.size()==2) extraElectrons.push_back(currentLepton); 
      if(passEta && passLoosePt) probeElectrons.push_back(currentLepton);
    }
    return true;
  }

  bool selectMuons(std::vector<Muon> & tagMuons, std::vector<Muon> & probeMuons,std::vector<float> *MuCh, std::vector<float> *MuPt, std::vector<float> *MuEta, std::vector<float> *MuPhi, std::vector<float> *MuE, std::vector<unsigned int> *MuId, std::vector<unsigned int> *MuIdTight, std::vector<float> *MuPfIso)
  {
    for(int i = 0 ; i<MuPt->size() ; i++){
      bool passEta = false, passIso = false, passId = false, passPt = false, passLoosePt = false, passLooseId = false, passSoftId = false, passSoftPt = false;
      Muon currentLepton; 
      currentLepton.lvector.SetPtEtaPhiE(MuPt->at(i),MuEta->at(i),MuPhi->at(i),MuE->at(i));
      
      currentLepton.Pt = MuPt->at(i);
      currentLepton.Phi = MuPhi->at(i);
      currentLepton.Eta = MuEta->at(i);
      currentLepton.E = MuE->at(i);
      currentLepton.PfIso = MuPfIso->at(i);
      currentLepton.Id = MuId->at(i);
      currentLepton.IdTight = MuIdTight->at(i);
      currentLepton.Ch = MuCh->at(i);
      currentLepton.Seq = i;
      //Id //Very temporary!!! Used without much cross-checking.
      passId = MuIdTight->at(i) & (1<<0); //Look at the first vertex, hence the bit 0.
      passLooseId = MuId->at(i) & (1<<0);
      passSoftId = false; //Don't know yet how to implement this thing. FIXME
      int eta = fabs(MuEta->at(i));
      passEta = (eta<=2.4);
      //Iso //We use MuPfIso for now, we'll see after if it's mandatory to refine it. Iso is applied only for the "tight" selection, not for the extra lepton veto.
      passIso = (MuPfIso->at(i)<0.15); //Numbers are taken from llvv_fwk and have not been checked.
      passPt = (currentLepton.lvector.Pt() >=25);
      passLoosePt = (currentLepton.lvector.Pt() >=10);
      passSoftPt = (currentLepton.lvector.Pt() >=3);
     // if(passEta && passLooseId && passLoosePt && selMuons.size()==2) extraMuons.push_back(currentLepton); //No iso criteria for extra leptons.
     // if(passEta && !(passLooseId && passLoosePt) && passSoftId && passSoftPt && selMuons.size()==2) extraMuons.push_back(currentLepton); //Soft leptons. Need a particular cut?
     // if(passEta && passIso && passId && passPt && selMuons.size()<2) selMuons.push_back(currentLepton);
         if(passIso && passId ) tagMuons.push_back(currentLepton);
         if(passEta && passLoosePt) probeMuons.push_back(currentLepton);
    }
    return true;
  }

}
