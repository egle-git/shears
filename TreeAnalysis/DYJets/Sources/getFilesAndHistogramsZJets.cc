#include <iostream>
#include <sstream>
#include <RooUnfoldResponse.h>
#include <TFile.h>
#include <TString.h>
#include <TSystem.h>
#include <algorithm>
#include "getFilesAndHistogramsZJets.h"
using namespace std;

//------------------------------------------------------------
// getEnergy() returns a TString, either "7TeV" or "8TeV"
// according to the name of the directory from which the 
// code is being executed.
//------------------------------------------------------------
TString getEnergy()
{
    TString energy = "";
    TString fileBeingProcessed = __FILE__;
    if (fileBeingProcessed.Index("2012") >= 0) {
        energy = "8TeV";
    }
    else if (fileBeingProcessed.Index("2011") >= 0) {
        energy = "7TeV";
    }
    else 
    {
        std::cout << "WARNING ! Impossible to retrieve te energy from the current location !" << std::endl;
        energy = "Unknown";
    }

    return energy;
}
//------------------------------------------------------------

TFile* getFile(TString histoDir, TString lepSel, TString energy, TString Name, int jetPtMin, int jetEtaMax, TString closureTest, TString syst)
{

    TString fileName = histoDir; // TString to contain the name of the file
    if (!fileName.EndsWith("/")) fileName += "/";

    //--- make sure lepSel is short version ---
    if (lepSel == "Muons" || lepSel == "DMu_") lepSel = "DMu";
    else if (lepSel == "Electrons" || lepSel == "DE_") lepSel = "DE";

    fileName += lepSel + "_" + energy + "_" + Name; // update fileName with lepton information
    //-----------------------------------------------


    //--- deal with efficiency correction applied or not ---
    TString trigCorr = "0";
    if (Name.Index("Data") == 0 || energy == "8TeV") trigCorr = "1"; // trigger correction is applied to data and MC at 8TeV but only to data at 7TeV 

    //--- special case for the generator comparison ---
    if (Name.Index("Powheg") >= 0 || Name.Index("Sherpa") >= 0) trigCorr = "0"; 
    //-------------------------------------------------

    //--- update fileName with trigger correction ----------
    fileName += "_TrigCorr_" + trigCorr;
    //------------------------------------------------------

    //--- update fileName for a bunch of other things ---
    fileName += "_Syst_" + syst;
    fileName += "_JetPtMin_";
    fileName += jetPtMin;
    fileName += "_JetEtaMax_";
    fileName += jetEtaMax; 
    if (closureTest != "") fileName += closureTest;
    //---------------------------------------------------

    //--- fileName is complete: just add the extension and open it ---
    fileName += ".root";
    TFile *File = new TFile(fileName, "READ");
    std::cout << "Opening: " << fileName << "   --->   Opened ? " << File->IsOpen() << std::endl;
    if (!File->IsOpen()) {
        std::cerr << "Please check that you produced the following file. I was not able to open it." << std::endl;
        std::cerr << "\t\033[031m " << fileName << "\033[0m " << std::endl;
        return NULL;
    }
    else return File;
    //----------------------------------------------------------------
}

