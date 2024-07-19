#pragma once

#include "Common/SimplePlotTools.h"
#include "Common/DYTool.h"
#include "Common/DYOutput.h"

#include "TString.h"


namespace DYAcc {

vector<TString> vec_era = {"16pre", "16post", "17", "18"};

// -- in pb, only for 1 flavor
std::map<TString, Double_t> map_xSec = {
  {"m10to50", 7012.53},
  {"m50to100", 1925.65144658},
  {"m100to200", 77.95},
  {"m200to400", 2.78},
  {"m400to500", 0.15},
  {"m500to700", 0.084},
  {"m700to800", 0.013},
  {"m800to1000", 0.011},
  {"m1000to1500", 0.0060},
  {"m1500to2000", 0.00081},
  {"m2000toInf", 0.00020}
};

vector<Int_t> color_massRange = {
  kBlue,
  kGreen+2,
  kViolet,
  kCyan,
  kPink,
  kOrange-9,
  kGray,
  kViolet+7,
  kMagenta+1,
  kRed+2,
  kBlue-9
};

// const Int_t nMassBin = 39;
// Double_t arr_massBinEdge[nMassBin+1] = {
//   40,45,50,55,60,64,68,72,76,81,
//   86,91,96,101,106,110,115,120,126,133,
//   141,150,160,171,185,200,220,243,273,320,
//   380,440,510,600,700,830,1000,1500,2000,3000
// };

// -- have 2 more bins: under/overflow
// -- to sync. with the format used in the analysis (TUnfold output)
const Int_t nMassBin = 39+2;
Double_t arr_massBinEdge[nMassBin+1] = {
  10, // -- underflow
  40,45,50,55,60,64,68,72,76,81,
  86,91,96,101,106,110,115,120,126,133,
  141,150,160,171,185,200,220,243,273,320,
  380,440,510,600,700,830,1000,1500,2000,3000,
  5000 // --overflow
};

// -- no uf/of this case
const Int_t nMassBin_aboveM200 = 14;
Double_t arr_massBinEdge_aboveM200[nMassBin_aboveM200+1] = {
  200,220,243,273,320,
  380,440,510,600,700,830,1000,1500,2000,3000
};

vector<TString> GetVector_MassRange(Bool_t useNewM50Sample) {
  vector<TString> vec_massRange;
  for(const auto& pair : map_xSec ) {
    if( useNewM50Sample && pair.first == "m50to100" )
      vec_massRange.push_back("m50to100_ZptWeighted");
    else
      vec_massRange.push_back(pair.first);
  }

  return vec_massRange;
}

Double_t Get_SumWeight(TString inputPath, TString massRange, TString fileName) {
  Double_t sumWeight = -1;   
  
  TH1D* h_sumWeight = PlotTool::Get_Hist(inputPath+"/"+fileName, "DYAcceptanceProducer/h_weight");
  sumWeight = h_sumWeight->GetBinContent(1);

  if( massRange == "m10to50" ) {
    TString fileName_ext = fileName;
    fileName_ext.ReplaceAll("m10to50", "m10to50_ext");

    // cout << "[Get_SumWeight] fileName_ext = " << fileName_ext << endl;

    TH1D* h_sumWeight_ext = PlotTool::Get_Hist(inputPath+"/"+fileName_ext, "DYAcceptanceProducer/h_weight");
    sumWeight += h_sumWeight_ext->GetBinContent(1);
  }

  return sumWeight;
}

void Normalization(TH1D* h_mass, TString inputPath, TString massRange, TString fileName, TString era) {
  Double_t sumWeight = Get_SumWeight(inputPath, massRange, fileName);
  Double_t xSec = DYAcc::map_xSec[massRange];
  Double_t lumi = DYTool::GetLumi(era);
  Double_t normFactor = (xSec*lumi) / sumWeight; // -- norm. to corresponding era (to be consistent with TUnfold result)

  h_mass->Scale( normFactor );
}

// -- Get normalized & rebinned histogram
TH1D* Get_Hist_Norm_Rebinned(TString inputPath, TString channel, TString massRange, TString era, Bool_t isFiducial, TString tag = "cv") {
  TString fileName = TString::Format("%s_%s_%s.root", channel.Data(), massRange.Data(), era.Data());
  if( massRange.Contains("m50to100") ) // -- m50to100 or m50to100_ZptWeighted
    fileName.ReplaceAll("m50to100", "m50");

  // if( massRange == "m50to100_ZptWeighted" )
  //   fileName.ReplaceAll("m50to100_ZptWeighted", "m50_ZptWeighted");

  TString histName;
  if( tag == "cv" ) histName = "DYAcceptanceProducer/h_diLep_mass";
  else              histName = "DYAcceptanceProducer/h_diLep_mass_"+tag;

  if( isFiducial ) histName += "_acc";

  TH1D* h_mass = PlotTool::Get_Hist(inputPath+"/"+fileName, histName);
  // cout << "[Get_Hist_Norm_Rebinned] h_mass = " << h_mass << endl;
  // PlotTool::Print_Histogram(h_mass);

  // -- m10to50: add "_ext" sample also
  if( massRange == "m10to50" ) {
    TString fileName_ext = fileName;
    fileName_ext.ReplaceAll("m10to50", "m10to50_ext");
    TH1D* h_mass_ext = PlotTool::Get_Hist(inputPath+"/"+fileName_ext, histName);
    h_mass->Add( h_mass_ext );
  }

  if( massRange == "m50to100_ZptWeighted" )
    massRange.ReplaceAll("_ZptWeighted", "");

  // -- norm. to lumi
  // -- sum(weight) of m10to50 sample: m10to50 + m10to50_ext
  Normalization(h_mass, inputPath, massRange, fileName, era);

  // -- rebin
  h_mass = (TH1D*)h_mass->Rebin(DYAcc::nMassBin, "h_"+era, DYAcc::arr_massBinEdge);

  return h_mass;
}

TH1D* MergeHist(vector<TH1D*>& vec_hist) {
  TH1D* h_merged = nullptr;

  for(const auto& hist : vec_hist ) {
    if( !h_merged ) h_merged = (TH1D*)hist->Clone();
    else            h_merged->Add(hist);
  }

  return h_merged;
}

// -- merged over all mass ranges
TH1D* Get_MergedHist_AllMass(TString inputPath, TString channel, TString era, Bool_t isFiducial, TString tag = "cv", Bool_t useNewM50Sample = kFALSE) {
  vector<TString> vec_massRange = DYAcc::GetVector_MassRange(useNewM50Sample);
  vector<TH1D*> vec_hist;
  for(const auto& massRange : vec_massRange ) {
    // cout << "[Get_MergedHist_AllMass] massRange = " << massRange << endl;
    TH1D* h_massRange = DYAcc::Get_Hist_Norm_Rebinned(inputPath, channel, massRange, era, isFiducial, tag);
    vec_hist.push_back( h_massRange );
  }

  return DYAcc::MergeHist(vec_hist);
}

// -- merged for all mass range & all eras
TH1D* Get_MergedHist_Channel(TString inputPath, TString channel, Bool_t isFiducial, TString tag = "cv", Bool_t useNewM50Sample = kFALSE) {
  vector<TH1D*> vec_hist;
  for(const auto& era : DYAcc::vec_era) {
    // cout << "[Get_MergedHist_Channel] era = " << era << endl;
    TH1D* h_era = Get_MergedHist_AllMass(inputPath, channel, era, isFiducial, tag, useNewM50Sample);
    vec_hist.push_back( h_era );
  }

  return DYAcc::MergeHist(vec_hist);
}

struct DYGenHistInfo {
  TString inputPath;
  TString channel;
  TString massRange;
  TString era;
  Bool_t isFiducial;
  TString tag;

