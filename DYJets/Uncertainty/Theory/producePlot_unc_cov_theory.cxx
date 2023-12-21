#include "Common/SimplePlotTools.h"

class PlotProducer {
public:
  PlotProducer(TString channel, TString fileName): 
  channel_(channel), fileName_(fileName) {}

  void Produce() {
    TH1::AddDirectory(kFALSE);

    ProducePlot_Unc();

    ProducePlot_2D("corrM", "PDFHessian");
    ProducePlot_2D("corrM", "syst_alphaS");
    ProducePlot_2D("corrM", "syst_scale");
    ProducePlot_2D("corrM", "syst_tot");
    ProducePlot_2D("corrM", "tot");
  }

private:
  TString channel_ = "";
  TString fileName_ = "";
  TString plotDirPath_ = "./plot";

  void ProducePlot_Unc_Syst() {
    TString canvasName = "c_unc_syst";
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    TH1D* h_relUnc_set2 = PlotTool::Get_Hist(fileName_, "h_relUnc_muP_syst_set2");
    TH1D* h_relUnc_set3 = PlotTool::Get_Hist(fileName_, "h_relUnc_muP_syst_set3");
    TH1D* h_relUnc_set4 = PlotTool::Get_Hist(fileName_, "h_relUnc_muP_syst_set4");
    TH1D* h_relUnc_totSyst = PlotTool::Get_Hist(fileName_, "h_relUnc_muP_syst_tot");

    canvas->Register(h_relUnc_set2, "Syst. (set2, w/o Z p_{T} reweighting)", kBlack);
    canvas->Register(h_relUnc_set3, "Syst. (set3, w/o ad-hoc EWK weights)", kBlue);
    canvas->Register(h_relUnc_set4, "Syst. (set4, alt. profile #DeltaM mass window)", kGreen+2);
    canvas->Register(h_relUnc_totSyst, "Syst. (quad. sum)", kRed);

    canvas->SetLegendPosition(0.40, 0.68, 0.94, 0.90);

    // canvas->SetRangeY(0, 0.03);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the muon momentum correction (syst.)";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");
  }

  void ProducePlot_Unc() {
    TString canvasName = "c_unc_tot_"+channel_;
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    TH1D* h_relUnc_PDFHessian  = PlotTool::Get_Hist(fileName_, "h_relUnc_theory_PDFHessian");
    TH1D* h_relUnc_syst_alphaS = PlotTool::Get_Hist(fileName_, "h_relUnc_theory_syst_alphaS");
    TH1D* h_relUnc_syst_scale  = PlotTool::Get_Hist(fileName_, "h_relUnc_theory_syst_scale");
    TH1D* h_relUnc_tot         = PlotTool::Get_Hist(fileName_, "h_relUnc_theory_tot");

    canvas->Register(h_relUnc_PDFHessian, "PDF Hessian sets (100)", kBlack);

    canvas->Register(h_relUnc_syst_alphaS, "#alpha_{S} variation", kBlue);
    canvas->Register(h_relUnc_syst_scale, "Scale variation", kGreen+2);
    canvas->Register(h_relUnc_tot, "Total (quad. sum)", kRed);

    canvas->SetLegendPosition(0.60, 0.74, 0.94, 0.90);

    // canvas->SetRangeY(0, 0.275);
    // canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the theoretical inputs";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.6, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");
  }

  void ProducePlot_2D(TString matrixType, TString uncType) {
    TString canvasName = "c2D_"+matrixType+"_"+uncType+"_"+channel_;

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 0, 0, 0);
    canvas->SetTitle("mass bin number", "mass bin number");

    TString histName = "h_"+matrixType+"_theory_"+uncType;
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

void producePlot_unc_cov_theory(TString channel) {
  PlotProducer producer(channel, "UncAndCov_Theory_"+channel+".root");
  producer.Produce();
}

void producePlot_unc_cov_theory() {
  producePlot_unc_cov_theory("ee");
  producePlot_unc_cov_theory("mm");
}