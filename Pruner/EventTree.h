//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Aug  5 22:59:33 2015 by ROOT version 5.34/26
// from TTree EventTree/ EventTree
// found on file: ntuple_10.root
//////////////////////////////////////////////////////////

#ifndef EventTree_h
#define EventTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <vector>
#include <vector>
#include <vector>
#include <vector>

// Fixed size dimensions of array or collections stored in the TTree if any.

class EventTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           EvtIsRealData;
   UInt_t          EvtNum;
   UInt_t          EvtRunNum;
   Int_t           EvtLumiNum;
   Int_t           EvtBxNum;
   Int_t           EvtVtxCnt;
   Int_t           EvtPuCnt;
   Int_t           EvtPuCntTruth;
   vector<double>  *EvtWeights;
   Float_t         EvtFastJetRho;
   UInt_t          TrigHlt;
   ULong64_t       TrigHltPhot;
   ULong64_t       TrigHltMu;
   ULong64_t       TrigHltDiMu;
   vector<float>   *METPt;
   vector<float>   *METPx;
   vector<float>   *METPy;
   vector<float>   *METPz;
   vector<float>   *METE;
   vector<float>   *METsigx2;
   vector<float>   *METsigxy;
   vector<float>   *METsigy2;
   vector<float>   *METsig;
   vector<float>   *GLepDr01Pt;
   vector<float>   *GLepDr01Eta;
   vector<float>   *GLepDr01Phi;
   vector<float>   *GLepDr01E;
   vector<unsigned int> *GLepDr01Id;
   vector<int>     *GLepDr01St;
   vector<int>     *GLepDr01MomId;
   vector<float>   *GLepBarePt;
   vector<float>   *GLepBareEta;
   vector<float>   *GLepBarePhi;
   vector<float>   *GLepBareE;
   vector<unsigned int> *GLepBareId;
   vector<int>     *GLepBareSt;
   vector<int>     *GLepBareMomId;
   vector<float>   *GLepSt3Pt;
   vector<float>   *GLepSt3Eta;
   vector<float>   *GLepSt3Phi;
   vector<float>   *GLepSt3E;
   vector<int>     *GLepSt3Id;
   vector<int>     *GLepSt3St;
   vector<int>     *GLepSt3Mother0Id;
   vector<int>     *GLepSt3MotherCnt;
   vector<float>   *GLepClosePhotPt;
   vector<float>   *GLepClosePhotEta;
   vector<float>   *GLepClosePhotPhi;
   vector<float>   *GLepClosePhotE;
   vector<int>     *GLepClosePhotId;
   vector<int>     *GLepClosePhotMother0Id;
   vector<int>     *GLepClosePhotMotherCnt;
   vector<int>     *GLepClosePhotSt;
   vector<float>   *GJetAk04Pt;
   vector<float>   *GJetAk04Eta;
   vector<float>   *GJetAk04Phi;
   vector<float>   *GJetAk04E;
   vector<float>   *GJetAk04ChFrac;
   vector<int>     *GJetAk04ConstCnt;
   vector<int>     *GJetAk04ConstId;
   vector<float>   *GJetAk04ConstPt;
   vector<float>   *GJetAk04ConstEta;
   vector<float>   *GJetAk04ConstPhi;
   vector<float>   *GJetAk04ConstE;
   vector<int>     *GPdfId1;
   vector<int>     *GPdfId2;
   vector<float>   *GPdfx1;
   vector<float>   *GPdfx2;
   vector<float>   *GPdfScale;
   Float_t         GBinningValue;
   Int_t           GNup;
   vector<float>   *MuPt;
   vector<float>   *MuEta;
   vector<float>   *MuPhi;
   vector<float>   *MuE;
   vector<unsigned int> *MuId;
   vector<unsigned int> *MuIdTight;
   vector<float>   *MuCh;
   vector<float>   *MuVtxZ;
   vector<float>   *MuDxy;
   vector<float>   *MuIsoRho;
   vector<float>   *MuPfIso;
   vector<float>   *MuType;
   vector<float>   *MuIsoTkIsoAbs;
   vector<float>   *MuIsoTkIsoRel;
   vector<float>   *MuIsoCalAbs;
   vector<float>   *MuIsoCombRel;
   vector<float>   *MuTkNormChi2;
   vector<int>     *MuTkHitCnt;
   vector<int>     *MuMatchedStationCnt;
   vector<float>   *MuDz;
   vector<int>     *MuPixelHitCnt;
   vector<int>     *MuTkLayerCnt;
   vector<float>   *MuPfIsoChHad;
   vector<float>   *MuPfIsoNeutralHad;
   vector<float>   *MuPfIsoRawRel;
   vector<unsigned int> *MuHltMatch;
   vector<float>   *ElPt;
   vector<float>   *ElEta;
   vector<float>   *ElEtaSc;
   vector<float>   *ElPhi;
   vector<float>   *ElE;
   vector<unsigned int> *ElId;
   vector<float>   *ElCh;
   vector<float>   *ElMvaTrig;
   vector<float>   *ElMvaNonTrig;
   vector<float>   *ElMvaPresel;
   vector<float>   *ElDEtaTkScAtVtx;
   vector<float>   *ElDPhiTkScAtVtx;
   vector<float>   *ElHoE;
   vector<float>   *ElSigmaIetaIeta;
   vector<float>   *ElSigmaIetaIetaFull5x5;
   vector<float>   *ElEinvMinusPinv;
   vector<float>   *ElD0;
   vector<float>   *ElDz;
   vector<int>     *ElExpectedMissingInnerHitCnt;
   vector<int>     *ElPassConvVeto;
   vector<unsigned int> *ElHltMatch;
   vector<float>   *ElPfIsoChHad;
   vector<float>   *ElPfIsoNeutralHad;
   vector<float>   *ElPfIsoIso;
   vector<float>   *ElPfIsoPuChHad;
   vector<float>   *ElPfIsoRaw;
   vector<float>   *ElPfIsoDbeta;
   vector<float>   *ElPfIsoRho;
   vector<float>   *ElAEff;
   vector<float>   *charged;
   vector<float>   *photon;
   vector<float>   *neutral;
   vector<float>   *charged_Tom;
   vector<float>   *photon_Tom;
   vector<float>   *neutral_Tom;
   vector<float>   *PhotPt;
   vector<float>   *PhotEta;
   vector<float>   *PhotPhi;
   vector<float>   *PhotScRawE;
   vector<float>   *PhotScEta;
   vector<float>   *PhotScPhi;
   vector<float>   *PhotIsoEcal;
   vector<float>   *PhotIsoHcal;
   vector<float>   *PhotIsoTk;
   vector<float>   *PhotPfIsoChHad;
   vector<float>   *PhotPfIsoNeutralHad;
   vector<float>   *PhotPfIsoPhot;
   vector<float>   *PhotPfIsoPuChHad;
   vector<float>   *PhotPfIsoEcalClus;
   vector<float>   *PhotPfIsoHcalClus;
   vector<float>   *PhotE3x3;
   vector<float>   *PhotE1x5;
   vector<float>   *PhotE2x5;
   vector<float>   *PhotE5x5;
   vector<float>   *PhotSigmaIetaIeta;
   vector<float>   *PhotEtaWidth;
   vector<float>   *PhotPhiWidth;
   vector<float>   *PhotHoE;
   vector<unsigned int> *PhotId;
   vector<bool>    *PhotHasPixelSeed;
   vector<float>   *JetAk04Pt;
   vector<float>   *JetAk04Eta;
   vector<float>   *JetAk04Phi;
   vector<float>   *JetAk04E;
   vector<float>   *JetAk04Id;
   vector<bool>    *JetAk04PuId;
   vector<float>   *JetAk04PuMva;
   vector<float>   *JetAk04RawPt;
   vector<float>   *JetAk04RawE;
   vector<float>   *JetAk04HfHadE;
   vector<float>   *JetAk04HfEmE;
   vector<float>   *JetAk04ChHadFrac;
   vector<float>   *JetAk04NeutralHadAndHfFrac;
   vector<float>   *JetAk04ChEmFrac;
   vector<float>   *JetAk04NeutralEmFrac;
   vector<float>   *JetAk04ChMult;
   vector<float>   *JetAk04ConstCnt;
   vector<float>   *JetAk04JetBeta;
   vector<float>   *JetAk04JetBetaClassic;
   vector<float>   *JetAk04JetBetaStar;
   vector<float>   *JetAk04JetBetaStarClassic;
   vector<float>   *JetAk04BTagCsv;
   vector<float>   *JetAk04BTagCsvV1;
   vector<float>   *JetAk04BTagCsvSLV1;
   vector<float>   *JetAk04BDiscCisvV2;
   vector<float>   *JetAk04BDiscJp;
   vector<float>   *JetAk04BDiscBjp;
   vector<float>   *JetAk04BDiscTche;
   vector<float>   *JetAk04BDiscTchp;
   vector<float>   *JetAk04BDiscSsvhe;
   vector<float>   *JetAk04BDiscSsvhp;
   vector<float>   *JetAk04PartFlav;
   vector<float>   *JetAk04JecUncUp;
   vector<float>   *JetAk04JecUncDwn;
   vector<int>     *JetAk04ConstId;
   vector<float>   *JetAk04ConstPt;
   vector<float>   *JetAk04ConstEta;
   vector<float>   *JetAk04ConstPhi;
   vector<float>   *JetAk04ConstE;
   vector<int>     *JetAk04GenJet;

   // List of branches
   TBranch        *b_EvtIsRealData;   //!
   TBranch        *b_EvtNum;   //!
   TBranch        *b_EvtRunNum;   //!
   TBranch        *b_EvtLumiNum;   //!
   TBranch        *b_EvtBxNum;   //!
   TBranch        *b_EvtVtxCnt;   //!
   TBranch        *b_EvtPuCnt;   //!
   TBranch        *b_EvtPuCntTruth;   //!
   TBranch        *b_EvtWeights;   //!
   TBranch        *b_EvtFastJetRho;   //!
   TBranch        *b_TrigHlt;   //!
   TBranch        *b_TrigHltPhot;   //!
   TBranch        *b_TrigHltMu;   //!
   TBranch        *b_TrigHltDiMu;   //!
   TBranch        *b_METPt;   //!
   TBranch        *b_METPx;   //!
   TBranch        *b_METPy;   //!
   TBranch        *b_METPz;   //!
   TBranch        *b_METE;   //!
   TBranch        *b_METsigx2;   //!
   TBranch        *b_METsigxy;   //!
   TBranch        *b_METsigy2;   //!
   TBranch        *b_METsig;   //!
   TBranch        *b_GLepDr01Pt;   //!
   TBranch        *b_GLepDr01Eta;   //!
   TBranch        *b_GLepDr01Phi;   //!
   TBranch        *b_GLepDr01E;   //!
   TBranch        *b_GLepDr01Id;   //!
   TBranch        *b_GLepDr01St;   //!
   TBranch        *b_GLepDr01MomId;   //!
   TBranch        *b_GLepBarePt;   //!
   TBranch        *b_GLepBareEta;   //!
   TBranch        *b_GLepBarePhi;   //!
   TBranch        *b_GLepBareE;   //!
   TBranch        *b_GLepBareId;   //!
   TBranch        *b_GLepBareSt;   //!
   TBranch        *b_GLepBareMomId;   //!
   TBranch        *b_GLepSt3Pt;   //!
   TBranch        *b_GLepSt3Eta;   //!
   TBranch        *b_GLepSt3Phi;   //!
   TBranch        *b_GLepSt3E;   //!
   TBranch        *b_GLepSt3Id;   //!
   TBranch        *b_GLepSt3St;   //!
   TBranch        *b_GLepSt3Mother0Id;   //!
   TBranch        *b_GLepSt3MotherCnt;   //!
   TBranch        *b_GLepClosePhotPt;   //!
   TBranch        *b_GLepClosePhotEta;   //!
   TBranch        *b_GLepClosePhotPhi;   //!
   TBranch        *b_GLepClosePhotE;   //!
   TBranch        *b_GLepClosePhotId;   //!
   TBranch        *b_GLepClosePhotMother0Id;   //!
   TBranch        *b_GLepClosePhotMotherCnt;   //!
   TBranch        *b_GLepClosePhotSt;   //!
   TBranch        *b_GJetAk04Pt;   //!
   TBranch        *b_GJetAk04Eta;   //!
   TBranch        *b_GJetAk04Phi;   //!
   TBranch        *b_GJetAk04E;   //!
   TBranch        *b_GJetAk04ChFrac;   //!
   TBranch        *b_GJetAk04ConstCnt;   //!
   TBranch        *b_GJetAk04ConstId;   //!
   TBranch        *b_GJetAk04ConstPt;   //!
   TBranch        *b_GJetAk04ConstEta;   //!
   TBranch        *b_GJetAk04ConstPhi;   //!
   TBranch        *b_GJetAk04ConstE;   //!
   TBranch        *b_GPdfId1;   //!
   TBranch        *b_GPdfId2;   //!
   TBranch        *b_GPdfx1;   //!
   TBranch        *b_GPdfx2;   //!
   TBranch        *b_GPdfScale;   //!
   TBranch        *b_GBinningValue;   //!
   TBranch        *b_GNup;   //!
   TBranch        *b_MuPt;   //!
   TBranch        *b_MuEta;   //!
   TBranch        *b_MuPhi;   //!
   TBranch        *b_MuE;   //!
   TBranch        *b_MuId;   //!
   TBranch        *b_MuIdTight;   //!
   TBranch        *b_MuCh;   //!
   TBranch        *b_MuVtxZ;   //!
   TBranch        *b_MuDxy;   //!
   TBranch        *b_MuIsoRho;   //!
   TBranch        *b_MuPfIso;   //!
   TBranch        *b_MuType;   //!
   TBranch        *b_MuIsoTkIsoAbs;   //!
   TBranch        *b_MuIsoTkIsoRel;   //!
   TBranch        *b_MuIsoCalAbs;   //!
   TBranch        *b_MuIsoCombRel;   //!
   TBranch        *b_MuTkNormChi2;   //!
   TBranch        *b_MuTkHitCnt;   //!
   TBranch        *b_MuMatchedStationCnt;   //!
   TBranch        *b_MuDz;   //!
   TBranch        *b_MuPixelHitCnt;   //!
   TBranch        *b_MuTkLayerCnt;   //!
   TBranch        *b_MuPfIsoChHad;   //!
   TBranch        *b_MuPfIsoNeutralHad;   //!
   TBranch        *b_MuPfIsoRawRel;   //!
   TBranch        *b_MuHltMatch;   //!
   TBranch        *b_ElPt;   //!
   TBranch        *b_ElEta;   //!
   TBranch        *b_ElEtaSc;   //!
   TBranch        *b_ElPhi;   //!
   TBranch        *b_ElE;   //!
   TBranch        *b_ElId;   //!
   TBranch        *b_ElCh;   //!
   TBranch        *b_ElMvaTrig;   //!
   TBranch        *b_ElMvaNonTrig;   //!
   TBranch        *b_ElMvaPresel;   //!
   TBranch        *b_ElDEtaTkScAtVtx;   //!
   TBranch        *b_ElDPhiTkScAtVtx;   //!
   TBranch        *b_ElHoE;   //!
   TBranch        *b_ElSigmaIetaIeta;   //!
   TBranch        *b_ElSigmaIetaIetaFull5x5;   //!
   TBranch        *b_ElEinvMinusPinv;   //!
   TBranch        *b_ElD0;   //!
   TBranch        *b_ElDz;   //!
   TBranch        *b_ElExpectedMissingInnerHitCnt;   //!
   TBranch        *b_ElPassConvVeto;   //!
   TBranch        *b_ElHltMatch;   //!
   TBranch        *b_ElPfIsoChHad;   //!
   TBranch        *b_ElPfIsoNeutralHad;   //!
   TBranch        *b_ElPfIsoIso;   //!
   TBranch        *b_ElPfIsoPuChHad;   //!
   TBranch        *b_ElPfIsoRaw;   //!
   TBranch        *b_ElPfIsoDbeta;   //!
   TBranch        *b_ElPfIsoRho;   //!
   TBranch        *b_ElAEff;   //!
   TBranch        *b_charged;   //!
   TBranch        *b_photon;   //!
   TBranch        *b_neutral;   //!
   TBranch        *b_charged_Tom;   //!
   TBranch        *b_photon_Tom;   //!
   TBranch        *b_neutral_Tom;   //!
   TBranch        *b_PhotPt;   //!
   TBranch        *b_PhotEta;   //!
   TBranch        *b_PhotPhi;   //!
   TBranch        *b_PhotScRawE;   //!
   TBranch        *b_PhotScEta;   //!
   TBranch        *b_PhotScPhi;   //!
   TBranch        *b_PhotIsoEcal;   //!
   TBranch        *b_PhotIsoHcal;   //!
   TBranch        *b_PhotIsoTk;   //!
   TBranch        *b_PhotPfIsoChHad;   //!
   TBranch        *b_PhotPfIsoNeutralHad;   //!
   TBranch        *b_PhotPfIsoPhot;   //!
   TBranch        *b_PhotPfIsoPuChHad;   //!
   TBranch        *b_PhotPfIsoEcalClus;   //!
   TBranch        *b_PhotPfIsoHcalClus;   //!
   TBranch        *b_PhotE3x3;   //!
   TBranch        *b_PhotE1x5;   //!
   TBranch        *b_PhotE2x5;   //!
   TBranch        *b_PhotE5x5;   //!
   TBranch        *b_PhotSigmaIetaIeta;   //!
   TBranch        *b_PhotEtaWidth;   //!
   TBranch        *b_PhotPhiWidth;   //!
   TBranch        *b_PhotHoE;   //!
   TBranch        *b_PhotId;   //!
   TBranch        *b_PhotHasPixelSeed;   //!
   TBranch        *b_JetAk04Pt;   //!
   TBranch        *b_JetAk04Eta;   //!
   TBranch        *b_JetAk04Phi;   //!
   TBranch        *b_JetAk04E;   //!
   TBranch        *b_JetAk04Id;   //!
   TBranch        *b_JetAk04PuId;   //!
   TBranch        *b_JetAk04PuMva;   //!
   TBranch        *b_JetAk04RawPt;   //!
   TBranch        *b_JetAk04RawE;   //!
   TBranch        *b_JetAk04HfHadE;   //!
   TBranch        *b_JetAk04HfEmE;   //!
   TBranch        *b_JetAk04ChHadFrac;   //!
   TBranch        *b_JetAk04NeutralHadAndHfFrac;   //!
   TBranch        *b_JetAk04ChEmFrac;   //!
   TBranch        *b_JetAk04NeutralEmFrac;   //!
   TBranch        *b_JetAk04ChMult;   //!
   TBranch        *b_JetAk04ConstCnt;   //!
   TBranch        *b_JetAk04JetBeta;   //!
   TBranch        *b_JetAk04JetBetaClassic;   //!
   TBranch        *b_JetAk04JetBetaStar;   //!
   TBranch        *b_JetAk04JetBetaStarClassic;   //!
   TBranch        *b_JetAk04BTagCsv;   //!
   TBranch        *b_JetAk04BTagCsvV1;   //!
   TBranch        *b_JetAk04BTagCsvSLV1;   //!
   TBranch        *b_JetAk04BDiscCisvV2;   //!
   TBranch        *b_JetAk04BDiscJp;   //!
   TBranch        *b_JetAk04BDiscBjp;   //!
   TBranch        *b_JetAk04BDiscTche;   //!
   TBranch        *b_JetAk04BDiscTchp;   //!
   TBranch        *b_JetAk04BDiscSsvhe;   //!
   TBranch        *b_JetAk04BDiscSsvhp;   //!
   TBranch        *b_JetAk04PartFlav;   //!
   TBranch        *b_JetAk04JecUncUp;   //!
   TBranch        *b_JetAk04JecUncDwn;   //!
   TBranch        *b_JetAk04ConstId;   //!
   TBranch        *b_JetAk04ConstPt;   //!
   TBranch        *b_JetAk04ConstEta;   //!
   TBranch        *b_JetAk04ConstPhi;   //!
   TBranch        *b_JetAk04ConstE;   //!
   TBranch        *b_JetAk04GenJet;   //!

   EventTree(TTree *tree=0);
   virtual ~EventTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef EventTree_cxx
