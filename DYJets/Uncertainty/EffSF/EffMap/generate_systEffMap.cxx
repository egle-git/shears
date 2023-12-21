#include "Common/SimplePlotTools.h"
#include "Common/DYPath.h"

// -- output: efficiency histograms in 2D (pt, eta): for 4-eras, for both ee and mm channels
// -- for 1 era:
// ---- muon, ID
// ------ cv map (cv = central value)
// ------ nStatMap maps with the variation using stat. unc.
// ------ maps with syst. variation from each source
// ---- muon, ISO
// ------ cv map
// ------ nStatMap maps with the variation using stat. unc.
// ------ maps with syst. variation from each source
// -- muon, STRIG (single muon trigger)
// ------ cv map
// ------ nStatMap maps with the variation using stat. unc.
// ------ maps with syst. variation from each source
// ---- muon, DTRIG (double muon trigger)
// ------ cv map
// ------ nStatMap maps with the variation using stat. unc.
// ------ maps with syst. variation from each source
// ---- electron, ID
// ------ cv map
// ------ nStatMap maps with the variation using stat. unc.
// ------ maps with syst. variation from each source
// ---- electron, TRIG_Leg1
// ------ cv map
// ------ nStatMap maps with the variation using stat. unc.
// ------ maps with syst. variation from each source
// ---- electron, TRIG_Leg2
// ------ cv map
// ------ nStatMap maps with the variation using stat. unc.
// ------ maps with syst. variation from each source

#define nStatMap 100 // how many stat-varied maps will be generated?

class EffMap {
public:
  // -- era: 16pre, 16post, 17, 18
  // -- leptType: el, mu
  // -- effType: ID, ISO, TRIG_Leg1 (el-only), TRIG_Leg2 (el-only), STRIG (mu-only), DTRIG (mu-only)
  // -- dataType: data, mc, sf
  EffMap(TString era, TString lepType, TString effType, TString dataType) {
    era_ = era;
    lepType_  = lepType;
    effType_  = effType;
    dataType_ = dataType;
    Init();
  }

  // type: cv, stat_1, stat_2, ... stat_nEffMap, syst_all_plus, etc
  TH2D* GetEffHist(TString type) { 
    auto it_pair = map_effHist_.find(type);
    if( it_pair != map_effHist_.end() )
      return it_pair->second;
      
    std::cout << "no histogram corresponding to " << type << std::endl;
    return nullptr;
  };

  void Save(TFile *f_output) {
    f_output->cd();

    for(auto pair : map_effHist_ ) {
      TString histType = pair.first;

      TString histName = TString::Format("%s-%s-%s-%s-%s", era_.Data(), lepType_.Data(), effType_.Data(), dataType_.Data(), histType.Data());
      pair.second->SetName(histName);
      pair.second->Write(histName);
    }
  }

private:
  TString era_ = "";
  TString lepType_ = "";
  TString effType_ = "";
  TString dataType_ = "";

  TString fileName_ = "";
  std::map<TString, TH2D*> map_effHist_;

  void Init() {
    TString basePath = DYTool::path_centralEffMap;
    fileName_ = TString::Format("%s/%s/eff_%s_%s.root", basePath.Data(), era_.Data(), lepType_.Data(), effType_.Data());

    if( lepType_ == "mu" ) Init_Mu();
    if( lepType_ == "el" ) Init_El();
  }

