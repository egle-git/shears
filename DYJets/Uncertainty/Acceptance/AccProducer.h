#include "DYAccTool.h"
#include "Common/DYUncertainty.h"

#include "TEfficiency.h"

class AccInfo {
public:
  AccInfo(TString inputPath, TString tag): 
  inputPath_(inputPath), tag_(tag) { }

  void Produce() {
    Init();
  }

  TH1D* Acc() const { return (TH1D*)h_acc_->Clone(); }
  TH1D* DEN() const { return (TH1D*)h_DEN_->Clone(); }
  TH1D* NUM() const { return (TH1D*)h_NUM_->Clone(); }
  TString Tag() const { return tag_; }

private:
  TString inputPath_ = "";
  TString tag_ = "";

  TH1D* h_DEN_; // -- denominator (full phase space)
  TH1D* h_NUM_; // -- numerator (fiducial phase space)

  TH1D* h_acc_;

  void Init() {
    h_DEN_ = Get_MergedHist(kFALSE);
    h_NUM_ = Get_MergedHist(kTRUE);

    ConsistencyCheck(h_NUM_, h_DEN_);

    TEfficiency *tEff_acc = new TEfficiency(*h_NUM_, *h_DEN_);
    TGraphAsymmErrors* g_acc = (TGraphAsymmErrors*)tEff_acc->CreateGraph()->Clone();
    h_acc_ = PlotTool::Convert_GraphToHist(g_acc);
    printf("\n");

    delete g_acc;
    delete tEff_acc;
  }

  TH1D* Get_MergedHist(Bool_t isFiducial) {
    TH1D* h_ee = DYAcc::Get_MergedHist_Channel(inputPath_, "ee", isFiducial, tag_);
    TH1D* h_mm = DYAcc::Get_MergedHist_Channel(inputPath_, "mm", isFiducial, tag_);

    TH1D* h_merged = (TH1D*)h_ee->Clone();
    h_merged->Add( h_mm );

    h_merged->Scale(1 / 2.0); // -- average

    return h_merged;

    // return h_mm; // -- use mm result only for now
    // return h_ee; // -- test to use ee only
  }

  void ConsistencyCheck(TH1D* h_NUM, TH1D* h_DEN) {
    Int_t nBin_NUM = h_NUM->GetNbinsX();
    Int_t nBin_DEN = h_DEN->GetNbinsX();

    // printf("[# bins] (DEN, NUM) = (%d, %d) \n", nBin_DEN, nBin_NUM);
    if( nBin_DEN != nBin_NUM ) {
      printf("[# bins] (DEN, NUM) = (%d, %d) ... not consistent each other!\n", nBin_DEN, nBin_NUM);
      throw std::runtime_error("[AccInfo::ConsistencyCheck] Inconsistent bin numbers beween DEN and NUM");
    }

    printf("Consistency check between DEN and NUM (tag = %s) ... \n", tag_.Data());
    for(Int_t i=-1; i<nBin_NUM+1; ++i) { // -- include under/overflow
      Int_t i_bin = i+1;

      Double_t value_DEN = h_DEN->GetBinContent(i_bin);
      Double_t value_NUM = h_NUM->GetBinContent(i_bin);

      // if( tag_ == "PDFVar_30" )
      //   printf("[%02dth bin] (NUM, DEN) = (%.1lf, %.1lf)\n", i_bin, value_NUM, value_DEN);

      if( value_NUM > value_DEN ) {
        printf("[%02dth bin] # events in NUM > DEN (%.1lf > %.1lf) ... force to NUM=DEN\n", i_bin, value_NUM, value_DEN);

        h_NUM->SetBinContent(i_bin, value_DEN);
        Double_t error_DEN = h_DEN->GetBinError(i_bin);
        h_NUM->SetBinError(i_bin, error_DEN);
      }
    }
    printf("Check is done\n");
  }
};

class AccProducer {
public:
  AccProducer(TString inputPath): inputPath_(inputPath) {}

  void Set_OutputFileName(TString name) { fileName_output_ = name; }

