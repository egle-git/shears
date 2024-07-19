#include "Common/SimplePlotTools.h"
#include "Common/DYPath.h"
#include "Common/DYTool.h"

class PlotProducer_EffSFSyst {
public:
  PlotProducer_EffSFSyst(TString channel, TString fileName) : channel_(channel), fileName_(fileName) { }

  void Produce() {
    TH1::AddDirectory(kFALSE);
    fileName_ = DYTool::Get_OutputPath(fileName_);

    std::map<TString, vector<TString>>& map_systSource = 
    channel_ == "mm" ? map_systSource_mm_ : map_systSource_ee_;

    plotDirPath_ = DYTool::Set_PlotPath("EffSF/"+channel_);

    for( const auto& pair : map_systSource ) {      
      ProducePlot_Unc_PerSource(pair.first, pair.second);
      // -- additional plots for stat. unc.: unc. per era
      for( const auto& source : pair.second ) {
        if( source.Contains("stat") )
          ProducePlot_Unc_Stat_PerEra(pair.first, source);
      }

      // ProducePlot_2D_PerSource(pair.first, pair.second, "covM");
      ProducePlot_2D_PerSource(pair.first, pair.second, "corrM");
    }

    ProducePlot_Unc_Total();
    ProducePlot_2D_Total("covM");
    ProducePlot_2D_Total("corrM");
  }

private:
  TString channel_;
  TString fileName_;
  TString plotDirPath_;

  std::map<TString, vector<TString>> map_systSource_mm_ = {
    {"ID",    {"data-stat", "data-syst_all", "mc-stat", "mc-syst_all"}},
    {"ISO",   {"data-stat", "data-syst_all", "mc-stat", "mc-syst_all"}},
    {"STRIG", {"data-stat", "data-syst_all", "mc-stat", "mc-syst_all"}},
    {"DTRIG", {"sf-stat", "sf-syst_all"}}
  };
  // -- to keep the order for the plotting
  // -- std::map doesn't keep the order of insersion (automatically alphabetical ordered)
  vector<TString> vec_effType_mu_ = {"ID", "ISO", "STRIG", "DTRIG"};

  std::map<TString, vector<TString>> map_systSource_ee_ = {
    {"RECO",      {"sf-stat_data", "sf-stat_mc", "sf-syst_altBkg", "sf-syst_altSig", "sf-syst_altMC", "sf-syst_altTag"}},
    {"ID",        {"sf-stat_data", "sf-stat_mc", "sf-syst_altBkg", "sf-syst_altSig", "sf-syst_altMC", "sf-syst_altTag"}},
    {"TRIG_Leg1", {"sf-stat", "sf-syst_altMC", "sf-syst_altTag", "sf-syst_altSub"}},
    {"TRIG_Leg2", {"sf-stat", "sf-syst_altMC", "sf-syst_altTag", "sf-syst_altSub"}}
  };
  vector<TString> vec_effType_el_ = {"RECO", "ID", "TRIG_Leg1", "TRIG_Leg2"};

  vector<Int_t> vec_colorForCase_ = {
    kBlack, kBlue, kGreen+2, kViolet, kCyan, kGray
  };

  std::map<TString, TH1D*> map_uncHistEachType_;

