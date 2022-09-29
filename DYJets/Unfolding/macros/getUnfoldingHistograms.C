#include "../include/Utilities.h"

void GetSignal();
void GetBackgrounds();
void GetMatrix();
void SaveAll();

TCanvas*MakeCanvas(TString cName);
TString mc_name     = "histograms/fromShears/dyjets-DYJets.root";
TString data_name   = "histograms/fromShears/dyjets-data.root";

TString save_loc    = "histograms/unfolding/unfolding_histograms.root";

TString reco_hist   = "mass_wide_range_inc0jet";
TString gen_hist    = "mass_wide_range_inc0jet-gen";
TString matrix_hist = reco_hist + "-matrix";

// data luminosity
double lumi = 19520.0;

TH1D*_hGen  = nullptr;
TH1D*_hRec  = nullptr;
TH1D*_hDat  = nullptr;
TH1D*_hBack = nullptr;
TH2D*_hMat  = nullptr;
vector<TH1D*> _back_hist;

void getUnfoldingHistograms()
{
    gStyle->SetOptStat(0);
    gStyle->SetPalette(1);
    gROOT->SetBatch(true);

    // Get histograms 
    GetSignal();
    GetBackgrounds();
    GetMatrix();

    SaveAll();
} 

void GetSignal()
{
    TFile*mc_file   = new TFile(mc_name);
    TFile*data_file = new TFile(data_name);

    _hGen = (TH1D*)mc_file->Get(gen_hist);
    _hGen->SetName("gen_mass");
    _hRec = (TH1D*)mc_file->Get(reco_hist);
    _hRec->SetName("reco_mass");
    _hDat = (TH1D*)data_file->Get(reco_hist);
    _hDat->SetName("data");

    double wsum, xsec;
    TH1*job_info = nullptr;
    TVectorD*job_info_average = nullptr;
    double samplescale;

    job_info = (TH1*)mc_file->Get("_job_info");
    mc_file->GetObject("_job_info_average", job_info_average);

    if(!job_info){
        cout << "job_info failed to load" << endl;
    }

    wsum = job_info->GetBinContent(2);
    xsec = (*job_info_average)[1];
    samplescale = lumi*xsec/wsum;
    
    _hGen->Scale(samplescale);
    _hRec->Scale(samplescale);

    if(!_hGen) cout << "ERROR: _hGen could not be loaded from file" << endl;
    if(!_hRec) cout << "ERROR: _hRec could not be loaded from file" << endl;
    if(!_hDat) cout << "ERROR: _hDat could not be loaded from file" << endl;
}

void GetBackgrounds()
{
    vector<TString> back_file_name = {
        "TauTau",
        "WJetsToLNu",
        "ST_s-channel",
        "ST_t-channel_top",
        "ST_t-channel_antitop",
        "ST_tW_top",
        "ST_tW_antitop",
        "WWTo2L2Nu",
        "WZ",
        "ZZ",
        "TT",
    };
    vector<int> hist_color = {
        kGreen+3,   // tautau
        kRed+1,     // WToLNu
        kBlue+2,    // ST-s-channel
        kBlue+3,    // ST-t-channel_top
        kBlue+4,    // ST-t-channel_antitop
        kAzure+4,   // ST-tW
        kAzure+7,   // STbar-tW
        kRed+2,     // WWTo2L2Nu
        kRed+3,     // WZ
        kRed+4,     // ZZ
        kAzure,     // tt
    };
    int nBackFiles = back_file_name.size();
    vector<TFile*> back_file;
    double wsum, xsec;

    for(int i=0;i<nBackFiles;i++){
        TString prefix = "histograms/fromShears/dyjets-";
        TString suffix = ".root";
        TH1*job_info;
        TVectorD*job_info_average = nullptr;
        double samplescale;

        TString load_name = prefix;
        load_name += back_file_name.at(i);
        load_name += suffix;
        back_file.push_back(new TFile(load_name));
        _back_hist.push_back((TH1D*)back_file.at(i)->Get(reco_hist));
        _back_hist.at(i)->SetName(back_file_name.at(i));

        job_info = (TH1*)back_file.at(i)->Get("_job_info");
        back_file.at(i)->GetObject("_job_info_average", job_info_average);

        wsum = job_info->GetBinContent(2);
        xsec = (*job_info_average)[1];
        samplescale = lumi*xsec/wsum;

        _back_hist.at(i)->Scale(samplescale);

        cout << "******************************" << endl;
        cout << "Sample: " << load_name << endl;
        cout << "wsum: " << wsum << endl;
        cout << "lumi: " << lumi << endl;
        cout << "xsec: " << xsec << endl;
        cout << "samplescale: " << samplescale << endl;
        cout << "******************************" << endl;

        if(i==0) _hBack = (TH1D*)_back_hist.at(0)->Clone("Backgrounds");
        else _hBack->Add(_back_hist.at(i));
    }// end loop over background files
    _hBack->SetName("backgrounds");
}

void GetMatrix()
{
    TFile*mc_file   = new TFile(mc_name);
    TH2D*hMatTmp = (TH2D*)mc_file->Get(matrix_hist);
    _hMat = Utilities::RebinTH2(hMatTmp,"hMatrix",_hGen,false);
    _hMat = (TH2D*)mc_file->Get(matrix_hist);
    
    if(!_hMat){
        cout << "ERROR: _hMat not properly loaded from file" << endl;
    }
    _hMat->SetName("matrix");

//    TCanvas*cMatrix = new TCanvas("cMatrix","",0,0,1000,1000);
//    cMatrix->SetGrid();
//    cMatrix->SetLogx();
//    cMatrix->SetLogy();
//    cMatrix->SetLogz();
//    _hMat->Draw("colz");
//    cMatrix->SaveAs("plots/testMatrix.png");
}

void SaveAll()
{
    TFile*save_file = new TFile(save_loc,"recreate");
    _hGen->Write();
    _hRec->Write(); 
    _hDat->Write(); 
    _hBack->Write();
    _hMat->Write(); 

    int nFiles = _back_hist.size();
    for(int i=0;i<nFiles;i++){
        _back_hist.at(i)->Write();;
    }

    save_file->Close();
}
