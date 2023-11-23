// -- general tool to compare two outputs from shears
// -- would be useful to check what is changed after an update (e.g. bug-fix)
#pragma once
#include "DYOutput.h"

namespace {
  void RemoveNegativeBins(TH1D* h, TString tag) {
    Bool_t isFound = kFALSE;

    // -- Remove negative bins
    for(Int_t i=0; i<h->GetNbinsX(); ++i) {
      Double_t content = h->GetBinContent(i+1);

      if( content < 0 ) {
        isFound = kTRUE;
        h->SetBinContent(i+1, 0);
        h->SetBinError(i+1, 0);
      }
    }

    if( isFound )
      cout << "*** Negative bins are found in " << tag << " ... negative bins are set to 0 ***" << endl;
  }
};

// -- compare two cases under same era
class EraComparator {

public:
  EraComparator(TString era, TString channel):
  era_(era), channel_(channel) { }

  void Set_Case(TString basePath, TString info) {
    EraCase eraCase( vec_eraCase_.size(), new EraOutput(era_, basePath), info );
    vec_eraCase_.push_back( eraCase );
  }

  void Set_Fake( vector<TString> vec_fileName_fake, vector<TString> vec_histName_fake ) {
    auto nCase = vec_eraCase_.size();

    if( nCase != vec_fileName_fake.size() || nCase != vec_histName_fake.size() )
      throw std::invalid_argument("[Set_Fake] nCase != vec_fileName_fake.size() or nCase != vec_histName_fake.size()");

    for(Int_t i=0; i<nCase; ++i) {
      TString fileName_fake = vec_fileName_fake[i];
      TString histName_fake = vec_histName_fake[i];
      vec_eraCase_[i].Output()->Set_Fake( fileName_fake, histName_fake );
    }
  }

  void OnlyMC(Bool_t flag = kTRUE) { onlyMC_ = flag; }

  void Expect_PerfectAgreement(Bool_t flag = kTRUE) { expect_same_ = flag; }

  // -- if the histogram name for the comparison is same for all cases
  void Add_Comparison(TString histName) { 
    for( auto& eraCase : vec_eraCase_ )
      eraCase.Add_HistName(histName);
    nComp_++;
  };

  // -- if the histogram name for the comparison is different for each case
  void Add_Comparison(vector<TString> vec_histName ) {
    auto nCase = vec_eraCase_.size();
    if( nCase != vec_histName.size() )
      throw std::invalid_argument("[Add_HistName] nCase != vec_histName.size()");

    for(Int_t i=0; i<nCase; ++i)
      vec_eraCase_[i].Add_HistName( vec_histName[i] );

    nComp_++;
  }

  void Compare() {
    TH1::SetDefaultSumw2(); // -- to suppress sumw2 warnings

    nCase_ = (Int_t)vec_eraCase_.size();

    if( nCase_ > vec_colorForCase_.size() )
      throw std::runtime_error("nCase_ > vec_colorForCase_.size()");

    Make_PlotDir();
    for(Int_t i_comp=0; i_comp<nComp_; ++i_comp)
      ProducePlots(i_comp);
  }

  class EraCase {
  public:
    EraCase() {}
    EraCase(Int_t index, EraOutput* output, TString info):
    index_(index), output_(output), info_(info) {}

    void Add_HistName(TString histName) { vec_histName_.push_back(histName); }

    TString HistName(Int_t index) const { return vec_histName_[index]; }

    EraOutput* Output() const { return output_; }

    TString Info() const { return info_; }

    Int_t Index() const { return index_; }

  private:
    // -- i-th case in the comparison
    Int_t index_;
    EraOutput* output_;
    // -- legend
    TString info_;
    // -- histNames to compare
    vector<TString> vec_histName_;
  };

private:
  TString era_;
  TString channel_;
  TString plotDirPath_;

  vector<EraCase> vec_eraCase_;
  // -- color for each case in the plots
  vector<Int_t> vec_colorForCase_ = {
    kBlack, kBlue, kGreen+2, kViolet, kRed, kCyan, kGray
  };

  // -- if the provided cases are expected to be same between cases
  // -- enable additional test whether the ratio case2/case1 == exact 1?
  Bool_t expect_same_ = kFALSE;

  // -- only compare MC histograms
  Bool_t onlyMC_ = kFALSE;

  // -- # comparisons
  Int_t nComp_ = 0;

