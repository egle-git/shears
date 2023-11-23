#include "Common/SimplePlotTools.h"

class PlotProducer {
public:
  PlotProducer(TString channel, TString fileName): 
  channel_(channel), fileName_(fileName) {}

  void Produce() {
    TH1::AddDirectory(kFALSE);

    vec_uncType_ = {"uncorr_16", "uncorr_17", "uncorr_18", "corr_161718", "corr_1718"};

    ProducePlot_Unc();
    for( const auto& uncType : vec_uncType_ )
      ProducePlot_2D("corrM", uncType);

    ProducePlot_2D("corrM", "tot");
  }

private:
  TString channel_;
  TString fileName_;
  TString plotDirPath_ = "./plot";

  vector<TString> vec_uncType_;

  void ProducePlot_Unc() {
    TString canvasName = "c_unc_tot_"+channel_;
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    // TString titleX = "m(#mu#mu) [GeV]";
    // if( channel_ == "ee" ) titleX.ReplaceAll("#mu", "e");
    TString titleX = "mass bin number";
    canvas->SetTitle(titleX, "Rel. uncertainty");

    vector<Int_t> vec_color = {kBlack, kBlue, kGreen+2, kViolet, kCyan};
    Int_t i_hist = 0;
    for(const auto& uncType : vec_uncType_ ) {
      if( i_hist > 4 )
        throw std::runtime_error("# hists are too large: add more colors in vec_color");

      TH1D* h_relUnc = PlotTool::Get_Hist(fileName_, "h_relUnc_lumi_"+uncType);
      canvas->Register(h_relUnc, uncType, vec_color[i_hist]);
      i_hist++;
    }
    TH1D* h_relUnc_tot = PlotTool::Get_Hist(fileName_, "h_relUnc_lumi_tot");
    canvas->Register(h_relUnc_tot, "Total (quad. sum)", kRed);

    canvas->SetLegendPosition(0.55, 0.70, 0.94, 0.95);

    canvas->SetRangeY(0, 0.04);
    // if( channel_ == "ee" ) canvas->SetRangeY(0, 0.13);
    // if( channel_ == "mm" ) canvas->SetRangeY(0, 0.6);
    // canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from the luminosity";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.6, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");
  }

  void ProducePlot_2D(TString matrixType, TString uncType) {
    TString canvasName = "c2D_"+matrixType+"_"+uncType+"_"+channel_;

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 1, 1, 0);
    TString title = "m(#mu#mu) [GeV]";
    if( channel_ == "ee" ) title.ReplaceAll("#mu", "e");
    canvas->SetTitle(title, title);

    TString histName = "h_"+matrixType+"_lumi_"+uncType;
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


void producePlot_unc_cov_lumi(TString channel) {
  PlotProducer producer(channel, "Unfolded_And_Uncertainty_lumi_"+channel+".root");
  producer.Produce();
}

void producePlot_unc_cov_lumi() {
  producePlot_unc_cov_lumi("ee");
  producePlot_unc_cov_lumi("mm");
}