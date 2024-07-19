#include "Common/SimplePlotTools.h"
#include "Common/DYTool.h"

class PlotProducer {
public:
  PlotProducer(TString channel, TString fileName):
  channel_(channel), fileName_(fileName) { }

  void Use_Fake(Bool_t flag = kTRUE) { useFake_ = flag; }

  void Produce() {
    TH1::AddDirectory(kFALSE);
    fileName_ = DYTool::Get_OutputPath(fileName_);

    plotDirPath_ = DYTool::Set_PlotPath("Stat/"+channel_);

    bool recursive = kTRUE;
    if( gSystem->mkdir(plotDirPath_.Data(), recursive) < 0 )
      throw std::runtime_error("Directory = " + plotDirPath_ + " cannot be created (already exists?)");

    ProducePlot_Validation();
    ProducePlot_Unc();

    vector<TString> vec_uncType = {"stat_data", "stat_DYMC", "stat_bkgMC", "stat_bkgDYFake", "stat_totMC", "stat_tot", "stat_tot_TUnfold"};
    if( useFake_ )
      vec_uncType.push_back( "stat_bkgFakeLep" );
    for(const auto& uncType : vec_uncType)
      ProducePlot_2D("corrM", uncType);
  }

private:
  TString channel_;
  TString fileName_;
  TString plotDirPath_;

  Bool_t useFake_ = kTRUE; // -- default: true

  void ProducePlot_Validation() {
    TString canvasName = "c_unc_validation_"+channel_;
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    // TString titleX = "m(#mu#mu) [GeV]";
    // if( channel_ == "ee" ) titleX.ReplaceAll("#mu", "e");
    TString titleX = "mass bin number";
    canvas->SetTitle(titleX, "Rel. uncertainty");

    TH1D* h_relUnc_stat_tot         = PlotTool::Get_Hist(fileName_, "h_relUnc_stat_tot");
    TH1D* h_relUnc_stat_tot_TUnfold = PlotTool::Get_Hist(fileName_, "h_relUnc_stat_tot_TUnfold");

    canvas->Register(h_relUnc_stat_tot,         "Quad. sum (data, DY MC, bkg. MC)", kBlack);
    canvas->Register(h_relUnc_stat_tot_TUnfold, "Total Unc. (from TUnfold)", kBlue);

    canvas->SetLegendPosition(0.55, 0.70, 0.94, 0.91);

    // canvas->SetRangeY(0, 0.03);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the data & MC statistics";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.6, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");

    canvas->SetRangeY(0, 0.1);
    canvas->SetCanvasName( canvasName + "_zoomIn" );
    canvas->Draw("HISTLP");

    PlotTool::IsRatio1( h_relUnc_stat_tot, h_relUnc_stat_tot_TUnfold );
  }

  void ProducePlot_Unc() {
    TString canvasName = "c_unc_tot_"+channel_;
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    // TString titleX = "m(#mu#mu) [GeV]";
    // if( channel_ == "ee" ) titleX.ReplaceAll("#mu", "e");
    TString titleX = "mass bin number";
    canvas->SetTitle(titleX, "Rel. uncertainty");

    TH1D* h_relUnc_stat_data  = PlotTool::Get_Hist(fileName_, "h_relUnc_stat_data");
    TH1D* h_relUnc_stat_DYMC  = PlotTool::Get_Hist(fileName_, "h_relUnc_stat_DYMC");
    TH1D* h_relUnc_stat_bkgMC = PlotTool::Get_Hist(fileName_, "h_relUnc_stat_bkgMC");
    TH1D* h_relUnc_stat_bkgDYFake = PlotTool::Get_Hist(fileName_, "h_relUnc_stat_bkgDYFake");
    TH1D* h_relUnc_stat_tot   = PlotTool::Get_Hist(fileName_, "h_relUnc_stat_tot");

    canvas->Register(h_relUnc_stat_data, "Stat. (data)", kBlack);
    canvas->Register(h_relUnc_stat_DYMC, "Stat. (DY MC)", kBlue);
    canvas->Register(h_relUnc_stat_bkgMC, "Stat. (bkg. MC)", kGreen+2);
    canvas->Register(h_relUnc_stat_bkgDYFake, "Stat. (Fake, from DY MC)", kViolet);

    if( useFake_ ) {
      TH1D* h_relUnc_stat_bkgFakeLep = PlotTool::Get_Hist(fileName_, "h_relUnc_stat_bkgFakeLep");
      canvas->Register(h_relUnc_stat_bkgFakeLep, "Stat. (Fake lepton bkg.)", kCyan);
    }

    canvas->Register(h_relUnc_stat_tot, "Total (quad. sum)", kRed);

    canvas->SetLegendPosition(0.55, 0.70, 0.94, 0.91);

    // canvas->SetRangeY(0, 0.03);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the data & MC statistics";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.6, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");

    canvas->SetRangeY(0, 0.1);
    canvas->SetCanvasName( canvasName + "_zoomIn" );
    canvas->Draw("HISTLP");
  }

  void ProducePlot_2D(TString matrixType, TString uncType) {
    TString canvasName = "c2D_"+matrixType+"_"+uncType+"_"+channel_;

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 0, 0, 0);
    // TString title = "m(#mu#mu) [GeV]";
    // if( channel_ == "ee" ) title.ReplaceAll("#mu", "e");
    TString title = "mass bin number";
    canvas->SetTitle(title, title);

    TString histName = "h_"+matrixType+"_"+uncType;
    TH2D* h2D = PlotTool::Get_Hist2D(fileName_, histName);
    canvas->Register(h2D);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeZ(minZ, maxZ);
    if( matrixType == "corrM" ) canvas->SetRangeZ(-1.01, 1.01);
    else                        canvas->SetAutoRangeZ();

    canvas->Latex_CMSInternal();
    TString info = "Covariance matrix ("+uncType+")";
    if( matrixType == "corrM" ) info.ReplaceAll("Covariance", "Correlation");
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, info);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }
};

void producePlot_unc_and_cov_stat() {
  TString fileName_mm = "Uncertainty_and_Covariance_Stat_mm.root";
  TString fileName_ee = fileName_mm;
  fileName_ee.ReplaceAll("mm", "ee");

  PlotProducer producer_ee("ee", fileName_ee);
  producer_ee.Produce();

  PlotProducer producer_mm("mm", fileName_mm);
  producer_mm.Produce();


}