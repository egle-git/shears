// -*- mode: c++ -*-
// Program to take one histogram from a root file and produce png or pdf file.
// Djarcaro - 1/22/16

// Standard includes:
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Root include files:
//#include "FWCore/ServiceRegistry/interface/Service.h"
//#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TLine.h"
#include "TMath.h"
#include "TPaveText.h"
#include "TStyle.h"

#define NFILES 2
#define DEBUG 0

// Gaussian
double gaussian(double *x, double *par)
{
    double arg = 0;
    if (par[2] < 0) par[2] = -par[2];                // par[2]: sigma
    if (par[2] != 0) arg = (x[0] - par[1]) / par[2]; // par[1]: mean

    // return par[0]*BIN_SIZE*TMath::Exp(-0.5*arg*arg)/
    //   (TMath::Sqrt(2*TMath::Pi())*par[2]);
    return par[0] * TMath::Exp(-0.5 * arg * arg) /
           (TMath::Sqrt(2 * TMath::Pi()) * par[2]); // par[0] is constant
}

// Breit-Wigner function
double breitwigner(double *x, double *par)
{
    double arg1 = 14.0 / 22.0;                       // 2 over pi
    double arg2 = par[1] * par[1] * par[2] * par[2]; // Gamma=par[1]  M=par[2]
    double arg3 = ((x[0] * x[0]) - (par[2] * par[2])) * ((x[0] * x[0]) - (par[2] * par[2]));
    double arg4 = x[0] * x[0] * x[0] * x[0] * ((par[1] * par[1]) / (par[2] * par[2]));
    return par[0] * arg1 * arg2 / (arg3 + arg4);
}

TF1 *FitZMass(TH1D *inputHisto, bool fitSwitch)
{
    double massMIN = 71.0;
    double massMAX = 111.0;
    double BIN_SIZE = inputHisto->GetBinWidth(1);
    TF1 *fit = 0;
    ////////////////
    // For Gaussian//
    ///////////////

    if (fitSwitch) {
        if (DEBUG) printf("Fitting Gaussian\n");
        TF1 *gaussfunc = new TF1("gauss", gaussian, massMIN, massMAX, 3);
        gaussfunc->SetParameter(0, 1.0);
        gaussfunc->SetParName(0, "const");
        gaussfunc->SetParameter(2, 5.0);
        gaussfunc->SetParName(2, "sigma");
        gaussfunc->SetParameter(1, 95.0);
        gaussfunc->SetParName(1, "mean");

        inputHisto->Fit("gauss", "QR0");
        fit = inputHisto->GetFunction("gauss");
    }
    /////////////////////
    // For Breit-Wigner//
    ////////////////////
    if (!fitSwitch) {
        if (DEBUG) printf("Fitting Breit-Wigner\n");
        TF1 *bwfunc = new TF1("bw", breitwigner, massMIN, massMAX, 3);
        bwfunc->SetParameter(0, 1.0);
        bwfunc->SetParName(0, "const");
        bwfunc->SetParameter(2, 5.0);
        bwfunc->SetParName(1, "sigma");
        bwfunc->SetParameter(1, 95.0);
        bwfunc->SetParName(2, "mean");

        inputHisto->Fit("bw", "QR0");
        fit = inputHisto->GetFunction("bw");
    }

    if (fit == 0) printf("Problem with the fit\n");

    return fit;
}