void getFiles(TString histoDir, TFile *Files[], TString lepSel, TString energy, TString Name, int jetPtMin, int jetEtaMax)
{

    //--- make sure lepSel is short version ---
    if (lepSel == "Muons" || lepSel == "DMu_") lepSel = "DMu";
    else if (lepSel == "Electrons" || lepSel == "DE_") lepSel = "DE";
    //-----------------------------------------------

    vector<TString> Syst;
    if (Name.Index("Data") >= 0) { // for data we have:
        Syst.push_back("0");                 //   0: central
        Syst.push_back("2_Up");              //   2 up: JES up
        Syst.push_back("2_Down");            //   2 down: JES down
    }
    else if (Name.Index("UNFOLDING") >= 0 && Name.Index("DYJets") >= 0 && Name.Index("Tau") < 0) {
        // for DYJets in case of Z+Jets or for WJets in case of W+Jets analysis we have:
        Syst.push_back("0");         // 0: central
        Syst.push_back("1_Up");      // 1 up: PU up
        Syst.push_back("1_Down");    // 1 down: PU down
        Syst.push_back("4_Up");      // 4 up: JER up
        Syst.push_back("4_Down");    // 4 down: JER down
        Syst.push_back("5_Up");      // 5 up: LES up
        Syst.push_back("5_Down");    // 5 down: LES down
        Syst.push_back("6_Up");      // 6 up: LER up
        Syst.push_back("6_Down");    // 6 down: LER down
    }
    else { // for background we have
        Syst.push_back("0");         // 0: central
        Syst.push_back("1_Up");      // 1 up: PU up
        Syst.push_back("1_Down");    // 1 down: PU down
        Syst.push_back("3_Up");      // 3 up: XSec up
        Syst.push_back("3_Down");    // 3 down: Xsec down
        Syst.push_back("5_Up");      // 5 up: LES up
        Syst.push_back("5_Down");    // 5 down: LES down
    };

    //--- determnie how many files we have and open them all ---
    int nSyst(Syst.size());
    for (int i(0); i < nSyst; i++) {
        Files[i] = getFile(histoDir, lepSel, energy, Name, jetPtMin, jetEtaMax, "", Syst[i]);
    }
    //----------------------------------------------------------
}

void getAllFiles(TString histoDir, TString lepSel, TString energy, int jetPtMin, int jetEtaMax, TFile *fData[3], TFile *fDYJets[9], TFile *fBg[][7], int nBg)
{
    //--- Open data files ---------------------------------------------------------------------- 
    getFiles(histoDir, fData, lepSel, energy, Samples[DATA].name, jetPtMin, jetEtaMax); 
    //------------------------------------------------------------------------------------------ 

    //--- Open DYJets files --------------------------------------------------------------------
    getFiles(histoDir, fDYJets, lepSel, energy, Samples[DYJETS].name, jetPtMin, jetEtaMax); 
    //------------------------------------------------------------------------------------------ 

    //--- Open Bg files ------------------------------------------------------------------------
    for (unsigned short iBg = 0; iBg < nBg; ++iBg) {
        getFiles(histoDir, fBg[iBg], lepSel, energy, Samples[iBg+1].name, jetPtMin, jetEtaMax);
    }
    //------------------------------------------------------------------------------------------ 
}

void getAllHistos(TString variable, TH1D *hRecData[3], TFile *fData[3], TH1D *hRecDYJets[13], TH1D *hGenDYJets[11], TH2D *hResDYJets[13], TFile *fDYJets[9], TH1D *hRecBg[][11], TH1D *hRecSumBg[11], TFile *fBg[][7], int nBg, RooUnfoldResponse *respDYJets[], TH1D *hFakDYJets[18])
{

    //--- get rec Data histograms ---
    getHistos(hRecData, fData, variable);

    //--- get rec DYJets histograms ---
    getHistos(hRecDYJets, fDYJets, variable);

    //--- get gen DYJets histograms ---
    getHistos(hGenDYJets, fDYJets, "gen" + variable);

    //--- get res DYJets histograms ---
    getHistos(hResDYJets, fDYJets, "hresponse" + variable);

    //--- get rec Bg histograms ---
    for (unsigned short iBg = 0; iBg < nBg; ++iBg) {
        getHistos(hRecBg[iBg], fBg[iBg], variable);
        for (unsigned short iSyst = 0; iSyst < 11; ++iSyst) { 
            if (iBg == 0) hRecSumBg[iSyst] = (TH1D*) hRecBg[0][iSyst]->Clone();
            else hRecSumBg[iSyst]->Add(hRecBg[iBg][iSyst]);
        }
    }

    //--- get response DYJets objects ---
    getResps(respDYJets, hRecDYJets, hGenDYJets, hResDYJets);

    //--- get fakes DYJets ---
    getFakes(hFakDYJets, hRecData, hRecSumBg, hRecDYJets, hResDYJets);
}

//------------------------------------------------------------
// Close the file if open and delete the pointer
//------------------------------------------------------------
void closeFile(TFile *File)
{
    if (File) {
        if (File->IsOpen()) File->Close();
        cout << "Closing: " << File->GetName() << "   --->   Closed ? " << (!(File->IsOpen())) << endl;
    }
}

