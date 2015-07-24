#include <TH1.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TLegend.h>
#include <TLatex.h>
#include "Includes/getFilesAndHistograms.h"
#include "Includes/fileNames.h"


void FuncPlot(string  variable = "SpTJets_Zinc2jet", bool log = 0, bool decrease = 0);

#include "Includes/variablesOfInterestVarWidth.h"

double Luminosity(19290.);
bool isMuon = 1;
string leptonFlavor = "DMu";
string unfAlg = "Bayes";
bool doVarWidth = true ;
int JetPtMin = 30 ;

double getErrors(string variable = "ThirdJetPt_Zinc3jet"){
   string energy = getEnergy();
   if ( !isMuon ) leptonFlavor = "DE";
   gStyle->SetOptStat(0);
   string filesIN[4] = {"ZNGoodJets_Zexc","JetsHT","JetPt","JetEta"};
   string addNames[5] = {"First","Second","Third","Fourth","Fifth"};
   string a = "inc1";
   // get number of jets
   unsigned pos = variable.find("jet");         // position of "live" in str
   std::string str3 ;
   if (variable.find("ZNGoodJets")!=std::string::npos) str3 = "1"; 
   else  str3 = variable.substr (pos-1,1);
   int num;  istringstream ( str3 ) >> num;
   stringstream ss;   ss << num;
   cout << " which multiplicity : " << num << endl;

   double values[4] ;
   double min = 100000000.;
   double max = 0.;
   double mean = 0.;
   for (int i(0); i <= 3; i++){ 
      string variableT;
      if ( i == 0 ) variableT = filesIN[0];
      else if (i  == 1 ) variableT = filesIN[i] + "_Zinc" +  ss.str()  + "jet";
      else variableT = addNames[num - 1]+filesIN[i] + "_Zinc" +  ss.str()  + "jet"; 
      string fileName = "PNGFiles/FinalUnfold/" + leptonFlavor + "_" + energy +   "_unfolded_" + variableT + "_histograms_" + unfAlg ;
      if (doVarWidth) fileName += "_VarWidth";
      fileName +=".root";
      cout << " opening : " <<fileName <<endl;
      TFile *f  = new TFile(fileName.c_str());
      TH1D *dataCentral = (TH1D*) f->Get("Central");
      if ( i == 0 ) values[i] = dataCentral ->Integral(num+1,dataCentral->GetNbinsX()+1)	;
      else values[i] = dataCentral ->Integral(1,dataCentral->GetNbinsX()+1)   ;
      cout << " grabbing central unfolds for errors  : "  <<  values[i] << endl;
      if (values[i] < min )  min = values[i] ;
      if (values[i] > max )  max = values[i] ;
      mean +=  values[i] ;
   }

   cout << " Envelope error " << (max-min)/2 << " min : " << min << " max " << max << endl;
   return ((max-min)/2)/(mean/4);

}



void runPlotting()
{
   NVAROFINTEREST = 41;
   for (int i(0); i < NVAROFINTEREST; i++){
      FuncPlot(VAROFINTEREST[i].name, VAROFINTEREST[i].log, VAROFINTEREST[i].decrease);
   }
   //cout << getErrors() << endl;
   //-- for testing on specific variable
   //makeLISTOFVAROFINTEREST();
   //TObjString *name = new TObjString("ZNGoodJets_Zexc");
   //int index(LISTOFVAROFINTEREST->IndexOf(name));
   //FuncPlot(VAROFINTEREST[index].name, VAROFINTEREST[index].log, VAROFINTEREST[index].decrease);

}


