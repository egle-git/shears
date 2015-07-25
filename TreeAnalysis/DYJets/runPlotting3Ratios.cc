#include <TStyle.h>
#include <TH1.h>
#include <TH2.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TLegend.h>
#include <TLatex.h>
#include "Includes/getFilesAndHistograms.h"
#include "Includes/tableFunctions.h"
//#include "Includes/unfoldingFunctions.h"
//#include "Includes/test.h"
#include "Includes/setTDRStyle.h"

//double getErrors(string variable = "ThirdJetPt_Zinc3jet");
void getErrors(const TH1D * dataCentral, const TH1D * dataUnfWithSherpa, TH1D* UnfErrors);
void FuncPlot(string  variable = "SpTJets_Zinc2jet", bool log = 0, bool decrease = 0);

#include "Includes/variablesOfInterestVarWidth.h"

double Luminosity(19789.);
//double Luminosity(4950.);
double luminosityErr = 0.044;
string energy = "8TeV";//getEnergy();
bool isMuon = 0;
string leptonFlavor = "DMu";
string unfAlg = "Bayes";
bool doVarWidth = true ;
/// define names od output png files and tex
string outputDirectory = "PNGFiles/FinalUnfold_30_test/";


void runPlotting3Ratios()
{

  setTDRStyle();
  for (int i(2); i < 3/*NVAROFINTERESTZJETS*/; i++){
  	for (int j(1); j < 2; j++){
    		isMuon = j;
    		FuncPlot(VAROFINTERESTZJETS[i].name, VAROFINTERESTZJETS[i].log, VAROFINTERESTZJETS[i].decrease);
   	}
  }

  //-- for testing on specific variable
  //makeLISTOFVAROFINTEREST();
  //TObjString *name = new TObjString("ZNGoodJets_Zexc");
  //int index(LISTOFVAROFINTEREST->IndexOf(name));
  //FuncPlot(VAROFINTEREST[index].name, VAROFINTEREST[index].log, VAROFINTEREST[index].decrease);

}


