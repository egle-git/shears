#ifndef _MAKEZJETSPOTS_H_
#define _MAKEZJETSPOTS_H_

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

TCanvas* makeZJetsPlots(TH1D *hStat, TH2D *hCovSyst, TH1D *gen1, TH1D *gen2 = NULL, TH1D *gen3 = NULL);
TCanvas* makeZJetsPlots(TH1D *hStat, TH1D *hSyst, TH1D *hPDFUp, TH1D *hPDFDown, TH1D *gen1, TH1D *gen2 = NULL, TH1D *gen3 = NULL);
TCanvas* makeZJetsPlots(TGraphAsymmErrors *grStat, TGraphAsymmErrors *grSyst, TH1D *hPDFUp, TH1D *hPDFDown, TH1D *gen1, TH1D *gen2 = NULL, TH1D *gen3 = NULL);
#endif
