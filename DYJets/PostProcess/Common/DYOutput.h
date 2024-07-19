#pragma once
#include "SimplePlotTools.h"

#define LUMI_16pre 19520.0
#define LUMI_16post 16810.0
#define LUMI_17 41480.0
#define LUMI_18 59830.0

#include <map>
#include <unordered_map> // -- faster than std::map in finding elements

namespace DYTool {
// -- process vs. vec_sampleTag
// -- commonly used in below classes (EraOutput & HistNameContainer)
std::map<TString, vector<TString>> map_tag = {
  { "data", {"data"} },
  { "DY", {"DYJets_M-10to50", "DYJets_M-50to100",  
           "DYJets_M-100to200", "DYJets_M-200to400", "DYJets_M-400to500", 
           "DYJets_M-500to700", "DYJets_M-700to800", "DYJets_M-800to1000", 
           "DYJets_M-1000to1500", "DYJets_M-1500to2000", "DYJets_M-2000toInf"} },
  { "VV", {"WWTo2L2Nu", "WZ", "ZZ"} },
  { "singleTop", {"ST_s-channel", "ST_t-channel_antitop", 
                  "ST_t-channel_top", "ST_tW_antitop", "ST_tW_top"} },
  // { "GG", {"GammaGamma"} },
  { "GG", {"GammaGamma", "GammaGamma_M-5to50"} },
  { "TauTau", {"TauTau"} },
  { "TT", {"TT"} }
};

};

// -- output from a sample
class SampleOutput {
public:
  // -- exmaple:
  // SampleOutput sampleOutput("DYJets_M-50to100", "./input/16pre", LUMI_16pre);
  SampleOutput(TString tag, TString basePath, Double_t lumi):
  tag_(tag), basePath_(basePath), lumi_(lumi) { 
    Init(); 
  }

  void Set_Lumi(Double_t lumi_new) { 
    lumi_ = lumi_new;
    // -- re-calc. the norm. factor with the new lumi.
    normFactor_ = (tag_ == "data") ? 1.0 : Get_NormFactor(fileName_);
  }

  void DoNormalization(Bool_t flag) { doNorm_ = flag; }

  TH1D* Get(TString histName) {
    auto iter = map_hist_.find(histName.Data());
    // -- if it was not called: get it from the root file
    if( iter == map_hist_.end() ) {
      TH1D* h = PlotTool::Get_Hist(fileName_, histName);
      if( doNorm_ ) h->Scale(normFactor_);
      map_hist_.insert( std::make_pair(histName.Data(), h) );
      return (TH1D*)h->Clone();
    }

    return (TH1D*)((iter->second)->Clone());
  }

  Bool_t DoesExist(TString histName) {
    std::unique_ptr<TFile> f_input( TFile::Open(fileName_) );
    Bool_t flag = f_input->Get(histName) != nullptr;
    f_input->Close();

    return flag;
  }

  TH2D* Get2D(TString histName2D) {
    auto iter = map_hist2D_.find(histName2D.Data());
    // -- if it was not called: get it from the root file
    if( iter == map_hist2D_.end() ) {
      TH2D* h2D = PlotTool::Get_Hist2D(fileName_, histName2D);
      if( doNorm_ ) h2D->Scale(normFactor_);
      map_hist2D_.insert( std::make_pair(histName2D.Data(), h2D) );
      return (TH2D*)h2D->Clone();
    }

    return (TH2D*)((iter->second)->Clone());
  }

  Double_t NormFactor() const { return normFactor_; }

private:
  TString tag_;
  TString basePath_;
  Double_t lumi_;

  TString fileName_;
  Bool_t doNorm_ = kTRUE; // -- default: do normalize
  Double_t normFactor_;

  std::unordered_map<std::string, TH1D*> map_hist_;
  std::unordered_map<std::string, TH2D*> map_hist2D_;

  void Init() {
    TH1::AddDirectory(kFALSE);
    fileName_ = TString::Format("%s/dyjets-%s.root", basePath_.Data(), tag_.Data());
    normFactor_ = (tag_ == "data") ? 1.0 : Get_NormFactor(fileName_);
  }

  Double_t Get_NormFactor(TString fileName) {
    TH1D* h_jobInfo = PlotTool::Get_Hist(fileName, "_job_info");
    Double_t sumWeight = h_jobInfo->GetBinContent(2);
    Double_t xSec = Get_XSec(fileName);

    // cout << "fileName = " << fileName << ", sum(weight) = " << sumWeight << endl;

    Double_t normFactor = (lumi_ * xSec) / sumWeight;

    return normFactor;
  }

  Double_t Get_XSec(TString fileName) {
    TVectorD* job_info_average = nullptr;
    TFile* f = TFile::Open(fileName);
    f->GetObject("_job_info_average", job_info_average);
    f->Close();

    return (*job_info_average)[1];
  }
};

// -- output from a physics process
// -- possible collection of samples e.g. DY = DY_m10to50 + DY_m50to100 + DY_m100to200 ...
class ProcessOutput {
public:
  // -- exmaple:
  // ProcessOutput processOutput("DY", {"DYJets_M-10to50", "DYJets_M-50to100"}, 
  //                             "./input/16pre", LUMI_16pre);
  ProcessOutput(TString process, vector<TString> vec_tag, TString basePath, Double_t lumi):
  process_(process), vec_tag_(vec_tag), basePath_(basePath), lumi_(lumi) {
    Init();
  }

  void Set_Lumi(Double_t lumi_new) {
    lumi_ = lumi_new;
    for(auto& pair : map_sampleOutput_ )
      pair.second.Set_Lumi(lumi_new);
  }

  // -- when the histName is same for all samples
  TH1D* Get(TString histName) {
    auto iter = map_hist_.find(histName.Data());
    // -- if it was not called: get it from the root file
    if( iter == map_hist_.end() ) {
      TH1D* h = Get_MergedHist(histName);
      map_hist_.insert( std::make_pair(histName.Data(), h) );
      return (TH1D*)h->Clone();
    }

    return (TH1D*)((iter->second)->Clone());
  }

  TH2D* Get2D(TString histName2D) {
    auto iter = map_hist2D_.find(histName2D.Data());
    // -- if it was not called: get it from the root file
    if( iter == map_hist2D_.end() ) {
      TH2D* h2D = Get_MergedHist2D(histName2D);
      map_hist2D_.insert( std::make_pair(histName2D.Data(), h2D) );
      return (TH2D*)h2D->Clone();
    }

    return (TH2D*)((iter->second)->Clone());
  }