  // -- # cases (set when "Compare()" is executed)
  Int_t nCase_ = 0;

  void ProducePlots(Int_t i_comp) {
    TString histName_first = vec_eraCase_[0].HistName(i_comp);
    // -- generator level histogram: only DY MC is interesting
    if( histName_first.Contains("-gen") ) {
      ProducePlot_CompBtwCases(i_comp, "DY");
      return;
    }

    ProducePlot_CompBtwCases(i_comp, "DY");
    ProducePlot_CompBtwCases(i_comp, "bkgMC");
    if( !onlyMC_ ) ProducePlot_CompBtwCases(i_comp, "data");

    if( histName_first.Contains("mass_wide_range_inc0jet") ) {
      // -- check all cases have fakes
      Bool_t hasFake_all = kTRUE;
      for( const auto& eraCase : vec_eraCase_ ) {
        if( !(eraCase.Output()->HasFake()) ) {
          hasFake_all = kFALSE;
          break;
        }
      }
      if( hasFake_all )
        ProducePlot_CompBtwCases(i_comp, "bkgAll");
    }

    // -- data vs MC comparison for each case
    if( !onlyMC_ ) {
      for(const auto& eraCase : vec_eraCase_ )
        ProducePlot_DatavsMCStack(i_comp, eraCase);      
    }
  }

  void ProducePlot_CompBtwCases(Int_t i_comp, TString process) {
    vector<TH1D*> vec_hist;
    for(Int_t i_case=0; i_case<nCase_; ++i_case) {
      TString histName = vec_eraCase_[i_case].HistName(i_comp);
      TH1D* h_case = vec_eraCase_[i_case].Output()->Get(histName, process);
      RemoveNegativeBins(h_case, process + ", i_comp = " + TString::Format("%d", i_comp));

      vec_hist.push_back( h_case );
    }

    // -- used to get canvasName, isLogX, titleX, etc...
    TString histName_first = vec_hist[0]->GetName();

    TString canvasName = TString::Format("c%02d_%s_comp_%s", i_comp, histName_first.Data(), process.Data());
    PlotTool::HistCanvaswRatio* canvas = 
      new PlotTool::HistCanvaswRatio(canvasName, Get_isLogX(histName_first), kTRUE);
    
    canvas->SetTitle( Get_TitleX(histName_first), Get_TitleY(vec_hist[0]), "ratio over black" );

    for(Int_t i_case=0; i_case<nCase_; ++i_case) {
      TH1D* h_case = vec_hist[i_case];
      TString info_case = vec_eraCase_[i_case].Info();
      Int_t color_case  = vec_colorForCase_[i_case];

      canvas->Register(h_case, info_case, color_case);
    }

    canvas->SetLegendPosition(0.70, 0.82, 0.94, 0.95);
    for(const auto& eraCase : vec_eraCase_ ) {
      if( eraCase.Info().Length() > 8 )
        canvas->SetLegendPosition(0.50, 0.82, 0.94, 0.95);
    }      

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeRatio(minRatio, maxRatio);
    canvas->SetAutoRangeY();
    canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    if( !histName_first.Contains("-gen") )
      canvas->Latex_LumiEnergy(vec_eraCase_[0].Output()->Lumi()/1000.0, 13);

    TString channelInfo = "";
    if( channel_ == "mm" ) channelInfo = "Muon channel";
    if( channel_ == "ee" ) channelInfo = "Electron channel";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.7, channelInfo);

    // canvas->RemoveRatioError(); // remove error in the ratio (useful when the error is meaningless)

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();

    // -- if the ratio range is too large to see details
    if( (canvas->AxisRangeRatio()) > 0.35 ) {
      TString canvasName_zoomIn = canvasName;
      canvasName_zoomIn.ReplaceAll("comp", "compZoomIn");
      canvas->SetCanvasName(canvasName_zoomIn);
      canvas->SetAutoRangeRatio(kFALSE);
      canvas->SetRangeRatio(0.83, 1.17);
      canvas->Draw();
    }

