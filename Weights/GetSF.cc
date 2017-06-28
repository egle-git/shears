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
#include "TH1.h"
#include "TH2.h"
#include "TStyle.h" 

int main(int argc, char *argv[]){
  time_t now = time(0);
  tm *ltm = localtime(&now);
  char date[50]; snprintf(date,50,"_%d_%d_%d",1+ltm->tm_mon,ltm->tm_mday,1900+ltm->tm_year);
  printf("Opening: %s\n", argv[1]);
  //std::string file = "/afs/cern.ch/work/d/darcaro/private/CMSSW_8_0_26_patch2/src/shears/DYJets/EfficiencyTables/Eff_SF_ID_BF.root";
  TFile * rootFileInput = new TFile(argv[1], "read");
  //TFile * rootFileInput = new TFile(file.c_str(), "read");
  TDirectory * histDir = (TDirectory*)rootFileInput->Get(argv[2]);
  std::cout<<histDir->GetName()<<std::endl;
  TH2D * ptetaHisto = (TH2D*)histDir->Get("abseta_pt_ratio");
  std::cout<<ptetaHisto->GetName()<<std::endl;

  std::string outputFileName(argv[1]);
  outputFileName = outputFileName.substr( outputFileName.find("Eff_SF"), 
					  outputFileName.find(".root")-outputFileName.find("Eff_SF"));
  outputFileName += date;
  std::cout<<outputFileName<<std::endl;
  FILE * outputFile;
  outputFile = fopen ( (outputFileName+".txt").c_str(),"w");

  //std::cout<<ptetaHisto->GetNbinsX()<<std::endl;
  //std::cout<<ptetaHisto->GetNbinsY()<<std::endl;
  int nBinsX = ptetaHisto->GetNbinsX();
  int nBinsY = ptetaHisto->GetNbinsY();
  
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






  fclose(outputFile);
  rootFileInput->Close();
 
  return 0;
}
  

