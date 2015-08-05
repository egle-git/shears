#include "BonzaiMaker.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TBranch.h>
#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define DEBUG 0

using namespace std;

BonzaiMaker::BonzaiMaker(ConfigVJets cfg) : fChain(0), cfg_(cfg)
{

string doWhat;
string storageElement;


    storageElement = "root://eoscms//eos/cms/store/group/phys_smp/AnalysisFramework/Baobab/13TeV_50ns/";

    string dirPath = "/tupel";
    string treeName = "/EventTree";
    string fullFileName;
 
    TString txtFileNameTmp = cfg_.getS("txtFile");


    fChain = new TChain();
    if (txtFileNameTmp.EndsWith(".txt") != string::npos) {
        ifstream listOfFiles(txtFileNameTmp);
        if (listOfFiles.is_open()) {
            while (listOfFiles.good()) {
                string line;
                getline(listOfFiles, line);
                if (line != "") {
                    fullFileName = storageElement + line + dirPath + treeName;  
                    std::cout << fullFileName << std::endl;
                    fChain->Add(fullFileName.c_str());
                }
            }
        }
        else {
            std::cout << "Oups, file " << txtFileNameTmp << " does not exist" << std::endl;
            std::cout << "Aborting..." << std::endl;
            abort();
        }
    }
    else {
        fChain->Add(fullFileName.c_str());
    }


    Init(fChain);

}

BonzaiMaker::~BonzaiMaker()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

