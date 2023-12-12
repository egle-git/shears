#include "DYAccTool.h"

class ValidationTool {
public:
  ValidationTool(TString inputPath): inputPath_(inputPath) {

  }

  void Set_ShearsPath(TString path_ee, TString path_mm) { 
    shearsPath_ee_ = path_ee;
    shearsPath_mm_ = path_mm; 
  }

  void Validate() {
    TH1::AddDirectory(kFALSE);
    Make_BasePlotDir();

    Comparison_Between_Eras();
    Comparison_Between_Channels();    
    Comparison_Between_MassRange();
    Comparison_With_Shears();
  }

private:
  TString inputPath_ = "";
  TString plotBasePath_ = "./validation/";

  TString shearsPath_ee_ = "";
  TString shearsPath_mm_ = "";

  void Make_BasePlotDir() {
    bool recursive = kTRUE;
    if( gSystem->mkdir(plotBasePath_.Data(), recursive) < 0 )
      throw std::runtime_error("Directory = " + plotBasePath_ + " cannot be created (already exists?)");
  }

  void Comparison_With_Shears() {
    gSystem->mkdir(plotBasePath_+"/comp_shears", kTRUE);

    Comparison_With_Shears_PerChannel("ee");
    Comparison_With_Shears_PerChannel("mm");
  }

  void Comparison_With_Shears_PerChannel(TString channel) {
    TString shearsPath = (channel == "ee") ? shearsPath_ee_ : shearsPath_mm_;

    TH1D* h_shears = PlotTool::Get_Hist(shearsPath, "h_gen_DY");
    h_shears = DYTool::Convert_TUnfoldOutput_MassAxis(h_shears);
    // h_shears->Scale( 1000.0 / (LUMI_16pre + LUMI_16post + LUMI_17 + LUMI_18) ); // -- norm. to 1 /fb

    TH1D* h_miniAOD = DYAcc::Get_MergedHist_Channel(inputPath_, channel, kTRUE); // -- fiducial
    h_miniAOD = DYTool::Convert_TUnfoldOutput_MassAxis(h_miniAOD); // -- remove under(10-40) and over (3000-5000) GeV

    Int_t nBin_shears  = h_shears->GetNbinsX();
    Int_t nBin_miniAOD = h_miniAOD->GetNbinsX();

    if( nBin_shears != nBin_miniAOD ) {
      printf("[# bin] (h_shears, h_miniAOD) = (%d, %d)\n", nBin_shears, nBin_miniAOD);
      throw std::runtime_error("# bin for two histograms are different ... need to check");
    }

    // -- canvas
    TString canvasName = TString::Format("c_comp_shears_%s", channel.Data());
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 1, 1);
    canvas->SetTitle("m [GeV]", "# events (norm to Run2 lumi.)", "miniAOD/shears");

    canvas->Register(h_shears, "From Shears (nanoAOD)", kBlack);
    canvas->Register(h_miniAOD, "From EDAnalyzer (miniAOD)", kBlue);

    // canvas->FillHist(0.5); // -- fill the histograms with alpha=0.5

    // canvas->SetRebin(2); // -- rebin the histograms before drawing

    canvas->SetLegendPosition(0.50, 0.72, 0.95, 0.90);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeRatio(minRatio, maxRatio);
    canvas->SetAutoRangeY();
    canvas->SetAutoRangeRatio();
    // canvas->SetSimpleLogLabel(); // -- simple log label on x-axis
    // canvas->SetRatioLogY(); // -- SetLogY = true for the ratio pad
    
