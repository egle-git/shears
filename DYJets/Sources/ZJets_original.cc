//-*- c-basic-offset: 4; -*-
#define PI 3.14159265359
#define DEBUG              0

#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include <TDatime.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TRandom.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "LHAPDF/LHAPDF.h"
#include "functions.h"
#include "standalone_LumiReWeighting.h"
#include "HistoSetZJets.h"
#include "ZJets.h"
#include <sys/time.h>
//#include "rochcor.h"


using namespace std;

void ZJets::Loop(bool hasRecoInfo, bool hasGenInfo, TString pdfSet, int pdfMember, double muR, double muF)
{

    //--- Random generator necessary for BTagging ---
    TRandom3* RandGen = new TRandom3();
    //--------------------------------------------
    doRochester = false;
    //    rmcor = new rochcor2012();

    //--- Initialize PDF from LHAPDF if needed ---
    if (pdfSet != "") initLHAPDF(pdfSet, pdfMember);
    //--------------------------------------------

    //--- Counters to check the yields ---
    unsigned int nEvents(0);
    unsigned int nEventsVInc0Jets(0), nEventsVInc1Jets(0), nEventsVInc2Jets(0), nEventsVInc3Jets(0);
    unsigned int nGenEventsVInc0Jets(0), nGenEventsVInc1Jets(0), nGenEventsVInc2Jets(0), nGenEventsVInc3Jets(0);
    unsigned int nEventsWithTwoGoodLeptons(0), nEventsWithTwoGoodLeptonsWithOppCharge(0), nEventsWithTwoGoodLeptonsWithOppChargeAndGoodMass(0);
    //------------------------------------

    //==========================================================================================================//
    int ZMCutLow(71), ZMCutHigh(111);
    int MTCutLow(50), METCutLow(0);
    // additional variables
    double ZptRange[6] = {0, 40, 80, 120, 160, 1000};
    int LeptonID(11);
    if (lepSel == "DMu" || lepSel == "SMu") LeptonID = 13;


    //==========================================================================================================//
    //         Output file name           //
    //===================================//
    TString outputFileName = CreateOutputFileName(pdfSet, pdfMember, muR, muF);
    TFile *outputFile = new TFile(outputFileName, "RECREATE");
    //==========================================================================================================//


    if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
    //==========================================================================================================//
    //       Load efficiency tables        //
    //====================================//
    table TableJESunc("EfficiencyTables/JESUnce_FT_53_V21_AN4_Uncertainty_AK5PFchs.txt");
    table LeptIso, LeptID, LeptTrig, Ele_Rec;
    // electron SF
    table Ele_Rec_8TeV("EfficiencyTables/Ele_SF_Reconstruction_2012.txt");
    table SC_Ele_2012EA("EfficiencyTables/Ele_SF_EA2012.txt");
    // muon SF
    table TrigMu17Mu8SF("EfficiencyTables/Efficiency_SF_Mu17Mu8.txt");
    table SC_RunABCD_TightID("EfficiencyTables/Muon_IDTight_Efficiencies_Run_2012ABCD_53X_Eta_Pt.txt");
    table SC_RunABCD_LooseIso("EfficiencyTables/Muon_ISOLoose_forTight_Efficiencies_Run_2012ABCD_53X_Eta_Pt.txt");
    table TrigIsoMu24SF("EfficiencyTables/Efficiency_SF_IsoMu24_eta2p1.txt");

    LeptID = SC_RunABCD_TightID;
    LeptIso = SC_RunABCD_LooseIso;
    LeptTrig = TrigMu17Mu8SF;
    Ele_Rec = Ele_Rec_8TeV;
    if (lepSel == "DE" || lepSel == "SE") LeptID = SC_Ele_2012EA;
    else if (lepSel == "SMu") LeptTrig = TrigIsoMu24SF;
    //==========================================================================================================//


    if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
    //==========================================================================================================//
    //     Systematics: jec, pu, xsec     //
    //====================================//
    cout << "Lepton Flavor: " << lepSel << "  systematics: " << systematics << "  direction: " << direction << endl;

    standalone_LumiReWeighting puWeight(lepSel, 2013);
    if (systematics == 1) puWeight = standalone_LumiReWeighting(lepSel, 2013, direction);

    int scale(0); //0,+1,-1; (keep 0 for noJEC shift study)
    if (systematics == 2) scale =  direction;

    double xsec(1.);
    if (systematics == 3) xsec = 1. + direction * xsecfactor;

    int smearJet(0);
    if (systematics == 4) smearJet = direction;

    int lepscale(0);
    if (systematics == 5) lepscale = direction;

    int smearlepton(0);
    if (systematics == 6) smearlepton = direction;
    //==========================================================================================================//



    // setting weight when running on MIX of exclusive DY/WJets files to match number of parton events
    double mixingWeightsDY[4] = {0.192686, 0.0718097, 0.04943495, 0.0360337}; // here we match all partons, and combine electron and muon side
    double mixingWeightsWJ_SMu[4] ={0.366713,  0.1119323,  0.07641136,  0.03803325};

    //======================================================================
    //additionnal PU weights
    TH1* addPuWeights = 0;
    string addPuFile = cfg_.getS("additionalPuWeightFile");
    if(addPuFile.size() > 0){
        TFile f(addPuFile.c_str());
        if(f.IsZombie()){
            std::cerr << "PU reweighting file, " << addPuFile << " was not found!" << endl;
        } else{
            cout << "Event will be reweighting according to their number of vertices using weights from file "
                << addPuFile << "." << endl;
            f.GetObject("hWeights", addPuWeights);
            addPuWeights->SetDirectory(0);
        }
    }
    //======================================================================


    //==========================================================================================================//
    // Start looping over all the events //
    //===================================//
    cout << endl;
    cout << "\nProcessing : " << fileName << "    -->  " << outputFileName << endl;

    //--- Initialize the tree branches ---
    Init(hasRecoInfo, hasGenInfo);
    if (fChain == 0) return;
    Long64_t nentries = fChain->GetEntries();
    if (0 <= nMaxEvents && nMaxEvents  < nentries) nentries = nMaxEvents;

    cout << "We will run on " << nentries << " events" << endl;
    //------------------------------------

    struct timeval t0;
    gettimeofday(&t0, 0);
    int mess_every_n =  std::min(10000LL, nentries/10);

    double weight_amcNLO_sum = 0;

    // ------ Random number for lepton energy resolution smearing -----
    //TRandom* RamMu = new TRandom(10);
    //TRandom* RamEle = new TRandom(20);
    // --------------------------------

    for (Long64_t jentry(0); jentry < nentries; jentry += 1) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        //cout << "---------------------------------------------------------------------" << endl;

        if (jentry % mess_every_n == 0 && jentry > 0){
            timeval t1;
            gettimeofday(&t1, 0);
            double dt = (t1.tv_sec - t0.tv_sec) + 1.e-6*(t1.tv_usec - t0.tv_usec);
            dt *= double(nentries  - jentry) / mess_every_n;
            int dt_s = int(dt + 0.5);
            int dt_h = int(dt_s) / 3600; 
            dt_s -= dt_h * 3600;
            int dt_m = dt_s / 60;
            dt_s -= dt_m *60;
            cout << TString::Format("%4.1f%%", (100. * jentry) / nentries)
                << "\t" << std::setw(11) << jentry << " / " << nentries
                << "\t " << std::setw(4) << int(dt / mess_every_n * 1.e6 + 0.5) << " us/event"
                << "\t Remaining time for this dataset loop: " 
                << std::setw(2) << dt_h << " h "
                << std::setw(2) << dt_m << " min "
                << std::setw(2) << dt_s << " s"
                << "\r" << std::flush;
            t0 = t1;
        }

        fChain->GetEntry(jentry);  
        nEvents++;


        //=======================================================================================================//
        //         Continue Statements        //
        //====================================//
        //if (jentry % 2 == 0) continue;
        //if (EvtInfo_NumVtx <= 14) continue;

        //=======================================================================================================//


        if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
        //=======================================================================================================//
        //         Computing weight            //
        //====================================//
        double weight(1);
        // line below is to see distributions as provided with default MC PU distribution
        if (hasRecoInfo && !isData) weight *= puWeight.weight(int(PU_npT));
        weight *= lumiScale * xsec;

        if(addPuWeights){
            double add_w_ = addPuWeights->GetBinContent(addPuWeights->GetXaxis()->FindBin(EvtInfo_NumVtx));
            if(add_w_ > 0) weight *= add_w_;
        }

        if (fileName.Index("DYJets") >= 0 && fileName.Index("MIX") >= 0 && nup_ > 5) weight *= mixingWeightsDY[nup_ - 6]; 
        if (fileName.Index("SMu_8TeV_WJets") >= 0 && fileName.Index("MIX") >= 0 && nup_ > 5) weight *= mixingWeightsWJ_SMu[nup_ - 6]; 
        if (fileName.Index("Sherpa") >= 0 && fileName.Index("UNFOLDING") >= 0) {
            weight *= mcSherpaWeights_->at(0) / 43597515.;
        }
        if (fileName.Index("Sherpa2") >= 0) {
            weight *= mcEventWeight_->at(0);
            weight_amcNLO_sum += mcEventWeight_->at(1);
        }
        if (fileName.Index("mcatnlo") >= 0) {
            if (muR == 0.0 && muF == 0.0 && pdfMember == -1) weight *= mcEventWeight_->at(0);
            if (muR == 1.0 && muF == 1.0 && pdfMember == -1) weight *= mcEventWeight_->at(0);
            if (muR == 1.0 && muF == 2.0 && pdfMember == -1) weight *= mcEventWeight_->at(2);
            if (muR == 1.0 && muF == 0.5 && pdfMember == -1) weight *= mcEventWeight_->at(3);
            if (muR == 2.0 && muF == 1.0 && pdfMember == -1) weight *= mcEventWeight_->at(4);
            if (muR == 2.0 && muF == 2.0 && pdfMember == -1) weight *= mcEventWeight_->at(5);
            if (muR == 2.0 && muF == 0.5 && pdfMember == -1) weight *= mcEventWeight_->at(6);
            if (muR == 0.5 && muF == 1.0 && pdfMember == -1) weight *= mcEventWeight_->at(7);
            if (muR == 0.5 && muF == 2.0 && pdfMember == -1) weight *= mcEventWeight_->at(8);
            if (muR == 0.5 && muF == 0.5 && pdfMember == -1) weight *= mcEventWeight_->at(9);
            if (muR == 0.0 && muF == 0.0 && pdfMember != -1) weight *= mcEventWeight_->at(pdfMember+10);
            weight_amcNLO_sum += mcEventWeight_->at(1);
        }

        //==========================================================================================================//
        // Compute the weight for PDF syst    //
        //===================================//
        double wPdf(1);
        if (pdfSet != "") wPdf = computePDFWeight();
        //==========================================================================================================//

        //--- There is no pile-up so no need to reweight for that ---
        double genWeight = weight * wPdf;
        //=======================================================================================================//


        if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
        //=======================================================================================================//
        //         Retrieving leptons          //
        //====================================//
        bool passesLeptonCut(0), passesLeptonChargeCut(0), passesLeptonMassCut(0), passesTauCut(1);
        unsigned short nLeptons(0), nVetoMuons(0), nVetoElectrons(0);
        vector<leptonStruct> leptons;
        vector<leptonStruct> vetoMuons;
        vector<leptonStruct> vetoElectrons;
        TLorentzVector MET;
        double MT = -99;
        TLorentzVector EWKBoson;

        if (hasRecoInfo) {

            //--- get Muons ---
            if (lepSel == "DMu" || lepSel == "SMu") {
                getMuons(leptons, vetoMuons);
            }

            //--- get Electrons ---
            if (lepSel == "DE" || lepSel == "SE") {
                getElectrons(leptons, vetoElectrons);
            }

            //--- get MET --- 
            if (lepSel == "SMu" || lepSel == "SE") {
                int whichMET(2); //  0 - pfMETPFlow, 1 - pfMet, 2 - pfType1CorrectedMet, 3 - pfType1p2CorrectedMet
                MET.SetPtEtaPhiM(patMetPt_->at(whichMET), 0, patMetPhi_->at(whichMET), 0);
            }

            //--- get the size of the collections ---
            nLeptons = leptons.size();
            nVetoMuons = vetoMuons.size();
            nVetoElectrons = vetoElectrons.size();

            //--- sort leptons by descending pt ---
            sort(leptons.begin(), leptons.end(), LepDescendingOrder);
            sort(vetoMuons.begin(), vetoMuons.end(), LepDescendingOrder);
            sort(vetoElectrons.begin(), vetoElectrons.end(), LepDescendingOrder);

            //-- determine if the event passes the leptons requirements for EWKBoson = Z Boson
            if ((lepSel == "DMu" || lepSel == "DE") && nLeptons >= 2) {
                nEventsWithTwoGoodLeptons++;

                // --- lepton energy scale and resolution variation ---
                if(lepSel == "DMu"){
                    // --- muon energy scale variation ---
                    leptons[0].v.SetPtEtaPhiE(leptons[0].v.Pt() * (1 + lepscale*0.002), leptons[0].v.Eta(), leptons[0].v.Phi(), leptons[0].v.E() * (1 + lepscale*0.002));
                    leptons[1].v.SetPtEtaPhiE(leptons[1].v.Pt() * (1 + lepscale*0.002), leptons[1].v.Eta(), leptons[1].v.Phi(), leptons[1].v.E() * (1 + lepscale*0.002));

                }

                else if(lepSel == "DE")
                {
                    // --- electron energy scale variation ---
                    if(fabs(leptons[0].v.Eta())<1.479){
                        leptons[0].v.SetPtEtaPhiE(leptons[0].v.Pt() * (1 + lepscale * 0.006), leptons[0].v.Eta(), leptons[0].v.Phi(), leptons[0].v.E() * (1 + lepscale * 0.006));
                    }
                    else{
                        leptons[0].v.SetPtEtaPhiE(leptons[0].v.Pt() * (1 + lepscale * 0.015), leptons[0].v.Eta(),leptons[0].v.Phi(), leptons[0].v.E() * (1 + lepscale * 0.015));
                    }

                    if(fabs(leptons[1].v.Eta())<1.479){
                        leptons[1].v.SetPtEtaPhiE(leptons[1].v.Pt() * (1 + lepscale * 0.006), leptons[1].v.Eta(), leptons[1].v.Phi(), leptons[1].v.E() * (1 + lepscale * 0.006));
                    }
                    else{
                        leptons[1].v.SetPtEtaPhiE(leptons[1].v.Pt() * (1 + lepscale * 0.015), leptons[1].v.Eta(),leptons[1].v.Phi(), leptons[1].v.E() * (1 + lepscale * 0.015));
                    }

                }

                if(!hasGenInfo){
                    // build Electroweak boson candidate: here it is expected to be a Z
                    EWKBoson = leptons[0].v + leptons[1].v;

                    // apply charge, mass cut
                    if (leptons[0].charge * leptons[1].charge < 0) {
                        nEventsWithTwoGoodLeptonsWithOppCharge++;
                        passesLeptonChargeCut = 1;
                        if (EWKBoson.M() > ZMCutLow && EWKBoson.M() < ZMCutHigh && leptons[0].v.Pt() > lepPtCutMin && leptons[1].v.Pt() > lepPtCutMin) {
                            nEventsWithTwoGoodLeptonsWithOppChargeAndGoodMass++;
                            passesLeptonCut = 1;
                            passesLeptonMassCut = 1;
                        }
                    }
                }
                // apply scale factors only on MC.
                if (!isData) {
                    double effWeight = 1.;
                    if (lepSel == "DMu") {
                        effWeight *= LeptID.getEfficiency(leptons[0].v.Pt(), fabs(leptons[0].v.Eta()));
                        effWeight *= LeptID.getEfficiency(leptons[1].v.Pt(), fabs(leptons[1].v.Eta()));
                        effWeight *= LeptIso.getEfficiency(leptons[0].v.Pt(), fabs(leptons[0].v.Eta())); 
                        effWeight *= LeptIso.getEfficiency(leptons[1].v.Pt(), fabs(leptons[1].v.Eta())); 
                        if (useTriggerCorrection) effWeight *= LeptTrig.getEfficiency(fabs(leptons[0].v.Eta()), fabs(leptons[1].v.Eta()));

                    }
                    else if (lepSel == "DE") {
                        effWeight *= Ele_Rec.getEfficiency(leptons[0].v.Pt(), fabs(leptons[0].scEta));
                        effWeight *= Ele_Rec.getEfficiency(leptons[1].v.Pt(), fabs(leptons[1].scEta));
                        effWeight *= LeptID.getEfficiency(leptons[0].v.Pt(), fabs(leptons[0].scEta));
                        effWeight *= LeptID.getEfficiency(leptons[1].v.Pt(), fabs(leptons[1].scEta)); 
                    }
                    weight *= effWeight;
                }

            } // end if Z study and nLeptons >= 2

            // determine if the event passes the leptons requirements for EWKBoson = W Boson
            // exactly one muon (or exactly one electron) must be present and no additional
            // charged leptons can be present.
            else if ((lepSel == "SMu" || lepSel == "SE") && (nLeptons == 1 && nVetoMuons == 0 && nVetoElectrons == 0)) {
                // add the MET to the leptons collection: leptons[1] = MET
                leptons.push_back(leptonStruct(MET.Pt(), 0, MET.Phi(), MET.Pt(), 0, 0, 0, 0, 0)); 

                // build Electroweak boson candidate: here it is expected to be a W
                EWKBoson = leptons[0].v + MET;
                MT = sqrt(2 * leptons[0].v.Pt() * MET.Pt() * (1 - cos(leptons[0].v.Phi() - MET.Phi())));

                // apply transver mass and MET cut
                if (MT > MTCutLow && MET.Pt() > METCutLow && leptons[0].v.Pt() > lepPtCutMin) passesLeptonCut = 1;

                // apply scale factors only on MC.
                if (!isData) {
                    double effWeight = 1.;
                    if (lepSel == "SMu") {
                        effWeight *= LeptID.getEfficiency(leptons[0].v.Pt(), fabs(leptons[0].v.Eta()));
                        effWeight *= LeptIso.getEfficiency(leptons[0].v.Pt(), fabs(leptons[0].v.Eta())); 
                        if (useTriggerCorrection) effWeight *= LeptTrig.getEfficiency(fabs(leptons[0].v.Pt()), fabs(leptons[0].v.Eta()));
                    }
                    else if (lepSel == "SE") {
                        effWeight *= Ele_Rec.getEfficiency(leptons[0].v.Pt(), fabs(leptons[0].scEta));
                        effWeight *= LeptID.getEfficiency(leptons[0].v.Pt(), fabs(leptons[0].scEta));
                    }
                    weight *= effWeight;
                }
            } // end if W study and nLeptons >= 1
        }// end has reco info


        if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
        //=======================================================================================================//
        //       Retrieving gen leptons        //
        //====================================//
        bool passesgenLeptonCut(0);
        unsigned short nTotgenLeptons(0), ngenLeptons(0), nTotGenPhotons(0);
        vector<leptonStruct> genLeptons;
        vector<int> usedGenPho;
        TLorentzVector genMET;
        double genMT = -99;
        TLorentzVector genEWKBoson;
        int countTauS3 = 0;
        int nTauWithStatus3 = 0;

        if (hasGenInfo) {
            if (hasRecoInfo) countTauS3 = (lepSel == "DMu" || lepSel == "DE") ? 2 : 1;
            nTotGenPhotons = genPhoEta_->size();
            nTotgenLeptons = genLepEta_->size();
            //-- retriveing generated leptons with status 1
            for (unsigned short i(0); i < nTotgenLeptons; i++) {
                bool lepToBeConsidered(false); 
                if ((lepSel == "DMu" || lepSel == "DE") && abs(genLepId_->at(i)) == LeptonID) lepToBeConsidered = true; 
                else if ((lepSel == "SMu" || lepSel == "SE") && (abs(genLepId_->at(i)) == LeptonID || abs(genLepId_->at(i)) == 12 || abs(genLepId_->at(i)) == 14)) lepToBeConsidered = true;
                // following two lines should give the same result
                if (genLepSt_->at(i) == 3 && abs(genLepId_->at(i)) != LeptonID && (abs(genLepId_->at(i)) == 15 || abs(genLepId_->at(i)) == 13 || abs(genLepId_->at(i)) == 11)) countTauS3++;
                if (genLepSt_->at(i) == 3 && abs(genLepId_->at(i)) == LeptonID) countTauS3--;

                if (genLepSt_->at(i) == 3 && abs(genLepId_->at(i)) == 15) nTauWithStatus3++;

                if (!lepToBeConsidered) continue;

                int charge;
                if (abs(genLepId_->at(i)) == 12 || abs(genLepId_->at(i)) == 14 || abs(genLepId_->at(i)) == 16) charge = 0;
                else if (genLepId_->at(i) < 0) charge = -1;
                else charge = 1;
                leptonStruct genLep(genLepPt_->at(i), genLepEta_->at(i), genLepPhi_->at(i), genLepE_->at(i), charge, 0, 0, 0, 0);
                leptonStruct genLepNoFSR(genLepPt_->at(i), genLepEta_->at(i), genLepPhi_->at(i), genLepE_->at(i), charge, 0, 0, 0, 0);

                //-- dress the leptons with photon (cone size = 0.1). Only for status 1 leptons (after FSR)
                if ((genLepSt_->at(i) == 1 && lepToBeConsidered) || ((lepSel == "SMu" || lepSel == "SE") && charge == 0)) {

                    for (unsigned short j(0); j < nTotGenPhotons; j++){
                        TLorentzVector tmpGenPho;
                        tmpGenPho.SetPtEtaPhiM(genPhoPt_->at(j), genPhoEta_->at(j), genPhoPhi_->at(j), 0.);
                        int used(0);
                        for (unsigned short k(0); k < usedGenPho.size(); k++){
                            if (j == usedGenPho[k]) used = 1;
                        }
                        if (deltaR(tmpGenPho.Phi(), tmpGenPho.Eta(), genLepNoFSR.v.Phi(), genLepNoFSR.v.Eta()) < 0.1 && !used){
                            genLep.v += tmpGenPho;
                            usedGenPho.push_back(j);
                        }
                    }   

                    if ((genLep.v.Pt() >= lepPtCutMin && fabs(genLep.v.Eta()) <= 0.1*lepEtaCutMax && abs(genLep.charge) > 0) 
                            || ((lepSel == "SMu" || lepSel == "SE") && genLep.charge == 0)) {
                        genLeptons.push_back(genLep);
                    }
                }
            }

            ngenLeptons = genLeptons.size();

            // sort leptons by descending pt
            sort(genLeptons.begin(), genLeptons.end(), LepDescendingOrder);

            if (countTauS3 == 0 && fileName.Index("UNFOLDING") >= 0 && fileName.Index("Sherpa") < 0) {
                partonsN->Fill(nup_-5);
                partonsNWeighted->Fill(nup_-5, genWeight);
            }

            //--- if there are taus, but we do not run on the Tau file, thus we run on the DYJets file, 
            //    then we don't count the event at reco.
            if (countTauS3 > 0 && fileName.Index("Tau") < 0 && fileName.Index("Sherpa") < 0 && fileName.Index("MG5") < 0) {
                passesTauCut = 0;
                passesLeptonCut = 0; 
            }

            //-- determine if the event passes the leptons requirements
            if ((lepSel == "DMu" || lepSel == "DE") && ngenLeptons >= 2) {

                // build the EWKBoson candidate and the kinematic
                genEWKBoson = genLeptons[0].v + genLeptons[1].v;

                // apply charge, mass and eta cut
                if (genLeptons[0].charge * genLeptons[1].charge < 0 && genEWKBoson.M() > ZMCutLow && genEWKBoson.M() < ZMCutHigh) {
                    passesgenLeptonCut = 1;
                }
                //--- if there are taus we don't want the gen level
                if (countTauS3 > 0 && fileName.Index("Bugra") < 0 && fileName.Index("MG5") < 0) passesgenLeptonCut = 0;
            }
            else if ((lepSel == "SMu" || lepSel == "SE") && (ngenLeptons >= 2)) {


                if (abs(genLeptons[0].charge) > 0 && genLeptons[1].charge == 0) {
                    genMET = genLeptons[1].v;
                }
                else if (abs(genLeptons[1].charge) > 0 && genLeptons[0].charge == 0) {
                    genMET = genLeptons[0].v;
                    genLeptons[0] = genLeptons[1];
                }
                else passesgenLeptonCut = 0;

                genEWKBoson = genLeptons[0].v + genMET;
                genMT = sqrt(2 * genLeptons[0].v.Pt() * genMET.Pt() * (1 - cos(genLeptons[0].v.Phi() - genMET.Phi())));


                // apply transver mass and MET cut
                if (genMT > MTCutLow && genMET.Pt() > METCutLow) passesgenLeptonCut = 1;
                //--- if there are taus we don't want the gen level
                if (countTauS3 > 0) passesgenLeptonCut = 0;
            }
        } // end of hasGenInfo

        //=======================================================================================================//
        //   ------- lepton energy smearing ------
        //==========================================//
        if (hasRecoInfo && hasGenInfo){
            if((lepSel == "DMu" || lepSel == "DE") && nLeptons >= 2 && ngenLeptons >= 2){
                double oldLeptonPt;
                double genLeptonPt;
                double newLeptonPt;
                double smearFactor;

                smearFactor = (lepSel == "DMu") ? 0.006 : 0.06;

                oldLeptonPt = leptons[0].v.Pt();
                genLeptonPt = genLeptons[0].v.Pt();
                newLeptonPt = SmearLepPt(oldLeptonPt,genLeptonPt,smearlepton,smearFactor);
                leptons[0].v.SetPtEtaPhiE(newLeptonPt, leptons[0].v.Eta(), leptons[0].v.Phi(), leptons[0].v.E()*newLeptonPt/oldLeptonPt);

                oldLeptonPt = leptons[1].v.Pt();
                genLeptonPt = genLeptons[1].v.Pt();
                newLeptonPt = SmearLepPt(oldLeptonPt,genLeptonPt,smearlepton,smearFactor);
                leptons[1].v.SetPtEtaPhiE(newLeptonPt, leptons[1].v.Eta(), leptons[1].v.Phi(), leptons[1].v.E()*newLeptonPt/oldLeptonPt);

                // build Electroweak boson candidate: here it is expected to be a Z
                EWKBoson = leptons[0].v + leptons[1].v;

                // apply charge, mass cut
                if (leptons[0].charge * leptons[1].charge < 0) {
                    nEventsWithTwoGoodLeptonsWithOppCharge++;
                    passesLeptonChargeCut = 1;
                    if (EWKBoson.M() > ZMCutLow && EWKBoson.M() < ZMCutHigh && leptons[0].v.Pt() > lepPtCutMin && leptons[1].v.Pt() > lepPtCutMin) {
                        nEventsWithTwoGoodLeptonsWithOppChargeAndGoodMass++;
                        passesLeptonCut = 1;
                        passesLeptonMassCut = 1;
                    }
                } // end if re-selection of leptons

            } // end if DMu/DE channel

        } // end of hasRecoInfo and hasGenInfo


        if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
        //=======================================================================================================//
        //          Retrieving jets           //
        //====================================//
        unsigned short nGoodJets(0), nGoodJets_20(0), nTotJets(0);
        bool bTagJetFound(false);
        double jetsHT(0);
        vector<jetStruct> jets;
        vector<jetStruct> jets_20; // additional jet collection with pt threshold of 20 GeV
        TLorentzVector jet1Plus2, jet1Minus2;

        if (hasRecoInfo) {
            nTotJets = patJetPfAk05Eta_->size();
            for (unsigned short i(0); i < nTotJets; i++) {
                bool passesBJets = false;
                if (fileName.Index("Sherpa") < 0) passesBJets = (patJetPfAk05OCSV_->at(i) >= 0.679); 

                if (!isData && lepSel == "SMu") {
                    BTagModification(RandGen->Rndm(), patJetPfAk05Pt_->at(i), patJetPfAk05Eta_->at(i),patJetPfAk05PartonFlavour_->at(i), passesBJets);
                }

                jetStruct jet(patJetPfAk05Pt_->at(i), patJetPfAk05Eta_->at(i), patJetPfAk05Phi_->at(i), patJetPfAk05En_->at(i), i, passesBJets);

                //-- apply jet energy scale uncertainty (need to change the scale when initiating the object)
                double jetEnergyCorr = 0.; 
                bool jetPassesPtCut(jet.v.Pt() >= 10); 
                jetEnergyCorr = TableJESunc.getEfficiency(jet.v.Pt(), jet.v.Eta());

                jet.v.SetPtEtaPhiE(jet.v.Pt() * (1 + scale * jetEnergyCorr), jet.v.Eta(), jet.v.Phi(), jet.v.E() * (1 + scale * jetEnergyCorr));

                bool jetPassesEtaCut(fabs(jet.v.Rapidity()) <= 0.1*jetEtaCutMax); 
                bool jetPassesIdCut(patJetPfAk05LooseId_->at(i) > 0);
                bool jetPassesMVACut(patJetPfAk05jetpuMVA_->at(i) > 0);

                bool jetPassesdRCut(1);
                unsigned short nRemovedLep = min(int(nLeptons), 2);
                for (unsigned short j(0); j < nRemovedLep; j++) {
                    if (deltaR(jet.v, leptons[j].v) < 0.5) {
                        jetPassesdRCut = 0;
                    }
                }

                if (jetPassesPtCut && jetPassesEtaCut && jetPassesIdCut && jetPassesMVACut && jetPassesdRCut) {
                    jets.push_back(jet);
                    // as soon as one selected jet is a b-jet, turn bTagJetFound to true
                    bTagJetFound = (bTagJetFound || passesBJets); 
                }
            } //End of loop over all the jets 

            nGoodJets = jets.size();

            // line below to test reco events that originate from TAU
            if (fileName.Index("Tau") >= 0 && countTauS3 == 0 && hasGenInfo) {
                passesTauCut = 0;
                passesLeptonCut = 0;
            }
        }  // END IF HAS RECO
        //=======================================================================================================//


        if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
        //=======================================================================================================//
        //        Retrieving gen jets          //
        //====================================//
        unsigned short nGoodGenJets(0), nGoodGenJets_20(0), nTotGenJets(0);
        double genJetsHT(0);
        vector<jetStruct> genJets;
        vector<TLorentzVector> genLVJets;
        vector<jetStruct> genJets_20;
        TLorentzVector genJet1Plus2, genJet1Minus2;

        if (hasGenInfo){
            nTotGenJets = genJetEta_->size();
            //-- retrieving generated jets
            for (unsigned short i(0); i < nTotGenJets; i++){
                jetStruct genJet(genJetPt_->at(i), genJetEta_->at(i), genJetPhi_->at(i), genJetE_->at(i), i, 0);
                bool genJetPassesdRCut(1);
                for (unsigned short j(0); j < ngenLeptons; j++){ 
                    if (deltaR(genJet.v, genLeptons[j].v) < 0.5) {
                        genJetPassesdRCut = 0;
                    }
                }
                if (genJet.v.Pt() >= 10 && fabs(genJet.v.Eta()) <= 5.0 && genJetPassesdRCut){
                    genJets.push_back(genJet);                  
                }
            }
            nGoodGenJets = genJets.size();
        }
        //=======================================================================================================//


        if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
        //=======================================================================================================//
        //     Matching gen and reco jets     //
        //====================================//
        vector<int> genJetsIndex(nGoodGenJets, 0);
        vector<vector<int> > matchingTable(nGoodJets, genJetsIndex);
        if (hasRecoInfo && hasGenInfo){
            for (unsigned short i(0); i < nGoodJets; i++){
                double mindR(0.15);
                int index(-1);
                double dR(9999);
                for (unsigned short j(0); j < nGoodGenJets; j++){
                    dR = deltaR(genJets[j].v, jets[i].v);
                    if (dR < mindR){
                        mindR = dR;
                        index = j;
                    }
                }
                if (index > -1 ){
                    matchingTable[i][index] = 1; 
                    double oldJetPt = jets[i].v.Pt();
                    double newJetPt = SmearJetPt(oldJetPt, genJets[index].v.Pt(), jets[i].v.Eta(), smearJet);
                    jets[i].v.SetPtEtaPhiE(newJetPt, jets[i].v.Eta(), jets[i].v.Phi(), jets[i].v.E() * newJetPt / oldJetPt);
                }
            }

            //-- print the mathcing table
            //cout << "\n mathcing Table: \n" << endl; 
            //for (int i = 0; i < int(matchingTable.size()); i++){
            //  for (int j = 0; j < int(matchingTable[i].size()); j++){
            //    cout << matchingTable[i][j] << "  ";
            //  }
            //  cout << endl;
            //}

        }

        //=======================================================================================================//
        // Re-analyze the jets collections and cut on the Pt    //
        // we can do it only now since we needed to smear      //
        // the jet pt distribution for the MC                 //
        //===================================================//

        if (hasRecoInfo){     
            // subsamble the prior jet collection by applying pt cut
            vector<jetStruct> tmpJets;
            for (unsigned short i(0); i < nGoodJets; i++){
                if (jets[i].v.Pt() >= jetPtCutMin) tmpJets.push_back(jets[i]);
                if (jets[i].v.Pt() >= 20) jets_20.push_back(jets[i]);
            }
            jets.clear(); 
            jets = tmpJets; 
            nGoodJets = jets.size();
            nGoodJets_20 = jets_20.size();
            sort(jets.begin(), jets.end(), JetDescendingOrder);
            sort(jets_20.begin(), jets_20.end(), JetDescendingOrder);
            if (nGoodJets >= 2){
                jet1Plus2 = jets[0].v + jets[1].v;
                jet1Minus2 = jets[0].v - jets[1].v;
            }
            jetsHT = 0;
            for (unsigned short i(0); i < nGoodJets; i++){
                jetsHT += jets[i].v.Pt();  
            }
        }

        if (hasGenInfo){
            vector<jetStruct> tmpGenJets;
            for (unsigned short i(0); i < nGoodGenJets; i++){
                if (genJets[i].v.Pt() >= jetPtCutMin && fabs(genJets[i].v.Rapidity()) <= 0.1*jetEtaCutMax) tmpGenJets.push_back(genJets[i]);
                if (genJets[i].v.Pt() >= 20 && fabs(genJets[i].v.Rapidity()) <= 0.1*jetEtaCutMax) genJets_20.push_back(genJets[i]);
            }
            genJets.clear();
            genJets = tmpGenJets; 
            nGoodGenJets = genJets.size();
            nGoodGenJets_20 = genJets_20.size();
            sort(genJets.begin(), genJets.end(), JetDescendingOrder);
            if (nGoodGenJets >= 2){
                genJet1Plus2 = genJets[0].v + genJets[1].v;
                genJet1Minus2 = genJets[0].v - genJets[1].v;
            }
            genJetsHT = 0.;
            for (unsigned short i(0); i < nGoodGenJets; i++){
                genJetsHT += genJets[i].v.Pt();  
            }
            sort(genJets_20.begin(), genJets_20.end(), JetDescendingOrder);

        }
        //=======================================================================================================//


        if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
        //=======================================================================================================//
        // Select the best pair of jets for DPS  //
        //=======================================//
        pair<TLorentzVector, TLorentzVector> bestTwoJets;
        TLorentzVector bestJet1Plus2, bestJet1Minus2;
        if (hasRecoInfo){
            bestTwoJetsCandidatesPt(jets, bestTwoJets);
            //bestTwoJetsCandidatesPhi(jets, bestTwoJets);
            if (nGoodJets >= 2){
                bestJet1Plus2 = bestTwoJets.first + bestTwoJets.second;
                bestJet1Minus2 = bestTwoJets.first - bestTwoJets.second;
            }
        }

        pair<TLorentzVector, TLorentzVector> genBestTwoJets;
        TLorentzVector genBestJet1Plus2, genBestJet1Minus2;
        if (hasGenInfo){
            bestTwoJetsCandidatesPt(genJets, genBestTwoJets);
            //bestTwoJetsCandidatesPhi(genJets, genBestTwoJets);
            if (nGoodGenJets >= 2){
                genBestJet1Plus2 = genBestTwoJets.first + genBestTwoJets.second; 
                genBestJet1Minus2 = genBestTwoJets.first - genBestTwoJets.second; 
            }
        }
        //=======================================================================================================//



        if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
        //=======================================================================================================//
        //   Filling gen end parton histos    //
        //====================================//

        double gentau_sum(0), gentau_max(0);
        double gentau_c_sum(0), gentau_c_max(0);
        double gentau_cm_sum(0), gentau_cm_max(0);
        double gentau_c_cm_sum(0), gentau_c_cm_max(0); 

        if (hasGenInfo){
            if (passesgenLeptonCut){
                //cout << "Selected at gen level" << endl;
                nGenEventsVInc0Jets++;
                genZNGoodJets_Zexc->Fill(nGoodGenJets, genWeight);
                genZNGoodJets_Zinc->Fill(0., genWeight);
                genZMass_Zinc0jet->Fill(genEWKBoson.M(), genWeight);
                genZPt_Zinc0jet->Fill(genEWKBoson.Pt(), genWeight);
                genZRapidity_Zinc0jet->Fill(genEWKBoson.Rapidity(), genWeight);
                genZEta_Zinc0jet->Fill(genEWKBoson.Eta(), genWeight);
                genlepPt_Zinc0jet->Fill(genLeptons[0].v.Pt(), genWeight);
                genlepPt_Zinc0jet->Fill(genLeptons[1].v.Pt(), genWeight);
                genlepEta_Zinc0jet->Fill(genLeptons[0].v.Eta(), genWeight);
                genlepEta_Zinc0jet->Fill(genLeptons[1].v.Eta(), genWeight);

                if (nGoodGenJets_20 >= 1) {
                    genFirstJetPt_Zinc1jet->Fill(genJets_20[0].v.Pt(), genWeight);
                    genFirstJetPtEta_Zinc1jet->Fill(genJets_20[0].v.Pt(), fabs(genJets[0].v.Eta()), genWeight);
                }
                if (nGoodGenJets >= 1){
                    nGenEventsVInc1Jets++;
                    genAbsZRapidity_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                    genAbsFirstJetRapidity_Zinc1jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                    genSumZFirstJetRapidity_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                    genDifZFirstJetRapidity_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);
                    //cross check//////
                    genSumZFirstJetEta_Zinc1jet->Fill(fabs(genEWKBoson.Eta()+genJets[0].v.Eta())/2.0,genWeight);
                    genDifZFirstJetEta_Zinc1jet->Fill(fabs(genEWKBoson.Eta()-genJets[0].v.Eta())/2.0,genWeight);

                    ///Azimuth cross check//////////////////////////
                    genDPhiZFirstJet_Zinc1jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[0].v)),genWeight);


                    if(genEWKBoson.Pt()>100.)
                    {
                        genAbsZRapidity_ZPt100_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                        genAbsFirstJetRapidity_ZPt100_Zinc1jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                        genSumZFirstJetRapidity_ZPt100_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                        genDifZFirstJetRapidity_ZPt100_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);

                    }

                    if(genEWKBoson.Pt()>150.)
                    {
                        genAbsZRapidity_ZPt150_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                        genAbsFirstJetRapidity_ZPt150_Zinc1jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                        genSumZFirstJetRapidity_ZPt150_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                        genDifZFirstJetRapidity_ZPt150_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);


                        genDPhiZFirstJet_ZPt150_Zinc1jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[0].v)),genWeight);
                    }

                    if(genEWKBoson.Pt()>300.)
                    {
                        genAbsZRapidity_ZPt300_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                        genAbsFirstJetRapidity_ZPt300_Zinc1jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                        genSumZFirstJetRapidity_ZPt300_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                        genDifZFirstJetRapidity_ZPt300_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);

                        genDPhiZFirstJet_ZPt300_Zinc1jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[0].v)),genWeight);
                    }

                    ///different JetPt cuts///////
                    if(genJets[0].v.Pt()>50.)
                    {
                        genAbsZRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                        genAbsFirstJetRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                        genSumZFirstJetRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                        genDifZFirstJetRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);

                    }

                    if(genJets[0].v.Pt()>80.)
                    {
                        genAbsZRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                        genAbsFirstJetRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                        genSumZFirstJetRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                        genDifZFirstJetRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);

                    }


                    for (unsigned short i(0); i < nGoodGenJets; i++) {
                        double trans_mass = genJets[i].v.Mt();

                        gentau_sum += trans_mass * exp(-fabs(genJets[i].v.Rapidity() - genEWKBoson.Rapidity())); 
                        gentau_max = max(gentau_max, trans_mass * exp(-fabs(genJets[i].v.Rapidity() - genEWKBoson.Rapidity()))); 

                        gentau_c_sum += trans_mass / (2 * cosh(genJets[i].v.Rapidity() - genEWKBoson.Rapidity()));
                        gentau_c_max = max(gentau_c_max, trans_mass / (2 * cosh(genJets[i].v.Rapidity() - genEWKBoson.Rapidity())));

                        gentau_cm_sum += trans_mass * exp(-fabs(genJets[i].v.Rapidity())); 
                        gentau_cm_max = max(gentau_cm_max, trans_mass * exp(-fabs(genJets[i].v.Rapidity()))); 

                        gentau_c_cm_sum += trans_mass / (2 * cosh(genJets[i].v.Rapidity()));
                        gentau_c_cm_max = max(gentau_c_cm_max, trans_mass / (2 * cosh(genJets[i].v.Rapidity())));

                    }

                    for (unsigned short i(0); i < 5; i++) {
                        if (genEWKBoson.Pt() > ZptRange[i] && genEWKBoson.Pt() <= ZptRange[i+1]) {
                            gentau_sum_Zinc1jet[i]->Fill(gentau_sum, genWeight);
                            gentau_max_Zinc1jet[i]->Fill(gentau_max, genWeight);
                            gentau_c_sum_Zinc1jet[i]->Fill(gentau_c_sum, genWeight);
                            gentau_c_max_Zinc1jet[i]->Fill(gentau_c_max, genWeight);
                            gentau_cm_sum_Zinc1jet[i]->Fill(gentau_cm_sum, genWeight);
                            gentau_cm_max_Zinc1jet[i]->Fill(gentau_cm_max, genWeight);
                            gentau_c_cm_sum_Zinc1jet[i]->Fill(gentau_c_cm_sum, genWeight);
                            gentau_c_cm_max_Zinc1jet[i]->Fill(gentau_c_cm_max, genWeight);
                        }
                    }

                    genZNGoodJets_Zinc->Fill(1., genWeight);
                    genZPt_Zinc1jet->Fill(genEWKBoson.Pt(), genWeight);
                    genZRapidity_Zinc1jet->Fill(genEWKBoson.Rapidity(), genWeight);
                    genZAbsRapidity_Zinc1jet->Fill(fabs(genEWKBoson.Rapidity()), genWeight);
                    genZEta_Zinc1jet->Fill(genEWKBoson.Eta(), genWeight);
                    genFirstJetEta_Zinc1jet->Fill(fabs(genJets[0].v.Eta()), genWeight);
                    genFirstJetAbsRapidity_Zinc1jet->Fill(fabs(genJets[0].v.Rapidity()), genWeight);
                    genFirstJetEtaHigh_Zinc1jet->Fill(fabs(genJets[0].v.Eta()), genWeight);
                    genFirstJetAbsRapidity_Zinc1jet->Fill(fabs(genJets[0].v.Rapidity()), genWeight);
                    genFirstJetRapidityHigh_Zinc1jet->Fill(fabs(genJets[0].v.Rapidity()), genWeight);
                    genJetsHT_Zinc1jet->Fill(genJetsHT, genWeight);
                    genSumZJetRapidity_Zinc1jet->Fill(0.5*fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity()), genWeight);
                    genDifZJetRapidity_Zinc1jet->Fill(0.5*fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity()), genWeight);
                    if (nGoodGenJets == 1){
                        genFirstJetPt_Zexc1jet->Fill(genJets[0].v.Pt(), genWeight);
                        //Additional Branch
                        genAbsZRapidity_Zexc1jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                        genAbsJetRapidity_Zexc1jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                        genSumZJetRapidity_Zexc1jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                        genDifZJetRapidity_Zexc1jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);

                        if(genEWKBoson.Pt()>100.)
                        {
                            genAbsZRapidity_ZPt100_Zexc1jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                            genAbsJetRapidity_ZPt100_Zexc1jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                            genSumZJetRapidity_ZPt100_Zexc1jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                            genDifZJetRapidity_ZPt100_Zexc1jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);
                        }

                        if(genEWKBoson.Pt()>150.)
                        {
                            genAbsZRapidity_ZPt150_Zexc1jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                            genAbsJetRapidity_ZPt150_Zexc1jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                            genSumZJetRapidity_ZPt150_Zexc1jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                            genDifZJetRapidity_ZPt150_Zexc1jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);
                        }

                    }
                }
                if (nGoodGenJets_20 >= 2) genSecondJetPt_Zinc2jet->Fill(genJets_20[1].v.Pt(), genWeight);
                if (nGoodGenJets >= 2) {
                    TLorentzVector genJet1Plus2PlusZ = genJet1Plus2 + genEWKBoson;
                    nGenEventsVInc2Jets++;
                    ///////////Special Branch//////////////////
                    genAbsFirstJetRapidity_Zinc2jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                    genSumZFirstJetRapidity_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                    genDifZFirstJetRapidity_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);

                    genAbsZRapidity_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                    genAbsSecondJetRapidity_Zinc2jet->Fill(fabs(genJets[1].v.Rapidity()),genWeight);
                    genSumZSecondJetRapidity_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,genWeight);
                    genDifZSecondJetRapidity_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,genWeight);

                    genSumFirstSecondJetRapidity_Zinc2jet->Fill(fabs(genJets[0].v.Rapidity()+genJets[1].v.Rapidity())/2.0,genWeight);
                    genDifFirstSecondJetRapidity_Zinc2jet->Fill(fabs(genJets[0].v.Rapidity()-genJets[1].v.Rapidity())/2.0,genWeight);

                    TLorentzVector genDiJets = genJets[0].v + genJets[1].v;
                    genSumZTwoJetsRapidity_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()+genDiJets.Rapidity())/2.0,genWeight);
                    genDifZTwoJetsRapidity_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()-genDiJets.Rapidity())/2.0,genWeight);

                    /////Azimuth cross check//////////////////////////////////
                    genDPhiZFirstJet_Zinc2jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[0].v)),genWeight);
                    genDPhiZSecondJet_Zinc2jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[1].v)),genWeight);
                    genDPhiFirstSecondJet_Zinc2jet->Fill(fabs(genJets[0].v.DeltaPhi(genJets[1].v)),genWeight);


                    if(genEWKBoson.Pt()>100.)
                    {
                        genAbsZRapidity_ZPt100_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                        genAbsSecondJetRapidity_ZPt100_Zinc2jet->Fill(fabs(genJets[1].v.Rapidity()),genWeight);
                        genSumZSecondJetRapidity_ZPt100_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,genWeight);
                        genDifZSecondJetRapidity_ZPt100_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,genWeight);
                    }

                    if(genEWKBoson.Pt()>150.)
                    {
                        genAbsZRapidity_ZPt150_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                        genAbsSecondJetRapidity_ZPt150_Zinc2jet->Fill(fabs(genJets[1].v.Rapidity()),genWeight);
                        genSumZSecondJetRapidity_ZPt150_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,genWeight);
                        genDifZSecondJetRapidity_ZPt150_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,genWeight);

                        genDPhiZFirstJet_ZPt150_Zinc2jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[0].v)),genWeight);
                    }

                    if(genEWKBoson.Pt()>300.)
                    {
                        genDPhiZFirstJet_ZPt300_Zinc2jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[0].v)),genWeight);
                    }

                    //Set Jet rapidity discriminator/////

                    if(fabs(genJets[0].v.Rapidity()-genJets[1].v.Rapidity())>2)
                    {
                        genAbsZRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                        genAbsFirstJetRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                        genSumZFirstJetRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                        genDifZFirstJetRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);
                    }

                    if(fabs(genJets[0].v.Rapidity()-genJets[1].v.Rapidity())<2)
                    {
                        genAbsZRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                        genAbsFirstJetRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(genJets[0].v.Rapidity()),genWeight);
                        genSumZFirstJetRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,genWeight);
                        genDifZFirstJetRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,genWeight);
                    }


                    genZNGoodJets_Zinc->Fill(2., genWeight);
                    genTwoJetsPtDiff_Zinc2jet->Fill(genJet1Minus2.Pt(), genWeight);
                    genBestTwoJetsPtDiff_Zinc2jet->Fill(genBestJet1Minus2.Pt(), genWeight);
                    genJetsMass_Zinc2jet->Fill(genJet1Plus2.M(), genWeight);
                    genllJetsMass_Zinc2jet->Fill(genJet1Plus2PlusZ.M(), genWeight);

                    if (genJet1Plus2PlusZ.M() > 450 && genJet1Plus2PlusZ.M() < 600) {
                        if (fabs(genJets[0].v.Eta()) < fabs(genJets[1].v.Eta())) {
                            genCentralJetEta_Zinc2jet->Fill(fabs(genJets[0].v.Eta()), genWeight);
                            genForwardJetEta_Zinc2jet->Fill(fabs(genJets[1].v.Eta()), genWeight);
                            genCentralJetPt_Zinc2jet->Fill(genJets[0].v.Pt(), genWeight);
                            genForwardJetPt_Zinc2jet->Fill(genJets[1].v.Pt(), genWeight);
                        }
                        else {
                            genCentralJetEta_Zinc2jet->Fill(fabs(genJets[1].v.Eta()), genWeight);
                            genForwardJetEta_Zinc2jet->Fill(fabs(genJets[0].v.Eta()), genWeight);
                            genCentralJetPt_Zinc2jet->Fill(genJets[1].v.Pt(), genWeight);
                            genForwardJetPt_Zinc2jet->Fill(genJets[0].v.Pt(), genWeight);
                        }
                    }

                    if (EvtInfo_NumVtx < 14) genJetsMassLowPU_Zinc2jet->Fill(genJet1Plus2.M(), genWeight);
                    else if (EvtInfo_NumVtx < 18) genJetsMassMidPU_Zinc2jet->Fill(genJet1Plus2.M(), genWeight);
                    else genJetsMassHigPU_Zinc2jet->Fill(genJet1Plus2.M(), genWeight);
                    genZPt_Zinc2jet->Fill(genEWKBoson.Pt(), genWeight);
                    genZRapidity_Zinc2jet->Fill(genEWKBoson.Rapidity(), genWeight);
                    genZEta_Zinc2jet->Fill(genEWKBoson.Eta(), genWeight);
                    genSecondJetEta_Zinc2jet->Fill(fabs(genJets[1].v.Eta()), genWeight);
                    genSecondJetAbsRapidity_Zinc2jet->Fill(fabs(genJets[1].v.Rapidity()), genWeight);
                    genSecondJetEtaHigh_Zinc2jet->Fill(fabs(genJets[1].v.Eta()), genWeight);
                    genSecondJetRapidityHigh_Zinc2jet->Fill(fabs(genJets[1].v.Rapidity()), genWeight);
                    genJetsHT_Zinc2jet->Fill(genJetsHT, genWeight);
                    genptBal_Zinc2jet->Fill(genJet1Plus2PlusZ.Pt(), genWeight);
                    gendPhiJets_Zinc2jet->Fill(deltaPhi(genJets[0].v, genJets[1].v), genWeight);
                    genBestdPhiJets_Zinc2jet->Fill(deltaPhi(genBestTwoJets.first, genBestTwoJets.second), genWeight);
                    gendEtaJets_Zinc2jet->Fill(genJets[0].v.Eta() - genJets[1].v.Eta(), genWeight);
                    gendEtaFirstJetZ_Zinc2jet->Fill(genJets[0].v.Eta() - genEWKBoson.Eta(), genWeight);
                    gendEtaSecondJetZ_Zinc2jet->Fill(genJets[1].v.Eta() - genEWKBoson.Eta(), genWeight);
                    gendEtaJet1Plus2Z_Zinc2jet->Fill(genJet1Plus2.Eta() - genEWKBoson.Eta(), genWeight);
                    genPHI_Zinc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                    genBestPHI_Zinc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), genWeight);
                    genPHI_T_Zinc2jet->Fill(PHI_T(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                    genBestPHI_T_Zinc2jet->Fill(PHI_T(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), genWeight);
                    genSpT_Zinc2jet->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                    genBestSpT_Zinc2jet->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), genWeight);
                    genSpTJets_Zinc2jet->Fill(SpTsub(genJets[0].v, genJets[1].v), genWeight);
                    genBestSpTJets_Zinc2jet->Fill(SpTsub(genBestTwoJets.first, genBestTwoJets.second), genWeight);
                    genSpTLeptons_Zinc2jet->Fill(SpTsub(genLeptons[0].v, genLeptons[1].v), genWeight);
                    genSPhi_Zinc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                    genBestSPhi_Zinc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), genWeight);

                    if (genEWKBoson.Pt() < 25){
                        genptBal_LowPt_Zinc2jet->Fill(genJet1Plus2PlusZ.Pt(), genWeight);
                        gendPhiJets_LowPt_Zinc2jet->Fill(deltaPhi(genJets[0].v, genJets[1].v), genWeight);
                        genBestdPhiJets_LowPt_Zinc2jet->Fill(deltaPhi(genBestTwoJets.first, genBestTwoJets.second), genWeight);
                        genPHI_T_LowPt_Zinc2jet->Fill(PHI_T(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        genBestPHI_T_LowPt_Zinc2jet->Fill(PHI_T(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), genWeight);
                        genPHI_LowPt_Zinc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        genBestPHI_LowPt_Zinc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), genWeight);
                        genSpTJets_LowPt_Zinc2jet->Fill(SpTsub(genJets[0].v, genJets[1].v), genWeight);
                        genBestSpTJets_LowPt_Zinc2jet->Fill(SpTsub(genBestTwoJets.first, genBestTwoJets.second), genWeight);
                        genSpTLeptons_LowPt_Zinc2jet->Fill(SpTsub(genLeptons[0].v, genLeptons[1].v), genWeight);
                        genSpT_LowPt_Zinc2jet->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        genBestSpT_LowPt_Zinc2jet->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), genWeight);
                        genSPhi_LowPt_Zinc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        genBestSPhi_LowPt_Zinc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), genWeight);
                        if (SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v) < 0.5){ 
                            genPHI_LowSpT_LowPt_Zinc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            genSPhi_LowSpT_LowPt_Zinc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        }
                        else {
                            genPHI_HighSpT_LowPt_Zinc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            genSPhi_HighSpT_LowPt_Zinc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        }
                        if (SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v) < 0.5){
                            genSpT_LowSPhi_LowPt_Zinc2jet->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        }
                        else {
                            genSpT_HighSPhi_LowPt_Zinc2jet ->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        }
                    }
                    else {
                        genptBal_HighPt_Zinc2jet->Fill(genJet1Plus2PlusZ.Pt(), genWeight);
                        gendPhiJets_HighPt_Zinc2jet->Fill(deltaPhi(genJets[0].v, genJets[1].v), genWeight);
                        genPHI_HighPt_Zinc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        genPHI_T_HighPt_Zinc2jet->Fill(PHI_T(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        genSpTJets_HighPt_Zinc2jet->Fill(SpTsub(genJets[0].v, genJets[1].v), genWeight);
                        genSpTLeptons_HighPt_Zinc2jet->Fill(SpTsub(genLeptons[0].v, genLeptons[1].v), genWeight);
                        genSpT_HighPt_Zinc2jet->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        genSPhi_HighPt_Zinc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                    }
                    if (nGoodGenJets == 2){
                        //////Special Branch/////////////////////////
                        genAbsZRapidity_Zexc2jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                        genAbsSecondJetRapidity_Zexc2jet->Fill(fabs(genJets[1].v.Rapidity()),genWeight);
                        genSumZSecondJetRapidity_Zexc2jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,genWeight);
                        genDifZSecondJetRapidity_Zexc2jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,genWeight);

                        if(genEWKBoson.Pt()>100.)
                        {
                            genAbsZRapidity_ZPt100_Zexc2jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                            genAbsSecondJetRapidity_ZPt100_Zexc2jet->Fill(fabs(genJets[1].v.Rapidity()),genWeight);
                            genSumZSecondJetRapidity_ZPt100_Zexc2jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,genWeight);
                            genDifZSecondJetRapidity_ZPt100_Zexc2jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,genWeight);
                        }

                        if(genEWKBoson.Pt()>150.)
                        {
                            genAbsZRapidity_ZPt150_Zexc2jet->Fill(fabs(genEWKBoson.Rapidity()),genWeight);
                            genAbsSecondJetRapidity_ZPt150_Zexc2jet->Fill(fabs(genJets[1].v.Rapidity()),genWeight);
                            genSumZSecondJetRapidity_ZPt150_Zexc2jet->Fill(fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,genWeight);
                            genDifZSecondJetRapidity_ZPt150_Zexc2jet->Fill(fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,genWeight);
                        }


                        genTwoJetsPtDiff_Zexc2jet->Fill(genJet1Minus2.Pt(), genWeight);
                        genJetsMass_Zexc2jet->Fill(genJet1Plus2.M(), genWeight);
                        genSecondJetPt_Zexc2jet->Fill(genJets[1].v.Pt(), genWeight);
                        gendPhiJets_Zexc2jet->Fill(deltaPhi(genJets[0].v, genJets[1].v), genWeight);
                        genPHI_Zexc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        genPHI_T_Zexc2jet->Fill(PHI_T(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        gendEtaJets_Zexc2jet->Fill(genJets[0].v.Eta() - genJets[1].v.Eta(), genWeight);
                        gendEtaFirstJetZ_Zexc2jet->Fill(genJets[0].v.Eta() - genEWKBoson.Eta(), genWeight);
                        gendEtaSecondJetZ_Zexc2jet->Fill(genJets[1].v.Eta() - genEWKBoson.Eta(), genWeight);
                        gendEtaJet1Plus2Z_Zexc2jet->Fill(genJet1Plus2.Eta() - genEWKBoson.Eta(), genWeight);
                        genSpT_Zexc2jet->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        genSpTJets_Zexc2jet->Fill(SpTsub(genJets[0].v, genJets[1].v), genWeight);
                        genSpTLeptons_Zexc2jet->Fill(SpTsub(genLeptons[0].v, genLeptons[1].v), genWeight);
                        genSPhi_Zexc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        genptBal_Zexc2jet->Fill(genJet1Plus2PlusZ.Pt(), genWeight);

                        if (genEWKBoson.Pt() < 25){
                            genptBal_LowPt_Zexc2jet->Fill(genJet1Plus2PlusZ.Pt(), genWeight);
                            gendPhiJets_LowPt_Zexc2jet->Fill(deltaPhi(genJets[0].v, genJets[1].v), genWeight);
                            genPHI_T_LowPt_Zexc2jet->Fill(PHI_T(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            genPHI_LowPt_Zexc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            genSpTJets_LowPt_Zexc2jet->Fill(SpTsub(genJets[0].v, genJets[1].v), genWeight);
                            genSpTLeptons_LowPt_Zexc2jet->Fill(SpTsub(genLeptons[0].v, genLeptons[1].v), genWeight);
                            genSpT_LowPt_Zexc2jet->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            genSPhi_LowPt_Zexc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            if (SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v) < 0.5) { 
                                genPHI_LowSpT_LowPt_Zexc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                                genSPhi_LowSpT_LowPt_Zexc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            }
                            else {
                                genPHI_HighSpT_LowPt_Zexc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                                genSPhi_HighSpT_LowPt_Zexc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            }
                            if (SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v) < 0.5) {
                                genSpT_LowSPhi_LowPt_Zexc2jet->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            }
                            else {
                                genSpT_HighSPhi_LowPt_Zexc2jet ->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            }
                        }
                        else {
                            genptBal_HighPt_Zexc2jet->Fill(genJet1Plus2PlusZ.Pt(), genWeight);
                            gendPhiJets_HighPt_Zexc2jet->Fill(deltaPhi(genJets[0].v, genJets[1].v), genWeight);
                            genPHI_HighPt_Zexc2jet->Fill(PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            genPHI_T_HighPt_Zexc2jet->Fill(PHI_T(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            genSpTJets_HighPt_Zexc2jet->Fill(SpTsub(genJets[0].v, genJets[1].v), genWeight);
                            genSpTLeptons_HighPt_Zexc2jet->Fill(SpTsub(genLeptons[0].v, genLeptons[1].v), genWeight);
                            genSpT_HighPt_Zexc2jet->Fill(SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                            genSPhi_HighPt_Zexc2jet->Fill(SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), genWeight);
                        }
                    }

                }
                if (nGoodGenJets_20 >= 3) genThirdJetPt_Zinc3jet->Fill(genJets_20[2].v.Pt(), genWeight);
                if (nGoodGenJets >= 3){
                    nGenEventsVInc3Jets++;
                    genZNGoodJets_Zinc->Fill(3., genWeight);
                    genThirdJetEta_Zinc3jet->Fill(fabs(genJets[2].v.Eta()), genWeight);
                    genThirdJetAbsRapidity_Zinc3jet->Fill(fabs(genJets[2].v.Rapidity()), genWeight);
                    genThirdJetEtaHigh_Zinc3jet->Fill(fabs(genJets[2].v.Eta()), genWeight);
                    genThirdJetRapidityHigh_Zinc3jet->Fill(fabs(genJets[2].v.Rapidity()), genWeight);
                    genJetsHT_Zinc3jet->Fill(genJetsHT, genWeight);

                    /////Azimuth cross check//////////////////////////////////
                    genDPhiZFirstJet_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[0].v)),genWeight);
                    genDPhiZSecondJet_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[1].v)),genWeight);
                    genDPhiZThirdJet_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[2].v)),genWeight);
                    genDPhiFirstSecondJet_Zinc3jet->Fill(fabs(genJets[0].v.DeltaPhi(genJets[1].v)),genWeight);
                    genDPhiFirstThirdJet_Zinc3jet->Fill(fabs(genJets[0].v.DeltaPhi(genJets[2].v)),genWeight);
                    genDPhiSecondThirdJet_Zinc3jet->Fill(fabs(genJets[1].v.DeltaPhi(genJets[2].v)),genWeight);

                    if(genEWKBoson.Pt()>150.)
                    {
                        genDPhiZFirstJet_ZPt150_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[0].v)),genWeight);
                        genDPhiZSecondJet_ZPt150_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[1].v)),genWeight);
                        genDPhiZThirdJet_ZPt150_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[2].v)),genWeight);
                        genDPhiFirstSecondJet_ZPt150_Zinc3jet->Fill(fabs(genJets[0].v.DeltaPhi(genJets[1].v)),genWeight);
                        genDPhiFirstThirdJet_ZPt150_Zinc3jet->Fill(fabs(genJets[0].v.DeltaPhi(genJets[2].v)),genWeight);
                        genDPhiSecondThirdJet_ZPt150_Zinc3jet->Fill(fabs(genJets[1].v.DeltaPhi(genJets[2].v)),genWeight);
                    }

                    if(genEWKBoson.Pt()>300.)
                    {
                        genDPhiZFirstJet_ZPt300_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[0].v)),genWeight);
                        genDPhiZSecondJet_ZPt300_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[1].v)),genWeight);
                        genDPhiZThirdJet_ZPt300_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[2].v)),genWeight);
                        genDPhiFirstSecondJet_ZPt300_Zinc3jet->Fill(fabs(genJets[0].v.DeltaPhi(genJets[1].v)),genWeight);
                        genDPhiFirstThirdJet_ZPt300_Zinc3jet->Fill(fabs(genJets[0].v.DeltaPhi(genJets[2].v)),genWeight);
                        genDPhiSecondThirdJet_ZPt300_Zinc3jet->Fill(fabs(genJets[1].v.DeltaPhi(genJets[2].v)),genWeight);
                    }

                    if(genEWKBoson.Pt()>150. && (genJets[0].v.Pt()+genJets[1].v.Pt()+genJets[2].v.Pt()>300.))
                    {
                        genDPhiZFirstJet_ZPt150_HT300_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[0].v)),genWeight);
                        genDPhiZSecondJet_ZPt150_HT300_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[1].v)),genWeight);
                        genDPhiZThirdJet_ZPt150_HT300_Zinc3jet->Fill(fabs(genEWKBoson.DeltaPhi(genJets[2].v)),genWeight);
                    }

                }
                if (nGoodGenJets_20 >= 4) genFourthJetPt_Zinc4jet->Fill(genJets_20[3].v.Pt(), genWeight);
                if (nGoodGenJets >= 4){
                    genZNGoodJets_Zinc->Fill(4., genWeight);
                    genFourthJetEta_Zinc4jet->Fill(fabs(genJets[3].v.Eta()), genWeight);
                    genFourthJetAbsRapidity_Zinc4jet->Fill(fabs(genJets[3].v.Rapidity()), genWeight);
                    genFourthJetEtaHigh_Zinc4jet->Fill(fabs(genJets[3].v.Eta()), genWeight);
                    genFourthJetRapidityHigh_Zinc4jet->Fill(fabs(genJets[3].v.Rapidity()), genWeight);
                    genJetsHT_Zinc4jet->Fill(genJetsHT, genWeight);
                }
                if (nGoodGenJets_20 >= 5) genFifthJetPt_Zinc5jet->Fill(genJets_20[4].v.Pt(), genWeight);
                if (nGoodGenJets >= 5){
                    genZNGoodJets_Zinc->Fill(5., genWeight);
                    genFifthJetEta_Zinc5jet->Fill(fabs(genJets[4].v.Eta()), genWeight);
                    genFifthJetAbsRapidity_Zinc5jet->Fill(fabs(genJets[4].v.Rapidity()), genWeight);
                    genFifthJetEtaHigh_Zinc5jet->Fill(fabs(genJets[4].v.Eta()), genWeight);
                    genFifthJetRapidityHigh_Zinc5jet->Fill(fabs(genJets[4].v.Rapidity()), genWeight);
                    genJetsHT_Zinc5jet->Fill(genJetsHT, genWeight);
                }
                if (nGoodGenJets_20 >= 6) genSixthJetPt_Zinc6jet->Fill(genJets_20[5].v.Pt(), genWeight);
                if (nGoodGenJets >= 6){
                    genZNGoodJets_Zinc->Fill(6., genWeight);
                    genSixthJetEta_Zinc6jet->Fill(fabs(genJets[5].v.Eta()), genWeight);
                    genSixthJetEtaHigh_Zinc6jet->Fill(fabs(genJets[5].v.Eta()), genWeight);
                    genSixthJetRapidityHigh_Zinc6jet->Fill(fabs(genJets[5].v.Rapidity()), genWeight);
                    genJetsHT_Zinc6jet->Fill(genJetsHT, genWeight);
                }
                if (nGoodGenJets >= 7) {
                    genZNGoodJets_Zinc->Fill(7., genWeight);               
                }
                if (nGoodGenJets >= 8) {
                    genZNGoodJets_Zinc->Fill(8., genWeight);
                }
            }
        }
        //=======================================================================================================//
        if (DEBUG) cout << "Stop after line " << __LINE__ << endl;




        //=======================================================================================================//
        //      Selection for Reco Histos      //
        //====================================//
        double tau_sum(0), tau_max(0);
        double tau_c_sum(0), tau_c_max(0);
        double tau_cm_sum(0), tau_cm_max(0);
        double tau_c_cm_sum(0), tau_c_cm_max(0); 

        if (hasRecoInfo && passesLeptonChargeCut && passesTauCut) {
            ZMassFrom60_Zinc0jet->Fill((leptons[0].v + leptons[1].v).M(), weight);
        }
        if (hasRecoInfo && passesLeptonCut && (!bTagJetFound || !rejectBTagEvents)) { 
            //=======================================================================================================//

            if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
            //=======================================================================================================//
            //      Start filling histograms      //
            //====================================//

            //cout << "Selected at reco level" << endl;
            NVtx->Fill(EvtInfo_NumVtx, weight);
            double weightNoPUweight(1);
            if (hasRecoInfo && !isData) weightNoPUweight = weight/puWeight.weight(int(PU_npT));
            NVtx_NoPUweight->Fill(EvtInfo_NumVtx, weightNoPUweight);

            nEventsVInc0Jets++;
            ZNGoodJetsNVtx_Zexc->Fill(nGoodJets, EvtInfo_NumVtx  , weight);
            ZNGoodJets_Zinc->Fill(0., weight);
            ZNGoodJets_Zexc->Fill(nGoodJets, weight);
            ZNGoodJets_Zinc_NoWeight->Fill(0.);
            ZMass_Zinc0jet->Fill(EWKBoson.M(), weight);
            ZPt_Zinc0jet->Fill(EWKBoson.Pt(), weight);
            ZRapidity_Zinc0jet->Fill(EWKBoson.Rapidity(), weight);
            ZEta_Zinc0jet->Fill(EWKBoson.Eta(), weight);
            ZEtaUpTo5_Zinc0jet->Fill(EWKBoson.Eta(), weight);
            lepPt_Zinc0jet->Fill(leptons[0].v.Pt(), weight);
            lepEta_Zinc0jet->Fill(leptons[0].v.Eta(), weight);
            lepPhi_Zinc0jet->Fill(leptons[0].v.Phi(), weight);
            lepPt_Zinc0jet->Fill(leptons[1].v.Pt(), weight);
            lepEta_Zinc0jet->Fill(leptons[1].v.Eta(), weight);
            lepPhi_Zinc0jet->Fill(leptons[1].v.Phi(), weight);
            dPhiLeptons_Zinc0jet->Fill(deltaPhi(leptons[0].v, leptons[1].v), weight);
            dEtaLeptons_Zinc0jet->Fill(leptons[0].v.Eta() - leptons[1].v.Eta(), weight);
            dRLeptons_Zinc0jet->Fill(deltaR(leptons[0].v, leptons[1].v), weight);
            SpTLeptons_Zinc0jet->Fill(SpTsub(leptons[0].v, leptons[1].v), weight);

            if (nGoodJets == 0){
                //TruePU_0->Fill(PU_npT, weight);
                //PU_0->Fill(PU_npIT, weight);
                PU_0->Fill(EvtInfo_NumVtx, weight);
                ZNGoodJets_Zexc_NoWeight->Fill(0.);
                ZPt_Zexc0jet->Fill(EWKBoson.Pt(), weight);
                ZRapidity_Zexc0jet->Fill(EWKBoson.Rapidity(), weight);
                ZEta_Zexc0jet->Fill(EWKBoson.Eta(), weight);
                lepPt_Zexc0jet->Fill(leptons[0].v.Pt(), weight);
                lepEta_Zexc0jet->Fill(leptons[0].v.Eta(), weight);
                lepPt_Zexc0jet->Fill(leptons[1].v.Pt(), weight);
                lepEta_Zexc0jet->Fill(leptons[1].v.Eta(), weight);
                dPhiLeptons_Zexc0jet->Fill(deltaPhi(leptons[0].v, leptons[1].v), weight);
                dEtaLeptons_Zexc0jet->Fill(leptons[0].v.Eta() - leptons[1].v.Eta(), weight);
                SpTLeptons_Zexc0jet->Fill(SpTsub(leptons[0].v, leptons[0].v), weight);
            }

            if (nGoodJets_20 >= 1) {
                FirstJetPt_Zinc1jet->Fill(jets_20[0].v.Pt(), weight);
                FirstJetPt_Zinc1jet_NVtx->Fill(jets_20[0].v.Pt(), EvtInfo_NumVtx, weight);
                FirstJetPtEta_Zinc1jet->Fill(jets_20[0].v.Pt(), fabs(jets[0].v.Eta()), weight);
            }

            if (nGoodJets >= 1){
                nEventsVInc1Jets++;
                AbsZRapidity_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                AbsFirstJetRapidity_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),weight);
                SumZFirstJetRapidity_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                DifZFirstJetRapidity_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);

                //cross check//////
                SumZFirstJetEta_Zinc1jet->Fill(fabs(EWKBoson.Eta()+jets[0].v.Eta())/2.0,weight);
                DifZFirstJetEta_Zinc1jet->Fill(fabs(EWKBoson.Eta()-jets[0].v.Eta())/2.0,weight);

                ///Azimuth cross check/////////////////////////////
                DPhiZFirstJet_Zinc1jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),weight);


                if(EWKBoson.Pt()>100.)
                {
                    AbsZRapidity_ZPt100_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                    AbsFirstJetRapidity_ZPt100_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),weight);
                    SumZFirstJetRapidity_ZPt100_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                    DifZFirstJetRapidity_ZPt100_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);
                }

                if(EWKBoson.Pt()>150.)
                {
                    AbsZRapidity_ZPt150_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                    AbsFirstJetRapidity_ZPt150_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),weight);
                    SumZFirstJetRapidity_ZPt150_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                    DifZFirstJetRapidity_ZPt150_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);

                    DPhiZFirstJet_ZPt150_Zinc1jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),weight);

                }

                if(EWKBoson.Pt()>300.)
                {
                    AbsZRapidity_ZPt300_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                    AbsFirstJetRapidity_ZPt300_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),weight);
                    SumZFirstJetRapidity_ZPt300_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                    DifZFirstJetRapidity_ZPt300_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);

                    DPhiZFirstJet_ZPt300_Zinc1jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),weight);
                }

                ///different JetPt Cuts//////
                if(jets[0].v.Pt()>50.)
                {
                    AbsZRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                    AbsFirstJetRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),weight);
                    SumZFirstJetRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                    DifZFirstJetRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);
                }

                if(jets[0].v.Pt()>80.)
                {
                    AbsZRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                    AbsFirstJetRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),weight);
                    SumZFirstJetRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                    DifZFirstJetRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);
                }

                ZNGoodJets_Zinc->Fill(1., weight);
                ZNGoodJets_Zinc_NoWeight->Fill(1.);
                ZPt_Zinc1jet->Fill(EWKBoson.Pt(), weight);
                ZRapidity_Zinc1jet->Fill(EWKBoson.Rapidity(), weight);
                ZAbsRapidity_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()), weight);
                ZEta_Zinc1jet->Fill(EWKBoson.Eta(), weight);
                ZEtaUpTo5_Zinc1jet->Fill(EWKBoson.Eta(), weight);
                SpTLeptons_Zinc1jet->Fill(SpTsub(leptons[0].v, leptons[1].v), weight);
                FirstJetEta_Zinc1jet->Fill(fabs(jets[0].v.Eta()), weight);
                FirstJetAbsRapidity_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()), weight);
                FirstJetEtaHigh_Zinc1jet->Fill(fabs(jets[0].v.Eta()), weight);
                FirstJetAbsRapidity_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()), weight);
                FirstJetRapidityHigh_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()), weight);
                FirstJetEtaFull_Zinc1jet->Fill(jets[0].v.Eta(), weight);
                FirstJetPhi_Zinc1jet->Fill(jets[0].v.Phi(), weight);
                JetsHT_Zinc1jet->Fill(jetsHT, weight);
                dEtaBosonJet_Zinc1jet->Fill(fabs(jets[0].v.Eta() - EWKBoson.Eta()), weight);
                SumZJetRapidity_Zinc1jet->Fill(0.5*fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity()), weight);
                DifZJetRapidity_Zinc1jet->Fill(0.5*fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity()), weight);

                for (unsigned short i(0); i < nGoodJets; i++) {
                    double trans_mass = jets[i].v.Mt();

                    tau_sum += trans_mass * exp(-fabs(jets[i].v.Rapidity() - EWKBoson.Rapidity())); 
                    tau_max = max(tau_max, trans_mass * exp(-fabs(jets[i].v.Rapidity() - EWKBoson.Rapidity()))); 

                    tau_c_sum += trans_mass / (2 * cosh(jets[i].v.Rapidity() - EWKBoson.Rapidity()));
                    tau_c_max = max(tau_c_max, trans_mass / (2 * cosh(jets[i].v.Rapidity() - EWKBoson.Rapidity())));

                    tau_cm_sum += trans_mass * exp(-fabs(jets[i].v.Rapidity())); 
                    tau_cm_max = max(tau_cm_max, trans_mass * exp(-fabs(jets[i].v.Rapidity()))); 

                    tau_c_cm_sum += trans_mass / (2 * cosh(jets[i].v.Rapidity()));
                    tau_c_cm_max = max(tau_c_cm_max, trans_mass / (2 * cosh(jets[i].v.Rapidity())));
                }

                for (unsigned short i(0); i < 5; i++) {
                    if (EWKBoson.Pt() > ZptRange[i] && EWKBoson.Pt() <= ZptRange[i+1]) {
                        tau_sum_Zinc1jet[i]->Fill(tau_sum, weight);
                        tau_max_Zinc1jet[i]->Fill(tau_max, weight);
                        tau_c_sum_Zinc1jet[i]->Fill(tau_c_sum, weight);
                        tau_c_max_Zinc1jet[i]->Fill(tau_c_max, weight);
                        tau_cm_sum_Zinc1jet[i]->Fill(tau_cm_sum, weight);
                        tau_cm_max_Zinc1jet[i]->Fill(tau_cm_max, weight);
                        tau_c_cm_sum_Zinc1jet[i]->Fill(tau_c_cm_sum, weight);
                        tau_c_cm_max_Zinc1jet[i]->Fill(tau_c_cm_max, weight);
                    }
                }

                if (nGoodJets == 1){
                    //TruePU_1->Fill(PU_npT, weight);
                    //PU_1->Fill(PU_npIT, weight);
                    PU_1->Fill(EvtInfo_NumVtx, weight);
                    ZNGoodJets_Zexc_NoWeight->Fill(1.);
                    ZPt_Zexc1jet->Fill(EWKBoson.Pt(), weight);
                    ZRapidity_Zexc1jet->Fill(EWKBoson.Rapidity(), weight);
                    ZEta_Zexc1jet->Fill(EWKBoson.Eta(), weight);
                    SpTLeptons_Zexc1jet->Fill(SpTsub(leptons[0].v, leptons[1].v), weight);
                    FirstJetPt_Zexc1jet->Fill(jets[0].v.Pt(), weight);
                    FirstJetEta_Zexc1jet->Fill(jets[0].v.Eta(), weight);
                    FirstJetPhi_Zexc1jet->Fill(jets[0].v.Phi(), weight);
                    dEtaBosonJet_Zexc1jet->Fill(fabs(jets[0].v.Eta()-EWKBoson.Eta()), weight);
                    //Additional Branch
                    AbsZRapidity_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()), weight);
                    AbsJetRapidity_Zexc1jet->Fill(fabs(jets[0].v.Rapidity()), weight);
                    SumZJetRapidity_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                    DifZJetRapidity_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);

                    if(EWKBoson.Pt()>100.)
                    {
                        AbsZRapidity_ZPt100_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()), weight);
                        AbsJetRapidity_ZPt100_Zexc1jet->Fill(fabs(jets[0].v.Rapidity()), weight);
                        SumZJetRapidity_ZPt100_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                        DifZJetRapidity_ZPt100_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);
                    }

                    if(EWKBoson.Pt()>150.)
                    {
                        AbsZRapidity_ZPt150_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()), weight);
                        AbsJetRapidity_ZPt150_Zexc1jet->Fill(fabs(jets[0].v.Rapidity()), weight);
                        SumZJetRapidity_ZPt150_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                        DifZJetRapidity_ZPt150_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);
                    }


                }
            }
            if (nGoodJets_20 >= 2) SecondJetPt_Zinc2jet->Fill(jets_20[1].v.Pt(), weight);
            if (nGoodJets >= 2){
                nEventsVInc2Jets++;
                //////////////////Special Branch////////////////////////
                AbsFirstJetRapidity_Zinc2jet->Fill(fabs(jets[0].v.Rapidity()),weight);
                SumZFirstJetRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                DifZFirstJetRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);

                AbsZRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                AbsSecondJetRapidity_Zinc2jet->Fill(fabs(jets[1].v.Rapidity()),weight);
                SumZSecondJetRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,weight);
                DifZSecondJetRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,weight);

                SumFirstSecondJetRapidity_Zinc2jet->Fill(fabs(jets[0].v.Rapidity()+jets[1].v.Rapidity())/2.0,weight);
                DifFirstSecondJetRapidity_Zinc2jet->Fill(fabs(jets[0].v.Rapidity()-jets[1].v.Rapidity())/2.0,weight);

                TLorentzVector DiJets = jets[0].v + jets[1].v;
                SumZTwoJetsRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+DiJets.Rapidity())/2.0,weight);
                DifZTwoJetsRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-DiJets.Rapidity())/2.0,weight);

                ///Azimuth cross check
                DPhiZFirstJet_Zinc2jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),weight);
                DPhiZSecondJet_Zinc2jet->Fill(fabs(EWKBoson.DeltaPhi(jets[1].v)),weight);
                DPhiFirstSecondJet_Zinc2jet->Fill(fabs(jets[0].v.DeltaPhi(jets[1].v)),weight);


                if(EWKBoson.Pt()>100.)
                {
                    AbsZRapidity_ZPt100_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                    AbsSecondJetRapidity_ZPt100_Zinc2jet->Fill(fabs(jets[1].v.Rapidity()),weight);
                    SumZSecondJetRapidity_ZPt100_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,weight);
                    DifZSecondJetRapidity_ZPt100_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,weight);
                }

                if(EWKBoson.Pt()>150.)
                {
                    AbsZRapidity_ZPt150_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                    AbsSecondJetRapidity_ZPt150_Zinc2jet->Fill(fabs(jets[1].v.Rapidity()),weight);
                    SumZSecondJetRapidity_ZPt150_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,weight);
                    DifZSecondJetRapidity_ZPt150_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,weight);

                    DPhiZFirstJet_ZPt150_Zinc2jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),weight);

                }

                if(EWKBoson.Pt()>300.)
                {
                    DPhiZFirstJet_ZPt300_Zinc2jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),weight);
                }

                //set jet rapidity discriminator/////

                if(fabs(jets[0].v.Rapidity()-jets[1].v.Rapidity())>2)
                {
                    AbsZRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                    AbsFirstJetRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(jets[0].v.Rapidity()),weight);
                    SumZFirstJetRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                    DifZFirstJetRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);
                }

                if(fabs(jets[0].v.Rapidity()-jets[1].v.Rapidity())<2)
                {
                    AbsZRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                    AbsFirstJetRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(jets[0].v.Rapidity()),weight);
                    SumZFirstJetRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,weight);
                    DifZFirstJetRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,weight);
                }


                TLorentzVector jet1Plus2PlusZ = jet1Plus2 + EWKBoson;
                ZNGoodJets_Zinc->Fill(2., weight);
                ZNGoodJets_Zinc_NoWeight->Fill(2.);
                TwoJetsPtDiff_Zinc2jet->Fill(jet1Minus2.Pt(), weight);
                BestTwoJetsPtDiff_Zinc2jet->Fill(bestJet1Minus2.Pt(), weight);
                JetsMass_Zinc2jet->Fill(jet1Plus2.M(), weight);
                llJetsMass_Zinc2jet->Fill(jet1Plus2PlusZ.M(), genWeight);

                if (jet1Plus2PlusZ.M() > 450 && jet1Plus2PlusZ.M() < 600) {
                    if (fabs(jets[0].v.Eta()) < fabs(jets[1].v.Eta())) {
                        CentralJetEta_Zinc2jet->Fill(fabs(jets[0].v.Eta()), weight);
                        ForwardJetEta_Zinc2jet->Fill(fabs(jets[1].v.Eta()), weight);
                        CentralJetPt_Zinc2jet->Fill(jets[0].v.Pt(), weight);
                        ForwardJetPt_Zinc2jet->Fill(jets[1].v.Pt(), weight);
                    }
                    else {
                        CentralJetEta_Zinc2jet->Fill(fabs(jets[1].v.Eta()), weight);
                        ForwardJetEta_Zinc2jet->Fill(fabs(jets[0].v.Eta()), weight);
                        CentralJetPt_Zinc2jet->Fill(jets[1].v.Pt(), weight);
                        ForwardJetPt_Zinc2jet->Fill(jets[0].v.Pt(), weight);
                    }
                }

                if (EvtInfo_NumVtx < 14) JetsMassLowPU_Zinc2jet->Fill(jet1Plus2.M(), weight);
                else if (EvtInfo_NumVtx < 18) JetsMassMidPU_Zinc2jet->Fill(jet1Plus2.M(), weight);
                else JetsMassHigPU_Zinc2jet->Fill(jet1Plus2.M(), weight);
                ZPt_Zinc2jet->Fill(EWKBoson.Pt(), weight);
                ZRapidity_Zinc2jet->Fill(EWKBoson.Rapidity(), weight);
                ZEta_Zinc2jet->Fill(EWKBoson.Eta(), weight);
                SpTLeptons_Zinc2jet->Fill(SpTsub(leptons[0].v, leptons[1].v), weight);
                SecondJetEta_Zinc2jet->Fill(fabs(jets[1].v.Eta()), weight);
                SecondJetAbsRapidity_Zinc2jet->Fill(fabs(jets[1].v.Rapidity()), weight);
                SecondJetEtaHigh_Zinc2jet->Fill(fabs(jets[1].v.Eta()), weight);
                SecondJetRapidityHigh_Zinc2jet->Fill(fabs(jets[1].v.Rapidity()), weight);
                SecondJetEtaFull_Zinc2jet->Fill(jets[1].v.Eta(), weight);
                SecondJetPhi_Zinc2jet->Fill(jets[1].v.Phi(), weight);        
                JetsHT_Zinc2jet->Fill(jetsHT, weight);
                ptBal_Zinc2jet->Fill(jet1Plus2PlusZ.Pt(), weight);
                dPhiJets_Zinc2jet->Fill(deltaPhi(jets[0].v, jets[1].v), weight);
                BestdPhiJets_Zinc2jet->Fill(deltaPhi(bestTwoJets.first, bestTwoJets.second), weight);
                dEtaJets_Zinc2jet->Fill(jets[0].v.Eta() - jets[1].v.Eta(), weight);
                dEtaFirstJetZ_Zinc2jet->Fill(jets[0].v.Eta() - EWKBoson.Eta(), weight);
                dEtaSecondJetZ_Zinc2jet->Fill(jets[1].v.Eta() - EWKBoson.Eta(), weight);
                dEtaJet1Plus2Z_Zinc2jet->Fill(jet1Plus2.Eta() - EWKBoson.Eta(), weight);
                PHI_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                BestPHI_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), weight);
                PHI_T_Zinc2jet->Fill(PHI_T(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                BestPHI_T_Zinc2jet->Fill(PHI_T(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), weight);
                SpT_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                BestSpT_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), weight);
                SpTJets_Zinc2jet->Fill(SpTsub(jets[0].v, jets[1].v), weight);
                BestSpTJets_Zinc2jet->Fill(SpTsub(bestTwoJets.first, bestTwoJets.second), weight);
                SPhi_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                BestSPhi_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), weight);

                if (EWKBoson.Pt() < 25){
                    ptBal_LowPt_Zinc2jet->Fill(jet1Plus2PlusZ.Pt(), weight);
                    dPhiJets_LowPt_Zinc2jet->Fill(deltaPhi(jets[0].v, jets[1].v), weight);
                    BestdPhiJets_LowPt_Zinc2jet->Fill(deltaPhi(bestTwoJets.first, bestTwoJets.second), weight);
                    dPhiLeptons_LowPt_Zinc2jet->Fill(deltaPhi(leptons[0].v, leptons[1].v), weight);
                    PHI_LowPt_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    BestPHI_LowPt_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), weight);
                    PHI_T_LowPt_Zinc2jet->Fill(PHI_T(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    BestPHI_T_LowPt_Zinc2jet->Fill(PHI_T(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), weight);
                    SpT_LowPt_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    BestSpT_LowPt_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), weight);
                    SpTJets_LowPt_Zinc2jet->Fill(SpTsub(jets[0].v, jets[1].v), weight);
                    BestSpTJets_LowPt_Zinc2jet->Fill(SpTsub(bestTwoJets.first, bestTwoJets.second), weight);
                    SpTLeptons_LowPt_Zinc2jet->Fill(SpTsub(leptons[0].v, leptons[1].v), weight);
                    SPhi_LowPt_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    BestSPhi_LowPt_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), weight);
                    if (SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){ 
                        PHI_LowSpT_LowPt_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        SPhi_LowSpT_LowPt_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    }
                    else {
                        PHI_HighSpT_LowPt_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        SPhi_HighSpT_LowPt_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    }
                    if (SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){
                        SpT_LowSPhi_LowPt_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    }
                    else {
                        SpT_HighSPhi_LowPt_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    }
                }
                else {
                    ptBal_HighPt_Zinc2jet->Fill(jet1Plus2PlusZ.Pt(),weight);
                    dPhiJets_HighPt_Zinc2jet->Fill(deltaPhi(jets[0].v, jets[1].v), weight);
                    dPhiLeptons_HighPt_Zinc2jet->Fill(deltaPhi(leptons[0].v, leptons[1].v), weight);
                    PHI_HighPt_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    PHI_T_HighPt_Zinc2jet->Fill(PHI_T(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    SpT_HighPt_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    SpTJets_HighPt_Zinc2jet->Fill(SpTsub(jets[0].v, jets[1].v), weight);
                    SpTLeptons_HighPt_Zinc2jet->Fill(SpTsub(leptons[0].v, leptons[1].v), weight);
                    SPhi_HighPt_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    if (SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){
                        PHI_LowSpT_HighPt_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        SPhi_LowSpT_HighPt_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight); 
                    }
                    else {
                        PHI_HighSpT_HighPt_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight); 
                        SPhi_HighSpT_HighPt_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    }
                    if (SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){
                        SpT_LowSPhi_HighPt_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    }
                    else {
                        SpT_HighSPhi_HighPt_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    }
                }
                if (SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){
                    SpT_LowSPhi_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                }
                else {
                    SpT_HighSPhi_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                }
                if (SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){
                    PHI_LowSpT_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    SPhi_LowSpT_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                }
                else {
                    PHI_HighSpT_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    SPhi_HighSpT_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                }
                if (nGoodJets == 2){
                    ////////////Special Branch/////////////////////////////////
                    AbsZRapidity_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                    AbsSecondJetRapidity_Zexc2jet->Fill(fabs(jets[1].v.Rapidity()),weight);
                    SumZSecondJetRapidity_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,weight);
                    DifZSecondJetRapidity_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,weight);

                    if(EWKBoson.Pt()>100.)
                    {
                        AbsZRapidity_ZPt100_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                        AbsSecondJetRapidity_ZPt100_Zexc2jet->Fill(fabs(jets[1].v.Rapidity()),weight);
                        SumZSecondJetRapidity_ZPt100_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,weight);
                        DifZSecondJetRapidity_ZPt100_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,weight);
                    }

                    if(EWKBoson.Pt()>150.)
                    {
                        AbsZRapidity_ZPt150_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()),weight);
                        AbsSecondJetRapidity_ZPt150_Zexc2jet->Fill(fabs(jets[1].v.Rapidity()),weight);
                        SumZSecondJetRapidity_ZPt150_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,weight);
                        DifZSecondJetRapidity_ZPt150_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,weight);
                    }

                    //TruePU_2->Fill(PU_npT, weight);
                    //PU_2->Fill(PU_npIT, weight);              
                    PU_2->Fill(EvtInfo_NumVtx, weight);
                    ZNGoodJets_Zexc_NoWeight->Fill(2.);
                    ZPt_Zexc2jet->Fill(EWKBoson.Pt(), weight);
                    ZRapidity_Zexc2jet->Fill(EWKBoson.Rapidity(), weight);
                    ZEta_Zexc2jet->Fill(EWKBoson.Eta(), weight);
                    SpTLeptons_Zexc2jet->Fill(SpTsub(leptons[0].v, leptons[1].v), weight);
                    SecondJetPt_Zexc2jet->Fill(jets[1].v.Pt(), weight);
                    SecondJetEta_Zexc2jet->Fill(jets[1].v.Eta(), weight);
                    SecondJetPhi_Zexc2jet->Fill(jets[1].v.Phi(), weight); 

                    //-- DPS Histograms
                    TwoJetsPtDiff_Zexc2jet->Fill(jet1Minus2.Pt(), weight);
                    JetsMass_Zexc2jet->Fill(jet1Plus2.M(), weight);
                    ptBal_Zexc2jet->Fill(jet1Plus2PlusZ.Pt(), weight);
                    dPhiJets_Zexc2jet->Fill(deltaPhi(jets[0].v, jets[1].v), weight);
                    dEtaJets_Zexc2jet->Fill(jets[0].v.Eta() - jets[1].v.Eta(), weight);
                    dEtaFirstJetZ_Zexc2jet->Fill(jets[0].v.Eta() - EWKBoson.Eta(), weight);
                    dEtaSecondJetZ_Zexc2jet->Fill(jets[1].v.Eta() - EWKBoson.Eta(), weight);
                    dEtaJet1Plus2Z_Zexc2jet->Fill(jet1Plus2.Eta() - EWKBoson.Eta(), weight);
                    PHI_Zexc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    PHI_T_Zexc2jet->Fill(PHI_T(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    SpT_Zexc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    SpTJets_Zexc2jet->Fill(SpTsub(jets[0].v, jets[1].v), weight);
                    SPhi_Zexc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    if (EWKBoson.Pt() < 25){
                        ptBal_LowPt_Zexc2jet->Fill(jet1Plus2PlusZ.Pt(), weight);
                        dPhiJets_LowPt_Zexc2jet->Fill(deltaPhi(jets[0].v, jets[1].v), weight);
                        PHI_LowPt_Zexc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        PHI_T_LowPt_Zexc2jet->Fill(PHI_T(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        SpT_LowPt_Zexc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        SpTJets_LowPt_Zexc2jet->Fill(SpTsub(jets[0].v, jets[1].v), weight);
                        SpTLeptons_LowPt_Zexc2jet->Fill(SpTsub(leptons[0].v, leptons[1].v), weight);
                        SPhi_LowPt_Zexc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        if (SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){ 
                            PHI_LowSpT_LowPt_Zexc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                            SPhi_LowSpT_LowPt_Zexc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        }
                        else {
                            PHI_HighSpT_LowPt_Zexc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                            SPhi_HighSpT_LowPt_Zexc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        }
                        if (SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){
                            SpT_LowSPhi_LowPt_Zexc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        }
                        else {
                            SpT_HighSPhi_LowPt_Zexc2jet ->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        }
                    }
                    else {
                        ptBal_HighPt_Zexc2jet->Fill(jet1Plus2PlusZ.Pt(),weight);
                        dPhiJets_HighPt_Zexc2jet->Fill(deltaPhi(jets[0].v, jets[1].v), weight);
                        PHI_HighPt_Zexc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        PHI_T_HighPt_Zexc2jet->Fill(PHI_T(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        SpT_HighPt_Zexc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        SpTJets_HighPt_Zexc2jet->Fill(SpTsub(jets[0].v, jets[1].v), weight);
                        SpTLeptons_HighPt_Zexc2jet->Fill(SpTsub(leptons[0].v, leptons[1].v), weight);
                        SPhi_HighPt_Zexc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        if (SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){
                            PHI_LowSpT_HighPt_Zexc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                            SPhi_LowSpT_HighPt_Zexc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight); 
                        }
                        else {
                            PHI_HighSpT_HighPt_Zexc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight); 
                            SPhi_HighSpT_HighPt_Zexc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        }
                        if (SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){
                            SpT_LowSPhi_HighPt_Zexc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        }
                        else {
                            SpT_HighSPhi_HighPt_Zexc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        }
                    }
                    if (SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){
                        SpT_LowSPhi_Zexc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    }
                    else {
                        SpT_HighSPhi_Zexc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    }
                    if (SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v) < 0.5){
                        PHI_LowSpT_Zexc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        SPhi_LowSpT_Zexc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    }
                    else {
                        PHI_HighSpT_Zexc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                        SPhi_HighSpT_Zexc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), weight);
                    }
                }
            }
            if (nGoodJets_20 >= 3) ThirdJetPt_Zinc3jet->Fill(jets_20[2].v.Pt(), weight);
            if (nGoodJets >= 3) {
                nEventsVInc3Jets++;
                ZNGoodJets_Zinc->Fill(3., weight);
                ZNGoodJets_Zinc_NoWeight->Fill(3.);
                ThirdJetEta_Zinc3jet->Fill(fabs(jets[2].v.Eta()), weight);
                ThirdJetAbsRapidity_Zinc3jet->Fill(fabs(jets[2].v.Rapidity()), weight);
                ThirdJetEtaHigh_Zinc3jet->Fill(fabs(jets[2].v.Eta()), weight);
                ThirdJetRapidityHigh_Zinc3jet->Fill(fabs(jets[2].v.Rapidity()), weight);
                ThirdJetEtaFull_Zinc3jet->Fill(jets[2].v.Eta(), weight);
                ThirdJetPhi_Zinc3jet->Fill(jets[2].v.Phi(), weight);        
                JetsHT_Zinc3jet->Fill(jetsHT, weight);

                ///Azimuth cross check
                DPhiZFirstJet_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),weight);
                DPhiZSecondJet_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[1].v)),weight);
                DPhiZThirdJet_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[2].v)),weight);
                DPhiFirstSecondJet_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[1].v)),weight);
                DPhiFirstThirdJet_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[2].v)),weight);
                DPhiSecondThirdJet_Zinc3jet->Fill(fabs(jets[1].v.DeltaPhi(jets[2].v)),weight);

                if(EWKBoson.Pt()>150.)
                {
                    DPhiZFirstJet_ZPt150_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),weight);
                    DPhiZSecondJet_ZPt150_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[1].v)),weight);
                    DPhiZThirdJet_ZPt150_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[2].v)),weight);
                    DPhiFirstSecondJet_ZPt150_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[1].v)),weight);
                    DPhiFirstThirdJet_ZPt150_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[2].v)),weight);
                    DPhiSecondThirdJet_ZPt150_Zinc3jet->Fill(fabs(jets[1].v.DeltaPhi(jets[2].v)),weight);
                }

                if(EWKBoson.Pt()>300.)
                {
                    DPhiZFirstJet_ZPt300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),weight);
                    DPhiZSecondJet_ZPt300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[1].v)),weight);
                    DPhiZThirdJet_ZPt300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[2].v)),weight);
                    DPhiFirstSecondJet_ZPt300_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[1].v)),weight);
                    DPhiFirstThirdJet_ZPt300_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[2].v)),weight);
                    DPhiSecondThirdJet_ZPt300_Zinc3jet->Fill(fabs(jets[1].v.DeltaPhi(jets[2].v)),weight);
                }

                if(EWKBoson.Pt()>150. && (jets[0].v.Pt()+jets[1].v.Pt()+jets[2].v.Pt()>300.))
                {
                    DPhiZFirstJet_ZPt150_HT300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),weight);
                    DPhiZSecondJet_ZPt150_HT300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[1].v)),weight);
                    DPhiZThirdJet_ZPt150_HT300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[2].v)),weight);
                }


                if (nGoodJets == 3){
                    //TruePU_3->Fill(PU_npT, weight);
                    //PU_3->Fill(PU_npIT, weight);
                    PU_3->Fill(EvtInfo_NumVtx, weight);
                    ZNGoodJets_Zexc_NoWeight->Fill(3.);
                }
            }
            if (nGoodJets_20 >= 4) FourthJetPt_Zinc4jet->Fill(jets_20[3].v.Pt(), weight);
            if (nGoodJets >= 4){
                ZNGoodJets_Zinc->Fill(4., weight);
                ZNGoodJets_Zinc_NoWeight->Fill(4.);
                FourthJetEta_Zinc4jet->Fill(fabs(jets[3].v.Eta()), weight);
                FourthJetAbsRapidity_Zinc4jet->Fill(fabs(jets[3].v.Rapidity()), weight);
                FourthJetEtaHigh_Zinc4jet->Fill(fabs(jets[3].v.Eta()), weight);
                FourthJetRapidityHigh_Zinc4jet->Fill(fabs(jets[3].v.Rapidity()), weight);
                FourthJetEtaFull_Zinc4jet->Fill(jets[3].v.Eta(), weight);
                FourthJetPhi_Zinc4jet->Fill(jets[3].v.Phi(), weight);        
                JetsHT_Zinc4jet->Fill(jetsHT, weight);
                if (nGoodJets == 4){
                    //TruePU_4->Fill(PU_npT, weight);
                    //PU_4->Fill(PU_npIT, weight);
                    PU_4->Fill(EvtInfo_NumVtx, weight);
                    ZNGoodJets_Zexc_NoWeight->Fill(4.);
                }
            }    
            if (nGoodJets_20 >= 5) FifthJetPt_Zinc5jet->Fill(jets_20[4].v.Pt(), weight);
            if (nGoodJets >= 5){
                ZNGoodJets_Zinc->Fill(5., weight);
                ZNGoodJets_Zinc_NoWeight->Fill(5.);
                FifthJetEta_Zinc5jet->Fill(fabs(jets[4].v.Eta()), weight);
                FifthJetAbsRapidity_Zinc5jet->Fill(fabs(jets[4].v.Rapidity()), weight);
                FifthJetEtaHigh_Zinc5jet->Fill(fabs(jets[4].v.Eta()), weight);
                FifthJetRapidityHigh_Zinc5jet->Fill(fabs(jets[4].v.Rapidity()), weight);
                FifthJetEtaFull_Zinc5jet->Fill(jets[4].v.Eta(), weight);
                FifthJetPhi_Zinc5jet->Fill(jets[4].v.Phi(), weight);        
                JetsHT_Zinc5jet->Fill(jetsHT, weight);
                if (nGoodJets == 5){
                    //TruePU_5->Fill(PU_npT, weight);
                    //PU_5->Fill(PU_npIT, weight);
                    PU_5->Fill(EvtInfo_NumVtx, weight);
                    ZNGoodJets_Zexc_NoWeight->Fill(5.);
                }
            }    
            if (nGoodJets_20 >= 6) SixthJetPt_Zinc6jet->Fill(jets_20[5].v.Pt(), weight);
            if (nGoodJets >= 6){
                ZNGoodJets_Zinc->Fill(6., weight);
                ZNGoodJets_Zinc_NoWeight->Fill(6.);
                SixthJetEta_Zinc6jet->Fill(fabs(jets[5].v.Eta()), weight);
                SixthJetEtaHigh_Zinc6jet->Fill(fabs(jets[5].v.Eta()), weight);
                SixthJetRapidityHigh_Zinc6jet->Fill(fabs(jets[5].v.Rapidity()), weight);
                SixthJetEtaFull_Zinc6jet->Fill(jets[5].v.Eta(), weight);
                SixthJetPhi_Zinc6jet->Fill(jets[5].v.Phi(), weight);        
                JetsHT_Zinc6jet->Fill(jetsHT, weight);
                if (nGoodJets == 6){
                    //TruePU_6->Fill(PU_npT, weight);
                    //PU_6->Fill(PU_npIT, weight);
                    PU_6->Fill(EvtInfo_NumVtx, weight);
                    ZNGoodJets_Zexc_NoWeight->Fill(6.);
                }
            }
            if (nGoodJets >= 7){
                ZNGoodJets_Zinc->Fill(7., weight);
                if (nGoodJets == 7 ){
                    //TruePU_7->Fill(PU_npT, weight);
                    //PU_7->Fill(PU_npIT, weight);
                    PU_7->Fill(EvtInfo_NumVtx, weight);
                }
            }
            if (nGoodJets >= 8){
                ZNGoodJets_Zinc->Fill(8., weight);
            }

            //=======================================================================================================//
        }

        //=======================================================================================================//
        //             Unfolding               //
        //====================================//
        if (hasRecoInfo && hasGenInfo && (!bTagJetFound || !rejectBTagEvents)) {

            if (nGoodJets_20 >= 1 && nGoodGenJets_20 >= 1) {
                //looks for matching gen jet:
                int igen = 0;
                double dr2 = 999.;
                for(int i = 0; i < nGoodGenJets_20; ++i){
                    double dr2_ = std::pow(jets_20[0].v.Eta() - genJets_20[i].v.Eta(), 2)
                        + std::pow(jets_20[0].v.Pt() - genJets_20[i].v.Pt(), 2);
                    if( dr2_ < dr2) { dr2 = dr2_; igen = i;}
                }
                FirstJetPtRecoOvGen_Zinc1jet_NVtx->Fill(jets_20[0].v.Pt()/genJets_20[igen].v.Pt(), EvtInfo_NumVtx, weight);
            }

            //-- EWKBoson Mass and jet multiplicity
            if (passesgenLeptonCut && passesLeptonCut) {
                hresponseZNGoodJets_Zexc->Fill(nGoodJets, nGoodGenJets, weight);
                hresponseZPt_Zinc0jet->Fill(EWKBoson.Pt(), genEWKBoson.Pt(), weight);
            }

            //-- First Jet Pt 
            if (nGoodGenJets >= 1 && passesgenLeptonCut && nGoodJets >= 1 && passesLeptonCut) {

                hresponseZPt_Zinc1jet->Fill(EWKBoson.Pt(),genEWKBoson.Pt(),weight);
                hresponseZAbsRapidity_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()), fabs(genEWKBoson.Rapidity()), weight);      

                hresponseFirstJetEta_Zinc1jet->Fill(fabs(jets[0].v.Eta()), fabs(genJets[0].v.Eta()), weight);      
                hresponseFirstJetAbsRapidity_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()), fabs(genJets[0].v.Rapidity()), weight);      
                hresponseFirstJetEtaHigh_Zinc1jet->Fill(fabs(jets[0].v.Eta()), fabs(genJets[0].v.Eta()), weight);      
                hresponseFirstJetAbsRapidity_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()), fabs(genJets[0].v.Rapidity()), weight);      
                hresponseFirstJetRapidityHigh_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()), fabs(genJets[0].v.Rapidity()), weight);      
                hresponseJetsHT_Zinc1jet->Fill(jetsHT, genJetsHT, weight);
                // Additional Abs responses of variables
                hresponseAbsZRapidity_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                hresponseAbsFirstJetRapidity_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                hresponseSumZFirstJetRapidity_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                hresponseDifZFirstJetRapidity_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);

                //cross check//////
                hresponseSumZFirstJetEta_Zinc1jet->Fill(fabs(EWKBoson.Eta()+jets[0].v.Eta())/2.0,fabs(genEWKBoson.Eta()+genJets[0].v.Eta())/2.0,weight);
                hresponseDifZFirstJetEta_Zinc1jet->Fill(fabs(EWKBoson.Eta()-jets[0].v.Eta())/2.0,fabs(genEWKBoson.Eta()-genJets[0].v.Eta())/2.0,weight);

                /////Azimuthal cross check////////////////////////////
                hresponseDPhiZFirstJet_Zinc1jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),fabs(genEWKBoson.DeltaPhi(genJets[0].v)),weight);

                if(genEWKBoson.Pt()>100.&&EWKBoson.Pt()>100.)
                {
                    hresponseAbsZRapidity_ZPt100_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsFirstJetRapidity_ZPt100_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                    hresponseSumZFirstJetRapidity_ZPt100_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                    hresponseDifZFirstJetRapidity_ZPt100_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);
                }

                if(genEWKBoson.Pt()>150.&&EWKBoson.Pt()>150.)
                {
                    hresponseAbsZRapidity_ZPt150_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsFirstJetRapidity_ZPt150_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                    hresponseSumZFirstJetRapidity_ZPt150_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                    hresponseDifZFirstJetRapidity_ZPt150_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);

                    hresponseDPhiZFirstJet_ZPt150_Zinc1jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),fabs(genEWKBoson.DeltaPhi(genJets[0].v)),weight);

                }

                if(genEWKBoson.Pt()>300.&&EWKBoson.Pt()>300.)
                {
                    hresponseAbsZRapidity_ZPt300_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsFirstJetRapidity_ZPt300_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                    hresponseSumZFirstJetRapidity_ZPt300_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                    hresponseDifZFirstJetRapidity_ZPt300_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);

                    hresponseDPhiZFirstJet_ZPt300_Zinc1jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),fabs(genEWKBoson.DeltaPhi(genJets[0].v)),weight);
                }

                ///different JetPt Cuts///////

                if(genJets[0].v.Pt()>50.&&jets[0].v.Pt()>50.)
                {
                    hresponseAbsZRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsFirstJetRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                    hresponseSumZFirstJetRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                    hresponseDifZFirstJetRapidity_FirstJetPt50_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);
                }

                if(genJets[0].v.Pt()>80.&&jets[0].v.Pt()>80.)
                {
                    hresponseAbsZRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsFirstJetRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                    hresponseSumZFirstJetRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                    hresponseDifZFirstJetRapidity_FirstJetPt80_Zinc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);
                }

                hresponseSumZJetRapidity_Zinc1jet->Fill(0.5*fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity()), 0.5*fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity()), weight);
                hresponseDifZJetRapidity_Zinc1jet->Fill(0.5*fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity()), 0.5*fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity()), weight);


                for (unsigned short i(0); i < 5; i++) {
                    if (EWKBoson.Pt() > ZptRange[i] && EWKBoson.Pt() <= ZptRange[i+1]) {
                        hresponsetau_sum_Zinc1jet[i]->Fill(tau_sum, gentau_sum, weight);
                        hresponsetau_max_Zinc1jet[i]->Fill(tau_max, gentau_max, weight);
                        hresponsetau_c_sum_Zinc1jet[i]->Fill(tau_c_sum, gentau_c_sum, weight);
                        hresponsetau_c_max_Zinc1jet[i]->Fill(tau_c_max, gentau_c_max, weight);
                        hresponsetau_cm_sum_Zinc1jet[i]->Fill(tau_cm_sum, gentau_cm_sum, weight);
                        hresponsetau_cm_max_Zinc1jet[i]->Fill(tau_cm_max, gentau_cm_max, weight);
                        hresponsetau_c_cm_sum_Zinc1jet[i]->Fill(tau_c_cm_sum, gentau_c_cm_sum, weight);
                        hresponsetau_c_cm_max_Zinc1jet[i]->Fill(tau_c_cm_max, gentau_c_cm_max, weight);
                    }
                }
            }

            if (nGoodGenJets_20 >= 1 && passesgenLeptonCut && nGoodJets_20 >= 1 && passesLeptonCut) {
                hresponseFirstJetPt_Zinc1jet->Fill(jets_20[0].v.Pt(), genJets_20[0].v.Pt(), weight);      
                hresponseFirstJetPtEta_Zinc1jet->Fill(0.5 + FirstJetPtEta_Zinc1jet->FindBin(jets_20[0].v.Pt(), fabs(jets_20[0].v.Eta())), 
                        0.5 + FirstJetPtEta_Zinc1jet->FindBin(genJets_20[0].v.Pt(), fabs(genJets_20[0].v.Eta())),
                        weight);      
            }

            //exclusive one jet case
            if (nGoodGenJets == 1 && passesgenLeptonCut && nGoodJets == 1 && passesLeptonCut){
                hresponseAbsZRapidity_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                hresponseAbsJetRapidity_Zexc1jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                hresponseSumZJetRapidity_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                hresponseDifZJetRapidity_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);

                if(EWKBoson.Pt()>100.&&genEWKBoson.Pt()>100.)
                {
                    hresponseAbsZRapidity_ZPt100_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsJetRapidity_ZPt100_Zexc1jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                    hresponseSumZJetRapidity_ZPt100_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                    hresponseDifZJetRapidity_ZPt100_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);
                }

                if(EWKBoson.Pt()>150.&&genEWKBoson.Pt()>150.)
                {
                    hresponseAbsZRapidity_ZPt150_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsJetRapidity_ZPt150_Zexc1jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                    hresponseSumZJetRapidity_ZPt150_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                    hresponseDifZJetRapidity_ZPt150_Zexc1jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);
                }


            }

            //-- Second Jet Pt inclusive 
            if (nGoodGenJets >= 2 && passesgenLeptonCut && nGoodJets >= 2 && passesLeptonCut) {
                ////////////////////////Special Branch//////////////////////
                hresponseZPt_Zinc2jet->Fill(EWKBoson.Pt(),genEWKBoson.Pt(),weight);

                hresponseAbsFirstJetRapidity_Zinc2jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                hresponseSumZFirstJetRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                hresponseDifZFirstJetRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);

                hresponseAbsZRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                hresponseAbsSecondJetRapidity_Zinc2jet->Fill(fabs(jets[1].v.Rapidity()),fabs(genJets[1].v.Rapidity()),weight);
                hresponseSumZSecondJetRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,weight);
                hresponseDifZSecondJetRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,weight);

                hresponseSumFirstSecondJetRapidity_Zinc2jet->Fill(fabs(jets[0].v.Rapidity()+jets[1].v.Rapidity())/2.0,fabs(genJets[0].v.Rapidity()+genJets[1].v.Rapidity())/2.0,weight);
                hresponseDifFirstSecondJetRapidity_Zinc2jet->Fill(fabs(jets[0].v.Rapidity()-jets[1].v.Rapidity())/2.0,fabs(genJets[0].v.Rapidity()-genJets[1].v.Rapidity())/2.0,weight);

                TLorentzVector genDiJets = genJets[0].v + genJets[1].v;
                TLorentzVector DiJets = jets[0].v + jets[1].v;
                hresponseSumZTwoJetsRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+DiJets.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genDiJets.Rapidity())/2.0,weight);
                hresponseDifZTwoJetsRapidity_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-DiJets.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genDiJets.Rapidity())/2.0,weight);

                /////Azimuthal cross check//////////////////////////////
                hresponseDPhiZFirstJet_Zinc2jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),fabs(genEWKBoson.DeltaPhi(genJets[0].v)),weight);
                hresponseDPhiZSecondJet_Zinc2jet->Fill(fabs(EWKBoson.DeltaPhi(jets[1].v)),fabs(genEWKBoson.DeltaPhi(genJets[1].v)),weight);
                hresponseDPhiFirstSecondJet_Zinc2jet->Fill(fabs(jets[0].v.DeltaPhi(jets[1].v)),fabs(genJets[0].v.DeltaPhi(genJets[1].v)),weight);

                if(EWKBoson.Pt()>100.&&genEWKBoson.Pt()>100.)
                {
                    hresponseAbsZRapidity_ZPt100_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsSecondJetRapidity_ZPt100_Zinc2jet->Fill(fabs(jets[1].v.Rapidity()),fabs(genJets[1].v.Rapidity()),weight);
                    hresponseSumZSecondJetRapidity_ZPt100_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,weight);
                    hresponseDifZSecondJetRapidity_ZPt100_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,weight);
                }

                if(EWKBoson.Pt()>150.&&genEWKBoson.Pt()>150.)
                {
                    hresponseAbsZRapidity_ZPt150_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsSecondJetRapidity_ZPt150_Zinc2jet->Fill(fabs(jets[1].v.Rapidity()),fabs(genJets[1].v.Rapidity()),weight);
                    hresponseSumZSecondJetRapidity_ZPt150_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,weight);
                    hresponseDifZSecondJetRapidity_ZPt150_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,weight);

                    hresponseDPhiZFirstJet_ZPt150_Zinc2jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),fabs(genEWKBoson.DeltaPhi(genJets[0].v)),weight);

                }

                if(EWKBoson.Pt()>300.&&genEWKBoson.Pt()>300.)
                {
                    hresponseDPhiZFirstJet_ZPt300_Zinc2jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),fabs(genEWKBoson.DeltaPhi(genJets[0].v)),weight);
                }

                //set jet rapidity discriminator////

                if(fabs(genJets[0].v.Rapidity()-genJets[1].v.Rapidity())>2 && fabs(jets[0].v.Rapidity()-jets[1].v.Rapidity())>2)
                {
                    hresponseAbsZRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsFirstJetRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                    hresponseSumZFirstJetRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                    hresponseDifZFirstJetRapidity_DifJetRapidityl2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);
                }

                if(fabs(genJets[0].v.Rapidity()-genJets[1].v.Rapidity())<2 && fabs(jets[0].v.Rapidity()-jets[1].v.Rapidity())<2)
                {
                    hresponseAbsZRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsFirstJetRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(jets[0].v.Rapidity()),fabs(genJets[0].v.Rapidity()),weight);
                    hresponseSumZFirstJetRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[0].v.Rapidity())/2.0,weight);
                    hresponseDifZFirstJetRapidity_DifJetRapiditys2_Zinc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[0].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[0].v.Rapidity())/2.0,weight);
                }


                hresponseSecondJetEta_Zinc2jet->Fill(fabs(jets[1].v.Eta()), fabs(genJets[1].v.Eta()), weight);      
                hresponseSecondJetAbsRapidity_Zinc2jet->Fill(fabs(jets[1].v.Rapidity()), fabs(genJets[1].v.Rapidity()), weight);      
                hresponseSecondJetEtaHigh_Zinc2jet->Fill(fabs(jets[1].v.Eta()), fabs(genJets[1].v.Eta()), weight);      
                hresponseSecondJetRapidityHigh_Zinc2jet->Fill(fabs(jets[1].v.Rapidity()), fabs(genJets[1].v.Rapidity()), weight);      
                hresponseJetsHT_Zinc2jet->Fill(jetsHT, genJetsHT, weight);
                //responseTwoJetsPtDiffInc->Fill(jet1Minus2.Pt(), genJet1Minus2.Pt(), weight);
                //responseBestTwoJetsPtDiffInc->Fill(bestJet1Minus2.Pt(), genBestJet1Minus2.Pt(), weight);
                //responseJetsMassInc->Fill(jet1Plus2.M(), genJet1Plus2.M(), weight);
                hresponseJetsMass_Zinc2jet->Fill(jet1Plus2.M(), genJet1Plus2.M(), weight);

                if (EvtInfo_NumVtx < 14) hresponseJetsMassLowPU_Zinc2jet->Fill(jet1Plus2.M(), genJet1Plus2.M(), weight);
                else if (EvtInfo_NumVtx < 18) hresponseJetsMassMidPU_Zinc2jet->Fill(jet1Plus2.M(), genJet1Plus2.M(), weight);
                else hresponseJetsMassHigPU_Zinc2jet->Fill(jet1Plus2.M(), genJet1Plus2.M(), weight);

                //responseBestJetsMassInc->Fill(bestJet1Plus2.M(), genBestJet1Plus2.M(), weight);
                //responseSpTJets_Zinc2jet->Fill(SpTsub(jets[0].v, jets[1].v), SpTsub(genJets[0].v, genJets[1].v), weight);
                //responseBestSpTJets_Zinc2jet->Fill(SpTsub(bestTwoJets.first, bestTwoJets.second), SpTsub(genBestTwoJets.first, genBestTwoJets.second), weight);
                //responseSpT_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), weight);
                //responseBestSpT_Zinc2jet->Fill(SpT(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), SpT(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), weight);
                //responsedPhiJets_Zinc2jet->Fill(deltaPhi(jets[0].v, jets[1].v), deltaPhi(genJets[0].v, genJets[1].v), weight);
                //responseBestdPhiJets_Zinc2jet->Fill(deltaPhi(bestTwoJets.first, bestTwoJets.second), deltaPhi(genBestTwoJets.first, genBestTwoJets.second), weight);
                //responsePHI_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), weight);
                //responseBestPHI_Zinc2jet->Fill(PHI(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), PHI(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), weight);
                //responsePHI_T_Zinc2jet->Fill(PHI_T(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), PHI_T(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), weight);
                //responseBestPHI_T_Zinc2jet->Fill(PHI_T(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), PHI_T(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), weight);
                //responseSPhi_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), weight);
                //responsedEtaJets_Zinc2jet->Fill(fabs(genJets[0].eta-genJets[1].eta),fabs(jets[0].eta-jets[1].eta), weight);
                //responseBestSPhi_Zinc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, bestTwoJets.first, bestTwoJets.second), SPhi(genLeptons[0].v, genLeptons[1].v, genBestTwoJets.first, genBestTwoJets.second), weight);

            }


            if (nGoodGenJets_20 >= 2 && passesgenLeptonCut && nGoodJets_20 >= 2 && passesLeptonCut) {
                hresponseSecondJetPt_Zinc2jet->Fill(jets_20[1].v.Pt(), genJets_20[1].v.Pt(), weight);      
            }

            //-- Second Jet Pt exclusive
            if (nGoodGenJets == 2 && passesgenLeptonCut && nGoodJets == 2 && passesLeptonCut) {
                //////////////////////////////Special Branch/////////////////
                hresponseAbsZRapidity_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                hresponseAbsSecondJetRapidity_Zexc2jet->Fill(fabs(jets[1].v.Rapidity()),fabs(genJets[1].v.Rapidity()),weight);
                hresponseSumZSecondJetRapidity_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,weight);
                hresponseDifZSecondJetRapidity_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,weight);

                if(EWKBoson.Pt()>100.&&genEWKBoson.Pt()>100.)
                {
                    hresponseAbsZRapidity_ZPt100_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsSecondJetRapidity_ZPt100_Zexc2jet->Fill(fabs(jets[1].v.Rapidity()),fabs(genJets[1].v.Rapidity()),weight);
                    hresponseSumZSecondJetRapidity_ZPt100_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,weight);
                    hresponseDifZSecondJetRapidity_ZPt100_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,weight);
                }

                if(EWKBoson.Pt()>150.&&genEWKBoson.Pt()>150.)
                {
                    hresponseAbsZRapidity_ZPt150_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()),fabs(genEWKBoson.Rapidity()),weight);
                    hresponseAbsSecondJetRapidity_ZPt150_Zexc2jet->Fill(fabs(jets[1].v.Rapidity()),fabs(genJets[1].v.Rapidity()),weight);
                    hresponseSumZSecondJetRapidity_ZPt150_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()+jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()+genJets[1].v.Rapidity())/2.0,weight);
                    hresponseDifZSecondJetRapidity_ZPt150_Zexc2jet->Fill(fabs(EWKBoson.Rapidity()-jets[1].v.Rapidity())/2.0,fabs(genEWKBoson.Rapidity()-genJets[1].v.Rapidity())/2.0,weight);
                }

                //

                //responseTwoJetsPtDiffExc->Fill(jet1Minus2.Pt(), genJet1Minus2.Pt(), weight);
                //responseJetsMassExc->Fill(jet1Plus2.M(), genJet1Plus2.M(), weight);
                //responseSpTJets_Zexc2jet->Fill(SpTsub(jets[0].v, jets[1].v), SpTsub(genJets[0].v, genJets[1].v), weight);
                //responseSpT_Zexc2jet->Fill(SpT(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), SpT(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), weight);
                //responsedPhiJets_Zexc2jet->Fill(deltaPhi(jets[0].v, jets[1].v), deltaPhi(genJets[0].v, genJets[1].v), weight);
                //responsePHI_Zexc2jet->Fill(PHI(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), PHI(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), weight);
                //responsePHI_T_Zexc2jet->Fill(PHI_T(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), PHI_T(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), weight);
                //responsedEtaJets_Zexc2jet->Fill(fabs(genJets[0].eta-genJets[1].eta),fabs(jets[0].eta-jets[1].eta), weight);
                //responseSPhi_Zexc2jet->Fill(SPhi(leptons[0].v, leptons[1].v, jets[0].v, jets[1].v), SPhi(genLeptons[0].v, genLeptons[1].v, genJets[0].v, genJets[1].v), weight);
            }

            //-- Third Jet Pt  
            if (nGoodGenJets >= 3 && passesgenLeptonCut && nGoodJets >= 3 && passesLeptonCut) {

                hresponseThirdJetEta_Zinc3jet->Fill(fabs(jets[2].v.Eta()), fabs(genJets[2].v.Eta()), weight);      
                hresponseThirdJetAbsRapidity_Zinc3jet->Fill(fabs(jets[2].v.Rapidity()), fabs(genJets[2].v.Rapidity()), weight);      
                hresponseThirdJetEtaHigh_Zinc3jet->Fill(fabs(jets[2].v.Eta()), fabs(genJets[2].v.Eta()), weight);      
                hresponseThirdJetRapidityHigh_Zinc3jet->Fill(fabs(jets[2].v.Rapidity()), fabs(genJets[2].v.Rapidity()), weight);      
                hresponseJetsHT_Zinc3jet->Fill(jetsHT, genJetsHT, weight);

                /////Azimuthal cross check//////////////////////////////
                hresponseDPhiZFirstJet_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),fabs(genEWKBoson.DeltaPhi(genJets[0].v)),weight);
                hresponseDPhiZSecondJet_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[1].v)),fabs(genEWKBoson.DeltaPhi(genJets[1].v)),weight);
                hresponseDPhiZThirdJet_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[2].v)),fabs(genEWKBoson.DeltaPhi(genJets[2].v)),weight);
                hresponseDPhiFirstSecondJet_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[1].v)),fabs(genJets[0].v.DeltaPhi(genJets[1].v)),weight);
                hresponseDPhiFirstThirdJet_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[2].v)),fabs(genJets[0].v.DeltaPhi(genJets[2].v)),weight);
                hresponseDPhiSecondThirdJet_Zinc3jet->Fill(fabs(jets[1].v.DeltaPhi(jets[2].v)),fabs(genJets[1].v.DeltaPhi(genJets[2].v)),weight);

                if(EWKBoson.Pt()>150.&&genEWKBoson.Pt()>150.)
                {
                    hresponseDPhiZFirstJet_ZPt150_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),fabs(genEWKBoson.DeltaPhi(genJets[0].v)),weight);
                    hresponseDPhiZSecondJet_ZPt150_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[1].v)),fabs(genEWKBoson.DeltaPhi(genJets[1].v)),weight);
                    hresponseDPhiZThirdJet_ZPt150_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[2].v)),fabs(genEWKBoson.DeltaPhi(genJets[2].v)),weight);
                    hresponseDPhiFirstSecondJet_ZPt150_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[1].v)),fabs(genJets[0].v.DeltaPhi(genJets[1].v)),weight);
                    hresponseDPhiFirstThirdJet_ZPt150_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[2].v)),fabs(genJets[0].v.DeltaPhi(genJets[2].v)),weight);
                    hresponseDPhiSecondThirdJet_ZPt150_Zinc3jet->Fill(fabs(jets[1].v.DeltaPhi(jets[2].v)),fabs(genJets[1].v.DeltaPhi(genJets[2].v)),weight);
                }

                if(EWKBoson.Pt()>300.&&genEWKBoson.Pt()>300.)
                {
                    hresponseDPhiZFirstJet_ZPt300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),fabs(genEWKBoson.DeltaPhi(genJets[0].v)),weight);
                    hresponseDPhiZSecondJet_ZPt300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[1].v)),fabs(genEWKBoson.DeltaPhi(genJets[1].v)),weight);
                    hresponseDPhiZThirdJet_ZPt300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[2].v)),fabs(genEWKBoson.DeltaPhi(genJets[2].v)),weight);
                    hresponseDPhiFirstSecondJet_ZPt300_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[1].v)),fabs(genJets[0].v.DeltaPhi(genJets[1].v)),weight);
                    hresponseDPhiFirstThirdJet_ZPt300_Zinc3jet->Fill(fabs(jets[0].v.DeltaPhi(jets[2].v)),fabs(genJets[0].v.DeltaPhi(genJets[2].v)),weight);
                    hresponseDPhiSecondThirdJet_ZPt300_Zinc3jet->Fill(fabs(jets[1].v.DeltaPhi(jets[2].v)),fabs(genJets[1].v.DeltaPhi(genJets[2].v)),weight);
                }

                if(EWKBoson.Pt()>150. && genEWKBoson.Pt()>150. && (jets[0].v.Pt()+jets[1].v.Pt()+jets[2].v.Pt()>300.) && (genJets[0].v.Pt()+genJets[1].v.Pt()+genJets[2].v.Pt()>300.))
                {
                    hresponseDPhiZFirstJet_ZPt150_HT300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[0].v)),fabs(genEWKBoson.DeltaPhi(genJets[0].v)),weight);
                    hresponseDPhiZSecondJet_ZPt150_HT300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[1].v)),fabs(genEWKBoson.DeltaPhi(genJets[1].v)),weight);
                    hresponseDPhiZThirdJet_ZPt150_HT300_Zinc3jet->Fill(fabs(EWKBoson.DeltaPhi(jets[2].v)),fabs(genEWKBoson.DeltaPhi(genJets[2].v)),weight);
                }

            }


            if (nGoodGenJets_20 >= 3 && passesgenLeptonCut && nGoodJets_20 >= 3 && passesLeptonCut) {
                hresponseThirdJetPt_Zinc3jet->Fill(jets_20[2].v.Pt(), genJets_20[2].v.Pt(), weight);      
            }


            //-- Fourth Jet Pt  
            if (nGoodGenJets >= 4 && passesgenLeptonCut && nGoodJets >= 4 && passesLeptonCut){

                hresponseFourthJetEta_Zinc4jet->Fill(fabs(jets[3].v.Eta()), fabs(genJets[3].v.Eta()), weight);      
                hresponseFourthJetAbsRapidity_Zinc4jet->Fill(fabs(jets[3].v.Rapidity()), fabs(genJets[3].v.Rapidity()), weight);      
                hresponseFourthJetEtaHigh_Zinc4jet->Fill(fabs(jets[3].v.Eta()), fabs(genJets[3].v.Eta()), weight);      
                hresponseFourthJetRapidityHigh_Zinc4jet->Fill(fabs(jets[3].v.Rapidity()), fabs(genJets[3].v.Rapidity()), weight);      
                hresponseJetsHT_Zinc4jet->Fill(jetsHT, genJetsHT, weight);
            }

            if (nGoodGenJets_20 >= 4 && passesgenLeptonCut && nGoodJets_20 >= 4 && passesLeptonCut){
                hresponseFourthJetPt_Zinc4jet->Fill(jets_20[3].v.Pt(), genJets_20[3].v.Pt(), weight);      
            }

            //-- Fifth Jet Pt  
            if (nGoodGenJets >= 5 && passesgenLeptonCut && nGoodJets >= 5 && passesLeptonCut){

                hresponseFifthJetEta_Zinc5jet->Fill(fabs(jets[4].v.Eta()), fabs(genJets[4].v.Eta()), weight);      
                hresponseFifthJetAbsRapidity_Zinc5jet->Fill(fabs(jets[4].v.Rapidity()), fabs(genJets[4].v.Rapidity()), weight);      
                hresponseFifthJetEtaHigh_Zinc5jet->Fill(fabs(jets[4].v.Eta()), fabs(genJets[4].v.Eta()), weight);      
                hresponseFifthJetRapidityHigh_Zinc5jet->Fill(fabs(jets[4].v.Rapidity()), fabs(genJets[4].v.Rapidity()), weight);      
                hresponseJetsHT_Zinc5jet->Fill(jetsHT, genJetsHT, weight);
            } 

            if (nGoodGenJets_20 >= 5 && passesgenLeptonCut && nGoodJets_20 >= 5 && passesLeptonCut){
                hresponseFifthJetPt_Zinc5jet->Fill(jets_20[4].v.Pt(), genJets_20[4].v.Pt(), weight);      
            }

        }
        //=======================================================================================================//

    } //End of loop over all the events
    cout << endl;
    //==========================================================================================================//

    if (DEBUG) cout << "Stop after line " << __LINE__ << endl;
    //==========================================================================================================//
    //         Writing file              //
    //==================================//

    outputFile->cd();

    //--- Save all the histograms ---
    unsigned short numbOfHistograms = listOfHistograms.size();
    for (unsigned short i(0); i < numbOfHistograms; i++){
        string hName = listOfHistograms[i]->GetName();
        if ((!hasGenInfo && hName.find("gen") != string::npos) || (!hasRecoInfo && hName.find("gen") == string::npos)) continue; 
        listOfHistograms[i]->Write();        
    }

    //--- let's delete all histograms --- 
    for (unsigned short i(0); i < numbOfHistograms; i++){
        delete listOfHistograms[i];
    }

    outputFile->Write();
    outputFile->Close();

    //==========================================================================================================//


    cout << "Number of processed events                                : " << nEvents << endl;
    cout << "Number with two good leptons                              : " << nEventsWithTwoGoodLeptons << endl;
    cout << "Number with two good leptons of opp. charge               : " << nEventsWithTwoGoodLeptonsWithOppCharge << endl;
    cout << "Number with two good leptons of opp. charge and good mass : " << nEventsWithTwoGoodLeptonsWithOppChargeAndGoodMass << endl;
    cout << "Number Reco Inclusif V + 0 jets                           : " << nEventsVInc0Jets << endl;
    cout << "Number Reco Inclusif V + 1 jets                           : " << nEventsVInc1Jets << endl;
    cout << "Number Reco Inclusif V + 2 jets                           : " << nEventsVInc2Jets << endl;
    cout << "Number Reco Inclusif V + 3 jets                           : " << nEventsVInc3Jets << endl;
    cout << "Number GEN Inclusif V + 0 jets                            : " << nGenEventsVInc0Jets << endl;
    cout << "Number GEN Inclusif V + 1 jets                            : " << nGenEventsVInc1Jets << endl;
    cout << "Number GEN Inclusif V + 2 jets                            : " << nGenEventsVInc2Jets << endl;
    cout << "Number GEN Inclusif V + 3 jets                            : " << nGenEventsVInc3Jets << endl;
    cout << "Rescal for amcNLO                                         : " << weight_amcNLO_sum << endl;
}

