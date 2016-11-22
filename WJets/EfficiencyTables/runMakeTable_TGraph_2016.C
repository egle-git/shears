#include <iostream>
#include <iomanip>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TH1.h>
#include <TH2.h>

runMakeTable_TGraph_2016()
{
  TFile *effFile = new TFile("ratios.root", "READ");
  string effName = "ratio_eta";
  
  TGraphAsymmErrors *g1_Efficiency = (TGraphAsymmErrors*)effFile->Get(effName.c_str());
  g1_Efficiency->Draw(); 
  
  int etaBins = g1_Efficiency->GetN(); 
  cout<<"EtaBins: "<<etaBins<<endl; 
  
  double etaLow(-1.), etaHigh(-1.), ptLow(10.), ptHigh(500.), eff(-1.), effErrLow(-1.), effErrHigh(-1.); 
  double x(0.),y(0.); 
  
  for(int eta(0); eta<=etaBins; eta++){
    g1_Efficiency->GetPoint(eta, x, y); 
    
    etaLow = x - g1_Efficiency->GetErrorXlow(eta);
    etaHigh = x + g1_Efficiency->GetErrorXhigh(eta);
    eff = y;
    effErrLow = g1_Efficiency->GetErrorYlow(eta);
    effErrHigh = g1_Efficiency->GetErrorYhigh(eta);
    cout<<etaLow<<"	"<<etaHigh<<"	"<<ptLow<<"	"<<ptHigh<<"	"<<eff<<"	"<<effErrLow<<"	"<<effErrHigh<<endl; 
  }
  
  
  
}
