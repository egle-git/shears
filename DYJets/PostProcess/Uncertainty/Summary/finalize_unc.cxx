#include "Common/SimplePlotTools.h"
#include "Common/DYTool.h"

class UncInfo {
public:
  UncInfo(TString type, TString legend): type_(type), legend_(legend) {};

  void Set_Color(Int_t color) { color_ = color; }

  void Set_FileName(TString f_ee, TString f_mm) {
    fileName_ee_ = f_ee;
    fileName_mm_ = f_mm;
  }

  void Set_HistTag(TString tag) { histTag_ = tag; }

  TString Type() const   { return type_; }
  TString Legend() const { return legend_; }
  TString FileName(TString channel) const { 
    return (channel == "mm") ? fileName_mm_ : fileName_ee_;
  }
  TString HistTag() const { return histTag_; }
  Int_t Color() const { return color_; }

private:
  TString type_ = "";
  TString legend_ = "";
  Int_t color_ = 0;
  TString fileName_mm_ = "";
  TString fileName_ee_ = "";
  TString histTag_ = "";
};

class UncFinalizer {
public:
  UncFinalizer(TString channel) : channel_(channel) {};

  void Use_Fake(Bool_t flag = kTRUE) { useFake_ = flag; }
  void FullPhaseSpace(Bool_t flag = kTRUE) { isFPS_ = flag; }

  void Finalize() {
    TH1::AddDirectory(kFALSE);
    Init();

    if( !gSystem->AccessPathName(fileName_output_) )
      throw std::runtime_error(fileName_output_+" already exists: remove or rename it");

    TString plotDirName = isFPS_ ? "Summary/FPS" : "Summary/fid";
    plotDirPath_ = DYTool::Set_PlotPath(plotDirName);

    Calc_TotalUnc();
    Calc_TotalCov();
    Check_Consistency();

    ProducePlot_Unc();
    ProducePlot_Unc(kTRUE);
    ProducePlot_2D("covM");
    ProducePlot_2D("corrM");
    ProducePlot_2D("covM", kTRUE);
    ProducePlot_2D("corrM", kTRUE);

    Save();
  }

private:
  TString channel_ = "";
  TString fileName_output_ = "";
  TString plotDirPath_ = "";

  vector<UncInfo> vec_uncInfo_;
  // vector<TString> vec_uncType_;

  // -- for save
  vector<TH1D*> vec_hist_relUnc_;
  vector<TH2D*> vec_hist_covM_;
  vector<TH2D*> vec_hist_corrM_;

  TH1D* h_absUnc_tot_;
  TH1D* h_relUnc_tot_;
  TH2D* h_covM_tot_;
  TH2D* h_corrM_tot_;

  Bool_t useFake_ = kTRUE; // -- default: true
  Bool_t isFPS_ = kFALSE; // -- is full phase space result?

  DYRun2Result* result_cv_ = nullptr;
  TH1D* h_nEvent_cv_ = nullptr; // -- for converting "relUnc -> absUnc" & "corrM -> covM"

