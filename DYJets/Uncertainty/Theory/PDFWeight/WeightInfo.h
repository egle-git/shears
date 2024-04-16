#include "Common/SimplePlotTools.h"
#include "Common/DYOutput.h"
#include "Common/DYTool.h"

class WeightInfo {
public:
  WeightInfo(TString channel, TString era, TString sampleTag, TString weightType, Int_t iSet):
  channel_(channel),
  era_(era),
  sampleTag_(sampleTag),
  weightType_(weightType),
  iSet_(iSet) {
    Init();
  }

  Double_t Mean() const { return mean_; }
  Double_t Sigma() const { return sigma_; }
  Double_t LowerLimit() const { return lowerLimit_; }
  Double_t UpperLimit() const { return upperLimit_; }

  Double_t nEvent_HugeWRatio() const { return nEvent_HugeWRatio_; }
  Double_t Frac_HugeWRatio() const { return frac_HugeWRatio_; }

private:
  TString channel_ = "";
  TString era_ = "";
  TString sampleTag_ = "";
  TString weightType_ = "";
  Int_t iSet_ = -1;

  TString weightTag_ = "";

  TH1D* h_wRatio_ = nullptr;

  // -- values calculated when the histogram is filled (unbinned values), w/o under or overflow
  // -- details: https://root.cern.ch/doc/master/classTH1.html#a3e2fa7eca22330a7f6458e481e6ca0ae
  Double_t mean_ = 0.0;
  Double_t sigma_ = 0.0;
  Double_t lowerLimit_ = 0.0;
  Double_t upperLimit_ = 0.0;
  Double_t nEvent_HugeWRatio_ = 0;
  Double_t frac_HugeWRatio_ = 0;

  void Init() {
    weightTag_ = TString::Format("%s_%03d", weightType_.Data(), iSet_);

    TString basePath = DYTool::path_systVar_theory+"/"+channel_+"/"+era_;

    SampleOutput output(sampleTag_, basePath, DYTool::GetLumi(era_));
    output.DoNormalization(kFALSE);

    TString histName_base = "pdfWRatio";
    TString histName = TString::Format("%s_%s", histName_base.Data(), weightTag_.Data());

    h_wRatio_ = output.Get(histName);

    mean_  = h_wRatio_->GetMean();
    sigma_ = h_wRatio_->GetStdDev();
    // -- +- 5 sigma range
    lowerLimit_ = mean_ - 5*sigma_;
    upperLimit_ = mean_ + 5*sigma_;
    SanityCheck();

    nEvent_HugeWRatio_ = Count_Event_HugeWRatio();
    frac_HugeWRatio_ = nEvent_HugeWRatio_ / h_wRatio_->Integral(0, h_wRatio_->GetNbinsX()+1);
  }

  void SanityCheck() {
    if( lowerLimit_ < -1.0 || upperLimit_ > 3.0 ) {
      printf("[%s, %s, %s, %s]\n", channel_.Data(), era_.Data(), sampleTag_.Data(), weightTag_.Data());
      printf("(mean, sigma) = (%lf, %lf)\n", mean_, sigma_);
      if( lowerLimit_ < -1.0 )
        printf("--> lowerLimit = %lf < -1.0: outside of the histogram range\n", lowerLimit_);
      if( upperLimit_ > 3.0 )
        printf("--> upperLimit = %lf > 3.0: outside of the histogram range\n", upperLimit_);
    }
  }

  Double_t Count_Event_HugeWRatio() {
    Int_t count = 0;

    Int_t nBin = h_wRatio_->GetNbinsX();

    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;

      Double_t binCenter = h_wRatio_->GetXaxis()->GetBinCenter(i_bin);
      if( IsEvent_HugeWRatio(binCenter) )
        count += h_wRatio_->GetBinContent(i_bin);
    }

    count += h_wRatio_->GetBinContent(0); // -- underflow
    count += h_wRatio_->GetBinContent(nBin+1); // -- overflow

    return count;
  }

  Bool_t IsEvent_HugeWRatio(Double_t value) {
    Bool_t flag = kFALSE;

    if( lowerLimit_ < value && value < upperLimit_ ) flag = kFALSE; // -- safe: usual weights
    else flag = kTRUE;

    return flag;
  }
};

// -- all weight info (PDFVar and scaleVar) for a given sample
class SampleWeightInfo {
public:
  SampleWeightInfo(TString channel, TString era, TString sampleTag):
  channel_(channel),
  era_(era),
  sampleTag_(sampleTag) {
    Init();
  }

  TString SampleTag() const { return sampleTag_; }

