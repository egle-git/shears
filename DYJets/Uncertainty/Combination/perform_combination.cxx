#include "ChannelResult.h"
#include "Common/DYPath.h"
#include "Common/DYOutput.h"

class Combinator {
public:
  Combinator(TString type): type_(type) {}

  void Set_Theory(TH1D* h_dsigdm_theory) {
    h_dsigdm_theory_ = (TH1D*)h_dsigdm_theory->Clone();
  }

  void Set(TString channel, TString fileName) {
    if( channel == "ee" ) result_ee_ = new ChannelResult(fileName);
    if( channel == "mm" ) result_mm_ = new ChannelResult(fileName);
  }

  void Set_CorrelatedUnc_BtwChannel(vector<TString> vec_uncType) { vec_uncType_corr_em_ = vec_uncType; }

  void Set_NoCorr_BtwChannel(Bool_t flag = kTRUE) { noCorr_em_ = flag; }

  void Combine() {
    if( !result_ee_ || !result_mm_ )
      throw std::runtime_error("***[Combinator::Combine] ChannelResult is not set yet!");

    plotDirPath_ = "./plot/"+type_;
    if( noCorr_em_ ) plotDirPath_ += "/noCorr_em";
    DYTool::Make_Dir(plotDirPath_);

    nMassBin_ = result_ee_->Get("dsigdm_data")->GetNbinsX();

    Comparison_Channel();

    if( !noCorr_em_ ) {
      printf("***[Combinator::Combine] Correlated uncertainties between channels: ");
      for(const auto& uncType : vec_uncType_corr_em_ )
        printf("%s / ", uncType.Data());
      printf("\n");

      Construct_Covariance_BtwChannel();
    }

    // -- 2N matrices
    TMatrixD m2N_dsigdm = Convert_Matrix2N_dsigdm();
    TMatrixD m2N_lambda = Calc_Coefficient();
    TMatrixD m2N_covM   = Construct_Matrix2N();
    TMatrixD m2N_corrM  = Make_CorrM(m2N_covM, kTRUE);
    Validation_TMatrixD(m2N_covM, m2N_corrM);

    InitHist_CombinedResult();

    Calc_CentralValue_Combined(m2N_dsigdm, m2N_lambda);

    TMatrixD mCov_ll = Calc_Cov_Combined(m2N_lambda, m2N_covM);

    Construct_CombinedHist(mCov_ll);

    ProducePlot_Coefficient(m2N_lambda);
    ProducePlot_CombinedResult();
    ProducePlot_2D(m2N_covM,  "covM", "tot", kTRUE);
    ProducePlot_2D(m2N_corrM, "corrM", "tot", kTRUE);

    Save();
  }

private:
  TString type_ = "";

  Int_t nMassBin_ = 0;
  ChannelResult* result_ee_ = nullptr;
  ChannelResult* result_mm_ = nullptr;

  TString plotDirPath_ = "";

  vector<TString> vec_uncType_corr_em_;

  std::map<TString, TH2D*> map_covM_em_;

  // -- combined results
  TH1D* h_dsigdm_ll_ = nullptr;
  TH1D* h_relUnc_ll_ = nullptr;
  TH1D* h_absUnc_ll_ = nullptr;
  TH2D* h_covM_ll_ = nullptr;
  TH2D* h_corrM_ll_ = nullptr;

  // -- theory result (mass-bin-number axis)
  TH1D* h_dsigdm_theory_ = nullptr;

  // -- test: no correlation between two channels
  Bool_t noCorr_em_ = kFALSE;

