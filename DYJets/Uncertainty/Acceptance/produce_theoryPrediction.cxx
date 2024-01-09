#include "DYAccTool.h"

class TheoryHist {
public:
  TheoryHist() { }

  TheoryHist(TString inputPath, TString tag): 
  inputPath_(inputPath), tag_(tag) { }

  void Use_NewM50Sample(Bool_t flag = kTRUE) { useNewM50_ = flag; }

  void AboveM200(Bool_t flag = kTRUE) { aboveM200_ = flag; }

  // -- histType
  // ---- disgdm_FPS/fid
  // ---- nEvent_FPS/fid (m > 200)
  // ---- nEvent_ufof_FPS/fid (entire mass range)
  TH1D* Get(TString histType) { 
    if( !isSet_ ) Init();

    auto iter = map_hist_.find(histType);
    if( iter == map_hist_.end() )
      throw std::invalid_argument("[TheoryHist::Get] no histogram for histType = "+histType+" is available");

    return iter->second;
  }

  void Save(TFile* f_output, TString saveType) {
    if( !isSet_ ) Init();

    f_output->cd();

    if( saveType == "reduced" ) { // -- save dsigdm only
      map_hist_["dsigdm_FPS"]->SetName("h_dsigdm_FPS_"+tag_);
      map_hist_["dsigdm_fid"]->SetName("h_dsigdm_fid_"+tag_);

      map_hist_["dsigdm_FPS"]->Write();
      map_hist_["dsigdm_fid"]->Write();
    }
    else { // -- save all
      for(const auto& pair : map_hist_ ) {
        pair.second->SetName("h_"+pair.first+"_"+tag_);
        pair.second->Write();
      }
    }

  }

private:
  Bool_t isSet_ = kFALSE;
  Bool_t useNewM50_ = kFALSE; // -- use the new M50 sample (ZptReweighted)
  Bool_t aboveM200_ = kFALSE; // -- only above m = 200 GeV?

  TString inputPath_;
  TString tag_;

  std::map<TString, TH1D*> map_hist_;

  void Init() {
    isSet_ = kTRUE;

    InsertHist(kTRUE);
    InsertHist(kFALSE);
    InsertHist_DXSec();
  }

  DYAcc::EraMerger Get_Merger(TString channel, Bool_t isFiducial) {
    DYAcc::EraMerger merger(inputPath_, channel, isFiducial, tag_);
    if( useNewM50_ ) merger.Use_NewM50Sample();
    if( aboveM200_ ) merger.AboveM200();

    return merger;
  }

  void InsertHist(Bool_t isFiducial) {
    DYAcc::EraMerger merger_ee = Get_Merger("ee", isFiducial);
    DYAcc::EraMerger merger_mm = Get_Merger("mm", isFiducial);

    TH1D* h_ee = merger_ee.Get();
    TH1D* h_mm = merger_mm.Get();

    // -- take average
    TH1D* h_merged = (TH1D*)h_ee->Clone();
    h_merged->Add( h_mm );

    h_merged->Scale(1 / 2.0); // -- average

    // -- ufof: include underflow(uf) and overflow(of); total (N+2) bins
    TString histType_base = "";
    if( aboveM200_ )
      histType_base = (isFiducial) ? "nEvent_fid" : "nEvent_FPS";
    else
      histType_base = (isFiducial) ? "nEvent_ufof_fid" : "nEvent_ufof_FPS";

    map_hist_.insert( std::make_pair(histType_base+"_ee", h_ee) );
    map_hist_.insert( std::make_pair(histType_base+"_mm", h_mm) );
    map_hist_.insert( std::make_pair(histType_base,       h_merged) );
  }

  void InsertHist_DXSec() {
    if( aboveM200_ )
      InsertHist_DXSec_AboveM200();
    else
      InsertHist_DXSec_EntireRange();
  }