    if( expect_same_ ) {
      for(Int_t i_case=1; i_case<nCase_; ++i_case) {
        cout << "Comparison: (case1, case2) = (0, " << i_case << ")" << endl;
        PlotTool::IsRatio1( vec_hist[0], vec_hist[i_case] );
      }
    }
  }

  void ProducePlot_DatavsMCStack(Int_t i_comp, const EraCase& eraCase) {
    TString type = TString::Format("case%02d", eraCase.Index());
    TString histName = eraCase.HistName(i_comp);

    TString canvasName = TString::Format("c%02d_%s_stack_%s", i_comp, histName.Data(), type.Data());
    PlotTool::HistStackCanvaswRatio* canvas = 
      new PlotTool::HistStackCanvaswRatio(canvasName, Get_isLogX(histName), 1);

    canvas->Ratio_Reversed();

    TH1D* h_data = eraCase.Output()->Get(histName, "data");
    canvas->SetTitle( Get_TitleX(histName), Get_TitleY(h_data), "MC/data" );

    canvas->RegisterData(h_data, "Data", kBlack);

    // -- order of stacks (from bottom to top)
    vector<TString> vec_process = {
      "fake", "GG", "VV", "TauTau", "singleTop", "TT", "DY"};

    for(auto& process : vec_process) {
      if( process == "fake" ) {
        if( !eraCase.Output()->HasFake() ) continue;
        if( !histName.Contains("mass_wide_range_inc0jet") ) continue;
      }

      TH1D* h_process = eraCase.Output()->Get(histName, process);
      RemoveNegativeBins(h_process, process);
      canvas->Register(h_process, Get_Legend(process), Get_Color(process));
    }

    canvas->SetAutoRangeY();
    // canvas->SetRangeY(0.5, 5e9);
    canvas->SetAutoRangeRatio();

    canvas->SetLegendColumn(2);
    canvas->SetLegendPosition(0.45, 0.80, 0.95, 0.95);

    canvas->Latex_CMSInternal();
    canvas->Latex_LumiEnergy(eraCase.Output()->Lumi()/1000.0, 13);

    TString channelInfo = "";
    if( channel_ == "mm" ) channelInfo = "Muon channel";
    if( channel_ == "ee" ) channelInfo = "Electron channel";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.7, channelInfo);

    TString info = eraCase.Info();
    canvas->RegisterLatex(0.16, 0.87, 42, 0.7, info);

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();

    // -- if the ratio range is too large to see details
    if( (canvas->AxisRangeRatio()) > 0.35 ) {
      TString canvasName_zoomIn = canvasName;
      canvasName_zoomIn.ReplaceAll("stack", "stackZoomIn");
      canvas->SetCanvasName(canvasName_zoomIn);
      canvas->SetAutoRangeRatio(kFALSE);
      canvas->SetRangeRatio(0.83, 1.17);
      canvas->Draw();
    }
  }

  Bool_t Get_isLogX(TString histName) {
    Bool_t isLogX = kFALSE;
    if( histName.Contains("mass_wide_range") ) isLogX = kTRUE;

    return isLogX;
  }

  TString Get_Legend(TString tag) {
    TString legend = "";
    if( tag == "TT" ) legend = "t#bar{t}";
    if( tag == "singleTop" ) legend = "single t";
    if( tag == "TauTau" ) legend = "Z/#gamma*#rightarrow#tau#tau";
    if( tag == "WJetsToLNu" ) legend = "W+jets";
    if( tag == "VV" ) legend = "VV";
    if( tag == "GG" ) legend = "#gamma#gamma#rightarrow ll";
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
    if( tag == "GG" ) color = kOrange;
    if( tag == "fake" ) color = kViolet-5;
    if( tag == "DY" ) color = kGreen-8;

    return color;
  }

  TString Get_TitleX(TString histName) {
    TString title = "undefined";
    if( histName == "mass_inc0jet" ) title = "m(ll) [GeV]";
    if( histName == "mass_inc0jet_BB" ) title = "m(ll) [GeV] (Barrel-Barrel)";
    if( histName == "mass_inc0jet_BE" ) title = "m(ll) [GeV] (Barrel-Endcap)";
    if( histName == "mass_inc0jet_EE" ) title = "m(ll) [GeV] (Endcap-Endcap)";
    if( histName == "mass_wide_range_inc0jet" ) title = "m(ll) [GeV]";
    if( histName == "mass_wide_range_inc0jet_BB" ) title = "m(ll) [GeV] (Barrel-Barrel)";
    if( histName == "mass_wide_range_inc0jet_BE" ) title = "m(ll) [GeV] (Barrel-Endcap)";
    if( histName == "mass_wide_range_inc0jet_EE" ) title = "m(ll) [GeV] (Endcap-Endcap)";
    if( histName == "mass_wide_range_inc0jet-gen" ) title = "m(ll) [GeV] (Gen-level)";
    if( histName == "nvtx_inc0jet") title = "# reco. vertices";

    if( histName == "pt_inc0jet" ) title = "p_{T}(ll) [GeV]";

    return title;
  }

  TString Get_TitleY(TH1D* h) {
    TString title = "";

    bool hasUniformBinSize = !(h->GetXaxis()->IsVariableBinSize());
    if( hasUniformBinSize ) {
      double binWidth = h->GetBinWidth(1);
      title = TString::Format("Entry / %.1lf", binWidth);

      TString histName = h->GetName();
      if( histName.Contains("mass_") || histName.Contains("pt_") )
        title = title + " GeV";
    }
    else
      title = "Entry per bin";

    return title;
  }

  void Make_PlotDir() {
    plotDirPath_ = TString::Format("./plot/%s", era_.Data());
    bool recursive = kTRUE;
    if( gSystem->mkdir(plotDirPath_.Data(), recursive) < 0 )
      throw std::runtime_error("Directory = " + plotDirPath_ + " cannot be created (already exists?)");
  }
};