  void ProduceAndSave() {
    TH1::AddDirectory(kFALSE);
    Init();

    for(auto& pair : map_accInfo_ ) {
      // cout << "tag = " << pair.first << ", tag for AccInfo = " << pair.second->Tag() << endl;
      pair.second->Produce();
    }

    ProducePlot_Acc();

    EstimateUnc();
    ProducePlot_Unc();

    Save();
  }

private:
  TString inputPath_ = "";  
  std::map<TString, AccInfo*> map_accInfo_;
  TString fileName_output_ = "";

  TH1D* h_relUnc_stat_;
  TH1D* h_relUnc_PDFHessian_;
  TH1D* h_relUnc_alphaS_;
  TH1D* h_relUnc_scale_;
  TH1D* h_relUnc_tot_;

  TString plotDirPath_ = "";

  void Init() {
    plotDirPath_ = "./plot";
    DYTool::Make_Dir(plotDirPath_);

    map_accInfo_.insert( std::make_pair( "cv", new AccInfo(inputPath_, "cv")) );

    vector<TString> vec_PDFVar;
    for(Int_t i=0; i<=102; ++i) {
      // TString tag         = TString::Format("PDFVar_%02d", i);
      // TString tag_3digits = TString::Format("PDFVar_%03d", i); // -- to have consistent naming with the shears code
      // map_accInfo_.insert( std::make_pair( tag_3digits, new AccInfo(inputPath_, tag)) );

      TString tag_3digits = TString::Format("PDFVar_%03d", i); // -- to have consistent naming with the shears code
      map_accInfo_.insert( std::make_pair( tag_3digits, new AccInfo(inputPath_, tag_3digits)) );
    }

    vector<TString> vec_scaleVar;
    for(Int_t i=1; i<=9; ++i) {
      // TString tag         = TString::Format("scaleVar_%02d", i);
      // TString tag_3digits = TString::Format("scaleVar_%03d", i); // -- to have consistent naming with the shears code
      // map_accInfo_.insert( std::make_pair( tag_3digits, new AccInfo(inputPath_, tag)) );

      TString tag_3digits = TString::Format("scaleVar_%03d", i); // -- to have consistent naming with the shears code
      map_accInfo_.insert( std::make_pair( tag_3digits, new AccInfo(inputPath_, tag_3digits)) );
    }
  }

  void EstimateUnc() {
    TH1D* h_acc_cv = map_accInfo_["cv"]->Acc();

    h_relUnc_stat_ = Extract_RelUnc(h_acc_cv);

    // -- PDF hessian (PDFVar_001 to 100): quadrature sum
    vector<TH1D*> vec_relUnc_PDFHessian;
    for(Int_t i=1; i<=100; ++i) {
      TString tag = TString::Format("PDFVar_%03d", i);

      TH1D* h_acc_systVar = map_accInfo_[tag]->Acc();

      Uncertainty unc(tag, "alternative", "fullyCorr");
      unc.Set_CentralHist(h_acc_cv);
      unc.Set_AltHist( {h_acc_systVar} );
      unc.Estimate();
      vec_relUnc_PDFHessian.push_back( unc.RelUnc() );
    }
    h_relUnc_PDFHessian_ = PlotTool::QuadSum_Hist(vec_relUnc_PDFHessian); 

    // -- alphaS: take the maximum deviation
    TString tag_alphaS_up   = "PDFVar_101";
    TString tag_alphaS_down = "PDFVar_102";
    TH1D* h_acc_systVar_alphaS_up   = map_accInfo_[tag_alphaS_up]->Acc();
    TH1D* h_acc_systVar_alphaS_down = map_accInfo_[tag_alphaS_down]->Acc();
    Uncertainty unc_alphaS("alphaS", "alternative", "fullyCorr");
    unc_alphaS.Set_CentralHist(h_acc_cv);
    unc_alphaS.Set_AltHist( {h_acc_systVar_alphaS_up, h_acc_systVar_alphaS_down} );
    unc_alphaS.Estimate();
    
    h_relUnc_alphaS_ = unc_alphaS.RelUnc();

    // -- scale variation: maximum deviation w.r.t. selected variations (2, 3, 4, 5, 7, 9)

    vector<TH1D*> vec_acc_scaleVar;
    for(Int_t i=1; i<=9; ++i) {
      if( i == 1 ) continue; // -- (mu_R, mu_F): (1.0, 1.0)
      if( i == 6 ) continue; // -- (mu_R, mu_F): (2.0, 0.5)
      if( i == 8 ) continue; // -- (mu_R, mu_F): (0.5, 2.0)

      TString tag = TString::Format("scaleVar_%03d", i);
      // TString tag = SynchronizedTag_withNanoAOD(i);

      TH1D* h_acc_systVar = map_accInfo_[tag]->Acc();
      vec_acc_scaleVar.push_back( h_acc_systVar );
    }

    Uncertainty unc_scaleVar("scaleVar", "alternative", "fullyCorr");
    unc_scaleVar.Set_CentralHist(h_acc_cv);
    unc_scaleVar.Set_AltHist( vec_acc_scaleVar );
    unc_scaleVar.Estimate();
    h_relUnc_scale_ = unc_scaleVar.RelUnc();

    vector<TH1D*> vec_relUnc = { h_relUnc_stat_, h_relUnc_PDFHessian_, h_relUnc_alphaS_, h_relUnc_scale_};
    h_relUnc_tot_ = PlotTool::QuadSum_Hist(vec_relUnc);
  }

