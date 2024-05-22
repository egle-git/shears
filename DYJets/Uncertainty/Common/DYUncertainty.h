#pragma once
#include "DYOutput.h"

class Uncertainty {
public:
  Uncertainty(): systTag_("undefined") {
    Set_UncEstMethod("null");
    Set_CovEstMethod("null");
  }

  Uncertainty(TString systTag): systTag_(systTag) {
    Set_UncEstMethod("null");
    Set_CovEstMethod("null");
  }

  Uncertainty(TString systTag, TString uncEstMethod, TString covEstMethod): systTag_(systTag) {
    Set_UncEstMethod(uncEstMethod);
    Set_CovEstMethod(covEstMethod);
  }

  // -- retrieve Uncertainty object from the histograms in the file
  Uncertainty(TString systTag, TString fileName) {
    systTag_ = systTag;

    // -- uncertainty
    h_absUnc_ = PlotTool::Get_Hist(fileName, "h_absUnc_"+systTag_);
    h_relUnc_ = PlotTool::Get_Hist(fileName, "h_relUnc_"+systTag_);

    // -- matrices
    h_covM_  = PlotTool::Get_Hist2D(fileName, "h_covM_"+systTag_);
    h_corrM_ = PlotTool::Get_Hist2D(fileName, "h_corrM_"+systTag_);
  }

  // -- if you want to change the tag... (for saving with a different name)
  void Set_Tag(TString tag) { systTag_ = tag; }

  void Set_CentralHist( TH1D* h_cv ) { h_cv_ = (TH1D*)h_cv->Clone(); }
  void Set_AltHist( vector<TH1D*> vec_altHist ) { vec_altHist_ = vec_altHist; }

  TString Tag() const { return systTag_; }
  TH1D* RelUnc() const { return h_relUnc_; }
  TH1D* AbsUnc() const { return h_absUnc_; }
  TH2D* CovM() const   { return h_covM_; }
  TH2D* CorrM() const  { return h_corrM_; }

  // -- always construct the covariance matrix first & take the diagonal term as the uncertainty
  // -- to be consistent between uncertainty and the cov. matrices for the future
  void Estimate() {
    SetFormat_CovM();

    if( uncEstMethod_ == UncEstMethod::smearing ) EstimateCov_Smearing();
    else if( uncEstMethod_ == UncEstMethod::alternative ||
             uncEstMethod_ == UncEstMethod::oneSigmaShift ) EstimateCov_TakeLargerUnc();
    else
      throw std::invalid_argument("[Estimate] This uncertainty estimation method is not supported");

    Set_CorrM();
    ExtractUnc_FromCov();
  }

  // -- combine the uncertainty (quadrature sum) and the covariance matrices
  void Combine(vector<Uncertainty> vec_unc ) {
    vector<TH1D*> vec_absUncHist;
    vector<TH1D*> vec_relUncHist;
    for(const auto& unc : vec_unc ) {
      vec_absUncHist.push_back( unc.AbsUnc() );
      vec_relUncHist.push_back( unc.RelUnc() );

      // -- cov matrix: simple add
      TH2D* h_covM_this = unc.CovM();
      if( h_covM_ == nullptr ) h_covM_ = (TH2D*)h_covM_this->Clone();
      else                     h_covM_->Add( h_covM_this );
    }

    h_absUnc_ = PlotTool::QuadSum_Hist(vec_absUncHist);
    h_relUnc_ = PlotTool::QuadSum_Hist(vec_relUncHist);

    // -- correlation matrix: calculate after all cov. matrices are added
    Set_CorrM();
  }

  void Save(TFile* f_output) {
    f_output->cd();

    // -- write objects
    h_absUnc_->SetName("h_absUnc_"+systTag_);
    h_relUnc_->SetName("h_relUnc_"+systTag_);
    h_covM_->SetName("h_covM_"+systTag_);
    h_corrM_->SetName("h_corrM_"+systTag_);

    h_absUnc_->Write();
    h_relUnc_->Write();
    h_covM_->Write();
    h_corrM_->Write();
  }

private:
  enum class UncEstMethod {
    // -- nothing
    // -- can be used for the combined uncertainties
    null,
    // -- change something and reproduce results (e.g. model uncertainty of unfolding)
    // -- take the larger uncertainty if multiple alternative values are provided
    alternative, 
    // -- change +-1sigma (e.g. pileup)
    // -- take the larger uncertainty if multiple alternative values are provided
    oneSigmaShift,
    // -- smearing within uncertainty and produce multiple (e.g. 100) results (e.g. efficiency SF)
    smearing
  };