// -- to  compare combined results also (e.g. unfolded results)
// class Run2Comparator {

// public:
//   Run2Comparator(TString channel): channel_(channel) {}

//   void Set_First(TString commonPath, TString info)  { Set("first", commonPath, info); }
//   void Set_Second(TString commonPath, TString info) { Set("second", commonPath, info); }

//   void Add_HistName(TString histName) { vec_histName_.push_back(histName); }

//   void Compare() {
//     vector<TString> vec_era = {"16pre", "16post", "17", "18"};
//     for(const auto& era : vec_era)
//       Compare_eachEra(era);

//     ProducePlot_Unfolded("DY");
//     ProducePlot_Unfolded("data");
//     ProducePlot_Unfolded_ClosureTest("first");
//     ProducePlot_Unfolded_ClosureTest("second");
//     ProducePlot_Unfolded_DatavsMC("first");
//     ProducePlot_Unfolded_DatavsMC("second");
//   }

// private:
//   TString channel_;
//   std::pair<Run2Output, Run2Output> pair_output_;
//   std::pair<DYRun2Result, DYRun2Result> pair_result_;
//   std::pair<TString, TString> pair_info_;

//   vector<TString> vec_histName_;

//   void Set(TString type, TString commonPath, TString info) {
//     if( type == "first" ) {
//       pair_output_.first = Run2Output(commonPath);
//       pair_info_.first = info;

//       pair_result_.first = DYRun2Result(&pair_output_.first);
//       pair_result_.first.Produce();
//     }
//     else if( type == "second" ) {
//       pair_output_.second = Run2Output(commonPath);
//       pair_info_.second = info;

//       pair_result_.second = DYRun2Result(&pair_output_.second);
//       pair_result_.second.Produce();
//     }
//   }

//   void Compare_eachEra(TString era) {
//     EraComparator comparator(era, channel_,
//                              pair_output_.first.Get_EraOutput(era),   pair_info_.first,
//                              pair_output_.second.Get_EraOutput(era),  pair_info_.second);

//     for(const auto& histName : vec_histName_ )
//       comparator.Add_HistName(histName);

//     comparator.Compare();
//   }

//   void ProducePlot_Unfolded_DatavsMC(TString type) {
//     DYRun2Result& result = (type == "first") ? pair_result_.first : pair_result_.second;
//     TH1D* h_data = result.Get_AllEra("unfolded", "data");
//     TH1D* h_MC   = result.Get_AllEra("unfolded", "DY");

//     TString canvasName = "unfolded_compDatavsMC_"+type;
//     PlotTool::HistCanvaswRatio* canvas = 
//       new PlotTool::HistCanvaswRatio(canvasName, 0, kTRUE);
    
//     canvas->SetTitle( "mass bin number", "Entry per bin", "MC/data" );

//     canvas->Register(h_data, "Data", kBlack);
//     canvas->Register(h_MC, "DY MC", kGreen-8);

//     canvas->SetLegendPosition(0.70, 0.82, 0.94, 0.95);

//     // canvas->SetRangeX(minX, maxX);
//     // canvas->SetRangeY(minY, maxY);
//     // canvas->SetRangeRatio(minRatio, maxRatio);
//     canvas->SetAutoRangeY();
//     canvas->SetAutoRangeRatio();

