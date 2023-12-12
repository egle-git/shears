#include "Common/SimplePlotTools.h"
#include "Common/DYPath.h"
#include "Common/DYTool.h"
#include "Common/DYUncertainty.h"

namespace {
  std::map<TString, TString> map_histName_cv = {
    {"gen_DY", "true"},
    {"unfolded_data", "unfolded"},
    {"migM", "matrix"}
  };

  std::map<TString, TString> map_histName_alt = {
    {"gen_DY", "true_altMC"},
    {"unfolded_data", "unfolded_altMC"},
    {"migM", "matrix_altMC"}
  };

};

class PlotProducer {
public:
  PlotProducer(TString channel): channel_(channel) {}

  void Set_FileName(TString name) { fileName_ = name; }

  void Use_Fake(Bool_t flag = kTRUE) { useFake_ = flag; }

  void Produce() {
    TH1::AddDirectory(kFALSE);
    plotDirPath_ = "./plot/"+channel_;
    DYTool::Make_Dir(plotDirPath_);

    Validation();
    Compare();
    EstimateUncAndSave();
  }

private:
  TString channel_ = "";
  TString fileName_ = "";
  Bool_t useFake_ = kTRUE; // -- default: true
  TString plotDirPath_ = "";

  void Compare() {
    ComparisonPlot("gen_DY");
    ComparisonPlot("unfolded_data");
  }

  void ComparisonPlot(TString histType) {
    TH1D* h_cv  = PlotTool::Get_Hist(fileName_, map_histName_cv[histType]+"_"+channel_);
    TH1D* h_alt = PlotTool::Get_Hist(fileName_, map_histName_alt[histType]+"_"+channel_);

    DYTool::Remove_NegativeBin(h_cv);
    DYTool::Remove_NegativeBin(h_alt);

    TString canvasName = "c_comp_"+histType+"_"+channel_;
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 0, 1);
    canvas->SetTitle("mass bin number", "# events", "alt./default");
    canvas->Register(h_cv,  "Default MC (MiNNLO)", kBlack);
    canvas->Register(h_alt, "Alt. MC (aMC@NLO)",   kBlue);

    canvas->SetLegendPosition(0.60, 0.74, 0.94, 0.90);

    // canvas->SetRangeY(0, 0.275);
    canvas->SetAutoRangeY();
    canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, channelInfo);
    TString info = "histType = ("+histType+"), Stat. uncertainty only";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.6, info);

    canvas->SetSavePath(plotDirPath_);

    // canvas->RemoveRatioError();

    canvas->Draw();
  }

  void Validation() {
    DYRun2Result* result_cv = new DYRun2Result(DYTool::path_default+"/"+channel_);
    if( useFake_ ) DYTool::Set_Fake(channel_, result_cv);
    result_cv->Produce();

    Validation("gen",      "DY",   result_cv);
    Validation("unfolded", "data", result_cv);
  }

  void Validation(TString level, TString sample, DYRun2Result* result_cv) {
    TH1D* h_default = result_cv->Get_AllEra(level, sample);
    TString histType = TString::Format("%s_%s", level.Data(), sample.Data());
    TH1D* h_Robert  = PlotTool::Get_Hist(fileName_, map_histName_cv[histType]+"_"+channel_);

    TString canvasName = "c_validation_"+histType+"_"+channel_;
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 0, 1);
    canvas->SetTitle("mass bin number", "# events", "Robert/default");
    canvas->Register(h_default, "Default",         kBlack);
    canvas->Register(h_Robert,  "Robert's result", kBlue);

    canvas->SetLegendPosition(0.60, 0.74, 0.94, 0.90);

    // canvas->SetRangeY(0, 0.275);
    canvas->SetAutoRangeY();
    canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, channelInfo);
    TString info = "histType = ("+histType+")";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.6, info);

    canvas->SetSavePath(plotDirPath_);

    canvas->RemoveRatioError();

    canvas->Draw();

    // canvas->SetCanvasName(canvasName+"_zoomIn");
    // canvas->SetRangeY(0, 0.2);
    // canvas->Draw("HISTLP");
  }

  void EstimateUncAndSave() {
    TH1D* h_cv  = PlotTool::Get_Hist(fileName_, map_histName_cv["unfolded_data"]+"_"+channel_);
    TH1D* h_alt = PlotTool::Get_Hist(fileName_, map_histName_alt["unfolded_data"]+"_"+channel_);

    Uncertainty* unc = new Uncertainty("unfold_model", "alternative", "fullyCorr");

    unc->Set_CentralHist( h_cv );
    unc->Set_AltHist( {h_alt} );
    unc->Estimate();

    ProducePlot_Unc(unc->RelUnc());
    ProducePlot_2D("covM", unc->CovM());
    ProducePlot_2D("corrM", unc->CorrM());

    TString fileName_output = "UncAndCov_Unfolding_"+channel_+".root";
    TFile *f_output = TFile::Open(fileName_output, "RECREATE");
    unc->Save(f_output);
  }

  void ProducePlot_Unc(TH1D* h_relUnc) {
    TString canvasName = "c_relUnc_"+channel_;
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");
    canvas->Register(h_relUnc, "Unc. (Unfolding, Model)", kBlack);

    canvas->SetLegendPosition(0.50, 0.74, 0.94, 0.91);

    // canvas->SetRangeY(0, 0.275);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, channelInfo);

    canvas->SetSavePath(plotDirPath_);
    canvas->Draw("HISTLP");

    canvas->SetCanvasName(canvasName+"_zoomIn");
    canvas->SetRangeY(0, 0.2);
    canvas->Draw("HISTLP");
  }

  void ProducePlot_2D(TString matrixType, TH2D* h2D) {
    TString canvasName = "c2D_"+matrixType+"_"+channel_;

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 0, 0, 0);
    canvas->SetTitle("mass bin number", "mass bin number");

    canvas->Register(h2D);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeZ(minZ, maxZ);
    if( matrixType == "corrM" ) canvas->SetRangeZ(-1.01, 1.01);
    else                        canvas->SetAutoRangeZ();

    canvas->Latex_CMSInternal();
    TString info = "Covariance matrix (unfolding, model)";
    if( matrixType == "corrM" ) info.ReplaceAll("Covariance", "Correlation");
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, info);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }

};

void producePlot_unc_cov_unfolding() {
  PlotProducer producer_ee("ee");
  producer_ee.Set_FileName("unfolding_model_uncertainties.root");
  producer_ee.Produce();

  PlotProducer producer_mm("mm");
  producer_mm.Set_FileName("unfolding_model_uncertainties.root");
  producer_mm.Produce();
}