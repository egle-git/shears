#ifndef __UnfoldingSyst__ 
#define __UnfoldingSyst__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void createZNGoodJets_Zinc2(TH1D* hCombinedStat, TH2D *hError2D[], bool doVarWidth){
//    string energy = getEnergy();
//    string variable1 = "ZNGoodJets_Zinc";
//    int JetPtMin = 30 ;
//    int JetPtMax = 0 ;
//    string leptonFlavor = "DMu";
//    //if (!isMuon) leptonFlavor = "DE";
//    bool doFlat = 0 ;
//
//    //-- fetch the data files and histograms --------------
//    TFile *fData[3];             // 0 = central, 1 = JES Up, 2 = JES Down 
//    getFiles(FILESDIRECTORY, fData, leptonFlavor, energy, ProcessInfo[DATAFILENAME].filename, JetPtMin, JetPtMax, doFlat, doVarWidth);
//    TH1D *hData[3];
//    getHistos(hData, fData, variable1);
//    //-----------------------------------------------------
//
//
//    //-- fetch the DY files and histograms ----------------
//    TFile *fDYMadGraph[4];
//    TFile *fDYSherpa = NULL;               // 0 = central, 1 = PU Up,  2 = PU Down,  3 = JER Up
//    TFile *fDYPowheg = NULL ;
//    getFiles(FILESDIRECTORY, fDYMadGraph, leptonFlavor, energy, ProcessInfo[DYMADGRAPHFILENAME].filename, JetPtMin, JetPtMin, doFlat, doVarWidth);
//    fDYSherpa = getFile(FILESDIRECTORY, leptonFlavor, energy, DYSHERPAFILENAME, JetPtMin, JetPtMin, doFlat, doVarWidth);
//    fDYPowheg = getFile(FILESDIRECTORY, leptonFlavor, energy, DYPOWHEGFILENAME, JetPtMin, JetPtMin, doFlat, doVarWidth);
//    TH1D *hDY[4];
//    TH1D *hDYGenMadGraph = NULL, *hDYGenSherpa = NULL,  *hDYGenPowheg = NULL;
//    getHistos(hDY, fDYMadGraph, variable1);
//    hDYGenMadGraph = getHisto(fDYMadGraph[0], "gen" + variable1);
//    hDYGenSherpa = getHisto(fDYSherpa, "gen" + variable1);
//    hDYGenPowheg = getHisto(fDYPowheg, "gen" + variable1);
//    //-----------------------------------------------------
//
//    TH1D* hOut = (TH1D*) hData[0]->Clone();
//
//    // now sum merged histogram
//    // then cum errors histograms
//    const int nBins(hCombinedStat->GetNbinsX());
//    int nSyst = 8 ;
//    for (int bin(1); bin <= nBins; bin++) {
//        double sum = hCombinedStat->GetBinContent(bin);
//        double sumErr[nSyst];
//
//        for (int syst(0); syst < nSyst; syst++) {
//            sumErr[syst] = hError2D[syst]->GetBinContent(bin, bin);
//        }
//        for (int j(bin + 1 ); j <= nBins; j++) {
//            sum += hCombinedStat->GetBinContent(j);
//            for (int syst(0); syst < nSyst; syst++) sumErr[syst] += hError2D[syst]->GetBinContent(j, j);
//        }
//        hOut->SetBinContent(bin, sum);
//        for (int syst(0); syst < nSyst; syst++) {
//            hError2D[syst]->SetBinContent(bin, bin, sumErr[syst]);
//            //for (int syst(0); syst < nSyst; syst++) sumErr[syst] = hError2D[syst]->SetBinContent(bin, bin, 1.);
//        }
//    }
//}
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <assert.h>
#include <iostream>

#include <cstring>

std::string pground(double& val, std::vector<double>& sys);

std::string pground(double& val, std::vector<double>& sys, std::vector<std::string>& result){
    std::string sval = pground(val, sys);
    result.resize(1+sys.size());
    result[0] = sval;

    for(unsigned j = 0; j < sys.size(); ++j){
        char buf[256];

        sprintf(buf, "%.2lg", sys[j]);

        //fix trailing 0
        int n = strlen(buf);
        int i = 0;
        int nd = 0;
        int dp = 0;
        int heading_zero = 1;
        while(i < n){
            if(buf[i]!='0') heading_zero = 0;
            if(heading_zero) { ++i; continue;}
            if(buf[i]=='e' || buf[i]=='E') break;
            if(isdigit(buf[i])) ++nd;
            if(buf[i]=='.') dp = 1;
            ++i;

        }

        if(nd<2){
            if(dp){
                buf[n] = '0';
                buf[n+1] = 0;
            } else{
                buf[n] = '.';
                buf[n+1] = '0';
                buf[n+2] = 0;
            }
        }

        result[1+j] = buf;
    }

    return sval;
}