  // -- compare TMatrixD values vs. TH2D values in the input file (independent with ChannelResult)
  void Validation_TMatrixD(TMatrixD& m2N_covM, TMatrixD& m2N_corrM) {
    TString fileName_ee = result_ee_->FileName();
    TString fileName_mm = result_mm_->FileName();

    TH1D* h_dsigdm_ee = PlotTool::Get_Hist(fileName_ee, "h_dsigdm_data");
    TH1D* h_dsigdm_mm = PlotTool::Get_Hist(fileName_mm, "h_dsigdm_data");

    TH1D* h_relUnc_tot_ee = PlotTool::Get_Hist(fileName_ee, "h_relUnc_tot_mAxis");
    TH1D* h_relUnc_tot_mm = PlotTool::Get_Hist(fileName_mm, "h_relUnc_tot_mAxis");

    // -- comparison: relUnc (i.e. diagonal terms) from mCov vs. from h_relUnc
    for(Int_t i=0; i<nMassBin_; ++i) {
      Int_t i_bin = i+1;

      // -- relUnc from mCov diagonal term
      Double_t dsigdm_ee = h_dsigdm_ee->GetBinContent(i_bin);
      Double_t dsigdm_mm = h_dsigdm_mm->GetBinContent(i_bin);

      Double_t mCov_ee = m2N_covM[i][i];
      Double_t mCov_mm = m2N_covM[i+nMassBin_][i+nMassBin_];

      Double_t mCov_absUnc_ee = sqrt(mCov_ee);
      Double_t mCov_absUnc_mm = sqrt(mCov_mm);

      Double_t mCov_relUnc_ee = mCov_absUnc_ee / dsigdm_ee;
      Double_t mCov_relUnc_mm = mCov_absUnc_mm / dsigdm_mm;

      // -- relUnc from the input histogram
      Double_t hRelUnc_ee = h_relUnc_tot_ee->GetBinContent(i_bin);
      Double_t hRelUnc_mm = h_relUnc_tot_mm->GetBinContent(i_bin);

      // if( (mCov_relUnc_ee != hRelUnc_ee) || (mCov_relUnc_mm != hRelUnc_mm) ) {
      if( !IsDiffSmall(mCov_relUnc_ee, hRelUnc_ee) || !IsDiffSmall(mCov_relUnc_mm, hRelUnc_mm) ) {
        printf("[%02d bin]\n", i_bin);
        printf("  [ee] RelUnc. (from mCov, from h_relUnc) = (%lf, %lf)\n", mCov_relUnc_ee, hRelUnc_ee);
        printf("  [mm] RelUnc. (from mCov, from h_relUnc) = (%lf, %lf)\n", mCov_relUnc_mm, hRelUnc_mm);
        throw std::runtime_error("Inconsistent diagonal terms in m2N_covM");
      }
    } // -- iter over i
    printf("***[Combinator::Validation_TMatrixD] Pass the test: same diagonal terms with the input rel. uncertainty\n");


    // -- comparison: covariance matrix (from mCov vs. from h_cov)
    TH2D* h2D_cov_ee = PlotTool::Get_Hist2D(fileName_ee, "h_covM_tot_mAxis");
    TH2D* h2D_cov_mm = PlotTool::Get_Hist2D(fileName_mm, "h_covM_tot_mAxis");

    Double_t lumi = DYTool::GetLumi("all");
    for(Int_t i_x=0; i_x<nMassBin_; ++i_x) {
      Int_t i_binX = i_x+1;

      Double_t binWidth_x = h_dsigdm_ee->GetBinWidth(i_binX);

      for(Int_t i_y=0; i_y<nMassBin_; ++i_y) {
        Int_t i_binY = i_y+1;

        Double_t binWidth_y = h_dsigdm_ee->GetBinWidth(i_binY);

        // -- covariance from m2N_covM
        Double_t mCov_ee = m2N_covM[i_x][i_y];
        Double_t mCov_mm = m2N_covM[i_x+nMassBin_][i_y+nMassBin_];

        // -- covariance from the input histogram
        Double_t hCovM_ee = h2D_cov_ee->GetBinContent(i_binX, i_binY);
        Double_t hCovM_mm = h2D_cov_mm->GetBinContent(i_binX, i_binY);

        // -- normalize them: convert to the value w.r.t. dsigma/dm
        hCovM_ee = hCovM_ee / ( (binWidth_x*lumi)*(binWidth_y*lumi) );
        hCovM_mm = hCovM_mm / ( (binWidth_x*lumi)*(binWidth_y*lumi) );

        // if( (mCov_ee != hCovM_ee) || (mCov_mm != hCovM_mm) ) {
        if( !IsDiffSmall(mCov_ee, hCovM_ee) || !IsDiffSmall(mCov_mm, hCovM_mm) ) {
          printf("(%02d, %02d) bin\n", i_binX, i_binY);
          printf("  [ee] (mCov, hCovM) = (%lf, %lf)\n", mCov_ee, hCovM_ee);
          printf("  [mm] (mCov, hCovM) = (%lf, %lf)\n", mCov_mm, hCovM_mm);
          throw std::runtime_error("Inconsistent covariance values with the input covariance matrix");
        }
      } // -- iter over y
    } // -- iter over x
    printf("***[Combinator::Validation_TMatrixD] Pass the test: same covariance values with the input covariance matrix\n");


    // -- comparison: correlation matrix (from mCorr vs. from h_corrM)
    TH2D* h2D_corr_ee = PlotTool::Get_Hist2D(fileName_ee, "h_corrM_tot_mAxis");
    TH2D* h2D_corr_mm = PlotTool::Get_Hist2D(fileName_mm, "h_corrM_tot_mAxis");

    for(Int_t i_x=0; i_x<nMassBin_; ++i_x) {
      Int_t i_binX = i_x+1;

      for(Int_t i_y=0; i_y<nMassBin_; ++i_y) {
        Int_t i_binY = i_y+1;

        // -- correlation from m2N_corrM
        Double_t mCorr_ee = m2N_corrM[i_x][i_y];
        Double_t mCorr_mm = m2N_corrM[i_x+nMassBin_][i_y+nMassBin_];

        // -- correlation from the input histogram
        Double_t hCorrM_ee = h2D_corr_ee->GetBinContent(i_binX, i_binY);
        Double_t hCorrM_mm = h2D_corr_mm->GetBinContent(i_binX, i_binY);

        // if( (mCorr_ee != hCorrM_ee) || (mCorr_mm != hCorrM_mm) ) { 
        if( !IsDiffSmall(mCorr_ee, hCorrM_ee) || !IsDiffSmall(mCorr_mm, hCorrM_mm) ) {
          printf("(%02d, %02d) bin\n", i_binX, i_binY);
          printf("  [ee] (mCorr, hCorrM) = (%lf, %lf)\n", mCorr_ee, hCorrM_ee);
          printf("  [mm] (mCorr, hCorrM) = (%lf, %lf)\n", mCorr_mm, hCorrM_mm);
          throw std::runtime_error("Inconsistent correlation values with the input correlation matrix");
        }
      } // -- iter over y
    } // -- iter over x
    printf("***[Combinator::Validation_TMatrixD] Pass the test: same correlation values with the input correlation matrix\n");
  }

  Bool_t IsDiffSmall(Double_t value1, Double_t value2) {
    if( value1 == 0 ) {
      if( value2 == 0 ) return kTRUE;
      else return kFALSE;
    }

    Double_t relDiff = (value1 - value2) / value1;
    return relDiff < 0.0001; // -- 0.01%
  }

