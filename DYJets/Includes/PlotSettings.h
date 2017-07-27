#ifndef _PLOTSETTINGS_H_
#define _PLOTSETTINGS_H_

#include <iostream>
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TMath.h>
#include <TAxis.h>
#include <TLatex.h>

const int ZJetsFillColor[4] = {kBlue-10, kOrange-2, kGreen-10, kPink-2};
const int ZJetsScaleFillColor[4] = {kBlue-6, kOrange-5, kGreen-8, kPink-6};
const int ZJetsLineColor[4] = {kBlue, kOrange+10, kGreen+3, kPink+3};
const int ZJetsMarkerColor[4] = {ZJetsLineColor[0], ZJetsLineColor[1], ZJetsLineColor[2], ZJetsLineColor[3]};
const int ZJetsMarkerStyle[4] = {24, 25, 26, 27};
const int ZJetsFillStyle = 1001;


void setAndDrawTPad(TString canvasName, TPad *plot, int plotNumber, int numbOfGenerator);
std::string getYaxisTitle(bool doNormalized, const TH1 *gen1);
void customizeLegend(TLegend *legend, int numbOfGenerator);
void customizeLegend(TLegend *legend, int genNumb, int numbOfGenerator);
void configYaxis(TH1 *grCentralSyst, TH1 *gen1, TH1 *gen2 = NULL, TH1 *gen3 = NULL);
void configXaxis(TH1 *grCentralSyst, TH1 *gen1, TString variable);
void customizeCentral(TGraphAsymmErrors *grCentral, bool ratio);
void customizeCentral(TGraphAsymmErrors *grCentral, TLegend *legend, TString legText = "");
void customizeGenHist(TH1 *gen, int genNumb, TLegend *legend, TString legText);
void customizeRatioGraph(TH1 *hSyst, TGraphAsymmErrors *gen, TGraphAsymmErrors *gPDF, int genNumb, TString yTitle, int numbOfGenerator, TLegend *legend = NULL);
TGraphAsymmErrors* createGrFromHist(const TH1 *h);
TGraphAsymmErrors* createRatioGraph(const TGraphAsymmErrors* grCentral);
TGraphErrors* createRatioGraph(const TGraphErrors* grCentral);
TGraphAsymmErrors *createGenToCentral(const TH1 *gen, const TGraphAsymmErrors *grCentral);
TGraphAsymmErrors* createPDFSystGraph(TString sample, TString lepSel, TString variable,
				      const TGraphAsymmErrors *grGenToCentral,
				      const TGraphAsymmErrors *grGen3ScaleSyst =0);
TGraphAsymmErrors* createScaleSystGraph(TString sample, TString lepSel, TString variable,
					const TGraphAsymmErrors *grGenToCentral);
TCanvas* makeCrossSectionPlot(TString lepSel, double lumi, TString variable, bool doNormalized,
			      TH1* hStat, TH2* hCovSyst,
			      std::vector<std::string> gens,
			      int nFirstBinsToSkip, int nLastBinsToSkip);
//TCanvas* makeCrossSectionPlot(TString lepSel, TString variable, bool doNormalized, TH1 *hData, TH2D *hCovSyst, TH1 *hGen, TH1 *hGen1 = NULL, TH1 *hGen2 = NULL, double integratedLumi = -1);
void createTitleVariableAnddSigma(TString variable, bool doNormalized, TString xtitle, TString &title, TString &var, TString &dSigma) ;
void makeCrossSectionPlot(const char* variable = 0, const char* ref = "Data");
TH1* makeCrossSectionHist(TH1* hGenDYJets, double integratedLumi);
#endif
