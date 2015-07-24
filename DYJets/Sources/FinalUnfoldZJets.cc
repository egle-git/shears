#include <iostream>
#include <sstream>
#include <vector>
#include <TFile.h>
#include <TH1.h>
#include <RooUnfoldResponse.h>
#include <RooUnfold.h>
#include <RooUnfoldBayes.h>
#include <TMatrixD.h>
#include "UnfoldingSyst.h"
#include "getFilesAndHistogramsZJets.h"
#include "FinalUnfoldZJets.h"


using namespace std;

//--  Setting global variables --------------------------------------------------------------
#include "fileNamesZJets.h"
#include "variablesOfInterestZJets.h"
//-------------------------------------------------------------------------------------------

void FinalUnfold(int start, int end)
{
    TH1::SetDefaultSumw2();
    int jetPtMin(30);
    int jetEtaMax(24);

    // create the loop over the variables
    if (end == -1) end = start + 1;
    for (int i(start); i < end/*NVAROFINTERESTZJETS*/; i++) {
        for (int j(1); j < 2; j++){
            bool isMuon = j;
            if (isMuon) FuncUnfold(isMuon, VAROFINTERESTZJETS[i].name, VAROFINTERESTZJETS[i].MuBayeskterm, jetPtMin, jetEtaMax);
            else FuncUnfold(isMuon, VAROFINTERESTZJETS[i].name, VAROFINTERESTZJETS[i].EBayeskterm, jetPtMin, jetEtaMax);
        }
    }
}


