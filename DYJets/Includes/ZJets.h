//-*- c-basic-offset: 8; -*-
#ifndef ZJets_h
#define ZJets_h

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include <RooUnfoldResponse.h>
#include <TDatime.h>
#include <TMath.h>
#include <TRandom3.h>

// Header file for the classes stored in the TTree if any.
#include "functions.h"
#include "getFilesAndHistogramsZJets.h"
#include "HistoSetZJets.h"

#include "ConfigVJets.h"
#include "rochcor.h"


using namespace std;


class ZJets: public HistoSetZJets {
    public :
        bool doRochester;
        rochcor2012 *rmcor;
        //TTree          *fChain;   //!pointer to the analyzed TTree or TChain
        TChain          *fChain;   //!pointer to the analyzed TTree or TChain
        //TTree          *tree;
        Int_t           fCurrent; //!current Tree number in a TChain

        // Declaration of leaf types
        Double_t        PU_npT;
        Double_t        PU_npIT;

        Int_t           EvtInfo_NumVtx;
        Int_t           EvtInfo_RunNum;
        Int_t           EvtInfo_EventNum;
        Int_t	        nup_;
        vector<double>  *pdfInfo_;
        vector<double>  *genLepPt_;
        vector<double>  *genLepEta_;
        vector<double>  *genLepPhi_;
        vector<double>  *genLepE_;
        vector<double>  *genLepQ_;
        vector<double>  *genLepId_;
        vector<double>  *genLepSt_;
        vector<double>  *genPhoPt_;
        vector<double>  *genPhoEta_;
        vector<double>  *genPhoPhi_;
        vector<double>  *genJetPt_;
        vector<double>  *genJetEta_;
        vector<double>  *genJetPhi_;
        vector<double>  *genJetE_;

        vector<double>  *patElecPt_;
        vector<double>  *patElecEta_;
        vector<double>  *patElecPhi_;
        vector<double>  *patElecEn_;
        vector<double>  *patElecCharge_;
        vector<double>  *patElecID_;
        vector<double>  *patElecTrig_;
        vector<double>  *patElecPfIsoRho_;
        vector<double>  *patElecScEta_;

        vector<double>  *patMuonPt_;
        vector<double>  *patMuonEta_;
        vector<double>  *patMuonPhi_;
        vector<double>  *patMuonEn_;
        vector<double>  *patMuonCharge_;
        vector<double>  *patMuonCombId_Double;
        vector<int>  *patMuonCombId_Int;
        vector<double>  *patMuonTrig_;
        vector<double>  *patMuonPfIsoDbeta_;

        // -- JET ---
        vector<double>  *patJetPfAk05En_;
        vector<double>  *patJetPfAk05Pt_;
        vector<double>  *patJetPfAk05Eta_;
        vector<double>  *patJetPfAk05Phi_;
        vector<double>  *patJetPfAk05LooseId_;
        vector<double>  *patJetPfAk05jetpuMVA_;
        vector<double>  *patJetPfAk05OCSV_;
        vector<int>     *patJetPfAk05PartonFlavour_;

        // -- MET ---
        vector<double>  *patMetPt_;
        vector<double>  *patMetPhi_;
        vector<double>  *patMetSig_;

        vector<double>  *mcSherpaWeights_;
        vector<double>  *mcEventWeight_;
        //Double_t mcEveWeight_;

        // List of branches
        TBranch        *b_mcEventWeight_;   //!
        //TBranch        *b_mcSherpaSumWeight3_;   //!
        TBranch        *b_mcSherpaWeights_;   //!
        //TBranch        *b_weight_amcNLO_;   //!
        //TBranch        *b_weight_amcNLO_sum_;   //!
        TBranch        *b_nup_;   //!
        TBranch        *b_pdfInfo_;   //!
        TBranch        *b_PU_npT;   //!
        TBranch        *b_PU_npIT;   //!
        TBranch        *b_EvtInfo_NumVtx;   //!
        TBranch        *b_EvtInfo_RunNum;   //!
        TBranch        *b_EvtInfo_EventNum;   //!
        TBranch        *b_genLepPt_;   //!
        TBranch        *b_genLepEta_;   //!
        TBranch        *b_genLepPhi_;   //!
        TBranch        *b_genLepE_;   //!
        TBranch        *b_genLepQ_;   //!
        TBranch        *b_genLepId_;   //!
        TBranch        *b_genLepSt_;   //!
        TBranch        *b_genPhoPt_;   //!
        TBranch        *b_genPhoEta_;   //!
        TBranch        *b_genPhoPhi_;   //!
        TBranch        *b_genJetPt_;   //!
        TBranch        *b_genJetEta_;   //!
        TBranch        *b_genJetPhi_;   //!
        TBranch        *b_genJetE_;   //!
        TBranch        *b_dpsParton_Pt;   //!
        TBranch        *b_dpsParton_Eta;   //!
        TBranch        *b_dpsParton_Phi;   //!
        TBranch        *b_dpsParton_E;   //!
        TBranch        *b_genMatchDPSpar;   //!
        TBranch        *b_dpsParton_dR;   //!

