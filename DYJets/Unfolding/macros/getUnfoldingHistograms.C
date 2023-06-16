#include "../include/Utilities.h"

void GetSignal();
void GetBackgrounds();
vector<TH1D*> GetBackgroundHists(TString era);
void SaveAll();
TH1D*GetDataHist();
TH1D*GetHistogram1D(TString era,TString hist_load,TString hist_name);
TH2D*GetHistogram2D(TString era,TString hist_load,TString hist_name);

TCanvas*MakeCanvas(TString cName);
TString _directory;
TString _channel;

vector<TString> _era = {
    "2016preAPV",
    "2016postAPV",
    "2017",
    "2018",
};
int _nEras = _era.size();
int _nBackFiles;

vector<TString> mc_name = {
    "dyjets-DYJets_M-10to50.root",
    "dyjets-DYJets_M-50to100.root",
    "dyjets-DYJets_M-100to200.root",
    "dyjets-DYJets_M-200to400.root",
    "dyjets-DYJets_M-400to500.root",
    "dyjets-DYJets_M-500to700.root",
    "dyjets-DYJets_M-700to800.root",
    "dyjets-DYJets_M-800to1000.root",
    "dyjets-DYJets_M-1000to1500.root",
    "dyjets-DYJets_M-1500to2000.root",
    "dyjets-DYJets_M-2000toInf.root",
};
vector<TString> mc_name17 = {
    "dyjets-DYJets_M-10to50.root",
    "dyjets-DYJets_M-50to100.root",
    "dyjets-DYJets_M-200to400.root",
    "dyjets-DYJets_M-400to500.root",
    "dyjets-DYJets_M-500to700.root",
    "dyjets-DYJets_M-700to800.root",
    "dyjets-DYJets_M-800to1000.root",
    "dyjets-DYJets_M-1000to1500.root",
    "dyjets-DYJets_M-1500to2000.root",
    "dyjets-DYJets_M-2000toInf.root",
};

TString data_name   = "dyjets-data.root";
TString save_name   = "unfolding_histograms.root";
TString reco_hist   = "TUnfold1DReco_inc0jet";
TString gen_hist    = "TUnfold1DTrue_inc0jet";
TString matrix_hist = "TUnfold2DMig_inc0jet";

// data luminosity
vector<double> _lumi = {
    19520.0, // 2016preAPV
    16810.0, // 2016postAPV
    41480.0, // 2017
    59830.0, // 2018
};
vector<double> sample_xsec_mm = {
    7012.53,        // 10to50
    1925.65144658,  // 50to100
    77.95,          // 100to200
    2.78,           // 200to400
    0.15,           // 400to500
    0.084,          // 500to700
    0.013,          // 700to800
    0.011,          // 800to1000
    0.0060,         // 1000to1500
    0.00081,        // 1500to2000
    0.00020,        // 2000toInf
};

TH1D*_hGen  = nullptr;
TH1D*_hRec  = nullptr;
TH1D*_hDat  = nullptr;
TH1D*_hBack = nullptr;
TH2D*_hMat  = nullptr;

vector<TH1D*> _gen_hist;
vector<TH1D*> _rec_hist;
vector<TH2D*> _mat_hist;
vector<TH1D*> _back_hist;

void getUnfoldingHistograms(TString directory,TString channel)
{
    TH1::SetDefaultSumw2();
    TH2::SetDefaultSumw2();
    gStyle->SetOptStat(0);
    gStyle->SetPalette(1);
    gROOT->SetBatch(true);

    cout << "******************************" << endl;
    cout << "Getting unfolding histograms from directory: " << endl;
    cout << directory << endl;
    cout << "For channel " << channel << endl;
    cout << "******************************" << endl;
    
    _directory = directory;
    _channel = channel;

    // Get histograms 
    GetSignal();
    GetBackgrounds();

    SaveAll();
} 