EventTree::EventTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("ntuple_10.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("ntuple_10.root");
      }
      TDirectory * dir = (TDirectory*)f->Get("ntuple_10.root:/tupel");
      dir->GetObject("EventTree",tree);

   }
   Init(tree);
}

EventTree::~EventTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t EventTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t EventTree::LoadTree(Long64_t entry)
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

void EventTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   EvtWeights = 0;
   METPt = 0;
   METPx = 0;
   METPy = 0;
   METPz = 0;
   METE = 0;
   METsigx2 = 0;
   METsigxy = 0;
   METsigy2 = 0;
   METsig = 0;
   GLepDr01Pt = 0;
   GLepDr01Eta = 0;
   GLepDr01Phi = 0;
   GLepDr01E = 0;
   GLepDr01Id = 0;
   GLepDr01St = 0;
   GLepDr01MomId = 0;
   GLepBarePt = 0;
   GLepBareEta = 0;
   GLepBarePhi = 0;
   GLepBareE = 0;
   GLepBareId = 0;
   GLepBareSt = 0;
   GLepBareMomId = 0;
   GLepSt3Pt = 0;
   GLepSt3Eta = 0;
   GLepSt3Phi = 0;
   GLepSt3E = 0;
   GLepSt3Id = 0;
   GLepSt3St = 0;
   GLepSt3Mother0Id = 0;
   GLepSt3MotherCnt = 0;
   GLepClosePhotPt = 0;
   GLepClosePhotEta = 0;
   GLepClosePhotPhi = 0;
   GLepClosePhotE = 0;
   GLepClosePhotId = 0;
   GLepClosePhotMother0Id = 0;
   GLepClosePhotMotherCnt = 0;
   GLepClosePhotSt = 0;
   GJetAk04Pt = 0;
   GJetAk04Eta = 0;
   GJetAk04Phi = 0;
   GJetAk04E = 0;
   GJetAk04ChFrac = 0;
   GJetAk04ConstCnt = 0;
   GJetAk04ConstId = 0;
   GJetAk04ConstPt = 0;
   GJetAk04ConstEta = 0;
   GJetAk04ConstPhi = 0;
   GJetAk04ConstE = 0;
   GPdfId1 = 0;
   GPdfId2 = 0;
   GPdfx1 = 0;
   GPdfx2 = 0;
   GPdfScale = 0;
   MuPt = 0;
   MuEta = 0;
   MuPhi = 0;
   MuE = 0;
   MuId = 0;
   MuIdTight = 0;
   MuCh = 0;
   MuVtxZ = 0;
   MuDxy = 0;
   MuIsoRho = 0;
   MuPfIso = 0;
   MuType = 0;
   MuIsoTkIsoAbs = 0;
   MuIsoTkIsoRel = 0;
   MuIsoCalAbs = 0;
   MuIsoCombRel = 0;
   MuTkNormChi2 = 0;
   MuTkHitCnt = 0;
   MuMatchedStationCnt = 0;
   MuDz = 0;
   MuPixelHitCnt = 0;
   MuTkLayerCnt = 0;
   MuPfIsoChHad = 0;
   MuPfIsoNeutralHad = 0;
   MuPfIsoRawRel = 0;
   MuHltMatch = 0;
   ElPt = 0;
   ElEta = 0;
   ElEtaSc = 0;
   ElPhi = 0;
   ElE = 0;
   ElId = 0;
   ElCh = 0;
   ElMvaTrig = 0;
   ElMvaNonTrig = 0;
   ElMvaPresel = 0;
   ElDEtaTkScAtVtx = 0;
   ElDPhiTkScAtVtx = 0;
   ElHoE = 0;
   ElSigmaIetaIeta = 0;
   ElSigmaIetaIetaFull5x5 = 0;
   ElEinvMinusPinv = 0;
   ElD0 = 0;
   ElDz = 0;
   ElExpectedMissingInnerHitCnt = 0;
   ElPassConvVeto = 0;
   ElHltMatch = 0;
   ElPfIsoChHad = 0;
   ElPfIsoNeutralHad = 0;
   ElPfIsoIso = 0;
   ElPfIsoPuChHad = 0;
   ElPfIsoRaw = 0;
   ElPfIsoDbeta = 0;
   ElPfIsoRho = 0;
   ElAEff = 0;
   charged = 0;
   photon = 0;
   neutral = 0;
   charged_Tom = 0;
   photon_Tom = 0;
   neutral_Tom = 0;
   PhotPt = 0;
   PhotEta = 0;
   PhotPhi = 0;
   PhotScRawE = 0;
   PhotScEta = 0;
   PhotScPhi = 0;
   PhotIsoEcal = 0;
   PhotIsoHcal = 0;
   PhotIsoTk = 0;
   PhotPfIsoChHad = 0;
   PhotPfIsoNeutralHad = 0;
   PhotPfIsoPhot = 0;
   PhotPfIsoPuChHad = 0;
   PhotPfIsoEcalClus = 0;
   PhotPfIsoHcalClus = 0;
   PhotE3x3 = 0;
   PhotE1x5 = 0;
   PhotE2x5 = 0;
   PhotE5x5 = 0;
   PhotSigmaIetaIeta = 0;
   PhotEtaWidth = 0;
   PhotPhiWidth = 0;
   PhotHoE = 0;
   PhotId = 0;
   PhotHasPixelSeed = 0;
   JetAk04Pt = 0;
   JetAk04Eta = 0;
   JetAk04Phi = 0;
   JetAk04E = 0;
   JetAk04Id = 0;
   JetAk04PuId = 0;
   JetAk04PuMva = 0;
   JetAk04RawPt = 0;
   JetAk04RawE = 0;
   JetAk04HfHadE = 0;
   JetAk04HfEmE = 0;
   JetAk04ChHadFrac = 0;
   JetAk04NeutralHadAndHfFrac = 0;
   JetAk04ChEmFrac = 0;
   JetAk04NeutralEmFrac = 0;
   JetAk04ChMult = 0;
   JetAk04ConstCnt = 0;
   JetAk04JetBeta = 0;
   JetAk04JetBetaClassic = 0;
   JetAk04JetBetaStar = 0;
   JetAk04JetBetaStarClassic = 0;
   JetAk04BTagCsv = 0;
   JetAk04BTagCsvV1 = 0;
   JetAk04BTagCsvSLV1 = 0;
   JetAk04BDiscCisvV2 = 0;
   JetAk04BDiscJp = 0;
   JetAk04BDiscBjp = 0;
   JetAk04BDiscTche = 0;
   JetAk04BDiscTchp = 0;
   JetAk04BDiscSsvhe = 0;
   JetAk04BDiscSsvhp = 0;
   JetAk04PartFlav = 0;
   JetAk04JecUncUp = 0;
   JetAk04JecUncDwn = 0;
   JetAk04ConstId = 0;
   JetAk04ConstPt = 0;
   JetAk04ConstEta = 0;
   JetAk04ConstPhi = 0;
   JetAk04ConstE = 0;
   JetAk04GenJet = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("EvtIsRealData", &EvtIsRealData, &b_EvtIsRealData);
   fChain->SetBranchAddress("EvtNum", &EvtNum, &b_EvtNum);
   fChain->SetBranchAddress("EvtRunNum", &EvtRunNum, &b_EvtRunNum);
   fChain->SetBranchAddress("EvtLumiNum", &EvtLumiNum, &b_EvtLumiNum);
   fChain->SetBranchAddress("EvtBxNum", &EvtBxNum, &b_EvtBxNum);
   fChain->SetBranchAddress("EvtVtxCnt", &EvtVtxCnt, &b_EvtVtxCnt);
   fChain->SetBranchAddress("EvtPuCnt", &EvtPuCnt, &b_EvtPuCnt);
   fChain->SetBranchAddress("EvtPuCntTruth", &EvtPuCntTruth, &b_EvtPuCntTruth);
   fChain->SetBranchAddress("EvtWeights", &EvtWeights, &b_EvtWeights);
   fChain->SetBranchAddress("EvtFastJetRho", &EvtFastJetRho, &b_EvtFastJetRho);
   fChain->SetBranchAddress("TrigHlt", &TrigHlt, &b_TrigHlt);
   fChain->SetBranchAddress("TrigHltPhot", &TrigHltPhot, &b_TrigHltPhot);
   fChain->SetBranchAddress("TrigHltMu", &TrigHltMu, &b_TrigHltMu);
   fChain->SetBranchAddress("TrigHltDiMu", &TrigHltDiMu, &b_TrigHltDiMu);
   fChain->SetBranchAddress("METPt", &METPt, &b_METPt);
   fChain->SetBranchAddress("METPx", &METPx, &b_METPx);
   fChain->SetBranchAddress("METPy", &METPy, &b_METPy);
   fChain->SetBranchAddress("METPz", &METPz, &b_METPz);
   fChain->SetBranchAddress("METE", &METE, &b_METE);
   fChain->SetBranchAddress("METsigx2", &METsigx2, &b_METsigx2);
   fChain->SetBranchAddress("METsigxy", &METsigxy, &b_METsigxy);
   fChain->SetBranchAddress("METsigy2", &METsigy2, &b_METsigy2);
   fChain->SetBranchAddress("METsig", &METsig, &b_METsig);
   fChain->SetBranchAddress("GLepDr01Pt", &GLepDr01Pt, &b_GLepDr01Pt);
   fChain->SetBranchAddress("GLepDr01Eta", &GLepDr01Eta, &b_GLepDr01Eta);
   fChain->SetBranchAddress("GLepDr01Phi", &GLepDr01Phi, &b_GLepDr01Phi);
   fChain->SetBranchAddress("GLepDr01E", &GLepDr01E, &b_GLepDr01E);
   fChain->SetBranchAddress("GLepDr01Id", &GLepDr01Id, &b_GLepDr01Id);
   fChain->SetBranchAddress("GLepDr01St", &GLepDr01St, &b_GLepDr01St);
   fChain->SetBranchAddress("GLepDr01MomId", &GLepDr01MomId, &b_GLepDr01MomId);
   fChain->SetBranchAddress("GLepBarePt", &GLepBarePt, &b_GLepBarePt);
   fChain->SetBranchAddress("GLepBareEta", &GLepBareEta, &b_GLepBareEta);
   fChain->SetBranchAddress("GLepBarePhi", &GLepBarePhi, &b_GLepBarePhi);
   fChain->SetBranchAddress("GLepBareE", &GLepBareE, &b_GLepBareE);
   fChain->SetBranchAddress("GLepBareId", &GLepBareId, &b_GLepBareId);
   fChain->SetBranchAddress("GLepBareSt", &GLepBareSt, &b_GLepBareSt);
   fChain->SetBranchAddress("GLepBareMomId", &GLepBareMomId, &b_GLepBareMomId);
   fChain->SetBranchAddress("GLepSt3Pt", &GLepSt3Pt, &b_GLepSt3Pt);
   fChain->SetBranchAddress("GLepSt3Eta", &GLepSt3Eta, &b_GLepSt3Eta);
   fChain->SetBranchAddress("GLepSt3Phi", &GLepSt3Phi, &b_GLepSt3Phi);
   fChain->SetBranchAddress("GLepSt3E", &GLepSt3E, &b_GLepSt3E);
   fChain->SetBranchAddress("GLepSt3Id", &GLepSt3Id, &b_GLepSt3Id);
   fChain->SetBranchAddress("GLepSt3St", &GLepSt3St, &b_GLepSt3St);
   fChain->SetBranchAddress("GLepSt3Mother0Id", &GLepSt3Mother0Id, &b_GLepSt3Mother0Id);
   fChain->SetBranchAddress("GLepSt3MotherCnt", &GLepSt3MotherCnt, &b_GLepSt3MotherCnt);
   fChain->SetBranchAddress("GLepClosePhotPt", &GLepClosePhotPt, &b_GLepClosePhotPt);
   fChain->SetBranchAddress("GLepClosePhotEta", &GLepClosePhotEta, &b_GLepClosePhotEta);
   fChain->SetBranchAddress("GLepClosePhotPhi", &GLepClosePhotPhi, &b_GLepClosePhotPhi);
   fChain->SetBranchAddress("GLepClosePhotE", &GLepClosePhotE, &b_GLepClosePhotE);
   fChain->SetBranchAddress("GLepClosePhotId", &GLepClosePhotId, &b_GLepClosePhotId);
   fChain->SetBranchAddress("GLepClosePhotMother0Id", &GLepClosePhotMother0Id, &b_GLepClosePhotMother0Id);
   fChain->SetBranchAddress("GLepClosePhotMotherCnt", &GLepClosePhotMotherCnt, &b_GLepClosePhotMotherCnt);
   fChain->SetBranchAddress("GLepClosePhotSt", &GLepClosePhotSt, &b_GLepClosePhotSt);
   fChain->SetBranchAddress("GJetAk04Pt", &GJetAk04Pt, &b_GJetAk04Pt);
   fChain->SetBranchAddress("GJetAk04Eta", &GJetAk04Eta, &b_GJetAk04Eta);
   fChain->SetBranchAddress("GJetAk04Phi", &GJetAk04Phi, &b_GJetAk04Phi);
   fChain->SetBranchAddress("GJetAk04E", &GJetAk04E, &b_GJetAk04E);
   fChain->SetBranchAddress("GJetAk04ChFrac", &GJetAk04ChFrac, &b_GJetAk04ChFrac);
   fChain->SetBranchAddress("GJetAk04ConstCnt", &GJetAk04ConstCnt, &b_GJetAk04ConstCnt);
   fChain->SetBranchAddress("GJetAk04ConstId", &GJetAk04ConstId, &b_GJetAk04ConstId);
   fChain->SetBranchAddress("GJetAk04ConstPt", &GJetAk04ConstPt, &b_GJetAk04ConstPt);
   fChain->SetBranchAddress("GJetAk04ConstEta", &GJetAk04ConstEta, &b_GJetAk04ConstEta);
   fChain->SetBranchAddress("GJetAk04ConstPhi", &GJetAk04ConstPhi, &b_GJetAk04ConstPhi);
   fChain->SetBranchAddress("GJetAk04ConstE", &GJetAk04ConstE, &b_GJetAk04ConstE);
   fChain->SetBranchAddress("GPdfId1", &GPdfId1, &b_GPdfId1);
   fChain->SetBranchAddress("GPdfId2", &GPdfId2, &b_GPdfId2);
   fChain->SetBranchAddress("GPdfx1", &GPdfx1, &b_GPdfx1);
   fChain->SetBranchAddress("GPdfx2", &GPdfx2, &b_GPdfx2);
   fChain->SetBranchAddress("GPdfScale", &GPdfScale, &b_GPdfScale);
   fChain->SetBranchAddress("GBinningValue", &GBinningValue, &b_GBinningValue);
   fChain->SetBranchAddress("GNup", &GNup, &b_GNup);
   fChain->SetBranchAddress("MuPt", &MuPt, &b_MuPt);
   fChain->SetBranchAddress("MuEta", &MuEta, &b_MuEta);
   fChain->SetBranchAddress("MuPhi", &MuPhi, &b_MuPhi);
   fChain->SetBranchAddress("MuE", &MuE, &b_MuE);
   fChain->SetBranchAddress("MuId", &MuId, &b_MuId);
   fChain->SetBranchAddress("MuIdTight", &MuIdTight, &b_MuIdTight);
   fChain->SetBranchAddress("MuCh", &MuCh, &b_MuCh);
   fChain->SetBranchAddress("MuVtxZ", &MuVtxZ, &b_MuVtxZ);
   fChain->SetBranchAddress("MuDxy", &MuDxy, &b_MuDxy);
   fChain->SetBranchAddress("MuIsoRho", &MuIsoRho, &b_MuIsoRho);
   fChain->SetBranchAddress("MuPfIso", &MuPfIso, &b_MuPfIso);
   fChain->SetBranchAddress("MuType", &MuType, &b_MuType);
   fChain->SetBranchAddress("MuIsoTkIsoAbs", &MuIsoTkIsoAbs, &b_MuIsoTkIsoAbs);
   fChain->SetBranchAddress("MuIsoTkIsoRel", &MuIsoTkIsoRel, &b_MuIsoTkIsoRel);
   fChain->SetBranchAddress("MuIsoCalAbs", &MuIsoCalAbs, &b_MuIsoCalAbs);
   fChain->SetBranchAddress("MuIsoCombRel", &MuIsoCombRel, &b_MuIsoCombRel);
   fChain->SetBranchAddress("MuTkNormChi2", &MuTkNormChi2, &b_MuTkNormChi2);
   fChain->SetBranchAddress("MuTkHitCnt", &MuTkHitCnt, &b_MuTkHitCnt);
   fChain->SetBranchAddress("MuMatchedStationCnt", &MuMatchedStationCnt, &b_MuMatchedStationCnt);
   fChain->SetBranchAddress("MuDz", &MuDz, &b_MuDz);
   fChain->SetBranchAddress("MuPixelHitCnt", &MuPixelHitCnt, &b_MuPixelHitCnt);
   fChain->SetBranchAddress("MuTkLayerCnt", &MuTkLayerCnt, &b_MuTkLayerCnt);
   fChain->SetBranchAddress("MuPfIsoChHad", &MuPfIsoChHad, &b_MuPfIsoChHad);
   fChain->SetBranchAddress("MuPfIsoNeutralHad", &MuPfIsoNeutralHad, &b_MuPfIsoNeutralHad);
   fChain->SetBranchAddress("MuPfIsoRawRel", &MuPfIsoRawRel, &b_MuPfIsoRawRel);
   fChain->SetBranchAddress("MuHltMatch", &MuHltMatch, &b_MuHltMatch);
   fChain->SetBranchAddress("ElPt", &ElPt, &b_ElPt);
   fChain->SetBranchAddress("ElEta", &ElEta, &b_ElEta);
   fChain->SetBranchAddress("ElEtaSc", &ElEtaSc, &b_ElEtaSc);
   fChain->SetBranchAddress("ElPhi", &ElPhi, &b_ElPhi);
   fChain->SetBranchAddress("ElE", &ElE, &b_ElE);
   fChain->SetBranchAddress("ElId", &ElId, &b_ElId);
   fChain->SetBranchAddress("ElCh", &ElCh, &b_ElCh);
   fChain->SetBranchAddress("ElMvaTrig", &ElMvaTrig, &b_ElMvaTrig);
   fChain->SetBranchAddress("ElMvaNonTrig", &ElMvaNonTrig, &b_ElMvaNonTrig);
   fChain->SetBranchAddress("ElMvaPresel", &ElMvaPresel, &b_ElMvaPresel);
   fChain->SetBranchAddress("ElDEtaTkScAtVtx", &ElDEtaTkScAtVtx, &b_ElDEtaTkScAtVtx);
   fChain->SetBranchAddress("ElDPhiTkScAtVtx", &ElDPhiTkScAtVtx, &b_ElDPhiTkScAtVtx);
   fChain->SetBranchAddress("ElHoE", &ElHoE, &b_ElHoE);
   fChain->SetBranchAddress("ElSigmaIetaIeta", &ElSigmaIetaIeta, &b_ElSigmaIetaIeta);
   fChain->SetBranchAddress("ElSigmaIetaIetaFull5x5", &ElSigmaIetaIetaFull5x5, &b_ElSigmaIetaIetaFull5x5);
   fChain->SetBranchAddress("ElEinvMinusPinv", &ElEinvMinusPinv, &b_ElEinvMinusPinv);
   fChain->SetBranchAddress("ElD0", &ElD0, &b_ElD0);
   fChain->SetBranchAddress("ElDz", &ElDz, &b_ElDz);
   fChain->SetBranchAddress("ElExpectedMissingInnerHitCnt", &ElExpectedMissingInnerHitCnt, &b_ElExpectedMissingInnerHitCnt);
   fChain->SetBranchAddress("ElPassConvVeto", &ElPassConvVeto, &b_ElPassConvVeto);
   fChain->SetBranchAddress("ElHltMatch", &ElHltMatch, &b_ElHltMatch);
   fChain->SetBranchAddress("ElPfIsoChHad", &ElPfIsoChHad, &b_ElPfIsoChHad);
   fChain->SetBranchAddress("ElPfIsoNeutralHad", &ElPfIsoNeutralHad, &b_ElPfIsoNeutralHad);
   fChain->SetBranchAddress("ElPfIsoIso", &ElPfIsoIso, &b_ElPfIsoIso);
   fChain->SetBranchAddress("ElPfIsoPuChHad", &ElPfIsoPuChHad, &b_ElPfIsoPuChHad);
   fChain->SetBranchAddress("ElPfIsoRaw", &ElPfIsoRaw, &b_ElPfIsoRaw);
   fChain->SetBranchAddress("ElPfIsoDbeta", &ElPfIsoDbeta, &b_ElPfIsoDbeta);
   fChain->SetBranchAddress("ElPfIsoRho", &ElPfIsoRho, &b_ElPfIsoRho);
   fChain->SetBranchAddress("ElAEff", &ElAEff, &b_ElAEff);
   fChain->SetBranchAddress("charged", &charged, &b_charged);
   fChain->SetBranchAddress("photon", &photon, &b_photon);
   fChain->SetBranchAddress("neutral", &neutral, &b_neutral);
   fChain->SetBranchAddress("charged_Tom", &charged_Tom, &b_charged_Tom);
   fChain->SetBranchAddress("photon_Tom", &photon_Tom, &b_photon_Tom);
   fChain->SetBranchAddress("neutral_Tom", &neutral_Tom, &b_neutral_Tom);
   fChain->SetBranchAddress("PhotPt", &PhotPt, &b_PhotPt);
   fChain->SetBranchAddress("PhotEta", &PhotEta, &b_PhotEta);
   fChain->SetBranchAddress("PhotPhi", &PhotPhi, &b_PhotPhi);
   fChain->SetBranchAddress("PhotScRawE", &PhotScRawE, &b_PhotScRawE);
   fChain->SetBranchAddress("PhotScEta", &PhotScEta, &b_PhotScEta);
   fChain->SetBranchAddress("PhotScPhi", &PhotScPhi, &b_PhotScPhi);
   fChain->SetBranchAddress("PhotIsoEcal", &PhotIsoEcal, &b_PhotIsoEcal);
   fChain->SetBranchAddress("PhotIsoHcal", &PhotIsoHcal, &b_PhotIsoHcal);
   fChain->SetBranchAddress("PhotIsoTk", &PhotIsoTk, &b_PhotIsoTk);
   fChain->SetBranchAddress("PhotPfIsoChHad", &PhotPfIsoChHad, &b_PhotPfIsoChHad);
   fChain->SetBranchAddress("PhotPfIsoNeutralHad", &PhotPfIsoNeutralHad, &b_PhotPfIsoNeutralHad);
   fChain->SetBranchAddress("PhotPfIsoPhot", &PhotPfIsoPhot, &b_PhotPfIsoPhot);
   fChain->SetBranchAddress("PhotPfIsoPuChHad", &PhotPfIsoPuChHad, &b_PhotPfIsoPuChHad);
   fChain->SetBranchAddress("PhotPfIsoEcalClus", &PhotPfIsoEcalClus, &b_PhotPfIsoEcalClus);
   fChain->SetBranchAddress("PhotPfIsoHcalClus", &PhotPfIsoHcalClus, &b_PhotPfIsoHcalClus);
   fChain->SetBranchAddress("PhotE3x3", &PhotE3x3, &b_PhotE3x3);
   fChain->SetBranchAddress("PhotE1x5", &PhotE1x5, &b_PhotE1x5);
   fChain->SetBranchAddress("PhotE2x5", &PhotE2x5, &b_PhotE2x5);
   fChain->SetBranchAddress("PhotE5x5", &PhotE5x5, &b_PhotE5x5);
   fChain->SetBranchAddress("PhotSigmaIetaIeta", &PhotSigmaIetaIeta, &b_PhotSigmaIetaIeta);
   fChain->SetBranchAddress("PhotEtaWidth", &PhotEtaWidth, &b_PhotEtaWidth);
   fChain->SetBranchAddress("PhotPhiWidth", &PhotPhiWidth, &b_PhotPhiWidth);
   fChain->SetBranchAddress("PhotHoE", &PhotHoE, &b_PhotHoE);
   fChain->SetBranchAddress("PhotId", &PhotId, &b_PhotId);
   fChain->SetBranchAddress("PhotHasPixelSeed", &PhotHasPixelSeed, &b_PhotHasPixelSeed);
   fChain->SetBranchAddress("JetAk04Pt", &JetAk04Pt, &b_JetAk04Pt);
   fChain->SetBranchAddress("JetAk04Eta", &JetAk04Eta, &b_JetAk04Eta);
   fChain->SetBranchAddress("JetAk04Phi", &JetAk04Phi, &b_JetAk04Phi);
   fChain->SetBranchAddress("JetAk04E", &JetAk04E, &b_JetAk04E);
   fChain->SetBranchAddress("JetAk04Id", &JetAk04Id, &b_JetAk04Id);
   fChain->SetBranchAddress("JetAk04PuId", &JetAk04PuId, &b_JetAk04PuId);
   fChain->SetBranchAddress("JetAk04PuMva", &JetAk04PuMva, &b_JetAk04PuMva);
   fChain->SetBranchAddress("JetAk04RawPt", &JetAk04RawPt, &b_JetAk04RawPt);
   fChain->SetBranchAddress("JetAk04RawE", &JetAk04RawE, &b_JetAk04RawE);
   fChain->SetBranchAddress("JetAk04HfHadE", &JetAk04HfHadE, &b_JetAk04HfHadE);
   fChain->SetBranchAddress("JetAk04HfEmE", &JetAk04HfEmE, &b_JetAk04HfEmE);
   fChain->SetBranchAddress("JetAk04ChHadFrac", &JetAk04ChHadFrac, &b_JetAk04ChHadFrac);
   fChain->SetBranchAddress("JetAk04NeutralHadAndHfFrac", &JetAk04NeutralHadAndHfFrac, &b_JetAk04NeutralHadAndHfFrac);
   fChain->SetBranchAddress("JetAk04ChEmFrac", &JetAk04ChEmFrac, &b_JetAk04ChEmFrac);
   fChain->SetBranchAddress("JetAk04NeutralEmFrac", &JetAk04NeutralEmFrac, &b_JetAk04NeutralEmFrac);
   fChain->SetBranchAddress("JetAk04ChMult", &JetAk04ChMult, &b_JetAk04ChMult);
   fChain->SetBranchAddress("JetAk04ConstCnt", &JetAk04ConstCnt, &b_JetAk04ConstCnt);
   fChain->SetBranchAddress("JetAk04JetBeta", &JetAk04JetBeta, &b_JetAk04JetBeta);
   fChain->SetBranchAddress("JetAk04JetBetaClassic", &JetAk04JetBetaClassic, &b_JetAk04JetBetaClassic);
   fChain->SetBranchAddress("JetAk04JetBetaStar", &JetAk04JetBetaStar, &b_JetAk04JetBetaStar);
   fChain->SetBranchAddress("JetAk04JetBetaStarClassic", &JetAk04JetBetaStarClassic, &b_JetAk04JetBetaStarClassic);
   fChain->SetBranchAddress("JetAk04BTagCsv", &JetAk04BTagCsv, &b_JetAk04BTagCsv);
   fChain->SetBranchAddress("JetAk04BTagCsvV1", &JetAk04BTagCsvV1, &b_JetAk04BTagCsvV1);
   fChain->SetBranchAddress("JetAk04BTagCsvSLV1", &JetAk04BTagCsvSLV1, &b_JetAk04BTagCsvSLV1);
   fChain->SetBranchAddress("JetAk04BDiscCisvV2", &JetAk04BDiscCisvV2, &b_JetAk04BDiscCisvV2);
   fChain->SetBranchAddress("JetAk04BDiscJp", &JetAk04BDiscJp, &b_JetAk04BDiscJp);
   fChain->SetBranchAddress("JetAk04BDiscBjp", &JetAk04BDiscBjp, &b_JetAk04BDiscBjp);
   fChain->SetBranchAddress("JetAk04BDiscTche", &JetAk04BDiscTche, &b_JetAk04BDiscTche);
   fChain->SetBranchAddress("JetAk04BDiscTchp", &JetAk04BDiscTchp, &b_JetAk04BDiscTchp);
   fChain->SetBranchAddress("JetAk04BDiscSsvhe", &JetAk04BDiscSsvhe, &b_JetAk04BDiscSsvhe);
   fChain->SetBranchAddress("JetAk04BDiscSsvhp", &JetAk04BDiscSsvhp, &b_JetAk04BDiscSsvhp);
   fChain->SetBranchAddress("JetAk04PartFlav", &JetAk04PartFlav, &b_JetAk04PartFlav);
   fChain->SetBranchAddress("JetAk04JecUncUp", &JetAk04JecUncUp, &b_JetAk04JecUncUp);
   fChain->SetBranchAddress("JetAk04JecUncDwn", &JetAk04JecUncDwn, &b_JetAk04JecUncDwn);
   fChain->SetBranchAddress("JetAk04ConstId", &JetAk04ConstId, &b_JetAk04ConstId);
   fChain->SetBranchAddress("JetAk04ConstPt", &JetAk04ConstPt, &b_JetAk04ConstPt);
   fChain->SetBranchAddress("JetAk04ConstEta", &JetAk04ConstEta, &b_JetAk04ConstEta);
   fChain->SetBranchAddress("JetAk04ConstPhi", &JetAk04ConstPhi, &b_JetAk04ConstPhi);
   fChain->SetBranchAddress("JetAk04ConstE", &JetAk04ConstE, &b_JetAk04ConstE);
   fChain->SetBranchAddress("JetAk04GenJet", &JetAk04GenJet, &b_JetAk04GenJet);
   Notify();
}

Bool_t EventTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void EventTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t EventTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef EventTree_cxx
