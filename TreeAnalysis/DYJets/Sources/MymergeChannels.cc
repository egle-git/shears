#include <TFile.h>
#include <TCanvas.h>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <TDecompLU.h>
#include <TDecompSVD.h>
#include <TSystem.h>
#include <TLegend.h>
#include <TLegendEntry.h>
#include <TGraphAsymmErrors.h>
#include <TLatex.h>
#include <TPaveText.h>
#include <iostream>
#include <TH1.h>
#include <TH2.h>
#include <TPad.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TMath.h>
#include <TString.h>
#include <fstream>
#include <sstream>
#include <iomanip> 
#include <RooUnfoldResponse.h>
#include <RooUnfoldSvd.h>
#include <RooUnfoldBayes.h>
#include <RooUnfoldBinByBin.h>
#include <RooUnfoldInvert.h>
#include <TSVDUnfold.h>
#include "unfoldingFunctions.h"
#include "fileNames.h"
#include "getFilesAndHistograms.h"
#include "variablesOfInterestVarWidth.h"
#include "UnfoldingError.h"
#include "makeZJetsPlots.h"
#include "setTDRStyle.h"

#define DEBUG              1
using namespace std;

//-- prototypes ---------------------------------------------------------------------------------//
void mergeChannelsRun(string var = "ZNGoodJets_Zexc", bool logZ = 0, bool decrease = 0, int optionCorr = 0);
void plotLepRatioComb(string variable, int optionCorr, TH1D* h_combine, TH1D* hMuon, TH1D* hEle, bool logz, bool decrease);
void plotCombination(string variable, int optionCorr, TH1D* hCombinedStat, TH1D* hCombinedTot, TH2D* allErrorsTH2[], TH1D* genMadTemp[], TH1D* genSheTemp[], TH1D* genPowTemp[]);
void dumpElements(TMatrixD& a);
void dumpElements(TVectorD& a);
void exclusifToInclusif(TH1D *h, TH2D *cov = NULL, bool overflow = false);
//-----------------------------------------------------------------------------------------------//

//-- global variables ---------------------------------------------------------------------------//
string OUTPUTDIRECTORY = "PNGFiles/NiceUnfold_30_1000_Toys/";

int doXSec = 1;
int doNormalize = 0;
const double Luminosity(19600.);
const bool doVarWidth = true;
//-----------------------------------------------------------------------------------------------//


