#include <iostream>
#include <sstream>
#include <vector>
#include <TFile.h>
#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TPaveStats.h>
#include <TLatex.h>
#include <TLegend.h>
#include "getFilesAndHistograms.h"

//--  Setting global variables --------------------------------------------------------------
#include "Includes/getFilesAndHistograms.h"
#include "Includes/fileNames.h"
#include "Includes/variablesOfInterestVarWidth.h"
#include "Includes/plottingFunctions.h"

//-------------------------------------------------------------------------------------------
//initiallize some global variables
string leptonFlavor = "DE";
bool doVarWidth = true ;
int JetPtMin = 30 ;
int JetPtMax = 30 ;
string energy = getEnergy();
//TFile* outputRootFile = new TFile("PNGFiles/MCComparison.root", "RECREATE");
TFile* outputRootFile;


// main function
void MCComparison(int sel = 4){

    gStyle->SetOptStat(0);
    TFile* files[10];
    TH1D* Histos[10];
    cout << "MCComparison  "<< sel << endl;
    int nFiles = 3 ;
    string sLegend[] = {"MG+Py6","PowMiNLO+Py6","Sherpa2NLO"}; 
    string fileName = "PNGFiles/MCComparison_MG_PowZjjMiNLO_Sherpa2NLO";

    if ( sel == 0 ){
        files[0] = getFile(FILESDIRECTORY, "DE", energy, ProcessInfo[DYMADGRAPHFILENAME].filename, JetPtMin, JetPtMax, 0, doVarWidth);
        files[1] = getFile(FILESDIRECTORY, "DMu", energy, DYPOWHEGFILENAME,   JetPtMin, JetPtMax, 0, doVarWidth);
        //files[2] = getFile(FILESDIRECTORY, "DE", energy, DYSHERPAFILENAME  , JetPtMin, JetPtMax, 0, doVarWidth);
        files[2] = getFile(FILESDIRECTORY, "DE", energy, DYSHERPA2NLOFILENAME  , JetPtMin, JetPtMax, 0, doVarWidth);
        outputRootFile = new TFile("PNGFiles/MCComparison_MG_PowZjjMiNLO_Sherpa2NLO.root", "RECREATE");
    }
    else if ( sel == 1 ){
        sLegend[0] = "Sherpa1NLO"; sLegend[1] = "Sherpa1NLO ScaleDown";sLegend[2] = "Sherpa1NLO ScaleUp"; 
        fileName = "PNGFiles/MCComparison_Sherpa1NLO_withScales";	
        string fileOut = fileName +".root" ;
        files[0] = getFile(FILESDIRECTORY, "DE", energy, DYSHERPAFILENAME  , JetPtMin, JetPtMax, 0, doVarWidth);
        files[1] = getFile(FILESDIRECTORY, "DE", energy, "DY_Sherpa_1NLO4_scaleDown_HepMC_dR_Full_List", JetPtMin, JetPtMax, 0, doVarWidth);
        files[2] = getFile(FILESDIRECTORY, "DE", energy, "DY_Sherpa_1NLO4_scaleUp_HepMC_dR_Full_List", JetPtMin, JetPtMax, 0, doVarWidth);

        //outputRootFile = new TFile("PNGFiles/MCComparison_MG_PowMiNLO_Sherpa1NLO.root", "RECREATE");
        //outputRootFile = new TFile(fileName.c_str()+".root", "RECREATE");
        outputRootFile = new TFile(fileOut.c_str(), "RECREATE");
    }
    else if ( sel == 2 ){
        sLegend[0] = "MG"; sLegend[1] = "Sherpa1NLO";sLegend[2] = "Sherpa2NLO"; 
        fileName = "PNGFiles/MCComparison_MG_Sherpa1NLO_Sherpa2NLO";
        files[0] = getFile(FILESDIRECTORY, "DE", energy, ProcessInfo[DYMADGRAPHFILENAME].filename, JetPtMin, JetPtMax, 0, doVarWidth);
        files[1] = getFile(FILESDIRECTORY, "DE", energy, DYSHERPAFILENAME  , JetPtMin, JetPtMax, 0, doVarWidth);
        files[2] = getFile(FILESDIRECTORY, "DE", energy, DYSHERPA2NLOFILENAME,   JetPtMin, JetPtMax, 0, doVarWidth);
        outputRootFile = new TFile("PNGFiles/MCComparison_MG_Sherpa1NLO_Sherpa2NLO.root", "RECREATE");
    }
    else if ( sel == 3 ){
        sLegend[0] = "MG"; sLegend[1] = "SherpaUnf";sLegend[2] = "Sherpa2NLO";
        fileName = "PNGFiles/MCComparison_MG_SherpaUnf_Sherpa2NLO";
        files[0] = getFile(FILESDIRECTORY, "DE", energy, ProcessInfo[DYMADGRAPHFILENAME].filename, JetPtMin, JetPtMax, 0, doVarWidth);
        files[1] = getFile(FILESDIRECTORY, "DE", energy, "DYJets_Sherpa_UNFOLDING_dR_5311_Inf"  , JetPtMin, JetPtMax, 0, doVarWidth);
        files[2] = getFile(FILESDIRECTORY, "DE", energy, DYSHERPA2NLOFILENAME,   JetPtMin, JetPtMax, 0, doVarWidth);
        outputRootFile = new TFile("PNGFiles/MCComparison_MG_SherpaUnf_Sherpa2NLO.root", "RECREATE");
    }
    else if ( sel == 4 ){
        sLegend[0] = "Sherpa"; sLegend[1] = "Sherpa2";sLegend[2] = "Sherpa2NLO";
        fileName = "PNGFiles/MCComparison_Sherpa2NLO";
        files[0] = getFile(FILESDIRECTORY, "DE", energy, "DY_Sherpa_2NLO4_HepMC_dR_Full_List", JetPtMin, JetPtMax, 0, doVarWidth);
        files[1] = getFile(FILESDIRECTORY, "DE", energy, "DY_Sherpa_2NLO4_HepMC_dR_Full_List2"  , JetPtMin, JetPtMax,  0, doVarWidth);
        files[2] = getFile(FILESDIRECTORY, "DE", energy, "DY_Sherpa_2NLO4_HepMC_dR_Full_ListALL",   JetPtMin, JetPtMax, 0, doVarWidth);
        outputRootFile = new TFile("PNGFiles/MCComparison_Sherpa2NLO.root", "RECREATE");
    }
    else if ( sel == 5 ){
        sLegend[0] = "MG+Py6"; sLegend[1] = "Pow1NLO";sLegend[2] = "PowMiNLO+Py6";
        fileName = "PNGFiles/MCComparison_Powhegs";
        files[0] = getFile(FILESDIRECTORY, "DE", energy, ProcessInfo[DYMADGRAPHFILENAME].filename, JetPtMin, JetPtMax, 0,   doVarWidth);
        files[1] = getFile(FILESDIRECTORY, "DMu", energy, DYPOWHEGFILENAME,   JetPtMin, JetPtMax, 0, doVarWidth);
        files[2] = getFile(FILESDIRECTORY, "DE", energy, DYSHERPAFILENAME  , JetPtMin, JetPtMax, 0, doVarWidth);
        outputRootFile = new TFile("PNGFiles/MCComparison_MG_Powhegs.root", "RECREATE");
    }
    else if ( sel == 6 ){
        sLegend[0] = "MG+Py6"; sLegend[1] = "scaleUp";sLegend[2] = "scaleDown";
        fileName = "PNGFiles/MCComparison_Madgraphs";
        files[0] = getFile(FILESDIRECTORY, "DMu", energy, ProcessInfo[DYMADGRAPHFILENAME].filename, JetPtMin, JetPtMax, 0,   doVarWidth);
        files[1] = getFile(FILESDIRECTORY, "DMuDE", energy, "DYJets_UNFOLDING_dR_5311_Inf3_scaleUp",   JetPtMin, JetPtMax, 0, doVarWidth);
        files[2] = getFile(FILESDIRECTORY, "DMuDE", energy, "DYJets_UNFOLDING_dR_5311_Inf3_scaleDown"  , JetPtMin, JetPtMax, 0, doVarWidth);
        outputRootFile = new TFile("PNGFiles/MCComparison_MG_Madgraphs.root", "RECREATE");
    }

    // get number of histograms
    int nHist = files[0]->GetListOfKeys()->GetEntries();
    for (int i = 0; i < nHist; i++) {
        string histoNameTemp = files[0]->GetListOfKeys()->At(i)->GetName();
        if (histoNameTemp.find("gen") == string::npos) continue ;
        //        if (histoNameTemp.find("genZNG") == string::npos) continue ;
        //i = nHist ;
        cout <<  histoNameTemp << endl;
        for (int j = 0; j < nFiles; j++) {
            TH1D* hTemp  = (TH1D*)  files[j]->Get(histoNameTemp.c_str());
            Histos[j] = (TH1D*) hTemp->Clone();
            //	outputRootFile->cd();
            //	Histos[j]->Write();

        }
        // now call plotter
        cout << " used legend:"<< sLegend[0] << endl;
        TCanvas *a = (TCanvas *) plottingFunctions(nFiles,Histos, 1, 1, histoNameTemp,sLegend, 0.6, 1.8);
        //plottingFunctions(4,Histos, true, true  );
        //plottingFunctions(4,Histos  );
        //plottingFunctions(4 );
        outputRootFile->cd();
        a->Write();
        string PNGout = fileName+"/"+histoNameTemp+".pdf";
        a->Print(PNGout.c_str());
        cout << " Will be saved in :" << PNGout << endl;

    }



    for (int j = 0; j < nFiles; j++) {
        files[j]->Close();
    }
    outputRootFile->Close();



}
