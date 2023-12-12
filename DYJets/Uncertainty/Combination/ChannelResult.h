#include "Common/SimplePlotTools.h"
#include "Common/DYTool.h"

class ChannelResult {
public:
  ChannelResult() {}

  ChannelResult(TString fileName): fileName_(fileName) { Init(); }

  // TH1D* Get(TString histType)   { return map_hist_[histType]; }
  // TH2D* Get2D(TString histType) { return map_hist2D_[histType]; }

  TString FileName() const { return fileName_; };

  TH1D* Get(TString histType) const {
    auto iter = map_hist_.find(histType);
    if( iter == map_hist_.end() )
      throw std::invalid_argument("[ChannelResult::Get] histType = " + histType + " is not available");

    return (TH1D*)iter->second->Clone();
  }

  TH2D* Get2D(TString histType) const {
    auto iter = map_hist2D_.find(histType);
    if( iter == map_hist2D_.end() )
      throw std::invalid_argument("[ChannelResult::Get] histType = " + histType + " is not available");

    return (TH2D*)iter->second->Clone();
  }

  // -- only "mAxis" result will be saved
  void Save(TFile* f_output, TString tag) {
    f_output->cd();

    for(const auto& pair : map_hist_) {
      TString histType = pair.first;
      if( !histType.Contains("mAxis") ) continue; // -- only "mAxis" result will be saved

      TString histName = "h_"+histType;
      if( tag != "" ) histName += "_"+tag;
      histName.ReplaceAll("_mAxis", ""); // -- remove "mAxis" tag (as only these results will be saved)

      TH1D* h = (TH1D*)pair.second->Clone();
      h->SetName(histName);
      h->Write();
    }

    for(const auto& pair : map_hist2D_) {
      TString histType = pair.first;
      if( !histType.Contains("mAxis") ) continue; // -- only "mAxis" result will be saved

      TString histName = "h_"+histType;
      if( tag != "" ) histName += "_"+tag;
      histName.ReplaceAll("_mAxis", ""); // -- remove "mAxis" tag (as only these results will be saved)

      TH2D* h2D = (TH2D*)pair.second->Clone();
      h2D->SetName(histName);
      h2D->Write();
    }
  }

private:
  TString fileName_;

  std::map<TString, TH1D*> map_hist_;
  std::map<TString, TH2D*> map_hist2D_;

  void Init() {
    TFile *f_input = TFile::Open(fileName_);

    // -- loop over histograms in the file
    // -- absUnc, covM: do not get from the file (they are w.r.t. # events, not dsigma/dm)
    // ---- they will be reconstructed using dsigma/dm, relUnc and corrM
    for(auto&& keyAsObj : *f_input->GetListOfKeys()){
     auto key = (TKey*)keyAsObj;

     TString histName = key->GetName();
     if( histName.Contains("absUnc_") || histName.Contains("covM_") ) continue;

     TString className = key->GetClassName();

     TString histType = histName;
     histType.ReplaceAll("h_", "");

     if( className.Contains("TH1") ) map_hist_.insert( std::make_pair(histType, (TH1D*)key->ReadObj()) );
     if( className.Contains("TH2") ) map_hist2D_.insert( std::make_pair(histType, (TH2D*)key->ReadObj()) );
    }

    Init_AbsUnc_wrt_DSigDM();
    Init_CovM_wrt_DSigDM();

    // -- update abs. unc on the dsigma/dm
    DYTool::Assign_RelUnc(map_hist_["dsigdm_data"], map_hist_["relUnc_tot_mAxis"]);
    // PlotTool::Print_Histogram(map_hist_["dsigdm_data"]);

    // for(const auto& pair : map_hist_ )   printf("%s\n", pair.first.Data());
    // for(const auto& pair : map_hist2D_ ) printf("%s\n", pair.first.Data());
  }

  void Init_AbsUnc_wrt_DSigDM() {
    TH1D* h_dsigdm = map_hist_["dsigdm_data"];

    for(const auto& pair : map_hist_) {
      TString histType = pair.first;
      if( !histType.Contains("mAxis") ) continue; // -- not necessary (no under/overflow values in dsigma/dm)
      if( !histType.Contains("relUnc_") ) continue;
      
      TH1D* h_relUnc = (TH1D*)pair.second->Clone();
      TH1D* h_absUnc = Get_AbsUnc(h_dsigdm, h_relUnc);

      TString histType_absUnc = histType;
      histType_absUnc.ReplaceAll("relUnc", "absUnc");
      h_absUnc->SetName("h_"+histType_absUnc);
      map_hist_.insert( std::make_pair(histType_absUnc, h_absUnc) );
    }
  }

