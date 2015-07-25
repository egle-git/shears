//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Jun  1 13:11:42 2015 by ROOT version 5.34/22
// from TTree MuonTree/MuonTree
// found on file: root://eoscms//eos/cms/store/group/phys_smp/VJets/DYJetsToLL_ntuple.root
//////////////////////////////////////////////////////////

#ifndef test_h
#define test_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <vector>
#include <vector>

// Fixed size dimensions of array or collections stored in the TTree if any.
   const Int_t kMaxpatMuonPt = 1;
   const Int_t kMaxpatMuonEta = 1;
   const Int_t kMaxpatMuonPhi = 1;
   const Int_t kMaxpatMuonVtxZ = 1;
   const Int_t kMaxpatMuonEn = 1;
   const Int_t kMaxpatMuonCharge = 1;
   const Int_t kMaxpatMuonDxy = 1;
   const Int_t kMaxpatMuonCombId = 1;
   const Int_t kMaxpatMuonTrig = 1;
   const Int_t kMaxpatMuonDetIsoRho = 1;
   const Int_t kMaxpatMuonPfIsoDbeta = 1;
   const Int_t kMaxpatMuonM = 1;
   const Int_t kMaxpatMuonPx = 1;
   const Int_t kMaxpatMuonPy = 1;
   const Int_t kMaxpatMuonPz = 1;
   const Int_t kMaxpatMuonGlobalType = 1;
   const Int_t kMaxpatMuonTrackerType = 1;
   const Int_t kMaxpatMuonPFType = 1;
   const Int_t kMaxpatMuonIsoSumPt = 1;
   const Int_t kMaxpatMuonIsoRelative = 1;
   const Int_t kMaxpatMuonIsoCalComb = 1;
   const Int_t kMaxpatMuonIsoDY = 1;
   const Int_t kMaxpatMuonChi2Ndoff = 1;
   const Int_t kMaxpatMuonNhits = 1;
   const Int_t kMaxpatMuonNMatches = 1;
   const Int_t kMaxpatMuonDz = 1;
   const Int_t kMaxpatMuonPhits = 1;
   const Int_t kMaxpatMuonTkLayers = 1;
   const Int_t kMaxpatMuon_PF_IsoSumChargedHadronPt = 1;
   const Int_t kMaxpatMuon_PF_IsoSumNeutralHadronEt = 1;
   const Int_t kMaxpatMuon_PF_IsoDY = 1;
   const Int_t kMaxpatMuon_Mu17_Mu8_Matched = 1;
   const Int_t kMaxpatMuon_Mu17_TkMu8_Matched = 1;
   const Int_t kMaxpatElecdEtaIn = 1;
   const Int_t kMaxpatElecdPhiIn = 1;
   const Int_t kMaxpatElechOverE = 1;
   const Int_t kMaxpatElecsigmaIetaIeta = 1;
   const Int_t kMaxpatElecfull5x5_sigmaIetaIeta = 1;
   const Int_t kMaxpatElecooEmooP = 1;
   const Int_t kMaxpatElecd0 = 1;
   const Int_t kMaxpatElecdz = 1;
   const Int_t kMaxpatElecexpectedMissingInnerHits = 1;
   const Int_t kMaxpatElecpassConversionVeto = 1;
   const Int_t kMaxpatElecTrig = 1;
   const Int_t kMaxpatElecDz = 1;
   const Int_t kMaxpatElecMVATrigId = 1;
   const Int_t kMaxpatElecMVANonTrigId = 1;
   const Int_t kMaxpatElecPt = 1;
   const Int_t kMaxpatElecEta = 1;
   const Int_t kMaxpatElecScEta = 1;
   const Int_t kMaxpatElecPhi = 1;
   const Int_t kMaxpatElecEnergy = 1;
   const Int_t kMaxpatElecCharge = 1;
   const Int_t kMaxpatElecMediumIDOff = 1;
   const Int_t kMaxpatElecMediumIDOff_Tom = 1;
   const Int_t kMaxpatElecchIso03 = 1;
   const Int_t kMaxpatElecnhIso03 = 1;
   const Int_t kMaxpatElecphIso03 = 1;
   const Int_t kMaxpatElecpuChIso03 = 1;
   const Int_t kMaxpatElecPfIso = 1;
   const Int_t kMaxpatElecPfIsodb = 1;
   const Int_t kMaxpatElecPfIsoRho = 1;
   const Int_t kMaxneutral = 1;
   const Int_t kMaxphoton = 1;
   const Int_t kMaxcharged = 1;
   const Int_t kMaxneutral_Tom = 1;
   const Int_t kMaxphoton_Tom = 1;
   const Int_t kMaxcharged_Tom = 1;
   const Int_t kMaxpatElec_mva_presel = 1;
   const Int_t kMaxpatJetPfAk05En = 1;
   const Int_t kMaxpatJetPfAk05Pt = 1;
   const Int_t kMaxpatJetPfAk05Eta = 1;
   const Int_t kMaxpatJetPfAk05Phi = 1;
   const Int_t kMaxpatJetPfAk05LooseId = 1;
   const Int_t kMaxpatJetPfAk05Et = 1;
   const Int_t kMaxpatJetPfAk05RawPt = 1;
   const Int_t kMaxpatJetPfAk05RawEn = 1;
   const Int_t kMaxpatJetPfAk05HadEHF = 1;
   const Int_t kMaxpatJetPfAk05EmEHF = 1;
   const Int_t kMaxpatJetPfAk05chf = 1;
   const Int_t kMaxpatJetPfAk05nhf = 1;
   const Int_t kMaxpatJetPfAk05cemf = 1;
   const Int_t kMaxpatJetPfAk05nemf = 1;
   const Int_t kMaxpatJetPfAk05cmult = 1;
   const Int_t kMaxpatJetPfAk05nconst = 1;
   const Int_t kMaxpatJetPfAk05jetBeta = 1;
   const Int_t kMaxpatJetPfAk05jetBetaClassic = 1;
   const Int_t kMaxpatJetPfAk05jetBetaStar = 1;
   const Int_t kMaxpatJetPfAk05jetBetaStarClassic = 1;
   const Int_t kMaxpatJetPfAk05jetpuMVA = 1;
   const Int_t kMaxpatJetPfAk05jetpukLoose = 1;
   const Int_t kMaxpatJetPfAk05jetpukMedium = 1;
   const Int_t kMaxpatJetPfAk05jetpukTight = 1;
   const Int_t kMaxpatJetPfAk05BDiscCSV = 1;
   const Int_t kMaxpatJetPfAk05BDiscCSVV1 = 1;
   const Int_t kMaxpatJetPfAk05BDiscCSVSLV1 = 1;
   const Int_t kMaxunc = 1;
   const Int_t kMaxpatJetPfAk05PtUp = 1;
   const Int_t kMaxpatJetPfAk05PtDn = 1;
   const Int_t kMaxcaloJetPt = 1;
   const Int_t kMaxcaloJetRawPt = 1;
   const Int_t kMaxcaloJetEn = 1;
   const Int_t kMaxcaloJetEta = 1;
   const Int_t kMaxcaloJetPhi = 1;
   const Int_t kMaxcaloJetHadEHF = 1;
   const Int_t kMaxcaloJetEmEHF = 1;
   const Int_t kMaxcaloJetEmFrac = 1;
   const Int_t kMaxcaloJetn90 = 1;
   const Int_t kMaxid1_pdfInfo = 1;
   const Int_t kMaxid2_pdfInfo = 1;
   const Int_t kMaxx1_pdfInfo = 1;
   const Int_t kMaxx2_pdfInfo = 1;
   const Int_t kMaxscalePDF_pdfInfo = 1;
   const Int_t kMaxptHat = 1;
   const Int_t kMaxmcWeight = 1;