//--- Main function -----------------------------------------------------------------------------//
void MymergeChannels(int start = 0, int end = -1)
{
    setTDRStyle();
    gStyle->SetOptStat(0);
    gStyle->SetErrorX(0.5);
    gStyle->SetPadGridX(0);
    gStyle->SetPadGridY(0);
    int kCorrMax = 1; 

    if (end == -1) end = start + 1;
    for (int i(start); i < end; i++){
        std::cout << "Processing variable n°: " << i << "  " << VAROFINTERESTZJETS[i].name << std::endl;
        for (int k(0); k < kCorrMax; k++){
            int optionCorr = k;
            // 0 - simple weighted average, 
            // 1 - full cov matrix for each channel 
            // 2 - full cov matrix for each channel and correlation = 1 for same bins in the two channels 
            // 3 - full cov matrix for each channel is geometric average for two channels (for each bin)  
            // 4 - full cov matrix for each channel and  correlation = 1 for ALL bins between the two channels 
            // 6 - diag for each channel plus diagonal of ele-mu cov  
            mergeChannelsRun(VAROFINTERESTZJETS[i].name, VAROFINTERESTZJETS[i].log, VAROFINTERESTZJETS[i].decrease, optionCorr);
        }
    }
}
//-----------------------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------------------//
void mergeChannelsRun(string variable, bool logZ, bool decrease, int optionCorr)
{
    string ENERGY = getEnergy();
    string unfAlg = "Bayes";
    TH1::SetDefaultSumw2();

    //-- fetch Muon and Electron files produced by FinalUnfold.cc ---------------------
    string fileNameEl = "PNGFiles/FinalUnfold_30_1000_Toys/DE_"  + ENERGY + "_unfolded_" + variable + "_histograms_Bayes_VarWidth.root";
    string fileNameMu = "PNGFiles/FinalUnfold_30_1000_Toys/DMu_" + ENERGY + "_unfolded_" + variable + "_histograms_Bayes_VarWidth.root";

    TFile *f[2];
    f[0] = new TFile(fileNameEl.c_str());
    f[1] = new TFile(fileNameMu.c_str());
    //---------------------------------------------------------------------------------

    //-- Retrieve histograms from the files -------------------------------------------
    TH1D *dataReco[2], *dataCentral[2], *dataUnfWithSherpa[2], *unfErrorDistr[2], *dataCentralOppAlgo[2], *genMad[2], *genShe[2], *genPow[2];
    TH1D *dataPUup[2], *dataPUdown[2], *dataJERup[2], *dataJERdown[2], *dataPU[2], *dataJER[2];
    TH2D *myToyStatCov[2], *myToyJESCov[2], *myToyPUCov[2], *myToyXSECCov[2], *myToyJERCov[2], *myToyEFFCov[2], *myToyLUMICov[2];

    double luminosityErr = 0.026;

    //--- loop to run on muon (i=1) and electron (i=0) ---
    for (int i(0); i < 2; i++) {
        dataReco[i]           = (TH1D*) f[i]->Get("Data");
        dataCentral[i]        = (TH1D*) f[i]->Get("Central");
        dataUnfWithSherpa[i]  = (TH1D*) f[i]->Get("unfWithSherpa");
        dataCentralOppAlgo[i] = (TH1D*) f[i]->Get("oppCentral");
        genMad[i]             = (TH1D*) f[i]->Get("genMad");
        genShe[i]             = (TH1D*) f[i]->Get("genShe");
        genPow[i]             = (TH1D*) f[i]->Get("genPow");

        dataPUup[i]           = (TH1D*) f[i]->Get("PUup");
        dataPUdown[i]         = (TH1D*) f[i]->Get("PUdown");
        dataJERup[i]          = (TH1D*) f[i]->Get("JERup");
        dataJERdown[i]        = (TH1D*) f[i]->Get("JERdown");
        myToyStatCov[i]       = (TH2D*) f[i]->Get("MyToyCov");
        myToyJESCov[i]        = (TH2D*) f[i]->Get("MyToyJESCov");
        myToyPUCov[i]         = (TH2D*) f[i]->Get("MyToyPU2Cov");
        myToyXSECCov[i]       = (TH2D*) f[i]->Get("MyToyXSECCov");
        myToyJERCov[i]        = (TH2D*) f[i]->Get("MyToyJER2Cov");
        myToyEFFCov[i]        = (TH2D*) f[i]->Get("MyToyEFFCov");

        //--- call errors from unfolding with Sherpa and MadGraph ----
        // returns histogram filled with the relative errors
        unfErrorDistr[i] = getErrors(dataCentral[i], dataUnfWithSherpa[i]);

        //--- compute error for PU ---
        // returns histogram filled with max of the |up - central| or |down - central| 
        dataPU[i] = getPUErrors(dataCentral[i], dataPUup[i], dataPUdown[i]);

        //--- compute error for JER ---
        // returns histogram filled with max of the |up - central| or |down - central| 
        dataJER[i] = getJERErrors(dataCentral[i], dataJERup[i], dataJERdown[i]);

        // set covariance for luminosity uncertainty --> take correlation from JES and rescale
        myToyLUMICov[i] = setCovariance(myToyJESCov[i], dataCentral[i], luminosityErr);

    }
    //---------------------------------------------------------------------------------
    // Determine the number of bins once for all
    int nbins = dataCentral[0]->GetNbinsX();
    //---------------------------------------------------------------------------------

    //---------------------------------------------------------------------------------
    // For the combination we use the BLUE method. Reference may be found below:
    // see http://www.sciencedirect.com/science/article/pii/S0168900203003292 
    // Combining correlated measurements of several different physical quantities

    //---------------------------------------------------------------------------------
    // Declare the measurement vector. This vector contains the unfolded data for each
    // channel, hence its size 2 * nbins. It corresponds to the y_{i} vector of the 
    // paper mentioned above.
    TVectorD measurement(2 * nbins);
    for (int ibin = 0; ibin < nbins; ibin++) {
        // electron channel
        measurement(ibin) = dataCentral[0]->GetBinContent(ibin + 1);
        // muon channel
        measurement(nbins + ibin) = dataCentral[1]->GetBinContent(ibin + 1);
    }
    //---------------------------------------------------------------------------------

    //-------------------------------------------------------------------------------------
    // Now we must build up the 2n X 2n covariance matrices for each source of uncertainty.
    // This is the trickiest part of the combination, since there is no recepy giving
    // the rules to follow regarding what correlation to put between the different 
    // measurements (electron - muon). That is why we have different option according to
    // the variable optionCorr.
    int optStat = (optionCorr > 0) ? 1 : 0; 
    TMatrixD covMatrixStat = getCovMatrixOfCombination(myToyStatCov[0], myToyStatCov[1], optStat);
    TMatrixD covMatrixJES  = getCovMatrixOfCombination(myToyJESCov[0], myToyJESCov[1], optionCorr);
    TMatrixD covMatrixPU   = getCovMatrixOfCombination(myToyPUCov[0], dataPU[0], dataPU[1], optionCorr);
    //TMatrixD covMatrixPU   = getCovMatrixOfCombination(myToyPUCov[0], myToyPUCov[1], optionCorr);
    TMatrixD covMatrixXSEC = getCovMatrixOfCombination(myToyXSECCov[0], myToyXSECCov[1], optionCorr);
    TMatrixD covMatrixJER  = getCovMatrixOfCombination(myToyJERCov[0], dataJER[0], dataJER[1], optionCorr);
    //TMatrixD covMatrixJER  = getCovMatrixOfCombination(myToyJERCov[0], myToyJERCov[1], optionCorr);
    TMatrixD covMatrixUNF  = getCovMatrixOfCombinationUNF(unfErrorDistr[0], unfErrorDistr[1], dataCentral[0], dataCentral[1], dataCentralOppAlgo[0], dataCentralOppAlgo[1], optionCorr);
    TMatrixD covMatrixLUMI = setCovMatrixOfCombination(luminosityErr, dataCentral[0], dataCentral[1], optionCorr);
    TMatrixD covMatrixLEP  = getCovMatrixOfCombination(myToyEFFCov[0], myToyEFFCov[1], optStat); // two  leptons are independednt
    //-------------------------------------------------------------------------------------
    //cout << "Study of stat covariance" << endl;
    //TCanvas *cc = new TCanvas();
    //cc->Divide(2);
    //cc->cd(1);
    //myToyStatCov[0]->DrawCopy("text");
    //cc->cd(2);
    //myToyStatCov[1]->DrawCopy("text");
    //dumpElements(covMatrixStat);
    //cout << "End of study of stat covariance" << endl;

    //---------------------------------------------------------------------------------
    // Declare the total covariance matrix. This corresponds to the caligraphic M_{ij}
    // matrix of the paper mentioned above.
    // It is obtained by summing the covariance matrices of each independent source of
    // uncertainty that were computed above.
    TMatrixD errorM(2*nbins, 2*nbins);
    errorM = covMatrixStat;
    errorM += covMatrixJES; 
    errorM += covMatrixPU;
    errorM += covMatrixXSEC;
    errorM += covMatrixJER;
    errorM += covMatrixLEP;
    errorM += covMatrixUNF;
    //---------------------------------------------------------------------------------

    //---------------------------------------------------------------------------------
    // Declare the inverse of the total covariance matrix. This corresponds to the 
    // inverse of the caligraphic M_{ij} matrix of the paper mentioned above.
    TMatrixD errorInverse = errorM;
    if (optionCorr == 0) {
        // If optionCorr == 0 simply take the inverse of the diagonal since
        // errorM is in this case diagonal
        int nrows = errorInverse.GetNrows();
        for (int i(0); i < nrows; i++) {
            errorInverse(i, i) = 1. / errorInverse(i, i);
        }
    }
    else {
        // If optionCorr != 0 we call the invert method
        errorInverse.Invert();
    }
    //---------------------------------------------------------------------------------


    //------------------------------------------------------------------------
    // Declare the 2N x N unit matrix Unit_2nXn:
    //
    //             / 1 0 0 \  .
    //             | 0 1 0 |
    // Unit_2nXn = | 0 0 1 |
    //             | 1 0 0 |
    //             | 0 1 0 |
    //             \ 0 0 1 /
    //
    // This matrix corresponds to the caligraphic U matrix of the paper
    // mentioned above
    TMatrixD Unit_2nXn(2*nbins, nbins);
    for (int irow = 0; irow < 2*nbins; irow++) {
        for (int icol = 0; icol < nbins; icol++) {
            // 1 if irow = icol or if irow = icol + nbins 
            Unit_2nXn(irow, icol) = ((irow == icol) || (irow == icol + nbins));
        }
    }
    if (DEBUG) {
        cout << "Printing U" << endl;
        dumpElements(Unit_2nXn);
    }
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Declare the N x 2N unit matrix Unit_nX2n:
    //
    //              / 1 0 0 1 0 0 \  .
    //  Unit_nX2n = | 0 1 0 0 1 0 |
    //              \ 0 0 1 0 0 1 /
    //
    // This matrix corresponds to the transpose of caligraphic U matrix of 
    // the paper mentioned above, i.e. U tilda
    TMatrixD Unit_nX2n(nbins, 2*nbins);
    for (int irow = 0; irow < nbins; irow++) {
        for (int icol = 0; icol < 2*nbins; icol++) {
            // 1 if irow = icol or if irow + nbins = icol 
            Unit_nX2n(irow, icol) = ((irow == icol) || (irow + nbins == icol));
        }
    }
    if (DEBUG) {
        cout << "Printing U tilde (transpose of U)" << endl;
        dumpElements(Unit_nX2n);
    }
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // Declare the N x 2N matrix lambda_nX2n and its transpose. These matrices
    // correpond to the lambda and lambda tilda matrices of the paper mentioned
    // above. They are use to obtained the result vector of the combination
    // xhat_{i} of the paper, and its covariance matrix.
    TMatrixD lambda_nX2n(nbins, 2*nbins);
    lambda_nX2n = TMatrixD(Unit_nX2n * errorInverse * Unit_2nXn).Invert() * (Unit_nX2n * errorInverse);
    if (DEBUG) {
        cout << "Printing lambda" << endl;
        dumpElements(lambda_nX2n);
    }

    TMatrixD lambda_2nXn(2*nbins,nbins);
    lambda_2nXn.Transpose(lambda_nX2n);
    if (DEBUG) {
        cout << "Printing lambda tilde (transpose of lambda)" << endl;
        dumpElements(lambda_2nXn);
    }
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // for optionCorr == 0, this is the same than simple weighted average
    // (sum_{i=0}^{2} x_{i} sigma_{i}^{-2}) / (sum_{i=0}^{2} sigma_{i}^{-2})
    // This vector corresponds to xhat_{i} of the paper mentioned above.
    TVectorD combined_value(nbins);
    combined_value = lambda_nX2n * measurement;
    if (DEBUG) {
        cout << "Printing xhat_{i}" << endl;
        dumpElements(combined_value);
    }
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    // for optionCorr == 0, this is the same than simple weighted average:
    // 1 / ( sum_{i=1}^{2} sigma_{i}^{-2} )
    // This matrix corresponds to the covariance matrix of the xhat_{i} vector
    // of the paper mentioned above.
    TMatrixD combined_error(nbins, nbins);
    combined_error = lambda_nX2n * errorM * lambda_2nXn;
    //------------------------------------------------------------------------

    TMatrixD combined_error_LUMI(nbins, nbins);
    // after all matrix operation, now set the histogram
    // store combined result and all the individual contributions to error in vector
    TH1D* h_combine = (TH1D*) dataReco[0]->Clone();
    h_combine->SetDirectory(0);

    for (int i(0); i < nbins; i++) {

        h_combine->SetBinContent(i+1, combined_value(i));
        combined_error_LUMI(i,i) = pow(luminosityErr * combined_value(i), 2);
        double error = combined_error(i,i) + combined_error_LUMI(i, i);
        /// add luminosity error to the combination result instead of each of channels before the combination
        //		error +=  pow(luminosityErr *  h_combine->GetBinContent(i+1),2 ) ;
        if (error > 1e-10) error = sqrt(error);
        else error = 1e-10;

        /// add luminosity error to the combination result instead of each of channels before the combination
        h_combine->SetBinError(i+1, error);
    }
    /// NOW ADD LUMI ERRORS TO COMBINATION
    combined_error += combined_error_LUMI;

    //------------------------------------------------------------------------
    // set covariance matrices for individual errors
    TMatrixD combined_error_stat(nbins, nbins);
    TMatrixD combined_error_JES(nbins, nbins);
    TMatrixD combined_error_PU(nbins, nbins);
    TMatrixD combined_error_XSEC(nbins, nbins);
    TMatrixD combined_error_JER(nbins, nbins);
    TMatrixD combined_error_UNF(nbins, nbins);
    TMatrixD combined_error_LEP(nbins, nbins);

    combined_error_stat = lambda_nX2n * covMatrixStat * lambda_2nXn;
    combined_error_JES  = lambda_nX2n * covMatrixJES * lambda_2nXn;
    combined_error_PU   = lambda_nX2n * covMatrixPU * lambda_2nXn;
    combined_error_XSEC = lambda_nX2n * covMatrixXSEC * lambda_2nXn;
    combined_error_JER  = lambda_nX2n * covMatrixJER * lambda_2nXn;
    combined_error_UNF  = lambda_nX2n * covMatrixUNF * lambda_2nXn;
    combined_error_LEP  = lambda_nX2n * covMatrixLEP * lambda_2nXn;
    //combined_error_LUMI  =  lambda_nX2n * covMatrixLUMI * lambda_2nXn; // 
    //------------------------------------------------------------------------


    TH2D* allErrorsTH2[9];
    allErrorsTH2[0] = new TH2D(combined_error);
    allErrorsTH2[1] = new TH2D(combined_error_stat);
    allErrorsTH2[2] = new TH2D(combined_error_JES);
    allErrorsTH2[3] = new TH2D(combined_error_JER);
    allErrorsTH2[4] = new TH2D(combined_error_PU);
    allErrorsTH2[5] = new TH2D(combined_error_XSEC);
    allErrorsTH2[6] = new TH2D(combined_error_LUMI);
    allErrorsTH2[7] = new TH2D(combined_error_UNF);
    allErrorsTH2[8] = new TH2D(combined_error_LEP);


    // lets create som tables
    ostringstream optionCorrStr; optionCorrStr << optionCorr;

    string command = "mkdir -p " + OUTPUTDIRECTORY;
    system(command.c_str());
    string fileNameTable = OUTPUTDIRECTORY + "/TableSystematics_" + ENERGY + "_" + variable + "_" + unfAlg;
    fileNameTable += "_CorrelationOption_" + optionCorrStr.str();
    if (doVarWidth) fileNameTable += "_VarWidth";
    fileNameTable += ".tex";
    createTexTable(variable, fileNameTable, h_combine, allErrorsTH2, doXSec, doNormalize, Luminosity);
    // end table creation


    // create combination histogram with statistical errors
    TH1D* h_combine_stat = SetHistWithErrors(h_combine, combined_error_stat, "Stat");

    /// PLOT COMPARISON OF ELECTRONS AND MUONS TO COMBINED
    //plotLepRatioComb(variable, optionCorr, (TH1D*) h_combine->Clone(), (TH1D*) dataCentral[0]->Clone(),(TH1D*) dataCentral[1]->Clone(), logz, decrease);

    /// PLOT FINAL PLOTS: COMBINATION VS MC
    plotCombination(variable, optionCorr,(TH1D*) h_combine_stat->Clone(), (TH1D*) h_combine->Clone(), (TH2D**)allErrorsTH2, genMad, genShe, genPow);

    f[0]->Close();
    f[1]->Close();
}


