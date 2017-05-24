#include <iostream>
#include <fstream>
#include <vector>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TParameter.h>
#include "variablesOfInterestZJets.h"
#include "PlotSettings.h"
#include "Combination.h"
#include "BLUEMeth.h"
#include "ConfigVJets.h"
#include "SectionedConfig.h"
#include "getFilesAndHistogramsZJets.h"
#include "functions.h"

extern ConfigVJets cfg; //defined in runCombination.cc

using namespace std;

//void createInclusivePlots(bool doNormalized, TString outputFileName, TH1D *hUnfData, vector<TH2D*> hCov, TH2D *hCovSyst, TH1D *hMadGenCrossSection, TH1D *hSheGenCrossSection, TH1D *hPowGenCrossSection);
//void createInclusivePlots(bool doNormalized, TString outputFileName, TH1D *hUnfData, vector<TH2D*> hCov, TH2D *hCovSyst, TH1D *hMadGenCrossSection, TH1D *hSheGenCrossSection);
void createInclusivePlots(bool doNormalized, TString outputFileDir, TString outputFileName, 
			  TH1 *hUnfData, vector<TH2*> hCov, TH2 *hCovSyst,
			  const std::vector<std::string>& predictions, int nFirstBinsToSkip, int nLastBinsToSkip);
//void createInclusivePlots(TString outputFileName, TH1D *hUnfData, vector<TH2D*> hCov, TH2D *hCovSyst, TH1D *hUnfDataNorm, vector<TH2D*> hCovNorm, TH2D *hCovSystNorm, TH1D *hMadGenCrossSection, TH1D *hSheGenCrossSection, TH1D *hPowGenCrossSection);
void createTable(TString outputFileName, TString variable, bool doNormalized, TH1 *hCombination, vector<TH2*> &covuxaxb, TH2* covxaxbSyst, bool withLERS = false);

