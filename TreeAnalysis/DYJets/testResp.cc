#include <iostream>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <RooUnfoldResponse.h>

void normalizeTH2D(TH2D *h)
{
    int xbin(h->GetNbinsX()), ybin(h->GetNbinsY());
    for (int i(0); i <= xbin + 1; i++){
        double sum(0.);
        for (int j(0); j <= ybin + 1; j++){
            sum += h->GetBinContent(i,j);
        }
        for (int j(0); j <= ybin + 1; j++){
            if (sum > 0) h->SetBinContent(i, j, h->GetBinContent(i, j) / sum );
        }
    }
}

void testResp() {
    TFile *file = new TFile("HistoFilesJetsMass/DMu_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR_EffiCorr_0_TrigCorr_1_Syst_0_JetPtMin_30_VarWidth.root");
    RooUnfoldResponse *resp = (RooUnfoldResponse*) file->Get("responseJetsMass_Zinc2jet");
    TH2D *h_resp = (TH2D*) resp->Hresponse();
    normalizeTH2D(h_resp);
    h_resp->DrawCopy("colz");

    file->Close();
}