//////////////////////////////////
void plotCombination(string variable, int optionCorr, TH1D* hCombinedStat, TH1D* hCombinedTot, TH2D* allErrorsTH2[], TH1D* genMadTemp[], TH1D* genSheTemp[], TH1D* genPowTemp[])
{

    //--- fetch the generated histograms for electrons ([0]) and muons ([1]) 
    // and add theim together ---
    TH1D* genMad = (TH1D*) genMadTemp[0]->Clone();
    genMad->SetDirectory(0);
    TH1D* genShe = (TH1D*) genSheTemp[0]->Clone();
    genShe->SetDirectory(0);
    TH1D* genPow = (TH1D*) genPowTemp[0]->Clone();
    genPow->SetDirectory(0);
    genMad->Add(genMadTemp[1]);
    genMad->Scale(0.5);
    genShe->Add(genSheTemp[1]);
    genShe->Scale(0.5);
    genPow->Add(genPowTemp[1]);
    genPow->Scale(0.5);
    //-----------------------------

    //--- load PDF systematic file ---
    string genVariable = "gen" + variable;
    //string PDFfileName = "PDFSystFiles/PDFSyst_" + genVariable + ".root";
    string PDFfileName = "HistoFiles/CT10_PDF_Uncertainties.root";
    TFile *fPDF = new TFile(PDFfileName.c_str());

    // get histograms from PDF file
    TH1D *hPDFUp = (TH1D*) fPDF->Get(string("Up_" + genVariable).c_str());
    hPDFUp->SetTitle("");
    TH1D *hPDFDown = (TH1D*) fPDF->Get(string("Down_" + genVariable).c_str());
    hPDFDown->SetTitle("");

    const int nBins(hCombinedStat->GetNbinsX());
    // renormalize first
    if (doXSec) {
        for (int i(1); i <= nBins + 1; i++) { 
            double binW = hCombinedStat->GetBinWidth(i);
            hCombinedStat->SetBinContent(i, hCombinedStat->GetBinContent(i)*1./(Luminosity*binW));
            hCombinedTot->SetBinContent(i, hCombinedTot->GetBinContent(i)*1./(Luminosity*binW));
            genMad->SetBinContent(i, genMad->GetBinContent(i)*1./(Luminosity*binW));
            genShe->SetBinContent(i, genShe->GetBinContent(i)*1./(Luminosity*binW));
            genPow->SetBinContent(i, genPow->GetBinContent(i)*1./(Luminosity*binW));

            hCombinedStat->SetBinError(i, hCombinedStat->GetBinError(i)*1./(Luminosity*binW));
            hCombinedTot->SetBinError(i, hCombinedTot->GetBinError(i)*1./(Luminosity*binW));
            genMad->SetBinError(i, genMad->GetBinError(i)*1./(Luminosity*binW));
            genShe->SetBinError(i, genShe->GetBinError(i)*1./(Luminosity*binW));
            genPow->SetBinError(i, genPow->GetBinError(i)*1./(Luminosity*binW));

        }
    }

    TCanvas *plots = makeZJetsPlots(hCombinedStat, hCombinedTot, hPDFUp, hPDFDown, genShe, genMad);
    //TCanvas *plots = makeZJetsPlots(hCombinedStat, hCombinedTot, hPDFUp, hPDFDown, genShe, genPow, genMad);

    string outputFileName = OUTPUTDIRECTORY;
    if (doXSec) outputFileName += "Combination_XSec_";
    else if (doNormalize) outputFileName += "Combination_Normalized_";

    outputFileName += variable;
    ostringstream optionCorrStr; optionCorrStr << optionCorr;
    outputFileName += "_CorrelationOption_" + optionCorrStr.str();
    if (doVarWidth) outputFileName += "_VarWidth";
    string outputFileNameROOT = outputFileName + ".root";
    string outputFileNamePDF = outputFileName + ".pdf";
    string outputFileNamePNG = outputFileName + ".png";

    plots->Print(outputFileNamePDF.c_str());
    plots->Print(outputFileNamePNG.c_str());
    plots->SaveAs(outputFileNameROOT.c_str());


    if (variable == "ZNGoodJets_Zexc") {

        string variable_Zinc = "ZNGoodJets_Zinc";
        string genVariable_Zinc = "genZNGoodJets_Zinc";
        TH1D *hCombinedStat_ZincNoCorr = (TH1D*) hCombinedStat->Clone();
        TH1D *hCombinedStat_Zinc = (TH1D*) hCombinedStat->Clone();
        hCombinedStat_Zinc->SetDirectory(0);
        hCombinedStat_Zinc->SetTitle(variable_Zinc.c_str());
        exclusifToInclusif(hCombinedStat_ZincNoCorr);
        exclusifToInclusif(hCombinedStat_Zinc, allErrorsTH2[1]);
        TH1D *hCombinedTot_Zinc = (TH1D*) hCombinedTot->Clone();
        hCombinedTot_Zinc->SetDirectory(0);
        hCombinedTot_Zinc->SetTitle(variable_Zinc.c_str());
        exclusifToInclusif(hCombinedTot_Zinc, allErrorsTH2[0]);
        TH1D *hPDFUp_Zinc = (TH1D*) fPDF->Get(string("Up_" + genVariable_Zinc).c_str());
        hPDFUp_Zinc->SetTitle("");
        TH1D *hPDFDown_Zinc = (TH1D*) fPDF->Get(string("Down_" + genVariable_Zinc).c_str());
        hPDFDown_Zinc->SetTitle("");
        hPDFUp_Zinc->SetDirectory(0);
        hPDFUp_Zinc->SetName(variable_Zinc.c_str());
        hPDFDown_Zinc->SetDirectory(0);
        hPDFDown_Zinc->SetName(variable_Zinc.c_str());
        TH1D *genShe_Zinc = (TH1D*) genShe->Clone();
        genShe_Zinc->SetDirectory(0);
        genShe_Zinc->SetName(variable_Zinc.c_str());
        exclusifToInclusif(genShe_Zinc);
        TH1D *genPow_Zinc = (TH1D*) genPow->Clone();
        genPow_Zinc->SetDirectory(0);
        genPow_Zinc->SetName(variable_Zinc.c_str());
        exclusifToInclusif(genPow_Zinc);
        TH1D *genMad_Zinc = (TH1D*) genMad->Clone();
        genMad_Zinc->SetDirectory(0);
        genMad_Zinc->SetName(variable_Zinc.c_str());
        exclusifToInclusif(genMad_Zinc);


        TCanvas *plots_Zinc = makeZJetsPlots(hCombinedStat_Zinc, hCombinedTot_Zinc, hPDFUp_Zinc, hPDFDown_Zinc, genShe_Zinc, genMad_Zinc);
        //TCanvas *plots_Zinc = makeZJetsPlots(hCombinedStat_Zinc, hCombinedTot_Zinc, hPDFUp_Zinc, hPDFDown_Zinc, genShe_Zinc, genPow_Zinc, genMad_Zinc);

        string outputFileName_Zinc = OUTPUTDIRECTORY;
        if (doXSec) outputFileName_Zinc += "Combination_XSec_";
        else if (doNormalize) outputFileName_Zinc += "Combination_Normalized_";

        outputFileName_Zinc += variable_Zinc;
        ostringstream optionCorrStr_Zinc; optionCorrStr_Zinc << optionCorr;
        outputFileName_Zinc += "_CorrelationOption_" + optionCorrStr_Zinc.str();
        if (doVarWidth) outputFileName_Zinc += "_VarWidth";
        string outputFileNameROOT_Zinc = outputFileName_Zinc + ".root";
        string outputFileNamePDF_Zinc = outputFileName_Zinc + ".pdf";
        string outputFileNamePNG_Zinc = outputFileName_Zinc + ".png";

        plots_Zinc->Print(outputFileNamePDF_Zinc.c_str());
        plots_Zinc->Print(outputFileNamePNG_Zinc.c_str());
        plots_Zinc->SaveAs(outputFileNameROOT_Zinc.c_str());



    }

    fPDF->Close();

}

