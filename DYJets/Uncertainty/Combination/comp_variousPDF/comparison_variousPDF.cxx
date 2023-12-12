#include "Common/DYPath.h"
#include "Common/SimplePlotTools.h"
#include "Common/DYTool.h"

class PlotProducer {
public:
  // -- type: fid or FPS
  PlotProducer(TString channel, TString type): channel_(channel), type_(type) {}

  void Set_RatioRange(Double_t min, Double_t max) {
    ratioMin_ = min;
    ratioMax_ = max;
  }


  void Produce() {
    TH1::AddDirectory(kFALSE);
    Init();

    ProducePlot();
   }

private:
  struct PDFInfo {
    TString pdfTag;
    TString legend;
    Int_t marker;
    Int_t color;
  };

  TString channel_ = "";
  TString type_ = "";

  TH1D* h_data_ = nullptr;

  // -- plotting order
  vector<TString> vec_PDFTag_ = {"NNPDF30", "PDFVar", "NNPDF40", "CT18", "CT18Z", "MMHT2014", "MSHT20", "HERAPDF20", "ABMP16"};

  std::map<TString, TH1D*> map_theoryHist_;
  std::map<TString, PDFInfo> map_pdfInfo_;

  Double_t small_ = 0.000001;

  Double_t yTitleSize_pixel_ = 8;
  Double_t yTitleOffset_ = 0.3;

  Double_t yLabelSize_pixel_ = 19;
  Double_t yLabelOffset_pixel_ = 1;

  Double_t ratioMin_ = 0.31;
  Double_t ratioMax_ = 1.69;

  void Init() {
    TString fileName = "../CombinedResult_DYXSec_"+type_+".root";

    // -- data histogram
    h_data_ = PlotTool::Get_Hist(fileName, "h_dsigdm_"+channel_);

    // -- theory histogram
    for(const auto& PDFTag : vec_PDFTag_ ) {
      TString histName = TString::Format("h_dsigdm_%s_%s_000", type_.Data(), PDFTag.Data());
      TH1D* h_theory = PlotTool::Get_Hist(DYTool::path_theoryPred_m200, histName);

      map_theoryHist_.insert( std::make_pair(PDFTag, h_theory) );
    }

    ConvertData_AboveM200();

    Insert_PDFInfo();
  }

  void Insert_PDFInfo() {
    Insert_PDFInfo("NNPDF30",   "NNPDF 3.0",   20, kRed);
    Insert_PDFInfo("PDFVar",    "NNPDF 3.1",   20, kGreen+2);
    Insert_PDFInfo("NNPDF40",   "NNPDF 4.0",   20, kBlue);
    Insert_PDFInfo("CT18",      "CT18",        20, kCyan);
    Insert_PDFInfo("CT18Z",     "CT18Z",       20, kViolet);
    Insert_PDFInfo("MMHT2014",  "MMHT2014",    20, kGray);
    Insert_PDFInfo("MSHT20",    "MSHT20",      20, kOrange+2);
    Insert_PDFInfo("HERAPDF20", "HERAPDF 2.0", 20, kBlue-9);
    Insert_PDFInfo("ABMP16",    "ABMP16",      20, kGreen-6);
  }

  void Insert_PDFInfo(TString pdfTag, TString legend, Int_t marker, Int_t color) {
    PDFInfo info{ pdfTag, legend, marker, color };
    map_pdfInfo_.insert( std::make_pair(pdfTag, info) );
  }

  void ConvertData_AboveM200() {
    TH1D* h_axis = map_theoryHist_[vec_PDFTag_[0]]; // -- random

    TH1D* h_data_m200 = (TH1D*)h_axis->Clone();
    h_data_m200->Reset("ICES");

    for(Int_t i=0; i<h_data_->GetNbinsX(); ++i) {
      Int_t i_bin = i+1;

      Double_t lowerEdge = h_data_->GetBinLowEdge(i_bin);
      Double_t upperEdge = h_data_->GetBinLowEdge(i_bin+1);

      if( upperEdge < 200.0 ) continue;

      Double_t value = h_data_->GetBinContent(i_bin);
      Double_t error = h_data_->GetBinError(i_bin);

      for(Int_t j=0; j<h_data_m200->GetNbinsX(); ++j) {
        Int_t j_bin = j+1;

        Double_t lowerEdge_j = h_data_m200->GetBinLowEdge(j_bin);
        Double_t upperEdge_j = h_data_m200->GetBinLowEdge(j_bin+1);

        if( lowerEdge == lowerEdge_j && upperEdge == upperEdge_j) {
          h_data_m200->SetBinContent(j_bin, value);
          h_data_m200->SetBinError(j_bin, error);
        }
      } // -- iteration over j
    } // -- iteration over i

    delete h_data_;
    h_data_ = h_data_m200;
  }