  void Init() {
    fileName_output_ = "UncAndCov_All_"+channel_+".root";
    if( isFPS_ )
      fileName_output_.ReplaceAll(".root", "_FPS.root");

    fileName_output_ = DYTool::Get_OutputPath(fileName_output_);

    Insert_UncInfo("statData", "Data statistics", kBlack,
                   DYTool::Get_OutputPath("Uncertainty_and_Covariance_Stat_ee.root"),
                   DYTool::Get_OutputPath("Uncertainty_and_Covariance_Stat_mm.root"),
                   "stat_data");

    Insert_UncInfo("statMC", "MC statistics", kGray+2,
                   DYTool::Get_OutputPath("Uncertainty_and_Covariance_Stat_ee.root"),
                   DYTool::Get_OutputPath("Uncertainty_and_Covariance_Stat_mm.root"),
                   "stat_totMC");

    Insert_UncInfo("bkg", "Data-driven backgrounds", kGreen-7,  
                   DYTool::Get_OutputPath("Unfolded_And_Uncertainty_Bkg_ee.root"),
                   DYTool::Get_OutputPath("Unfolded_And_Uncertainty_Bkg_mm.root"),
                   "bkg_tot");

    Insert_UncInfo("lumi", "Luminosity", kOrange-3,
                   DYTool::Get_OutputPath("Unfolded_And_Uncertainty_lumi_ee.root"),
                   DYTool::Get_OutputPath("Unfolded_And_Uncertainty_lumi_mm.root"),
                   "lumi_tot");

    Insert_UncInfo("effSF", "Efficiency SF", kBlue,
                   DYTool::Get_OutputPath("UncAndCov_EffSF_ee.root"),
                   DYTool::Get_OutputPath("UncAndCov_EffSF_mm.root"),
                   "effSF_tot");

    Insert_UncInfo("L1Pref", "L1 pre-firing", kViolet,
                   DYTool::Get_OutputPath("Unfolded_And_Uncertainty_L1Pref_ee.root"),
                   DYTool::Get_OutputPath("Unfolded_And_Uncertainty_L1Pref_mm.root"),
                   "L1Pref");

    Insert_UncInfo("pileup", "Pileup reweighting", kGreen+2,
                   DYTool::Get_OutputPath("Unfolded_And_Uncertainty_pileup_ee.root"),
                   DYTool::Get_OutputPath("Unfolded_And_Uncertainty_pileup_mm.root"),
                   "pileup");


    TString fileName_theory_ee = "UncAndCov_Theory_ee.root";
    TString fileName_theory_mm = "UncAndCov_Theory_mm.root";
    if( isFPS_ ) {
      fileName_theory_ee.ReplaceAll(".root", "_FPS.root");
      fileName_theory_mm.ReplaceAll(".root", "_FPS.root");
    }
    Insert_UncInfo("theory", "Theoretical inputs", kCyan,
                   DYTool::Get_OutputPath(fileName_theory_ee),
                   DYTool::Get_OutputPath(fileName_theory_mm),
                   "theory_tot");


    Insert_UncInfo("muP", "Muon momentum correction", kRed+2,
                   "",
                   DYTool::Get_OutputPath("UncAndCov_muP.root"),
                   "muP_tot");

    Insert_UncInfo("elE", "Electron energy correction", kRed+2,
                   DYTool::Get_OutputPath("UncAndCov_elE.root"),
                   "",
                   "elE_tot");

    Insert_UncInfo("unfold_model", "Model uncertainty (Unfolding)", kBlue-6,
                   DYTool::Get_OutputPath("UncAndCov_Unfolding_ee.root"),
                   DYTool::Get_OutputPath("UncAndCov_Unfolding_mm.root"),
                   "unfold_model");

    Insert_UncInfo("bVeto", "b-veto", kGreen-6,
                   DYTool::Get_OutputPath("Unfolded_And_Uncertainty_bVeto_ee.root"),
                   DYTool::Get_OutputPath("Unfolded_And_Uncertainty_bVeto_mm.root"),
                   "bVeto_tot");

    result_cv_ = new DYRun2Result(DYTool::path_default+"/"+channel_);
    if( useFake_ ) DYTool::Set_Fake(channel_, result_cv_);
    if( isFPS_ )   DYTool::Set_Acc(result_cv_, "cv");
    result_cv_->Produce();

    h_nEvent_cv_ = (isFPS_) ? result_cv_->Get_AllEra("unfoldedFPS", "data") :
                              result_cv_->Get_AllEra("unfolded", "data");
  }

  void Insert_UncInfo(TString type, TString legend, Int_t color,
                      TString fileName_ee, TString fileName_mm, TString histTag) {
    UncInfo uncInfo(type, legend);
    uncInfo.Set_Color(color);
    uncInfo.Set_FileName(fileName_ee, fileName_mm);
    uncInfo.Set_HistTag(histTag);
    vec_uncInfo_.push_back(uncInfo);
  }

  void Calc_TotalUnc() {
    // -- to be safe, only the relative uncertainty is quad. sum
    // -- and, total relative uncertainty is converted to the absolute uncertainty
    for(const auto& uncInfo : vec_uncInfo_) {
      TString fileName = uncInfo.FileName(channel_);
      if( fileName == "" ) continue;
      // cout << "uncType = " << uncInfo.Type() << ": fileName = " << fileName << endl;

      TString histName_relUnc = "h_relUnc_"+uncInfo.HistTag();
      TH1D* h_relUnc = PlotTool::Get_Hist(fileName, histName_relUnc);
      vec_hist_relUnc_.push_back( h_relUnc );
    }
    h_relUnc_tot_ = PlotTool::QuadSum_Hist(vec_hist_relUnc_);
    h_absUnc_tot_ = PlotTool::HistOperation("h_absUnc_tot", h_nEvent_cv_, h_relUnc_tot_, "*");
  }