void closeFiles(TFile *Files[])
{
    if (Files[0]) {
        TString fileName = Files[0]->GetName();
        int nFiles;
        if (fileName.Index("Data") >= 0) {
            nFiles = 3; 
        }
        else if (fileName.Index("DYJets") >= 0 && fileName.Index("UNFOLDING") >=0 && fileName.Index("Tau") < 0){
            nFiles = 9;
        }
        else nFiles = 7; 

        for (int i(0); i < nFiles; i++){
            Files[i]->cd();
            closeFile(Files[i]);
        }
    }
}

void closeFiles(TFile *Files[], int nFiles)
{
    TString fileName = Files[0]->GetName();
    for (int i(0); i < nFiles; i++){
        Files[i]->cd();
        closeFile(Files[i]);
        cout << "Closing file: " << Files[i]->GetName() << "   --->   Closed ? " << (!(Files[i]->IsOpen())) << endl;
    }
}

void closeAllFiles(TFile *fData[3], TFile *fDYJets[9], TFile *fBg[][7], int nBg)
{
    //--- Close data files ---------------------------------------------------------------------
    closeFiles(fData);
    //------------------------------------------------------------------------------------------ 

    //--- Close DYJets files -------------------------------------------------------------------
    closeFiles(fDYJets);
    //------------------------------------------------------------------------------------------ 

    //--- Close Bg files -----------------------------------------------------------------------
    for (unsigned short iBg = 0; iBg < nBg; ++iBg) {
        closeFiles(fBg[iBg]);
    }
    //------------------------------------------------------------------------------------------ 
}

TH1D* getHisto(TFile *File, const TString variable)
{
    TH1D *histo = (TH1D*) File->Get(variable);
    histo->SetDirectory(0);
    return histo;
}

void getHistos(TH1D *histograms[], TFile *Files[], TString variable)
{
    TString fileName = Files[0]->GetName();
    bool isData = (fileName.Index("Data") >= 0);
    bool isSignal = (fileName.Index("DYJets") >= 0 && fileName.Index("UNFOLDING") >=0 && fileName.Index("Tau") < 0);
    int nFiles = 0;

    if (fileName.Index("Data") >= 0) {
        nFiles = 3; 
    }
    else if (fileName.Index("DYJets") >= 0 && fileName.Index("UNFOLDING") >=0 && fileName.Index("Tau") < 0){
        nFiles = 9;
    }
    else nFiles = 7; 

    for (int i(0); i < nFiles; i++){
        Files[i]->cd();
        histograms[i] = (TH1D*) Files[i]->Get(variable);
    } 

    if (!isData) {
        Files[0]->cd();
        //--- From central histograms, we simulate the histograms
        //    for lumi up and down systematics. It is just a rescaliing
        //    since it is a global effect. The error is estimated to
        //    2.6%

        double lumiErr = 0.026;
        if (isSignal) {
            //--- lumi scale up ---
            histograms[9] = (TH1D*) histograms[0]->Clone();
            histograms[9]->Scale(1. + lumiErr);

            //--- lumi scale down ---
            histograms[10] = (TH1D*) histograms[0]->Clone();
            histograms[10]->Scale(1. - lumiErr);
        }

        else{
            //--- lumi scale up ---
            histograms[7] = (TH1D*) histograms[0]->Clone();
            histograms[7]->Scale(1. + lumiErr);

            //--- lumi scale down ---
            histograms[8] = (TH1D*) histograms[0]->Clone();
            histograms[8]->Scale(1. - lumiErr);
        }

        //--- From central histograms, we simulate the histograms
        //    for scale factors up and down systematics. It is just 
        //    a rescaliinga since the errors are global. The error 
        //    is different for the two channels and are estimated to
        //    2.5% for muons and 0.5% for electron.
        //    This should not be applied to gen histograms however.
        //    That is why errSF = 0 when variable contains "gen"
        TString lepSel = (fileName.Index("DMu") >= 0) ? "DMu" : "DE";
        //double errSF = (lepSel == "DMu") ? 0.025 : 0.005;
        double errSF = (lepSel == "DMu") ? 0.025 : 0.005;
        if (variable.Index("gen") < 0) {
            if (isSignal) {
                //--- SF up ---
                histograms[11] = (TH1D*) histograms[0]->Clone();
                histograms[11]->Scale(1. + errSF);

                //--- SF down ---
                histograms[12] = (TH1D*) histograms[0]->Clone();
                histograms[12]->Scale(1. - errSF);
            }

            else {
                //--- SF up ---
                histograms[9] = (TH1D*) histograms[0]->Clone();
                histograms[9]->Scale(1. + errSF);

                //--- SF down ---
                histograms[10] = (TH1D*) histograms[0]->Clone();
                histograms[10]->Scale(1. - errSF);
            }
        }
    }
}


