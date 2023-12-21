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

  void Finalize() {
    TH1::AddDirectory(kFALSE);
    Init();

    if( !gSystem->AccessPathName(fileName_output_) )
      throw std::runtime_error(fileName_output_+" already exists: remove or rename it");

    Calc_TotalUnc();
    Calc_TotalCov();
    Check_Consistency();

    ProducePlot_Unc();
    ProducePlot_Unc(kTRUE);
    ProducePlot_2D("covM");
    ProducePlot_2D("corrM");

    Save();
  }

private:
  TString channel_ = "";
  TString fileName_output_ = "";

  vector<UncInfo> vec_uncInfo_;
  // vector<TString> vec_uncType_;

  TH1D* h_absUnc_tot_;
  TH1D* h_relUnc_tot_;
  TH2D* h_covM_tot_;
  TH2D* h_corrM_tot_;

  void Init() {
    fileName_output_ = "UncAndCov_All_"+channel_+".root";

    TString path_default = "/Users/kplee/Research/Analysis/Logbook/231025_update_subtractDYFake";
    Insert_UncInfo("statData", "Data statistics", kBlack,
                   path_default+"/Stat/Uncertainty_and_Covariance_Stat_ee.root",
                   path_default+"/Stat/Uncertainty_and_Covariance_Stat_mm.root",
                   "stat_data");

    Insert_UncInfo("statMC", "MC statistics", kGray+2,
                   path_default+"/Stat/Uncertainty_and_Covariance_Stat_ee.root",
                   path_default+"/Stat/Uncertainty_and_Covariance_Stat_mm.root",
                   "stat_totMC");

    Insert_UncInfo("lumi", "Luminosity", kOrange-3,
                   path_default+"/Luminosity/Unfolded_And_Uncertainty_lumi_ee.root",
                   path_default+"/Luminosity/Unfolded_And_Uncertainty_lumi_mm.root",
                   "lumi_tot");

    Insert_UncInfo("effSF", "Efficiency SF", kBlue,
                   path_default+"/EffSF/UncAndCov_EffSF_ee.root",
                   path_default+"/EffSF/UncAndCov_EffSF_mm.root",
                   "effSF_tot");

    Insert_UncInfo("L1Pref", "L1 pre-firing", kViolet,
                   path_default+"/L1Prefiring/Unfolded_And_Uncertainty_L1Pref_ee.root",
                   path_default+"/L1Prefiring/Unfolded_And_Uncertainty_L1Pref_mm.root",
                   "L1Pref");

    Insert_UncInfo("pileup", "Pileup reweighting", kGreen+2,
                   path_default+"/Pileup/Unfolded_And_Uncertainty_pileup_ee.root",
                   path_default+"/Pileup/Unfolded_And_Uncertainty_pileup_mm.root",
                   "pileup");

    Insert_UncInfo("theory", "Theoretical inputs", kCyan,
                   path_default+"/Theory/UncAndCov_Theory_ee.root",
                   path_default+"/Theory/UncAndCov_Theory_mm.root",
                   "theory_tot");

    Insert_UncInfo("muP", "Muon momentum correction", kRed+2,
                   "",
                   path_default+"/muP/UncAndCov_muP.root",
                   "muP_tot");

    Insert_UncInfo("elE", "Electron energy correction", kRed+2,
                   path_default+"/elE/UncAndCov_elE.root",
                   "",
                   "elE_tot");

    // for(const auto& uncInfo : vec_uncInfo_ ) {
    //   TString uncType = uncInfo.Type();
    //   if( channel_ == "ee" && uncType == "muP" ) continue;
    //   if( channel_ == "mm" && uncType == "elE" ) continue;
    //   vec_uncType_.push_back( uncType );
    // }
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
    vector<TH1D*> vec_absUnc;
    vector<TH1D*> vec_relUnc;
    for(const auto& uncInfo : vec_uncInfo_) {
      TString fileName = uncInfo.FileName(channel_);
      if( fileName == "" ) continue;
      // cout << "uncType = " << uncInfo.Type() << ": fileName = " << fileName << endl;

      TString histName_absUnc = "h_absUnc_"+uncInfo.HistTag();
      TH1D* h_absUnc = PlotTool::Get_Hist(fileName, histName_absUnc);
      vec_absUnc.push_back( h_absUnc );

      TString histName_relUnc = "h_relUnc_"+uncInfo.HistTag();
      TH1D* h_relUnc = PlotTool::Get_Hist(fileName, histName_relUnc);
      vec_relUnc.push_back( h_relUnc );
    }

    h_absUnc_tot_ = PlotTool::QuadSum_Hist(vec_absUnc);
    h_relUnc_tot_ = PlotTool::QuadSum_Hist(vec_relUnc);
  }

  void Calc_TotalCov() {
    vector<TH2D*> vec_covM;
    for(const auto& uncInfo : vec_uncInfo_) {
      TString fileName = uncInfo.FileName(channel_);
      if( fileName == "" ) continue;

      TString histName_covM = "h_covM_"+uncInfo.HistTag();
      TH2D* h_covM = PlotTool::Get_Hist2D(fileName, histName_covM);
      vec_covM.push_back( h_covM );
    }

    // -- construct: covariance matrix
    h_covM_tot_ = (TH2D*)vec_covM[0]->Clone();
    h_covM_tot_->Reset("ICES");

    Int_t nBinX = h_covM_tot_->GetNbinsX();
    Int_t nBinY = h_covM_tot_->GetNbinsY();
    for(Int_t i_x=0; i_x<nBinX; ++i_x) {
      Int_t i_binX = i_x+1;

      for(Int_t i_y=0; i_y<nBinY; ++i_y) {
        Int_t i_binY = i_y+1;

        Double_t cov_tot = 0;
        for(const auto& h_covM : vec_covM )
          cov_tot += h_covM->GetBinContent(i_binX, i_binY);

        h_covM_tot_->SetBinContent(i_binX, i_binY, cov_tot);
        h_covM_tot_->SetBinError(i_binX, i_binY, 0);
      }
    }

    // -- construct: correlation matrix
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

  void Save() {
    TFile *f_output = TFile::Open(fileName_output_, "RECREATE");
    f_output->cd();

    // -- save the central value
    for(const auto& uncInfo : vec_uncInfo_ ) {
      if( uncInfo.Type() == "pileup" ) {
        TString fileName = uncInfo.FileName(channel_);
        TH1D* h_unfolded_data = PlotTool::Get_Hist(fileName, "h_allEra_unfolded_data_cv");
        h_unfolded_data->SetName("h_unfolded_data");

        TH1D* h_gen_DY = PlotTool::Get_Hist(fileName, "h_allEra_gen_DY_cv");
        h_gen_DY->SetName("h_gen_DY");

        f_output->cd();
        h_unfolded_data->Write();
        h_gen_DY->Write();

        break;
      }
    }

    h_absUnc_tot_->SetName("h_absUnc_tot");
    h_absUnc_tot_->Write();

    h_relUnc_tot_->SetName("h_relUnc_tot");
    h_relUnc_tot_->Write();

    h_covM_tot_->SetName("h_covM_tot");
    h_covM_tot_->Write();

    h_corrM_tot_->SetName("h_corrM_tot");
    h_corrM_tot_->Write();

    f_output->Close();
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

    canvas->SetRangeY(0, 0.4);
    // canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    TString uncInfo = "Systematic uncertainty ("+channelInfo+")";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);
    if( !useMassAxis )
      canvas->RegisterLatex(0.16, 0.87, 42, 0.6, "Lumi. unc. is not presented");

    // canvas->RegisterLatex(0.16, 0.87, 42, 0.6, channelInfo);

    canvas->SetSavePath("./plot");

    canvas->Draw("HISTLP");

    canvas->SetCanvasName(canvasName+"_zoomIn");
    canvas->SetRangeY(0, 0.2);
    canvas->Draw("HISTLP");
  }

  void ProducePlot_2D(TString matrixType) {

    TString canvasName = "c2D_"+matrixType+"_"+channel_;

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 0, 0, 0);
    canvas->SetTitle("mass bin number", "mass bin number");

    if( matrixType == "covM" )  canvas->Register(h_covM_tot_);
    if( matrixType == "corrM" ) canvas->Register(h_corrM_tot_);
    
    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeZ(minZ, maxZ);
    if( matrixType == "corrM" ) canvas->SetRangeZ(-1.01, 1.01);
    else                        canvas->SetAutoRangeZ();

    canvas->Latex_CMSInternal();
    TString info = "Covariance matrix (total)";
    if( matrixType == "corrM" ) info.ReplaceAll("Covariance", "Correlation");
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, info);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, channelInfo);

    canvas->SetSavePath("./plot");

    canvas->Draw();
  }
};