void exclusifToInclusif(TH1D *h, TH2D *cov, bool overflow) {
    int nBins = h->GetNbinsX();

    //if (cov) {
    //    TCanvas *abcd = new TCanvas("abcd", "abcd", 600, 600);
    //    abcd->cd();
    //    cov->Draw("text");
    //}
    for (int i(1); i <= nBins; i++) {
        double sum(0);
        double error2(0);
        double err2(0);
        int overflowBin = overflow ? 1 : 0;
        for (int j(i); j <= nBins + overflowBin; j++) {
            sum += h->GetBinContent(j);
            if (cov == NULL) {
                error2 += pow(h->GetBinError(j), 2);
            }
            else {
                err2 += pow(h->GetBinError(j), 2);
                for (int k(i); k <= nBins + overflowBin; k++) {
                    error2 += cov->GetBinContent(j, k)*1./pow(19600, 2);
                }
            }
        }
        h->SetBinContent(i, sum);
        h->SetBinError(i, sqrt(error2));
    }
}

//////////////////////// NICE plots
void plotLepRatioComb(string variable, int optionCorr, TH1D* hCombined, TH1D* hEle, TH1D* hMuon, bool logz, bool decrease){

    TH1D* hCombErr = (TH1D *) hCombined ->Clone();
    hCombErr->GetYaxis()->SetTitle("");

    const int nBins =  hCombined->GetNbinsX();	
    double xCoor[nBins], yCoor[nBins], xErr[nBins], yStat[nBins];
    double yCoorMuonRatio[nBins], yStatMuonRatio[nBins], ySystDownMuonRatio[nBins], ySystUpMuonRatio[nBins];
    double yCoorEleRatio[nBins], yStatEleRatio[nBins], ySystDownEleRatio[nBins], ySystUpEleRatio[nBins];

    for(int bin = 1; bin <= nBins; bin++) {
        hCombErr->SetBinContent(bin, 1 );
        double ErrCom = hCombined->GetBinError(bin) / hCombined->GetBinContent(bin);
        hCombErr->SetBinError(bin , ErrCom );
        double binNormWidth = 1.;
        if (doXSec) {
            binNormWidth = hCombined->GetBinWidth(bin) * Luminosity ;
            double aCom = hCombined->GetBinContent(bin) / binNormWidth ;
            double aMu = hMuon->GetBinContent(bin) / binNormWidth ;
            double aE  = hEle->GetBinContent(bin) / binNormWidth ;
            double ErrMu =  hMuon->GetBinError(bin) / binNormWidth;
            double ErrEle = hEle->GetBinError(bin) / binNormWidth ;
            ErrCom = hCombined->GetBinError(bin) /binNormWidth ;
            hCombined->SetBinContent(bin, aCom);
            hCombined->SetBinError(bin, ErrCom);
            hEle->SetBinContent(bin, aE);
            hEle->SetBinError(bin, ErrEle);
            hMuon->SetBinContent(bin, aMu);
            hMuon->SetBinError(bin, ErrMu);
        }

        double totalSystematicsUp(0.), totalSystematicsDown(0.);
        double totalStatistics(hCombined->GetBinError(bin));
        double centralValue(hCombined->GetBinContent(bin));


        xCoor[bin-1] = hCombined->GetBinCenter(bin);
        yCoor[bin-1] = centralValue;
        xErr[bin-1] = 0.;			
        yStat[bin-1] = totalStatistics;

        /// get ratio od muon to merged
        yCoorMuonRatio[bin-1]  =  (hMuon->GetBinContent(bin)) / centralValue ;
        yStatMuonRatio[bin-1]     =  totalStatistics / centralValue ;
        ySystDownMuonRatio[bin-1]    =  sqrt(totalStatistics * totalStatistics + totalSystematicsDown )/(hMuon->GetBinContent(bin));
        ySystUpMuonRatio[bin-1]      =  sqrt(totalStatistics * totalStatistics + totalSystematicsUp )/(hMuon->GetBinContent(bin));

        /// get ratio od Electron to merged
        yCoorEleRatio[bin-1]  =  (hEle->GetBinContent(bin)) / centralValue ;
        yStatEleRatio[bin-1]     =  totalStatistics / centralValue ;
        ySystDownEleRatio[bin-1]    =  sqrt(totalStatistics * totalStatistics + totalSystematicsDown )/(hMuon->GetBinContent(bin));
        ySystUpEleRatio[bin-1]      =  sqrt(totalStatistics * totalStatistics + totalSystematicsUp )/(hMuon->GetBinContent(bin));
        // set errors to zero
        yStatMuonRatio[bin-1]        = 0. ;
        ySystDownMuonRatio[bin-1]    = 0. ;
        ySystUpMuonRatio[bin-1]      = 0. ;

        yStatEleRatio[bin-1]        = 0. ;
        ySystDownEleRatio[bin-1]    = 0. ;
        ySystUpEleRatio[bin-1]      = 0. ; 
        // set errors to zero

    }
    cout << "things are merged lets go to the plotting" << endl;
    TGraph *mcGraph  =  new TGraph(nBins, xCoor, yCoor);
    TGraphAsymmErrors *grCentralStat       =  new TGraphAsymmErrors(nBins, xCoor, yCoor, xErr, xErr, yStat, yStat);
    TGraphAsymmErrors *grCentralStatMuonRatio  =  new TGraphAsymmErrors(nBins, xCoor, yCoorMuonRatio, xErr, xErr, yStatMuonRatio, yStatMuonRatio);
    TGraphAsymmErrors *grCentralStatEleRatio  =  new TGraphAsymmErrors(nBins, xCoor, yCoorEleRatio, xErr, xErr, yStatEleRatio, yStatEleRatio);
    TGraphAsymmErrors *grCentralSystMuonRatio  =  new TGraphAsymmErrors(nBins, xCoor, yCoorMuonRatio, xErr, xErr, ySystDownMuonRatio, ySystUpMuonRatio);
    TGraphAsymmErrors *grCentralSystEleRatio  =  new TGraphAsymmErrors(nBins, xCoor, yCoorEleRatio, xErr, xErr, ySystDownEleRatio, ySystUpEleRatio);


    cout<< " TGraphs are done"<< endl;
    //	grCentralStat->Draw();
    //	grCentralStatMuonRatio->Draw("same");


    grCentralStat->SetMarkerStyle(20);
    grCentralStat->SetMarkerSize(0.8);
    grCentralStat->SetMarkerColor(kBlack);
    grCentralStat->SetLineColor(kBlack);
    //grCentralStat->SetLineWidth(2);
    TGraphAsymmErrors *grCentralSyst  =  new TGraphAsymmErrors(nBins, xCoor, yCoor, xErr, xErr, yStat, yStat);
    grCentralSyst->SetLineColor(kRed+1);
    //grCentralSyst->SetLineWidth(2);
    grCentralSyst->SetMarkerStyle(20);
    grCentralSyst->SetMarkerSize(0.8);



    ////////////////////////////////////// now do the canvas
    string mVariable = "Merged_" + variable;
    TCanvas *can = new TCanvas(mVariable.c_str(), mVariable.c_str(), 600, 900);
    can->cd();
    TPad *pad1 = new TPad("pad1","pad1", 0, 0.4, 1, 1);
    pad1->SetBottomMargin(0);
    pad1->SetRightMargin(0.05);
    pad1->SetLeftMargin(0.1);
    if (logz) pad1->SetLogy(); pad1->SetLogy();
    pad1->SetTickx();
    pad1->SetTicky();
    pad1->Draw();
    pad1->cd();
    grCentralSyst->GetXaxis()->SetLimits(hMuon->GetXaxis()->GetXmin(),hMuon->GetXaxis()->GetXmax());
    //grCentralSyst->GetYaxis()->SetLimits(yCoor[0]-0.4,xCoor[nBins-1]+0.4);
    //grCentralSyst->Draw("AP");
    double MineYMax = hMuon->GetMaximum();
    if (grCentralSyst->GetMaximum() > MineYMax) {
        MineYMax = grCentralSyst->GetMaximum();
    }
    hMuon->GetXaxis()->SetRangeUser(hMuon->GetXaxis()->GetXmin(),hMuon->GetXaxis()->GetXmax());
    if (variable.find("ZNGood") !=  std::string::npos) {
        MineYMax = 65.;
        hMuon->GetXaxis()->SetRangeUser(hMuon->GetXaxis()->GetXmin()+hMuon->GetBinWidth(1),hMuon->GetXaxis()->GetXmax()- 2*hMuon->GetBinWidth(1));
        grCentralSyst->GetXaxis()->SetLimits(hMuon->GetXaxis()->GetXmin()+hMuon->GetBinWidth(1),hMuon->GetXaxis()->GetXmax() -2* hMuon->GetBinWidth(1));
    }


    hMuon->SetFillStyle(3001);
    hMuon->SetFillColor(kRed);
    hMuon->SetLineColor(kRed);
    hMuon->GetYaxis()->SetTitleSize(0.05);
    hMuon->GetYaxis()->SetTitleOffset(0.95);
    if (logz) hMuon->SetMaximum(MineYMax*5);
    else hMuon->SetMaximum(MineYMax*1.5);
    if (doXSec || doNormalize) { 
        hMuon->GetYaxis()->SetTitle("");
        string temp;
        temp = "d#sigma/dH_{T} [pb/GeV]";
        if (variable.find("Pt") != std::string::npos) temp = "d#sigma/dp_{T} [pb/GeV]";
        if (variable.find("Eta") != std::string::npos) temp = "d#sigma/d|#eta| [pb]";
        if (variable.find("ZNGood") != std::string::npos){
            temp = "d#sigma/dN [pb]";

        }
        if (variable.find("ZNGood") != std::string::npos){
            temp = "d#sigma/dN [pb]";
        }

        if (variable.find("SpT") != std::string::npos) {temp = "d#sigma/dSpT [pb]";
            if (doNormalize) temp = "1/#sigma d#sigma/dSpT";
        }
        if (variable.find("PHI") != std::string::npos || variable.find("Phi") !=  std::string::npos ) {temp = "d#sigma/d#Delta#Phi  [pb]";
            if (doNormalize) temp = "1/#sigma d#sigma/d#Delta#Phi";
        }
        const char *tempYAxisTitle = temp.c_str();
        hMuon->GetYaxis()->SetTitle(tempYAxisTitle);
        hEle->GetYaxis()->SetTitle(tempYAxisTitle);

    }
    hMuon->Draw("E2");
    mcGraph->SetLineColor(kRed);
    hEle->SetFillStyle(3002);
    hEle->SetFillColor(kBlue);
    hEle->SetLineColor(kBlue);
    hEle->Draw("E2 same");
    grCentralSyst->Draw("P same");
    grCentralStat->Draw("P same");
    pad1->Draw();
    TPaveText *pt  =  new TPaveText(0.07,0.88,0.99,0.98,"NDC");
    pt->AddText("CMS Preliminary                    #sqrt{s}  =  8 TeV                    #int L dt  =  19.8 fb^{-1}  ");
    pt->SetBorderSize(0);
    pt->SetFillStyle(0);
    pt->Draw();
    // set the legend
    double xLowLeg(0.65), xHighLeg(0.95);
    if (!decrease){
        xLowLeg = 0.14;
        xHighLeg = 0.47;
    }
    TLegend *legend = new TLegend(xLowLeg, 0.66, xHighLeg, 0.91);
    legend->SetHeader("  ");
    hMuon->SetTitle("");
    legend->SetFillStyle(0);
    legend->SetBorderSize(0);
    legend->AddEntry(hMuon, "Muon", "f");
    legend->AddEntry(hEle, "Electron", "f");
    legend->AddEntry(grCentralStat, "Combination", "pl");
    legend->AddEntry(hCombErr, " Comb. Err.", "f");
    legend->Draw();

    can->cd();
    TPad *pad2  =  new TPad("pad2","pad2", 0, 0.236, 1, 0.4);
    pad2->SetTopMargin(0);
    pad2->SetBottomMargin(0);
    pad2->SetRightMargin(0.05);
    pad2->SetLeftMargin(0.1);
    pad2->SetGridy();
    pad2->SetTickx();
    pad2->SetTicky();
    pad2->Draw();
    pad2->cd();
    grCentralStatMuonRatio->SetMarkerStyle(20);
    grCentralStatMuonRatio->SetMarkerSize(0.8);
    grCentralStatMuonRatio->SetMarkerColor(kBlack);
    grCentralStatMuonRatio->SetLineColor(kBlack);
    grCentralSystMuonRatio->SetLineColor(kRed+1);
    grCentralSystMuonRatio->SetMarkerStyle(20);
    grCentralSystMuonRatio->SetMarkerSize(0.8);
    grCentralSystMuonRatio->GetXaxis()->SetLimits(hMuon->GetXaxis()->GetXmin(),hMuon->GetXaxis()->GetXmax());
    if (variable.find("ZNGood") !=  std::string::npos) grCentralSystMuonRatio->GetXaxis()->SetLimits(hMuon->GetXaxis()->GetXmin()+hMuon->GetBinWidth(1),hMuon->GetXaxis()->GetXmax() - hMuon->GetBinWidth(1));
    //grCentralSystMuonRatio->GetYaxis()->SetRangeUser(0.41,1.59);
    grCentralSystMuonRatio->GetYaxis()->SetRangeUser(0.7,1.3);
    grCentralSystMuonRatio->GetYaxis()->SetTitle("Muon / Comb.");
    grCentralSystMuonRatio->GetYaxis()->SetTitleSize(0.14);
    grCentralSystMuonRatio->GetYaxis()->SetTitleOffset(0.35);
    grCentralSystMuonRatio->GetYaxis()->CenterTitle();
    grCentralSystMuonRatio->GetYaxis()->SetLabelSize(0.12);
    grCentralSystMuonRatio->GetYaxis()->SetLabelOffset(0.01);
    grCentralSystMuonRatio->SetTitle("");
    grCentralSystMuonRatio->Draw("AP");
    hCombErr->SetFillColor(kGreen-3);
    hCombErr->SetLineColor(kGreen-3);
    hCombErr->SetFillStyle(3001);
    hCombErr->DrawCopy("E2 same");
    grCentralSystMuonRatio->Draw("P same");
    grCentralStatMuonRatio->Draw("P same");
    TLatex *latexLabel  =  new TLatex();
    latexLabel->SetTextSize(0.15);
    latexLabel->SetTextFont(42);
    latexLabel->SetLineWidth(2);
    latexLabel->SetTextColor(kAzure-5);
    latexLabel->SetNDC();
    latexLabel->DrawLatex(0.15,0.09,"Muon");
    pad2->Draw();
    pad2->cd();

    can->cd();
    TPad *pad3  =  new TPad("pad3","pad3", 0, 0., 1, 0.236);
    pad3->SetTopMargin(0);
    pad3->SetBottomMargin(0.3);
    pad3->SetRightMargin(0.05);
    pad3->SetLeftMargin(0.1);
    pad3->SetGridy();
    pad3->SetTickx();
    pad3->SetTicky();
    pad3->Draw();
    pad3->cd();
    grCentralStatEleRatio->SetMarkerStyle(20);
    grCentralStatEleRatio->SetMarkerSize(0.8);
    grCentralStatEleRatio->SetMarkerColor(kBlack);
    grCentralStatEleRatio->SetLineColor(kBlack);
    grCentralSystEleRatio->SetLineColor(kRed+1);
    grCentralSystEleRatio->SetMarkerStyle(20);
    grCentralSystEleRatio->SetMarkerSize(0.8);
    grCentralSystEleRatio->GetXaxis()->SetLimits(hMuon->GetXaxis()->GetXmin(),hMuon->GetXaxis()->GetXmax());
    if (variable.find("ZNGood") !=  std::string::npos) grCentralSystEleRatio->GetXaxis()->SetLimits(hMuon->GetXaxis()->GetXmin()+hMuon->GetBinWidth(1),hMuon->GetXaxis()->GetXmax() - hMuon->GetBinWidth(1));
    grCentralSystEleRatio->GetXaxis()->SetTitle(hMuon->GetXaxis()->GetTitle());
    grCentralSystEleRatio->GetXaxis()->SetTitleSize(0.1);
    grCentralSystEleRatio->GetXaxis()->SetTitleOffset(1.14);
    grCentralSystEleRatio->GetXaxis()->SetLabelSize(0.1);
    grCentralSystEleRatio->GetXaxis()->SetLabelOffset(0.014);
    grCentralSystEleRatio->GetYaxis()->SetRangeUser(0.65,1.35);
    grCentralSystEleRatio->GetYaxis()->SetTitle("Elec. / Comb.");
    grCentralSystEleRatio->GetYaxis()->SetTitleSize(0.1);
    grCentralSystEleRatio->GetYaxis()->SetTitleOffset(0.48);
    grCentralSystEleRatio->GetYaxis()->CenterTitle();
    grCentralSystEleRatio->GetYaxis()->SetLabelSize(0.085);
    grCentralSystEleRatio->GetYaxis()->SetLabelOffset(0.012);
    grCentralSystEleRatio->SetTitle("");
    grCentralSystEleRatio->Draw("AP");
    hCombErr->SetFillColor(kGreen-3);
    hCombErr->SetLineColor(kGreen-3);
    hCombErr->SetFillStyle(3001);
    hCombErr->DrawCopy("E2 same");
    grCentralStatEleRatio->Draw("P same");
    TLatex *latexLabel1  =  new TLatex();
    latexLabel1->SetTextSize(0.11);
    latexLabel1->SetTextFont(42);
    latexLabel1->SetLineWidth(2);
    latexLabel1->SetTextColor(kOrange+2);
    latexLabel1->SetNDC();
    latexLabel1->DrawLatex(0.15,0.35,"Electron");
    pad3->Draw();
    pad3->cd();
    can->cd();

    string outputFileNamePNG = OUTPUTDIRECTORY;
    if (doXSec) outputFileNamePNG += "CompLeptonsAndComb_XSec_";
    else if (doNormalize) outputFileNamePNG += "CompLeptonsAndComb_Normalized_";

    ostringstream optionCorrStr; optionCorrStr << optionCorr;
    outputFileNamePNG += variable;
    outputFileNamePNG += "_CorrelationOption_" + optionCorrStr.str();
    if (doVarWidth) outputFileNamePNG += "_VarWidth";
    outputFileNamePNG += ".pdf";

    can->Print(outputFileNamePNG.c_str());

}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void dumpElements(TMatrixD& a)
{
    cout << endl;
    const int nrows = a.GetNrows();
    const int ncols = a.GetNcols();
    if (nrows == ncols) cout << "determinant  =  " << a.Determinant() << endl;
    a.Print();
    cout << endl;
    return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void dumpElements(TVectorD& a)
{
    cout << endl;
    a.Print();
    cout << endl;
    return;
}

