#include "Common/DYPath.h"
#include "Common/DYOutput.h"
#include "Common/DYTool.h"

class CentralValueProducer {
public:
  CentralValueProducer(TString channel): channel_(channel) {}

  void SetPath_ShearsOutput(TString path) { shearsPath_ = path; }

  void Use_Fake(Bool_t flag=kTRUE) { useFake_ = flag; }

  void Produce() {
    Init();

    for(const auto& era: vec_era_ ) {
      ProducePlot_DileptonMass_Reco(era);
      ProducePlot_DileptonMass_Reco_RatioToData(era);
      ProducePlot_MigM(era, kFALSE);
      ProducePlot_MigM(era, kTRUE);
    }

    ProducePlot_TUnfold_Reco();
    ProducePlot_TUnfold_MigM(kFALSE);
    ProducePlot_TUnfold_MigM(kTRUE);
    ProducePlot_TUnfold_UnderOverflow("under", "reco"); // -- DY inefficiencies
    ProducePlot_TUnfold_UnderOverflow("under", "true"); // -- DY fakes
    ProducePlot_TUnfold_UnderOverflow("over", "reco"); // -- for x-check
    ProducePlot_TUnfold_UnderOverflow("over", "true"); // -- for x-check
    ProducePlot_ClosureTest();
    ProducePlot_Unfolded("unfolded");
    ProducePlot_Unfolded("dsigdm");

    // -- save the theory/data ratio as well (for the comparison in the future)!
    Save();
  }

private:
  TString channel_ = "";
  TString shearsPath_ = "";
  TString plotDirPath_ = "";
  Bool_t useFake_ = kTRUE;

  vector<TString> vec_era_ = {"16pre", "16post", "17", "18"};

  Run2Output* output_;
  DYRun2Result* result_;

  Int_t count_canvas_ = 0;

  void Init() {
    if( shearsPath_ == "" ) {
      shearsPath_ = DYTool::path_default+"/"+channel_;
      cout << "[CentralValueProducer::Init] Default path is used: " + shearsPath_ << endl;
    }
    output_ = new Run2Output(shearsPath_);
    result_ = new DYRun2Result(output_);
    if( useFake_ )
      DYTool::Set_Fake(channel_, result_);
    result_->Produce();

    plotDirPath_ = "./basicPlot/"+channel_;
    if( !useFake_ ) plotDirPath_ += "/noFake";

    bool recursive = kTRUE;
    if( gSystem->mkdir(plotDirPath_.Data(), recursive) < 0 )
      throw std::runtime_error("Directory = " + plotDirPath_ + " cannot be created (already exists?)");
  }

  void ProducePlot_DileptonMass_Reco(TString era) {

    EraOutput& eraOutput = output_->Get_EraOutput(era);

    TString tag = TString::Format("dileptonMass_reco_%s_%s", era.Data(), channel_.Data());
    if( !useFake_ )
      tag = tag + "_noFake";

    TString canvasName = Make_CanvasName(tag);

    PlotTool::HistStackCanvaswRatio* canvas = new PlotTool::HistStackCanvaswRatio(canvasName, 1, 1);
    canvas->Ratio_Reversed();
    canvas->SetTitle( "m [GeV]", "# events", "Pred./data" );

    TString histName = "mass_wide_range_inc0jet";


    canvas->RegisterData(eraOutput.Get(histName, "data"), "Data", kBlack);

    if( useFake_ ) 
      canvas->Register(eraOutput.Get("dummy", "fake"), Get_Legend("fake"), Get_Color("fake"));

    canvas->Register(eraOutput.Get(histName, "GG"),        Get_Legend("GammaGamma"), Get_Color("GammaGamma"));
    canvas->Register(eraOutput.Get(histName, "VV"),        Get_Legend("VV"),         Get_Color("VV"));
    canvas->Register(eraOutput.Get(histName, "TauTau"),    Get_Legend("TauTau"),     Get_Color("TauTau"));
    canvas->Register(eraOutput.Get(histName, "singleTop"), Get_Legend("singleTop"),  Get_Color("singleTop"));
    canvas->Register(eraOutput.Get(histName, "TT"),        Get_Legend("TT"),         Get_Color("TT"));

    canvas->Register(eraOutput.Get(histName, "DY"), Get_Legend("DY"), Get_Color("DY"));

    // canvas->SetRangeX(81, 101);

    // canvas->SetAutoRangeY();
    canvas->SetRangeX(40, 3000);
    canvas->SetRangeY(0.5, 5e9);
    canvas->SetRangeRatio(0.86, 1.14);

    canvas->ShowDataMCRatio();

    canvas->SetLegendColumn(2);
    canvas->SetLegendPosition(0.45, 0.80, 0.95, 0.95);

    canvas->Latex_CMSInternal();
    canvas->Latex_LumiEnergy(eraOutput.Lumi()/1000.0, 13);

    TString channelInfo = "";
    if( channel_ == "mm" ) channelInfo = "Muon channel";
    if( channel_ == "ee" ) channelInfo = "Electron channel";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.7, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }

