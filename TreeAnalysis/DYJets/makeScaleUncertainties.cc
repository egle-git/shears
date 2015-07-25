#include <iostream>
#include <TH1D.h>
#include <TFile.h>
#include <TCanvas.h>

using namespace std;

void makeScaleUncertainties() {

    TString varName = "genZNGoodJets_Zexc";

    
    TFile *f[7];
    f[0] = new TFile("HistoFilesApr/DMu_8TeV_DYJetsToLL_M-50_TuneCUETP8M1_8TeV-amcatnloFXFX-Bonzai_fixed_allWeights_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_muR_1_muF_1.root");
    f[1] = new TFile("HistoFilesApr/DMu_8TeV_DYJetsToLL_M-50_TuneCUETP8M1_8TeV-amcatnloFXFX-Bonzai_fixed_allWeights_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_muR_0.5_muF_0.5.root");
    f[2] = new TFile("HistoFilesApr/DMu_8TeV_DYJetsToLL_M-50_TuneCUETP8M1_8TeV-amcatnloFXFX-Bonzai_fixed_allWeights_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_muR_0.5_muF_1.root");
    f[3] = new TFile("HistoFilesApr/DMu_8TeV_DYJetsToLL_M-50_TuneCUETP8M1_8TeV-amcatnloFXFX-Bonzai_fixed_allWeights_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_muR_1_muF_0.5.root");
    f[4] = new TFile("HistoFilesApr/DMu_8TeV_DYJetsToLL_M-50_TuneCUETP8M1_8TeV-amcatnloFXFX-Bonzai_fixed_allWeights_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_muR_1_muF_2.root");
    f[5] = new TFile("HistoFilesApr/DMu_8TeV_DYJetsToLL_M-50_TuneCUETP8M1_8TeV-amcatnloFXFX-Bonzai_fixed_allWeights_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_muR_2_muF_1.root");
    f[6] = new TFile("HistoFilesApr/DMu_8TeV_DYJetsToLL_M-50_TuneCUETP8M1_8TeV-amcatnloFXFX-Bonzai_fixed_allWeights_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24_muR_2_muF_2.root");


    TH1D *h[7];
    for (int i(0); i < 7; ++i) {
        h[i] = (TH1D*) f[i]->Get(varName);
    }

    for (int i(1); i < 7; ++i) {
        h[i]->Divide(h[0]);
    }

    int nBins(h[0]->GetNbinsX());
    TH1D *hErrorsUp = (TH1D*) h[0]->Clone("hErrorsUp");
    hErrorsUp->Reset();
    TH1D *hErrorsDown = (TH1D*) h[0]->Clone("hErrorsDown");
    hErrorsDown->Reset();
    for (int i(1); i <= nBins; ++i) {
        hErrorsUp->SetBinContent(i, 1);
        hErrorsDown->SetBinContent(i, 1);
        for (int j(1); j < 7; ++j) {
            hErrorsUp->SetBinContent(i, max(hErrorsUp->GetBinContent(i), h[j]->GetBinContent(i)));
            hErrorsDown->SetBinContent(i, min(hErrorsDown->GetBinContent(i), h[j]->GetBinContent(i)));
            cout << "bin " << i << " h " << j << " " << h[j]->GetBinContent(i) << endl;
        }
    }
    
    TCanvas *c = new TCanvas("c", "c", 600, 800);
    c->cd();
    hErrorsUp->Draw();
    hErrorsDown->Draw("histsame");

}