  TH1D* Extract_RelUnc(TH1D* h) {
    TH1D* h_relUnc = (TH1D*)h->Clone();

    for(Int_t i=0; i<h->GetNbinsX(); ++i) {
      Int_t i_bin = i+1;

      Double_t value = h->GetBinContent(i_bin);
      Double_t error = h->GetBinError(i_bin);

      Double_t relUnc;
      if( value == 0 ) relUnc = 0;
      else             relUnc = error / value;

      h_relUnc->SetBinContent(i_bin, relUnc);
      h_relUnc->SetBinError(i_bin, 0);
    }

    return h_relUnc;
  }

  void ProducePlot_Acc() {
    ProducePlot_DEN_NUM();
    ProducePlot_Acc_CentralValue();
    ProducePlot_Acc_SystVar("PDFHessian");
    ProducePlot_Acc_SystVar("alphaS");
    ProducePlot_Acc_SystVar("scaleVar");
  }

  void ProducePlot_DEN_NUM() {
    TString canvasName = "c_DEN_NUM";
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 1, 1);
    canvas->SetTitle("m [GeV]", "# events (norm. to Run2 lumi.)", "Acc.");


    TH1D* h_DEN = map_accInfo_["cv"]->DEN();
    TH1D* h_NUM = map_accInfo_["cv"]->NUM();
    canvas->Register(h_DEN, "Denominator (full phase space)", kBlack);
    canvas->Register(h_NUM, "Numerator (fiducial phase space)", kBlue);

    canvas->SetLegendPosition(0.50, 0.70, 0.95, 0.87);

    // canvas->SetRangeY(0, 1.01);
    canvas->SetRangeX(40, 3000);
    canvas->SetAutoRangeY();
    canvas->SetRangeRatio(0, 1.01);