void GetSignal()
{
    _hDat = GetDataHist();

    TH1D*hGen16pre = GetHistogram1D("2016preAPV",gen_hist,"gen_mass_16pre");
    TH1D*hGen16post = GetHistogram1D("2016postAPV",gen_hist,"gen_mass_16post");
    TH1D*hGen17 = GetHistogram1D("2017",gen_hist,"gen_mass_17");
    TH1D*hGen18 = GetHistogram1D("2018",gen_hist,"gen_mass_18");

    TH1D*hRec16pre = GetHistogram1D("2016preAPV",reco_hist,"reco_mass_16pre");
    TH1D*hRec16post = GetHistogram1D("2016postAPV",reco_hist,"reco_mass_16post");
    TH1D*hRec17 = GetHistogram1D("2017",reco_hist,"reco_mass_17");
    TH1D*hRec18 = GetHistogram1D("2018",reco_hist,"reco_mass_18");

    TH2D*hMat16pre = GetHistogram2D("2016preAPV",matrix_hist,"matrix16pre");
    TH2D*hMat16post = GetHistogram2D("2016postAPV",matrix_hist,"matrix16post");
    TH2D*hMat17 = GetHistogram2D("2017",matrix_hist,"matrix17");
    TH2D*hMat18 = GetHistogram2D("2018",matrix_hist,"matrix18");

    _hGen = (TH1D*)hGen16pre->Clone("gen_mass");
    _hGen->Add(hGen16post);
    _hGen->Add(hGen17);
    _hGen->Add(hGen18);

    _hRec = (TH1D*)hRec16pre->Clone("reco_mass");
    _hRec->Add(hRec16post);
    _hRec->Add(hRec17);
    _hRec->Add(hRec18);

    _hMat = (TH2D*)hMat16pre->Clone("matrix");
    _hMat->Add(hMat16post);
    _hMat->Add(hMat17);
    _hMat->Add(hMat18);
}

void GetBackgrounds()
{
    // Get vectors of all backgrounds for each era
    vector<TH1D*> h2016preAPV = GetBackgroundHists("2016preAPV");
    vector<TH1D*> h2016postAPV = GetBackgroundHists("2016postAPV");
    vector<TH1D*> h2017 = GetBackgroundHists("2017");
    vector<TH1D*> h2018 = GetBackgroundHists("2018");

    // Combine eras
    for(int i=0;i<_nBackFiles;i++){ 
        _back_hist.push_back(h2016preAPV.at(i));
        _back_hist.at(i)->Add(h2016postAPV.at(i));
        _back_hist.at(i)->Add(h2017.at(i));
        _back_hist.at(i)->Add(h2018.at(i));

        if(i==0) _hBack = (TH1D*)_back_hist.at(i)->Clone("backgrounds");
        else _hBack->Add(_back_hist.at(i));
    }
}

vector<TH1D*> GetBackgroundHists(TString era)
{
    double lumi = -1;
    if(era=="2016preAPV") lumi = _lumi.at(0);
    else if(era=="2016postAPV") lumi = _lumi.at(1);
    else if(era=="2017") lumi = _lumi.at(2);
    else if(era=="2018") lumi = _lumi.at(3);
    if(lumi<0) cout << "era not properly defined for GetBackgroundHists()" << endl;
    vector<TString> back_file_name = {
        "TauTau",
        "ST_s-channel",
//        "ST_t-channel_top",
        "ST_t-channel_antitop",
        "ST_tW_top",
        "ST_tW_antitop",
        "WWTo2L2Nu",
        "WZ",
        "ZZ",
        "TT",
        "GammaGamma",
    };
    vector<int> hist_color = {
        kGreen+3,   // tautau
        kBlue+2,    // ST-s-channel
//        kBlue+3,    // ST-t-channel_top
        kBlue+4,    // ST-t-channel_antitop
        kAzure+4,   // ST-tW
        kAzure+7,   // STbar-tW
        kRed+2,     // WWTo2L2Nu
        kRed+3,     // WZ
        kRed+4,     // ZZ
        kAzure,     // tt
        kRed+1,     // GammaGamma
    };
    _nBackFiles = back_file_name.size();
    TFile*load_file;
    vector<TH1D*> back_hist;
    double wsum, xsec;

    for(int i=0;i<_nBackFiles;i++){
        TString prefix = _directory;
        prefix += "/";
        prefix += _channel;
        prefix += "/";
        prefix += era;
        prefix += "/";
        prefix += "/dyjets-";
        TString suffix = ".root";
        TH1*job_info;
        TVectorD*job_info_average = nullptr;
        double samplescale;

        TString load_name = prefix;
        load_name += back_file_name.at(i);
        load_name += suffix;
        load_file = new TFile(load_name);
        back_hist.push_back((TH1D*)load_file->Get(reco_hist));
        back_hist.at(i)->SetName(back_file_name.at(i));

        job_info = (TH1*)load_file->Get("_job_info");
        load_file->GetObject("_job_info_average", job_info_average);

        wsum = job_info->GetBinContent(2);
        xsec = (*job_info_average)[1];
        samplescale = lumi*xsec/wsum;

        back_hist.at(i)->Scale(samplescale);

    }// end loop over background files
    return back_hist;
}