    canvas->Latex_CMSInternal();
    TString info = TString::Format("channel: %s", channel.Data());
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, info);
    canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Dressed level (Fiducial region: p_{T}>20(15) GeV, |#eta|<2.4)");

    // canvas->RemoveRatioError(); // -- remove error in the ratio (useful when the error is meaningless)

    canvas->SetSavePath(plotBasePath_+"/comp_shears"); // -- path to be saved (default: "./")

    canvas->Draw();
  }

  void Comparison_Between_MassRange() {
    gSystem->mkdir(plotBasePath_+"/comp_massRange", kTRUE);

    for(const auto& era : DYAcc::vec_era) {
      gSystem->mkdir(plotBasePath_+"/comp_massRange/"+era, kTRUE);

      Comparison_Between_MassRange_PerChannel("ee", era, kFALSE);
      Comparison_Between_MassRange_PerChannel("ee", era, kTRUE);

      Comparison_Between_MassRange_PerChannel("mm", era, kFALSE);
      Comparison_Between_MassRange_PerChannel("mm", era, kTRUE);
    }

    gSystem->mkdir(plotBasePath_+"/comp_massRange/all", kTRUE);

    Comparison_Between_MassRange_PerChannel("ee", "all", kFALSE);
    Comparison_Between_MassRange_PerChannel("ee", "all", kTRUE);

    Comparison_Between_MassRange_PerChannel("mm", "all", kFALSE);
    Comparison_Between_MassRange_PerChannel("mm", "all", kTRUE);
  }

  void Comparison_Between_MassRange_PerChannel(TString channel, TString era, Bool_t isFiducial) {
    vector<TString> vec_massRange = DYAcc::GetVector_MassRange();
    vector<TH1D*> vec_hist_massRange;
    for(const auto& massRange : vec_massRange) {
      TH1D* h_massRange = nullptr;
      if( era == "all" ) {
        vector<TH1D*> vec_hist_era;
        for(const auto& theEra : DYAcc::vec_era) {
          TH1D* h_era = DYAcc::Get_Hist_Norm_Rebinned(inputPath_, channel, massRange, theEra, isFiducial);
          vec_hist_era.push_back(h_era);
        }
        h_massRange = DYAcc::MergeHist( vec_hist_era );
      }
      else
        h_massRange = DYAcc::Get_Hist_Norm_Rebinned(inputPath_, channel, massRange, era, isFiducial);

      vec_hist_massRange.push_back( h_massRange );
    }

    TH1D* h_entireMass = DYAcc::MergeHist(vec_hist_massRange);

    TString canvasName = TString::Format("c_comp_massRange_%s_%s", era.Data(), channel.Data());
    if( isFiducial )
      canvasName = canvasName + "_fiducial";
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 1, 1);
    canvas->SetTitle("m [GeV]", "# events (norm to era lumi.)", "each/merged");

    canvas->Register(h_entireMass, "Merged distribution", kBlack);
    Int_t nSample = (Int_t)vec_massRange.size();
    for(Int_t i_sample=0; i_sample<nSample; ++i_sample) {
      TString massRange = vec_massRange[i_sample];
      TH1D* h_massRange = vec_hist_massRange[i_sample];
      Int_t color = DYAcc::color_massRange[i_sample];

      canvas->Register(h_massRange, massRange, color);
    }

    canvas->SetLegendPosition(0.65, 0.72, 0.95, 0.95);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeRatio(minRatio, maxRatio);
    // canvas->SetAutoRangeY();
    canvas->SetRangeY(1e-3, 5e9);
    // canvas->SetAutoRangeRatio();
    canvas->SetRangeRatio(0, 1.01);
    // canvas->SetSimpleLogLabel(); // -- simple log label on x-axis
    // canvas->SetRatioLogY(); // -- SetLogY = true for the ratio pad
    
    canvas->Latex_CMSInternal();
    TString info = TString::Format("channel: %s, era: %s", channel.Data(), era.Data());
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, info);
    if( !isFiducial) canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Dressed level (Full phase space)");
    else             canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Dressed level (Fiducial region: p_{T}>20(15) GeV, |#eta|<2.4)");

    // canvas->RemoveRatioError(); // -- remove error in the ratio (useful when the error is meaningless)

    canvas->SetSavePath(plotBasePath_+"/comp_massRange/"+era); // -- path to be saved (default: "./")

    canvas->Draw();
  }

  void Comparison_Between_Channels() {
    gSystem->mkdir(plotBasePath_+"/comp_channel", kTRUE);

    for(const auto& era : DYAcc::vec_era) {
      Comparison_Between_Channels_PerEra(era, kFALSE);
      Comparison_Between_Channels_PerEra(era, kTRUE);
    }
  }

  void Comparison_Between_Channels_PerEra(TString era, Bool_t isFiducial) {
    TH1D* h_ee = DYAcc::Get_MergedHist_AllMass(inputPath_, "ee", era, isFiducial);
    TH1D* h_mm = DYAcc::Get_MergedHist_AllMass(inputPath_, "mm", era, isFiducial);

    // -- canvas
    TString canvasName = TString::Format("c_comp_channel_%s", era.Data());
    if( isFiducial )
      canvasName = canvasName + "_fiducial";
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 1, 1);
    canvas->SetTitle("m [GeV]", "# events (norm to era lumi.)", "mm/ee");

    canvas->Register(h_ee, "Electron channel", kBlack);
    canvas->Register(h_mm, "Muon channel", kBlue);

    // canvas->FillHist(0.5); // -- fill the histograms with alpha=0.5

    // canvas->SetRebin(2); // -- rebin the histograms before drawing

    canvas->SetLegendPosition(0.50, 0.72, 0.95, 0.91);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeRatio(minRatio, maxRatio);
    canvas->SetAutoRangeY();
    canvas->SetAutoRangeRatio();
    // canvas->SetSimpleLogLabel(); // -- simple log label on x-axis
    // canvas->SetRatioLogY(); // -- SetLogY = true for the ratio pad
    
    canvas->Latex_CMSInternal();
    TString info = TString::Format("era: %s", era.Data());
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, info);
    if( !isFiducial) canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Dressed level (Full phase space)");
    else             canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Dressed level (Fiducial region: p_{T}>20(15) GeV, |#eta|<2.4)");

    // canvas->RemoveRatioError(); // -- remove error in the ratio (useful when the error is meaningless)

    canvas->SetSavePath(plotBasePath_+"/comp_channel"); // -- path to be saved (default: "./")

    canvas->Draw();

  }

  void Comparison_Between_Eras() {
    gSystem->mkdir(plotBasePath_+"/comp_era", kTRUE);

    vector<TString> vec_massRange = DYAcc::GetVector_MassRange();
    for(const auto& massRange : vec_massRange ) {      
      Bool_t isFiducial = kFALSE;
      Comparison_Between_Eras_PerMassRange("ee", massRange, isFiducial);
      Comparison_Between_Eras_PerMassRange("mm", massRange, isFiducial);

      isFiducial = kTRUE;
      Comparison_Between_Eras_PerMassRange("ee", massRange, isFiducial);
      Comparison_Between_Eras_PerMassRange("mm", massRange, isFiducial);
    }

    Bool_t isFiducial = kFALSE;
    Comparison_Between_Eras_PerMassRange("ee", "all", isFiducial);
    Comparison_Between_Eras_PerMassRange("mm", "all", isFiducial);

    isFiducial = kTRUE;
    Comparison_Between_Eras_PerMassRange("ee", "all", isFiducial);
    Comparison_Between_Eras_PerMassRange("mm", "all", isFiducial);
  }

  void Comparison_Between_Eras_PerMassRange(TString channel, TString massRange, Bool_t isFiducial) {

    TH1D* h_16pre  = nullptr;
    TH1D* h_16post = nullptr;
    TH1D* h_17     = nullptr;
    TH1D* h_18     = nullptr;
    if( massRange == "all" ) {
      h_16pre  = DYAcc::Get_MergedHist_AllMass(inputPath_, channel, "16pre", isFiducial);
      h_16post = DYAcc::Get_MergedHist_AllMass(inputPath_, channel, "16post", isFiducial);
      h_17     = DYAcc::Get_MergedHist_AllMass(inputPath_, channel, "17", isFiducial);
      h_18     = DYAcc::Get_MergedHist_AllMass(inputPath_, channel, "18", isFiducial);
    }
    else {
      h_16pre  = DYAcc::Get_Hist_Norm_Rebinned(inputPath_, channel, massRange, "16pre", isFiducial);
      h_16post = DYAcc::Get_Hist_Norm_Rebinned(inputPath_, channel, massRange, "16post", isFiducial);
      h_17     = DYAcc::Get_Hist_Norm_Rebinned(inputPath_, channel, massRange, "17", isFiducial);
      h_18     = DYAcc::Get_Hist_Norm_Rebinned(inputPath_, channel, massRange, "18", isFiducial);
    }
    // -- sync. the scale
    h_16pre->Scale(1000.0 / LUMI_16pre);
    h_16post->Scale(1000.0 / LUMI_16post);
    h_17->Scale(1000.0 / LUMI_17);
    h_18->Scale(1000.0 / LUMI_18);

    // -- canvas
    TString canvasName = TString::Format("c_comp_era_%s_%s", channel.Data(), massRange.Data());
    if( isFiducial )
      canvasName = canvasName + "_fiducial";
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 1, 1);
    canvas->SetTitle("m [GeV]", "# events (norm to 1 fb^{-1})", "ratio to preAPV");

    canvas->Register(h_16pre, "2016, preAPV", kBlack);
    canvas->Register(h_16post, "2016, postAPV", kBlue);
    canvas->Register(h_17, "2017", kGreen+2);
    canvas->Register(h_18, "2018", kViolet);

    // canvas->FillHist(0.5); // -- fill the histograms with alpha=0.5

    // canvas->SetRebin(2); // -- rebin the histograms before drawing

    canvas->SetLegendPosition(0.70, 0.70, 0.95, 0.95);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeRatio(minRatio, maxRatio);
    // canvas->SetAutoRangeY();
    canvas->SetRangeY(1e-3, 5e7);
    canvas->SetAutoRangeRatio();
    // canvas->SetSimpleLogLabel(); // -- simple log label on x-axis
    // canvas->SetRatioLogY(); // -- SetLogY = true for the ratio pad
    
    canvas->Latex_CMSInternal();
    TString info = TString::Format("%s channel, %s", channel.Data(), massRange.Data());
    if( massRange == "all" )
      info = channel+" channel";
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, info);
    if( !isFiducial) canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Dressed level (Full phase space)");
    else             canvas->RegisterLatex(0.16, 0.87, 42, 0.5, "Dressed level (Fiducial region: p_{T}>20(15) GeV, |#eta|<2.4)");

    // canvas->RemoveRatioError(); // -- remove error in the ratio (useful when the error is meaningless)

    canvas->SetSavePath(plotBasePath_+"/comp_era"); // -- path to be saved (default: "./")

    canvas->Draw();
  }
};