    canvas->Latex_CMSInternal();
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, "Dilepton mass distribution at the dressed level");
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Fiducial region: p_{T} > 20(15) GeV, |#eta| < 2.4");

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }

  void ProducePlot_Acc_CentralValue() {
    TString canvasName = "c_acceptance_centralValue";
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 1, 0);
    canvas->SetTitle("m [GeV]", "Acceptance");

    TH1D* h_acc_cv = map_accInfo_["cv"]->Acc();
    canvas->Register(h_acc_cv, "Acceptance", kBlue);

    canvas->SetLegendPosition(0, 0, 0, 0);

    canvas->SetRangeX(40, 3000);
    canvas->SetRangeY(0, 1.01);
    // canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, "Dilepton acceptance w.r.t full phase space");
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "p_{T} > 20(15) GeV, |#eta| < 2.4");

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }

  void ProducePlot_Acc_SystVar(TString systType) {
    TString canvasName = "c_acceptance_systVar_"+systType;
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 1, 0);
    canvas->SetTitle("m [GeV]", "Acceptance", "ratio to c.v.");

    TH1D* h_acc_cv = map_accInfo_["cv"]->Acc();
    canvas->Register(h_acc_cv, "Central value", kBlack);
    if( systType == "PDFHessian" ) {
      canvas->Register(map_accInfo_["PDFVar_001"]->Acc(), "Hessian variation, 001", kBlue);
      canvas->Register(map_accInfo_["PDFVar_002"]->Acc(), "Hessian variation, 002", kGreen+2);
      canvas->Register(map_accInfo_["PDFVar_003"]->Acc(), "Hessian variation, 003", kViolet);
      canvas->Register(map_accInfo_["PDFVar_004"]->Acc(), "Hessian variation, 004", kCyan);
      canvas->SetLegendPosition(0.50, 0.32, 0.95, 0.70);
    }
    else if( systType == "alphaS" ) {
      canvas->Register(map_accInfo_["PDFVar_101"]->Acc(), "#alpha_{S} = 0.120", kBlue);
      canvas->Register(map_accInfo_["PDFVar_102"]->Acc(), "#alpha_{S} = 0.116", kGreen+2);
      canvas->SetLegendPosition(0.50, 0.32, 0.95, 0.50);
    }
    else if( systType == "scaleVar" ) {
      PlotTool::Print_Histogram(map_accInfo_["scaleVar_002"]->Acc());
      canvas->Register(map_accInfo_["scaleVar_002"]->Acc(), "(#mu_{R}, #mu_{F}): (1.0, 2.0) var.", kBlue);
      canvas->Register(map_accInfo_["scaleVar_003"]->Acc(), "(#mu_{R}, #mu_{F}): (1.0, 0.5) var.", kGreen+2);
      canvas->Register(map_accInfo_["scaleVar_004"]->Acc(), "(#mu_{R}, #mu_{F}): (2.0, 1.0) var.", kViolet);
      canvas->Register(map_accInfo_["scaleVar_005"]->Acc(), "(#mu_{R}, #mu_{F}): (2.0, 2.0) var.", kCyan);
      canvas->Register(map_accInfo_["scaleVar_007"]->Acc(), "(#mu_{R}, #mu_{F}): (0.5, 1.0) var.", kGray);
      canvas->Register(map_accInfo_["scaleVar_009"]->Acc(), "(#mu_{R}, #mu_{F}): (0.5, 0.5) var.", kRed);

      canvas->SetLegendPosition(0.50, 0.32, 0.95, 0.70);
    }

    canvas->SetRangeX(40, 3000);
    canvas->SetRangeY(0, 1.01);
    // canvas->SetRangeRatio(0.7, 1.3);
    canvas->SetRangeRatio(0.93, 1.07);
    // canvas->SetAutoRangeY();
    // canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, "Dilepton acceptance w.r.t full phase space");
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "p_{T} > 20(15) GeV, |#eta| < 2.4");
    canvas->RegisterLatex(0.16, 0.83, 42, 0.5, "Systematic variations");

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }

  void ProducePlot_Unc() {
    TString canvasName = "c_unc_acceptance_tot";
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 1, 0);
    canvas->SetTitle("m [GeV]", "Rel. uncertainty");

    canvas->Register(h_relUnc_stat_,       "MC stat.", kBlack);
    canvas->Register(h_relUnc_PDFHessian_, "PDF Hessian errors", kBlue);
    canvas->Register(h_relUnc_alphaS_,     "#alpha_{S} variation", kGreen+2);
    canvas->Register(h_relUnc_scale_,      "Scale variation", kViolet);
    canvas->Register(h_relUnc_tot_,        "Total (quad. sum)", kRed);

    canvas->SetLegendPosition(0.60, 0.74, 0.94, 0.90);

    canvas->SetRangeX(40, 3000);
    // canvas->SetRangeY(0, 0.03);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty on the acceptance";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);
    // canvas->SetSavePath(plotDirPath_);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");

    // -- zoom-in y-axis
    canvas->SetCanvasName(canvasName+"_zoomIn");
    canvas->SetAutoRangeY(kFALSE);
    canvas->SetRangeY(0, 0.05);
    canvas->Draw("HISTLP");
  }

  void Save() {
    TFile* f_output = TFile::Open(fileName_output_, "RECREATE");
    f_output->cd();

    // -- acceptance
    for(const auto& pair: map_accInfo_ ) {
      TH1D* h_acc = pair.second->Acc();
      TString type = pair.first;
      if( type.Contains("scaleVar_") ) {
        Save_ScaleVar_nanoAODOrder(h_acc, "h_acc", type); // -- save with the other name (sync. with nanoAOD order)
        h_acc->SetName("h_acc_"+pair.first+"_miniAOD"); // -- id: miniAOD order
      }
      else
        h_acc->SetName("h_acc_"+pair.first);
      h_acc->Write();
    }

    // -- denominator (full phase space)
    for(const auto& pair: map_accInfo_ ) {
      TH1D* h_DEN = pair.second->DEN();
      TString type = pair.first;
      if( type.Contains("scaleVar_") ) {
        Save_ScaleVar_nanoAODOrder(h_DEN, "h_FPS", type); // -- save with the other name (sync. with nanoAOD order)
        h_DEN->SetName("h_FPS_"+pair.first+"_miniAOD"); // -- id: miniAOD order
      }
      else
        h_DEN->SetName("h_FPS_"+pair.first);
      h_DEN->Write();
    }


    // -- numerator (fiducial region)
    for(const auto& pair: map_accInfo_ ) {
      TH1D* h_NUM = pair.second->NUM();
      TString type = pair.first;
      if( type.Contains("scaleVar_") ) {
        Save_ScaleVar_nanoAODOrder(h_NUM, "h_fid", type); // -- save with the other name (sync. with nanoAOD order)
        h_NUM->SetName("h_fid_"+pair.first+"_miniAOD"); // -- id: miniAOD order
      }
      else
        h_NUM->SetName("h_fid_"+pair.first);
      h_NUM->Write();
    }

    h_relUnc_stat_->SetName("h_relUnc_stat");
    h_relUnc_PDFHessian_->SetName("h_relUnc_PDFHessian");
    h_relUnc_alphaS_->SetName("h_relUnc_alphaS");
    h_relUnc_scale_->SetName("h_relUnc_scale");
    h_relUnc_tot_->SetName("h_relUnc_tot");

    h_relUnc_stat_->Write();
    h_relUnc_PDFHessian_->Write();
    h_relUnc_alphaS_->Write();
    h_relUnc_scale_->Write();
    h_relUnc_tot_->Write();
  }

  void Save_ScaleVar_nanoAODOrder(TH1D* h, TString baseName, TString type_miniAOD) {
    TString type_nanoAOD;

    if( type_miniAOD == "scaleVar_001" ) type_nanoAOD = "scaleVar_004"; //-- (1.0, 1.0);
    if( type_miniAOD == "scaleVar_002" ) type_nanoAOD = "scaleVar_005"; //-- (1.0, 2.0);
    if( type_miniAOD == "scaleVar_003" ) type_nanoAOD = "scaleVar_003"; //-- (1.0, 0.5);
    if( type_miniAOD == "scaleVar_004" ) type_nanoAOD = "scaleVar_007"; //-- (2.0, 1.0);
    if( type_miniAOD == "scaleVar_005" ) type_nanoAOD = "scaleVar_008"; //-- (2.0, 2.0);
    if( type_miniAOD == "scaleVar_006" ) type_nanoAOD = "scaleVar_006"; //-- (2.0, 0.5);
    if( type_miniAOD == "scaleVar_007" ) type_nanoAOD = "scaleVar_001"; //-- (0.5, 1.0);
    if( type_miniAOD == "scaleVar_008" ) type_nanoAOD = "scaleVar_002"; //-- (0.5, 2.0);
    if( type_miniAOD == "scaleVar_009" ) type_nanoAOD = "scaleVar_000"; //-- (0.5, 0.5);

    h->SetName(baseName+"_"+type_nanoAOD);
    h->Write();
  }
};