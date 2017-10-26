#include <iomanip>
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TH3.h"
#include "TRandom.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "math.h"
#include <fstream>
#include <string>
#include <iostream>
#include <TStyle.h>
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TChain.h"
#include "TF2.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TPostScript.h"
#include "TLine.h"
#include "TEllipse.h"
#include "TMath.h"
#include "TLatex.h"
#include "TLorentzVector.h"
#include <vector>
#include "standalone_LumiReWeighting.h"
//#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"




using namespace std;
void tree1r()
{
 bool muon=0;
//JetCorrectionUncertainty *jecUnc;
// jecUnc= new JetCorrectionUncertainty("Fall12_V7_DATA_Uncertainty_AK5PFchs.txt");

  TChain myTree("analyzeBasicPat/MuonTree");
  myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_2012ABCD.root");

  TH1::AddDirectory(true);



  Double_t        PU_npT;
  Double_t        PU_npIT;
  double           EvtInfo_NumVtx;
  double          MyWeight;
   double nup;
  Int_t          event;
  double          realdata;
  int          run;
//  int          lumi;
  double HLT_Elec17_Elec8;

  std::vector<double> *Dr01LepPt=0;
  std::vector<double> *Dr01LepEta=0;
  std::vector<double> *Dr01LepPhi=0;
  std::vector<double> *Dr01LepE=0;
  std::vector<double> *Dr01LepM=0;
  std::vector<double> *Dr01LepId=0;
  std::vector<double> *Dr01LepStatus=0;

  std::vector<double>  *Bare01LepPt=0;
  std::vector<double> *Bare01LepEta=0;
  std::vector<double> *Bare01LepPhi=0;
  std::vector<double> *Bare01LepE=0;
  std::vector<double> *Bare01LepM=0;
  std::vector<double> *Bare01LepId=0;
  std::vector<double> *Bare01LepStatus=0;

  std::vector<double> *St01PhotonPt=0;
  std::vector<double> *St01PhotonEta=0;
  std::vector<double> *St01PhotonPhi=0;
  std::vector<double> *St01PhotonE=0;
  std::vector<double> *St01PhotonM=0;
  std::vector<double> *St01PhotonId=0;
  std::vector<double> *St01PhotonMomId=0;
  std::vector<double> *St01PhotonNumberMom=0;
  std::vector<double> *St01PhotonStatus=0;



  std::vector<double> *St03Pt=0;
  std::vector<double> *St03Eta=0;
  std::vector<double> *St03Phi=0;
  std::vector<double> *St03E=0;
  std::vector<double> *St03M=0;
  std::vector<double> *St03Id=0;
  std::vector<double> *St03Status=0;

  std::vector<double> *GjPt=0;
  std::vector<double> *Gjeta=0;
  std::vector<double> *Gjphi=0;
  std::vector<double> *GjE=0;
  std::vector<double> *GjPx=0;
  std::vector<double> *GjPy=0;
  std::vector<double> *GjPz=0;

  vector<double>  *patMuonEn_ =0; 
  vector<double>  *patMuonCharge_ =0;
  vector<double>  *patMuonPt_ =0;
  vector<double>  *patMuonEta_=0;
  vector<double>  *patMuonPhi_ =0;
  vector<double>  *patMuonCombId_ =0;
  vector<double>  *patMuonTrig_ =0;
  vector<double>  *patMuonDetIsoRho_ =0;
  vector<double>  *patMuonPfIsoDbeta_ =0;

  std::vector<double> *patElecTrig_ =0;
  std::vector<double> *patElecCharge_ =0;
  std::vector<double> *patElecEnergy_ =0;
  std::vector<double> *patElecEta_ =0;
  std::vector<double> *patElecScEta_ =0;
  std::vector<double> *patElecPhi_ =0;
//  std::vector<double> *patElecEcalEnergy_ =0;
  std::vector<double> *patElecPt_ =0;
  std::vector<double> *patElecPfIso_ =0;
  std::vector<double> *patElecPfIsodb_ =0;
  std::vector<double> *patElecPfIsoRho_ =0;
  std::vector<double>   *patElecMediumIDOff_ =0;
//  std::vector<double> *patElecMVATrigId_ =0;
//  std::vector<double> *patElecMVANonTrigId_ =0;

  vector<double>  *patJetPfAk05En_ =0;
  vector<double>  *patJetPfAk05Pt_ =0;
  vector<double>  *patJetPfAk05Eta_ =0;
  vector<double>  *patJetPfAk05Phi_ =0;
//  vector<double>  *patJetPfAk05JesUncert_ =0;
  vector<double>  *patJetPfAk05LooseId_ =0;
  vector<double>  *patJetPfAk05Et_ =0;
//  vector<double>  *patJetPfAk05HadEHF_ =0;
//  vector<double>  *patJetPfAk05EmEHF_ =0;
//  vector<double>  *patJetPfAk05RawPt_ =0;

  vector<double>  *patJetPfAk05RawEn_ =0;
//  vector<double>  *patJetPfAk05jetBetaClassic_ =0;
//  vector<double>  *patJetPfAk05jetBeta_ =0;
//  vector<double>  *patJetPfAk05jetBetaStar_ =0;
//  vector<double>  *patJetPfAk05jetBetaStarClassic_ =0;
  vector<double>  *patJetPfAk05jetpuMVA_ =0;

  vector<bool>  *patJetPfAk05jetpukLoose_ =0;
  vector<bool>  *patJetPfAk05jetpukMedium_ =0;
  vector<bool>  *patJetPfAk05jetpukTight_ =0;


  vector<double> *patJetPfAk05chf_=0;
  vector<double> *patJetPfAk05nhf_=0;
  vector<double> *patJetPfAk05cemf_=0;
  vector<double> *patJetPfAk05nemf_=0;
  vector<double> *patJetPfAk05cmult_=0;
  vector<double> *patJetPfAk05nconst_=0;



  myTree.SetBranchAddress("event",&event);


  myTree.SetBranchAddress("MyWeight",&MyWeight);
  myTree.SetBranchAddress("realdata",&realdata);
  myTree.SetBranchAddress("run",&run);
//  myTree.SetBranchAddress("lumi",&lumi);
  myTree.SetBranchAddress("PU_npT", &PU_npT);
  myTree.SetBranchAddress("PU_npIT", &PU_npIT);
  myTree.SetBranchAddress("nup", &nup);
  myTree.SetBranchAddress("EvtInfo_NumVtx", &EvtInfo_NumVtx);

  myTree.SetBranchAddress("patMuonEn_", &patMuonEn_);
  myTree.SetBranchAddress("patMuonCharge_", &patMuonCharge_);
  myTree.SetBranchAddress("patMuonPt_", &patMuonPt_);
  myTree.SetBranchAddress("patMuonEta_", &patMuonEta_);
  myTree.SetBranchAddress("patMuonPhi_", &patMuonPhi_);
  myTree.SetBranchAddress("patMuonCombId_", &patMuonCombId_);
  myTree.SetBranchAddress("patMuonTrig_", &patMuonTrig_);
  myTree.SetBranchAddress("patMuonDetIsoRho_", &patMuonDetIsoRho_);
  myTree.SetBranchAddress("patMuonPfIsoDbeta_", &patMuonPfIsoDbeta_);



  myTree.SetBranchAddress("patElecCharge_",&patElecCharge_);
  myTree.SetBranchAddress("patElecPt_",&patElecPt_);
  myTree.SetBranchAddress("patElecEnergy_",&patElecEnergy_);
//  myTree.SetBranchAddress("patElecEcalEnergy_",&patElecEcalEnergy_);
  myTree.SetBranchAddress("patElecEta_",&patElecEta_);
  myTree.SetBranchAddress("patElecScEta_",&patElecScEta_);
  myTree.SetBranchAddress("patElecPhi_",&patElecPhi_);
  myTree.SetBranchAddress("patElecPfIso_",&patElecPfIso_);
  myTree.SetBranchAddress("patElecPfIsodb_",&patElecPfIsodb_);
  myTree.SetBranchAddress("patElecPfIsoRho_",&patElecPfIsoRho_);
  myTree.SetBranchAddress("patElecMediumIDOff_",&patElecMediumIDOff_);
//  myTree.SetBranchAddress("patElecMVATrigId_",&patElecMVATrigId_);
//  myTree.SetBranchAddress("patElecMVANonTrigId_",&patElecMVANonTrigId_);
  myTree.SetBranchAddress("patElecTrig_",&patElecTrig_);

  myTree.SetBranchAddress("patJetPfAk05RawEn_",&patJetPfAk05RawEn_);
   //myTree.SetBranchAddress("patJetPfAk05HadEHF_",&patJetPfAk05HadEHF_);
   //myTree.SetBranchAddress("patJetPfAk05EmEHF_",&patJetPfAk05EmEHF_);
  myTree.SetBranchAddress("patJetPfAk05chf_",&patJetPfAk05chf_);
  myTree.SetBranchAddress("patJetPfAk05nhf_",&patJetPfAk05nhf_);
  myTree.SetBranchAddress("patJetPfAk05cemf_",&patJetPfAk05cemf_);
  myTree.SetBranchAddress("patJetPfAk05nemf_",&patJetPfAk05nemf_);
  myTree.SetBranchAddress("patJetPfAk05cmult_",&patJetPfAk05cmult_);
  myTree.SetBranchAddress("patJetPfAk05nconst_",&patJetPfAk05nconst_);
  myTree.SetBranchAddress("patJetPfAk05En_", &patJetPfAk05En_);
  myTree.SetBranchAddress("patJetPfAk05Pt_", &patJetPfAk05Pt_);
  myTree.SetBranchAddress("patJetPfAk05Eta_", &patJetPfAk05Eta_);
  myTree.SetBranchAddress("patJetPfAk05Phi_", &patJetPfAk05Phi_);
//  myTree.SetBranchAddress("patJetPfAk05JesUncert_", &patJetPfAk05JesUncert_);
  myTree.SetBranchAddress("patJetPfAk05LooseId_", &patJetPfAk05LooseId_);
  myTree.SetBranchAddress("patJetPfAk05Et_", &patJetPfAk05Et_);
//  myTree.SetBranchAddress("patJetPfAk05HadEHF_", &patJetPfAk05HadEHF_);
//  myTree.SetBranchAddress("patJetPfAk05EmEHF_", &patJetPfAk05EmEHF_);
//  myTree.SetBranchAddress("patJetPfAk05RawPt_", &patJetPfAk05RawPt_);
//  myTree.SetBranchAddress("patJetPfAk05RawEn_", &patJetPfAk05RawEn_);
//  myTree.SetBranchAddress("patJetPfAk05jetBetaClassic_", &patJetPfAk05jetBetaClassic_);
//  myTree.SetBranchAddress("patJetPfAk05jetBeta_", &patJetPfAk05jetBeta_);
//  myTree.SetBranchAddress("patJetPfAk05jetBetaStar_", &patJetPfAk05jetBetaStar_);
//  myTree.SetBranchAddress("patJetPfAk05jetBetaStarClassic_", &patJetPfAk05jetBetaStarClassic_);
  myTree.SetBranchAddress("patJetPfAk05jetpuMVA_", &patJetPfAk05jetpuMVA_);
  myTree.SetBranchAddress("patJetPfAk05jetpukLoose_", &patJetPfAk05jetpukLoose_);
  myTree.SetBranchAddress("patJetPfAk05jetpukMedium_", &patJetPfAk05jetpukMedium_);
  myTree.SetBranchAddress("patJetPfAk05jetpukTight_", &patJetPfAk05jetpukTight_);
  myTree.SetBranchAddress("HLT_Elec17_Elec8",&HLT_Elec17_Elec8);
//Gen jets//
  myTree.SetBranchAddress("GjPt",&GjPt);
  myTree.SetBranchAddress("Gjeta",&Gjeta);
  myTree.SetBranchAddress("Gjphi",&Gjphi);
  myTree.SetBranchAddress("GjE",&GjE);
  myTree.SetBranchAddress("GjPx",&GjPx);
  myTree.SetBranchAddress("GjPy",&GjPy);
  myTree.SetBranchAddress("GjPz",&GjPz); 
  //Dressed Leptons//
  myTree.SetBranchAddress("Dr01LepPt",&Dr01LepPt);
  myTree.SetBranchAddress("Dr01LepEta",&Dr01LepEta);
  myTree.SetBranchAddress("Dr01LepPhi",&Dr01LepPhi);
  myTree.SetBranchAddress("Dr01LepE",&Dr01LepE);
  myTree.SetBranchAddress("Dr01LepM",&Dr01LepM);
  myTree.SetBranchAddress("Dr01LepId",&Dr01LepId);
  myTree.SetBranchAddress("Dr01LepStatus",&Dr01LepStatus);

  myTree.SetBranchAddress("Bare01LepPt",&Bare01LepPt);
  myTree.SetBranchAddress("Bare01LepEt",&Bare01LepEta);
  myTree.SetBranchAddress("Bare01LepPhi",&Bare01LepPhi);
  myTree.SetBranchAddress("Bare01LepE",&Bare01LepE);
  myTree.SetBranchAddress("Bare01LepM",&Bare01LepM);
  myTree.SetBranchAddress("Bare01LepId",&Bare01LepId);
  myTree.SetBranchAddress("Bare01LepStatus",&Bare01LepStatus);

  myTree.SetBranchAddress("St01PhotonPt",&St01PhotonPt);
  myTree.SetBranchAddress("St01PhotonEta",&St01PhotonEta);
  myTree.SetBranchAddress("St01PhotonPhi",&St01PhotonPhi);
  myTree.SetBranchAddress("St01PhotonE",&St01PhotonE);
  myTree.SetBranchAddress("St01PhotonM",&St01PhotonM);
  myTree.SetBranchAddress("St01PhotonId",&St01PhotonId);
  myTree.SetBranchAddress("St01PhotonMomId",&St01PhotonMomId);
  myTree.SetBranchAddress("St01PhotonNumberMom",&St01PhotonNumberMom);
  myTree.SetBranchAddress("St01PhotonStatus",&St01PhotonStatus);


  myTree.SetBranchAddress("St03Pt",&St03Pt);
  myTree.SetBranchAddress("St03Eta",&St03Eta);
  myTree.SetBranchAddress("St03Phi",&St03Phi);
  myTree.SetBranchAddress("St03E",&St03E);
  myTree.SetBranchAddress("St03M",&St03M);
  myTree.SetBranchAddress("St03Id",&St03Id);
  myTree.SetBranchAddress("St03Status",&St03Status);





  ////Histogram booking///////////
 
  TFile *theFile = new TFile ("test.root","RECREATE");
  theFile->cd();
  TH1D* h_MZ = new TH1D("MZ","M(Z)#rightarrow #mu#mu",40, 71,111.);
  TH1D* h_GMZElec = new TH1D("GMZElec","M(Z)#rightarrow #mu#mu",40, 71,111.);



  int puYear(2013);
    cout << "Pile Up Distribution: " << puYear << endl;
    standalone_LumiReWeighting puWeight(puYear), puUp(puYear,1), puDown(puYear,-1);


  Int_t nevent = myTree.GetEntries();
  nevent =100000;
  for (Int_t iev=0;iev<nevent;iev++) {


    if (iev%100000 == 0) cout<<"DATA"<<" ===>>> "<<iev<<"/"<<nevent<<endl;
    myTree.GetEntry(iev);    

    if(HLT_Elec17_Elec8!=1)continue; //require global trigger fired 


  ///////////////////////////////
    double PUweight =1;
    double PUweightUp =1;
    double PUweightDn =1;

    if (PU_npT > 0) PUweight = PUweight * puWeight.weight(int(PU_npT));
    if (PU_npT > 0) PUweightUp = PUweightUp * puUp.weight(int(PU_npT));
    if (PU_npT > 0) PUweightDn = PUweightDn * puDown.weight(int(PU_npT));

 
    int Gcommon = 0;
 
    int Gindex1[10],Gindex2[10];



    for (unsigned int j = 0; j < Dr01LepPt->size(); ++j){  

      for (unsigned int jk = j; jk < Dr01LepPt->size(); ++jk){
       // if(fabs(Dr01LepId->at(j))==11)cout<<Dr01LepId->at(j)<<endl;
        if (j == jk) continue;

        if ( Dr01LepStatus->at(j) == 1 && Dr01LepStatus->at(jk) == 1 &&
	     (Dr01LepId->at(j)*Dr01LepId->at(jk)) == -121 && //common1
	     Dr01LepPt->at(j) > 30. && Dr01LepPt->at(jk) > 30. && //common2
	     fabs(Dr01LepEta->at(j)) <2.4 && fabs(Dr01LepEta->at(jk)) <2.4 //common3
            ){

          Gindex1[Gcommon] = j;
          Gindex2[Gcommon] = jk;
          Gcommon++;
        }
      }
    }

 //   cout<<Gcommon<<endl;

    float GMZElec = 0;
//    float Gdielecpt =0;
//    float Gdielecrapidity =0;
//    float Gdielecphi =0;
 

    int Gind1 = -99;
    int Gind2 = -99;
 
    for (int gg = 0;gg<Gcommon;++gg){
      Gind1 = Gindex1[gg];
      Gind2 = Gindex2[gg];
    }




    if (Gind1 != -99 && Gind2 != -99){

      TLorentzVector m1;
      TLorentzVector m2;
      m1.SetPtEtaPhiE(Dr01LepPt->at(Gind1), Dr01LepEta->at(Gind1) ,Dr01LepPhi->at(Gind1),Dr01LepE->at(Gind1) );
      m2.SetPtEtaPhiE(Dr01LepPt->at(Gind2), Dr01LepEta->at(Gind2) ,Dr01LepPhi->at(Gind2),Dr01LepE->at(Gind2) );
      GMZElec= (m1+m2).M();

      if (GMZElec > 20.){
//        Gdielecpt = (m1+m2).Perp(); 
//        Gdielecrapidity = (m1+m2).Rapidity();
//        Gdielecphi=(m1+m2).Phi();
        if (GMZElec > 71.&& GMZElec < 111.){
   
          h_GMZElec->Fill(GMZElec,PUweight);
        }
      }
    }


     int common = 0;
    int select_dielec=0;
    int index1[100],index2[100];
    //if(doUnf)cout<<"NEW EVENT"<<endl;
    if(muon==0){
      for (unsigned int j = 0; j < patElecPt_->size(); ++j){ 
     
        for (unsigned int jk = j; jk < patElecPt_->size(); ++jk){
	  if (j == jk) continue;
          if (select_dielec==1) continue;
	  if ( (patElecCharge_->at(j)*patElecCharge_->at(jk)) == -1 && 
	      patElecPt_->at(j) > 20. && patElecPt_->at(jk) > 20. && 
	      fabs(patElecScEta_->at(j)) <=2.4 && fabs(patElecScEta_->at(jk)) <=2.4 &&
              (
               !(fabs(patElecScEta_->at(j)) >1.4442 && fabs(patElecScEta_->at(j))<1.566) && 
	       !(fabs(patElecScEta_->at(jk)) >1.4442 && fabs(patElecScEta_->at(jk))<1.566)
              ) &&
              patElecTrig_->at(j) > 0 && patElecTrig_->at(jk) > 0 &&
              patElecPfIsoRho_->at(j)<0.150 && patElecPfIsoRho_->at(jk)<0.150 &&
              patElecMediumIDOff_->at(j) >0 && patElecMediumIDOff_->at(jk) >0
	    ){

              select_dielec=1;
              if(fabs(patElecScEta_->at(j)) >1.4442 && fabs(patElecScEta_->at(j))<1.566)cout<<"Elektron gap dışlanmamış!!"<<endl;
              TLorentzVector e1_tmp;
              TLorentzVector e2_tmp;
              e1_tmp.SetPtEtaPhiE(patElecPt_->at(j), patElecEta_->at(j) ,patElecPhi_->at(j),patElecEnergy_->at(j) );
              e2_tmp.SetPtEtaPhiE(patElecPt_->at(jk), patElecEta_->at(jk) ,patElecPhi_->at(jk),patElecEnergy_->at(jk) );

              float MZElec_tmp= (e1_tmp+e2_tmp).M();

              if(MZElec_tmp<71. || MZElec_tmp>111.)continue;            
         

	      index1[common] = j;
	      index2[common] = jk;
	      common++;
	    
          }
        }
      }
    }


      double ptcut = 20;
    if(muon==1){
      for (unsigned int j = 0; j < patMuonPt_->size(); ++j){
        for (unsigned int jk = j; jk < patMuonPt_->size(); ++jk){
          if (j == jk) continue;
          if ((patMuonCharge_->at(j)*patMuonCharge_->at(jk)) == -1 &&
                patMuonPt_->at(j) > ptcut && patMuonPt_->at(jk) > ptcut && 
                fabs(patMuonEta_->at(j)) <2.4 && fabs(patMuonEta_->at(jk)) <2.4 &&
                patMuonTrig_->at(j) ==1  && patMuonTrig_->at(jk) == 1 &&
                patMuonPfIsoDbeta_->at(j)<0.2 && patMuonPfIsoDbeta_->at(jk)<0.2 &&
                patMuonCombId_->at(j) ==1 && patMuonCombId_->at(jk) ==1
               )
          {

            index1[common] = j;
            index2[common] = jk;
            common++;

          }
        }
      }
    }

    int ind1 = -99;
    int ind2 = -99;
    if(common>0){
      ind1 = index1[0];
      ind2 = index2[0];
    }


    float MZ = 0;
//    float Zpt =0;
//    float Zphi =0;
//    float Zrapidity =0;

    TLorentzVector l1;
    TLorentzVector l2;



    if (ind1 != -99 && ind2 != -99 &&common>0 /*&&common==1*/){
 
      if(muon==0){
        l1.SetPtEtaPhiE(patElecPt_->at(ind1), patElecEta_->at(ind1) ,patElecPhi_->at(ind1),patElecEnergy_->at(ind1) );
        l2.SetPtEtaPhiE(patElecPt_->at(ind2), patElecEta_->at(ind2) ,patElecPhi_->at(ind2),patElecEnergy_->at(ind2) );
      }

      if(muon==1){
        l1.SetPtEtaPhiE(patMuonPt_->at(ind1), patMuonEta_->at(ind1) ,patMuonPhi_->at(ind1),patMuonEn_->at(ind1) );
        l2.SetPtEtaPhiE(patMuonPt_->at(ind2), patMuonEta_->at(ind2) ,patMuonPhi_->at(ind2),patMuonEn_->at(ind2) );
      }
      MZ= (l1+l2).M();
      if ( MZ > 20.){
//      Zpt= (l1+l2).Perp();           
//	Zrapidity = (l1+l2).Rapidity();
//      Zphi = (l1+l2).Phi();

	if (MZ>71 && MZ<111){

          h_MZ->Fill(MZ,PUweight);

        }
      }
    }
    TLorentzVector j1_BEF;
    TLorentzVector j2_BEF;
 
    TLorentzVector j1G_BE;
    TLorentzVector j2G_BE;




    int Gn_jet_30_n=0;
    int n_jet_30_n_bef=0;

   /////////////Gen Jets///////////////////////
      for (unsigned int pf=0;pf < GjPt->size();pf++){

	if (GjPt->at(pf) > 30. && fabs(Gjeta->at(pf))<2.4){             

	  Gn_jet_30_n++;	 	  
	  if(Gn_jet_30_n==1){
	    j1G_BE.SetPtEtaPhiE(GjPt->at(pf),Gjeta->at(pf),Gjphi->at(pf),GjE->at(pf));
          }
          if(Gn_jet_30_n==2){
	    j2G_BE.SetPtEtaPhiE(GjPt->at(pf),Gjeta->at(pf),Gjphi->at(pf),GjE->at(pf));
          }
        }
      }/// end Gen jet loop

    /////////////Jets///////////////////////




      for (unsigned int pf=0;pf < patJetPfAk05Pt_->size();pf++){

          if(patJetPfAk05Pt_->at(pf) > 30.&&0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<4.7  
             ){           
            if ( patJetPfAk05LooseId_->at(pf)>=1 && patJetPfAk05jetpukLoose_->at(pf)==1){
              n_jet_30_n_bef++;

	      TLorentzVector j;
              j.SetPtEtaPhiE(patJetPfAk05Pt_->at(pf),patJetPfAk05Eta_->at(pf),patJetPfAk05Phi_->at(pf),patJetPfAk05En_->at(pf));

    	      if(n_jet_30_n_bef==1){
	        j1_BEF.SetPtEtaPhiE(j.Pt(),j.Eta(),j.Phi(),j.E());          
              }
              if(n_jet_30_n_bef==2){
	        j2_BEF.SetPtEtaPhiE(j.Pt(),j.Eta(),j.Phi(),j.E());
              }
            }  
          }
      }

  }// end of event loop
  theFile->Write();
  theFile->Close();

}//end void  

int main(int argc,char **argv){
  tree1r();
}


float DeltaR(float eta1, float eta2, float phi1, float phi2)
{
  float deta = eta2 - eta1;
  float dphi = phi2 - phi1;
  if (fabs(dphi) > 3.14) dphi = 6.28 - fabs(dphi);
  float DELTAR = sqrt(pow(dphi,2)+pow(deta,2))*1.0;
  return DELTAR;
}
float DeltaPhi(float phi1, float phi2)
{
  float dphi = phi2 - phi1;
  if (fabs(dphi) > 3.14) dphi = 6.28 - fabs(dphi);
  return dphi;
}




