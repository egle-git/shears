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

  void FullPhaseSpace(Bool_t flag = kTRUE) { isFPS_ = flag; }

  void AssignTotalUnc(Bool_t flag = kTRUE) { assignTotalUnc_ = kTRUE; }

  void Produce() {
    Init();

    if( !isFPS_ ) ProducePlot_Comparison_with_Theory("unfolded");
    ProducePlot_Comparison_with_Theory("dsigdm");
  }

private:
  TString channel_ = "";
  TString shearsPath_ = "";
  TString plotDirPath_ = "";

  TH1D* h_unfolded_data_   = nullptr;
  TH1D* h_theory_ = nullptr;

  TH1D* h_dsigdm_data_;
  TH1D* h_dsigdm_theory_;

  TString fileName_unc_ = "";

  Bool_t assignTotalUnc_ = kFALSE;

  Bool_t isFPS_ = kFALSE; // -- is full phase space result?

  void Init() {
    TH1::AddDirectory(kFALSE);
    plotDirPath_ = DYTool::Set_PlotPath("Summary");

    fileName_unc_ = "UncAndCov_All_"+channel_+".root";
    if( isFPS_ )
      fileName_unc_.ReplaceAll(".root", "_FPS.root");

    fileName_unc_ = DYTool::Get_OutputPath(fileName_unc_);

    cout << "X-sec & Uncertainty is from the file: " << fileName_unc_ << " ... update it first if it is not the latest one!" << endl;

    h_unfolded_data_ = (isFPS_) ? PlotTool::Get_Hist(fileName_unc_, "h_allEra_unfoldedFPS_data") :
                                  PlotTool::Get_Hist(fileName_unc_, "h_allEra_unfolded_data");
    
    h_theory_        = (isFPS_) ? PlotTool::Get_Hist(DYTool::path_theoryPred, "h_nEvent_ufof_FPS_PDFVar_000") :
                                  PlotTool::Get_Hist(DYTool::path_theoryPred, "h_nEvent_ufof_fid_PDFVar_000");
    h_theory_ = Convert_Theory_to_MassBinNumberAxis(h_theory_, h_unfolded_data_);

    Double_t lumi = DYTool::GetLumi("all");

    h_dsigdm_data_   = PlotTool::Get_Hist(fileName_unc_, "h_dsigdm_data");
    h_dsigdm_theory_ = DYTool::Convert_TUnfoldOutput_DSigmaDM(h_theory_, lumi);

    if( assignTotalUnc_ ) Assign_TotalUnc(h_dsigdm_data_);
  }

  void Assign_TotalUnc(TH1D* h_data) {
    TH1D* h_relUnc_tot = PlotTool::Get_Hist(fileName_unc_, "h_relUnc_tot_mAxis");
    
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
    if( isFPS_ ) canvasName.ReplaceAll("c_", "c_FPS_");

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
      canvas->Register(h_theory_,        "Theory (Powheg, MiNNLO)", kGreen+2);
    }
    if( type == "dsigdm" ) {
      canvas->Register(h_dsigdm_data_, "Data", kBlack);
      canvas->Register(h_dsigdm_theory_, "Theory (Powheg, MiNNLO)", kGreen+2);
    }

    canvas->SetLegendPosition(0.50, 0.70, 0.94, 0.87);

    if( type == "unfolded" ) canvas->SetRangeY(0.5, 5e9);
    if( type == "dsigdm" )   canvas->SetRangeY(5e-9, 2e3);

    canvas->SetRangeRatio(0.7, 1.3);

    canvas->Latex_CMSInternal();
    Double_t run2Lumi = LUMI_16pre + LUMI_16post + LUMI_17 + LUMI_18;
    canvas->Latex_LumiEnergy(run2Lumi/1000.0, 13);

    TString channelInfo = "";
    if( channel_ == "mm" ) channelInfo = "Muon channel";
    if( channel_ == "ee" ) channelInfo = "Electron channel";

    canvas->RegisterLatex(0.16, 0.91, 42, 0.7, channelInfo);
    if( type == "dsigdm" ) {
      if( isFPS_ ) {
        canvas->RegisterLatex(0.18, 0.38, 62, 0.6, "Full phase space (dressed level)");
        Put_UncInfo(canvas, 0.34);
      }
      else {
        canvas->RegisterLatex(0.18, 0.46, 62, 0.6, "Fiducial phase space (dressed level)");    
        canvas->RegisterLatex(0.18, 0.42, 42, 0.6, "p_{T}^{lead}(l) > 20 GeV, p_{T}^{sub}(l) > 15 GeV");
        canvas->RegisterLatex(0.18, 0.38, 42, 0.6, "|#eta(l)| < 2.4");
        Put_UncInfo(canvas, 0.34);
      }
    }
    if( type == "unfolded" ) {
      canvas->RegisterLatex(0.16, 0.87, 42, 0.6, "Unfolded distribution");
      Put_UncInfo(canvas, 0.34);
    }

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();

    TString baseName = canvas->GetCanvasName();
    canvas->SetRangeRatio(0, 2.5);
    canvas->SetCanvasName( baseName + "_ratioZoomOut" );
    canvas->Draw();
  }

  void Put_UncInfo(PlotTool::HistCanvaswRatio* canvas, Double_t y) {
    if( assignTotalUnc_ )
      canvas->RegisterLatex(0.18, y, 42, 0.6, "Stat.+Syst. uncertainty");
    else
      canvas->RegisterLatex(0.18, y, 42, 0.6, "Stat. uncertainty only");
  }

  TH1D* Convert_Theory_to_MassBinNumberAxis(TH1D* h_theory, TH1D* h_binNumAxis) {
    Int_t nBin_theory = h_theory->GetNbinsX();
    Int_t nBin_binNumAxis = h_binNumAxis->GetNbinsX();
    if( nBin_theory != nBin_binNumAxis ) {
      printf("[Convert_Theory_to_MassBinNumberAxis] (nBin_theory, nBin_binNumAxis) = (%d, %d)\n", nBin_theory, nBin_binNumAxis);
      throw std::runtime_error("[Convert_Theory_to_MassBinNumberAxis] Inconsistent bin numbers");
    }

    TH1D* h_return = (TH1D*)h_binNumAxis->Clone();
    for(Int_t i=0; i<nBin_theory; ++i) {
      Int_t i_bin = i+1;

      Double_t value = h_theory->GetBinContent(i_bin);
      Double_t error = h_theory->GetBinError(i_bin);

      h_return->SetBinContent(i_bin, value);
      h_return->SetBinError(i_bin, error);
    }

    return h_return;
  }
};

void producePlot_xSec_FPS() {
  PlotProducer producer_ee("ee");
  producer_ee.FullPhaseSpace();
  producer_ee.AssignTotalUnc();
  producer_ee.Produce();

  PlotProducer producer_mm("mm");
  producer_mm.FullPhaseSpace();
  producer_mm.AssignTotalUnc();
  producer_mm.Produce();
}

void producePlot_xSec() {
  PlotProducer producer_ee("ee");
  producer_ee.AssignTotalUnc();
  producer_ee.Produce();

  PlotProducer producer_mm("mm");
  producer_mm.AssignTotalUnc();
  producer_mm.Produce();

  producePlot_xSec_FPS();
}