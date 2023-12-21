#include "Common/DYTool.h"
#include "Common/DYPath.h"
#include "Common/DYOutput.h"
#include "Common/ShearsComparator.h"
#include "Common/DYUncertainty.h"

void SaveUncAndCov_withoutNormByL(Uncertainty *unc, TString tag, Double_t lumi) {
  TH1D* h_absUnc_normByL = unc->AbsUnc();
  TH1D* h_relUnc_normByL = unc->RelUnc();
  TH2D* h_covM_normByL   = unc->CovM();
  TH2D* h_corrM_normByL  = unc->CorrM();

  TH1D* h_absUnc = (TH1D*)h_absUnc_normByL->Clone();
  h_absUnc->Scale( lumi ); // -- multiply by lumi (to cancel the norm. by L)

  // -- rel.unc: nothing to do
  TH1D* h_relUnc = (TH1D*)h_relUnc_normByL->Clone();

  TH2D* h_covM = (TH2D*)h_covM_normByL->Clone();
  h_covM->Scale( lumi*lumi ); // -- multiply by lumi^2 (to cancel the norm. by L)

  // -- corr. M: nothing to do
  TH2D* h_corrM = (TH2D*)h_corrM_normByL->Clone();

  h_absUnc->SetName("h_absUnc_lumi_"+tag);
  h_relUnc->SetName("h_relUnc_lumi_"+tag);
  h_covM->SetName("h_covM_lumi_"+tag);
  h_corrM->SetName("h_corrM_lumi_"+tag);

  h_absUnc->Write();
  h_relUnc->Write();
  h_covM->Write();
  h_corrM->Write();
}

class SubUncEstimator_Lumi {
public:
  SubUncEstimator_Lumi(TString channel, TString tag, const std::map<TString, Double_t>& map_relUnc) :
  channel_(channel), tag_(tag), map_relUnc_(map_relUnc) { }

  void ProduceAndSave(TString shearsPath, DYRun2Result* result_cv, TFile *f_output) {
    TH1::AddDirectory(kFALSE);

    cout << "[SubUncEstimator_Lumi::ProduceAndSave] tag = " << tag_ << endl;
    cout << "Input rel. uncertainty on luminosity: " << endl;
    for(const auto& pair : map_relUnc_ )
      cout << "  era = " << pair.first << " --> relUnc. = " << pair.second << endl;
    cout << endl;

    Init(shearsPath);
    // -- convert to dsigma/dm
    // TH1D* h_axis = output_plus_->Get_EraOutput("16pre").Get("mass_wide_range_inc0jet", "data"); // -- just to get the mass axis
    // h_dSigdM_cv_ = Calc_dSigdM(h_axis, result_cv);
    // h_dSigdM_plus_  = Calc_dSigdM(h_axis, result_plus_);
    // h_dSigdM_minus_ = Calc_dSigdM(h_axis, result_minus_);

    h_normByL_cv_    = Calc_nEvent_NormByLumi(result_cv);
    h_normByL_plus_  = Calc_nEvent_NormByLumi(result_plus_);
    h_normByL_minus_ = Calc_nEvent_NormByLumi(result_minus_);

    ProducePlots_DYRun2Result(result_cv);
    // ProducePlot_dSigdM();
    ProducePlot_nEvent_NormByLumi();

    Estimate_Unc_Cov();

    // -- save
    f_output->cd();
    result_plus_->Save(f_output, "lumi_"+tag_+"_plus");
    h_normByL_plus_->SetName("h_unfolded_data_normbyL_lumi_"+tag_+"_plus");
    h_normByL_plus_->Write();

    result_minus_->Save(f_output, "lumi_"+tag_+"_minus");
    h_normByL_minus_->SetName("h_unfolded_data_normbyL_lumi_"+tag_+"_minus");
    h_normByL_minus_->Write();

    unc_->Save(f_output);

    // -- to keep the same format with the other uncertainties
    // -- the absolute uncertainty & covariance should be the numbers w.r.t. unfolded events (not event/lumi)
    SaveUncAndCov_withoutNormByL( unc_, tag_, result_cv->Run2Lumi() );
  }