  // -- no under/overflow events
  // -- just normalize with the bin width & luminosity
  void InsertHist_DXSec_AboveM200() {
    for(const auto& pair : map_hist_) {
      if( !pair.first.Contains("nEvent") ) continue;

      TH1D* h_dsigdm = PlotTool::DivideEachBin_ByBinWidth(pair.second);
      h_dsigdm->Scale(1.0 / DYTool::GetLumi("all") );

      TString histType = pair.first;
      histType.ReplaceAll("nEvent", "dsigdm");
      map_hist_.insert( std::make_pair(histType, h_dsigdm) );
    }    
  }

  // -- with under/overflow bins
  // -- dedicated converting function
  void InsertHist_DXSec_EntireRange() {
    for(const auto& pair : map_hist_) {
      if( !pair.first.Contains("nEvent") ) continue;

      TH1D* h_dsigdm = DYTool::Convert_TUnfoldOutput_DSigmaDM(pair.second, DYTool::GetLumi("all"));

      TString histType = pair.first;
      histType.ReplaceAll("nEvent_ufof", "dsigdm");
      map_hist_.insert( std::make_pair(histType, h_dsigdm) );
    }
  }
};


class HistProducer {
public:
  HistProducer() {}

  void Use_NewM50Sample(Bool_t flag = kTRUE) { useNewM50_ = flag; }
  void AboveM200(Bool_t flag = kTRUE) { aboveM200_ = flag; }

  void Produce() {
    TH1::AddDirectory(kFALSE);

    Init();

    Save();
  }

private:
  TString fileName_output_ = "TheoryPrediction_VariousPDF.root";
  vector<TString> vec_tag_;
  std::map<TString, TheoryHist> map_theoryHist_;

  Bool_t useNewM50_ = kFALSE; // -- use ZptRweighted sample (same PDF weights with the other mass-binned samples)
  Bool_t aboveM200_ = kFALSE; // -- produce the plots only above m > 200 GeV

  void Init() {
    if( useNewM50_ )
      fileName_output_.ReplaceAll(".root", "_newM50.root");

    if( aboveM200_ )
      fileName_output_.ReplaceAll(".root", "_aboveM200.root");

    Init_Tag();

    TString path = DYTool::path_inputForAcc;
    for(const auto& tag : vec_tag_ ) {
      if( !useNewM50_ && !aboveM200_ )
        if( !( tag.Contains("PDFVar_") || tag.Contains("scaleVar_") ) ) continue; // -- only NNPDF 3.1 variations

      TheoryHist hist(path, tag);
      if( useNewM50_ ) hist.Use_NewM50Sample();
      if( aboveM200_ ) hist.AboveM200();

      map_theoryHist_.insert( std::make_pair(tag, hist) );
    }
  }

  void Init_Tag() {
    vec_tag_.push_back("PDFVar_000"); // 000: cv

    // -- use new sample or only use m > 200 GeV samples: other PDF variations can be used
    if( aboveM200_ || useNewM50_ ) {
      vec_tag_.push_back("ABMP16_000");
      vec_tag_.push_back("CT18Z_000");
      vec_tag_.push_back("CT18_000");
      vec_tag_.push_back("HERAPDF20_000");
      vec_tag_.push_back("MMHT2014_000");
      vec_tag_.push_back("MSHT20_000");
      vec_tag_.push_back("NNPDF30_000");
      vec_tag_.push_back("NNPDF40_000");
      vec_tag_.push_back("PDF4LHC21_000"); // -- maybe not useful, though
    }
  }

  void Save() {
    TFile* f_output = TFile::Open(fileName_output_, "RECREATE");

    for(auto& pair : map_theoryHist_ ) {
      if( pair.first == "PDFVar_000")
        pair.second.Save(f_output, "full");
      else
        pair.second.Save(f_output, "reduced");
    }

    f_output->Close();
  }
};

void produce_theoryPrediction() {
  // -- default
  HistProducer producer;
  producer.Produce();

  // -- newMu50 sample
  HistProducer producer_newM50;
  producer_newM50.Use_NewM50Sample();
  producer_newM50.Produce();

  // -- m > 200 GeV only
  HistProducer producer_m200;
  producer_m200.AboveM200();
  producer_m200.Produce();
}