  // -- when histName is customized for each sample
  TH1D* Get(const std::map<TString, TString>& map_histName) {
    TH1D* h = Get_MergedHist(map_histName);
    return (TH1D*)h->Clone();
  }

  TH2D* Get2D(const std::map<TString, TString>& map_histName) {
    TH2D* h = Get_MergedHist2D(map_histName);
    return (TH2D*)h->Clone();
  }

  Bool_t DoesExist(TString histName) {
    Bool_t flag = kFALSE;
    for(auto& pair : map_sampleOutput_ ) {
      if( pair.second.DoesExist(histName) ) { // -- one sample existence is enough
        flag = kTRUE;
        break;
      }
    }

    return flag;
  }

private:
  TString process_;
  vector<TString> vec_tag_;
  TString basePath_;
  Double_t lumi_;

  std::map<TString, SampleOutput> map_sampleOutput_;

  std::unordered_map<std::string, TH1D*> map_hist_;
  std::unordered_map<std::string, TH2D*> map_hist2D_;

  void Init() {
    for(const auto& tag: vec_tag_ )
      map_sampleOutput_.insert( std::make_pair(tag, SampleOutput(tag, basePath_, lumi_)) );
  }

  // -- when the histName is same for all samples
  TH1D* Get_MergedHist(TString histName) {
    TH1D* h_merged = nullptr;

    for( auto& pair : map_sampleOutput_ ) {

      if( !pair.second.DoesExist(histName) ) {
        printf("[ProcessOutput::Get_MergedHist] %s does not exist in %s ... skip\n", histName.Data(), pair.first.Data());
        continue;
      }

      TH1D* h_temp = pair.second.Get(histName);

      if( h_merged ) h_merged->Add( h_temp );
      else           h_merged = (TH1D*)h_temp->Clone();
    }

    if( !h_merged ) {
      throw std::runtime_error("[ProcessOutput::Get_MergedHist] merged histogram is not made for " + histName);
      // printf("[ProcessOutput::Get_MergedHist] no merged histogram is found ... return nullptr\n");
    }

    return h_merged;
  }

  TH2D* Get_MergedHist2D(TString histName2D) {
    TH2D* h_merged = nullptr;

    for( auto& pair : map_sampleOutput_ ) {
      TH2D* h_temp = pair.second.Get2D(histName2D);

      if( h_merged ) h_merged->Add( h_temp );
      else           h_merged = (TH2D*)h_temp->Clone();
    }

    return h_merged;
  }

  // -- when histName is customized for each sample
  TH1D* Get_MergedHist(const std::map<TString, TString>& map_histName) {
    TH1D* h_merged = nullptr;

    for( auto& pair : map_sampleOutput_ ) {
      TString tag = pair.first;

      auto iter = map_histName.find(tag);
      if( iter == map_histName.end() )
        throw std::invalid_argument("[ProcessOutput::Get_MergedHist] tag = " + tag + " is not found");

      TString histName = iter->second;
      TH1D* h_temp = pair.second.Get(histName);

      if( h_merged ) h_merged->Add( h_temp );
      else           h_merged = (TH1D*)h_temp->Clone();
    }

    return h_merged;
  }

  TH2D* Get_MergedHist2D(const std::map<TString, TString>& map_histName) {
    TH2D* h_merged = nullptr;

    for( auto& pair : map_sampleOutput_ ) {
      TString tag = pair.first;

      auto iter = map_histName.find(tag);
      if( iter == map_histName.end() )
        throw std::invalid_argument("[ProcessOutput::Get_MergedHist2D] tag = " + tag + " is not found");

      TString histName = iter->second;
      TH2D* h_temp = pair.second.Get2D(histName);

      if( h_merged ) h_merged->Add( h_temp );
      else           h_merged = (TH2D*)h_temp->Clone();
    }

    return h_merged;
  }
};

// -- all processes in an era
class EraOutput {
public:
  EraOutput() {}

  EraOutput(TString era, TString basePath): 
  era_(era), basePath_(basePath) { 
    Init();
  }

  void Set_Fake(TString fileName_fake, TString histName_fake) {
    hasFake_ = kTRUE;
    h_fake_ = PlotTool::Get_Hist(fileName_fake, histName_fake);
  }

  bool HasFake() {return hasFake_; };

  void Set_Lumi(Double_t lumi_new) { 
    lumi_ = lumi_new;
    for(auto& pair : map_processOutput_ )
      pair.second.Set_Lumi(lumi_);
  };

  Double_t Lumi() const { return lumi_; }

  // -- when histogram names are same for all samples in a process
  TH1D* Get(TString histName, TString process) {
    if( process == "fake" && hasFake_ ) 
      return h_fake_; // -- only mass distribution only

    if( process == "bkgMC" ) 
      return Get_BkgMC(histName);

    if( process == "bkgAll" )
      return Get_BkgAll(histName);

    auto iter = map_processOutput_.find(process.Data());

    if( iter == map_processOutput_.end() )
      throw std::invalid_argument("process = " + process + " doesn't exist");

    return iter->second.Get(histName);
  }

  TH2D* Get2D(TString histName, TString process) {
    auto iter = map_processOutput_.find(process.Data());

    if( iter == map_processOutput_.end() )
      throw std::invalid_argument("process = " + process + " doesn't exist");

    return iter->second.Get2D(histName);
  }

  // -- when the histogram names are customized for each sample in a process
  TH1D* Get(const std::map<TString, TString>& map_histName, TString process) {
    if( process == "fake" && hasFake_ ) 
      return h_fake_; // -- only mass distribution only

    if( process == "bkgMC" || process == "bkgAll" )
      throw std::invalid_argument("[EraOutput::Get] process = bkgMC or bkgAll is supported only when histName is same for all processes");

    auto iter = map_processOutput_.find(process.Data());

    if( iter == map_processOutput_.end() )
      throw std::invalid_argument("process = " + process + " doesn't exist");

    return iter->second.Get(map_histName);
  }

  TH2D* Get2D(const std::map<TString, TString> map_histName, TString process) {
    auto iter = map_processOutput_.find(process.Data());

    if( iter == map_processOutput_.end() )
      throw std::invalid_argument("process = " + process + " doesn't exist");

    return iter->second.Get2D(map_histName);
  }

  Bool_t DoesExist(TString histName, TString process)  {
    auto iter = map_processOutput_.find(process.Data());

    if( iter == map_processOutput_.end() )
      throw std::invalid_argument("process = " + process + " doesn't exist");

    return iter->second.DoesExist(histName);
  }

