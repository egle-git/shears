//-*- mode: c++; c-basic-offset: 4 -*-
#include "PlotSettings.h"
#include "TStyle.h"
#include "TFile.h"
#include "TMath.h"
#include "TSystem.h"
#include "TRegexp.h"
#include "TParameter.h"
#include <sstream>
#include <string>
#include <set>
#include <limits>
#include "rootFunctions.h"
#include "functions.h"
#include "variablesOfInterestZJets.h"
#include "getFilesAndHistogramsZJets.h"
#include "fixYscale.h"
#include "SectionedConfig.h"

using namespace std;
#include "ConfigVJets.h"

extern ConfigVJets cfg;

void setAndDrawTPad(TString canvasName, TPad *plot, int plotNumber, int numbOfGenerator)
{
    plot->SetNumber(plotNumber);
    if (numbOfGenerator == 1) {
        if (plotNumber == 1) {
            plot->SetPad(0.01, 0.35, 0.99, 0.99);
            plot->SetTopMargin(0.11);
            plot->SetBottomMargin(0.005);
        }
        else if (plotNumber == 2) {
            plot->SetPad(0.01, 0.01, 0.99, 0.35);
            plot->SetTopMargin(0.0);
            plot->SetBottomMargin(0.3);
        }
    }
    else if (numbOfGenerator == 2) {
        if (plotNumber == 1) {
            plot->SetPad(0.01, 0.45, 0.99, 0.99);
            plot->SetTopMargin(0.11);
            plot->SetBottomMargin(0.005);
        }
        else if (plotNumber == 2) {
            plot->SetPad(0.01, 0.27, 0.99, 0.45);
            plot->SetTopMargin(0.0);
            plot->SetBottomMargin(0.0);
        }
        else if (plotNumber == 3) {
            plot->SetPad(0.01, 0.01, 0.99, 0.27);
            plot->SetTopMargin(0.0);
            plot->SetBottomMargin(0.3);
        }

    }
    else if (numbOfGenerator == 3) {
        if (plotNumber == 1) {
            plot->SetPad(0.01, 0.55, 0.99, 0.99);
            plot->SetTopMargin(0.11);
            plot->SetBottomMargin(0.005);
        }
        else if (plotNumber == 2) {
            plot->SetPad(0.01, 0.39, 0.99, 0.55);
            plot->SetTopMargin(0.0);
            plot->SetBottomMargin(0.0);
        }
        else if (plotNumber == 3) {
            plot->SetPad(0.01, 0.23, 0.99, 0.39);
            plot->SetTopMargin(0.0);
            plot->SetBottomMargin(0.0);
        }
        else if (plotNumber == 4) {
            plot->SetPad(0.01, 0.01, 0.99, 0.23);
            plot->SetTopMargin(0.0);
            plot->SetBottomMargin(0.3);
        }
    }


    if (plotNumber == 1 && (canvasName.Index("Eta") < 0 && canvasName.Index("AbsRapidity") < 0 && canvasName.Index("DPhi") < 0)) plot->SetLogy();
    if (plotNumber == 1 && canvasName.Index("DPhiZFirstJet") > 0) plot->SetLogy();
    plot->SetLeftMargin(0.13);
    plot->SetRightMargin(0.07);
    plot->SetFillStyle(0);
    plot->Draw();
    plot->cd();
}

void customizeLegend(TLegend *legend, int numbOfGenerator)
{
    legend->SetFillColor(0);
    legend->SetFillStyle(1001);
    legend->SetBorderSize(1);
    legend->SetMargin(0.15);
    legend->SetTextFont(43);
    legend->SetTextSize(15);
//    if (numbOfGenerator == 1) {
//        legend->SetX1(0.39);
//        legend->SetY1(0.84);
//        legend->SetX2(0.96);
//        legend->SetY2(0.98);
//	//        legend->SetTextSize(.029);
//    }
//    else if (numbOfGenerator == 2) {
//        legend->SetX1(0.39);
//        legend->SetY1(0.77);
//        legend->SetX2(0.96);
//        legend->SetY2(0.98);
//        //legend->SetTextSize(.034);
//    }
//    else if (numbOfGenerator == 3) {
//        legend->SetX1(0.39);
//        legend->SetY1(0.7);
//        legend->SetX2(0.96);
//        legend->SetY2(0.98);
//	//        legend->SetTextSize(.042);
//    }
    legend->SetX1(0.39);
    legend->SetY1(std::max(0., 0.91 - numbOfGenerator*0.07));
    legend->SetX2(0.96);
    legend->SetY2(0.98);
}

void customizeLegend(TLegend *legend, int genNumb, int numbOfGenerator)
{
    legend->SetFillColor(0);
    legend->SetFillStyle(ZJetsFillStyle);
    legend->SetBorderSize(0);
    //legend->SetTextSize(.075);
    legend->SetTextSize(.12);

    if (genNumb == numbOfGenerator) {
        if (numbOfGenerator == 1) {
            legend->SetY1(0.35);
            legend->SetX2(0.43);
            legend->SetY2(0.45);
            //legend->SetTextSize(0.06);
            legend->SetTextSize(0.08);
        }

        if (numbOfGenerator == 2) {
            legend->SetY1(0.34);
            legend->SetX2(0.43);
            legend->SetY2(0.45);
            //legend->SetTextSize(0.06);
            legend->SetTextSize(0.08);
        }
        if (numbOfGenerator == 3) {
            legend->SetY1(0.34);
            legend->SetX2(0.43);
            legend->SetY2(0.45);
            //legend->SetTextSize(0.06);
            legend->SetTextSize(0.09);
        }
    }

}

void customizeCentral(TGraphAsymmErrors *grCentral, bool ratio)
{
    customizeCentral(grCentral, (TLegend*) NULL);
    if (ratio) grCentral->SetMarkerSize(0);
}

void customizeCentral(TGraphAsymmErrors *grCentral, TLegend *legend, TString legText)
{
    grCentral->SetLineColor(kBlack);
    grCentral->SetLineWidth(2);
    grCentral->SetMarkerStyle(20);
    grCentral->SetFillColor(12);
    gStyle->SetHatchesSpacing(1.5);
    gStyle->SetHatchesLineWidth(2);
    grCentral->SetFillStyle(3354);
    grCentral->SetMarkerColor(kBlack);

    grCentral->GetXaxis()->SetTitleOffset(1.0);
    grCentral->GetXaxis()->SetTitleSize(0.05);
    grCentral->GetXaxis()->SetLabelSize(0.0);
    grCentral->GetXaxis()->SetLabelFont(42);
    grCentral->GetXaxis()->SetTitleFont(42);

    grCentral->GetYaxis()->SetTitleOffset(1.1);
    grCentral->GetYaxis()->SetTitleSize(0.07);
    grCentral->GetYaxis()->SetLabelSize(0.05);
    grCentral->GetYaxis()->SetLabelFont(42);
    grCentral->GetYaxis()->SetTitleFont(42);

    grCentral->SetTitle();
    grCentral->GetXaxis()->SetTitle();
    if (legend) legend->AddEntry(grCentral, legText, "PLEF");

}

TGraphAsymmErrors* createGrFromHist(const TH1 *h)
{
    int nPoints = h->GetNbinsX();
    double *xCoor = new double[nPoints];
    double *yCoor = new double[nPoints];
    double *xErr  = new double[nPoints];
    double *yErr = new double[nPoints];

    for (int i(0); i < nPoints; i++) {
        xCoor[i] = h->GetBinCenter(i+1);
        xErr[i]  = 0.5*h->GetBinWidth(i+1);
        yCoor[i] = h->GetBinContent(i+1);
        yErr[i] = h->GetBinError(i+1);
    }

    TGraphAsymmErrors *gr = new TGraphAsymmErrors(nPoints, xCoor, yCoor, xErr, xErr, yErr, yErr);

    delete [] xCoor; delete [] yCoor; delete [] xErr; delete [] yErr; 
    return gr;
}

//======================================================================
// This function creates a TGraphAsymmErrors from a TGraphAsymmErrors.
// The output TGraph has all yCoor set to 1. and y low and high errors
// representing the relative errors of the input TGraph.
//======================================================================
TGraphAsymmErrors* createRatioGraph(const TGraphAsymmErrors* grCentral)
{       
    int nPoints = grCentral->GetN();
    double *xCoor = new double[nPoints];
    double *yCoor = new double[nPoints];
    double *xErr  = new double[nPoints];
    double *yErrL = new double[nPoints];
    double *yErrH = new double[nPoints];

    for (int i(0); i < nPoints; i++) {
        grCentral->GetPoint(i, xCoor[i], yCoor[i]);
        xErr[i] = grCentral->GetErrorXlow(i);
        yErrL[i] = grCentral->GetErrorYlow(i)/yCoor[i];
        yErrH[i] = grCentral->GetErrorYhigh(i)/yCoor[i];
        yCoor[i] = 1.;
    }   

    TGraphAsymmErrors *grCentralRatio = new TGraphAsymmErrors(nPoints, xCoor, yCoor, xErr, xErr, yErrL, yErrH);

    delete [] xCoor; delete [] yCoor; delete [] xErr; delete [] yErrL; delete [] yErrH; 
    return grCentralRatio;
}

//======================================================================
// This function creates a TGraphErrors from a TGraphErrors.
// The output TGraph has all yCoor set to 1. and y errors
// representing the relative errors of the input TGraph.
//======================================================================
TGraphErrors* createRatioGraph(const TGraphErrors* grCentral)
{       
    int nPoints = grCentral->GetN();
    double *xCoor = new double[nPoints];
    double *yCoor = new double[nPoints];
    double *xErr  = new double[nPoints];
    double *yErr = new double[nPoints];

    for (int i(0); i < nPoints; i++) {
        grCentral->GetPoint(i, xCoor[i], yCoor[i]);
        xErr[i] = grCentral->GetErrorX(i);
        yErr[i] = grCentral->GetErrorY(i)/yCoor[i];
        yCoor[i] = 1.;
    }   

    TGraphErrors *grCentralRatio = new TGraphErrors(nPoints, xCoor, yCoor, xErr, yErr);

    delete [] xCoor; delete [] yCoor; delete [] xErr; delete [] yErr; 
    return grCentralRatio;
}