// -- update: make a class to hold the relevant info for each uncType
// namespace {
//   TString path_default = "/Users/kplee/Research/Analysis/Logbook/230729_update_Syst/Uncertainty";

//   std::map<TString, TString> map_path_mm = {
//     { "effSF",  path_default+"/EffSF/Uncertainty_EffSF_mm.root" },
//     { "L1Pref", path_default+"/L1Prefiring/Unfolded_And_Uncertainty_L1Pref_mm.root" },
//     { "pileup", path_default+"/Pileup/Unfolded_And_Uncertainty_pileup_mm.root" },
//     { "muP",    path_default+"/muP/UncAndCov_muP.root" },
//     { "theory", path_default+"/Theory/UncAndCov_Theory_mm.root" }
//   };

//   std::map<TString, TString> map_path_ee = {
//     { "effSF",  path_default+"/EffSF/Uncertainty_EffSF_ee.root" },
//     { "L1Pref", path_default+"/L1Prefiring/Unfolded_And_Uncertainty_L1Pref_ee.root" },
//     { "pileup", path_default+"/Pileup/Unfolded_And_Uncertainty_pileup_ee.root" },
//     { "theory", path_default+"/Theory/UncAndCov_Theory_ee.root" }
//   };

//   std::map<TString, TString> map_histTag = {
//     { "effSF", "effSF_total" },
//     { "L1Pref", "L1Pref" },
//     { "pileup", "pileup" },
//     { "muP",    "muP_tot" },
//     { "theory", "theory_tot" }
//   };