  void Init_El() {
    if( dataType_ != "sf" ) {
      cout << "no separate data or MC value is provided for " << lepType_ << endl;
      return;
    }

    TString histName_cv = Get_HistName_El();
    TH2D* h2D_cv = PlotTool::Get_Hist2D(fileName_, histName_cv);
    map_effHist_.insert( std::make_pair("cv", h2D_cv) );

    if( effType_ == "RECO" || effType_ == "ID" ) {

      vector<TString> vec_statSource = {"stat_data", "stat_mc"};
      for( auto uncType : vec_statSource ) {
        TString histName_sf_absUnc_stat = Get_HistName_El(uncType);
        TH2D* h2D_absUnc_stat = PlotTool::Get_Hist2D(fileName_, histName_sf_absUnc_stat);
        Produce_EffMap_StatVariation(h2D_cv, h2D_absUnc_stat, uncType);
      }

      vector<TString> vec_systSource = {
        "syst_altBkg", "syst_altSig", "syst_altMC", "syst_altTag"
      };
      for( auto uncType : vec_systSource ) {
        TString histName_sf_absUnc_syst = Get_HistName_El(uncType);
        TH2D* h2D_absUnc_syst = PlotTool::Get_Hist2D(fileName_, histName_sf_absUnc_syst);
        Produce_EffMap_SystVariation(h2D_cv, h2D_absUnc_syst, uncType);
      }

    }
    else if( effType_ == "TRIG_Leg1" || effType_ == "TRIG_Leg2" ) {
      // -- SF with stat variation
      TString histName_sf_wStatUnc = Get_HistName_El("stat");
      TH2D* h2D_sf_wStatUnc = PlotTool::Get_Hist2D(fileName_, histName_sf_wStatUnc);
      TH2D* h2D_absUnc_stat = Get_UncHist2D(h2D_sf_wStatUnc);
      Produce_EffMap_StatVariation(h2D_cv, h2D_absUnc_stat);
      // ProducePlot_ConvertedTo1D(h2D_absUnc_stat, "absUnc_stat");

      // vector<TString> vec_systSource = {
      //   "syst_altBkg", "syst_altSig", "syst_altMC", "syst_altTag", "syst_altSub", "syst_altGenMat"
      // };

      // -- these are the only relevant sources for trigger SF (cut&count, not fitting)
      vector<TString> vec_systSource = {
        "syst_altMC", "syst_altTag", "syst_altSub"
      };

      for( auto uncType : vec_systSource ) {
        // -- SF with systematic variation
        TString histName_sf_systVar = Get_HistName_El(uncType);
        TH2D* h2D_sf_systVar = PlotTool::Get_Hist2D(fileName_, histName_sf_systVar);
        map_effHist_.insert( std::make_pair(uncType,  h2D_sf_systVar) );

        TH2D* h2D_absUnc_syst = PlotTool::Hist2DOperation("h2D_absUnc_syst", h2D_sf_systVar, h2D_cv, "-");
        if( uncType == "syst_altSig" ) {
          // h2D_absUnc_syst = Get_UncHist2D(h2D_sf_systVar);
          // PlotTool::Print_Histogram2D(h2D_absUnc_syst);
        }

        map_effHist_.insert( std::make_pair("absUnc_"+uncType,  h2D_absUnc_syst) ); // -- also save the uncertainty map
        // ProducePlot_ConvertedTo1D(h2D_absUnc_syst, "absUnc_"+uncType);
      }
    }

  }

  TString Get_HistName_El(TString uncType = "cv") {
    TString histName = "";

    if( effType_ == "RECO" || effType_ == "ID" ) {
      histName = "EGamma_SF2D";

      if( uncType == "stat_data")   histName = "statData";
      if( uncType == "stat_mc")     histName = "statMC";
      if( uncType == "syst_altBkg") histName = "altBkgModel";
      if( uncType == "syst_altSig") histName = "altSignalModel";
      if( uncType == "syst_altMC")  histName = "altMCEff";
      if( uncType == "syst_altTag")  histName = "altTagSelection";
    }
    else if( effType_ == "TRIG_Leg1" || effType_ == "TRIG_Leg2" ) {
      histName = "sf";

      if( uncType == "stat" ) histName = "sf_s0m0";
      if( uncType == "syst_altBkg" ) histName = "sf_s1m0";
      if( uncType == "syst_altSig" ) histName = "sf_s2m0";
      if( uncType == "syst_altTag" ) histName = "sf_s3m0";
      if( uncType == "syst_altMC" ) histName = "sf_s4m0";
      if( uncType == "syst_altSub" ) histName = "sf_s5m0";
      if( uncType == "syst_altGenMat" ) histName = "sf_s6m0";
    }

    return histName;
  }

