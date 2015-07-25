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
#include "getFilesAndHistograms.h"
#include "fileNames.h"
#include "unfoldingFunctions.h"
#include "setTDRStyle.h"
#include "UnfoldingError.h"
#include "tableFunctions.h"
#include "makeZJetsPlots.h"
#include "variablesOfInterestVarWidth.h"

void FuncPlot(string  variable = "SpTJets_Zinc2jet", bool log = 0, bool decrease = 0);
TH1D* getCovErrors(const TH1D *data, const TH2D *cov);


double Luminosity(19549.);
//double Luminosity(4950.);
double luminosityErr = 0.044;
string energy = "8TeV";//getEnergy();
bool isMuon = 0;
string leptonFlavor = "DMu";
string unfAlg = "Bayes";
bool doVarWidth = true ;
/// define names od output png files and tex
string outputDirectory = "PNGFiles/FinalUnfold_30_1000_Toys/";


void runPlotting3RatiosCommon()
{
    setTDRStyle();
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0.5);
    gStyle->SetPadGridX(0);
    gStyle->SetPadGridY(0);

    for (int i(0); i < 1/*NVAROFINTERESTZJETS*/; i++){
        for (int j(0); j < 2; j++){
            isMuon = j;
            FuncPlot(VAROFINTERESTZJETS[i].name, VAROFINTERESTZJETS[i].log, VAROFINTERESTZJETS[i].decrease);
        }
    }
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

    if (!isMuon) leptonFlavor = "DE";
    if (doXSec) doNormalize = 0;
    TH1::SetDefaultSumw2();
    string fileName = outputDirectory + leptonFlavor + "_" + energy + "_unfolded_" + variable + "_histograms_" + unfAlg;
    if (doVarWidth) fileName += "_VarWidth";
    fileName += ".root";



    cout << " opening : " << fileName <<endl;
    TFile *f  = new TFile(fileName.c_str());
    TFile *fPDFSyst = NULL;
    string PDFname = "PDFSystFiles/PDFSyst_gen" + variable + ".root";
    fPDFSyst = new TFile(PDFname.c_str());


    string genVariable = "gen" + variable;
    TH1D *data               = (TH1D*) f->Get("Data");
    TH1D *dataCentral        = (TH1D*) f->Get("Central");
    TH1D *dataUnfWithSherpa  = (TH1D*) f->Get("unfWithSherpa");
    TH1D *dataCentralOppAlgo = (TH1D*) f->Get("oppCentral");
    TH1D *genMad             = (TH1D*) f->Get("genMad");
    TH1D *genShe             = (TH1D*) f->Get("genShe");
    TH1D *genPow             = (TH1D*) f->Get("genPow");

    TH1D *dataPUup           = (TH1D*) f->Get("PUup");
    TH1D *dataPUdown         = (TH1D*) f->Get("PUdown");
    TH1D *dataJER            = (TH1D*) f->Get("JERup");
    TH2D *covUnf             = (TH2D*) f->Get("MyToyCor") ;  // this choice has data, background and response uncertainties included
    TH2D *myToyStatCov       = (TH2D*) f->Get("MyToyCov");
    TH2D *myToyJESCov        = (TH2D*) f->Get("MyToyJESCov");
    TH2D *myToyPUCov         = (TH2D*) f->Get("MyToyPU2Cov");
    TH2D *myToyXSECCov       = (TH2D*) f->Get("MyToyXSECCov");
    TH2D *myToyJERCov        = (TH2D*) f->Get("MyToyJER2Cov");
    TH2D *myToyEFFCov        = (TH2D*) f->Get("MyToyEFFCov");

    TH1D *dataUNF = getErrors(dataCentral, dataUnfWithSherpa);
    TH1D *dataPU = getPUErrors(dataCentral, dataPUup, dataPUdown);
    TH1D *dataJES = getCovErrors(dataCentral, myToyJESCov); 
    TH1D *dataXSEC = getCovErrors(dataCentral, myToyXSECCov); 
    TH1D *dataEFF = getCovErrors(dataCentral, myToyEFFCov);


    // now get histograms for DPS if 7 TeV
    const int NAddGEN = 5;
    int MarkerSel[NAddGEN] = {24, 25, 26, 27 , 28}; 
    TH1D *genDYAdd[NAddGEN];
    TH1D *genDYAddRatio[NAddGEN];
    string namesDYAdd[NAddGEN] = {"MadZ2MPIoff","MadZ2Star","MadZ2StarMPIoff","P84C","PowZjjMiNLO"};

    TH1D *hPDFSyst = NULL;
    hPDFSyst = (TH1D*) fPDFSyst->Get(genVariable.c_str());
    hPDFSyst->SetTitle("");

    int nSyst(6);
    TH1D *hSyst[6];
    //hSyst[0]          = (TH1D*) f->Get("JECup");
    //hSyst[1]          = (TH1D*) f->Get("JECdown");
    //hSyst[2]          = (TH1D*) f->Get("PUup");
    //hSyst[3]          = (TH1D*) f->Get("PUdown");
    //hSyst[4]          = (TH1D*) f->Get("XSECup");
    //hSyst[5]          = (TH1D*) f->Get("XSECdown");
    //hSyst[6]          = (TH1D*) f->Get("JERup");
    hSyst[0] = (TH1D*) dataJES->Clone(); hSyst[0]->SetDirectory(0);
    hSyst[1] = (TH1D*) dataPU->Clone();  hSyst[1]->SetDirectory(0);
    hSyst[2] = (TH1D*) dataXSEC->Clone(); hSyst[2]->SetDirectory(0);
    hSyst[3] = (TH1D*) dataJER->Clone(); hSyst[3]->SetDirectory(0);
    hSyst[4] = (TH1D*) dataUNF->Clone(); hSyst[4]->SetDirectory(0);
    hSyst[5] = (TH1D*) dataEFF->Clone(); hSyst[5]->SetDirectory(0);


    //string systNames[7] = {"JEC", "JEC", "PU", "PU", "XSEC", "XSEC", "JER"};
    string systNames[6] = {"JES", "PU", "XSEC", "JER", "UNF", "EFF"};
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
        string shortVarLatex = shortVar;  // changeToLatexFormat(shortVarLatex);
        myReplace(shortVarLatex, "#", "\\");
        tempTab = "$\\frac{d\\sigma}{d" + shortVarLatex + "}$";
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

    string outputFileNamePNG = outputDirectory + leptonFlavor + "_" + energy + "_RunPlotting_";
    if (doXSec)         outputFileNamePNG += "XSec_";
    else if (doNormalize) outputFileNamePNG += "Normalized_";
    outputFileNamePNG += variable + "_" + unfAlg ;
    if (doVarWidth)  outputFileNamePNG += "_VarWidth";

    string fileNameTableCov = outputFileNamePNG + "_CorrelationMatrix.tex";
    string fileNameTable = outputFileNamePNG + ".tex";

    outputFileNamePNG += ".png";


    string title = data->GetTitle(); changeToLatexFormat(title);
    myReplace(title, "1 2", "1");
    myReplace(title, "2 2", "2");
    string sigmaTitle = temp; changeToLatexFormat(sigmaTitle); 
    string sigmaTitleTab = tempTab; //changeToLatexFormat(sigmaTitleTab);
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
        for (int syst(0); syst < nSyst; syst++) hSyst[syst]->Scale(1./ hSyst[syst]->Integral(1, nBins));
        for (int j(0); j < NAddGEN; j++) genDYAdd[j]->Scale(1./genDYAdd[j]->Integral(1, nBins));
    }
    //----------------------------------------------------------------------//

    //----------------------------------------------------------------------//
    //    Declare the necessary vector for the TGrahAsymmErrors             //
    //----------------------------------------------------------------------//
    double xCoor[nBins], yCoor[nBins], xErr[nBins], yStat[nBins], ySystDown[nBins], ySystUp[nBins];
    //----------------------------------------------------------------------//

    cout << "go to error computation " << endl;

    for (int bin(1); bin <= nBins; bin++) {

        double binW = data->GetBinWidth(bin);
        double totalSystematicsUp(0.), totalSystematicsDown(0.);
        double totalStatistics(dataCentral->GetBinError(bin)); 
        std::cout << bin << " " << totalStatistics << std::endl;
        double centralValue(dataCentral->GetBinContent(bin));
        double centralValueOppAlgo(dataCentralOppAlgo->GetBinContent(bin));


        //double diffUnfError = errorFromUnfold * centralValue; // error from unfolding different distributions 
        std::cout << "accessing result of my function" << std::endl;
        //double diffUnfError = UnfErrors->GetBinContent(bin);
        //totalSystematicsUp += diffUnfError * diffUnfError; 
        //totalSystematicsDown += diffUnfError * diffUnfError;
        std::cout << "unf syst done" << std::endl;

        myFile.precision(4);
        double xLow = dataCentral->GetBinLowEdge(bin);
        double xHigh = xLow + binW;
        if (fabs(xLow) < 0.00001) xLow = 0.;
        if (fabs(xHigh) < 0.00001) xHigh = 0.; 
        if (variable.find("ZNGoodJets_Zexc") != string::npos && bin > 1) {
            myFile <<  "$= " << bin - 1 << " $& ";
        }
        else if (variable.find("ZNGoodJets_Zinc") != string::npos && bin > 1) {
            myFile <<  "$\\geq " << bin - 1 << " $& ";
        }
        else if (variable.find("ZNGoodJets") == string::npos) {
            myFile << xLow << "\\ -\\ " << xHigh << " & ";
        }
        myFile.precision(3);
        if (variable.find("ZNGoodJets") != string::npos && bin > 1) {
            myFile << centralValue;
            myFile.precision(2);
            myFile << "&  " << totalStatistics << " "; 
        }
        else if (variable.find("ZNGoodJets") == string::npos ) {
            myFile << centralValue; 
            myFile.precision(2);
            myFile << "&  " << totalStatistics << " "; 
        }

        //double effSF = 0.01;
        //if (doXSec && !doNormalize){
        //    totalSystematicsUp += pow( pow((1. + effSF), 2) - 1, 2 ) / pow(Luminosity*binW, 2);
        //    totalSystematicsDown += pow( pow((1. + effSF), 2) - 1, 2 ) / pow(Luminosity*binW, 2);
        //}
        //if (doXSec && !doNormalize){
        //    totalSystematicsUp +=  pow(luminosityErr, 2) / pow(Luminosity*binW, 2);
        //    totalSystematicsDown +=  pow(luminosityErr, 2) / pow(Luminosity*binW, 2);
        //}

        //if (doNormalize){
        //    //totalSystematicsUp += pow( pow((1.+effSF), 2) - 1 , 2)/(pow(Luminosity*binW, 2));
        //    //totalSystematicsDown += pow( pow((1.+effSF), 2) - 1 , 2)/(pow(Luminosity*binW, 2));
        //}


        double mcValue(genMad->GetBinContent(bin));
        xCoor[bin-1] = dataCentral->GetBinCenter(bin);
        yCoor[bin-1] = centralValue; 
        xErr[bin-1] = 0.5 * dataCentral->GetBinWidth(bin);
        yStat[bin-1] = totalStatistics;

        double systUp(0.), systDown(0.);

        for (int syst(0); syst < nSyst; syst++) {
            //double difference(hSyst[syst]->GetBinContent(bin) - centralValue);
            //if (difference > 0  || syst == 6) totalSystematicsUp += difference * difference;
            //if (difference <= 0 || syst == 6) totalSystematicsDown += difference * difference;
            totalSystematicsUp += pow(hSyst[syst]->GetBinContent(bin), 2);
            totalSystematicsDown += pow(hSyst[syst]->GetBinContent(bin), 2);


            //cout << "Name: " << systNames[syst] << " " << difference << endl; 
            //if (syst % 2 == 0 ) systUp = difference; 
            //if (syst % 2 == 1 || syst == 6) {
            //    systDown = difference;
            //    cout << " up: " << systUp << "  down: " << systDown << endl; 
            //    cout << " average: " << 0.5 * (fabs(systUp)+fabs(systDown)) << endl;
            //    if (variable.find("ZNGoodJets") != string::npos && bin > 1) {
            //        myFile <<  " & " << 0.5*(fabs(systUp)+fabs(systDown)) << "  "; 
            //    }
            //    else if (variable.find("ZNGoodJets") == string::npos) {
            //        myFile <<  " & " << 0.5*(fabs(systUp)+fabs(systDown)) << "  "; 
            //    }

            //}
        } 


        // compute systematics between two unfolding methods
        double diffBayesSVD(centralValueOppAlgo - centralValue);
        totalSystematicsUp += fabs(diffBayesSVD * diffBayesSVD);
        totalSystematicsDown += fabs(diffBayesSVD * diffBayesSVD);
        //if (variable.find("ZNGoodJets") != string::npos && bin > 1) {
        //    myFile <<  " & " << sqrt(diffUnfError*diffUnfError + diffBayesSVD * diffBayesSVD) << "  "; 
        //}
        //else if (variable.find("ZNGoodJets") == string::npos) {
        //    myFile <<  " & " << sqrt(diffUnfError*diffUnfError + diffBayesSVD * diffBayesSVD) << "  "; 
        //}
        //cout << endl;

        if (symmetricalErrors){
            double avgError = 0.5*(fabs(totalSystematicsUp) + fabs(totalSystematicsDown));
            totalSystematicsUp = avgError;
            totalSystematicsDown = avgError;
        }

        ySystDown[bin-1]   = sqrt(totalStatistics * totalStatistics + totalSystematicsDown);
        ySystUp[bin-1]     = sqrt(totalStatistics * totalStatistics + totalSystematicsUp);


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
    TGraphAsymmErrors *grCentralStat = new TGraphAsymmErrors(nBins, xCoor, yCoor, xErr, xErr, yStat, yStat);
    TGraphAsymmErrors *grCentralSyst = new TGraphAsymmErrors(nBins, xCoor, yCoor, xErr, xErr, ySystDown, ySystUp);

    cout << " create canvas " << endl;
    TCanvas *plots = makeZJetsPlots(grCentralStat, grCentralSyst, hPDFSyst, genShe, genPow, genMad);
    plots->Print(outputFileNamePNG.c_str());

    myFile.close();
    f->Close();
    if (fPDFSyst->IsOpen()) fPDFSyst->Close(); 
}

////////////////////////////////////////////////////////////////////////////////////

TH1D* getCovErrors(const TH1D *data, const TH2D *cov)
{
    TH1D *out = (TH1D*) data->Clone();
    out->SetDirectory(0);

    int nBins = out->GetNbinsX();
    for (int i(0); i <= nBins + 1; i++) {
        out->SetBinContent(i, sqrt(cov->GetBinContent(i, i)));
    }
    return out;
    
}
