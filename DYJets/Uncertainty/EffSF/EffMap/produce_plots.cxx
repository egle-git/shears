#include "Common/SimplePlotTools.h"
#include "Common/DYPath.h"

// -- produce the plots w/ systematic variation produced by `generate_systEffMap.cxx`

class PlotProducer {
public:
  PlotProducer(TString dataType, TString lepType, TString effType) {
    dataType_ = dataType;
    lepType_ = lepType;
    effType_ = effType;
    Init();
  }

  void Produce() {
    cout << "Input file: " << fileName_ << endl;

    for( auto histType : vec_histType_ ) {
      ProducePlot_1D_Comp_AllEra(histType);

      if( lepType_ == "el" && effType_ == "ID" ) { // has different binning between 2016 vs. 2017 & 18
        ProducePlot_1D_Comp_2016pre_2016post(histType);
        ProducePlot_1D_Comp_2017_2018(histType);
      }
    }
  }
private:
  TString dataType_ = "";
  TString lepType_ = "";
  TString effType_ = "";
  vector<TString> vec_histType_;

  // TString fileName_ = "LeptonEffMap_FullRun2.root";
  TString fileName_ = DYTool::path_systEffMap;

  void ProducePlot_1D_Comp_AllEra(TString histType) {
    TString histName_16pre  = TString::Format("16pre-%s-%s-%s-%s", lepType_.Data(), effType_.Data(), dataType_.Data(), histType.Data());
    TString histName_16post = histName_16pre; histName_16post.ReplaceAll("16pre", "16post");
    TString histName_17     = histName_16pre; histName_17.ReplaceAll("16pre", "17");
    TString histName_18     = histName_16pre; histName_18.ReplaceAll("16pre", "18");

    if( histType == "nominal" )
      CheckConsistency_BinEdge(histName_16pre, histName_16post, histName_17, histName_18);

    TH1D* h_16pre  = Get_Converted1D(histName_16pre);
    TH1D* h_16post = Get_Converted1D(histName_16post);
    TH1D* h_17     = Get_Converted1D(histName_17);
    TH1D* h_18     = Get_Converted1D(histName_18);

    TString canvasName = histName_16pre;
    canvasName.ReplaceAll("16pre", "comp_allEra");
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 0, 0);

    TH2D* h2D_16pre = PlotTool::Get_Hist2D(fileName_, histName_16pre); // -- for # bins per axis
    TString titleX = TString::Format("p_{T}-#eta bin number (%d p_{T} #times %d #eta bins)", h2D_16pre->GetNbinsY(), h2D_16pre->GetNbinsX());
    if( effType_ == "DTRIG" )
      titleX = TString::Format("#eta(#mu1)-#eta(#mu2) bin number (%d #times %d bins)", h2D_16pre->GetNbinsY(), h2D_16pre->GetNbinsX());

    canvas->SetTitle(titleX, "Value", "ratio to 16pre");

    canvas->Register(h_16pre, "2016, preAPV", kBlack); // -- first element: denominator of the ratio
    canvas->Register(h_16post, "2016, postAPV", kBlue);
    canvas->Register(h_17, "2017", kGreen+2);
    canvas->Register(h_18, "2018", kViolet);

    canvas->SetLegendPosition(0.70, 0.70, 0.95, 0.95);