void ZJets::initLHAPDF(TString pdfSet, int pdfMember)
{
    LHAPDF::initPDFSet(1, pdfSet.Data(), pdfMember);
    //LHAPDF::initPDFSet(1, "CT10.LHgrid");
    LHAPDF::initPDFSet(2, "cteq6ll.LHpdf");
    if (pdfMember > LHAPDF::numberPDF(1) + 1) {
        cout << "Warning pdfMember to high" << endl;
        return;
    }
}

double ZJets::computePDFWeight()
{
    //-- get the pdgId of the two colliding partons 
    double wPdf(1.);
    int id1 = pdfInfo_->at(0);
    int id2 = pdfInfo_->at(1);
    if (id1 == 21) id1 = 0; // 21 is Pythia convention for gluon, but needs to be 0 for LHAPDF
    if (id2 == 21) id2 = 0;

    double pdf1  = LHAPDF::xfx(1, pdfInfo_->at(2), pdfInfo_->at(4), id1);
    double pdf2  = LHAPDF::xfx(1, pdfInfo_->at(3), pdfInfo_->at(4), id2);
    double pdf01 = LHAPDF::xfx(2, pdfInfo_->at(2), pdfInfo_->at(4), id1);
    double pdf02 = LHAPDF::xfx(2, pdfInfo_->at(3), pdfInfo_->at(4), id2);

    if (pdfInfo_->at(2) * pdfInfo_->at(3) > 0) {
        wPdf = pdf1 * pdf2;
        if (pdf01*pdf02 <= 0 || pdf1*pdf2 <= 0) {
            cout << "Small problem" << endl;
            wPdf = 1;
        }
        else {
            wPdf /= (pdf01 * pdf02);
        }
    }
    return wPdf;
}