//   std::map<TString, TString> map_legend = {
//     { "effSF",  "Efficiency SF" },
//     { "L1Pref", "L1 pre-firing correction" },
//     { "pileup", "Pileup reweighting" },
//     { "muP",    "Muon momentum correction" },
//     { "theory", "Theoretical inputs" }
//   };

//   vector<Int_t> vec_color = {
//     kBlack, kBlue, kGreen+2, kViolet, kCyan, kRed+2, kGray
//   };
// }

// void producePlot_unc(TString channel) {
//   TH1::AddDirectory(kFALSE);

//   vector<TString> vec_uncType = {"effSF", "L1Pref", "pileup", "theory"};
//   if( channel == "mm" ) 
//     vec_uncType.push_back("muP");

//   TString canvasName = "c_unc_"+channel;
//   PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
//   canvas->SetTitle("mass bin number", "Rel. uncertainty");

//   vector<TH1D*> vec_relUnc;
//   Int_t i_case = 0;
//   for(const auto& uncType : vec_uncType) {
//     TString fileName = (channel == "mm") ? map_path_mm[uncType] : map_path_ee[uncType];
//     TString histName = "h_relUnc_"+map_histTag[uncType];

//     TH1D* h_relUnc = PlotTool::Get_Hist(fileName, histName);
//     canvas->Register(h_relUnc, map_legend[uncType], vec_color[i_case] );
//     i_case++;

//     vec_relUnc.push_back( h_relUnc );
//   }
//   TH1D* h_relUnc_tot = PlotTool::QuadSum_Hist(vec_relUnc);

//   canvas->Register(h_relUnc_tot, "Total (quad. sum)", kRed);

//   canvas->SetLegendPosition(0.50, 0.67, 0.94, 0.90);

//   canvas->SetRangeY(0, 0.45);
//   // canvas->SetAutoRangeY();

//   canvas->Latex_CMSInternal();
//   TString channelInfo = (channel == "mm") ? "Muon channel" : "Electron channel";
//   TString uncInfo = "Systematic uncertainty ("+channelInfo+")";
//   canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);

//   // canvas->RegisterLatex(0.16, 0.87, 42, 0.6, channelInfo);

//   // canvas->SetSavePath("./plot");

//   canvas->Draw("HISTLP");
// }

// void producePlot_2D(TString matrixType, TString channel) {

//   TString canvasName = "c2D_"+matrixType+"_"+uncType+"_"+channel_;

//   PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 0, 0, 0);
//   canvas->SetTitle("mass bin number", "mass bin number");

//   TString histName = "h_"+matrixType+"_theory_"+uncType;
//   TH2D* h2D = PlotTool::Get_Hist2D(fileName_, histName);
//   canvas->Register(h2D);

//   // canvas->SetRangeX(minX, maxX);
//   // canvas->SetRangeY(minY, maxY);
//   // canvas->SetRangeZ(minZ, maxZ);
//   if( matrixType == "corrM" ) canvas->SetRangeZ(-1.01, 1.01);
//   else                        canvas->SetAutoRangeZ();

//   canvas->Latex_CMSInternal();
//   TString info = "Covariance matrix ("+uncType+")";
//   if( matrixType == "corrM" ) info.ReplaceAll("Covariance", "Correlation");
//   canvas->RegisterLatex(0.16, 0.91, 42, 0.5, info);
//   TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
//   canvas->RegisterLatex(0.16, 0.87, 42, 0.5, channelInfo);

//   canvas->SetSavePath(plotDirPath_);

//   canvas->Draw();
// }

void producePlot_unc() {
  UncFinalizer finalizer_ee("ee");
  finalizer_ee.Finalize();

  UncFinalizer finalizer_mm("mm");
  finalizer_mm.Finalize();
}