  TString Path() const { return basePath_; }

private:
  TString era_;
  TString basePath_;

  std::map<TString, vector<TString>> map_tag_ = DYTool::map_tag;

  std::unordered_map<std::string, ProcessOutput> map_processOutput_;

  Double_t lumi_ = 0;

  // -- fake: not from shears output; directly from a root file
  // -- only 'wide_mass_range_inc0jet' type is available
  Bool_t hasFake_ = kFALSE;
  TH1D* h_fake_ = nullptr;
  TH1D* h_bkgAll_ = nullptr; // bkgMC + fake

  // -- sum of bkg. MC histograms
  std::unordered_map<std::string, TH1D*> map_bkgMCHist_;

  void Init() {
    if( era_ == "16pre" )  lumi_ = LUMI_16pre;
    if( era_ == "16post" ) lumi_ = LUMI_16post;
    if( era_ == "17" )     lumi_ = LUMI_17;
    if( era_ == "18" )     lumi_ = LUMI_18;
    if( lumi_ == 0 )
      throw std::invalid_argument("era_ = " + era_ +" is not proper value!");

    Init_ProcessOutput();
  }

  void Init_ProcessOutput() {
    for(const auto& pair : map_tag_) {
      TString process = pair.first;
      vector<TString> vec_tag = pair.second;

      ProcessOutput processOutput(process, vec_tag, basePath_, lumi_);
      map_processOutput_.insert( std::make_pair(process.Data(), processOutput) );
    }
  }

  TH1D* Get_BkgMC(TString histName) {
    auto iter = map_bkgMCHist_.find(histName.Data());

    // -- if not found: create
    if( iter == map_bkgMCHist_.end() ) {

      TH1D* h_bkgMC = nullptr;
      for( const auto& pair : map_tag_ ) {
        TString bkgProcess = pair.first;

        if( bkgProcess == "data" ) continue;
        if( bkgProcess == "DY" ) continue;

        if( h_bkgMC == nullptr ) h_bkgMC = Get(histName, bkgProcess);
        else                     h_bkgMC->Add( Get(histName, bkgProcess) );
      }

      map_bkgMCHist_.insert( std::make_pair(histName, h_bkgMC) );
      return h_bkgMC;
    }

    return (TH1D*)((iter->second)->Clone());
  }

  TH1D* Get_BkgAll(TString histName) {
    if( !hasFake_ )
      throw std::invalid_argument("no input fake histogram");
    if( histName != "mass_wide_range_inc0jet" )
      throw std::invalid_argument("no fake histogram for histName = " + histName);

    // -- if not made before: create
    if( h_bkgAll_ == nullptr ) {
      TH1D* h_bkgMC = Get_BkgMC(histName);
      h_bkgAll_ = (TH1D*)h_bkgMC->Clone();
      h_bkgAll_->Add( h_fake_ );
    }

    return h_bkgAll_;
  }
};

// -- all eras (16pre, 16post, 17 and 18)
class Run2Output {
public:
  Run2Output() {}

  Run2Output(TString commonPath): commonPath_(commonPath) {
    Init();
  }

  EraOutput& Get_EraOutput(TString era) {
    auto iter = map_eraOutput_.find(era.Data());
    if( iter == map_eraOutput_.end() )
      throw std::invalid_argument("[Run2Output::Get_EraOutput] era = " + era +" does not exist in the map");

    return iter->second;
  }

  Double_t TotalLumi() {
    Double_t totalLumi = 0;
    for(const auto& pair : map_eraOutput_)
      totalLumi += pair.second.Lumi();

    return totalLumi;
  }

private:
  TString commonPath_;
  std::unordered_map<std::string, EraOutput> map_eraOutput_;

  void Init() {
    vector<TString> vec_era = {"16pre", "16post", "17", "18"};
    for(const auto& era : vec_era) {
      // -- The .root file should be under the sub-directory with the name of era
      // -- (e.g. "16pre" directory under commonPath)
      TString basePath = TString::Format("%s/%s", commonPath_.Data(), era.Data());
      EraOutput eraOutput(era, basePath);
      map_eraOutput_.insert( std::make_pair(era.Data(), eraOutput) );
    }
  }
};


// -- result: essential results (histograms) for the DY measurement
// ---- kind of subset of Output: extraction of DY measurement-related histograms
// ------ 1D distribution for data, signal MC, background, response matrix, etc
// ---- directly get them from Output classes

// class DYEraResult {

// public:
//   DYEraResult(EraOutput& output): output_(output) {

//   }

// private:
//   EraOutput& output_;

// };

// -- helper class for DYRun2Result
// -- holds all histogram names in an era for the DY result
// -- possible to change individual histograms (useful when the systematic variation is calculated)
class HistNameContainer {
public:
  HistNameContainer() { 
    Init(); 
  }

  // -- fake histogram: could be from a separate file
  void Set_Fake(TString fileName, TString histName) {
    hasFake_ = kTRUE;
    fileName_fake_ = fileName;
    histName_fake_ = histName;
  }

  Bool_t HasFake() const { return hasFake_; }

  void Get_Fake(TString& fileName, TString& histName) const {
    if( !hasFake_ )
      throw std::invalid_argument("fake histogram was not provided");

    fileName = fileName_fake_;
    histName = histName_fake_;
  }

  // -- when all histogram names are same for the given histType
  void Customize(TString histType, TString histName) {
    // -- change all histogram names for bkg. MC processes
    if( histType == "reco_bkgMC" ) {
      vector<TString> vec_bkgMCProcess = Get_BkgMCProcess();
      for(const auto& process : vec_bkgMCProcess ) {
        TString histType_bkgProcess = "reco_"+process;
        Customize(histType_bkgProcess, histName);
      }
      return;
    }
    
    auto iter = map_histNameMap_ .find(histType);
    if( iter == map_histNameMap_ .end() )
      throw std::invalid_argument("[HistNameContainer::Customize] histType = " + histType + " is not supported");

    // -- change all sub-sample's histogram name with the given name
    std::map<TString, TString>& map_histName = iter->second;
    for(auto& pair : map_histName )
      pair.second = histName;
  }

  // -- when individual histogram name is cusotmized for a given histType
  // -- e.g. when you want to change the histogram name of ST_s-channel sample in reco_singleTop:
  // --> Customize("reco_singleTop", "ST_s-channel", "histName_new");
  void Customize(TString histType, TString tag, TString histName) {    
    auto iter = map_histNameMap_ .find(histType);
    if( iter == map_histNameMap_ .end() )
      throw std::invalid_argument("[HistNameContainer::Customize] histType = " + histType + " is not supported");

    std::map<TString, TString>& map_histName = iter->second;
    auto iter2 = map_histName.find(tag);
    if( iter2 == map_histName.end() )
      throw std::invalid_argument("[HistNameContainer::Customize] tag = " + tag + " is not in histType = "+histType);

    iter2->second = histName;
  }

