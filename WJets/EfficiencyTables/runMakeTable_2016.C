#include <iostream>
#include <iomanip>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>
#include <TH2.h>

runMakeTable_2016()
{  
  // Muon-Isolation
  //TFile *effFile = new TFile("MuonIso_Z_RunBCD_prompt80X_7p65.root", "READ");
  //string effName = "MC_NUM_TightRelIso_DEN_TightID_PAR_pt_spliteta_bin1/abseta_pt_ratio"; 
  
  // Muon-ID
  //TFile *effFile = new TFile("MuonID_Z_RunBCD_prompt80X_7p65.root", "READ");
  //string effName = "MC_NUM_TightIDandIPCut_DEN_genTracks_PAR_pt_spliteta_bin1/abseta_pt_ratio";
  
  // Muon-Trigger
  TFile *effFile = new TFile("SingleMuonTrigger_Z_RunBCD_prompt80X_7p65.root", "READ");
  string effName = "IsoMu22_OR_IsoTkMu22_PtEtaBins_Run273158_to_274093/efficienciesDATA/abseta_pt_DATA"; 
  
  TH2D *h2_Efficiency = (TH2D*)effFile->Get(effName.c_str());
  h2_Efficiency->Draw("colz"); 
  
  int etaBins = h2_Efficiency->GetNbinsX(); 
  int ptBins = h2_Efficiency->GetNbinsY(); 
  cout<<"EtaBins: "<<etaBins<<", PtBins: "<<ptBins<<endl; 
  
  double etaLow(-1.), etaHigh(-1.), ptLow(-1.), ptHigh(-1.), eff(-1.), effErr(-1.); 
  
  for(int eta(1); eta<=etaBins; eta++){
    for(int pt(1); pt<=ptBins; pt++){
      etaLow = h2_Efficiency->GetXaxis()->GetBinLowEdge(eta);
      etaHigh = etaLow + h2_Efficiency->GetXaxis()->GetBinWidth(eta);
      ptLow =  h2_Efficiency->GetYaxis()->GetBinLowEdge(pt); 
      ptHigh = ptLow + h2_Efficiency->GetYaxis()->GetBinWidth(pt);
      eff = h2_Efficiency->GetBinContent(eta, pt);
      effErr = h2_Efficiency->GetBinError(eta, pt);
      
      cout<<etaLow<<"	"<<etaHigh<<"	"<<ptLow<<"	"<<ptHigh<<"	"<<eff<<"	"<<effErr<<"	"<<effErr<<endl; 
      
    }
  }
  

  
}