        TBranch        *b_gsfElecPt_;   //!
        TBranch        *b_gsfElecEta_;   //!
        TBranch        *b_gsfElecPhi_;   //!
        TBranch        *b_gsfElecEnergy_;   //!
        TBranch        *b_patElecPt_;   //!
        TBranch        *b_patElecEta_;   //!
        TBranch        *b_patElecPhi_;   //!
        TBranch        *b_patElecEn_;   //!
        TBranch        *b_patElecCharge_;   //!
        TBranch        *b_patElecID_;   //!
        TBranch        *b_patElecTrig_;   //!
        TBranch        *b_patElecDetIso_;   //!
        TBranch        *b_patElecPfIsoRho_;   //!
        TBranch        *b_patElecScEta_;   //!
        TBranch        *b_patElecIsPF_;   //!

        TBranch        *b_patMuonPt_;   //!
        TBranch        *b_patMuonEta_;   //!
        TBranch        *b_patMuonPhi_;   //!
        TBranch        *b_patMuonVtxZ_;   //!
        TBranch        *b_patMuonEn_;   //!
        TBranch        *b_patMuonCharge_;   //!
        TBranch        *b_patMuonDxy_;   //!
        TBranch        *b_patMuonCombId_Double;   //!
        TBranch        *b_patMuonCombId_Int;   //!
        TBranch        *b_patMuonTrig_;   //!
        TBranch        *b_patMuonPfIsoDbeta_;   //!

        TBranch        *b_patJetPfAk05En_;   //!
        TBranch        *b_patJetPfAk05Pt_;   //!
        TBranch        *b_patJetPfAk05Eta_;   //!
        TBranch        *b_patJetPfAk05Phi_;   //!
        TBranch        *b_patJetPfAk05LooseId_;   //!
        TBranch        *b_patJetPfAk05jetBSZ_;   //!
        TBranch        *b_patJetPfAk05jetBZ_;   //!
        TBranch        *b_patJetPfAk05jetpuMVA_;   //!
        TBranch        *b_patJetPfAk05OCSV_;   //!
        TBranch        *b_patJetPfAk05PartonFlavour_;  //!

        TBranch        *b_patMetPt_;   //!
        TBranch        *b_patMetPhi_;   //!
        TBranch        *b_patMetSig_;   //!




        ZJets(TString fileName_, float lumiScale_ = 1., bool useTriggerCorrection_ = 0, int systematics_ = 0, int direction_ = 0, float xsecfactor_ = 1., 
                int lepPtCutMin_ = 20, int lepEtaCutMax_ = 24, int jetPtCutMin_ = 30, int jetEtaCutMax_ = 24, Long_t nEvents_ = 0, TString outDir_ = "TEST", 
                TString bonzaiDir = "/afs/cern.ch/work/a/aleonard/public/ZJetsFiles/");
        ~ZJets();
        string   CreateOutputFileName(TString pdfSet = "", int pdfMember = -1, double muR = 0, double muF = 0);
        Int_t    Cut(Long64_t entry);
        Int_t    GetEntry(Long64_t entry);
        Long64_t LoadTree(Long64_t entry);
        void     Init(bool hasRecoInfo, bool hasGenInfo);
        void     initLHAPDF(TString pdfSet, int pdfMember);
        double   computePDFWeight();
        void     Loop(bool hasRecoInfo = 1, bool hasGenInfo = 0, TString pdfSet = "", int pdfMember = -1, double muR = 0, double muF = 0);
        void     getMuons(vector<leptonStruct>& leptons,  vector<leptonStruct>& vetoMuons);
        void     getElectrons(vector<leptonStruct>& leptons,  vector<leptonStruct>& vetoElectrons);
        Bool_t   Notify();
        void     Show(Long64_t entry = -1);

        TString outputDirectory;
        TString fileName; 
        float lumiScale;
        bool useTriggerCorrection;
        bool isData;
        int systematics;
        int direction;
        float xsecfactor;
        int lepPtCutMin;
        int lepEtaCutMax;
        int jetPtCutMin;
        int jetEtaCutMax;
        Long_t nMaxEvents;
        TString lepSel;
        bool rejectBTagEvents;

        ConfigVJets cfg_;
};
#endif