void Combination(TString unfoldDir, TString combDir, TString algo, 
		 int jetPtMin, int jetEtaMax, 
		 bool diagXChanCov, bool fullXChanCov, bool fullSChanCov, 
		 bool modifiedSWA, TString variable, bool doNormalized)
{
    //--- create output directory if does not exist ---
    system("mkdir -p " + combDir);

    int start = 0;
    int end = NVAROFINTERESTZJETS;

    std::vector<std::string> predictions = cfg.getVS("predictions");
    
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
        commonName += "_MGPYTHIA6_"; //+ gen1 + "_" + gen2;
        //TString commonName2;
        //if(doNormband) commonName2 = commonName + "_normalized.root";
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
/*
        TFile *fDENorm;
        TFile *fDMuNorm;
        if(doNormband){
            fDENorm = new TFile(unfoldDir + "DE" + commonName2);
            if (!fDENorm->IsOpen()) {
                cerr << "\nError: file " << unfoldDir + "DE" + commonName2 << " does not exist for band." << endl;
                cerr << " You can create it using\n\t ./runUnfoldingZJets lepSel=DE doNormalized=true variable=" << variable << endl;
                cerr << "Skipping variable " << variable << ".\n" << endl;
                continue;
            }

            fDMuNorm = new TFile(unfoldDir + "DMu" + commonName2);
            if (!fDMuNorm->IsOpen()) {
                cerr << "\nError: file " << unfoldDir + "DMu" + commonName2 << " does not exist for band." << endl;
                cerr << " You can create it using\n\t ./runUnfoldingZJets lepSel=DMu doNormalized=true variable=" << variable << endl;
                cerr << "Skipping variable " << variable << ".\n" << endl;
                continue;
            }
        }
*/
        //--- fetch the cross section histogram and the covariance matrices ---
        fDE->cd();
        TH1 *hUnfDE = (TH1*) fDE->Get("UnfDataCentral");
        TH1 *hMadGenDE = (TH1*) fDE->Get("hMadGenDYJetsCrossSection");
        TH1 *hGen1DE = (TH1*) fDE->Get("hGen1DYJetsCrossSection");
        //TH1 *hGen2DE = (TH1*) fDE->Get("hGen2DYJetsCrossSection");
        TH2 *hCovDataStatDE = (TH2*) fDE->Get("CovDataStat");
        TH2 *hCovMCStatDE = (TH2*) fDE->Get("CovMCStat");
        TH2 *hCovPUSystDE = (TH2*) fDE->Get("CovPU");
        TH2 *hCovJERSystDE = (TH2*) fDE->Get("CovJER");
        TH2 *hCovXSecSystDE = (TH2*) fDE->Get("CovXSec");
        TH2 *hCovLumiSystDE = (TH2*) fDE->Get("CovLumi");
        TH2 *hCovSFSystDE = (TH2*) fDE->Get("CovSF");
        TH2 *hCovJESSystDE = (TH2*) fDE->Get("CovJES");
        TH2 *hCovLESSystDE = (TH2*) fDE->Get("CovLES");
        TH2 *hCovLERSystDE = (TH2*) fDE->Get("CovLER");
        TH2 *hCovSherpaUnfSystDE = (TH2*) fDE->Get("CovSherpaUnf");

        fDMu->cd();
        TH1 *hUnfDMu = (TH1*) fDMu->Get("UnfDataCentral");
        TH1 *hMadGenDMu = (TH1*) fDMu->Get("hMadGenDYJetsCrossSection");
        TH1 *hGen1DMu = (TH1*) fDMu->Get("hGen1DYJetsCrossSection");
        //TH1 *hGen2DMu = (TH1*) fDMu->Get("hGen2DYJetsCrossSection");
        TH2 *hCovDataStatDMu = (TH2*) fDMu->Get("CovDataStat");
        TH2 *hCovMCStatDMu = (TH2*) fDMu->Get("CovMCStat");
        TH2 *hCovPUSystDMu = (TH2*) fDMu->Get("CovPU");
        TH2 *hCovJERSystDMu = (TH2*) fDMu->Get("CovJER");
        TH2 *hCovXSecSystDMu = (TH2*) fDMu->Get("CovXSec");
        TH2 *hCovLumiSystDMu = (TH2*) fDMu->Get("CovLumi");
        TH2 *hCovSFSystDMu = (TH2*) fDMu->Get("CovSF");
        TH2 *hCovJESSystDMu = (TH2*) fDMu->Get("CovJES");
        TH2 *hCovLESSystDMu = (TH2*) fDMu->Get("CovLES");
        TH2 *hCovLERSystDMu = (TH2*) fDMu->Get("CovLER");
        TH2 *hCovSherpaUnfSystDMu = (TH2*) fDMu->Get("CovSherpaUnf");
        //---------------------------------------------------------------------
	
	/*
        // --- prepare the histograms for normalized band ---
        TH1 *hUnfDENorm;
        TH2 *hCovDataStatDENorm;
        TH2 *hCovMCStatDENorm;
        TH2 *hCovPUSystDENorm;
        TH2 *hCovJERSystDENorm;
        TH2 *hCovXSecSystDENorm;
        TH2 *hCovLumiSystDENorm;
        TH2 *hCovSFSystDENorm;
        TH2 *hCovJESSystDENorm;
        TH2 *hCovLESSystDENorm;
        TH2 *hCovLERSystDENorm;
        TH2 *hCovSherpaUnfSystDENorm;

        TH1 *hUnfDMuNorm;
        TH2 *hCovDataStatDMuNorm;
        TH2 *hCovMCStatDMuNorm;
        TH2 *hCovPUSystDMuNorm;
        TH2 *hCovJERSystDMuNorm;
        TH2 *hCovXSecSystDMuNorm;
        TH2 *hCovLumiSystDMuNorm;
        TH2 *hCovSFSystDMuNorm;
        TH2 *hCovJESSystDMuNorm;
        TH2 *hCovLESSystDMuNorm;
        TH2 *hCovLERSystDMuNorm;
        TH2 *hCovSherpaUnfSystDMuNorm;

        if(doNormband){
            fDENorm->cd();
            hUnfDENorm = (TH1*) fDENorm->Get("UnfDataCentral");
            hCovDataStatDENorm = (TH2*) fDENorm->Get("CovDataStat");
            hCovMCStatDENorm = (TH2*) fDENorm->Get("CovMCStat");
            hCovPUSystDENorm = (TH2*) fDENorm->Get("CovPU");
            hCovJERSystDENorm = (TH2*) fDENorm->Get("CovJER");
            hCovXSecSystDENorm = (TH2*) fDENorm->Get("CovXSec");
            hCovLumiSystDENorm = (TH2*) fDENorm->Get("CovLumi");
            hCovSFSystDENorm = (TH2*) fDENorm->Get("CovSF");
            hCovJESSystDENorm = (TH2*) fDENorm->Get("CovJES");
            hCovLESSystDENorm = (TH2*) fDENorm->Get("CovLES");
            hCovLERSystDENorm = (TH2*) fDENorm->Get("CovLER");
            hCovSherpaUnfSystDENorm = (TH2*) fDENorm->Get("CovSherpaUnf");

            fDMuNorm->cd();
            hUnfDMuNorm = (TH1*) fDMuNorm->Get("UnfDataCentral");
            hCovDataStatDMuNorm = (TH2*) fDMuNorm->Get("CovDataStat");
            hCovMCStatDMuNorm = (TH2*) fDMuNorm->Get("CovMCStat");
            hCovPUSystDMuNorm = (TH2*) fDMuNorm->Get("CovPU");
            hCovJERSystDMuNorm = (TH2*) fDMuNorm->Get("CovJER");
            hCovXSecSystDMuNorm = (TH2*) fDMuNorm->Get("CovXSec");
            hCovLumiSystDMuNorm = (TH2*) fDMuNorm->Get("CovLumi");
            hCovSFSystDMuNorm = (TH2*) fDMuNorm->Get("CovSF");
            hCovJESSystDMuNorm = (TH2*) fDMuNorm->Get("CovJES");
            hCovLESSystDMuNorm = (TH2*) fDMuNorm->Get("CovLES");
            hCovLERSystDMuNorm = (TH2*) fDMuNorm->Get("CovLER");
            hCovSherpaUnfSystDMuNorm = (TH2*) fDMuNorm->Get("CovSherpaUnf");
        }
*/
	TString genList;
	for(auto g: predictions){
	  genList += TString("_") + g;
	}
	genList.ReplaceAll("DYJets_", "");
	genList.ReplaceAll("UNFOLDING", "FXFX");

        //--- create the output root file ---
        TString outputFileName = variable + "_" + algo;
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
	//outputFileName += genList;
        outputFileName += doNormalized ? "_normalized" : "";
	TString outputFilePath = combDir + outputFileName;

        TFile *outputRootFile = new TFile(outputFilePath + ".root", "RECREATE");
        //---------------------------------------------------------------------


        //--- fill in the vector of measurements ---
        vector<TH1*> measurements{hUnfDE, hUnfDMu};
        //vector<TH1*> measurementsNorm;
        //if(doNormband) measurementsNorm = {hUnfDENorm, hUnfDMuNorm};
        //---------------------------------------------------------------------

        //--- fill in the vector of vector of covariances ---
        vector<vector<TH2*>> covariances{
	  {hCovDataStatDE, hCovMCStatDE, hCovPUSystDE, hCovJERSystDE, hCovXSecSystDE, hCovLumiSystDE, hCovSFSystDE, hCovJESSystDE, hCovLESSystDE, hCovLERSystDE, hCovSherpaUnfSystDE}, 
	    {hCovDataStatDMu, hCovMCStatDMu, hCovPUSystDMu, hCovJERSystDMu, hCovXSecSystDMu, hCovLumiSystDMu, hCovSFSystDMu, hCovJESSystDMu, hCovLESSystDMu, hCovLERSystDMu, hCovSherpaUnfSystDMu}
	    /*
	      {hCovDataStatDE, hCovMCStatDE, hCovPUSystDE, hCovJERSystDE, hCovXSecSystDE, hCovLumiSystDE, hCovSFSystDE, hCovJESSystDE, hCovLESSystDE, hCovLERSystDE}, 
	      {hCovDataStatDMu, hCovMCStatDMu, hCovPUSystDMu, hCovJERSystDMu, hCovXSecSystDMu, hCovLumiSystDMu, hCovSFSystDMu, hCovJESSystDMu, hCovLESSystDMu, hCovLERSystDMu}
	    */
        };
/*
        vector<vector<TH2*>> covariancesNorm;
        if(doNormband){
            covariancesNorm = {
                {hCovDataStatDENorm, hCovMCStatDENorm, hCovPUSystDENorm, hCovJERSystDENorm, hCovXSecSystDENorm, hCovLumiSystDENorm, hCovSFSystDENorm, hCovJESSystDENorm, hCovLESSystDENorm, hCovLERSystDENorm, hCovSherpaUnfSystDENorm}, 
                    {hCovDataStatDMuNorm, hCovMCStatDMuNorm, hCovPUSystDMuNorm, hCovJERSystDMuNorm, hCovXSecSystDMuNorm, hCovLumiSystDMuNorm, hCovSFSystDMuNorm, hCovJESSystDMuNorm, hCovLESSystDMuNorm, hCovLERSystDMuNorm, hCovSherpaUnfSystDMuNorm}
            };
        }
        //---------------------------------------------------------------------
*/

        //--- create objects to be filled with output of combination ---
        vector<TH2*> covuxaxb(covariances[0].size(), NULL); // each covariance matrix
        //vector<TH2*> covuxaxbNorm(covariances[0].size(), NULL); // each normalized covariance matrix 
        TH2* covxaxb = NULL; // total covariance matrix
        TH2* covxaxbSyst = NULL; // total syst covariance matrix
        //TH2* covxaxbNorm = NULL; // total normalized covariance matrix
        //TH2* covxaxbSystNorm = NULL; // total normalized syst covariance matrix
        TH1* hTotComUnc = NULL; // total uncertainty if combined cross section
        TH1* hCombination = NULL; // combined cross section
        //TH1* hCombinationNorm = NULL; // combined normalized cross section for band
        TH1* hMadGenCombined = NULL;
        TH1* hGen1Combined = NULL;
        //TH1* hGen2Combined = NULL;
        //---------------------------------------------------------------------

        //--- create the BLUEMeth object to compute the covariance ---
        BLUEMeth* blueXSec = new BLUEMeth(measurements, covariances, variable);
        //BLUEMeth* blueXSecNorm;
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

        covxaxbSyst = (TH2*) covxaxb->Clone();
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
/*
        if(doNormband){
            blueXSecNorm = new BLUEMeth(measurementsNorm, covariancesNorm, variable);
            hCombinationNorm = blueXSecNorm->GetCombination(diagXChanCov, fullXChanCov, fullSChanCov, modifiedSWA, covuxaxbNorm, covxaxbNorm);

            covxaxbSystNorm = (TH2*) covxaxbNorm->Clone();
            covxaxbSystNorm->Reset();
            covxaxbSystNorm->Add(covuxaxbNorm[1]);
            covxaxbSystNorm->Add(covuxaxbNorm[2]);
            covxaxbSystNorm->Add(covuxaxbNorm[3]);
            covxaxbSystNorm->Add(covuxaxbNorm[4]);
            covxaxbSystNorm->Add(covuxaxbNorm[5]);
            covxaxbSystNorm->Add(covuxaxbNorm[6]);
            covxaxbSystNorm->Add(covuxaxbNorm[7]);
            covxaxbSystNorm->Add(covuxaxbNorm[8]);
            covxaxbSystNorm->Add(covuxaxbNorm[9]);
            covxaxbSystNorm->Add(covuxaxbNorm[10]);
            covxaxbSystNorm->SetName("CombCovTotSystNorm");
        }
*/
        hMadGenCombined = (TH1*) hMadGenDE->Clone();
        hMadGenCombined->Add(hMadGenDMu);
        hMadGenCombined->Scale(0.5);
        hMadGenCombined->SetName("hMadCombGenDYJetsCrossSection");

        hGen1Combined = (TH1*) hGen1DE->Clone();
        hGen1Combined->Add(hGen1DMu);
        hGen1Combined->Scale(0.5);
        hGen1Combined->SetName("hCombGen1DYJetsCrossSection");
/*
        hGen2Combined = (TH1*) hGen2DE->Clone();
        hGen2Combined->Add(hGen2DMu);
        hGen2Combined->Scale(0.5);
        hGen2Combined->SetName("hCombGen2DYJetsCrossSection");
*/
        //---------------------------------------------------------------------
        TCanvas *crossSectionPlot;
/*        if(doNormband){
            crossSectionPlot = makeCrossSectionPlot("", variable, hCombination, covxaxbSyst, hCombinationNorm, covxaxbSystNorm, hMadGenCombined, hGen1Combined, hGen2Combined); 
            crossSectionPlot->Draw();
	    saveCanvas(crossSectionPlot, combDir, outputFileName);
//            crossSectionPlot->SaveAs(outputFilePath + ".png");
//            crossSectionPlot->SaveAs(outputFilePath + ".pdf");
//            crossSectionPlot->SaveAs(outputFilePath + ".ps");
//            crossSectionPlot->SaveAs(outputFilePath + ".C");
        }
	
        else{*/
	//crossSectionPlot = makeCrossSectionPlot("", variable, doNormalized, hCombination, covxaxbSyst, hMadGenCombined, hGen1Combined, hGen2Combined); 
	//crossSectionPlot = makeCrossSectionPlot("", variable, doNormalized, hCombination, covxaxbSyst, hMadGenCombined, hGen1Combined);

	TString unfCfgFile = cfg.getS("unfConf");
	static SectionedConfig unfCfg;
	unfCfg.read(unfCfgFile, true);
	
	TString sectionDE = TString::Format("DE_%s", variable.Data());
	TString sectionDMu = TString::Format("DMu_%s", variable.Data());
	int nFirstBinsToSkip = unfCfg.get(sectionDE.Data(), "nFirstBinsToSkip", 0);
	nFirstBinsToSkip = std::max(nFirstBinsToSkip, unfCfg.get(sectionDMu.Data(), "nFirstBinsToSkip", 0));
	int nLastBinsToSkip = unfCfg.get(sectionDE.Data(), "nLastBinsToSkip", 0);
	nLastBinsToSkip = std::max(nLastBinsToSkip, unfCfg.get(sectionDMu.Data(), "nLastBinsToSkip", 0));
	
	crossSectionPlot = makeCrossSectionPlot("", variable, doNormalized, hCombination, covxaxbSyst,
						predictions, nFirstBinsToSkip, nLastBinsToSkip);
	crossSectionPlot->Draw();
	saveCanvas(crossSectionPlot, combDir, outputFileName);
//	crossSectionPlot->SaveAs(outputFilePath + ".png");
//	crossSectionPlot->SaveAs(outputFilePath + ".pdf");
//	crossSectionPlot->SaveAs(outputFilePath + ".ps");
//	crossSectionPlot->SaveAs(outputFilePath + ".C");
        //}
	
        //--- print out the combined cross section measurement and fill the total uncertainty ---
        double tempunc = 0;
        hTotComUnc = (TH1*)hCombination->Clone();
        for (int i = 1; i <= hCombination->GetNbinsX(); ++i) {
            cout << hCombination->GetBinContent(i) << endl;
            tempunc = sqrt(covuxaxb[0]->GetBinContent(i,i) + covxaxbSyst->GetBinContent(i,i));
            hTotComUnc->SetBinContent(i,tempunc);
        }
        //--- print out break down of errors ---
        for (int i = 2; i <= 11; ++i) {
            cout << hCombination->GetBinContent(i);
            for (unsigned j = 0; j < covuxaxb.size(); ++j) {
	      if(!covuxaxb[j]) continue;	  
	      cout << " +/- " << sqrt(covuxaxb[j]->GetBinContent(i,i))*100./hCombination->GetBinContent(i) << "%";
            }
            cout << endl;
        }
	
        createTable(outputFilePath + "_withLERS", variable, doNormalized, hCombination, covuxaxb, covxaxbSyst, true);
        createTable(outputFilePath, variable, doNormalized, hCombination, covuxaxb, covxaxbSyst, false);
        if (variable.Index("ZNGoodJets_Zexc") >= 0) {
	  /*           if(doNormband){
		       createInclusivePlots(combDir, outputFileName, hCombination, covuxaxb, covxaxbSyst, hCombinationNorm, covuxaxbNorm, covxaxbSystNorm, hMadGenCombined, hGen1Combined, hGen2Combined);
		       }
		       else{*/
	  //createInclusivePlots(doNormalized, outputFilePath, hCombination, covuxaxb, covxaxbSyst, hMadGenCombined, hGen1Combined);
	    createInclusivePlots(doNormalized, combDir, outputFileName, hCombination, covuxaxb, covxaxbSyst, predictions, nFirstBinsToSkip, nLastBinsToSkip);

	  //createInclusivePlots(doNormalized, outputFilePath, hCombination, covuxaxb, covxaxbSyst, hMadGenCombined, hGen1Combined, hGen2Combined);
	  //}
        }

        //--- save results and inputs to root file ---
        outputRootFile->cd();
        crossSectionPlot->Write();
        hCombination->Write("CombDataCentral");
        hTotComUnc->Write("CombTotUnc");
        hMadGenCombined->Write();
        hGen1Combined->Write();
        //hGen2Combined->Write();
        covxaxb->Write("CombCovTot");
        covxaxbSyst->Write("CombCovTotSyst");
        for (unsigned int i = 0; i < covuxaxb.size(); ++i) {
	  if(covuxaxb[i]) covuxaxb[i]->Write();
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

        //if (end == start + 1) system("display " + outputFilePath + ".png &");
        //if (end == start + 1 && variable == "ZNGoodJets_Zexc") system("display " + outputFilePath.ReplaceAll("ZNGoodJets_Zexc", "ZNGoodJets_Zinc") + ".png &");
    }
}

 //void createInclusivePlots(bool doNormalized, TString outputFilePath, TH1 *hUnfData, vector<TH2*> hCov, TH2 *hCovSyst, TH1 *hMadGenCrossSection, TH1 *hSheGenCrossSection)
