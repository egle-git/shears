#ifndef objectselection_h
#define objectselection_h

#include <iostream>
#include <string>
#include <TMath.h>
#include <vector>
#include "TLorentzVector.h"


struct Electron
  {

    float Pt,Eta,Phi,E;
    float EtaSc, PfIsoRho;
    int Id;
    int Seq;// avoiding a pair containing 2 same lep
	 TLorentzVector lvector;     
  };

  struct Muon
  {

    float Pt,Eta,Phi,E;
    float PfIso;
    int Id, IdTight;
    int Seq;// avoiding a pair containing 2 same lep
	 TLorentzVector lvector;
  };
struct Muonforfill
  {
	float Zmass,Pt_tag,Pt_probe,Eta_tag,Eta_probe,Phi_tag,Phi_probe,E_tag,E_probe;
	float PfIso_probe;
  	int Id_tag,Id_probe,IdTight_tag,IdTight_probe;
  };

namespace objectSelection
{

  bool selectElectrons(std::vector<Electron> & tagElectrons, std::vector<Electron> & probeElectrons, std::vector<float> *ElPt, std::vector<float> *ElEta, std::vector<float> *ElPhi, std::vector<float> *ElE, std::vector<unsigned int> *ElId, std::vector<float> *ElEtaSc, std::vector<float> *ElPfIsoRho);

  bool selectMuons(std::vector<Muon> & tagMuons, std::vector<Muon> & probeMuons, std::vector<float> *MuPt, std::vector<float> *MuEta, std::vector<float> *MuPhi, std::vector<float> *MuE, std::vector<unsigned int> *MuId, std::vector<unsigned int> *MuIdTight, std::vector<float> *MuPfIso);


}

#endif
