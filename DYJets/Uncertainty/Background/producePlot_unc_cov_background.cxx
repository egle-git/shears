#include "Common/SimplePlotTools.h"



class PlotProducer {
public:
  PlotProducer(TString channel): channel_(channel) {}

  void Produce() {
    fileName_ = "Unfolded_And_Uncertainty_Bkg_"+channel_+".root";
    plotDirPath_ = "./plot";

    ProducePlot_Unc_TopBkg();
    ProducePlot_Unc_FakeLepBkg();
    ProducePlot_Unc_All();

    // -- top bkg.
    ProducePlot_2D("corrM", "topBkg_fake");
    ProducePlot_2D("corrM", "topBkg_fitParam");

    // -- fake lepton bkg.
    ProducePlot_2D("corrM", "fakeLepBkg_stat");
    ProducePlot_2D("corrM", "fakeLepBkg_constRatio");
    ProducePlot_2D("corrM", "fakeLepBkg_fitParam");
    if( channel_ == "ee" ) {
      ProducePlot_2D("corrM", "fakeLepBkg_misID");
      ProducePlot_2D("corrM", "fakeLepBkg_altInterp");
    }
    ProducePlot_2D("corrM", "fakeLepBkg_tot");

    ProducePlot_2D("corrM", "bkg_tot");
  }

private:
  TString channel_ = "";
  TString fileName_ = "";

  TString plotDirPath_ = "";

  void ProducePlot_Unc_TopBkg() {
    TString canvasName = "c_unc_topBkg_"+channel_;

    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    TH1D* h_relUnc_fake     = PlotTool::Get_Hist(fileName_, "h_relUnc_topBkg_fake");
    TH1D* h_relUnc_fitParam = PlotTool::Get_Hist(fileName_, "h_relUnc_topBkg_fitParam");
    TH1D* h_relUnc_tot      = PlotTool::Get_Hist(fileName_, "h_relUnc_topBkg_tot");

    canvas->Register(h_relUnc_fake,     "e#mu fake estimation", kBlack);
    canvas->Register(h_relUnc_fitParam, "Fit parameter", kBlue);
    canvas->Register(h_relUnc_tot, "Total (quad. sum)", kRed);

    canvas->SetLegendPosition(0.16, 0.65, 0.60, 0.87);

    canvas->SetRangeY(0, 0.01);
    // canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the reweighting factor for the top-quark backgrounds";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, uncInfo);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");
  }

  void ProducePlot_Unc_FakeLepBkg() {
    TString canvasName = "c_unc_fakeLepBkg_"+channel_;

    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    TH1D* h_relUnc_stat       = PlotTool::Get_Hist(fileName_, "h_relUnc_fakeLepBkg_stat");
    TH1D* h_relUnc_constRatio = PlotTool::Get_Hist(fileName_, "h_relUnc_fakeLepBkg_constRatio");
    TH1D* h_relUnc_fitParam   = PlotTool::Get_Hist(fileName_, "h_relUnc_fakeLepBkg_fitParam");
    TH1D* h_relUnc_misID = nullptr;
    TH1D* h_relUnc_altInterp = nullptr;
    if( channel_ == "ee" ) {
      h_relUnc_misID     = PlotTool::Get_Hist(fileName_, "h_relUnc_fakeLepBkg_misID");
      h_relUnc_altInterp = PlotTool::Get_Hist(fileName_, "h_relUnc_fakeLepBkg_altInterp");
    }
    TH1D* h_relUnc_tot      = PlotTool::Get_Hist(fileName_, "h_relUnc_fakeLepBkg_tot");

    canvas->Register(h_relUnc_stat,       "stat. (same sign data sample)", kBlack);
    canvas->Register(h_relUnc_constRatio, "const. OS/SS ratio", kCyan);
    canvas->Register(h_relUnc_fitParam, "Fit parameter (OS/SS ratio fit)", kBlue);
    if( channel_ == "ee" ) {
      canvas->Register(h_relUnc_misID,     "Charge mis-identification", kGreen+2);
      canvas->Register(h_relUnc_altInterp, "Alternative interpolation (Z peak)", kViolet);
    }
    canvas->Register(h_relUnc_tot, "Total (quad. sum)", kRed);

    canvas->SetLegendPosition(0.16, 0.65, 0.60, 0.87);
    if( channel_ == "mm" )
      canvas->SetLegendPosition(0.50, 0.65, 0.95, 0.87);

    // canvas->SetRangeY(0, 0.275);
    if( channel_ == "ee" )      canvas->SetAutoRangeY();
    else if( channel_ == "mm" ) canvas->SetRangeY(0, 0.02);

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the fake lepton backgrounds (same sign method)";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, uncInfo);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");

    if( channel_ == "ee" ) {
      canvas->SetCanvasName(canvasName+"_zoomIn");
      canvas->SetAutoRangeY(kFALSE);
      canvas->SetRangeY(0, 0.02);
      canvas->Draw("HISTLP");
    }
  }

  void ProducePlot_Unc_All() {
    TString canvasName = "c_unc_tot_"+channel_;

    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    TH1D* h_relUnc_topBkg     = PlotTool::Get_Hist(fileName_, "h_relUnc_topBkg_tot");
    TH1D* h_relUnc_fakeLepBkg = PlotTool::Get_Hist(fileName_, "h_relUnc_fakeLepBkg_tot");
    TH1D* h_relUnc_tot        = PlotTool::Get_Hist(fileName_, "h_relUnc_bkg_tot");


    canvas->Register(h_relUnc_topBkg,     "Reweighting factor on top-quark bkg.", kBlack);
    canvas->Register(h_relUnc_fakeLepBkg, "Fake lepton bkg.", kBlue);
    canvas->Register(h_relUnc_tot, "Total (quad. sum)", kRed);

    canvas->SetLegendPosition(0.20, 0.65, 0.78, 0.87);

    // canvas->SetRangeY(0, 0.275);
    if( channel_ == "ee" )      canvas->SetAutoRangeY();
    else if( channel_ == "mm" ) canvas->SetRangeY(0, 0.02);

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the data-driven backgrounds";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, uncInfo);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");

    // canvas->SetCanvasName(canvasName+"_zoomIn");
    // canvas->SetRangeY(0, 0.2);
    // canvas->Draw("HISTLP");

    if( channel_ == "ee" ) {
      canvas->SetCanvasName(canvasName+"_zoomIn");
      canvas->SetAutoRangeY(kFALSE);
      canvas->SetRangeY(0, 0.02);
      canvas->Draw("HISTLP");
    }
  }


  void ProducePlot_2D(TString matrixType, TString uncType) {
    TString canvasName = "c2D_"+matrixType+"_"+uncType+"_"+channel_;

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 0, 0, 0);
    canvas->SetTitle("mass bin number", "mass bin number");

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


void producePlot_unc_cov_background() {
  PlotProducer producer_ee("ee");
  producer_ee.Produce();

  PlotProducer producer_mm("mm");
  producer_mm.Produce();
}