  Uncertainty Unc() const { return *unc_; }

private:
  TString channel_;
  TString tag_;
  const std::map<TString, Double_t>& map_relUnc_;

  // -- output should be different between two cases:
  // -- as each output has different lumi.
  Run2Output* output_plus_;
  Run2Output* output_minus_;
  DYRun2Result* result_plus_;
  DYRun2Result* result_minus_;
  // TH1D* h_dSigdM_cv_;
  // TH1D* h_dSigdM_plus_;
  // TH1D* h_dSigdM_minus_;

  // -- # events divided by luminosity (to calc. uncertainty properly)
  TH1D* h_normByL_cv_;
  TH1D* h_normByL_plus_;
  TH1D* h_normByL_minus_;

  Uncertainty *unc_;

  void Init(TString shearsPath) {
    output_plus_  = new Run2Output(shearsPath);
    output_minus_ = new Run2Output(shearsPath);

    result_plus_  = new DYRun2Result(output_plus_);
    result_minus_ = new DYRun2Result(output_minus_);

    Update_Lumi( result_plus_,  "plus" );
    Update_Lumi( result_minus_, "minus" );

    result_plus_->Produce();
    result_minus_->Produce();
  }

  void ProducePlots_DYRun2Result(DYRun2Result* result_cv) {
    ResultComparator comparator(channel_);
    comparator.Remove_RatioError();
    comparator.Set_Case(result_cv,     "Central value");
    comparator.Set_Case(result_plus_,  "+1#sigma ("+tag_+")");
    comparator.Set_Case(result_minus_, "-1#sigma ("+tag_+")");
    comparator.Compare("DYRun2Result/"+channel_+"/"+tag_);
  }

  // void ProducePlot_dSigdM() {
  //   TString canvasName = "c07_dSigma_dm";
  //   PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 1, 1);
  //   canvas->SetTitle("m [GeV]", "d#sigma/dm [pb/GeV]", "ratio to black");

  //   canvas->Register(h_dSigdM_cv_,    "Central value",       kBlack);
  //   canvas->Register(h_dSigdM_plus_,  "+1#sigma ("+tag_+")", kBlue);
  //   canvas->Register(h_dSigdM_minus_, "-1#sigma ("+tag_+")", kGreen+2);

  //   canvas->SetLegendPosition(0.70, 0.70, 0.95, 0.95);

  //   // canvas->SetAutoRangeY();
  //   canvas->SetRangeY(2e-8, 2e2);
  //   canvas->SetAutoRangeRatio();

  //   canvas->Latex_CMSInternal();
  //   TString channelInfo  = (channel_ == "mm" ) ? "Muon channel" : "Electron channel";
  //   canvas->RegisterLatex(0.16, 0.91, 42, 0.6, channelInfo);
  //   canvas->RegisterLatex(0.16, 0.87, 42, 0.6, "Fiducial d#sigma/dm");
  //   TString fidInfo = "(p_{T}^{lead}(#mu) > 20 GeV, (p_{T}^{sub}(#mu) > 15 GeV, |#eta(#mu)| < 2.4)";
  //   if( channel_ == "ee" ) fidInfo.ReplaceAll("#mu", "e");
  //   canvas->RegisterLatex(0.16, 0.83, 42, 0.5, fidInfo);
  //   // canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Gaussian distributions}}"); // same with above

  //   canvas->RemoveRatioError(); // -- remove error in the ratio (useful when the error is meaningless)

  //   TString plotPath = "./plot/DYRun2Result/"+channel_+"/"+tag_;
  //   canvas->SetSavePath(plotPath);

  //   canvas->Draw();
  // }

  void ProducePlot_nEvent_NormByLumi() {
    TString canvasName = "c07_nEvent_NormByLumi";
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 0, 1);
    canvas->SetTitle("mass bin number", "# events / lumi", "ratio to black");

    canvas->Register(h_normByL_cv_,    "Central value",       kBlack);
    canvas->Register(h_normByL_plus_,  "+1#sigma ("+tag_+")", kBlue);
    canvas->Register(h_normByL_minus_, "-1#sigma ("+tag_+")", kGreen+2);