  std::map<TString, TString> Get(TString histType) const {
    auto iter = map_histNameMap_ .find(histType);
    if( iter == map_histNameMap_ .end() )
      throw std::invalid_argument("histType = " + histType + " is not supported");

    return iter->second;
  }

  vector<TString> Get_BkgMCProcess() const {
    vector<TString> vec_bkgMCProcess;

    for(const auto& pair : map_histNameMap_  ) {
      TString histType = pair.first;

      if( histType.Contains("reco_") ) {
        TString process = histType.ReplaceAll("reco_", "");
        if( !process.Contains("data") && !process.Contains("DY") )
          vec_bkgMCProcess.push_back(process);
      }
    }
    return vec_bkgMCProcess;
  }

private:
  Bool_t hasFake_ = kFALSE;
  TString fileName_fake_ = "";
  TString histName_fake_ = "";

  // std::map<TString, TString> map_histName_  = {
  //   { "gen_DY", "TUnfold1DTrue_inc0jet" },
  //   { "reco_data"     , "TUnfold1DReco_inc0jet" },
  //   { "reco_DY"       , "TUnfold1DReco_inc0jet" },
  //   { "reco_VV"       , "TUnfold1DReco_inc0jet" },
  //   { "reco_singleTop", "TUnfold1DReco_inc0jet" },
  //   { "reco_GG"       , "TUnfold1DReco_inc0jet" },
  //   { "reco_TauTau"   , "TUnfold1DReco_inc0jet" },
  //   { "reco_TT"       , "TUnfold1DReco_inc0jet" },
  //   { "migM", "TUnfold2DMig_inc0jet" }
  // };

  std::map<TString, std::map<TString, TString>> map_histNameMap_;

  void Init() {
    std::map<TString, vector<TString>> map_tag = DYTool::map_tag;

    map_histNameMap_.insert( std::make_pair("gen_DY", Init_Map_PerProcess(map_tag["DY"], "TUnfold1DTrue_inc0jet")) );

    map_histNameMap_.insert( std::make_pair("reco_data",      Init_Map_PerProcess(map_tag["data"],      "TUnfold1DReco_inc0jet")) );
    map_histNameMap_.insert( std::make_pair("reco_DY",        Init_Map_PerProcess(map_tag["DY"],        "TUnfold1DReco_inc0jet")) );
    map_histNameMap_.insert( std::make_pair("reco_TT",        Init_Map_PerProcess(map_tag["TT"],        "TUnfold1DReco_inc0jet")) );
    map_histNameMap_.insert( std::make_pair("reco_VV",        Init_Map_PerProcess(map_tag["VV"],        "TUnfold1DReco_inc0jet")) );
    map_histNameMap_.insert( std::make_pair("reco_GG",        Init_Map_PerProcess(map_tag["GG"],        "TUnfold1DReco_inc0jet")) );
    map_histNameMap_.insert( std::make_pair("reco_TauTau",    Init_Map_PerProcess(map_tag["TauTau"],    "TUnfold1DReco_inc0jet")) );
    map_histNameMap_.insert( std::make_pair("reco_singleTop", Init_Map_PerProcess(map_tag["singleTop"], "TUnfold1DReco_inc0jet")) );

    map_histNameMap_.insert( std::make_pair("migM", Init_Map_PerProcess(map_tag["DY"], "TUnfold2DMig_inc0jet")) );
  }

  // -- e.g. Init_Map_PerProcess(map_tag["VV"], "TUnfold1DReco_inc0jet") 
  // --> output:
  // { {"WWTo2L2Nu", "TUnfold1DReco_inc0jet"},
  //   {"WZ",        "TUnfold1DReco_inc0jet"},
  //   {"ZZ",        "TUnfold1DReco_inc0jet"} }
  std::map<TString, TString> Init_Map_PerProcess(vector<TString> vec_tag, TString histName_init) {
    std::map<TString, TString> map_hName;
    for(const auto& tag: vec_tag)
      map_hName.insert( std::make_pair(tag, histName_init) );

    return map_hName;
  }
};


// -- full Run-2 result of a channel
class DYRun2Result {
public:
  DYRun2Result(): output_(nullptr) {}

  DYRun2Result(Run2Output* output): output_(output) { }

  DYRun2Result(TString path) { output_ = new Run2Output(path); }

  void Set_Fake(TString era, TString fileName, TString histName) {
    if( era == "all" ) {
      for(auto& pair : map_hNameC_ )
        pair.second.Set_Fake(fileName, histName);

      return;
    }

    auto iter = map_hNameC_.find(era);
    if( iter == map_hNameC_.end() )
      throw std::invalid_argument("[Set_Fake] era = " + era + " is not supported");

    iter->second.Set_Fake(fileName, histName);
  }

  // -- remove negative bins in the fake histogram
  void Remove_NegativeBin_Fake(Bool_t flag = kTRUE) { removeNegativeBinFakeLep_ = flag; }

  void Set_Acc(TString fileName, TString histName) {
    hasAcc_ = kTRUE;
    h_acc_ = PlotTool::Get_Hist(fileName, histName);
  }

  // -- when all histogram names are same for a given histType
  void Update_HistName(TString era, TString histType, TString histName_new) {
    if( era == "all" ) {
      for(auto& pair : map_hNameC_ )
        pair.second.Customize(histType, histName_new);

      return;
    }

    auto iter = map_hNameC_.find(era);
    if( iter == map_hNameC_.end() )
      throw std::invalid_argument("[Set_Fake] era = " + era + " is not supported");

    iter->second.Customize(histType, histName_new);
  }

  // -- when the histogram name is customized for a specific sample in a given histType
  // -- e.g. when you want to change the histogram name of ST_s-channel sample in reco_singleTop:
  // --> Update_HistName("all", "reco_singleTop", "ST_s-channel", "histName_new");
  void Update_HistName(TString era, TString histType, TString tag, TString histName_new) {
    if( era == "all" ) {
      for(auto& pair : map_hNameC_ )
        pair.second.Customize(histType, tag, histName_new);

      return;
    }

    auto iter = map_hNameC_.find(era);
    if( iter == map_hNameC_.end() )
      throw std::invalid_argument("[Set_Fake] era = " + era + " is not supported");

    iter->second.Customize(histType, tag, histName_new);
  }