//     canvas->Latex_CMSInternal();
//     Double_t lumi_Run2 = LUMI_16pre + LUMI_16post + LUMI_17 + LUMI_18;
//     canvas->Latex_LumiEnergy(lumi_Run2/1000.0, 13);

//     TString channelInfo = "";
//     if( channel_ == "mm" ) channelInfo = "Muon channel";
//     if( channel_ == "ee" ) channelInfo = "Electron channel";
//     channelInfo = channelInfo + ", unfolded";
//     canvas->RegisterLatex(0.16, 0.91, 42, 0.7, channelInfo);
//     TString& info = (type == "first") ? pair_info_.first : pair_info_.second;
//     canvas->RegisterLatex(0.16, 0.87, 42, 0.7, info);

//     // canvas->RemoveRatioError(); // remove error in the ratio (useful when the error is meaningless)

//     canvas->SetSavePath("./plot");

//     canvas->Draw();

//     // -- if the ratio range is too large to see details
//     if( (canvas->AxisRangeRatio()) > 0.35 ) {
//       TString canvasName_zoomIn = canvasName;
//       canvasName_zoomIn.ReplaceAll("compDatavsMC", "compDatavsMCZoomIn");
//       canvas->SetCanvasName(canvasName_zoomIn);
//       canvas->SetAutoRangeRatio(kFALSE);
//       canvas->SetRangeRatio(0.83, 1.17);
//       canvas->Draw();
//     }
//   }

//   void ProducePlot_Unfolded_ClosureTest(TString type) {
//     DYRun2Result& result = (type == "first") ? pair_result_.first : pair_result_.second;
//     TH1D* h_gen_DY      = result.Get_AllEra("gen", "DY");
//     TH1D* h_unfolded_DY = result.Get_AllEra("unfolded", "DY");

//     TString canvasName = "unfolded_closureTest_"+type;
//     PlotTool::HistCanvaswRatio* canvas = 
//       new PlotTool::HistCanvaswRatio(canvasName, 0, kTRUE);
    
//     canvas->SetTitle( "mass bin number", "Entry per bin", "unfolded/Gen" );

//     canvas->Register(h_gen_DY, "Generator level (dressed)", kBlack);
//     canvas->Register(h_unfolded_DY, "Unfolded distribution", kBlue);

//     canvas->SetLegendPosition(0.50, 0.75, 0.94, 0.87);

//     // canvas->SetRangeX(minX, maxX);
//     // canvas->SetRangeY(minY, maxY);
//     // canvas->SetRangeRatio(minRatio, maxRatio);
//     canvas->SetAutoRangeY();
//     canvas->SetAutoRangeRatio();

//     canvas->Latex_CMSInternal();

//     TString channelInfo = "";
//     if( channel_ == "mm" ) channelInfo = "Muon channel";
//     if( channel_ == "ee" ) channelInfo = "Electron channel";
//     channelInfo = channelInfo + ", unfolding closure test";
//     canvas->RegisterLatex(0.16, 0.91, 42, 0.7, channelInfo);
//     TString& info = (type == "first") ? pair_info_.first : pair_info_.second;
//     canvas->RegisterLatex(0.16, 0.87, 42, 0.7, info);

//     // canvas->RemoveRatioError(); // remove error in the ratio (useful when the error is meaningless)

//     canvas->SetSavePath("./plot");

//     canvas->Draw();

//     // -- if the ratio range is too large to see details
//     if( (canvas->AxisRangeRatio()) > 0.35 ) {
//       TString canvasName_zoomIn = canvasName;
//       canvasName_zoomIn.ReplaceAll("compDatavsMC", "compDatavsMCZoomIn");
//       canvas->SetCanvasName(canvasName_zoomIn);
//       canvas->SetAutoRangeRatio(kFALSE);
//       canvas->SetRangeRatio(0.83, 1.17);
//       canvas->Draw();
//     }
//   }


//   void ProducePlot_Unfolded(TString process) {
//     TH1D* h_first  = pair_result_.first.Get_AllEra("unfolded", process);
//     TH1D* h_second = pair_result_.second.Get_AllEra("unfolded", process);

//     TString canvasName = "unfolded_comp_"+process;
//     PlotTool::HistCanvaswRatio* canvas = 
//       new PlotTool::HistCanvaswRatio(canvasName, 0, kTRUE);
    
