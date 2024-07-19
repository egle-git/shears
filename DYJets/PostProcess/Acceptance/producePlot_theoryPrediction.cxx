#include "Common/SimplePlotTools.h"
#include "Common/DYTool.h"

class PlotProducer {
public:
  PlotProducer(TString fileName_oldM50, TString fileName_newM50):
  fileName_oldM50_(fileName_oldM50), fileName_newM50_(fileName_newM50) { }

  void Produce() {
    TH1::AddDirectory(kFALSE);

    // plotDirPath_ = "./plot_theory";
    // DYTool::Make_Dir(plotDirPath_);
    plotDirPath_ = DTYool::Set_PlotPath("Acceptance/plot_theory");

    Compare_OldM50_NewM50("h_dsigdm_FPS_PDFVar_000");
    Compare_OldM50_NewM50("h_dsigdm_fid_PDFVar_000");
  }

private:
  TString plotDirPath_ = "";

  TString fileName_oldM50_;
  TString fileName_newM50_;

  void Compare_OldM50_NewM50(TString histName) {
    TH1D* h_oldM50 = PlotTool::Get_Hist(fileName_oldM50_, histName);
    TH1D* h_newM50 = PlotTool::Get_Hist(fileName_newM50_, histName);

    TString canvasName = histName;
    canvasName.ReplaceAll("h_", "c_comp_oldNewM50_");

    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 0, 1);
    canvas->SetTitle("m [GeV]", "d#sigma/dm [pb/GeV]", "new/old");

    canvas->Register(h_oldM50, "Old M50 sample", kBlack);
    canvas->Register(h_newM50, "New M50 sample (ZptReweighted)", kBlue);

    canvas->SetLegendPosition(0.50, 0.70, 0.95, 0.87);

    // canvas->SetRangeY(0, 1.01);
    // canvas->SetRangeX(40, 3000)
    canvas->SetRangeX(50, 100);
    canvas->SetAutoRangeY();
    // canvas->SetRangeY(5e-9, 5e3);
    // canvas->SetRangeRatio(0, 1.01);
    canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, "Dilepton mass distribution at the dressed level");
    if( histName.Contains("fid") )
      canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Fiducial region: p_{T} > 20(15) GeV, |#eta| < 2.4");
    else
      canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Full phase space result");

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();

    // PlotTool::IsRatio1(h_oldM50, h_newM50);


  }
};

void producePlot_theoryPrediction() {
  TString fileName = "TheoryPrediction_VariousPDF_newM50.root";
  TString fileName_oldM50 = fileName;
  fileName_oldM50.ReplaceAll("_newM50.root", ".root");

  PlotProducer producer(fileName_oldM50, fileName);
  producer.Produce();
}