  void ProducePlot() {
    TString canvasName = "c_comp_variousPDF_"+channel_+"_"+type_;

    gStyle->SetPadBorderMode(0);
    gStyle->SetFrameBorderMode(0);

    TCanvas* canvas = SquareCanvas(canvasName);

    // -- just for drawing latex -- //
    TPad* bigPad = new TPad("bigPad","bigPad", 0.01, 0.01, 0.99, 0.99 );
    bigPad->Draw();
    bigPad->cd();
    bigPad->SetTopMargin(0.05);
    bigPad->SetBottomMargin(0.05);

    Double_t upperMargin = 0.05;
    Double_t lowerMargin = 0.1;

    Double_t ySize_pads = 0;
    Double_t ySize_lastPad = 0;
    Double_t lowerMargin_lastPad = 0;

    Int_t nResult = (Int_t)vec_PDFTag_.size();
    ySize_pads = ( 1 - upperMargin - lowerMargin ) / nResult;

    ySize_lastPad = ySize_pads + lowerMargin;

    lowerMargin_lastPad = lowerMargin / ySize_lastPad;

    // -- inner pad with top and bottom margin -- //
    TPad *multiPad = new TPad("multiPad", "multiPad", 0.01, ySize_lastPad, 0.99, 1-upperMargin );
    multiPad->Draw();
    multiPad->cd();
    // multiPad->SetBottomMargin(0.1);
    multiPad->Divide(1, nResult-1, small_, small_);

    for(Int_t i=0; i<nResult-1; ++i) {
      Int_t iPad = i+1; // -- start from 1
      multiPad->cd(iPad);
      Draw_EachPad(vec_PDFTag_[i]);
    }

    // -- last pad
    bigPad->cd();
    TPad *lastPad = new TPad("lastPad", "lastPad", 0.01, 0.00, 0.99, ySize_lastPad );
    lastPad->Draw();
    lastPad->cd();

    Draw_EachPad(vec_PDFTag_[nResult-1], kTRUE, ySize_lastPad, ySize_pads);

    lastPad->SetBottomMargin( lowerMargin_lastPad );

    bigPad->cd();
    TLatex latex;
    Double_t lumi = DYTool::GetLumi("all") / 1000.0; // -- fb
    Int_t energy_CM = 13;

    latex.DrawLatexNDC(0.09, 0.96, "#font[62]{CMS}");
    latex.DrawLatexNDC(0.19, 0.96, "#font[42]{#it{#scale[0.8]{ Preliminary}}}");
    // latex.DrawLatexNDC(0.09, 0.96, "#scale[0.8]{#font[62]{CMS }#font[42]{#it{Supplementary}}}");
    // latex.DrawLatexNDC(0.74, 0.96, "#font[62]{#scale[0.7]{2.8 fb^{-1} (13 TeV)}}");

    if( channel_ == "ll" )
      latex.DrawLatexNDC(0.57, 0.96, "#font[42]{#scale[0.7]{"+TString::Format("%.1lf fb^{-1} (ee+#mu#mu) (%d TeV)", lumi, energy_CM)+"}}");
    if( channel_ == "ee" )
      latex.DrawLatexNDC(0.62, 0.96, "#font[42]{#scale[0.7]{"+TString::Format("%.1lf fb^{-1} (ee) (%d TeV)", lumi, energy_CM)+"}}");
    if( channel_ == "mm" )
      latex.DrawLatexNDC(0.62, 0.96, "#font[42]{#scale[0.7]{"+TString::Format("%.1lf fb^{-1} (#mu#mu) (%d TeV)", lumi, energy_CM)+"}}");

    canvas->SaveAs(".pdf");
  }

  void SetAttribute(TH1D* h, PDFInfo& info) {
    h->SetStats(kFALSE);
    h->SetMarkerStyle(info.marker);
    h->SetMarkerColor(info.color);
    h->SetLineColor(info.color);
    h->SetFillColorAlpha(kWhite, 0);
    h->SetTitle("");
  }

