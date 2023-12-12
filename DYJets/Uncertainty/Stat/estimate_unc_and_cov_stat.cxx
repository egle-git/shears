#include "Common/DYPath.h"
#include "Common/DYOutput.h"
#include "Common/DYTool.h"

class UncSet {
public:
  UncSet(TString tag, TH1D* h_cv, TH2D* h_covM): tag_(tag) {
    h_cv_ = (TH1D*)h_cv->Clone();
    h_covM_ = (TH2D*)h_covM->Clone();
    Init();
  }

  UncSet(TString tag, TH1D* h_cv, vector<UncSet> vec_uncSet ): tag_(tag) {
    h_cv_ = (TH1D*)h_cv->Clone();
    Combine(vec_uncSet);
  }

  // template<typename T>
  // T Get(TString type) const {
  //   if( type == "absUnc" )      return h_absUnc_;
  //   else if( type == "relUnc" ) return h_relUnc_;
  //   else if( type == "covM" )   return h_covM_;
  //   else if( type == "corrM" )  return h_corrM_;
  //   else
  //     throw std::invalid_argument("[UncSet::Get] type = " + type + " is not supported");

  //   return nullptr;
  // }

  void Save(TFile* f_output) {
    f_output->cd();

    h_absUnc_->SetName("h_absUnc_"+tag_);
    h_relUnc_->SetName("h_relUnc_"+tag_);
    h_covM_->SetName("h_covM_"+tag_);
    h_corrM_->SetName("h_corrM_"+tag_);

    h_absUnc_->Write();
    h_relUnc_->Write();
    h_covM_->Write();
    h_corrM_->Write();
  }

  TH1D* Get1D(TString type) const {
    if( type == "absUnc" )      return h_absUnc_;
    else if( type == "relUnc" ) return h_relUnc_;
    else
      throw std::invalid_argument("[UncSet::Get1D] type = " + type + " is not supported");

    return nullptr;
  }

  TH2D* Get2D(TString type) const {
    if( type == "covM" )       return h_covM_;
    else if( type == "corrM" ) return h_corrM_;
    else
      throw std::invalid_argument("[UncSet::Get2D] type = " + type + " is not supported");

    return nullptr;
  }

private:
  TString tag_ = "";

  TH1D* h_cv_; // -- central value

  TH1D* h_absUnc_ = nullptr;  
  TH1D* h_relUnc_ = nullptr;
  TH2D* h_covM_   = nullptr;
  TH2D* h_corrM_  = nullptr;

  // -- init. all histograms from the covariance matrice & h_cv
  void Init() {
    Init_Unc();
    Init_CorrM();
  }

  void Init_Unc() {
    h_absUnc_ = (TH1D*)h_cv_->Clone();
    h_absUnc_->Reset("ICES");
    h_absUnc_->SetName("h_absUnc_"+tag_);

    h_relUnc_ = (TH1D*)h_absUnc_->Clone();
    h_relUnc_->SetName("h_relUnc_"+tag_);

    Int_t nBin = h_covM_->GetNbinsX();
    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;

      Double_t cv_ith = h_cv_->GetBinContent(i_bin);
      Double_t cov_ii = h_covM_->GetBinContent(i_bin, i_bin);
      Double_t absUnc = std::sqrt(cov_ii);
      Double_t relUnc = absUnc / cv_ith;

      h_absUnc_->SetBinContent(i_bin, absUnc);
      h_absUnc_->SetBinError(i_bin, 0);

      h_relUnc_->SetBinContent(i_bin, relUnc);
      h_relUnc_->SetBinError(i_bin, 0);

      // printf("[%02d bin] (cv, absUnc, relUnc) = (%lf, %lf, %.3lf)\n", i_bin, cv_ith, absUnc, relUnc);
    }
  }

  void Init_CorrM() {
    h_corrM_ = (TH2D*)h_covM_->Clone();;
    h_corrM_->Reset("ICES");

    Int_t nBin = h_covM_->GetNbinsX();
    if( nBin != h_covM_->GetNbinsY() )
      throw std::runtime_error("[UncSet::Init_CorrM] Number of X and Y bins are different!");

    for(Int_t i_x=0; i_x<nBin; ++i_x) {
      Int_t i_binX = i_x+1;

      Double_t cov_xx = h_covM_->GetBinContent(i_binX, i_binX);
      Double_t sigma_x = std::sqrt(cov_xx);
      for(Int_t i_y=i_x; i_y<nBin; ++i_y) { // -- start at i_x
        Int_t i_binY = i_y+1;

        Double_t cov_yy = h_covM_->GetBinContent(i_binY, i_binY);
        Double_t sigma_y = std::sqrt(cov_yy);

        Double_t cov_xy = h_covM_->GetBinContent(i_binX, i_binY);

        Double_t corr_xy = cov_xy / (sigma_x*sigma_y);

        h_corrM_->SetBinContent(i_binX, i_binY, corr_xy);
        h_corrM_->SetBinError(i_binX, i_binY, 0);

        h_corrM_->SetBinContent(i_binY, i_binX, corr_xy); // -- symmetric
        h_corrM_->SetBinError(i_binY, i_binX, 0);
      }
    }
  }

  void Combine(vector<UncSet> vec_uncSet) {
    for(const auto& uncSet : vec_uncSet ) {
      TH2D* h_covM_this = (TH2D*)( uncSet.Get2D("covM")->Clone() );
      if( !h_covM_ ) h_covM_ = h_covM_this;
      else           h_covM_->Add( h_covM_this );
    }

    Init();
  }
};

