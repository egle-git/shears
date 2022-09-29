#include "../include/Utilities.h"

// Functions
void PlotProjections(TH1D*hProjX,TH1D*hProjY,TString saveName);
void MakeMCvsData(bool logplot);
void MakeMigrationMatrixPlot();
void SetHistOptions();
TCanvas*MakeCanvas(TString cName);

// Global TStrings
TString mc_name     = "histograms/fromShears/dyjets-DYJets.root";
TString data_name   = "histograms/fromShears/dyjets-data.root";
TString save_loc    = "plots/";
TString reco_hist   = "mass_wide_range_inc0jet";
TString gen_hist    = "mass_wide_range_inc0jet-gen";
TString matrix_hist = reco_hist + "-matrix";

// Global files
TFile*mc_file = new TFile(mc_name);
TFile*data_file = new TFile(data_name);

// Global histograms
TH1D*_hGen;
TH1D*_hRec;
TH1D*_hDat;
TH2D*_hMat;

void makePlots()
{
    gStyle->SetOptStat(0);
    gStyle->SetPalette(1);
    gROOT->SetBatch(true);

    // Get histograms 
    _hGen = (TH1D*)mc_file->Get(gen_hist);
    _hRec = (TH1D*)mc_file->Get(reco_hist);
    _hDat = (TH1D*)data_file->Get(reco_hist);
    _hMat = (TH2D*)mc_file->Get(matrix_hist);
    SetHistOptions();

    bool logPlot = true;
    MakeMCvsData(logPlot);
    MakeMigrationMatrixPlot();
} 

void SetHistOptions()
{
    _hGen->SetTitle("gen dimuon mass");
    _hGen->GetXaxis()->SetTitle("m_{gen} [GeV]");
    _hGen->GetXaxis()->SetNoExponent();
    _hGen->GetXaxis()->SetMoreLogLabels();
    _hGen->GetYaxis()->SetNoExponent();
    _hGen->GetYaxis()->SetMoreLogLabels();

    _hRec->SetTitle("reco dimuon mass");
    _hRec->GetXaxis()->SetTitle("m_{reco} [GeV]");
    _hRec->GetXaxis()->SetNoExponent();
    _hRec->GetXaxis()->SetMoreLogLabels();
    _hRec->GetYaxis()->SetNoExponent();
    _hRec->GetYaxis()->SetMoreLogLabels();

    _hDat->SetTitle("data dimuon mass");
    _hDat->GetXaxis()->SetTitle("m_{data} [GeV]");
    _hDat->GetXaxis()->SetNoExponent();
    _hDat->GetXaxis()->SetMoreLogLabels();
    _hDat->GetYaxis()->SetNoExponent();
    _hDat->GetYaxis()->SetMoreLogLabels();

    _hMat->SetTitle("dimuon mass migration matrix");
    _hMat->GetXaxis()->SetTitle("m_{reco} [GeV]");
    _hMat->GetYaxis()->SetTitle("m_{gen} [GeV]");
    _hMat->GetXaxis()->SetNoExponent();
    _hMat->GetXaxis()->SetMoreLogLabels();
    _hMat->GetYaxis()->SetNoExponent();
    _hMat->GetYaxis()->SetMoreLogLabels();
}