  enum class CovEstMethod {
    null, // -- nothing. can be used for the combined uncertainties
    fullyCorr, // -- 100% correlation (fully corr. or fully anti-corr.)
    noCorr, // -- no correlation
    smearing // -- partially correlated: estimated with smeared values
  };

  TString systTag_;
  UncEstMethod uncEstMethod_;
  CovEstMethod covEstMethod_;

  // -- ingredients
  TH1D* h_cv_; // -- cv = central value
  vector<TH1D*> vec_altHist_; // -- alternative histograms with systematic variations

  // -- uncertainty
  TH1D* h_absUnc_ = nullptr;
  TH1D* h_relUnc_ = nullptr;

  // -- matrices
  TH2D* h_covM_ = nullptr;
  TH2D* h_corrM_ = nullptr;

  void Set_UncEstMethod(TString tstr_estMethod) {
    if( tstr_estMethod == "alternative" )        uncEstMethod_ = UncEstMethod::alternative;
    else if( tstr_estMethod == "oneSigmaShift" ) uncEstMethod_ = UncEstMethod::oneSigmaShift;
    else if( tstr_estMethod == "smearing" )      uncEstMethod_ = UncEstMethod::smearing;
    else if( tstr_estMethod == "null" )          uncEstMethod_ = UncEstMethod::null;
    else
      throw std::invalid_argument("Uncertainty Estimation method = " + tstr_estMethod + " is not supported");
  }

  void Set_CovEstMethod(TString tstr_estMethod) {
    if( tstr_estMethod == "fullyCorr" )     covEstMethod_ = CovEstMethod::fullyCorr;
    else if( tstr_estMethod == "noCorr" )   covEstMethod_ = CovEstMethod::noCorr;
    else if( tstr_estMethod == "smearing" ) covEstMethod_ = CovEstMethod::smearing;
    else if( tstr_estMethod == "null" )     covEstMethod_ = CovEstMethod::null;
    else
      throw std::invalid_argument("Uncertainty Estimation method = " + tstr_estMethod + " is not supported");
  }