  TH2D* Convert_Corr_To_Cov(TH2D* h_corrM, TH1D* h_relUnc) {
    TH2D* h_covM = (TH2D*)h_corrM->Clone();
    h_covM->Reset("ICES");

    TString histName = h_corrM->GetName();
    histName.ReplaceAll("corr", "cov");
    h_covM->SetName(histName);

    Int_t nBin = h_relUnc->GetNbinsX();
    for(Int_t i_x=0; i_x<nBin; ++i_x) {
      Int_t i_binX = i_x+1;

      Double_t relUnc_x = h_relUnc->GetBinContent(i_binX);
      Double_t absUnc_x = h_nEvent_cv_->GetBinContent(i_binX) * relUnc_x;

      for(Int_t i_y=0; i_y<nBin; ++i_y) {
        Int_t i_binY = i_y+1;

        Double_t relUnc_y = h_relUnc->GetBinContent(i_binY);
        Double_t absUnc_y = h_nEvent_cv_->GetBinContent(i_binY) * relUnc_y;

        Double_t corr_xy = h_corrM->GetBinContent(i_binX, i_binY);
        Double_t cov_xy = corr_xy * absUnc_x * absUnc_y;
        h_covM->SetBinContent(i_binX, i_binY, cov_xy);
        h_covM->SetBinError(i_binX, i_binY, 0);
      }
    }

    return h_covM;
  }

  void Calc_TotalCov() {
    // -- to be safe, individual covariance matrices are constructed from
    // -- corresponding correlation matrix
    // -- and then summed up
    // -- (to avoid the different central value used for the cov. matrix)
    for(const auto& uncInfo : vec_uncInfo_) {
      TString fileName = uncInfo.FileName(channel_);
      if( fileName == "" ) continue;

      TString histName_corrM = "h_corrM_"+uncInfo.HistTag();
      TH2D* h_corrM = PlotTool::Get_Hist2D(fileName, histName_corrM);
      vec_hist_corrM_.push_back(h_corrM);

      TString histName_relUnc = "h_relUnc_"+uncInfo.HistTag();
      TH1D* h_relUnc = PlotTool::Get_Hist(fileName, histName_relUnc);

      TH2D* h_covM = Convert_Corr_To_Cov(h_corrM, h_relUnc);

      vec_hist_covM_.push_back( h_covM );
    }

    // -- construct: total covariance matrix
    h_covM_tot_ = (TH2D*)vec_hist_covM_[0]->Clone();
    h_covM_tot_->Reset("ICES");

    Int_t nBinX = h_covM_tot_->GetNbinsX();
    Int_t nBinY = h_covM_tot_->GetNbinsY();
    for(Int_t i_x=0; i_x<nBinX; ++i_x) {
      Int_t i_binX = i_x+1;

      for(Int_t i_y=0; i_y<nBinY; ++i_y) {
        Int_t i_binY = i_y+1;

        Double_t cov_tot = 0;
        for(const auto& h_covM : vec_hist_covM_ )
          cov_tot += h_covM->GetBinContent(i_binX, i_binY);

        h_covM_tot_->SetBinContent(i_binX, i_binY, cov_tot);
        h_covM_tot_->SetBinError(i_binX, i_binY, 0);
      }
    }

    // -- construct: total correlation matrix
    h_corrM_tot_ = (TH2D*)h_covM_tot_->Clone();
    h_corrM_tot_->Reset("ICES");

    for(Int_t i_x=0; i_x<nBinX; ++i_x) {
      Int_t i_binX = i_x+1;

      Double_t unc_x = sqrt( h_covM_tot_->GetBinContent(i_binX, i_binX) );

      for(Int_t i_y=0; i_y<nBinY; ++i_y) {
        Int_t i_binY = i_y+1;

        Double_t unc_y = sqrt( h_covM_tot_->GetBinContent(i_binY, i_binY) );

        Double_t cov_xy = h_covM_tot_->GetBinContent(i_binX, i_binY);

        if( unc_x*unc_y == 0 )
          throw std::runtime_error("[Calc_TotalCov] unc_x*unc_y == 0!");

        Double_t corr_xy = cov_xy / (unc_x*unc_y);
        if( corr_xy > 1.0 ) {
          printf("[Calc_TotalCov] (%02d, %02d) corr_xy = %lf > 1.0 ... force it to be corr_xy = 1.0\n", i_binX, i_binY, corr_xy);
          corr_xy = 1.0;
          h_covM_tot_->SetBinContent(i_binX, i_binY, corr_xy*unc_x*unc_y);
        }
        if( corr_xy < -1.0 ) {
          printf("[Calc_TotalCov] (%02d, %02d) corr_xy = %lf < -1.0 ... force it to be corr_xy = -1.0\n", i_binX, i_binY, corr_xy);
          corr_xy = -1.0;
          h_covM_tot_->SetBinContent(i_binX, i_binY, corr_xy*unc_x*unc_y);
        }

        h_corrM_tot_->SetBinContent(i_binX, i_binY, corr_xy);
        h_corrM_tot_->SetBinError(i_binX, i_binY, 0);
      }
    }
  }