void BonzaiMaker::Loop()
{


   string hasGenInfo = "false";  // fasle for data, true for MC
   string hasRecoInfo = "true";
 
   if(cfg_.getS("doWhat")!="DATA"){
    hasGenInfo = "true";
   }

   // before writing output on eos one has to mount eos first:  eosmount ~/eos 
 
    unsigned first = (cfg_.getS("txtFile").find_last_of("/")+1);
    unsigned last = cfg_.getS("txtFile").find(".txt");
    string strNew = cfg_.getS("txtFile").substr (first,last-first);

   string outFileRoot = cfg_.getS("lepSel")+"_"+ strNew +"_"+cfg_.getS("CMEnergy")  + ".root" ; 
   
   TFile *outputFile = new TFile(outFileRoot.c_str(), "recreate");   
   if(cfg_.getS("outputDir") == "eos"){
           string outFileRoot = "/afs/cern.ch/user/a/agrebeny/eos/cms/store/group/phys_smp/VJets/Bonzai13TeVoutput/"+cfg_.getS("lepSel")+"_"+ strNew + "_fullstat_" + cfg_.getS("CMEnergy")+".root" ;
          outputFile = new TFile(outFileRoot.c_str(), "recreate");
    }

    //-- output tree --
    TTree *outputTree = new TTree("tree", "tree");

    int leptonIdSum; // leptonIdSum: lepton1 id + lepton2 id
    if(cfg_.getS("lepSel") == "DMu") leptonIdSum = 26;  // mumu => leptonIdSum = 13 + 13 = 26
    else if(cfg_.getS("lepSel") == "DE") leptonIdSum = 22;  // leptonIdSum = 11 + 11 = 22
    else if(cfg_.getS("lepSel") == "SMuSE") leptonIdSum = 24; // emu  => leptonIdSum = 11 + 13 = 24 
    else if(cfg_.getS("lepSel") == "SE") leptonIdSum = 11;  // e    => leptonIdSum = 11 
    else if(cfg_.getS("lepSel") == "SMu") leptonIdSum = 13;   // mu   => leptonIdSum = 13
    else  cout << "WARNING: leptonIdSum is not defined" << "\n";


    // statistics 
    int passedEvents = 0;
    double weight_amcNLO_sum = 0;
    //--------------------------------------------------------

    //--------------------------------------------------------
    //-- output tree containt --
    // boson vector id. This is overwritten according to leptonIdSum
    // 23: Z boson; 24: W boson
    int doVector = 24;

    // lepton id. This is overwritten according to leptonIdSum
    int lepID = 11; 

    // leptonIdSum: lepton1 id + lepton2 id
    // ee   => leptonIdSum = 11 + 11 = 22
    // mumu => leptonIdSum = 13 + 13 = 26
    // emu  => leptonIdSum = 11 + 13 = 24
    // e    => leptonIdSum = 11 
    // mu   => leptonIdSum = 13
    if (leptonIdSum == 22) { // double electron and thus Z boson
        doVector = 23;
        lepID = 11;
    }
    else if (leptonIdSum == 26) { // double muon and thus Z boson
        doVector = 23; 
        lepID = 13;
    }
    else if (leptonIdSum == 13) { // single muon and thus W boson
        doVector = 24;
        lepID = 13;
    }
    else if (leptonIdSum == 24) { // electron - muon
        lepID = 24;
    }

    cout << " leptons are " << lepID << " leptonIdSum " << leptonIdSum << " doVector " << doVector << endl;


    Double_t EvtPuCnt_out(0);
    Double_t EvtPuCntTruth_out(0);

    Int_t  EvtVtxCnt_out = -111;
    Int_t EvtRunNum_out = -111;
    Int_t  EvtLumiNum_out = -111;
    Int_t EvtNum_out = -111;
  
    Int_t GNup_out;
    vector<double> pdfInfo_;

    vector<double>  mcEventWeight_out;

    // -- bare gen leptons (status 1 = post FSR)--
    vector<double> GLepSt1and3Pt_out;
    vector<double> GLepSt1and3Eta_out;
    vector<double> GLepSt1and3E_out;
    vector<double> GLepSt1and3Phi_out;
    vector<double> GLepSt1and3Id_out;
    vector<double> GLepSt1and3Q_out;
    vector<double> GLepSt1and3St_out;


    // -- gen photons (status 1 = post FSR)--
    vector<double> GLepClosePhotPt_out;
    vector<double> GLepClosePhotEta_out;
    vector<double> GLepClosePhotPhi_out;

    // -- gen jets --
    vector<double> GJetAk04Pt_out;
    vector<double> GJetAk04Eta_out;
    vector<double> GJetAk04Phi_out;
    vector<double> GJetAk04E_out;
    vector<double> GJetAk04ChFrac_out;

    // -- rec muons --
    int eventMuonTrig;
    vector<double>  MuPt_out;
    vector<double>  MuEta_out;
    vector<double>  MuPhi_out;
    vector<double>  MuE_out;
    vector<double>  MuCh_out;
    vector<int>     MuId_out;
    vector<double>  patMuonTrig_;
    vector<double>  MuPfIso_out;
    vector<double>  MuVtxZ_out;
    vector<double>  MuDxy_out;

    // -- rec electrons --
    int eventElecTrig;
    vector<double>   ElId_out;
    vector<double>   ElPt_out;
    vector<double>   ElEta_out;
    vector<double>   ElPhi_out;
    vector<double>   ElE_out;
    vector<double>   ElCh_out;
    vector<double>   ElPfIsoRho_out;
    vector<double>   ElEtaSc_out;
    vector<double>   patElecTrig_;

    // -- rec jets --
    vector<double> JetAk04E_out;
    vector<double> JetAk04Pt_out;
    vector<double> JetAk04Eta_out;
    vector<double> JetAk04Phi_out;
    vector<double>    JetAk04Id_out;
    vector<double> JetAk04PuId_out;
    vector<double> JetAk04PuMva_out;
    //  vector<double> JetAk04ChHadFrac_out;
    vector<double> JetAk04JetBeta_out;
    vector<double> JetAk04JetBetaStar_out;

    vector<double> JetAk04PartFlav_out;
    vector<double> JetAk04BTagCsv_out;
    vector<double> JetAk04BTagCsvV1_out;
    vector<double> JetAk04BTagCsvSLV1_out;
    vector<double> JetAk04BDiscCisvV2_out;

    // -- rec MET --
    vector<double> METPt_out;
    vector<double> METPhi_out;
    vector<double> METsigx2_out;
    vector<double> METsigy2_out;
    vector<double> METsigxy_out;
    vector<double> METsig_out;
 
   // ---------------------------------------
    outputTree->Branch("EvtInfo_NumVtx", &EvtVtxCnt_out);
    outputTree->Branch("EvtInfo_RunNum", &EvtRunNum_out);
    outputTree->Branch("EvtInfo_LumiNum", &EvtLumiNum_out);
    outputTree->Branch("EvtInfo_EventNum", &EvtNum_out);
    outputTree->Branch("mcEventWeight_", &mcEventWeight_out);

 // cout << " EvtIsRealData~~~~~~~~~~ " << EvtIsRealData << "\n";

    if (cfg_.getS("doWhat")!="DATA") outputTree->Branch("nup_", &GNup_out);
    if (hasRecoInfo == "true" && cfg_.getS("doWhat")!="DATA"){
        outputTree->Branch("PU_npIT", &EvtPuCnt_out);
        outputTree->Branch("PU_npT", &EvtPuCntTruth_out);
    }


    if(hasGenInfo == "true"){

        outputTree->Branch("pdfInfo_", &pdfInfo_);

        // -- bare gen leptons (status 1 = post FSR)-- In Alex code genLep: st1 +st3
        outputTree->Branch("genLepId_", &GLepSt1and3Id_out);
        outputTree->Branch("genLepQ_", &GLepSt1and3Q_out);
        outputTree->Branch("genLepPt_", &GLepSt1and3Pt_out);
        outputTree->Branch("genLepEta_", &GLepSt1and3Eta_out);
        outputTree->Branch("genLepPhi_", &GLepSt1and3Phi_out);
        outputTree->Branch("genLepE_", &GLepSt1and3E_out);
        outputTree->Branch("genLepSt_", &GLepSt1and3St_out);   // always 1 (?)
 
        // -- gen photons --
        outputTree->Branch("genPhoPt_", &GLepClosePhotPt_out);
        outputTree->Branch("genPhoEta_", &GLepClosePhotEta_out);
        outputTree->Branch("genPhoPhi_", &GLepClosePhotPhi_out);

        // --- gen jets ---
        outputTree->Branch("genJetPt_", &GJetAk04Pt_out);
        outputTree->Branch("genJetEta_", &GJetAk04Eta_out);
        outputTree->Branch("genJetPhi_", &GJetAk04Phi_out);
        outputTree->Branch("genJetE_",  &GJetAk04E_out);
        outputTree->Branch("genJetChF_", &GJetAk04ChFrac_out);

    }

    // add reco branches
    // electrons 
    outputTree->Branch("eventElecTrig", &eventElecTrig);
    outputTree->Branch("patElecPt_", &ElPt_out);
    outputTree->Branch("patElecEta_", &ElEta_out);
    outputTree->Branch("patElecPhi_", &ElPhi_out);
    outputTree->Branch("patElecEnergy_", &ElE_out);
    outputTree->Branch("patElecCharge_", &ElCh_out);
    outputTree->Branch("patElecID_", &ElId_out);
    outputTree->Branch("patElecTrig_", &patElecTrig_);
    outputTree->Branch("patElecPfIsoRho_", &ElPfIsoRho_out);
    outputTree->Branch("patElecScEta_", &ElEtaSc_out);

    //  muons 
    outputTree->Branch("eventMuonTrig",&eventMuonTrig);
    outputTree->Branch("patMuonPt_", &MuPt_out);
    outputTree->Branch("patMuonEta_", &MuEta_out);
    outputTree->Branch("patMuonPhi_", &MuPhi_out);
    outputTree->Branch("patMuonVtxZ_", &MuVtxZ_out);
    outputTree->Branch("patMuonEn_", &MuE_out);
    outputTree->Branch("patMuonCharge_", &MuCh_out);
    outputTree->Branch("patMuonDxy_", &MuDxy_out);
    outputTree->Branch("patMuonCombId_", &MuId_out);
    outputTree->Branch("patMuonTrig_", &patMuonTrig_);
    outputTree->Branch("patMuonPfIsoDbeta_", &MuPfIso_out);

    //  jets 
    outputTree->Branch("patJetPfAk05En_", &JetAk04E_out);
    outputTree->Branch("patJetPfAk05Pt_", &JetAk04Pt_out);
    outputTree->Branch("patJetPfAk05Eta_", &JetAk04Eta_out);
    outputTree->Branch("patJetPfAk05Phi_", &JetAk04Phi_out);
    outputTree->Branch("patJetPfAk05LooseId_", &JetAk04Id_out);
    outputTree->Branch("patJetPfAk05jetpuMVA_", &JetAk04PuMva_out);
    // outputTree->Branch("JetAk04ChHadFrac_", &JetAk04ChHadFrac_out);
    outputTree->Branch("patJetPfAk05jetBSZ_", &JetAk04JetBetaStar_out);
    outputTree->Branch("patJetPfAk05jetBZ_", &JetAk04JetBeta_out);
    // which Tag CSV was used in 8 TeV ???
    outputTree->Branch("JetAk04BTagCsv",  &JetAk04BTagCsv_out);  // check this!
    outputTree->Branch("JetAk04BTagCsvV1", &JetAk04BTagCsvV1_out);
    outputTree->Branch("JetAk04BTagCsvSLV1", &JetAk04BTagCsvSLV1_out);
    outputTree->Branch("patJetPfAk05OCSV_", &JetAk04BDiscCisvV2_out);

    outputTree->Branch("patJetPfAk05PartonFlavour_", &JetAk04PartFlav_out);

    // --- reco met ---
    outputTree->Branch("patMetPt_", &METPt_out);
    outputTree->Branch("patMetPhi_", &METPhi_out);
    outputTree->Branch("patMetSig_", &METsig_out);

    //--------------------------------------------------------


    //--------------------------------------------------------
    //-- start loop over the big tree --
    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntries();

    cout << " reco : " <<  hasRecoInfo  << "  Gen:  " << hasGenInfo  << ",  entries (tot): " << nentries << endl;

    if(cfg_.getS("maxNEvent") !="MAX") istringstream (cfg_.getS("maxNEvent"))  >> nentries;   // check if it's correct
     cout << "We run on " <<  nentries << " nentries " << "\n";
    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
       Long64_t ientry = LoadTree(jentry);
       if (ientry < 0) break;
       nb = fChain->GetEntry(jentry);   nbytes += nb;
       // if (Cut(ientry) < 0) continue;

       if (jentry % 10000 == 0) cout << jentry << " of " << nentries << endl; 
       if (DEBUG) cout << " EvtNum : " << EvtNum << endl;
        
       if (!EvtIsRealData) weight_amcNLO_sum += EvtWeights->at(0);
        
        
       EvtVtxCnt_out = -111;
       EvtRunNum_out = -111;
       EvtLumiNum_out = -111;
       EvtNum_out = -111;
       eventMuonTrig = 0 ;
       eventElecTrig = 0 ;
  
       //-- Event info --------------
       EvtVtxCnt_out = EvtVtxCnt;
       EvtRunNum_out = EvtRunNum;
       EvtLumiNum_out = EvtLumiNum;
       EvtNum_out = EvtNum;
       EvtPuCnt_out =  EvtPuCnt;
       EvtPuCntTruth_out = EvtPuCntTruth;
       
       //cout << EvtIsRealData << "\n";
       if (!EvtIsRealData) GNup_out = GNup;
       //-- pdf info --------------
       pdfInfo_.clear();
       mcEventWeight_out.clear();
       if (hasGenInfo == "true"){
            mcEventWeight_out.push_back((double) EvtWeights->at(0));
            //cout << EvtWeights->at(0) << "\n";
            pdfInfo_.push_back((double) GPdfId1->at(0));
            pdfInfo_.push_back((double) GPdfId2->at(0));
            pdfInfo_.push_back((double) GPdfx1->at(0));
            pdfInfo_.push_back((double) GPdfx2->at(0));
            pdfInfo_.push_back((double) GPdfScale->at(0));
       } 

       //-- mcEventWeight_out.clear();
       //-- gen particles --------
       GLepSt1and3Pt_out.clear();
       GLepSt1and3Eta_out.clear();
       GLepSt1and3E_out.clear();
       GLepSt1and3Phi_out.clear();
       GLepSt1and3Id_out.clear();
       GLepSt1and3Q_out.clear();
       GLepSt1and3St_out.clear();
   

       // -- gen photons ---
       GLepClosePhotPt_out.clear();
       GLepClosePhotEta_out.clear();
       GLepClosePhotPhi_out.clear();

       // -- gen jets ---
       GJetAk04Pt_out.clear();
       GJetAk04Eta_out.clear();
       GJetAk04Phi_out.clear();
       GJetAk04E_out.clear();
       GJetAk04ChFrac_out.clear();

       // -- reco electrons ---
       ElPt_out.clear();
       ElEta_out.clear();
       ElPhi_out.clear();
       ElE_out.clear();
       ElCh_out.clear();
       ElId_out.clear();
       ElPfIsoRho_out.clear();
       ElEtaSc_out.clear();
       patElecTrig_.clear();

       // -- reco muons ---
       MuPt_out.clear();
       MuEta_out.clear();
       MuPhi_out.clear();
       MuE_out.clear();
       MuCh_out.clear();
      // patMuonCombId_out.clear();
       patMuonTrig_.clear();
       MuPfIso_out.clear();
       MuVtxZ_out.clear();
       MuDxy_out.clear();
       MuId_out.clear();

       // -- rec jets --
       JetAk04Pt_out.clear();
       JetAk04Eta_out.clear();
       JetAk04Phi_out.clear();
       JetAk04E_out.clear();
       JetAk04Id_out.clear();
       JetAk04PuMva_out.clear();
       JetAk04JetBetaStar_out.clear();
       JetAk04JetBeta_out.clear();

       JetAk04PartFlav_out.clear();
       JetAk04BTagCsv_out.clear();
       JetAk04BTagCsvV1_out.clear();
       JetAk04BTagCsvSLV1_out.clear();
       JetAk04BDiscCisvV2_out.clear();

       // --- reco met ---
       METPt_out.clear();
       METPhi_out.clear();
       METsigx2_out.clear();
       METsigxy_out.clear();
       METsigy2_out.clear();
       METsig_out.clear();


       int genLep = 0;
       int recoLep = 0;
       int countMuon(0), countElec(0), invIsoCountMuon(0), invIsoCountElec(0); 
       if(hasGenInfo == "true"){
             
//--------------- filling gen particles branches --
            // -- fill gen leptons with status 3 (pre FSR) ----
            int GenLepSt03Size(GLepSt3Pt->size());
            for (int i(0); i < GenLepSt03Size; ++i){ 
                // Id of leptons: 11,12,13,14,15,16 
                if ((abs(GLepSt3Id->at(i)) >= 11 &&  abs(GLepSt3Id->at(i)) <= 16) || (abs(GLepSt3Id->at(i)) < 7)) {
                    if (leptonIdSum != 24 && abs(GLepSt3Id->at(i)) == lepID) genLep++; 
                }
                //    double sign = GLepSt3Id->at(i) > 0 ? 1 : -1;
/*
                    // cout << "sign = " << sign << "\n";
                    GLepSt1and3Id_out.push_back((int) GLepSt3Id->at(i)); 
                    GLepSt1and3Q_out.push_back((double) sign);
                    GLepSt1and3Pt_out.push_back((float) GLepSt3Pt->at(i));
                    GLepSt1and3Eta_out.push_back((float) GLepSt3Eta->at(i));
                    GLepSt1and3Phi_out.push_back((float) GLepSt3Phi->at(i));
                    GLepSt1and3E_out.push_back((float) GLepSt3E->at(i));
                    GLepSt1and3St_out.push_back((int) GLepSt3St->at(i));
*/
            } 

            // -- fill bare gen leptons with status 1 ----
            int GenLepBare01Size(GLepBarePt->size());   
            for (int i(0); i < GenLepBare01Size; ++i){         
           
                //cout << GLepBareId->at(i) << "\n";
                 if( abs(GLepBareId->at(i)) >= 11 && (abs(GLepBareId->at(i)) <= 16 || abs(GLepBareId->at(i)) <= 7 )){      
                 double sign = GLepBareId->at(i) > 0 ? 1 : -1;
                     if (fabs(GLepBareMomId->at(i)) != lepID && fabs(GLepBareMomId->at(i)) != 12 && lepID && fabs(GLepBareMomId->at(i)) != 14 && fabs(GLepBareMomId->at(i)) != 16 && fabs(GLepBareMomId->at(i)) != 23 && fabs(GLepBareMomId->at(i)) != 24) continue;  //  leave 6 entries from 26
                    // CommentAG: need to check line above
                    // cout <<  "fabs(GLepBareMomId->at(i))  : " << fabs(GLepBareMomId->at(i))  << "\n";
   
                    GLepSt1and3Id_out.push_back((int) GLepBareId->at(i)); 
                    GLepSt1and3Q_out.push_back((double) sign);
                    GLepSt1and3Pt_out.push_back((float) GLepBarePt->at(i));
                    GLepSt1and3Eta_out.push_back((float) GLepBareEta->at(i));
                    GLepSt1and3Phi_out.push_back((float) GLepBarePhi->at(i));
                    GLepSt1and3E_out.push_back((float) GLepBareE->at(i));
                    GLepSt1and3St_out.push_back((int) GLepBareSt->at(i));
             

                  }

            } // end gen leptons loop


            //-- select the gen photons only --
            int GenPhotonSize(GLepClosePhotPt->size());
            for (int i(0); i < GenPhotonSize; ++i){
                   
                  if(GLepClosePhotSt->at(i) != 1) continue;  // it's always one
                  GLepClosePhotPt_out.push_back((float) GLepClosePhotPt->at(i));
                  GLepClosePhotEta_out.push_back((float) GLepClosePhotEta->at(i));
                  GLepClosePhotPhi_out.push_back((float) GLepClosePhotPhi->at(i));

            } // end gen photons loop

            //-- filling gen jets branches --
  
            int GenJetSize(GJetAk04Pt->size());
            for (int i(0); i < GenJetSize; ++i){
                if(GJetAk04Pt->at(i) > 10){
                    GJetAk04Pt_out.push_back((float) GJetAk04Pt->at(i));
                    GJetAk04Eta_out.push_back((float) GJetAk04Eta->at(i));
                    GJetAk04Phi_out.push_back((float) GJetAk04Phi->at(i));
                    GJetAk04E_out.push_back((float) GJetAk04E->at(i));
                    //  GJetAk04ChFrac_out.push_back((float) GJetAk04ChFrac->at(i));
                }
            } // end gen jets loop

        }  // hasGenInfo


//---------------- GO TO RECOOOOOOOOOOOOO ------------------ 
       if(hasRecoInfo == "true"){

            eventMuonTrig = 0;
            if (leptonIdSum == 13 && TrigHltMu & 1<<17) eventMuonTrig += 1; // single muon HLT_IsoMu24_eta2p1_v2 ??
            if (leptonIdSum == 26 && TrigHlt & 1<<2) eventMuonTrig += 4; // HLT_Mu17_TkMu8_v muon ??
            if (leptonIdSum == 26 && TrigHlt & 1<<1) eventMuonTrig += 8; // HLT_Mu17_Mu8_v muon ??
            //   if (leptonIdSum == 24 && t_bits[4]) eventMuonTrig += 16;
            //   if (leptonIdSum == 24 && t_bits[5]) eventMuonTrig += 32;

            eventElecTrig = 0 ;
            //  if (leptonIdSum == 11 && t_bits[13]) eventElecTrig += 1; // HLT_Ele27_WP80_v
            if (leptonIdSum == 22 && TrigHlt & 1<<0) eventElecTrig += 2; //  Ele17_Ele8
            //  if (leptonIdSum == 24 && t_bits[4]) eventElecTrig += 16; 
            //  if (leptonIdSum == 24 && t_bits[5]) eventElecTrig += 32; 

           // -- fill reco electrons ----
           int recoElecSize(ElPt->size());
           for (int i(0); i < recoElecSize; ++i) {
                
                if(ElPt->at(i) < 5.) continue;
                if (doVector == 23 && !(ElId->at(i) & 4)) continue;   // electron medium id not passed, Alex code: !bits[5]
                if ( (leptonIdSum != 24 && 11 == lepID ) || ( leptonIdSum == 24 ) ) recoLep++;  // sum muons + epectrons
                ElPt_out.push_back((float)ElPt->at(i));
                ElEta_out.push_back((float)ElEta->at(i));
                ElPhi_out.push_back((float)ElPhi->at(i));
                ElE_out.push_back((float)ElE->at(i));
                ElCh_out.push_back((int)ElCh->at(i));

                double elecID = 0 ;
               // cout << ElId->at(i) << "\n"
                //cout << "jentry " << jentry  << " ith lep " << i << " ElId " << ElId->at(i) << endl;
                if (ElId->at(i) & 1<<0) elecID += 1 ; // Veto ??
                if (ElId->at(i) & 1<<1) elecID += 2 ;  // Loose ?
                if (ElId->at(i) & 1<<2) elecID += 4 ; // Medium ??
                if (ElId->at(i) & 1<<3) elecID += 8 ; // Tight ??
                //if ( bits[9]) muonID += 16 ; // HEEP Tight ??
                ElId_out.push_back(elecID);

                ElPfIsoRho_out.push_back((float)ElPfIsoRho->at(i));
                /*
                 cout << "--------" << "\n";
                 if(0 & ElId->at(i)) cout << "veto electrons" << "\n";
                 if(1 & ElId->at(i)) cout << "loose electrons" << "\n";
                 if(2 & ElId->at(i)) cout << "medium electrons" << "\n";
                 if(3 & ElId->at(i)) cout << "tight electrons" << "\n";
                */
                // develop this part elecID = medium, tight, isoPF < 0.15
                double singleElecTrig = 0;
                if ( elecID > 3 && ElPt->at(i) > 15.) {
                        if ( ElPfIsoRho->at(i) < 0.15 ) countElec++;
                        else invIsoCountElec++;
                 }
                 
               // if (leptonIdSum == 11 && TRIGbits[13])  singleElecTrig += 1;  // HLT_Ele27_WP80_v
                if (leptonIdSum == 22 && TrigHlt & 0)   singleElecTrig += 2;  //  // Elec17_Elec8 
               // if (leptonIdSum == 24 && TRIGbits[4])   singleElecTrig += 16; // MuEle ?? 
               // if (leptonIdSum == 24 && TRIGbits[5])   singleElecTrig += 32; // EleMu ?? 
                patElecTrig_.push_back((double) singleElecTrig);
             
                ElEtaSc_out.push_back((float)ElEtaSc->at(i));
             
           } // end reco electrons loop

           // -- fill reco muons ----
           int recoMuonSize(MuPt->size());
           for (int i(0); i < recoMuonSize; ++i) {

                if(MuPt->at(i) < 5) continue;  
                // if (doVector == 23 && !(MuIdTight->at(i) & 1) ) continue;  // muon tight id not passed, Alex code: !bits[10] 
                if (!(MuIdTight->at(i) & 1) ) continue;  // This is just to check!!!!!!!

                if ( (leptonIdSum != 24 && 13 == lepID ) || ( leptonIdSum == 24 ) ) recoLep++; // sum muons + epectrons
                MuPt_out.push_back((float)MuPt->at(i));
                MuEta_out.push_back((float)MuEta->at(i));
                MuPhi_out.push_back((float)MuPhi->at(i));
                MuE_out.push_back((float)MuE->at(i));
                MuCh_out.push_back((int)MuCh->at(i));
                MuDxy_out.push_back((float)MuDxy->at(i));
                //  patMuonCombId_out.push_back((int)patMuonCombId->at(i));  //?
                MuPfIso_out.push_back((float)MuPfIso->at(i));   // ?  
              //  MuId_out.push_back((int)MuId->at(i));

                int  muonID = 0.;
                //cout << "jentry " << jentry  << " ith lep " << i << " MuIdTight " << MuIdTight->at(i) << endl;
               
                 if((MuIdTight->at(i) & 1)) muonID += 1 ;
                 else muonID += 2 ;
                 MuId_out.push_back(muonID);

                if((MuIdTight->at(i) & 1) && MuPt->at(i) > 15.){
                    if(MuPfIso->at(i) < 0.2)   countMuon++;
                    else invIsoCountMuon++;
                }


               double muonTrig = 0 ;
               //cout << TrigHlt << "\n";
               /*
                if (leptonIdSum == 13 && TrigHlt & 8 ) muonTrig += 8 ;  // single HLT_IsoMu24_eta2p1_LooseIsoPFTau20_v2
                if (leptonIdSum == 13 && TrigHlt & 14 ) muonTrig += 14 ; // HLT_IsoMu24_eta2p1_CentralPFJet30_BTagCSV07_v2
                if (leptonIdSum == 13 && TrigHlt & 15 ) muonTrig += 15 ; // HLT_IsoMu24_eta2p1_TriCentralPFJet30_v2
                if (leptonIdSum == 13 && TrigHlt & 16 ) muonTrig += 16 ;  //  HLT_IsoMu24_eta2p1_TriCentralPFJet50_40_30_v2
                if (leptonIdSum == 13 && TrigHlt & 17 ) muonTrig += 17 ;  // HLT_IsoMu24_eta2p1_v2
                */
               if (leptonIdSum == 13 && TrigHltMu & 1<<17) muonTrig += 1 ; // single muon HLT_IsoMu24_eta2p1_v ??
               if (leptonIdSum == 26 && TrigHlt & 1<<2) muonTrig += 4 ; // HLT_Mu17_TkMu8_v muon ??
               //if (leptonIdSum == 26 && TrigHlt & 1<<1) muonTrig += 8 ; // HLT_Mu17_Mu8_v muon ??
               // if (leptonIdSum == 24 && TRIGbits[4]) muonTrig += 16 ; // MuEle ?? 
               // if (leptonIdSum == 24 && TRIGbits[5]) muonTrig += 32 ; // EleMu ?? 
               // cout << muonTrig << "\n";
                patMuonTrig_.push_back(muonTrig);
                MuVtxZ_out.push_back((float)MuVtxZ->at(i));
             
           } // end reco muons loop 



     
           // -- fill reco jets ----
           int patJetSize(JetAk04Pt->size());
           for (int i(0); i < patJetSize; ++i){
   
                if(JetAk04Pt->at(i) < 15.) continue ;
                JetAk04E_out.push_back((float) JetAk04E->at(i));
                JetAk04Pt_out.push_back((float) JetAk04Pt->at(i));
                JetAk04Eta_out.push_back((float) JetAk04Eta->at(i));
                JetAk04Phi_out.push_back((float) JetAk04Phi->at(i));
                JetAk04Id_out.push_back((int) JetAk04Id->at(i));  // ?
                JetAk04PuMva_out.push_back((float) JetAk04PuMva->at(i)); //? 
                //   JetAk04JetBetaStar_out.push_back((float) JetAk04JetBetaStar->at(i));
                //   JetAk04JetBeta_out.push_back((float) JetAk04JetBeta->at(i));

                JetAk04PartFlav_out.push_back((float) JetAk04PartFlav->at(i));
                JetAk04BTagCsv_out.push_back((float) JetAk04BTagCsv->at(i));
                JetAk04BTagCsvV1_out.push_back((float) JetAk04BTagCsvV1->at(i));
                JetAk04BTagCsvSLV1_out.push_back((float) JetAk04BTagCsvSLV1->at(i));
                JetAk04BDiscCisvV2_out.push_back((float) JetAk04BDiscCisvV2->at(i));


            } // end reco jets loop

            int METSize(METPt->size());
            for (int i(0); i < METSize; ++i){
                TLorentzVector tmp;
                tmp.SetPxPyPzE(METPx->at(i), METPy->at(i), METPz->at(i), METE->at(i));
                METPt_out.push_back((float) METPt->at(i));
                METPhi_out.push_back((float) tmp.Phi());
                // cout << tmp.Pt() << " , " << METPt->at(i) << "\n"; 
                METsigx2_out.push_back((float) METsigx2->at(i));
                METsigxy_out.push_back((float) METsigxy->at(i));
                METsigy2_out.push_back((float) METsigy2->at(i));
                METsig_out.push_back((float) METsig->at(i));
            }

        } // end hasRecoInfo

        // if ZJets signal: require at least two gen muons from Z (no cuts on rec for Unfolding)
        // if ZJets background: require at least two rec muons (do we need gen info here?)
        // if WJets signal: at least one gen muon/electron and one neutrino from W
        // if WJets background: MET and at least one reco muon/electron 


          //cout << MuPt_out.size() << " , " << GLepSt1and3Pt_out.size() << "\n";
         //-- fill the output tree --
          if (cfg_.getS("doUnfold") == "false" && leptonIdSum == 24 && ElPt_out.size() < 1 && MuPt_out.size() < 1) continue;// at least one letons on RECO
          if (cfg_.getS("doUnfold") == "false" && ((leptonIdSum == 13 && MuPt_out.size() < 1) || (leptonIdSum == 11 && ElPt_out.size() < 1) || ((leptonIdSum == 11 || leptonIdSum == 13) && ( countElec + countMuon) > 1) ) )  continue ; //running on single electrons
          if ( cfg_.getS("doUnfold") == "false" && leptonIdSum == 24 && (ElPt_out.size() < 1 || MuPt_out.size() < 1) ) continue ;
          if ( cfg_.getS("doUnfold") == "false" && leptonIdSum == 22 && ElPt_out.size() < 2 ) continue ;
          if ( cfg_.getS("doUnfold") == "false" && leptonIdSum == 26 && MuPt_out.size() < 2 ) continue ;
        
          //cout  << genLep << "\n";
   
          if ((leptonIdSum == 11 || leptonIdSum == 13) && cfg_.getS("doUnfold") == "true" && recoLep < 1 && genLep < 1) continue;
          if ((leptonIdSum == 22 || leptonIdSum == 26) && cfg_.getS("doUnfold") == "true" && recoLep < 2 && genLep < 2) continue;

          // cout << "muon size" << MuEta_out.size() << "\n";
          //cout << "muon pt" << MuPt_out << "\n";
          //cout << "EvtVtxCnt_out " << EvtVtxCnt_out << "\n";
          //cout << "EvtNum_out " << EvtNum_out << "\n";

         // cout << "GLepSt1and3Eta_out : " << GLepSt1and3Eta_out.size() << "\n";

          outputTree->Fill();
          passedEvents++;
          if (DEBUG) cout << "Passes###  " <<  EvtNum << endl;
   }

   //-- save the output tree --
   outputTree->Write();
   cout << " Passed netries =  " << passedEvents << endl;
   cout << " weight_amcNLO_sum = " << weight_amcNLO_sum << "\n";
   cout << " Saving the tree " << endl;
    //  delete outputFile;
   outputFile->Close();
}

