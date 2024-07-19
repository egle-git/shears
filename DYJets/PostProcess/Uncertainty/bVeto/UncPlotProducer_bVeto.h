#include "Common/SimplePlotTools.h"
#include "Common/DYTool.h"

class UncPlotProducer_bVeto {
public:
  UncPlotProducer_bVeto(TString channel): channel_(channel) {

  }

  void Set_InputFileName(TString fileName) { inputFileName_ = DYTool::Get_OutputPath(fileName); }

  void Set_PlotDir(TString dirName) { plotDirPath_ = DYTool::Set_PlotPath(dirName); }

  void Produce() {
    if( gSystem->AccessPathName(inputFileName_) )
      throw std::runtime_error(inputFileName_+ " does not exist");

    if( plotDirPath_ == "" )
      plotDirPath_ = DYTool::Set_PlotPath("plot_unc/"+channel_);

    ProducePlot_RelUnc_All();
    ProducePlot_Cov_All();
  }

private:
  TString channel_ = "";
  TString inputFileName_ = "";
  TString plotDirPath_ = "";

  vector<TString> vec_era_ = {"16pre", "16post", "17", "18"};

  // vector<TString> vec_uncType_mcEff_ = {"MC_eff_high", "MC_eff_low"};

  vector<TString> vec_flavor_bTagSF_ = {"light", "heavy"};
  // vector<TString> vec_uncType_bTagSF_corr_   = {"up_correlated", "down_correlated"};
  // vector<TString> vec_uncType_bTagSF_unCorr_ = {"up_uncorrelated", "down_uncorrelated"};

  void ProducePlot_RelUnc_All() {
    ProducePlot_RelUnc_MCEffStat();

    // -- vs. each era (for each flavor)
    for(const auto& flavor : vec_flavor_bTagSF_ )
      ProducePlot_RelUnc_bTagSF_UnCorr_EachEra(flavor);

    // -- light vs. heavy
    ProducePlot_RelUnc_bTagSF("unCorr");
    ProducePlot_RelUnc_bTagSF("corr");

    // -- final uncertainty
    ProducePlot_RelUnc_Total();
  }

  void ProducePlot_RelUnc_Total() {
    // -- (legend, relUnc) pair
    std::map<TString, TH1D*> map_hist;
    vector<TString> vec_legend; // -- to keep order

    TH1D* h_relUnc_mcStatEff     = PlotTool::Get_Hist(inputFileName_, "h_relUnc_bVeto_mcEffStat_tot");
    TH1D* h_relUnc_bTagSF_unCorr = PlotTool::Get_Hist(inputFileName_, "h_relUnc_bVeto_bTagSF_unCorr_tot");
    TH1D* h_relUnc_bTagSF_corr   = PlotTool::Get_Hist(inputFileName_, "h_relUnc_bVeto_bTagSF_corr_tot");
    TH1D* h_relUnc_tot           = PlotTool::Get_Hist(inputFileName_, "h_relUnc_bVeto_tot");

    map_hist.insert( std::make_pair("MC eff. stat.", h_relUnc_mcStatEff) ); vec_legend.push_back( "MC eff. stat." );
    map_hist.insert( std::make_pair("b-tagging SF (uncorr.)", h_relUnc_bTagSF_unCorr) ); vec_legend.push_back( "b-tagging SF (uncorr.)" );
    map_hist.insert( std::make_pair("b-tagging SF (corr.)", h_relUnc_bTagSF_corr) ); vec_legend.push_back( "b-tagging SF (corr.)" );
    map_hist.insert( std::make_pair("Total (quad. sum)", h_relUnc_tot) ); vec_legend.push_back( "Total (quad. sum)" );

    ProducePlot_RelUnc("total", map_hist, vec_legend);
  }

  // -- corrType: unCorr, corr
  void ProducePlot_RelUnc_bTagSF(TString corrType) {
    // -- (legend, relUnc) pair
    std::map<TString, TH1D*> map_hist;
    vector<TString> vec_legend; // -- to keep order

    for(const auto& flavor : vec_flavor_bTagSF_) {
      TString uncTag = "bVeto_bTagSF_"+corrType+"_"+flavor;
      if( corrType == "unCorr" ) uncTag += "_tot"; // -- unCorr: quad. sum over eras
      TString histName = "h_relUnc_"+uncTag;
      TH1D* h_relUnc = PlotTool::Get_Hist(inputFileName_, histName);
      TString legend;
      if( flavor == "light" ) legend = "light-jet";
      if( flavor == "heavy" ) legend = "heavy-jet (b, c)";

      map_hist.insert( std::make_pair(legend, h_relUnc) );
      vec_legend.push_back( legend );
    }

    TString histName_tot = "h_relUnc_bVeto_bTagSF_"+corrType+"_tot";
    TH1D* h_relUnc_tot = PlotTool::Get_Hist(inputFileName_, histName_tot);
    TString legend_tot = "Quad. sum";
    map_hist.insert( std::make_pair(legend_tot, h_relUnc_tot) );
    vec_legend.push_back( legend_tot );

    ProducePlot_RelUnc("bTagSF_"+corrType, map_hist, vec_legend);
  }