  void ProducePlot_Coefficient(const TMatrixD& m2N_lambda) {
    cout << "ProducePlot_Coefficient" << endl;
  }

  void ProducePlot_CombinedResult() {
    Comparison_Combined_vs_Channel("dsigdm_data");
    Comparison_Combined_vs_Channel("relUnc_tot_mAxis");
    Comparison_Combined_vs_Channel("absUnc_tot_mAxis");
    Comparison_Combined_vs_Theory();
  }

  void Comparison_Combined_vs_Theory() {
    TString canvasName = "c_comp_vs_theory";

    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 1, 1);
    canvas->SetTitle("m [GeV]", "d#sigma/dm [pb/GeV]", "theory/data");

    canvas->Register(h_dsigdm_ll_, "Data", kBlack);
    canvas->Register(h_dsigdm_theory_, "Theory (Powheg, MiNNLO)", kGreen+2);

    canvas->SetLegendPosition(0.50, 0.70, 0.94, 0.87);

    canvas->SetRangeY(5e-9, 2e3);

    canvas->SetRangeRatio(0.7, 1.3);

    canvas->Latex_CMSInternal();

    canvas->Latex_LumiEnergy(DYTool::GetLumi("all")/1000.0, 13);

    if( type_ == "FPS" ) {
      canvas->RegisterLatex(0.18, 0.38, 62, 0.6, "Full phase space (dressed level)");
      canvas->RegisterLatex(0.18, 0.34, 42, 0.6, "Stat.+Syst. uncertainty");
    }
    else {
      canvas->RegisterLatex(0.16, 0.91, 42, 0.7, "Combined results");
      canvas->RegisterLatex(0.18, 0.46, 62, 0.6, "Fiducial phase space (dressed level)");    
      canvas->RegisterLatex(0.18, 0.42, 42, 0.6, "p_{T}^{lead}(l) > 20 GeV, p_{T}^{sub}(l) > 15 GeV");
      canvas->RegisterLatex(0.18, 0.38, 42, 0.6, "|#eta(l)| < 2.4");
      canvas->RegisterLatex(0.18, 0.34, 42, 0.6, "Stat.+Syst. uncertainty");
    }

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();