Int_t BonzaiMaker::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t BonzaiMaker::LoadTree(Long64_t entry)
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

void BonzaiMaker::Init(TChain *fChain)
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
   PhotSigmaIetaIeta = 0;
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
   fCurrent = -1;
   fChain->SetMakeClass(1);

  fChain->SetBranchAddress("EvtWeights", &EvtWeights, &b_EvtWeights);

   // if(anaSel_ == "WJETS"){
   fChain->SetBranchAddress("METPt", &METPt, &b_METPt);
   fChain->SetBranchAddress("METPx", &METPx, &b_METPx);
   fChain->SetBranchAddress("METPy", &METPy, &b_METPy);
   fChain->SetBranchAddress("METPz", &METPz, &b_METPz);
   fChain->SetBranchAddress("METE", &METE, &b_METE);
   fChain->SetBranchAddress("METsigx2", &METsigx2, &b_METsigx2);
   fChain->SetBranchAddress("METsigxy", &METsigxy, &b_METsigxy);
   fChain->SetBranchAddress("METsigy2", &METsigy2, &b_METsigy2);
   fChain->SetBranchAddress("METsig", &METsig, &b_METsig);

   fChain->SetBranchAddress("EvtIsRealData", &EvtIsRealData, &b_EvtIsRealData);
   fChain->SetBranchAddress("EvtNum", &EvtNum, &b_EvtNum);
   fChain->SetBranchAddress("EvtRunNum", &EvtRunNum, &b_EvtRunNum);
   fChain->SetBranchAddress("EvtLumiNum", &EvtLumiNum, &b_EvtLumiNum);
   fChain->SetBranchAddress("EvtPuCnt", &EvtPuCnt, &b_EvtPuCnt);
   fChain->SetBranchAddress("EvtPuCntTruth", &EvtPuCntTruth, &b_EvtPuCntTruth);
   fChain->SetBranchAddress("EvtVtxCnt", &EvtVtxCnt, &b_EvtVtxCnt);

   fChain->SetBranchAddress("TrigHlt", &TrigHlt, &b_TrigHlt);
   fChain->SetBranchAddress("TrigHltMu", &TrigHltMu, &b_TrigHltMu);