std::string pground(double& val, std::vector<double>& sys){
    assert(sys.size()>0);
    double sys_min = fabsl(sys[0]);
    for(unsigned i(0); i < sys.size(); ++i) if(fabsl(sys[i])<sys_min) sys_min = fabsl(sys[i]);

    //rounding systematics
    std::vector<std::string> sSys(sys.size());
    char buf[256];
    for(unsigned i = 0; i < sys.size(); ++i){
        sprintf(buf, "%.2lg", sys[i]);
        sys[i] = strtod(buf, 0);
        sSys[i] = buf;
    }

    //number of digis:
    double l10 = floor(log10(sys_min));
    double a = pow(10, l10-1);


    sprintf(buf, "%%.%dlf", std::max(0, (int)(1-l10)));

    char buf2[256];

    val = round(val / a) * a;

    sprintf(buf2, buf, val);

    std::string r = buf2;

    //r.insert(0, "$");
    //size_t i = r.find('.');
    //if(i!=std::string::npos){
    //    r.replace(i, 1, "$&$.");
    //    r += "$";
    //} else{
    //    r+="$&";
    //}
    //
    std::cout << "1-l10: " << (int)(1-l10) << "  r: " << r << std::endl;
    return r;
}

std::string pground(double& val, double& sys){
    std::vector<double> s(1, sys);
    std::string sv = pground(val, s);
    sys = s[0];
    return sv;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void changeToLatexFormat(string& title)
{

    cout << "Start formating:  " << title << endl;
    myReplace(title, "#", "\\");
    //cout << "Remove #:         " << title << endl;
    vector<string> sentence;
    //cout << "Pieces : " << endl;
    while (title.find(" ") != string::npos){
        string tmp = title.substr(0, title.find(" "));
        //cout << tmp << endl;
        sentence.push_back(tmp);
        string space = " ";
        string tmp2 = title.substr(title.find(tmp) + tmp.size() + space.size(), title.size());
        title = tmp2;
    }
    //cout << title << endl;
    sentence.push_back(title);

    title = "";
    //cout << endl;
    for (int i(0); i < int(sentence.size()); i++){
        //cout << sentence[i] << endl;
        if (sentence[i].find("_") != string::npos || sentence[i].find("\\") != string::npos){
            sentence[i] = "$"+sentence[i]+"$";
        }
        //cout << sentence[i] << endl;
        title += sentence[i] + " ";
    }
    myReplace(title, "_{jets}", "_{\\text{jets}}");
    cout << " New Title:      " << title << endl;

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void createTexTable(string variable, string fileNameTable, const TH1D* data, TH2D *hError2D[], bool doXSec, bool doNormalize, double Luminosity)
{
    string temp, tempTab;
    if (doXSec) {
        string xtitle = data->GetXaxis()->GetTitle();
        string shortVar = xtitle.substr(0, xtitle.find(" "));
        if (shortVar.find("\\eta") != string::npos) xtitle = "\\vert" + xtitle + "\\vert";
        string unit = "";
        if (xtitle.find("[") != string::npos){
            size_t begin = xtitle.find("[") + 1;
            unit = xtitle.substr(begin);
            unit = unit.substr(0, unit.find("]"));
        }
        temp = "d#sigma/d" + shortVar;
        string shortVarLatex = shortVar;  // changeToLatexFormat(shortVarLatex);
        myReplace(shortVarLatex, "#", "\\");
        tempTab = "$\\frac{d\\sigma}{d" + shortVarLatex + "}$";
        if (doNormalize) temp = "1/#sigma " + temp;
        else if (doXSec){
            temp += "  [pb";
            tempTab += " \\tiny{$\\left[";
            if (unit != "" ){
                temp += "/" + unit;
                tempTab += "\\frac{ \\text{pb}}{" + unit + "}";
            }
            else tempTab += " \\text{pb}";
            temp += "]";
            tempTab += "\\right]$}";
        }
    }

    string title = data->GetTitle(); changeToLatexFormat(title);
    // fix a bug
    myReplace(title, "1 2", "1");
    myReplace(title, "2 2", "2");
    cout << "HERE IS THE TITLE " << title << endl;
    string xtitle = data->GetXaxis()->GetTitle(); 
    if (xtitle.find("#eta") != string::npos) xtitle = "\\vert" + xtitle + "\\vert";
    changeToLatexFormat(xtitle);
    string sigmaTitle = temp; changeToLatexFormat(sigmaTitle);
    cout << "CHANGE" << endl;
    string sigmaTitleTab = tempTab;// changeToLatexFormat(sigmaTitleTab);
    cout << "CHANGE" << endl;

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



    ofstream myFile(fileNameTable.c_str());
    myFile << "\\begin{table}[htb!]\\begin{center}\n";
    if (title.find("ounter") == string::npos) {
        myFile << "\\caption{Differential cross section in " << title << " and break down of the systematic uncertainties (in percentage) for the combination of both decay channels."; 
    }
    else {
        myFile << "\\caption{Cross section from the combination of both decay channels as the function of the exclusive jet multiplicity and break down of the uncertainties. The column denoted $\\sigma$ contains the cross section of Z boson production with exactly $N_{\\text{jets}}$ jet(s) times the branching ratio to dilepton of one flavour; the column denoted {\\em Tot. Unc.} contains the total uncertainty; the column denoted      {\\em stat} contains the statistical uncertainty; the remaining columns contain the different systematic uncertainties.";
    }
    myFile << "}"<<endl;
    myFile << "\\scriptsize{" << endl;
    myFile << "\\begin{tabular}{c|cc|cccccccc}\n \\multicolumn{11}{c}{";
    //myFile << title << "} \\\\" << endl;
    myFile << "  " << "} \\\\" << endl;
    myFile << xtitle << " & ";
    myFile << sigmaTitleTab << " & Tot. Unc. $\\left[\\%\\right]$  & stat $\\left[\\%\\right]$ & JEC $\\left[\\%\\right]$ & JER $\\left[\\%\\right]$ & PU $\\left[\\%\\right]$ & XSEC $\\left[\\%\\right]$ & Lumi $\\left[\\%\\right]$ & Unf $\\left[\\%\\right]$ & Eff $\\left[\\%\\right]$  \\\\ \\hline" << endl;

    int nSyst = 9;
    const int nBins(data->GetNbinsX());

    for (int bin(1); bin <= nBins; bin++){
        bool doPrint(1);
        if (variable.find("ZNGoodJet") != string::npos && bin == 1) doPrint = 0;

        double binW = data->GetBinWidth(bin);
        double Norm = 1.;
        if (doXSec) Norm = binW * Luminosity;


        double centralValue(data->GetBinContent(bin)/Norm);
        myFile.precision(4);
        if (variable.find("ZNGoodJets_Zexc") != string::npos && bin > 1) myFile <<  "$= " << bin - 1 << " $& ";
        else if (variable.find("ZNGoodJets_Zinc") != string::npos && bin > 1) myFile <<  "$\\geq " << bin - 1 << " $& ";
        else if (variable.find("ZNGoodJets") == string::npos) {
            if ( fabs(data->GetBinLowEdge(bin) + binW) < 0.01) myFile << data->GetBinLowEdge(bin) << "\\ -\\ " <<  0  << " & ";
            else myFile << data->GetBinLowEdge(bin) << "\\ -\\ " <<  data->GetBinLowEdge(bin) + binW  << " & ";
        }
        myFile.precision(4);
        if (variable.find("ZNGoodJets") != string::npos && bin > 1){
            double err = sqrt(hError2D[0]->GetBinContent(bin, bin)) / Norm;
            myFile << pground(centralValue, err) ;
            //myFile << fixed;
            //myFile << setprecision(2);
            myFile.precision(2);
            myFile << showpoint;
        }
        else if (variable.find("ZNGoodJets") == string::npos){
            double err = sqrt(hError2D[0]->GetBinContent(bin, bin)) / Norm;
            myFile << pground(centralValue, err) ;
            //myFile << fixed;
            //myFile << setprecision(2);
            myFile.precision(2);
            myFile << showpoint;
        }

        for (int syst(0); syst < nSyst; syst++){
            double error = 100*sqrt(hError2D[syst]->GetBinContent(bin,bin)) / (centralValue * Norm);
            if (error > 0.01) {
            // percentage
            //if (syst == 0) {
                if (variable.find("ZNGoodJets") != string::npos && bin > 1) myFile <<  " & " << 100*sqrt(hError2D[syst]->GetBinContent(bin,bin)) / (centralValue * Norm)  << "  ";
                else if (variable.find("ZNGoodJets") == string::npos) myFile <<  " & " << 100*sqrt(hError2D[syst]->GetBinContent(bin,bin)) / (centralValue * Norm) << "  ";
            //}
            //else {
            //    if (variable.find("ZNGoodJets") != string::npos && bin > 1) myFile <<  " & " << sqrt(hError2D[syst]->GetBinContent(bin,bin)) / (Norm)  << "  ";
            //    else if (variable.find("ZNGoodJets") == string::npos) myFile <<  " & " << sqrt(hError2D[syst]->GetBinContent(bin,bin)) / (Norm) << "  ";
            //}
            }
            else {
                if (variable.find("ZNGoodJets") != string::npos && bin > 1) myFile <<  " & " << "< 0.01" << "  ";
                else if (variable.find("ZNGoodJets") == string::npos) myFile <<  " & " << "< 0.01" << "  ";
            }

        }

        if (doPrint)  myFile << " \\\\" << endl;
    }

    myFile << "\\end{tabular}}" << endl;
    myFile << "\\label{tab:" << variable << "}"<< endl;
    myFile << "\\end{center}\\end{table}\n";
    myFile.close();

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Create an histogram with name "name" and content copied from "hInput" but with errors
// computed from the diagonal of the matrix MatInp.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TH1D* SetHistWithErrors(const TH1D* hInput, const TMatrixD &MatInp, const string name)
{
    TH1D* hOut = (TH1D*) hInput->Clone(name.c_str());
    hOut->SetDirectory(0);

    for (int i(1); i <= hInput->GetNbinsX(); i++) {
        hOut->SetBinError(i, sqrt(MatInp(i-1, i-1)));
    }

    return hOut;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TH2D* CovToCorr(const TH2D *h)
{
    TH2D* hCorr = (TH2D*) h->Clone();
    hCorr->SetDirectory(0);
    int xbin(h->GetNbinsX()), ybin(h->GetNbinsY());

    for (int i(1); i <= ybin; i++) {
        for (int j(1); j <= xbin; j++) {
            double temp = 0.;
            temp = h->GetBinContent(j, i) / sqrt(h->GetBinContent(i, i) * h->GetBinContent(j, j));
            hCorr->SetBinContent(j, i, temp);
        }
    }
    return hCorr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TH2D* setCovariance(const TH2D *h, const TH1D* hCent, const double error)
{
    TH2D* hCorr = (TH2D *) h->Clone();
    hCorr->SetDirectory(0);

    int xbin(h->GetNbinsX()), ybin(h->GetNbinsY());
    for (int i(1); i <= ybin; i++) {
        for (int j(1); j <= xbin; j++) {
            double temp = 0.;
            temp = h->GetBinContent(j, i) / sqrt(h->GetBinContent(i, i) * h->GetBinContent(j, j));
            hCorr->SetBinContent(j, i, temp * hCent->GetBinContent(i) * error * hCent->GetBinContent(j) * error);
        }
    }
    std::cout << "ok " << __LINE__ << std::endl;
    return hCorr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TH1D* getErrorsRel(const TH1D * dataCentral, const TH1D * dataUnfWithSherpa)
{
    // clone the histogram to make sure we don't mess it up
    TH1D *UnfErrors = (TH1D*) dataCentral->Clone();
    UnfErrors->SetDirectory(0);

    // get the number of bins to run on each of them
    int nBins = dataCentral->GetNbinsX();
    for (int i(0); i <= nBins + 1; i++) {

        // compute the absolute difference between the two generators
        double diff = fabs(dataCentral->GetBinContent(i) - dataUnfWithSherpa->GetBinContent(i));

        // make it relative to the bin content of dataCentral
        if (dataCentral->GetBinContent(i) != 0) diff /= dataCentral->GetBinContent(i);
        else diff = 0.;

        // fill in the histogram with the relative error
        UnfErrors->SetBinContent(i, diff);
        UnfErrors->SetBinError(i, 0);
    }

    // return the histogram containing the relative error for each bin
    return UnfErrors;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TH1D* getErrors(const TH1D * dataCentral, const TH1D * dataUnfWithSherpa)
{
    // clone the histogram to make sure we don't mess it up
    TH1D *UnfErrors = (TH1D*) dataCentral->Clone();
    UnfErrors->SetDirectory(0);

    // get the number of bins to run on each of them
    int nBins = dataCentral->GetNbinsX();
    for (int i(0); i <= nBins + 1; i++) {

        // compute the absolute difference between the two generators
        double diff = fabs(dataCentral->GetBinContent(i) - dataUnfWithSherpa->GetBinContent(i));

        // make it relative to the bin content of dataCentral
        //if (dataCentral->GetBinContent(i) != 0) diff /= dataCentral->GetBinContent(i);
        //else diff = 0.;

        // fill in the histogram with the relative error
        UnfErrors->SetBinContent(i, diff);
        UnfErrors->SetBinError(i, 0);
    }

    // return the histogram containing the relative error for each bin
    return UnfErrors;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TMatrixD setCovMatrixOfCombination(double luminosityErr, TH1D* hEle, TH1D* hMu, int optionCorrTemp){

    // declare the big matrix
    int nbins = hEle->GetNbinsX();
    const int NELE = 2*nbins;
    TMatrixD errorMTemp(NELE,NELE);

    for(int ibin = 0; ibin<nbins; ibin++){
        // electron channel
        double value_e  =  hEle->GetBinContent(ibin+1) ;
        // muon channel
        double value_m  =  hMu->GetBinContent(ibin+1) ;
        // first put electron error matrix component
        errorMTemp(ibin,ibin) = pow((luminosityErr * value_e),2);
        errorMTemp(nbins+ibin,nbins+ibin) = pow((luminosityErr * value_m),2);
        if ( optionCorrTemp > 0 ) {
            errorMTemp(ibin,nbins+ibin) = (luminosityErr * value_e * luminosityErr * value_m );
            errorMTemp(nbins + ibin,ibin) = (luminosityErr * value_e * luminosityErr * value_m );
        }
        for(int jbin = 0; jbin<nbins; jbin++){
            if(optionCorrTemp > 1 ){
                if(jbin !=  ibin ){
                    // electron
                    errorMTemp(ibin,jbin) = (luminosityErr * value_e * luminosityErr * hEle->GetBinContent(jbin+1)) ;	
                    // muon
                    errorMTemp(nbins+ibin,nbins+jbin) = (luminosityErr * value_m * luminosityErr * hMu->GetBinContent(jbin+1) );
                }
                if ( optionCorrTemp > 1 ) {
                    // electron-muon channel
                    errorMTemp(ibin,nbins+jbin) = (luminosityErr * value_e * luminosityErr * hMu->GetBinContent(jbin+1) );
                    // muon-electron channel
                    errorMTemp(nbins + ibin , jbin)   = (luminosityErr * hEle->GetBinContent(jbin+1) * luminosityErr * value_m );
                }
            }
        }

    }

    return errorMTemp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TMatrixD setCovMatrixOfCombinationLep(double muonIDIsoHLTError, double electronIDIsoHLTError, const TH1D* hEle, const TH1D* hMu, int optionCorrTemp)
{
    double correlationSameBin  =  0.;
    double correlationDiffBin  =  0.;
    /*
       if ( optionCorrTemp  ==  1 ) correlationSameBin  =  0.;
       if ( optionCorrTemp  ==  2 ) correlationSameBin  =  1.;
       if ( optionCorrTemp  ==  3 ) correlationSameBin  =  1.;
       if ( optionCorrTemp  ==  4 ) {
       correlationSameBin  =  1.;
       correlationDiffBin  =  1.;
       }
       if ( optionCorrTemp  > 0 ) {
       correlationSameBin  =  1.;
       correlationDiffBin  =  1.;
       }
       */
    // declare the big matrix
    int nbins = hEle->GetNbinsX();
    const int NELE = 2*nbins;
    TMatrixD errorMTemp(NELE,NELE);

    for(int ibin = 0; ibin<nbins; ibin++){
        // electron channel
        double value_e  =  hEle->GetBinContent(ibin+1) ;
        // muon channel
        double value_m  =  hMu->GetBinContent(ibin+1) ;
        // first put electron error matrix component
        errorMTemp(ibin,ibin) = pow((electronIDIsoHLTError * value_e),2);
        errorMTemp(nbins+ibin,nbins+ibin) = pow((muonIDIsoHLTError * value_m),2);
        if ( optionCorrTemp > 1 ) {
            errorMTemp(ibin,nbins+ibin) = correlationSameBin * (electronIDIsoHLTError * value_e * muonIDIsoHLTError * value_m );
            errorMTemp(nbins + ibin,ibin) = correlationSameBin *  (electronIDIsoHLTError * value_e * muonIDIsoHLTError * value_m );
        }
        for(int jbin = 0; jbin<nbins; jbin++){

            if(jbin !=  ibin  && optionCorrTemp > 0 ){
                // electron
                errorMTemp(ibin,jbin) =  (electronIDIsoHLTError * value_e * electronIDIsoHLTError * hEle->GetBinContent(jbin+1)) ;
                // muon
                errorMTemp(nbins+ibin,nbins+jbin) = (muonIDIsoHLTError * value_m * muonIDIsoHLTError * hMu->GetBinContent(jbin+1) );
                if ( optionCorrTemp > 1 ) {
                    // electron-muon channel
                    errorMTemp(ibin,nbins+jbin) = correlationDiffBin * (electronIDIsoHLTError * value_e * muonIDIsoHLTError * hMu->GetBinContent(jbin+1) );
                    // muon-electron channel
                    errorMTemp(nbins + ibin , jbin)   = correlationDiffBin * (electronIDIsoHLTError * hEle->GetBinContent(jbin+1) * muonIDIsoHLTError * value_m );
                }
            }
        }

    }

    return errorMTemp;
}

TMatrixD getCovMatrixOfCombination(const TH2D* CovEle, const TH2D* CovMuon, int optionCorrTemp)
{
    // get correlation matrix this is needed for option 3 of combination
    TH2D* hCorrEle = CovToCorr(CovEle);
    TH2D* hCorrMu = CovToCorr(CovMuon);

    double correlationDiffBin = 0.;
    if (optionCorrTemp >= 4) correlationDiffBin = 1.;

    // declare the big matrix
    int nbins = CovEle->GetNbinsX();
    const int NELE = 2 * nbins;
    TMatrixD errorMTemp(NELE, NELE);

    for (int ibin = 0; ibin < nbins; ibin++) {

        //---------------------------------------------------------------------
        // Fill in the diagonal of the 2n X 2n matrix:
        //
        //  / Cov E           |                \  .
        //  |       .    0    |                |
        //  |      0    .     |        0       |
        //  |               . |                |
        //  | --------------- | -------------- |
        //  |                 | Cov M          |
        //  |        0        |       .   0    |
        //  |                 |     0     .    |
        //  \                 |              . /
        //
        
        // first put electron error matrix component
        errorMTemp(ibin, ibin) = CovEle->GetBinContent(ibin + 1, ibin + 1);
        // then muon
        errorMTemp(nbins + ibin, nbins + ibin) = CovMuon->GetBinContent(ibin + 1, ibin + 1);
        //---------------------------------------------------------------------
       
        // stop here with the above matrix for optionCorr == 0

        if (optionCorrTemp > 1) {

            //---------------------------------------------------------------------
            // Fill in same bin correlation if requested (with corr = 1 = Cov(X, Y) / (sigmaX sigmaY) )
            //
            //  / Cov E           | CovE*CovM      \  .
            //  |       .    0    |       .   0    |
            //  |     0     .     |    0      .    |
            //  |               . |              . |
            //  | --------------- | -------------- |
            //  | CovE*CovM       | Cov M          |
            //  |       .    0    |       .   0    |
            //  |     0     .     |    0      .    |
            //  \               . |              . /
            //

            errorMTemp(ibin, nbins + ibin) = sqrt(CovEle->GetBinContent(ibin + 1, ibin + 1) * CovMuon->GetBinContent(ibin + 1, ibin + 1));
            errorMTemp(ibin + nbins, ibin) = errorMTemp(ibin, nbins + ibin);
        } 

        //---------------------------------------------------------------------



        if (optionCorrTemp > 0 && optionCorrTemp != 6) {
            for (int jbin = 0; jbin < nbins; jbin++) {

                //-------------------------------------------------------------------------
                // Complete electron and muon covariance matrices
                //
                //  /                 | CovE*CovM      \  .
                //  |      Cov E      |       .   0    |
                //  |                 |    0      .    |
                //  |                 |              . |
                //  | --------------- | -------------- |
                //  | CovE*CovM       |                |
                //  |       .    0    |      Cov M     |
                //  |     0     .     |                |
                //  \               . |                /
                //
                // electron channel
                errorMTemp(ibin, jbin) = CovEle->GetBinContent(ibin + 1, jbin + 1);
                // muon channel
                errorMTemp(nbins + ibin, nbins + jbin) = CovMuon->GetBinContent(ibin + 1, jbin + 1);
                //-------------------------------------------------------------------------

                if (optionCorrTemp == 5) {
                    //-------------------------------------------------------------------------
                    // Replace off-diagonal of Cov E and Cov M matrices by sqrt(CovE_ii * CovE_jj)
                    //
                    errorMTemp(ibin, jbin) = sqrt(CovEle->GetBinContent(ibin + 1, ibin + 1) * CovEle->GetBinContent(jbin + 1, jbin + 1));
                    errorMTemp(nbins + ibin, nbins + jbin) = sqrt(CovMuon->GetBinContent(ibin + 1, ibin + 1) * CovMuon->GetBinContent(jbin + 1, jbin + 1));
                    //-------------------------------------------------------------------------
                }

                if (ibin != jbin) {
                    // electron-muon channel
                    if (optionCorrTemp == 3) {
                        double valueEle = hCorrEle->GetBinContent(ibin, jbin);
                        double valueMu = hCorrMu->GetBinContent(ibin, jbin);
                        correlationDiffBin = sqrt(fabs(valueEle * valueMu));
                        if (valueEle < 0 && valueMu < 0) correlationDiffBin *= -1;
                        if (valueEle * valueMu < 0) {
                            if (fabs(valueEle) > fabs(valueMu)) correlationDiffBin *= valueEle / fabs(valueEle);
                            else correlationDiffBin *= valueMu / fabs(valueMu);
                        } 	
                    }
                    // muon-electron channel
                    errorMTemp(ibin, nbins + jbin) = correlationDiffBin * sqrt(fabs(CovEle->GetBinContent(ibin + 1, ibin + 1) * CovMuon->GetBinContent(jbin + 1, jbin + 1)));

                    errorMTemp(nbins + ibin, jbin) = correlationDiffBin * sqrt(fabs(CovMuon->GetBinContent(ibin + 1, ibin + 1) * CovEle->GetBinContent(jbin + 1, jbin + 1)));
                }
            }      
        }      
    } // loop over the number of bins

    return errorMTemp;

}

//////////////////////////////////////////////////////////////////////////////////////
TMatrixD getCovMatrixOfCombinationUNF(const TH1D* unfErrorDistrEle, const TH1D* unfErrorDistrMu, const TH1D* hEle, const TH1D* hMu, const TH1D* hEleOpp, const TH1D* hMuOpp, int optionCorrTemp)
{

    double correlationDiffBin = 0.;
    if (optionCorrTemp >= 4) correlationDiffBin = 1.;


    // declare the big matrix
    int nbins = hEle->GetNbinsX();
    TMatrixD errorMTemp(2*nbins, 2*nbins);
    double value_e[nbins];
    double value_m[nbins];
    double err_e[nbins];
    double err_m[nbins];
    double err_e_algo[nbins];
    double err_m_algo[nbins];

    for (int ibin = 0; ibin<nbins; ibin++) {
        value_e[ibin]    = hEle->GetBinContent(ibin+1);
        value_m[ibin]    = hMu->GetBinContent(ibin+1);
        err_e[ibin]      = unfErrorDistrEle->GetBinContent(ibin+1);
        err_m[ibin]      = unfErrorDistrMu->GetBinContent(ibin+1);
        err_e_algo[ibin] = fabs(value_e[ibin] - hEleOpp->GetBinContent(ibin+1));
        err_m_algo[ibin] = fabs(value_m[ibin] - hMuOpp->GetBinContent(ibin+1));
        err_e_algo[ibin] = 0.;
        err_m_algo[ibin] = 0.;
    }	

    for (int ibin = 0; ibin<nbins; ibin++) {
        // electron channel
        errorMTemp(ibin,ibin) = pow(err_e[ibin],2) + pow(err_e_algo[ibin], 2);
        // muon channel
        errorMTemp(nbins+ibin,nbins+ibin) = pow(err_m[ibin],2) + pow(err_m_algo[ibin], 2);

        // this part is to setup correlation between same bins fo e and mu for the unfolding error
        for (int jbin = 0; jbin<nbins; jbin++){
            if (jbin !=  ibin && optionCorrTemp > 0 ) {
                errorMTemp(ibin,jbin) = err_e[ibin] * err_e[jbin] + err_e_algo[ibin] * err_e_algo[jbin] ;
                errorMTemp(jbin,ibin) =  errorMTemp(ibin,jbin) ;
                errorMTemp(nbins+ibin,nbins + jbin) = err_m[ibin] * err_m[jbin] + err_m_algo[ibin] * err_m_algo[jbin] ;
                errorMTemp(nbins+jbin,nbins + ibin) = errorMTemp(nbins+ibin,nbins + jbin);

            }
            if (optionCorrTemp > 1) {
                errorMTemp(ibin,nbins+jbin) = err_e[ibin] * err_m[jbin] + err_e_algo[ibin] * err_m_algo[jbin];
                errorMTemp(nbins+ibin,jbin) = err_e[jbin] * err_m[ibin] + err_e_algo[jbin] * err_m_algo[ibin];
            }

        }

    }

    return errorMTemp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TH1D* getPUErrors(const TH1D *dataCentral, const TH1D *PUup, const TH1D *PUdown)
{
    // clone the data histogram to make sure we don't mess it up
    TH1D *dataPU = (TH1D*) dataCentral->Clone();
    dataPU->SetDirectory(0);

    // get the number of bins and run on each of them
    int nbins = dataCentral->GetNbinsX();
    for (int i(0); i <= nbins + 1; i++) {

        // compute the absolute difference between central and up, 
        // and central and down, and take the max as the error
        double diff = fabs(PUup->GetBinContent(i) - dataCentral->GetBinContent(i));
        diff = max(diff, fabs(PUdown->GetBinContent(i) - dataCentral->GetBinContent(i)));
        dataPU->SetBinContent(i, diff);
    }

    return dataPU;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TH1D* getJERErrors(const TH1D *dataCentral, const TH1D *JERup, const TH1D *JERdown)
{
    // clone the data histogram to make sure we don't mess it up
    TH1D *dataJER = (TH1D*) dataCentral->Clone();
    dataJER->SetDirectory(0);

    // get the number of bins and run on each of them
    int nbins = dataCentral->GetNbinsX();
    for (int i(0); i <= nbins + 1; i++) {

        // compute the absolute difference between central and up, 
        // and central and down, and take the max as the error
        double diff = fabs(JERup->GetBinContent(i) - dataCentral->GetBinContent(i));
        diff = max(diff, fabs(JERdown->GetBinContent(i) - dataCentral->GetBinContent(i)));
        dataJER->SetBinContent(i, diff);
    }

    return dataJER;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TMatrixD getCovMatrixOfCombination(const TH2D* Cov2D, const TH1D* DiffEle, const TH1D* DiffMuon, int optionCorrTemp)
{
    TH2D *CovEle2D = (TH2D*) Cov2D->Clone();
    CovEle2D->SetDirectory(0);

    TH2D *CovMuon2D = (TH2D*) Cov2D->Clone();
    CovMuon2D->SetDirectory(0);

    int nbins =  DiffEle->GetNbinsX();

    for (int i(1); i <= nbins; i++) {
        for (int j(1); j <= nbins; j++) {
            if (i == j) {
                CovEle2D->SetBinContent(i, j, pow(DiffEle->GetBinContent(i), 2));
                CovMuon2D->SetBinContent(i, j, pow(DiffMuon->GetBinContent(i), 2));
            }
            else {
                CovEle2D->SetBinContent(i, j, 0.);
                CovMuon2D->SetBinContent(i, j, 0.);
            }
        }
    }

    TMatrixD errorMTemp = getCovMatrixOfCombination(CovEle2D, CovMuon2D, optionCorrTemp);
    return errorMTemp;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#endif