class test {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   vector<float>   *METPt;
   vector<float>   *METPx;
   vector<float>   *METPy;
   vector<float>   *METPz;
   vector<float>   *METE;
   vector<float>   *METsigx2;
   vector<float>   *METsigxy;
   vector<float>   *METsigy2;
   vector<float>   *METsig;
   UInt_t          event;
   Int_t           realdata;
   UInt_t          run;
   UInt_t          lumi;
   Int_t           bxnumber;
   Double_t        EvtInfo_NumVtx;
   Double_t        PU_npT;
   Double_t        PU_npIT;
   Double_t        MyWeight;
   vector<float>   *Dr01LepPt;
   vector<float>   *Dr01LepEta;
   vector<float>   *Dr01LepPhi;
   vector<float>   *Dr01LepE;
   vector<float>   *Dr01LepM;
   vector<float>   *Dr01LepId;
   vector<float>   *Dr01LepStatus;
   vector<float>   *Bare01LepPt;
   vector<float>   *Bare01LepEta;
   vector<float>   *Bare01LepPhi;
   vector<float>   *Bare01LepE;
   vector<float>   *Bare01LepM;
   vector<float>   *Bare01LepId;
   vector<float>   *Bare01LepStatus;
   vector<float>   *St03Pt;
   vector<float>   *St03Eta;
   vector<float>   *St03Phi;
   vector<float>   *St03E;
   vector<float>   *St03M;
   vector<float>   *St03Id;
   vector<float>   *St03Status;
   vector<float>   *St01PhotonPt;
   vector<float>   *St01PhotonEta;
   vector<float>   *St01PhotonPhi;
   vector<float>   *St01PhotonE;
   vector<float>   *St01PhotonM;
   vector<float>   *St01PhotonId;
   vector<float>   *St01PhotonMomId;
   vector<float>   *St01PhotonNumberMom;
   vector<float>   *St01PhotonStatus;
   vector<float>   *GjPt;
   vector<float>   *Gjeta;
   vector<float>   *Gjphi;
   vector<float>   *GjE;
   vector<float>   *GjPx;
   vector<float>   *GjPy;
   vector<float>   *GjPz;
   vector<float>   *GjChargedFraction;
   vector<bool>    *matchGjet;
   vector<float>   *MGjPt;
   vector<float>   *MGjeta;
   vector<float>   *MGjphi;
   vector<float>   *MGjE;
   Float_t         HLT_Mu17_Mu8;
   Float_t         HLT_Mu17_TkMu8;
   Float_t         HLT_Elec17_Elec8;
   vector<float>   *patMuonPt_;
   vector<float>   *patMuonEta_;
   vector<float>   *patMuonPhi_;
   vector<float>   *patMuonVtxZ_;
   vector<float>   *patMuonEn_;
   vector<float>   *patMuonCharge_;
   vector<float>   *patMuonDxy_;
   vector<float>   *patMuonCombId_;
   vector<float>   *patMuonTrig_;
   vector<float>   *patMuonDetIsoRho_;
   vector<float>   *patMuonPfIsoDbeta_;
   vector<float>   *patMuonM_;
   vector<float>   *patMuonPx_;
   vector<float>   *patMuonPy_;
   vector<float>   *patMuonPz_;
   vector<float>   *patMuonGlobalType_;
   vector<float>   *patMuonTrackerType_;
   vector<float>   *patMuonPFType_;
   vector<float>   *patMuonIsoSumPt_;
   vector<float>   *patMuonIsoRelative_;
   vector<float>   *patMuonIsoCalComb_;
   vector<float>   *patMuonIsoDY_;
   vector<float>   *patMuonChi2Ndoff_;
   vector<float>   *patMuonNhits_;
   vector<float>   *patMuonNMatches_;
   vector<float>   *patMuonDz_;
   vector<float>   *patMuonPhits_;
   vector<float>   *patMuonTkLayers_;
   vector<float>   *patMuon_PF_IsoSumChargedHadronPt_;
   vector<float>   *patMuon_PF_IsoSumNeutralHadronEt_;
   vector<float>   *patMuon_PF_IsoDY_;
   vector<float>   *patMuon_Mu17_Mu8_Matched_;
   vector<float>   *patMuon_Mu17_TkMu8_Matched_;
   vector<float>   *patElecdEtaIn_;
   vector<float>   *patElecdPhiIn_;
   vector<float>   *patElechOverE_;
   vector<float>   *patElecsigmaIetaIeta_;
   vector<float>   *patElecfull5x5_sigmaIetaIeta_;
   vector<float>   *patElecooEmooP_;
   vector<float>   *patElecd0_;
   vector<float>   *patElecdz_;
   vector<int>     *patElecexpectedMissingInnerHits_;
   vector<int>     *patElecpassConversionVeto_;
   vector<float>   *patElecTrig_;
   vector<float>   *patElecDz_;
   vector<float>   *patElecMVATrigId_;
   vector<float>   *patElecMVANonTrigId_;
   vector<float>   *patElecPt_;
   vector<float>   *patElecEta_;
   vector<float>   *patElecScEta_;
   vector<float>   *patElecPhi_;
   vector<float>   *patElecEnergy_;
   vector<float>   *patElecCharge_;
   vector<float>   *patElecMediumIDOff_;
   vector<float>   *patElecMediumIDOff_Tom_;
   vector<float>   *patElecchIso03_;
   vector<float>   *patElecnhIso03_;
   vector<float>   *patElecphIso03_;
   vector<float>   *patElecpuChIso03_;
   vector<float>   *patElecPfIso_;
   vector<float>   *patElecPfIsodb_;
   vector<float>   *patElecPfIsoRho_;
   Float_t         rhoPrime;
   vector<float>   *neutral_;
   vector<float>   *photon_;
   vector<float>   *charged_;
   vector<float>   *neutral_Tom_;
   vector<float>   *photon_Tom_;
   vector<float>   *charged_Tom_;
   Float_t         AEff;
   vector<float>   *patElec_mva_presel_;
   vector<float>   *patJetPfAk05En_;
   vector<float>   *patJetPfAk05Pt_;
   vector<float>   *patJetPfAk05Eta_;
   vector<float>   *patJetPfAk05Phi_;
   vector<float>   *patJetPfAk05LooseId_;
   vector<float>   *patJetPfAk05Et_;
   vector<float>   *patJetPfAk05RawPt_;
   vector<float>   *patJetPfAk05RawEn_;
   vector<float>   *patJetPfAk05HadEHF_;
   vector<float>   *patJetPfAk05EmEHF_;
   vector<float>   *patJetPfAk05chf_;
   vector<float>   *patJetPfAk05nhf_;
   vector<float>   *patJetPfAk05cemf_;
   vector<float>   *patJetPfAk05nemf_;
   vector<float>   *patJetPfAk05cmult_;
   vector<float>   *patJetPfAk05nconst_;
   vector<float>   *patJetPfAk05jetBeta_;
   vector<float>   *patJetPfAk05jetBetaClassic_;
   vector<float>   *patJetPfAk05jetBetaStar_;
   vector<float>   *patJetPfAk05jetBetaStarClassic_;
   vector<float>   *patJetPfAk05jetpuMVA_;
   vector<bool>    *patJetPfAk05jetpukLoose_;
   vector<bool>    *patJetPfAk05jetpukMedium_;
   vector<bool>    *patJetPfAk05jetpukTight_;
   vector<float>   *patJetPfAk05BDiscCSV_;
   vector<float>   *patJetPfAk05BDiscCSVV1_;
   vector<float>   *patJetPfAk05BDiscCSVSLV1_;
   vector<float>   *unc_;
   vector<float>   *patJetPfAk05PtUp_;
   vector<float>   *patJetPfAk05PtDn_;
   vector<float>   *caloJetPt_;
   vector<float>   *caloJetRawPt_;
   vector<float>   *caloJetEn_;
   vector<float>   *caloJetEta_;
   vector<float>   *caloJetPhi_;
   vector<float>   *caloJetHadEHF_;
   vector<float>   *caloJetEmEHF_;
   vector<float>   *caloJetEmFrac_;
   vector<float>   *caloJetn90_;
   vector<float>   *id1_pdfInfo_;
   vector<float>   *id2_pdfInfo_;
   vector<float>   *x1_pdfInfo_;
   vector<float>   *x2_pdfInfo_;
   vector<float>   *scalePDF_pdfInfo_;
   Float_t         ptHat_;
   Float_t         mcWeight_;
   Double_t        nup;

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

