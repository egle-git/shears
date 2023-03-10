#include "../include/Utilities.h"

void GetSignal();
void GetBackgrounds();
void GetMatrix();
void SaveAll();

TCanvas*MakeCanvas(TString cName);
TString _directory;
TString _getData;
TString _channel;
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
TString data_name   = "dyjets-data.root";
TString save_name   = "unfolding_histograms.root";
TString reco_hist   = "mass_wide_range_inc0jet";
TString gen_hist    = "mass_wide_range_inc0jet-gen";
TString matrix_hist = reco_hist + "-matrix";

// data luminosity
double lumi = 19520.0;
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

vector<TH1D*> _back_hist;
vector<TH1D*> _gen_hist;
vector<TH1D*> _rec_hist;

void getUnfoldingHistograms(TString directory,TString channel)
{
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

    _getData = _directory;
    _getData += "/";
    _getData += _channel;
    _getData += "/";
    _getData += data_name;

    // Get histograms 
    GetSignal();
    GetBackgrounds();
    GetMatrix();

    SaveAll();
} 

void GetSignal()
{
    TFile*mc_file;
    TFile*data_file = new TFile(_getData);

    double wsum, xsec;

    TH1*job_info = nullptr;
    TVectorD*job_info_average = nullptr;
    double samplescale;

    int nDYSamples = mc_name.size();
    for(int i=0;i<nDYSamples;i++){
        TString mcFileName = _directory;
        mcFileName += "/";
        mcFileName += _channel;
        mcFileName += "/";
        mcFileName += mc_name.at(i);
        mc_file   = new TFile(mcFileName);
        _gen_hist.push_back((TH1D*)mc_file->Get(gen_hist));
        _gen_hist.at(i)->SetName("gen_mass");
        _rec_hist.push_back((TH1D*)mc_file->Get(reco_hist));
        _rec_hist.at(i)->SetName("reco_mass");

        job_info = (TH1*)mc_file->Get("_job_info");
        mc_file->GetObject("_job_info_average", job_info_average);
        wsum = job_info->GetBinContent(2);
        xsec = (*job_info_average)[1];
        //xsec = sample_xsec_mm.at(i);
        samplescale = lumi*xsec/wsum;
        //samplescale = lumi*xsec;

        _gen_hist.at(i)->Scale(samplescale);
        _rec_hist.at(i)->Scale(samplescale);
        if(i==0){
            _hGen = (TH1D*)_gen_hist.at(i)->Clone();
            _hRec = (TH1D*)_rec_hist.at(i)->Clone();
        }
        else{
            _hGen->Add(_gen_hist.at(i));
            _hRec->Add(_rec_hist.at(i));
        }

        cout << "******************************" << endl;
        cout << "Sample: " << mc_name.at(i) << endl;
        cout << "wsum: " << wsum << endl;
        cout << "xsec: " << xsec << endl;
        cout << "samplescale: " << samplescale << endl;
        cout << "******************************" << endl;
    }// end loop over mass binned samples
    _hDat = (TH1D*)data_file->Get(reco_hist);
    _hDat->SetName("data");


    

    if(!_hGen) cout << "ERROR: _hGen could not be loaded from file" << endl;
    if(!_hRec) cout << "ERROR: _hRec could not be loaded from file" << endl;
    if(!_hDat) cout << "ERROR: _hDat could not be loaded from file" << endl;
}

void GetBackgrounds()
{
    vector<TString> back_file_name = {
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
    int nBackFiles = back_file_name.size();
    vector<TFile*> back_file;
    double wsum, xsec;

    for(int i=0;i<nBackFiles;i++){
        TString prefix = _directory;
        prefix += "/";
        prefix += _channel;
        prefix += "/dyjets-";
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
/*
        cout << "******************************" << endl;
        cout << "Sample: " << load_name << endl;
        cout << "wsum: " << wsum << endl;
        cout << "lumi: " << lumi << endl;
        cout << "xsec: " << xsec << endl;
        cout << "samplescale: " << samplescale << endl;
        cout << "******************************" << endl;
*/
        if(i==0) _hBack = (TH1D*)_back_hist.at(0)->Clone("Backgrounds");
        else _hBack->Add(_back_hist.at(i));
    }// end loop over background files
    _hBack->SetName("backgrounds");
}

void GetMatrix()
{
    TFile*mc_file;
    int nDYSamples = mc_name.size();
    for(int i=0;i<nDYSamples;i++){
        TString mcFileName = _directory;
        mcFileName += "/";
        mcFileName += _channel;
        mcFileName += "/";
        mcFileName += mc_name.at(i);
        mc_file   = new TFile(mcFileName);
        if(i==0){
            _hMat = (TH2D*)mc_file->Get(matrix_hist);
        }
        else{
            _hMat->Add((TH2D*)mc_file->Get(matrix_hist));
        }
    }
    if(!_hMat){
        cout << "ERROR: _hMat not properly loaded from file" << endl;
    }
    _hMat->SetName("matrix");
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
