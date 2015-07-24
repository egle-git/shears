#define bugraToBonzai_cxx
#include "bugraToBonzai.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>

void bugraToBonzai::Loop(string dataset_, string fileName_, int doMuon, bool doUnfold, bool hasRecoInfo, bool hasGenInfo, bool hasPartonInfo)
{

    //--------------------------------------------------------
    //-- output file --
    string outFileRoot = dataset_ +"/"+ fileName_ ;
    TFile *outputFile = new TFile("bugraBonzai.root", "recreate");
    //TFile *outputFile = new TFile(outFileRoot.c_str(), "recreate");

    //-- subdirectory to match our ntuples sturcture --
    TDirectory *cdtree = outputFile->mkdir("tree");
    cdtree->cd();

    //-- output tree --
    TTree *outputTree = new TTree("tree", "tree");
    //--------------------------------------------------------

    //--------------------------------------------------------
    //-- output tree containt --
    double        mcEveWeight_;
    double        mcSherpaSumWeight3_ ;

    int lepID = 11;
    if (doMuon == 1) lepID = 13;
    else if (doMuon == 24) lepID = 24;
    bool isMC = false;
    if (dataset_.find("MC") != string::npos ) isMC = true;


    double PU_weight(0);
    double PU_npT(0);
    double PU_npIT(0);

    int EvtInfo_NumVtx = -111;
    int EvtInfo_RunNum = -111;
    int EvtInfo_EventNum = -111;

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

    vector<double>  genJetPt_;
    vector<double>  genJetEta_;
    vector<double>  genJetPhi_;
    vector<double>  genJetE_;

    int eventElecTrig;
    vector<double>  patElecPt_B;
    vector<double>  patElecEta_B;
    vector<double>  patElecPhi_B;
    vector<double>  patElecEn_B;
    vector<double>  patElecCharge_B;
    vector<double>  patElecID_B;
    vector<double>  patElecTrig_B;
    vector<double>  patElecDetIso_B;
    vector<double>  patElecPfIsoRho_B;
    vector<double>  patElecScEta_B;

    int eventMuonTrig;
    vector<double>  patMuonPt_B;
    vector<double>  patMuonEta_B;
    vector<double>  patMuonPhi_B;
    vector<double>  patMuonVtxZ_B;
    vector<double>  patMuonEn_B;
    vector<double>  patMuonCharge_B;
    vector<double>  patMuonDxy_B;
    vector<double>  patMuonCombId_B;
    vector<double>  patMuonTrig_B;
    vector<double>  patMuonDetIsoRho_B;
    vector<double>  patMuonPfIsoDbeta_B;

    vector<double>  patJetPfAk05En_B;
    vector<double>  patJetPfAk05Pt_B;
    vector<double>  patJetPfAk05Eta_B;
    vector<double>  patJetPfAk05Phi_B;
    vector<double>  patJetPfAk05JesUncert_B;
    vector<double>  patJetPfAk05LooseId_B;
    vector<double>  patJetPfAk05jetBSZ_B;
    vector<double>  patJetPfAk05jetBZ_B;
    vector<double>  patJetPfAk05jetpuMVA_B;

    vector<double> patMetPt_ ;
    vector<double> patMetPhi_;
    vector<double> patMetSig_;
    vector<double> patMetSigXX_;
    vector<double> patMetSigXY_;
    vector<double> patMetSigYY_;


    //  outputTree->Branch("PU_weight", &PU_weight );
    //  outputTree->Branch("PU_npT", &PU_npT);
    //  outputTree->Branch("PU_npIT", &PU_npIT);

    outputTree->Branch("EvtInfo_NumVtx", &EvtInfo_NumVtx);
    outputTree->Branch("EvtInfo_RunNum", &EvtInfo_RunNum);
    outputTree->Branch("EvtInfo_EventNum", &EvtInfo_EventNum);
    if ( hasRecoInfo && isMC ){
        outputTree->Branch("PU_npT", &PU_npT);
        outputTree->Branch("PU_npIT", &PU_npIT);
    }

    //  if ( hasGenInfo ){
    outputTree->Branch("pdfInfo_",  &pdfInfo_);

    outputTree->Branch("genLepId_",  &genLepId_);
    outputTree->Branch("genLepSt_",  &genLepSt_);
    outputTree->Branch("genLepQ_",   &genLepQ_);
    outputTree->Branch("genLepPt_",  &genLepPt_);
    outputTree->Branch("genLepEta_", &genLepEta_);
    outputTree->Branch("genLepPhi_", &genLepPhi_);
    outputTree->Branch("genLepE_",   &genLepE_);

    outputTree->Branch("genPhoPt_",  &genPhoPt_);
    outputTree->Branch("genPhoEta_", &genPhoEta_);
    outputTree->Branch("genPhoPhi_", &genPhoPhi_);

    outputTree->Branch("genJetPt_",  &genJetPt_ );
    outputTree->Branch("genJetE_",   &genJetE_ );
    outputTree->Branch("genJetEta_", &genJetEta_);
    outputTree->Branch("genJetPhi_", &genJetPhi_);
    //  }
    // add reco branches

    outputTree->Branch("eventElecTrig", &eventElecTrig );
    outputTree->Branch("patElecPt_", &patElecPt_B);
    outputTree->Branch("patElecEta_", &patElecEta_B);
    outputTree->Branch("patElecPhi_", &patElecPhi_B);
    outputTree->Branch("patElecEnergy_", &patElecEn_B);
    outputTree->Branch("patElecCharge_", &patElecCharge_B);
    outputTree->Branch("patElecID_",&patElecID_B);
    outputTree->Branch("patElecTrig_", &patElecTrig_B);
    // outputTree->Branch("patElecDetIso_", &patElecDetIso_);
    // outputTree->Branch("patElecPfIsoRho_", &patElecPfIsoRho_);
    outputTree->Branch("patElecScEta_",&patElecScEta_B);


    // muons
    outputTree->Branch("eventMuonTrig",&eventMuonTrig);
    outputTree->Branch("patMuonPt_", &patMuonPt_B);
    outputTree->Branch("patMuonEta_", &patMuonEta_B);
    outputTree->Branch("patMuonPhi_", &patMuonPhi_B);
    outputTree->Branch("patMuonVtxZ_", &patMuonVtxZ_B);
    outputTree->Branch("patMuonEn_", &patMuonEn_B);
    outputTree->Branch("patMuonCharge_", &patMuonCharge_B);
    outputTree->Branch("patMuonDxy_", &patMuonDxy_B);
    outputTree->Branch("patMuonCombId_", &patMuonCombId_B);
    outputTree->Branch("patMuonTrig_", &patMuonTrig_B);
    //outputTree->Branch("patMuonDetIsoRho_", &patMuonDetIsoRho_);
    outputTree->Branch("patMuonPfIsoDbeta_", &patMuonPfIsoDbeta_B);



    // jet
    outputTree->Branch("patJetPfAk05En_", &patJetPfAk05En_B);
    outputTree->Branch("patJetPfAk05Pt_", &patJetPfAk05Pt_B);
    outputTree->Branch("patJetPfAk05Eta_", &patJetPfAk05Eta_B);
    outputTree->Branch("patJetPfAk05Phi_", &patJetPfAk05Phi_B);
    //outputTree->Branch("patJetPfAk05JesUncert_", &patJetPfAk05JesUncert_); /// for now not computed ; maybe tables to save some space
    outputTree->Branch("patJetPfAk05LooseId_", &patJetPfAk05LooseId_B);
    outputTree->Branch("patJetPfAk05jetBSZ_", &patJetPfAk05jetBSZ_B);
    outputTree->Branch("patJetPfAk05jetBZ_", &patJetPfAk05jetBZ_B);
    outputTree->Branch("patJetPfAk05jetpuMVA_", &patJetPfAk05jetpuMVA_B);


    // MET
    //outputTree->Branch("patMetSumEt_",&patMetSumEt_);
    outputTree->Branch("patMetPt_",&patMetPt_);
    outputTree->Branch("patMetPhi_",&patMetPhi_);
    outputTree->Branch("patMetSig_",&patMetSig_);
    outputTree->Branch("patMetSigXX_",&patMetSigXX_);
    outputTree->Branch("patMetSigXY_",&patMetSigXY_);
    outputTree->Branch("patMetSigYY_",&patMetSigYY_);

    //--------------------------------------------------------


    //--------------------------------------------------------
    //-- start loop over the big tree --
    if (fChain == 0) return;

    Long64_t nentries = fChain->GetEntries();

    cout << " reco : " <<hasRecoInfo << "  Gen:  " << hasGenInfo << " entries: " << nentries << endl;


    Long64_t nbytes = 0, nb = 0;
    nentries = 100;
    for (Long64_t jentry=0; jentry<nentries;jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);   nbytes += nb;
        // if (Cut(ientry) < 0) continue;

        EvtInfo_NumVtx = -111;
        EvtInfo_RunNum = -111;
        EvtInfo_EventNum = -111;
        eventMuonTrig = 0 ;
        eventElecTrig = 0 ;

        //-- Event info --------------
        //PU_npT  = ngenTruepu ;
        //PU_npIT = ngenITpu;
        //EvtInfo_NumVtx = nvtx;
        EvtInfo_RunNum = run;
        EvtInfo_EventNum = event;
        //-- pdf info --------------
        pdfInfo_.clear();
        /*if ( hasGenInfo ){
          pdfInfo_.push_back(id1_pdfInfo_);
          pdfInfo_.push_back(id2_pdfInfo_);
          pdfInfo_.push_back(x1_pdfInfo_);
          pdfInfo_.push_back(x2_pdfInfo_);
          pdfInfo_.push_back(scalePDF_pdfInfo_);
          }*/
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

        //  -- reco electrons -- 
        patElecPt_B.clear();
        patElecEta_B.clear();
        patElecPhi_B.clear();
        patElecEn_B.clear();
        patElecCharge_B.clear();
        patElecID_B.clear();
        patElecTrig_B.clear();
        patElecDetIso_B.clear();
        patElecPfIsoRho_B.clear();
        patElecScEta_B.clear();
        // -- reco muons ---
        patMuonPt_B.clear();
        patMuonEta_B.clear();
        patMuonPhi_B.clear();
        patMuonEn_B.clear();
        patMuonCharge_B.clear();
        patMuonDxy_B.clear();
        patMuonVtxZ_B.clear();
        patMuonCombId_B.clear();
        patMuonTrig_B.clear();
        patMuonPfIsoDbeta_B.clear();

        // -- rec jets ---
        patJetPfAk05En_B.clear();
        patJetPfAk05Pt_B.clear();
        patJetPfAk05Eta_B.clear();
        patJetPfAk05Phi_B.clear();
        patJetPfAk05JesUncert_B.clear();
        patJetPfAk05LooseId_B.clear();
        patJetPfAk05jetBSZ_B.clear();
        patJetPfAk05jetBZ_B.clear();
        patJetPfAk05jetpuMVA_B.clear();

        // --- reco met ---
        patMetPt_.clear() ;
        patMetPhi_.clear() ;
        patMetSig_.clear() ;
        patMetSigXX_.clear() ;
        patMetSigXY_.clear() ;
        patMetSigYY_.clear() ;


        int genLep = 0;
        int recoLep = 0;

        if (hasGenInfo) {

            mcEveWeight_ = MyWeight; 
            //mcSherpaSumWeight3_ = weight_three;
            /*       mcSherpaWeights_.push_back(weight_zero);
                     mcSherpaWeights_.push_back(weight_one);
                     mcSherpaWeights_.push_back(weight_two);
                     mcSherpaWeights_.push_back(weight_three);*/


            // gen leptons
            for (size_t i=0; i<Dr01LepPt->size(); ++i){
                if(Dr01LepId->at(i)==lepID)genLep++;
                genLepPt_.push_back(Dr01LepPt->at(i));
                genLepEta_.push_back(Dr01LepEta->at(i));
                genLepPhi_.push_back(Dr01LepPhi->at(i));
                genLepE_.push_back(Dr01LepE->at(i));
                genLepQ_.push_back(Dr01LepId->at(i));
                genLepId_.push_back(Dr01LepId->at(i));
                genLepSt_.push_back(Dr01LepStatus->at(i));		
            }


            // gen jets
            for (size_t i=0; i<GjPt->size(); ++i){
                if(GjPt->at(i)<=20)continue;
                genJetPt_.push_back(GjPt->at(i));
                genJetEta_.push_back(Gjeta->at(i));
                genJetPhi_.push_back(Gjphi->at(i));
                genJetE_.push_back(GjE->at(i));
            }

            //cout<<i<<endl;
        }  // END IF HAS GEN INFO

        /// GO TO RECOOOOOOOOOOOOO
        if ( hasRecoInfo ) {

            //   event info
            /// event triggers:
            // std::bitset<12>   evTRIGbits(t_bits);
            eventMuonTrig = 0;

            if (HLT_Mu17_TkMu8==1) eventMuonTrig += 4; // HLT_Mu17_TkMu8_v muon ?? 
            if ( HLT_Mu17_Mu8==1) eventMuonTrig += 8; // HLT_Mu17_Mu8_v muon ?? 

            eventElecTrig = 0;
            if (HLT_Elec17_Elec8==1) eventElecTrig += 2; // HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v



            // cout << " aaaa " << t_bits[6] <<"   " << t_bits[12] << "   " << t_bits[13] << endl;
            // -- fill reco leptons ----


            for(int i=0;i<(int) patElecPt_->size();++i){
                if(lepID==11)recoLep++;
                patElecPt_B.push_back(patElecPt_->at(i));
                patElecEta_B.push_back(patElecEta_->at(i));
                patElecPhi_B.push_back(patElecPhi_->at(i));
                patElecEn_B.push_back(patElecEnergy_->at(i));
                patElecCharge_B.push_back(patElecCharge_->at(i));

                double elecID = 0 ;

                if (patElecMediumIDOff_->at(i)) elecID += 4 ; // Medium ??
                //if ( bits[9]) muonID += 16 ; // HEEP Tight ??
                patElecID_B.push_back((double)  elecID );
                //					  cout << " electron ID:" << elecID << endl;
                eventElecTrig = 0 ;
                //					  cout << TRIGbits[0] << "   " << TRIGbits[1] << endl;
                //					  cout << TRIGbits[12] << "   " << TRIGbits[13] << endl;


                patElecTrig_B.push_back((double)   eventElecTrig );

                //		    patElecDetIso_.push_back((double)  ln_id[i] );
                //		    patElecPfIsoRho_.push_back((double)  ln_id[i] );
                patElecScEta_B.push_back(patElecScEta_->at(i) );

            }

            for(int i=0;i<patMuonPt_->size();i++){
                if(lepID==13)recoLep++;
                patMuonPt_B.push_back((double)  patMuonPt_->at(i));
                patMuonEta_B.push_back((double) patMuonEta_->at(i));
                patMuonPhi_B.push_back((double) patMuonPhi_->at(i));
                patMuonEn_B.push_back((double) patMuonEn_->at(i));
                patMuonCharge_B.push_back(patMuonCharge_->at(i));
                patMuonDxy_B.push_back(patMuonDxy_->at(i) );


                patMuonPfIsoDbeta_B.push_back(patMuonPfIsoDbeta_->at(i));

                double muonID = 0. ;
                if (patMuonCombId_->at(i)==1) muonID += 1 ; // Tight ??

                patMuonCombId_B.push_back(muonID);

                double muonTrig = 0 ;

                if (patMuon_Mu17_TkMu8_Matched_->at(i)==1) muonTrig += 4 ; // HLT_Mu17_TkMu8_v muon ?? 
                if ( patMuon_Mu17_Mu8_Matched_->at(i)==1) muonTrig += 8 ; // HLT_Mu17_Mu8_v muon ?? 

                patMuonTrig_B.push_back(muonTrig);
                patMuonVtxZ_B.push_back(patMuonVtxZ_->at(i)); 


            }



            // -- fill reco jets ----
            for (int i(0); i < patJetPfAk05En_->size(); i++){


                patJetPfAk05En_B.push_back(patJetPfAk05En_->at(i) );
                patJetPfAk05Pt_B.push_back(patJetPfAk05Pt_->at(i) );
                patJetPfAk05Eta_B.push_back(patJetPfAk05Eta_->at(i) );
                patJetPfAk05Phi_B.push_back(patJetPfAk05Phi_->at(i));
                patJetPfAk05LooseId_B.push_back( patJetPfAk05LooseId_->at(i) );

                patJetPfAk05JesUncert_B.push_back(unc_->at(i) );   /// HOW TO FILL THIS ? INSIDE CMSSW ?

                patJetPfAk05jetBSZ_B.push_back( patJetPfAk05jetBetaStar_->at(i) );
                patJetPfAk05jetBZ_B.push_back(patJetPfAk05jetBeta_->at(i)  );

                patJetPfAk05jetpuMVA_B.push_back(patJetPfAk05jetpuMVA_->at(i));



            }

            // --- reco MET fill ---
            for (int i(0); i < 4; i++){
                patMetPt_.push_back(0.) ;
                patMetPhi_.push_back(0.) ;
                patMetSig_.push_back(0.) ;
                patMetSigXX_.push_back(0.) ;
                patMetSigXY_.push_back(0.) ;
                patMetSigYY_.push_back(0.) ;

            }

        }  //  RECO if

        //-- fill the output tree --
        //cout << " number of muons : " << patMuonEta_.size() << endl;
        if ( !doUnfold && patElecEta_->size() < 1 && patMuonEta_->size() < 1 ) continue ;
        if ( !doUnfold && doMuon == 24 && (patElecEta_->size() < 1 || patMuonEta_->size() < 1) ) continue ;

        if ( doUnfold  && recoLep < 1 && genLep < 1 ) continue ;
        /*		  if ( !doUnfold && doMuon && patMuonEta_.size() < 1 ) continue ;
                  if ( !doUnfold && !doMuon && patElecEta_.size() < 1 ) continue ;
                  */
        outputTree->Fill();

        // clear all vectors
        EvtInfo_NumVtx = -111;
        EvtInfo_RunNum = -111;
        EvtInfo_EventNum = -111;


    }
    //-- save the output tree --
    outputTree->Write();
    //cout << " Saving the tree " << endl;
    //  delete outputFile;
    outputFile->Close();

}