void getHistos(TH2D *histograms[], TFile *Files[], TString variable)
{
    TString fileName = Files[0]->GetName();
    bool isData = (fileName.Index("Data") >= 0);
    bool isSignal = (fileName.Index("DYJets") >= 0 && fileName.Index("UNFOLDING") >=0 && fileName.Index("Tau") < 0);
    int nFiles = 0;

    if (fileName.Index("Data") >= 0) {
        nFiles = 3; 
    }
    else if (fileName.Index("DYJets") >= 0 && fileName.Index("UNFOLDING") >=0 && fileName.Index("Tau") < 0){
        nFiles = 9;
    }
    else nFiles = 7; 

    for (unsigned short i = 0; i < nFiles; i++){
        Files[i]->cd();
        histograms[i] = (TH2D*) Files[i]->Get(variable);
    } 

    if (!isData) {
        //--- From central histograms, we simulate the histograms
        //    for lumi up and down systematics. It is just a rescaliing
        //    since it is a global effect. The error is estimated to
        //    2.6%

        double lumiErr = 0.026;
        if (isSignal) {
            //--- lumi scale up ---
            histograms[9] = (TH2D*) histograms[0]->Clone();
            histograms[9]->Scale(1. + lumiErr);

            //--- lumi scale down ---
            histograms[10] = (TH2D*) histograms[0]->Clone();
            histograms[10]->Scale(1. - lumiErr);
        }

        else{
            //--- lumi scale up ---
            histograms[7] = (TH2D*) histograms[0]->Clone();
            histograms[7]->Scale(1. + lumiErr);

            //--- lumi scale down ---
            histograms[8] = (TH2D*) histograms[0]->Clone();
            histograms[8]->Scale(1. - lumiErr);
        }

        //--- From central histograms, we simulate the histograms
        //    for scale factors up and down systematics. It is just 
        //    a rescaliinga since the errors are global. The error 
        //    is different for the two channels and are estimated to
        //    2.5% for muons and 0.5% for electron.
        //    This should not be applied to gen histograms however.
        TString lepSel = (fileName.Index("DMu") >= 0) ? "DMu" : "DE";
        //double errSF = (lepSel == "DMu") ? 0.025 : 0.005;
        double errSF = (lepSel == "DMu") ? 0.005 : 0.005;
        if (variable.Index("gen") < 0) {
            if (isSignal) {
                //--- SF up ---
                histograms[11] = (TH2D*) histograms[0]->Clone();
                histograms[11]->Scale(1. + errSF);

                //--- SF down ---
                histograms[12] = (TH2D*) histograms[0]->Clone();
                histograms[12]->Scale(1. - errSF);
            }

            else{
                //--- SF up ---
                histograms[9] = (TH2D*) histograms[0]->Clone();
                histograms[9]->Scale(1. + errSF);

                //--- SF down ---
                histograms[10] = (TH2D*) histograms[0]->Clone();
                histograms[10]->Scale(1. - errSF);
            }
        }
    }
}


void getResp(RooUnfoldResponse *response, TFile *File, TString variable)
{
    TH1D *hRec = (TH1D*) File->Get(variable)->Clone();
    hRec->Reset();

    response = new RooUnfoldResponse(
            hRec, 
            (TH1D*) File->Get("gen" + variable), 
            (TH2D*) File->Get("hresponse" + variable)
            ); 
    response->UseOverflow();
}

RooUnfoldResponse* getResp(TFile *File, TString variable)
{
    TH1D *hRec = (TH1D*) File->Get(variable)->Clone();
    hRec->Reset();

    RooUnfoldResponse *response = new RooUnfoldResponse(
            hRec, 
            (TH1D*) File->Get("gen" + variable), 
            (TH2D*) File->Get("hresponse" + variable)
            ); 
    response->UseOverflow();
    return response;
}

