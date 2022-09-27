#include "../include/Utilities.h"

void HistOptions(TH1D*hist,int kColor);
void PlotProjections(TH1D*hProjX,TH1D*hProjY,TString saveName);
void MakeMCvsData(bool logplot);
void MakeMigrationMatrixPlot();

TCanvas*MakeCanvas(TString cName);
TString mc_name     = "histograms/fromShears/dyjets-DYJets.root";
TString data_name   = "histograms/fromShears/dyjets-data.root";

TString save_loc    = "plots/";

TString reco_hist   = "mass_wide_range_inc0jet";
TString gen_hist    = "mass_wide_range_inc0jet-gen";
TString matrix_hist = reco_hist + "-matrix";

TFile*mc_file = new TFile(mc_name);
TFile*data_file = new TFile(data_name);
double mcScale = 1.0;

TH1D*_hGen;
TH1D*_hRec;
TH1D*_hDat;

void makePlots()
{
    gStyle->SetOptStat(0);
    gStyle->SetPalette(1);
    gROOT->SetBatch(true);

    // Get histograms 
    _hGen = (TH1D*)mc_file->Get(gen_hist);
    HistOptions(_hGen,kRed);
    _hRec = (TH1D*)mc_file->Get(reco_hist);
    HistOptions(_hRec,kBlue);
    _hDat = (TH1D*)data_file->Get(reco_hist);

    MakeMCvsData(true);
    MakeMigrationMatrixPlot();
} 