    TString baseName = canvas->GetCanvasName();
    canvas->SetRangeRatio(0, 2.5);
    canvas->SetCanvasName( baseName + "_ratioZoomOut" );
    canvas->Draw();
  }

  void Comparison_Combined_vs_Channel(TString histType) {
    TH1D* h_ee = result_ee_->Get(histType);
    TH1D* h_mm = result_mm_->Get(histType);
    TH1D* h_ll = nullptr;
    if( histType == "dsigdm_data" )      h_ll = h_dsigdm_ll_;
    if( histType == "relUnc_tot_mAxis" ) h_ll = h_relUnc_ll_;
    if( histType == "absUnc_tot_mAxis" ) h_ll = h_absUnc_ll_;

    TString canvasName = "c_comp_"+histType;
    Bool_t isLogY = (histType == "relUnc_tot_mAxis") ? kFALSE : kTRUE;
    
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 1, isLogY);
    TString titleY = "";
    if( histType == "dsigdm_data" )      titleY = "d#sigma/dm [pb/GeV]";
    if( histType == "relUnc_tot_mAxis" ) titleY = "Rel. uncertainty";
    if( histType == "absUnc_tot_mAxis" ) titleY = "Abs. uncertainty [pb/GeV]";

    canvas->SetTitle("m [GeV]", titleY, "ee(mm)/ll");

    canvas->Register(h_ll, "Combined result", kRed);
    canvas->Register(h_ee, "Electron channel", kGreen+2);
    canvas->Register(h_mm, "Muon channel", kBlue);

    canvas->SetLegendPosition(0.40, 0.70, 0.94, 0.90);
    if( histType == "dsigdm_data" )           canvas->SetRangeY(5e-9, 2e3);
    else if( histType == "absUnc_tot_mAxis" ) canvas->SetRangeY(5e-9, 2e3);
    else                                      canvas->SetAutoRangeY();

    canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    Double_t run2Lumi = LUMI_16pre + LUMI_16post + LUMI_17 + LUMI_18;
    canvas->Latex_LumiEnergy(run2Lumi/1000.0, 13);

    if( histType == "dsigdm_data" ) {
      canvas->RegisterLatex(0.18, 0.46, 62, 0.6, "Fiducial phase space (dressed level)");    
      canvas->RegisterLatex(0.18, 0.42, 42, 0.6, "p_{T}^{lead}(l) > 20 GeV, p_{T}^{sub}(l) > 15 GeV");
      canvas->RegisterLatex(0.18, 0.38, 42, 0.6, "|#eta(l)| < 2.4");
    }
    if( histType == "relUnc_tot_mAxis" || histType == "absUnc_tot_mAxis")
      canvas->RegisterLatex(0.16, 0.91, 42, 0.6, "Total uncertainty");

    canvas->SetSavePath(plotDirPath_);

    if( histType == "dsigdm_data" )      canvas->Draw();
    if( histType == "relUnc_tot_mAxis" ) canvas->Draw("HISTLP");
    if( histType == "absUnc_tot_mAxis" ) canvas->Draw("HISTLP");

    if( histType == "relUnc_tot_mAxis" || histType == "absUnc_tot_mAxis" ) {
      canvas->SetCanvasName(canvasName+"_zoomInRatio");
      canvas->SetAutoRangeRatio(kFALSE);
      canvas->SetRangeRatio(0.5, 2.5);
      canvas->Draw("HISTLP");
    }

    if( histType == "dsigdm_data" ) {
      canvas->SetCanvasName(canvasName+"_zoomInRatio");
      canvas->SetAutoRangeRatio(kFALSE);
      canvas->SetRangeRatio(0.87, 1.13);
      canvas->Draw();
    }
  }

  void InitHist_CombinedResult() {
    // -- initialize the histograms for the combined results
    h_dsigdm_ll_ = result_ee_->Get("dsigdm_data");
    h_dsigdm_ll_->Reset("ICES");

    h_relUnc_ll_ = (TH1D*)h_dsigdm_ll_->Clone();
    h_absUnc_ll_ = (TH1D*)h_dsigdm_ll_->Clone();

    h_covM_ll_ = result_ee_->Get2D("covM_tot_mAxis");
    h_covM_ll_->Reset("ICES");

    h_corrM_ll_ = (TH2D*)h_covM_ll_->Clone();
  }

  void Construct_CombinedHist(const TMatrixD& mCov_ll) {
    for(Int_t i=0; i<nMassBin_; ++i) {
      Int_t i_bin = i+1;

      // -- set the error on the combined result
      Double_t absUnc_i = sqrt(mCov_ll[i][i]);
      h_dsigdm_ll_->SetBinError(i_bin, absUnc_i);

      // -- fill the abs & rel. unc. histogram
      Double_t dsigdm_i = h_dsigdm_ll_->GetBinContent(i_bin);
      Double_t relUnc_i = absUnc_i / dsigdm_i;
      h_absUnc_ll_->SetBinContent(i_bin, absUnc_i);
      h_absUnc_ll_->SetBinError(i_bin, 0);

      h_relUnc_ll_->SetBinContent(i_bin, relUnc_i);
      h_relUnc_ll_->SetBinError(i_bin, 0);

      // -- fill the cov. matrix histogram
      for(Int_t j=0; j<nMassBin_; ++j) {
        Int_t j_bin = j+1;

        h_covM_ll_->SetBinContent(i_bin, j_bin, mCov_ll[i][j]);
        h_covM_ll_->SetBinError(i_bin, j_bin, 0);
      }
    }

    // -- convert cov to corr.
  }

  TMatrixD Calc_Cov_Combined(const TMatrixD& m2N_lambda, const TMatrixD& m2N_covM) {
    TMatrixD mCov_ll(nMassBin_, nMassBin_);

    TMatrixD m2N_lambdaT(m2N_lambda);
    m2N_lambdaT.Transpose(m2N_lambdaT);
    mCov_ll = (m2N_lambda * m2N_covM) * m2N_lambdaT;

    return mCov_ll;
  }

  void Calc_CentralValue_Combined(const TMatrixD& m2N_dsigdm, const TMatrixD& m2N_lambda) {
    TMatrixD m2N_dsigdm_T = TMatrixD(m2N_dsigdm);
    m2N_dsigdm_T.Transpose(m2N_dsigdm_T);

    TMatrixD m_combined_T = m2N_lambda * m2N_dsigdm_T; // -- nMassBin * 1 matrix
    TMatrixD m_combined(m_combined_T);
    m_combined.Transpose(m_combined);  // -- 1 * nMassBin matrix

    for(Int_t i=0; i<nMassBin_; ++i) {
      Int_t i_bin = i+1;

      h_dsigdm_ll_->SetBinContent(i_bin, m_combined[0][i]);
      h_dsigdm_ll_->SetBinError(i_bin, 0); // -- no error yet. It will be filled after the covariance matrix for the combiend result is constructed
    }
  }


  // ---- 0 ... nMassBin-1: dXSec for e channel
  // ---- nMassBin ... 2*nMassBin-1: dXSec for mu channel
  TMatrixD Convert_Matrix2N_dsigdm() {
    TMatrixD m2N_dsigdm(1, 2*nMassBin_);

    TH1D* h_dsigdm_ee = result_ee_->Get("dsigdm_data");
    TH1D* h_dsigdm_mm = result_mm_->Get("dsigdm_data");

    for(Int_t i=0; i<nMassBin_; ++i) {
      Int_t i_bin = i+1;

      Double_t dsigdm_ee = h_dsigdm_ee->GetBinContent(i_bin);
      m2N_dsigdm[0][i] = dsigdm_ee;

      Double_t dsigdm_mm = h_dsigdm_mm->GetBinContent(i_bin);
      m2N_dsigdm[0][i+nMassBin_] = dsigdm_mm;
    }

    return m2N_dsigdm;
  }

  TMatrixD Construct_Matrix2N() {
    Int_t nBin = result_ee_->Get("dsigdm_data")->GetNbinsX();
    TMatrixD m2N_covM(2*nBin, 2*nBin);
    // -- initialization: set as 0 for all elements
    for(Int_t i=0; i<2*nBin; i++)
      for(Int_t j=0; j<2*nBin; j++)
        m2N_covM[i][j] = 0;

    // -- N*N matrices
    TMatrixD mCov_ee = Convert_To_TMatrixD(result_ee_->Get2D("covM_tot_mAxis"));
    TMatrixD mCov_mm = Convert_To_TMatrixD(result_mm_->Get2D("covM_tot_mAxis"));
    TMatrixD mCov_em_tot(nBin, nBin);
    Init_Matrix(mCov_em_tot);
    if( !noCorr_em_ ) {
      for(const auto& pair : map_covM_em_ )
        mCov_em_tot = mCov_em_tot + Convert_To_TMatrixD(pair.second);
    }

    Fill_CovMatrix2N(m2N_covM, "ee", mCov_ee);
    Fill_CovMatrix2N(m2N_covM, "mm", mCov_mm);
    Fill_CovMatrix2N(m2N_covM, "em", mCov_em_tot);

    return m2N_covM;
  }

  void Init_Matrix(TMatrixD& matrix) {
    Int_t nRow = matrix.GetNcols();
    Int_t nCol = matrix.GetNrows();

    for(Int_t i_x=0; i_x<nRow; ++i_x)
      for(Int_t i_y=0; i_y<nCol; ++i_y)
        matrix[i_x][i_y] = 0;

  }

  void Fill_CovMatrix2N(TMatrixD& m2N_covM, TString covType, const TMatrixD& mCov) {
    for(Int_t i=0; i<nMassBin_; ++i) {

      for(Int_t j=0; j<nMassBin_; ++j) {

        if( covType == "ee" )
          m2N_covM[i][j] = mCov[i][j];

        if( covType == "mm" )
          m2N_covM[i+nMassBin_][j+nMassBin_] = mCov[i][j];

        if( covType == "em" ) {
          m2N_covM[i][j+nMassBin_] = mCov[i][j];
          m2N_covM[j+nMassBin_][i] = mCov[j][i]; // -- ij = ji, though...
        }

      } // -- iteration over j
    } // -- iteration over i

  }

  // -- fill m2N_lambda_ with the coefficient values (simple weighted mean, same with the case without any correlation)
  // -- 1...N: electron channel, N+1 ... 2N: muon channel
  TMatrixD Calc_Coefficient() {
    TH1D* h_dsigdm_ee = result_ee_->Get("dsigdm_data");
    TH1D* h_dsigdm_mm = result_mm_->Get("dsigdm_data");

    // printf("ee\n");
    // PlotTool::Print_Histogram(h_dsigdm_ee);
    // printf("mm\n");
    // PlotTool::Print_Histogram(h_dsigdm_mm);

    Int_t nBin = h_dsigdm_ee->GetNbinsX();

    TMatrixD m2N_lambda(nBin, 2*nBin);
    // -- initialization: set as 0 for all elements
    for(Int_t i=0; i<nBin; i++)
      for(Int_t j=0; j<2*nBin; j++)
        m2N_lambda[i][j] = 0;

    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;

      Double_t absUnc_ee = h_dsigdm_ee->GetBinError(i_bin);
      Double_t absUnc_mm = h_dsigdm_mm->GetBinError(i_bin);

      if( absUnc_ee == 0 || absUnc_mm == 0 ) {
        printf("***[Combinator::Calc_Coefficient] %d bin: (absUnc_ee, absUnc_mm) = (%lf, %lf) - one of them is 0!\n", i+1, absUnc_ee, absUnc_mm);
        throw std::runtime_error("Inconsistent absolute uncertainty on the cross section");
      }

      Double_t num_ee = (1.0/absUnc_ee)*(1.0/absUnc_ee);
      Double_t num_mm = (1.0/absUnc_mm)*(1.0/absUnc_mm);
      Double_t den = num_ee + num_mm;

      Double_t coeff_ee = num_ee / den;
      Double_t coeff_mm = num_mm / den;

      m2N_lambda[i][i]      = coeff_ee; // -- 1, 2, ... N: electron channel
      m2N_lambda[i][i+nBin] = coeff_mm; // -- N+1, N+2, ... 2N: muon channel

      // printf("[%d bin] (absUnc_ee, absUnc_mm, coeff_ee, coeff_mm) = (%lf, %lf, %.3lf, %.3lf)\n", i+1, absUnc_ee, absUnc_mm, coeff_ee, coeff_mm);
    }

    return m2N_lambda;
  }

  void Construct_Covariance_BtwChannel() {
    for(const auto& uncType : vec_uncType_corr_em_ )
      Construct_Covariance_BtwChannel(uncType);
  }

  // void Construct_Covariance_BtwChannel(TString uncType, TH1D* h_relUnc_ee, TH1D* h_relUnc_mm) {
  void Construct_Covariance_BtwChannel(TString uncType) {
    // cout << "Construct_Covariance_BtwChannel: uncType = " << uncType << endl;

    // -- x-axis: ee; y-axis: mm
    TH2D* h_covM_em = result_ee_->Get2D("covM_tot_mAxis");
    h_covM_em->Reset("ICES");

    TH1D* h_dsigdm_ee = result_ee_->Get("dsigdm_data");
    TH1D* h_dsigdm_mm = result_mm_->Get("dsigdm_data");

    TH1D* h_relUnc_ee = result_ee_->Get("relUnc_"+uncType+"_mAxis");
    TH1D* h_relUnc_mm = result_mm_->Get("relUnc_"+uncType+"_mAxis");

    Int_t nBin = h_covM_em->GetNbinsX();

    for(Int_t i_x=0; i_x<nBin; ++i_x) {
      Int_t i_binX = i_x+1;

      Double_t dsigdm_ee = h_dsigdm_ee->GetBinContent(i_binX);
      Double_t relUnc_ee = h_relUnc_ee->GetBinContent(i_binX);
      Double_t absUnc_ee = dsigdm_ee*relUnc_ee;

      // if( uncType == "theory" )
      //   printf("[%02d ee bin] (dsigmadm, relUnc, absUnc) = (%lf, %.3lf, %lf)\n", i_binX, dsigdm_ee, relUnc_ee, absUnc_ee);

      for(Int_t i_y=0; i_y<nBin; ++i_y) {
        Int_t i_binY = i_y+1;

        Double_t dsigdm_mm = h_dsigdm_mm->GetBinContent(i_binY);
        Double_t relUnc_mm = h_relUnc_mm->GetBinContent(i_binY);
        Double_t absUnc_mm = dsigdm_mm*relUnc_mm;

        Double_t cov_em = 0;
        if( i_x == i_y ) // -- diagonal terms (correlation between the same bin in each channel)
          cov_em = absUnc_ee*absUnc_mm; // -- 100% correlation
        else { // -- off-diagonal terms (correlation between different bins in each channel)
          if( uncType == "lumi_tot" ) cov_em = absUnc_ee*absUnc_mm; // -- lumi: still 100% corr.
          else                        cov_em = 0; // -- else: no correlated
        }

        // if( uncType == "theory" && i_x == i_y) {
        //   printf("  [%02d mm bin] (dsigmadm, relUnc, absUnc) = (%lf, %.3lf, %lf)", i_binY, dsigdm_mm, relUnc_mm, absUnc_mm);
        //   printf("  --> cov_em = %lf (cov_ee = %lf, cov_mm = %lf)\n", cov_em, 
        //     result_ee_->Get2D("covM_mAxis")->GetBinContent(i_binX, i_binY),
        //     result_mm_->Get2D("covM_mAxis")->GetBinContent(i_binX, i_binY) );
        // }

        h_covM_em->SetBinContent(i_binX, i_binY, cov_em);
        h_covM_em->SetBinError(i_binX, i_binY, 0);

      } // -- iteration over y
    } // -- iteration over x

    map_covM_em_.insert( std::make_pair(uncType, h_covM_em) );
  }

  void Comparison_Channel() {
    TString canvasName = "c_comp_ee_mm_"+type_;

    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 1, 1);
    canvas->SetTitle("m [GeV]", "d#sigma/dm [pb/GeV]", "mm/ee");

    // -- FIXME: remove the correlated uncertainty on each dsigma/dm, to be consistent with the chi2/ndof calculation
    canvas->Register(result_ee_->Get("dsigdm_data"), "Electron channel", kGreen+2);
    canvas->Register(result_mm_->Get("dsigdm_data"), "Muon channel",     kBlue);

    canvas->SetLegendPosition(0.50, 0.70, 0.94, 0.87);

    canvas->SetRangeY(5e-9, 2e3);
    // canvas->SetRangeRatio(0.7, 1.3);
    canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    Double_t run2Lumi = LUMI_16pre + LUMI_16post + LUMI_17 + LUMI_18;
    canvas->Latex_LumiEnergy(run2Lumi/1000.0, 13);

    Double_t totalChi2; Int_t nDOF;
    Double_t normChi2 = Compute_NormChi2(totalChi2, nDOF);
    Double_t pValue = ROOT::Math::chisquared_cdf_c(totalChi2, nDOF);
    TString normChi2Info = TString::Format("Compatibility (ee, mm): #chi2/ndof = %.3lf/%d = %.3lf (p-value = %.3lf)", totalChi2, nDOF, normChi2, pValue);
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, normChi2Info);

    canvas->RegisterLatex(0.18, 0.46, 62, 0.6, "Fiducial phase space (dressed level)");    
    canvas->RegisterLatex(0.18, 0.42, 42, 0.6, "p_{T}^{lead}(l) > 20 GeV, p_{T}^{sub}(l) > 15 GeV");
    canvas->RegisterLatex(0.18, 0.38, 42, 0.6, "|#eta(l)| < 2.4");

    canvas->RegisterLatex(0.18, 0.34, 42, 0.6, "Stat.+Syst. uncertainty");

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();

    TString baseName = canvas->GetCanvasName();
    canvas->SetCanvasName( baseName + "_ratioZoomIn" );
    canvas->SetAutoRangeRatio(kFALSE);
    canvas->SetRangeRatio(0.87, 1.13);    
    canvas->Draw();
  }

  Double_t Compute_NormChi2(Double_t &totalChi2, Int_t& nDOF) {

    // -- initialize
    TMatrixD y_ee = Convert_To_TMatrixD(result_ee_->Get("dsigdm_data"));
    TMatrixD y_mm = Convert_To_TMatrixD(result_mm_->Get("dsigdm_data"));
    TMatrixD y = y_ee - y_mm; // -- difference between two channels

    TMatrixD mCov_ee = Convert_To_TMatrixD(result_ee_->Get2D("covM_tot_mAxis"));
    TMatrixD mCov_mm = Convert_To_TMatrixD(result_mm_->Get2D("covM_tot_mAxis"));

    Subtract_CorrelatedSource(mCov_ee, result_ee_);
    Subtract_CorrelatedSource(mCov_mm, result_mm_);

    TMatrixD mCov_y = mCov_ee + mCov_mm;

    // -- calc. chi2
    TMatrixD mCovInvert(mCov_y);
    mCovInvert.Invert();

    TMatrixD yT(y);
    yT.Transpose(yT);

    TMatrixD mChi2 = (y * mCovInvert) * yT;
    totalChi2 = mChi2[0][0];
    nDOF = result_ee_->Get("dsigdm_data")->GetNbinsX(); // -- # dof = # bins
    Double_t normChi2 = totalChi2 / (Double_t)nDOF;

    printf("***[Combinator::Compute_NormChi2] Chi2/ndof = %.5lf/%02d = %.5lf\n", totalChi2, nDOF, normChi2);
    Compute_NormChi2_EachBin(y, mCovInvert, totalChi2);

    return normChi2;
  }

  // -- remove the covariance matrix from the correlated source between two channels
  void Subtract_CorrelatedSource(TMatrixD& mCov, ChannelResult* result) {
    // vector<TString> vec_tag_corrEM = { };
    // vector<TString> vec_tag_corrEM = {"lumi_tot", "theory_tot", "unfold_model"};
    // vector<TString> vec_tag_corrEM = {"lumi_tot"};
    // vector<TString> vec_tag_corrEM = {"theory_tot"};
    // vector<TString> vec_tag_corrEM = {"unfold_model"};
    // vector<TString> vec_tag_corrEM = {"lumi_tot", "theory_tot"};
    vector<TString> vec_tag_corrEM = vec_uncType_corr_em_;

    printf("***[Combinator::Subtract_CorrelatedSource] chi2(ee,mm): below uncertainties are not considered in chi2 calculation (correlated source between channels)\n");
    for(const auto& tag : vec_tag_corrEM)
      printf("%s / ", tag.Data());
    printf("\n");

    for(const auto & tag : vec_tag_corrEM ) {
      mCov =  mCov - Convert_To_TMatrixD(result->Get2D("covM_"+tag+"_mAxis"));
    }
    Check_Negative(mCov);
  }

  void Check_Negative(TMatrixD& matrix) {
    Int_t nRow = matrix.GetNcols();
    Int_t nCol = matrix.GetNrows();

    for(Int_t i_x=0; i_x<nRow; ++i_x) {
      for(Int_t i_y=0; i_y<nCol; ++i_y) {

        if( (i_x == i_y) && matrix[i_x][i_y] < 0)
          cout << i_x << ", " << i_y << " --> matrix = " << matrix[i_x][i_y] << " < 0!" << endl;
      }
    }
  }

  void Compute_NormChi2_EachBin(const TMatrixD& y, const TMatrixD& mCovInvert, const Double_t& totalChi2) {
    TH1D* h_absChi2 = result_ee_->Get("dsigdm_data");
    h_absChi2->Reset("ICES");
    TH1D* h_relChi2 = (TH1D*)h_absChi2->Clone();


    Int_t nBin = h_absChi2->GetNbinsX();
    Double_t totalChi2_validation = 0;
    for(Int_t i=0; i<nBin; i++) {
      Int_t i_bin = i+1;

      Double_t y_i = y[0][i];
      Double_t cont_toChi2 = 0;

      // printf("[i_bin = %02d]\n", i_bin);

      for(Int_t j=0; j<nBin; j++) {
        Double_t y_j = y[0][j];
        Double_t covInv_ij = mCovInvert[i][j];

        cont_toChi2 += y_i * covInv_ij * y_j;

        Int_t j_bin = j+1;
        // printf("  [j_bin = %02d] (y_i, covInv_ij, y_j) = (%.3lf, %.3lf, %.3lf) --> contribution = %lf\n",
               // j_bin, y_i, covInv_ij, y_j, y_i * covInv_ij * y_j);
      }

      Double_t ratio = (cont_toChi2 / totalChi2) * 100;

      // printf("[%4.0lf < M < %4.0lf] (chi2, ratio to total chi2.) = (%.3lf, %.3lf %%)\n",
      //          h_absChi2->GetBinLowEdge(i_bin), h_absChi2->GetBinLowEdge(i_bin+1), cont_toChi2, ratio);

      h_absChi2->SetBinContent(i_bin, cont_toChi2); h_absChi2->SetBinError(i_bin, 0);
      h_relChi2->SetBinContent(i_bin, ratio);       h_relChi2->SetBinError(i_bin, 0);

      totalChi2_validation += cont_toChi2;
    }

    // cout << "  * Validation: total chi2 by matrix multiplication, total chi2 by adding each contribution = " << totalChi2 << ", " << totalChi2_validation << endl;

    ProducePlot_Chi2("absChi2", h_absChi2);
    ProducePlot_Chi2("relChi2", h_relChi2);
  }

  void ProducePlot_Chi2(TString chi2Type, TH1D* h_chi2) {
    TString canvasName = "c_chi2_ee_mm_"+chi2Type;

    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 1, 0);
    TString titleY = "";
    if( chi2Type == "absChi2" ) titleY = "#chi2 contribution";
    if( chi2Type == "relChi2" ) titleY = "Relative #chi2 contribution [%]";
    canvas->SetTitle("m [GeV]", titleY);

    canvas->Register(h_chi2, "#chi2 per bin", kBlack);

    // canvas->SetLegendPosition(0.50, 0.70, 0.94, 0.87);
    canvas->SetLegendPosition(0, 0, 0, 0);

    canvas->SetAutoRangeY();
    // canvas->SetRangeY(5e-9, 2e3);

    canvas->Latex_CMSInternal();
    Double_t run2Lumi = LUMI_16pre + LUMI_16post + LUMI_17 + LUMI_18;
    canvas->Latex_LumiEnergy(run2Lumi/1000.0, 13);

    TString info = "Compatibility between two channels ("+type_+")";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, info);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");
  }

  TMatrixD Convert_To_TMatrixD(TH1D* h) {
    Int_t nBin = h->GetNbinsX();

    TMatrixD matrix(1, nBin);

    for(Int_t i_x=0; i_x<nBin; ++i_x) {
      Int_t i_binX = i_x+1;

      Double_t value = h->GetBinContent(i_binX);
      matrix[0][i_x] = value;

    } // -- iteration over x

    return matrix;
  }

  TMatrixD Convert_To_TMatrixD(TH2D* h2D) {
    Int_t nBin = h2D->GetNbinsX();

    TMatrixD matrix(nBin, nBin);

    for(Int_t i_x=0; i_x<nBin; ++i_x) {
      Int_t i_binX = i_x+1;

      for(Int_t i_y=0; i_y<nBin; ++i_y) {
        Int_t i_binY = i_y+1;

        Double_t value = h2D->GetBinContent(i_binX, i_binY);
        matrix[i_x][i_y] = value;

      } // -- iteration over y
    } // -- iteration over x

    return matrix;
  }

  TMatrixD Make_CorrM(TMatrixD& m_cov, Bool_t is2NcovM = kTRUE)
  {
    Int_t nBin = 0;
    if( is2NcovM ) nBin = 2*nMassBin_;
    else           nBin = nMassBin_;

    TMatrixD m_corr(nBin, nBin);

    // -- initialization: set as 0 for all elements
    for(Int_t i=0; i<nBin; i++)
      for(Int_t j=0; j<nBin; j++)
        m_corr[i][j] = 0;


    for(Int_t i=0; i<nBin; i++) {
      for(Int_t j=0; j<nBin; j++) {
        Double_t cov_ij = m_cov[i][j];
        Double_t var_i  = m_cov[i][i];
        Double_t var_j  = m_cov[j][j];

        Double_t corr_ij = cov_ij / sqrt(var_i*var_j);
        m_corr[i][j] = corr_ij;
        if( corr_ij > 1.0 ) {
          printf("***[Combinator::Make_CorrM] (i, j) = (%02d, %02d) --> corr = %lf > 1.0! (cov_ij = %lf, sqrt(var_i) = %lf, sqrt(var_j) = %lf\n",
                  i, j, corr_ij, cov_ij, sqrt(var_i), sqrt(var_j));
        }
        // printf("(i, j) = (%d, %d) -> (cov, var_i, var_j, corr) = (%.3e, %.3e, %.3e, %.3e)\n", i, j, cov_ij, var_i, var_j, corr_ij);
      }
    }

    return m_corr;
  }

  void ProducePlot_2D(TMatrixD& matrix, TString matrixType, TString tag, Bool_t is2N) {
    TString canvasName = "c2D_"+matrixType+"_"+tag;

    Bool_t isLogX = kFALSE;
    Bool_t isLogY = kFALSE;
    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, isLogX, isLogY, 0);
    TString title = "m [GeV]";
    if( is2N ) title = "mass bin number (ee: 1-39; mm: 40-78)";
    canvas->SetTitle(title, title);

    TH2D* h2D = new TH2D(matrix);
    canvas->Register(h2D);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeZ(minZ, maxZ);
    if( matrixType == "corrM" ) canvas->SetRangeZ(-1.01, 1.01);
    else                        canvas->SetAutoRangeZ();

    canvas->Latex_CMSInternal();
    TString info = "Covariance matrix ("+tag+")";
    if( matrixType == "corrM" ) info.ReplaceAll("Covariance", "Correlation");
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, info);
    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }

  void Save() {
    TString fileName_output = "CombinedResult_DYXSec_"+type_+".root";
    TFile *f_output = TFile::Open(fileName_output, "RECREATE"); 

    result_ee_->Save(f_output, "ee");
    result_mm_->Save(f_output, "mm");

    h_dsigdm_ll_->SetName("h_dsigdm_ll");
    h_dsigdm_ll_->Write();

    h_relUnc_ll_->SetName("h_relUnc_ll");
    h_relUnc_ll_->Write();

    h_absUnc_ll_->SetName("h_absUnc_ll");
    h_absUnc_ll_->Write();

    h_covM_ll_->SetName("h_covM_ll");
    h_covM_ll_->Write();

    h_corrM_ll_->SetName("h_corrM_ll");
    h_corrM_ll_->Write();
  }

};