void getResps(RooUnfoldResponse *responses[], TFile *Files[], TString variable)
{
    TString fileName = Files[0]->GetName();
    int nFiles;
    if (fileName.Index("Data") >= 0) nFiles = 3;
    else if (fileName.Index("DYJets") >= 0 && fileName.Index("UNFOLDING") >=0 && fileName.Index("Tau") < 0) nFiles = 9;
    else nFiles = 7;

    for (int i(0); i < nFiles; i++){
        Files[i]->cd();
        TH1D *hRec = (TH1D*) Files[i]->Get(variable)->Clone();
        hRec->Reset();
        responses[i] = new RooUnfoldResponse(
                hRec, 
                (TH1D*) Files[i]->Get("gen" + variable), 
                (TH2D*) Files[i]->Get("hresponse" + variable)
                ); 
        responses[i]->UseOverflow();
    } 
}


TH1D* getFakes(TH1D *hRecDYJets, TH1D *hRecData, TH1D *hRecSumBg, TH2D *hResDYJets)
{
    TH1D *hFakDYJets = (TH1D*) hRecDYJets->Clone();

    int sm= hRecDYJets->GetSumw2N();
    int s = hResDYJets->GetSumw2N();
    int nm = hResDYJets->GetNbinsX() + 2;
    int nt = hResDYJets->GetNbinsY() + 2;

    double dyIntegral = hRecDYJets->Integral(0, hRecDYJets->GetNbinsX()+1);
    double dataIntegral = hRecData->Integral(0, hRecData->GetNbinsX()+1);
    double bgIntegral = hRecSumBg->Integral(0, hRecSumBg->GetNbinsX()+1);
    for (int i= 0; i<nm; i++) {
        double nmes= 0.0, wmes= 0.0;
        for (int j= 0; j<nt; j++) {
            nmes += hResDYJets->GetBinContent(i, j);
            if (s) wmes += pow(hResDYJets->GetBinError(i, j), 2);
        }
        double fake = hRecDYJets->GetBinContent(i) - nmes;
        double factor = dyIntegral;
        if (factor != 0) factor = (dataIntegral - bgIntegral) / factor;
        if (!s) wmes= nmes;
        hFakDYJets->SetBinContent (i, factor*fake);
        hFakDYJets->SetBinError   (i, sqrt (wmes + (sm ? pow(hRecDYJets->GetBinError(i),2) : hRecDYJets->GetBinContent(i))));
    }
    hFakDYJets->SetEntries (hFakDYJets->GetEffectiveEntries());  // 0 entries if 0 fakes

    return hFakDYJets;

}

void getFakes(TH1D *hFakDYJets[18], TH1D *hRecData[3], TH1D *hRecSumBg[11], TH1D *hRecDYJets[13], TH2D *hResDYJets[13])
{

    hFakDYJets[0] = getFakes(hRecDYJets[0], hRecData[0], hRecSumBg[0], hResDYJets[0]);
    hFakDYJets[1] = getFakes(hRecDYJets[0], hRecData[1], hRecSumBg[0], hResDYJets[0]);
    hFakDYJets[2] = getFakes(hRecDYJets[0], hRecData[2], hRecSumBg[0], hResDYJets[0]);
    hFakDYJets[3] = getFakes(hRecDYJets[1], hRecData[0], hRecSumBg[1], hResDYJets[1]);
    hFakDYJets[4] = getFakes(hRecDYJets[2], hRecData[0], hRecSumBg[2], hResDYJets[2]);
    hFakDYJets[5] = getFakes(hRecDYJets[3], hRecData[0], hRecSumBg[0], hResDYJets[3]);
    hFakDYJets[6] = getFakes(hRecDYJets[4], hRecData[0], hRecSumBg[0], hResDYJets[4]);
    hFakDYJets[7] = getFakes(hRecDYJets[0], hRecData[0], hRecSumBg[3], hResDYJets[0]);
    hFakDYJets[8] = getFakes(hRecDYJets[0], hRecData[0], hRecSumBg[4], hResDYJets[0]);
    hFakDYJets[9] = getFakes(hRecDYJets[5], hRecData[0], hRecSumBg[5], hResDYJets[5]);
    hFakDYJets[10] = getFakes(hRecDYJets[6], hRecData[0], hRecSumBg[6], hResDYJets[6]);
    hFakDYJets[11] = getFakes(hRecDYJets[7], hRecData[0], hRecSumBg[0], hResDYJets[7]);
    hFakDYJets[12] = getFakes(hRecDYJets[8], hRecData[0], hRecSumBg[0], hResDYJets[8]);
    hFakDYJets[13] = getFakes(hRecDYJets[9], hRecData[0], hRecSumBg[7], hResDYJets[9]);
    hFakDYJets[14] = getFakes(hRecDYJets[10], hRecData[0], hRecSumBg[8], hResDYJets[10]);
    hFakDYJets[15] = getFakes(hRecDYJets[11], hRecData[0], hRecSumBg[9], hResDYJets[11]);
    hFakDYJets[16] = getFakes(hRecDYJets[12], hRecData[0], hRecSumBg[10], hResDYJets[12]);
    hFakDYJets[17] = getFakes(hRecDYJets[0], hRecData[0], hRecSumBg[0], hResDYJets[0]);
}

