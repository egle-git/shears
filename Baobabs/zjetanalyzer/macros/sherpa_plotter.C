/*void splithist(double ratio=0.2){
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
  
  p1->SetTopMargin(pmain->GetTopMargin()/p1h);
  p1->SetBottomMargin((ymid-yp1bot)/p1h);
  p1->SetLeftMargin(pmain->GetLeftMargin());
  p1->SetRightMargin(pmain->GetRightMargin());
  
  p2->SetTopMargin((ymid-yp2top)/p2h);
  p2->SetBottomMargin(pmain->GetBottomMargin()/p2h);
  p2->SetLeftMargin(pmain->GetLeftMargin());
  p2->SetRightMargin(pmain->GetRightMargin());

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
  double ss = htop->GetYaxis()->GetLabelSize() *1.5;
  hbot->GetYaxis()->SetLabelSize(s*1.5);
  htop->GetYaxis()->SetLabelSize(ss);
  s = htop->GetYaxis()->GetTitleSize() * scale;
  hbot->GetYaxis()->SetTitleSize(s);

   hbot->GetYaxis()->SetTitleOffset(0.5);  
   htop->GetYaxis()->SetTitleOffset(0.5);  

  s = htop->GetYaxis()->GetLabelOffset() * scale;

  s = htop->GetXaxis()->GetLabelSize() * scale;
  hbot->GetXaxis()->SetLabelSize(s*1.5);
  htop->GetXaxis()->SetLabelSize(0.);
  s = htop->GetXaxis()->GetTitleSize() * scale;
  hbot->GetXaxis()->SetTitleSize(s*1.5);

 

  s = htop->GetXaxis()->GetLabelOffset() * scale;
  hbot->GetXaxis()->SetLabelOffset(s);  


  hbot->GetYaxis()->SetNdivisions(5);
}*/


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
        hMidA->SetMarkerSize(0.7);
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
void colorBG(TH1D *hMidBG, int kBG ){
        hMidBG->SetLineWidth(0.0);
        hMidBG->SetMarkerColor(kBG);
        hMidBG->SetFillColor(kBG);
        hMidBG->SetLineColor(kBG);
        }

