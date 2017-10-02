void splithist(double ratio=0.2){
  TVirtualPad* pmain = TVirtualPad::Pad();
  
  double h = 1. - pmain->GetTopMargin() - pmain->GetBottomMargin();

  double xlow = 0.; //gStyle->GetPadLeftMargin();
  double xhigh = 1.; // - gStyle->GetPadRightMargin();

  double ytop = 1.; //- gStyle->GetPadTopMargin();
  double ybot = 0.; //gStyle->GetPadBottomMargin();
  double ymid = pmain->GetBottomMargin() + ratio * h; //ybot + ratio * (ytop-ybot);
  

  double yp1bot = ymid;
  double yp2top = ymid;

  TPad* p1 = new TPad(TString(pmain->GetName()) + "_1", pmain->GetTitle(),
		      xlow, yp1bot, xhigh, ytop);
  p1->SetNumber(1);
  TPad* p2 = new TPad(TString(pmain->GetName()) + "_2", pmain->GetTitle(), 
		      xlow, ybot, xhigh, yp2top);
  p2->SetNumber(2);
  p1->SetFillStyle(4000);
  p2->SetFillStyle(4000);

  double p1h = ytop - yp1bot;
  double p2h = yp2top - ybot;
  

/*
  p1->SetTopMargin(pmain->GetTopMargin()/p1h);
  p1->SetBottomMargin((ymid-yp1bot)/p1h);
  p1->SetLeftMargin(pmain->GetLeftMargin());
  p1->SetRightMargin(pmain->GetRightMargin());
  
  p2->SetTopMargin((ymid-yp2top)/p2h);
  p2->SetBottomMargin(pmain->GetBottomMargin()/p2h);
  p2->SetLeftMargin(pmain->GetLeftMargin());
  p2->SetRightMargin(pmain->GetRightMargin());
*/
    p1->SetTopMargin(0.11);
    p1->SetBottomMargin(0.);
    p1->SetRightMargin(0.02);
 
    p2->SetTopMargin(0.);
    p2->SetBottomMargin(0.3);
    p2->SetRightMargin(0.02);



  p2->Draw();
  p1->Draw();
  pmain->Modified();

  p1->cd();
}

void fixsplithist(TH1* htop, TH1* hbot){
  TVirtualPad* pmain = TVirtualPad::Pad()->GetCanvas();
  if(!pmain) return;
  TVirtualPad* p1 = pmain->cd(1);
  TVirtualPad* p2 = pmain->cd(2);

  if(!p1 || !p2) return;
  
  double scale = p1->GetHNDC() / p2->GetHNDC();

  double s = htop->GetYaxis()->GetLabelSize() * scale;
  double ss = htop->GetYaxis()->GetLabelSize();
  hbot->GetYaxis()->SetLabelSize(s);
  htop->GetYaxis()->SetLabelSize(ss);
  s = htop->GetYaxis()->GetTitleSize() * scale;
  hbot->GetYaxis()->SetTitleSize(s);

   hbot->GetYaxis()->SetTitleOffset(0.5);  
   htop->GetYaxis()->SetTitleOffset(0.5);  

  s = htop->GetYaxis()->GetLabelOffset() * scale;

  s = htop->GetXaxis()->GetLabelSize() * scale;
  hbot->GetXaxis()->SetLabelSize(s);
  htop->GetXaxis()->SetLabelSize(0.);
  s = htop->GetXaxis()->GetTitleSize() * scale;
  hbot->GetXaxis()->SetTitleSize(s);

 

  s = htop->GetXaxis()->GetLabelOffset() * scale;
  hbot->GetXaxis()->SetLabelOffset(s);  


  hbot->GetYaxis()->SetNdivisions(5);
}
void colorIt(TH1D *hMidA, int kCyan ){
	hMidA->SetMarkerColor(kCyan);
	hMidA->SetLineColor(kCyan);
	hMidA->SetMarkerStyle(20);//24
     //   hMidA->SetMarkerSize(0.7);
	hMidA->SetFillColor(kCyan);
	hMidA->SetFillStyle(0);	
        hMidA->GetXaxis()->SetLabelSize(0.05);
	hMidA->GetXaxis()->SetTitleSize(0.06);
        hMidA->GetYaxis()->SetTitleSize(0.05);
	hMidA->GetYaxis()->SetTitleOffset(0.9);
	hMidA->GetYaxis()->SetLabelSize(0.05);
	hMidA->SetTitleFont(42, "XYZ");
        hMidA->SetLabelFont(42, "XYZ");
        }


void macro_exc_inc(){
  TFile *f1   = TFile::Open("../MD_DYJetstoLL_inclonlyisMu_0_doUnf_1_isSS_0_jetPt_20_Tightjet_0_JES_0_ZPTweightcorr_0_18_02.root");
  TFile *f2   = TFile::Open("../MD_DYJetstoLL_isMu_0_doUnf_1_isSS_0_jetPt_20_Tightjet_0_JES_0_ZPTweightcorr_0_18_02.root");

//  TH1D *data     =(TH1D*)(f1->Get("nup"));
//  TH1D *mc       =(TH1D*)(f2->Get("nup"));

  TH1D *data     =(TH1D*)(f1->Get("nup_mu_with_cuts"));
  TH1D *mc     =(TH1D*)(f1->Get("nup_mu_with_cuts"));
  Int_t DY;
  Int_t DY2;
  TTree *tree_DY = (TTree *)f1->Get("tree");
  tree_DY->SetBranchAddress("nevent",&DY);
  tree_DY->GetEntry(0);

  TTree *tree_DY2 = (TTree *)f2->Get("tree");
  tree_DY2->SetBranchAddress("nevent",&DY2);
  tree_DY2->GetEntry(0);

//    Double_t Mc_scale= 3531.5*19701./30396328.;
//    Double_t Mc_scale2= 3531.5*19701./;
//    data->Scale(Mc_scale);
//    mc->Scale(Mc_scale2);

  TH1D *Mont= (TH1D*)data->Clone("Mont");

  Mont->Divide(data,mc,1.0,1.0);

for(int i=1;i<=5;i++){
cout<<Mont->GetBinContent(i)<<",";
}

  TCanvas *c1 = new TCanvas("c1","", 700, 900);

          c1->cd(); 
	  colorIt(data,kBlack);  
          splithist(0.2);
          c1->cd(1);
          gPad->SetLogy(); 
	  mc->Draw("hhist");
	  data->SetMarkerStyle(10);
	  data->Draw("peXOCsames");

	  c1->cd(2);
          gPad->SetGridy();
	  colorIt(Mont,kBlack);
	  Mont->SetMinimum(0.5);
	  Mont->SetMaximum(1.5);
	  Mont->GetXaxis()->SetTitleOffset(0.8);
          Mont->SetYTitle("Data/MC");
	  Mont->Draw("peX0C");// rec/gen
          fixsplithist(data,Mont);

}