// -- fiducial cross section
void perform_combination_fid(Bool_t noCorr_em = kFALSE) {
  Combinator combinator("fid");
  combinator.Set_CorrelatedUnc_BtwChannel( {"lumi_tot", "theory_tot", "unfold_model", "bVeto_tot"} );
  combinator.Set_NoCorr_BtwChannel(noCorr_em);

  // -- for the final comparison
  TH1D* h_dsigdm_theory = PlotTool::Get_Hist(DYTool::path_theoryPred, "h_dsigdm_fid_PDFVar_000");
  combinator.Set_Theory(h_dsigdm_theory);

  vector<TString> vec_channel = {"ee", "mm"};
  for(const auto& channel : vec_channel ) {
    TString fileName = "../Summary/UncAndCov_All_"+channel+".root";
    combinator.Set(channel, fileName);
  }

  combinator.Combine();
}

// -- full phase space cross section
void perform_combination_FPS(Bool_t noCorr_em = kFALSE) {
  Combinator combinator("FPS");
  combinator.Set_CorrelatedUnc_BtwChannel( {"lumi_tot", "theory_tot", "unfold_model", "bVeto_tot", "pileup"} );
  combinator.Set_NoCorr_BtwChannel(noCorr_em);

  // -- for the final comparison
  TH1D* h_dsigdm_theory = PlotTool::Get_Hist(DYTool::path_theoryPred, "h_dsigdm_FPS_PDFVar_000");
  combinator.Set_Theory(h_dsigdm_theory);

  vector<TString> vec_channel = {"ee", "mm"};
  for(const auto& channel : vec_channel ) {
    TString fileName = "../Summary/UncAndCov_All_"+channel+"_FPS.root";
    combinator.Set(channel, fileName);
  }

  combinator.Combine();
}

void perform_combination() {
  TH1::AddDirectory(kFALSE);

  perform_combination_fid();
  perform_combination_FPS();

  // -- remove correlation between channels: for test
  // perform_combination_fid(kTRUE);
  // perform_combination_FPS(kTRUE);
}