  void EstimateCov_TakeLargerUnc() {
    // -- calculate the uncertainty first
    Int_t nBin = h_cv_->GetNbinsX();

    // std::map<Int_t, vector<Double_t>> map_altValue;
    // FillMap_AltValue(map_altValue);

    // // -- diff(=alt.-cv) > 0 or < 0?
    // // -- need to fill cov_ij for fully-correlated (rho_ij = 1 or -1) case
    // std::map<Int_t, Int_t> map_sign;

    // for(Int_t i=0; i<nBin; ++i) {
    //   Int_t i_bin = i+1;
    //   Double_t cv_ith = h_cv_->GetBinContent(i_bin);

    //   Double_t absUnc = -1;
    //   Int_t sign = 1;
    //   vector<Double_t> vec_altValue = map_altValue[i_bin];
    //   for(const auto& altValue : vec_altValue ) {
    //     Double_t diff = altValue - cv_ith;
    //     Double_t absDiff = std::abs(diff);

    //     if( absDiff > absUnc ) {
    //       absUnc = absDiff;
    //       sign = diff > 0 ? 1 : -1;
    //     }
    //   } // -- iteration over alt. values

    //   map_sign.insert( std::make_pair(i_bin, sign) );

    //   h_covM_->SetBinContent(i_bin, i_bin, absUnc*absUnc);
    // }

    // // -- fill the off-diagonal terms
    // for(Int_t i=0; i<nBin; ++i) {
    //   Int_t i_bin = i+1;
    //   Double_t absUnc_ith = std::sqrt( h_covM_->GetBinContent(i_bin, i_bin) );
    //   // -- start at i+1
    //   for(Int_t j=i+1; j<nBin; ++j) {
    //     Int_t j_bin = j+1;
    //     Double_t absUnc_jth = std::sqrt( h_covM_->GetBinContent(j_bin, j_bin) );

    //     Double_t rho_ij;
    //     if( covEstMethod_ == CovEstMethod::noCorr )        rho_ij = 0;
    //     else if( covEstMethod_ == CovEstMethod::fullyCorr) rho_ij = map_sign[i_bin]*map_sign[j_bin]; // -- fully corr. or fully anti-corr.
    //     else
    //       throw std::invalid_argument("[EstimateCov_TakeLargerUnc] This covariance estimation method is not supported");

    //     Double_t cov_ij = rho_ij * absUnc_ith * absUnc_jth;

    //     h_covM_->SetBinContent(i_bin, j_bin, cov_ij);
    //     h_covM_->SetBinError(i_bin, j_bin, 0);

    //     h_covM_->SetBinContent(j_bin, i_bin, cov_ij); // -- fill (j,i) also (symmetric)
    //     h_covM_->SetBinError(j_bin, i_bin, 0);
    //   } // -- iteration over j
    // }// -- iteration over i



    // Int_t nHist = vec_altHist_.size();
    // vector<TH2D*> vec_covM;
    // for(Int_t i=0; i<nHist; ++i)
    //   vec_covM.push_back( (TH2D*)h_covM_->Clone() );

    // for(Int_t i=0; i<nBin; ++i) {
    //   Int_t i_bin = i+1;
    //   Double_t cv_ith = h_cv_->GetBinContent(i_bin);
    //   vector<Double_t> vec_altValue_ith = map_altValue[i_bin];

    //   for(Int_t j=i; j<nBin; ++j) { // -- start at j=i
    //     Int_t j_bin = j+1;
    //     Double_t cv_jth = h_cv_->GetBinContent(j_bin);
    //     vector<Double_t> vec_altValue_jth = map_altValue[j_bin];

    //     for(Int_t i_hist=0; i_hist<nHist; ++i_hist) {
    //       Double_t altValue_ith = vec_altValue_ith[i_hist];
    //       Double_t altValue_jth = vec_altValue_jth[i_hist];

    //       Double_t cov_ij;
    //       if( i == j ) {
    //         cov_ij = (altValue_ith-cv_ith)*(altValue_jth-cv_jth);
    //       }
    //       else {
    //         if( covEstMethod_ == CovEstMethod::noCorr ) cov_ij = 0;
    //         else if( covEstMethod_ == CovEstMethod::fullyCorr )
    //           cov_ij = (altValue_ith-cv_ith)*(altValue_jth-cv_jth);
    //       }
    //       vec_covM[i_hist]->SetBinContent(i_bin, j_bin, cov_ij);
    //       vec_covM[i_hist]->SetBinContent(j_bin, i_bin, cov_ij);
    //     } // -- iteration over hists
    //   } // -- iteration over j
    // } // -- iteration over i

    // -- another method
    // ---- 1) estimate covariance matrix for each alternative histograms
    // ---- 2) for each (i, j) bin, take the covariance value with the largest absolute value
    // // -- estimate cov for each alt. histogram
    // vector<TH2D*> vec_covM;
    // for(const auto& h_alt : vec_altHist_ )
    //   vec_covM.push_back( EstimateCov(h_cv_, h_alt) );

    // // -- compare among covariance histograms
    // // -- for each bin, take the one with the largest absolute value (fullyCorr. case)
    // for(Int_t i=0; i<nBin; ++i) {
    //   Int_t i_bin = i+1;

    //   for(Int_t j=i; j<nBin; ++j) { // -- start at j=i
    //     Int_t j_bin = j+1;

    //     Double_t cov_ij;
    //     if( covEstMethod_ == CovEstMethod::noCorr ) 
    //       cov_ij = 0;
    //     else if( covEstMethod_ == CovEstMethod::fullyCorr )
    //       cov_ij = GetCov_LargestAbsoluteValue(vec_covM, i_bin, j_bin);
    //     else
    //       throw std::invalid_argument("This CovEstMethod is not supported");

    //     h_covM_->SetBinContent(i_bin, j_bin, cov_ij);
    //     h_covM_->SetBinContent(j_bin, i_bin, cov_ij);
    //   } // -- iteration over j
    // } // -- iteration over i

    // -- 1) calcluate the uncertainty first: take the larger uncertainty
    // -- 2) using the uncertainty, construct the covariance matrix
    // ---- cov(i, j) = rho * unc(i) * unc(j) (fully-correlated case)
    // ---- rho = (alt_i - cv_i) * (alt_j - cv_j) > 0? 1.0 (correlated) : -1.0 (anti-correlated)

    std::map<Int_t, vector<Double_t>> map_altValue;
    FillMap_AltValue(map_altValue);

    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;
      Double_t cv_ith = h_cv_->GetBinContent(i_bin);

      // -- take the largest value
      Double_t cov_ii = -1;
      vector<Double_t> vec_altValue = map_altValue[i_bin];
      for(const auto& altValue : vec_altValue ) {
        Double_t cov_ii_this = (altValue - cv_ith)*(altValue - cv_ith);
        if( cov_ii_this > cov_ii )
          cov_ii = cov_ii_this;
      } // -- iteration over alt. values

      h_covM_->SetBinContent(i_bin, i_bin, cov_ii);
    }