class UncEstimator_Stat {
public:
  UncEstimator_Stat(TString channel): channel_(channel) { }

  void Use_Fake(Bool_t flag = kTRUE) { useFake_ = flag; }

  void EstimateAndSave() {
    shearsPath_ = DYTool::path_default+"/"+channel_;
    fileName_output_ = "Uncertainty_and_Covariance_Stat_"+channel_+".root";
    TFile* f_output = TFile::Open(fileName_output_, "RECREATE");

    Construct_TUnfold();

    TH2D* h_covM_stat_data = (TH2D*)unfold_->GetEmatrixInput("h_covM_stat_data");
    TH2D* h_covM_stat_DYMC = (TH2D*)unfold_->GetEmatrixSysUncorr("h_covM_stat_DYMC");
    TH2D* h_covM_stat_bkgMC = (TH2D*)unfold_->GetEmatrixSysBackgroundUncorr("bkgMC", "h_covM_stat_bkgMC");
    TH2D* h_covM_stat_bkgDYFake = (TH2D*)unfold_->GetEmatrixSysBackgroundUncorr("DYFake", "h_covM_stat_bkgDYFake");
    TH2D* h_covM_stat_tot_TUnfold = (TH2D*)unfold_->GetEmatrixTotal("h_covM_stat_tot_TUnfold");

    uncSet_stat_data_  = new UncSet("stat_data",  h_cv_, h_covM_stat_data);
    uncSet_stat_DYMC_  = new UncSet("stat_DYMC",  h_cv_, h_covM_stat_DYMC);
    uncSet_stat_bkgMC_ = new UncSet("stat_bkgMC", h_cv_, h_covM_stat_bkgMC);
    uncSet_stat_bkgDYFake_ = new UncSet("stat_bkgDYFake", h_cv_, h_covM_stat_bkgDYFake);
    uncSet_stat_tot_TUnfold_ = new UncSet("stat_tot_TUnfold", h_cv_, h_covM_stat_tot_TUnfold);

    vector<UncSet> vec_uncSet_stat_MC = {*uncSet_stat_DYMC_, *uncSet_stat_bkgMC_, *uncSet_stat_bkgDYFake_};
    uncSet_stat_totMC_ = new UncSet("stat_totMC", h_cv_, vec_uncSet_stat_MC);

    vector<UncSet> vec_uncSet_stat_tot = {*uncSet_stat_totMC_, *uncSet_stat_data_};

    if( useFake_) {
      TH2D* h_covM_stat_bkgFakeLep = (TH2D*)unfold_->GetEmatrixSysBackgroundUncorr("bkgFakeLep", "h_covM_stat_bkgFakeLep");
      uncSet_stat_bkgFakeLep_      = new UncSet("stat_bkgFakeLep", h_cv_, h_covM_stat_bkgFakeLep);
      vec_uncSet_stat_tot.push_back( *uncSet_stat_bkgFakeLep_ );
    }

    uncSet_stat_tot_ = new UncSet("stat_tot", h_cv_, vec_uncSet_stat_tot);

    uncSet_stat_data_->Save(f_output);
    uncSet_stat_DYMC_->Save(f_output);
    uncSet_stat_bkgMC_->Save(f_output);
    uncSet_stat_bkgDYFake_->Save(f_output);
    if( useFake_ ) uncSet_stat_bkgFakeLep_->Save(f_output);
    uncSet_stat_totMC_->Save(f_output);
    uncSet_stat_tot_->Save(f_output);

    uncSet_stat_tot_TUnfold_->Save(f_output);

    Print_Summary();
  }

private:
  TString channel_;
  TString shearsPath_;
  TString fileName_output_;

