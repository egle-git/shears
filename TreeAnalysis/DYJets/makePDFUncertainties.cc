#include <iostream>
#include <TH1D.h>
#include <TFile.h>
#include <TCanvas.h>

using namespace std;

void makePDFUncertainties() {

    TString varName = "genFirstJetPt_Zinc1jet";

    
    TFile *f[99];
    for (int i(0); i<99; ++i) {
        TString name;
        name.Form("HistoFilesMay/DMu_8TeV_DYJetsToLL_M-50_TuneCUETP8M1_8TeV-amcatnloFXFX-Bonzai_fixed_allWeights_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_NNPDF_%d.root", i);
        f[i] = new TFile(name);
    }

    
    TH1D *h[99];
    for (int i(0); i < 99; ++i) {
        h[i] = (TH1D*) f[i]->Get(varName);
    }

    for (int i(1); i < 99; ++i) {
        h[i]->Divide(h[0]);
    }

    int nBins(h[0]->GetNbinsX());
    TH1D *hErrorsUp = (TH1D*) h[0]->Clone("hErrorsUp");
    hErrorsUp->Reset();
    TH1D *hErrorsDown = (TH1D*) h[0]->Clone("hErrorsDown");
    hErrorsDown->Reset();
    for (int i(1); i <= nBins; ++i) {
        hErrorsUp->SetBinContent(i, 0);
        hErrorsDown->SetBinContent(i, 0);
        double errUp2 = 0;
        double errDown2 = 0;
        for (int j(1); j < 99; j+=2) {
            if (h[j]->GetBinContent(i) >= 1 && h[j+1]->GetBinContent(i) <= 1) {
                errUp2 += pow(h[j]->GetBinContent(i)-1, 2);
                errDown2 += pow(1-h[j+1]->GetBinContent(i), 2);
            }
            else if (h[j]->GetBinContent(i) <= 1 && h[j+1]->GetBinContent(i) >= 1) {
                errUp2 += pow(1-h[j]->GetBinContent(i), 2);
                errDown2 += pow(h[j+1]->GetBinContent(i)-1, 2);
            }
            else if (h[j]->GetBinContent(i) >= 1 && h[j+1]->GetBinContent(i) >= 1) {
                errUp2 += pow(max(h[j]->GetBinContent(i), h[j+1]->GetBinContent(i)) - 1, 2);
            }
            else if (h[j]->GetBinContent(i) <= 1 && h[j+1]->GetBinContent(i) <= 1) {
                errDown2 += pow(1-min(h[j]->GetBinContent(i), h[j+1]->GetBinContent(i)), 2);
            }

        }
        hErrorsUp->SetBinContent(i, sqrt(errUp2));
        hErrorsDown->SetBinContent(i, sqrt(errDown2));
    }

    TCanvas *c = new TCanvas("c", "c", 600, 800);
    c->cd();
    hErrorsUp->DrawCopy();
    hErrorsDown->DrawCopy("histsame");

    for (int i(0); i<99; ++i) f[i]->Close();
}
