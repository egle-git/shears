#include <TH1D.h>
#include <TH2D.h>
#include <TGraphAsymmErrors.h>
#include <TRandom.h>
#include <TFile.h>

void InputGenerator() {
  TH1D* h_gaus_1   = new TH1D("h_gaus_1",   "", 60, -3, 3);
  TH1D* h_gaus_2   = new TH1D("h_gaus_2",   "", 60, -3, 3);
  TH1D* h_gaus_3   = new TH1D("h_gaus_3",   "", 60, -3, 3);
  TH1D* h_gaus_tot = new TH1D("h_gaus_tot", "", 60, -3, 3);

  h_gaus_1->FillRandom("gaus", 10000);
  h_gaus_2->FillRandom("gaus", 10000);
  h_gaus_3->FillRandom("gaus", 10000);
  h_gaus_tot->FillRandom("gaus", 10000);
  h_gaus_tot->Scale(3);

  TH2D* h2D = new TH2D("h2D", "", 60, -3, 3, 60, -3, 3);
  for(Int_t i_x=0; i_x<h2D->GetNbinsX(); i_x++) {
    Int_t i_binX = i_x+1;

    for(Int_t i_y=0; i_y<h2D->GetNbinsY(); i_y++) {
      Int_t i_binY = i_y+1;
      Double_t randomValue = gRandom->Rndm();
      h2D->SetBinContent(i_binX, i_binY, randomValue);
      h2D->SetBinError(i_binX, i_binY, std::sqrt(randomValue));
    }
  }

  TGraphAsymmErrors* g_gaus_1 = new TGraphAsymmErrors(h_gaus_1); g_gaus_1->SetName("g_gaus_1");
  TGraphAsymmErrors* g_gaus_2 = new TGraphAsymmErrors(h_gaus_2); g_gaus_2->SetName("g_gaus_2");
  TGraphAsymmErrors* g_gaus_3 = new TGraphAsymmErrors(h_gaus_3); g_gaus_3->SetName("g_gaus_3");

  TFile *f_output = TFile::Open("ROOTFile_Input.root", "RECREATE");
  h_gaus_1->Write();
  h_gaus_2->Write();
  h_gaus_3->Write();
  h_gaus_tot->Write();

  h2D->Write();

  g_gaus_1->Write();
  g_gaus_2->Write();
  g_gaus_3->Write();
}