    // -- construct the off-diagonal terms
    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;
      Double_t cv_ith = h_cv_->GetBinContent(i_bin);
      Double_t alt_ith = vec_altHist_[0]->GetBinContent(i_bin);
      Double_t unc_ith = std::sqrt(h_covM_->GetBinContent(i_bin, i_bin));

      for(Int_t j=i+1; j<nBin; j++) { // -- start at i+1
        Int_t j_bin = j+1;
        Double_t cv_jth = h_cv_->GetBinContent(j_bin);
        Double_t alt_jth = vec_altHist_[0]->GetBinContent(j_bin);
        Double_t unc_jth = std::sqrt(h_covM_->GetBinContent(j_bin, j_bin));

        Double_t rho_ij = 0; // -- correlation
        if( covEstMethod_ == CovEstMethod::noCorr ) 
          rho_ij = 0;
        else if( covEstMethod_ == CovEstMethod::fullyCorr ) // 100% corr. or 100% anti-corr.
          rho_ij = (alt_ith-cv_ith)*(alt_jth-cv_jth) > 0 ? 1.0 : -1.0;

        Double_t cov_ij = rho_ij * unc_ith * unc_jth;
        h_covM_->SetBinContent(i_bin, j_bin, cov_ij);
        h_covM_->SetBinContent(j_bin, i_bin, cov_ij);
      }
    }
  }

  Double_t GetCov_LargestAbsoluteValue(vector<TH2D*>& vec_covM, Int_t i_bin, Int_t j_bin) {
    Double_t abscov_ij_largest = 0;
    Double_t cov_ij;

    for(const auto& h_covM : vec_covM ) {
      Double_t cov_ij_this = h_covM->GetBinContent(i_bin, j_bin);

      // -- take the one with the largest "absolute" value
      if(std::abs(cov_ij_this) > abscov_ij_largest ) {
        abscov_ij_largest = std::abs(cov_ij_this);
        cov_ij = cov_ij_this;
      }
    }

    return cov_ij;
  }

  TH2D* EstimateCov(TH1D* h_cv, TH1D* h_alt) {
    TH2D* h_covM_this = (TH2D*)h_covM_->Clone();

    Int_t nBin = h_cv->GetNbinsX();
    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;
      Double_t cv_ith = h_cv->GetBinContent(i_bin);
      Double_t altValue_ith = h_alt->GetBinContent(i_bin);

      for(Int_t j=i; j<nBin; ++j) { // -- start at j=i
        Int_t j_bin = j+1;
        Double_t cv_jth = h_cv->GetBinContent(j_bin);
        Double_t altValue_jth = h_alt->GetBinContent(j_bin);

        Double_t cov_ij;
        if( i == j ) { // -- i.e. uncertainty^2
          cov_ij = (altValue_ith-cv_ith)*(altValue_jth-cv_jth);
        }
        else {
          if( covEstMethod_ == CovEstMethod::noCorr ) cov_ij = 0;
          else if( covEstMethod_ == CovEstMethod::fullyCorr )
            cov_ij = (altValue_ith-cv_ith)*(altValue_jth-cv_jth); // -- keep sign! (rho_ij = 1 or -1)
        }
        h_covM_this->SetBinContent(i_bin, j_bin, cov_ij);
        h_covM_this->SetBinContent(j_bin, i_bin, cov_ij);
      } // -- iteration over j
    } // -- iteration over i

    return h_covM_this;
  }

  void EstimateCov_Smearing() {
    if( covEstMethod_ != CovEstMethod::smearing )
      throw std::invalid_argument("[EstimateCov_Smearing] This covariance matrix estimation method is not supported");

    Int_t nBin = h_cv_->GetNbinsX();

    std::map<Int_t, vector<Double_t>> map_altValue;
    FillMap_AltValue(map_altValue);

    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;
      Double_t cv_ith = h_cv_->GetBinContent(i_bin);

      // -- start at i
      for(Int_t j=i; j<nBin; ++j) {
        Int_t j_bin = j+1;
        Double_t cv_jth = h_cv_->GetBinContent(j_bin);
        Double_t cov_ij = Calc_Cov( cv_ith, map_altValue[i_bin], 
                                    cv_jth, map_altValue[j_bin] );

        h_covM_->SetBinContent(i_bin, j_bin, cov_ij);
        h_covM_->SetBinError(i_bin, j_bin, 0);
        if( i_bin != j_bin ) {
          h_covM_->SetBinContent(j_bin, i_bin, cov_ij); // -- fill (j,i) also (symmetric)
          h_covM_->SetBinError(j_bin, i_bin, 0);
        }
      } // -- iteration over j-th bins
    } // -- iteration over i-th bins
  }

  void FillMap_AltValue(std::map<Int_t, vector<Double_t>>& map_altValue) {
    Int_t nBin = h_cv_->GetNbinsX();

    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;

      vector<Double_t> vec_altValue_ith;
      Fill_Vector_AltValue(i_bin, vec_altValue_ith);

      map_altValue.insert( std::make_pair(i_bin, vec_altValue_ith) );
    }
  }

  void Fill_Vector_AltValue(Int_t i_bin, vector<Double_t>& vec_altValue_ith) {
    for(const auto& h_alt : vec_altHist_ )
      vec_altValue_ith.push_back( h_alt->GetBinContent(i_bin) ); 
  }

  Double_t Calc_Cov(Double_t cv_ith, vector<Double_t> vec_altValue_ith, 
                    Double_t cv_jth, vector<Double_t> vec_altValue_jth) {
    if( vec_altValue_ith.size() != vec_altValue_jth.size() )
      throw std::invalid_argument("[Calc_Cov] vec_altValue_ith.size() != vec_altValue_jth.size()");

    Int_t N = (Int_t)vec_altValue_ith.size();

    // Double_t sum_ij = 0;
    // for(Int_t k=0; k<N; ++k)
    //   sum_ij = sum_ij + vec_altValue_ith[k]*vec_altValue_jth[k];

    // return (1.0/(N-1))*sum_ij - (N/(N-1))*cv_ith*cv_jth;

    Double_t sum_diff_ij = 0;
    for(Int_t k=0; k<N; ++k)
      sum_diff_ij = sum_diff_ij + (vec_altValue_ith[k] - cv_ith)*(vec_altValue_jth[k] - cv_jth);

    return sum_diff_ij / N;
  }

  void SetFormat_CovM() {
    Int_t nBin = h_cv_->GetNbinsX();
    Double_t* arr_binEdge = Get_BinEdge(nBin, h_cv_);

    h_covM_ = new TH2D("h_covM_"+systTag_, "", nBin, arr_binEdge, nBin, arr_binEdge );
  }

  Double_t* Get_BinEdge(Int_t nBin, TH1D* h) {
    Double_t* arr_binEdge = new Double_t[nBin+1];

    for(Int_t i=0; i<nBin+1; ++i) {
      Int_t i_bin = i+1;
      arr_binEdge[i] = h->GetBinLowEdge(i_bin);
    }

    return arr_binEdge;
  }

  void Set_CorrM() {
    h_corrM_ = (TH2D*)h_covM_->Clone();
    h_corrM_->SetName("h_corrM_"+systTag_);

    Int_t nBin = h_covM_->GetNbinsX();

    for(Int_t i_x=0; i_x<nBin; ++i_x) {
      Int_t i_bin = i_x+1;

      Double_t sigma_i = sqrt(h_covM_->GetBinContent(i_bin, i_bin));

      for(Int_t i_y=0; i_y<nBin; ++i_y) {
        Int_t j_bin = i_y+1;

        Double_t sigma_j = sqrt(h_covM_->GetBinContent(j_bin, j_bin));

        Double_t cov_ij = h_covM_->GetBinContent(i_bin, j_bin);
        Double_t corr_ij = cov_ij / (sigma_i*sigma_j);

        // -- for covEstMethod_ == CovEstMethod::smearing
        // -- due to stat. fluctuation, sometimes corr_ij can slightly be over 1.0
        // -- to be consistent, update the cov_ij as sigma_i * sigma_j & corr_ij = 1.0
        // -- do the same thing for the case of corr_ij is slightly lower than -1.0
        // -- another case: i=j -> corr_ij should be 1.0
        // -- due to the limited precision, sometimes it gives the value that slightly larger than 1.0 -> fix this
        if( (covEstMethod_ == CovEstMethod::smearing || i_x==i_y) && 
            (corr_ij > 1.0 || corr_ij < -1.0) ) {
          Bool_t adjusted = kFALSE;
          Double_t cov_ij_before  = cov_ij;
          Double_t corr_ij_before = corr_ij;

          if( corr_ij > 1.0 && std::abs(corr_ij-1.0) < 1e-3 ) {
            adjusted = kTRUE;
            cov_ij = sigma_i*sigma_j;
            h_covM_->SetBinContent(i_bin, j_bin, cov_ij);
            corr_ij = 1.0;
          }

          if( corr_ij < -1.0 && std::abs(corr_ij+1.0) < 1e-3 ) {
            adjusted = kTRUE;
            cov_ij = (-1.0)*sigma_i*sigma_j;
            h_covM_->SetBinContent(i_bin, j_bin, cov_ij);
            corr_ij = -1.0;
          }

          if( adjusted ) {
            printf("*covM, corrM are adjusted* (systTag = %s) [%02d, %02d] bin: after/before = %lf (cov), %lf (corr)\n",
                   systTag_.Data(), i_bin, j_bin, cov_ij/cov_ij_before, corr_ij/corr_ij_before);
          }
        }

        if( corr_ij < -1.0 || corr_ij  > 1.0 ) {
          printf("[%02d, %02d] bin: (sigma_i, sigma_j, cov_ij, corr_ij) = (%.3lf, %.3lf, %.3lf, %lf)\n",
            i_bin, j_bin, sigma_i, sigma_j, cov_ij, corr_ij);
          throw std::runtime_error("systTag_ = "+systTag_+": corr_ij < -1.0 || corr_ij  > 1.0!");
        }

        h_corrM_->SetBinContent(i_bin, j_bin, corr_ij);
      } // -- iteration over y
    } // --iteration over x
  }

  void ExtractUnc_FromCov() {
    h_absUnc_ = (TH1D*)h_cv_->Clone();
    h_absUnc_->Reset("ICES");
    h_absUnc_->SetName("h_absUnc_"+systTag_);

    h_relUnc_ = (TH1D*)h_absUnc_->Clone();
    h_relUnc_->SetName("h_relUnc_"+systTag_);

    Int_t nBin = h_covM_->GetNbinsX();
    for(Int_t i=0; i<nBin; i++) {
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

}; // -- class Uncertainty