  void Init_CovM_wrt_DSigDM() {
    TH1D* h_dsigdm = map_hist_["dsigdm_data"];

    for(const auto& pair : map_hist2D_) {
      TString histType = pair.first;
      if( !histType.Contains("mAxis") ) continue;
      if( !histType.Contains("corrM_") ) continue;

      TH2D* h_corrM = (TH2D*)pair.second->Clone();
      TString histType_relUnc = histType;
      histType_relUnc.ReplaceAll("corrM_", "relUnc_");
      TH1D* h_relUnc = map_hist_[histType_relUnc];

      TH2D* h_covM = Get_CovM(h_corrM, h_dsigdm, h_relUnc);
      TString histType_covM = histType;
      histType_covM.ReplaceAll("corrM", "covM");
      h_covM->SetName("h_"+histType_covM);

      // if( histType_covM == "covM_tot_mAxis" )
      //   cout << "h_covM->GetBinContent(1, 1) = " << h_covM->GetBinContent(1, 1) << endl;

      map_hist2D_.insert( std::make_pair(histType_covM, h_covM) );
    }
  }

  TH1D* Get_AbsUnc(TH1D* h_cv, TH1D* h_relUnc) {
    Check_nBinConsistency("Get_AbsUnc", h_cv, h_relUnc);

    TH1D* h_absUnc = (TH1D*)h_relUnc->Clone();
    Int_t nBin = h_cv->GetNbinsX();
    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;

      Double_t value = h_cv->GetBinContent(i_bin);
      Double_t relUnc = h_relUnc->GetBinContent(i_bin);

      Double_t absUnc = value*relUnc;

      h_absUnc->SetBinContent(i_bin, absUnc);
      h_absUnc->SetBinError(i_bin, 0);
    }

    return h_absUnc;
  }

  TH2D* Get_CovM(TH2D* h_corrM, TH1D* h_cv, TH1D* h_relUnc) const {
    Int_t nBinX_corrM = h_corrM->GetNbinsX();
    Int_t nBin_cv = h_cv->GetNbinsX();
    Int_t nBin_relUnc = h_relUnc->GetNbinsX();

    if( (nBinX_corrM != nBin_cv) || (nBinX_corrM != nBin_relUnc) ) {
      printf("[nBin] (corrM-xAxis, cv, relUnc) = (%d, %d, %d)\n", nBinX_corrM, nBin_cv, nBin_relUnc);
      throw std::runtime_error("[ChannelResult::Get_CovM] Inconsistent number of bins");
    }

    TH2D* h_covM = (TH2D*)h_corrM->Clone();
    h_covM->Reset("ICES");

    Int_t nBin = h_cv->GetNbinsX();
    for(Int_t i_x=0; i_x<nBin; ++i_x) {
      Int_t i_binX = i_x+1;

      Double_t value_x  = h_cv->GetBinContent(i_binX);
      Double_t relUnc_x = h_relUnc->GetBinContent(i_binX);
      Double_t absUnc_x = value_x*relUnc_x;

      for(Int_t i_y=0; i_y<nBin; ++i_y) {
        Int_t i_binY = i_y+1;

        Double_t value_y  = h_cv->GetBinContent(i_binY);
        Double_t relUnc_y = h_relUnc->GetBinContent(i_binY);
        Double_t absUnc_y = value_y*relUnc_y;

        Double_t corr_xy = h_corrM->GetBinContent(i_binX, i_binY);

        // -- convert to the covariance w.r.t. central value (e.g. dsigma/dm value)
        Double_t cov_xy = corr_xy * (absUnc_x*absUnc_y);

        h_covM->SetBinContent(i_binX, i_binY, cov_xy);
      } // -- iteration over y
    } // -- iteration over x

    return h_covM;
  }

  void Check_nBinConsistency(TString funcName, TH1D* h1, TH1D* h2) {
    Int_t nBin1 = h1->GetNbinsX();
    Int_t nBin2 = h2->GetNbinsX();

    if( nBin1 != nBin2 ) {
      printf("[%s] (nBin1, nBin2) = (%d, %d): not same\n", funcName.Data(), nBin1, nBin2);
      throw std::runtime_error("[ChannelResult::"+funcName+"] inconsistent bin numbers between two histograms");
    }
  }
};

void test() {
  ChannelResult result_mm("../Summary/UncAndCov_All_mm.root");
  // TH2D* h_cov_tot = result_mm.Get2D("covM_tot_mAxis");
  // cout << "h_cov_tot->GetBinContent(1, 1) = " << h_cov_tot->GetBinContent(1, 1) << endl;
}