void ZJets::getMuons(vector<leptonStruct>& leptons,  vector<leptonStruct>& vetoMuons)
{
    //--- get the number of Muon candidates from the vector size ---
    unsigned short nTotLeptons(patMuonEta_->size());

    bool eventTrigger = false;
    // we also have event trigger variables --> we should at least match one of the leptons to trigger
    for (unsigned short i(0); i < nTotLeptons; i++) {
        int whichTrigger(patMuonTrig_->at(i));
        if (lepSel == "SMu" && (whichTrigger & 0x1)) eventTrigger = true;
    }

    for (unsigned short i(0); i < nTotLeptons; i++) {
        double muonId = 0;

        if(fileName.Index("mcatnlo") >= 0 || fileName.Index("MG-MLM") >= 0) {
            muonId = (double) patMuonCombId_Int->at(i);
        }
        else { 
            muonId = (double) patMuonCombId_Double->at(i);
        }

        leptonStruct mu(patMuonPt_->at(i), 
                patMuonEta_->at(i), 
                patMuonPhi_->at(i), 
                patMuonEn_->at(i), 
                patMuonCharge_->at(i), 
                muonId, 
                patMuonPfIsoDbeta_->at(i), 
                patMuonEta_->at(i),
                patMuonTrig_->at(i));

        float qter = 1.0;
        /*        if (doRochester) {
                  if (!isData) {
                  rmcor->momcor_mc(mu.v, (float)mu.charge, 0, qter);
                  }
                  else {
                  rmcor->momcor_data(mu.v, (float)mu.charge, 0, qter);
                  }
                  } */
        //--- good muons ---
        bool muPassesPtCut(mu.v.Pt() >= (lepPtCutMin*0.8));
        bool muPassesEtaCut(fabs(mu.v.Eta()) <= 0.1*lepEtaCutMax);
        bool muPassesIdCut(mu.id & 0x1); // Tight muon Id
        bool muPassesIsoCut(0);
        if (lepSel == "DMu" && mu.iso < 0.2) muPassesIsoCut = 1;  
        else if (lepSel == "SMu" && mu.iso < 0.12) muPassesIsoCut = 1;  
        bool muPassesTrig(0);
        if (lepSel == "DMu" && (mu.trigger & 0x8)) muPassesTrig = 1;       // HLT_Mu17_Mu8 
        else if (lepSel == "SMu" && (mu.trigger & 0x1)) muPassesTrig = 1;  // HLT_IsoMu24_eta2p1_v

        //--- veto muons ---
        bool muPassesVetoPtCut(mu.v.Pt() >= 15);
        bool muPassesVetoEtaCut(fabs(mu.v.Eta()) <= 2.4);
        bool muPassesVetoIdCut(mu.id > 0); // muon Id




        /// for files obtained form bugra
        if (fileName.Index("Sherpa_Bugra_1_13_UNFOLDING") >= 0 && mu.trigger > 0) muPassesTrig = 1; // Bugra only keeps the double electron trigger !!!!! 

        // select the good muons only
        //cout << i << " pt=" << mu.v.Pt() << " eta=" << mu.v.Eta() << " phi=" << mu.v.Phi() << endl; 
        //cout << "id=" << muPassesIdCut << " iso=" << muPassesIsoCut << " trig=" << muPassesTrig << endl;
        if (muPassesPtCut && muPassesEtaCut && muPassesIdCut && muPassesIsoCut && (!useTriggerCorrection || muPassesTrig || eventTrigger)) {
            leptons.push_back(mu); 
        }
        // select the veto muons
        else if (lepSel == "SMu" && muPassesVetoPtCut && muPassesVetoEtaCut && muPassesVetoIdCut) {
            vetoMuons.push_back(mu); 
        }

    }//End of loop over all the muons
}