  DYGenHistInfo() {}

  DYGenHistInfo(const DYGenHistInfo& info) {
    inputPath = info.inputPath;
    channel = info.channel;
    massRange = info.massRange;
    era = info.era;
    isFiducial = info.isFiducial;
    tag = info.tag;
  }
};

class SingleDYHist {
public:
  SingleDYHist(DYAcc::DYGenHistInfo info): info_(info) {

  }

  void Use_NewM50Sample(Bool_t flag = kTRUE) { useNewM50_ = flag; }

  void Set_MassBin(Int_t nMassBin, Double_t* arr_massBinEdge) {
    nMassBin_= nMassBin;
    arr_massBinEdge_ = arr_massBinEdge;
  }

  TH1D* Get(TString histType = "norm_reBinned") {
    if( !isSet_ ) Init();

    if( histType == "norm_reBinned" ) return h_norm_reBinned_;
    if( histType == "raw" )           return h_raw_;

    throw std::invalid_argument("[SingleDYHist::Get] histType = " + histType + " is not supported");

    return nullptr;
  }

private:
  DYAcc::DYGenHistInfo info_;
  Bool_t isSet_ = kFALSE;

  Bool_t useNewM50_ = kFALSE; // -- use the new M50 sample (ZptReweighted)

  Int_t nMassBin_ = 0;
  Double_t *arr_massBinEdge_ = nullptr;