/*
   fChain->SetBranchAddress("GLepDr01Pt", &GLepDr01Pt, &b_GLepDr01Pt);
   fChain->SetBranchAddress("GLepDr01Eta", &GLepDr01Eta, &b_GLepDr01Eta);
   fChain->SetBranchAddress("GLepDr01Phi", &GLepDr01Phi, &b_GLepDr01Phi);
   fChain->SetBranchAddress("GLepDr01Id", &GLepDr01Id, &b_GLepDr01Id);
   fChain->SetBranchAddress("GLepDr01MomId", &GLepDr01MomId, &b_GLepDr01MomId);
*/
   fChain->SetBranchAddress("GLepBarePt", &GLepBarePt, &b_GLepBarePt);
   fChain->SetBranchAddress("GLepBareEta", &GLepBareEta, &b_GLepBareEta);
   fChain->SetBranchAddress("GLepBarePhi", &GLepBarePhi, &b_GLepBarePhi);
   fChain->SetBranchAddress("GLepBareId", &GLepBareId, &b_GLepBareId);
   fChain->SetBranchAddress("GLepBareMomId", &GLepBareMomId, &b_GLepBareMomId);
   fChain->SetBranchAddress("GLepBareE", &GLepBareE, &b_GLepBareE);
   fChain->SetBranchAddress("GLepBareSt", &GLepBareSt, &b_GLepBareSt);

   fChain->SetBranchAddress("GLepSt3Pt", &GLepSt3Pt, &b_GLepSt3Pt);
   fChain->SetBranchAddress("GLepSt3Eta", &GLepSt3Eta, &b_GLepSt3Eta);
   fChain->SetBranchAddress("GLepSt3Phi", &GLepSt3Phi, &b_GLepSt3Phi);
   fChain->SetBranchAddress("GLepSt3E", &GLepSt3E, &b_GLepSt3E);
   fChain->SetBranchAddress("GLepSt3Id", &GLepSt3Id, &b_GLepSt3Id);
   fChain->SetBranchAddress("GLepSt3St", &GLepSt3St, &b_GLepSt3St);


   fChain->SetBranchAddress("GLepClosePhotPt", &GLepClosePhotPt, &b_GLepClosePhotPt);
   fChain->SetBranchAddress("GLepClosePhotEta", &GLepClosePhotEta, &b_GLepClosePhotEta);
   fChain->SetBranchAddress("GLepClosePhotPhi", &GLepClosePhotPhi, &b_GLepClosePhotPhi);
   fChain->SetBranchAddress("GLepClosePhotSt", &GLepClosePhotSt, &b_GLepClosePhotSt);

   fChain->SetBranchAddress("GJetAk04Pt", &GJetAk04Pt, &b_GJetAk04Pt);
   fChain->SetBranchAddress("GJetAk04Eta", &GJetAk04Eta, &b_GJetAk04Eta);
   fChain->SetBranchAddress("GJetAk04Phi", &GJetAk04Phi, &b_GJetAk04Phi);
   fChain->SetBranchAddress("GJetAk04E", &GJetAk04E, &b_GJetAk04E);

   fChain->SetBranchAddress("GJetAk04ChFrac", &GJetAk04ChFrac, &b_GJetAk04ChFrac);

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
   fChain->SetBranchAddress("MuPfIso", &MuPfIso, &b_MuPfIso);

   fChain->SetBranchAddress("GPdfId1", &GPdfId1, &b_GPdfId1);
   fChain->SetBranchAddress("GPdfId2", &GPdfId2, &b_GPdfId2);
   fChain->SetBranchAddress("GPdfx1", &GPdfx1, &b_GPdfx1);
   fChain->SetBranchAddress("GPdfx2", &GPdfx2, &b_GPdfx2);
   fChain->SetBranchAddress("GPdfScale", &GPdfScale, &b_GPdfScale);

   fChain->SetBranchAddress("ElPt", &ElPt, &b_ElPt);
   fChain->SetBranchAddress("ElEta", &ElEta, &b_ElEta);
   fChain->SetBranchAddress("ElEtaSc", &ElEtaSc, &b_ElEtaSc);
   fChain->SetBranchAddress("ElPhi", &ElPhi, &b_ElPhi);
   fChain->SetBranchAddress("ElE", &ElE, &b_ElE);
   fChain->SetBranchAddress("ElId", &ElId, &b_ElId);
   fChain->SetBranchAddress("ElCh", &ElCh, &b_ElCh);
   fChain->SetBranchAddress("ElPfIsoRho", &ElPfIsoRho, &b_ElPfIsoRho);

   fChain->SetBranchAddress("JetAk04Pt", &JetAk04Pt, &b_JetAk04Pt);
   fChain->SetBranchAddress("JetAk04Eta", &JetAk04Eta, &b_JetAk04Eta);
   fChain->SetBranchAddress("JetAk04Phi", &JetAk04Phi, &b_JetAk04Phi);
   fChain->SetBranchAddress("JetAk04E", &JetAk04E, &b_JetAk04E);
   fChain->SetBranchAddress("JetAk04Id", &JetAk04Id, &b_JetAk04Id);
   fChain->SetBranchAddress("JetAk04PuMva", &JetAk04PuMva, &b_JetAk04PuMva);
 //  fChain->SetBranchAddress("JetAk04ChHadFrac", &JetAk04ChHadFrac, &b_JetAk04ChHadFrac);

   fChain->SetBranchAddress("JetAk04JetBeta", &JetAk04JetBeta, &b_JetAk04JetBeta);
 //  fChain->SetBranchAddress("JetAk04JetBetaClassic", &JetAk04JetBetaClassic, &b_JetAk04JetBetaClassic);
   fChain->SetBranchAddress("JetAk04JetBetaStar", &JetAk04JetBetaStar, &b_JetAk04JetBetaStar);
 //  fChain->SetBranchAddress("JetAk04JetBetaStarClassic", &JetAk04JetBetaStarClassic, &b_JetAk04JetBetaStarClassic);

   fChain->SetBranchAddress("JetAk04BTagCsv", &JetAk04BTagCsv, &b_JetAk04BTagCsv);
   fChain->SetBranchAddress("JetAk04BTagCsvV1", &JetAk04BTagCsvV1, &b_JetAk04BTagCsvV1);
   fChain->SetBranchAddress("JetAk04BTagCsvSLV1", &JetAk04BTagCsvSLV1, &b_JetAk04BTagCsvSLV1);
   fChain->SetBranchAddress("JetAk04BDiscCisvV2", &JetAk04BDiscCisvV2, &b_JetAk04BDiscCisvV2);

   fChain->SetBranchAddress("JetAk04PartFlav", &JetAk04PartFlav, &b_JetAk04PartFlav);