    // canvas->SetRangeX(minX, maxX);
    if( lepType_ == "el" && effType_ == "ID" ) canvas->SetRangeX(11, 40); // -- start at 20 GeV; restrict up to the same pt range
    if( lepType_ == "el" && effType_ == "TRIG_Leg1" ) canvas->SetRangeX(201, 550); // -- start at pt = 27 GeV
    if( lepType_ == "el" && effType_ == "TRIG_Leg2" ) canvas->SetRangeX(201, 600); // -- start at pt = 20 GeV
    // canvas->SetRangeY(minY, maxY);
    // canvas->SetRangeRatio(minRatio, maxRatio);
    canvas->SetAutoRangeY();
    canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    TString info = canvasName;
    info.ReplaceAll("comp_allEra_", "");
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, info);

    // canvas->RegisterLatex(0.16, 0.91, 42, 0.6, "Gaussian distributions"); // arguments: x, y, font type, font size, text
    // canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Gaussian distributions}}"); // same with above

    // canvas->RemoveRatioError(); // remove error in the ratio (useful when the error is meaningless)

    canvas->SetSavePath("./plot"); // -- path to be saved (default: "./")

    canvas->Draw("HISTLP");

    // -- fixed y-range for uncertainties (to compare between plots)
    if( histType != "cv" ) {
      canvas->SetAutoRangeY(kFALSE);
      canvas->SetRangeY(0, 0.03);
      canvas->SetCanvasName( canvas->GetCanvasName()+"_fixedYRange" );
      canvas->Draw("HISTLP");
    }
  }

  void ProducePlot_1D_Comp_2016pre_2016post(TString histType) {
    TString histName_16pre  = TString::Format("16pre-%s-%s-%s-%s", lepType_.Data(), effType_.Data(), dataType_.Data(), histType.Data());
    TString histName_16post = histName_16pre; histName_16post.ReplaceAll("16pre", "16post");
    // TString histName_17     = histName_16pre; histName_17.ReplaceAll("16pre", "17");
    // TString histName_18     = histName_16pre; histName_18.ReplaceAll("16pre", "18");

    TH1D* h_16pre  = Get_Converted1D(histName_16pre);
    TH1D* h_16post = Get_Converted1D(histName_16post);
    // TH1D* h_17     = Get_Converted1D(histName_17);
    // TH1D* h_18     = Get_Converted1D(histName_18);

    TString canvasName = histName_16pre;
    canvasName.ReplaceAll("16pre", "comp_2016prePost");
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 0, 0);

    TH2D* h2D_16pre = PlotTool::Get_Hist2D(fileName_, histName_16pre); // -- for # bins per axis
    TString titleX = TString::Format("p_{T}-#eta bin number (%d p_{T} #times %d #eta bins)", h2D_16pre->GetNbinsY(), h2D_16pre->GetNbinsX());
    if( effType_ == "DTRIG" )
      titleX = TString::Format("#eta(#mu1)-#eta(#mu2) bin number (%d #times %d bins)", h2D_16pre->GetNbinsY(), h2D_16pre->GetNbinsX());

    canvas->SetTitle(titleX, "Value", "ratio to 16pre");

    canvas->Register(h_16pre, "2016, preAPV", kBlack); // -- first element: denominator of the ratio
    canvas->Register(h_16post, "2016, postAPV", kBlue);
    // canvas->Register(h_17, "2017", kGreen+2);
    // canvas->Register(h_18, "2018", kViolet);

    canvas->SetLegendPosition(0.70, 0.70, 0.95, 0.95);

    // canvas->SetRangeX(minX, maxX);
    if( lepType_ == "el" && effType_ == "ID" ) canvas->SetRangeX(11, 50); // -- start at 20 GeV; restrict up to the same pT range
    // canvas->SetRangeRatio(minRatio, maxRatio);
    canvas->SetAutoRangeY();
    canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    TString info = canvasName;
    info.ReplaceAll("comp_2016prePost_", "");
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, info);

    canvas->SetSavePath("./plot"); // -- path to be saved (default: "./")

    canvas->Draw("HISTLP");
  }

  void ProducePlot_1D_Comp_2017_2018(TString histType) {
    TString histName_16pre  = TString::Format("16pre-%s-%s-%s-%s", lepType_.Data(), effType_.Data(), dataType_.Data(), histType.Data());
    // TString histName_16post = histName_16pre; histName_16post.ReplaceAll("16pre", "16post");
    TString histName_17     = histName_16pre; histName_17.ReplaceAll("16pre", "17");
    TString histName_18     = histName_16pre; histName_18.ReplaceAll("16pre", "18");

    // TH1D* h_16pre  = Get_Converted1D(histName_16pre);
    // TH1D* h_16post = Get_Converted1D(histName_16post);
    TH1D* h_17     = Get_Converted1D(histName_17);
    TH1D* h_18     = Get_Converted1D(histName_18);

    TString canvasName = histName_16pre;
    canvasName.ReplaceAll("16pre", "comp_201718");
    PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 0, 0);

    TH2D* h2D_17 = PlotTool::Get_Hist2D(fileName_, histName_17); // -- for # bins per axis
    TString titleX = TString::Format("p_{T}-#eta bin number (%d p_{T} #times %d #eta bins)", h2D_17->GetNbinsY(), h2D_17->GetNbinsX());
    if( effType_ == "DTRIG" )
      titleX = TString::Format("#eta(#mu1)-#eta(#mu2) bin number (%d #times %d bins)", h2D_17->GetNbinsY(), h2D_17->GetNbinsX());

    canvas->SetTitle(titleX, "Value", "ratio to 17");

    // canvas->Register(h_16pre, "2016, preAPV", kBlack); // -- first element: denominator of the ratio
    // canvas->Register(h_16post, "2016, postAPV", kBlue);
    canvas->Register(h_17, "2017", kGreen+2);
    canvas->Register(h_18, "2018", kViolet);

    canvas->SetLegendPosition(0.70, 0.70, 0.95, 0.95);

    // canvas->SetRangeX(minX, maxX);
    // canvas->SetRangeY(minY, maxY);
    if( lepType_ == "el" && effType_ == "ID" ) canvas->SetRangeX(11, 60); // -- start at 20 GeV
    // canvas->SetRangeRatio(minRatio, maxRatio);
    canvas->SetAutoRangeY();
    canvas->SetAutoRangeRatio();

    canvas->Latex_CMSInternal();
    TString info = canvasName;
    info.ReplaceAll("comp_201718_", "");
    canvas->RegisterLatex(0.16, 0.91, 42, 0.6, info);

    canvas->SetSavePath("./plot"); // -- path to be saved (default: "./")

    canvas->Draw("HISTLP");
  }

  void Init() {
    vec_histType_ = Init_Vec_HistType();
  }

  vector<TString> Init_Vec_HistType() {
    vector<TString> vec_histType;

    if( lepType_ == "mu" ) {
      vec_histType = {"cv", "absUnc_stat", "absUnc_syst_all"};
    }
    else if( lepType_ == "el" ) {
      if( effType_ == "RECO" || effType_ == "ID" ) {
        vec_histType = {"cv", "absUnc_stat_data", "absUnc_stat_mc", "absUnc_syst_altBkg", "absUnc_syst_altSig", "absUnc_syst_altMC", "absUnc_syst_altTag"};
      }
      else if( effType_ == "TRIG_Leg1" || effType_ == "TRIG_Leg2" ) {
        vec_histType = {"cv", "absUnc_stat", "absUnc_syst_altMC", "absUnc_syst_altTag", "absUnc_syst_altSub"}; 
      }
    }
    return vec_histType;
  }

  TH1D* Get_Converted1D(TString histName) {
    return Convert_2Dto1D( PlotTool::Get_Hist2D(fileName_, histName) );
  }

  TH1D* Convert_2Dto1D(TH2D* h2D) {

    Int_t nBinX = h2D->GetNbinsX();
    Int_t nBinY = h2D->GetNbinsY();

    Int_t nBin_tot = Int_t(nBinX * nBinY);
    // -- keep same # bins for all eras
    if( lepType_ == "el" && effType_ == "ID" && nBin_tot < 60 ) nBin_tot = 60;

    TString histName = h2D->GetName();
    TString histName_new = histName + "_conv_1D";
    TH1D* h = new TH1D(histName_new, "", nBin_tot, 0, nBin_tot);

    Int_t i_bin1D = 1;
    for(Int_t i_y=0; i_y<nBinY; ++i_y) {
      Int_t i_binY = i_y+1;

      for(Int_t i_x=0; i_x<nBinX; ++i_x) {
        Int_t i_binX = i_x+1;

        Double_t value = h2D->GetBinContent(i_binX, i_binY);

        // -- change the dummy uncertainty value
        if( lepType_ == "el" && 
            (effType_ == "ID") && histName.Contains("absUnc") &&
            value == 1.0 )
          value = 0.0;

        // -- bins below pt threshold: not presented
        if( lepType_ == "el" && 
            (effType_ == "TRIG_Leg1" || effType_ == "TRIG_Leg2") &&
            i_bin1D <= 200 )
          value = 0.0;

        h->SetBinContent(i_bin1D, value);
        h->SetBinError(i_bin1D, 0); // -- meaningless
        // printf("(%02d, %02d) -> 1D %03d: value = %lf\n", i_binX, i_binY, i_bin1D, value);

        i_bin1D++;
      }
    }

    return h;
  }

  void CheckConsistency_BinEdge(TString histName_16pre, TString histName_16post, TString histName_17, TString histName_18) {
    TH2D* h2D_16pre  = PlotTool::Get_Hist2D(fileName_, histName_16pre);
    TH2D* h2D_16post = PlotTool::Get_Hist2D(fileName_, histName_16post);
    TH2D* h2D_17     = PlotTool::Get_Hist2D(fileName_, histName_17);
    TH2D* h2D_18     = PlotTool::Get_Hist2D(fileName_, histName_18);

    vector<Double_t> vec_binEdgeX_16pre  = Get_BinEdge(h2D_16pre, "X");
    vector<Double_t> vec_binEdgeX_16post = Get_BinEdge(h2D_16post, "X");
    vector<Double_t> vec_binEdgeX_17     = Get_BinEdge(h2D_17, "X");
    vector<Double_t> vec_binEdgeX_18     = Get_BinEdge(h2D_18, "X");
    cout << "[CheckConsistency_BinEdge] x-axis" << endl;
    Print_BinEdge(vec_binEdgeX_16pre);
    Print_BinEdge(vec_binEdgeX_16post);
    Print_BinEdge(vec_binEdgeX_17);
    Print_BinEdge(vec_binEdgeX_18);
    cout << endl;

    vector<Double_t> vec_binEdgeY_16pre  = Get_BinEdge(h2D_16pre, "Y");
    vector<Double_t> vec_binEdgeY_16post = Get_BinEdge(h2D_16post, "Y");
    vector<Double_t> vec_binEdgeY_17     = Get_BinEdge(h2D_17, "Y");
    vector<Double_t> vec_binEdgeY_18     = Get_BinEdge(h2D_18, "Y");
    cout << "[CheckConsistency_BinEdge] y-axis" << endl;
    Print_BinEdge(vec_binEdgeY_16pre);
    Print_BinEdge(vec_binEdgeY_16post);
    Print_BinEdge(vec_binEdgeY_17);
    Print_BinEdge(vec_binEdgeY_18);
    cout << endl;
    cout << endl;
  }

  void Print_BinEdge(vector<Double_t> vec_binEdge) {
    for( auto binEdge : vec_binEdge ) {
      // if( std::abs(binEdge) < 10 ) printf("%.3lf ", binEdge);
      // else                         printf("%.1lf ", binEdge);
      printf("%.1lf ", binEdge);
    }
    printf(" (total %02lu bins)\n", vec_binEdge.size()-1);
  }

  vector<Double_t> Get_BinEdge(TH2D* h2D, TString axis) {
    vector<Double_t> vec_edge;
    Int_t nBin;
    if( axis == "X") nBin = h2D->GetNbinsX();
    if( axis == "Y") nBin = h2D->GetNbinsY();

    for(Int_t i=0; i<nBin; i++) {
      Int_t i_bin = i+1;

      Double_t binEdge;
      if( axis == "X" ) binEdge = h2D->GetXaxis()->GetBinLowEdge(i_bin);
      if( axis == "Y" ) binEdge = h2D->GetYaxis()->GetBinLowEdge(i_bin);
      vec_edge.push_back( binEdge );
    }

    Double_t binEdge_last;
    if( axis == "X" ) binEdge_last = h2D->GetXaxis()->GetBinLowEdge(nBin+1);
    if( axis == "Y" ) binEdge_last = h2D->GetYaxis()->GetBinLowEdge(nBin+1);
    vec_edge.push_back( binEdge_last );

    return vec_edge;
  }
};

void produce_plots() {
  vector<TString> vec_effType_mu = {"ID", "ISO", "STRIG", "DTRIG"};
  vector<TString> vec_effType_el = {"RECO", "ID", "TRIG_Leg1", "TRIG_Leg2"};

  for(auto effType : vec_effType_mu ) {

    // -- DTRIG: only SF is available
    if( effType == "DTRIG" ) {
      PlotProducer* producer = new PlotProducer("sf", "mu", effType);
      producer->Produce();
    }

    // -- others: MC & data efficiencies are available
    else {
      PlotProducer* producer_data = new PlotProducer("data", "mu", effType);
      producer_data->Produce();

      PlotProducer* producer_mc = new PlotProducer("mc", "mu", effType);
      producer_mc->Produce();
    }

  }

  for( auto effType : vec_effType_el ) {
    PlotProducer* producer = new PlotProducer("sf", "el", effType);
    producer->Produce();
  }
}