//================================================================================
// This function creates a TGraphAsymmErrors from a TH1 and a TGraphAsymmErrors.
// The output TGraph is the ratio of the TH1 by the TGraphAsymmErrors.
//================================================================================
TGraphAsymmErrors *createGenToCentral(const TH1 *gen, const TGraphAsymmErrors *grCentral)
{
    if(!gen) return 0;
    
    int nPoints = grCentral->GetN();
    int nPoints2 = gen->GetNbinsX();
    int diff = fabs(nPoints - nPoints2);
    if (nPoints != nPoints2) {
        nPoints = nPoints2;
    }
    double *xCoor = new double[nPoints];
    double *yCoor = new double[nPoints];
    double *xErr  = new double[nPoints];
    double *yErr  = new double[nPoints];


    for (int i(0); i < nPoints; i++) {
        grCentral->GetPoint(i+diff, xCoor[i], yCoor[i]);
        xErr[i] = grCentral->GetErrorXlow(i+diff);
        yErr[i] = 0.;
        if (yCoor[i] != 0) {
            yErr[i]  = gen->GetBinError(i+1)/yCoor[i];
            yCoor[i] = gen->GetBinContent(i+1)/yCoor[i];
        }
    }
    /*
      int nPoints = grCentral->GetN();
      double *xCoor = new double[nPoints];
      double *yCoor = new double[nPoints];
      double *xErr  = new double[nPoints];
      double *yErr  = new double[nPoints];

      for (int i(0); i < nPoints; i++) {
      grCentral->GetPoint(i, xCoor[i], yCoor[i]);
      xErr[i] = grCentral->GetErrorXlow(i);
      yErr[i] = 0.;
      if (yCoor[i] != 0) {
      yErr[i]  = gen->GetBinError(i+1)/yCoor[i];
      yCoor[i] = gen->GetBinContent(i+1)/yCoor[i];
      }
      }

    */
    TGraphAsymmErrors *grGenToCentral = new TGraphAsymmErrors(nPoints, xCoor, yCoor, xErr, xErr, yErr, yErr);
    delete [] xCoor; delete [] yCoor; delete [] xErr; delete [] yErr; 
    return grGenToCentral;
}