void FuncUnfold(bool isMuon, TString variable, int UsedKterm, int jetPtMin, int jetEtaMax)
{
    TString energy = getEnergy();
    TString unfAlg = "Bayes";
    TString lepSel = isMuon ? "DMu" : "DE";
    TString outDir = "PNGFilesAugust/FinalUnfoldZJets/";

    TString command = "mkdir -p " + outDir;
    system(command);


    int NumberOfToys(100);

    // set efficiency errors
    double muonIDIsoHLTError = 0.025; // 2012 : id,iso = 0.005, 0.002 --> 1.4% for dimuon + HLT: 2% ??????
    double electronIDIsoHLTError = 0.005; // SF factors for 2012: 0.002
    double EffError = muonIDIsoHLTError;
    if (!isMuon) EffError = electronIDIsoHLTError;

    //-- fetch the data files and histograms -------------------------------------------------
    TFile *fData[5] = {NULL}; // 0 = central, 1 = JES Up, 2 = JES Down , 3 - SF Up , 4 - SF down
    TH1D *hData[5] = {NULL};  
    getFiles(FILESDIRECTORY, fData, lepSel, energy, Samples[DATA].name, jetPtMin, jetEtaMax);
    getHistos(hData, fData, variable);
    //----------------------------------------------------------------------------------------

    //-- fetch the DY files and histograms ----------------
    TFile *fDYMadGraph[5] = {NULL}; // 0 = central, 1 = PU Up, 2 = PU Down, 3 = JER Up, 4 = JER Down
    TFile *fDYSherpa = NULL; // only central value for Sherpa and Powheg            
    TFile *fDYPowheg = NULL; 
    getFiles(FILESDIRECTORY, fDYMadGraph, lepSel, energy, Samples[DYJETS].name, jetPtMin, jetEtaMax);
    fDYSherpa = getFile(FILESDIRECTORY, lepSel, energy, DYSHERPAFILENAME, jetPtMin, jetEtaMax);
    fDYPowheg = getFile(FILESDIRECTORY, lepSel, energy, DYPOWHEGFILENAME, jetPtMin, jetEtaMax);
    //TFile *fDYPowhegUp = NULL, *fDYPowhegDown = NULL;
    //fDYPowhegUp = getFile(FILESDIRECTORY, lepSel, energy, DYPOWHEGUPFILENAME, jetPtMin, jetEtaMax);
    //fDYPowhegDown = getFile(FILESDIRECTORY, lepSel, energy, DYPOWHEGDOWNFILENAME, jetPtMin, jetEtaMax);
    

    TH1D *hDY[5] = {NULL}; // 0 = central, 1 = PU Up, 2 = PU Down, 3 = JER Up, 4 = JER Down
    TH1D *hDYGenMadGraph = NULL, *hDYGenSherpa = NULL, *hDYGenPowheg = NULL;
    //TH1D *hDYGenPowhegUp = NULL, *hDYGenPowhegDown = NULL;
    getHistos(hDY, fDYMadGraph, variable);
    hDYGenMadGraph = getHisto(fDYMadGraph[0], "gen" + variable);
    if (fDYSherpa->IsOpen()) hDYGenSherpa = getHisto(fDYSherpa, "gen" + variable);
    else {
        hDYGenSherpa = (TH1D*) hDYGenMadGraph->Clone();
        hDYGenSherpa->SetDirectory(0);
    }
    if (fDYPowheg->IsOpen()) hDYGenPowheg = getHisto(fDYPowheg, "gen" + variable);
    else {
        hDYGenPowheg = (TH1D*) hDYGenMadGraph->Clone();
        hDYGenPowheg->SetDirectory(0);
    }
    //hDYGenPowhegUp = getHisto(fDYPowhegUp, "gen" + variable);
    //hDYGenPowhegDown = getHisto(fDYPowhegDown, "gen" + variable);
    //-----------------------------------------------------

    //--- Get MadGraph Resp Matrix ---
    RooUnfoldResponse *resDY[5] = {NULL}; 
    getResps(resDY, fDYMadGraph, variable);
    //-----------------------------------------------------

    //--- Get Sherpa Resp Matrix ---
    stringstream sheUnfFile;
    sheUnfFile << "HistoFilesAugust/" << lepSel << "_" << energy << "_DYJets_Sherpa_UNFOLDING_dR_5311_Inf_mcEveWeight_EffiCorr_0_TrigCorr_1_Syst_0_jetPtMin_" << jetPtMin << "_VarWidth.root";
    TFile *sheUnf = new TFile(sheUnfFile.str().c_str());
    RooUnfoldResponse *resDYSherpa = NULL;
    if (sheUnf->IsOpen()) {
        resDYSherpa = new RooUnfoldResponse(
                (TH1D*) sheUnf->Get(variable), 
                (TH1D*) sheUnf->Get("gen" + variable), 
                (TH2D*) sheUnf->Get("hresponse" + variable)
                ); 
    }
    else {
        cout << "Warning: sherpa unfolding file not found !" << endl;
        resDYSherpa = (RooUnfoldResponse*) resDY[0]->Clone();
    }
    //-----------------------------------------------------


    //-- fetch the BG files and histograms ----------------
    cout << " Fetch background files " << endl;
    const int NBkgGroups(7);
    const int NBkgSyst(5);  // 0 = central, 1 = PU Up, 2 = PU Down, 3 = XS Up, 4 = XS Down 

    TFile *fBG[NFILESDYJETS-2][NBkgSyst] = {{NULL}};
    TH1D *hBG[NFILESDYJETS-2][10] = {{NULL}}, *hSumBG[NBkgSyst] = {NULL}, *hSumBGgroup[15][10] = {{NULL}};
    int group = -1;
    int countBGp[2] = {-1,-1};
    for (int i(0); i < NFILESDYJETS - 2; i++){ // ends at NFILESDYJETS-2 to skip data and DY signal (have to add 1 in below line)
        TString fileNameTemp = Samples[i+1].name;

        //--- fetch the current BG files and histos ---
        getFiles(FILESDIRECTORY, fBG[i], lepSel, energy, fileNameTemp, jetPtMin, jetEtaMax);
        getHistos(hBG[i], fBG[i], variable);
        //---------------------------------------------

        if (fileNameTemp.Index("ZZ") >= 0) {
            if (countBGp[0] == -1) group++;
        }
        else if (fileNameTemp.Index("WZ") >= 0) {
            if (countBGp[1] == -1) group++;          
        }
        else group++;

        for (int j(0); j < NBkgSyst; j++){
            if (i == 0) hSumBG[j] = (TH1D*) hBG[i][j]->Clone();
            else hSumBG[j]->Add(hBG[i][j]);

            if (fileNameTemp.Index("ZZ") >= 0) {
                if (countBGp[0] == -1) { 
                    if (j == 4) countBGp[0] = group;  
                    hSumBGgroup[j][group] = (TH1D*) hBG[i][j]->Clone();
                }
                else hSumBGgroup[j][countBGp[0]]->Add(hBG[i][j]);
            }
            else if (fileNameTemp.Index("WZ") >= 0) {
                if (countBGp[1] == -1) { 
                    if (j == 4) countBGp[1] = group;   
                    hSumBGgroup[j][group] = (TH1D*) hBG[i][j]->Clone();
                }
                else hSumBGgroup[j][countBGp[1]]->Add(hBG[i][j]);
            }
            else hSumBGgroup[j][group] = (TH1D*) hBG[i][j]->Clone();
        }
    }
    //-----------------------------------------------------

    //--- create the output file and save already some basic distribution ---
    cout << "Loaded all the necessary files" << endl;
    // set the output
    TString outputRootFileName = outDir + lepSel + "_" + energy + "_unfolded_" + variable + "_histograms_" + unfAlg + ".root";
    TFile* outputRootFile = new TFile(outputRootFileName, "RECREATE");

    //-- save data reco
    outputRootFile->cd(); hData[0]->Write("Data"); hData[1]->Write("DataJECup"); hData[2]->Write("DataJECdown");
    //-- save madgraph gen also in the file
    outputRootFile->cd(); hDYGenMadGraph->Write("genMad");	
    //-- save sherpa gen also in the file
    outputRootFile->cd(); hDYGenSherpa->Write("genShe");
    //-- save powheg gen also in the file
    outputRootFile->cd(); hDYGenPowheg->Write("genPow");  
    //hDYGenPowhegUp->Write("genPowUp");  
    //hDYGenPowhegDown->Write("genPowDown");

    //-----------------------------------------------------

    //--- now unfold the different offseted distributions ---
    // 0 -- for central, everything is central: Data -> 0; DY -> 0; BG -> 0
    // 1 -- for JECup, Data is JECup, DY is central and BG is central: Data -> 1; DY -> 0; BG -> 0
    // 2 -- " JECdown,     " JECdown,               "                : Data -> 2; DY -> 0; BG -> 0
    // 3 -- for PUup, Data is central, DY and BG are PUup: Data -> 0; DY -> 1; BG -> 1
    // 4 -- " PUdown,      "         ,      "      PUdown: Data -> 0; DY -> 2; BG -> 2
    // 5 -- for XSECup, Data is central, DY is central and BG is XSECup: Data -> 0; DY -> 0; BG -> 3
    // 6 -- " XSECdown,      "         ,      "        and   " XSECdown: Data -> 0; DY -> 0; BG -> 4
    // 7 -- for JERup, Data is central, DY is JERup and BG is central: Data -> 0; DY -> 3; BG -> 0
    // 8 -- " JERdown,      "         ,   " JERdown and     "        : Data -> 0; DY -> 4; BG -> 0


    TString hNames[9] = {"Central", "JECup", "JECdown", "PUup", "PUdown", "XSECup", "XSECdown", "JERup", "JERdown"};
    int SelData[9]   = {0, 1, 2, 0, 0, 0, 0, 0, 0}; 
    int SelDY[9]     = {0, 0, 0, 1, 2, 0, 0, 3, 4}; 
    int SelBG[9]     = {0, 0, 0, 1, 2, 3, 4, 0, 0}; 

    for (int i(0); i < 9; i++) {

        if (i == 0) {
            //Unfold with Sherpa for the Central only
            TH1D* hUnfoldedWithSherpa = Unfold(unfAlg, resDYSherpa, hData[SelData[i]], hSumBG[SelBG[i]], UsedKterm, hNames[i]); 
            outputRootFile->cd(); hUnfoldedWithSherpa->Write("unfWithSherpa");
            //Unfold with MadGraph
            TH1D* hUnfoldedWithMadgraph = Unfold(unfAlg, resDY[0], hData[SelData[i]], hSumBG[SelBG[i]], UsedKterm, hNames[i]); 
            outputRootFile->cd(); hUnfoldedWithMadgraph->Write("unfWithMadgraph");
            // write the sum of the background for simple check
            outputRootFile->cd(); hSumBG[SelBG[i]]->Write("background");
        }

        TH1D* hUnfolded = Unfold(unfAlg, resDY[SelDY[i]], hData[SelData[i]], hSumBG[SelBG[i]], UsedKterm, hNames[i]); 
        outputRootFile->cd();  hUnfolded->Write();
    }
    //-----------------------------------------------------

    //--- now for central values compute Roounfold Covariance matrix
    TH2D* covCentralRoo    = CovFromRoo(unfAlg, resDY[0], hData[0], hSumBG[0], UsedKterm, "CentralCov", 1);
    TH2D* covCentralRooToy = CovFromRoo(unfAlg, resDY[0], hData[0], hSumBG[0], UsedKterm, "CentralCovToy", NumberOfToys);
    outputRootFile->cd();  covCentralRoo->Write();  covCentralRooToy->Write();
    // -- create correlation matrix from covariance
    TH2D* corCentralRoo    = CovToCorr2((TH2D*) covCentralRoo->Clone("RooCor"), "RooCor");
    TH2D* corCentralRooToy = CovToCorr2((TH2D*) covCentralRooToy->Clone("RooCorToy"), "RooCorToy");
    outputRootFile->cd();  corCentralRoo->Write();  corCentralRooToy->Write();
    //-----------------------------------------------------

    // finished with central stuff, now do covariance 
    // and my toy error propagation --> MY PRIVATE MAGIC 
    // first set errors
    //TH1D *hDataJES = SetSystErrorsMax(hData[0], hData[1], hData[2], "JESerrors");     // JES errors
    TH1D *hDataJES = SetSystErrorsMean(hData[0], hData[1], hData[2], "JESerrors");     // JES errors
    TH1D *hDataEFF = SetSystErrorsMean(hData[0], EffError, "EFFerrors");     // lepton EFF errors
    TH1D *hDataPU = SetSystErrorsMean(hData[0], hDY[0], hDY[1], hDY[2], "PUerrors");      // PU errors
    TH1D *hDataJER = SetSystErrorsMean(hData[0], hDY[0], hDY[3], hDY[4], "JERerrors");     // JER errors
    TH2D *hResPUErrors = SetResponseErrorsMax((TH2D*) resDY[0]->Hresponse(), (TH2D*) resDY[1]->Hresponse(), (TH2D*) resDY[2]->Hresponse());	// PU effect on response 
    TH2D *hResJERErrors = SetResponseErrorsMax((TH2D*) resDY[0]->Hresponse(), (TH2D*) resDY[3]->Hresponse(), (TH2D*) resDY[4]->Hresponse()); // JER effect on response 

    // my stat variation of data, BG and response object 
    RooUnfoldResponse *responseNEW[3];
    responseNEW[0] = new RooUnfoldResponse((TH1D*) resDY[0]->Hmeasured(), (TH1D*) resDY[0]->Htruth(), resDY[0]->Hresponse());
    responseNEW[1] = new RooUnfoldResponse((TH1D*) resDY[0]->Hmeasured(), (TH1D*) resDY[0]->Htruth(), hResPUErrors);
    responseNEW[2] = new RooUnfoldResponse((TH1D*) resDY[0]->Hmeasured(), (TH1D*) resDY[0]->Htruth(), hResJERErrors);

    // PU and XSEC errors on background
    TH1D* hSumBGgroupErrors[3][NBkgGroups] = {{NULL}};
    for (int i(0); i < NBkgGroups; i++){
        hSumBGgroupErrors[0][i] = (TH1D*) hSumBGgroup[0][i]->Clone();
        hSumBGgroupErrors[1][i] = SetSystErrorsMax(hSumBGgroup[0][i], hSumBGgroup[1][i], hSumBGgroup[2][i], "gBGPUerrors");
        hSumBGgroupErrors[2][i] = SetSystErrorsMax(hSumBGgroup[0][i], hSumBGgroup[3][i], hSumBGgroup[4][i], "gBGXSECerrors");
    }

    const int nUnfold = 9; 
    string Cov[nUnfold] = {"MyToy", "MyToyMC", "MyToyJES", "MyToyPU", "MyToyXSEC", "MyToyJER", "MyToyEFF","MyToyJER2","MyToyPU2"};
    int selRes[nUnfold] = {0, 0, 0, 1, 0, 2, 0, 0, 0};
    int selBGerrors[nUnfold] = {0, 0, 0, 1, 2, 0, 0, 0, 0};
    TH2D *hCovariance[nUnfold];
    TH2D *hCorrelation[nUnfold];
    TH1D *hOyMySyst[nUnfold];
    int SetTypeOfVariation[nUnfold] = {1, 101, 10, 2, 11, 4, 10, 10, 10}; // select what and how to varry

    int nbin(hData[0]->GetNbinsX());	
    for (int i(0); i < nUnfold; i++){	
        string CovName = Cov[i] + "Cov";
        string CorName = Cov[i] + "Cor";
        string hNameTemp = "h" + Cov[i];
        TH1D *hDataTemp = (TH1D*) hData[0]->Clone(); 
        if (i == 2) hDataTemp = (TH1D*) hDataJES->Clone();
        if (i == 6) hDataTemp = (TH1D*) hDataEFF->Clone();
        if (i == 7) hDataTemp = (TH1D*) hDataJER->Clone();
        if (i == 8) hDataTemp = (TH1D*) hDataPU->Clone();
        hCovariance[i] = new TH2D(CovName.c_str(), CovName.c_str(), nbin, 0.5, nbin+0.5, nbin, 0.5, nbin+0.5);
        hCorrelation[i] = new TH2D(CorName.c_str(), CorName.c_str(), nbin, 0.5, nbin+0.5, nbin, 0.5, nbin+0.5);
        RooUnfoldResponse *responseTemp = (RooUnfoldResponse*) responseNEW[selRes[i]]->Clone(); 
        TH1D* vhBack[1];
        vhBack[0] = (TH1D*) hSumBG[selBGerrors[i]]->Clone();
        // this function has to be cleaned
        TH1D *hToy2 = NULL;
        if (i == 4) hToy2 = (TH1D*) ToyMCErrorsStat(unfAlg, hDataTemp, hSumBGgroupErrors[selBGerrors[i]], responseTemp, UsedKterm, hCovariance[i], hCorrelation[i], NumberOfToys, SetTypeOfVariation[i], NBkgGroups);
        else hToy2 = (TH1D*) ToyMCErrorsStat(unfAlg, hDataTemp, vhBack, responseTemp, UsedKterm, hCovariance[i], hCorrelation[i], NumberOfToys, SetTypeOfVariation[i]);
        hOyMySyst[i] = (TH1D*) hToy2->Clone(hNameTemp.c_str());

        outputRootFile->cd();
        hOyMySyst[i]->Write();
        hCovariance[i]->Write();
        hCorrelation[i]->Write();
    }

    //-- Close all the files ------------------------------
    outputRootFile->Close();
    //closeFile(outputRootFile);
    closeFiles(fData);
    closeFiles(fDYMadGraph);
    if (fDYSherpa->IsOpen()) closeFile(fDYSherpa);
    if (fDYPowheg->IsOpen()) closeFile(fDYPowheg);
    if (sheUnf->IsOpen()) sheUnf->Close();
    //closeFile(fDYPowhegUp);
    //closeFile(fDYPowhegDown);
    for (int i(0); i < NFILESDYJETS - 2; i++) closeFiles(fBG[i]);
    //-----------------------------------------------------

}