void ZJets::getElectrons(vector<leptonStruct>& leptons,  vector<leptonStruct>& vetoElectrons)
{
    //--- get the number of Electron candidates from the vector size ---
    unsigned short nTotLeptons(patElecEta_->size());

    // if we don't really care to match both leptons to trigger
    bool eventTrigger = false;
    for (unsigned short i(0); i < nTotLeptons; i++){
        int whichTrigger(patElecTrig_->at(i));
        if (lepSel == "DE" && (whichTrigger & 0x2)) eventTrigger = true;      // HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_v
        else if (lepSel == "SE" && (whichTrigger & 0x1)) eventTrigger = true; // HLT_Ele27_WP80_v
    }
    for (unsigned short i(0); i < nTotLeptons; i++){

        leptonStruct ele(patElecPt_->at(i), 
                patElecEta_->at(i), 
                patElecPhi_->at(i), 
                patElecEn_->at(i), 
                patElecCharge_->at(i), 
                patElecID_->at(i), 
                patElecPfIsoRho_->at(i), 
                patElecScEta_->at(i), 
                patElecTrig_->at(i));

        //--- good electrons ---
        bool elePassesPtCut(ele.v.Pt() >= (lepPtCutMin*0.8));
        bool elePassesEtaCut(fabs(ele.scEta) <= min(1.4442, 0.1*lepEtaCutMax) || (fabs(ele.scEta) >= 1.566 && fabs(ele.scEta) <= 0.1*lepEtaCutMax));
        bool elePassesIdCut(ele.id >= 4); /// 4 is medium ID, 2 is Loose ID
        bool elePassesIsoCut(ele.iso < 0.15);
        bool elePassesAnyTrig(ele.trigger & 0x2);
        if (fileName.Index("Sherpa_Bugra_1_13_UNFOLDING") > 0) elePassesAnyTrig = true;

        //--- veto electrons ---
        bool elePassesVetoPtCut(ele.v.Pt() >= 15);
        bool elePassesVetoEtaCut(fabs(ele.v.Eta()) <= 2.4);
        bool elePassesVetoIdCut(ele.id >= 2); // ele Id
        bool elePassesVetoIsoCut(ele.iso < 0.25);

        // select the good electrons only
        if (elePassesPtCut && elePassesEtaCut && elePassesIdCut && elePassesIsoCut && (!useTriggerCorrection || elePassesAnyTrig || eventTrigger)){
            leptons.push_back(ele);
        }
        // select the veto electrons
        else if (elePassesVetoPtCut && elePassesVetoEtaCut && elePassesVetoIdCut && elePassesVetoIsoCut) {
            vetoElectrons.push_back(ele);
        }

    }//End of loop over all the electrons
}