void PlotProjections(TH1D*hProjX,TH1D*hProjY,TString saveName)
{
    TH1D*hGen = (TH1D*)_hGen->Clone();
    TH1D*hRec = (TH1D*)_hRec->Clone();

    hGen->SetLineColor(kRed);
    hGen->SetMarkerColor(kRed);
    hGen->SetMarkerStyle(20);
    hRec->SetLineColor(kBlue);
    hRec->SetMarkerColor(kBlue);
    hRec->SetMarkerStyle(20);
    int nBins = hRec->GetNbinsX();
    double x1 = hRec->GetBinLowEdge(1);
    double x2 = hRec->GetBinLowEdge(nBins);
    x2 += hRec->GetBinWidth(nBins);

    TLegend*legend = new TLegend(0.65,0.9,0.9,0.75);
    legend->SetTextSize(0.02);
    legend->AddEntry(hGen,"gen");
    legend->AddEntry(hRec,"reco");
    legend->AddEntry(hProjX,"x-projection");
    legend->AddEntry(hProjY,"y-projection");

    TLine*line = new TLine(x1,1,x2,1);
    line->SetLineColor(kBlack);

    double ratioRange = 0.2;
    double upperBound = 1.0-ratioRange;
    double lowerBound = 1.0+ratioRange;
    TH1F*hRatioTrue = (TH1F*)hGen->Clone("trueRatio");
    hRatioTrue->Divide(hProjY);
    hRatioTrue->SetMarkerStyle(20);
    hRatioTrue->SetMarkerColor(kRed);
    hRatioTrue->SetMinimum(1.0-ratioRange);
    hRatioTrue->SetMaximum(1.0+ratioRange);

    TH1F*hRatioReco = (TH1F*)hRec->Clone("recoRatio");
    hRatioReco->Divide(hProjX);
    hRatioReco->SetMarkerStyle(20);
    hRatioReco->SetMarkerColor(kBlue);
    hRatioReco->SetMinimum(1.0-ratioRange);
    hRatioReco->SetMaximum(1.0+ratioRange);

    TCanvas*canvas = new TCanvas("canvas","",0,0,1000,1000);
    const float padmargins = 0.03;
    const float yAxisMinimum = 0.1;
    const float yAxisMaximum = 1e7;
    TPad*pad1 = new TPad("","",0,0.3,1.0,1.0);
    pad1->SetLogx();
    pad1->SetLogy();
    pad1->SetBottomMargin(padmargins);
    pad1->SetGrid();
    pad1->SetTicks(1,1);
    pad1->Draw();
    pad1->cd();

    hProjX->SetTitle("matrix projections vs 1D distributions");
    hProjX->GetXaxis()->SetTitle("m_{#mu#mu} [GeV]");
    hProjX->GetXaxis()->SetNoExponent();
    hProjX->GetXaxis()->SetMoreLogLabels();
    hProjX->Draw("hist");
    hProjY->Draw("hist,same");
    hGen->Draw("pe,same");
    hRec->Draw("pe,same");
    legend->Draw("same");

    double ratioSplit = 0.18;
    canvas->cd();
    TPad*pad2 = new TPad("","",0,ratioSplit,1,0.3);
    pad2->SetLogx();
    pad2->SetTopMargin(padmargins);
    pad2->SetBottomMargin(0.2);
    pad2->SetGrid();
    pad2->SetTicks(1,1);
    pad2->Draw();
    pad2->cd();
    hRatioTrue->GetYaxis()->SetLabelSize(0.06);
    hRatioTrue->GetYaxis()->SetTitleSize(0.08);
    hRatioTrue->GetYaxis()->SetTitleOffset(0.3);
    hRatioTrue->GetYaxis()->SetTitle("gen/matrix projection");
    hRatioTrue->GetXaxis()->SetLabelSize(0);
    hRatioTrue->GetXaxis()->SetTitleSize(0);
    hRatioTrue->Draw("pe");
    line->Draw("same");

    canvas->cd();
    TPad*pad3 = new TPad("","",0,0.05,1,ratioSplit);
    pad3->SetLogx();
    pad3->SetTopMargin(padmargins);
    pad3->SetBottomMargin(0.2);
    pad3->SetGrid();
    pad3->SetTicks(1,1);
    pad3->Draw();
    pad3->cd();
    hRatioReco->GetYaxis()->SetLabelSize(0.06);
    hRatioReco->GetYaxis()->SetTitleSize(0.08);
    hRatioReco->GetYaxis()->SetTitleOffset(0.3);
    hRatioReco->GetYaxis()->SetTitle("reco/matrix projection");
    hRatioReco->GetXaxis()->SetLabelSize(0.1);
    hRatioReco->GetXaxis()->SetTitleSize(0.1);
    hRatioReco->GetXaxis()->SetNoExponent();
    hRatioReco->GetXaxis()->SetMoreLogLabels();
    hRatioReco->GetXaxis()->SetTitle("mass [GeV]");
    hRatioReco->Draw("pe");
    line->Draw("same");

    TString saveProj = save_loc;
    saveProj += saveName;
    canvas->SaveAs(saveProj);
}

