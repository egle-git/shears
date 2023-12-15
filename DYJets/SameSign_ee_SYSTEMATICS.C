#include "TFile.h"
#include "TH1D.h"
#include "TCanvas.h"

void SameSign_ee_SYSTEMATICS(TString era="2016")
{
    TString filename_nominal   = "";
    TString filename_fitplus0  = "";
    TString filename_fitminus0 = "";
    TString filename_fitplus1  = "";
    TString filename_fitminus1 = "";
    TString filename_fitplus2  = "";
    TString filename_fitminus2 = "";
    TString filename_fitplus3  = "";
    TString filename_fitminus3 = "";
    TString filename_misplus   = "";
    TString filename_misminus  = "";
    TString filename_interp3   = "";
    TString filename_ratiocon  = "";

    filename_nominal   = "EE_"+era+"_samesign_REWEIGHTED";
    filename_fitplus0  = "EE_"+era+"_samesign_REWEIGHTED_plus0";
    filename_fitminus0 = "EE_"+era+"_samesign_REWEIGHTED_minus0";
    filename_fitplus1  = "EE_"+era+"_samesign_REWEIGHTED_plus1";
    filename_fitminus1 = "EE_"+era+"_samesign_REWEIGHTED_minus1";
    filename_fitplus2  = "EE_"+era+"_samesign_REWEIGHTED_plus2";
    filename_fitminus2 = "EE_"+era+"_samesign_REWEIGHTED_minus2";
    filename_fitplus3  = "EE_"+era+"_samesign_REWEIGHTED_plus3";
    filename_fitminus3 = "EE_"+era+"_samesign_REWEIGHTED_minus3";
    filename_misplus   = "EE_"+era+"_samesign_REWEIGHTED_misidPlus";
    filename_misminus  = "EE_"+era+"_samesign_REWEIGHTED_misidMinus";
    filename_interp3   = "EE_"+era+"_samesign_REWEIGHTED";
    filename_ratiocon  = "EE_"+era+"_samesign_NOTreweighted";

    TFile *f_in_nominal   = new TFile(filename_nominal   + "/FAKE_EST/dyjets-FakesInt.root", "READ");
    TFile *f_in_fitplus0  = new TFile(filename_fitplus0  + "/FAKE_EST/dyjets-FakesInt.root", "READ");
    TFile *f_in_fitminus0 = new TFile(filename_fitminus0 + "/FAKE_EST/dyjets-FakesInt.root", "READ");
    TFile *f_in_fitplus1  = new TFile(filename_fitplus1  + "/FAKE_EST/dyjets-FakesInt.root", "READ");
    TFile *f_in_fitminus1 = new TFile(filename_fitminus1 + "/FAKE_EST/dyjets-FakesInt.root", "READ");
    TFile *f_in_fitplus2  = new TFile(filename_fitplus2  + "/FAKE_EST/dyjets-FakesInt.root", "READ");
    TFile *f_in_fitminus2 = new TFile(filename_fitminus2 + "/FAKE_EST/dyjets-FakesInt.root", "READ");
    TFile *f_in_fitplus3  = new TFile(filename_fitplus3  + "/FAKE_EST/dyjets-FakesInt.root", "READ");
    TFile *f_in_fitminus3 = new TFile(filename_fitminus3 + "/FAKE_EST/dyjets-FakesInt.root", "READ");
    TFile *f_in_misplus   = new TFile(filename_misplus   + "/FAKE_EST/dyjets-FakesInt.root", "READ");
    TFile *f_in_misminus  = new TFile(filename_misminus  + "/FAKE_EST/dyjets-FakesInt.root", "READ");
    TFile *f_in_interp3   = new TFile(filename_interp3   + "/FAKE_EST/dyjets-FakesInt_alt.root", "READ");
    TFile *f_in_ratiocon  = new TFile(filename_ratiocon  + "/FAKE_EST/dyjets-FakesInt.root", "READ");

    TH1D *h_nominal   = (TH1D*)(f_in_nominal  ->Get("mass_wide_range_inc0jet"));
    TH1D *h_fitplus0  = (TH1D*)(f_in_fitplus0 ->Get("mass_wide_range_inc0jet"));
    TH1D *h_fitminus0 = (TH1D*)(f_in_fitminus0->Get("mass_wide_range_inc0jet"));
    TH1D *h_fitplus1  = (TH1D*)(f_in_fitplus1 ->Get("mass_wide_range_inc0jet"));
    TH1D *h_fitminus1 = (TH1D*)(f_in_fitminus1->Get("mass_wide_range_inc0jet"));
    TH1D *h_fitplus2  = (TH1D*)(f_in_fitplus2 ->Get("mass_wide_range_inc0jet"));
    TH1D *h_fitminus2 = (TH1D*)(f_in_fitminus2->Get("mass_wide_range_inc0jet"));
    TH1D *h_fitplus3  = (TH1D*)(f_in_fitplus3 ->Get("mass_wide_range_inc0jet"));
    TH1D *h_fitminus3 = (TH1D*)(f_in_fitminus3->Get("mass_wide_range_inc0jet"));
    TH1D *h_misplus   = (TH1D*)(f_in_misplus  ->Get("mass_wide_range_inc0jet"));
    TH1D *h_misminus  = (TH1D*)(f_in_misminus ->Get("mass_wide_range_inc0jet"));
    TH1D *h_interp3   = (TH1D*)(f_in_interp3  ->Get("mass_wide_range_inc0jet"));
    TH1D *h_ratiocon  = (TH1D*)(f_in_ratiocon ->Get("mass_wide_range_inc0jet"));

    h_nominal  ->SetDirectory(0);
    h_fitplus0 ->SetDirectory(0);
    h_fitminus0->SetDirectory(0);
    h_fitplus1 ->SetDirectory(0);
    h_fitminus1->SetDirectory(0);
    h_fitplus2 ->SetDirectory(0);
    h_fitminus2->SetDirectory(0);
    h_fitplus3 ->SetDirectory(0);
    h_fitminus3->SetDirectory(0);
    h_misplus  ->SetDirectory(0);
    h_misminus ->SetDirectory(0);
    h_interp3  ->SetDirectory(0);
    h_ratiocon ->SetDirectory(0);

    h_nominal  ->SetStats(0);
    h_fitplus0 ->SetStats(0);
    h_fitminus0->SetStats(0);
    h_fitplus1 ->SetStats(0);
    h_fitminus1->SetStats(0);
    h_fitplus2 ->SetStats(0);
    h_fitminus2->SetStats(0);
    h_fitplus3 ->SetStats(0);
    h_fitminus3->SetStats(0);
    h_misplus  ->SetStats(0);
    h_misminus ->SetStats(0);
    h_interp3  ->SetStats(0);
    h_ratiocon ->SetStats(0);

    f_in_nominal  ->Close();
    f_in_fitplus0 ->Close();
    f_in_fitminus0->Close();
    f_in_fitplus1 ->Close();
    f_in_fitminus1->Close();
    f_in_fitplus2 ->Close();
    f_in_fitminus2->Close();
    f_in_fitplus3 ->Close();
    f_in_fitminus3->Close();
    f_in_misplus  ->Close();
    f_in_misminus ->Close();
    f_in_interp3  ->Close();
    f_in_ratiocon ->Close();

    TH1D *h_syst_fit = (TH1D *)(h_nominal->Clone("h_syst_fit"));
    TH1D *h_syst_mis = (TH1D *)(h_nominal->Clone("h_syst_mis"));
    TH1D *h_syst_int = (TH1D *)(h_nominal->Clone("h_syst_int"));
    TH1D *h_syst_tot = (TH1D *)(h_nominal->Clone("h_syst_tot"));
    TH1D *h_syst_rat = (TH1D *)(h_nominal->Clone("h_syst_rat"));

    TH1D *h_rel_syst_fit = (TH1D *)(h_nominal->Clone("h_rel_syst_fit"));
    TH1D *h_rel_syst_mis = (TH1D *)(h_nominal->Clone("h_rel_syst_mis"));
    TH1D *h_rel_syst_int = (TH1D *)(h_nominal->Clone("h_rel_syst_int"));
    TH1D *h_rel_syst_tot = (TH1D *)(h_nominal->Clone("h_rel_syst_tot"));
    TH1D *h_rel_syst_rat = (TH1D *)(h_nominal->Clone("h_rel_syst_rat"));

    for (uint i = 1; i < h_nominal->GetNbinsX(); ++i)
    {
        std::vector<double> d_fits;
        d_fits.push_back(std::abs(h_fitplus0 ->GetBinContent(i) - h_nominal->GetBinContent(i)));
        d_fits.push_back(std::abs(h_fitminus0->GetBinContent(i) - h_nominal->GetBinContent(i)));
        d_fits.push_back(std::abs(h_fitplus1 ->GetBinContent(i) - h_nominal->GetBinContent(i)));
        d_fits.push_back(std::abs(h_fitminus1->GetBinContent(i) - h_nominal->GetBinContent(i)));
        d_fits.push_back(std::abs(h_fitplus2 ->GetBinContent(i) - h_nominal->GetBinContent(i)));
        d_fits.push_back(std::abs(h_fitminus2->GetBinContent(i) - h_nominal->GetBinContent(i)));
        d_fits.push_back(std::abs(h_fitplus3 ->GetBinContent(i) - h_nominal->GetBinContent(i)));
        d_fits.push_back(std::abs(h_fitminus3->GetBinContent(i) - h_nominal->GetBinContent(i)));

        std::vector<double> d_miss;
        d_miss.push_back(std::abs(h_misplus ->GetBinContent(i) - h_nominal->GetBinContent(i)));
        d_miss.push_back(std::abs(h_misminus->GetBinContent(i) - h_nominal->GetBinContent(i)));

        double d_fit = *std::max_element(d_fits.begin(), d_fits.end());
        double d_mis = *std::max_element(d_miss.begin(), d_miss.end());
        double d_int = std::abs(h_interp3 ->GetBinContent(i) - h_nominal->GetBinContent(i));
        double d_rat = std::abs(h_ratiocon->GetBinContent(i) - h_nominal->GetBinContent(i));

        double d_tot = std::sqrt(d_fit*d_fit + d_mis*d_mis + d_int*d_int + d_rat*d_rat);

        h_syst_fit->SetBinContent(i, d_fit);
        h_syst_mis->SetBinContent(i, d_mis);
        h_syst_int->SetBinContent(i, d_int);
        h_syst_tot->SetBinContent(i, d_tot);
        h_syst_rat->SetBinContent(i, d_rat);
        h_syst_fit->SetBinError(i, 0);
        h_syst_mis->SetBinError(i, 0);
        h_syst_int->SetBinError(i, 0);
        h_syst_tot->SetBinError(i, 0);
        h_syst_rat->SetBinError(i, 0);

        h_rel_syst_fit->SetBinContent(i, d_fit/h_nominal->GetBinContent(i)*100);
        h_rel_syst_mis->SetBinContent(i, d_mis/h_nominal->GetBinContent(i)*100);
        h_rel_syst_int->SetBinContent(i, d_int/h_nominal->GetBinContent(i)*100);
        h_rel_syst_tot->SetBinContent(i, d_tot/h_nominal->GetBinContent(i)*100);
        h_rel_syst_rat->SetBinContent(i, d_rat/h_nominal->GetBinContent(i)*100);
        h_rel_syst_fit->SetBinError(i, 0);
        h_rel_syst_mis->SetBinError(i, 0);
        h_rel_syst_int->SetBinError(i, 0);
        h_rel_syst_tot->SetBinError(i, 0);
        h_rel_syst_rat->SetBinError(i, 0);
    }

    TCanvas *c_syst = new TCanvas("c_syst", "Plot w systematics", 800, 800);
    c_syst->SetLeftMargin(0.11);
    c_syst->SetRightMargin(0.05);
    c_syst->SetTopMargin(0.05);
    h_nominal->SetTitle("");
    h_nominal->GetXaxis()->SetTitle("m_{ee} [GeV]");
    h_nominal->GetYaxis()->SetTitle("#events");
    h_nominal->GetYaxis()->SetTitleOffset(1.5);
    h_nominal->GetXaxis()->SetNoExponent();
    h_nominal->GetXaxis()->SetMoreLogLabels();
    h_nominal->GetYaxis()->SetRangeUser(1e-1, 1e4);
    h_nominal->SetFillColor(30);
    h_nominal->Draw("hist");
    // Nominal plus systematic error
    TH1D *h_syst_pluserr = (TH1D *)(h_nominal->Clone("h_syst_pluserr"));
    h_syst_pluserr->Add(h_syst_tot);
    h_syst_pluserr->SetFillStyle(3144);
    h_syst_pluserr->SetFillColor(kGreen-1);
    h_syst_pluserr->Draw("samehist");
    // Nominal minus systematic error
    TH1D *h_syst_minuserr = (TH1D *)(h_nominal->Clone("h_syst_minuserr"));
    h_syst_minuserr->Add(h_syst_tot, -1);
    h_syst_minuserr->Draw("samehist");
    h_nominal->Draw("sameaxis");
    c_syst->SetLogx();
    c_syst->SetLogy();
    c_syst->SetGridx();
    c_syst->SetGridy();
    TLegend *leg = new TLegend(0.55, 0.85, 0.95, 0.95);
    leg->AddEntry(h_nominal, "Fakes (data-driven)", "f");
    leg->AddEntry(h_syst_pluserr, "OS/SS ratio fit err.", "f");
    leg->Draw();
    c_syst->Update();
    c_syst->SaveAs("fakes_ee_syst_"+era+".pdf");

    TCanvas *c_rel_syst = new TCanvas("c_rel_syst", "Relative errors", 800, 800);
    c_rel_syst->SetLeftMargin(0.12);
    c_rel_syst->SetRightMargin(0.05);
    c_rel_syst->SetTopMargin(0.05);
    // Fit error
    h_rel_syst_fit->GetXaxis()->SetTitle("m_{ee} [GeV]");
    h_rel_syst_fit->GetXaxis()->SetNoExponent();
    h_rel_syst_fit->GetXaxis()->SetMoreLogLabels();
    h_rel_syst_fit->GetYaxis()->SetTitle("Fake bkg. syst. err. [%]");    
    h_rel_syst_fit->GetYaxis()->SetTitleOffset(1.5);
    h_rel_syst_fit->SetFillStyle(0);
    h_rel_syst_fit->SetLineColor(kBlack);
    h_rel_syst_fit->SetMarkerColor(kBlack);
    h_rel_syst_fit->SetMarkerStyle(kFullDotLarge);
    h_rel_syst_fit->Draw("PL");
    // Charge misid error
    h_rel_syst_mis->SetFillStyle(0);
    h_rel_syst_mis->SetLineColor(kBlue);
    h_rel_syst_mis->SetMarkerColor(kBlue);
    h_rel_syst_mis->SetMarkerStyle(kFullDotLarge);
    h_rel_syst_mis->Draw("SAME PL");
    // Z-peak interpolation error
    h_rel_syst_int->SetFillStyle(0);
    h_rel_syst_int->SetLineColor(kMagenta+2);
    h_rel_syst_int->SetMarkerColor(kMagenta+2);
    h_rel_syst_int->SetMarkerStyle(kFullDotLarge);
    h_rel_syst_int->Draw("SAME PL");
    // SS/OS ratio function choice error
    h_rel_syst_rat->SetFillStyle(0);
    h_rel_syst_rat->SetLineColor(kGreen+3);
    h_rel_syst_rat->SetMarkerColor(kGreen+3);
    h_rel_syst_rat->SetMarkerStyle(kFullDotLarge);
    h_rel_syst_rat->Draw("SAME PL");
    // Total error
    h_rel_syst_tot->SetFillStyle(0);
    h_rel_syst_tot->SetLineColor(kRed);
    h_rel_syst_tot->SetMarkerColor(kRed);
    h_rel_syst_tot->SetMarkerStyle(kFullDotLarge);
    h_rel_syst_tot->Draw("SAME PL");
    h_rel_syst_fit->SetTitle("");
    h_rel_syst_fit->GetYaxis()->SetRangeUser(0, 80);
    c_rel_syst->SetLogx();
    c_rel_syst->SetGridx();
    c_rel_syst->SetGridy();
    TLegend *leg_rel = new TLegend(0.5, 0.7, 0.95, 0.95);
    leg_rel->AddEntry(h_rel_syst_tot, "Total error", "lp");
    leg_rel->AddEntry(h_rel_syst_fit, "OS/SS ratio fit error", "lp");
    leg_rel->AddEntry(h_rel_syst_rat, "OS/SS ratio function choice", "lp");
    leg_rel->AddEntry(h_rel_syst_mis, "Charge mis-ID SF error", "lp");
    leg_rel->AddEntry(h_rel_syst_int, "Z-peak interpolation choice", "lp");
    leg_rel->Draw();
    c_rel_syst->Update();
    c_rel_syst->SaveAs("fakes_ee_syst_rel_"+era+".pdf");

}