ZJets::ZJets(TString fileName_, float lumiScale_, bool useTriggerCorrection_,
        int systematics_, int direction_, float xsecfactor_, int lepPtCutMin_, int lepEtaCutMax_, 
        int jetPtCutMin_, int jetEtaCutMax_,  Long_t maxEvents_, TString outDir_, TString bonzaiDir): 
    HistoSetZJets(fileName_(0, fileName_.Index("_"))), outputDirectory(outDir_),
    fileName(fileName_), lumiScale(lumiScale_), useTriggerCorrection(useTriggerCorrection_), 
    systematics(systematics_), direction(direction_), xsecfactor(xsecfactor_), 
    lepPtCutMin(lepPtCutMin_), lepEtaCutMax(lepEtaCutMax_), jetPtCutMin(jetPtCutMin_), jetEtaCutMax(jetEtaCutMax_),
    nMaxEvents(maxEvents_)
{

    //--- Create output directory if necessary ---
    if (nMaxEvents > 0) {
        outputDirectory.Remove(TString::kTrailing, '/');
        outputDirectory += TString::Format("_%ldevts/", nMaxEvents);
        cout << "Output directory has been changed to " << outputDirectory << endl;
    }

    TString command = "mkdir -p " + outputDirectory;
    system(command);
    //--------------------------------------------

    TChain *chain = new TChain("", "");

    isData = (fileName.Index("Data") >= 0); 
    TString fullFileName = bonzaiDir + fileName;


    if (fileName.BeginsWith("DMu_")) lepSel = "DMu";
    else if (fileName.BeginsWith("DE_"))  lepSel = "DE"; 
    else if (fileName.BeginsWith("SE_"))  lepSel = "SE"; 
    else if (fileName.BeginsWith("SMu_"))  lepSel = "SMu"; 

    rejectBTagEvents = lepSel.BeginsWith("S"); 

    if (fileName.Index("List") < 0){
        fullFileName += ".root";
        TString treePath = fullFileName + "/tree/tree";
        if (fileName.Index("mcatnlo") >= 0) treePath = fullFileName + "/tree";
        if (fileName.Index("MG-MLM") >= 0) treePath = fullFileName + "/tree";
        if (fileName.Index("Sherpa") >= 0) treePath = fullFileName + "/tree";
        cout << "Loading file: " << fullFileName << endl;
        chain->Add(treePath);
    }
    else {
        fullFileName += ".txt";
        ifstream infile(fullFileName.Data());
        string line; 
        int countFiles(0);
        while (getline(infile, line)){
            countFiles++;
            TString treePath = line + "/tree/tree";
            if (fileName.Index("Sherpa") >= 0) treePath = line + "/tree";
            chain->Add(treePath);       
        }
    }
    fChain = chain;
}