void createInclusivePlots(bool doNormalized, TString outputFileDir, TString outputFileName, 
			  TH1 *hUnfData, vector<TH2*> hCov, TH2 *hCovSyst,
			  const std::vector<std::string>& predictions,
			  int nFirstBinsToSkip, int nLastBinsToSkip)
//void createInclusivePlots(bool doNormalized, TString outputFilePath, TH1 *hUnfData, vector<TH2*> hCov, TH2 *hCovSyst, TH1 *hMadGenCrossSection, TH1 *hSheGenCrossSection, TH1 *hPowGenCrossSection)
{
    TH1 *hInc = (TH1*) hUnfData->Clone("ZNGoodJets_Zinc");

    std::vector<TH1*> hGens = getGenHistos(predictions, "", "ZNGoodJets_Zinc", true, true);
    hGens.resize(3, 0);    
    
    TH1D *hIncMad = hGens[0] ? (TH1D*) hGens[0]->Clone("ZNGoodJets_Zinc_Mad") : 0;
    TH1D *hIncShe = hGens[1] ? (TH1D*) hGens[1]->Clone("ZNGoodJets_Zinc_She") : 0;
    TH1D *hIncPow = hGens[2] ? (TH1D*) hGens[2]->Clone("ZNGoodJets_Zinc_Pow") : 0;

    const int kTot = 11;
    if(hCov.size() != 11){
      std::cerr << "Bug found in " << __FILE__ << ":" << __LINE__
		<< ": hCov vector has an unexpected size! Aborts.\n";
      abort();
    }
    hCov.push_back(hCovSyst);

    std::vector<TH2*> hCovInc(kTot + 1, 0);
    if(hCov[0])  hCovInc[0]  = (TH2*) hCov[0]->Clone("CovDataStat");
    if(hCov[1])  hCovInc[1]  = (TH2*) hCov[1]->Clone("CovMCStat");
    if(hCov[2])  hCovInc[2]  = (TH2*) hCov[2]->Clone("CovPU");
    if(hCov[3])  hCovInc[3]  = (TH2*) hCov[3]->Clone("CovJER");
    if(hCov[4])  hCovInc[4]  = (TH2*) hCov[4]->Clone("CovXSec");
    if(hCov[5])  hCovInc[5]  = (TH2*) hCov[5]->Clone("CovLumi");
    if(hCov[6])  hCovInc[6]  = (TH2*) hCov[6]->Clone("CovSF");
    if(hCov[7])  hCovInc[7]  = (TH2*) hCov[7]->Clone("CovJES");
    if(hCov[8])  hCovInc[8]  = (TH2*) hCov[8]->Clone("CovLES");
    if(hCov[9])  hCovInc[9]  = (TH2*) hCov[9]->Clone("CovLER");
    if(hCov[10]) hCovInc[10] = (TH2*) hCov[10]->Clone("CovSherpaUnf");
    if(hCov[11]) hCovInc[11] = (TH2*) hCov[11]->Clone("CovSystTot");

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
        for (int j = i; j <= nBins; j++) {
	    binSum += hInc->GetBinContent(j);
            if(hIncMad) binSumMad += hIncMad->GetBinContent(j);
	    if(hIncShe) binSumShe += hIncShe->GetBinContent(j);
            if(hIncPow) binSumPow += hIncPow->GetBinContent(j);
            binStatError2 += pow(hInc->GetBinError(j), 2);
	    if(hIncMad) binStatMadError2 += pow(hIncMad->GetBinError(j), 2);
	    if(hIncShe) binStatSheError2 += pow(hIncShe->GetBinError(j), 2);
	    if(hIncPow) binStatPowError2 += pow(hIncPow->GetBinError(j), 2);
        }
        hInc->SetBinContent(i, binSum);
        if(hIncMad) hIncMad->SetBinContent(i, binSumMad);
        if(hIncShe) hIncShe->SetBinContent(i, binSumShe);
        if(hIncPow) hIncPow->SetBinContent(i, binSumPow);
        hInc->SetBinError(i, sqrt(binStatError2));
        if(hIncMad) hIncMad->SetBinError(i, sqrt(binStatMadError2));
        if(hIncShe) hIncShe->SetBinError(i, sqrt(binStatSheError2));
        if(hIncPow) hIncPow->SetBinError(i, sqrt(binStatPowError2));
    }

    //Covariance matrix.
    //We can write:
    //    Y_inc = A * Y_exc, with Y_inc and Y_exc the vector of the respective
    //                       distribution bin contents
    //                       and A_ij = 1 if j >=i, 0 otherwise
    //   => Cov_inc = A * Cov_exc * A^{T}
    for(int m = 0; m <= kTot; ++m){
      if(hCov[m]==0) continue;
      for(int i = 1; i <= nBins; ++i){
	for(int j = 1; j <= nBins; ++j){
	  double c = 0;
	  for(int k = i; k <= nBins; ++k){
	    for(int l = j; l <= nBins; ++l){
	      c += hCov[m]->GetBinContent(k, l);
	    }
	  }
	  hCovInc[m]->SetBinContent(i, j, c);
	}
      }
    }
    
    
    TCanvas *crossSectionPlot = makeCrossSectionPlot(TString(""), TString("ZNGoodJets_Zinc"),
						     doNormalized, hInc, hCovInc[kTot],
						     predictions, nFirstBinsToSkip, nLastBinsToSkip);
    outputFileName.ReplaceAll("ZNGoodJets_Zexc", "ZNGoodJets_Zinc");
    crossSectionPlot->Draw();
    saveCanvas(crossSectionPlot, outputFileDir, outputFileName);
    createTable(outputFileDir + "/" + outputFileName + "_withLERS",
		TString("ZNGoodJets_Zinc"), doNormalized, hInc, hCovInc, hCovInc[kTot], true);
    createTable(outputFileDir + "/" + outputFileName, TString("ZNGoodJets_Zinc"),
		doNormalized, hInc, hCovInc, hCovInc[kTot], false);
}

