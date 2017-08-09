//Short program to print the values from the pileup histogram. This will be used for the 
//standaloneLumiReweighting.h file
//Djarcaro - 01/05/2017

//Standard includes:                                                                                                                                                   
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include <sstream>
#include "TFile.h"
#include "TLine.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TGraphAsymmErrors.h"
#include "TH1.h"
#include "TH2.h"
#include "TStyle.h" 

int main(int argc, char *argv[]){
  time_t now = time(0);
  tm *ltm = localtime(&now);
  char date[50]; snprintf(date,50,"_%02d_%02d_%d",1+ltm->tm_mon,ltm->tm_mday,1900+ltm->tm_year);
  printf("Opening: %s\n", argv[1]);
  //std::string file = "/afs/cern.ch/work/d/darcaro/private/CMSSW_8_0_26_patch2/src/shears/DYJets/EfficiencyTables/Eff_SF_ID_BF.root";
  TFile * rootFileInput = new TFile(argv[1], "read");
  std::string inputString(argv[1]);
  TGraphAsymmErrors * etaHisto = 0;
  TDirectory * histDir = 0;
  TH2D * ptetaHisto = 0;
  std::cout<<"Right before tracking\n";
  if(inputString.find("Tracking")){
    std::cout<<"Tracking\n";
    etaHisto = (TGraphAsymmErrors*)rootFileInput->Get("ratio_eff_aeta_dr030e030_corr");
    std::cout<<etaHisto->GetName()<<std::endl;
  }else{
    //TFile * rootFileInput = new TFile(file.c_str(), "read");
    histDir = (TDirectory*)rootFileInput->Get(argv[2]);
    std::cout<<histDir->GetName()<<std::endl;
    ptetaHisto = (TH2D*)histDir->Get("abseta_pt_ratio");
    std::cout<<ptetaHisto->GetName()<<std::endl;
  }

  std::string outputFileName(argv[1]);
  outputFileName = outputFileName.substr( outputFileName.find("Eff_SF"), 
					  outputFileName.find(".root")-outputFileName.find("Eff_SF"));
  outputFileName += date;
  std::cout<<outputFileName<<std::endl;
  FILE * outputFile;
  outputFile = fopen ( (outputFileName+".txt").c_str(),"w");

  //std::cout<<ptetaHisto->GetNbinsX()<<std::endl;
  //std::cout<<ptetaHisto->GetNbinsY()<<std::endl;
  int nBinsX = 0;
  int nBinsY = 0;
  double x = 0;
  double y = 0;
  double yErrorHigh=0;
  double yErrorLow=0;  
  if(inputString.find("Tracking")){
    //std::cout<<"etaHisto->GetN() = "<<etaHisto->GetN()<<std::endl;    
    for(int iEta = 0;iEta<etaHisto->GetN();iEta++){
      etaHisto->GetPoint(iEta,x,y);
      yErrorHigh = etaHisto->GetErrorYhigh(iEta);
      yErrorLow = etaHisto->GetErrorYlow(iEta);
      //std::cout<<iEntry*0.2<<"    "<<(iEntry+1)*0.2<<"       "<<y<<std::endl;
      printf("%f   %f   %f   %f   %f   %f   %f\n", iEta*0.2, (iEta+1)*0.2, 20.0, 120.0, 
	     y, yErrorLow, yErrorHigh);
      fprintf(outputFile,"%f   %f   %f   %f   %f   %f   %f\n", iEta*0.2, (iEta+1)*0.2, 20.0, 120.0, 
	     y, yErrorLow, yErrorHigh);
    }
  }else{
    nBinsX = ptetaHisto->GetNbinsX();
    nBinsY = ptetaHisto->GetNbinsY();
    //printf("%F",ptetaHisto->GetBinContent(2,2));
    
    for(int iBinX=1;iBinX<nBinsX+1;iBinX++){
      //printf("%f\n",xBinLowEdge[iBinX-1]);
      for(int iBinY=1;iBinY<nBinsY+1;iBinY++){
	/*
	  printf("%f   %f   %f   %f   %f   %f   %f",
	  ptetaHisto->GetXaxis()->GetBinLowEdge(iBinX), ptetaHisto->GetXaxis()->GetBinUpEdge(iBinX),
	  ptetaHisto->GetYaxis()->GetBinLowEdge(iBinY), ptetaHisto->GetYaxis()->GetBinUpEdge(iBinY),
	  ptetaHisto->GetBinContent(iBinX,iBinY),
	  ptetaHisto->GetBinErrorLow(iBinX,iBinY),ptetaHisto->GetBinErrorUp(iBinX,iBinY));
	  printf("\n");
	*/
	fprintf(outputFile,"%f   %f   %f   %f   %f   %f   %f",
		ptetaHisto->GetXaxis()->GetBinLowEdge(iBinX), ptetaHisto->GetXaxis()->GetBinUpEdge(iBinX),
		ptetaHisto->GetYaxis()->GetBinLowEdge(iBinY), ptetaHisto->GetYaxis()->GetBinUpEdge(iBinY),
		ptetaHisto->GetBinContent(iBinX,iBinY),
		ptetaHisto->GetBinErrorLow(iBinX,iBinY),ptetaHisto->GetBinErrorUp(iBinX,iBinY));
	fprintf(outputFile,"\n");
      }
      //    printf("\n");
    }
  }






  fclose(outputFile);
  rootFileInput->Close();
 
  return 0;
}
  

