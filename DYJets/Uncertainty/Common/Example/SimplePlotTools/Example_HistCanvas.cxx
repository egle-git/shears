#include "../SimplePlotTools.h"
#include <TSystem.h>

void Example_HistCanvas(TH1D* h1, TH1D* h2, TH1D* h3);
void Example_HistCanvaswRatio(TH1D* h1, TH1D* h2, TH1D* h3);
void Example_HistStackCanvaswRatio(TH1D* h1, TH1D* h2, TH1D* h3, TH1D* h_tot);
void Example_Hist2DCanvas(TH2D* h2D);

void Example_HistCanvas() {

  TString fileName = "Input/ROOTFile_Input.root";
  if( gSystem->AccessPathName(fileName) ) {
    cout << "Please go to Input directory and run the command first: root -l -b -q InputGenerator.cxx" << endl;
    return;
  }

  TString histName_1   = "h_gaus_1";
  TString histName_2   = "h_gaus_2";
  TString histName_3   = "h_gaus_3";
  TString histName_tot = "h_gaus_tot";

  TH1D* h1    = PlotTool::Get_Hist(fileName, histName_1);
  TH1D* h2    = PlotTool::Get_Hist(fileName, histName_2);
  TH1D* h3    = PlotTool::Get_Hist(fileName, histName_3);
  TH1D* h_tot = PlotTool::Get_Hist(fileName, histName_tot);

  // -- 1st example: HistCanvas
  Example_HistCanvas(h1, h2, h3);

  // -- 2nd example: HistCanvaswRatio
  Example_HistCanvaswRatio(h1, h2, h3);

  // -- 3rd example: HistStackCanvaswRatio
  Example_HistStackCanvaswRatio(h1, h2, h3, h_tot);

  // -- 4th example: Hist2DCanvas
  TString histName_2D = "h2D";
  TH2D* h2D = PlotTool::Get_Hist2D(fileName, histName_2D);
  Example_Hist2DCanvas(h2D);
}

void Example_HistCanvas(TH1D* h1, TH1D* h2, TH1D* h3) {
  PlotTool::HistCanvas* canvas = new PlotTool::HistCanvas("c_HistCanvas", 0, 0);
  canvas->SetTitle("titleX", "titleY");

  canvas->Register(h1, "h1", kRed);
  canvas->Register(h2, "h2", kBlue);
  canvas->Register(h3, "h3", kGreen+2);

  // canvas->FillHist(0.5); // -- fill the histograms with alpha=0.5

  // canvas->SetRebin(2); // -- rebin the histograms before drawing

  canvas->SetLegendPosition(0.70, 0.70, 0.95, 0.95);

  // canvas->SetRangeX(minX, maxX);
  // canvas->SetSimpleLogLabel(); // -- simple log label on x-axis
  // canvas->SetRangeY(minY, maxY);
  canvas->SetAutoRangeY();

  canvas->Latex_CMSInternal();
  canvas->RegisterLatex(0.16, 0.91, 42, 0.6, "Gaussian distributions"); // arguments: x, y, font type, font size, text
  // canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Gaussian distributions}}"); // same with above

  // canvas->SetSavePath("./output"); // -- path to be saved (default: "./")

  // canvas->Draw("HISTLP"); // -- draw option can be provided (default: HISTSAME)
  canvas->Draw(); // -- default: .pdf format

  canvas->SetFormat(".png");
  canvas->Draw(); // -- print again with .png format

  canvas->SetRangeX(-1.0, 1.0);
  canvas->SetCanvasName( canvas->GetCanvasName() + "_zoomIn" );
  canvas->SetFormat(".pdf");
  canvas->Draw(); // -- draw the same plot with zoom-in in x axis
}