int main(int argc, char *argv[])
{

    TFile *rootFileInput[NFILES];
    rootFileInput[0] = new TFile(argv[1], "READ");
    rootFileInput[1] = new TFile(argv[2], "READ");

    // Options
    bool normBool = false;
    double_t norm = 1;
    bool log = false;
    bool grid = false;
    bool contBool = false;

    TCanvas *C = new TCanvas("C", "", 800, 800);

    TList *listOfKeys1 = rootFileInput[0]->GetListOfKeys();
    TObjLink *link1 = listOfKeys1->FirstLink();

    int count = 0;
    TH1D *histo[6] = {0};
    TH1D *tmpHisto1 = 0;
    TH1D *tmpHisto2 = 0;
    TF1 *fitFunction1 = 0;
    TF1 *fitFunction2 = 0;

    while (link1) {
        if (DEBUG) printf("While loop: %s   \n", link1->GetObject()->GetName());

        contBool = false;
        count++;

        if (DEBUG) {
            if (count > 800) break;
        }

        std::string histoName = link1->GetObject()->GetName();

        // if(histoName.substr(0,3)!="gen" || (histoName.find("JetsHT") != std::string::npos) ){
        // if(DEBUG) printf("not gen\n");
        //  link1 = link1->Next();
        // printf("Continuing\n");
        //  continue;
        //}
        if ((histoName.find("tau") != std::string::npos) ||
            (histoName.find("DPhi") != std::string::npos) ||
            (histoName.find("SpT") != std::string::npos) ||
            (histoName.find("dPhi") != std::string::npos) ||
            (histoName.find("alphas") != std::string::npos) ||
            (histoName.find("DifZ") != std::string::npos) ||
            (histoName.find("dEta") != std::string::npos) ||
            (histoName.find("SPhi") != std::string::npos) ||
            (histoName.find("_rej") != std::string::npos) ||
            (histoName.find("Third") != std::string::npos) ||
            (histoName.find("Fourth") != std::string::npos) ||
            (histoName.find("Fifth") != std::string::npos) ||
            (histoName.find("Sixth") != std::string::npos) ||
            (histoName.find("SumZ") != std::string::npos) ||
            (histoName.find("PHI") != std::string::npos) ||
            (histoName.find("VisPt") != std::string::npos) ||
            (histoName.find("JZB") != std::string::npos) ||
            (histoName.find("PU_") != std::string::npos)) {
            if (DEBUG) printf("bad name\n");
            link1 = link1->Next();
            if (DEBUG) printf("Continuing\n");
            continue;
        }

        /*
        if( (histoName.find("ZMass_Zinc0") == std::string::npos)){
          if(DEBUG) printf("bad name\n");
          link1 = link1->Next();
          if(DEBUG) printf("Continuing\n");
          continue;
        }
        */

        // if(DEBUG) printf("Creating TH1Ds\n");
        for (int iFile = 0; iFile < NFILES; iFile++) {
            if (!rootFileInput[iFile]->Get(histoName.c_str())->InheritsFrom(TH1D::Class())) {
                if (DEBUG) printf("Does not Inherit\n");
                contBool = true;
                break;
            }
            // if(isnan( ((TH1D*)rootFileInput[iFile]->Get(histoName.c_str()))->GetEntries())){
            //	//if(DEBUG) printf("Bad Entries\n");
            //	contBool= true;
            //   break;
            //}
            if (((TH1D *)rootFileInput[iFile]->Get(histoName.c_str()))->GetEntries() == 0.0) {
                if (DEBUG) printf("0 Entries\n");
                contBool = true;
                break;
            }
        }
        if (contBool) {
            link1 = link1->Next();
            if (DEBUG) printf("Continuing\n");
            continue;
        }

        if (DEBUG) printf("\nMade it through: %s\n", link1->GetObject()->GetName());
        for (int iFile = 0; iFile < NFILES; iFile++) {
            if (DEBUG) {
                // printf("_____________File %d_________________\n",iFile);
                // std::cout<<((TH1D*)rootFileInput[iFile]->Get(histoName.c_str()))->GetEntries()<<std::endl;
                // std::cout<<((TH1D*)rootFileInput[iFile]->Get(histoName.c_str()))->GetMaximum()<<std::endl;
                // std::cout<<((TH1D*)rootFileInput[iFile]->Get(histoName.c_str()))->GetName()<<std::endl;
            }
            // Attempting to copy the histograms
            histo[iFile] = (TH1D *)rootFileInput[iFile]->Get(histoName.c_str());
            // Normalize for bin width changes
            if (normBool) {
                histo[iFile]->Scale(1 / histo[iFile]->Integral(), "width");
                histo[iFile]->GetYaxis()->SetTitle("Norm # Events");
            } else
                histo[iFile]->Scale(1, "width");
            if (DEBUG) {
                std::cout << "histo GetEntries " << histo[iFile]->GetEntries() << std::endl;
                std::cout << "histo Max " << histo[iFile]->GetMaximum() << std::endl;
            }
        }

        // Draw upper pad
        if (DEBUG) printf("Drawing upper pad\n");
        TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
        pad1->SetBottomMargin(0); // Upper and lower plot are joined
        pad1->Draw();
        pad1->cd();

        if (DEBUG) printf("Drawing upper histograms\n");
        if (log) pad1->SetLogy();

        TPaveText *pt = 0;

        if (histoName.find("ZMass_Zinc") != std::string::npos) {
            histo[0]->SetAxisRange(71.0, 111.0);
            histo[1]->SetAxisRange(71.0, 111.0);
        }

        histo[0]->Draw();
        histo[1]->Draw("SAME");

        /*
        if(histoName.find("ZMass_Zinc") != std::string::npos){
          fitFunction1 = FitZMass(histo[0],0);
          fitFunction2 = FitZMass(histo[1],0);

          fitFunction2->SetLineColor(4);

          //fitFunction1->Draw("SAME");
          fitFunction2->Draw("SAME");

          std::stringstream tmpStream;
          pt = new TPaveText(0.7,0.45,0.9,0.7,"NDC");



          pt->AddText("Breit-Wigner Fits");

          pt->AddText("Roch=False");
          tmpStream<<"Mean="<<fitFunction1->GetParameter("mean")
                    <<"  Width="<<fitFunction1->GetParameter("sigma");
          pt->AddText(tmpStream.str().c_str());

          tmpStream.str("");
          pt->AddText("Roch=True");
          tmpStream<<"Mean="<<fitFunction2->GetParameter("mean")
                    <<"  Width="<<fitFunction2->GetParameter("sigma");
          pt->AddText(tmpStream.str().c_str());
          tmpStream.str("");
          pt->AddText("PDG");
          tmpStream<<"Mean=91.19"
                    <<"  Width=2.50";
          pt->AddText(tmpStream.str().c_str());

          pt->Draw("SAME");


        }
    */

        TLegend *leg = new TLegend(0.75, 0.75, 1.0, 1.0);
        leg->SetHeader("");
        leg->SetFillColor(0);
        leg->SetBorderSize(0);
        leg->SetTextSize(0.04);
        std::stringstream tmpStream1;
        std::stringstream tmpStream2;
        tmpStream1 << "#splitline{Roch=False}{ (" << abs(histo[0]->GetEntries()) << " Evts)}";
        std::cout << tmpStream1.str() << std::endl;
        leg->AddEntry(histo[0], tmpStream1.str().c_str(), "l");
        tmpStream2 << "#splitline{Roch=True}{ (" << abs(histo[1]->GetEntries()) << " Evts)}";
        std::cout << tmpStream2.str() << std::endl;
        leg->AddEntry(histo[1], tmpStream2.str().c_str(), "l");
        leg->Draw();

        gStyle->SetOptStat(0);

        // Draw lower pad (go back to canvas first)
        C->cd();
        if (DEBUG) printf("Drawing lower pad\n");
        TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
        pad2->SetTopMargin(0);
        pad2->SetBottomMargin(0.2);
        pad2->SetGridy(); // horizontal grid
        pad2->SetTopMargin(0);
        pad2->Draw();
        pad2->cd();
        // Define the ratio plot
        TH1D *ratioHisto = (TH1D *)histo[0]->Clone("ratioHisto");
        ratioHisto->SetLineColor(kBlack);
        ratioHisto->SetMinimum(0.5); // Define Y ..
        ratioHisto->SetMaximum(1.5); // .. range
        // ratioHisto->Sumw2();
        ratioHisto->SetStats(0); // No statistics on lower plot
        ratioHisto->Divide(histo[1]);
        // h3->SetMarkerStyle(21);
        ratioHisto->Draw("ep"); // Draw the ratio plot

        // Plot Settings
        // tmpHisto1 settings
        histo[0]->SetLineWidth(2);
        histo[0]->SetLineColor(2);

        // Y axis h1 plot settings
        histo[0]->GetYaxis()->SetTitleSize(20);
        histo[0]->GetYaxis()->SetTitleFont(43);
        histo[0]->GetYaxis()->SetTitleOffset(1.55);

        // h2 settings
        histo[1]->SetLineWidth(2);

        // Ratio plot (h3) settings
        ratioHisto->SetTitle(""); // Remove the ratio title

        // Y axis ratio plot settings
        ratioHisto->GetYaxis()->SetTitle("ratio");
        ratioHisto->GetYaxis()->SetNdivisions(505);
        ratioHisto->GetYaxis()->SetTitleSize(20);
        ratioHisto->GetYaxis()->SetTitleFont(43);
        ratioHisto->GetYaxis()->SetTitleOffset(1.55);
        ratioHisto->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
        ratioHisto->GetYaxis()->SetLabelSize(15);

        // X axis ratio plot settings
        ratioHisto->GetXaxis()->SetTitleSize(20);
        ratioHisto->GetXaxis()->SetTitleFont(43);
        ratioHisto->GetXaxis()->SetTitleOffset(4.);
        ratioHisto->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
        ratioHisto->GetXaxis()->SetLabelSize(15);

        if (DEBUG) printf("Printing histograms\n");
        C->cd();
        std::string tmpName = histoName;
        if (normBool) tmpName += "_norm";
        if (log) tmpName += "_log";
        tmpName += ".pdf";

        C->Print(tmpName.c_str(), "pdf");
        pad1->Clear();
        pad2->Clear();
        C->Clear();
        link1 = link1->Next();

        for (int iFile = 0; iFile < 6; iFile++) {
            histo[iFile] = NULL;
        }
        tmpHisto1 = 0;
        tmpHisto2 = 0;
    }
    if (DEBUG) printf("Done with the while loop\n");

    for (int iFile = 0; iFile < NFILES; iFile++) {
        rootFileInput[iFile]->Close();
        delete rootFileInput[iFile];
    }
    delete C;

    // std::string outputNamePNG = inputName +"_"+ rootFileName +".png";
    // std::string outputNamePNG = inputName +"zoom_"+ rootFileName +".png";

    // tmpHisto->Fit("gaus");

    // std::cout<<outputNamePNG<<std::endl;

    // tmpHisto->GetXaxis()->SetRangeUser(2, 8);

    // C->Print(outputNamePDF.c_str(),"pdf");
    // C->Print(outputNamePNG.c_str(),"png");

    // std::string curDir(getenv("PWD"));

    // std::string oldPDFName = curDir +"/"+ outputNamePDF;
    // std::string newPDFName = curDir + "/histograms/pdf/"+ outputNamePDF;
    // std::string oldPNGName = curDir +"/"+ outputNamePNG;
    // std::string newPNGName = curDir + "/histograms/png/" + outputNamePNG;

    // rename(oldPDFName.c_str(),newPDFName.c_str());
    // rename(oldPNGName.c_str(),newPNGName.c_str());

    return 0;
}