void FuncPlot(string variable, bool logZ, bool decrease)
{
   string energy = getEnergy();
   if ( !isMuon ) leptonFlavor = "DE";
   gStyle->SetOptStat(0);
   TH1::SetDefaultSumw2();
   string fileName = "PNGFiles/FinalUnfold/" + leptonFlavor + "_" + energy +   "_unfolded_" + variable + "_histograms_" + unfAlg ;
   if (doVarWidth) fileName += "_VarWidth";
   fileName += ".root";
   cout << " opening : " <<fileName <<endl;
   TFile *f  = new TFile(fileName.c_str());
   //TFile *f2 = new TFile("PNGFiles/PDFSystematics.root");
   //TFile *f2 = new TFile("PDFSystematics_MadGraph_JetPtMin_30.root");
   //TFile *f3 = new TFile("PDFSystematics_Sherpa_JetPtMin_20.root");
   string genVariable = "gen" + variable;
   //TH1D *PDFSystMad = (TH1D*) f2->Get(genVariable.c_str());
   //TH1D *PDFSystShe = (TH1D*) f3->Get(genVariable.c_str());
   TH1D *data        = (TH1D*) f->Get("Data");
   TH1D *dataCentral = (TH1D*) f->Get("Central");
   TH1D *genMad      = (TH1D*) f->Get("genMad");
//   TH1D *genShe      = (TH1D*) f->Get("genShe");;
   TFile *fShe;
   fShe = getFile(FILESDIRECTORY, "DE", energy, DYSHERPAFILENAME, JetPtMin, JetPtMin, 0, doVarWidth);
   TH1D *genShe      = (TH1D*) fShe->Get(genVariable.c_str());

//   if (isMuon) genShe     = (TH1D*) f->Get("genShe");
//   else genShe = (TH1D*) f->Get("genMad");
   int nSyst(7);
   TH1D *hSyst[7];
   hSyst[0]          = (TH1D*) f->Get("JECup");
   hSyst[1]          = (TH1D*) f->Get("JECdown");
   hSyst[2]          = (TH1D*) f->Get("PUup");
   hSyst[3]          = (TH1D*) f->Get("PUdown");
   hSyst[4]          = (TH1D*) f->Get("XSECup");
   hSyst[5]          = (TH1D*) f->Get("XSECdown");
   hSyst[6]          = (TH1D*) f->Get("JERup");


   const int nBins(dataCentral->GetNbinsX());
   cout << " central int:  " << variable << "   " << dataCentral->Integral(1, nBins) << " with under/over:  " << dataCentral->Integral(0, nBins+1) << endl;
   for (int syst(0); syst < nSyst; syst++){
      double tempScale = hSyst[syst]->Integral(1, nBins) ;
      cout << "original integral " << syst << "   " << tempScale << endl;
   }

   bool doXSec(1);
   bool doNormalize(0);
   if (doXSec) doNormalize = false;
   if (doNormalize) doXSec = false;
   if (doXSec && !doNormalize ){
      //double Luminosity=->Integral()/19300.0;
      for (int i(1); i <= nBins; i++){
         double binW = data->GetBinWidth(i);
         data->SetBinContent(i, data->GetBinContent(i)*1./(Luminosity*binW));
         dataCentral->SetBinContent(i, dataCentral->GetBinContent(i)*1./(Luminosity*binW));
         genMad->SetBinContent(i, genMad->GetBinContent(i)*1./(Luminosity*binW));
         genShe->SetBinContent(i, genShe->GetBinContent(i)*1./(Luminosity*binW));
         data->SetBinError(i, data->GetBinError(i)*1./(Luminosity*binW));
         dataCentral->SetBinError(i, dataCentral->GetBinError(i)*1./(Luminosity*binW));
         genMad->SetBinError(i, genMad->GetBinError(i)*1./(Luminosity*binW));
         genShe->SetBinError(i, genShe->GetBinError(i)*1./(Luminosity*binW));
      }
      for (int syst(0); syst < nSyst; syst++){
         double tempScale = hSyst[syst]->Integral(1,nBins) ;
         cout << "scale "<< syst<<"   "<<tempScale <<endl;
         double sum = 0  ;
         for (int k(1) ; k < nBins + 1 ; k++) sum += hSyst[syst]->GetBinContent(k);
         //cout << "scale "<<sum <<endl;
         for (int i(1); i <= nBins; i++){
            double binW = data->GetBinWidth(i);
            hSyst[syst]->SetBinContent(i, hSyst[syst]->GetBinContent(i)*1./(Luminosity*binW));
            hSyst[syst]->SetBinError(i, hSyst[syst]->GetBinError(i)*1./(Luminosity*binW));
         }
      }

   }
   if (doNormalize) {
      data->Scale(1/data->Integral(1,nBins));
      dataCentral->Scale(1/dataCentral->Integral(1,nBins));
      genMad->Scale(1/genMad->Integral(1,nBins));
      genShe->Scale(1/genShe->Integral(1,nBins));
      for (int syst(0); syst < nSyst; syst++){
         double tempScale = hSyst[syst]->Integral(1,nBins) ;
         cout << "scale "<< syst<<"   "<<tempScale <<endl;
         hSyst[syst]->Scale(1/tempScale);
      }
   }

   double xCoor[nBins], yCoor[nBins], xErr[nBins], yStat[nBins], ySystDown[nBins], ySystUp[nBins];
   double xCoorMadRatio[nBins], yCoorMadRatio[nBins], yStatMadRatio[nBins], ySystDownMadRatio[nBins], ySystUpMadRatio[nBins];
   double xCoorSheRatio[nBins], yCoorSheRatio[nBins], yStatSheRatio[nBins], ySystDownSheRatio[nBins], ySystUpSheRatio[nBins];
   double yCoorMC[nBins]; 

   cout << "go to error computation " << endl;
   for (int bin(1); bin <= nBins; bin++){
      double totalSystematicsUp(0.), totalSystematicsDown(0.);
      double totalStatistics(dataCentral->GetBinError(bin)); 
      double centralValue(dataCentral->GetBinContent(bin));

      double effSF = 0.00;
      totalSystematicsUp += pow( pow((1.+effSF), 2) - 1 , 2);
      totalSystematicsDown += pow( pow((1.+effSF), 2) - 1 , 2);

    double diffUnfError = getErrors(variable) * centralValue; // error from unfolding different distributions 
      cout << "aaaa :" << centralValue << "   " << diffUnfError <<endl;
/*      totalSystematicsUp += diffUnfError *diffUnfError ; 
      totalSystematicsDown += diffUnfError *diffUnfError ; 
*/
      double mcValue(genMad->GetBinContent(bin));
      yCoorMC[bin-1] = mcValue;
      xCoor[bin-1]    = dataCentral->GetBinCenter(bin);
      xCoorMadRatio[bin-1]    = dataCentral->GetBinCenter(bin);
      xCoorSheRatio[bin-1]    = dataCentral->GetBinCenter(bin);
      yCoor[bin-1]    = centralValue; 
      //xErr[bin-1]     = 0.5 * dataCentral->GetBinWidth(bin);
      xErr[bin-1]     = 0.;
      yStat[bin-1]    = totalStatistics;
      yStatMadRatio[bin-1]    = totalStatistics/(genMad->GetBinContent(bin));
      yStatSheRatio[bin-1]    = totalStatistics/(genShe->GetBinContent(bin));
      for (int syst(0); syst < nSyst; syst++){
         double difference(hSyst[syst]->GetBinContent(bin) - centralValue);
         cout << "Syst:  "<< syst<<"   "<<difference<<"  ";
         if ( difference > 0) totalSystematicsUp += difference * difference;
         else totalSystematicsDown += difference * difference;
      } 
      cout<<endl;
      ySystDown[bin-1]   = sqrt(totalStatistics * totalStatistics + totalSystematicsDown );
      ySystUp[bin-1]     = sqrt(totalStatistics * totalStatistics + totalSystematicsUp );
      if(genMad->GetBinContent(bin) > 0){
         yCoorMadRatio[bin-1]       = centralValue / (genMad->GetBinContent(bin)); 
         ySystDownMadRatio[bin-1]   = sqrt(totalStatistics * totalStatistics + totalSystematicsDown )/(genMad->GetBinContent(bin));
         ySystUpMadRatio[bin-1]     = sqrt(totalStatistics * totalStatistics + totalSystematicsUp )/(genMad->GetBinContent(bin));
      }
      else{ 
         xCoorMadRatio[bin-1]       = -999.;
         yCoorMadRatio[bin-1]       = 0.1; 
         ySystDownMadRatio[bin-1]   = 0.1; 
         ySystUpMadRatio[bin-1]     = 0.1; 
      }
      if(genShe->GetBinContent(bin) > 0){
         yCoorSheRatio[bin-1]       = centralValue / (genShe->GetBinContent(bin)); 
         ySystDownSheRatio[bin-1]   = sqrt(totalStatistics * totalStatistics + totalSystematicsDown )/(genShe->GetBinContent(bin));
         ySystUpSheRatio[bin-1]     = sqrt(totalStatistics * totalStatistics + totalSystematicsUp )/(genShe->GetBinContent(bin));
      }
      else{ 
         xCoorSheRatio[bin-1]       = -999.;
         yCoorSheRatio[bin-1]       = 0.1;
         ySystDownSheRatio[bin-1]   = 0.1; 
         ySystUpSheRatio[bin-1]     = 0.1; 
      }


      totalSystematicsUp   = sqrt(totalSystematicsUp);
      totalSystematicsDown = sqrt(totalSystematicsDown);

   }

   cout << " finished error computation " << endl;

   TGraph *mcGraph = new TGraph(nBins, xCoor, yCoorMC);
   TGraphAsymmErrors *grCentralStat      = new TGraphAsymmErrors(nBins, xCoor, yCoor, xErr, xErr, yStat, yStat);
   TGraphAsymmErrors *grCentralStatMadRatio = new TGraphAsymmErrors(nBins, xCoorMadRatio, yCoorMadRatio, xErr, xErr, yStatMadRatio, yStatMadRatio);
   TGraphAsymmErrors *grCentralStatSheRatio = new TGraphAsymmErrors(nBins, xCoorSheRatio, yCoorSheRatio, xErr, xErr, yStatSheRatio, yStatSheRatio);
   TGraphAsymmErrors *grCentralSystMadRatio = new TGraphAsymmErrors(nBins, xCoorMadRatio, yCoorMadRatio, xErr, xErr, ySystDownMadRatio, ySystUpMadRatio);
   TGraphAsymmErrors *grCentralSystSheRatio = new TGraphAsymmErrors(nBins, xCoorSheRatio, yCoorSheRatio, xErr, xErr, ySystDownSheRatio, ySystUpSheRatio);
   grCentralStat->SetMarkerStyle(20);
   grCentralStat->SetMarkerSize(0.8);
   grCentralStat->SetMarkerColor(kBlack);
   grCentralStat->SetLineColor(kBlack);
   //grCentralStat->SetLineWidth(2);
   TGraphAsymmErrors *grCentralSyst = new TGraphAsymmErrors(nBins, xCoor, yCoor, xErr, xErr, ySystDown, ySystUp);
   grCentralSyst->SetLineColor(kRed+1);
   //grCentralSyst->SetLineWidth(2);
   grCentralSyst->SetMarkerStyle(20);
   grCentralSyst->SetMarkerSize(0.8);

   cout << " create canvas " << endl;


   TCanvas *can = new TCanvas(variable.c_str(), variable.c_str(), 600, 1000);
   can->cd();
   TPad *pad1 = new TPad("pad1","pad1", 0, 0.4, 1, 1); 
   pad1->SetTopMargin(0.11);
   pad1->SetBottomMargin(0);
   pad1->SetRightMargin(0.02);
   pad1->SetLeftMargin(0.11);
   pad1->SetTicks();
   pad1->Draw();
   pad1->cd();
   if (logZ) pad1->SetLogy();
   double MineYMax = genMad->GetMaximum();
   double lowerLimit = genMad->GetXaxis()->GetXmin();
   if (grCentralSyst->GetMaximum() > MineYMax) MineYMax = grCentralSyst->GetMaximum();
   if (variable.find("ZNGood") != string::npos){
      //MineYMax = 85.;
      cout << "Remove first bin: " << genMad->GetXaxis()->GetXmin() + genMad->GetBinWidth(1) << endl;
      genMad->GetXaxis()->SetRangeUser(genMad->GetXaxis()->GetXmin() + genMad->GetBinWidth(1), genMad->GetXaxis()->GetXmax());
      cout << "Remove first bin: " << genMad->GetXaxis()->GetXmin() + genMad->GetBinWidth(1) << endl;
      lowerLimit = genMad->GetXaxis()->GetXmin() + genMad->GetBinWidth(1);
   }
   grCentralSyst->GetXaxis()->SetLimits(lowerLimit, genMad->GetXaxis()->GetXmax());
   genMad->SetFillStyle(3001);
   genMad->SetFillColor(kAzure-9);
   genMad->SetLineColor(kAzure-9);
   genMad->GetYaxis()->SetTitleSize(0.04);
   genMad->GetYaxis()->SetTitleOffset(1.45);
   genMad->SetMaximum(MineYMax*1.2);
   string temp;
   if (doXSec || doNormalize) { 
      string xtitle = genMad->GetXaxis()->GetTitle();
      string shortVar = xtitle.substr(0, xtitle.find(" "));
      string unit = "";
      if (xtitle.find("[") != string::npos){
         size_t begin = xtitle.find("[") + 1;
         unit = xtitle.substr(begin);
         unit = unit.substr(0, unit.find("]"));
      }
      cout << shortVar << "   unit: " << unit << endl;
      temp = "d#sigma / d" + shortVar;
      if (doNormalize) temp = "1/#sigma " + temp;
      else if (doXSec){
         temp += "  [pb";
         if (unit != "" ) temp += " / " + unit;
         temp += "]";
      }
      //genMad->GetYaxis()->SetTitle(temp.c_str());
      //genShe->GetYaxis()->SetTitle(temp.c_str());
      genMad->GetYaxis()->SetTitle("");
      genShe->GetYaxis()->SetTitle("");
   }


   genMad->Draw("E2");
   mcGraph->SetLineColor(kAzure-9);
   //mcGraph->Draw("L same");
   //mcGraph->Draw("L");
   genShe->SetFillStyle(3002);
   genShe->SetFillColor(kOrange-3);
   genShe->SetLineColor(kOrange-3);
   genShe->Draw("E2 same");
   grCentralSyst->Draw("P same");
   grCentralStat->Draw("P same");
   pad1->Draw();
   // set the legend
   double xLowLeg(0.65), xHighLeg(0.98);
   if (!decrease){
      xLowLeg = 0.14;
      xHighLeg = 0.47;
   }
   TLegend *legend = new TLegend(xLowLeg, 0.66, xHighLeg, 0.91);
   legend->SetHeader("  ");
   genMad->SetTitle("");
   legend->SetFillStyle(0);
   legend->SetBorderSize(0);
   legend->AddEntry(genMad, "MG+Py6 at NNLO Stat. Error", "pf");
   legend->AddEntry(genShe, "Sherpa NLO Stat. Error", "pf");
   legend->AddEntry(grCentralStat, "Data Stat. Error ", "pl");
   legend->AddEntry(grCentralSyst, "Data Tot. Error", "l");
   legend->Draw();

   TLatex *variableLabel = new TLatex();
   variableLabel->SetTextSize(0.035);
   variableLabel->SetTextFont(42);
   variableLabel->SetLineWidth(2);
   variableLabel->SetTextColor(kBlack);
   variableLabel->SetNDC();
   variableLabel->SetTextAlign(21);
   variableLabel->DrawLatex(0.54,0.965,data->GetTitle());

   TLatex *cmsColl = new TLatex();
   cmsColl->SetTextSize(0.025);
   cmsColl->SetTextFont(42);
   cmsColl->SetLineWidth(2);
   cmsColl->SetTextColor(kBlack);
   cmsColl->SetNDC();
   cmsColl->SetTextAlign(11);
   cmsColl->DrawLatex(0.11,0.905, "CMS Collaboration");

   TLatex *sqrtXTeV = new TLatex();
   sqrtXTeV->SetTextSize(0.025);
   sqrtXTeV->SetTextFont(42);
   sqrtXTeV->SetLineWidth(2);
   sqrtXTeV->SetTextColor(kBlack);
   sqrtXTeV->SetNDC();
   sqrtXTeV->SetTextAlign(21);
   if (energy == "7TeV") sqrtXTeV->DrawLatex(0.54,0.905, "#sqrt{s} = 7 TeV");
   if (energy == "8TeV") sqrtXTeV->DrawLatex(0.54,0.905, "#sqrt{s} = 8 TeV");

   TLatex *intLumi = new TLatex();
   intLumi->SetTextSize(0.022);
   intLumi->SetTextFont(42);
   intLumi->SetLineWidth(2);
   intLumi->SetTextColor(kBlack);
   intLumi->SetNDC();
   intLumi->SetTextAlign(31);
   if (energy == "7TeV") intLumi->DrawLatex(0.98,0.91, "#int L dt = 4.95 fb^{-1}");
   if (energy == "8TeV") intLumi->DrawLatex(0.98,0.91, "#int L dt = 19.29 fb^{-1}");

   TLatex *ytitle = new TLatex();
   ytitle->SetTextSize(0.035);
   ytitle->SetTextFont(42);
   ytitle->SetLineWidth(2);
   ytitle->SetTextColor(kBlack);
   ytitle->SetNDC();
   ytitle->SetTextAlign(33);
   ytitle->SetTextAngle(90);
   if (doXSec || doNormalize) ytitle->DrawLatex(0.008,0.91,temp.c_str());
   pad1->cd();


   //// 	NEW TPAD
   can->cd();
   TPad *pad2 = new TPad("pad2","pad2", 0, 0.2286, 1, 0.4);
   pad2->SetTopMargin(0);
   pad2->SetBottomMargin(0);
   pad2->SetRightMargin(0.02);
   pad2->SetLeftMargin(0.11);
   pad2->SetGridy();
   pad2->SetTicks();
   pad2->Draw();
   pad2->cd();
   grCentralStatMadRatio->SetMarkerStyle(20);
   grCentralStatMadRatio->SetMarkerSize(0.8);
   grCentralStatMadRatio->SetMarkerColor(kBlack);
   grCentralStatMadRatio->SetLineColor(kBlack);
   grCentralSystMadRatio->SetLineColor(kRed+1);
   grCentralSystMadRatio->SetMarkerStyle(20);
   grCentralSystMadRatio->SetMarkerSize(0.8);
   grCentralSystMadRatio->GetXaxis()->SetLimits(lowerLimit, genMad->GetXaxis()->GetXmax());
   grCentralSystMadRatio->GetYaxis()->SetRangeUser(0.41,1.59);
   grCentralSystMadRatio->GetYaxis()->SetTitle("Data / MC");
   grCentralSystMadRatio->GetYaxis()->SetTitleSize(0.135);
   grCentralSystMadRatio->GetYaxis()->SetTitleOffset(0.44);
   grCentralSystMadRatio->GetYaxis()->CenterTitle();
   grCentralSystMadRatio->GetYaxis()->SetLabelSize(0.12);
   grCentralSystMadRatio->GetYaxis()->SetLabelOffset(0.01);
   grCentralSystMadRatio->SetTitle("");
   grCentralSystMadRatio->GetXaxis()->SetTickLength(0.06);
   grCentralSystMadRatio->Draw("AP");
   //PDFSystMad->SetFillColor(kGreen-3);
   //PDFSystMad->SetLineColor(kGreen-3);
   //PDFSystMad->SetFillStyle(3001);
   //PDFSystMad->Draw("E3 same");

   grCentralSystMadRatio->Draw("P same");
   grCentralStatMadRatio->Draw("P same");
   TLatex *latexLabel = new TLatex();
   latexLabel->SetTextSize(0.15);
   latexLabel->SetTextFont(42);
   latexLabel->SetLineWidth(2);
   latexLabel->SetTextColor(kAzure-5);
   latexLabel->SetNDC();
   latexLabel->DrawLatex(0.15,0.09,"Madgraph");
   TLegend *legend2 = new TLegend(0.75,0.85,0.9,0.95);
   legend2->SetFillStyle(0);
   legend2->SetBorderSize(0);
   //legend2->AddEntry(PDFSystMad, " PDF Syst.", "f");
   legend2->Draw();

   pad2->Draw();
   pad2->cd();

   can->cd();
   TPad *pad3 = new TPad("pad3","pad3", 0, 0., 1, 0.2286);
   pad3->SetTopMargin(0);
   pad3->SetBottomMargin(0.25);
   pad3->SetRightMargin(0.02);
   pad3->SetLeftMargin(0.11);
   pad3->SetGridy();
   pad3->SetTicks();
   pad3->Draw();
   pad3->cd();
   grCentralStatSheRatio->SetMarkerStyle(20);
   grCentralStatSheRatio->SetMarkerSize(0.8);
   grCentralStatSheRatio->SetMarkerColor(kBlack);
   grCentralStatSheRatio->SetLineColor(kBlack);
   grCentralSystSheRatio->SetLineColor(kRed+1);
   grCentralSystSheRatio->SetMarkerStyle(20);
   grCentralSystSheRatio->SetMarkerSize(0.8);
   grCentralSystSheRatio->GetXaxis()->SetLimits(lowerLimit, genMad->GetXaxis()->GetXmax());
   grCentralSystSheRatio->GetXaxis()->SetTitle(genMad->GetXaxis()->GetTitle());
   grCentralSystSheRatio->GetXaxis()->SetTitleSize(0.1);
   grCentralSystSheRatio->GetXaxis()->SetTitleOffset(1.05);
   grCentralSystSheRatio->GetXaxis()->SetLabelSize(0.1);
   grCentralSystSheRatio->GetXaxis()->SetLabelOffset(0.014);
   grCentralSystSheRatio->GetYaxis()->SetRangeUser(0.41,1.59);
   grCentralSystSheRatio->GetYaxis()->SetTitle("Data / MC");
   grCentralSystSheRatio->GetYaxis()->SetTitleSize(0.1);
   grCentralSystSheRatio->GetYaxis()->SetTitleOffset(0.6);
   grCentralSystSheRatio->GetYaxis()->CenterTitle();
   grCentralSystSheRatio->GetYaxis()->SetLabelSize(0.09);
   grCentralSystSheRatio->GetYaxis()->SetLabelOffset(0.012);
   grCentralSystSheRatio->SetTitle("");
   grCentralSystSheRatio->GetXaxis()->SetTickLength(0.06);
   grCentralSystSheRatio->Draw("AP");
   grCentralStatSheRatio->Draw("P same");
   //PDFSystShe->SetFillColor(kOrange-3);
   //PDFSystShe->SetLineColor(kOrange-3);
   //PDFSystShe->SetFillStyle(3001);
   //PDFSystShe->Draw("E3 same");
   grCentralSystSheRatio->Draw("P same");
   grCentralStatSheRatio->Draw("P same");
   TLatex *latexLabel1 = new TLatex();
   latexLabel1->SetTextSize(0.11);
   latexLabel1->SetTextFont(42);
   latexLabel1->SetLineWidth(2);
   latexLabel1->SetTextColor(kOrange+2);
   latexLabel1->SetNDC();
   latexLabel1->DrawLatex(0.15,0.35,"Sherpa");
   pad3->Draw();
   pad3->cd();
   can->cd();





   string outputDirectory = "PNGFiles/FinalUnfold/";
   //if (!isMuon) outputDirectory = "PNGFiles/FinalDataUnfoldElectron/";
   string outputFileNamePNG = outputDirectory + leptonFlavor + "_" + energy + "_RunPlotting_";
   if (doXSec)         outputFileNamePNG += "XSec_";
   else if (doNormalize) outputFileNamePNG += "Normalized_";
   outputFileNamePNG += variable + "_" + unfAlg ; 
   if (doVarWidth)  outputFileNamePNG += "_VarWidth";
   outputFileNamePNG += ".png";
   can->Print(outputFileNamePNG.c_str());

}

////////////////////////////////////////////////////////////////////////////////////
