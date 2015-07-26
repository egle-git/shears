//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu May  7 13:50:00 2015 by ROOT version 5.34/22
// from TTree MuonTree/MuonTree
// found on file: root://eoscms//eos/cms/store/group/phys_smp/VJets/DYJetsToLL_ntuple.root
//////////////////////////////////////////////////////////

#ifndef BonzaiMaker_h
#define BonzaiMaker_h

//#include <vector>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TString.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ConfigVJets.h>

using namespace std;



class BonzaiMaker {
public :
   TChain          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   vector<double>  *METPt;
   vector<double>  *METPx;
   vector<double>  *METPy;
   vector<double>  *METPz;
   vector<double>  *METE;
   vector<double>  *METsigx2;
   vector<double>  *METsigxy;
   vector<double>  *METsigy2;
   vector<double>  *METsig;
   UInt_t          event;
   Int_t           realdata;
   UInt_t          run;
   UInt_t          lumi;
   Int_t           bxnumber;
   Double_t        EvtInfo_NumVtx;
   Double_t        PU_npT;
   Double_t        PU_npIT;
   Double_t        MyWeight;
   vector<double>  *Dr01LepPt;
   vector<double>  *Dr01LepEta;
   vector<double>  *Dr01LepPhi;
   vector<double>  *Dr01LepE;
   vector<double>  *Dr01LepM;
   vector<double>  *Dr01LepId;
   vector<double>  *Dr01LepStatus;
   vector<double>  *Bare01LepPt;
   vector<double>  *Bare01LepEta;
   vector<double>  *Bare01LepPhi;
   vector<double>  *Bare01LepE;
   vector<double>  *Bare01LepM;
   vector<double>  *Bare01LepId;
   vector<double>  *Bare01LepStatus;
   vector<double>  *St03Pt;
   vector<double>  *St03Eta;
   vector<double>  *St03Phi;
   vector<double>  *St03E;
   vector<double>  *St03M;
   vector<double>  *St03Id;
   vector<double>  *St03Status;
   vector<double>  *St01PhotonPt;
   vector<double>  *St01PhotonEta;
   vector<double>  *St01PhotonPhi;
   vector<double>  *St01PhotonE;
   vector<double>  *St01PhotonM;
   vector<double>  *St01PhotonId;
   vector<double>  *St01PhotonMomId;
   vector<double>  *St01PhotonNumberMom;
   vector<double>  *St01PhotonStatus;
   vector<double>  *GjPt;
   vector<double>  *Gjeta;
   vector<double>  *Gjphi;
   vector<double>  *GjE;
   vector<double>  *GjPx;
   vector<double>  *GjPy;
   vector<double>  *GjPz;
   vector<double>  *GjChargedFraction;
   vector<bool>    *matchGjet;
   vector<double>  *MGjPt;
   vector<double>  *MGjeta;
   vector<double>  *MGjphi;
   vector<double>  *MGjE;
   Double_t        HLT_Mu17_Mu8;
   Double_t        HLT_Mu17_TkMu8;
   Double_t        HLT_Elec17_Elec8;
   vector<double>  *patMuonPt_;
   vector<double>  *patMuonEta_;
   vector<double>  *patMuonPhi_;
   vector<double>  *patMuonVtxZ_;
   vector<double>  *patMuonEn_;
   vector<double>  *patMuonCharge_;
   vector<double>  *patMuonDxy_;
   vector<double>  *patMuonCombId_;
   vector<double>  *patMuonTrig_;
   vector<double>  *patMuonDetIsoRho_;
   vector<double>  *patMuonPfIsoDbeta_;
   vector<double>  *patMuonM_;
   vector<double>  *patMuonPx_;
   vector<double>  *patMuonPy_;
   vector<double>  *patMuonPz_;
   vector<double>  *patMuonGlobalType_;
   vector<double>  *patMuonTrackerType_;
   vector<double>  *patMuonPFType_;
   vector<double>  *patMuonIsoSumPt_;
   vector<double>  *patMuonIsoRelative_;
   vector<double>  *patMuonIsoCalComb_;
   vector<double>  *patMuonIsoDY_;
   vector<double>  *patMuonChi2Ndoff_;
   vector<double>  *patMuonNhits_;
   vector<double>  *patMuonNMatches_;
   vector<double>  *patMuonDz_;
   vector<double>  *patMuonPhits_;
   vector<double>  *patMuonTkLayers_;
   vector<double>  *patMuon_PF_IsoSumChargedHadronPt_;
   vector<double>  *patMuon_PF_IsoSumNeutralHadronEt_;
   vector<double>  *patMuon_PF_IsoDY_;
   vector<double>  *patMuon_Mu17_Mu8_Matched_;
   vector<double>  *patMuon_Mu17_TkMu8_Matched_;
   vector<double>  *patElecdEtaIn_;
   vector<double>  *patElecdPhiIn_;
   vector<double>  *patElechOverE_;
   vector<double>  *patElecsigmaIetaIeta_;
   vector<double>  *patElecfull5x5_sigmaIetaIeta_;
   vector<double>  *patElecooEmooP_;
   vector<double>  *patElecd0_;
   vector<double>  *patElecdz_;
   vector<int>     *patElecexpectedMissingInnerHits_;
   vector<int>     *patElecpassConversionVeto_;
   vector<double>  *patElecTrig_;
   vector<double>  *patElecDz_;
   vector<double>  *patElecMVATrigId_;
   vector<double>  *patElecMVANonTrigId_;
   vector<double>  *patElecPt_;
   vector<double>  *patElecEta_;
   vector<double>  *patElecScEta_;
   vector<double>  *patElecPhi_;
   vector<double>  *patElecEnergy_;
   vector<double>  *patElecCharge_;
   vector<double>  *patElecMediumIDOff_;
   vector<double>  *patElecMediumIDOff_Tom_;
   vector<double>  *patElecchIso03_;
   vector<double>  *patElecnhIso03_;
   vector<double>  *patElecphIso03_;
   vector<double>  *patElecpuChIso03_;
   vector<double>  *patElecPfIso_;
   vector<double>  *patElecPfIsodb_;
   vector<double>  *patElecPfIsoRho_;
   Double_t        rhoPrime;
   vector<double>  *neutral_;
   vector<double>  *photon_;
   vector<double>  *charged_;
   vector<double>  *neutral_Tom_;
   vector<double>  *photon_Tom_;
   vector<double>  *charged_Tom_;
   Double_t        AEff;
   vector<double>  *patElec_mva_presel_;
   vector<double>  *patJetPfAk05En_;
   vector<double>  *patJetPfAk05Pt_;
   vector<double>  *patJetPfAk05Eta_;
   vector<double>  *patJetPfAk05Phi_;
   vector<double>  *patJetPfAk05LooseId_;
   vector<double>  *patJetPfAk05Et_;
   vector<double>  *patJetPfAk05RawPt_;
   vector<double>  *patJetPfAk05RawEn_;
   vector<double>  *patJetPfAk05HadEHF_;
   vector<double>  *patJetPfAk05EmEHF_;
   vector<double>  *patJetPfAk05chf_;
   vector<double>  *patJetPfAk05nhf_;
   vector<double>  *patJetPfAk05cemf_;
   vector<double>  *patJetPfAk05nemf_;
   vector<double>  *patJetPfAk05cmult_;
   vector<double>  *patJetPfAk05nconst_;
   vector<double>  *patJetPfAk05jetBeta_;
   vector<double>  *patJetPfAk05jetBetaClassic_;
   vector<double>  *patJetPfAk05jetBetaStar_;
   vector<double>  *patJetPfAk05jetBetaStarClassic_;
   vector<double>  *patJetPfAk05jetpuMVA_;
   vector<bool>    *patJetPfAk05jetpukLoose_;
   vector<bool>    *patJetPfAk05jetpukMedium_;
   vector<bool>    *patJetPfAk05jetpukTight_;
   vector<double>  *patJetPfAk05BDiscCSV_;
   vector<double>  *patJetPfAk05BDiscCSVV1_;
   vector<double>  *patJetPfAk05BDiscCSVSLV1_;
   vector<double>  *unc_;
   vector<double>  *patJetPfAk05PtUp_;
   vector<double>  *patJetPfAk05PtDn_;
   vector<double>  *caloJetPt_;
   vector<double>  *caloJetRawPt_;
   vector<double>  *caloJetEn_;
   vector<double>  *caloJetEta_;
   vector<double>  *caloJetPhi_;
   vector<double>  *caloJetHadEHF_;
   vector<double>  *caloJetEmEHF_;
   vector<double>  *caloJetEmFrac_;
   vector<double>  *caloJetn90_;
   vector<double>  *id1_pdfInfo_;
   vector<double>  *id2_pdfInfo_;
   vector<double>  *x1_pdfInfo_;
   vector<double>  *x2_pdfInfo_;
   vector<double>  *scalePDF_pdfInfo_;
   Double_t        ptHat_;
   Double_t        mcWeight_;
   Double_t        nup;
   Int_t           ln;
   Float_t         ln_px[5];   //[ln]
   Float_t         ln_py[5];   //[ln]
   Float_t         ln_pz[5];   //[ln]
   Float_t         ln_en[5];   //[ln]