    canvas->SetLegendPosition(0.70, 0.70, 0.95, 0.95);

    // canvas->SetAutoRangeY();
    canvas->SetRangeY(5e-7, 5e3);
    // canvas->SetRangeY(2e-8, 2e2);
    canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    TString channelInfo  = (channel_ == "mm" ) ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, channelInfo);
    canvas->RegisterLatex(0.16, 0.87, 42, 0.6, "Unfolded distribution normalized by lumi.");
    // TString fidInfo = "(p_{T}^{lead}(#mu) > 20 GeV, (p_{T}^{sub}(#mu) > 15 GeV, |#eta(#mu)| < 2.4)";
    // if( channel_ == "ee" ) fidInfo.ReplaceAll("#mu", "e");
    // canvas->RegisterLatex(0.16, 0.83, 42, 0.5, fidInfo);
    // canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Gaussian distributions}}"); // same with above

    canvas->RemoveRatioError(); // -- remove error in the ratio (useful when the error is meaningless)

    TString plotPath = "./plot/DYRun2Result/"+channel_+"/"+tag_;
    canvas->SetSavePath(plotPath);

    canvas->Draw();
  }

  void Update_Lumi( DYRun2Result* result, TString shift ) {
    vector<TString> vec_era = {"16pre", "16post", "17", "18"};
    for(const auto& era : vec_era ) {
      Double_t relUnc_lumi = Get_RelUnc_Lumi(era);
      if( relUnc_lumi == 0 ) continue; // -- does not change the default lumi in this case

      Double_t defaultLuml = Get_DefaultLumi(era);
      Double_t shiftedLumi = ShiftedLumi(defaultLuml, relUnc_lumi, shift);

      result->Update_Lumi(era, shiftedLumi);
    }
  }

  Double_t ShiftedLumi(Double_t defaultLumi, Double_t relUnc, TString shift) {
    Double_t shiftedLumi;
    if( shift == "plus" )
      shiftedLumi = defaultLumi + defaultLumi*relUnc; 
    else if( shift == "minus" )
      shiftedLumi = defaultLumi - defaultLumi*relUnc; 
    else
      throw std::invalid_argument("[SubUncEstimator_Lumi::ShiftedLumi] shift = " + shift + " is not supported");

    return shiftedLumi;
  }

  Double_t Get_RelUnc_Lumi(TString era) {
    // -- const map: can't use [] operator
    // if( era == "16pre" || era == "16post" )
    //   return map_relUnc_.find("16")->second;

    // return map_relUnc_.find(era)->second;

    auto iter = (era == "16pre" || era == "16post") ? 
                map_relUnc_.find("16") : map_relUnc_.find(era);

    if( iter == map_relUnc_.end() )
      throw std::invalid_argument("[SubUncEstimator_Lumi::Get_RelUnc_Lumi] era = " + era + " is not supported");

    return iter->second;
  }

  Double_t Get_DefaultLumi(TString era) {
    if( era == "16pre" ) return LUMI_16pre;
    if( era == "16post" ) return LUMI_16post;
    if( era == "17" ) return LUMI_17;
    if( era == "18" ) return LUMI_18;

    throw std::invalid_argument("[SubUncEstimator_Lumi::Get_DefaultLumi] era = " + era + " is not supported");

    return 0;
  }

  // TH1D* Calc_dSigdM(TH1D* h_axis, DYRun2Result* result) {
  //   TH1D* h_unfolded = result->Get_AllEra("unfolded", "data");
  //   Double_t lumi = result->Run2Lumi();
  //   return DYTool::Convert_TUnfoldOutput_DSigmaDM(h_axis, h_unfolded, lumi);
  // }

  TH1D* Calc_nEvent_NormByLumi(DYRun2Result* result) {
    TH1D* h_return = result->Get_AllEra("unfolded", "data");
    Double_t lumi = result->Run2Lumi();
    h_return->Scale( 1.0 / lumi );

    return h_return;
  }

  void Estimate_Unc_Cov() {
    unc_ = new Uncertainty("lumi_"+tag_+"_normByL", "oneSigmaShift", "fullyCorr");
    unc_->Set_CentralHist( h_normByL_cv_ );
    unc_->Set_AltHist( {h_normByL_plus_, h_normByL_minus_} );
    unc_->Estimate();
  }
};

