#include <iostream>
#include <fstream>
#include <vector>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include "variablesOfInterestZJets.h"
#include "PlotSettings.h"
#include "Combination.h"
#include "BLUEMeth.h"

using namespace std;

void createInclusivePlots(bool doNormalized, TString outputFileName, TH1D *hUnfData, vector<TH2D*> hCov, TH2D *hCovSyst, TH1D *hMadGenCrossSection, TH1D *hSheGenCrossSection, TH1D *hPowGenCrossSection);
void createTable(TString outputFileName, TString variable, bool doNormalized, TH1D *hCombination, vector<TH2D*> &covuxaxb, TH2D* covxaxbSyst);
void Combination(TString unfoldDir, TString combDir, TString algo, int jetPtMin, int jetEtaMax, bool diagXChanCov, bool fullXChanCov, bool fullSChanCov, bool modifiedSWA, TString gen1, TString gen2, TString variable, bool doNormalized)
{
    //--- create output directory if does not exist ---
    system("mkdir -p " + combDir);

    int start = 0;
    int end = NVAROFINTERESTZJETS;

    if (variable != "") {
        start = findVariable(variable);
        if (start >= 0) {
            end = start + 1;
        }
        else {
            cerr << "\nError: variable " << variable << " is not interesting." << endl;
            cerr << "See below the list of interesting variables:" << endl;
            for (unsigned int i = 0; i < NVAROFINTERESTZJETS; ++i) {
                cerr << "\t" << i << ": " << VAROFINTERESTZJETS[i].name << "\n" << endl;
            }
            return;
        }
    }

    //--- loop over the variable of interest ---
    for (int i = start; i < end; ++i) {
        variable = VAROFINTERESTZJETS[i].name;

        //--- fetch the electron and muon unfolded files ---
        TString commonName = "_unfolded_" + variable + "_" + algo;
        commonName += "_JetPtMin_";
        commonName += jetPtMin;
        commonName += "_JetEtaMax_";
        commonName += jetEtaMax;
        commonName += "_MGPYTHIA6_" + gen1 + "_" + gen2;
        commonName += doNormalized ? "_normalized" : "";
        commonName += ".root";
        TFile *fDE = new TFile(unfoldDir + "DE" + commonName);
        if (!fDE->IsOpen()) {
            cerr << "\nError: file " << unfoldDir + "DE" + commonName << " does not exist." << endl;
            cerr << "       You can create it using\n\t ./runUnfoldingZJets lepSel=DE variable=" << variable << endl;
            cerr << "Skipping variable " << variable << ".\n" << endl;
            continue;
        }

        TFile *fDMu = new TFile(unfoldDir + "DMu" + commonName);
        if (!fDMu->IsOpen()) {
            cerr << "\nError: file " << unfoldDir + "DMu" + commonName << " does not exist." << endl;
            cerr << "       You can create it using\n\t ./runUnfoldingZJets lepSel=DMu variable=" << variable << endl;
            cerr << "Skipping variable " << variable << ".\n" << endl;
            continue;
        }
        //---------------------------------------------------------------------

        //--- fetch the cross section histogram and the covariance matrices ---
        fDE->cd();
        TH1D *hUnfDE = (TH1D*) fDE->Get("UnfDataCentral");
        TH1D *hMadGenDE = (TH1D*) fDE->Get("hMadGenDYJetsCrossSection");
        TH1D *hGen1DE = (TH1D*) fDE->Get("hGen1DYJetsCrossSection");
        TH1D *hGen2DE = (TH1D*) fDE->Get("hGen2DYJetsCrossSection");
        TH2D *hCovDataStatDE = (TH2D*) fDE->Get("CovDataStat");
        TH2D *hCovMCStatDE = (TH2D*) fDE->Get("CovMCStat");
        TH2D *hCovPUSystDE = (TH2D*) fDE->Get("CovPU");
        TH2D *hCovJERSystDE = (TH2D*) fDE->Get("CovJER");
        TH2D *hCovXSecSystDE = (TH2D*) fDE->Get("CovXSec");
        TH2D *hCovLumiSystDE = (TH2D*) fDE->Get("CovLumi");
        TH2D *hCovSFSystDE = (TH2D*) fDE->Get("CovSF");
        TH2D *hCovJESSystDE = (TH2D*) fDE->Get("CovJES");
        TH2D *hCovLESSystDE = (TH2D*) fDE->Get("CovLES");
        TH2D *hCovLERSystDE = (TH2D*) fDE->Get("CovLER");
        TH2D *hCovSherpaUnfSystDE = (TH2D*) fDE->Get("CovSherpaUnf");

        fDMu->cd();
        TH1D *hUnfDMu = (TH1D*) fDMu->Get("UnfDataCentral");
        TH1D *hMadGenDMu = (TH1D*) fDMu->Get("hMadGenDYJetsCrossSection");
        TH1D *hGen1DMu = (TH1D*) fDMu->Get("hGen1DYJetsCrossSection");
        TH1D *hGen2DMu = (TH1D*) fDMu->Get("hGen2DYJetsCrossSection");
        TH2D *hCovDataStatDMu = (TH2D*) fDMu->Get("CovDataStat");
        TH2D *hCovMCStatDMu = (TH2D*) fDMu->Get("CovMCStat");
        TH2D *hCovPUSystDMu = (TH2D*) fDMu->Get("CovPU");
        TH2D *hCovJERSystDMu = (TH2D*) fDMu->Get("CovJER");
        TH2D *hCovXSecSystDMu = (TH2D*) fDMu->Get("CovXSec");
        TH2D *hCovLumiSystDMu = (TH2D*) fDMu->Get("CovLumi");
        TH2D *hCovSFSystDMu = (TH2D*) fDMu->Get("CovSF");
        TH2D *hCovJESSystDMu = (TH2D*) fDMu->Get("CovJES");
        TH2D *hCovLESSystDMu = (TH2D*) fDMu->Get("CovLES");
        TH2D *hCovLERSystDMu = (TH2D*) fDMu->Get("CovLER");
        TH2D *hCovSherpaUnfSystDMu = (TH2D*) fDMu->Get("CovSherpaUnf");
        //---------------------------------------------------------------------

        //--- create the output root file ---
        TString outputFileName = combDir + variable + "_" + algo;
        outputFileName += "_diagXChanCov_"; 
        outputFileName += (int) diagXChanCov;
        outputFileName += "_fullXChanCov_"; 
        outputFileName += (int) fullXChanCov;
        outputFileName += "_fullSChanCov_"; 
        outputFileName += (int) fullSChanCov;
        outputFileName += "_modifiedSWA_"; 
        outputFileName += (int) modifiedSWA;
        outputFileName += "_JetPtMin_";
        outputFileName += jetPtMin;
        outputFileName += "_JetEtaMax_";
        outputFileName += jetEtaMax;
        outputFileName += "_MGPYTHIA6_" + gen1 + "_" + gen2;
        outputFileName += doNormalized ? "_normalized" : "";

        TFile *outputRootFile = new TFile(outputFileName + ".root", "RECREATE");
        //---------------------------------------------------------------------


        //--- fill in the vector of measurements ---
        vector<TH1D*> measurements{hUnfDE, hUnfDMu};
        //---------------------------------------------------------------------

        //--- fill in the vector of vector of covariances ---
        vector<vector<TH2D*>> covariances{
            {hCovDataStatDE, hCovMCStatDE, hCovPUSystDE, hCovJERSystDE, hCovXSecSystDE, hCovLumiSystDE, hCovSFSystDE, hCovJESSystDE, hCovLESSystDE, hCovLERSystDE, hCovSherpaUnfSystDE}, 
                {hCovDataStatDMu, hCovMCStatDMu, hCovPUSystDMu, hCovJERSystDMu, hCovXSecSystDMu, hCovLumiSystDMu, hCovSFSystDMu, hCovJESSystDMu, hCovLESSystDMu, hCovLERSystDMu, hCovSherpaUnfSystDMu}
        };
        //---------------------------------------------------------------------


        //--- create objects to be filled with output of combination ---
        vector<TH2D*> covuxaxb(covariances[0].size(), NULL); // each covariance matrix
        TH2D* covxaxb = NULL; // total covariance matrix
        TH2D* covxaxbSyst = NULL; // total syst covariance matrix
        TH1D* hCombination = NULL; // combined cross section
        TH1D* hMadGenCombined = NULL;
        TH1D* hGen1Combined = NULL;
        TH1D* hGen2Combined = NULL;
        //---------------------------------------------------------------------

        //--- create the BLUEMeth object to compute the covariance ---
        BLUEMeth* blueXSec = new BLUEMeth(measurements, covariances, variable);
        //---------------------------------------------------------------------

        //--- set up how you want to combine the channels and do the combination ---
        //one could rerun first do the combination with simple weighted average 
        //
        // hCombination = blue->GetCombination(false, false, false, covuxaxb, covxaxb);
        //
        //and then do it with full covariance to get the proper error without fetching 
        //the output of the measurement:
        //
        // blue->GetCombination(true, true, true, covuxaxb, covxaxb);
        //
        hCombination = blueXSec->GetCombination(diagXChanCov, fullXChanCov, fullSChanCov, modifiedSWA, covuxaxb, covxaxb);

        covxaxbSyst = (TH2D*) covxaxb->Clone();
        covxaxbSyst->Reset();
        covxaxbSyst->Add(covuxaxb[1]);
        covxaxbSyst->Add(covuxaxb[2]);
        covxaxbSyst->Add(covuxaxb[3]);
        covxaxbSyst->Add(covuxaxb[4]);
        covxaxbSyst->Add(covuxaxb[5]);
        covxaxbSyst->Add(covuxaxb[6]);
        covxaxbSyst->Add(covuxaxb[7]);
        covxaxbSyst->Add(covuxaxb[8]);
        covxaxbSyst->Add(covuxaxb[9]);
        covxaxbSyst->Add(covuxaxb[10]);
        covxaxbSyst->SetName("CombCovTotSyst");

        int nbins = covxaxb->GetNbinsX();
        for (int i = 1; i <= nbins; i++) {
            std::cout << i << "  " << sqrt(covxaxb->GetBinContent(i, i)) << std::endl;
        }

        hMadGenCombined = (TH1D*) hMadGenDE->Clone();
        hMadGenCombined->Add(hMadGenDMu);
        hMadGenCombined->Scale(0.5);
        hMadGenCombined->SetName("hMadCombGenDYJetsCrossSection");

        hGen1Combined = (TH1D*) hGen1DE->Clone();
        hGen1Combined->Add(hGen1DMu);
        hGen1Combined->Scale(0.5);
        hGen1Combined->SetName("hCombGen1DYJetsCrossSection");

        hGen2Combined = (TH1D*) hGen2DE->Clone();
        hGen2Combined->Add(hGen2DMu);
        hGen2Combined->Scale(0.5);
        hGen2Combined->SetName("hCombGen2DYJetsCrossSection");

        //---------------------------------------------------------------------


        TCanvas *crossSectionPlot = makeCrossSectionPlot("", variable, doNormalized, hCombination, covxaxbSyst, hMadGenCombined, hGen1Combined, hGen2Combined); 
        crossSectionPlot->Draw();
        crossSectionPlot->SaveAs(outputFileName + ".png");
        crossSectionPlot->SaveAs(outputFileName + ".pdf");
        crossSectionPlot->SaveAs(outputFileName + ".ps");
        crossSectionPlot->SaveAs(outputFileName + ".C");

        //--- print out the combined cross section measurement ---
        for (int i = 1; i <= hCombination->GetNbinsX(); ++i) {
            cout << hCombination->GetBinContent(i) << endl;
        }
        //--- print out break down of errors ---
        for (int i = 2; i <= 11; ++i) {
            cout << hCombination->GetBinContent(i);
            for (int j = 0; j < covuxaxb.size(); ++j) {
                cout << " +/- " << sqrt(covuxaxb[j]->GetBinContent(i,i))*100./hCombination->GetBinContent(i) << "%";
            }
            cout << endl;
        }

        createTable(outputFileName, variable, doNormalized, hCombination, covuxaxb, covxaxbSyst);
        if (variable.Index("ZNGoodJets_Zexc") >= 0) {
            createInclusivePlots(doNormalized, outputFileName, hCombination, covuxaxb, covxaxbSyst, hMadGenCombined, hGen1Combined, hGen2Combined);
        }


        //--- save results and inputs to root file ---
        outputRootFile->cd();
        crossSectionPlot->Write();
        hCombination->Write("CombDataCentral");
        hMadGenCombined->Write();
        hGen1Combined->Write();
        hGen2Combined->Write();
        covxaxb->Write("CombCovTot");
        covxaxbSyst->Write("CombCovTotSyst");
        for (unsigned int i = 0; i < covuxaxb.size(); ++i) {
            covuxaxb[i]->Write();
        }

        hUnfDE->Write("DEUnfDataCentral");
        hCovDataStatDE->Write("DECovDataStat");
        hCovMCStatDE->Write("DECovMCStat");
        hCovPUSystDE->Write("DECovPU");
        hCovJERSystDE->Write("DECovJER");
        hCovXSecSystDE->Write("DECovXSec");
        hCovLumiSystDE->Write("DECovLumi");
        hCovSFSystDE->Write("DECovSF");
        hCovJESSystDE->Write("DECovJES");
        hCovLESSystDE->Write("DECovLES");
        hCovLERSystDE->Write("DECovLER");

        hUnfDMu->Write("DMuUnfDataCentral");
        hCovDataStatDMu->Write("DMuCovDataStat");
        hCovMCStatDMu->Write("DMuCovMCStat");
        hCovPUSystDMu->Write("DMuCovPU");
        hCovJERSystDMu->Write("DMuCovJER");
        hCovXSecSystDMu->Write("DMuCovXSec");
        hCovLumiSystDMu->Write("DMuCovLumi");
        hCovSFSystDMu->Write("DMuCovSF");
        hCovJESSystDMu->Write("DMuCovJES");
        hCovLESSystDMu->Write("DMuCovLES");
        hCovLERSystDMu->Write("DMuCovLER");

        //--- Close all files ---
        outputRootFile->Close();
        fDE->Close();
        fDMu->Close();

        //if (end == start + 1) system("display " + outputFileName + ".png &");
        //if (end == start + 1 && variable == "ZNGoodJets_Zexc") system("display " + outputFileName.ReplaceAll("ZNGoodJets_Zexc", "ZNGoodJets_Zinc") + ".png &");
    }
}