void      macro_tmp(std::string var1="__HISTNAME__",
                   std::string file1="../MD_Sherpa200_unweighted_isMu_0_doUnf_1_isSS_0_jetPt_30_Tightjet_0_JES_0_ZPTweightcorr_0_20_02.root",
//                   std::string file1="Sherpa200_2911_small.root",
                   std::string file2="../MD_DYJetstoLL_isMu_0_doUnf_1_isSS_0_jetPt_30_Tightjet_0_JES_0_ZPTweightcorr_0_20_02.root",
//                   std::string file2="../MD_DYJetstoLL_doUnf_0_isSS_0_02_12.root",
//		   std::string file2="../Sherpa_multithread_2JNLO_4JLO_3.root",
//		   std::string file1="../Sherpa_MPI.root", 
//                 std::string file2="../Sherpa_Sherpack.root",
		   char *Xtitle="__TITLE__",
		   int rebin = __NBIN__
		  
 )
{
TH1::SetDefaultSumw2();
   THStack *hs = new THStack("hs","Stacked 1D histograms");

   gStyle->SetOptStat(kFALSE);
   gStyle->SetOptTitle(kFALSE);
   gROOT->SetBatch(kTRUE);

  TFile *f1   = TFile::Open(file1.data());
  TFile *f2   = TFile::Open(file2.data());
 
  TH1D *data     =(TH1D*)(f1->Get(var1.data()));
  TH1D *mc       =(TH1D*)(f2->Get(var1.data()));


//    Double_t data_scale=1/(1.53242e+09);

 TTree *tree1 = (TTree *)f1->Get("tree");
 TTree *tree2 = (TTree *)f2->Get("tree");
double MyWeight1;
double MyWeight2;
  tree1->SetBranchAddress("cnt0",&MyWeight1);
  tree2->SetBranchAddress("cnt0",&MyWeight2);
 tree1->GetEntry(0);
 tree2->GetEntry(0);

cout<<MyWeight1<<" "<<MyWeight2<<endl;

//    Double_t Mc_scale= 3531.8/30397000.;
    Double_t Mc_scale= 3531.5/30459425.;
    Double_t data_scale= 1/MyWeight1;
//    Double_t Mc_scale= (3503.75/MyWeight2);
//    Double_t Mc_scale= 1/MyWeight2;
    data->Scale(data_scale);
    mc->Scale(Mc_scale);

 data->Rebin(rebin);
  mc->Rebin(rebin);

   double bin = data->GetNbinsX();

    mc->SetFillColor(kCyan);
    //double bin = data->GetNbinsX();

    for(int bb=1;bb<=bin;bb++){
      double acc = data->GetBinWidth(bb);
      double scale_gen =  data->GetBinContent(bb)/acc;
      double scale_unf1 = mc->GetBinContent(bb)/acc;
      double scale_gen_err =  data->GetBinError(bb)/acc;
      double scale_unf_err1 = mc->GetBinError(bb)/acc;
      data->SetBinContent(bb,scale_gen);
      data->SetBinError(bb,scale_gen_err);
      mc->SetBinContent(bb,scale_unf1);
      mc->SetBinError(bb,scale_unf_err1);


   }


    double MaxPlot_mc = (mc->GetMaximum(10000000000000))*6.;
    double MaxPlot_data = (data->GetMaximum(10000000000000))*6.;

   double MinPlot_mc = (mc->GetMinimum(0))/6;
    double MinPlot_data = (data->GetMinimum(0))/6;
    double MinPlot =0;
    if(MaxPlot_mc>MaxPlot_data) MaxPlot = MaxPlot_mc;
    else MaxPlot = MaxPlot_data;

    if(MinPlot_mc<MinPlot_data) MinPlot = MinPlot_mc;
    else MinPlot = MinPlot_data;
    cout<<MaxPlot_mc<<"  "<<MaxPlot_data<<"  "<<MaxPlot<<endl;
    cout<<MinPlot_mc<<"  "<<MinPlot_data<<"  "<<MinPlot<<endl;
    //////////////////////////

    TH1D *Mont= (TH1D*)data->Clone("Mont");


double xLowLeg(0.79), xHighLeg(0.89);
  leg1 = new TLegend(xLowLeg, 0.65, xHighLeg, 0.87);
    //leg1 = new TLegend(0.831567,0.653719,0.980511,1.025385);
    leg1->SetBorderSize(0);
    leg1->SetFillColor(kWhite);
    leg1->SetTextSize(0.03);
  leg1->SetTextFont(42);
    leg1->AddEntry(data, "Sherpa2.0.0 1JNLO 4JLO ","pe");
    leg1->AddEntry(mc, "MG ","peF");

  
//    leg2 = new TLegend(0.711567,0.853719,0.930511,1.265385);
  leg2 = new TLegend();
    leg2->SetBorderSize(0);
    leg2->SetFillColor(kWhite);
    leg2->SetTextSize(0.05);
    leg2->AddEntry(Mont, "Multithread/MPI","p");


  Mont->Divide(data,mc,1.0,1.0);

  double ntot_data=0.;
  double ntot_mc=0.;


  TCanvas *c1 = new TCanvas("c1","",600,600);

 //TPaveText *pt = new TPaveText(.0,.885,.70,0.93);
//pt->SetFillColor(0);
//   pt->SetBorderSize(0);
//   pt->SetLineColor(0);
  // pt->AddText("CMS Preliminary #sqrt{s} = 8 TeV, #int L dt = 19.8 fb^{-1} ");
//pt->Draw();
          c1->cd(); 
          splithist(0.2);
          c1->cd(1);
          gPad->SetLogy();
	  colorIt(data,kBlack);
          mc->SetMaximum(MaxPlot);
          mc->SetMinimum(MinPlot);
	  mc->Draw("e2");
	//  mc->Draw("hhistsames");
         
	  data->Draw("eX0Csames");
          leg1->Draw("same");
	  c1->cd(2);
          gStyle->SetGridWidth(300.);

	  colorIt(Mont,kBlack);
	  Mont->SetMinimum(0.5);
	  Mont->SetMaximum(1.5);
	  Mont->SetXTitle(Xtitle); 
          Mont->SetYTitle("Data/MC");
	  Mont->Draw("peX0C");
          fixsplithist(mc,Mont);


	  c1->Print("histos_sherpa_2dec/__HISTNAME__.pdf");
	  c1->Print("histos_sherpa_2dec/__HISTNAME__.png");
          c1->Print("histos_sherpa_2dec/__HISTNAME__.C");


  TCanvas *c2 = new TCanvas("c2","",600,600);

 //TPaveText *pt = new TPaveText(.0,.885,.70,0.93);
//pt->SetFillColor(0);
//   pt->SetBorderSize(0);
//   pt->SetLineColor(0);
  // pt->AddText("CMS Preliminary #sqrt{s} = 8 TeV, #int L dt = 19.8 fb^{-1} ");
//pt->Draw();
          c2->cd(); 
          splithist(0.2);
          c2->cd(1);
          //gPad->SetLogy();
	  colorIt(data,kBlack);
          mc->SetMaximum(MaxPlot/5.);
          mc->SetMinimum(0.);
	  mc->Draw("e2");
	//  mc->Draw("hhistsames");
         
	  data->Draw("eX0Csames");
          leg1->Draw("same");
	  c2->cd(2);
          gStyle->SetGridWidth(300.);

	  colorIt(Mont,kBlack);
	  Mont->SetMinimum(0.5);
	  Mont->SetMaximum(1.5);
	  Mont->SetXTitle(Xtitle); 
          Mont->SetYTitle("Data/MC");
	  Mont->Draw("peX0C");
          fixsplithist(mc,Mont);


	  c2->Print("histos_sherpa_2dec/Linear___HISTNAME__.pdf");
	  c2->Print("histos_sherpa_2dec/Linear___HISTNAME__.png");
          c2->Print("histos_sherpa_2dec/Linear___HISTNAME__.C");
}