void getResps(RooUnfoldResponse *responses[], TH1D *hRecDYJets[13], TH1D *hGenDYJets[11], TH2D *hResDYJets[13])
{
    TH1D *hRec = (TH1D*) hRecDYJets[0]->Clone();
    hRec->Reset();
    //--- build response object for central ---
    responses[0] = new RooUnfoldResponse(hRec, hGenDYJets[0], hResDYJets[0]); 
    responses[0]->UseOverflow();

    //--- build response object for JES up (same as central because JES is done on data) ---
    responses[1] = new RooUnfoldResponse(hRec, hGenDYJets[0], hResDYJets[0]); 
    responses[1]->UseOverflow();

    //--- build response object for JES down (same as central because JES is done on data) ---
    responses[2] = new RooUnfoldResponse(hRec, hGenDYJets[0], hResDYJets[0]); 
    responses[2]->UseOverflow();

    //--- build response object for PU up ---
    responses[3] = new RooUnfoldResponse(hRec, hGenDYJets[1], hResDYJets[1]); 
    responses[3]->UseOverflow();

    //--- build response object for PU down ---
    responses[4] = new RooUnfoldResponse(hRec, hGenDYJets[2], hResDYJets[2]); 
    responses[4]->UseOverflow();

    //--- build response object for JER up ---
    responses[5] = new RooUnfoldResponse(hRec, hGenDYJets[3], hResDYJets[3]); 
    responses[5]->UseOverflow();

    //--- build response object for JER down ---
    responses[6] = new RooUnfoldResponse(hRec, hGenDYJets[4], hResDYJets[4]); 
    responses[6]->UseOverflow();

    //--- build response object for XSec up ---
    responses[7] = new RooUnfoldResponse(hRec, hGenDYJets[0], hResDYJets[0]); 
    responses[7]->UseOverflow();

    //--- build response object for XSec down ---
    responses[8] = new RooUnfoldResponse(hRec, hGenDYJets[0], hResDYJets[0]); 
    responses[8]->UseOverflow();

    //--- build response object for LES up ---
    responses[9] = new RooUnfoldResponse(hRec, hGenDYJets[5], hResDYJets[5]); 
    responses[9]->UseOverflow();

    //--- build response object for LES down ---
    responses[10] = new RooUnfoldResponse(hRec, hGenDYJets[6], hResDYJets[6]); 
    responses[10]->UseOverflow();

    //--- build response object for LER up ---
    responses[11] = new RooUnfoldResponse(hRec, hGenDYJets[7], hResDYJets[7]); 
    responses[11]->UseOverflow();

    //--- build response object for LER down ---
    responses[12] = new RooUnfoldResponse(hRec, hGenDYJets[8], hResDYJets[8]); 
    responses[12]->UseOverflow();

    //--- build response object for Lumi up ---
    responses[13] = new RooUnfoldResponse(hRec, hGenDYJets[9], hResDYJets[9]); 
    responses[13]->UseOverflow();

    //--- build response object for Lumi down ---
    responses[14] = new RooUnfoldResponse(hRec, hGenDYJets[10], hResDYJets[10]); 
    responses[14]->UseOverflow();

    //--- build response object for SF up ---
    responses[15] = new RooUnfoldResponse(hRec, hGenDYJets[0], hResDYJets[11]); 
    responses[15]->UseOverflow();

    //--- build response object for SF down ---
    responses[16] = new RooUnfoldResponse(hRec, hGenDYJets[0], hResDYJets[12]); 
    responses[16]->UseOverflow();

}