  void ProducePlot_Unc_Total() {
    TString canvasName = TString::Format("c_relUnc_%s_final", channel_.Data());

    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    vector<TString> vec_effType = (channel_ == "mm") ? vec_effType_mu_ : vec_effType_el_;
    Int_t i_hist = 0;
    for(const auto& effType : vec_effType) {
      canvas->Register(map_uncHistEachType_[effType], Get_EffInfo(effType), vec_colorForCase_[i_hist]);
      i_hist++;
    }

    TH1D* h_relUnc_tot = PlotTool::Get_Hist(fileName_, "h_relUnc_effSF_tot");
    canvas->Register(h_relUnc_tot, "Total", kRed);

    canvas->SetLegendPosition(0.50, 0.70, 0.95, 0.95);

    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, "Uncertainty from the efficiency SF");
    if( channel_ == "mm" ) canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Muon channel");
    if( channel_ == "ee" ) canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Electron channel");

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP"); // -- default: .pdf format
  }

  void ProducePlot_Unc_PerSource(const TString effType, const vector<TString>& vec_systSource) {
    cout << "[ProducePlot_Unc_PerSource] effType = " + effType << endl;
    TString canvasName = TString::Format("c_relUnc_%s_%s", channel_.Data(), effType.Data());

    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    Int_t i_hist=0;
    vector<TH1D*> vec_uncHist;
    for( const auto& systSource : vec_systSource ) {
      TString systTag = TString::Format("%s-%s", effType.Data(), systSource.Data());
      TString histName = TString::Format("h_relUnc_effSF_%s", systTag.Data());

      TH1D* h_relUnc = PlotTool::Get_Hist(fileName_, histName);
      TString legend = Get_Legend(systSource);
      canvas->Register(h_relUnc, legend, vec_colorForCase_[i_hist]);
      i_hist++;

      vec_uncHist.push_back( h_relUnc );
    }
    TH1D* h_relUnc_tot = PlotTool::QuadSum_Hist(vec_uncHist);
    canvas->Register(h_relUnc_tot, "Total", kRed);
    map_uncHistEachType_.insert( std::make_pair(effType, h_relUnc_tot) );

    canvas->SetLegendPosition(0.50, 0.70, 0.95, 0.95);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString effInfo = Get_EffInfo(effType);
    if( channel_ == "mm" ) effInfo = "Muon, " + effInfo;
    if( channel_ == "ee" ) effInfo = "Electron, " + effInfo;

    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, effInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP"); // -- default: .pdf format
  }

  void ProducePlot_Unc_Stat_PerEra(TString effType, TString statSource) {
    cout << "[ProducePlot_Unc_Stat_PerEra] effType = " << effType << ", statSource = " + statSource << endl;
    TString canvasName = TString::Format("c_relUnc_%s_%s_%s", channel_.Data(), effType.Data(), statSource.Data());

    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    canvas->SetTitle("mass bin number", "Rel. uncertainty");

    Int_t i_hist = 0;
    vector<TString> vec_era = {"16pre", "16post", "17", "18"};
    std::map<TString, TString> map_eraLegend = {
      {"16pre", "2016 preAPV"},
      {"16post", "2016 postAPV"},
      {"17", "2017"},
      {"18", "2018"}
    };

    for(const auto& era : vec_era ) {
      TString systTag = TString::Format("%s-%s_%s", effType.Data(), statSource.Data(), era.Data());
      TString histName = TString::Format("h_relUnc_effSF_%s", systTag.Data());

      TH1D* h_relUnc = PlotTool::Get_Hist(fileName_, histName);
      canvas->Register(h_relUnc, "Stat. ("+map_eraLegend[era]+")", vec_colorForCase_[i_hist]);
      i_hist++;
    }
    TString systTag_tot = TString::Format("%s-%s", effType.Data(), statSource.Data());
    TString histName_tot = TString::Format("h_relUnc_effSF_%s", systTag_tot.Data());
    TH1D* h_relUnc_tot = PlotTool::Get_Hist(fileName_, histName_tot);
    canvas->Register(h_relUnc_tot, "Total Stat. (quad. sum)", kRed);

    canvas->SetLegendPosition(0.50, 0.70, 0.95, 0.95);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString effInfo = Get_EffInfo(effType);
    if( channel_ == "mm" ) effInfo = "Muon, " + effInfo;
    if( channel_ == "ee" ) effInfo = "Electron, " + effInfo;

    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, effInfo);

    TString statType = "";
    if( statSource == "data-stat" || 
        statSource == "sf-stat_data" ) statType = "Stat. Unc. (data eff.)";
    if( statSource == "mc-stat" || 
        statSource == "sf-stat_mc" )   statType = "Stat. Unc. (MC eff.)";
    if( statSource == "sf-stat" ) statType = "Stat. Unc.";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, statType);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP"); // -- default: .pdf format
  }

  TString Get_EffInfo(TString effType) {
    TString effInfo = effType + " SF";
    if( effType == "STRIG" )     effInfo = "Single muon trigger SF";
    if( effType == "DTRIG" )     effInfo = "Double muon trigger SF";
    if( effType == "TRIG_Leg1" ) effInfo = "Double electron trigger SF, Leg1";
    if( effType == "TRIG_Leg2" ) effInfo = "Double electron trigger SF, Leg2";

    return effInfo;
  }

  void ProducePlot_2D_Total(TString matrixType) {

    TString canvasName = "c2D_"+matrixType+"_"+channel_+"_tot";

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 0, 0, 0);
    canvas->SetTitle("mass bin number", "mass bin number");

    TString histName = TString::Format("h_%s_effSF_tot", matrixType.Data());
    TH2D* h2D = PlotTool::Get_Hist2D(fileName_, histName);
    canvas->Register(h2D);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeZ(minZ, maxZ);
    if( matrixType == "corrM" ) canvas->SetRangeZ(-1.01, 1.01);
    else                        canvas->SetAutoRangeZ();

    canvas->Latex_CMSInternal();
    TString info = "Total covariance matrix (eff. SF)";
    if( matrixType == "corrM" ) info.ReplaceAll("covariance", "correlation");
    canvas->RegisterLatex(0.16, 0.91, 42, 0.5, info);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();
  }

  void ProducePlot_2D_PerSource(const TString effType, const vector<TString>& vec_systSource, TString matrixType) {

    for(const auto& systSource : vec_systSource) {
      TString canvasName = TString::Format(
        "c2D_%s_%s_%s_%s", 
        matrixType.Data(), channel_.Data(), effType.Data(), systSource.Data());

      PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 0, 0, 0);
      canvas->SetTitle("mass bin number", "mass bin number");

      TString systTag = TString::Format("%s-%s", effType.Data(), systSource.Data());
      TString histName = TString::Format("h_%s_effSF_%s", matrixType.Data(), systTag.Data());

      TH2D* h2D = PlotTool::Get_Hist2D(fileName_, histName);
      canvas->Register(h2D);

      if( matrixType == "corrM" ) canvas->SetRangeZ(-1.01, 1.01);
      else                        canvas->SetAutoRangeZ();

      canvas->Latex_CMSInternal();
      TString info = "Covariance matrix (eff. SF, " + effType + "-" + systSource + ")";
      if( matrixType == "corrM" ) info.ReplaceAll("Covariance", "Correlation");
      canvas->RegisterLatex(0.16, 0.91, 42, 0.5, info);

      canvas->SetSavePath(plotDirPath_);

      canvas->Draw();
    }
  }

  TString Get_Legend(TString systSource) {
    TString legend = "undefined";

    if( systSource == "data-stat" ) legend = "Stat. (data eff.)";
    if( systSource == "mc-stat" )   legend = "Stat. (mc eff.)";

    if( systSource == "data-syst_all" ) legend = "Syst. (data eff.)";
    if( systSource == "mc-syst_all" )   legend = "Syst. (mc eff.)";

    if( systSource == "sf-stat" )     legend = "Stat.";
    if( systSource == "sf-syst_all" ) legend = "Syst.";

    if( systSource == "sf-stat_data" ) legend = "Stat. (data eff.)";
    if( systSource == "sf-stat_mc" )   legend = "Stat. (mc eff.)";

    if( systSource == "sf-syst_altBkg" ) legend = "Syst. (alt. bkg. shape)";
    if( systSource == "sf-syst_altSig" ) legend = "Syst. (alt. sig. shape)";
    if( systSource == "sf-syst_altMC" )  legend = "Syst. (alt. MC sample)";
    if( systSource == "sf-syst_altTag" ) legend = "Syst. (alt. Tag selection)";
    if( systSource == "sf-syst_altSub" ) legend = "Syst. (alt. bkg. subtraction)";

    return legend;
  }
};

void producePlots_unc() {
  PlotProducer_EffSFSyst producer_mm("mm", "UncAndCov_EffSF_mm.root");
  producer_mm.Produce();

  PlotProducer_EffSFSyst producer_ee("ee", "UncAndCov_EffSF_ee.root");
  producer_ee.Produce();
}