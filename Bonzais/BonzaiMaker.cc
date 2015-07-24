#define BonzaiMaker_cxx
#include "BonzaiMaker.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TBranch.h>
#include <TFile.h>
#include <TTree.h>
#include <TLorentzVector.h>
#include <iostream>
#include <TMath.h>
#include <bitset>
#include <vector>

#define DEBUG 0
using namespace std;


void BonzaiMaker::Loop(string dataset_, string fileName_, int leptonIdSum, bool doUnfold, bool hasRecoInfo, bool hasGenInfo)
{
    //--------------------------------------------------------
    //-- output file --
    cout << "Input is: " << storageElement << dataset_ << endl;
    string command = "mkdir -p " + dataset_;
    system(command.c_str());
    string outFileRoot = dataset_ + "/" + fileName_ ;
    cout << "Output is: " << outFileRoot << endl;
    TFile *outputFile = new TFile(outFileRoot.c_str(), "recreate");

    //-- subdirectory to match our ntuples sturcture --
    TDirectory *cdtree = outputFile->mkdir("tree");
    cdtree->cd();

    //-- output tree --
    TTree *outputTree = new TTree("tree", "tree");

    // statistics 
    int passedEvents = 0;
    //--------------------------------------------------------

    //--------------------------------------------------------
    //-- output tree containt --
    // boson vector id. This is overwritten according to leptonIdSum
    // 23: Z boson; 24: W boson
    int doVector = 23; 

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

    cout << " leptons are " << lepID << " leptonIdSum " << leptonIdSum << endl;

    // Here we check if it is MC or Data according to dataset_
    bool isMC = false;
    if (dataset_.find("MC") != string::npos) isMC = true;

    double PU_npT(0);
    double PU_npIT(0);

    int EvtInfo_NumVtx = -111;
    int EvtInfo_RunNum = -111;
    int EvtInfo_LumiNum = -111;
    int EvtInfo_EventNum = -111;

    int nup_;
    vector<double> pdfInfo_;

    vector<double> genLepId_;
    vector<double> genLepSt_;
    vector<double> genLepQ_;
    vector<double> genLepPt_;
    vector<double> genLepEta_;
    vector<double> genLepPhi_;
    vector<double> genLepE_;

    vector<double> genPhoPt_;
    vector<double> genPhoEta_;
    vector<double> genPhoPhi_;

    vector<double> genJetPt_;
    vector<double> genJetEta_;
    vector<double> genJetPhi_;
    vector<double> genJetE_;
    vector<double> genJetChF_;

    int eventElecTrig;
    vector<double> patElecPt_;
    vector<double> patElecEta_;
    vector<double> patElecPhi_;
    vector<double> patElecEn_;
    vector<double> patElecCharge_;
    vector<double> patElecID_;
    vector<double> patElecTrig_;
    vector<double> patElecDetIso_;
    vector<double> patElecPfIsoRho_;
    vector<double> patElecScEta_;

    int eventMuonTrig;
    vector<double> patMuonPt_;
    vector<double> patMuonEta_;
    vector<double> patMuonPhi_;
    vector<double> patMuonVtxZ_;
    vector<double> patMuonEn_;
    vector<double> patMuonCharge_;
    vector<double> patMuonDxy_;
    vector<int> patMuonCombId_;
    vector<double> patMuonTrig_;
    vector<double> patMuonDetIsoRho_;
    vector<double> patMuonPfIsoDbeta_;

    vector<double> patJetPfAk05En_;
    vector<double> patJetPfAk05Pt_;
    vector<double> patJetPfAk05Eta_;
    vector<double> patJetPfAk05Phi_;
    vector<double> patJetPfAk05JesUncert_;
    vector<double> patJetPfAk05LooseId_;
    vector<double> patJetPfAk05jetBSZ_;
    vector<double> patJetPfAk05jetBZ_;
    vector<double> patJetPfAk05jetpuMVA_;
    vector<double> patJetPfAk05OCSV_;
    vector<double> patJetPfAk05PartonFlavour_;

    vector<double> patMetPt_ ;
    vector<double> patMetPhi_;
    vector<double> patMetSig_;
    vector<double> patMetSigXX_;
    vector<double> patMetSigXY_;
    vector<double> patMetSigYY_;


    outputTree->Branch("EvtInfo_NumVtx", &EvtInfo_NumVtx);
    outputTree->Branch("EvtInfo_RunNum", &EvtInfo_RunNum);
    outputTree->Branch("EvtInfo_LumiNum", &EvtInfo_LumiNum);
    outputTree->Branch("EvtInfo_EventNum", &EvtInfo_EventNum);
    if (isMC) outputTree->Branch("nup_", &nup_);
    if (hasRecoInfo && isMC){
        outputTree->Branch("PU_npT", &PU_npT);
        outputTree->Branch("PU_npIT", &PU_npIT);
    }

    if (hasGenInfo){
        outputTree->Branch("pdfInfo_", &pdfInfo_);

        outputTree->Branch("genLepId_", &genLepId_);
        outputTree->Branch("genLepSt_", &genLepSt_);
        outputTree->Branch("genLepQ_", &genLepQ_);
        outputTree->Branch("genLepPt_", &genLepPt_);
        outputTree->Branch("genLepEta_", &genLepEta_);
        outputTree->Branch("genLepPhi_", &genLepPhi_);
        outputTree->Branch("genLepE_", &genLepE_);

        outputTree->Branch("genPhoPt_", &genPhoPt_);
        outputTree->Branch("genPhoEta_", &genPhoEta_);
        outputTree->Branch("genPhoPhi_", &genPhoPhi_);

        outputTree->Branch("genJetPt_", &genJetPt_ );
        outputTree->Branch("genJetEta_", &genJetEta_);
        outputTree->Branch("genJetPhi_", &genJetPhi_);
        outputTree->Branch("genJetE_", &genJetE_ );
        outputTree->Branch("genJetChF_", &genJetChF_ );
    }
    // add reco branches

    outputTree->Branch("eventElecTrig", &eventElecTrig );
    outputTree->Branch("patElecPt_", &patElecPt_);
    outputTree->Branch("patElecEta_", &patElecEta_);
    outputTree->Branch("patElecPhi_", &patElecPhi_);
    outputTree->Branch("patElecEnergy_", &patElecEn_);
    outputTree->Branch("patElecCharge_", &patElecCharge_);
    outputTree->Branch("patElecID_",&patElecID_);
    outputTree->Branch("patElecTrig_", &patElecTrig_);
    // outputTree->Branch("patElecDetIso_", &patElecDetIso_);
    outputTree->Branch("patElecPfIsoRho_", &patElecPfIsoRho_);
    outputTree->Branch("patElecScEta_",&patElecScEta_);


    // muons
    outputTree->Branch("eventMuonTrig",&eventMuonTrig);
    outputTree->Branch("patMuonPt_", &patMuonPt_);
    outputTree->Branch("patMuonEta_", &patMuonEta_);
    outputTree->Branch("patMuonPhi_", &patMuonPhi_);
    outputTree->Branch("patMuonVtxZ_", &patMuonVtxZ_);
    outputTree->Branch("patMuonEn_", &patMuonEn_);
    outputTree->Branch("patMuonCharge_", &patMuonCharge_);
    outputTree->Branch("patMuonDxy_", &patMuonDxy_);
    outputTree->Branch("patMuonCombId_", &patMuonCombId_);
    outputTree->Branch("patMuonTrig_", &patMuonTrig_);
    //outputTree->Branch("patMuonDetIsoRho_", &patMuonDetIsoRho_);
    outputTree->Branch("patMuonPfIsoDbeta_", &patMuonPfIsoDbeta_);


    // jet
    outputTree->Branch("patJetPfAk05En_", &patJetPfAk05En_);
    outputTree->Branch("patJetPfAk05Pt_", &patJetPfAk05Pt_);
    outputTree->Branch("patJetPfAk05Eta_", &patJetPfAk05Eta_);
    outputTree->Branch("patJetPfAk05Phi_", &patJetPfAk05Phi_);
    //outputTree->Branch("patJetPfAk05JesUncert_", &patJetPfAk05JesUncert_); /// for now not computed ; maybe tables to save some space
    outputTree->Branch("patJetPfAk05LooseId_", &patJetPfAk05LooseId_);
    outputTree->Branch("patJetPfAk05jetBSZ_", &patJetPfAk05jetBSZ_);
    outputTree->Branch("patJetPfAk05jetBZ_", &patJetPfAk05jetBZ_);
    outputTree->Branch("patJetPfAk05jetpuMVA_", &patJetPfAk05jetpuMVA_);
    outputTree->Branch("patJetPfAk05OCSV_", &patJetPfAk05OCSV_);
    outputTree->Branch("patJetPfAk05PartonFlavour_", &patJetPfAk05PartonFlavour_);


    // MET
    //outputTree->Branch("patMetSumEt_",&patMetSumEt_);
    outputTree->Branch("patMetPt_",&patMetPt_);
    outputTree->Branch("patMetPhi_",&patMetPhi_);
    outputTree->Branch("patMetSig_",&patMetSig_);
    //outputTree->Branch("patMetSigXX_",&patMetSigXX_);
    //outputTree->Branch("patMetSigXY_",&patMetSigXY_);
    //outputTree->Branch("patMetSigYY_",&patMetSigYY_);

    //--------------------------------------------------------


    //--------------------------------------------------------
    //-- start loop over the big tree --
    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntries();

    cout << " reco : " <<hasRecoInfo << "  Gen:  " << hasGenInfo << " entries: " << nentries << endl;


    Long64_t nbytes = 0, nb = 0;
    //nentries = 100;
    for (Long64_t jentry = 0; jentry < nentries; jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        // if (Cut(ientry) < 0) continue;

        if (jentry % 10000 == 0) cout << jentry << " of " << nentries << endl; 
        if (DEBUG) cout << " event : " << event << endl;
        EvtInfo_NumVtx = -111;
        EvtInfo_RunNum = -111;
        EvtInfo_LumiNum = -111;
        EvtInfo_EventNum = -111;
        eventMuonTrig = 0 ;
        eventElecTrig = 0 ;

        //-- Event info --------------
        PU_npT  = ngenTruepu ;
        PU_npIT = ngenITpu;
        EvtInfo_NumVtx = nvtx;
        EvtInfo_RunNum = run;
        EvtInfo_LumiNum = lumi;
        EvtInfo_EventNum = event;

        if ( isMC ) 	nup_ = nup;
        //-- pdf info --------------
        pdfInfo_.clear();
        if ( hasGenInfo ){
            pdfInfo_.push_back((double) id1);
            pdfInfo_.push_back((double) id2);
            pdfInfo_.push_back((double) x1);
            pdfInfo_.push_back((double) x2);
            pdfInfo_.push_back((double) qscale);
        }
        //-- gen particles --------
        genLepId_.clear();
        genLepSt_.clear();
        genLepQ_.clear();
        genLepPt_.clear();
        genLepEta_.clear();
        genLepPhi_.clear();
        genLepE_.clear();

        // -- gen photons ---
        genPhoPt_.clear();
        genPhoEta_.clear();
        genPhoPhi_.clear();

        // -- gen jets ---
        genJetPt_.clear();
        genJetEta_.clear();
        genJetPhi_.clear();
        genJetE_.clear();
        genJetChF_.clear();

        //  -- reco electrons -- 
        patElecPt_.clear();
        patElecEta_.clear();
        patElecPhi_.clear();
        patElecEn_.clear();
        patElecCharge_.clear();
        patElecID_.clear();
        patElecTrig_.clear();
        patElecDetIso_.clear();
        patElecPfIsoRho_.clear();
        patElecScEta_.clear();
        // -- reco muons ---
        patMuonPt_.clear();
        patMuonEta_.clear();
        patMuonPhi_.clear();
        patMuonEn_.clear();
        patMuonCharge_.clear();
        patMuonDxy_.clear();
        patMuonVtxZ_.clear();
        patMuonCombId_.clear();
        patMuonTrig_.clear();
        patMuonPfIsoDbeta_.clear();

        // -- rec jets ---
        patJetPfAk05En_.clear();
        patJetPfAk05Pt_.clear();
        patJetPfAk05Eta_.clear();
        patJetPfAk05Phi_.clear();
        patJetPfAk05JesUncert_.clear();
        patJetPfAk05LooseId_.clear();
        patJetPfAk05jetBSZ_.clear();
        patJetPfAk05jetBZ_.clear();
        patJetPfAk05jetpuMVA_.clear();
        patJetPfAk05OCSV_.clear();
        patJetPfAk05PartonFlavour_.clear();

        // --- reco met ---
        patMetPt_.clear() ;
        patMetPhi_.clear() ;
        patMetSig_.clear() ;
        patMetSigXX_.clear() ;
        patMetSigXY_.clear() ;
        patMetSigYY_.clear() ;


        int genLep = 0;
        int recoLep = 0;
        int countMuon(0), countElec(0), invIsoCountMuon(0),invIsoCountElec(0);
        if (hasGenInfo) {
            //		cout << " Event:"<< endl;
            vector<double> genPhoPtDupl_;
            vector<double> genPhoEtaDupl_;
            vector<double> genPhoPhiDupl_;

            //-- filling gen particles branches --
            for (int i(0); i < mcn; i++) {
                //-- select the gen leptons only --
                if ((abs(mc_id[i]) >= 11 &&  abs(mc_id[i]) <= 16) || (abs(mc_id[i]) < 7)) {
                    if (leptonIdSum != 24 && abs(mc_id[i]) == lepID && mc_status[i] == 3) genLep++;

                    TLorentzVector tmp;
                    tmp.SetPxPyPzE(mc_px[i], mc_py[i], mc_pz[i], mc_en[i]);
                    double sign = mc_id[i] > 0 ? 1 : -1;
                    if (mc_status[i] == 1 && fabs(mc_mother[i]) != lepID && fabs(mc_mother[i]) != 12 && lepID && fabs(mc_mother[i]) != 14 && lepID && fabs(mc_mother[i]) != 16 && fabs(mc_mother[i]) != 23 && fabs(mc_mother[i]) != 24) continue;
                    genLepId_.push_back((double)  mc_id[i]);
                    genLepSt_.push_back((double)  mc_status[i]);
                    genLepQ_.push_back((double)   sign);
                    genLepPt_.push_back((double)  tmp.Pt());
                    genLepEta_.push_back((double) tmp.Eta());
                    genLepPhi_.push_back((double) tmp.Phi());
                    genLepE_.push_back((double)   tmp.E());

                }
                //-- select the gen photons only --
                else if (mc_id[i] == 22) {
                    TLorentzVector tmp;
                    tmp.SetPxPyPzE(mc_px[i], mc_py[i], mc_pz[i], mc_en[i]);
                    if (mc_status[i] != 1) continue;
                    bool photonPass = true;
                    for (unsigned int ll = 0; ll < genPhoEtaDupl_.size(); ll++) {
                        double minPt = tmp.Pt() > genPhoPtDupl_[ll] ? genPhoPtDupl_[ll] : tmp.Pt();
                        double razlikaPT = fabs((tmp.Pt() - genPhoPtDupl_[ll]) / minPt);
                        double dPhi = tmp.Phi()-genPhoPhiDupl_[ll] ;
                        double dPhi1 = tmp.Phi()-genPhoPhiDupl_[ll] ;
                        if (fabs(dPhi) > TMath::Pi()) dPhi = 2 * TMath::Pi() - fabs(dPhi1);
                        if (fabs(tmp.Eta() - genPhoEtaDupl_[ll]) < 0.01 &&  fabs(dPhi) < 0.01 && razlikaPT < 0.01) {
                            if (dPhi1 != dPhi) cout << dPhi1 << " " << dPhi << endl;
                            photonPass = false;
                        }
                    }
                    if ( photonPass){
                        //cout << mc_px[i]<<"  " << mc_py[i]<<" " << mc_pz[i]<<"  " << mc_en[i] << endl;
                        genPhoPt_.push_back((double)  tmp.Pt());
                        genPhoEta_.push_back((double) tmp.Eta());
                        genPhoPhi_.push_back((double) tmp.Phi());
                        genPhoPtDupl_.push_back((double) tmp.Pt());
                        genPhoEtaDupl_.push_back((double) tmp.Eta());				  
                        genPhoPhiDupl_.push_back((double) tmp.Phi());
                    }   
                }
            }

            //-- filling gen jets branches --
            for (int i(0); i < jnUnf; i++){
                TLorentzVector tmp;
                //tmp.SetPxPyPzE(jn_genjpx[i], jn_genjpy[i], jn_genjpz[i], jn_genjen[i]);
                tmp.SetPxPyPzE(jn_genUnfjpx[i], jn_genUnfjpy[i], jn_genUnfjpz[i], jn_genUnfjen[i]);
                //cout << " jets: " <<  jn  <<" pt: " << tmp.Pt() << " others:" << jn_genjpx[i]  << "    " <<  jn_genjpy[i]  << "    " <<  jn_genjpz[i] << "    " << jn_genjen[i] <<  endl;
                if ( tmp.Pt() > 10 ) {
                    genJetPt_.push_back((double)  tmp.Pt());
                    genJetEta_.push_back((double) tmp.Eta());
                    genJetPhi_.push_back((double) tmp.Phi());
                    genJetE_.push_back((double) tmp.E());
                    genJetChF_.push_back((double) jn_genUnfjptcf[i]);
                }


            }
        }  // END IF HAS GEN INFO


        /// GO TO RECOOOOOOOOOOOOO
        if (hasRecoInfo) {

            eventMuonTrig = 0;
            if (leptonIdSum == 13 && t_bits[6]) eventMuonTrig += 1; // single muon HLT_IsoMu24_eta2p1_v ?? 
            if (leptonIdSum == 26 && t_bits[3]) eventMuonTrig += 4; // HLT_Mu17_TkMu8_v muon ?? 
            if (leptonIdSum == 26 && t_bits[2]) eventMuonTrig += 8; // HLT_Mu17_Mu8_v muon ?? 
            if (leptonIdSum == 24 && t_bits[4]) eventMuonTrig += 16;
            if (leptonIdSum == 24 && t_bits[5]) eventMuonTrig += 32;

            eventElecTrig = 0 ;
            if (leptonIdSum == 11 && t_bits[13]) eventElecTrig += 1; // HLT_Ele27_WP80_v
            if (leptonIdSum == 22 && t_bits[0]) eventElecTrig += 2; // HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v
            if (leptonIdSum == 24 && t_bits[4]) eventElecTrig += 16; 
            if (leptonIdSum == 24 && t_bits[5]) eventElecTrig += 32; 

            // -- fill reco leptons ----
            for (int i(0); i < ln; i++) {
                TLorentzVector tmp;
                tmp.SetPxPyPzE(ln_px[i], ln_py[i], ln_pz[i], ln_en[i]);
                std::bitset<32> bits(ln_idbits[i]);
                std::bitset<32> TRIGbits(ln_Tbits[i]);

                if (tmp.Pt() < 5) continue;
                if (doVector == 23 && ((!bits[5] && abs(ln_id[i]) == 11) || (!bits[10] && abs(ln_id[i]) == 13))) continue;  
                if ((leptonIdSum != 24 && abs(ln_id[i]) == lepID ) || ( leptonIdSum == 24 && (abs(ln_id[i]) == 11 || abs(ln_id[i]) == 13))) recoLep++;
                if (abs(ln_id[i]) == 11) {
                    patElecPt_.push_back((double)  tmp.Pt());
                    patElecEta_.push_back((double) tmp.Eta());
                    patElecPhi_.push_back((double) tmp.Phi());
                    patElecEn_.push_back((double) tmp.E());
                    patElecCharge_.push_back((double)  ln_id[i] );

                    double elecID = 0 ;
                    if (bits[3]) elecID += 1 ; // Veto ??
                    if (bits[4]) elecID += 2 ;  // Loose ?
                    if (bits[5]) elecID += 4 ; // Medium ??
                    if (bits[6]) elecID += 8 ; // Tight ??
                    //if ( bits[9]) muonID += 16 ; // HEEP Tight ??
                    patElecID_.push_back((double)  elecID );
                    double Aeff(1.0);
                    // match to SC eta --> all electros are also in the egm collection...
                    double eta(-111111.);
                    double mindR(2.5);
                    for (int k(0); k < egn; k++){
                        if ( DEBUG ) cout << " dR = " << mindR<< endl;
                        double dphi = tmp.Phi()-egn_scphi[k];
                        if (fabs(dphi) > TMath::Pi() ) dphi = 2 * TMath::Pi() - fabs(dphi);
                        double dR = sqrt((tmp.Eta()-egn_sceta[k])*(tmp.Eta()-egn_sceta[k]) + dphi * dphi);
                        if (  dR < mindR ){
                            mindR = dR;
                            eta = egn_sceta[k] ; 
                            if ( DEBUG ) cout << " dR = " << mindR<< "  " <<"  dPhi and dR " << dphi <<"   " << tmp.Eta()-egn_sceta[k] << endl;
                        }
                    }
                    if      (fabs(eta) < 1.0)   Aeff = 0.130;
                    else if (fabs(eta) < 1.479) Aeff = 0.137;
                    else if (fabs(eta) < 2.0)   Aeff = 0.067;
                    else if (fabs(eta) < 2.2)   Aeff = 0.089;
                    else if (fabs(eta) < 2.3)   Aeff = 0.107;
                    else if (fabs(eta) < 2.4)   Aeff = 0.110;
                    else                        Aeff = 0.138;
                    double isoPF = ln_chIso03[i] + max(ln_nhIso03[i] + ln_gIso03[i] - rho * Aeff, 0.);

                    isoPF /= tmp.Pt();
                    patElecPfIsoRho_.push_back(isoPF );

                    double singleElecTrig = 0;
                    if ( elecID > 3 && tmp.Pt() > 15. ) {
                        if ( isoPF < 0.15 ) countElec++;
                        else invIsoCountElec++;
                    }
                    if (leptonIdSum == 11 && TRIGbits[13])  singleElecTrig += 1;  // HLT_Ele27_WP80_v
                    if (leptonIdSum == 22 && TRIGbits[0])   singleElecTrig += 2;  // HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v 
                    if (leptonIdSum == 24 && TRIGbits[4])   singleElecTrig += 16; // MuEle ?? 
                    if (leptonIdSum == 24 && TRIGbits[5])   singleElecTrig += 32; // EleMu ?? 
                    patElecTrig_.push_back((double) singleElecTrig);
                    patElecScEta_.push_back((double) eta);
                }

                if (abs(ln_id[i]) == 13){
                    patMuonPt_.push_back((double)  tmp.Pt());
                    patMuonEta_.push_back((double) tmp.Eta());
                    patMuonPhi_.push_back((double) tmp.Phi());
                    patMuonEn_.push_back((double) tmp.E());
                    patMuonCharge_.push_back((double)  ln_id[i] );
                    patMuonDxy_.push_back((double) ln_d0[i] );

                    double isoPF = ln_chIso04[i] + max(ln_nhIso04[i] + ln_gIso04[i] - 0.5 * ln_puchIso04[i], 0.);
                    isoPF/=tmp.Pt();
                    patMuonPfIsoDbeta_.push_back(isoPF);

                    int muonID = 0. ;
                    if (bits[10]) muonID += 1 ; // Tight ??
                    if (bits[8]) muonID += 2 ; // Loose ?
                    if (bits[11]) muonID += 4 ; // HighPt ??
                    patMuonCombId_.push_back(muonID);
                    if ((muonID & 0x1)  && tmp.Pt() > 15.){
                        if (isoPF < 0.2 )  countMuon++;
                        else invIsoCountMuon++;
                    }
                    double muonTrig = 0 ;
                    if (leptonIdSum == 13 && TRIGbits[6]) muonTrig += 1 ; // single muon HLT_IsoMu24_eta2p1_v ?? 
                    if (leptonIdSum == 26 && TRIGbits[3]) muonTrig += 4 ; // HLT_Mu17_TkMu8_v muon ?? 
                    if (leptonIdSum == 26 && TRIGbits[2]) muonTrig += 8 ; // HLT_Mu17_Mu8_v muon ?? 
                    if (leptonIdSum == 24 && TRIGbits[4]) muonTrig += 16 ; // MuEle ?? 
                    if (leptonIdSum == 24 && TRIGbits[5]) muonTrig += 32 ; // EleMu ?? 
                    patMuonTrig_.push_back(muonTrig);
                    patMuonVtxZ_.push_back((double) ln_dZ[i]); 
                }


            } // end leptons loop


            // -- fill reco jets ----
            for (int i(0); i < jn; i++){
                TLorentzVector tmp;
                tmp.SetPxPyPzE(jn_px[i], jn_py[i], jn_pz[i], jn_en[i]);
                if ( tmp.Pt()  < 15. ) continue ;
                std::bitset<32>   bits(jn_idbits[i]);
                double jetID = 0. ;
                if ( bits[0] ) jetID += 1 ;
                if ( bits[1] ) jetID += 2 ;
                if ( bits[2] ) jetID += 4 ;

                patJetPfAk05En_.push_back((double) jn_en[i] );
                patJetPfAk05Pt_.push_back((double) tmp.Pt() );
                patJetPfAk05Eta_.push_back((double) tmp.Eta() );
                patJetPfAk05Phi_.push_back((double) tmp.Phi() );
                patJetPfAk05JesUncert_.push_back((double) 0. );   /// HOW TO FILL THIS ? INSIDE CMSSW ?
                patJetPfAk05LooseId_.push_back( jetID  );
                patJetPfAk05jetBSZ_.push_back((double) jn_betaStar[i] );
                patJetPfAk05jetBZ_.push_back((double) jn_beta[i] );
                ///				  patJetPfAk05jetpuMVA_.push_back((double) jn_puMVA[i] ); /// current version not correct --> switch to Loose PU ID
                //				  int simplePuId( ( idbits >>7 ) & 0xf );
                //				  bool passLoosePuId( ( puId >> 2) & 0x1);
                int simplePuId( ( jn_idbits[i] >>7 ) & 0xf );
                bool passLooseSimplePuId(  ( simplePuId >> 2) & 0x1);
                double fakeMVA(-1.0);
                if ( DEBUG ) cout << " PU id " << tmp.Pt()  << "  " << tmp.Eta() << " simple ID  " << simplePuId << "  " << passLooseSimplePuId << "   " << jn_idbits[i] << endl;
                if ( passLooseSimplePuId ) fakeMVA = 1.;
                patJetPfAk05jetpuMVA_.push_back(fakeMVA);
                patJetPfAk05OCSV_.push_back(jn_origcsv[i]);
                //if ( hasGenInfo ) patJetPfAk05PartonFlavour_.push_back(jn_genflav[i]);
                patJetPfAk05PartonFlavour_.push_back(jn_genflav[i]);
            }

            // --- reco MET fill ---
            for (int i(0); i < metn; i++){
                patMetPt_.push_back((double) met_pt[i]) ;
                patMetPhi_.push_back((double) met_phi[i]) ;
                patMetSig_.push_back((double) met_sig[i]) ;
                patMetSigXX_.push_back((double) met_sigx2[i]) ;
                patMetSigXY_.push_back((double) met_sigxy[i]) ;
                patMetSigYY_.push_back((double) met_sigy2[i]) ;

            }
        }  //  RECO if

        if (DEBUG) cout << "Passes  " <<  EvtInfo_EventNum << endl;
        //-- fill the output tree --
        if (!doUnfold && leptonIdSum == 24 &&patElecEta_.size() < 1 && patMuonEta_.size() < 1) continue;// at least one letons on RECO
        if (!doUnfold && ((leptonIdSum == 13 && patMuonEta_.size() < 1) || (leptonIdSum == 0 && patElecEta_.size() < 1) || ((leptonIdSum == 11 || leptonIdSum == 13) && ( countElec + countMuon) > 1) ) )  continue ; //running on single electrons
        if (!doUnfold && leptonIdSum == 24 && (patElecEta_.size() < 1 || patMuonEta_.size() < 1) ) continue ;

        if ( !doUnfold && leptonIdSum == 22 && patElecEta_.size() < 2 ) continue ;
        if ( !doUnfold && leptonIdSum == 26 && patMuonEta_.size() < 2 ) continue ;

        if (DEBUG ) cout << "PassesAAA  " <<  EvtInfo_EventNum << "  " << recoLep << "  " << genLep << endl;
        if ((leptonIdSum == 11 || leptonIdSum == 13) && doUnfold  && recoLep < 1 && genLep < 1) continue;
        if ((leptonIdSum == 22 || leptonIdSum == 26) && doUnfold  && recoLep < 2 && genLep < 2) continue;

        outputTree->Fill();
        passedEvents++;
        if (DEBUG) cout << "Passes###  " <<  EvtInfo_EventNum << endl;
        // clear all vectors
        EvtInfo_NumVtx = -111;
        EvtInfo_RunNum = -111;
        EvtInfo_LumiNum = -111;
        EvtInfo_EventNum = -111;
    }
    cout << " pass " << nentries<<"     " << passedEvents << endl;
    //-- save the output tree --
    outputTree->Write();
    cout << " Saving the tree " << endl;
    //  delete outputFile;
    outputFile->Close();
}
