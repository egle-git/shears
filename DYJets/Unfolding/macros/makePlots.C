
TFile*load_file;
TString load_file_name = "unfolding_histograms.root";
TString load_unf_name = "unfolding_output.root";
TString reco_name = "reco_mass";
TString data_name = "data";
TString _directory;
TString _channel;
vector<TString> _back_file_name = {
    "TauTau",
    "ST_s-channel",
    "ST_t-channel_top",
    "ST_t-channel_antitop",
    "ST_tW_top",
    "ST_tW_antitop",
    "WWTo2L2Nu",
    "WZ",
    "ZZ",
    "TT",
    "GammaGamma",
};

TH1D*_hReco;
TH1D*_hData;
vector<TH1D*> _back_hists;
THStack*_hStack;
TLegend*_legend;
vector<TCanvas*> _canvas;
vector<TString> _plot_save;

int _nBackgrounds;
void OpenFiles();
void GetBackgrounds();
void GetSignal();
void SetHistProperties();
void PlotDataVsMC();
void MakeStackPlot();
void MakeLegend();
void SaveAll();

void makePlots(TString directory,TString channel)
{
    gROOT->SetBatch(true);
    gStyle->SetPalette(1);
    gStyle->SetOptStat(0);
    _directory = directory;
    _directory += "/";
    _directory += channel;
    _channel = channel;    

    OpenFiles();
    GetBackgrounds();
    GetSignal();
    
    SetHistProperties();
    PlotDataVsMC();

    SaveAll();
}

void PlotDataVsMC()
{
    MakeStackPlot();
    MakeLegend();

    TH1F*hMC = (TH1F*)_hReco->Clone();
    TH1D*hRec = (TH1D*)_hReco->Clone();
    TH1D*hDat = (TH1D*)_hData->Clone();

    for(int i=0;i<_nBackgrounds;i++){
        hMC->Add(_back_hists.at(i));
    }

    int nBinsX = hRec->GetNbinsX();
    double x1 = hRec->GetBinLowEdge(1);
    double x2 = hRec->GetBinLowEdge(nBinsX);
    x2 += hRec->GetBinWidth(nBinsX);

    TLine*line = new TLine(x1,1,x2,1);
    line->SetLineColor(kRed);

    double ratioRange = 0.3;
    double upperBound = 1.0+ratioRange;
    double lowerBound = 1.0-ratioRange;

    TH1F*hRatio = (TH1F*)hDat->Clone("ratio");
    hRatio->Divide(hMC);
    hRatio->SetMarkerStyle(20);
    hRatio->SetMarkerColor(kBlack);
    hRatio->SetMinimum(lowerBound);
    hRatio->SetMaximum(upperBound);

    TCanvas*c1 = new TCanvas("c1","",0,0,1400,1000);
    const float padmargins = 0.03;
    const float yAxisMinimum = 1;
    const float yAxisMaximum = 1e7;
    double ratioSplit = 0.20;
    TPad*pad1 = new TPad("","",0,ratioSplit,1.0,1.0);
    pad1->SetLogy();
    pad1->SetBottomMargin(padmargins);
    pad1->SetGrid();
    pad1->SetTicks(1,1);
    pad1->Draw();
    pad1->cd();

    _hStack->Draw("hist");
    _hStack->SetMinimum(yAxisMinimum);
    _hStack->GetXaxis()->SetLabelSize(0);
    _hStack->GetXaxis()->SetTitleSize(0);
    _hStack->GetYaxis()->SetTitle("number of events");
    hDat->Draw("pe,same");
    _legend->Draw("same");

    c1->cd();
    TPad*pad2 = new TPad("","",0,0.05,1,ratioSplit);
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
    hRatio->GetXaxis()->SetTitle("reco mass bin number");
    hRatio->GetXaxis()->SetNoExponent();
    hRatio->GetXaxis()->SetMoreLogLabels();
    hRatio->Draw("pe");
    line->Draw("same");

    TString saveName = "dataVsMC_";
    saveName += _channel;
    saveName += ".png";
    _canvas.push_back(c1);
    _plot_save.push_back(saveName);
}

void MakeLegend()
{
    _legend = new TLegend(0.8,0.9,0.9,0.7);
    _legend->SetTextSize(0.015);
    _legend->AddEntry(_hData,"Data");
    _legend->AddEntry(_hReco,"DY#rightarrow#mu#mu");
    for(int i=0;i<_nBackgrounds;i++){
        _legend->AddEntry(_back_hists.at(i),_back_file_name.at(i));
    } 
}

void MakeStackPlot()
{
    _hStack = new THStack("stack plot","");
    for(int i=0;i<_nBackgrounds;i++){
        _hStack->Add(_back_hists.at(i));
    }
    _hStack->Add(_hReco);
}

void SetHistProperties()
{
    _hData->SetMarkerStyle(20);
    _hData->SetMarkerColor(kBlack);
    _hData->SetLineColor(kBlack);

    _hReco->SetFillColor(kOrange-1);
    _hReco->SetLineColor(kOrange-1);
}

void GetSignal()
{
    _hReco = (TH1D*)load_file->Get(reco_name);
    _hData = (TH1D*)load_file->Get(data_name);
}

void GetBackgrounds()
{
    vector<int> hist_color = {
        kGreen+3,   // tautau
        kBlue+2,    // ST-s-channel
        kBlue+3,    // ST-t-channel_top
        kBlue+4,    // ST-t-channel_antitop
        kAzure+4,   // ST-tW
        kAzure+7,   // STbar-tW
        kRed+2,     // WWTo2L2Nu
        kRed+3,     // WZ
        kRed+4,     // ZZ
        kAzure,     // tt
        kRed+1,     // GammaGamma
    };

    _nBackgrounds = _back_file_name.size();
    for(int i=0;i<_nBackgrounds;i++){
        _back_hists.push_back((TH1D*)load_file->Get(_back_file_name.at(i)));
        _back_hists.at(i)->SetFillColor(hist_color.at(i));
        _back_hists.at(i)->SetLineColor(hist_color.at(i));
    }
}

void OpenFiles()
{
    TString open_file = _directory;
    open_file += "/";
    open_file += load_file_name;
    load_file = new TFile(open_file);
}

void SaveAll()
{
    TString plotSaveLocation = _directory;
    plotSaveLocation += "/plots/";
    int nPlots = _canvas.size();
    for(int i=0;i<nPlots;i++){
        _canvas.at(i)->SaveAs(plotSaveLocation+_plot_save.at(i));
    }

}