  // -- change the luminosity (e.g. for systematic variations)
  void Update_Lumi(TString era, Double_t lumi_new) {
    output_->Get_EraOutput(era).Set_Lumi(lumi_new);
  }

  Double_t Run2Lumi() {
    return output_->TotalLumi();
  }

  // -- when you want to use a pre-defined histogram for the result
  // -- useful for tests/investigations
  void Add_CustomAllEraHist(TString level, TString process, TH1D* h_allEra) {
    if( isSetup_ )
      throw std::invalid_argument("[DYRun2Result::Add_CustomAllEraHist] this can be called only before the result is not produced (i.e. before Produce() is called)");

    TString histType = level+"_"+process;
    map_allEraHist_.insert( std::make_pair(histType.Data(), h_allEra) );
  }

  void Add_CustomAllEraMigM(TH2D* h_allEra_migM) {
    if( isSetup_ )
      throw std::invalid_argument("[DYRun2Result::Add_CustomAllEraMigM] this can be called only before the result is not produced (i.e. before Produce() is called)");

    h_allEra_migM_ = (TH2D*)h_allEra_migM->Clone();
  }

  void Produce() {
    isSetup_ = kTRUE;

    // -- hasFake_fullRun2_ is true only if all eras have fake histograms
    for(const auto& pair : map_hNameC_ ) {
      if( !pair.second.HasFake() ) {
        hasFake_fullRun2_ = kFALSE;
        break;
      }
      else
        hasFake_fullRun2_ = kTRUE;
    }

    // -- for closure test
    Insert_AllEraHist("gen", "DY");
    Insert_AllEraHist("reco", "DY");

    // -- for actual data results
    Insert_AllEraHist("reco", "data");
    // -- individual bkg. MCs
    for( const auto& pair : DYTool::map_tag ) {
      TString process = pair.first;
      if( process == "data" || process == "DY" ) continue;
      Insert_AllEraHist("reco", process);
    }
    // -- sum of bkg. MCs    
    Insert_AllEraHist_BkgMC();

    if( hasFake_fullRun2_ ) {
      Insert_AllEraHist_Fake();
      Insert_AllEraHist("reco", "bkgAll");
    }

    Insert_AllEraHist("reco", "data_bkgSub");
    Insert_MigM();

    Insert_AllEraHist_Unfolded("DY");
    Insert_AllEraHist_Unfolded("data");

    if( hasAcc_ ) {
      Insert_AllEraHist_Unfolded_FPS("DY");
      Insert_AllEraHist_Unfolded_FPS("data");
    }
  }

  TH1D* Get_AllEra(TString level, TString process) const {
    TString histType = TString::Format("%s_%s", level.Data(), process.Data());
    auto iter = map_allEraHist_.find(histType.Data());

    if( iter == map_allEraHist_.end() )
      throw std::invalid_argument("histType = " + histType + " doesn't exist in map_allEraHist_");

    return (TH1D*)((iter->second)->Clone());
  }

  TH2D* Get_AllEra_MigM() const { return h_allEra_migM_; }
  TH2D* Get_AllEra_RespM() const { return h_allEra_respM_; }

  Run2Output* Get_Run2Output() const { return output_; }

  Bool_t HasFPS() const { return hasAcc_; }

  Bool_t HasFake() const { 
    if( !isSetup_ )
      throw std::runtime_error("[DYRun2Result::HasFake] Produce() is not called yet!");

    return hasFake_fullRun2_; 
  } 

  void Save(TFile* f_output, TString tag = "") {
    f_output->cd();

    // -- write
    for( auto& pair : map_allEraHist_ ) {
      TString histType = pair.first;
      TString histName = tag != "" ? 
        TString::Format("h_allEra_%s_%s", histType.Data(), tag.Data()) :
        TString::Format("h_allEra_%s", histType.Data());

      TH1D* h_allEra = pair.second;
      h_allEra->SetName(histName);
      h_allEra->Write();
    }

    if( tag == "" ) h_allEra_migM_->SetName("h_allEra_migM");
    else            h_allEra_migM_->SetName("h_allEra_migM_"+tag);
    h_allEra_migM_->Write();

    if( tag == "" ) h_allEra_respM_->SetName("h_allEra_respM");
    else            h_allEra_respM_->SetName("h_allEra_respM_"+tag);
    h_allEra_respM_->Write();
  } 

  std::unordered_map<std::string, TH1D*> Get_Map_AllEraHist() const { return map_allEraHist_; }


protected:
  // -- declare as a reference: problem when it was not initialized -> decide to change to a pointer
  Run2Output* output_ = nullptr;

  Bool_t isSetup_ = kFALSE;

  Bool_t hasFake_fullRun2_ = kFALSE;

  Bool_t removeNegativeBinFakeLep_ = kTRUE; // -- default: remove negative bin

  std::map<TString, HistNameContainer> map_hNameC_ = {
    {"16pre",  HistNameContainer()},
    {"16post", HistNameContainer()},
    {"17",     HistNameContainer()},
    {"18",     HistNameContainer()}
  };

  std::unordered_map<std::string, TH1D*> map_allEraHist_;

  TH2D* h_allEra_migM_ = nullptr; 
  TH2D* h_allEra_respM_ = nullptr; // -- normalized

  // -- has acceptance? (if it has, it will also produce the full phase space results)
  Bool_t hasAcc_ = kFALSE;
  TH1D* h_acc_ = nullptr;

  void Insert_AllEraHist(TString level, TString process) {
    TString histType = level+"_"+process;

    if( map_allEraHist_.find(histType.Data()) != map_allEraHist_.end() ) {
      cout << "[DYRun2Result::Insert_AllEraHist] (level, process) = (" + level + ", " + process + "): custom histogram is already provided ... do not produce" << endl; 
      return;
    }

    if( level == "reco" && process == "bkgAll" && hasFake_fullRun2_ ) {
      TH1D* h_allEra_bkgMC = map_allEraHist_["reco_bkgMC"];
      TH1D* h_allEra_fake  = map_allEraHist_["reco_fake"];
      TH1D* h_allEra_bkgAll = (TH1D*)h_allEra_bkgMC->Clone();
      h_allEra_bkgAll->Add( h_allEra_fake );
      map_allEraHist_.insert( std::make_pair("reco_bkgAll", h_allEra_bkgAll) );
    }
    else if( level == "reco" && process == "data_bkgSub" ) {
      TH1D* h_allEra_data = map_allEraHist_["reco_data"];
      TH1D* h_allEra_bkg = hasFake_fullRun2_ ? 
                           map_allEraHist_["reco_bkgAll"] : map_allEraHist_["reco_bkgMC"];
      TH1D* h_allEra_data_bkgSub = (TH1D*)h_allEra_data->Clone();
      h_allEra_data_bkgSub->Add( h_allEra_bkg, -1.0 );
      map_allEraHist_.insert( std::make_pair("reco_data_bkgSub", h_allEra_data_bkgSub) );
    }
    else {
      TH1D* h_allEra = Make_AllEraHist(level, process);
      map_allEraHist_.insert( std::make_pair(histType.Data(), h_allEra) );
    }
  }

