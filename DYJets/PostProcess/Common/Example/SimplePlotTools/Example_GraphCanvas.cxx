#include "../SimplePlotTools.h"

void Example_GraphCanvas(TGraphAsymmErrors* g1, TGraphAsymmErrors* g2, TGraphAsymmErrors* g3);
void Example_GraphCanvaswRatio(TGraphAsymmErrors* g1, TGraphAsymmErrors* g2, TGraphAsymmErrors* g3);

void Example_GraphCanvas() {
  TString fileName = "Input/ROOTFile_Input.root";
  if( gSystem->AccessPathName(fileName) ) {
    cout << "Please go to Input directory and run the command first: root -l -b -q InputGenerator.cxx" << endl;
    return;
  }

  TString graphName_1   = "g_gaus_1";
  TString graphName_2   = "g_gaus_2";
  TString graphName_3   = "g_gaus_3";

  TGraphAsymmErrors* g1    = PlotTool::Get_Graph(fileName, graphName_1);
  TGraphAsymmErrors* g2    = PlotTool::Get_Graph(fileName, graphName_2);
  TGraphAsymmErrors* g3    = PlotTool::Get_Graph(fileName, graphName_3);

  // -- 1st example: GraphCanvas: same syntax with HistCanvas
  Example_GraphCanvas(g1, g2, g3);

  // -- 2nd example: GraphCanvaswRatio: same syntax with HistCanvaswRatio
  Example_GraphCanvaswRatio(g1, g2, g3);
}

void Example_GraphCanvas(TGraphAsymmErrors* g1, TGraphAsymmErrors* g2, TGraphAsymmErrors* g3) {
  PlotTool::GraphCanvas* canvas = new PlotTool::GraphCanvas("c_GraphCanvas", 0, 0);
  canvas->SetTitle("titleX", "titleY");

  canvas->Register(g1, "g1", kRed);
  canvas->Register(g2, "g2", kBlue);
  canvas->Register(g3, "g3", kGreen+2);

  // canvas->SetMarkerSize(2.0);

  canvas->SetLegendPosition(0.70, 0.70, 0.95, 0.95);

  // canvas->SetRangeX(minX, maxX);
  // canvas->SetRangeY(minY, maxY);
  canvas->SetAutoRangeY();

  canvas->Latex_CMSInternal();
  canvas->RegisterLatex(0.16, 0.91, 42, 0.6, "Gaussian distributions"); // -- arguments: x, y, font type, font size, text
  // canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Gaussian distributions}}"); // -- same with above

  // canvas->SetSavePath("./output"); // -- path to be saved (default: "./")

  canvas->Draw();

  canvas->SetFormat(".png");
  canvas->Draw();

  canvas->SetRangeX(-1.0, 1.0);
  canvas->SetCanvasName( canvas->GetCanvasName() + "_zoomIn" );
  canvas->SetFormat(".pdf");
  canvas->Draw(); // -- draw the same plot with zoom-in in x axis
}

void Example_GraphCanvaswRatio(TGraphAsymmErrors* g1, TGraphAsymmErrors* g2, TGraphAsymmErrors* g3) {
  PlotTool::GraphCanvaswRatio* canvas = new PlotTool::GraphCanvaswRatio("c_GraphCanvaswRatio", 0, 0);
  canvas->SetTitle("titleX", "titleY", "g2(3)/g1");

  canvas->Register(g1, "g1", kRed);
  canvas->Register(g2, "g2", kBlue);
  canvas->Register(g3, "g3", kGreen+2);

  canvas->SetLegendPosition(0.70, 0.70, 0.95, 0.95);

  // canvas->SetRangeX(minX, maxX);
  // canvas->SetRangeY(minY, maxY);
  // canvas->SetRangeRatio(minRatio, maxRatio);
  canvas->SetAutoRangeY();

  canvas->Latex_CMSInternal();
  canvas->RegisterLatex(0.16, 0.91, 42, 0.6, "Gaussian distributions"); // -- arguments: x, y, font type, font size, text
  // canvas->RegisterLatex(0.16, 0.91, "#font[42]{#scale[0.6]{Gaussian distributions}}"); // -- same with above

  // canvas->RemoveRatioError(); // -- remove error in the ratio (useful when the error is meaningless)

  // canvas->SetSavePath("./output"); // -- path to be saved (default: "./")

  canvas->Draw();

  canvas->SetFormat(".png");
  canvas->Draw();
}