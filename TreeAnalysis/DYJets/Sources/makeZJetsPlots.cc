#include "PlotSettings.h"
#include "makeZJetsPlots.h"
#include <TStyle.h>
#include <sstream>


TCanvas* makeZJetsPlots(TH1D *hStat, TH2D *hCovSyst, TH1D *gen1, TH1D *gen2, TH1D *gen3)
{
    TH1D *hSyst = (TH1D*) hStat->Clone();
    int nBins = hSyst->GetNbinsX();

    for (int i = 1; i <= nBins; ++i) {
        hSyst->SetBinError(i, sqrt(pow(hStat->GetBinError(i), 2) + hCovSyst->GetBinContent(i, i)));
    }

    TH1D *hPDFUp = (TH1D*) hStat->Clone();
    hPDFUp->Scale(1.10);
    TH1D *hPDFDown = (TH1D*) hStat->Clone();
    hPDFDown->Scale(0.75);

    TCanvas *can = makeZJetsPlots(hStat, hSyst, hPDFUp, hPDFDown, gen1);
    can->Update();

    return can;


}

TCanvas* makeZJetsPlots(TH1D *hStat, TH1D *hSyst, TH1D *hPDFUp, TH1D *hPDFDown, TH1D *gen1, TH1D *gen2, TH1D *gen3)
{
    TGraphAsymmErrors *grCentralStat = createGrFromHist(hStat);
    TGraphAsymmErrors *grCentralSyst = createGrFromHist(hSyst);

    TCanvas *plots = makeZJetsPlots(grCentralStat, grCentralSyst, hPDFUp, hPDFDown, gen1, gen2, gen3);

    return plots;

}

