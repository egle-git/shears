#include "Common/SimplePlotTools.h"
#include "Common/DYTool.h"

class PlotProducer {
public:
  PlotProducer(TString fileName): fileName_(fileName) { }

  void Produce() {
    fileName_ = DYTool::Get_OutputPath(fileName_);
    plotDirPath_ = DYTool::Set_PlotPath("elE");
    
    TH1::AddDirectory(kFALSE);
    ProducePlot_Unc();
    ProducePlot_Unc(kTRUE);

    ProducePlot_2D("corrM", "scale");
    ProducePlot_2D("corrM", "smearing");
    ProducePlot_2D("corrM", "tot");

    ProducePlot_2D("covM", "scale");
    ProducePlot_2D("covM", "smearing");
    ProducePlot_2D("covM", "tot");
  }

private:
  TString fileName_ = "";
  TString plotDirPath_ = "";

  void ProducePlot_Unc(Bool_t useMassAxis = kFALSE) {
    Bool_t isLogX = useMassAxis ? kTRUE : kFALSE;
    TString titleX = useMassAxis ? "m(ee) [GeV]" : "mass bin number";
    TString canvasName = "c_unc_tot";
    if( useMassAxis ) canvasName += "_massAxis";

    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, isLogX, 0);

    canvas->SetTitle(titleX, "Rel. uncertainty");

    TH1D* h_relUnc_scale    = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_scale");
    TH1D* h_relUnc_smearing = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_smearing");
    TH1D* h_relUnc_tot      = PlotTool::Get_Hist(fileName_, "h_relUnc_elE_tot");

    if( useMassAxis ) {
      h_relUnc_scale    = DYTool::Convert_TUnfoldOutput_MassAxis(h_relUnc_scale);
      h_relUnc_smearing = DYTool::Convert_TUnfoldOutput_MassAxis(h_relUnc_smearing);
      h_relUnc_tot      = DYTool::Convert_TUnfoldOutput_MassAxis(h_relUnc_tot);
    }

    canvas->Register(h_relUnc_scale,    "Scale unc. (Data-only)", kBlack);
    canvas->Register(h_relUnc_smearing, "Smearing unc. (MC-only)", kBlue);
    canvas->Register(h_relUnc_tot,      "Total (quad. sum)", kRed);

    canvas->SetLegendPosition(0.60, 0.74, 0.94, 0.90);

    // canvas->SetRangeY(0, 0.03);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the electron energy correction";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);

    canvas->SetSavePath(plotDirPath_);

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
  TString fileName_input = "UncAndCov_elE.root";
  PlotProducer producer(fileName_input);
  producer.Produce();
}