void getStatistics(TString lepSel, int jetPtMin, int jetEtaMax)
{
    TString variable = "ZNGoodJets_Zexc";
    TString energy = getEnergy();

    //--- make sure lepSel is short version ---
    if (lepSel == "Muons" || lepSel == "DMu_") lepSel = "DMu";
    else if (lepSel == "Electrons" || lepSel == "DE_") lepSel = "DE";
    //-----------------------------------------------

    // jet counter
    int NBins = 8;
    double DataEv[20][20] = {{0}};

    //-- fetch the data files and histograms --------------
    int usedFiles = NFILESDYJETS; 

    for (int i(0); i < usedFiles; i++) {
        TFile *fData;
        int sel = FilesDYJets[i];

        fData = getFile(FILESDIRECTORY,  lepSel, energy, Samples[sel].name, jetPtMin, jetEtaMax);
        TH1D *hTemp = getHisto(fData, variable);

        for (int j = 1 ; j < NBins + 1 ; j++ ){
            Double_t binContent = hTemp->GetBinContent(j);
            DataEv[i][j] = binContent;
            if ( i > 0 ) DataEv[usedFiles][j]+=int(binContent);
        }
        // close all input root files
        fData->Close();
    }

    cout << "Closed all files" << endl;

    ostringstream nameStr;
    nameStr << "outputTable_" << lepSel << "_JetPtMin_" << jetPtMin << "_JetEtaMax_" << jetEtaMax;
    nameStr << ".tex";

    FILE *outFile = fopen(nameStr.str().c_str(),"w");
    fprintf( outFile, "\\footnotesize{\n\\begin{tabular}{l|cccccccc} \n ");
    fprintf( outFile, " &  $N_{\\text{jets}} = 0 $ & $N_{\\text{jets}} = 1 $ & $N_{\\text{jets}} = 2 $ & $N_{\\text{jets}} = 3 $ & $N_{\\text{jets}} = 4 $ & $N_{\\text{jets}} = 5 $ & $N_{\\text{jets}} = 6 $ & $N_{\\text{jets}} = 7$ \\\\ \\hline \n ");

    //// print statistics of all the MC samples
    for (int i=1; i< usedFiles + 1 ; i++){
        int sel = FilesDYJets[i];

        if (i < usedFiles) fprintf(outFile, " %s        & ", Samples[sel].legendAN.Data());
        else {
            fprintf( outFile, "\\hline \n");
            fprintf( outFile, " TOTAL & ");
        }
        for (int j = 1 ; j < NBins + 1  ; j++ ) {
            if (j < NBins ) fprintf( outFile, "%d & ", int(DataEv[i][j]));
            else fprintf( outFile, "%d \\\\ \n ", int(DataEv[i][j]));

        }
        std::cout << std::endl;
    }

    // print data statistics
    fprintf( outFile, "\\hline \n");
    fprintf( outFile, " Data          & ");
    for (int j = 1; j< NBins + 1 ; j++){
        if (j < NBins ) fprintf( outFile, "%d & ",  int(DataEv[0][j]));
        else fprintf( outFile, "%d \\\\ \n ",  int(DataEv[0][j]));
    }
    // print ratio of MC/data
    fprintf( outFile, " Ratio          & ");
    for (int j=1; j<NBins + 1; j++){
        double temp= DataEv[usedFiles][j]/DataEv[0][j];
        std:: cout << DataEv[usedFiles][j] << "   " << DataEv[0][j] << std::endl;
        if (j<NBins) fprintf( outFile, "%f & ", float(temp));
        else fprintf( outFile, "%f \\\\ \n ",temp);

    }
    fprintf( outFile, "\\end{tabular}}");
    fclose(outFile);
}



