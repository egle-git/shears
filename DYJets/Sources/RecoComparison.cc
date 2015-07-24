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
#include "getFilesAndHistogramsZJets.h"

//--  Setting global variables --------------------------------------------------------------
#include "fileNamesZJets.h"
//-------------------------------------------------------------------------------------------

using namespace std;

void RecoComparison(bool doPASPlots, TString lepSel, TString histoDir, TString recoCompDir, int jetPtMin, int jetEtaMax)
{
    TH1::SetDefaultSumw2();
    gStyle->SetOptStat(0);

    TString energy = "8TeV";

    int Colors[NFILESDYJETS];
    TString legendNames[NFILESDYJETS];
    
    //-- get the files, legend names and colors -----------------------------------------------------------
    TFile *fSamples[NFILESDYJETS];
    for (unsigned short iSample = 0; iSample < NFILESDYJETS; ++iSample){
        //--- get the file ---
        TString syst = "0";
        if (iSample != 0) syst = "0";
        fSamples[iSample] = getFile(histoDir, lepSel, energy, Samples[iSample].name, jetPtMin, jetEtaMax, "", syst);
        if (!fSamples[iSample]) return;
        //-- set the legend name for the current file ---
        if (iSample == 0) {
            if (lepSel == "DMu") legendNames[iSample] = " #mu#mu Data";
            else if (lepSel == "DE") legendNames[iSample] = " ee Data";
            else if (lepSel == "SMu") legendNames[iSample] = " #mu Data";
            else if (lepSel == "SE") legendNames[iSample] = " e Data";
            else legendNames[iSample] = " Data";
        }
        else if (iSample == NFILESDYJETS-1) 
            legendNames[iSample] = (lepSel == "DMu") ? " Z/#gamma^{*} #rightarrow #mu#mu" : "Z/#gamma^{*} #rightarrow ee"; 
        else 
            legendNames[iSample] = doPASPlots ? Samples[iSample].legendPAS : Samples[iSample].legendAN; 
        //--- set the legend color for the current file ---
        Colors[iSample] = doPASPlots ? Samples[iSample].colorPAS : Samples[iSample].colorAN;    
    }
    //-----------------------------------------------------------------------------------------------------

    TString outputFileName = recoCompDir;
    system("mkdir -p " + recoCompDir);
    if (doPASPlots) outputFileName += "PAS_";
    outputFileName += "Comparison_" + lepSel + "_" + energy + "_Data_All_MC";
    outputFileName += "_JetPtMin_";
    outputFileName += jetPtMin;
    outputFileName += "_JetEtaMax_";
    outputFileName += jetEtaMax;
    //--- create the directory if it doesn't exist ---
    system("mkdir -p " + outputFileName);
    TString outputFileRoot = outputFileName + ".root";
    cout << "Output directory is: " << outputFileName << endl;
    cout << "Output root file is: " << outputFileRoot << endl;
    TFile *outputFile = new TFile(outputFileRoot, "RECREATE");
    outputFile->cd();

    //--- vector to contain the names and titles of the reco histograms ---
    vector<TString> vhNames;
    vector<TString> vhTitles;
    //---------------------------------------------------------------------

    //--- total number of histograms inside the data file 
    //    (included gen and TH2 that we dont want for comparison) ---
    unsigned short nTotHist = fSamples[0]->GetListOfKeys()->GetEntries();
    for (unsigned short i = 0; i < nTotHist; ++i) {
        TString hName = fSamples[0]->GetListOfKeys()->At(i)->GetName();
        TH1D *hTemp = (TH1D*) fSamples[0]->Get(hName);
        TString hTitle = hTemp->GetName();
        //--- skip histogram if it is gen or has no entry or is not a TH1 ---
        if (hName.Index("gen") >= 0 || hTemp->GetEntries() < 1 || !hTemp->InheritsFrom(TH1D::Class())) continue;

        //--- store the histograme name  and title ---
        vhNames.push_back(hName);
        vhTitles.push_back(hTitle);
    } 


    //--- vhNames size gives us the number of reco histograms
    //    interesting for comparison at reco level
    int nHist = vhNames.size();


    TH1D *hist[NFILESDYJETS][nHist];
    THStack *hSumMC[nHist];
    TLegend *legend[nHist];

    TLatex *cmsColl = new TLatex();
    cmsColl->SetTextSize(0.04);
    cmsColl->SetTextFont(61);
    cmsColl->SetLineWidth(2);
    cmsColl->SetTextColor(kBlack);
    cmsColl->SetNDC();
    cmsColl->SetTextAlign(11);


    TLatex *cmsPrel = new TLatex();
    cmsPrel->SetTextSize(0.04);
    cmsPrel->SetTextFont(52);
    cmsPrel->SetLineWidth(2);
    cmsPrel->SetTextColor(kBlack);
    cmsPrel->SetNDC();
    cmsPrel->SetTextAlign(11);

    TLatex *jetAlgo = new TLatex();
    jetAlgo->SetTextSize(0.035);
    jetAlgo->SetTextFont(42);
    jetAlgo->SetLineWidth(2);
    jetAlgo->SetTextColor(kBlack);
    jetAlgo->SetNDC();
    jetAlgo->SetTextAlign(11);

    TLatex *jetCuts = new TLatex();
    jetCuts->SetTextSize(0.035);
    jetCuts->SetTextFont(42);
    jetCuts->SetLineWidth(2);
    jetCuts->SetTextColor(kBlack);
    jetCuts->SetNDC();
    jetCuts->SetTextAlign(11);

    TLatex *intLumi = new TLatex();
    intLumi->SetTextSize(0.03);
    intLumi->SetTextFont(42);
    intLumi->SetLineWidth(2);
    intLumi->SetTextColor(kBlack);
    intLumi->SetNDC();
    intLumi->SetTextAlign(31);

    for (unsigned int i = 0; i < NFILESDYJETS; ++i) {
        for (int j = 0; j < nHist; ++j) {
            hist[i][j] = getHisto(fSamples[i], vhNames[j]);
            hist[i][j]->SetTitle(vhTitles[j]);
            if (i == 0) {
                hist[0][j]->SetMarkerStyle(20);
                hist[0][j]->SetMarkerColor(Colors[0]);
                hist[0][j]->SetLineColor(Colors[0]);
                hSumMC[j] = new THStack(vhNames[j], vhTitles[j]);

                if (!doPASPlots) { 
                    legend[j] = new TLegend(0.72, 0.5, 0.76, 0.86);
                    legend[j]->SetTextSize(0.032);
                }
                else {
                    legend[j] = new TLegend(0.63, 0.60, 0.81, 0.87);
                    legend[j]->SetTextSize(0.042);
                }
                legend[j]->SetFillStyle(0);
                legend[j]->SetBorderSize(0);
                legend[j]->SetTextFont(42);
                legend[j]->AddEntry(hist[0][j], legendNames[0], "ep");
            }
            else {
                hist[i][j]->SetFillColor(Colors[i]);
                hist[i][j]->SetLineColor(Colors[i]);
                hSumMC[j]->Add(hist[i][j]);
                if (!doPASPlots || i == 1 || i == 3 || i == 5 || i == 11) legend[j]->AddEntry(hist[i][j], legendNames[i], "f");
            }
        }
    }



    cout << "Now creating the pdf files ..." << endl;
    for (unsigned short i = 0; i < nHist; ++i) {

        TCanvas *canvas = new TCanvas(vhNames[i], vhNames[i], 700, 900);
        canvas->cd();

        TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
        pad1->SetTopMargin(0.11);
        pad1->SetBottomMargin(0.);
        pad1->SetRightMargin(0.03);
        pad1->SetTicks();
        pad1->SetLogy();
        pad1->Draw();
        pad1->cd();

        TH1D *hRatio = (TH1D*) hSumMC[i]->GetStack()->Last()->Clone();
        // Need to draw MC Stack first other wise
        // cannot access Xaxis !!!
        hSumMC[i]->Draw("HIST"); 
        if (vhNames[i].Index("ZMass_Z") >= 0){
            hist[0][i]->GetXaxis()->SetRangeUser(71,110.9);
            hSumMC[i]->GetXaxis()->SetRangeUser(71,110.9);
            hRatio->GetXaxis()->SetRangeUser(71,110.9);
        }
        //if (vhNames[i].Index("JetEta") >= 0){
        //    hist[0][i]->GetXaxis()->SetRangeUser(-2.4,2.4);
        //    hSumMC[i]->GetXaxis()->SetRangeUser(-2.4,2.4);
        //    hRatio->GetXaxis()->SetRangeUser(-2.4,2.4);

        //}

        hSumMC[i]->SetTitle(""); 
        hSumMC[i]->GetYaxis()->SetLabelSize(0.04); 
        hSumMC[i]->GetYaxis()->SetLabelOffset(0.002); 
        hSumMC[i]->GetYaxis()->SetTitle("# Events"); 
        hSumMC[i]->GetYaxis()->SetTitleSize(0.04); 
        hSumMC[i]->GetYaxis()->SetTitleOffset(1.32); 
        hSumMC[i]->SetMinimum(8);
        hSumMC[i]->SetMaximum(100*hSumMC[i]->GetMaximum()); 

        // first pad plots
        hist[0][i]->DrawCopy("e same");
        legend[i]->Draw();
        cmsColl->DrawLatex(0.13,0.82, "CMS");
        cmsPrel->DrawLatex(0.13,0.78, "Preliminary");
        if (energy == "7TeV")      intLumi->DrawLatex(0.97,0.9, "5.05 fb^{-1} (7 TeV)");
        else if (energy == "8TeV") intLumi->DrawLatex(0.97,0.9, "19.6 fb^{-1} (8 TeV)");
        if (vhNames[i].Index("inc0") < 0){
            if (!doPASPlots) {
                ostringstream ptLegend;
                if (vhNames[i].Index("JetPt_Zinc") > 0) {
                    ptLegend << "p_{T}^{jet} > 20GeV,  |#eta^{jet}| < " << (0.1*jetEtaMax);
                }
                else {
                    ptLegend << "p_{T}^{jet} > " << jetPtMin << "GeV,  |#eta^{jet}| < " << (0.1*jetEtaMax);
                }
                jetAlgo->DrawLatex(0.13,0.74, "anti-k_{t} jets,  R = 0.5");
                jetCuts->DrawLatex(0.13,0.70, ptLegend.str().c_str());
            }
            pad1->Draw();
        }
        canvas->cd();
        TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
        pad2->SetTopMargin(0.);
        pad2->SetBottomMargin(0.3);
        pad2->SetRightMargin(0.03);
        pad2->SetGridy();
        pad2->SetTicks();
        pad2->Draw();
        pad2->cd();

        hRatio->SetStats(0);
        hRatio->SetTitle("");

        hRatio->SetMarkerStyle(20);
        hRatio->SetMarkerColor(Colors[0]);
        hRatio->SetLineColor(Colors[0]);

        hRatio->GetXaxis()->SetTickLength(0.03);
        hRatio->GetXaxis()->SetTitleSize(0.1);
        hRatio->GetXaxis()->SetTitleOffset(1.2);
        hRatio->GetXaxis()->SetLabelSize(0.12);
        hRatio->GetXaxis()->SetLabelOffset(0.017);

        hRatio->GetYaxis()->SetRangeUser(0.51,1.49);
        hRatio->GetYaxis()->SetNdivisions(5,5,0);
        hRatio->GetYaxis()->SetTitle("Simulation/Data");
        hRatio->GetYaxis()->SetTitleSize(0.1);
        hRatio->GetYaxis()->SetTitleOffset(0.5);
        hRatio->GetYaxis()->CenterTitle();
        hRatio->GetYaxis()->SetLabelSize(0.08);

        hRatio->Divide(hist[0][i]);
        hRatio->DrawCopy("EP");

        canvas->cd();
        canvas->Update();

        TString outputFilePDF = outputFileName + "/" + vhNames[i] + ".pdf";
        canvas->Print(outputFilePDF);
        outputFile->cd();
        canvas->Write();

        hSumMC[i]->SetMaximum(1.5*hSumMC[i]->GetMaximum());
        TCanvas *tmpCanvas = (TCanvas*) canvas->Clone();
        tmpCanvas->cd();
        tmpCanvas->Draw();
        TPad *tmpPad = (TPad*) tmpCanvas->GetPrimitive("pad1");
        tmpPad->SetLogy(0);
        vhNames[i] += "_Lin";
        tmpCanvas->SetTitle(vhNames[i]);
        tmpCanvas->SetName(vhNames[i]);
        tmpCanvas->Update();
        TString outputFileLinPDF = outputFileName + "/" + vhNames[i] + ".pdf";
        tmpCanvas->Print(outputFileLinPDF);
        outputFile->cd();
        tmpCanvas->Write();
    }

    outputFile->cd();
    outputFile->Close();

    //-- Close all the files ------------------------------
    for (unsigned short i(0); i < NFILESDYJETS; ++i) closeFile(fSamples[i]);
    //-----------------------------------------------------

}