   // List of branches
   TBranch        *b_METPt;   //!
   TBranch        *b_METPx;   //!
   TBranch        *b_METPy;   //!
   TBranch        *b_METPz;   //!
   TBranch        *b_METE;   //!
   TBranch        *b_METsigx2;   //!
   TBranch        *b_METsigxy;   //!
   TBranch        *b_METsigy2;   //!
   TBranch        *b_METsig;   //!
   TBranch        *b_event;   //!
   TBranch        *b_realdata;   //!
   TBranch        *b_run;   //!
   TBranch        *b_lumi;   //!
   TBranch        *b_bxnumber;   //!
   TBranch        *b_EvtInfo_NumVtx;   //!
   TBranch        *b_PU_npT;   //!
   TBranch        *b_PU_npIT;   //!
   TBranch        *b_MyWeight;   //!
   TBranch        *b_Dr01LepPt;   //!
   TBranch        *b_Dr01LepEta;   //!
   TBranch        *b_Dr01LepPhi;   //!
   TBranch        *b_Dr01LepE;   //!
   TBranch        *b_Dr01LepM;   //!
   TBranch        *b_Dr01LepId;   //!
   TBranch        *b_Dr01LepStatus;   //!
   TBranch        *b_Bare01LepPt;   //!
   TBranch        *b_Bare01LepEta;   //!
   TBranch        *b_Bare01LepPhi;   //!
   TBranch        *b_Bare01LepE;   //!
   TBranch        *b_Bare01LepM;   //!
   TBranch        *b_Bare01LepId;   //!
   TBranch        *b_Bare01LepStatus;   //!
   TBranch        *b_St03Pt;   //!
   TBranch        *b_St03Eta;   //!
   TBranch        *b_St03Phi;   //!
   TBranch        *b_St03E;   //!
   TBranch        *b_St03M;   //!
   TBranch        *b_St03Id;   //!
   TBranch        *b_St03Status;   //!
   TBranch        *b_St01PhotonPt;   //!
   TBranch        *b_St01PhotonEta;   //!
   TBranch        *b_St01PhotonPhi;   //!
   TBranch        *b_St01PhotonE;   //!
   TBranch        *b_St01PhotonM;   //!
   TBranch        *b_St01PhotonId;   //!
   TBranch        *b_St01PhotonMomId;   //!
   TBranch        *b_St01PhotonNumberMom;   //!
   TBranch        *b_St01PhotonStatus;   //!
   TBranch        *b_GjPt;   //!
   TBranch        *b_Gjeta;   //!
   TBranch        *b_Gjphi;   //!
   TBranch        *b_GjE;   //!
   TBranch        *b_GjPx;   //!
   TBranch        *b_GjPy;   //!
   TBranch        *b_GjPz;   //!
   TBranch        *b_GjChargedFraction;   //!
   TBranch        *b_matchGjet;   //!
   TBranch        *b_MGjPt;   //!
   TBranch        *b_MGjeta;   //!
   TBranch        *b_MGjphi;   //!
   TBranch        *b_MGjE;   //!
   TBranch        *b_HLT_Mu17_Mu8;   //!
   TBranch        *b_HLT_Mu17_TkMu8;   //!
   TBranch        *b_HLT_Elec17_Elec8;   //!
   TBranch        *b_patMuonPt_;   //!
   TBranch        *b_patMuonEta_;   //!
   TBranch        *b_patMuonPhi_;   //!
   TBranch        *b_patMuonVtxZ_;   //!
   TBranch        *b_patMuonEn_;   //!
   TBranch        *b_patMuonCharge_;   //!
   TBranch        *b_patMuonDxy_;   //!
   TBranch        *b_patMuonCombId_;   //!
   TBranch        *b_patMuonTrig_;   //!
   TBranch        *b_patMuonDetIsoRho_;   //!
   TBranch        *b_patMuonPfIsoDbeta_;   //!
   TBranch        *b_patMuonM_;   //!
   TBranch        *b_patMuonPx_;   //!
   TBranch        *b_patMuonPy_;   //!
   TBranch        *b_patMuonPz_;   //!
   TBranch        *b_patMuonGlobalType_;   //!
   TBranch        *b_patMuonTrackerType_;   //!
   TBranch        *b_patMuonPFType_;   //!
   TBranch        *b_patMuonIsoSumPt_;   //!
   TBranch        *b_patMuonIsoRelative_;   //!
   TBranch        *b_patMuonIsoCalComb_;   //!
   TBranch        *b_patMuonIsoDY_;   //!
   TBranch        *b_patMuonChi2Ndoff_;   //!
   TBranch        *b_patMuonNhits_;   //!
   TBranch        *b_patMuonNMatches_;   //!
   TBranch        *b_patMuonDz_;   //!
   TBranch        *b_patMuonPhits_;   //!
   TBranch        *b_patMuonTkLayers_;   //!
   TBranch        *b_patMuon_PF_IsoSumChargedHadronPt_;   //!
   TBranch        *b_patMuon_PF_IsoSumNeutralHadronEt_;   //!
   TBranch        *b_patMuon_PF_IsoDY_;   //!
   TBranch        *b_patMuon_Mu17_Mu8_Matched_;   //!
   TBranch        *b_patMuon_Mu17_TkMu8_Matched_;   //!
   TBranch        *b_patElecdEtaIn_;   //!
   TBranch        *b_patElecdPhiIn_;   //!
   TBranch        *b_patElechOverE_;   //!
   TBranch        *b_patElecsigmaIetaIeta_;   //!
   TBranch        *b_patElecfull5x5_sigmaIetaIeta_;   //!
   TBranch        *b_patElecooEmooP_;   //!
   TBranch        *b_patElecd0_;   //!
   TBranch        *b_patElecdz_;   //!
   TBranch        *b_patElecexpectedMissingInnerHits_;   //!
   TBranch        *b_patElecpassConversionVeto_;   //!
   TBranch        *b_patElecTrig_;   //!
   TBranch        *b_patElecDz_;   //!
   TBranch        *b_patElecMVATrigId_;   //!
   TBranch        *b_patElecMVANonTrigId_;   //!
   TBranch        *b_patElecPt_;   //!
   TBranch        *b_patElecEta_;   //!
   TBranch        *b_patElecScEta_;   //!
   TBranch        *b_patElecPhi_;   //!
   TBranch        *b_patElecEnergy_;   //!
   TBranch        *b_patElecCharge_;   //!
   TBranch        *b_patElecMediumIDOff_;   //!
   TBranch        *b_patElecMediumIDOff_Tom_;   //!
   TBranch        *b_patElecchIso03_;   //!
   TBranch        *b_patElecnhIso03_;   //!
   TBranch        *b_patElecphIso03_;   //!
   TBranch        *b_patElecpuChIso03_;   //!
   TBranch        *b_patElecPfIso_;   //!
   TBranch        *b_patElecPfIsodb_;   //!
   TBranch        *b_patElecPfIsoRho_;   //!
   TBranch        *b_rhoPrime;   //!
   TBranch        *b_neutral_;   //!
   TBranch        *b_photon_;   //!
   TBranch        *b_charged_;   //!
   TBranch        *b_neutral_Tom_;   //!
   TBranch        *b_photon_Tom_;   //!
   TBranch        *b_charged_Tom_;   //!
   TBranch        *b_AEff;   //!
   TBranch        *b_patElec_mva_presel_;   //!
   TBranch        *b_patJetPfAk05En_;   //!
   TBranch        *b_patJetPfAk05Pt_;   //!
   TBranch        *b_patJetPfAk05Eta_;   //!
   TBranch        *b_patJetPfAk05Phi_;   //!
   TBranch        *b_patJetPfAk05LooseId_;   //!
   TBranch        *b_patJetPfAk05Et_;   //!
   TBranch        *b_patJetPfAk05RawPt_;   //!
   TBranch        *b_patJetPfAk05RawEn_;   //!
   TBranch        *b_patJetPfAk05HadEHF_;   //!
   TBranch        *b_patJetPfAk05EmEHF_;   //!
   TBranch        *b_patJetPfAk05chf_;   //!
   TBranch        *b_patJetPfAk05nhf_;   //!
   TBranch        *b_patJetPfAk05cemf_;   //!
   TBranch        *b_patJetPfAk05nemf_;   //!
   TBranch        *b_patJetPfAk05cmult_;   //!
   TBranch        *b_patJetPfAk05nconst_;   //!
   TBranch        *b_patJetPfAk05jetBeta_;   //!
   TBranch        *b_patJetPfAk05jetBetaClassic_;   //!
   TBranch        *b_patJetPfAk05jetBetaStar_;   //!
   TBranch        *b_patJetPfAk05jetBetaStarClassic_;   //!
   TBranch        *b_patJetPfAk05jetpuMVA_;   //!
   TBranch        *b_patJetPfAk05jetpukLoose_;   //!
   TBranch        *b_patJetPfAk05jetpukMedium_;   //!
   TBranch        *b_patJetPfAk05jetpukTight_;   //!
   TBranch        *b_patJetPfAk05BDiscCSV_;   //!
   TBranch        *b_patJetPfAk05BDiscCSVV1_;   //!
   TBranch        *b_patJetPfAk05BDiscCSVSLV1_;   //!
   TBranch        *b_unc_;   //!
   TBranch        *b_patJetPfAk05PtUp_;   //!
   TBranch        *b_patJetPfAk05PtDn_;   //!
   TBranch        *b_caloJetPt_;   //!
   TBranch        *b_caloJetRawPt_;   //!
   TBranch        *b_caloJetEn_;   //!
   TBranch        *b_caloJetEta_;   //!
   TBranch        *b_caloJetPhi_;   //!
   TBranch        *b_caloJetHadEHF_;   //!
   TBranch        *b_caloJetEmEHF_;   //!
   TBranch        *b_caloJetEmFrac_;   //!
   TBranch        *b_caloJetn90_;   //!
   TBranch        *b_id1_pdfInfo_;   //!
   TBranch        *b_id2_pdfInfo_;   //!
   TBranch        *b_x1_pdfInfo_;   //!
   TBranch        *b_x2_pdfInfo_;   //!
   TBranch        *b_scalePDF_pdfInfo_;   //!
   TBranch        *b_ptHat_;   //!
   TBranch        *b_mcWeight_;   //!
   TBranch        *b_nup;   //!
   TBranch        *b_ln;   //!

   BonzaiMaker(ConfigVJets cfg);//, TString txtFileName="", TString anaSel="", TString lepSel="", TString doWhat="");
   virtual ~BonzaiMaker();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TChain *fChain);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   TString txtFileName_;
   TString anaSel_;
   TString lepSel_;
   TString doWhat_;
   ConfigVJets cfg_;
};

#endif