  void Init_Mu() {
    if( effType_ == "DTRIG" && dataType_ != "sf" ) {
      cout << "dataType = " << dataType_ << " is not available for the double muon trigger!" << endl;
      return;
    }

    TString histName_cv = Get_HistName_Mu();
    TH2D* h2D_cv = PlotTool::Get_Hist2D(fileName_, histName_cv);
    map_effHist_.insert( std::make_pair("cv", h2D_cv) );

    if( effType_ == "DTRIG" ) {
      // -- only SF is available
      TString histName_sf_absUnc_stat = Get_HistName_Mu("stat");
      TH2D* h2D_absUnc_stat = PlotTool::Get_Hist2D(fileName_, histName_sf_absUnc_stat);
      Produce_EffMap_StatVariation(h2D_cv, h2D_absUnc_stat);

      TString histName_sf_absUnc_syst = Get_HistName_Mu("syst");
      TH2D* h2D_absUnc_syst = PlotTool::Get_Hist2D(fileName_, histName_sf_absUnc_syst);
      Produce_EffMap_SystVariation(h2D_cv, h2D_absUnc_syst, "syst_all");
    }
    else { // -- ID, ISO, STRIG case

      // -- central value +- stat uncertainty
      TString histName_eff_wStatUnc = Get_HistName_Mu("stat");
      TH2D* h2D_eff_wStatUnc = PlotTool::Get_Hist2D(fileName_, histName_eff_wStatUnc);
      TH2D* h2D_absUnc_stat = Get_UncHist2D(h2D_eff_wStatUnc);
      Produce_EffMap_StatVariation(h2D_cv, h2D_absUnc_stat);

      // -- central value +- syst uncertainty
      TString histName_eff_wSystUnc = Get_HistName_Mu("syst");
      TH2D* h2D_eff_wSystUnc = PlotTool::Get_Hist2D(fileName_, histName_eff_wSystUnc);
      TH2D* h2D_absUnc_syst = Get_UncHist2D(h2D_eff_wSystUnc);
      Produce_EffMap_SystVariation(h2D_cv, h2D_absUnc_syst, "syst_all");

      // if( effType_ == "STRIG" ) {
      //   ProducePlot_ConvertedTo1D(h2D_cv, "cv");
      // }
    }

  }

  TString Get_HistName_Mu(TString uncType = "cv") {
    TString histName = "";
    if( effType_ == "ID" ) {
      if( dataType_ == "data" ) histName = "NUM_TightID_DEN_TrackerMuons_abseta_pt_efficiencyData";
      if( dataType_ == "mc" )   histName = "NUM_TightID_DEN_TrackerMuons_abseta_pt_efficiencyMC";

      if( uncType == "stat" ) histName = histName+"_stat";
      if( uncType == "syst" ) histName = histName+"_syst";
    }
    else if( effType_ == "ISO" ) {
      if( dataType_ == "data" ) histName = "NUM_TightRelIso_DEN_TightIDandIPCut_abseta_pt_efficiencyData";
      if( dataType_ == "mc" )   histName = "NUM_TightRelIso_DEN_TightIDandIPCut_abseta_pt_efficiencyMC";

      if( uncType == "stat" ) histName = histName+"_stat";
      if( uncType == "syst" ) histName = histName+"_syst";
    }
    else if( effType_ == "STRIG" ) {
      if (dataType_ == "data" ) histName = "NUM_IsoMu24_DEN_CutBasedIdTight_and_PFIsoTight_abseta_pt_efficiencyData";
      if (dataType_ == "mc" )   histName = "NUM_IsoMu24_DEN_CutBasedIdTight_and_PFIsoTight_abseta_pt_efficiencyMC";

      if( era_ == "16pre" || era_ == "16post" ) histName.ReplaceAll("IsoMu24", "IsoMu24_or_IsoTkMu24");
      if( era_ == "17" ) histName.ReplaceAll("IsoMu24", "IsoMu27");

      if( uncType == "stat" ) histName = histName+"_stat";
      if( uncType == "syst" ) histName = histName+"_syst";
    }
    else if( effType_ == "DTRIG" ) { // no data or MC efficiency available; only SF
      TString tag_era = "";
      if( era_ == "16pre" ) tag_era = "UL2016_HIPM";
      if( era_ == "16post" ) tag_era = "UL2016";
      if( era_ == "17" ) tag_era = "UL2017";
      if( era_ == "18" ) tag_era = "UL2018";

      histName = "ScaleFactorTight_"+tag_era;
      if( uncType == "stat" ) histName = histName+"_stat";
      if( uncType == "syst" ) histName = histName+"_syst";
    }

    return histName;
  }

  TH2D* Get_UncHist2D(TH2D* h2D_eff_wUnc) {

    TH2D* h2D_absUnc = (TH2D*)h2D_eff_wUnc->Clone();

    Int_t nBinX = h2D_eff_wUnc->GetNbinsX();
    Int_t nBinY = h2D_eff_wUnc->GetNbinsY();

    for(Int_t i_x=0; i_x<nBinX; ++i_x) {
      Int_t i_binX = i_x+1;

      for(Int_t i_y=0; i_y<nBinY; ++i_y) {
        Int_t i_binY = i_y+1;

        Double_t error = h2D_eff_wUnc->GetBinError(i_binX, i_binY);

        h2D_absUnc->SetBinContent(i_binX, i_binY, error);
        h2D_absUnc->SetBinError(i_binX, i_binY, 0);
      }
    }

    return h2D_absUnc;
  }