void SaveAll()
{
    TString save_loc = _directory;
    save_loc += "/";
    save_loc += _channel;
    save_loc += "/";
    save_loc += save_name;
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

TH1D*GetHistogram1D(TString era,TString hist_load,TString hist_name)
{
    TH1D*hist;
    vector<TH1D*> histV;
    double wsum, xsec, lumi;
    TFile*mc_file;
    vector<TString> mcName = mc_name;
    if(era=="2016preAPV") lumi = _lumi.at(0);
    else if(era=="2016postAPV") lumi = _lumi.at(1);
    else if(era=="2017") lumi = _lumi.at(2);
    else if(era=="2018") lumi = _lumi.at(3);

    TH1*job_info = nullptr;
    TVectorD*job_info_average = nullptr;
    double samplescale;
    if(era=="2017" && _channel=="mm")mcName = mc_name17; 
    int nDYSamples = mcName.size();
    for(int i=0;i<nDYSamples;i++){
//        if(era=="2017" && i==2) continue;
        TString mcFileName = _directory;
        mcFileName += "/";
        mcFileName += _channel;
        mcFileName += "/";
        mcFileName += era;
        mcFileName += "/";
        mcFileName += mcName.at(i);
        cout << "Loading file: " << mcFileName << endl;

        mc_file   = new TFile(mcFileName);
        histV.push_back((TH1D*)mc_file->Get(hist_load));
        histV.at(i)->SetName(hist_name);

        job_info = (TH1*)mc_file->Get("_job_info");
        mc_file->GetObject("_job_info_average", job_info_average);
        wsum = job_info->GetBinContent(2);
        xsec = (*job_info_average)[1];

        samplescale = lumi*xsec/wsum;

        histV.at(i)->Scale(samplescale);

        if(i==0){
            hist = (TH1D*)histV.at(i)->Clone();
        }
        else{
            hist->Add(histV.at(i));
        }
    }// end loop over mass binned samples

    return hist;
}

TH2D*GetHistogram2D(TString era,TString hist_load,TString hist_name)
{
    TH2D*hist;
    vector<TH2D*> histV;
    double wsum, xsec, lumi;
    TFile*mc_file;
    
    vector<TString> mcName = mc_name;
    if(era=="2016preAPV") lumi = _lumi.at(0);
    else if(era=="2016postAPV") lumi = _lumi.at(1);
    else if(era=="2017") lumi = _lumi.at(2);
    else if(era=="2018") lumi = _lumi.at(3);

    TH1*job_info = nullptr;
    TVectorD*job_info_average = nullptr;
    double samplescale;
    if(era=="2017" && _channel=="mm")mcName = mc_name17; 

    int nDYSamples = mcName.size();
    for(int i=0;i<nDYSamples;i++){
//        if(era=="2017" && i ==2) continue;
        TString mcFileName = _directory;
        mcFileName += "/";
        mcFileName += _channel;
        mcFileName += "/";
        mcFileName += era;
        mcFileName += "/";
        mcFileName += mcName.at(i);
        cout << "Loading file: " << mcFileName << endl;

        mc_file   = new TFile(mcFileName);
        histV.push_back((TH2D*)mc_file->Get(hist_load));
        histV.at(i)->SetName(hist_name);

        job_info = (TH1*)mc_file->Get("_job_info");
        mc_file->GetObject("_job_info_average", job_info_average);
        wsum = job_info->GetBinContent(2);
        xsec = (*job_info_average)[1];

        samplescale = lumi*xsec/wsum;

        histV.at(i)->Scale(samplescale);

        if(i==0){
            hist = (TH2D*)histV.at(i)->Clone();
        }
        else{
            hist->Add(histV.at(i));
        }
    }// end loop over mass binned samples

    return hist;
}

TH1D*GetDataHist()
{
    TH1D*hist;
    TH1D*hDat;
    TString getData;
    for(int j=0;j<_nEras;j++){
        getData = _directory;
        getData += "/";
        getData += _channel;
        getData += "/";
        getData += _era.at(j);
        getData += "/";
        getData += data_name;

        TFile*data_file = new TFile(getData);

        hDat = (TH1D*)data_file->Get(reco_hist);
        hDat->SetName("data");
        if(j==0){
            hist = (TH1D*)hDat->Clone();
        }
        else{
            hist->Add(hDat);
        }
    }// end loop over eras
    return hist;
}