  TH1D* Make_AllEraHist(TString level, TString process) {
    TString histType = level+"_"+process;

    std::map<TString, TString> map_histName_16pre  = map_hNameC_["16pre"].Get(histType);
    std::map<TString, TString> map_histName_16post = map_hNameC_["16post"].Get(histType);
    std::map<TString, TString> map_histName_17     = map_hNameC_["17"].Get(histType);
    std::map<TString, TString> map_histName_18     = map_hNameC_["18"].Get(histType);

    TH1D* h_16pre  = output_->Get_EraOutput("16pre" ).Get(map_histName_16pre,  process);
    TH1D* h_16post = output_->Get_EraOutput("16post").Get(map_histName_16post, process);
    TH1D* h_17     = output_->Get_EraOutput("17"    ).Get(map_histName_17,     process);
    TH1D* h_18     = output_->Get_EraOutput("18"    ).Get(map_histName_18,     process);

    return MergeHist( {h_16pre, h_16post, h_17, h_18} );
  }

  void Insert_MigM() {
    if( h_allEra_migM_ == nullptr ) {
      std::map<TString, TString> map_histName_16pre  = map_hNameC_["16pre"].Get("migM");
      std::map<TString, TString> map_histName_16post = map_hNameC_["16post"].Get("migM");
      std::map<TString, TString> map_histName_17     = map_hNameC_["17"].Get("migM");
      std::map<TString, TString> map_histName_18     = map_hNameC_["18"].Get("migM");

      TH2D* h_16pre  = output_->Get_EraOutput("16pre" ).Get2D(map_histName_16pre,  "DY");
      TH2D* h_16post = output_->Get_EraOutput("16post").Get2D(map_histName_16post, "DY");
      TH2D* h_17     = output_->Get_EraOutput("17"    ).Get2D(map_histName_17,     "DY");
      TH2D* h_18     = output_->Get_EraOutput("18"    ).Get2D(map_histName_18,     "DY");

      h_allEra_migM_  = MergeHist2D( {h_16pre, h_16post, h_17, h_18} );
    }
    else
      cout << "[DYRun2Result::Insert_MigM] Custom migration matrix is already provided ... do not produce" << endl;

    // -- get "DYFake" events from the migration matrix (true underflow)
    TH1D* h_underflow_true = MakeHist_Underflow("true", h_allEra_migM_);
    map_allEraHist_.insert( std::make_pair("reco_DYFake", h_underflow_true) );

    // -- remove all true underflow
    // -- following the manual: https://root.cern.ch/doc/master/classTUnfold.html#a0ca33a31861aa9583000a41c1eceb106
    Clean_TrueUnderflow(h_allEra_migM_);
    // -- check there is no true "over"flow (it should be in our case)
    Check_TrueOverflow(h_allEra_migM_);

    h_allEra_respM_ = Normalize_RespM(h_allEra_migM_);

    // -- consistency check
    ConsistencyCheck_NumberOfBins();
  }

  TH1D* MakeHist_Underflow(TString type, TH2D* h2D) {
    TH1D* h_return = nullptr;
    if( type == "reco" )      h_return = h2D->ProjectionY();
    else if( type == "true" ) h_return = h2D->ProjectionX();
    else
      throw std::invalid_argument("[MakeHist_Underflow] type = " + type + " is not supported");

    h_return->Reset("ICES");

    Int_t nBin = (type == "reco") ? h2D->GetNbinsY() : h2D->GetNbinsX();

    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;

      Double_t value = (type == "reco") ? h2D->GetBinContent(0, i_bin) : h2D->GetBinContent(i_bin, 0);
      Double_t error = (type == "reco") ? h2D->GetBinError(0, i_bin)   : h2D->GetBinError(i_bin, 0);

      h_return->SetBinContent(i_bin, value);
      h_return->SetBinError(i_bin, error);
    }