   test(TTree *tree=0);
   virtual ~test();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef test_cxx
test::test(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("root://eoscms//eos/cms/store/group/phys_smp/VJets/DYJetsToLL_ntuple.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("root://eoscms//eos/cms/store/group/phys_smp/VJets/DYJetsToLL_ntuple.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("root://eoscms//eos/cms/store/group/phys_smp/VJets/DYJetsToLL_ntuple.root:/tupel");
      dir->GetObject("MuonTree",tree);

   }
   Init(tree);
}

test::~test()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t test::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t test::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void test::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   METPt = 0;
   METPx = 0;
   METPy = 0;
   METPz = 0;
   METE = 0;
   METsigx2 = 0;
   METsigxy = 0;
   METsigy2 = 0;
   METsig = 0;
   Dr01LepPt = 0;
   Dr01LepEta = 0;
   Dr01LepPhi = 0;
   Dr01LepE = 0;
   Dr01LepM = 0;
   Dr01LepId = 0;
   Dr01LepStatus = 0;
   Bare01LepPt = 0;
   Bare01LepEta = 0;
   Bare01LepPhi = 0;
   Bare01LepE = 0;
   Bare01LepM = 0;
   Bare01LepId = 0;
   Bare01LepStatus = 0;
   St03Pt = 0;
   St03Eta = 0;
   St03Phi = 0;
   St03E = 0;
   St03M = 0;
   St03Id = 0;
   St03Status = 0;
   St01PhotonPt = 0;
   St01PhotonEta = 0;
   St01PhotonPhi = 0;
   St01PhotonE = 0;
   St01PhotonM = 0;
   St01PhotonId = 0;
   St01PhotonMomId = 0;
   St01PhotonNumberMom = 0;
   St01PhotonStatus = 0;
   GjPt = 0;
   Gjeta = 0;
   Gjphi = 0;
   GjE = 0;
   GjPx = 0;
   GjPy = 0;
   GjPz = 0;
   GjChargedFraction = 0;
   matchGjet = 0;
   MGjPt = 0;
   MGjeta = 0;
   MGjphi = 0;
   MGjE = 0;
   patMuonPt_ = 0;
   patMuonEta_ = 0;
   patMuonPhi_ = 0;
   patMuonVtxZ_ = 0;
   patMuonEn_ = 0;
   patMuonCharge_ = 0;
   patMuonDxy_ = 0;
   patMuonCombId_ = 0;
   patMuonTrig_ = 0;
   patMuonDetIsoRho_ = 0;
   patMuonPfIsoDbeta_ = 0;
   patMuonM_ = 0;
   patMuonPx_ = 0;
   patMuonPy_ = 0;
   patMuonPz_ = 0;
   patMuonGlobalType_ = 0;
   patMuonTrackerType_ = 0;
   patMuonPFType_ = 0;
   patMuonIsoSumPt_ = 0;
   patMuonIsoRelative_ = 0;
   patMuonIsoCalComb_ = 0;
   patMuonIsoDY_ = 0;
   patMuonChi2Ndoff_ = 0;
   patMuonNhits_ = 0;
   patMuonNMatches_ = 0;
   patMuonDz_ = 0;
   patMuonPhits_ = 0;
   patMuonTkLayers_ = 0;
   patMuon_PF_IsoSumChargedHadronPt_ = 0;
   patMuon_PF_IsoSumNeutralHadronEt_ = 0;
   patMuon_PF_IsoDY_ = 0;
   patMuon_Mu17_Mu8_Matched_ = 0;
   patMuon_Mu17_TkMu8_Matched_ = 0;
   patElecdEtaIn_ = 0;
   patElecdPhiIn_ = 0;
   patElechOverE_ = 0;
   patElecsigmaIetaIeta_ = 0;
   patElecfull5x5_sigmaIetaIeta_ = 0;
   patElecooEmooP_ = 0;
   patElecd0_ = 0;
   patElecdz_ = 0;
   patElecexpectedMissingInnerHits_ = 0;
   patElecpassConversionVeto_ = 0;
   patElecTrig_ = 0;
   patElecDz_ = 0;
   patElecMVATrigId_ = 0;
   patElecMVANonTrigId_ = 0;
   patElecPt_ = 0;
   patElecEta_ = 0;
   patElecScEta_ = 0;
   patElecPhi_ = 0;
   patElecEnergy_ = 0;
   patElecCharge_ = 0;
   patElecMediumIDOff_ = 0;
   patElecMediumIDOff_Tom_ = 0;
   patElecchIso03_ = 0;
   patElecnhIso03_ = 0;
   patElecphIso03_ = 0;
   patElecpuChIso03_ = 0;
   patElecPfIso_ = 0;
   patElecPfIsodb_ = 0;
   patElecPfIsoRho_ = 0;
   neutral_ = 0;
   photon_ = 0;
   charged_ = 0;
   neutral_Tom_ = 0;
   photon_Tom_ = 0;
   charged_Tom_ = 0;
   patElec_mva_presel_ = 0;
   patJetPfAk05En_ = 0;
   patJetPfAk05Pt_ = 0;
   patJetPfAk05Eta_ = 0;
   patJetPfAk05Phi_ = 0;
   patJetPfAk05LooseId_ = 0;
   patJetPfAk05Et_ = 0;
   patJetPfAk05RawPt_ = 0;
   patJetPfAk05RawEn_ = 0;
   patJetPfAk05HadEHF_ = 0;
   patJetPfAk05EmEHF_ = 0;
   patJetPfAk05chf_ = 0;
   patJetPfAk05nhf_ = 0;
   patJetPfAk05cemf_ = 0;
   patJetPfAk05nemf_ = 0;
   patJetPfAk05cmult_ = 0;
   patJetPfAk05nconst_ = 0;
   patJetPfAk05jetBeta_ = 0;
   patJetPfAk05jetBetaClassic_ = 0;
   patJetPfAk05jetBetaStar_ = 0;
   patJetPfAk05jetBetaStarClassic_ = 0;
   patJetPfAk05jetpuMVA_ = 0;
   patJetPfAk05jetpukLoose_ = 0;
   patJetPfAk05jetpukMedium_ = 0;
   patJetPfAk05jetpukTight_ = 0;
   patJetPfAk05BDiscCSV_ = 0;
   patJetPfAk05BDiscCSVV1_ = 0;
   patJetPfAk05BDiscCSVSLV1_ = 0;
   unc_ = 0;
   patJetPfAk05PtUp_ = 0;
   patJetPfAk05PtDn_ = 0;
   caloJetPt_ = 0;
   caloJetRawPt_ = 0;
   caloJetEn_ = 0;
   caloJetEta_ = 0;
   caloJetPhi_ = 0;
   caloJetHadEHF_ = 0;
   caloJetEmEHF_ = 0;
   caloJetEmFrac_ = 0;
   caloJetn90_ = 0;
   id1_pdfInfo_ = 0;
   id2_pdfInfo_ = 0;
   x1_pdfInfo_ = 0;
   x2_pdfInfo_ = 0;
   scalePDF_pdfInfo_ = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("METPt", &METPt, &b_METPt);
   fChain->SetBranchAddress("METPx", &METPx, &b_METPx);
   fChain->SetBranchAddress("METPy", &METPy, &b_METPy);
   fChain->SetBranchAddress("METPz", &METPz, &b_METPz);
   fChain->SetBranchAddress("METE", &METE, &b_METE);
   fChain->SetBranchAddress("METsigx2", &METsigx2, &b_METsigx2);
   fChain->SetBranchAddress("METsigxy", &METsigxy, &b_METsigxy);
   fChain->SetBranchAddress("METsigy2", &METsigy2, &b_METsigy2);
   fChain->SetBranchAddress("METsig", &METsig, &b_METsig);
   fChain->SetBranchAddress("event", &event, &b_event);
   fChain->SetBranchAddress("realdata", &realdata, &b_realdata);
   fChain->SetBranchAddress("run", &run, &b_run);
   fChain->SetBranchAddress("lumi", &lumi, &b_lumi);
   fChain->SetBranchAddress("bxnumber", &bxnumber, &b_bxnumber);
   fChain->SetBranchAddress("EvtInfo_NumVtx", &EvtInfo_NumVtx, &b_EvtInfo_NumVtx);
   fChain->SetBranchAddress("PU_npT", &PU_npT, &b_PU_npT);
   fChain->SetBranchAddress("PU_npIT", &PU_npIT, &b_PU_npIT);
   fChain->SetBranchAddress("MyWeight", &MyWeight, &b_MyWeight);
   fChain->SetBranchAddress("Dr01LepPt", &Dr01LepPt, &b_Dr01LepPt);
   fChain->SetBranchAddress("Dr01LepEta", &Dr01LepEta, &b_Dr01LepEta);
   fChain->SetBranchAddress("Dr01LepPhi", &Dr01LepPhi, &b_Dr01LepPhi);
   fChain->SetBranchAddress("Dr01LepE", &Dr01LepE, &b_Dr01LepE);
   fChain->SetBranchAddress("Dr01LepM", &Dr01LepM, &b_Dr01LepM);
   fChain->SetBranchAddress("Dr01LepId", &Dr01LepId, &b_Dr01LepId);
   fChain->SetBranchAddress("Dr01LepStatus", &Dr01LepStatus, &b_Dr01LepStatus);
   fChain->SetBranchAddress("Bare01LepPt", &Bare01LepPt, &b_Bare01LepPt);
   fChain->SetBranchAddress("Bare01LepEta", &Bare01LepEta, &b_Bare01LepEta);
   fChain->SetBranchAddress("Bare01LepPhi", &Bare01LepPhi, &b_Bare01LepPhi);
   fChain->SetBranchAddress("Bare01LepE", &Bare01LepE, &b_Bare01LepE);
   fChain->SetBranchAddress("Bare01LepM", &Bare01LepM, &b_Bare01LepM);
   fChain->SetBranchAddress("Bare01LepId", &Bare01LepId, &b_Bare01LepId);
   fChain->SetBranchAddress("Bare01LepStatus", &Bare01LepStatus, &b_Bare01LepStatus);
   fChain->SetBranchAddress("St03Pt", &St03Pt, &b_St03Pt);
   fChain->SetBranchAddress("St03Eta", &St03Eta, &b_St03Eta);
   fChain->SetBranchAddress("St03Phi", &St03Phi, &b_St03Phi);
   fChain->SetBranchAddress("St03E", &St03E, &b_St03E);
   fChain->SetBranchAddress("St03M", &St03M, &b_St03M);
   fChain->SetBranchAddress("St03Id", &St03Id, &b_St03Id);
   fChain->SetBranchAddress("St03Status", &St03Status, &b_St03Status);
   fChain->SetBranchAddress("St01PhotonPt", &St01PhotonPt, &b_St01PhotonPt);
   fChain->SetBranchAddress("St01PhotonEta", &St01PhotonEta, &b_St01PhotonEta);
   fChain->SetBranchAddress("St01PhotonPhi", &St01PhotonPhi, &b_St01PhotonPhi);
   fChain->SetBranchAddress("St01PhotonE", &St01PhotonE, &b_St01PhotonE);
   fChain->SetBranchAddress("St01PhotonM", &St01PhotonM, &b_St01PhotonM);
   fChain->SetBranchAddress("St01PhotonId", &St01PhotonId, &b_St01PhotonId);
   fChain->SetBranchAddress("St01PhotonMomId", &St01PhotonMomId, &b_St01PhotonMomId);
   fChain->SetBranchAddress("St01PhotonNumberMom", &St01PhotonNumberMom, &b_St01PhotonNumberMom);
   fChain->SetBranchAddress("St01PhotonStatus", &St01PhotonStatus, &b_St01PhotonStatus);
   fChain->SetBranchAddress("GjPt", &GjPt, &b_GjPt);
   fChain->SetBranchAddress("Gjeta", &Gjeta, &b_Gjeta);
   fChain->SetBranchAddress("Gjphi", &Gjphi, &b_Gjphi);
   fChain->SetBranchAddress("GjE", &GjE, &b_GjE);
   fChain->SetBranchAddress("GjPx", &GjPx, &b_GjPx);
   fChain->SetBranchAddress("GjPy", &GjPy, &b_GjPy);
   fChain->SetBranchAddress("GjPz", &GjPz, &b_GjPz);
   fChain->SetBranchAddress("GjChargedFraction", &GjChargedFraction, &b_GjChargedFraction);
   fChain->SetBranchAddress("matchGjet", &matchGjet, &b_matchGjet);
   fChain->SetBranchAddress("MGjPt", &MGjPt, &b_MGjPt);
   fChain->SetBranchAddress("MGjeta", &MGjeta, &b_MGjeta);
   fChain->SetBranchAddress("MGjphi", &MGjphi, &b_MGjphi);
   fChain->SetBranchAddress("MGjE", &MGjE, &b_MGjE);
   fChain->SetBranchAddress("HLT_Mu17_Mu8", &HLT_Mu17_Mu8, &b_HLT_Mu17_Mu8);
   fChain->SetBranchAddress("HLT_Mu17_TkMu8", &HLT_Mu17_TkMu8, &b_HLT_Mu17_TkMu8);
   fChain->SetBranchAddress("HLT_Elec17_Elec8", &HLT_Elec17_Elec8, &b_HLT_Elec17_Elec8);
   fChain->SetBranchAddress("patMuonPt_", &patMuonPt_, &b_patMuonPt_);
   fChain->SetBranchAddress("patMuonEta_", &patMuonEta_, &b_patMuonEta_);
   fChain->SetBranchAddress("patMuonPhi_", &patMuonPhi_, &b_patMuonPhi_);
   fChain->SetBranchAddress("patMuonVtxZ_", &patMuonVtxZ_, &b_patMuonVtxZ_);
   fChain->SetBranchAddress("patMuonEn_", &patMuonEn_, &b_patMuonEn_);
   fChain->SetBranchAddress("patMuonCharge_", &patMuonCharge_, &b_patMuonCharge_);
   fChain->SetBranchAddress("patMuonDxy_", &patMuonDxy_, &b_patMuonDxy_);
   fChain->SetBranchAddress("patMuonCombId_", &patMuonCombId_, &b_patMuonCombId_);
   fChain->SetBranchAddress("patMuonTrig_", &patMuonTrig_, &b_patMuonTrig_);
   fChain->SetBranchAddress("patMuonDetIsoRho_", &patMuonDetIsoRho_, &b_patMuonDetIsoRho_);
   fChain->SetBranchAddress("patMuonPfIsoDbeta_", &patMuonPfIsoDbeta_, &b_patMuonPfIsoDbeta_);
   fChain->SetBranchAddress("patMuonM_", &patMuonM_, &b_patMuonM_);
   fChain->SetBranchAddress("patMuonPx_", &patMuonPx_, &b_patMuonPx_);
   fChain->SetBranchAddress("patMuonPy_", &patMuonPy_, &b_patMuonPy_);
   fChain->SetBranchAddress("patMuonPz_", &patMuonPz_, &b_patMuonPz_);
   fChain->SetBranchAddress("patMuonGlobalType_", &patMuonGlobalType_, &b_patMuonGlobalType_);
   fChain->SetBranchAddress("patMuonTrackerType_", &patMuonTrackerType_, &b_patMuonTrackerType_);
   fChain->SetBranchAddress("patMuonPFType_", &patMuonPFType_, &b_patMuonPFType_);
   fChain->SetBranchAddress("patMuonIsoSumPt_", &patMuonIsoSumPt_, &b_patMuonIsoSumPt_);
   fChain->SetBranchAddress("patMuonIsoRelative_", &patMuonIsoRelative_, &b_patMuonIsoRelative_);
   fChain->SetBranchAddress("patMuonIsoCalComb_", &patMuonIsoCalComb_, &b_patMuonIsoCalComb_);
   fChain->SetBranchAddress("patMuonIsoDY_", &patMuonIsoDY_, &b_patMuonIsoDY_);
   fChain->SetBranchAddress("patMuonChi2Ndoff_", &patMuonChi2Ndoff_, &b_patMuonChi2Ndoff_);
   fChain->SetBranchAddress("patMuonNhits_", &patMuonNhits_, &b_patMuonNhits_);
   fChain->SetBranchAddress("patMuonNMatches_", &patMuonNMatches_, &b_patMuonNMatches_);
   fChain->SetBranchAddress("patMuonDz_", &patMuonDz_, &b_patMuonDz_);
   fChain->SetBranchAddress("patMuonPhits_", &patMuonPhits_, &b_patMuonPhits_);
   fChain->SetBranchAddress("patMuonTkLayers_", &patMuonTkLayers_, &b_patMuonTkLayers_);
   fChain->SetBranchAddress("patMuon_PF_IsoSumChargedHadronPt_", &patMuon_PF_IsoSumChargedHadronPt_, &b_patMuon_PF_IsoSumChargedHadronPt_);
   fChain->SetBranchAddress("patMuon_PF_IsoSumNeutralHadronEt_", &patMuon_PF_IsoSumNeutralHadronEt_, &b_patMuon_PF_IsoSumNeutralHadronEt_);
   fChain->SetBranchAddress("patMuon_PF_IsoDY_", &patMuon_PF_IsoDY_, &b_patMuon_PF_IsoDY_);
   fChain->SetBranchAddress("patMuon_Mu17_Mu8_Matched_", &patMuon_Mu17_Mu8_Matched_, &b_patMuon_Mu17_Mu8_Matched_);
   fChain->SetBranchAddress("patMuon_Mu17_TkMu8_Matched_", &patMuon_Mu17_TkMu8_Matched_, &b_patMuon_Mu17_TkMu8_Matched_);
   fChain->SetBranchAddress("patElecdEtaIn_", &patElecdEtaIn_, &b_patElecdEtaIn_);
   fChain->SetBranchAddress("patElecdPhiIn_", &patElecdPhiIn_, &b_patElecdPhiIn_);
   fChain->SetBranchAddress("patElechOverE_", &patElechOverE_, &b_patElechOverE_);
   fChain->SetBranchAddress("patElecsigmaIetaIeta_", &patElecsigmaIetaIeta_, &b_patElecsigmaIetaIeta_);
   fChain->SetBranchAddress("patElecfull5x5_sigmaIetaIeta_", &patElecfull5x5_sigmaIetaIeta_, &b_patElecfull5x5_sigmaIetaIeta_);
   fChain->SetBranchAddress("patElecooEmooP_", &patElecooEmooP_, &b_patElecooEmooP_);
   fChain->SetBranchAddress("patElecd0_", &patElecd0_, &b_patElecd0_);
   fChain->SetBranchAddress("patElecdz_", &patElecdz_, &b_patElecdz_);
   fChain->SetBranchAddress("patElecexpectedMissingInnerHits_", &patElecexpectedMissingInnerHits_, &b_patElecexpectedMissingInnerHits_);
   fChain->SetBranchAddress("patElecpassConversionVeto_", &patElecpassConversionVeto_, &b_patElecpassConversionVeto_);
   fChain->SetBranchAddress("patElecTrig_", &patElecTrig_, &b_patElecTrig_);
   fChain->SetBranchAddress("patElecDz_", &patElecDz_, &b_patElecDz_);
   fChain->SetBranchAddress("patElecMVATrigId_", &patElecMVATrigId_, &b_patElecMVATrigId_);
   fChain->SetBranchAddress("patElecMVANonTrigId_", &patElecMVANonTrigId_, &b_patElecMVANonTrigId_);
   fChain->SetBranchAddress("patElecPt_", &patElecPt_, &b_patElecPt_);
   fChain->SetBranchAddress("patElecEta_", &patElecEta_, &b_patElecEta_);
   fChain->SetBranchAddress("patElecScEta_", &patElecScEta_, &b_patElecScEta_);
   fChain->SetBranchAddress("patElecPhi_", &patElecPhi_, &b_patElecPhi_);
   fChain->SetBranchAddress("patElecEnergy_", &patElecEnergy_, &b_patElecEnergy_);
   fChain->SetBranchAddress("patElecCharge_", &patElecCharge_, &b_patElecCharge_);
   fChain->SetBranchAddress("patElecMediumIDOff_", &patElecMediumIDOff_, &b_patElecMediumIDOff_);
   fChain->SetBranchAddress("patElecMediumIDOff_Tom_", &patElecMediumIDOff_Tom_, &b_patElecMediumIDOff_Tom_);
   fChain->SetBranchAddress("patElecchIso03_", &patElecchIso03_, &b_patElecchIso03_);
   fChain->SetBranchAddress("patElecnhIso03_", &patElecnhIso03_, &b_patElecnhIso03_);
   fChain->SetBranchAddress("patElecphIso03_", &patElecphIso03_, &b_patElecphIso03_);
   fChain->SetBranchAddress("patElecpuChIso03_", &patElecpuChIso03_, &b_patElecpuChIso03_);
   fChain->SetBranchAddress("patElecPfIso_", &patElecPfIso_, &b_patElecPfIso_);
   fChain->SetBranchAddress("patElecPfIsodb_", &patElecPfIsodb_, &b_patElecPfIsodb_);
   fChain->SetBranchAddress("patElecPfIsoRho_", &patElecPfIsoRho_, &b_patElecPfIsoRho_);
   fChain->SetBranchAddress("rhoPrime", &rhoPrime, &b_rhoPrime);
   fChain->SetBranchAddress("neutral_", &neutral_, &b_neutral_);
   fChain->SetBranchAddress("photon_", &photon_, &b_photon_);
   fChain->SetBranchAddress("charged_", &charged_, &b_charged_);
   fChain->SetBranchAddress("neutral_Tom_", &neutral_Tom_, &b_neutral_Tom_);
   fChain->SetBranchAddress("photon_Tom_", &photon_Tom_, &b_photon_Tom_);
   fChain->SetBranchAddress("charged_Tom_", &charged_Tom_, &b_charged_Tom_);
   fChain->SetBranchAddress("AEff", &AEff, &b_AEff);
   fChain->SetBranchAddress("patElec_mva_presel_", &patElec_mva_presel_, &b_patElec_mva_presel_);
   fChain->SetBranchAddress("patJetPfAk05En_", &patJetPfAk05En_, &b_patJetPfAk05En_);
   fChain->SetBranchAddress("patJetPfAk05Pt_", &patJetPfAk05Pt_, &b_patJetPfAk05Pt_);
   fChain->SetBranchAddress("patJetPfAk05Eta_", &patJetPfAk05Eta_, &b_patJetPfAk05Eta_);
   fChain->SetBranchAddress("patJetPfAk05Phi_", &patJetPfAk05Phi_, &b_patJetPfAk05Phi_);
   fChain->SetBranchAddress("patJetPfAk05LooseId_", &patJetPfAk05LooseId_, &b_patJetPfAk05LooseId_);
   fChain->SetBranchAddress("patJetPfAk05Et_", &patJetPfAk05Et_, &b_patJetPfAk05Et_);
   fChain->SetBranchAddress("patJetPfAk05RawPt_", &patJetPfAk05RawPt_, &b_patJetPfAk05RawPt_);
   fChain->SetBranchAddress("patJetPfAk05RawEn_", &patJetPfAk05RawEn_, &b_patJetPfAk05RawEn_);
   fChain->SetBranchAddress("patJetPfAk05HadEHF_", &patJetPfAk05HadEHF_, &b_patJetPfAk05HadEHF_);
   fChain->SetBranchAddress("patJetPfAk05EmEHF_", &patJetPfAk05EmEHF_, &b_patJetPfAk05EmEHF_);
   fChain->SetBranchAddress("patJetPfAk05chf_", &patJetPfAk05chf_, &b_patJetPfAk05chf_);
   fChain->SetBranchAddress("patJetPfAk05nhf_", &patJetPfAk05nhf_, &b_patJetPfAk05nhf_);
   fChain->SetBranchAddress("patJetPfAk05cemf_", &patJetPfAk05cemf_, &b_patJetPfAk05cemf_);
   fChain->SetBranchAddress("patJetPfAk05nemf_", &patJetPfAk05nemf_, &b_patJetPfAk05nemf_);
   fChain->SetBranchAddress("patJetPfAk05cmult_", &patJetPfAk05cmult_, &b_patJetPfAk05cmult_);
   fChain->SetBranchAddress("patJetPfAk05nconst_", &patJetPfAk05nconst_, &b_patJetPfAk05nconst_);
   fChain->SetBranchAddress("patJetPfAk05jetBeta_", &patJetPfAk05jetBeta_, &b_patJetPfAk05jetBeta_);
   fChain->SetBranchAddress("patJetPfAk05jetBetaClassic_", &patJetPfAk05jetBetaClassic_, &b_patJetPfAk05jetBetaClassic_);
   fChain->SetBranchAddress("patJetPfAk05jetBetaStar_", &patJetPfAk05jetBetaStar_, &b_patJetPfAk05jetBetaStar_);
   fChain->SetBranchAddress("patJetPfAk05jetBetaStarClassic_", &patJetPfAk05jetBetaStarClassic_, &b_patJetPfAk05jetBetaStarClassic_);
   fChain->SetBranchAddress("patJetPfAk05jetpuMVA_", &patJetPfAk05jetpuMVA_, &b_patJetPfAk05jetpuMVA_);
   fChain->SetBranchAddress("patJetPfAk05jetpukLoose_", &patJetPfAk05jetpukLoose_, &b_patJetPfAk05jetpukLoose_);
   fChain->SetBranchAddress("patJetPfAk05jetpukMedium_", &patJetPfAk05jetpukMedium_, &b_patJetPfAk05jetpukMedium_);
   fChain->SetBranchAddress("patJetPfAk05jetpukTight_", &patJetPfAk05jetpukTight_, &b_patJetPfAk05jetpukTight_);
   fChain->SetBranchAddress("patJetPfAk05BDiscCSV_", &patJetPfAk05BDiscCSV_, &b_patJetPfAk05BDiscCSV_);
   fChain->SetBranchAddress("patJetPfAk05BDiscCSVV1_", &patJetPfAk05BDiscCSVV1_, &b_patJetPfAk05BDiscCSVV1_);
   fChain->SetBranchAddress("patJetPfAk05BDiscCSVSLV1_", &patJetPfAk05BDiscCSVSLV1_, &b_patJetPfAk05BDiscCSVSLV1_);
   fChain->SetBranchAddress("unc_", &unc_, &b_unc_);
   fChain->SetBranchAddress("patJetPfAk05PtUp_", &patJetPfAk05PtUp_, &b_patJetPfAk05PtUp_);
   fChain->SetBranchAddress("patJetPfAk05PtDn_", &patJetPfAk05PtDn_, &b_patJetPfAk05PtDn_);
   fChain->SetBranchAddress("caloJetPt_", &caloJetPt_, &b_caloJetPt_);
   fChain->SetBranchAddress("caloJetRawPt_", &caloJetRawPt_, &b_caloJetRawPt_);
   fChain->SetBranchAddress("caloJetEn_", &caloJetEn_, &b_caloJetEn_);
   fChain->SetBranchAddress("caloJetEta_", &caloJetEta_, &b_caloJetEta_);
   fChain->SetBranchAddress("caloJetPhi_", &caloJetPhi_, &b_caloJetPhi_);
   fChain->SetBranchAddress("caloJetHadEHF_", &caloJetHadEHF_, &b_caloJetHadEHF_);
   fChain->SetBranchAddress("caloJetEmEHF_", &caloJetEmEHF_, &b_caloJetEmEHF_);
   fChain->SetBranchAddress("caloJetEmFrac_", &caloJetEmFrac_, &b_caloJetEmFrac_);
   fChain->SetBranchAddress("caloJetn90_", &caloJetn90_, &b_caloJetn90_);
   fChain->SetBranchAddress("id1_pdfInfo_", &id1_pdfInfo_, &b_id1_pdfInfo_);
   fChain->SetBranchAddress("id2_pdfInfo_", &id2_pdfInfo_, &b_id2_pdfInfo_);
   fChain->SetBranchAddress("x1_pdfInfo_", &x1_pdfInfo_, &b_x1_pdfInfo_);
   fChain->SetBranchAddress("x2_pdfInfo_", &x2_pdfInfo_, &b_x2_pdfInfo_);
   fChain->SetBranchAddress("scalePDF_pdfInfo_", &scalePDF_pdfInfo_, &b_scalePDF_pdfInfo_);
   fChain->SetBranchAddress("ptHat_", &ptHat_, &b_ptHat_);
   fChain->SetBranchAddress("mcWeight_", &mcWeight_, &b_mcWeight_);
   fChain->SetBranchAddress("nup", &nup, &b_nup);
   Notify();
}

Bool_t test::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void test::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t test::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef test_cxx