  TH1D* h_raw_;
  TH1D* h_norm_reBinned_;

  void Init() {
    isSet_ = kTRUE;

    if( nMassBin_ == 0 || !arr_massBinEdge_ ) { // -- if bin information is not provided
      nMassBin_ = DYAcc::nMassBin;
      arr_massBinEdge_ = DYAcc::arr_massBinEdge; // -- use the default range
    }

    TString fileName = Get_FileName();
    TString histName = Get_HistName();

    h_raw_ = Get_RawHist(fileName, histName);

    // -- norm. to lumi
    // -- sum(weight) of m10to50 sample: m10to50 + m10to50_ext
    DoNorm(h_raw_, fileName);

    // -- rebin
    h_norm_reBinned_ = (TH1D*)h_raw_->Rebin(nMassBin_, "h_"+info_.era, arr_massBinEdge_);
  }

  TH1D* Get_RawHist(TString fileName, TString histName) {
    TH1D* h_raw = PlotTool::Get_Hist(info_.inputPath+"/"+fileName, histName);

    // -- m10to50: add "_ext" sample also
    if( info_.massRange == "m10to50" ) {
      TString fileName_ext = fileName;
      fileName_ext.ReplaceAll("m10to50", "m10to50_ext");
      TH1D* h_raw_ext = PlotTool::Get_Hist(info_.inputPath+"/"+fileName_ext, histName);
      h_raw->Add( h_raw_ext );
    }

    return h_raw;
  }

  TString Get_FileName() {
    TString fileName = TString::Format("%s_%s_%s.root", info_.channel.Data(), info_.massRange.Data(), info_.era.Data());
    if( info_.massRange.Contains("m50to100") ) {
      if( useNewM50_ ) fileName.ReplaceAll("m50to100", "m50_ZptWeighted");
      else             fileName.ReplaceAll("m50to100", "m50");
    }

    return fileName;
  }

  TString Get_HistName() {
    TString histName;
    if( info_.tag == "cv" ) histName = "DYAcceptanceProducer/h_diLep_mass";
    else                    histName = "DYAcceptanceProducer/h_diLep_mass_"+info_.tag;

    if( info_.isFiducial ) histName += "_acc";

    return histName;
  }

  void DoNorm(TH1D* h_mass, TString fileName) {
    Double_t sumWeight = Get_SumWeight(fileName);
    Double_t xSec = DYAcc::map_xSec[info_.massRange];
    Double_t lumi = DYTool::GetLumi(info_.era);
    Double_t normFactor = (xSec*lumi) / sumWeight; // -- norm. to corresponding era (to be consistent with TUnfold result)

    h_mass->Scale( normFactor );
  }

  Double_t Get_SumWeight(TString fileName) {
    Double_t sumWeight = -1;   
    
    TH1D* h_sumWeight = PlotTool::Get_Hist(info_.inputPath+"/"+fileName, "DYAcceptanceProducer/h_weight");
    sumWeight = h_sumWeight->GetBinContent(1);

    if( info_.massRange == "m10to50" ) {
      TString fileName_ext = fileName;
      fileName_ext.ReplaceAll("m10to50", "m10to50_ext");

      // cout << "[Get_SumWeight] fileName_ext = " << fileName_ext << endl;

      TH1D* h_sumWeight_ext = PlotTool::Get_Hist(info_.inputPath+"/"+fileName_ext, "DYAcceptanceProducer/h_weight");
      sumWeight += h_sumWeight_ext->GetBinContent(1);
    }

    return sumWeight;
  }
};

class MassMerger {
public:
  MassMerger(TString inputPath, TString channel, TString era, Bool_t isFiducial, TString tag = "cv"):
  inputPath_(inputPath), channel_(channel), era_(era), isFiducial_(isFiducial), tag_(tag) { }

