#include "Common/DYPath.h"
#include "Common/DYTool.h"

// -- get the alternative top quark background histogram
// -- and total bkgMC histograms as well (default non-top bkg. + alt top bkg.)
// -- input
// ---- channel (ee or mm)
// ---- map_filePath: (era, filePath) pair, for era = 16pre, 16post, 17 and 18
class TopBkgHist {
public:
  TopBkgHist(TString channel, std::map<TString, TString> map_filePath): 
  channel_(channel), map_filePath_(map_filePath) { Init(); }

  TH1D* Get(TString process) const {
    if( process == "TT" )        return (TH1D*)h_TT_->Clone();
    if( process == "singleTop" ) return (TH1D*)h_singleTop_->Clone();
    if( process == "bkgMC" )     return (TH1D*)h_bkgMC_->Clone();

    throw std::invalid_argument("[TopBkgHist::Get] process = " + process + " is not supported");
    return nullptr;
  }

private:
  TString channel_ = "";
  // -- (era, filePath) map
  std::map<TString, TString> map_filePath_;

  // -- mass-bin-number axis, reco-level hist after merging all eras
  TH1D* h_TT_ = nullptr;
  TH1D* h_singleTop_ = nullptr;

  // -- default MCs + alternative TT & singleTop bkg. here
  // -- ingredient for DYRun2Result
  TH1D* h_bkgMC_;

  void Init() {    
    Bool_t useCustomPath = kTRUE;
    h_TT_        = Get_RecoHist("TT", useCustomPath);
    h_singleTop_ = Get_RecoHist("singleTop", useCustomPath);

    vector<TH1D*> vec_hist_bkgMC;
    vec_hist_bkgMC.push_back( h_TT_ );
    vec_hist_bkgMC.push_back( h_singleTop_ );

    for(const auto& pair : DYTool::map_tag ) {
      TString process = pair.first;
      if( process == "data" )      continue;
      if( process == "DY" )        continue;
      if( process == "TT" )        continue; // -- already included
      if( process == "singleTop" ) continue; // -- already included

      TH1D* h_MC = Get_RecoHist(process); // -- use default path
      vec_hist_bkgMC.push_back( h_MC );
    }

    h_bkgMC_ = DYTool::MergeHist( vec_hist_bkgMC );
  }

  TH1D* Get_RecoHist(TString process, Bool_t useCustomPath = kFALSE) {
    vector<TH1D*> vec_hist;

    vector<TString> vec_era = {"16pre", "16post", "17", "18"};
    for(const auto& era : vec_era )
      vec_hist.push_back( Get_RecoHist(process, era, useCustomPath) );

    return DYTool::MergeHist( vec_hist );
  }

  TH1D* Get_RecoHist(TString process, TString era, Bool_t useCustomPath) {
    TString filePath = "";
    if( useCustomPath ) filePath = map_filePath_[era];
    else                filePath = TString::Format("%s/%s/%s", DYTool::path_default.Data(), channel_.Data(), era.Data());
    ProcessOutput output(process, DYTool::map_tag[process], filePath, DYTool::GetLumi(era));
    TString histName = "TUnfold1DReco_inc0jet";

    return output.Get(histName);
  }
};

// -- compare default bkgMC vs. bkgMC made by TopBkgHist using default path
void validation(TString channel) {
  // -- default bkgMC
  TString defaultPath = DYTool::path_default+"/"+channel;
  DYRun2Result result(defaultPath);
  result.Produce();
  TH1D* h_bkgMC_default = result.Get_AllEra("reco", "bkgMC");


  // -- bkgMC from TopBkgHist
  std::map<TString, TString> map_defaultPath = {
    {"16pre",  defaultPath+"/16pre"},
    {"16post", defaultPath+"/16post"},
    {"17",     defaultPath+"/17"},
    {"18",     defaultPath+"/18"}
  };

  TopBkgHist topHist(channel, map_defaultPath);
  TH1D* h_bkgMC_topBkgHist = topHist.Get("bkgMC");

  DYTool::Remove_NegativeBin(h_bkgMC_default);
  DYTool::Remove_NegativeBin(h_bkgMC_topBkgHist);

  // -- comparison
  TString canvasName = "c_validation_topBkgHist_"+channel;
  PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio(canvasName, 0, 1);
  canvas->SetTitle("mass bin number", "# events", "topHist/default");

  canvas->Register(h_bkgMC_default, "Default", kBlack);
  canvas->Register(h_bkgMC_topBkgHist, "from TopBkgHist", kBlue);

  canvas->SetLegendPosition(0.50, 0.82, 0.95, 0.95);

  // canvas->SetRangeX(minX, maxX);
  // canvas->SetRangeY(minY, maxY);
  // canvas->SetRangeRatio(minRatio, maxRatio);
  canvas->SetAutoRangeY();
  canvas->SetAutoRangeRatio();
  
  canvas->Latex_CMSInternal();
  TString channelInfo = (channel == "ee" ) ? "Electron channel" : "Muon channel";
  canvas->RegisterLatex(0.16, 0.91, 42, 0.6, channelInfo);
  // canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Gaussian distributions}}"); // same with above

  canvas->RemoveRatioError(); // remove error in the ratio (useful when the error is meaningless)

  TString plotDirPath = DYTool::Set_PlotPath("Background/TopBkgHist");
  canvas->SetSavePath(plotDirPath); // -- path to be saved (default: "./")

  canvas->Draw();

  PlotTool::IsRatio1(h_bkgMC_default, h_bkgMC_topBkgHist);
}

void validation() {
  validation("ee");
  validation("mm");
}