class UncEstimator_Lumi {
public:
  UncEstimator_Lumi(TString channel): channel_(channel) { }

  void ProduceAndSave() {
    TH1::AddDirectory(kFALSE);

    fileName_output_ = "Unfolded_And_Uncertainty_lumi_"+channel_+".root";
    TFile *f_output = TFile::Open(fileName_output_, "RECREATE");

    shearsPath_ = DYTool::path_default+"/"+channel_;
    Run2Output* output = new Run2Output(shearsPath_);
    DYRun2Result* result_cv = new DYRun2Result(output);
    result_cv->Produce();

    f_output->cd();
    result_cv->Save(f_output, "cv");
    // -- also save the distribution norm. by L
    TH1D* h_normByL = result_cv->Get_AllEra("unfolded", "data");
    Double_t lumi = result_cv->Run2Lumi();
    h_normByL->Scale( 1.0 / lumi );
    h_normByL->SetName("h_unfolded_data_normbyL_cv");
    h_normByL->Write();

    std::map<TString, std::map<TString, Double_t>> map_case = {
      {"uncorr_16",   map_relUnc_uncorr_16_},
      {"uncorr_17",   map_relUnc_uncorr_17_},
      {"uncorr_18",   map_relUnc_uncorr_18_},
      {"corr_161718", map_relUnc_corr_161718_},
      {"corr_1718",   map_relUnc_corr_1718_}
    };

    // vector<SubUncEstimator_Lumi> vec_estimator;
    vector<Uncertainty> vec_unc;
    for( const auto& pair : map_case ) {
      SubUncEstimator_Lumi estimator(channel_, pair.first, pair.second);
      estimator.ProduceAndSave(shearsPath_, result_cv, f_output);
      // vec_estimator.push_back( estimator );
      vec_unc.push_back( estimator.Unc() );
    }

    f_output->cd();
    Uncertainty unc_tot("lumi_tot_normByL");
    unc_tot.Combine(vec_unc);
    unc_tot.Save(f_output);

    // -- to keep the same format with the other uncertainties
    // -- the absolute uncertainty & covariance should be the numbers w.r.t. unfolded events (not event/lumi)
    SaveUncAndCov_withoutNormByL( &unc_tot, "tot", result_cv->Run2Lumi() );

    f_output->Close();

    Print_Summary();
  }

private:
  TString channel_ = "";
  TString shearsPath_ = "";
  TString fileName_output_;

  // -- https://twiki.cern.ch/twiki/bin/view/CMS/LumiRecommendationsRun2#Combination_and_correlations
  std::map<TString, Double_t> map_relUnc_uncorr_16_ = {
    {"16", 0.01},
    {"17", 0.0},
    {"18", 0.0}
  };

  std::map<TString, Double_t> map_relUnc_uncorr_17_ = {
    {"16", 0.0},
    {"17", 0.02},
    {"18", 0.0}
  };

  std::map<TString, Double_t> map_relUnc_uncorr_18_ = {
    {"16", 0.0},
    {"17", 0.0},
    {"18", 0.015}
  };

  std::map<TString, Double_t> map_relUnc_corr_161718_ = {
    {"16", 0.006},
    {"17", 0.009},
    {"18", 0.02}
  };

  std::map<TString, Double_t> map_relUnc_corr_1718_ = {
    {"16", 0.0},
    {"17", 0.006},
    {"18", 0.002}
  };

  void Print_Summary() {
    cout << "============ [summary] ============" << endl;
    cout << "[input]" << endl;
    cout << "  shears result: " << shearsPath_ << endl;
    cout << "[output]" << endl;
    cout << "  All unfolded results with variations, " << endl;
    cout << "  uncertainties and covariance matrices: " << fileName_output_ << endl;
    cout << "===================================" << endl;
  }

};

void estimate_unc_cov_lumi() {
  UncEstimator_Lumi estimator_ee("ee");
  estimator_ee.ProduceAndSave();

  UncEstimator_Lumi estimator_mm("mm");
  estimator_mm.ProduceAndSave();
}