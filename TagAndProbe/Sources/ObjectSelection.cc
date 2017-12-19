#include <bitset> 
#include "../Includes/ObjectSelection.h"




namespace objectSelection
{
  
  bool TriggerMatchResult (ULong64_t MuHltMatch){
    bool MatchResult;
    bool IsoMu24 = MuHltMatch &(1<<2);
    bool IsotkMu24 = MuHltMatch &(1<<3);
    MatchResult = IsoMu24 || IsotkMu24;
    return MatchResult;
  }
  bool selectElectrons(std::vector<Electron> & tagElectrons, std::vector<Electron> & probeElectrons,std::vector<float> *ElCh, std::vector<float> *ElPt, std::vector<float> *ElEta, std::vector<float> *ElPhi, std::vector<float> *ElE, std::vector<unsigned int> *ElId, std::vector<float> *ElEtaSc, std::vector<float> *ElPfIsoRho)
  {
    for(int i = 0 ; i<ElPt->size() ; i++){
      bool passEta = false, passIso = false, passId = false, passPt = false;
      Electron currentLepton; 
      currentLepton.lvector.SetPtEtaPhiE(ElPt->at(i),ElEta->at(i),ElPhi->at(i),ElE->at(i));
      currentLepton.Pt = ElPt->at(i);
      currentLepton.Phi = ElPhi->at(i);
      currentLepton.Eta = ElEta->at(i);
      currentLepton.PfIsoRho = ElPfIsoRho->at(i);
      currentLepton.EtaSc = ElEtaSc->at(i);
      currentLepton.Ch = ElCh->at(i);
      currentLepton.Seq = i;

      passId = (ElId->at(i) & (1<<17))?1:0;//need to check this later
      currentLepton.Id = passId;

      int eta = fabs(ElEtaSc->at(i));//I took the supercluster eta since it's really the geometry which is taken here.
      passEta = (eta<=2.5 && (eta>=1.5660 || eta<=1.4442));
      currentLepton.E = fabs(ElEta->at(i));
      if(eta>=1.5660 && ElPfIsoRho->at(i)<0.0646) passIso = true;
      if(eta<=1.4442 && ElPfIsoRho->at(i)<0.0354) passIso = true; //Numbers are taken from llvv_fwk and have not been checked.
      passPt = (currentLepton.lvector.Pt() >=10);
      if(passIso && passId ) tagElectrons.push_back(currentLepton);
      if(passEta && passPt) probeElectrons.push_back(currentLepton);
    }
    return true;
  }

  bool selectMuons(std::vector<Muon> & tagMuons, std::vector<Muon> & probeMuons,std::vector<float> *MuCh, std::vector<float> *MuPt, std::vector<float> *MuEta, std::vector<float> *MuPhi, std::vector<float> *MuE, std::vector<unsigned int> *MuId, std::vector<unsigned int> *MuIdTight, std::vector<float> *MuPfIso,std::vector<ULong64_t> *MuHltMatch)
  {
    for(int i = 0 ; i<MuPt->size() ; i++){
      bool passEta = false, passIso = false, passId = false, passPt = false ;
      Muon currentLepton; 
      currentLepton.lvector.SetPtEtaPhiE(MuPt->at(i),MuEta->at(i),MuPhi->at(i),MuE->at(i));
      
      currentLepton.Pt = MuPt->at(i);
      currentLepton.Phi = MuPhi->at(i);
      currentLepton.Eta = MuEta->at(i);
      currentLepton.E = MuE->at(i);
      currentLepton.PfIso = MuPfIso->at(i);
      currentLepton.Id = MuId->at(i);
      currentLepton.IdTight = MuIdTight->at(i)& (1<<0);
      //std::cout<< "The hltmatch number is "<<std::bitset<sizeof(int)*8>(MuHltMatch->at(i))<<std::endl;
      currentLepton.Ch = MuCh->at(i);
      currentLepton.Seq = i;
      currentLepton.HltMatch = TriggerMatchResult(MuHltMatch->at(i));
      for(int k=0;k<sizeof(currentLepton.TriggerInf) / sizeof(currentLepton.TriggerInf[0]);k++){currentLepton.TriggerInf[k]=((MuHltMatch->at(i))>>k)&1;}
      passId = MuIdTight->at(i) & (1<<0); //Look at the first vertex, hence the bit 0.
      float eta = fabs(MuEta->at(i));
      currentLepton.E = eta;
      passEta = (eta<=2.4);
      passIso = (MuPfIso->at(i)<0.15); //Numbers are taken from llvv_fwk and have not been checked.
      passPt = (currentLepton.lvector.Pt() >=10);
         if(passIso && passId && currentLepton.HltMatch) tagMuons.push_back(currentLepton);
         if(passEta && passPt) probeMuons.push_back(currentLepton);
    }
    return true;
  }

}