//------ no need-----------------------------------

/*
   fChain->SetBranchAddress("EvtBxNum", &EvtBxNum, &b_EvtBxNum);
  

 
   fChain->SetBranchAddress("EvtFastJetRho", &EvtFastJetRho, &b_EvtFastJetRho);

   fChain->SetBranchAddress("GLepDr01E", &GLepDr01E, &b_GLepDr01E);
   fChain->SetBranchAddress("GLepDr01St", &GLepDr01St, &b_GLepDr01St);


 
   fChain->SetBranchAddress("GLepSt3Mother0Id", &GLepSt3Mother0Id, &b_GLepSt3Mother0Id);
   fChain->SetBranchAddress("GLepSt3MotherCnt", &GLepSt3MotherCnt, &b_GLepSt3MotherCnt);

   fChain->SetBranchAddress("GLepClosePhotE", &GLepClosePhotE, &b_GLepClosePhotE);
   fChain->SetBranchAddress("GLepClosePhotId", &GLepClosePhotId, &b_GLepClosePhotId);
   fChain->SetBranchAddress("GLepClosePhotMother0Id", &GLepClosePhotMother0Id, &b_GLepClosePhotMother0Id);
   fChain->SetBranchAddress("GLepClosePhotMotherCnt", &GLepClosePhotMotherCnt, &b_GLepClosePhotMotherCnt);


   fChain->SetBranchAddress("GJetAk04ConstCnt", &GJetAk04ConstCnt, &b_GJetAk04ConstCnt);
   fChain->SetBranchAddress("GJetAk04ConstId", &GJetAk04ConstId, &b_GJetAk04ConstId);
   fChain->SetBranchAddress("GJetAk04ConstPt", &GJetAk04ConstPt, &b_GJetAk04ConstPt);
   fChain->SetBranchAddress("GJetAk04ConstEta", &GJetAk04ConstEta, &b_GJetAk04ConstEta);
   fChain->SetBranchAddress("GJetAk04ConstPhi", &GJetAk04ConstPhi, &b_GJetAk04ConstPhi);
   fChain->SetBranchAddress("GJetAk04ConstE", &GJetAk04ConstE, &b_GJetAk04ConstE);

   fChain->SetBranchAddress("GBinningValue", &GBinningValue, &b_GBinningValue);
   
   fChain->SetBranchAddress("MuIsoRho", &MuIsoRho, &b_MuIsoRho);

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
   fChain->SetBranchAddress("PhotSigmaIetaIeta", &PhotSigmaIetaIeta, &b_PhotSigmaIetaIeta);
   fChain->SetBranchAddress("PhotHoE", &PhotHoE, &b_PhotHoE);
   fChain->SetBranchAddress("PhotId", &PhotId, &b_PhotId);
   fChain->SetBranchAddress("PhotHasPixelSeed", &PhotHasPixelSeed, &b_PhotHasPixelSeed);

   fChain->SetBranchAddress("JetAk04PuId", &JetAk04PuId, &b_JetAk04PuId);
 
   fChain->SetBranchAddress("JetAk04RawPt", &JetAk04RawPt, &b_JetAk04RawPt);
   fChain->SetBranchAddress("JetAk04RawE", &JetAk04RawE, &b_JetAk04RawE);
   fChain->SetBranchAddress("JetAk04HfHadE", &JetAk04HfHadE, &b_JetAk04HfHadE);
   fChain->SetBranchAddress("JetAk04HfEmE", &JetAk04HfEmE, &b_JetAk04HfEmE);
  
   fChain->SetBranchAddress("JetAk04NeutralHadAndHfFrac", &JetAk04NeutralHadAndHfFrac, &b_JetAk04NeutralHadAndHfFrac);
   fChain->SetBranchAddress("JetAk04ChEmFrac", &JetAk04ChEmFrac, &b_JetAk04ChEmFrac);
   fChain->SetBranchAddress("JetAk04NeutralEmFrac", &JetAk04NeutralEmFrac, &b_JetAk04NeutralEmFrac);
   fChain->SetBranchAddress("JetAk04ChMult", &JetAk04ChMult, &b_JetAk04ChMult);
   fChain->SetBranchAddress("JetAk04ConstCnt", &JetAk04ConstCnt, &b_JetAk04ConstCnt);


   fChain->SetBranchAddress("JetAk04BDiscJp", &JetAk04BDiscJp, &b_JetAk04BDiscJp);
   fChain->SetBranchAddress("JetAk04BDiscBjp", &JetAk04BDiscBjp, &b_JetAk04BDiscBjp);
   fChain->SetBranchAddress("JetAk04BDiscTche", &JetAk04BDiscTche, &b_JetAk04BDiscTche);
   fChain->SetBranchAddress("JetAk04BDiscTchp", &JetAk04BDiscTchp, &b_JetAk04BDiscTchp);
   fChain->SetBranchAddress("JetAk04BDiscSsvhe", &JetAk04BDiscSsvhe, &b_JetAk04BDiscSsvhe);
   fChain->SetBranchAddress("JetAk04BDiscSsvhp", &JetAk04BDiscSsvhp, &b_JetAk04BDiscSsvhp);
 
   fChain->SetBranchAddress("JetAk04JecUncUp", &JetAk04JecUncUp, &b_JetAk04JecUncUp);
   fChain->SetBranchAddress("JetAk04JecUncDwn", &JetAk04JecUncDwn, &b_JetAk04JecUncDwn);
   fChain->SetBranchAddress("JetAk04ConstId", &JetAk04ConstId, &b_JetAk04ConstId);
   fChain->SetBranchAddress("JetAk04ConstPt", &JetAk04ConstPt, &b_JetAk04ConstPt);
   fChain->SetBranchAddress("JetAk04ConstEta", &JetAk04ConstEta, &b_JetAk04ConstEta);
   fChain->SetBranchAddress("JetAk04ConstPhi", &JetAk04ConstPhi, &b_JetAk04ConstPhi);
   fChain->SetBranchAddress("JetAk04ConstE", &JetAk04ConstE, &b_JetAk04ConstE);
   fChain->SetBranchAddress("JetAk04GenJet", &JetAk04GenJet, &b_JetAk04GenJet);
*/
   Notify();
}

Bool_t BonzaiMaker::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void BonzaiMaker::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t BonzaiMaker::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