  void ProducePlot_DileptonMass_Reco_RatioToData(TString era) {

    EraOutput& eraOutput = output_->Get_EraOutput(era);

    TString histName = "mass_wide_range_inc0jet";
    TH1D* h_data = eraOutput.Get(histName, "data");

    TH1D* h_fake = useFake_ ? GetHist_RatiotoData(eraOutput, histName, "fake", h_data) : nullptr;
    TH1D* h_GG = GetHist_RatiotoData(eraOutput, histName, "GG", h_data);
    TH1D* h_VV = GetHist_RatiotoData(eraOutput, histName, "VV", h_data);
    TH1D* h_TauTau = GetHist_RatiotoData(eraOutput, histName, "TauTau", h_data);
    TH1D* h_singleTop = GetHist_RatiotoData(eraOutput, histName, "singleTop", h_data);
    TH1D* h_TT = GetHist_RatiotoData(eraOutput, histName, "TT", h_data);
    TH1D* h_DY = GetHist_RatiotoData(eraOutput, histName, "DY", h_data);
    TH1D* h_pred = GetHist_RatiotoData(eraOutput, histName, "pred", h_data); // -- total prediction

    TString tag = TString::Format("dileptonMass_reco_ratioToData_%s_%s", era.Data(), channel_.Data());
    if( !useFake_ )
      tag = tag + "_noFake";

    TString canvasName = Make_CanvasName(tag);

    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 1, 0);
    TString titleX = (channel_ == "ee") ? "m(ee) [GeV]" : "m(#mu#mu) [GeV]";
    canvas->SetTitle(titleX, "Fraction to data");

    if( useFake_ ) 
      canvas->Register(h_fake, Get_Legend("fake"), Get_Color("fake"));

    canvas->Register(h_GG,        Get_Legend("GammaGamma"), Get_Color("GammaGamma"));
    canvas->Register(h_VV,        Get_Legend("VV"),         Get_Color("VV"));
    canvas->Register(h_TauTau,    Get_Legend("TauTau"),     Get_Color("TauTau"));
    canvas->Register(h_singleTop, Get_Legend("singleTop"),  Get_Color("singleTop"));
    canvas->Register(h_TT,        Get_Legend("TT"),         Get_Color("TT"));
    canvas->Register(h_DY,        Get_Legend("DY"),         Get_Color("DY"));
    canvas->Register(h_pred,      "Total pred. (DY+bkg.)",  kOrange-3);

    // canvas->SetRangeX(81, 101);
    // canvas->SetAutoRangeY();
    canvas->SetRangeX(40, 3000);
    canvas->SetRangeY(0, 1.4);

    canvas->SetLegendColumn(2);
    canvas->SetLegendPosition(0.45, 0.80, 0.95, 0.95);

    canvas->Latex_CMSInternal();
    canvas->Latex_LumiEnergy(eraOutput.Lumi()/1000.0, 13);