TCanvas*MakeCanvas(TString cName)
{
    TCanvas*canvas = new TCanvas(cName,"",0,0,1000,1000);
    canvas->SetGrid();
    canvas->SetLogy();
    canvas->SetLogx();
    canvas->SetLogz();

    return canvas;
}

void MakeMCvsData(bool logplot)
{
    TH1D*hDat = (TH1D*)_hDat->Clone();
    TH1D*hRec = (TH1D*)_hRec->Clone();

    double wsum, lumi, xsec;
    TH1*job_info;
    TVectorD*job_info_average = nullptr;
    double samplescale = 1.0;
    lumi = 19520.0;

    TString prefix = "histograms/fromShears/dyjets-";
    TString suffix = ".root";

    hDat->SetMarkerStyle(20);
    hDat->SetMarkerColor(kBlack);
    hDat->SetLineColor(kBlack);
    hRec->SetFillColor(kOrange-1);
    hRec->SetLineColor(kOrange-1);
    hRec->SetMarkerColor(kOrange-1);
    hDat->SetMarkerStyle(20);
    hDat->SetMarkerColor(kBlack);
    hDat->SetLineColor(kBlack);
    hRec->SetFillColor(kOrange-1);
    hRec->SetLineColor(kOrange-1);

    job_info = (TH1*)mc_file->Get("_job_info");
    mc_file->GetObject("_job_info_average", job_info_average);

    wsum = job_info->GetBinContent(2);
    xsec = (*job_info_average)[1];
    samplescale = lumi*xsec/wsum;

    hRec->Scale(samplescale);

    vector<TString> back_file_name = {
        "WJetsToLNu",
        "WWTo2L2Nu",
        "WZ",
        "ZZ",
        "TauTau",
        "TT",
        "ST_s-channel",
        "ST_t-channel_top",
        "ST_t-channel_antitop",
        "ST_tW_top",
        "ST_tW_antitop",
    };
    vector<int> hist_color = {
        kRed+1,     // WToLNu
        kRed+2,     // WW
        kRed+3,     // WZ
        kRed+4,     // ZZ
        kGreen+3,   // tautau
        kAzure,     // tt
        kBlue+2,    // ST-s-channel
        kBlue+3,    // ST-t-channel_top
        kBlue+4,    // ST-t-channel_antitop
        kAzure+4,   // ST-tW
        kAzure+7,   // STbar-tW
    };

    int nBackFiles = back_file_name.size();
    vector<TFile*> back_file;
    vector<TH1D*> back_hist;
    THStack*hStack = new THStack("stack plot","");
    TLegend*legend = new TLegend(0.65,0.9,0.9,0.60);
    legend->SetTextSize(0.02);
    legend->AddEntry(hDat,"Data");
    legend->AddEntry(hRec,"DY#rightarrow#mu#mu");
    TH1F*hMC = (TH1F*)hRec->Clone();

    int nBins = hRec->GetNbinsX();
    double x1 = hRec->GetBinLowEdge(1);
    double x2 = hRec->GetBinLowEdge(nBins);
    x2 += hRec->GetBinWidth(nBins);

    for(int i=0;i<nBackFiles;i++){
        TString load_name = prefix;
        TString back_name = "background";
        back_name += i;
        load_name += back_file_name.at(i);
        load_name += suffix;
        back_file.push_back(new TFile(load_name));
        back_hist.push_back((TH1D*)back_file.at(i)->Get(reco_hist));
        job_info = (TH1*)back_file.at(i)->Get("_job_info");
        back_file.at(i)->GetObject("_job_info_average", job_info_average);

        if(!job_info) cout << "job_info not loaded in file " << back_file_name.at(i) << endl;
        wsum = job_info->GetBinContent(2);
        xsec = (*job_info_average)[1];
        samplescale = lumi*xsec/wsum;
        back_hist.at(i)->Scale(samplescale);

        back_hist.at(i)->SetFillColor(hist_color.at(i));
        back_hist.at(i)->SetLineColor(hist_color.at(i));
        hStack->Add(back_hist.at(i));
        legend->AddEntry(back_hist.at(i),back_file_name.at(i));
        hMC->Add(back_hist.at(i));
    }
    hStack->Add(hRec);

    TLine*line = new TLine(x1,1,x2,1);
    line->SetLineColor(kRed);

    double ratioRange = 0.2;
    double upperBound = 1.0+ratioRange;
    double lowerBound = 1.0-ratioRange;

    TH1F*hRatio = (TH1F*)hDat->Clone("ratio");
    hRatio->Divide(hMC);
    hRatio->SetMarkerStyle(20);
    hRatio->SetMarkerColor(kBlack);
    hRatio->SetMinimum(lowerBound);
    hRatio->SetMaximum(upperBound);

    TCanvas*canvas = new TCanvas("canvas","",0,0,1000,1000);
    const float padmargins = 0.03;
    const float yAxisMinimum = 1;
    const float yAxisMaximum = 1e7;
    double ratioSplit = 0.20;
    TPad*pad1 = new TPad("","",0,ratioSplit,1.0,1.0);
    if(logplot) pad1->SetLogx();
    pad1->SetLogy();
    pad1->SetBottomMargin(padmargins);
    pad1->SetGrid();
    pad1->SetTicks(1,1);
    pad1->Draw();
    pad1->cd();

    hStack->Draw("hist");
    hStack->SetMinimum(yAxisMinimum);
    hStack->GetXaxis()->SetLabelSize(0);
    hStack->GetXaxis()->SetTitleSize(0);
    hDat->Draw("pe,same");
    legend->Draw("same");

    canvas->cd();
    TPad*pad2 = new TPad("","",0,0.05,1,ratioSplit);
    if(logplot)pad2->SetLogx();
    pad2->SetTopMargin(padmargins);
    pad2->SetBottomMargin(0.2);
    pad2->SetGrid();
    pad2->SetTicks(1,1);
    pad2->Draw();
    pad2->cd();
    hRatio->SetTitle("");
    hRatio->GetYaxis()->SetLabelSize(0.06);
    hRatio->GetYaxis()->SetTitleSize(0.08);
    hRatio->GetYaxis()->SetTitleOffset(0.3);
    hRatio->GetYaxis()->SetTitle("data/mc");
    hRatio->GetXaxis()->SetLabelSize(0.1);
    hRatio->GetXaxis()->SetTitleSize(0.11);
    hRatio->GetXaxis()->SetTitleOffset(0.8);
    hRatio->GetXaxis()->SetTitle("m_{#mu#mu} [GeV]");
    hRatio->GetXaxis()->SetNoExponent();
    hRatio->GetXaxis()->SetMoreLogLabels();
    hRatio->Draw("pe");
    line->Draw("same");

    TString saveName = save_loc;
    saveName += "dataVsMC_mass_new";
    saveName += ".png";
    canvas->SaveAs(saveName);
}