void createTable(TString outputFilePath, TString variable, bool doNormalized, TH1 *hCombination, vector<TH2*> &covuxaxb, TH2* covxaxbSyst, bool withLERS)
{
    //--- print out break down of errors ---

    TString title = hCombination->GetTitle();
    int nBins = hCombination->GetNbinsX();
    TString var = "";
    TString dSigma = "";
    TString xtitle = hCombination->GetXaxis()->GetTitle();
    createTitleVariableAnddSigma(variable, doNormalized, xtitle, title, var, dSigma);
    title.ReplaceAll("Exclusive", "exclusive");
    title.ReplaceAll("Inclusive", "inclusive");
    title.ReplaceAll("Hadronic", "hadronic");

    cout << "Title: " << title << endl;
    cout << "Var: " << var << endl;
    cout << "dSig: " << dSigma << endl;
    cout << "nBins: " << nBins << endl;

    TString table = "\\begin{table}[htb!]\n\\begin{center}\n";
    table += "\\caption{Differential cross section in " + title;
    table += " and break down of the systematic uncertainties for the ";
    table += "combination of both decay channels.}\n";
    table += "\\scriptsize{\n";
    if(withLERS){
      table += "\\begin{tabular}{c|cc|ccccccccccc}\n";
    } else{
      table += "\\begin{tabular}{c|cc|ccccccccc}\n";
    }
    table += var + " & " + dSigma + " & \\tiny{Tot[\\%]} & ";
    table += "\\tiny{stat[\\%]} & \\tiny{Unf stat[\\%]} & \\tiny{JES[\\%]} & \\tiny{JER[\\%]} & ";
    table += "\\tiny{PU[\\%]} & \\tiny{Bkg[\\%]} & \\tiny{Lumi[\\%]} & ";
    if(withLERS){
      table += "\\tiny{LES[\\%]} & \\tiny{LER[\\%]} & ";
    }
    table += "\\tiny{Unf sys[\\%]} & \\tiny{Eff[\\%]} \\\\\\hline\n";

    int start = 1;
    /*if (title.Index("multiplicity", 0, TString::ECaseCompare::kIgnoreCase) >= 0) {
        start = 2; 
        //nBins--;
    }*/
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
        // XSec (Bgnd) uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[4]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // Lumi uncertainty
        numbers.Form("%#.2g", sqrt(covuxaxb[5]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
	if(withLERS){
	  // LES uncertainty
	  numbers.Form("%#.2g", sqrt(covuxaxb[8]->GetBinContent(i,i))*100./xs);
	  table += numbers + " & ";
	  // LER uncertainty
	  numbers.Form("%#.2g", sqrt(covuxaxb[9]->GetBinContent(i,i))*100./xs);
	  table += numbers + " & ";
	}
	// Unf uncertainty
	if(covuxaxb[10]) numbers.Form("%#.2g", sqrt(covuxaxb[10]->GetBinContent(i,i))*100./xs);
	else numbers = "-";
	table += numbers + " & ";
	// SF uncertinaty
	numbers.Form("%#.2g", sqrt(covuxaxb[6]->GetBinContent(i,i))*100./xs);
	table += numbers + " \\\\\n";
    }

    table += "\\end{tabular}}\n";
    table += "\\label{tab:comb" + variable + "}\n";
    table += "\\end{center}\\end{table}\n";
    ofstream out(outputFilePath + ".tex");
    out << table;
    out.close();
    cout << table << endl;
}