void createInclusivePlots(bool doNormalized, TString outputFileName, TH1D *hUnfData, vector<TH2D*> hCov, TH2D *hCovSyst, TH1D *hMadGenCrossSection, TH1D *hSheGenCrossSection, TH1D *hPowGenCrossSection)
{
    TH1D *hInc = (TH1D*) hUnfData->Clone("ZNGoodJets_Zinc");
    TH1D *hIncMad = (TH1D*) hMadGenCrossSection->Clone("ZNGoodJets_Zinc_Mad");
    TH1D *hIncShe = (TH1D*) hSheGenCrossSection->Clone("ZNGoodJets_Zinc_She");
    TH1D *hIncPow = (TH1D*) hPowGenCrossSection->Clone("ZNGoodJets_Zinc_Pow");
    TH2D *hIncCovSyst = (TH2D*) hCovSyst->Clone("CovSystTot");
    vector<TH2D*> hCovInc;
    hCovInc.push_back((TH2D*) hCov[0]->Clone("CovDataStat"));
    hCovInc.push_back((TH2D*) hCov[1]->Clone("CovMCStat"));
    hCovInc.push_back((TH2D*) hCov[2]->Clone("CovPU"));
    hCovInc.push_back((TH2D*) hCov[3]->Clone("CovJER"));
    hCovInc.push_back((TH2D*) hCov[4]->Clone("CovXSec"));
    hCovInc.push_back((TH2D*) hCov[5]->Clone("CovLumi"));
    hCovInc.push_back((TH2D*) hCov[6]->Clone("CovSF"));
    hCovInc.push_back((TH2D*) hCov[7]->Clone("CovJES"));
    hCovInc.push_back((TH2D*) hCov[8]->Clone("CovLES"));
    hCovInc.push_back((TH2D*) hCov[9]->Clone("CovLER"));
    hCovInc.push_back((TH2D*) hCov[10]->Clone("CovSherpaUnf"));

    int nBins = hInc->GetNbinsX();
    for (int i = 1; i <= nBins; i++) {
        double binSum = 0;
        double binSumMad = 0;
        double binSumShe = 0;
        double binSumPow = 0;
        double binStatError2 = 0;
        double binStatMadError2 = 0;
        double binStatSheError2 = 0;
        double binStatPowError2 = 0;
        double binCov[11] = {0};
        double binCovSystError2 = 0;
        for (int j = i; j <= nBins; j++) {
            binSum += hInc->GetBinContent(j);
            binSumMad += hIncMad->GetBinContent(j);
            binSumShe += hIncShe->GetBinContent(j);
            binSumPow += hIncPow->GetBinContent(j);
            binStatError2 += pow(hInc->GetBinError(j), 2);
            binStatMadError2 += pow(hIncMad->GetBinError(j), 2);
            binStatSheError2 += pow(hIncShe->GetBinError(j), 2);
            binStatPowError2 += pow(hIncPow->GetBinError(j), 2);
            binCovSystError2 += hIncCovSyst->GetBinError(j, j);
            for (int k = 0; k < 11; k++) {
                binCov[k] += hCovInc[k]->GetBinContent(j, j);
            }
        }
        hInc->SetBinContent(i, binSum);
        hIncMad->SetBinContent(i, binSumMad);
        hIncShe->SetBinContent(i, binSumShe);
        hIncPow->SetBinContent(i, binSumPow);
        hInc->SetBinError(i, sqrt(binStatError2));
        hIncMad->SetBinError(i, sqrt(binStatMadError2));
        hIncShe->SetBinError(i, sqrt(binStatSheError2));
        hIncPow->SetBinError(i, sqrt(binStatPowError2));
        hIncCovSyst->SetBinError(i, i, binCovSystError2);
        for (int k = 0; k < 11; k++) {
            hCovInc[k]->SetBinContent(i, i, binCov[k]);
        }
    }

    TCanvas *crossSectionPlot = makeCrossSectionPlot("", TString("ZNGoodJets_Zinc"), doNormalized, hInc, hIncCovSyst, hIncMad, hIncShe, hIncPow); 
    outputFileName.ReplaceAll("ZNGoodJets_Zexc", "ZNGoodJets_Zinc");
    crossSectionPlot->Draw();
    crossSectionPlot->SaveAs(outputFileName + ".png");
    crossSectionPlot->SaveAs(outputFileName + ".pdf");
    crossSectionPlot->SaveAs(outputFileName + ".eps");
    crossSectionPlot->SaveAs(outputFileName + ".ps");
    crossSectionPlot->SaveAs(outputFileName + ".C");
    createTable(outputFileName, TString("ZNGoodJets_Zinc"), doNormalized, hInc, hCovInc, hIncCovSyst);
}

