#include "Common/SimplePlotTools.h"
#include "Common/DYOutput.h"
#include "Common/DYTool.h"
#include "Common/DYPath.h"

class PlotProducer {
public:
  PlotProducer(TString channel): channel_(channel) {

  }

  ~PlotProducer() {
    delete h_unfolded_data_;
    delete h_theory_;

    delete h_dsigdm_data_;
    delete h_dsigdm_theory_;
  }

  void AssignTotalUnc(Bool_t flag = kTRUE) { assignTotalUnc_ = kTRUE; }

  void Produce() {
    Init();

    ProducePlot_Comparison_with_Theory("unfolded");
    ProducePlot_Comparison_with_Theory("dsigdm");
  }

private:
  TString channel_ = "";
  TString shearsPath_ = "";

  TH1D* h_unfolded_data_   = nullptr;
  TH1D* h_theory_ = nullptr;

  TH1D* h_dsigdm_data_;
  TH1D* h_dsigdm_theory_;

  TString fileName_unc_ = "";

  Bool_t assignTotalUnc_ = kFALSE;

  void Init() {
    TH1::AddDirectory(kFALSE);

    // shearsPath_ = DYTool::path_default+"/"+channel_;

    // Run2Output* output = new Run2Output(shearsPath_);

    // // -- central value
    // DYRun2Result* result = new DYRun2Result(output);
    // result->Produce();

    // h_unfolded_data_   = result->Get_AllEra("unfolded", "data");
    // h_theory_          = result->Get_AllEra("gen", "DY");

    fileName_unc_ = "UncAndCov_All_"+channel_+".root";
    cout << "X-sec & Uncertainty is from the file: " << fileName_unc_ << " ... update it first if it is not the latest one!" << endl;

    h_unfolded_data_ = PlotTool::Get_Hist(fileName_unc_, "h_unfolded_data");
    h_theory_        = PlotTool::Get_Hist(fileName_unc_, "h_gen_DY");

    Double_t lumi = LUMI_16pre + LUMI_16post + LUMI_17 + LUMI_18;

    h_dsigdm_data_   = DYTool::Convert_TUnfoldOutput_DSigmaDM(h_unfolded_data_, lumi);
    h_dsigdm_theory_ = DYTool::Convert_TUnfoldOutput_DSigmaDM(h_theory_, lumi);

    if( assignTotalUnc_ ) Assign_TotalUnc(h_dsigdm_data_);
  }

  void Assign_TotalUnc(TH1D* h_data) {
    TH1D* h_relUnc_tot = PlotTool::Get_Hist(fileName_unc_, "h_relUnc_tot");
    h_relUnc_tot = DYTool::Convert_TUnfoldOutput_MassAxis(h_relUnc_tot);
    
    if( h_relUnc_tot->GetNbinsX() != h_data->GetNbinsX() )
      throw std::invalid_argument("[Assign_TotalUnc] # bins are not consistent between dsigma/dm vs. uncertainty plots");

    Int_t nBin = h_data->GetNbinsX();
    for(Int_t i=0; i<nBin; ++i) {
      Int_t i_bin = i+1;

      Double_t relUnc = h_relUnc_tot->GetBinContent(i_bin);
      Double_t dsigdm = h_data->GetBinContent(i_bin);
      Double_t absUnc = dsigdm * relUnc;

      h_data->SetBinError(i_bin, absUnc);
    }
  }

  void ProducePlot_Comparison_with_Theory(TString type) {
    TString canvasName = "c_"+type+"_"+channel_;
    TString titleX = "";
    if( type == "unfolded" ) titleX = "mass bin number";
    if( type == "dsigdm" )   titleX = "m(#mu#mu) [GeV]";
    if( channel_ == "ee" ) titleX.ReplaceAll("#mu", "e");

    TString titleY = "";
    if( type == "unfolded" ) titleY = "# events";
    if( type == "dsigdm" )   titleY = "d#sigma/dm [pb/GeV]";

    Bool_t isLogX = kFALSE;
    if( type == "dsigdm" ) isLogX = kTRUE;

    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, isLogX, 1);
    canvas->SetTitle(titleX, titleY, "theory/data");

    if( type == "unfolded" ) {
      canvas->Register(h_unfolded_data_, "Data", kBlack);
      canvas->Register(h_theory_,        "Theory (Powheg, MiNNLO)", kGreen-8);
    }
    if( type == "dsigdm" ) {
      canvas->Register(h_dsigdm_data_, "Data", kBlack);
      canvas->Register(h_dsigdm_theory_, "Theory (Powheg, MiNNLO)", kGreen-8);
    }

    canvas->SetLegendPosition(0.50, 0.70, 0.94, 0.87);

    if( type == "unfolded" ) canvas->SetRangeY(0.5, 5e9);
    if( type == "dsigdm" ) canvas->SetRangeY(5e-9, 2e3);

    canvas->SetRangeRatio(0.7, 1.3);

    canvas->Latex_CMSInternal();
    Double_t run2Lumi = LUMI_16pre + LUMI_16post + LUMI_17 + LUMI_18;
    canvas->Latex_LumiEnergy(run2Lumi/1000.0, 13);

    TString channelInfo = "";
    if( channel_ == "mm" ) channelInfo = "Muon channel";
    if( channel_ == "ee" ) channelInfo = "Electron channel";

    canvas->RegisterLatex(0.16, 0.91, 42, 0.7, channelInfo);
    if( type == "dsigdm" ) {
      canvas->RegisterLatex(0.18, 0.46, 62, 0.6, "Fiducial phase space (dressed level)");    
      canvas->RegisterLatex(0.18, 0.42, 42, 0.6, "p_{T}^{lead}(l) > 20 GeV, p_{T}^{sub}(l) > 15 GeV");
      canvas->RegisterLatex(0.18, 0.38, 42, 0.6, "|#eta(l)| < 2.4");
    }
    if( type == "unfolded" )
      canvas->RegisterLatex(0.18, 0.87, 42, 0.6, "Unfolded distribution");

    if( assignTotalUnc_ )
      canvas->RegisterLatex(0.18, 0.34, 42, 0.6, "Stat.+Syst. uncertainty");
    else
      canvas->RegisterLatex(0.18, 0.34, 42, 0.6, "Stat. uncertainty only");

    canvas->SetSavePath("./plot");

    canvas->Draw();

    TString baseName = canvas->GetCanvasName();
    canvas->SetRangeRatio(0, 2.5);
    canvas->SetCanvasName( baseName + "_ratioZoomOut" );
    canvas->Draw();
  }
};

void producePlot_xSec() {
  PlotProducer producer_ee("ee");
  producer_ee.AssignTotalUnc();
  producer_ee.Produce();

  PlotProducer producer_mm("mm");
  producer_mm.AssignTotalUnc();
  producer_mm.Produce();
}