    return h_return;
  }

  void Clean_TrueUnderflow(TH2D* h_migM) {
    Int_t nBin = h_migM->GetNbinsX();

    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;

      h_migM->SetBinContent(i_bin, 0);
      h_migM->SetBinError(i_bin, 0);
    }
  }

  void Check_TrueOverflow(TH2D* h_migM) {
    Bool_t isFound = kFALSE;

    Int_t nBin = h_migM->GetNbinsX();
    Int_t nBin_true = h_migM->GetNbinsY();
    Int_t i_trueBin_OF = nBin_true+1;

    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;

      Double_t value = h_migM->GetBinContent(i_bin, i_trueBin_OF);
      if( value != 0 ) {
        isFound = kTRUE;
        printf("[DYRun2Result::Check_TrueOverflow] true overflow is not zero: (%d, %d) --> value = %lf\n", i_bin, i_trueBin_OF, value);
      }
    }

    if( !isFound )
      cout << "[DYRun2Result::Check_TrueOverflow] No entry is found in the true overflow" << endl;
    else
      throw std::invalid_argument("[DYRun2Result::Check_TrueOverflow] At least an entry is found in the true overflow ... need to check");
  }

  TH2D* Normalize_RespM(TH2D* h) {
    TH2D* h_norm = (TH2D*)h->Clone();

    for(Int_t i_y=0; i_y<h->GetNbinsY(); i_y++) {
      Int_t i_binY = i_y+1;

      Double_t sum_trueBin = 0;

      // -- for a given true bin, calculate sum of events
      for(Int_t i_x=0; i_x<h->GetNbinsX(); i_x++) {
        Int_t i_binX = i_x+1;
        sum_trueBin += h->GetBinContent(i_binX, i_binY);
      }
      // sum_trueBin += h->GetBinContent(0, i_binY); // -- reco underflow: does not count (it is mixture of low mass event + not reco'ed event)
      sum_trueBin += h->GetBinContent(h->GetNbinsX()+1, i_binY); // -- reco overflow

      // printf("[%02d true bin] sum_trueBin = %lf\n", i_binY, sum_trueBin);

      // -- loop again: normalize 
      for(Int_t i_x=0; i_x<h->GetNbinsX(); i_x++) {
        Int_t i_binX = i_x+1;

        Double_t content = h->GetBinContent(i_binX, i_binY);
        Double_t error   = h->GetBinError(i_binX, i_binY);

        h_norm->SetBinContent(i_binX, i_binY, content/sum_trueBin );
        h_norm->SetBinError(i_binX, i_binY, error/sum_trueBin );

        // if( i_binX == i_binY+2 ) {
        //   printf("[%d, %d] value = %lf / %lf = %lf\n", i_binX, i_binY, content, sum_trueBin, content/sum_trueBin);
        // }
      }
    }

    return h_norm;
  }

  void ConsistencyCheck_NumberOfBins() {
    Int_t nBin_gen = map_allEraHist_["gen_DY"]->GetNbinsX();
    Int_t nBin_reco = map_allEraHist_["reco_DY"]->GetNbinsX();

    Int_t nBinX = h_allEra_migM_->GetNbinsX();
    Int_t nBinY = h_allEra_migM_->GetNbinsY();

    if( nBin_gen != nBinY )
      throw std::runtime_error("[Insert_MigM] Inconsistency in # bins: (nBin_gen, nBinY) = (" + std::to_string(nBin_gen) + ", " + std::to_string(nBinY) +")");
    if( nBin_reco != nBinX )
      throw std::runtime_error("[Insert_MigM] Inconsistency in # bins: (nBin_reco, nBinX) = (" + std::to_string(nBin_reco) + ", " + std::to_string(nBinX) +")");
  }

  void Insert_AllEraHist_Unfolded(TString process) {
    TString histType_reco     = TString::Format("reco_%s", process.Data());
    TString histType_unfolded = TString::Format("unfolded_%s", process.Data());

    TUnfold::ERegMode            regMode        = TUnfold::kRegModeSize;
    TUnfold::EConstraint         constraintMode = TUnfold::kEConstraintNone;
    TUnfoldDensity::EDensityMode densityFlags   = TUnfoldDensity::kDensityModeBinWidth;

    double tau = 0; // -- regularization strength (0: no reg.)

    double backScale = 1.0;
    double backScaleError = 0.0; //scale error for background

    TUnfoldDensity unfold(h_allEra_migM_, TUnfold::kHistMapOutputVert, regMode, constraintMode, densityFlags);
    // -- subtract "DYFakes" (the case for "no true pair, but reconstructed pair exist")
    // -- it should be considered as "background"
    // -- reference: https://root.cern.ch/doc/master/classTUnfold.html#a0ca33a31861aa9583000a41c1eceb106
    unfold.SubtractBackground(map_allEraHist_["reco_DYFake"], 
                              "DYFake", 
                              backScale, backScaleError);
    cout << "[DYRun2Result::Insert_AllEraHist_Unfolded] DY fake is subtracted" << endl;

    if( process == "data" ) { // -- subtract backgrounds
      TH1D* h_allEra_bkg = hasFake_fullRun2_ ? 
                           map_allEraHist_["reco_bkgAll"] : map_allEraHist_["reco_bkgMC"];
      unfold.SubtractBackground(h_allEra_bkg, "background", backScale, backScaleError);
    }
    // -- do unfolding    
    unfold.DoUnfold(tau, map_allEraHist_[histType_reco.Data()]);
    // -- get the unfolded result
    TH1D* h_unfolded = (TH1D*)unfold.GetOutput("h_"+histType_unfolded);
    // -- assign the errors
    AssignTotalError_Unfolding(h_unfolded, unfold);
    // -- insert
    map_allEraHist_.insert( std::make_pair(histType_unfolded.Data(), h_unfolded) );
  }

  void Insert_AllEraHist_Unfolded_FPS(TString process) {
    TString histType = "unfolded_"+process;
    TH1D* h_unfolded = map_allEraHist_[histType.Data()];
    TH1D* h_unfolded_FPS = Apply_Acceptance(h_unfolded);

    TString histType_FPS = "unfoldedFPS_"+process;
    map_allEraHist_.insert( std::make_pair(histType_FPS.Data(), h_unfolded_FPS) );
  }

  TH1D* Apply_Acceptance(TH1D* h_fiducial) {
    TH1D* h_FPS = (TH1D*)h_fiducial->Clone();
    h_FPS->Reset("ICES");

    Int_t nBin_acc = h_acc_->GetNbinsX();
    Int_t nBin_fiducial = h_fiducial->GetNbinsX();

    if( nBin_acc != nBin_fiducial ) {
      printf("(nBin_acc, nBin_fiducial) = (%d, %d)\n", nBin_acc, nBin_fiducial);
      throw std::invalid_argument("[DYRun2Result::Apply_Acceptance] inconsistent number of bins");
    }

    for(Int_t i=0; i<nBin_acc; ++i) {
      Int_t i_bin = i+1;

      Double_t acc = h_acc_->GetBinContent(i_bin);
      if( acc == 0 ) {
        printf("[DYRun2Result::Apply_Acceptance] %2d bin's acceptance = 0\n", i_bin);
        throw std::invalid_argument("Acceptance is 0");
      }
 
      Double_t nEvent_fiducial = h_fiducial->GetBinContent(i_bin);
      Double_t error_fiducial = h_fiducial->GetBinError(i_bin);

      Double_t nEvent_FPS = nEvent_fiducial / acc;
      Double_t error_FPS  = error_fiducial / acc;

      h_FPS->SetBinContent(i_bin, nEvent_FPS);
      h_FPS->SetBinError(i_bin, error_FPS);
    }

    return h_FPS;
  }

  void AssignTotalError_Unfolding(TH1D* h_unfolded, TUnfoldDensity& unfold) {
    TH2* h_covM = unfold.GetEmatrixTotal("h_covM");

    for(Int_t i=0; i<h_unfolded->GetNbinsX(); i++) {
      Int_t i_bin = i+1;

      Double_t error = std::sqrt( h_covM->GetBinContent(i_bin, i_bin) ); // -- diagonal term: unc^2
      h_unfolded->SetBinError(i_bin, error);
    }
  }

  // -- Even though "EraOutput" has its own way to provide "bkgMC" process (by adding up all bkgMC processes),
  // -- DYRun2Result does not use "EraOutput" method
  // -- to individually control the histogran name per bkg. MC processes
  // -- here, bkgMC histogram is merged
  void Insert_AllEraHist_BkgMC() {
    if( map_allEraHist_.find("reco_bkgMC") != map_allEraHist_.end() ) {
      cout << "[DYRun2Result::Insert_AllEraHist_BkgMC] custom bkg.MC histogram is already provided ... do not produce" << endl; 
      return;
    }

    vector<TString> vec_bkgMCProcess = map_hNameC_["16pre"].Get_BkgMCProcess();

    TH1D* h_allEra_bkgMC = nullptr;
    for(const auto& process : vec_bkgMCProcess ) {
      // TH1D* h_allEra_process = Make_AllEraHist("reco", process);
      TString histType = "reco_"+process;
      TH1D* h_allEra_process = (map_allEraHist_.find(histType.Data()) != map_allEraHist_.end()) ? 
                               (TH1D*)map_allEraHist_[histType.Data()]->Clone() :
                               Make_AllEraHist("reco", process);

      if( h_allEra_bkgMC == nullptr )
        h_allEra_bkgMC = (TH1D*)h_allEra_process->Clone();
      else
        h_allEra_bkgMC->Add( h_allEra_process );
    }

    map_allEraHist_.insert( std::make_pair("reco_bkgMC", h_allEra_bkgMC) );
  }

  void Insert_AllEraHist_Fake() {
    if( !hasFake_fullRun2_ ) return;

    vector<TString> vec_era = {"16pre", "16post", "17", "18"};
    for(const auto& era : vec_era ) {
      TString fileName_fake, histName_fake;
      map_hNameC_[era].Get_Fake(fileName_fake, histName_fake);
      output_->Get_EraOutput(era).Set_Fake(fileName_fake, histName_fake);
    }

    TH1D* h_16pre  = output_->Get_EraOutput("16pre").Get("dummy", "fake");
    TH1D* h_16post = output_->Get_EraOutput("16post").Get("dummy", "fake");
    TH1D* h_17     = output_->Get_EraOutput("17").Get("dummy", "fake");
    TH1D* h_18     = output_->Get_EraOutput("18").Get("dummy", "fake");

    // -- remove negative bins in individual era (before merging into "allEra" hist)
    if( removeNegativeBinFakeLep_ ) {
      Remove_NegativeBin(h_16pre);
      Remove_NegativeBin(h_16post);
      Remove_NegativeBin(h_17);
      Remove_NegativeBin(h_18);
    }

    TH1D* h_allEra_16pre  = Convert_To_AllEraFormat( h_16pre, "16pre" );
    TH1D* h_allEra_16post = Convert_To_AllEraFormat( h_16post, "16post" );
    TH1D* h_allEra_17     = Convert_To_AllEraFormat( h_17, "17" );
    TH1D* h_allEra_18     = Convert_To_AllEraFormat( h_18, "18" );

    TH1D* h_allEra = MergeHist( {h_allEra_16pre, h_allEra_16post, h_allEra_17, h_allEra_18} );

    map_allEraHist_.insert( std::make_pair("reco_fake", h_allEra) );
    cout << "[DYRun2Result::Insert_AllEraHist_Fake] fake lepton backgrounds are inserted" << endl;
  }

  void Remove_NegativeBin(TH1D* h, Bool_t removeErrorAlso = kFALSE) {
    Int_t nBin = h->GetNbinsX();
    for(Int_t i=-1; i<nBin+1; ++i) {
      Int_t i_bin = i+1;
      Double_t value = h->GetBinContent(i_bin);
      if( value < 0 ) {

        if( i_bin == 0 )
          printf("  [%03d bin] (Underflow < %.3lf) -> value = %lf < 0 ... set it 0\n",
                 i_bin, h->GetBinLowEdge(i_bin+1), value);
        
        else if( i_bin == nBin+1)
          printf("  [%03d bin] (Overflow > %.3lf) -> value = %lf < 0 ... set it 0\n",
               i_bin, h->GetBinLowEdge(i_bin), value);
        
        else
          printf("  [%03d bin] (%.3lf, %.3lf) -> value = %lf < 0 ... set it 0\n",
                 i_bin, h->GetBinLowEdge(i_bin), h->GetBinLowEdge(i_bin+1), value);

        h->SetBinContent(i_bin, 0);
        if( removeErrorAlso )
          h->SetBinError(i_bin, 0);
      }
    }
  }

  TH1D* Convert_To_AllEraFormat( TH1D* h, TString era ) {
    TH1D* h_return = (TH1D*)map_allEraHist_["reco_data"]->Clone();

    // -- reset except for axis
    h_return->Reset("ICES");
    for(Int_t i=-1; i<h_return->GetNbinsX()+1; i++) { // -- include under/overflow
      Int_t i_bin = i+1;
      h_return->SetBinContent(i_bin, 0);
      h_return->SetBinError(i_bin, 0);
    }

    Int_t iEra = 0;
    if( era == "16pre" ) iEra = 0;
    if( era == "16post" ) iEra = 1;
    if( era == "17" ) iEra = 2;
    if( era == "18" ) iEra = 3;

    Int_t nBin_era = h_return->GetNbinsX() / 4.0;
    // cout << "nBin_era = " << nBin_era << endl;

    // -- convert into "AllEra" format
    for(Int_t i=-1; i<h->GetNbinsX()+1; i++) { // -- include under/overflow
      Int_t i_bin = i+1;

      Double_t value = h->GetBinContent(i_bin);
      Double_t error = h->GetBinError(i_bin);

      Int_t i_binTUnfold = iEra*nBin_era + (i_bin+1);

      h_return->SetBinContent(i_binTUnfold, value);
      h_return->SetBinError(i_binTUnfold, error);
    }

    return h_return;
  }

  TH1D* MergeHist( vector<TH1D*> vec_hist ) {
    TH1D* h_merged = nullptr;

    for(const auto& h : vec_hist ) {
      if( h_merged == nullptr ) h_merged = (TH1D*)h->Clone();
      else                      h_merged->Add( h );
    }
    return h_merged;
  }

  TH2D* MergeHist2D( vector<TH2D*> vec_hist ) {
    TH2D* h_merged = nullptr;

    for(const auto& h : vec_hist ) {
      if( h_merged == nullptr ) h_merged = (TH2D*)h->Clone();
      else                      h_merged->Add( h );
    }
    return h_merged;
  }
};

// -- full Run-2 result w/ the combination between two channels
class DYRun2CombResult {
public:
  DYRun2CombResult(Run2Output& output_ee, Run2Output& output_mm): 
  output_ee_(output_ee), output_mm_(output_mm) {

  }

private:
  Run2Output& output_ee_;
  Run2Output& output_mm_;

};