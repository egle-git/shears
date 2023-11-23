#include "Common/ShearsComparator.h"
#include "Common/DYUncertainty.h"
#include "Common/DYPath.h"

class UncEstimator_Pileup {
public:
  UncEstimator_Pileup(TString channel, TString systType = "pileup"): 
  channel_(channel), systType_(systType) {}

  void ProduceAndSave() {
    Init();

    ProducePlots_DYRun2Result();
    Estimate_Unc_Cov();
    ProducePlots_Unc_Cov();

    TString fileName_output = "Unfolded_And_Uncertainty_"+systType_+"_"+channel_+".root";
    TFile *f_output = TFile::Open(fileName_output, "RECREATE");
    result_cv_->Save(f_output, "cv");
    result_plus_->Save(f_output, "plus");
    result_minus_->Save(f_output, "minus");
    unc_->Save(f_output);
  }

protected:
  TString channel_ = "";
  TString systType_ = "";

  TString plotDirPath_ = "";

  Run2Output* output_;
  DYRun2Result* result_cv_;
  DYRun2Result* result_plus_;
  DYRun2Result* result_minus_;

  Uncertainty* unc_;

  void Estimate_Unc_Cov() {
    TH1D* h_cv = result_cv_->Get_AllEra("unfolded", "data");
    TH1D* h_plus  = result_plus_->Get_AllEra("unfolded", "data");
    TH1D* h_minus = result_minus_->Get_AllEra("unfolded", "data");
    vector<TH1D*> vec_altHist = {h_plus, h_minus};

    unc_ = new Uncertainty(systType_, "oneSigmaShift", "fullyCorr");

    unc_->Set_CentralHist( h_cv );
    unc_->Set_AltHist( vec_altHist );
    unc_->Estimate();
  }

  void ProducePlots_DYRun2Result() {
    ResultComparator comparator(channel_);
    comparator.Set_Case(result_cv_,    "Central value");
    comparator.Set_Case(result_plus_,  "+1#sigma ("+systType_+")");
    comparator.Set_Case(result_minus_, "-1#sigma ("+systType_+")");
    comparator.Compare("DYRun2Result/"+channel_);
  }

  void Init() {
    plotDirPath_ = "./plot";

    TString commonPath;
    if( systType_ == "pileup" ) commonPath = DYTool::path_systVar_pileup;
    if( systType_ == "L1Pref" ) commonPath = DYTool::path_systVar_L1Pref;
    cout << "*** Using the output in " << commonPath << " ***" << endl;

    output_ = new Run2Output(commonPath+"/"+channel_);

    result_cv_    = new DYRun2Result(output_);
    result_plus_  = new DYRun2Result(output_);
    result_minus_ = new DYRun2Result(output_);

    TString tag_plus = TString::Format("%s_plus", systType_.Data());
    result_plus_->Update_HistName("all", "gen_DY",     "TUnfold1DTrue_inc0jet_"+tag_plus);
    result_plus_->Update_HistName("all", "reco_DY",    "TUnfold1DReco_inc0jet_"+tag_plus);
    result_plus_->Update_HistName("all", "reco_bkgMC", "TUnfold1DReco_inc0jet_"+tag_plus);
    result_plus_->Update_HistName("all", "migM",       "TUnfold2DMig_inc0jet_"+tag_plus);

    TString tag_minus = tag_plus;
    tag_minus.ReplaceAll("_plus", "_minus");
    result_minus_->Update_HistName("all", "gen_DY",     "TUnfold1DTrue_inc0jet_"+tag_minus);
    result_minus_->Update_HistName("all", "reco_DY",    "TUnfold1DReco_inc0jet_"+tag_minus);
    result_minus_->Update_HistName("all", "reco_bkgMC", "TUnfold1DReco_inc0jet_"+tag_minus);
    result_minus_->Update_HistName("all", "migM",       "TUnfold2DMig_inc0jet_"+tag_minus);

    result_cv_->Produce();
    result_plus_->Produce();
    result_minus_->Produce();
  }

  void ProducePlots_Unc_Cov() {
    ProducePlot_Unc();
    ProducePlot_2D("covM");
    ProducePlot_2D("corrM");
  }

  void ProducePlot_Unc() {
    TString canvasName = "c_unc_"+channel_;
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    TH1D* h_relUnc = unc_->RelUnc();
    canvas->Register(h_relUnc, "Unc. ("+systType_+")", kBlack);

    canvas->SetLegendPosition(0.50, 0.85, 0.94, 0.94);

    if( systType_ == "pileup" ) canvas->SetRangeY(0, 0.03);
    else                        canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString channelInfo = channel_ == "ee" ? "Electron channel" : "Muon channel";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");
  }

  void ProducePlot_2D(TString matrixType) {
    TString canvasName = "c2D_"+matrixType+"_"+channel_;

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 0, 0, 0);
    canvas->SetTitle("mass bin number", "mass bin number");

    TString histName = TString::Format("h_%s_effSF_total", matrixType.Data());
    TH2D* h2D = nullptr;
    if( matrixType == "covM" )  h2D = unc_->CovM();
    if( matrixType == "corrM" ) h2D = unc_->CorrM();
    canvas->Register(h2D);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeZ(minZ, maxZ);
    if( matrixType == "corrM" ) canvas->SetRangeZ(-1.01, 1.01);
    else                        canvas->SetAutoRangeZ();

    canvas->Latex_CMSInternal();
    TString info = "Covariance matrix ("+systType_+", "+channel_+")";
    if( matrixType == "corrM" ) info.ReplaceAll("Covariance", "Correlation");
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, info);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }

};

void estimate_unc_cov_pileup() {
  UncEstimator_Pileup estimator_ee("ee");
  estimator_ee.ProduceAndSave();

  UncEstimator_Pileup estimator_mm("mm");
  estimator_mm.ProduceAndSave();
}