  void Draw_EachPad(TString pdfTag, Bool_t isLast = kFALSE, Double_t ySize_lastPad = 0, Double_t ySize_pads = 0) {
    gPad->SetTopMargin(small_);
    gPad->SetBottomMargin(small_);
    gPad->SetLeftMargin(0.08);
    gPad->SetRightMargin(0.045);
    gPad->SetLogx();
    gPad->SetTickx();

    TH1D* h_ratio = Get_Ratio(pdfTag);
    SetAttribute(h_ratio, map_pdfInfo_[pdfTag]);
    h_ratio->Draw();

    TAxis *xAxis = h_ratio->GetXaxis();
    xAxis->SetMoreLogLabels();
    xAxis->SetNoExponent();
    xAxis->SetTitle( "" );
    xAxis->SetTitleOffset( 0.0 );
    xAxis->SetTitleSize( 0.0 );
    xAxis->SetLabelColor(1);
    xAxis->SetLabelFont(42);
    xAxis->SetLabelOffset(0.01);
    xAxis->SetLabelSize(0.13);
    // xAxis->SetTickLength( xTickWidth );

    TAxis *yAxis = h_ratio->GetYaxis();
    yAxis->SetTitle( map_pdfInfo_[pdfTag].legend+"/Data" );
    yAxis->CenterTitle();
    yAxis->SetTitleSize( yTitleSize_pixel_ / (gPad->GetWh()*gPad->GetAbsHNDC()) );
    // yAxis->SetTitleOffset( yTitleOffset_pixel / (gPad->GetWh()*gPad->GetAbsHNDC()) );
    yAxis->SetTitleOffset( yTitleOffset_ );

    yAxis->SetLabelOffset(0.005);
    yAxis->SetLabelSize( yLabelSize_pixel_ / (gPad->GetWh()*gPad->GetAbsHNDC()) );
    yAxis->SetRangeUser( ratioMin_, ratioMax_ );
    yAxis->SetNdivisions( 505 );
    // Hist_Theory->h_ratio->SetMinimum( 0.45 );
    // Hist_Theory->h_ratio->SetMaximum( 1.55 );
    // yAxis->SetRangeUser( 0.45, 1.55 );

    TLegend *legend;
    if( isLast )
      PlotTool::SetLegend( legend, 0.10, 0.52, 0.50, 0.65);
    else
      PlotTool::SetLegend( legend, 0.10, 0.05, 0.50, 0.25);
    legend->SetTextFont(62);
    legend->AddEntry( h_ratio, map_pdfInfo_[pdfTag].legend);
    legend->Draw();

    if( isLast ) {
      xAxis->SetTitleSize( 0.2 );
      xAxis->SetTitleOffset( 0.8 );
      xAxis->SetTitle( "m [GeV]" );
      yAxis->SetTitleOffset( yTitleOffset_ * (ySize_lastPad / ySize_pads) );

      // legend->SetY1NDC(0.3);
      // legend->SetY2NDC(0.6);
    }

    TF1 *f_line;
    PlotTool::DrawLine(f_line);
  }

  TH1D* Get_Ratio(TString pdfTag) {
    TH1D* h_ratio = (TH1D*)h_data_->Clone();
    TH1D* h_theory = map_theoryHist_[pdfTag];

    h_ratio->Divide( h_theory, h_data_);

    return h_ratio;

  }

  TCanvas* SquareCanvas(TString canvasName) {
    TCanvas* c = new TCanvas(canvasName, "", 800, 800);
    c->cd();
    
    c->SetTopMargin(0.05);
    c->SetLeftMargin(0.13);
    c->SetRightMargin(0.045);
    c->SetBottomMargin(0.13);

    return c;
  }
};

void comparison_variousPDF(TString type, Double_t ratioMin = 0.31, Double_t ratioMax = 1.69) {
  PlotProducer producer_ee("ee", type);
  producer_ee.Set_RatioRange(ratioMin, ratioMax);
  producer_ee.Produce();

  PlotProducer producer_mm("mm", type);
  producer_mm.Set_RatioRange(ratioMin, ratioMax);
  producer_mm.Produce();

  PlotProducer producer_ll("ll", type);
  producer_ll.Set_RatioRange(ratioMin, ratioMax);
  producer_ll.Produce();
}

void comparison_variousPDF() {
  // comparison_variousPDF("fid");
  // comparison_variousPDF("FPS");

  comparison_variousPDF("fid", 0.83, 1.17);
  comparison_variousPDF("FPS", 0.83, 1.17);
}