  Bool_t useFake_ = kTRUE; // -- default: true

  TH1D* h_cv_;

  TUnfoldDensity* unfold_;

  UncSet* uncSet_stat_data_;
  UncSet* uncSet_stat_DYMC_;
  UncSet* uncSet_stat_bkgMC_;
  UncSet* uncSet_stat_bkgDYFake_;
  UncSet* uncSet_stat_bkgFakeLep_; // -- fake lepton bkg.
  UncSet* uncSet_stat_totMC_; // -- DY MC + bkg MC
  UncSet* uncSet_stat_tot_; // -- DY MC + bkg MC + data (+ fake lepton bkg.)

  UncSet* uncSet_stat_tot_TUnfold_; // -- total output from TUnfold (for validation)

  void Construct_TUnfold() {
    Run2Output *output = new Run2Output(shearsPath_);
    DYRun2Result* result_cv = new DYRun2Result(output);
    if( useFake_ ) DYTool::Set_Fake(channel_, result_cv);
    result_cv->Produce();
    h_cv_ = result_cv->Get_AllEra("unfolded", "data");

    TH2D* h_allEra_migM  = result_cv->Get_AllEra_MigM();

    TH1D* h_allEra_bkgMC      = result_cv->Get_AllEra("reco", "bkgMC");
    TH1D* h_allEra_DYFake = result_cv->Get_AllEra("reco", "DYFake");
    TH1D* h_allEra_data   = result_cv->Get_AllEra("reco", "data");

    TUnfold::ERegMode            regMode        = TUnfold::kRegModeSize;
    TUnfold::EConstraint         constraintMode = TUnfold::kEConstraintNone;
    TUnfoldDensity::EDensityMode densityFlags   = TUnfoldDensity::kDensityModeBinWidth;

    double tau = 0; // -- regularization strength (0: no reg.)

    unfold_ = new TUnfoldDensity(h_allEra_migM, TUnfold::kHistMapOutputVert, regMode, constraintMode, densityFlags);

    double bkgNorm = 1.0; // -- global normalization factor for the background: do not scale it!
    double err_bkgNorm = 0.0; // -- scale error for background: we do not use it

    unfold_->SubtractBackground(h_allEra_DYFake, "DYFake", bkgNorm, err_bkgNorm);
    unfold_->SubtractBackground(h_allEra_bkgMC, "bkgMC", bkgNorm, err_bkgNorm);
    if( useFake_ ) {
      TH1D* h_allEra_bkgFakeLep = result_cv->Get_AllEra("reco", "fake"); // -- fake lepton bkg.
      unfold_->SubtractBackground(h_allEra_bkgFakeLep, "bkgFakeLep", bkgNorm, err_bkgNorm);
    }

    // -- do unfolding    
    unfold_->DoUnfold(tau, h_allEra_data);
  }

  void Print_Summary() {
    cout << "============ [summary] ============" << endl;
    if( !useFake_) 
      cout << "*** CAVEAT: fake histograms are not considered yet ***" << endl;
    cout << "[input]" << endl;
    cout << "  shears result: " << shearsPath_ << endl;
    cout << "[output]" << endl;
    cout << "  uncertainties and covariance matrices: " << fileName_output_ << endl;
    cout << "[Warn (231130)] The fake lepton background histogram should have stat. uncertainty only on its error!: " << endl;
    cout << "===================================" << endl;
  }

};

void estimate_unc_and_cov_stat() {
  TH1::AddDirectory(kFALSE);

  UncEstimator_Stat uncEstimator_ee("ee");
  uncEstimator_ee.EstimateAndSave();

  UncEstimator_Stat uncEstimator_mm("mm");
  uncEstimator_mm.EstimateAndSave();
}