#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"

void EMU_SYSTEMATICS(TString era="2016preAPV", TString channel="ee")
{
    TString filename_nominal    = "";
    TString filename_plus       = "";
    TString filename_minus      = "";
    TString filename_fakesplus  = "";
    TString filename_fakesminus = "";
    channel.ToUpper();
    TString Channel = "EE";
    if (channel == "MUMU") Channel = "MuMu";
    
    filename_nominal    = Channel+"_"+era+"_REWEIGHTED";
    filename_plus       = Channel+"_"+era+"_REWEIGHTED_plus";
    filename_minus      = Channel+"_"+era+"_REWEIGHTED_minus";
    filename_fakesplus  = Channel+"_"+era+"_REWEIGHTED_fakesPlus";
    filename_fakesminus = Channel+"_"+era+"_REWEIGHTED_fakesMinus";

    TFile *f_in_nominal    = new TFile(filename_nominal    + "/TOP_BKGS/dyjets-top.root", "READ");
    TFile *f_in_plus       = new TFile(filename_plus       + "/plots/dyjets-top.root", "READ");
    TFile *f_in_minus      = new TFile(filename_minus      + "/plots/dyjets-top.root", "READ");
    TFile *f_in_fakesplus  = new TFile(filename_fakesplus  + "/plots/dyjets-top.root", "READ");
    TFile *f_in_fakesminus = new TFile(filename_fakesminus + "/plots/dyjets-top.root", "READ");

    TH1D *h_nominal    = (TH1D*)(f_in_nominal   ->Get("mass_wide_range_inc0jet"));
    TH1D *h_plus       = (TH1D*)(f_in_plus      ->Get("mass_wide_range_inc0jet"));
    TH1D *h_minus      = (TH1D*)(f_in_minus     ->Get("mass_wide_range_inc0jet"));
    TH1D *h_fakesplus  = (TH1D*)(f_in_fakesplus ->Get("mass_wide_range_inc0jet"));
    TH1D *h_fakesminus = (TH1D*)(f_in_fakesminus->Get("mass_wide_range_inc0jet"));

    h_nominal   ->SetDirectory(0);
    h_plus      ->SetDirectory(0);
    h_minus     ->SetDirectory(0);
    h_fakesplus ->SetDirectory(0);
    h_fakesminus->SetDirectory(0);

    f_in_nominal   ->Close();
    f_in_plus      ->Close();
    f_in_minus     ->Close();
    f_in_fakesplus ->Close();
    f_in_fakesminus->Close();

    TH1D *h_syst_fit = (TH1D *)(h_nominal->Clone("h_syst_fit"));
    TH1D *h_syst_bkg = (TH1D *)(h_nominal->Clone("h_syst_bkg"));
    TH1D *h_syst_tot = (TH1D *)(h_nominal->Clone("h_syst_tot"));

    TH1D *h_rel_syst_fit = (TH1D*)(h_nominal->Clone("h_rel_syst_fit"));
    TH1D *h_rel_syst_bkg = (TH1D*)(h_nominal->Clone("h_rel_syst_bkg"));
    TH1D *h_rel_syst_tot = (TH1D*)(h_nominal->Clone("h_rel_syst_tot"));

    for (uint i = 1; i < h_nominal->GetNbinsX(); ++i)
    {
        double d1_fit = std::abs(h_plus ->GetBinContent(i) - h_nominal->GetBinContent(i));
        double d2_fit = std::abs(h_minus->GetBinContent(i) - h_nominal->GetBinContent(i));
        double d1_bkg = std::abs(h_fakesplus ->GetBinContent(i) - h_nominal->GetBinContent(i));
        double d2_bkg = std::abs(h_fakesminus->GetBinContent(i) - h_nominal->GetBinContent(i));

        double d_fit = d1_fit > d2_fit ? d1_fit : d2_fit;
        double d_bkg = d1_bkg > d2_bkg ? d1_bkg : d2_bkg;

        double d_tot = std::sqrt(d_fit * d_fit + d_bkg * d_bkg);

        h_syst_fit->SetBinContent(i, d_fit);
        h_syst_bkg->SetBinContent(i, d_bkg);
        h_syst_tot->SetBinContent(i, d_tot);
        h_syst_fit->SetBinError(i, 0);
        h_syst_bkg->SetBinError(i, 0);
        h_syst_tot->SetBinError(i, 0);

        h_rel_syst_fit->SetBinContent(i, d_fit/h_nominal->GetBinContent(i)*100);
        h_rel_syst_bkg->SetBinContent(i, d_bkg/h_nominal->GetBinContent(i)*100);
        h_rel_syst_tot->SetBinContent(i, d_tot/h_nominal->GetBinContent(i)*100);
        h_rel_syst_fit->SetBinError(i, 0);
        h_rel_syst_bkg->SetBinError(i, 0);
        h_rel_syst_tot->SetBinError(i, 0);
    }

    TCanvas *c_syst = new TCanvas("c_syst", "Plot w systematics", 800, 800);
    c_syst->SetLeftMargin(0.11);
    c_syst->SetRightMargin(0.05);
    c_syst->SetTopMargin(0.05);
    h_nominal->SetTitle("");
    if (Channel == "EE")
        h_nominal->GetXaxis()->SetTitle("m_{ee} [GeV]");
    else
        h_nominal->GetXaxis()->SetTitle("m_{#mu#mu} [GeV]");
    h_nominal->GetYaxis()->SetTitle("#events");
    h_nominal->GetYaxis()->SetTitleOffset(1.5);
    h_nominal->GetXaxis()->SetNoExponent();
    h_nominal->GetXaxis()->SetMoreLogLabels();
    h_nominal->SetFillColor(30);
    h_nominal->Draw("hist");
    // Nominal plus fake bkg error
    TH1D *h_syst_bkg_pluserr = (TH1D *)(h_nominal->Clone("h_syst_bkg_pluserr"));
    h_syst_bkg_pluserr->Add(h_syst_bkg);
    h_syst_bkg_pluserr->SetFillStyle(3144);
    h_syst_bkg_pluserr->SetFillColor(kCyan+3);
    h_syst_bkg_pluserr->Draw("samehist");
    // Nominal plus fit error
    TH1D *h_syst_fit_pluserr = (TH1D *)(h_nominal->Clone("h_syst_fit_pluserr"));
    h_syst_fit_pluserr->Add(h_syst_fit);
    h_syst_fit_pluserr->SetFillStyle(3144);
    h_syst_fit_pluserr->SetFillColor(kBlue-2);
    h_syst_fit_pluserr->Draw("samehist");
    // Nominal minus fit error
    TH1D *h_syst_fit_minuserr = (TH1D *)(h_nominal->Clone("h_syst_fit_minuserr"));
    h_syst_fit_minuserr->Add(h_syst_fit, -1);
    h_syst_fit_minuserr->SetFillStyle(3144);
    h_syst_fit_minuserr->SetFillColor(kCyan+3);
    h_syst_fit_minuserr->Draw("samehist");
    // Nominal minus fake bkg error
    TH1D *h_syst_bkg_minuserr = (TH1D *)(h_nominal->Clone("h_syst_bkg_minuserr"));
    h_syst_bkg_minuserr->Add(h_syst_bkg, -1);
    h_syst_bkg_minuserr->Draw("samehist");
    h_nominal->Draw("sameaxis");
    c_syst->SetLogx();
    c_syst->SetLogy();
    c_syst->SetGridx();
    c_syst->SetGridy();
    TLegend *leg = new TLegend(0.6, 0.8, 0.95, 0.95);
    leg->AddEntry(h_nominal, "t#bar{t}+ST (corr.)", "f");
    leg->AddEntry(h_syst_fit_pluserr, "e#mu method fit err.", "f");
    leg->AddEntry(h_syst_bkg_pluserr, "e#mu fake bkg. err", "f");
    leg->Draw();
    c_syst->Update();
    c_syst->SaveAs(channel+"_top_syst_"+era+".pdf");

    TCanvas *c_rel_syst = new TCanvas("c_rel_syst", "Relative errors", 800, 800);
    c_rel_syst->SetLeftMargin(0.12);
    c_rel_syst->SetRightMargin(0.05);
    c_rel_syst->SetTopMargin(0.05);
    // Fit error
    if (Channel == "EE")
        h_rel_syst_fit->GetXaxis()->SetTitle("m_{ee} [GeV]");
    else
        h_rel_syst_fit->GetXaxis()->SetTitle("m_{#mu#mu} [GeV]");
    h_rel_syst_fit->GetXaxis()->SetNoExponent();
    h_rel_syst_fit->GetXaxis()->SetMoreLogLabels();
    h_rel_syst_fit->GetYaxis()->SetTitle("Top bkg. syst. err. [%]");    
    h_rel_syst_fit->GetYaxis()->SetTitleOffset(1.5);
    h_rel_syst_fit->SetFillStyle(0);
    h_rel_syst_fit->SetLineColor(kBlack);
    h_rel_syst_fit->SetMarkerColor(kBlack);
    h_rel_syst_fit->SetMarkerStyle(kFullDotLarge);
    h_rel_syst_fit->Draw("PL");
    // Fake bkg error
    h_rel_syst_bkg->SetFillStyle(0);
    h_rel_syst_bkg->SetLineColor(kBlue);
    h_rel_syst_bkg->SetMarkerColor(kBlue);
    h_rel_syst_bkg->SetMarkerStyle(kFullDotLarge);
    h_rel_syst_bkg->Draw("SAME PL");
    // Total error
    h_rel_syst_tot->SetFillStyle(0);
    h_rel_syst_tot->SetLineColor(kRed);
    h_rel_syst_tot->SetMarkerColor(kRed);
    h_rel_syst_tot->SetMarkerStyle(kFullDotLarge);
    h_rel_syst_tot->Draw("SAME PL");
    h_rel_syst_fit->SetTitle("");
    h_rel_syst_fit->GetYaxis()->SetRangeUser(0, 3.3);
    c_rel_syst->SetLogx();
    c_rel_syst->SetGridx();
    c_rel_syst->SetGridy();
    TLegend *leg_rel = new TLegend(0.55, 0.8, 0.95, 0.95);
    leg_rel->AddEntry(h_rel_syst_tot, "Total error", "lp");
    leg_rel->AddEntry(h_rel_syst_fit, "e#mu method fit error", "lp");
    leg_rel->AddEntry(h_rel_syst_bkg, "e#mu fake bkg. error", "lp");
    leg_rel->Draw();
    c_rel_syst->Update();
    c_rel_syst->SaveAs(Channel+"_top_syst_rel_"+era+".pdf");
}