void createTable(TString outputFileName, TString variable, bool doNormalized, TH1D *hCombination, vector<TH2D*> &covuxaxb, TH2D* covxaxbSyst)
{
    cout << "Hello" << endl;    
    //--- print out break down of errors ---

    TString title = hCombination->GetTitle();
    int nBins = hCombination->GetNbinsX();
    TString var = "";
    TString dSigma = "";
    TString xtitle = hCombination->GetXaxis()->GetTitle();
    createTitleVariableAnddSigma(variable, doNormalized, xtitle, title, var, dSigma);
    cout << "Title: " << title << endl;
    cout << "Var: " << var << endl;
    cout << "dSig: " << dSigma << endl;
    cout << "nBins: " << nBins << endl;

    TString table = "\\begin{table}[htb!]\n\\begin{center}\n";
    table += "\\caption{Differential cross section in " + title;
    table += " and break down of the systematic uncertainties for the ";
    table += "combinaton of both decay channels.}\n";
    table += "\\scriptsize{\n";
    table += "\\begin{tabular}{c|cc|ccccccccc}\n";
    table += "\\multicolumn{12}{c}{" + title + "} \\\\\n";
    table += var + " & " + dSigma + " & \\tiny{Tot. Unc [\\%]} & ";
    table += "\\tiny{stat [\\%]} & \\tiny{MC stat [\\%]}  & \\tiny{JES [\\%]} & \\tiny{JER [\\%]} & ";
    table += "\\tiny{PU [\\%]} & \\tiny{XSEC [\\%]} & \\tiny{Lumi [\\%]} & ";
    table += "\\tiny{LES [\\%]} & \\tiny{LER [\\%]} & ";
    table += "\\tiny{Unf [\\%]} & \\tiny{Eff [\\%]} \\\\\\hline\n";

    int start = 1;
    if (title.Index("multiplicity", 0, TString::ECaseCompare::kIgnoreCase) >= 0) {
        start = 2; 
        //nBins--;
    }
    if (title.Index("jet $p_{\\text{T}}$", 0, TString::ECaseCompare::kIgnoreCase) >= 0) start = 3; 

    for (int i = start; i <= nBins; ++i) {
        double xs = hCombination->GetBinContent(i);
        TString numbers;
        if (title.Index("exclusive jet multiplicity", 0,  TString::ECaseCompare::kIgnoreCase) >= 0) {
            numbers.Form("= %d", i - 1);
        }
        else if (title.Index("inclusive jet multiplicity", 0,  TString::ECaseCompare::kIgnoreCase) >= 0) {
            numbers.Form("$\\geq$ %d", i - 1);
        }
        else {
            numbers.Form("$%g \\ -\\ %g$", hCombination->GetBinLowEdge(i), hCombination->GetBinLowEdge(i+1));
        }
        table += numbers + " & "; 
        numbers.Form("%#.3g", xs);
        table += numbers + " & ";
        // total uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[0]->GetBinContent(i,i) + covxaxbSyst->GetBinContent(i,i))*100./xs);
        //numbers.Form("%#.2g", sqrt(covuxaxb[0]->GetBinContent(i,i) + 
        //                           covuxaxb[1]->GetBinContent(i,i) +
        //                           covuxaxb[2]->GetBinContent(i,i) +
        //                           covuxaxb[3]->GetBinContent(i,i) +
        //                           covuxaxb[4]->GetBinContent(i,i) +
        //                           covuxaxb[5]->GetBinContent(i,i) +
        //                           covuxaxb[6]->GetBinContent(i,i) +
        //                           covuxaxb[7]->GetBinContent(i,i) +
        //                           covuxaxb[8]->GetBinContent(i,i) )*100./xs);
        table += numbers + " & ";
        // stat uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[0]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // MC stat uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[1]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // JES uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[7]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // JER uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[3]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // PU uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[2]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // XSec uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[4]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // Lumi uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[5]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // LES uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[8]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // LER uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[9]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // Unf uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[10]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // SF uncertinaty
        numbers.Form("%#.2g", sqrt(covuxaxb[6]->GetBinContent(i,i))*100./xs);
        table += numbers + " \\\\\n";
    }

    table += "\\end{tabular}}\n";
    table += "\\label{tab:comb" + variable + "}\n";
    table += "\\end{center}\\end{table}\n";
    ofstream out(outputFileName + ".tex");
    out << table;
    out.close();
    cout << table << endl;
}