  // -- compare: total absolute uncertainty vs. diagonal terms of the total covariance matrix
  void Check_Consistency() {

    TH1D* h_diag = (TH1D*)h_absUnc_tot_->Clone();
    h_diag->Reset("ICES");

    Int_t nBinX = h_covM_tot_->GetNbinsX();
    for(Int_t i=0; i<nBinX; ++i) {
      Int_t i_bin = i+1;

      Double_t unc_ith = sqrt( h_covM_tot_->GetBinContent(i_bin, i_bin) );
      h_diag->SetBinContent(i_bin, unc_ith);
    }

    cout << "[Check_Consistency] compare: h_absUnc_tot vs. h_diag" << endl;
    PlotTool::IsRatio1(h_absUnc_tot_, h_diag);
  }

  // -- should contain all information necessary for the combination
  // -- two versions: mass-bin-number axis & mass axis (with "mAxis" tag)
  // ---- central value
  // ---- total unc. and cov.
  // ---- individual unc. and cov.
  void Save() {
    TFile *f_output = TFile::Open(fileName_output_, "RECREATE");
    f_output->cd();

    // -- save the central value
    result_cv_->Save(f_output);

    f_output->cd();
    Save_UncAndCov(f_output, kFALSE);

    // -- mass axis version
    DYTool::Save_DYRun2Result_MassAxis(f_output, result_cv_, isFPS_);    
    Save_UncAndCov(f_output, kTRUE);

    f_output->Close();
  }

  void Save_UncAndCov(TFile* f_output, Bool_t useMassAxis) {
    Save1D(f_output, h_absUnc_tot_, "h_absUnc_tot", useMassAxis);
    Save1D(f_output, h_relUnc_tot_, "h_relUnc_tot", useMassAxis);

    Save2D(f_output, h_covM_tot_,  "h_covM_tot",  useMassAxis);
    Save2D(f_output, h_corrM_tot_, "h_corrM_tot", useMassAxis);

    for(const auto& h : vec_hist_relUnc_ ) Save1D(f_output, h, h->GetName(), useMassAxis);
    for(const auto& h : vec_hist_covM_ )   Save2D(f_output, h, h->GetName(), useMassAxis);
    for(const auto& h : vec_hist_corrM_ )  Save2D(f_output, h, h->GetName(), useMassAxis);
  }

  void Save1D(TFile *f_output, TH1D* h, TString histName_base, Bool_t useMassAxis, TString newTag = "mAxis") {
    TH1D* h_forSave = (TH1D*)h->Clone();
    if( !useMassAxis ) {
      h_forSave->SetName(histName_base);

      f_output->cd();
      h_forSave->Write();
    }
    else {
      h_forSave = DYTool::Convert_TUnfoldOutput_MassAxis(h_forSave);
      TString histName = histName_base + "_" + newTag;
      h_forSave->SetName(histName);

      f_output->cd();
      h_forSave->Write();
    }
  }

  void Save2D(TFile *f_output, TH2D* h2D, TString histName_base, Bool_t useMassAxis, TString newTag = "mAxis") {
    TH2D* h2D_forSave = (TH2D*)h2D->Clone();
    if( !useMassAxis ) {
      h2D_forSave->SetName(histName_base);

      f_output->cd();
      h2D_forSave->Write();
    }
    else {
      h2D_forSave = DYTool::Convert2D_TUnfoldOutput_MassAxis(h2D_forSave);
      TString histName = histName_base + "_" + newTag;
      h2D_forSave->SetName(histName);

      f_output->cd();
      h2D_forSave->Write();
    }
  }