////////////////////////////////////////////////////////////////////////////////////
void FuncPlot(string variable, bool logZ, bool decrease)
{

  //-- flags --------------------------------------------------------------
  bool doXSec(1);
  bool doNormalize(0);
  bool symmetricalErrors(1);
  int optionPlot7TeV(0);
  //-----------------------------------------------------------------------
  if ( energy == "7TeV"){
    Luminosity = 5051.;
    luminosityErr = 0.022;
    if ( energy == "7TeV" ) outputDirectory = "PNGFiles/FinalUnfold/";

  }

  gStyle->SetOptStat(0);
  gStyle->SetEndErrorSize(4);

  //  string energy = getEnergy();
  if ( !isMuon ) leptonFlavor = "DE";
  if (doXSec) doNormalize = 0 ;
  TH1::SetDefaultSumw2();
  string fileName = outputDirectory + leptonFlavor + "_" + energy +   "_unfolded_" + variable + "_histograms_" + unfAlg;
  if (doVarWidth) fileName += "_VarWidth";
  fileName += ".root";



  cout << " opening : " << fileName <<endl;
  TFile *f  = new TFile(fileName.c_str());
  //TFile *f2 = new TFile("PNGFiles/PDFSystematics.root");
  //TFile *f2 = new TFile("PDFSystematics_MadGraph_JetPtMin_30.root");
  //TFile *f3 = new TFile("PDFSystematics_Sherpa_JetPtMin_20.root");
  TFile *fPDFSystMad = NULL, *fPDFSystShe = NULL, *fPDFSystPow = NULL;
  if  ( energy == "7TeV") fPDFSystMad = new TFile("DMu_7TeV_PDFSystematics.root");
  else {
    string PDFname = "PDFSystFiles/PDFSyst_gen" + variable + ".root";
    fPDFSystMad = new TFile(PDFname.c_str());
    fPDFSystShe = new TFile(PDFname.c_str());
    fPDFSystPow = new TFile(PDFname.c_str());
    //fPDFSystMad = new TFile("HistoFiles/PDFSystematics_MadGraph.root");
    //fPDFSystShe = new TFile("HistoFiles/PDFSystematics_Sherpa.root");
    //fPDFSystPow = new TFile("HistoFiles/PDFSystematics_Powheg.root");
  }
    

  string genVariable = "gen" + variable;
  TH1D *data        = (TH1D*) f->Get("Data");
  TH1D *dataCentral = (TH1D*) f->Get("Central");
  TH1D *dataUnfWithSherpa = (TH1D*) f->Get("unfWithSherpa");
  TH1D *UnfErrors = (TH1D*) f->Get("unfWithSherpa");
  TH2D *covUnf      = (TH2D*) f->Get("MyToyCor") ;  // this choice has data, background and response uncertainties included
  TH1D *genMad      = (TH1D*) f->Get("genMad");
  TH1D *genShe      = (TH1D*) f->Get("genShe");
  TH1D *genPow      = (TH1D*) f->Get("genPow");
  TH1D *dataCentralOppAlgo = (TH1D*) f->Get("oppCentral");

  // now get histograms for DPS if 7 TeV
  const int NAddGEN = 5 ;
  int MarkerSel[NAddGEN] = {24, 25, 26, 27 , 28}; 
  TH1D *genDYAdd[NAddGEN];
  TH1D *genDYAddRatio[NAddGEN];
  string namesDYAdd[NAddGEN] = {"MadZ2MPIoff","MadZ2Star","MadZ2StarMPIoff","P84C","PowZjjMiNLO"};
  if ( energy == "7TeV"){
    for ( int i(0) ; i < NAddGEN; i++){
      string histoName = "gen" + namesDYAdd[i] ;
      genDYAdd[i] = (TH1D*) f->Get(histoName.c_str());
      genDYAdd[i]->SetTitle("");

      //		 genDYAddRatio[i] = (TH1D*) f->Get(histoName.c_str());
      //		 genDYAddRatio[i] ->Divide(dataCentral);
      //		 genDYAddRatio[i] = (TH1D*) dataCentral->Clone(namesDYAdd[i].c_str());
      //		 genDYAddRatio[i]->Divide(genDYAdd[i]);
    }
  }

  TH1D *PDFSystMad, *PDFSystShe, *PDFSystPow;
  PDFSystMad = (TH1D*) fPDFSystMad->Get(genVariable.c_str());
  PDFSystMad->SetTitle("");
  if (energy == "8TeV") {
    PDFSystShe = (TH1D*) fPDFSystShe->Get(genVariable.c_str());
    PDFSystShe->SetTitle("");
    PDFSystPow = (TH1D*) fPDFSystPow->Get(genVariable.c_str());
    PDFSystPow->SetTitle("");
  }
  //TH1D *PDFSystMad  =(TH1D*) genMad->Clone("");
  //PDFSystMad->Divide(genMad);


  int nSyst(7);
  TH1D *hSyst[7];
  hSyst[0]          = (TH1D*) f->Get("JECup");
  hSyst[1]          = (TH1D*) f->Get("JECdown");
  hSyst[2]          = (TH1D*) f->Get("PUup");
  hSyst[3]          = (TH1D*) f->Get("PUdown");
  hSyst[4]          = (TH1D*) f->Get("XSECup");
  hSyst[5]          = (TH1D*) f->Get("XSECdown");
  hSyst[6]          = (TH1D*) f->Get("JERup");
  string systNames[7] = {"JEC", "JEC", "PU", "PU", "XSEC", "XSEC", "JER"};
  string temp, tempTab;
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
    temp = "d#sigma/d" + shortVar;
    tempTab = "$\\frac{d\\sigma}{d" + shortVar + "}$";
    if (doNormalize) temp = "1/#sigma " + temp;
    else if (doXSec){
      temp += " [pb";
      tempTab += " \\tiny{$\\left[";
      if (unit != "" ){
        temp += "/" + unit;
        tempTab += "\\frac{ \\text{pb}}{" + unit + "}";
      }
      else tempTab += " \\text{pb}";
      temp += "]";
      tempTab += "\\right]$}";
    }
    //genMad->GetYaxis()->SetTitle(temp.c_str());
    //genShe->GetYaxis()->SetTitle(temp.c_str());
    genMad->GetYaxis()->SetTitle("");
    genShe->GetYaxis()->SetTitle("");
    genPow->GetYaxis()->SetTitle("");
  }

  cout << " We fetched all histograms" << endl;

  //if (!isMuon) outputDirectory = "PNGFiles/FinalDataUnfoldElectron/";
  string outputFileNamePNG = outputDirectory + leptonFlavor + "_" + energy + "_RunPlotting_";
  if (doXSec)         outputFileNamePNG += "XSec_";
  else if (doNormalize) outputFileNamePNG += "Normalized_";
  outputFileNamePNG += variable + "_" + unfAlg ;
  if (doVarWidth)  outputFileNamePNG += "_VarWidth";

  string fileNameTableCov = outputFileNamePNG + "_CorrelationMatrix.tex";
  string fileNameTable = outputFileNamePNG + ".tex";

  if ( energy == "7TeV" && optionPlot7TeV == 1) outputFileNamePNG += "_Comparison2";
  outputFileNamePNG += ".png";


  string title = data->GetTitle(); changeToLatexFormat(title);
  string sigmaTitle = temp; changeToLatexFormat(sigmaTitle); 
  string sigmaTitleTab = tempTab;
  string xtitle = data->GetXaxis()->GetTitle(); changeToLatexFormat(xtitle);
  myReplace(title, "1st", "$1^{\\text{st}}$");
  myReplace(title, "2nd", "$2^{\\text{nd}}$");
  myReplace(title, "3rd", "$3^{\\text{rd}}$");
  myReplace(title, "4th", "$4^{\\text{th}}$");
  myReplace(title, "5th", "$5^{\\text{th}}$");
  myReplace(title, "6th", "$6^{\\text{th}}$");
  myReplace(title, "p_T", "\\pt");
  myReplace(title, "p_{T}", "\\pt");
  myReplace(title, "H_T", "\\HT");
  myReplace(title, "H_{T}", "\\HT");
  myReplace(title, "GeV", "\\GeV");
  myReplace(xtitle, "p_T", "\\pt");
  myReplace(xtitle, "p_{T}", "\\pt");
  myReplace(xtitle, "H_T", "\\HT");
  myReplace(xtitle, "H_{T}", "\\HT");
  myReplace(xtitle, "GeV", "\\GeV");
  myReplace(xtitle, "[", "\\tiny{[");
  myReplace(xtitle, "]", "]}");
  myReplace(sigmaTitleTab, "GeV", "\\GeV");
  myReplace(sigmaTitleTab, "p_T", "\\pt");
  myReplace(sigmaTitleTab, "p_{T}", "\\pt");
  myReplace(sigmaTitleTab, "H_T", "\\HT");
  myReplace(sigmaTitleTab, "H_{T}", "\\HT");

////////////////////////  INITIALIZE TABLES FOR CROSS SECTION
  writeCorrTable(leptonFlavor, variable, title, fileNameTableCov ,(TH2D*) covUnf );
//covUnf->Draw("text");

