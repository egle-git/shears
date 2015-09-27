#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <math.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TArrow.h>
#include <TLine.h>
#include <RooUnfoldBayes.h>
#include <RooUnfoldBinByBin.h>
#include <RooUnfoldSvd.h>
#include <TSVDUnfold.h>
#include <TParameter.h>
#include "fileNamesZJets.h"
#include "getFilesAndHistogramsZJets.h"
#include "variablesOfInterestZJets.h"
#include "UnfoldingZJets.h"
#include "PlotSettings.h"

using namespace std;
void createInclusivePlots(bool doNormalized, TString outputFileName, TString lepSel, TH1D *hUnfData, TH2D *hCov[], TH1D *hMadGenCrossSection, TH1D *hSheGenCrossSection, TH1D *hPowGenCrossSection);

void UnfoldingZJets(TString lepSel, TString algo, TString histoDir, TString unfoldDir, 
        int jetPtMin, int jetEtaMax, TString gen1, TString gen2, TString variable, bool doNormalized)
{
    gStyle->SetOptStat(0);
    //--- create output directory if does not exist ---
    system("mkdir -p " + unfoldDir);

    int start = 0;
    int end = NVAROFINTERESTZJETS;

    if (variable != "") {
        start = findVariable(variable);
        if (start >= 0) {
            end = start + 1;
            cout << "Processing only variable: " << variable << endl;
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

    double integratedLumi = (lepSel == "DMu") ? 19584 : 19618;
    // Here we declare the different arrays of TFiles. 
    // fData is for the three data files: 
    // 0 - central, 1 - JES up, 2 - JES down
    TFile *fData[3] = {NULL}; 
    // fDYJets is for the five DYJets files:
    // 0 - central, 1 - PU up, 2 - PU down, 3 - JER up, 4 - JER down, 5 - LES up, 6 - LES down, 7 - LER up, 8 - LER down 
    TFile *fDYJets[9] = {NULL};
    // fBg is for the NBGDYJETS x 5 systematics files:
    // 0 - central, 1 - PU up, 2 - PU down, 3 - XSEc up, 4 - XSEC down, 5 - LES up, 6 - LES down 
    TFile *fBg[NBGDYJETS][7] = {{NULL}};

    //--- Open all files ---------------------------------------------------------------------- 
    getAllFiles(histoDir, lepSel, "8TeV", jetPtMin, jetEtaMax, fData, fDYJets, fBg, NBGDYJETS);
    //----------------------------------------------------------------------------------------- 

    //--- Open additional generator files -----------------------------------------------------
    TFile *fSheUnf = new TFile(histoDir + lepSel + "_8TeV_" + DYSHERPA14FILENAME + "_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");

    std::map<TString, vector<TString> > generatorNames;

    vector<TString> sherpa14;
    sherpa14.push_back(DYSHERPA14FILENAME);
    sherpa14.push_back(DYSHERPA14LEGEND);
    vector<TString> sherpa2;
    sherpa2.push_back(DYSHERPA2FILENAME);
    sherpa2.push_back(DYSHERPA2LEGEND);
    vector<TString> amcatnlo;
    amcatnlo.push_back(DYAMCATNLOFILENAME);
    amcatnlo.push_back(DYAMCATNLOLEGEND);
    vector<TString> mgpythia8;
    mgpythia8.push_back(DYMGPYTHIA8FILENAME);
    mgpythia8.push_back(DYMGPYTHIA8LEGEND);

    generatorNames["sherpa14"] = sherpa14;
    generatorNames["sherpa2"] = sherpa2;
    generatorNames["amcatnlo"] = amcatnlo; 
    generatorNames["mgpythia8"] = mgpythia8;

    TFile *fGen1 = NULL; 
    TFile *fGen2 = NULL; 

    TString gen1File = histoDir + lepSel + "_8TeV_" + generatorNames[gen1][0] + "_dR_TrigCorr_1_Syst_0_JetPtMin_";
    gen1File += jetPtMin;
    gen1File += "_JetEtaMax_";
    gen1File += jetEtaMax;
    gen1File += ".root";
    fGen1 = new TFile(gen1File);

    TString gen2File = histoDir + lepSel + "_8TeV_" + generatorNames[gen2][0] + "_dR_TrigCorr_1_Syst_0_JetPtMin_";
    gen2File += jetPtMin;
    gen2File += "_JetEtaMax_";
    gen2File += jetEtaMax;
    gen2File += ".root";
    fGen2 = new TFile(gen2File);

    //----------------------------------------------------------------------------------------- 

    //----------------------------------------------------------------------------------------- 
    //--- Now run on the different variables ---
    for (int i = start; i < end; ++i) {
        variable = VAROFINTERESTZJETS[i].name;

        TString outputFileName = unfoldDir + lepSel; 
        outputFileName += "_unfolded_" + variable + "_" + algo;
        outputFileName += "_JetPtMin_";
        outputFileName += jetPtMin;
        outputFileName += "_JetEtaMax_";
        outputFileName += jetEtaMax;
        outputFileName += "_MGPYTHIA6_" + gen1 + "_" + gen2;
        outputFileName += doNormalized ? "_normalized" : "";

        TFile *outputRootFile = new TFile(outputFileName + ".root", "RECREATE");


        //--- rec Data histograms ---
        TH1D *hRecData[3] = {NULL};
        //--- rec DYJets histograms ---
        TH1D *hRecDYJets[13] = {NULL};
        //--- fake DYJets histograms ---
        TH1D *hFakDYJets[18] = {NULL};
        //--- gen DYJets histograms ---
        TH1D *hGenDYJets[11] = {NULL};
        //--- res DYJets histograms ---
        TH2D *hResDYJets[13] = {NULL};
        //--- rec Bg histograms ---
        TH1D *hRecBg[NBGDYJETS][11] = {{NULL}};
        //--- rec Sum Bg histograms ---
        TH1D *hRecSumBg[11] = {NULL};
        //--- response DYJets objects ---
        RooUnfoldResponse *respDYJets[18] = {NULL};

        //--- Get all histograms ---
        getAllHistos(variable, hRecData, fData, 
                hRecDYJets, hGenDYJets, hResDYJets, fDYJets,
                hRecBg, hRecSumBg, fBg, NBGDYJETS, respDYJets, hFakDYJets);

        //--- Get Sherpa Unfolding response ---

        respDYJets[17] = getResp(fSheUnf, variable);
        TH1D *hGen1 = getHisto(fGen1, "gen" + variable);
        TH1D *hGen2 = getHisto(fGen2, "gen" + variable);
        //----------------------------------------------------------------------------------------- 

        TH1D *hMadGenCrossSection = makeCrossSectionHist(hGenDYJets[0], integratedLumi);
        hMadGenCrossSection->SetZTitle("MG5 + PY6 (#leq 4j LO + PS)");
        TH1D *hGen1CrossSection = makeCrossSectionHist(hGen1, integratedLumi);
        hGen1CrossSection->SetZTitle(generatorNames[gen1][1]);
        TH1D *hGen2CrossSection = makeCrossSectionHist(hGen2, integratedLumi);
        hGen2CrossSection->SetZTitle(generatorNames[gen2][1]);
        if (gen1 == "sherpa2" && lepSel == "DMu") hGen1CrossSection->Scale(1./3.9731e+09); 
        if (gen1 == "sherpa2" && lepSel == "DE") hGen1CrossSection->Scale(1./3.9731e+09); 
        if (gen2 == "sherpa2" && lepSel == "DMu") hGen2CrossSection->Scale(1./3.9731e+09); 
        if (gen2 == "sherpa2" && lepSel == "DE") hGen2CrossSection->Scale(1./3.9731e+09); 
        // this is for MLM
        //if (lepSel == "DE")  hPowGenCrossSection->Scale(9.68984524447932094e+06/1.26965652647094554e+18); 
        //if (lepSel == "DMu") hSheGenCrossSection->Scale(9.67682344591264986e+06/1.56860892310320000e+14); 
        if (gen1 == "amcatnlo" && lepSel == "DMu") hGen1CrossSection->Scale(9.67682344591264986e+06/1.93588665458e+18); 
        if (gen1 == "amcatnlo" && lepSel == "DE")  hGen1CrossSection->Scale(9.68984524447932094e+06/2.01906738616e+18); 
        if (gen2 == "amcatnlo" && lepSel == "DMu") hGen2CrossSection->Scale(9.67682344591264986e+06/1.93588665458e+18); 
        if (gen2 == "amcatnlo" && lepSel == "DE")  hGen2CrossSection->Scale(9.68984524447932094e+06/2.01906738616e+18); 

        // Here is an array of TH1D to store the various unfolded data:
        // 0 - Central, 
        // 1 - JES up, 2 - JES down, 
        // 3 - PU up, 4 - PU down, 
        // 5 - JER up, 6 - JER down, 
        // 7 - XSEC up, 8 - XSEC down
        // 9 - LES up, 10 - LES down
        // 11 - LER up, 12 -LER down
        // 13 - Lumi up, 14 - Lumi down
        // 15 - SF up, 16 - SF down
        // 17 - SherpaUnf
        TString name[] = {"Central", "JESUp", "JESDown", "PUUp", "PUDown", "JERUp", "JERDown", 
            "XSECUp", "XSECDown", "LESUp", "LESDown", "LERUp", "LERDown", "LumiUp", "LumiDown", "SFUp", "SFDown", "SherpaUnf"};
        TH1D *hUnfData[18] = {NULL};
        TH2D *hUnfDataStatCov[18] = {NULL};
        TH2D *hUnfMCStatCov[18] = {NULL};

        int nIter[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
        int svdKterm(0);
        if (lepSel == "DMu")  
            svdKterm = VAROFINTERESTZJETS[i].MuSVDkterm;
        else if (lepSel == "DE")   
            svdKterm = VAROFINTERESTZJETS[i].ESVDkterm;
        else {
            cerr << "Error: algo " << algo << " or lepSel " << lepSel << "invalid\n"; 
            cerr << "Aborting...\n";
            return;
        }
        //--- Unfold the Data histograms for each systematic ---
        for (unsigned short iSyst = 0; iSyst < 18; ++iSyst) {

            //--- only JES up and down (iSyst = 1 and 2) is applied on data ---
            unsigned short iData = (iSyst == 1 || iSyst == 2) ? iSyst : 0;
            unsigned short iBg = 0;
            if (iSyst == 0 || iSyst == 1 || iSyst == 2 || iSyst == 5 || iSyst == 6 || iSyst == 11 || iSyst == 12 || iSyst == 17) iBg = 0; // Central, JES, JER, LER, Sherpa
            else if (iSyst == 3 || iSyst == 4) iBg = iSyst - 2; // PU
            else if (iSyst == 7 || iSyst == 8 || iSyst == 9 || iSyst == 10) iBg = iSyst - 4; // XSec, LES
            else if (iSyst == 13 || iSyst == 14 || iSyst == 15 || iSyst == 16) iBg = iSyst - 6;  // Lumi, SF

            TH1D *hRecDataMinusFakes = (TH1D*) hRecData[iData]->Clone();
            hRecDataMinusFakes->Add(hRecSumBg[iBg], -1);
            hRecDataMinusFakes->Add(hFakDYJets[iSyst], -1);

            if (iSyst == 17) cout << "SHERPAUNFOLDING" << endl;
            nIter[iSyst] = UnfoldData(lepSel, algo, svdKterm, respDYJets[iSyst], hRecDataMinusFakes, hUnfData[iSyst], 
                    hUnfDataStatCov[iSyst], hUnfMCStatCov[iSyst], name[iSyst], integratedLumi);

            //--- save the unfolded histograms ---
            outputRootFile->cd(); 
            hUnfData[iSyst]->Write();
        }
        //----------------------------------------------------------------------------------------- 

        if (doNormalized) {
            for(int i = 0; i < 18; i++)
            {
                double totUnfData = hUnfData[i]->Integral("width"); // normalize to central or itself? 
                hUnfData[i]->Scale(1.0/totUnfData);
                if (i == 0) {
                    hUnfDataStatCov[0]->Scale(1.0/pow(totUnfData, 2));
                    hUnfMCStatCov[0]->Scale(1.0/pow(totUnfData, 2));
                }
            }
        }

        for (unsigned short iSyst = 0; iSyst < 18; ++iSyst) {
            outputRootFile->cd(); 
            hUnfData[iSyst]->Write();
        }
        //--- Now create the covariance matrices ---
        TH2D *hCov[12] = {NULL};
        hCov[0] = (TH2D*) hUnfDataStatCov[0]->Clone("CovDataStat");
        hCov[1] = (TH2D*) hUnfMCStatCov[0]->Clone("CovMCStat");
        hCov[2] = makeCovFromUpAndDown(hUnfData[0], hUnfData[1], hUnfData[2], "CovJES");
        hCov[3] = makeCovFromUpAndDown(hUnfData[0], hUnfData[3], hUnfData[4], "CovPU");
        hCov[4] = makeCovFromUpAndDown(hUnfData[0], hUnfData[5], hUnfData[6], "CovJER");
        hCov[5] = makeCovFromUpAndDown(hUnfData[0], hUnfData[7], hUnfData[8], "CovXSec");
        hCov[6] = makeCovFromUpAndDown(hUnfData[0], hUnfData[9], hUnfData[10], "CovLES");
        hCov[7] = makeCovFromUpAndDown(hUnfData[0], hUnfData[11], hUnfData[12], "CovLER");
        hCov[8] = makeCovFromUpAndDown(hUnfData[0], hUnfData[13], hUnfData[14], "CovLumi");
        hCov[9] = makeCovFromUpAndDown(hUnfData[0], hUnfData[15], hUnfData[16], "CovSF");
        hCov[10] = makeCovFromUpAndDown(hUnfData[0], hUnfData[17], hUnfData[0], "CovSherpaUnf");
        hCov[11] = (TH2D*) hUnfMCStatCov[0]->Clone("CovTotSyst");

        for (int i = 2; i < 11; ++i) hCov[11]->Add(hCov[i]);

        if (doNormalized) {
            double Madtot = hMadGenCrossSection->Integral("width");
            double gen1tot = hGen1CrossSection->Integral("width");
            double gen2tot = hGen2CrossSection->Integral("width");
            hMadGenCrossSection->Scale(1.0/Madtot);
            hGen1CrossSection->Scale(1.0/gen1tot);	
            hGen2CrossSection->Scale(1.0/gen2tot);	
        }

        TCanvas *crossSectionPlot = makeCrossSectionPlot(lepSel, variable, doNormalized, hUnfData[0], hCov[11], hMadGenCrossSection, hGen1CrossSection, hGen2CrossSection); 
        crossSectionPlot->Draw();
        crossSectionPlot->SaveAs(outputFileName + ".png");
        crossSectionPlot->SaveAs(outputFileName + ".pdf");
        crossSectionPlot->SaveAs(outputFileName + ".eps");
        crossSectionPlot->SaveAs(outputFileName + ".ps");
        crossSectionPlot->SaveAs(outputFileName + ".C");

        createSystPlots(outputFileName, variable, lepSel, hUnfData);

        //--- print out break down of errors ---
        for (int i = 2; i <= 11; ++i) {
            cout << hUnfData[0]->GetBinContent(i);
            for (int j = 0; j <= 11; ++j) {
                cout << " +/- " << sqrt(hCov[j]->GetBinContent(i,i))*100./hUnfData[0]->GetBinContent(i) << "%";
            }
            cout << endl;
        }

        createTable(outputFileName, lepSel, variable, doNormalized, hUnfData[0], hCov);

        if (variable.Index("ZNGoodJets_Zexc") >= 0) {
            createInclusivePlots(doNormalized, outputFileName, lepSel, hUnfData[0], hCov, hMadGenCrossSection, hGen1CrossSection, hGen2CrossSection);
        }
        //--------------------------------------

        //--- Save other things --- 
        outputRootFile->cd();
        hRecData[0]->Write("hRecDataCentral");
        hRecSumBg[0]->Write("hRecSumBgCentral");
        hRecDYJets[0]->Write("hRecDYJetsCentral");
        hGenDYJets[0]->Write("hGenDYJetsCentral");
        hMadGenCrossSection->Write("hMadGenDYJetsCrossSection");
        hGen1CrossSection->Write("hGen1DYJetsCrossSection");
        hGen2CrossSection->Write("hGen2DYJetsCrossSection");
        respDYJets[0]->Write("respDYJetsCentral");
        for (int i = 0; i < 11; ++i) {
            hCov[i]->Write();
        }
        TParameter<double> pIntegratedLumi("integratedLumi", integratedLumi);
        pIntegratedLumi.Write();
        TParameter<int> pNIter("nIter", nIter[0]);
        pNIter.Write();
        crossSectionPlot->Write();
        //----------------------------------------------------------------------------------------- 

        outputRootFile->Close();

        //if (end == start + 1) system("display " + outputFileName + ".png &");
        //if (end == start + 1 && variable == "ZNGoodJets_Zexc") system("display " + outputFileName.ReplaceAll("ZNGoodJets_Zexc", "ZNGoodJets_Zinc") + ".png &");

    }

    //--- Close all files ----------------------------------------------------------------------
    closeAllFiles(fData, fDYJets, fBg, NBGDYJETS);
    fSheUnf->Close();
    fGen1->Close();
    fGen2->Close();
    //------------------------------------------------------------------------------------------ 

}

void createSystPlots(TString outputFileName, TString variable, TString lepSel, TH1D *hUnfData[])
{

    // 0 - Central, 
    // 1 - JES up, 2 - JES down, 
    // 3 - PU up, 4 - PU down, 
    // 5 - JER up, 6 - JER down, 
    // 7 - XSEC up, 8 - XSEC down
    // 9 - LES up, 10 - LES down
    // 11 - LER up, 12 - LER down
    // 13 - Lumi up, 14 - Lumi down
    // 15 - SF up, 16 - SF down
    // 17 - SherpaUnf
    TString syst[] = {"JES", "PU", "JER", "XSec", "LES", "LER", "Lumi", "S.F."};
    for (int i = 1; i < 16; i += 2) {

        TH1D *hCent = (TH1D*) hUnfData[0]->Clone();
        hCent->SetMarkerColor(kBlack);
        hCent->SetMarkerStyle(20);
        if (variable == "ZNGoodJets_Zexc") hCent->GetXaxis()->SetRangeUser(1, 8);
        if (variable.Index("JetPt_Zinc") >= 0) hCent->GetXaxis()->SetRangeUser(30, hCent->GetXaxis()->GetXmax());
        hCent->GetXaxis()->SetLabelSize(0);
        hCent->GetYaxis()->SetTitle("d#sigma");
        hCent->GetYaxis()->SetTitleSize(0.05);
        hCent->GetYaxis()->SetTitleOffset(0.9);

        TH1D *hUp = (TH1D*) hUnfData[i]->Clone();
        hUp->SetLineColor(kGreen+2);
        hUp->SetLineWidth(2);
        TH1D *hDown = (TH1D*) hUnfData[i+1]->Clone();
        hDown->SetLineColor(kBlue);
        hDown->SetLineWidth(2);

        TCanvas *c = new TCanvas(variable + " - " + syst[i/2], variable + " - " + syst[i/2], 700, 900);
        c->cd();

        TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1);
        pad1->SetTopMargin(0.11);
        pad1->SetBottomMargin(0.02);
        pad1->SetRightMargin(0.03);
        pad1->SetLeftMargin(0.15);
        pad1->SetTicks();
        pad1->SetLogy();
        pad1->Draw();
        pad1->cd();

        TLegend *leg = new TLegend(0.8, 0.7, 0.95, 0.86);
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        hCent->DrawCopy("e");
        hUp->DrawCopy("samehist");
        hDown->DrawCopy("samehist");
        leg->AddEntry(hUp, syst[i/2] + " Up", "l");
        leg->AddEntry(hCent, "Central", "lp");
        leg->AddEntry(hDown, syst[i/2] + " Down", "l");
        leg->Draw();
        pad1->Draw();
        c->cd();

        TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.3);
        pad2->SetTopMargin(0.02);
        pad2->SetBottomMargin(0.3);
        pad2->SetRightMargin(0.03);
        pad2->SetLeftMargin(0.15);
        pad2->SetGridy();
        pad2->SetTicks();
        pad2->Draw();
        pad2->cd();
        hUp->Divide(hCent);
        hDown->Divide(hCent);
        hCent->Divide(hCent);

        double maxRatio = max(max(hCent->GetMaximum(), hUp->GetMaximum()), hDown->GetMaximum());
        double minRatio = min(min(hCent->GetMinimum(), hUp->GetMinimum()), hDown->GetMinimum());
        int nBins = hCent->GetNbinsX();
        for (int j = 1; j <= nBins; j++) {
            maxRatio = max(maxRatio, 1+hCent->GetBinError(j));
            minRatio = min(minRatio, 1-hCent->GetBinError(j));
        }
        hCent->GetYaxis()->SetRangeUser(minRatio-0.02*(maxRatio-minRatio), maxRatio+0.02*(maxRatio-minRatio));
        hCent->GetYaxis()->SetRangeUser(0.82, 1.18);
        if (variable == "ZNGoodJets_Zexc") hCent->GetXaxis()->SetRangeUser(1, 8);
        hCent->SetTitle(" ");
        hCent->GetYaxis()->SetTitle("Ratio to Central");
        hCent->GetYaxis()->SetTitleSize(0.08);
        hCent->GetYaxis()->SetTitleOffset(0.9);
        hCent->GetYaxis()->CenterTitle();
        hCent->GetYaxis()->SetLabelSize(0.08);
        hCent->GetYaxis()->SetLabelOffset(0.014);
        hCent->GetXaxis()->SetTitleSize(0.13);
        hCent->GetXaxis()->SetLabelSize(0.13);
        hCent->GetXaxis()->SetLabelOffset(0.012);

        hCent->DrawCopy("e");
        hUp->Draw("histsame");
        hDown->Draw("histsame");
        c->cd();
        c->Update();
        c->Draw();

        TString systStr = syst[i/2];
        if (systStr == "S.F.") systStr = "SF";
        c->SaveAs("SystPlot/" + lepSel + "_" + variable + "_" + systStr + ".png");
        c->SaveAs("SystPlot/" + lepSel + "_" + variable + "_" + systStr + ".ps");
        c->SaveAs("SystPlot/" + lepSel + "_" + variable + "_" + systStr + ".eps");
        c->SaveAs("SystPlot/" + lepSel + "_" + variable + "_" + systStr + ".pdf");
        c->SaveAs("SystPlot/" + lepSel + "_" + variable + "_" + systStr + ".C");
        c->SaveAs("SystPlot/" + lepSel + "_" + variable + "_" + systStr + ".root");
    }
}

void createInclusivePlots(bool doNormalized, TString outputFileName, TString lepSel, TH1D *hUnfData, TH2D *hCov[], TH1D *hMadGenCrossSection, TH1D *hSheGenCrossSection, TH1D *hPowGenCrossSection)
{
    TH1D *hInc = (TH1D*) hUnfData->Clone("ZNGoodJets_Zinc");
    TH1D *hIncMad = (TH1D*) hMadGenCrossSection->Clone("ZNGoodJets_Zinc_Mad");
    TH1D *hIncShe = (TH1D*) hSheGenCrossSection->Clone("ZNGoodJets_Zinc_She");
    TH1D *hIncPow = (TH1D*) hPowGenCrossSection->Clone("ZNGoodJets_Zinc_Pow");
    TH2D *hCovInc[12] = {NULL};
    hCovInc[0] = (TH2D*) hCov[0]->Clone("CovDataStat");
    hCovInc[1] = (TH2D*) hCov[1]->Clone("CovMCStat");
    hCovInc[2] = (TH2D*) hCov[2]->Clone("CovJES");
    hCovInc[3] = (TH2D*) hCov[3]->Clone("CovPU");
    hCovInc[4] = (TH2D*) hCov[4]->Clone("CovJER");
    hCovInc[5] = (TH2D*) hCov[5]->Clone("CovXSec");
    hCovInc[6] = (TH2D*) hCov[6]->Clone("CovLES");
    hCovInc[7] = (TH2D*) hCov[7]->Clone("CovLER");
    hCovInc[8] = (TH2D*) hCov[8]->Clone("CovLumi");
    hCovInc[9] = (TH2D*) hCov[9]->Clone("CovSF");
    hCovInc[10] = (TH2D*) hCov[10]->Clone("CovSherpaUnf");
    hCovInc[11] = (TH2D*) hCov[11]->Clone("CovTotSyst");

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
        double binCov[12] = {0};
        for (int j = i; j <= nBins; j++) {
            binSum += hInc->GetBinContent(j);
            binSumMad += hIncMad->GetBinContent(j);
            binSumShe += hIncShe->GetBinContent(j);
            binSumPow += hIncPow->GetBinContent(j);
            binStatError2 += pow(hInc->GetBinError(j), 2);
            binStatMadError2 += pow(hIncMad->GetBinError(j), 2);
            binStatSheError2 += pow(hIncShe->GetBinError(j), 2);
            binStatPowError2 += pow(hIncPow->GetBinError(j), 2);
            for (int k = 0; k < 12; k++) {
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
        for (int k = 0; k < 12; k++) {
            hCovInc[k]->SetBinContent(i, i, binCov[k]);
        }
    }

    TCanvas *crossSectionPlot = makeCrossSectionPlot(lepSel, TString("ZNGoodJets_Zinc"), doNormalized, hInc, hCovInc[11], hIncMad, hIncShe, hIncPow); 
    outputFileName.ReplaceAll("ZNGoodJets_Zexc", "ZNGoodJets_Zinc");
    crossSectionPlot->Draw();
    crossSectionPlot->SaveAs(outputFileName + ".png");
    crossSectionPlot->SaveAs(outputFileName + ".pdf");
    crossSectionPlot->SaveAs(outputFileName + ".eps");
    crossSectionPlot->SaveAs(outputFileName + ".ps");
    crossSectionPlot->SaveAs(outputFileName + ".C");
    createTable(outputFileName, lepSel, TString("ZNGoodJets_Zinc"), doNormalized, hInc, hCovInc);
}

void createTable(TString outputFileName, TString lepSel, TString variable, bool doNormalized, TH1D *hUnfData, TH2D *hCov[])
{
    cout << "Hello" << endl;    
    //--- print out break down of errors ---
    TString title = hUnfData->GetTitle();
    int nBins = hUnfData->GetNbinsX();
    TString var = "";
    TString dSigma = "";
    TString xtitle = hUnfData->GetXaxis()->GetTitle();
    createTitleVariableAnddSigma(variable, doNormalized, xtitle, title, var, dSigma);
    cout << "Title: " << title << endl;
    cout << "Var: " << var << endl;
    cout << "dSig: " << dSigma << endl;
    cout << "nBins: " << nBins << endl;

    TString table = "\\begin{table}[htb!]\n\\begin{center}\n";
    table += "\\caption{Differential cross section in " + title;
    table += " and break down of the systematic uncertainties for the ";
    if (lepSel == "DMu") table += "muon decay channel.}\n";
    if (lepSel == "DE") table += "electron decay channel.}\n";
    table += "\\scriptsize{\n";
    table += "\\begin{tabular}{c|cc|ccccccccc}\n";
    table += "\\multicolumn{12}{c}{" + title + "} \\\\\n";
    table += var + " & " + dSigma + " & \\tiny{Tot. Unc [\\%]} & ";
    table += "\\tiny{stat [\\%]} & \\tiny{MC stat [\\%]} & \\tiny{JES [\\%]} & \\tiny{JER [\\%]} & ";
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
        double xs = hUnfData->GetBinContent(i);
        TString numbers;
        if (title.Index("exclusive jet multiplicity", 0,  TString::ECaseCompare::kIgnoreCase) >= 0) {
            numbers.Form("= %d", i - 1);
        }
        else if (title.Index("inclusive jet multiplicity", 0,  TString::ECaseCompare::kIgnoreCase) >= 0) {
            numbers.Form("$\\geq$ %d", i - 1);
        }
        else {
            numbers.Form("$%g \\ -\\ %g$", hUnfData->GetBinLowEdge(i), hUnfData->GetBinLowEdge(i+1));
        }
        table += numbers + " & "; 
        numbers.Form("%#.3g", xs);
        table += numbers + " & ";
        // total uncertainty
        numbers.Form("%#.2g", sqrt(hCov[0]->GetBinContent(i,i) + hCov[11]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // stat uncertainty
        numbers.Form("%#.2g", sqrt(hCov[0]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // MC stat uncertainty
        numbers.Form("%#.2g", sqrt(hCov[1]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // JES uncertainty
        numbers.Form("%#.2g", sqrt(hCov[2]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // JER uncertainty
        numbers.Form("%#.2g", sqrt(hCov[4]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // PU uncertainty
        numbers.Form("%#.2g", sqrt(hCov[3]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // XSec uncertainty
        numbers.Form("%#.2g", sqrt(hCov[5]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // Lumi uncertainty
        numbers.Form("%#.2g", sqrt(hCov[8]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // LES uncertainty
        numbers.Form("%#.2g", sqrt(hCov[6]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // LER uncertainty
        numbers.Form("%#.2g", sqrt(hCov[7]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // Unf uncertainty
        numbers.Form("%#.2g", sqrt(hCov[10]->GetBinContent(i,i))*100./xs);
        table += numbers + " & ";
        // SF uncertinaty
        numbers.Form("%#.2g", sqrt(hCov[9]->GetBinContent(i,i))*100./xs);
        table += numbers + " \\\\\n";
    }

    table += "\\end{tabular}}\n";
    table += "\\label{tab:" + lepSel + variable + "}\n";
    table += "\\end{center}\\end{table}\n";
    ofstream out(outputFileName + ".tex");
    out << table;
    out.close();
    cout << table << endl;
}


int UnfoldData(const TString lepSel, const TString algo, int svdKterm, RooUnfoldResponse *resp, TH1D *hRecDataMinusFakes, 
        TH1D* &hUnfData, TH2D* &hUnfDataStatCov, TH2D* &hUnfMCStatCov, TString name, 
        double integratedLumi)
{
    //--- make sure we use OverFlow (should already be set to true) ---
    resp->UseOverflow();

    //--- Set the required unfolding algorithm ---
    RooUnfold::Algorithm alg;
    if (algo == "Bayes") {
        alg = RooUnfold::kBayes;
    }
    else if (algo == "SVD") {
        alg = RooUnfold::kSVD;
    }
    else {
        cerr << "Error: the specified algo: " << algo << " is not implemented!" << endl;
        cerr << "       I will proceed with kBayes algo" << endl;
        alg = RooUnfold::kBayes;
    }

    std::cout << "-----------------------" << std::endl;
    TString variable = TString(hRecDataMinusFakes->GetName());


    TFile *f = new TFile("UnfoldingCheck/" + lepSel + "_" + variable + "_" + name + ".root", "RECREATE"); 
    f->cd();
    int finalNIter = -1;
    int nIter = 99;
    int nBinsTmp = hRecDataMinusFakes->GetNbinsX();
    TH1D *hchi2 = new TH1D("hchi2", "hchi2", nBinsTmp, 0.5, nBinsTmp+0.5);
    hchi2->SetTitle("#chi^{2}/ndf for reco vs folded-unfolded for " + TString(hRecDataMinusFakes->GetTitle()));
    hchi2->GetYaxis()->SetTitle("#chi^{2}/ndf");
    hchi2->GetYaxis()->SetTitleOffset(1.40);
    hchi2->GetXaxis()->SetTitle("number of iterations of the Bayes method");
    hchi2->GetXaxis()->CenterTitle();
    hchi2->GetXaxis()->SetNdivisions(nBinsTmp, 0, 0);
    hchi2->GetXaxis()->SetLabelSize(0.03);
    hchi2->SetLineWidth(2);
    for (int i = 1; i <= nBinsTmp; ++i) {
        RooUnfoldResponse *respBis = (RooUnfoldResponse*) resp->Clone();
        TH1D *hRecDataMinusFakesBis = (TH1D*) hRecDataMinusFakes->Clone();
        RooUnfold *RObjectForDataTmp = RooUnfold::New(alg, respBis, hRecDataMinusFakesBis, i);
        int nBinsToSkip = (TString(hRecDataMinusFakesBis->GetName()).Index("JetPt_Zinc") > 0) ? 2 : 0;
        RObjectForDataTmp->IncludeSystematics(0); // new version of RooUnfold: will compute Cov based on Data Statistics only
        std::cout << "niter = " << i << std::endl;
        TH1D* hUnfDataBis = (TH1D*) RObjectForDataTmp->Hreco(RooUnfold::kCovariance);
        TString tmpName = "Unf" + name + "_"; 
        tmpName += i; 
        hUnfDataBis->SetName(tmpName);
        hUnfDataBis->SetTitle(tmpName);
        tmpName = "fol" + name + "_"; 
        tmpName += i; 
        TH1D *hfoldUnfData = foldUnfData(hUnfDataBis, respBis);
        hfoldUnfData->SetName(tmpName);
        hfoldUnfData->SetTitle(tmpName);
        TH1D *hgen = (TH1D*) respBis->Htruth();
        tmpName = "gen" + name + "_";
        tmpName += i;
        hgen->SetName(tmpName);
        hgen->SetTitle(tmpName);
        hgen->Write();
        TH1D *hfoldgen = foldUnfData(hgen, respBis);
        tmpName = "folgen" + name + "_";
        tmpName += i;
        hfoldgen->SetName(tmpName);
        hfoldgen->SetTitle(tmpName);
        hfoldgen->Write();
        TH1D *hmes = (TH1D*) respBis->Hmeasured();
        tmpName = "mes" + name + "_";
        tmpName += i;
        hmes->SetName(tmpName);
        hmes->SetTitle(tmpName);
        hmes->Write();

        double mychi2 = MyChi2Test(hfoldUnfData, hRecDataMinusFakesBis, nBinsToSkip);
        hchi2->SetBinContent(i, mychi2);
        if (i==1) hRecDataMinusFakesBis->Write("Unf" + name + "_0"); 
        hUnfDataBis->Write();
        hfoldUnfData->Write();
        if (mychi2 < 1./sqrt(2) && finalNIter < 0) {
            nIter = i;
            finalNIter = i;
            std::cout << "Will use " << nIter << " iterations with a final Chi2/ndf of: " << mychi2 << std::endl;
            //break;
        }
    }

    nIter = min(nIter, 20);
    //nIter = max(nIter, 2);
    nIter = 4;

    std::cout << "\n---------------------------------------------------------------------------------------------------------------\n-" << std::endl;
    std::cout << nIter << std::endl;
    std::cout << finalNIter << std::endl;
    hchi2->GetYaxis()->SetRangeUser(0, max(1.3, 1.1*hchi2->GetMaximum()));
    TArrow *arrowChi2 = new TArrow(nIter, min(1.2, hchi2->GetMaximum()), nIter, 0.8, 0.02, "|>");
    arrowChi2->SetLineColor(kRed);
    arrowChi2->SetFillColor(kRed);
    arrowChi2->SetLineWidth(2);
    TLine *line = new TLine(0.5, 0.7, nBinsTmp + 0.5, 0.7);
    line->SetLineColor(kBlack);
    line->SetLineStyle(kDashed);
    line->SetLineWidth(2);
    TCanvas *chchi2 = new TCanvas("chchi2", "chchi2", 600, 600);
    chchi2->cd();
    chchi2->SetGrid();
    hchi2->DrawCopy();
    arrowChi2->Draw();
    line->Draw();
    hchi2->Write();
    chchi2->Write();
    RooUnfold *RObjectForDataBinByBin = RooUnfold::New(RooUnfold::kBinByBin, resp, hRecDataMinusFakes);
    TH1D *hUnfDataBinByBin = (TH1D*) RObjectForDataBinByBin->Hreco(RooUnfold::kCovariance);
    hUnfDataBinByBin->SetName("UnfDataBinByBin" + name);
    hUnfDataBinByBin->Write();
    RooUnfold *RObjectForDataBayes = RooUnfold::New(RooUnfold::kBayes, resp, hRecDataMinusFakes, nIter);
    TH1D *hUnfDataBayes = (TH1D*) RObjectForDataBayes->Hreco(RooUnfold::kCovariance);
    hUnfDataBayes->SetName("UnfDataBayes" + name);
    hUnfDataBayes->Write();

    for (int i(1); i <= nBinsTmp; i++) {
        RooUnfold *RObjectForDataSVD = RooUnfold::New(RooUnfold::kSVD, resp, hRecDataMinusFakes, i);
        TH1D *hUnfDataSVD = (TH1D*) RObjectForDataSVD->Hreco(RooUnfold::kCovariance);
        hUnfDataSVD->SetName("UnfDataSVD_" + TString::Format("%d", i) + "_" + name);
        hUnfDataSVD->Write();
    }

    TSVDUnfold *unfoldTSVD = new TSVDUnfold(hRecDataMinusFakes, (TH1D*)resp->Htruth(), (TH1D*)resp->Hmeasured(), (TH2D*)resp->Hresponse());
    TH1D *unfresult = (TH1D*) unfoldTSVD->Unfold(1);
    TH1D *hmodDOriginal = (TH1D*) unfoldTSVD->GetD();
    TH1D *hSV       = (TH1D*) unfoldTSVD->GetSV();

    TH1D *hmodD = new TH1D("hmodD", "hmodD", nBinsTmp, 0.5, nBinsTmp+0.5);
    for (int i(0); i <= nBinsTmp+1; i++) {
        hmodD->SetBinContent(i, hmodDOriginal->GetBinContent(i));
    }
    hmodD->SetTitle(hmodDOriginal->GetTitle() + TString(" for ") + TString(hRecDataMinusFakes->GetTitle()));
    hmodD->GetXaxis()->SetNdivisions(nBinsTmp, 0, 0);
    hmodD->GetXaxis()->SetLabelSize(0.03);
    hmodD->GetYaxis()->SetTitle("|d_{i}|");
    hmodD->GetYaxis()->SetTitleOffset(1.40);
    hmodD->GetXaxis()->SetTitle("regularization parameter of the SVD method");
    hmodD->GetXaxis()->CenterTitle();
    TArrow *arrowSvd = new TArrow(svdKterm, 20, svdKterm, 1.1*hmodD->GetBinContent(svdKterm), 0.02, "|>");
    arrowSvd->SetLineColor(kRed);
    arrowSvd->SetFillColor(kRed);
    arrowSvd->SetLineWidth(2);

    hmodD->SetName("modD" + name);
    TCanvas *chmodD = new TCanvas("chmodD", "chmodD", 600, 600);
    chmodD->cd();
    chmodD->SetGrid();
    chmodD->SetLogy();
    hmodD->DrawCopy();
    arrowSvd->Draw();
    chmodD->Write();
    hmodD->Write();

    f->Close();

    std::cout << "\n---------------------------------------------------------------------------------------------------------------\n-" << std::endl;

    //--- Unfold data minus background ---
    RooUnfold *RObjectForData = RooUnfold::New(alg, resp, hRecDataMinusFakes, nIter);

    //--- get the unfolded result ---
    RObjectForData->IncludeSystematics(0); // new version of RooUnfold: will compute Cov based on Data Statistics only
    hUnfData = (TH1D*) RObjectForData->Hreco(RooUnfold::kCovariance);
    hUnfData->SetName("UnfData" + name);
    hUnfData->SetTitle(hRecDataMinusFakes->GetTitle());

    if (algo == "Bayes") {
        //--- get covariance from statistics on Data ---
        hUnfDataStatCov = M2H(RObjectForData->Ereco(RooUnfold::kCovariance)); // new version of RooUnfold   
        hUnfDataStatCov->SetName("UnfDataStatCov" + name);
        hUnfDataStatCov->SetTitle(hRecDataMinusFakes->GetTitle());

        //--- get covariance from MC stat ---
        RooUnfold *RObjectForMC = RooUnfold::New(alg, resp, hRecDataMinusFakes, nIter);
        RObjectForMC->IncludeSystematics(2); // new version of RooUnfold: will compute Cov based on MC Statistics only
        hUnfMCStatCov = M2H(RObjectForMC->Ereco(RooUnfold::kCovariance)); // new version of RooUnfold
        hUnfMCStatCov->SetName("UnfMCStatCov" + name);
        hUnfMCStatCov->SetTitle(hRecDataMinusFakes->GetTitle());
    }

    //--- divide by luminosity ---
    hUnfData->Scale(1./integratedLumi);
    hUnfDataStatCov->Scale(1./(integratedLumi*integratedLumi));
    hUnfMCStatCov->Scale(1./(integratedLumi*integratedLumi));
    if ("LumiUp" == name) {
        hUnfData->Scale(1./1.026);
        hUnfDataStatCov->Scale(1./(1.026*1.026));
        hUnfMCStatCov->Scale(1./(1.026*1.026));
    }
    else if ("LumiDown" == name) {
        hUnfData->Scale(1./0.974);
        hUnfDataStatCov->Scale(1./(0.974*0.974));
        hUnfMCStatCov->Scale(1./(0.974*0.974));
    }

    //--- divide by bin width to get cross section ---
    int nBins = hUnfData->GetNbinsX();
    for (int i = 1; i <= nBins; ++i) {
        double binWidth = hUnfData->GetBinWidth(i);
        hUnfData->SetBinContent(i, hUnfData->GetBinContent(i)*1./binWidth);
        hUnfData->SetBinError(i, hUnfData->GetBinError(i)*1./binWidth);
        for (int j = 1; j <= nBins; ++j) {
            hUnfDataStatCov->SetBinContent(i, j, hUnfDataStatCov->GetBinContent(i, j)*1./(binWidth*binWidth));
            hUnfDataStatCov->SetBinError(i, j, hUnfDataStatCov->GetBinError(i, j)*1./(binWidth*binWidth));
            hUnfMCStatCov->SetBinContent(i, j, hUnfMCStatCov->GetBinContent(i, j)*1./(binWidth*binWidth));
            hUnfMCStatCov->SetBinError(i, j, hUnfMCStatCov->GetBinError(i, j)*1./(binWidth*binWidth));
        }
    }
    return nIter;
}


//--- This is needed because we use overflow and the matrices have 2 additional bins ---
TH2D* M2H(const TMatrixD m) 
{
    int nBinsY = m.GetNrows();
    int nBinsX = m.GetNrows();

    TH2D *h = new TH2D(m.GetName(), m.GetName(), nBinsX-2, 0, nBinsX-2, nBinsY-2, 0, nBinsY-2);
    for (int i = 0; i < nBinsX; ++i) {
        for (int j = 0; j < nBinsY; ++j) {
            h->SetBinContent(i, j, m(i, j));
        }
    }

    return h;
}

TH2D* makeCovFromUpAndDown(const TH1D* hUnfDataCentral, const TH1D* hUnfDataUp, const TH1D* hUnfDataDown, TString name)
{
    int nBins = hUnfDataCentral->GetNbinsX();
    TH2D* h = new TH2D(name, name, nBins, 0, nBins, nBins, 0, nBins);

    for (int i = 1; i <= nBins; ++i) {
        double sigmaMeani = 0.5*fabs(hUnfDataUp->GetBinContent(i) - hUnfDataDown->GetBinContent(i)); 
        if (name.Index("Sherpa") >= 0) sigmaMeani *= 2;
        int signi = (hUnfDataUp->GetBinContent(i) - hUnfDataDown->GetBinContent(i) < 0) ? -1 : 1;
        if (i > 1 && i < nBins) {
            if ((hUnfDataUp->GetBinContent(i-1) - hUnfDataDown->GetBinContent(i-1)) * (hUnfDataUp->GetBinContent(i+1) - hUnfDataDown->GetBinContent(i+1)) < 0) {
                sigmaMeani = 0.5*(0.5*fabs(hUnfDataUp->GetBinContent(i-1) - hUnfDataDown->GetBinContent(i-1)) + 0.5*fabs(hUnfDataUp->GetBinContent(i+1) - hUnfDataDown->GetBinContent(i+1)));
                if (name.Index("Sherpa") >= 0) sigmaMeani *= 2;
            }
        }

        for (int j = 1; j <= nBins; ++j) {
            double sigmaMeanj = 0.5*fabs(hUnfDataUp->GetBinContent(j) - hUnfDataDown->GetBinContent(j)); 
            if (name.Index("Sherpa") >= 0) sigmaMeanj *= 2;
            int signj = (hUnfDataUp->GetBinContent(j) - hUnfDataDown->GetBinContent(j) < 0) ? -1 : 1;
            if (j > 1 && j < nBins) {
                if ((hUnfDataUp->GetBinContent(j-1) - hUnfDataDown->GetBinContent(j-1)) * (hUnfDataUp->GetBinContent(j+1) - hUnfDataDown->GetBinContent(j+1)) < 0) {
                    sigmaMeanj = 0.5*(0.5*fabs(hUnfDataUp->GetBinContent(j-1) - hUnfDataDown->GetBinContent(j-1)) + 0.5*fabs(hUnfDataUp->GetBinContent(j+1) - hUnfDataDown->GetBinContent(j+1)));
                    if (name.Index("Sherpa") >= 0) sigmaMeanj *= 2;
                }
            }

            double correlation = (i == j) ? 1 : 1;
            h->SetBinContent(i, j, correlation * signi * signj * sigmaMeani * sigmaMeanj);
        }
    }

    return h;
}

TH1D* makeCrossSectionHist(TH1D* hGenDYJets, double integratedLumi)
{
    TH1D *hGenCrossSection = (TH1D*) hGenDYJets->Clone();
    //--- divide by luminosity ---
    hGenCrossSection->Scale(1./integratedLumi);

    int nBins = hGenCrossSection->GetNbinsX();
    for (int i = 1; i <= nBins; ++i) {
        double binWidth = hGenCrossSection->GetBinWidth(i);
        hGenCrossSection->SetBinContent(i, hGenCrossSection->GetBinContent(i)*1./binWidth);
        hGenCrossSection->SetBinError(i, hGenCrossSection->GetBinError(i)*1./binWidth);
    }

    return hGenCrossSection;
}

TH1D* foldUnfData(TH1D *hUnfData, RooUnfoldResponse *hresp)
{
    TH1D *hfoldUnfData = (TH1D*) hUnfData->Clone();
    RooUnfoldResponse *resp = (RooUnfoldResponse*) hresp->Clone();
    TH2D *hres = (TH2D*) resp->Hresponse();
    TH1D *hgen = (TH1D*) resp->Htruth();


    int nBins = hfoldUnfData->GetNbinsX();

    for (int i = 0; i <= nBins; ++i) {
        double totGen = hgen->GetBinContent(i);
        for (int j = 0; j <= nBins; ++j) {
            if (totGen != 0.0) {
                hres->SetBinContent(j, i, hres->GetBinContent(j, i)/totGen);
                hres->SetBinError(j, i, hres->GetBinError(j, i)/totGen);
                hres->SetBinError(j, i, 0.0);
            }
            else {
                hres->SetBinContent(i, j, 0);
                hres->SetBinError(i, j, hres->GetBinError(i, j));
                hres->SetBinError(i, j, 0.0);
            }
        }
    }

    for (int i = 0; i <= nBins; ++i) {
        double sum = 0.0;
        double error2 = 0.0;
        for (int j = 0; j <= nBins; ++j) {
            sum += hres->GetBinContent(i, j) * hUnfData->GetBinContent(j);
            error2 += pow(hres->GetBinContent(i, j) * hUnfData->GetBinError(j), 2);
        }
        hfoldUnfData->SetBinContent(i, sum);
        hfoldUnfData->SetBinError(i, sqrt(error2));
    }
    return hfoldUnfData;
}


void test()
{
    TH1D *hreco = new TH1D("hreco", "hreco", 2, 0, 2);
    TH1D *hgen = new TH1D("hgen", "hgen", 2, 0, 2);
    TH2D *hresp = new TH2D("hresp", "hresp", 2, 0, 2, 2, 0, 2);

    hreco->Sumw2();
    hgen->Sumw2();
    hresp->Sumw2();

    hreco->Fill(0.5, 3.5);
    hreco->Fill(1.5, 5);

    hgen->Fill(0.5, 4);
    hgen->Fill(1.5, 6);

    hresp->Fill(0.5, 0.5, 3);
    hresp->Fill(0.5,1.5,0.5);
    hresp->Fill(1.5,0.5,0.5);
    hresp->Fill(1.5,1.5,4.5);

    RooUnfoldResponse *resp = new RooUnfoldResponse(NULL, hgen, hresp);

    RooUnfold *RObjectForData = RooUnfold::New(RooUnfold::kBayes, resp, hreco, 3);
    TH1D *hUnfData = (TH1D*) RObjectForData->Hreco(RooUnfold::kCovariance);

    TH1D *hfoldUnfData = foldUnfData(hUnfData, resp);

    std::cout << "reco: " << hreco->GetBinContent(1) << "   " << hreco->GetBinContent(2) << std::endl; 
    std::cout << "gen : " << hgen->GetBinContent(1) << "   " << hgen->GetBinContent(2) << std::endl; 
    std::cout << "unf : " << hUnfData->GetBinContent(1) << "   " << hUnfData->GetBinContent(2) << std::endl; 
    std::cout << "fol : " << hfoldUnfData->GetBinContent(1) << "   " << hfoldUnfData->GetBinContent(2) << std::endl; 



}

double MyChi2Test(TH1D *h1, TH1D *h2, int nBinsToSkip)
{
    TH1D *h1Copy = (TH1D*) h1->Clone();
    TH1D *h2Copy = (TH1D*) h2->Clone();

    for (int i = 0; i <= nBinsToSkip; ++i) {
        h1Copy->SetBinContent(i, 0.0);
        h2Copy->SetBinContent(i, 0.0);
    }
    return h1Copy->Chi2Test(h2Copy, "WW,P,CHI2/NDF");

}