    TString channelInfo = "";
    if( channel_ == "mm" ) channelInfo = "Muon channel";
    if( channel_ == "ee" ) channelInfo = "Electron channel";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.7, channelInfo);
    canvas->RegisterLatex(0.16, 0.87, 42, 0.6, "Stat. unc. only");

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }

  TH1D* GetHist_RatiotoData(EraOutput& eraOutput, TString histName, TString type, TH1D* h_data) {
    TH1D* h = nullptr;
    if( type == "pred" ) {
      if( useFake_ ) h = eraOutput.Get(histName, "bkgAll");
      else           h = eraOutput.Get(histName, "bkgMC");
      TH1D* h_DY = eraOutput.Get(histName, "DY");
      h->Add(h_DY); // (DY + total bkg.)
    }
    else
      h = eraOutput.Get(histName, type);

    h->Divide(h_data); // -- divide by h_data
    return h;
  }

  TString Make_CanvasName(TString tag) {
    TString name = TString::Format("c%02d_%s", count_canvas_, tag.Data());
    count_canvas_++;

    return name;
  }

  void ProducePlot_MigM(TString era, Bool_t doNorm) {
    TString tag = TString::Format("migM_%s_%s", era.Data(), channel_.Data());
    if( doNorm ) tag.ReplaceAll("migM", "respM");
    TString canvasName = Make_CanvasName(tag);
  }

  void ProducePlot_TUnfold_Reco() {

  }

  void ProducePlot_TUnfold_MigM(Bool_t doNorm) {
    TString tag = TString::Format("migM_TUnfold_%s", channel_.Data());
    if( doNorm ) tag.ReplaceAll("migM", "respM");
    TString canvasName = Make_CanvasName(tag);

  }

  void ProducePlot_TUnfold_UnderOverflow(TString binType, TString eventType) {
    // -- print "overflow" bins as well

  }

  void ProducePlot_ClosureTest() {

  }

  void ProducePlot_Unfolded(TString type) {
    Double_t lumi = LUMI_16pre + LUMI_16post + LUMI_17 + LUMI_18;

    TH1D* h_data   = result_->Get_AllEra("unfolded", "data");
    TH1D* h_theory = result_->Get_AllEra("gen", "DY");
    if( type == "dsigdm" ) {
      h_data   = DYTool::Convert_TUnfoldOutput_DSigmaDM(h_data, lumi);
      h_theory = DYTool::Convert_TUnfoldOutput_DSigmaDM(h_theory, lumi);
    }

    TString tag = TString::Format("%s_%s", type.Data(), channel_.Data());
    TString canvasName = Make_CanvasName(tag);
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

    canvas->Register(h_data,   "Data", kBlack);
    canvas->Register(h_theory, "Theory (Powheg, MiNNLO)", kGreen-2);

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
      canvas->RegisterLatex(0.16, 0.87, 42, 0.6, "Unfolded distribution");

    canvas->RegisterLatex(0.18, 0.34, 42, 0.6, "Stat. uncertainty only");

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();

    TString baseName = canvas->GetCanvasName();
    canvas->SetRangeRatio(0, 2.5);
    canvas->SetCanvasName( baseName + "_ratioZoomOut" );
    canvas->Draw();
  }

  TString Get_Legend(TString tag) {
    TString legend = "";
    if( tag == "TT" ) legend = "t#bar{t}";
    if( tag == "singleTop" ) legend = "single t";
    if( tag == "TauTau" ) legend = "Z/#gamma*#rightarrow#tau#tau";
    if( tag == "WJetsToLNu" ) legend = "W+jets";
    if( tag == "VV" ) legend = "VV";
    if( tag == "GammaGamma" ) legend = "#gamma#gamma#rightarrow ll";
    if( tag == "fake" ) legend = "Fakes";
    if( tag == "DY" ) {
      legend = "Z/#gamma*#rightarrow#mu#mu";
      if( channel_ == "ee" ) legend.ReplaceAll("#mu#mu", "ee");
    }

    return legend;
  }

  Int_t Get_Color(TString tag) {
    Int_t color = 1;
    if( tag == "TT" ) color = kBlue;
    if( tag == "singleTop" ) color = kMagenta;
    if( tag == "TauTau" ) color = kCyan + 2;
    if( tag == "WJetsToLNu" ) color = kOrange;
    if( tag == "VV" ) color = kRed + 1;
    if( tag == "GammaGamma" ) color = kOrange;
    if( tag == "fake" ) color = kViolet-5;
    if( tag == "DY" ) color = kGreen-8;

    return color;
  }

  void Save() {
    TString fileName = "DYRun2Result_CentralValue_"+channel_+".root";
    if( !useFake_ )
      fileName.ReplaceAll(".root", "_noFake.root");
    TFile *f_output = TFile::Open(fileName, "RECREATE");
    f_output->cd();
    result_->Save(f_output);
    f_output->Close();
  }
};

void produce_centralValue() {
  CentralValueProducer producer_ee("ee");
  CentralValueProducer producer_mm("mm");
  producer_ee.Produce();
  producer_mm.Produce();

  // -- results w/o fake lepton backgrounds
  producer_ee.Use_Fake(kFALSE);
  producer_mm.Use_Fake(kFALSE);
  producer_ee.Produce();
  producer_mm.Produce();
}