//     canvas->SetTitle( "mass bin number", "Entry per bin", "blue/black" );

//     canvas->Register(h_first, pair_info_.first, kBlack);
//     canvas->Register(h_second, pair_info_.second, kBlue);

//     canvas->SetLegendPosition(0.70, 0.82, 0.94, 0.95);
//     if( pair_info_.first.Length() > 8 )
//       canvas->SetLegendPosition(0.50, 0.82, 0.94, 0.95);

//     // canvas->SetRangeX(minX, maxX);
//     // canvas->SetRangeY(minY, maxY);
//     // canvas->SetRangeRatio(minRatio, maxRatio);
//     canvas->SetAutoRangeY();
//     canvas->SetAutoRangeRatio();

//     canvas->Latex_CMSInternal();
//     if( process == "data" ) {
//       Double_t lumi_Run2 = LUMI_16pre + LUMI_16post + LUMI_17 + LUMI_18;
//       canvas->Latex_LumiEnergy(lumi_Run2/1000.0, 13);
//     }

//     TString channelInfo = "";
//     if( channel_ == "mm" ) channelInfo = "Muon channel";
//     if( channel_ == "ee" ) channelInfo = "Electron channel";
//     canvas->RegisterLatex(0.16, 0.91, 42, 0.7, channelInfo);
//     if( process == "data" ) canvas->RegisterLatex(0.16, 0.87, 42, 0.7, "data");
//     if( process == "DY" )   canvas->RegisterLatex(0.16, 0.87, 42, 0.7, "DY MC");

//     canvas->RemoveRatioError(); // remove error in the ratio (useful when the error is meaningless)

//     canvas->SetSavePath("./plot");

//     canvas->Draw();

//     // -- if the ratio range is too large to see details
//     if( (canvas->AxisRangeRatio()) > 0.35 ) {
//       TString canvasName_zoomIn = canvasName;
//       canvasName_zoomIn.ReplaceAll("comp", "compZoomIn");
//       canvas->SetCanvasName(canvasName_zoomIn);
//       canvas->SetAutoRangeRatio(kFALSE);
//       canvas->SetRangeRatio(0.83, 1.17);
//       canvas->Draw();
//     }

//   }

// };

// -- Compare all histograms in DYRun2Result class
// -- mostly "allEra" histograms (unrolled histograms & unfolded histograms)
class ResultComparator {
public: 
  ResultComparator(TString channel): channel_(channel) {}

  void Set_Case(DYRun2Result* result, TString info) {
    vec_resultCase_.push_back( ResultCase(vec_resultCase_.size(), result, info) );
  }

  void Remove_RatioError(Bool_t flag = kTRUE) { removeRatioErr_ = flag; }

  void Expect_PerfectAgreement(Bool_t flag = kTRUE) { expect_same_ = flag; }

  void Compare(TString subDir = "") {
    TH1::SetDefaultSumw2(); // -- to suppress Sumw2 warnings

    Make_PlotDir(subDir);

    // -- TO-DO: add fake
    ProducePlot_CompBtwCases("gen", "DY");

    ProducePlot_CompBtwCases("reco", "data");
    ProducePlot_CompBtwCases("reco", "DY");
    ProducePlot_CompBtwCases("reco", "bkgMC");
    ProducePlot_CompBtwCases("reco", "data_bkgSub");

    ProducePlot_CompBtwCases("unfolded", "DY");
    ProducePlot_CompBtwCases("unfolded", "data");

    for(const auto& resultCase : vec_resultCase_ )
      ProducePlot2D(resultCase.Result()->Get_AllEra_RespM());
  }

private:
  class ResultCase {
  public:
    ResultCase() {}
    ResultCase(Int_t index, DYRun2Result* result, TString info):
    index_(index), result_(result), info_(info) {}

    DYRun2Result* Result() const { return result_; }

    TString Info() const { return info_; }

    Int_t Index() const { return index_; }

  private:
    // -- i-th case in the comparison
    Int_t index_;
    DYRun2Result* result_;
    // -- legend
    TString info_;
  };

  TString channel_;
  vector<ResultCase> vec_resultCase_;
  Int_t i_canvas = 0;

  TString plotDirPath_ = "";

  Bool_t expect_same_ = kFALSE;
  Bool_t removeRatioErr_ = kFALSE;