  void Use_NewM50Sample(Bool_t flag = kTRUE) { useNewM50_ = flag; }

  void AboveM200(Bool_t flag = kTRUE) { aboveM200_ = flag; }

  TH1D* Get() {
    if( !isSet_ ) Init();

    return h_merged_;
  }

private:
  TString inputPath_;
  TString channel_;
  TString era_;
  Bool_t isFiducial_;
  TString tag_;

  Bool_t isSet_ = kFALSE;

  Bool_t useNewM50_ = kFALSE; // -- use the new M50 sample (ZptReweighted)
  Bool_t aboveM200_ = kFALSE; // -- only above m = 200 GeV?

  std::map<TString, SingleDYHist> map_hist_;

  TH1D* h_merged_ = nullptr;

  void Init() {
    isSet_ = kTRUE;

    vector<TString> vec_massRange = GetVector_MassRange();
    for(const auto& massRange : vec_massRange ) {
      if( aboveM200_ ) {
        if( massRange == "m10to50" )   continue;
        if( massRange == "m50to100" )  continue;
        if( massRange == "m100to200" ) continue;
      }

      DYGenHistInfo info{};
      info.inputPath = inputPath_;
      info.channel = channel_;
      info.massRange = massRange;
      info.era = era_;
      info.isFiducial = isFiducial_;
      info.tag = tag_;

      SingleDYHist hist(info);
      if( useNewM50_ ) hist.Use_NewM50Sample();
      if( aboveM200_ ) hist.Set_MassBin(DYAcc::nMassBin_aboveM200, DYAcc::arr_massBinEdge_aboveM200);
      map_hist_.insert( std::make_pair(massRange, hist) );
    }

    vector<TH1D*> vec_massHist;
    for(auto& pair : map_hist_ )
      vec_massHist.push_back( pair.second.Get() );

    h_merged_ = DYAcc::MergeHist( vec_massHist );
  }

  vector<TString> GetVector_MassRange() {
    vector<TString> vec_massRange;
    for(const auto& pair : map_xSec )
      vec_massRange.push_back(pair.first);

    return vec_massRange;
  }

};

class EraMerger {
public:
  EraMerger(TString inputPath, TString channel, Bool_t isFiducial, TString tag = "cv"):
  inputPath_(inputPath), channel_(channel), isFiducial_(isFiducial), tag_(tag) { }

  void Use_NewM50Sample(Bool_t flag = kTRUE) { useNewM50_ = flag; }

  void AboveM200(Bool_t flag = kTRUE) { aboveM200_ = flag; }

  TH1D* Get() {
    if( !isSet_ ) Init();

    return h_merged_;
  }

private:
  TString inputPath_;
  TString channel_;
  Bool_t isFiducial_;
  TString tag_;

  Bool_t isSet_ = kFALSE;
  Bool_t useNewM50_ = kFALSE; // -- use the new M50 sample (ZptReweighted)
  Bool_t aboveM200_ = kFALSE; // -- only above m = 200 GeV?

  TH1D* h_merged_ = nullptr;

  std::map<TString, MassMerger> map_merger_;

  void Init() {
    isSet_ = kTRUE;

    vector<TString> vec_era = {"16pre", "16post", "17", "18"};

    for(const auto& era : vec_era ) {
      MassMerger merger(inputPath_, channel_, era, isFiducial_, tag_);
      if( useNewM50_ ) merger.Use_NewM50Sample();
      if( aboveM200_ ) merger.AboveM200();

      map_merger_.insert( std::make_pair(era, merger) );
    }

    vector<TH1D*> vec_massHist;
    for(auto& pair : map_merger_ )
      vec_massHist.push_back( pair.second.Get() );

    h_merged_ = DYAcc::MergeHist( vec_massHist );
  }
};


}; // -- namespcae DYAcc