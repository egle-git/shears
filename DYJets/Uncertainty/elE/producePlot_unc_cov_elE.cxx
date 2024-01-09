#include "Common/SimplePlotTools.h"

class PlotProducer {
public:
  PlotProducer(TString fileName): fileName_(fileName) {}

  void Produce() {
    TH1::AddDirectory(kFALSE);

    ProducePlot_Unc_Stat();
    ProducePlot_Unc_Syst();
    ProducePlot_Unc();

    ProducePlot_2D("corrM", "stat_16pre");
    ProducePlot_2D("corrM", "stat_16post");
    ProducePlot_2D("corrM", "stat_17");
    ProducePlot_2D("corrM", "stat_18");
    ProducePlot_2D("corrM", "stat_tot");


    ProducePlot_2D("corrM", "syst_set2");
    ProducePlot_2D("corrM", "syst_set3");
    ProducePlot_2D("corrM", "syst_set4");
    ProducePlot_2D("corrM", "syst_set5");
    ProducePlot_2D("corrM", "syst_set6");
    ProducePlot_2D("corrM", "syst_tot");

    ProducePlot_2D("corrM", "tot");
  }

private:
  TString fileName_ = "";
  TString plotDirPath_ = "./plot";

  void ProducePlot_Unc_Stat() {
    TString canvasName = "c_unc_stat";
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    TH1D* h_relUnc_16pre   = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_stat_16pre");
    TH1D* h_relUnc_16post  = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_stat_16post");
    TH1D* h_relUnc_17      = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_stat_17");
    TH1D* h_relUnc_18      = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_stat_18");
    TH1D* h_relUnc_totStat = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_stat_tot");

    canvas->Register(h_relUnc_16pre,   "Stat. (2016, preAPV)", kBlack);
    canvas->Register(h_relUnc_16post,  "Stat. (2016, postAPV)", kBlue);
    canvas->Register(h_relUnc_17,      "Stat. (2017)", kGreen+2);
    canvas->Register(h_relUnc_18,      "Stat. (2018)", kViolet);
    canvas->Register(h_relUnc_totStat, "Stat. (quad. sum)", kRed);

    canvas->SetLegendPosition(0.50, 0.68, 0.94, 0.90);

    // canvas->SetRangeY(0, 0.03);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the electron energy correction (stat.)";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");
  }

  void ProducePlot_Unc_Syst() {
    TString canvasName = "c_unc_syst";
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    TH1D* h_relUnc_set2 = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_syst_set2");
    TH1D* h_relUnc_set3 = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_syst_set3");
    TH1D* h_relUnc_set4 = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_syst_set4");
    TH1D* h_relUnc_set5 = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_syst_set5");
    TH1D* h_relUnc_set6 = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_syst_set6");
    TH1D* h_relUnc_totSyst = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_syst_tot");

    canvas->Register(h_relUnc_set2, "Syst. (set2, Z p_{T} modeling)", kBlue);
    canvas->Register(h_relUnc_set3, "Syst. (set3, alt. mass range)", kGreen+2);
    canvas->Register(h_relUnc_set4, "Syst. (set4, non-linearity)", kViolet);
    canvas->Register(h_relUnc_set5, "Syst. (set5, asymmetric resolution corr.)", kBlack);
    canvas->Register(h_relUnc_set6, "Syst. (set6, choice of scale reference point)", kCyan);
    canvas->Register(h_relUnc_set6, "Syst. (set7, run-inclusive resolution correction)", kGray);
    canvas->Register(h_relUnc_set6, "Syst. (set8, background systematics)", kBlue-9);
    canvas->Register(h_relUnc_totSyst, "Syst. (quad. sum)", kRed);

    canvas->SetLegendPosition(0.40, 0.68, 0.94, 0.90);

    // canvas->SetRangeY(0, 0.03);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the electron energy correction (syst.)";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");
  }

  void ProducePlot_Unc() {
    TString canvasName = "c_unc_tot";
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    TH1D* h_relUnc_stat = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_stat_tot");
    TH1D* h_relUnc_syst = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_syst_tot");
    TH1D* h_relUnc_tot  = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_tot");

    canvas->Register(h_relUnc_stat, "Stat.", kBlack);
    canvas->Register(h_relUnc_syst, "Syst.", kBlue);
    canvas->Register(h_relUnc_tot, "Total (quad. sum)", kRed);

    canvas->SetLegendPosition(0.60, 0.74, 0.94, 0.90);

    // canvas->SetRangeY(0, 0.03);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the electron energy correction";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");

    canvas->SetCanvasName(canvasName+"_zoomIn");
    canvas->SetRangeY(0, 0.05);
    canvas->Draw("HISTLP");
  }

  void ProducePlot_2D(TString matrixType, TString uncType) {
    TString canvasName = "c2D_"+matrixType+"_"+uncType;

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 0, 0, 0);
    canvas->SetTitle("mass bin number", "mass bin number");

    TString histName = "h_"+matrixType+"_elE_"+uncType;
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

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }
};

void producePlot_unc_cov_elE() {
  PlotProducer producer("UncAndCov_elE.root");
  producer.Produce();
}