ZJets::~ZJets(){
    if (!fChain) return;
    delete fChain->GetCurrentFile();
}

string ZJets::CreateOutputFileName(TString pdfSet, int pdfMember, double muR, double muF)
{
    ostringstream result;
    result << outputDirectory << fileName;
    result << "_TrigCorr_" << useTriggerCorrection;
    result << "_Syst_" << systematics;
    if (direction == 1) result << "_Up";
    else if (direction == -1) result << "_Down";
    result << "_JetPtMin_" << jetPtCutMin;
    result << "_JetEtaMax_" << jetEtaCutMax;

    if (muR != 0 && muF != 0) result << "_muR_" << muR << "_muF_" << muF;
    if (pdfSet != "") result << "_PDF_" << pdfSet << "_" << pdfMember;
    if (pdfSet == "" && pdfMember != -1) result << "_NNPDF_" << pdfMember;
    //--- Add your test names here ---
    //result << "_NoPUCut";
    //result << "_LooseID";

    result << ".root";
    return result.str();
}

Int_t ZJets::GetEntry(Long64_t entry){
    // Read contents of entry.
    if (!fChain) return 0;
    return fChain->GetEntry(entry);
}

Long64_t ZJets::LoadTree(Long64_t entry){
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

void ZJets::Init(bool hasRecoInfo, bool hasGenInfo){
    // The Init() function is called when the selector needs to initialize
    // a new tree or chain. Typically here the branch addresses and branch
    // pointers of the tree will be set.
    // It is normally not necessary to make changes to the generated
    // code, but the routine can be extended by the user if needed.
    // Init() will be called many times when running on PROOF
    // (once per file to be processed).

    // Set object pointer
    pdfInfo_ = 0;
    genLepPt_ = 0;
    genLepEta_ = 0;
    genLepPhi_ = 0;
    genLepE_ = 0;
    genLepQ_ = 0;
    genLepId_ = 0;
    genLepSt_ = 0;
    genPhoPt_ = 0;
    genPhoEta_ = 0;
    genPhoPhi_ = 0;
    genJetPt_ = 0;
    genJetEta_ = 0;
    genJetPhi_ = 0;
    genJetE_ = 0;

    patElecPt_ = 0;
    patElecEta_ = 0;
    patElecPhi_ = 0;
    patElecEn_ = 0;
    patElecCharge_ = 0;
    patElecID_ = 0;
    patElecTrig_ = 0;
    patElecPfIsoRho_ = 0;
    patElecScEta_ = 0;

    patMuonPt_ = 0;
    patMuonEta_ = 0;
    patMuonPhi_ = 0;
    patMuonEn_ = 0;
    patMuonCharge_ = 0;
    patMuonCombId_Int = 0;
    patMuonCombId_Double = 0;
    patMuonTrig_ = 0;
    patMuonPfIsoDbeta_ = 0;

    patJetPfAk05En_ = 0;
    patJetPfAk05Pt_ = 0;
    patJetPfAk05Eta_ = 0;
    patJetPfAk05Phi_ = 0;
    patJetPfAk05LooseId_ = 0;
    patJetPfAk05jetpuMVA_ = 0;
    patJetPfAk05OCSV_ = 0;
    patJetPfAk05PartonFlavour_ = 0;

    patMetPt_ = 0;
    patMetPhi_ = 0;
    patMetSig_ = 0;
    mcSherpaWeights_ = 0; 
    //weight_amcNLO_ = 0; 
    //weight_amcNLO_sum_ = 0; 
    mcEventWeight_ = 0;

    // Set branch addresses and branch pointers
    fCurrent = -1;
    fChain->SetMakeClass(1);
    if (fileName.Index("Data") < 0) {
        fChain->SetBranchAddress("PU_npT", &PU_npT, &b_PU_npT);
        fChain->SetBranchAddress("PU_npIT", &PU_npIT, &b_PU_npIT);
    }
    if (hasRecoInfo){
        fChain->SetBranchAddress("EvtInfo_NumVtx", &EvtInfo_NumVtx, &b_EvtInfo_NumVtx);
        fChain->SetBranchAddress("EvtInfo_RunNum", &EvtInfo_RunNum, &b_EvtInfo_RunNum); 
        fChain->SetBranchAddress("EvtInfo_EventNum", &EvtInfo_EventNum, &b_EvtInfo_EventNum); 
        fChain->SetBranchAddress("patJetPfAk05En_", &patJetPfAk05En_, &b_patJetPfAk05En_);
        fChain->SetBranchAddress("patJetPfAk05Pt_", &patJetPfAk05Pt_, &b_patJetPfAk05Pt_);
        fChain->SetBranchAddress("patJetPfAk05Eta_", &patJetPfAk05Eta_, &b_patJetPfAk05Eta_);
        fChain->SetBranchAddress("patJetPfAk05Phi_", &patJetPfAk05Phi_, &b_patJetPfAk05Phi_);
        fChain->SetBranchAddress("patJetPfAk05LooseId_", &patJetPfAk05LooseId_, &b_patJetPfAk05LooseId_);
        fChain->SetBranchAddress("patJetPfAk05jetpuMVA_", &patJetPfAk05jetpuMVA_, &b_patJetPfAk05jetpuMVA_);
        fChain->SetBranchAddress("patJetPfAk05OCSV_", &patJetPfAk05OCSV_, &b_patJetPfAk05OCSV_);
        //fChain->SetBranchAddress("patJetPfAk05PartonFlavour_", &patJetPfAk05PartonFlavour_, &b_patJetPfAk05PartonFlavour_);
        fChain->SetBranchAddress("patMetPt_", &patMetPt_, &b_patMetPt_);
        fChain->SetBranchAddress("patMetPhi_", &patMetPhi_, &b_patMetPhi_);
        //fChain->SetBranchAddress("patMetSig_", &patMetSig_, &b_patMetSig_); // not used

        if (lepSel == "DE" || lepSel == "SE"){
            fChain->SetBranchAddress("patElecPt_", &patElecPt_, &b_patElecPt_);
            fChain->SetBranchAddress("patElecEta_", &patElecEta_, &b_patElecEta_);
            fChain->SetBranchAddress("patElecPhi_", &patElecPhi_, &b_patElecPhi_);
            fChain->SetBranchAddress("patElecEnergy_", &patElecEn_, &b_patElecEn_);
            fChain->SetBranchAddress("patElecCharge_", &patElecCharge_, &b_patElecCharge_);
            fChain->SetBranchAddress("patElecID_", &patElecID_, &b_patElecID_);
            fChain->SetBranchAddress("patElecTrig_", &patElecTrig_, &b_patElecTrig_);
            fChain->SetBranchAddress("patElecPfIsoRho_", &patElecPfIsoRho_, &b_patElecPfIsoRho_); 
            fChain->SetBranchAddress("patElecScEta_", &patElecScEta_, &b_patElecScEta_);
        }
        if (lepSel == "DMu" || lepSel == "SMu"){
            fChain->SetBranchAddress("patMuonPt_", &patMuonPt_, &b_patMuonPt_);
            fChain->SetBranchAddress("patMuonEta_", &patMuonEta_, &b_patMuonEta_);
            fChain->SetBranchAddress("patMuonPhi_", &patMuonPhi_, &b_patMuonPhi_);
            fChain->SetBranchAddress("patMuonEn_", &patMuonEn_, &b_patMuonEn_);
            fChain->SetBranchAddress("patMuonCharge_", &patMuonCharge_, &b_patMuonCharge_);
            if(fileName.Index("mcatnlo") >= 0 || fileName.Index("MG-MLM") >= 0) {
                fChain->SetBranchAddress("patMuonCombId_", &patMuonCombId_Int, &b_patMuonCombId_Int);
            }
            else {
                fChain->SetBranchAddress("patMuonCombId_", &patMuonCombId_Double, &b_patMuonCombId_Double);
            }
            fChain->SetBranchAddress("patMuonTrig_", &patMuonTrig_, &b_patMuonTrig_);
            fChain->SetBranchAddress("patMuonPfIsoDbeta_", &patMuonPfIsoDbeta_, &b_patMuonPfIsoDbeta_);
        }
    }
    if (hasGenInfo){
        fChain->SetBranchAddress("genLepPt_", &genLepPt_, &b_genLepPt_);
        fChain->SetBranchAddress("genLepEta_", &genLepEta_, &b_genLepEta_);
        fChain->SetBranchAddress("genLepPhi_", &genLepPhi_, &b_genLepPhi_);
        fChain->SetBranchAddress("genLepE_", &genLepE_, &b_genLepE_);
        fChain->SetBranchAddress("genLepQ_", &genLepQ_, &b_genLepQ_);
        fChain->SetBranchAddress("genLepId_", &genLepId_, &b_genLepId_);
        fChain->SetBranchAddress("genLepSt_", &genLepSt_, &b_genLepSt_);
        fChain->SetBranchAddress("genJetPt_", &genJetPt_, &b_genJetPt_);
        fChain->SetBranchAddress("genJetEta_", &genJetEta_, &b_genJetEta_);
        fChain->SetBranchAddress("genJetPhi_", &genJetPhi_, &b_genJetPhi_);
        fChain->SetBranchAddress("genJetE_", &genJetE_, &b_genJetE_);
        fChain->SetBranchAddress("genPhoPt_", &genPhoPt_, &b_genPhoPt_);
        fChain->SetBranchAddress("genPhoEta_", &genPhoEta_, &b_genPhoEta_);
        fChain->SetBranchAddress("genPhoPhi_", &genPhoPhi_, &b_genPhoPhi_);
        if (fileName.Index("MIX") >= 0 && fileName.Index("UNFOLDING") >= 0) {
            fChain->SetBranchAddress("pdfInfo_", &pdfInfo_, &b_pdfInfo_);
            fChain->SetBranchAddress("nup_", &nup_, &b_nup_);
        }
        if (fileName.Index("Sherpa") >= 0 && fileName.Index("UNFOLDING") >= 0) {
            fChain->SetBranchAddress("mcSherpaWeights_", &mcSherpaWeights_, &b_mcSherpaWeights_);
        }
        if (fileName.Index("mcatnlo") >= 0) {
            fChain->SetBranchAddress("mcEventWeight_", &mcEventWeight_, &b_mcEventWeight_);
        }
        if(fileName.Index("Sherpa2") >= 0){
            fChain->SetBranchAddress("mcEventWeight_", &mcEventWeight_, &b_mcEventWeight_);
        }
        //if((fileName.Index("Sherpa") >= 0 && fileName.Index("UNFOLDING") >= 0) || fileName.Index("mcatnlo") >= 0) {
        //    fChain->SetBranchAddress("weight_amcNLO_", &weight_amcNLO_, &b_weight_amcNLO_);
        //    fChain->SetBranchAddress("weight_amcNLO_sum_", &weight_amcNLO_sum_, &b_weight_amcNLO_sum_);
        //}
    }
    Notify();
    cout << "Branches are properly initialized." << endl;
}

Bool_t ZJets::Notify(){
    // The Notify() function is called when a new file is opened. This
    // can be either for a new TTree in a TChain or when when a new TTree
    // is started when using PROOF. It is normally not necessary to make changes
    // to the generated code, but the routine can be extended by the
    // user if needed. The return value is currently not used.

    return kTRUE;
}

void ZJets::Show(Long64_t entry){
    // Print contents of entry.
    // If entry is not specified, print current entry
    if (!fChain) return;
    fChain->Show(entry);
}

Int_t ZJets::Cut(Long64_t entry){
    // This function may be called from Loop.
    // returns  1 if entry is accepted.
    // returns -1 otherwise.
    printf("entry %lld", entry);
    return 1;
}