void Example_HistCanvaswRatio(TH1D* h1, TH1D* h2, TH1D* h3) {
  PlotTool::HistCanvaswRatio* canvas = new PlotTool::HistCanvaswRatio("c_HistCanvaswRatio", 0, 0);
  canvas->SetTitle("titleX", "titleY", "h2(3)/h1");

  canvas->Register(h1, "h1", kRed); // -- first element: denominator of the ratio
  canvas->Register(h2, "h2", kBlue);
  canvas->Register(h3, "h3", kGreen+2);

  // canvas->FillHist(0.5); // -- fill the histograms with alpha=0.5

  // canvas->SetRebin(2); // -- rebin the histograms before drawing

  canvas->SetLegendPosition(0.70, 0.70, 0.95, 0.95);

  // canvas->SetRangeX(minX, maxX);
  // canvas->SetRangeY(minY, maxY);
  // canvas->SetRangeRatio(minRatio, maxRatio);
  canvas->SetAutoRangeY();
  canvas->SetAutoRangeRatio();
  // canvas->SetSimpleLogLabel(); // -- simple log label on x-axis
  // canvas->SetRatioLogY(); // -- SetLogY = true for the ratio pad
  
  canvas->Latex_CMSInternal();
  canvas->RegisterLatex(0.16, 0.91, 42, 0.6, "Gaussian distributions"); // -- arguments: x, y, font type, font size, text
  // canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Gaussian distributions}}"); // same with above

  // canvas->RemoveRatioError(); // -- remove error in the ratio (useful when the error is meaningless)

  // canvas->SetSavePath("./output"); // -- path to be saved (default: "./")

  canvas->Draw();

  canvas->SetFormat(".png");
  canvas->Draw();
}

void Example_HistStackCanvaswRatio(TH1D* h1, TH1D* h2, TH1D* h3, TH1D* h_tot) {
  PlotTool::HistStackCanvaswRatio* canvas = new PlotTool::HistStackCanvaswRatio("c_HistStackCanvaswRatio", 0, 0);
  // canvas->Ratio_Reversed(); // -- if it is called, the ratio will be MC/data, not data/MC
  canvas->SetTitle("titleX", "titleY", "data/MC"); // -- ratio: by default, total MC is the denominator; data is the numerator

  canvas->RegisterData(h_tot, "Total", kBlack);
  canvas->Register(h1, "h1", kRed);
  canvas->Register(h2, "h2", kBlue);
  canvas->Register(h3, "h3", kGreen+2);

  // canvas->SetRebin(2); // -- rebin the histograms before drawing

  canvas->SetLegendPosition(0.70, 0.70, 0.95, 0.95);

  // canvas->SetRangeX(minX, maxX);
  // canvas->SetRangeY(minY, maxY);
  // canvas->SetRangeRatio(minRatio, maxRatio);
  canvas->SetAutoRangeY();
  canvas->SetAutoRangeRatio();

  canvas->ShowDataMCRatio();

  canvas->Latex_CMSInternal();
  canvas->RegisterLatex(0.16, 0.91, 42, 0.6, "Gaussian distributions"); // -- arguments: x, y, font type, font size, text
  // canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Gaussian distributions}}"); // -- same with above

  // canvas->SetSavePath("./output"); // -- path to be saved (default: "./")

  canvas->Draw();

  canvas->SetFormat(".png");
  canvas->Draw();
}

void Example_Hist2DCanvas(TH2D* h2D) {
  PlotTool::Hist2DCanvas* canvas = new PlotTool::Hist2DCanvas("c_Hist2DCanvas", 0, 0, 0);
  canvas->SetTitle("titleX", "titleY");

  canvas->Register(h2D);

  // canvas->SetRangeX(minX, maxX);
  // canvas->SetRangeY(minY, maxY);
  // canvas->SetRangeZ(minZ, maxZ);
  canvas->SetAutoRangeZ();

  canvas->Latex_CMSInternal();
  canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Random Gaussian values}}");

  // canvas->SetSavePath("./output"); // -- path to be saved (default: "./")

  canvas->Draw();

  canvas->SetFormat(".png");
  canvas->Draw();
}