TCanvas* makeZJetsPlots(TGraphAsymmErrors *grCentralStat, TGraphAsymmErrors *grCentralSyst, TH1D *hPDFUp, TH1D *hPDFDown, TH1D *gen1, TH1D *gen2, TH1D *gen3)
{

    //--- Determine how many comparison we have ---
    int numbOfGenerator = 1;
    if (gen2) numbOfGenerator = 2;
    if (gen3) numbOfGenerator = 3;
    //---------------------------------------------


    //--- Declare all additional TGraphs ---
    TGraphAsymmErrors *grCentralStatRatio = createRatioGraph(grCentralStat);
    TGraphAsymmErrors *grCentralSystRatio = createRatioGraph(grCentralSyst);
    TGraphAsymmErrors *grGen1ToCentral = createGenToCentral(gen1, grCentralStat);
    TGraphAsymmErrors *grGen1PDFSyst = createPDFSystGraph(hPDFUp, hPDFDown, grGen1ToCentral); 
    TGraphAsymmErrors *grGen2ToCentral = NULL;
    TGraphAsymmErrors *grGen2PDFSyst = NULL;
    if (gen2) {
        grGen2ToCentral = createGenToCentral(gen2, grCentralStat);
        grGen2PDFSyst = createPDFSystGraph(hPDFUp, hPDFDown, grGen2ToCentral); 
    }
    TGraphAsymmErrors *grGen3ToCentral = NULL;
    TGraphAsymmErrors *grGen3PDFSyst = NULL;
    if (gen3) {
        grGen3ToCentral = createGenToCentral(gen3, grCentralStat);
        grGen3PDFSyst = createPDFSystGraph(hPDFUp, hPDFDown, grGen3ToCentral); 
    }
    //---------------------------------------------

    //--- Main Canvas ---
    std::string variable = gen1->GetName();
    double maximum = gen1->GetMaximum();
    variable = variable.substr(3);
    TString canvasName = "plots_" + variable;
    TCanvas *plots = new TCanvas(canvasName, canvasName, 600, 800);
    //-------------------

    //--- First Pad ---
    plots->cd();
    TPad *plot1 = new TPad("plot1", "plot1", 0., 0., 0., 0.);
    setAndDrawTPad(canvasName, plot1, 1, numbOfGenerator);

    //--- TLegend ---
    TLegend *legend = new TLegend(0.47, 0.74, 0.99, 0.98);
    customizeLegend(legend, numbOfGenerator);
    //------------------

    customizeCentral(grCentralSyst, legend, "Data");
    customizeCentral(grCentralStat, (bool)false);
    customizeCentral(grCentralSystRatio, (bool)true);
    customizeCentral(grCentralStatRatio, (bool)true);
    grCentralSyst->Draw("a2");
    customizeGenHist(gen1, 1, legend, "Sherpa2 (#leq2j@NLO 3,4j@LO + PS)");
    gen1->DrawCopy("ESAME");
    if (gen2) {
        customizeGenHist(gen2, 2, legend, "Madgraph + Pythia6 (#leq4j@LO + PS)");
        gen2->DrawCopy("ESAME");
    }
    if (gen3) {
        customizeGenHist(gen3, 3, legend, "Powheg + Pythia6 (Z+2j@NLO + PS)");
        gen3->DrawCopy("ESAME");
    }

    configYaxis(grCentralSyst, gen1, gen2, gen3);
    configXaxis(grCentralSyst, gen1);
    grCentralStat->Draw("p");
    if (canvasName.Index("Eta") >= 0) {
        grCentralSyst->GetHistogram()->GetYaxis()->SetRangeUser(0.001, 1.4*maximum);
    }
    legend->Draw("same");

    //--- TLatex stuff ---
    TLatex *latexLabel = new TLatex(); 
    latexLabel->SetTextSize(0.65);
    if (gen2) latexLabel->SetTextSize(0.041);
    if (gen3) latexLabel->SetTextSize(0.0405);
    latexLabel->SetTextFont(42);
    latexLabel->SetLineWidth(2);
    latexLabel->SetNDC();

    latexLabel->SetTextFont(61);
    latexLabel->DrawLatex(0.13,0.95,"CMS");
    latexLabel->SetTextFont(52);
    latexLabel->DrawLatex(0.20,0.95,"Preliminary");
    latexLabel->SetTextFont(42);
    latexLabel->DrawLatex(0.13,0.95-0.045,"19.6 fb^{-1} (8 TeV)");
    latexLabel->DrawLatex(0.18,0.21-0.05,"anti-k_{T} (R = 0.5) Jets");
    latexLabel->DrawLatex(0.18,0.21-0.11,"p_{T}^{jet} > 30 GeV, |#eta^{jet}| < 2.4 ");
    latexLabel->DrawLatex(0.18,0.21-0.17,"Z/#gamma*#rightarrow ll channel");
    latexLabel->Draw("same");

    TLatex *ytitle = new TLatex();
    ytitle->SetTextSize(0.05);
    if (gen2) ytitle->SetTextSize(0.056);
    if (gen3) ytitle->SetTextSize(0.06);
    ytitle->SetTextFont(42);
    ytitle->SetLineWidth(2);
    ytitle->SetTextColor(kBlack);
    ytitle->SetNDC();
    ytitle->SetTextAlign(33);
    ytitle->SetTextAngle(90);
    std::string strYtitle = getYaxisTitle(gen1);
    if (strYtitle.find("eta") != std::string::npos) {
        size_t first = strYtitle.find("#eta");
        std::string tmp1 = strYtitle.substr(0, first);
        std::string tmp2 = strYtitle.substr(first);
        strYtitle = tmp1 + "|" + tmp2;
        size_t second = strYtitle.find(")");
        tmp1 = strYtitle.substr(0, second+1);
        tmp2 = strYtitle.substr(second+1);
        strYtitle = tmp1 + "|" + tmp2;
    }
    ytitle->DrawLatex(0.008,0.91,strYtitle.c_str());
    //-----------------------

    //--- End Of first Pad ---

    //--- Second Pad ---
    plots->cd();
    TPad *plot2 = new TPad("plot2", "plot2", 0., 0., 0., 0.);
    setAndDrawTPad(canvasName, plot2, 2, numbOfGenerator);

    //--- TLegend ---
    TLegend *legend2 = new TLegend(0.16, 0.05, 0.42, 0.20);
    customizeLegend(legend2, 1, numbOfGenerator);
    customizeGenGraph(grGen1ToCentral, grGen1PDFSyst, 1, "Sherpa2/Data", numbOfGenerator, legend2);
    configXaxis(grGen1ToCentral, gen1);
    grGen1PDFSyst->SetFillStyle(ZJetsFillStyle);
    grGen1PDFSyst->SetFillColor(ZJetsPdfFillColor[0]);
    grGen1ToCentral->Draw("a2");
    //grGen1PDFSyst->Draw("2");
    grGen1ToCentral->Draw("2");
    grCentralSystRatio->Draw("2");
    grCentralStatRatio->Draw("p");
    grGen1ToCentral->Draw("p");
    legend2->Draw("same");
    plot2->RedrawAxis();
    //--- End of Second Pad ---


    if (gen2) {
        //--- Third Pad ---
        plots->cd();
        TPad *plot3 = new TPad("plot3", "plot3", 0., 0., 0., 0.);
        setAndDrawTPad(canvasName, plot3, 3, numbOfGenerator);

        //--- TLegend ---
        TLegend *legend3 = new TLegend(0.16, 0.05, 0.42, 0.20);
        customizeLegend(legend3, 2, numbOfGenerator);
        customizeGenGraph(grGen2ToCentral, grGen2PDFSyst, 2, "MadGraph/Data", numbOfGenerator, legend3);
        configXaxis(grGen2ToCentral, gen2);
        grGen2PDFSyst->SetFillStyle(ZJetsFillStyle);
        grGen2PDFSyst->SetFillColor(ZJetsPdfFillColor[2]);
        grGen2ToCentral->Draw("a2");
        //grGen2PDFSyst->Draw("2");
        grGen2ToCentral->Draw("2");
        grCentralSystRatio->Draw("2");
        grCentralStatRatio->Draw("p");
        grGen2ToCentral->Draw("p");
        legend3->Draw("same");
        plot3->RedrawAxis();
        //--- End of Third Pad ---
    }

    if (gen3) {
        //--- Fourth Pad ---
        plots->cd();
        TPad *plot4 = new TPad("plot4", "plot4", 0., 0., 0., 0.);
        setAndDrawTPad(canvasName, plot4, 4, numbOfGenerator);

        //--- TLegend ---
        TLegend *legend4 = new TLegend(0.16, 0.05, 0.42, 0.20);
        customizeLegend(legend4, 3, numbOfGenerator);
        customizeGenGraph(grGen3ToCentral, grGen3PDFSyst, 3, "Powheg/Data", numbOfGenerator, legend4);
        configXaxis(grGen3ToCentral, gen3);
        grGen3PDFSyst->SetFillStyle(ZJetsFillStyle);
        grGen3PDFSyst->SetFillColor(ZJetsPdfFillColor[1]);
        grGen3ToCentral->Draw("a2");
        //grGen3PDFSyst->Draw("2");
        grGen3ToCentral->Draw("2");
        grCentralSystRatio->Draw("2");
        grCentralStatRatio->Draw("p");
        grGen3ToCentral->Draw("p");
        legend4->Draw("same");
        plot4->RedrawAxis();
        //--- End of Fourth Pad ---
    }

    return plots;
}
