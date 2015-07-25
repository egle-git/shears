#include <iostream>
#include <TFile.h>
#include <TH1.h>

void ratioGenToMad()
{
    TFile *mad = new TFile("HistoFiles/DMu_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR_EffiCorr_0_TrigCorr_1_Syst_0_JetPtMin_30_VarWidth.root");
    TFile *she = new TFile("HistoFiles/DMu_8TeV_DY_Sherpa_2NLO4_HepMC_dR_Full_ListALL_EffiCorr_0_TrigCorr_0_Syst_0_JetPtMin_30_VarWidth.root");
    TFile *pow = new TFile("HistoFiles/DMu_8TeV_DYJets_PowhegZ2jMiNLO_dR_GEN_CernMOJ_EffiCorr_0_TrigCorr_0_Syst_0_JetPtMin_30_VarWidth.root");

    string variable = "genFirstJetPt_2_Zinc1jet";

    TH1D *hmad = (TH1D*) mad->Get(variable.c_str());
    TH1D *hshe = (TH1D*) she->Get(variable.c_str());
    TH1D *hpow = (TH1D*) pow->Get(variable.c_str());


    hshe->Divide(hmad);
    hpow->Divide(hmad);

    hshe->SetLineColor(kBlue);
    hshe->SetMarkerColor(kBlue);

    hpow->SetLineColor(kGreen);
    hpow->SetMarkerColor(kGreen);

    hshe->DrawCopy();
    hpow->DrawCopy("same");

    mad->Close();
    she->Close();
    pow->Close();
}