void PlotProjections(TH1D*hProjXTmp,TH1D*hProjY,TString saveName)
{
    TH1D*_hRecRebinTmp = (TH1D*)_hRec->Clone();
    TH1D*_hRecRebin = Utilities::RebinTH1(_hRecRebin,"_hRecRebin",_hGen);
    TH1D*hProjX = Utilities::RebinTH1(hProjXTmp,"x-projection",_hGen);

    int nBins = _hRecRebin->GetNbinsX();
    double x1 = _hRecRebin->GetBinLowEdge(1);
    double x2 = _hRecRebin->GetBinLowEdge(nBins);
    x2 += _hRecRebin->GetBinWidth(nBins);
    TLegend*legend = new TLegend(0.65,0.9,0.9,0.75);
    legend->SetTextSize(0.02);
    legend->AddEntry(hGen,"gen");
    legend->AddEntry(hRec,"reco");
    legend->AddEntry(hProjX,"x-projection");
    legend->AddEntry(hProjY,"y-projection");

    int nBins = hRec->GetNbinsX();
    double x1 = hRec->GetBinLowEdge(1);
    double x2 = hRec->GetBinLowEdge(nBins);
    x2 += hRec->GetBinWidth(nBins);
    TLine*line = new TLine(x1,1,x2,1);
    line->SetLineColor(kBlack);

    double ratioRange = 0.2;
    double upperBound = 1.0-ratioRange;
    double lowerBound = 1.0+ratioRange;
    TH1F*hRatioTrue = (TH1F*)_hGen->Clone("trueRatio");
    hRatioTrue->Divide(hProjY);
    hRatioTrue->SetMarkerStyle(20);
    hRatioTrue->SetMarkerColor(kRed);
    hRatioTrue->SetMinimum(1.0-ratioRange);
    hRatioTrue->SetMaximum(1.0+ratioRange);

    TH1F*hRatioReco = (TH1F*)_hRecRebin->Clone("recoRatio");
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
    _hGen->Draw("pe,same");
    _hRecRebin->Draw("pe,same");
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
void HistOptions(TH1D*hist,int kColor)
{
    if(!hist){
        cout << "ERROR in HistOptions(TH1D*hist,int kColor)" << endl;
        cout << "This histogram does not exist: " << endl;
    }
    hist->SetTitle("dimuon invariant mass");

    hist->SetLineColor(kColor);
    hist->SetMarkerColor(kColor);
    hist->SetMarkerStyle(20);
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
    double wsum, lumi, xsec;
    TH1*job_info;
    TVectorD*job_info_average = nullptr;
    double samplescale = 1.0;
    lumi = 19520.0;

    TString prefix = "histograms/fromShears/dyjets-";
    TString suffix = ".root";
    TH1D*hDataRebin = Utilities::RebinTH1(_hDat,"dataRebin",_hGen);
    TH1D*hRecoRebin = Utilities::RebinTH1(_hRec,"dataRebin",_hGen);

    hDataRebin->SetMarkerStyle(20);
    hDataRebin->SetMarkerColor(kBlack);
    hDataRebin->SetLineColor(kBlack);
    hRecoRebin->SetFillColor(kOrange-1);
    hRecoRebin->SetLineColor(kOrange-1);
    hRecoRebin->SetMarkerColor(kOrange-1);
    hData->SetMarkerStyle(20);
    hData->SetMarkerColor(kBlack);
    hData->SetLineColor(kBlack);
    hReco->SetFillColor(kOrange-1);
//    hReco->SetLineColor(kOrange-1);

    job_info = (TH1*)mc_file->Get("_job_info");
    mc_file->GetObject("_job_info_average", job_info_average);

    wsum = job_info->GetBinContent(2);
    xsec = (*job_info_average)[1];
    samplescale = lumi*xsec/wsum;

    hReco->Scale(samplescale);

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
    vector<TH1D*> back_histTmp;
    vector<TH1D*> back_hist;
    THStack*hStack = new THStack("stack plot","");
    TLegend*legend = new TLegend(0.65,0.9,0.9,0.60);
    legend->SetTextSize(0.02);
    legend->AddEntry(hData,"Data");
    legend->AddEntry(hReco,"DY#rightarrow#mu#mu");
    TH1F*hMC = (TH1F*)hReco->Clone();

    int nBins = hReco->GetNbinsX();
    double x1 = hReco->GetBinLowEdge(1);
    double x2 = hReco->GetBinLowEdge(nBins);
    x2 += hReco->GetBinWidth(nBins);

    for(int i=0;i<nBackFiles;i++){
        TString load_name = prefix;
        TString back_name = "background";
        back_name += i;
        load_name += back_file_name.at(i);
        load_name += suffix;
        back_file.push_back(new TFile(load_name));
        back_histTmp.push_back((TH1D*)back_file.at(i)->Get(reco_hist));
        back_hist.push_back(Utilities::RebinTH1(back_histTmp.at(i),back_name,_hGen));
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
    hStack->Add(hReco);

    TLine*line = new TLine(x1,1,x2,1);
    line->SetLineColor(kRed);

    double ratioRange = 0.2;
    double upperBound = 1.0+ratioRange;
    double lowerBound = 1.0-ratioRange;

    TH1F*hRatio = (TH1F*)hData->Clone("ratio");
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
    hData->Draw("pe,same");
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
    _hGen->Scale(samplescale);
    _hRec->Scale(samplescale);

    // Get migration matrix plot
    TH2D*hMatTmp = (TH2D*)mc_file->Get(matrix_hist);
    TH2D*hMatrix = Utilities::RebinTH2(hMatTmp,"hMatrix",_hGen,true);;
    TH2D*hMatrix = (TH2D*)mc_file->Get(matrix_hist);
    hMatrix->Scale(mcScale);
    hMatrix->GetXaxis()->SetTitle("m_{reco} [GeV]");
    hMatrix->GetYaxis()->SetTitle("m_{gen} [GeV]");
    hMatrix->SetTitle("muon migration matrix");
    hMatrix->GetXaxis()->SetNoExponent();
    hMatrix->GetXaxis()->SetMoreLogLabels();
    hMatrix->GetYaxis()->SetNoExponent();
    hMatrix->GetYaxis()->SetMoreLogLabels();

    hMatrix->Scale(samplescale);
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