TGraphAsymmErrors* createScaleSystGraph(TString sample, TString lepSel, TString variable,
					const TGraphAsymmErrors *grGenToCentral)
{
    int nPoints = grGenToCentral->GetN();
    double *xCoor    = new double[nPoints];
    double *yCoor    = new double[nPoints];
    double *xErr     = new double[nPoints];
    double *yErrUp   = new double[nPoints];
    double *yErrDown = new double[nPoints];

    TString histoDir = cfg.getS("histoDir");
    TFile *fDE;
    if (lepSel == "DE" || lepSel == "") {
        fDE = new TFile(histoDir + "/DE_13TeV_" + sample + "_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
	if(!fDE || fDE->IsZombie()){
	    std::cerr << "Fatal error. Failed to open file  " << fDE->GetName()  << ".\n";
	    abort();
	}
    }

    TFile *fDMu;
    if (lepSel == "DMu" || lepSel == "") {
        fDMu = new TFile(histoDir + "/DMu_13TeV_" + sample + "_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
	if(!fDMu || fDMu->IsZombie()){
	    std::cerr << "Fatal error. Failed to open file  " << fDMu->GetName()  << ".\n";
	    abort();
	}
    }

    TGraphAsymmErrors *grDE, *grDMu;
    if (lepSel == "DE" || lepSel == "") {
        grDE = (TGraphAsymmErrors*) fDE->Get("gen" + variable + "_scaleUnc");
	//FIXME mem. leak
	if(!grDE) return 0;
    }
    if (lepSel == "DMu" || lepSel == "") {
        grDMu = (TGraphAsymmErrors*) fDMu->Get("gen" + variable + "_scaleUnc");
	//FIXME mem. leak
	if(!grDMu) return 0;
    }

    // ---- this variable is used to fetch the TGraph of scale uncertainty from input file ----
    double *xMeanDMu  = new double[nPoints];
    double *yMeanDMu  = new double[nPoints];
    double *xMeanDE   = new double[nPoints];
    double *yMeanDE   = new double[nPoints];

    for (int i(0); i < nPoints; i++) {
        grGenToCentral->GetPoint(i, xCoor[i], yCoor[i]);

        xErr[i] = grGenToCentral->GetErrorXlow(i);

        yErrUp[i] = pow(grGenToCentral->GetErrorYhigh(i), 2);
        yErrDown[i] = pow(grGenToCentral->GetErrorYlow(i), 2);

        if ((lepSel == "DMu" && grDMu) || (lepSel == "DMu" && grDMu && !grDE)) {
            grDMu->GetPoint(i, xMeanDMu[i], yMeanDMu[i]);
            yErrUp[i] += pow((grDMu->GetErrorYhigh(i)/yMeanDMu[i]) * yCoor[i], 2);
            yErrDown[i] += pow((grDMu->GetErrorYlow(i)/yMeanDMu[i]) * yCoor[i], 2);
        }

        if ((lepSel == "DE" && grDE) || (lepSel == "DMu" && !grDMu && grDE)) {
            grDE->GetPoint(i, xMeanDE[i], yMeanDE[i]);
            yErrUp[i] += pow((grDE->GetErrorYhigh(i)/yMeanDE[i]) * yCoor[i], 2);
            yErrDown[i] += pow((grDE->GetErrorYlow(i)/yMeanDE[i]) * yCoor[i], 2);
        }

        if (lepSel == "" && grDE && grDMu) {
            grDMu->GetPoint(i, xMeanDMu[i], yMeanDMu[i]);
            grDE->GetPoint(i, xMeanDE[i], yMeanDE[i]);
            yErrUp[i] += pow(((grDMu->GetErrorYhigh(i) + grDE->GetErrorYhigh(i)) / (yMeanDMu[i] + yMeanDE[i])) * yCoor[i], 2);
            yErrDown[i] += pow(((grDMu->GetErrorYhigh(i) + grDE->GetErrorYhigh(i)) / (yMeanDMu[i] + yMeanDE[i])) * yCoor[i], 2);
        }

        yErrUp[i] = sqrt(yErrUp[i]);
        yErrDown[i] = sqrt(yErrDown[i]);

    }

    TGraphAsymmErrors *grScaleSyst = new TGraphAsymmErrors(nPoints, xCoor, yCoor, xErr, xErr, yErrDown, yErrUp);
    delete [] xCoor; delete [] yCoor; delete [] xErr; delete [] yErrDown; delete [] yErrUp;
    delete [] xMeanDMu; delete [] yMeanDMu; delete [] xMeanDE; delete [] yMeanDE;
    if (lepSel == "DE" || lepSel == "") {
        fDE->Close();
    }
    if (lepSel == "DMu" || lepSel == "") {
        fDMu->Close();
    }
    return grScaleSyst;

}

// --- This function is dedicated for NNLO theoretical prediction ---
TGraphAsymmErrors* createNNLOScaleSystGraph(TString lepSel, TString variable, const TGraphAsymmErrors *grGenToCentral)
{
    int nPoints = grGenToCentral->GetN();
    double *xCoor    = new double[nPoints];
    double *yCoor    = new double[nPoints];
    double *xErr     = new double[nPoints];
    double *yErrUp   = new double[nPoints];
    double *yErrDown = new double[nPoints];

    TString histoDir = cfg.getS("histoDir");

    TFile *fnnlo[3];
    if (lepSel == "DE" || lepSel == "DMu" || lepSel == "") {
        fnnlo[0] = new TFile(histoDir + "/DMu_13TeV_DYJets_ZjNNLO_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
	if(!fnnlo[0] || fnnlo[0]->IsZombie()){
	    std::cerr << "Fatal error. Histo file for ZjNNLO prediction, "
		      << histoDir + "NNLO.root"
		      << " was not found.\n";
	}
        fnnlo[1] = new TFile(histoDir + "/DMu_13TeV_DYJets_ZjNNLO_Scaleup_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
	if(!fnnlo[1] || fnnlo[1]->IsZombie()){
	    std::cerr << "Fatal error. Histo file for scale up uncertainties of ZjNNLO prediction, "
		      << histoDir + "NNLO.root"
		      << " was not found.\n";
	}
	
        fnnlo[2] = new TFile(histoDir + "/DMu_13TeV_DYJets_ZjNNLO_Scaledn_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
	if(!fnnlo[2] || fnnlo[2]->IsZombie()){
	    std::cerr << "Fatal error. Histo file for scale down uncertainties of ZjNNLO prediction, "
		      << histoDir + "NNLO.root"
		      << " was not found.\n";
	}

    }

    TH1D *hnnlo[3];
    if (lepSel == "DE" || lepSel == "DMu" || lepSel == "") {
        hnnlo[0] = (TH1D*) fnnlo[0]->Get("gen" + variable);
        hnnlo[1] = (TH1D*) fnnlo[1]->Get("gen" + variable);
        hnnlo[2] = (TH1D*) fnnlo[2]->Get("gen" + variable);
    }

    // ---- this variable is used to fetch the TGraph of scale uncertainty from input file ----

    for (int i(0); i < nPoints; i++) {
        grGenToCentral->GetPoint(i, xCoor[i], yCoor[i]);

        xErr[i] = grGenToCentral->GetErrorXlow(i);

        yErrUp[i] = pow(grGenToCentral->GetErrorYhigh(i), 2);
        yErrDown[i] = pow(grGenToCentral->GetErrorYlow(i), 2);

        if (lepSel == "DMu" || lepSel == "DE" || lepSel == "") {
            yErrUp[i] += pow((hnnlo[1]->GetBinContent(i+1) - hnnlo[0]->GetBinContent(i+1))/hnnlo[0]->GetBinContent(i+1) * yCoor[i], 2);
            yErrDown[i] += pow((hnnlo[0]->GetBinContent(i+1) - hnnlo[2]->GetBinContent(i+1))/hnnlo[0]->GetBinContent(i+1) * yCoor[i], 2);
        }

        yErrUp[i] = sqrt(yErrUp[i]);
        yErrDown[i] = sqrt(yErrDown[i]);

    }

    TGraphAsymmErrors *grScaleSyst = new TGraphAsymmErrors(nPoints, xCoor, yCoor, xErr, xErr, yErrDown, yErrUp);
    delete [] xCoor; delete [] yCoor; delete [] xErr; delete [] yErrDown; delete [] yErrUp;
    if (lepSel == "DE" || lepSel == "DMu" || lepSel == "") {
        fnnlo[0]->Close();
        fnnlo[1]->Close();
        fnnlo[2]->Close();
    }
    return grScaleSyst;

}

// ---- create PDF systematic graph with other uncertainty in quadrature ----
TGraphAsymmErrors* createPDFSystGraph(TString sample, TString lepSel, TString variable,
				      const TGraphAsymmErrors *grGenToCentral, const TGraphAsymmErrors *grGen3ScaleSyst)
{
    int nPoints = grGenToCentral->GetN();
    double *xCoor    = new double[nPoints];
    double *yCoor    = new double[nPoints];
    double *xErr     = new double[nPoints];
    double *yErrUp   = new double[nPoints];
    double *yErrDown = new double[nPoints];

    TString histoDir = cfg.getS("histoDir");
    TFile *fDE;
    if (lepSel == "DE" || lepSel == "") {
        fDE = new TFile(histoDir + "/DE_13TeV_" + sample + "_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    }

    TFile *fDMu;
    if (lepSel == "DMu" || lepSel == "") {
        fDMu = new TFile(histoDir + "/DMu_13TeV_" + sample + "_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    }

    TGraphAsymmErrors *grDE, *grDMu;
    if (lepSel == "DE" || lepSel == "") {
        grDE = (TGraphAsymmErrors*) fDE->Get("gen" + variable + "_pdfUncPlain");
	//FIXME mem. leak
	if(!grDE) return 0;
    }
    if (lepSel == "DMu" || lepSel == "") {
        grDMu = (TGraphAsymmErrors*) fDMu->Get("gen" + variable + "_pdfUncPlain");
	//FIXME mem. leak
	if(!grDMu) return 0;
    }

    // ---- this variable is used to fetch the TGraph of scale uncertainty from input file ----
    double *xMeanDMu  = new double[nPoints];
    double *yMeanDMu  = new double[nPoints];
    double *xMeanDE   = new double[nPoints];
    double *yMeanDE   = new double[nPoints];

    for (int i(0); i < nPoints; i++) {
        grGenToCentral->GetPoint(i, xCoor[i], yCoor[i]);

        xErr[i] = grGenToCentral->GetErrorXlow(i);
        yErrUp[i] = pow(grGenToCentral->GetErrorYhigh(i), 2);
        yErrDown[i] = pow(grGenToCentral->GetErrorYlow(i), 2);

	if(grGen3ScaleSyst){
	    yErrUp[i]   += pow(grGen3ScaleSyst->GetErrorYhigh(i), 2);
	    yErrDown[i] += pow(grGen3ScaleSyst->GetErrorYlow(i), 2);
	}
	
        if ((lepSel == "DMu" && grDMu) || (lepSel == "" && grDMu && !grDE)) {
            grDMu->GetPoint(i, xMeanDMu[i], yMeanDMu[i]);
            yErrUp[i] += pow((grDMu->GetErrorYhigh(i)/yMeanDMu[i]) * yCoor[i], 2);
            yErrDown[i] += pow((grDMu->GetErrorYlow(i)/yMeanDMu[i]) * yCoor[i], 2);
        }

        if ((lepSel == "DE" && grDE) || (lepSel == "" && !grDMu && grDE)) {
            grDE->GetPoint(i, xMeanDE[i], yMeanDE[i]);
            yErrUp[i] += pow((grDE->GetErrorYhigh(i)/yMeanDE[i]) * yCoor[i], 2);
            yErrDown[i] += pow((grDE->GetErrorYlow(i)/yMeanDE[i]) * yCoor[i], 2);
        }

        if (lepSel == "" && grDE && grDMu) {
            grDMu->GetPoint(i, xMeanDMu[i], yMeanDMu[i]);
            grDE->GetPoint(i, xMeanDE[i], yMeanDE[i]);
            yErrUp[i] += pow(((grDMu->GetErrorYhigh(i) + grDE->GetErrorYhigh(i)) / (yMeanDMu[i] + yMeanDE[i])) * yCoor[i], 2);
            yErrDown[i] += pow(((grDMu->GetErrorYhigh(i) + grDE->GetErrorYhigh(i)) / (yMeanDMu[i] + yMeanDE[i])) * yCoor[i], 2);
        }

        yErrUp[i] = sqrt(yErrUp[i]);
        yErrDown[i] = sqrt(yErrDown[i]);

    }

    TGraphAsymmErrors *grPDFSyst = new TGraphAsymmErrors(nPoints, xCoor, yCoor, xErr, xErr, yErrDown, yErrUp);
    delete [] xCoor; delete [] yCoor; delete [] xErr; delete [] yErrDown; delete [] yErrUp;
    delete [] xMeanDMu; delete [] yMeanDMu; delete [] xMeanDE; delete [] yMeanDE;

    if (lepSel == "DE" || lepSel == "") {
        fDE->Close();
    }
    if (lepSel == "DMu" || lepSel == "") {
        fDMu->Close();
    }

    return grPDFSyst;
}

//// ---- create PDF systematical graph without other uncertainty in quadrature ----
//TGraphAsymmErrors* createPDFSystGraph(TString lepSel, TString variable, const TGraphAsymmErrors *grGenToCentral)
//{
//    int nPoints = grGenToCentral->GetN();
//    double *xCoor    = new double[nPoints];
//    double *yCoor    = new double[nPoints];
//    double *xErr     = new double[nPoints];
//    double *yErrUp   = new double[nPoints];
//    double *yErrDown = new double[nPoints];
//
//    TString histoDir = cfg.getS("histoDir");
//    TFile *fDE;
//    if (lepSel == "DE" || lepSel == "") {
//      //        fDE = new TFile("HistoFilesUnc/DE_13TeV_DYJets_UNFOLDING_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
//        fDE = new TFile(histoDir + "/DE_13TeV_DYJets_UNFOLDING_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
//    }
//
//    TFile *fDMu;
//    if (lepSel == "DMu" || lepSel == "") {
//        fDMu = new TFile(histoDir + "/DMu_13TeV_DYJets_UNFOLDING_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
//    }
//
//    TGraphAsymmErrors *grDE, *grDMu;
//    if (lepSel == "DE" || lepSel == "") {
//        grDE = (TGraphAsymmErrors*) fDE->Get("gen" + variable + "_pdfUncPlain");
//    }
//    if (lepSel == "DMu" || lepSel == "") {
//        grDMu = (TGraphAsymmErrors*) fDMu->Get("gen" + variable + "_pdfUncPlain");
//    }
//
//    // ---- this variable is used to fetch the TGraph of scale uncertainty from input file ----
//    double *xMeanDMu  = new double[nPoints];
//    double *yMeanDMu  = new double[nPoints];
//    double *xMeanDE   = new double[nPoints];
//    double *yMeanDE   = new double[nPoints];
//
//    for (int i(0); i < nPoints; i++) {
//        grGenToCentral->GetPoint(i, xCoor[i], yCoor[i]);
//
//        xErr[i] = grGenToCentral->GetErrorXlow(i);
//
//        if (lepSel == "DMu") {
//            grDMu->GetPoint(i, xMeanDMu[i], yMeanDMu[i]);
//            yErrUp[i] += pow((grDMu->GetErrorYhigh(i)/yMeanDMu[i]) * yCoor[i], 2);
//            yErrDown[i] += pow((grDMu->GetErrorYlow(i)/yMeanDMu[i]) * yCoor[i], 2);
//        }
//
//        if (lepSel == "DE") {
//            grDE->GetPoint(i, xMeanDE[i], yMeanDE[i]);
//            yErrUp[i] += pow((grDE->GetErrorYhigh(i)/yMeanDE[i]) * yCoor[i], 2);
//            yErrDown[i] += pow((grDE->GetErrorYlow(i)/yMeanDE[i]) * yCoor[i], 2);
//        }
//
//        if (lepSel == "") {
//            grDMu->GetPoint(i, xMeanDMu[i], yMeanDMu[i]);
//            grDE->GetPoint(i, xMeanDE[i], yMeanDE[i]);
//            yErrUp[i] += pow(((grDMu->GetErrorYhigh(i) + grDE->GetErrorYhigh(i)) / (yMeanDMu[i] + yMeanDE[i])) * yCoor[i], 2);
//            yErrDown[i] += pow(((grDMu->GetErrorYhigh(i) + grDE->GetErrorYhigh(i)) / (yMeanDMu[i] + yMeanDE[i])) * yCoor[i], 2);
//        }
//
//    }
//
//    TGraphAsymmErrors *grPDFSyst = new TGraphAsymmErrors(nPoints, xCoor, yCoor, xErr, xErr, yErrDown, yErrUp);
//    delete [] xCoor; delete [] yCoor; delete [] xErr; delete [] yErrDown; delete [] yErrUp;
//    delete [] xMeanDMu; delete [] yMeanDMu; delete [] xMeanDE; delete [] yMeanDE;
//
//    if (lepSel == "DE" || lepSel == "") {
//        fDE->Close();
//    }
//    if (lepSel == "DMu" || lepSel == "") {
//        fDMu->Close();
//    }
//
//    return grPDFSyst;
//}

void customizeGenGraph(TH1 *hSyst, TGraphAsymmErrors *gen, TGraphAsymmErrors *gScale, TGraphAsymmErrors *gPDF, int genNum, TString yTitle, int numbOfGenerator, TLegend *legend)
{

    double minRatioY = cfg.getD("minRatioYUnf", 0.2);
    double maxRatioY = cfg.getD("maxRatioYUnf", 1.8);

    if(hSyst){
	hSyst->GetYaxis()->SetRangeUser(minRatioY, maxRatioY);
	hSyst->GetYaxis()->SetNdivisions(507);
	hSyst->GetYaxis()->SetLabelSize(0.15);
	hSyst->GetYaxis()->SetTitle(yTitle);
	hSyst->GetYaxis()->SetTitleSize(0.14);
	hSyst->GetYaxis()->SetTitleOffset(0.45);
	hSyst->GetYaxis()->CenterTitle();
	//hSyst->SetTitle();
    }
    if(gen){
	gen->SetFillColor(ZJetsFillColor[genNum-1]);
	gen->SetFillStyle(ZJetsFillStyle);
	gen->SetLineColor(ZJetsLineColor[genNum-1]);
	gen->SetLineWidth(2);
	gen->SetMarkerColor(ZJetsLineColor[genNum-1]);
	gen->SetMarkerStyle(ZJetsMarkerStyle[genNum-1]);
    }

    if(gScale){
	gScale->SetFillStyle(ZJetsFillStyle);
	gScale->SetLineColor(ZJetsLineColor[genNum-1]);
	gScale->SetLineWidth(2);
	gScale->SetFillColor(ZJetsScaleFillColor[genNum-1]);
    }
    
    if(gPDF){
	gPDF->SetFillStyle(0);
	gPDF->SetLineColor(ZJetsLineColor[genNum-1]);
	gPDF->SetLineWidth(2);
    }
    
    if (genNum == numbOfGenerator && hSyst) {
        hSyst->GetYaxis()->SetLabelSize(0.09);
        if (numbOfGenerator == 2) hSyst->GetYaxis()->SetLabelSize(0.105);
        if (numbOfGenerator == 3) hSyst->GetYaxis()->SetLabelSize(0.115);
        hSyst->GetYaxis()->SetTitleSize(0.08);
        if (numbOfGenerator == 2) hSyst->GetYaxis()->SetTitleSize(0.100);
        if (numbOfGenerator == 3) hSyst->GetYaxis()->SetTitleSize(0.10);
        hSyst->GetYaxis()->SetTitleOffset(0.8);
        if (numbOfGenerator == 2) hSyst->GetYaxis()->SetTitleOffset(0.63);
        if (numbOfGenerator == 3) hSyst->GetYaxis()->SetTitleOffset(0.60);
        hSyst->GetXaxis()->SetLabelSize(0.12);
        hSyst->GetXaxis()->SetTitleSize(0.12);
        hSyst->GetXaxis()->SetTitleOffset(1.0);
    }
    else if(hSyst){
        hSyst->GetXaxis()->SetTitle();
    }

    if (legend) {
        TLegendEntry *leEntry;
        TLegendEntry *statEntry;
        TLegendEntry *pdfEntry;
	//   if(/*genNum == 3 ||*/ genNum == 1) {
	legend->SetX2(0.64);
	legend->SetNColumns(3);
	//statEntry = legend->AddEntry(gen, "Stat", "f");
	TString l = "Stat.";
	if(!gScale && !gPDF) l += " unc.";
	statEntry = legend->AddEntry((TObject*)0, l, "f");
	statEntry->SetFillStyle(ZJetsFillStyle);
	statEntry->SetFillColor(ZJetsFillColor[genNum-1]);
	statEntry->SetLineColor(ZJetsFillColor[genNum-1]);

	if(gScale){
	    //leEntry = legend->AddEntry(gScale, "#oplus Theory", "f");
	    TString l = "#oplus theo.";
	    if(!gPDF) l += " unc.";
	    leEntry = legend->AddEntry((TObject*)0, l, "f");
	    leEntry->SetFillColor(ZJetsScaleFillColor[genNum-1]);
	    leEntry->SetFillStyle(ZJetsFillStyle);
	    leEntry->SetLineColor(ZJetsScaleFillColor[genNum-1]);
	}

	if(gPDF){
            pdfEntry = legend->AddEntry(gPDF, "#oplus PDF #oplus #alpha_{s} unc.", "f");
            pdfEntry->SetFillStyle(0);
	}
	//        }
	//        else {
	//            //leEntry = legend->AddEntry(gen, "Stat. unc.", "f");
	//            leEntry = legend->AddEntry((TObject*)0, "Stat. unc.", "f");
	//            leEntry->SetFillColor(ZJetsFillColor[genNum-1]);
	//            leEntry->SetFillStyle(ZJetsFillStyle);
	//            leEntry->SetLineColor(ZJetsFillColor[genNum-1]);
	//        }
    }
}


void customizeGenGraph(TH1 *hSyst, TGraphAsymmErrors *gen, TGraphAsymmErrors *gPDF, int genNum, TString yTitle, int numbOfGenerator, TLegend *legend)
{
    if(hSyst){
	hSyst->GetYaxis()->SetRangeUser(0.2, 1.8);
	hSyst->GetYaxis()->SetNdivisions(507);
	hSyst->GetYaxis()->SetLabelSize(0.15);
	hSyst->GetYaxis()->SetTitle(yTitle);
	hSyst->GetYaxis()->SetTitleSize(0.14);
	hSyst->GetYaxis()->SetTitleOffset(0.45);
	hSyst->GetYaxis()->CenterTitle();
	//hSyst->SetTitle();
    }

    if(gen){
	gen->SetFillColor(ZJetsFillColor[genNum-1]);
	gen->SetFillStyle(ZJetsFillStyle);
	gen->SetLineColor(ZJetsLineColor[genNum-1]);
	gen->SetLineWidth(2);
	gen->SetMarkerColor(ZJetsLineColor[genNum-1]);
	gen->SetMarkerStyle(ZJetsMarkerStyle[genNum-1]);
    }

    if(gPDF){
	gPDF->SetFillStyle(0);
	gPDF->SetLineColor(ZJetsLineColor[genNum-1]);
	gPDF->SetLineWidth(2);
    }
    
    if (genNum == numbOfGenerator && hSyst) {
        hSyst->GetYaxis()->SetLabelSize(0.09);
        if (numbOfGenerator == 2) hSyst->GetYaxis()->SetLabelSize(0.105);
        if (numbOfGenerator == 3) hSyst->GetYaxis()->SetLabelSize(0.115);
        hSyst->GetYaxis()->SetTitleSize(0.08);
        if (numbOfGenerator == 2) hSyst->GetYaxis()->SetTitleSize(0.100);
        if (numbOfGenerator == 3) hSyst->GetYaxis()->SetTitleSize(0.10);
        hSyst->GetYaxis()->SetTitleOffset(0.8);
        if (numbOfGenerator == 2) hSyst->GetYaxis()->SetTitleOffset(0.63);
        if (numbOfGenerator == 3) hSyst->GetYaxis()->SetTitleOffset(0.60);
	hSyst->GetXaxis()->SetLabelSize(0.12);
        hSyst->GetXaxis()->SetTitleSize(0.12);
        hSyst->GetXaxis()->SetTitleOffset(1.0);
    }
    else if(hSyst){
        hSyst->GetXaxis()->SetTitle();
    }

    if (legend) {
        TLegendEntry *leEntry;
        //leEntry = legend->AddEntry(gen, "Stat. unc.", "f");
        leEntry = legend->AddEntry((TObject*)0, "Stat. unc.", "f");
        leEntry->SetFillColor(ZJetsFillColor[genNum-1]);
        leEntry->SetFillStyle(ZJetsFillStyle);
        leEntry->SetLineColor(ZJetsFillColor[genNum-1]);
    }
}

void customizeGenHist(TH1 *gen, int genNumb, TLegend *legend, TString legText)
{

    //--- Customize gen Sherpa ---
    gen->SetFillColor(ZJetsFillColor[genNumb-1]);
    gen->SetFillStyle(ZJetsFillStyle);
    gen->SetLineColor(ZJetsLineColor[genNumb-1]);
    gen->SetLineWidth(2);
    gen->SetMarkerColor(ZJetsMarkerColor[genNumb-1]);
    gen->SetMarkerStyle(ZJetsMarkerStyle[genNumb-1]);
    TLegendEntry *le = legend->AddEntry(gen, legText, "pefl");
    le->SetFillColor(ZJetsFillColor[genNumb-1]);
    le->SetFillStyle(ZJetsFillStyle);
    le->SetLineColor(ZJetsLineColor[genNumb-1]);
    le->SetMarkerColor(ZJetsMarkerColor[genNumb-1]);
    le->SetMarkerStyle(ZJetsMarkerStyle[genNumb-1]);
}


void configYaxis(TH1 *grCentralSyst, TH1 *gen1, TH1 *gen2, TH1 *gen3)
{
    //--- Configure Y axis of the plot ---
    double minimumToPlot = std::numeric_limits<double>::max();
    if(grCentralSyst) minimumToPlot = TMath::Min(minimumToPlot, grCentralSyst->GetMinimum());
    if (gen1) minimumToPlot = TMath::Min(minimumToPlot, gen1->GetBinContent(gen1->GetMinimumBin()));
    if (gen2) minimumToPlot = TMath::Min(minimumToPlot, gen2->GetBinContent(gen2->GetMinimumBin()));
    if (gen3) minimumToPlot = TMath::Min(minimumToPlot, gen3->GetBinContent(gen3->GetMinimumBin()));

    double maximumToPlot = -std::numeric_limits<double>::max();
    if(grCentralSyst) maximumToPlot = TMath::Max(maximumToPlot, grCentralSyst->GetMaximum());
    if (gen1) maximumToPlot = TMath::Max(maximumToPlot, gen1->GetBinContent(gen1->GetMaximumBin()));
    if (gen2) maximumToPlot = TMath::Max(maximumToPlot, gen2->GetBinContent(gen2->GetMaximumBin()));
    if (gen3) maximumToPlot = TMath::Max(maximumToPlot, gen3->GetBinContent(gen3->GetMaximumBin()));

    if(grCentralSyst) grCentralSyst->GetYaxis()->SetRangeUser(0.2*minimumToPlot, 5*maximumToPlot);
    //if (TString(grCentralSyst->GetName()).Contains("Eta")) {
    //    grCentralSyst->GetYaxis()->SetRangeUser(0.001, 1.4*maximumToPlot);
    //}
}

//void configXaxis(TGraphAsymmErrors *grCentralSyst, TH1D *gen1)
void configXaxis(TH1 *grCentralSyst, TH1 *gen1, TString variable)
{
    //--- Configure X axis of the plot ---
    //double minX, tmp;
    //double maxX;
    //TString variable = gen1->GetName();
    //grCentralSyst->GetPoint(firstBin, minX, tmp);
    //grCentralSyst->GetPoint(grCentralSyst->GetN()-1, maxX, tmp);
    //minX -= grCentralSyst->GetErrorXlow(firstBin); 
    //maxX += grCentralSyst->GetErrorXhigh(grCentralSyst->GetN()-1);
    if (variable.Index("ZNGoodJets_Zexc") >= 0) {
	std::cout << __FILE__ << ":" << __LINE__ 
		  << ". Range of ZNGoodJets_Zexc x-axis is being modified.!\n";
	//grCentralSyst->GetXaxis()->Set(maxX-minX, minX, maxX);
	//	grCentralSyst->GetXaxis()->SetRangeUser(-0.5, 4.5);
        grCentralSyst->GetXaxis()->SetBinLabel(1, "= 0");
        grCentralSyst->GetXaxis()->SetBinLabel(2, "= 1");
        grCentralSyst->GetXaxis()->SetBinLabel(3, "= 2");
        grCentralSyst->GetXaxis()->SetBinLabel(4, "= 3");
        grCentralSyst->GetXaxis()->SetBinLabel(5, "= 4");
        grCentralSyst->GetXaxis()->SetBinLabel(6, "= 5");
	grCentralSyst->GetXaxis()->SetBinLabel(7, "= 6");
        //grCentralSyst->GetXaxis()->SetBinLabel(8, "= 7");
	//     grCentralSyst->GetXaxis()->SetBinLabel(9, "= 8");
        grCentralSyst->GetXaxis()->SetLabelSize(0.18);
        grCentralSyst->GetXaxis()->SetLabelOffset(0.01);
    }
    else if (variable.Index("ZNGoodJets_Zinc") >= 0) {
	std::cout << __FILE__ << ":" << __LINE__ 
		  << ". Range of ZNGoodJets_Zexc x-axis is being modified.!\n";
	if(grCentralSyst){
	    //	grCentralSyst->GetXaxis()->SetRangeUser(-0.5, 4.5);
	    //grCentralSyst->GetXaxis()->Set(maxX-minX, minX, maxX);
	    grCentralSyst->GetXaxis()->SetBinLabel(1, "#geq 0");
	    grCentralSyst->GetXaxis()->SetBinLabel(2, "#geq 1");
	    grCentralSyst->GetXaxis()->SetBinLabel(3, "#geq 2");
	    grCentralSyst->GetXaxis()->SetBinLabel(4, "#geq 3");
	    grCentralSyst->GetXaxis()->SetBinLabel(5, "#geq 4");
	    grCentralSyst->GetXaxis()->SetBinLabel(6, "#geq 5");
	    grCentralSyst->GetXaxis()->SetBinLabel(7, "#geq 6");
	    //        grCentralSyst->GetXaxis()->SetBinLabel(8, "#geq 7");
	    grCentralSyst->GetXaxis()->SetLabelSize(0.18);
	    grCentralSyst->GetXaxis()->SetLabelOffset(0.01);
	}
    }
    //grCentralSyst->GetXaxis()->SetRangeUser(minX, maxX);
    TString xtitle = gen1->GetXaxis()->GetTitle();
    if (xtitle.Index("^{gen}") >= 0) xtitle = xtitle.ReplaceAll("^{gen}","");
    if (xtitle.Index("H_{T}") >= 0) {
        TString njets;
        if (variable.Index("Zinc1jet") >= 0) njets = "1";
        else if (variable.Index("Zinc2jet") >= 0) njets = "2";
        else if (variable.Index("Zinc3jet") >= 0) njets = "3";
        else if (variable.Index("Zinc4jet") >= 0) njets = "4";
        else if (variable.Index("Zinc5jet") >= 0) njets = "5";
        else if (variable.Index("Zinc6jet") >= 0) njets = "6";
        else if (variable.Index("Zinc7jet") >= 0) njets = "7";
        else if (variable.Index("Zinc8jet") >= 0) njets = "8";
        xtitle = "H_{T}, N_{jets} #geq " + njets + " [GeV]";
    }
    if(grCentralSyst) grCentralSyst->GetXaxis()->SetTitle(xtitle);
    if(grCentralSyst) grCentralSyst->GetXaxis()->SetTitleSize(0.12);
    //-----------------------------------------

}

std::string getYaxisTitle(bool doNormalized, const TH1 *gen1)
{
    std::string title = "";
    std::string xtitle = gen1->GetXaxis()->GetTitle();
    std::string shortVar = xtitle.substr(0, xtitle.find(" "));
    std::string unit = "";
    if (xtitle.find("^{gen}") != std::string::npos) {
        xtitle.replace(xtitle.find("^{gen}"),6,"");
    }
    if (xtitle.find("[") != std::string::npos){
        size_t begin = xtitle.find("[") + 1;
        unit = xtitle.substr(begin);
        unit = unit.substr(0, unit.find("]"));
    }
    title = "d#sigma/d" + shortVar;
    if (doNormalized) {
        title = "1/#sigma " + title;
    }
    else {
        title += "  [pb";
        if (unit != "" ) title += "/" + unit;
        title += "]";
    }
    return title;
}


void createTitleVariableAnddSigma(TString variable, bool doNormalized, TString xtitle, TString& title, TString& var, TString& dSigma)
{

    // jet multiplicity
    if (variable.Index("ZNGoodJets_Zexc") >= 0) {
        title = "Exclusive jet multiplicity";
        var = "$N_{\\text{jets}}$";
        dSigma = "$\\frac{d\\sigma}{dN_{\\text{jets}}}$ \\tiny{[\\text{pb}]}";
    }
    if (variable.Index("ZNGoodJets_Zinc") >= 0) {
        title = "Inclusive jet multiplicity";
        var = "$N_{\\text{jets}}$";
        dSigma = "$\\frac{d\\sigma}{dN_{\\text{jets}}}$ \\tiny{[\\text{pb}]}";
    }

    if (xtitle.Index("p_{T}(Z)") >= 0) {
        title = "$p_{T}^{Z}$";
        var = "$p_{\\text{T}}(Z)$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dp_{\\text{T}}(Z)}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }

    if (xtitle.Index("p_{T} balance") >= 0) {
        title = "$p_{T}^{balance}$";
        var = "$p_{\\text{T}}(balance)$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dp_{\\text{T}}(balance)}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }

    if (xtitle.Index("Recoil") >= 0) {
        title = "$Hadronic recoil$";
        var = "Hadronic recoil \\tiny{[GeV]}";//"$p_{\\text{T}}(balance)$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dp_{\\text{T}}(hadronic recoil)}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }

    if (xtitle.Index("JZB") >= 0) {
        title = "$p_{T}^{JZB}$";
        var = "$p_{\\text{T}}(JZB)$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dp_{\\text{T}}(JZB)}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }

    // Z Boson and Jet rapidity
    if (variable.Index("AbsZRapidity_Zexc1jet") >= 0) {
        title = "$|y_\\text{Z}|$ ($N_{\\text{jets}} = 1$)";
        var = "$|y_{\\text{Z}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{Z}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsZRapidity_Zinc1jet") >= 0) {
        title = "$|y_\\text{Z}|$ ($N_{\\text{jets}} \\geq 1$)";
        var = "$|y_{\\text{Z}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{Z}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsJetRapidity_Zexc1jet") >= 0) {
        title = "$|y_\\text{jet}|$ ($N_{\\text{jets}} = 1$)";
        var = "$|y_{\\text{jet}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{jet}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsFirstJetRapidity_Zinc1jet") >= 0) {
        title = "$|y_\\text{jet1}|$ ($N_{\\text{jets}} \\geq 1$)";
        var = "$|y_{\\text{jet1}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{jet1}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZJetRapidity_Zexc1jet") >= 0) {
        title = "$y_\\text{sum}$ ($N_{\\text{jets}} = 1$)";
        var = "$y_{\\text{sum}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZFirstJetRapidity_Zinc1jet") >= 0) {
        title = "$y_\\text{sum(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1$)";
        var = "$y_{\\text{sum(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZJetRapidity_Zexc1jet") >= 0) {
        title = "$y_\\text{diff}$ ($N_{\\text{jets}} = 1$)";
        var = "$y_{\\text{diff}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZFirstJetRapidity_Zinc1jet") >= 0) {
        title = "$y_\\text{diff(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1$)";
        var = "$y_{\\text{diff(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsZRapidity_Zinc2jet") >= 0) {
        title = "$|y_\\text{Z}|$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$|y_{\\text{Z}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{Z}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsFirstJetRapidity_Zinc2jet") >= 0) {
        title = "$|y_\\text{jet1}|$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$|y_{\\text{jet1}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{jet1}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsSecondJetRapidity_Zinc2jet") >= 0) {
        title = "$|y_\\text{jet2}|$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$|y_{\\text{jet2}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{jet2}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZFirstJetRapidity_Zinc2jet") >= 0) {
        title = "$y_\\text{sum(Z,jet1)}$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$y_{\\text{sum(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZSecondJetRapidity_Zinc2jet") >= 0) {
        title = "$y_\\text{sum(Z,jet2)}$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$y_{\\text{sum(Z,jet2)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum(Z,jet2)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumFirstSecondJetRapidity_Zinc2jet") >= 0) {
        title = "$y_\\text{sum(jet1,jet2)}$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$y_{\\text{sum(jet1,jet2)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum(jet1,jet2)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZFirstJetRapidity_Zinc2jet") >= 0) {
        title = "$y_\\text{diff(Z,jet1)}$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$y_{\\text{diff(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZSecondJetRapidity_Zinc2jet") >= 0) {
        title = "$y_\\text{diff(Z,jet2)}$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$y_{\\text{diff(Z,jet2)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff(Z,jet2)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifFirstSecondJetRapidity_Zinc2jet") >= 0) {
        title = "$y_\\text{diff(jet1,jet2)}$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$y_{\\text{diff(jet1,jet2)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff(jet1,jet2)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZTwoJetsRapidity_Zinc2jet") >= 0) {
        title = "$y_\\text{sum(Z,jet1+jet2)}$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$y_{\\text{sum(Z,jet1+jet2)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum(Z,jet1+jet2)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZTwoJetsRapidity_Zinc2jet") >= 0) {
        title = "$y_\\text{diff(Z,jet1+jet2)}$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$y_{\\text{diff(Z,jet1+jet2)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff(Z,jet1+jet2)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsZRapidity_ZPt150_Zinc1jet") >= 0) {
        title = "$|y_\\text{Z}|$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$|y_{\\text{Z}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{Z}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsZRapidity_ZPt300_Zinc1jet") >= 0) {
        title = "$|y_\\text{Z}|$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$|y_{\\text{Z}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{Z}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsFirstJetRapidity_ZPt150_Zinc1jet") >= 0) {
        title = "$|y_\\text{jet1}|$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$|y_{\\text{jet1}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{jet1}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsFirstJetRapidity_ZPt300_Zinc1jet") >= 0) {
        title = "$|y_\\text{jet1}|$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$|y_{\\text{jet1}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{jet1}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZFirstJetRapidity_ZPt150_Zinc1jet") >= 0) {
        title = "$y_\\text{sum(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$y_{\\text{sum(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZFirstJetRapidity_ZPt300_Zinc1jet") >= 0) {
        title = "$y_\\text{sum(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$y_{\\text{sum(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZFirstJetRapidity_ZPt150_Zinc1jet") >= 0) {
        title = "$y_\\text{diff(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$y_{\\text{diff(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZFirstJetRapidity_ZPt300_Zinc1jet") >= 0) {
        title = "$y_\\text{diff(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$y_{\\text{diff(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZFirstJetRapidity_FirstJetPt50_Zinc1jet") >= 0) {
        title = "$y_\\text{sum(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{jet1}} \\geq 50 \\text{GeV}$)";
        var = "$y_{\\text{sum(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZFirstJetRapidity_FirstJetPt80_Zinc1jet") >= 0) {
        title = "$y_\\text{sum(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{jet1}} \\geq 80 \\text{GeV}$)";
        var = "$y_{\\text{sum(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZFirstJetRapidity_FirstJetPt50_Zinc1jet") >= 0) {
        title = "$y_\\text{diff(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{jet1}} \\geq 50 \\text{GeV}$)";
        var = "$y_{\\text{diff(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZFirstJetRapidity_FirstJetPt80_Zinc1jet") >= 0) {
        title = "$y_\\text{diff(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{jet1}} \\geq 80 \\text{GeV}$)";
        var = "$y_{\\text{diff(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsZRapidity_DifJetRapiditys2_Zinc2jet") >= 0) {
        title = "$|y_\\text{Z}|$ ($N_{\\text{jets}} \\geq 2, |y_{jet1} - y_{jet2}| \\leq 2 $)";
        var = "$|y_{\\text{Z}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{Z}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsZRapidity_DifJetRapidityl2_Zinc2jet") >= 0) {
        title = "$|y_\\text{Z}|$ ($N_{\\text{jets}} \\geq 2, |y_{jet1} - y_{jet2}| \\geq 2 $)";
        var = "$|y_{\\text{Z}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{Z}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsFirstJetRapidity_DifJetRapiditys2_Zinc2jet") >= 0) {
        title = "$|y_\\text{jet1}|$ ($N_{\\text{jets}} \\geq 2, |y_{jet1} - y_{jet2}| \\leq 2 $)";
        var = "$|y_{\\text{jet1}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{jet1}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("AbsFirstJetRapidity_DifJetRapidityl2_Zinc2jet") >= 0) {
        title = "$|y_\\text{jet1}|$ ($N_{\\text{jets}} \\geq 2, |y_{jet1} - y_{jet2}| \\geq 2 $)";
        var = "$|y_{\\text{jet1}}|$";
        dSigma = "$\\frac{d\\sigma}{d|y_{\\text{jet1}}|}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZFirstJetRapidity_DifJetRapiditys2_Zinc2jet") >= 0) {
        title = "$y_\\text{sum(Z,jet1)}$ ($N_{\\text{jets}} \\geq 2, |y_{jet1} - y_{jet2}| \\leq 2 $)";
        var = "$y_{\\text{sum(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZFirstJetRapidity_DifJetRapidityl2_Zinc2jet") >= 0) {
        title = "$y_\\text{sum(Z,jet1)}$ ($N_{\\text{jets}} \\geq 2, |y_{jet1} - y_{jet2}| \\geq 2 $)";
        var = "$y_{\\text{sum(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{sum(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZFirstJetRapidity_DifJetRapiditys2_Zinc2jet") >= 0) {
        title = "$y_\\text{diff(Z,jet1)}$ ($N_{\\text{jets}} \\geq 2, |y_{jet1} - y_{jet2}| \\leq 2 $)";
        var = "$y_{\\text{diff(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZFirstJetRapidity_DifJetRapidityl2_Zinc2jet") >= 0) {
        title = "$y_\\text{diff(Z,jet1)}$ ($N_{\\text{jets}} \\geq 2, |y_{jet1} - y_{jet2}| \\geq 2 $)";
        var = "$y_{\\text{diff(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{dy_{\\text{diff(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("SumZFirstJetEta_Zinc1jet") >= 0) {
        title = "$\\eta_\\text{sum(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1 $)";
        var = "$\\eta_{\\text{sum(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{d\\eta_{\\text{sum(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DifZFirstJetEta_Zinc1jet") >= 0) {
        title = "$\\eta_\\text{diff(Z,jet1)}$ ($N_{\\text{jets}} \\geq 1 $)";
        var = "$\\eta_{\\text{diff(Z,jet1)}}$";
        dSigma = "$\\frac{d\\sigma}{d\\eta_{\\text{diff(Z,jet1)}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZFirstJet_Zinc1jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet1}$ ($N_{\\text{jets}} \\geq 1 $)";
        var = "$\\Delta\\phi_{\\text{Z,jet1}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet1}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZFirstJet_Zinc2jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet1}$ ($N_{\\text{jets}} \\geq 2 $)";
        var = "$\\Delta\\phi_{\\text{Z,jet1}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet1}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZFirstJet_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet1}$ ($N_{\\text{jets}} \\geq 3 $)";
        var = "$\\Delta\\phi_{\\text{Z,jet1}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet1}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZFirstJet_ZPt150_Zinc1jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet1}$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet1}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet1}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZFirstJet_ZPt150_Zinc2jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet1}$ ($N_{\\text{jets}} \\geq 2, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet1}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet1}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZFirstJet_ZPt150_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet1}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet1}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet1}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZFirstJet_ZPt300_Zinc1jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet1}$ ($N_{\\text{jets}} \\geq 1, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet1}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet1}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZFirstJet_ZPt300_Zinc2jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet1}$ ($N_{\\text{jets}} \\geq 2, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet1}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet1}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZFirstJet_ZPt300_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet1}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet1}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet1}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZSecondJet_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet2}$ ($N_{\\text{jets}} \\geq 3 $)";
        var = "$\\Delta\\phi_{\\text{Z,jet2}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet2}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZThirdJet_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet3}$ ($N_{\\text{jets}} \\geq 3 $)";
        var = "$\\Delta\\phi_{\\text{Z,jet3}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet3}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZSecondJet_ZPt150_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet2}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet2}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet2}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZThirdJet_ZPt150_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet3}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet3}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet3}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZSecondJet_ZPt300_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet2}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet2}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet2}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZThirdJet_ZPt300_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet3}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet3}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet3}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiFirstSecondJet_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{jet1,jet2}$ ($N_{\\text{jets}} \\geq 3 $)";
        var = "$\\Delta\\phi_{\\text{jet1,jet2}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{jet1,jet2}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiFirstThirdJet_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{jet1,jet3}$ ($N_{\\text{jets}} \\geq 3 $)";
        var = "$\\Delta\\phi_{\\text{jet1,jet3}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{jet1,jet3}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiSecondThirdJet_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{jet2,jet3}$ ($N_{\\text{jets}} \\geq 3 $)";
        var = "$\\Delta\\phi_{\\text{jet2,jet3}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{jet2,jet3}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiFirstSecondJet_ZPt150_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{jet1,jet2}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{jet1,jet2}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{jet1,jet2}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiFirstThirdJet_ZPt150_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{jet1,jet3}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{jet1,jet3}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{jet1,jet3}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiSecondThirdJet_ZPt150_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{jet2,jet3}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{jet2,jet3}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{jet2,jet3}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiFirstSecondJet_ZPt300_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{jet1,jet2}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{jet1,jet2}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{jet1,jet2}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiFirstThirdJet_ZPt300_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{jet1,jet3}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{jet1,jet3}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{jet1,jet3}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiSecondThirdJet_ZPt300_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{jet2,jet3}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 300 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{jet2,jet3}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{jet2,jet3}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZFirstJet_ZPt150_HT300_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet1}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}, H_{\\text{T}} \\geq 300 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet1}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet1}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZSecondJet_ZPt150_HT300_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet2}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}, H_{\\text{T}} \\geq 300 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet2}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet2}}}$ \\tiny{[\\text{pb}]}";
    }

    if (variable.Index("DPhiZThirdJet_ZPt150_HT300_Zinc3jet") >= 0) {
        title = "$\\Delta\\phi_\\text{Z,jet3}$ ($N_{\\text{jets}} \\geq 3, p_{\\text{T}}^{\\text{Z}} \\geq 150 \\text{GeV}, H_{\\text{T}} \\geq 300 \\text{GeV}$)";
        var = "$\\Delta\\phi_{\\text{Z,jet3}}$";
        dSigma = "$\\frac{d\\sigma}{d\\Delta\\phi_{\\text{Z,jet3}}}$ \\tiny{[\\text{pb}]}";
    }

    // jet pt distributions
    if (xtitle.Index("p_{T}(j_{1})") >= 0) {
        title = "$1^{\\text{st}}$ jet $p_{\\text{T}}$ ($N_{\\text{jets}} \\geq 1$)";
        var = "$p_{\\text{T}}(j_{1})$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dp_{\\text{T}}(j_{1})}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }
    if (xtitle.Index("p_{T}(j_{2})") >= 0) {
        title = "$2^{\\text{nd}}$ jet $p_{\\text{T}}$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$p_{\\text{T}}(j_{2})$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dp_{\\text{T}}(j_{2})}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }
    if (xtitle.Index("p_{T}(j_{3})") >= 0) {
        title = "$3^{\\text{rd}}$ jet $p_{\\text{T}}$ ($N_{\\text{jets}} \\geq 3$)";
        var = "$p_{\\text{T}}(j_{3})$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dp_{\\text{T}}(j_{3})}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }
    if (xtitle.Index("p_{T}(j_{4})") >= 0) {
        title = "$4^{\\text{th}}$ jet $p_{\\text{T}}$ ($N_{\\text{jets}} \\geq 4$)";
        var = "$p_{\\text{T}}(j_{4})$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dp_{\\text{T}}(j_{4})}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }
    if (xtitle.Index("p_{T}(j_{5})") >= 0) {
        title = "$5^{\\text{th}}$ jet $p_{\\text{T}}$ ($N_{\\text{jets}} \\geq 5$)";
        var = "$p_{\\text{T}}(j_{5})$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dp_{\\text{T}}(j_{5})}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }
    if (xtitle.Index("p_{T}(j_{6})") >= 0) {
        title = "$6^{\\text{th}}$ jet $p_{\\text{T}}$ ($N_{\\text{jets}} \\geq 6$)";
        var = "$p_{\\text{T}}(j_{6})$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dp_{\\text{T}}(j_{6})}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }

    // jet HT distributions
    if (xtitle.Index("H_{T}") >= 0 && title.Index("N_{jets} #geq 1") >= 0) {
        title = "$H_{\\text{T}}$ ($N_{\\text{jets}} \\geq 1$)";
        var = "$H_{\\text{T}}$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dH_{\\text{T}}}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }
    if (xtitle.Index("H_{T}") >= 0 && title.Index("N_{jets} #geq 2") >= 0) {
        title = "$H_{\\text{T}}$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$H_{\\text{T}}$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dH_{\\text{T}}}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }
    if (xtitle.Index("H_{T}") >= 0 && title.Index("N_{jets} #geq 3") >= 0) {
        title = "$H_{\\text{T}}$ ($N_{\\text{jets}} \\geq 3$)";
        var = "$H_{\\text{T}}$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dH_{\\text{T}}}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }
    if (xtitle.Index("H_{T}") >= 0 && title.Index("N_{jets} #geq 4") >= 0) {
        title = "$H_{\\text{T}}$ ($N_{\\text{jets}} \\geq 4$)";
        var = "$H_{\\text{T}}$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dH_{\\text{T}}}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }
    if (xtitle.Index("H_{T}") >= 0 && title.Index("N_{jets} #geq 5") >= 0) {
        title = "$H_{\\text{T}}$ ($N_{\\text{jets}} \\geq 5$)";
        var = "$H_{\\text{T}}$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dH_{\\text{T}}}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }
    if (xtitle.Index("H_{T}") >= 0 && title.Index("N_{jets} #geq 6") >= 0) {
        title = "$H_{\\text{T}}$ ($N_{\\text{jets}} \\geq 6$)";
        var = "$H_{\\text{T}}$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dH_{\\text{T}}}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }

    // jet eta distributions
    if (xtitle.Index("eta(j_{1})") >= 0) {
        title = "$1^{\\text{st}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 1$)";
        var = "$\\eta(j_{1})$";
        dSigma = "$\\frac{d\\sigma}{d\\eta(j_{1})}$ \\tiny{[\\text{pb}]}";
    }
    if (xtitle.Index("eta(j_{2})") >= 0) {
        title = "$2^{\\text{nd}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$\\eta(j_{2})$";
        dSigma = "$\\frac{d\\sigma}{d\\eta(j_{2})}$ \\tiny{[\\text{pb}]}";
    }
    if (xtitle.Index("eta(j_{3})") >= 0) {
        title = "$3^{\\text{rd}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 3$)";
        var = "$\\eta(j_{3})$";
        dSigma = "$\\frac{d\\sigma}{d\\eta(j_{3})}$ \\tiny{[\\text{pb}]}";
    }
    if (xtitle.Index("eta(j_{4})") >= 0) {
        title = "$4^{\\text{th}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 4$)";
        var = "$\\eta(j_{4})$";
        dSigma = "$\\frac{d\\sigma}{d\\eta(j_{4})}$ \\tiny{[\\text{pb}]}";
    }
    if (xtitle.Index("eta(j_{5})") >= 0) {
        title = "$5^{\\text{th}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 5$)";
        var = "$\\eta(j_{5})$";
        dSigma = "$\\frac{d\\sigma}{d\\eta(j_{5})}$ \\tiny{[\\text{pb}]}";
    }
    if (xtitle.Index("eta(j_{6})") >= 0) {
        title = "$6^{\\text{th}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 6$)";
        var = "$\\eta(j_{6})$";
        dSigma = "$\\frac{d\\sigma}{d\\eta(j_{6})}$ \\tiny{[\\text{pb}]}";
    }

    //abs rapidity distributions
    if (xtitle.Index("|y(j_{1})|") >= 0) {
        title = "$1^{\\text{st}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 1$)";
        var = "$|y(j_{1})|$";
        dSigma = "$\\frac{d\\sigma}{d|y(j_{1})|}$ \\tiny{[\\text{pb}]}";
    }
    if (xtitle.Index("|y(j_{2})|") >= 0) {
        title = "$2^{\\text{nd}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$|y(j_{2})|$";
        dSigma = "$\\frac{d\\sigma}{d|y(j_{2})|}$ \\tiny{[\\text{pb}]}";
    }
    if (xtitle.Index("|y(j_{3})|") >= 0) {
        title = "$3^{\\text{rd}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 3$)";
        var = "$|y(j_{3})|$";
        dSigma = "$\\frac{d\\sigma}{d|y(j_{3})|}$ \\tiny{[\\text{pb}]}";
    }
    if (xtitle.Index("|y(j_{4})|") >= 0) {
        title = "$4^{\\text{th}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 4$)";
        var = "$|y(j_{4})|$";
        dSigma = "$\\frac{d\\sigma}{d|y(j_{4})|}$ \\tiny{[\\text{pb}]}";
    }
    if (xtitle.Index("|y(j_{5})|") >= 0) {
        title = "$5^{\\text{th}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 5$)";
        var = "$|y(j_{5})|$";
        dSigma = "$\\frac{d\\sigma}{d|y(j_{5})|}$ \\tiny{[\\text{pb}]}";
    }
    if (xtitle.Index("|y(j_{6})|") >= 0) {
        title = "$6^{\\text{th}}$ jet $\\vert\\eta\\vert$ ($N_{\\text{jets}} \\geq 6$)";
        var = "$|y(j_{6})|$";
        dSigma = "$\\frac{d\\sigma}{d|y(j_{6})|}$ \\tiny{[\\text{pb}]}";
    }

    
    // dijet mass distribution
    if (xtitle.Index("M_{j_{1}j_{2}}") >= 0) {
        title = "dijet mass $M_{jj}$ ($N_{\\text{jets}} \\geq 2$)";
        var = "$M_{jj}$ \\tiny{[GeV]}";
        dSigma = "$\\frac{d\\sigma}{dM_{jj}}$ ${\\scriptstyle [\\frac{\\text{pb}}{\\text{GeV}}]}$";
    }

    if (doNormalized) {
        dSigma = "$\\frac{1}{\\sigma}$ " + dSigma;
        dSigma.ReplaceAll("\\frac{\\text{pb}}", "\\frac{1}");
        dSigma.ReplaceAll("\\tiny{\\left[\\text{pb}\\right]}", "");
    }

}


void makeCrossSectionPlot(const char* variable, const char* ref){
    if(!variable){
	for(unsigned i = 0 ; i < NVAROFINTERESTZJETS; ++i){
	    const char* v = VAROFINTERESTZJETS[i].name.Data();
	    if(v) makeCrossSectionPlot(v, ref);
	}
    }

  
    std::string lepSel = cfg.getS("lepSel");
    std::string unfoldDir = cfg.getS("unfoldDir");
    std::string algo = cfg.getS("algo");
    bool doNormalized = false;
    int jetPtMin = cfg.getI("jetPtMin");
    int jetEtaMax = cfg.getI("jetEtaMax");

    std::vector<std::string> predictions = cfg.getVS("predictions");

    TString unfCfgFile = cfg.getS("unfConf");
    static SectionedConfig unfCfg;
    unfCfg.read(unfCfgFile, true);
  
    TString section = TString::Format("%s_%s", lepSel.c_str(), variable);
    int nFirstBinsToSkip = unfCfg.get(section.Data(), "nFirstBinsToSkip", 0);
    int nLastBinsToSkip = unfCfg.get(section.Data(), "nLastBinsToSkip", 0);
  
    TString path = getUnfoldedFileName(unfoldDir, lepSel, variable, algo,
				       jetPtMin, jetEtaMax, "_MGPYTHIA6_", doNormalized);

    Ssiz_t p = path.Index(TRegexp("[^/]*$"));
    TString dir = path(0, p-1);
    TString fname = path(p, path.Length());

    TH1* hRef = 0;
    TH2* hCov = 0;
    
    double lumi = 0;

    if(TString(ref).CompareTo("data", TString::kIgnoreCase) == 0){
	TFile* fdata = TFile::Open(path + ".root");
	if(fdata && fdata->IsZombie()){
	    delete fdata;
	    fdata = 0;
	}
	
	if(!fdata){
	    std::cerr << "Fatal error. Failed to open unfolded data file "
		      << (path + ".root") << "\n";
	    abort();
	}
	
	hRef = (TH1*) fdata->Get("UnfDataCentral");
	hRef->SetZTitle("Data");
	
	if(!hRef){
	    std::cerr << "Fatal error. Histogram UnfDataCentral  was not found in file "
		      << fdata->GetName() << "\n";
	    abort();
	}
	
	hCov = (TH2*) fdata->Get("CovTotSyst");
	if(!hCov){
	    std::cerr << "Fatal error. Histogram CovTotSyst  was not found in file "
		      << fdata->GetName() << "\n";
	    abort();
	}
	hRef->SetDirectory(0);
	hCov->SetDirectory(0);

	 TH1* Lumi = 0;
	 if(fdata) fdata->GetObject("Lumi", Lumi);
	 if(Lumi) lumi = Lumi->GetBinContent(1);
	 else {
	     cerr << "Error: Lumi histogram was not found.\n";
	     return;
	 }
	delete fdata;
    } else{
	hRef = getGenHistos(std::vector<std::string>(1, ref), lepSel.c_str(), variable, true, true)[0];
	hRef->SetZTitle(getLegendGen(ref));
    }


    TCanvas *crossSectionPlot = makeCrossSectionPlot(lepSel, lumi, variable, doNormalized,
						     hRef, hCov,
						     predictions, nFirstBinsToSkip, nLastBinsToSkip);
    saveCanvas(crossSectionPlot, dir, fname);
    delete hRef;
    delete hCov;
}

TH1* makeCrossSectionHist(TH1* hGenDYJets, double integratedLumi)
{
    TH1 *hGenCrossSection = (TH1*) hGenDYJets->Clone();
    //--- divide by luminosity ---
    hGenCrossSection->Scale(1./integratedLumi);

    int nBins = hGenCrossSection->GetNbinsX();
    for (int i = 1; i <= nBins; ++i) {
	double binWidth = hGenCrossSection->GetBinWidth(i);
	hGenCrossSection->SetBinContent(i, hGenCrossSection->GetBinContent(i)*1./binWidth);
	hGenCrossSection->SetBinError(i, hGenCrossSection->GetBinError(i)*1./binWidth);
    }

    return hGenCrossSection;
}


//======


TCanvas* makeCrossSectionPlot(TString lepSel, double lumi, TString variable, bool doNormalized,
			      TH1* hStat, TH2* hCovSyst,
			      std::vector<std::string> gens,
			      int nFirstBinsToSkip, int nLastBinsToSkip){

    if(gens.size() > 3){
	std::cerr << "Warning. Maxium three generator comparison is supported. Only the first three will be considered."
		  << " (" << __FILE__ << ":" << __LINE__ << ").\n";
	gens.resize(3);
    }
  
    //gStyle->SetOptStat(0);

    bool isPrel = cfg.getB("preliminaryTag", true);    
    std::vector<TH1*> hGens = getGenHistos(gens, lepSel, variable);
    std::vector<TH1*> tmp1;
    std::vector<std::string> tmp2;
    
    int ipred = -1;
    for(auto g: hGens){
	++ipred;
	if(!g) continue;
	if(g) g->SetZTitle(getLegendGen(gens[ipred].c_str()));
	if(TString(gens[ipred]).BeginsWith("DYJets_GE")){
	    g->Scale(2.);
	}
	double fac = cfg.getF(TString("scale_") + gens[ipred], 1.);
	if(fac != 1.){
	    std::cout << "Scaling " << gens[ipred] << " by factor " << fac << std::endl;
	    g->Scale(fac);
	}
	tmp1.push_back(g);
	tmp2.push_back(gens[ipred]);
    }

    //drops missing predictions (we have ZjNNLO for only few distributions):
    hGens = tmp1;
    gens = tmp2;

    int numbOfGenerator = gens.size(); 
    
    //    hGens.resize(3, 0);

    //--- TGraph for data central value and stat. unc.:
    TGraphAsymmErrors *grCentralStat = createGrFromHist(hStat);
    grCentralStat->SetName("gr" + variable + "CentralStatError");
    TGraphAsymmErrors *grCentralStatRatio = createRatioGraph(grCentralStat);
    
    TGraphAsymmErrors *grCentralSyst = 0;
    TGraphAsymmErrors *grCentralSystRatio = 0;
    TH1* hSyst = (TH1*) hStat->Clone("hSyst");
    if(hCovSyst){
	int nBins = hSyst->GetNbinsX();
	for (int i = 1; i <= nBins; ++i) {
	    hSyst->SetBinError(i, sqrt(pow(hStat->GetBinError(i), 2) + hCovSyst->GetBinContent(i, i)));
	}
	grCentralSyst = createGrFromHist(hSyst);
	grCentralSystRatio = createRatioGraph(grCentralSyst);
	grCentralSyst->SetName("gr" + variable + "CentralTotError"); 
    }
  
    std::vector<TGraphAsymmErrors*> grGen1ToCentral(hGens.size(), 0);
    std::vector<TGraphAsymmErrors*> grGen1ScaleSyst(hGens.size(), 0);
    std::vector<TGraphAsymmErrors*> grGen1PDFSyst(hGens.size(), 0);

    int igen = -1;
    for(auto h: hGens){
	++igen;
	grGen1ToCentral[igen] = createGenToCentral(h, grCentralStat);
	int showSys = cfg.getI(TString::Format("%s_unc", gens[igen].c_str()));
	if(showSys == 1){
	    grGen1ScaleSyst[igen] = createScaleSystGraph(gens[igen], lepSel, variable, grGen1ToCentral[igen]);
	    grGen1PDFSyst[igen]   = createPDFSystGraph(gens[igen], lepSel, variable, grGen1ToCentral[igen], grGen1ScaleSyst[igen]);
	    
	} else if(showSys == 2){
	    grGen1ScaleSyst[igen] = createNNLOScaleSystGraph(lepSel, variable, grGen1ToCentral[igen]);
	}
    }

    //--- Main Canvas ---
    //double maximum = hGen1->GetMaximum();
    //double minimum = hGen1->GetMinimum();
    double maximum = hStat->GetMaximum();
    double minimum = hStat->GetMinimum();
    TString canvasName = "canvas" + variable;
    TCanvas *plots = new TCanvas(canvasName, hStat->GetTitle(), 600, 800);
    //-------------------

    //--- First Pad ---
    plots->cd();
    TPad *plot1 = new TPad("plot1", "plot1", 0., 0., 0., 0.);
    setAndDrawTPad(canvasName, plot1, 1, numbOfGenerator);

    //--- TLegend ---
    TLegend *legend = new TLegend(0.7, 0.74, 0.99, 0.98);
    customizeLegend(legend, numbOfGenerator);
    //------------------

    if(grCentralSyst) customizeCentral(grCentralSyst, legend, hStat->GetZaxis()->GetTitle());
    customizeCentral(grCentralStat, false);
    if(grCentralSystRatio) customizeCentral(grCentralSystRatio, true);
    if(grCentralSystRatio) customizeCentral(grCentralStatRatio, true);
    if(hSyst){
	hSyst->SetLineColor(kWhite);
	hSyst->SetMarkerColor(kWhite);
	hSyst->SetTitle("");
	hSyst->GetXaxis()->SetLabelSize(0);
	hSyst->GetYaxis()->SetTitle("");
	hSyst->GetYaxis()->SetLabelSize(0.055);
    }

    if(hSyst){
	if (canvasName.Contains("ZNGoodJets")) {
	    hSyst->GetXaxis()->SetRangeUser(-0.5, hSyst->GetXaxis()->GetXmax());
	}
	//if (canvasName.Contains("JetPt_Zinc")) {
	//hSyst->GetXaxis()->SetRangeUser(30, hSyst->GetXaxis()->GetXmax());
	//}
	hSyst->GetXaxis()->SetRange(nFirstBinsToSkip + 1, hSyst->GetNbinsX() - nLastBinsToSkip);
	if (canvasName.Contains("Eta") || canvasName.Contains("AbsRapidity")) {
	    hSyst->GetYaxis()->SetRangeUser(0.001, 1.4*maximum);
	}
	if (canvasName.Contains("DPhi")) {
	    hSyst->GetYaxis()->SetRangeUser(0.2*minimum, 1.5*maximum);
	}
	if (canvasName.Contains("Vis")) {
	    hSyst->GetYaxis()->SetRangeUser(0.2*minimum, 1.3*maximum);
	}
	hSyst->SetStats(0);
	hSyst->DrawCopy("e");
	grCentralSyst->SetName("grCentralSyst");
	grCentralSyst->Draw("2");
    }

    igen = -1;
    for(auto hGen: hGens){
	++igen;
	if(!hGen) continue;
	configXaxis(hSyst, hGen, variable);
	//configYaxis(hSyst, hGen1, hGen2, hGen3);
	customizeGenHist(hGen, igen + 1, legend, TString::Format("%s", hGen->GetZaxis()->GetTitle()));
	//hGen1->SetName("hGen1");
    	hGen->SetStats(0);
	hGen->DrawCopy("ESAME");
    }

    grCentralStat->SetName("grCentralStat");
    grCentralStat->Draw("p");

    legend->SetName("mainLegend");
    legend->Draw("same");

    fixYscale(1.2, 1.1);

    //--- TLatex stuff ---
    TLatex *latexLabel = new TLatex(); 
    latexLabel->SetNDC();
    latexLabel->SetTextSize(0.035);
    if (gens.size() == 2) latexLabel->SetTextSize(0.035);
    if (gens.size() >= 3) latexLabel->SetTextSize(0.05);
    latexLabel->SetTextFont(42);
    latexLabel->SetLineWidth(2);

    latexLabel->SetTextFont(61);
    if(TString(hStat->GetZaxis()->GetTitle()).BeginsWith("data", TString::kIgnoreCase)
       || TString(hStat->GetZaxis()->GetTitle()).BeginsWith("meas", TString::kIgnoreCase)){
	latexLabel->DrawLatex(0.13,0.95,"CMS");	
	latexLabel->SetTextFont(52);
	if(isPrel) latexLabel->DrawLatex(0.20,0.95,"Preliminary");
	latexLabel->SetTextFont(42);
	//FIXME: integrated lumi must be read from data histo file
	//latexLabel->DrawLatex(0.13,0.95-0.045, "2.25 fb^{-1} (13 TeV)");
	if(lumi > 0) latexLabel->DrawLatex(0.13,0.95-0.045, TString::Format("%.3g fb^{-1} (13 TeV)", lumi/1000.));
    } else{
	latexLabel->DrawLatex(0.13,0.95,"MC study");	
	latexLabel->SetTextFont(52);
	if(isPrel) latexLabel->DrawLatex(0.13,0.95-0.045,"Preliminary");
	latexLabel->SetTextFont(42);
    }

    //    latexLabel->DrawLatex(0.18,0.21-0.05,"anti-k_{T} (R = 0.4) Jets");


    double xlabel;
    double ylabel;

    if (canvasName.Contains("Vis")){
	xlabel = 0.44;
	ylabel = 0.75;
	//	latexLabel->DrawLatex(0.44,0.7,"anti-k_{T} (R = 0.4) Jets");
    } else if (canvasName.Contains("JZB") &&  !canvasName.Contains("JZB_ptLow")){
	xlabel = 0.4;
	ylabel = 0.21;
	//	latexLabel->DrawLatex(0.4,0.21-0.05,"anti-k_{T} (R = 0.4) Jets");
    } else{
	xlabel = 0.18;
	ylabel = 0.21;
	//	latexLabel->DrawLatex(0.18,0.21-0.05,"anti-k_{T} (R = 0.4) Jets");
    }
    latexLabel->DrawLatex(xlabel, ylabel - 0.05, "anti-k_{T} (R = 0.4) Jets");

    if (canvasName.Contains("FirstJetPt50")){
        latexLabel->DrawLatex(xlabel,ylabel-0.11,"p_{T}^{jet} > 50 GeV, |y^{jet}| < 2.4 ");
    }else if (canvasName.Contains("FirstJetPt80")){
        latexLabel->DrawLatex(xlabel,ylabel-0.11,"p_{T}^{jet} > 80 GeV, |y^{jet}| < 2.4 ");
    } else if (canvasName.Contains("ZPt150")){
        latexLabel->DrawLatex(xlabel,ylabel-0.11,"p_{T}^{Z} > 150 GeV, p_{T}^{jet} > 30 GeV, |y^{jet}| < 2.4 ");
    } else if (canvasName.Contains("ZPt300")){
        latexLabel->DrawLatex(xlabel,ylabel-0.11,"p_{T}^{Z} > 300 GeV, p_{T}^{jet} > 30 GeV, |y^{jet}| < 2.4 ");
    } else if (canvasName.Contains("DifJetRapidityl2")){
        latexLabel->DrawLatex(xlabel,ylabel-0.11,"p_{T}^{jet} > 30 GeV, |y^{jet}| < 2.4, |y_{jet1}-y_{jet2}| > 2 ");
    } else if (canvasName.Contains("DifJetRapiditys2")){
        latexLabel->DrawLatex(xlabel,ylabel-0.11,"p_{T}^{jet} > 30 GeV, |y^{jet}| < 2.4, |y_{jet1}-y_{jet2}| < 2 ");
    } else if (canvasName.Contains("ZPt150_HT300")){
        latexLabel->DrawLatex(xlabel,ylabel-0.11,"p_{T}^{Z} > 150 GeV, p_{T}^{jet} > 30 GeV, |y^{jet}| < 2.4, H_{T}^{jet} > 300 GeV ");
    } else if (canvasName.Contains("Vis")){
	latexLabel->DrawLatex(xlabel, 0.7-0.06,"p_{T}^{#mu} > 20 GeV, p_{T}^{jet} > 30 GeV, |#eta^{jet}| < 2.4 ");
    } else{
        latexLabel->DrawLatex(xlabel, ylabel - 0.11,"p_{T}^{jet} > 30 GeV, |y^{jet}| < 2.4 ");
    }

    if (lepSel == "") latexLabel->DrawLatex(xlabel,ylabel-0.17,"Z/#gamma*#rightarrow ll channel");
    else if (lepSel == "DMu") latexLabel->DrawLatex(xlabel,ylabel-0.17,"Z/#gamma*#rightarrow #mu#mu channel");
    else if (lepSel == "DE") latexLabel->DrawLatex(xlabel,ylabel-0.17,"Z/#gamma*#rightarrow ee channel");
    latexLabel->SetName("latexLabel");
    latexLabel->Draw("same");

    TLatex *ytitle = new TLatex();
    ytitle->SetName("ytitle");
    ytitle->SetTextSize(0.04);
    if (gens.size() == 2) ytitle->SetTextSize(0.05);
    if (gens.size() == 3) ytitle->SetTextSize(0.06);
    ytitle->SetTextFont(42);
    ytitle->SetLineWidth(2);
    ytitle->SetTextColor(kBlack);
    ytitle->SetNDC();
    ytitle->SetTextAlign(33);
    ytitle->SetTextAngle(90);
    //TODO: check if we can use hStat anytime
    //    std::string strYtitle = hGen1 ?  getYaxisTitle(doNormalized, hGen1) :  getYaxisTitle(doNormalized, hStat);
    std::string strYtitle = getYaxisTitle(doNormalized, hStat);
    if (strYtitle.find("eta") != std::string::npos) {
        size_t first = strYtitle.find("#eta");
        std::string tmp1 = strYtitle.substr(0, first);
        std::string tmp2 = strYtitle.substr(first);
        strYtitle = tmp1 + "|" + tmp2;
        size_t second = strYtitle.find(")");
        tmp1 = strYtitle.substr(0, second+1);
        tmp2 = strYtitle.substr(second+1);
        strYtitle = tmp1 + "|" + tmp2;
    }
    ytitle->DrawLatex(0.008,0.91,strYtitle.c_str());
    //--- End Of first Pad ---

    //--- Ratio Pads ---
    igen = -1;
    for(auto hGen: hGens){
	++igen;
	int ipad = igen + 2;
	plots->cd();
	TString padName = TString::Format("plot%d", ipad);
	TPad *pad = new TPad(padName, padName, 0., 0., 0., 0.);
	setAndDrawTPad(canvasName, pad, ipad, numbOfGenerator);

	//--- TLegend ---
	TLegend *legend = new TLegend(0.16, 0.05, 0.42, 0.20);
	customizeLegend(legend, 1 + igen, numbOfGenerator);
	TString generator = hGen->GetZaxis()->GetTitle();
	generator = generator(0, generator.Index(" "));
	TString ref_shortname = hStat->GetZaxis()->GetTitle();
	ref_shortname = ref_shortname(0, ref_shortname.Index(" "));
	if(ref_shortname.Length()==0) ref_shortname = "Measurement";
	customizeGenGraph(hSyst, grGen1ToCentral[igen], grGen1ScaleSyst[igen], grGen1PDFSyst[igen], igen + 1,
			  //TString("#frac{") + generator + "}{" + ref_shortname + "}", numbOfGenerator, legend);
			  TString::Format("#frac{Prediction}{%s}", ref_shortname.Data()), numbOfGenerator, legend);
									    
	configXaxis(hSyst, hGen, variable);
	hSyst->DrawCopy("e");
	if(grGen1ToCentral[igen]){
	    grGen1ToCentral[igen]->SetName("grGen1ToCentral");
	    grGen1ToCentral[igen]->Draw("2");
	}
	if(grGen1ScaleSyst[igen]) grGen1ScaleSyst[igen]->Draw("2");
	if(grGen1ToCentral[igen]) grGen1ToCentral[igen]->Draw("2");
	if(grCentralSystRatio){
	    grCentralSystRatio->SetName("grCentralSystRatio");
	    grCentralSystRatio->Draw("2");
	}
	
	if(grCentralSystRatio) grCentralStatRatio->Draw("p");
	if(grGen1ToCentral[igen])    grGen1ToCentral[igen]->Draw("Xp");
	if(grGen1PDFSyst[igen])      grGen1PDFSyst[igen]->Draw("2");
	legend->Draw("same");
	
	//if (canvasName.Contains("JetPt_Zinc")) {
	//    grGen1ToCentral->GetXaxis()->SetRangeUser(30, x + ex);
	//}
	
	pad->RedrawAxis();
    }
    //--- End of Ratio Pads ---
    
    plots->Update();
    return plots;
}

