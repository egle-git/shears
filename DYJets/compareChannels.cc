#include <TFile.h>
#include <TString.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TLegend.h>


void compareChannels()
{
    
    gStyle->SetOptStat(0);
    TString variable = "JetsHT_Zinc3jet";
    TFile *fmu = new TFile("UnfoldedApr/DMu_unfolded_" + variable + "_Bayes_JetPtMin_30_JetEtaMax_24_MGPYTHIA6_sherpa2_amcatnlo.root");
    TFile *fe = new TFile("UnfoldedApr/DE_unfolded_" + variable + "_Bayes_JetPtMin_30_JetEtaMax_24_MGPYTHIA6_sherpa2_amcatnlo.root");

    TH1D *hmu = (TH1D*) fmu->Get("UnfDataCentral");
    TH1D *he = (TH1D*) fe->Get("UnfDataCentral");
    TH1D *hebis = (TH1D*) he->Clone();
    hebis->Divide(hmu);

    TCanvas *c = new TCanvas(variable, variable, 600, 800);
    c->cd();
    TLegend *leg = new TLegend(0.7,0.7,0.9,0.8);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);

    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
    pad1->SetTopMargin(0.11);
    pad1->SetBottomMargin(0.);
    pad1->SetRightMargin(0.03);
    pad1->SetTicks();
    if (variable.Index("Eta") < 0) pad1->SetLogy();
    pad1->Draw();
    pad1->cd();
    hmu->SetName("hmu");
    hmu->SetTitle("");
    he->SetName("he");
    hmu->SetMarkerStyle(20);
    hmu->SetLineWidth(2);
    hmu->SetLineColor(kGreen+2);
    hmu->SetMarkerColor(kGreen+2);
    hmu->GetYaxis()->SetTitle("d#sigma");
    hmu->GetYaxis()->SetTitleSize(0.05);
    he->SetMarkerStyle(25);
    he->SetLineWidth(2);
    he->SetLineColor(kBlue);
    he->SetMarkerColor(kBlue);
    hmu->Draw("e");
    if (variable == "ZNGoodJets_Zexc") hmu->GetXaxis()->SetRangeUser(0.5, 7.5);
    else if (variable.Index("JetPt_Zinc") > 0) hmu->GetXaxis()->SetRangeUser(30, hmu->GetXaxis()->GetXmax());
    he->Draw("esame");
    leg->AddEntry("hmu","Muon","lep");
    leg->AddEntry("he","Electron","lep");
    leg->Draw();


    c->cd();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
    pad2->SetTopMargin(0.);
    pad2->SetBottomMargin(0.3);
    pad2->SetRightMargin(0.03);
    pad2->SetGridy();
    pad2->SetTicks();
    pad2->Draw();
    pad2->cd();

    hebis->SetTitle("");
    hebis->GetXaxis()->SetTitleSize(0.1);
    hebis->GetXaxis()->SetTitleOffset(1.2);
    hebis->GetXaxis()->SetLabelSize(0.1);
    hebis->GetXaxis()->SetLabelOffset(0.012);

    hebis->GetYaxis()->SetRangeUser(0.61, 1.39);
    hebis->GetYaxis()->SetLabelSize(0.08);
    hebis->GetYaxis()->SetLabelOffset(0.01);
    hebis->GetYaxis()->SetTitle("Electron / Muon");
    hebis->GetYaxis()->SetTitleSize(0.08);
    hebis->GetYaxis()->SetTitleOffset(0.6);
    hebis->GetYaxis()->CenterTitle();

    hebis->SetMarkerStyle(21);
    hebis->SetLineWidth(2);
    hebis->SetLineColor(kBlack);
    hebis->SetMarkerColor(kBlack);
    hebis->Draw("e");
    if (variable == "ZNGoodJets_Zexc") hebis->GetXaxis()->SetRangeUser(0.5, 7.5);
    else if (variable.Index("JetPt_Zinc") > 0) hebis->GetXaxis()->SetRangeUser(30, hmu->GetXaxis()->GetXmax());
    c->cd();
    c->Update();

    c->SaveAs("UnfoldingCheck/ChannelComparison_" + variable + ".pdf");
    c->SaveAs("UnfoldingCheck/ChannelComparison_" + variable + ".ps");
    c->SaveAs("UnfoldingCheck/ChannelComparison_" + variable + ".eps");
    c->SaveAs("UnfoldingCheck/ChannelComparison_" + variable + ".png");
    c->SaveAs("UnfoldingCheck/ChannelComparison_" + variable + ".root");

}
