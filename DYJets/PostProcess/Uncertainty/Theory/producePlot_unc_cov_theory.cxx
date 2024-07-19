#include "Common/SimplePlotTools.h"
#include "Common/DYTool.h"

class PlotProducer {
public:
  PlotProducer(TString channel, TString fileName): 
  channel_(channel), fileName_(fileName) {
    if( fileName.Contains("_FPS") )
      isFPS_ = kTRUE;
  }

  void Produce() {
    TH1::AddDirectory(kFALSE);
    fileName_ = DYTool::Get_OutputPath(fileName_);

    TString dirName = isFPS_ ? "Theory/FPS" : "Theory/fid";
    plotDirPath_ = DYTool::Set_PlotPath(dirName);

    ProducePlot_Unc();
    if( isFPS_ ) {
      Compare_Fiducial_FullPhaseSpace("PDFHessian");
      Compare_Fiducial_FullPhaseSpace("syst_alphaS");
      Compare_Fiducial_FullPhaseSpace("syst_scale");
      Compare_Fiducial_FullPhaseSpace("syst_tot");
      Compare_Fiducial_FullPhaseSpace("tot");
    }

    ProducePlot_2D("corrM", "PDFHessian");
    ProducePlot_2D("corrM", "syst_alphaS");
    ProducePlot_2D("corrM", "syst_scale");
    ProducePlot_2D("corrM", "syst_tot");
    ProducePlot_2D("corrM", "tot");
  }

private:
  TString channel_ = "";
  TString fileName_ = "";
  TString plotDirPath_ = "";

  Bool_t isFPS_ = kFALSE; // -- is full phase space result?

  void Compare_Fiducial_FullPhaseSpace(TString tag) {
    TString canvasName = "c_comp_relUnc_fid_FPS_"+tag+"_"+channel_;
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty", "FPS/fid.");

    TString fileName_fiducial = fileName_;
    fileName_fiducial.ReplaceAll("_FPS", "");
    TH1D* h_fiducial = PlotTool::Get_Hist(fileName_fiducial, "h_relUnc_theory_"+tag);
    TH1D* h_FPS      = PlotTool::Get_Hist(fileName_,         "h_relUnc_theory_"+tag);

    canvas->Register(h_fiducial, "Unc. on fiducial result", kBlack);
    canvas->Register(h_FPS, "Unc. on full phase space result", kBlue);

    canvas->SetLegendPosition(0.50, 0.74, 0.94, 0.90);

    // canvas->SetRangeY(0, 0.275);
    canvas->SetAutoRangeY();
    canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the theoretical inputs (tag = "+tag+")";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.6, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");

    // canvas->SetCanvasName(canvasName+"_zoomIn");
    // canvas->SetRangeY(0, 0.2);
    // canvas->Draw("HISTLP");
  }

  void ProducePlot_Unc() {
    TString canvasName = "c_unc_tot_"+channel_;
    if( isFPS_ )
      canvasName.ReplaceAll("c_unc", "c_unc_FPS");

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

    canvas->SetRangeY(0, 0.2);

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the theoretical inputs";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.6, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");

    canvas->SetCanvasName(canvasName+"_zoomIn");
    canvas->SetRangeY(0, 0.08);
    canvas->Draw("HISTLP");
  }

  void ProducePlot_2D(TString matrixType, TString uncType) {
    TString canvasName = "c2D_"+matrixType+"_"+uncType+"_"+channel_;
    if( isFPS_ )
      canvasName.ReplaceAll("c2D_", "c2D_FPS_");

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

// -- FPS = full phase space
void producePlot_unc_cov_theory_FPS(TString channel) {
  PlotProducer producer(channel, "UncAndCov_Theory_"+channel+"_FPS.root");
  producer.Produce();
}

void producePlot_unc_cov_theory() {
  producePlot_unc_cov_theory("ee");
  producePlot_unc_cov_theory("mm");

  producePlot_unc_cov_theory_FPS("ee");
  producePlot_unc_cov_theory_FPS("mm");
}