  void ProducePlot_RelUnc_bTagSF_UnCorr_EachEra(TString flavor) {
    // -- (legend, relUnc) pair
    std::map<TString, TH1D*> map_hist;
    vector<TString> vec_legend; // -- to keep order

    for(const auto& era : vec_era_) {
      TString uncTag = "bVeto_bTagSF_unCorr_"+flavor+"_"+era;
      TString histName = "h_relUnc_"+uncTag;
      TH1D* h_relUnc = PlotTool::Get_Hist(inputFileName_, histName);
      TString legend = EraInfo(era);

      map_hist.insert( std::make_pair(legend, h_relUnc) );
      vec_legend.push_back( legend );
    }

    TH1D* h_relUnc_tot = PlotTool::Get_Hist(inputFileName_, "h_relUnc_bVeto_bTagSF_unCorr_"+flavor+"_tot");
    TString legend_tot = "Quad. sum";
    map_hist.insert( std::make_pair(legend_tot, h_relUnc_tot) );
    vec_legend.push_back( legend_tot );

    ProducePlot_RelUnc("bTagSF_unCorr_"+flavor, map_hist, vec_legend);
  }

  void ProducePlot_RelUnc_MCEffStat() {
    // -- (legend, relUnc) pair
    std::map<TString, TH1D*> map_hist;
    vector<TString> vec_legend; // -- to keep order

    for(const auto& era : vec_era_) {
      TString uncTag = "bVeto_mcEffStat_"+era;
      TString histName = "h_relUnc_"+uncTag;
      TH1D* h_relUnc = PlotTool::Get_Hist(inputFileName_, histName);
      TString legend = EraInfo(era);

      map_hist.insert( std::make_pair(legend, h_relUnc) );
      vec_legend.push_back( legend );
    }

    TH1D* h_relUnc_tot = PlotTool::Get_Hist(inputFileName_, "h_relUnc_bVeto_mcEffStat_tot");
    TString legend_tot = "Quad. sum";
    map_hist.insert( std::make_pair(legend_tot, h_relUnc_tot) );
    vec_legend.push_back( legend_tot );

    ProducePlot_RelUnc("MCEffStat", map_hist, vec_legend);
  }

  void ProducePlot_RelUnc(TString uncType, std::map<TString, TH1D*>& map_hist, vector<TString> vec_legend) {
    vector<Int_t> vec_color_era = {kBlack, kGreen+2, kBlue, kViolet};

    TString canvasName = "c_unc_"+uncType+"_"+channel_;
    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    // TString titleX = "m(#mu#mu) [GeV]";
    // if( channel_ == "ee" ) titleX.ReplaceAll("#mu", "e");
    TString titleX = "mass bin number";
    canvas->SetTitle(titleX, "Rel. uncertainty");

    Int_t i_hist = 0;
    for(const auto& legend : vec_legend ) {
      Int_t color = 0;
      // if( uncType == "MCEffStat" )             color = vec_color_era[i_hist];
      // if( uncType.Contains("bTagSF_unCorr_") ) color = vec_color_era[i_hist]; // -- light or heavy
      color = vec_color_era[i_hist];

      if( i_hist == vec_legend.size()-1 ) color = kRed; // -- color for "total" uncertainty (last element)

      canvas->Register(map_hist[legend], legend, color);

      i_hist++;
    }

    canvas->SetLegendPosition(0.55, 0.70, 0.94, 0.91);

    canvas->SetRangeY(0, 0.02);
    // if( channel_ == "ee" ) canvas->SetRangeY(0, 0.13);
    // if( channel_ == "mm" ) canvas->SetRangeY(0, 0.6);
    // canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    TString uncInfo = "Uncertainty from b-veto";
    if( uncType == "MCEffStat" )       uncInfo += " ( MC eff. stat. unc.)";
    if( uncType == "bTagSF_unCorr_light" ) uncInfo += " (b-tagging SF, uncorr., light-jets)";
    if( uncType == "bTagSF_unCorr_heavy" ) uncInfo += " (b-tagging SF, uncorr., heavy(b/c)-jets)";
    if( uncType == "bTagSF_corr_light" ) uncInfo += " (b-tagging SF, corr., light-jets)";
    if( uncType == "bTagSF_corr_heavy" ) uncInfo += " (b-tagging SF, corr., heavy(b/c)-jets)";
    if( uncType == "bTagSF_unCorr" ) uncInfo += " (b-tagging SF, uncorr. among eras)";
    if( uncType == "bTagSF_corr" )   uncInfo += " (b-tagging SF, corr. among eras)";

    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, uncInfo);
    TString channelInfo = (channel_ == "mm") ? "Muon channel" : "Electron channel";
    canvas->RegisterLatex(0.16, 0.87, 42, 0.6, channelInfo);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw("HISTLP");
  }


  TString EraInfo(TString era) {
    if( era == "16pre")       return "2016-preAPV";
    else if( era == "16post") return "2016-postAPV";
    else if( era == "17" )  return "2017";
    else if( era == "18" )  return "2018";
    else
      throw std::invalid_argument("[UncPlotProducer_bVeto::EraInfo] era = " + era + " is not supported");

    return "undefined";
  }

  void ProducePlot_Cov_All() {

  }

  void ProducePlot_2D(TString matrixType, TString uncType) {
    TString canvasName = "c2D_"+matrixType+"_"+uncType+"_"+channel_;

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 1, 1, 0);
    TString title = "m(#mu#mu) [GeV]";
    if( channel_ == "ee" ) title.ReplaceAll("#mu", "e");
    canvas->SetTitle(title, title);

    TString histName = "h_"+matrixType+"_bVeto_"+uncType;
    TH2D* h2D = PlotTool::Get_Hist2D(inputFileName_, histName);
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