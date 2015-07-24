#include <iostream>
#include <sstream>
#include <iomanip>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TLegend.h>
#include <TAxis.h>

void plotDistr()
{
    //TFile *f = new TFile("HistoFilesJetsMass/DE_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR_EffiCorr_0_TrigCorr_1_Syst_0_JetPtMin_30_VarWidth.root");
    TFile *f = new TFile("HistoFilesJetsMass/DE_8TeV_Data_dR_EffiCorr_1_TrigCorr_1_Syst_0_JetPtMin_30_VarWidth.root");
    TH1D* TruePU_0 = (TH1D*) f->Get("PU_0");
    TH1D* TruePU_1 = (TH1D*) f->Get("PU_1");
    TH1D* TruePU_2 = (TH1D*) f->Get("PU_2");
    TH1D* TruePU_3 = (TH1D*) f->Get("PU_3");
    TH1D* TruePU_4 = (TH1D*) f->Get("PU_4");
    TH1D* TruePU_5 = (TH1D*) f->Get("PU_5");
    TH1D* TruePU_6 = (TH1D*) f->Get("PU_6");
    TH1D* TruePU_7 = (TH1D*) f->Get("PU_7");

    TruePU_0->SetLineWidth(2);
    TruePU_1->SetLineWidth(2);
    TruePU_2->SetLineWidth(2);
    TruePU_3->SetLineWidth(2);
    TruePU_4->SetLineWidth(2);
    TruePU_5->SetLineWidth(2);
    TruePU_6->SetLineWidth(2);
    TruePU_7->SetLineWidth(2);

    TruePU_0->SetLineColor(kAzure);
    TruePU_1->SetLineColor(kAzure-9);
    TruePU_2->SetLineColor(kGreen+2);
    TruePU_3->SetLineColor(kGreen-6);
    TruePU_4->SetLineColor(kOrange);
    TruePU_5->SetLineColor(kOrange+4);
    TruePU_6->SetLineColor(kRed);
    TruePU_7->SetLineColor(kViolet);

    TruePU_0->SetMarkerColor(kAzure);
    TruePU_1->SetMarkerColor(kAzure-9);
    TruePU_2->SetMarkerColor(kGreen+2);
    TruePU_3->SetMarkerColor(kGreen-6);
    TruePU_4->SetMarkerColor(kOrange);
    TruePU_5->SetMarkerColor(kOrange+4);
    TruePU_6->SetMarkerColor(kRed);
    TruePU_7->SetMarkerColor(kViolet);


    TruePU_7->SetTitle("");
    TruePU_7->GetYaxis()->SetTitle("#Events/Total");
    TruePU_7->DrawNormalized("e");
    TruePU_6->DrawNormalized("esame");
    TruePU_5->DrawNormalized("esame");
    TruePU_4->DrawNormalized("esame");
    TruePU_3->DrawNormalized("esame");
    TruePU_2->DrawNormalized("esame");
    TruePU_1->DrawNormalized("esame");
    TruePU_0->DrawNormalized("esame");

    TLegend *leg = new TLegend(0.7, 0.7, 0.99, 0.96);
    leg->SetFillColor(0);
    leg->SetBorderSize(0);

    stringstream mean;
    mean.precision(4);
    mean << showpoint;
    mean.str(std::string()); mean << TruePU_0->GetMean();
    leg->AddEntry(TruePU_0, string("N_{jets} = 0;  #mu = " + mean.str()).c_str(), "lp");
    mean.str(std::string()); mean << TruePU_1->GetMean();
    leg->AddEntry(TruePU_1, string("N_{jets} = 1;  #mu = " + mean.str()).c_str(), "lp");
    mean.str(std::string()); mean << TruePU_2->GetMean();
    leg->AddEntry(TruePU_2, string("N_{jets} = 2;  #mu = " + mean.str()).c_str(), "lp");
    mean.str(std::string()); mean << TruePU_3->GetMean();
    leg->AddEntry(TruePU_3, string("N_{jets} = 3;  #mu = " + mean.str()).c_str(), "lp");
    mean.str(std::string()); mean << TruePU_4->GetMean();
    leg->AddEntry(TruePU_4, string("N_{jets} = 4;  #mu = " + mean.str()).c_str(), "lp");
    mean.str(std::string()); mean << TruePU_5->GetMean();
    leg->AddEntry(TruePU_5, string("N_{jets} = 5;  #mu = " + mean.str()).c_str(), "lp");
    mean.str(std::string()); mean << TruePU_6->GetMean();
    leg->AddEntry(TruePU_6, string("N_{jets} = 6;  #mu = " + mean.str()).c_str(), "lp");
    mean.str(std::string()); mean << TruePU_7->GetMean();
    leg->AddEntry(TruePU_7, string("N_{jets} = 7;  #mu = " + mean.str()).c_str(), "lp");
    leg->Draw();



    std::cout << TruePU_7->GetMean() << std::endl; 
    std::cout << TruePU_6->GetMean() << std::endl;
    std::cout << TruePU_5->GetMean() << std::endl;
    std::cout << TruePU_4->GetMean() << std::endl;
    std::cout << TruePU_3->GetMean() << std::endl;
    std::cout << TruePU_2->GetMean() << std::endl;
    std::cout << TruePU_1->GetMean() << std::endl;
    std::cout << TruePU_0->GetMean() << std::endl;

}