  void Produce_EffMap_StatVariation(TH2D* h2D_eff_cv, TH2D* h2D_absUnc_stat, TString uncType = "stat") {
    TRandom3 eran;
    eran.SetSeed(0);

    for(Int_t i_map=0; i_map<nStatMap; ++i_map) {

      TH2D* h2D_eff_statVar = (TH2D*)h2D_eff_cv->Clone();

      Int_t nBinX = h2D_eff_cv->GetNbinsX();
      Int_t nBinY = h2D_eff_cv->GetNbinsY();

      for(Int_t i_x=0; i_x<nBinX; ++i_x) {
        Int_t i_binX = i_x+1;

        for(Int_t i_y=0; i_y<nBinY; ++i_y) {
          Int_t i_binY = i_y+1;

          Double_t eff         = h2D_eff_cv->GetBinContent(i_binX, i_binY);
          Double_t absUnc_stat = h2D_absUnc_stat->GetBinContent(i_binX, i_binY);

          // each bin: independently varied (property of the stat. unc.)
          Double_t eff_statVar = eff + eran.Gaus(0.0, 1.0)*absUnc_stat;

          h2D_eff_statVar->SetBinContent(i_binX, i_binY, eff_statVar);
          h2D_eff_statVar->SetBinError(i_binX, i_binY, 0); // -- error is set to 0 (not used)
        } // -- iteration on y
      } // -- iteration on x

      TString histType = TString::Format("%s_%03d", uncType.Data(), i_map);
      map_effHist_.insert(std::make_pair(histType, h2D_eff_statVar) );
    } // -- iteration on i_map
    map_effHist_.insert( std::make_pair("absUnc_"+uncType, h2D_absUnc_stat) ); // -- also save the uncertainty map
  }

  void Produce_EffMap_SystVariation(TH2D* h2D_eff_cv, TH2D* h2D_absUnc_syst, TString uncType) {

    TH2D* h2D_eff_systVar_plus  = (TH2D*)h2D_eff_cv->Clone();
    TH2D* h2D_eff_systVar_minus = (TH2D*)h2D_eff_cv->Clone();

    Int_t nBinX = h2D_eff_cv->GetNbinsX();
    Int_t nBinY = h2D_eff_cv->GetNbinsY();

    for(Int_t i_x=0; i_x<nBinX; ++i_x) {
      Int_t i_binX = i_x+1;

      for(Int_t i_y=0; i_y<nBinY; ++i_y) {
        Int_t i_binY = i_y+1;

        Double_t eff         = h2D_eff_cv->GetBinContent(i_binX, i_binY);
        Double_t absUnc_syst = h2D_absUnc_syst->GetBinContent(i_binX, i_binY);

        Double_t eff_systVar_plus  = eff + absUnc_syst;
        h2D_eff_systVar_plus->SetBinContent(i_binX, i_binY, eff_systVar_plus);
        h2D_eff_systVar_plus->SetBinError(i_binX, i_binY, 0); // -- error is set to 0 (not used)

        Double_t eff_systVar_minus = eff - absUnc_syst;
        h2D_eff_systVar_minus->SetBinContent(i_binX, i_binY, eff_systVar_minus);
        h2D_eff_systVar_minus->SetBinError(i_binX, i_binY, 0); // -- error is set to 0 (not used)
      } // -- iteration on y
    } // -- iteration on x

    TString histType = uncType;
    map_effHist_.insert( std::make_pair(histType+"_plus",  h2D_eff_systVar_plus) );
    map_effHist_.insert( std::make_pair(histType+"_minus", h2D_eff_systVar_minus) );
    map_effHist_.insert( std::make_pair("absUnc_"+histType, h2D_absUnc_syst) ); // -- also save the uncertainty map
  }

  void ProducePlot2D(TH2D* h2D, TString tag) {

    TString canvasName = TString::Format("%s_%s_%s_%s_%s", dataType_.Data(), era_.Data(), lepType_.Data(), effType_.Data(), tag.Data());
    Check_NegativeBin(h2D, canvasName);

    cout << "Maximum = " << h2D->GetMaximum() << endl;

    PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas(canvasName, 0, 1, 0);
    canvas->SetTitle("#eta(l)", "p_{T}(l)");

    canvas->Register(h2D);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeZ(0, 0.1);
    canvas->SetRangeZ(0, h2D->GetMaximum()*1.01);
    // canvas->SetAutoRangeZ();

    canvas->Latex_CMSInternal();
    // canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Random Gaussian values}}");

    // canvas->SetSavePath("./output"); // -- path to be saved (default: "./")

    canvas->Draw();
    // canvas->Draw("COLZTEXT");
  }