  void ProducePlot_Unc(Bool_t useMassAxis = kFALSE) {
    TH1::AddDirectory(kFALSE);

    Bool_t isLogX = useMassAxis ? kTRUE : kFALSE;
    TString titleX = useMassAxis ? "m(ee) [GeV]" : "mass bin number";
    if( channel_ == "mm" ) titleX.ReplaceAll("ee", "#mu#mu");
    TString canvasName = "c_unc_"+channel_;
    if( useMassAxis ) canvasName += "_massAxis";

    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, isLogX, 0);
    canvas->SetTitle(titleX, "Rel. uncertainty");

    vector<TH1D*> vec_relUnc;
    for(const auto& uncInfo : vec_uncInfo_) {
      TString fileName = uncInfo.FileName(channel_);
      if( fileName == "" ) continue;
 
      TString histName = "h_relUnc_"+uncInfo.HistTag();
      TH1D* h_relUnc = PlotTool::Get_Hist(fileName, histName);
      if( useMassAxis )
        h_relUnc = DYTool::Convert_TUnfoldOutput_MassAxis(h_relUnc);

      TString legend = uncInfo.Legend();
      Int_t color = uncInfo.Color();
      canvas->Register(h_relUnc, legend, color );

      vec_relUnc.push_back( h_relUnc );
    }
    TH1D* h_relUnc_tot = PlotTool::QuadSum_Hist(vec_relUnc);

    canvas->Register(h_relUnc_tot, "Total (quad. sum)", kRed);

    canvas->SetLegendPosition(0.40, 0.60, 0.94, 0.90);

    canvas->SetRangeY(0, 1.01);
    // canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    TString uncInfo = "Systematic uncertainty ("+channelInfo+")";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);
    if( !useMassAxis )
      canvas->RegisterLatex(0.16, 0.87, 42, 0.6, "Lumi. unc. is not presented");

    // canvas->RegisterLatex(0.16, 0.87, 42, 0.6, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");

    canvas->SetCanvasName(canvasName+"_zoomIn");
    canvas->SetRangeY(0, 0.2);
    canvas->Draw("HISTLP");
  }

  void ProducePlot_2D(TString matrixType, Bool_t useMassAxis = kFALSE) {
    TH2D* h2D = nullptr;
    if( matrixType == "covM" )  h2D = (TH2D*)h_covM_tot_->Clone();
    if( matrixType == "corrM" ) h2D = (TH2D*)h_corrM_tot_->Clone();

    if( useMassAxis ) h2D = DYTool::Convert2D_TUnfoldOutput_MassAxis(h2D);

    TString canvasName = "c2D_"+matrixType+"_"+channel_;
    if( useMassAxis ) canvasName += "_massAxis";

    Bool_t isLogX = (useMassAxis) ? kTRUE : kFALSE;
    Bool_t isLogY = (useMassAxis) ? kTRUE : kFALSE;

    TString axisTitle = (useMassAxis) ? "m [GeV]" : "mass bin number";

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, isLogX, isLogY, 0);
    canvas->SetTitle(axisTitle, axisTitle);

    canvas->Register(h2D);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeZ(minZ, maxZ);
    if( matrixType == "corrM" ) canvas->SetRangeZ(-1.001, 1.001);
    else                        canvas->SetAutoRangeZ();

    canvas->Latex_CMSInternal();
    TString info = "Covariance matrix (total)";
    if( matrixType == "corrM" ) info.ReplaceAll("Covariance", "Correlation");
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, info);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }
};

void finalize_unc_FPS() {
  UncFinalizer finalizer_ee("ee");
  finalizer_ee.FullPhaseSpace();
  finalizer_ee.Finalize();

  UncFinalizer finalizer_mm("mm");
  finalizer_mm.FullPhaseSpace();
  finalizer_mm.Finalize();
}

void finalize_unc() {
  UncFinalizer finalizer_ee("ee");
  // finalizer_ee.Use_Fake(kFALSE);
  finalizer_ee.Finalize();

  UncFinalizer finalizer_mm("mm");
  finalizer_mm.Finalize();

  finalize_unc_FPS();
}