  TH1D* Hist(TString type) const {
    if( type == "mean" )            return (TH1D*)h_mean_->Clone();
    if( type == "sigma" )           return (TH1D*)h_sigma_->Clone();
    if( type == "frac_hugeWRatio" ) return (TH1D*)h_frac_hugeWRatio_->Clone();

    throw std::invalid_argument("[SampleWeightInfo::Hist] type = " + type + " is not supported");
    return nullptr;
  }

  void Save(TFile *f_output) {
    f_output->cd();

    h_mean_->Write();
    h_sigma_->Write();
    h_lowerLimit_->Write();
    h_upperLimit_->Write();
    h_frac_hugeWRatio_->Write();
  }


private:
  TString channel_ = "";
  TString era_ = "";
  TString sampleTag_ = "";

  std::map<TString, WeightInfo> map_weightInfo_;
  TH1D* h_mean_;
  TH1D* h_sigma_;
  TH1D* h_lowerLimit_;
  TH1D* h_upperLimit_;
  TH1D* h_frac_hugeWRatio_;

  void Init() {
    // -- set weight tags
    // -- PDF var.
    for(Int_t i=1; i<=102; ++i) {

      TString tag = TString::Format("PDFVar_%03d", i);
      WeightInfo info(channel_, era_, sampleTag_, "PDFVar", i);

      map_weightInfo_.insert( std::make_pair(tag, info) );
    }

    // -- scale var. : does not use for now (weight distribution should have larger x-axis)
    // for(Int_t i=0; i<=8; ++i) {
    //   if( i == 2 ) continue;
    //   if( i == 4 ) continue;
    //   if( i == 6 ) continue;

    //   TString tag = TString::Format("scaleVar_%03d", i);
    //   WeightInfo info(channel_, era_, sampleTag_, "scaleVar", i);

    //   map_weightInfo_.insert( std::make_pair(tag, info) );
    // }

    h_mean_            = MakeHist_vs_Weight("mean");
    h_sigma_           = MakeHist_vs_Weight("sigma");
    h_lowerLimit_      = MakeHist_vs_Weight("lowerLimit");
    h_upperLimit_      = MakeHist_vs_Weight("upperLimit");
    h_frac_hugeWRatio_ = MakeHist_vs_Weight("frac_hugeWRatio");
  }

  TH1D* MakeHist_vs_Weight(TString type) {
    Int_t nWeight = (Int_t)map_weightInfo_.size();

    TH1D* h_vsWeight = new TH1D("h_"+type+"_"+sampleTag_, "", nWeight, 0, nWeight);

    Int_t i = 0;
    for(const auto& pair : map_weightInfo_ ) {
      Int_t i_bin = i+1;

      TString label = pair.first;
      Double_t value = 0;
      if( type == "mean" )            value = pair.second.Mean();
      if( type == "sigma" )           value = pair.second.Sigma();
      if( type == "lowerLimit" )      value = pair.second.LowerLimit();
      if( type == "upperLimit" )      value = pair.second.UpperLimit();
      if( type == "frac_hugeWRatio" ) value = pair.second.Frac_HugeWRatio();

      h_vsWeight->GetXaxis()->SetBinLabel(i_bin, label);
      h_vsWeight->SetBinContent(i_bin, value);
      h_vsWeight->SetBinError(i_bin, 0);

      i++;
    }

    h_vsWeight->LabelsOption("v");

    return h_vsWeight;
  }
};

class EraWeightInfo {
public:
  std::map<TString, SampleWeightInfo> map_sampleWeightInfo_;

  EraWeightInfo(TString channel, TString era): channel_(channel), era_(era) {
    Init();
  }

  void Save(TFile *f_output) {
    f_output->cd();
    for(auto& pair : map_sampleWeightInfo_ )
      pair.second.Save(f_output);
  }

  std::map<TString, SampleWeightInfo>& InfoMap() { return map_sampleWeightInfo_; }

private:
  TString channel_;
  TString era_;

  vector<TString> vec_sampleTag_;


  void Init() {
    // -- collect sampleTags for the samples that have PDF weights
    for(const auto& tag : DYTool::map_tag["DY"])
      vec_sampleTag_.push_back( tag );

    for(const auto& tag : DYTool::map_tag["TT"])
      vec_sampleTag_.push_back( tag );

    for(const auto& tag : DYTool::map_tag["TauTau"])
      vec_sampleTag_.push_back( tag );

    vec_sampleTag_.push_back("ST_t-channel_antitop");
    vec_sampleTag_.push_back("ST_t-channel_top");
    vec_sampleTag_.push_back("ST_tW_antitop");
    vec_sampleTag_.push_back("ST_tW_top");

    for(const auto& sampleTag : vec_sampleTag_ ) {
      SampleWeightInfo info(channel_, era_, sampleTag);
      map_sampleWeightInfo_.insert( std::make_pair(sampleTag, info) );
    }
  }

};