void MakeMigrationMatrixPlot()
{
    TH2D*hMatrix = (TH2D*)_hMat->Clone();
    TH1D*hGen = (TH1D*)_hGen->Clone();
    TH1D*hRec = (TH1D*)_hRec->Clone();

    double wsum, lumi, xsec;
    TH1*job_info;
    TVectorD*job_info_average = nullptr;
    double samplescale;
    lumi = 19520.0;
    job_info = (TH1*)mc_file->Get("_job_info");
    mc_file->GetObject("_job_info_average", job_info_average);

    wsum = job_info->GetBinContent(2);
    xsec = (*job_info_average)[1];
    samplescale = lumi*xsec/wsum;
    hGen->Scale(samplescale);
    hRec->Scale(samplescale);


    // Draw migration matrix
    TCanvas*c2 = MakeCanvas("c2");
    c2->SetRightMargin(0.15);
    c2->SetLeftMargin(0.15);
    hMatrix->Draw("colz");
    TString saveMatrix = save_loc;
    saveMatrix += "testMatrix.png";
    c2->SaveAs(saveMatrix);

    // Make x and y projections of migration matrix
    TH1D*hProjX = (TH1D*)hMatrix->ProjectionX();
    hProjX->SetLineColor(kBlue);
    TH1D*hProjY = (TH1D*)hMatrix->ProjectionY();
    hProjY->SetLineColor(kRed);

    TString saveProjectionsName = "testMassPlot_MatrixProjections.png";
    PlotProjections(hProjX,hProjY,saveProjectionsName);
}
