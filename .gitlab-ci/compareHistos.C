TFile *referenceFile;
TFile *testFile;

void makeTheComparison(TString name, TString saveDirectory){
  TH1F *ref = (TH1F*) referenceFile->Get(name);
  TH1F *test = (TH1F*) testFile->Get(name);
  if (test==0) {
    cout << "the histo " << name << "do not exist anymore: please check !" << endl;
    return;
  }

  TCanvas *c0 = new TCanvas("c0","coucou",600,800);
  TPad *pad =new TPad("haut","haut",0,0.25,1,1);
  pad->SetNumber(1);
  pad->SetGridx();
  pad->SetGridy();
  pad->SetTicky();
  pad->SetBottomMargin(0.006);
  pad->Draw();

  TPad *pad2 =new TPad("bas","bas",0,0,1,0.25);
  pad2->SetNumber(2);
  pad2->SetTopMargin(0);
  pad2->SetBottomMargin(0.3);
  pad2->SetGridx();
  pad2->SetGridy();
  pad2->Draw();

  c0->cd(1);
  ref->SetLineColor(kRed);
  ref->SetLineWidth(3);
  ref->Draw("hist");

  test->SetMarkerColor(kBlue);
  test->SetLineColor(kBlue);
  test->SetLineWidth(3);
  test->Draw("sames");

  float kolmo = ref->KolmogorovTest(test, "N");
  TLatex latex;
  latex.SetNDC();
  latex.SetTextSize(0.03);
  latex.DrawLatex(.2,.91,Form("Kolmogorov result = %f",kolmo));

  TLegend *t = new TLegend(0.5,0.5,0.85,0.65);
  t->AddEntry(ref, "reference", "l");
  t->AddEntry(test, "test version", "l");
  t->Draw();

  gPad->Update();
  TPaveStats *st = (TPaveStats*) ref->GetListOfFunctions()->FindObject("stats");
  st->SetX1NDC(0.7);
  st->SetX2NDC(0.9);
  st->SetY1NDC(0.8);
  st->SetY2NDC(0.9);
  st->SetFillColor(kRed-10);
  st->Draw();

  TPaveStats *stTest = (TPaveStats*) test->GetListOfFunctions()->FindObject("stats");
  stTest->SetX1NDC(0.7);
  stTest->SetX2NDC(0.9);
  stTest->SetY1NDC(0.7);
  stTest->SetY2NDC(0.8);
  stTest->SetFillColor(kBlue-9);
  stTest->Draw();
  c0->cd(1)->SetLogy();

  c0->cd(2);

  TH1F *ratio = (TH1F*) test->Clone("ratio");
  ratio->Sumw2();
  ratio->SetStats(0);
  ratio->Divide(test, ref, 1,1);
  ratio->SetMarkerColor(kBlack);
  ratio->SetLineColor(kBlack);
  ratio->SetLineWidth(1);
  ratio->SetMaximum(1.3);
  ratio->SetMinimum(0.7);
  ratio->SetTitle("");
  ratio->GetYaxis()->SetTitle("");
  ratio->GetYaxis()->SetLabelSize(0.08);
  ratio->GetXaxis()->SetTitleSize(0.12);
  ratio->GetXaxis()->SetLabelSize(0.1);
  ratio->GetXaxis()->SetLabelOffset(0.02);
  ratio->Draw("E1");

  if (kolmo<1) {
        cout << "found differences in plot " << name << endl;
	c0->Print(saveDirectory+"/"+name+".png");
  } 
}

void compareHistos(TString ref, TString test, TString output){
  referenceFile = new TFile(ref);
  //testFile = new TFile("output_DoubleMuon-all_0.root");
  testFile = new TFile(test);

  TIter listPlots(referenceFile->GetListOfKeys());
  TKey *keyPlot;
  while ((keyPlot = (TKey*)listPlots())) {
    TString typeObject = keyPlot->GetClassName();
    TString nomObject = keyPlot->GetTitle();
    if (nomObject.Contains("totEventInBaobab")) continue;
    makeTheComparison(nomObject, output);
  }
}