////////////////////////  INITIALIZE TABLES FOR CROSS SECTION
  ofstream myFile(fileNameTable.c_str());
  myFile << "\\begin{table}[htb!]\\begin{center}" << endl;
  myFile << "\\caption{Differential cross section in " << title << " and uncertainties break down for the";
  if (leptonFlavor == "DMu") myFile << " muon decay channel.";  
  if (leptonFlavor == "DE") myFile << " electron decay channel.";  
  myFile << " }"<<endl;
  myFile << "\\scriptsize{" << endl; 
  myFile << "\\begin{tabular}{c|cccccccc}\n \\multicolumn{9}{c}{";
  if (leptonFlavor == "Muons" || leptonFlavor == "DMu")         myFile << " $Z\\rightarrow \\MM$: ";
  else if (leptonFlavor == "Electrons" || leptonFlavor == "DE") myFile << " $Z\\rightarrow \\EE$: ";
  myFile << title << "} \\\\" << endl;
  myFile << xtitle << " & ";
  myFile << sigmaTitleTab << " & stat & JES & PU & XSEC & JER & Unf. Meth. & tot \\\\ \\hline" << endl;

  const int nBins(dataCentral->GetNbinsX());
  cout << " central int:  " << variable << "   " << dataCentral->Integral(1, nBins) << " with under/over:  " << dataCentral->Integral(0, nBins+1) << endl;

  if (doXSec) doNormalize = false;
  if (doNormalize) doXSec = false;
  //----------------------------------------------------------------------//
  //    Rescale histograms to Luminosity and binWidth for cross-section   //
  //----------------------------------------------------------------------//
  if (doXSec){
    for (int i(1); i <= nBins; i++){
      double binW = data->GetBinWidth(i);
      //-- rescale the content
      data->SetBinContent(i, data->GetBinContent(i)*1./(Luminosity*binW));
      dataCentral->SetBinContent(i, dataCentral->GetBinContent(i)*1./(Luminosity*binW));
      dataUnfWithSherpa->SetBinContent(i, dataUnfWithSherpa->GetBinContent(i)*1./(Luminosity*binW));
      dataCentralOppAlgo->SetBinContent(i, dataCentralOppAlgo->GetBinContent(i)*1./(Luminosity*binW));
      genMad->SetBinContent(i, genMad->GetBinContent(i)*1./(Luminosity*binW));
      genShe->SetBinContent(i, genShe->GetBinContent(i)*1./(Luminosity*binW));
      genPow->SetBinContent(i, genPow->GetBinContent(i)*1./(Luminosity*binW));

      //-- rescale the errors
      data->SetBinError(i, data->GetBinError(i)*1./(Luminosity*binW));
      dataCentral->SetBinError(i, dataCentral->GetBinError(i)*1./(Luminosity*binW));
      dataUnfWithSherpa->SetBinError(i, dataUnfWithSherpa->GetBinError(i)*1./(Luminosity*binW));
      dataCentralOppAlgo->SetBinError(i, dataCentralOppAlgo->GetBinError(i)*1./(Luminosity*binW));
      genMad->SetBinError(i, genMad->GetBinError(i)*1./(Luminosity*binW));
      genShe->SetBinError(i, genShe->GetBinError(i)*1./(Luminosity*binW));
      genPow->SetBinError(i, genPow->GetBinError(i)*1./(Luminosity*binW));

      //-- rescale systematics
      for (int syst(0); syst < nSyst; syst++){
        hSyst[syst]->SetBinContent(i, hSyst[syst]->GetBinContent(i)*1./(Luminosity*binW));
        hSyst[syst]->SetBinError(i, hSyst[syst]->GetBinError(i)*1./(Luminosity*binW));
      }

      if ( energy == "7TeV"){
        for ( int j(0) ; j < NAddGEN; j++){
          genDYAdd[j]->SetBinContent(i, genDYAdd[j]->GetBinContent(i)*1./(Luminosity*binW)); 
          genDYAdd[j]->SetBinError(i,genDYAdd[j]->GetBinError(i)*1./(Luminosity*binW));
        }
      }

    }
  }
  //----------------------------------------------------------------------//

  //----------------------------------------------------------------------//
  //    Rescale histograms to unit area shape analysis                    //
  //----------------------------------------------------------------------//
  if (doNormalize) {
    cout << " Now normalizing " << endl;
    cout << genMad->GetBinContent(1) << "   " << genPow->GetBinContent(1)<< endl;
    data->Scale(1/data->Integral(1,nBins));
    dataCentral->Scale(1./dataCentral->Integral(1,nBins));
    dataUnfWithSherpa->Scale(1./dataUnfWithSherpa->Integral(1,nBins));
    dataCentralOppAlgo->Scale(1./dataCentralOppAlgo->Integral(1,nBins));
    genMad->Scale(1./genMad->Integral(1,nBins));
    genShe->Scale(1./genShe->Integral(1,nBins));
    genPow->Scale(1./genPow->Integral(1,nBins));
    cout << genMad->GetBinContent(1) << "   " <<genPow->GetBinContent(1)<< endl;
    for (int syst(0); syst < nSyst; syst++) hSyst[syst]->Scale( 1. / hSyst[syst]->Integral(1, nBins) );
    for ( int j(0) ; j < NAddGEN; j++) genDYAdd[j]->Scale( 1. /genDYAdd[j]->Integral(1, nBins) );
  }

  /// now do the ratio so other MC
  if ( energy == "7TeV"){
    for ( int i(0) ; i < NAddGEN; i++){
      genDYAddRatio[i] = (TH1D*) dataCentral->Clone(namesDYAdd[i].c_str());
      genDYAddRatio[i]->Divide(genDYAdd[i]);
      genDYAddRatio[i]->SetTitle("");
    }
  }
  //----------------------------------------------------------------------//

  //----------------------------------------------------------------------//
  //    Declare the necessary vector for the TGrahAsymmErrors             //
  //----------------------------------------------------------------------//
  double xCoor[nBins], yCoor[nBins], xErr[nBins], yStat[nBins], ySystDown[nBins], ySystUp[nBins];
  double xCoorMadRatio[nBins], yCoorMadRatio[nBins], yStatMadRatio[nBins], ySystDownMadRatio[nBins], ySystUpMadRatio[nBins];
  double xCoorSheRatio[nBins], yCoorSheRatio[nBins], yStatSheRatio[nBins], ySystDownSheRatio[nBins], ySystUpSheRatio[nBins];
  double xCoorPowRatio[nBins], yCoorPowRatio[nBins], yStatPowRatio[nBins], ySystDownPowRatio[nBins], ySystUpPowRatio[nBins];
  //----------------------------------------------------------------------//

  cout << "go to error computation " << endl;

  //double errorFromUnfold = getErrors(variable);
  //TH1D *UnfErrors = NULL;

  getErrors(dataCentral, dataUnfWithSherpa, UnfErrors);

  for (int bin(1); bin <= nBins; bin++){

    double binW = data->GetBinWidth(bin);
    double totalSystematicsUp(0.), totalSystematicsDown(0.);
    double totalStatistics(dataCentral->GetBinError(bin)); 
    double centralValue(dataCentral->GetBinContent(bin));
    double centralValueOppAlgo(dataCentralOppAlgo->GetBinContent(bin));

    myFile.precision(4);
    double xLow = dataCentral->GetBinLowEdge(bin);
    double xHigh = xLow + binW;
    if (fabs(xLow) < 0.00001) xLow = 0.;
    if (fabs(xHigh) < 0.00001) xHigh = 0.; 
    if (variable.find("ZNGoodJets_Zexc") != string::npos && bin > 1) myFile <<  "$= " << bin - 1 << " $& ";
    else if (variable.find("ZNGoodJets_Zinc") != string::npos && bin > 1) myFile <<  "$\\geq " << bin - 1 << " $& ";
    else if (variable.find("ZNGoodJets") == string::npos) myFile << xLow << "\\ -\\ " << xHigh << " & ";
    myFile.precision(3);
    if (variable.find("ZNGoodJets") != string::npos && bin > 1){
      myFile << centralValue;
      myFile.precision(2);
      myFile << "&  " << totalStatistics << " "; 
    }
    else if (variable.find("ZNGoodJets") == string::npos ){
      myFile << centralValue ; 
      myFile.precision(2);
      myFile << "&  " << totalStatistics << " "; 
    }
    double effSF = 0.01;
    if (doXSec && !doNormalize ){
      totalSystematicsUp += pow( pow((1.+effSF), 2) - 1 , 2 ) / ( pow(Luminosity*binW, 2) );
      totalSystematicsDown += pow( pow((1.+effSF), 2) - 1 , 2 ) / ( pow(Luminosity*binW, 2) );
    }
    if (doXSec && !doNormalize ){
      totalSystematicsUp +=  pow( luminosityErr , 2)  / ( pow(Luminosity*binW, 2) );
      totalSystematicsDown +=  pow( luminosityErr,2)  / ( pow(Luminosity*binW, 2) );
    }

    if (doNormalize){
      //totalSystematicsUp += pow( pow((1.+effSF), 2) - 1 , 2)/(pow(Luminosity*binW, 2));
      //totalSystematicsDown += pow( pow((1.+effSF), 2) - 1 , 2)/(pow(Luminosity*binW, 2));
    }
    //double diffUnfError = errorFromUnfold * centralValue; // error from unfolding different distributions 
    std::cout << "accessing result of my function" << std::endl;
    double diffUnfError = UnfErrors->GetBinContent(bin);
    totalSystematicsUp += diffUnfError * diffUnfError; 
    totalSystematicsDown += diffUnfError * diffUnfError;
    std::cout << "unf syst done" << std::endl;


    double mcValue(genMad->GetBinContent(bin));
    xCoor[bin-1]    = dataCentral->GetBinCenter(bin);
    xCoorMadRatio[bin-1]    = dataCentral->GetBinCenter(bin);
    xCoorSheRatio[bin-1]    = dataCentral->GetBinCenter(bin);
    xCoorPowRatio[bin-1]    = dataCentral->GetBinCenter(bin);
    yCoor[bin-1]    = centralValue; 
    //xErr[bin-1]     = 0.5 * dataCentral->GetBinWidth(bin);
    xErr[bin-1]     = 0.;
    yStat[bin-1]    = totalStatistics;
    yStatMadRatio[bin-1]    = totalStatistics/(genMad->GetBinContent(bin));
    yStatSheRatio[bin-1]    = totalStatistics/(genShe->GetBinContent(bin));
    yStatPowRatio[bin-1]    = totalStatistics/(genPow->GetBinContent(bin));

    double systUp(0.), systDown(0.);
    for (int syst(0); syst < nSyst; syst++){
      double difference(hSyst[syst]->GetBinContent(bin) - centralValue);
      if ( difference > 0  || syst == 6) totalSystematicsUp += difference * difference;
      if ( difference <= 0 || syst == 6) totalSystematicsDown += difference * difference;
      cout << "Name: " << systNames[syst] << " " << difference << endl; 
      if (syst % 2 == 0 ) systUp = difference; 
      if (syst % 2 == 1 || syst == 6) {
        systDown = difference;
        cout << " up: " << systUp << "  down: " << systDown << endl; 
        cout << " average: " << 0.5 * (fabs(systUp)+fabs(systDown)) << endl;
        if (variable.find("ZNGoodJets") != string::npos && bin > 1) myFile <<  " & " << 0.5*(fabs(systUp)+fabs(systDown)) << "  "; 
        else if (variable.find("ZNGoodJets") == string::npos) myFile <<  " & " << 0.5*(fabs(systUp)+fabs(systDown)) << "  "; 

      }
    } 

    // compute systematics between two unfolding methods
    double diffBayesSVD(centralValueOppAlgo - centralValue);
    totalSystematicsUp += fabs(diffBayesSVD * diffBayesSVD);
    totalSystematicsDown += fabs(diffBayesSVD * diffBayesSVD);
    if (variable.find("ZNGoodJets") != string::npos && bin > 1) myFile <<  " & " << sqrt(diffUnfError*diffUnfError + diffBayesSVD * diffBayesSVD) << "  "; 
    else if (variable.find("ZNGoodJets") == string::npos) myFile <<  " & " << sqrt(diffUnfError*diffUnfError + diffBayesSVD * diffBayesSVD) << "  "; 

    cout<<endl;

    if (symmetricalErrors){
      double avgError = 0.5*(fabs(totalSystematicsUp) + fabs(totalSystematicsDown));
      totalSystematicsUp = avgError;
      totalSystematicsDown = avgError;
    }

    ySystDown[bin-1]   = sqrt(totalStatistics * totalStatistics + totalSystematicsDown );
    ySystUp[bin-1]     = sqrt(totalStatistics * totalStatistics + totalSystematicsUp );
    if(genMad->GetBinContent(bin) > 0){
      cout << "Mad  " << genMad->GetBinContent(bin)<< endl;
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
    if(genPow->GetBinContent(bin) > 0){

      cout << "POW  " << genPow->GetBinContent(bin)<< endl;
      yCoorPowRatio[bin-1]       = centralValue / (genPow->GetBinContent(bin)); 
      ySystDownPowRatio[bin-1]   = sqrt(totalStatistics * totalStatistics + totalSystematicsDown )/(genPow->GetBinContent(bin));
      ySystUpPowRatio[bin-1]     = sqrt(totalStatistics * totalStatistics + totalSystematicsUp )/(genPow->GetBinContent(bin));
    }
    else{ 
      xCoorPowRatio[bin-1]       = -999.;
      yCoorPowRatio[bin-1]       = 0.1;
      ySystDownPowRatio[bin-1]   = 0.1; 
      ySystUpPowRatio[bin-1]     = 0.1; 
    }



    totalSystematicsUp   = sqrt(totalSystematicsUp);
    totalSystematicsDown = sqrt(totalSystematicsDown);


    if (variable.find("ZNGoodJet") != string::npos && bin > 1) myFile << "  & " << ySystUp[bin-1] << " \\\\" << endl;
    else if (variable.find("ZNGoodJet") == string::npos) myFile << "  & " << ySystUp[bin-1] << " \\\\" << endl;
  }
  myFile << "\\end{tabular}}" << endl; 
  myFile << "\\label{tab:" << leptonFlavor ; 
  if (doXSec)      myFile <<  "_UnfXSec_";
  else if (doNormalize)  myFile <<  "_UnfNormalized_";
  myFile << variable << "}"<< endl;
  myFile << "\\end{center}"  << endl;
  myFile << "\\end{table}" << endl;
  cout << " finished error computation " << endl;




  //////////////////////////////////////////////  NOW START PLOTTING PART  


  TGraphAsymmErrors *grCentralStat         = new TGraphAsymmErrors(nBins, xCoor, yCoor, xErr, xErr, yStat, yStat);
  TGraphAsymmErrors *grCentralStatMadRatio = new TGraphAsymmErrors(nBins, xCoorMadRatio, yCoorMadRatio, xErr, xErr, yStatMadRatio, yStatMadRatio);
  TGraphAsymmErrors *grCentralStatSheRatio = new TGraphAsymmErrors(nBins, xCoorSheRatio, yCoorSheRatio, xErr, xErr, yStatSheRatio, yStatSheRatio);
  TGraphAsymmErrors *grCentralStatPowRatio = new TGraphAsymmErrors(nBins, xCoorPowRatio, yCoorPowRatio, xErr, xErr, yStatPowRatio, yStatPowRatio);
  TGraphAsymmErrors *grCentralSystMadRatio = new TGraphAsymmErrors(nBins, xCoorMadRatio, yCoorMadRatio, xErr, xErr, ySystDownMadRatio, ySystUpMadRatio);
  TGraphAsymmErrors *grCentralSystSheRatio = new TGraphAsymmErrors(nBins, xCoorSheRatio, yCoorSheRatio, xErr, xErr, ySystDownSheRatio, ySystUpSheRatio);
  TGraphAsymmErrors *grCentralSystPowRatio = new TGraphAsymmErrors(nBins, xCoorPowRatio, yCoorPowRatio, xErr, xErr, ySystDownPowRatio, ySystUpPowRatio);
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


  TCanvas *can = new TCanvas(variable.c_str(), variable.c_str(), 600, 900);
  can->cd();
  TPad *pad1 = new TPad("pad1","pad1", 0, 0.5, 1, 1); 
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

  cout << "line 0 " << endl;
  MineYMax =  genMad->GetMaximum();
  if ( energy == "7TeV") MineYMax = genDYAdd[0] ->GetMaximum();

  cout << "line 0 " << endl;

  grCentralSyst->GetXaxis()->SetLimits(lowerLimit, genMad->GetXaxis()->GetXmax());
  genMad->SetFillStyle(3001);
  genMad->SetFillColor(kAzure-9);
  genMad->SetLineColor(kAzure-9);
  genMad->GetYaxis()->SetTitleSize(0.04);
  genMad->GetYaxis()->SetTitleOffset(1.45);
  genMad->GetYaxis()->SetLabelSize(0.04);
  if (logZ) genMad->SetMaximum(MineYMax*5.);
  else genMad->SetMaximum(MineYMax*1.5);
  genMad->SetTitle("");

  cout << "line 1 " << endl;

  if ( energy == "7TeV")  genDYAdd[0] ->DrawCopy("P");
  genMad->DrawCopy("E2");
  if (logZ) genMad->SetMaximum(genMad->GetMaximum()*3);
  if (( energy == "7TeV" && optionPlot7TeV == 0) || energy == "8TeV" ){
    genShe->SetFillStyle(3002);
    genShe->SetFillColor(kOrange-3);
    genShe->SetLineColor(kOrange-3);
    genShe->DrawCopy("E2 same");
    genPow->SetFillStyle(3002);
    genPow->SetFillColor(kViolet-6);
    genPow->SetLineColor(kViolet-6);
  }
  genPow->DrawCopy("E2 same");
  genMad->DrawCopy("E2 same");

  cout << "line 2 " << endl;
  grCentralSyst->Draw("P same");
  grCentralStat->Draw("P same");
  // set the legend
  double xLowLeg(0.57), xHighLeg(0.98);
  if (!decrease){
    xLowLeg = 0.14;
    xHighLeg = 0.55;
  }
  TLegend *legend = new TLegend(xLowLeg, 0.46, xHighLeg, 0.91);
  legend->SetHeader("  ");
  legend->SetFillStyle(0);
  legend->SetBorderSize(0);
  legend->AddEntry(genMad, "MG+Py6 at NNLO Stat. Err.", "pf");
  if (( energy == "7TeV" && optionPlot7TeV == 0) || energy == "8TeV" ){
    legend->AddEntry(genShe, "Sherpa2 at NLO Stat. Err.", "pf");
    legend->AddEntry(genPow, "Powheg 1jet at NLO Stat. Err.", "pf");
  }
  if ( energy == "7TeV" ){
    int startI =  NAddGEN - 1 ; 
    int endI   =  NAddGEN ;
    if ( optionPlot7TeV == 1 ) {startI = 0; endI = NAddGEN - 1 ;}


    // here we should set style for following histgrams
    //for ( int i(0) ; i < NAddGEN ; i++){
    for ( int i(startI) ; i < endI ; i++){
      genDYAdd[i] ->SetMarkerStyle(25+i);
      genDYAdd[i] ->DrawCopy("P same"); 
      legend->AddEntry(genDYAdd[i], namesDYAdd[i].c_str() , "P");	

    }
  }
  // and at the end DATA
  legend->AddEntry(grCentralStat, "Data Stat. Err.", "pl");
  legend->AddEntry(grCentralSyst, "Data Tot. Err.", "l");
  grCentralSyst->Draw("P same");
  grCentralStat->Draw("P same");
  if (leptonFlavor == "DMu") legend->AddEntry((TObject*)0, "Z #rightarrow #mu#mu", "");
  else if (leptonFlavor == "DE") legend->AddEntry((TObject*)0, "Z #rightarrow ee", "");
  pad1->Draw();


  legend->Draw();





  TLatex *variableLabel = new TLatex();
  //variableLabel->SetTextSize(0.035);
  variableLabel->SetTextSize(0.04);
  variableLabel->SetTextFont(42);
  variableLabel->SetLineWidth(2);
  variableLabel->SetTextColor(kBlack);
  variableLabel->SetNDC();
  variableLabel->SetTextAlign(21);
  variableLabel->DrawLatex(0.54,0.97,data->GetTitle());

  TLatex *cmsColl = new TLatex();
  //cmsColl->SetTextSize(0.025);
  cmsColl->SetTextSize(0.04);
  cmsColl->SetTextFont(42);
  cmsColl->SetLineWidth(2);
  cmsColl->SetTextColor(kBlack);
  cmsColl->SetNDC();
  cmsColl->SetTextAlign(11);
  cmsColl->DrawLatex(0.11,0.905, "CMS Preliminary");

  TLatex *sqrtXTeV = new TLatex();
  //sqrtXTeV->SetTextSize(0.025);
  sqrtXTeV->SetTextSize(0.04);
  sqrtXTeV->SetTextFont(42);
  sqrtXTeV->SetLineWidth(2);
  sqrtXTeV->SetTextColor(kBlack);
  sqrtXTeV->SetNDC();
  sqrtXTeV->SetTextAlign(21);
  if (energy == "7TeV") sqrtXTeV->DrawLatex(0.54,0.905, "#sqrt{s} = 7 TeV");
  if (energy == "8TeV") sqrtXTeV->DrawLatex(0.54,0.905, "#sqrt{s} = 8 TeV");

  TLatex *intLumi = new TLatex();
  //intLumi->SetTextSize(0.022);
  intLumi->SetTextSize(0.03);
  intLumi->SetTextFont(42);
  intLumi->SetLineWidth(2);
  intLumi->SetTextColor(kBlack);
  intLumi->SetNDC();
  intLumi->SetTextAlign(31);
  if (energy == "7TeV") intLumi->DrawLatex(0.98,0.91, "#int L dt = 5.05 fb^{-1}");
  if (energy == "8TeV") intLumi->DrawLatex(0.98,0.91, "#int L dt = 19.8 fb^{-1}");

  TLatex *ytitle = new TLatex();
  ytitle->SetTextSize(0.05);
  ytitle->SetTextFont(42);
  ytitle->SetLineWidth(2);
  ytitle->SetTextColor(kBlack);
  ytitle->SetNDC();
  ytitle->SetTextAlign(33);
  ytitle->SetTextAngle(90);
  if (doXSec || doNormalize) ytitle->DrawLatex(0.008,0.91,temp.c_str());
  pad1->cd();

  cout << " LETS PLOTS 2nd  PAT " << endl;
  //// 	NEW TPAD
  can->cd();
  TPad *pad2 = new TPad("pad2","pad2", 0, 0.35, 1, 0.5);
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
  PDFSystMad->SetFillColor(kGreen-3);
  PDFSystMad->SetLineColor(kGreen-3);
  PDFSystMad->SetFillStyle(3001);
  PDFSystMad->DrawCopy("E2 same");

  grCentralSystMadRatio->Draw("P same");
  grCentralStatMadRatio->Draw("P same");
  TLatex *latexLabel = new TLatex();
  latexLabel->SetTextSize(0.15);
  latexLabel->SetTextFont(42);
  latexLabel->SetLineWidth(2);
  latexLabel->SetTextColor(kAzure-5);
  latexLabel->SetNDC();
  if (( energy == "7TeV" && optionPlot7TeV == 0) || energy == "8TeV" ) latexLabel->DrawLatex(0.15,0.09,"Madgraph");
  else latexLabel->DrawLatex(0.15,0.09,"Madgraph Z2");
  TLegend *legend2 = new TLegend(0.75,0.85,0.9,0.95);
  legend2->SetFillStyle(0);
  legend2->SetBorderSize(0);
  legend2->AddEntry(PDFSystMad, " PDF Syst.", "f");
  legend2->Draw();


  if ( energy == "7TeV" && optionPlot7TeV == 1  ){
    for ( int i(0) ; i < 1 ; i++){
      genDYAddRatio[i] ->SetMarkerStyle(25+i);
      genDYAddRatio[i] ->DrawCopy("P same");
    }
  }

  pad2->Draw();
  pad2->cd();

  can->cd();
  cout << " LETS PLOTS 3rd  PAT " << endl;
  TPad *pad3 = new TPad("pad3","pad3", 0, 0.2, 1, 0.35);
  pad3->SetTopMargin(0);
  pad3->SetBottomMargin(0.0);
  pad3->SetRightMargin(0.02);
  pad3->SetLeftMargin(0.11);
  pad3->SetGridy();
  pad3->SetTicks();
  pad3->Draw();
  pad3->cd();
  if (( energy == "7TeV" && optionPlot7TeV == 0) || energy == "8TeV" ) {
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
    grCentralSystSheRatio->GetYaxis()->SetTitleSize(0.135);
    grCentralSystSheRatio->GetYaxis()->SetTitleOffset(0.44);
    grCentralSystSheRatio->GetYaxis()->CenterTitle();
    grCentralSystSheRatio->GetYaxis()->SetLabelSize(0.12);
    grCentralSystSheRatio->GetYaxis()->SetLabelOffset(0.01);
    grCentralSystSheRatio->SetTitle("");
    grCentralSystSheRatio->GetXaxis()->SetTickLength(0.06);
    grCentralSystSheRatio->Draw("AP");
    grCentralStatSheRatio->Draw("P same");
    PDFSystShe->SetFillColor(kGreen-3);
    PDFSystShe->SetLineColor(kGreen-3);
    PDFSystShe->SetFillStyle(3001);
    PDFSystShe->DrawCopy("E2 same");
    grCentralSystSheRatio->Draw("P same");
    grCentralStatSheRatio->Draw("P same");
  }
  else{
    genDYAddRatio[2]->GetXaxis()->SetLimits(lowerLimit, genMad->GetXaxis()->GetXmax());
    genDYAddRatio[2]->GetXaxis()->SetTitle(genMad->GetXaxis()->GetTitle());
    genDYAddRatio[2]->GetXaxis()->SetTitleSize(0.1);
    genDYAddRatio[2]->GetXaxis()->SetTitleOffset(1.05);
    genDYAddRatio[2]->GetXaxis()->SetLabelSize(0.1);
    genDYAddRatio[2]->GetXaxis()->SetLabelOffset(0.014);
    genDYAddRatio[2]->GetYaxis()->SetRangeUser(0.41,1.59);
    genDYAddRatio[2]->GetYaxis()->SetTitle("Data / MC");
    genDYAddRatio[2]->GetYaxis()->SetTitleSize(0.135);
    genDYAddRatio[2]->GetYaxis()->SetTitleOffset(0.44);
    genDYAddRatio[2]->GetYaxis()->CenterTitle();
    genDYAddRatio[2]->GetYaxis()->SetLabelSize(0.12);
    genDYAddRatio[2]->GetYaxis()->SetLabelOffset(0.01);
    genDYAddRatio[2]->SetTitle("");
    genDYAddRatio[2]->GetXaxis()->SetTickLength(0.06);



    genDYAddRatio[2] ->DrawCopy("P");
    for ( int i(1) ; i < 3 ; i++){
      genDYAddRatio[i] ->SetMarkerStyle(25+i);
      genDYAddRatio[i] ->DrawCopy("P same");
    }




  }

  TLatex *latexLabel1 = new TLatex();
  latexLabel1->SetTextSize(0.15);
  latexLabel1->SetTextFont(42);
  latexLabel1->SetLineWidth(2);
  latexLabel1->SetTextColor(kOrange+2);
  latexLabel1->SetNDC();
  //  latexLabel1->DrawLatex(0.15,0.09,"Sherpa");
  if (( energy == "7TeV" && optionPlot7TeV == 0) || energy == "8TeV" ) latexLabel1->DrawLatex(0.15,0.09,"Sherpa2");
  else latexLabel1->DrawLatex(0.15,0.09,"Madgraph Z2Star");
  pad3->Draw();
  pad3->cd();
  can->cd();

  // yet another pad
  cout << " AND THE LAST PAD " << endl;
  TPad *pad4 = new TPad("pad4","pad4", 0, 0., 1, 0.2);
  pad4->SetTopMargin(0);
  pad4->SetBottomMargin(0.25);
  pad4->SetRightMargin(0.02);
  pad4->SetLeftMargin(0.11);
  pad4->SetGridy();
  pad4->SetTicks();
  pad4->Draw();
  pad4->cd();
  if (( energy == "7TeV" && optionPlot7TeV == 0) || energy == "8TeV" ) {
    grCentralStatPowRatio->SetMarkerStyle(20);
    grCentralStatPowRatio->SetMarkerSize(0.8);
    grCentralStatPowRatio->SetMarkerColor(kBlack);
    grCentralStatPowRatio->SetLineColor(kBlack);
    grCentralSystPowRatio->SetLineColor(kRed+1);
    grCentralSystPowRatio->SetMarkerStyle(20);
    grCentralSystPowRatio->SetMarkerSize(0.8);
    grCentralSystPowRatio->GetXaxis()->SetLimits(lowerLimit, genMad->GetXaxis()->GetXmax());
    grCentralSystPowRatio->GetXaxis()->SetTitle(genMad->GetXaxis()->GetTitle());
    grCentralSystPowRatio->GetXaxis()->SetTitleSize(0.1);
    grCentralSystPowRatio->GetXaxis()->SetTitleOffset(1.05);
    grCentralSystPowRatio->GetXaxis()->SetLabelSize(0.1);
    grCentralSystPowRatio->GetXaxis()->SetLabelOffset(0.014);
    grCentralSystPowRatio->GetYaxis()->SetRangeUser(0.41,1.59);
    grCentralSystPowRatio->GetYaxis()->SetTitle("Data / MC");
    grCentralSystPowRatio->GetYaxis()->SetTitleSize(0.1);
    grCentralSystPowRatio->GetYaxis()->SetTitleOffset(0.6);
    grCentralSystPowRatio->GetYaxis()->CenterTitle();
    grCentralSystPowRatio->GetYaxis()->SetLabelSize(0.09);
    grCentralSystPowRatio->GetYaxis()->SetLabelOffset(0.012);
    grCentralSystPowRatio->SetTitle("");
    grCentralSystPowRatio->GetXaxis()->SetTickLength(0.06);
    grCentralSystPowRatio->Draw("AP");
    grCentralStatPowRatio->Draw("P same");
    PDFSystPow->SetFillColor(kGreen-3);
    PDFSystPow->SetLineColor(kGreen-3);
    PDFSystPow->SetFillStyle(3001);
    PDFSystPow->DrawCopy("E2 same");
    grCentralSystPowRatio->Draw("P same");
    grCentralStatPowRatio->Draw("P same");
    TLatex *latexLabel2 = new TLatex();
    latexLabel2->SetTextSize(0.11);
    latexLabel2->SetTextFont(42);
    latexLabel2->SetLineWidth(2);
    latexLabel2->SetTextColor(kViolet-6);
    latexLabel2->SetNDC();
    latexLabel2->DrawLatex(0.15,0.3,"Powheg");
    if ( energy == "7TeV"){
      for ( int i(NAddGEN - 1) ; i < NAddGEN  ; i++){
        genDYAddRatio[i] ->SetMarkerStyle(25+i);
        genDYAddRatio[i] ->DrawCopy("P same");
      }
    }
  }
  else{
    genDYAddRatio[3]->GetXaxis()->SetLimits(lowerLimit, genMad->GetXaxis()->GetXmax());
    genDYAddRatio[3]->GetXaxis()->SetTitle(genMad->GetXaxis()->GetTitle());
    genDYAddRatio[3]->GetXaxis()->SetTitleSize(0.1);
    genDYAddRatio[3]->GetXaxis()->SetTitleOffset(1.05);
    genDYAddRatio[3]->GetXaxis()->SetLabelSize(0.1);
    genDYAddRatio[3]->GetXaxis()->SetLabelOffset(0.014);
    genDYAddRatio[3]->GetYaxis()->SetRangeUser(0.41,1.59);
    genDYAddRatio[3]->GetYaxis()->SetTitle("Data / MC");
    genDYAddRatio[3]->GetYaxis()->SetTitleSize(0.135);
    genDYAddRatio[3]->GetYaxis()->SetTitleOffset(0.44);
    genDYAddRatio[3]->GetYaxis()->CenterTitle();
    genDYAddRatio[3]->GetYaxis()->SetLabelSize(0.12);
    genDYAddRatio[3]->GetYaxis()->SetLabelOffset(0.01);
    genDYAddRatio[3]->SetTitle("");
    genDYAddRatio[3]->GetXaxis()->SetTickLength(0.06);



    genDYAddRatio[3] ->DrawCopy("P");
    for ( int i(3) ; i < 4 ; i++){
      genDYAddRatio[i] ->SetMarkerStyle(25+i);
      genDYAddRatio[i] ->DrawCopy("P same");
    }


    TLatex *latexLabel2 = new TLatex();
    latexLabel2->SetTextSize(0.11);
    latexLabel2->SetTextFont(42);
    latexLabel2->SetLineWidth(2);
    latexLabel2->SetTextColor(kViolet-6);
    latexLabel2->SetNDC();
    latexLabel2->DrawLatex(0.15,0.3,"Pythia8 4C");


  }



  pad4->Draw();
  pad4->cd();
  can->cd();

  //end of yet another pad

  cout << " plot is done " << endl;

  can->Print(outputFileNamePNG.c_str());

  myFile.close();
  cout << " print plot to file " << endl;
  f->Close();
  if (fPDFSystMad->IsOpen()) fPDFSystMad->Close(); 
  if (fPDFSystPow->IsOpen()) fPDFSystPow->Close(); 
  if (fPDFSystShe->IsOpen()) fPDFSystShe->Close(); 
  }

  ////////////////////////////////////////////////////////////////////////////////////
  /*
  double getErrors(string variable){
    //string energy = getEnergy();
    if ( !isMuon ) leptonFlavor = "DE";
    gStyle->SetOptStat(0);
    string filesIN[8] = {"ZNGoodJets_Zexc","JetsHT","JetPt","JetEta","SpTJets_Zinc2jet","SpT_Zinc2jet","PHI_Zinc2jet","SPhi_Zinc2jet"};
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

    double values[8] ;
    double min = 100000000.;
    double max = 0.;
    double mean = 0.;

    int NumberOfVarUsed = 3;
    //if ( num == 2 && variable.find("Zinc2")!=std::string::npos ) NumberOfVarUsed = 7;
    if ( num == 2  ) NumberOfVarUsed = 7;

    for (int i(0); i <= NumberOfVarUsed; i++){ 
      string variableT;
      if ( i == 0 || i > 3 ) variableT = filesIN[i];
      else if (i  == 1 ) variableT = filesIN[i] + "_Zinc" +  ss.str()  + "jet";
      else variableT = addNames[num - 1]+filesIN[i] + "_Zinc" +  ss.str()  + "jet"; 
      string fileName = outputDirectory + leptonFlavor + "_" + energy +   "_unfolded_" + variableT + "_histograms_" + unfAlg ;
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
      f->Close();
    }

    // for exclusive stuff things need to be subtracted
    if ( num == 2 && variable.find("Zexc2")!=std::string::npos ) {
      //NumberOfVarUsed = 3;
      for ( int k = 1 ; k < 33 ; k++) cout << "tu sam" <<endl;
      num++;  
      stringstream ss2;   ss2 << num;
      string filesIN2[8] = {"ZNGoodJets_Zexc","JetsHT","JetPt","JetEta","SpTJets_Zexc2jet","SpT_Zexc2jet","PHI_Zexc2jet","SPhi_Zexc2jet"};
      double values2[8] ;
      min = 100000000.;
      max = 0.;


      for (int i(0); i <= NumberOfVarUsed; i++){
        string variableT;
        if ( i == 0 || i > 3 ) variableT = filesIN2[i];
        else if (i  == 1 ) variableT = filesIN2[i] + "_Zinc" +  ss2.str()  + "jet";
        else variableT = addNames[num - 1]+filesIN2[i] + "_Zinc" +  ss2.str()  + "jet";
        string fileName = outputDirectory + leptonFlavor + "_" + energy +   "_unfolded_" + variableT + "_histograms_" + unfAlg ;
        if (doVarWidth) fileName += "_VarWidth";
        fileName +=".root";
        cout << " opening : " <<fileName <<endl;
        TFile *f  = new TFile(fileName.c_str());
        TH1D *dataCentral = (TH1D*) f->Get("Central");
        if ( i == 0 ) values2[i] = dataCentral ->Integral(num+1,dataCentral->GetNbinsX()+1)    ;
        else values2[i] = dataCentral ->Integral(1,dataCentral->GetNbinsX()+1)   ;

        if ( i > 3 ) values[i] = values2[i];
        else values[i] -= values2[i];

        cout << " grabbing central unfolds for errors  : "  <<  values[i] << endl;
        if (values[i] < min )  min = values[i] ;
        if (values[i] > max )  max = values[i] ;
        mean +=  values[i] ;
        f->Close();
      }


    }




    cout << " Envelope error " << (max-min)/2 << " min : " << min << " max " << max << endl;
    return ((max-min)/2)/(mean/4);

  }
*/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void getErrors(const TH1D * dataCentral, const TH1D * dataUnfWithSherpa, TH1D* UnfErrors)
{
    std::cout << "Entering my function" << std::endl;
    TH1D *unfWithMad = (TH1D*) dataCentral->Clone();
    TH1D *unfWithSherpa = (TH1D*) dataUnfWithSherpa->Clone();

    int nBins = unfWithMad->GetNbinsX();
    for (int i(0); i <= nBins; i++) {
        double diff = fabs(unfWithMad->GetBinContent(i) - unfWithSherpa->GetBinContent(i));
        UnfErrors->SetBinContent(i, diff);
        UnfErrors->SetBinError(i, 0);
    }
    std::cout << "done with my function" << std::endl;

}