  // -- color for each case in the plots
  vector<Int_t> vec_colorForCase_ = {
    kBlack, kBlue, kGreen+2, kViolet, kRed, kCyan, kGray
  };

  void ProducePlot_CompBtwCases(TString level, TString process) {
    TString canvasName = TString::Format("c%02d_comp_%s_%s", i_canvas, level.Data(), process.Data());
    i_canvas++;

    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 0, kTRUE);

    TString titleX = "undefined";  
    if( level == "reco" )                       titleX = "(era, mass) bin number";
    if( level == "gen" || level == "unfolded" ) titleX = "mass bin number";
    canvas->SetTitle( titleX, "Entry per bin", "ratio over black" );

    vector<TH1D*> vec_hist; // -- to check IsRatio1 later
    for(const auto& resultCase : vec_resultCase_ ) {
      Int_t i_case = resultCase.Index();
      TH1D* h_case = resultCase.Result()->Get_AllEra(level, process);
      TString info_case = resultCase.Info();
      Int_t color_case = vec_colorForCase_[i_case];

      RemoveNegativeBins(h_case, TString::Format("%s_%s (i_case = %d)", level.Data(), process.Data(), i_case));

      canvas->Register(h_case, info_case, color_case);
      vec_hist.push_back( h_case );
    }

    canvas->SetLegendPosition(0.70, 0.82, 0.94, 0.95);
    for(const auto& resultCase : vec_resultCase_ ) {
      if( resultCase.Info().Length() > 8 )
        canvas->SetLegendPosition(0.50, 0.82, 0.94, 0.95);
    }

    canvas->SetAutoRangeY();
    canvas->SetAutoRangeRatio();

    if( removeRatioErr_ ) canvas->RemoveRatioError();

    canvas->Latex_CMSInternal();
    if( process.Contains("data") ) {
      Double_t lumi_Run2 = LUMI_16pre + LUMI_16post + LUMI_17 + LUMI_18;
      canvas->Latex_LumiEnergy(lumi_Run2/1000.0, 13);
    }

    TString channelInfo = "";
    if( channel_ == "mm" ) channelInfo = "Muon channel";
    if( channel_ == "ee" ) channelInfo = "Electron channel";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.7, channelInfo);

    canvas->RegisterLatex(0.16, 0.87, 42, 0.7, Get_HistInfo(level, process));;

    canvas->SetSavePath(plotDirPath_);

    canvas->Draw();

    // -- if the ratio range is too large to see details
    if( (canvas->AxisRangeRatio()) > 0.22 ) {
      TString canvasName_zoomIn = canvasName;
      canvasName_zoomIn.ReplaceAll("comp", "compZoomIn");
      canvas->SetCanvasName(canvasName_zoomIn);
      canvas->SetAutoRangeRatio(kFALSE);
      canvas->SetRangeRatio(0.89, 1.11);
      canvas->Draw();
    }

    if( expect_same_ ) {
      for(const auto& resultCase : vec_resultCase_ ) {
        Int_t i_case = resultCase.Index();
        if( i_case == 0 ) continue;

        cout << "Comparison: (case0, case" << i_case << ")" << endl;
        PlotTool::IsRatio1( vec_hist[0], vec_hist[i_case] );
      } // -- iteration over case
    } // -- if( expect_same_ )
  }

  TString Get_HistInfo(TString level, TString process) {
    TString histInfo = "";
    if( level == "gen" )      histInfo = "Gen-level";
    if( level == "reco" )     histInfo = "Reco-level";
    if( level == "unfolded" ) histInfo = "Unfolded";

    if( process == "data" )        histInfo += ", data";
    if( process == "data_bkgSub" ) histInfo += ", data (bkg. subtracted)";
    if( process == "DY" )    histInfo += ", DY MC";
    if( process == "bkgMC" ) histInfo += ", Sum of bkg. MC";

    return histInfo;
  }

  void Make_PlotDir(TString subDir) {
    plotDirPath_ = subDir == "" ? "./plot" : "./plot/"+subDir;
    bool recursive = kTRUE;
    if( gSystem->mkdir(plotDirPath_.Data(), recursive) < 0 )
      throw std::runtime_error("Directory = " + plotDirPath_ + " cannot be created (already exists?)");
  }

  // -- TO-DO: write
  void ProducePlot2D(TH2D* h2D) {

  }

};