  void Check_NegativeBin(TH2D* h2D, TString tag) {
    Bool_t isFound = kFALSE;

    cout << "[Check_NegativeBin] name = " << tag << endl;
    Int_t nBinX = h2D->GetNbinsX();
    Int_t nBinY = h2D->GetNbinsY();

    for(Int_t i_x=0; i_x<nBinX; ++i_x) {
      Int_t i_binX = i_x+1;

      for(Int_t i_y=0; i_y<nBinY; ++i_y) {
        Int_t i_binY = i_y+1;

        Double_t value = h2D->GetBinContent(i_binX, i_binY);
        if( value < 0 ) {
          printf("  (%02d, %02d) value = %lf < 0!\n", i_binX, i_binY, value);
          isFound = kTRUE;
        }
      }
    }

    if( isFound )
      cout << "--> negative bin is found" << endl;
    else
      cout << "--> *NO* negative bin is found" << endl;
  }

  void ProducePlot_ConvertedTo1D(TH2D* h2D, TString tag) {
    TString canvasName = TString::Format("%s_%s_%s_%s_%s", dataType_.Data(), era_.Data(), lepType_.Data(), effType_.Data(), tag.Data());
    Check_NegativeBin(h2D, canvasName);

    TH1D* h = Convert_2Dto1D(h2D);

    PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas(canvasName, 0, 0);
    TString titleX = TString::Format("p_{T}-#eta bin number (%d p_{T} #times %d #eta bins)", h2D->GetNbinsY(), h2D->GetNbinsX());
    canvas->SetTitle(titleX, "Value");

    canvas->Register(h, "", kBlack);

    canvas->SetLegendPosition(0, 0, 0, 0);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    canvas->SetAutoRangeY();

    canvas->Latex_CMSInternal();
    // canvas->RegisterLatex(0.16, 0.91, 42, 0.6, "Gaussian distributions"); // arguments: x, y, font type, font size, text
    // canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Gaussian distributions}}"); // same with above

    // canvas->SetSavePath("./output"); // -- path to be saved (default: "./")

    canvas->Draw(); // -- default: .pdf format
  }

  TH1D* Convert_2Dto1D(TH2D* h2D) {

    Int_t nBinX = h2D->GetNbinsX();
    Int_t nBinY = h2D->GetNbinsY();

    Int_t nBin_tot = Int_t(nBinX * nBinY);

    TString histName = h2D->GetName();
    histName = histName + "_conv_1D";
    TH1D* h = new TH1D(histName, "", nBin_tot, 0, nBin_tot);

    Int_t i_bin1D = 1;
    for(Int_t i_y=0; i_y<nBinY; ++i_y) {
      Int_t i_binY = i_y+1;

      for(Int_t i_x=0; i_x<nBinX; ++i_x) {
        Int_t i_binX = i_x+1;

        Double_t value = h2D->GetBinContent(i_binX, i_binY);

        h->SetBinContent(i_bin1D, value);
        h->SetBinError(i_bin1D, 0); // -- meaningless
        // printf("(%02d, %02d) -> 1D %03d: value = %lf\n", i_binX, i_binY, i_bin1D, value);

        i_bin1D++;
      }
    }

    return h;
  }
};

void generate_systEffMap() {
  TFile *f_output = TFile::Open("LeptonEffMap_FullRun2.root", "RECREATE");

  vector<TString> vec_era = {"16pre", "16post", "17", "18"};
  vector<TString> vec_effType_mu = {"ID", "ISO", "STRIG", "DTRIG"};
  vector<TString> vec_effType_el = {"RECO", "ID", "TRIG_Leg1", "TRIG_Leg2"};

  for(auto era : vec_era ) {

    // -- muon
    for(auto effType_mu : vec_effType_mu ) {
      if( effType_mu == "DTRIG" ) { // -- only SF is available
        EffMap* effMap_mu = new EffMap(era, "mu", effType_mu, "sf");
        effMap_mu->Save(f_output);
      }
      else { // -- data and mc efficiencies are available
        EffMap* effMap_mu_data = new EffMap(era, "mu", effType_mu, "data");
        effMap_mu_data->Save(f_output);

        EffMap* effMap_mu_mc = new EffMap(era, "mu", effType_mu, "mc");
        effMap_mu_mc->Save(f_output);
      }
    }

    // -- electron
    for(auto effType_el : vec_effType_el ) {
      EffMap* effMap_el = new EffMap(era, "el", effType_el, "sf");
      effMap_el->Save(f_output);
    }

  }

  f_output->Close();
}
