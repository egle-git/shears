#if !defined(__CINT__) || defined(__MAKECINT__)
#include <iostream>
using std::cout;
using std::endl;

#include "TRandom.h"
#include "TH1D.h"
#include "TH2D.h"

#include "TArrayD.h"
#include "TMatrixD.h"
#include "RooUnfoldResponse.h"
#include "RooUnfoldBayes.h"
#include "RooUnfoldBinByBin.h"
#include "RooUnfoldSvd.h"
#include "RooUnfoldTUnfold.h"
#endif
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
   htop->GetYaxis()->SetTitleOffset(1.5);  

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
void colorIt(TH1D *hMidA, int kCyan, int style ){
	hMidA->SetMarkerColor(kCyan);
	hMidA->SetLineColor(kCyan);
	hMidA->SetMarkerStyle(style);//24
        //hMidA->SetMarkerSize(0.10);
	hMidA->SetFillColor(kCyan);
	hMidA->SetFillStyle(0);	
        hMidA->GetXaxis()->SetLabelSize(0.12);
	hMidA->GetXaxis()->SetTitleSize(0.11);
        hMidA->GetYaxis()->SetTitleSize(0.11);
	hMidA->GetYaxis()->SetTitleOffset(0.9);
	hMidA->GetYaxis()->SetLabelSize(0.12);
	hMidA->SetTitleFont(42, "XYZ");
        hMidA->SetLabelFont(42, "XYZ");
        }
void colorBG(TH1D *hMidBG, int kBG ){
        hMidBG->SetLineWidth(0.0);
        hMidBG->SetMarkerColor(kBG);
//        hMidBG->SetFillColor(kBG);
        hMidBG->SetLineColor(kBG);
        }

void normalizeTH2D(TH2D *h)
{
   int xbin(h->GetNbinsX()), ybin(h->GetNbinsY());
   for (int i(1); i <= ybin; i++){
      double sum(0.);
      for (int j(1); j <= xbin; j++){
         sum += h->GetBinContent(j,i);
      }
      for (int j(1); j <= xbin; j++){
         if (sum > 0){
double val = h->GetBinContent(j,i)/sum;
val=val*100;
val=ceil(val);
double nearest=val/100;
h->SetBinContent(j,i,nearest);

         }
      }
   }
}

void      macro_tom_response(

                   //std::string var1="__HISTNAME__",
		   //char *Xtitle="__TITLE__",
		   int rebin = 1,
                   bool setlogx=false,
                   int qcd_sub=0,
                   int isMu=0,
                   int ZPTweightcorr=0,                   
                   int JES=0
                   //bool is3d= __IS3D__,
		   //std::string var3d="__3DEND__",
		  
 )
{
   gROOT->Reset();

	    std::string varData="n_jet_30_unfold";std::string varMC="n_jet_30_Gen_VS_reco";std::string varGen="n_jet_30_gen";char *Xtitle="N_{jet}";char *Ytitle="d#sigma/dN_{jet}";	    int plot=1; bool setlogres=false; int niter=3;
            //1st pT jet//
//	    std::string varData="jetpt_1";std::string varMC="jet1pt_Gen_VS_reco";std::string varGen="Gjetpt_1";char *Xtitle="p_{T}^{1st} [GeV]";char *Ytitle="d#sigma/dp_{T}^{1st}";int plot=2;bool   setlogres=true; int niter=8;if(isMu==1)niter=6;
	    //2nd pT jet//
//	    std::string varData="jetpt_2";std::string varMC="jet2pt_Gen_VS_reco";std::string varGen="Gjetpt_2";char *Xtitle="p_{T}^{2nd} [GeV]";char *Ytitle="d#sigma/dp_{T}^{2nd}";int plot=3;bool   setlogres=true; int niter=8; if(isMu==0)niter=6;
	    //1st Y jet//
//	    std::string varData="jetY_1";std::string varMC="jet1Y_Gen_VS_reco";std::string varGen="GjetY_1";char *Xtitle="#eta^{1st}";char *Ytitle="d#sigma/d#eta^{1st}";int plot=4; bool setlogres=false; int niter=4;
	    //2nd Y jet//
//	    std::string varData="jetY_2";std::string varMC="jet2Y_Gen_VS_reco";std::string varGen="GjetY_2";char *Xtitle="#eta^{2nd}";char *Ytitle="d#sigma/d#eta^{2nd}";int plot=5; bool setlogres=false; int niter=5;




 int no_ranges= 1;
bool unf1=true;//bayes
bool unf2=false;//svd
bool unf3=false;//bin by bin

#ifdef __CINT__
  gSystem->Load("libRooUnfold");
#endif
   gStyle->SetOptStat(kFALSE);
   gStyle->SetOptTitle(kFALSE);
   gROOT->SetBatch(kTRUE);
    TH1::SetDefaultSumw2();

//	double lumi=19777;
	double lumi=19601;
    if(isMu==1)lumi=19470.;
     int num_files=16;
    double scale_array[17];
    Double_t data_scale=1/lumi;

    //Double_t Mc_scale= 3531.5/74212048.;

     Double_t Mc_scale= 3531.5/30387292.;
    scale_array[0]=1.;
   // scale_array[1]=3531.5*lumi/74212048.; //MC
    scale_array[1]=3531.5*lumi/30387292.; //MC
 //   Double_t Mc_scale= 3531.5*lumi/74212048.;
    scale_array[2]=245*lumi/6923735.;//tt
    //Double_t tt_scale= 234*lumi/6923735.;
    scale_array[3]=54.838*0.10608*lumi/1928399.;//WWlNu
    //Double_t wwjLNu_scale= 54.838*0.10608*lumi/1928399.;
    scale_array[4]=33.21*0.068258*lumi/3215983.;//WZLQ
    //Double_t wzjLQ_scale= 33.21*0.068258*lumi/3215983.;
    scale_array[5]=33.21*0.032887*lumi/2017975.;//WZLNu
    //Double_t wzjLNu_scale=33.21*0.032887*lumi/2017975.;
 
    scale_array[6]=8.258*0.04039*lumi/964911.;//ZZLNU
   // Double_t zzjLNu_scale= 8.258*0.04039*lumi/964911.;
    scale_array[7]=8.258*0.010196*lumi/4807886.;//ZZL
   // Double_t zzjL_scale= 8.258*0.14118*lumi/4807886.;
    scale_array[8]=8.258*0.14118*lumi/1936727.;//ZZLQ
   // Double_t zzjLQ_scale= 8.258*0.010196*lumi/1936727.;
    scale_array[9]=3.79*lumi/259960.;
    //Double_t t_s_scale  = 3.79*lumi/259960.;
    scale_array[10]= 11.1*lumi/497657.;
    //Double_t t_tw_scale =11.1*lumi/497657.;
    scale_array[11]=56.4*lumi/3681614.;
    //Double_t t_t_scale =56.4*lumi/3681614.;
    scale_array[12]=1.76*lumi/139974.;
    //Double_t tbar_s_scale =1.76*lumi/139974.;
    scale_array[13]=11.1*lumi/482704.;
    //Double_t tbar_tw_scale =11.1*lumi/482704.;
    scale_array[14]= 30.7*lumi/1923607.;
    //Double_t tbar_t_scale =30.7*lumi/1923607.;
    scale_array[15]=36864.*lumi/16777216.;
    //Double_t wjets_scale =36864.*lumi/16777216.;

                   std::string filnam[]={"DoubleElec","DYJetstoLL_inclonly","tt","wwj2L2Nu","wzj2L2Q","wzj3LNu","zzj2L2Nu","zzj4L","zzj2L2Q","T_s","T_tW","T_t","Tbar_s","Tbar_tW","Tbar_t","WJets"};
                   char nam[5000];
  		   TFile *farray[17][2];
                   TH1D *histarray[17][100][2];
                   int ptthr=20;


	       TString var;
  TString var1_[100];
  TString var2_[100];
  TString var3_[100];
               for(const int file_index=0;file_index<num_files;file_index++){
                 for(int ss_index=0;ss_index<2;ss_index++){    
   		   for(int i=0 ; i<1;i++){
  		     //if(i==6)continue;
   		     var1_[i]= varData;
                     //if(is3d) var1_[i]+=(var3d);
   		     //var1_[i]+=(i);
                     //if(is3d) var1_[i]+="_";

   		     cout<<file_index<<endl;
   		     var= var1_[i];
     		     if(file_index==0)sprintf(nam,"../MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_ZPTweightcorr_%d_14_01.root",filnam[file_index].c_str(),isMu,1,ss_index,ptthr,0,JES,ZPTweightcorr);
                     if(file_index==1)sprintf(nam,"../MD_DYJetstoLL_inclonlyisMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_ZPTweightcorr_%d_14_01.root",isMu,1,ss_index,ptthr,0,0,ZPTweightcorr);
                     if(file_index>1)sprintf(nam,"../MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_ZPTweightcorr_%d_14_01.root",filnam[file_index].c_str(),isMu,1,ss_index,ptthr,0,0,ZPTweightcorr);
                     //cout<<nam<<endl;
  		     farray[file_index][ss_index]  = TFile::Open(nam);
                     histarray[file_index][i][ss_index]  =(TH1D*)(farray[file_index][ss_index]->Get(var));
                     histarray[file_index][i][ss_index]->Rebin(rebin);
                     if((file_index>1)||(file_index==1&&ss_index==1))histarray[file_index][i][ss_index]->Scale(scale_array[file_index]); 
    		    }
                  }  
	        }
  sprintf(nam,"../MD_Sherpa200_unweighted_doUnf_1_isSS_0_jetPt_%d_Tightjet_0_15_12.root",ptthr);
  TFile *sherpa = TFile::Open(nam);

  TH1D *mc_bg_ss[100];
  TH1D *tot_ss[100];
  TH1D *mc_bg[100];
  TH1D *data[100];
  TH1D *mc[100];
  TH1D *Mont[100];
  TH1D *Mont2[100];
  TH1D* hUnf1[100];
  TH1D* hUnf2[100];
  TH1D* hUnf3[100];
  TH1D *hGen[100];
  TH1D *hSherpa[100];
  double MaxPlot=0;  
  double MinPlot=999.;  
  for(int i=0 ; i<1;i++){

    hGen[i]     =(TH1D*)(farray[1][0]->Get(varGen.data()));
    hSherpa[i]  =(TH1D*)(sherpa->Get(varGen.data()));

    TTree *tree1 = (TTree *)sherpa->Get("tree");

    double MyWeight1;

    tree1->SetBranchAddress("cnt0",&MyWeight1);

    tree1->GetEntry(0);


    mc_bg_ss[i]= (TH1D*)histarray[1][i][1]->Clone("mc_bg_ss");
    for(int file_index=2;file_index<num_files;file_index++){
      mc_bg_ss[i]->Add(histarray[file_index][i][1]);
    }

    tot_ss[i]= (TH1D*)histarray[0][i][1]->Clone("tot_ss");
    tot_ss[i]->Add(mc_bg_ss[i],-1.0);


    mc_bg[i]= (TH1D*)histarray[2][i][0]->Clone("mc_bg");
    for(int file_index=3;file_index<num_files;file_index++){
      mc_bg[i]->Add(histarray[file_index][i][0]);
    }
      if(qcd_sub==1)mc_bg[i]->Add(tot_ss[i]);

    data[i]= (TH1D*) histarray[0][i][0]->Clone();
   // data[i]->Add(mc_bg[i],-1.0);
    mc[i]= (TH1D*) histarray[1][i][0]->Clone();

 cout << "==================================== TRAIN ====================================" << endl;
   TH1D *hRec   =(TH1D*)(farray[1][0]->Get(var));
   TH1D *hgen   =(TH1D*)(farray[1][0]->Get(varGen.data()));
   TH2D *hMat   =(TH2D*)(farray[1][0]->Get(varMC.data())); 

   TFile *tom =TFile::Open("DE_8TeV_DYJets_UNFOLDING_dR_5311_Inf3_EffiCorr_0_TrigCorr_1_Syst_0_JetPtMin_30_VarWidth.root");

   RooUnfoldResponse response;
   response= (RooUnfoldResponse)tom->Get("responseZNGoodJets_Zexc");
   TFile *tomD =TFile::Open("DE_8TeV_Data_dR_5311_EffiCorr_1_TrigCorr_1_Syst_0_JetPtMin_30_VarWidth.root");
   TH1D* hTom;
   hTom = (TH1D*)(tomD->Get("ZNGoodJets_Zexc"));
   //hTom->Add(mc_bg[i],-1.0);
   for(int ii =1;ii<=hTom->GetNbinsX()+1;ii++){
     cout<<hTom->GetBinContent(ii)-mc_bg[i]->GetBinContent(ii)<<"  "<<hTom->GetBinContent(ii)<<"  "<<mc_bg[i]->GetBinContent(ii)<<"  "<<ii<<endl;
     hTom->SetBinContent(ii,hTom->GetBinContent(ii)-mc_bg[i]->GetBinContent(ii));
   }
   //RooUnfoldResponse response (hRec, hgen, hMat); 
   response.UseOverflow();
    cout << "==================================== UNFOLD ===================================" << endl;
	RooUnfoldBayes   unfold1 (&response, hTom, niter);    // OR
	RooUnfoldSvd     unfold2 (&response, hTom,3 );   // OR

	RooUnfoldBinByBin unfold3 (&response, hTom);


//	RooUnfoldBayes   unfold1 (&response,  data[i], niter);    // OR
//	RooUnfoldSvd     unfold2 (&response,  data[i],3 );   // OR

//	RooUnfoldBinByBin unfold3 (&response,  data[i]);            
    hUnf1[i]= (TH1D*) unfold1.Hreco();
    hUnf2[i]= (TH1D*) unfold2.Hreco();
    hUnf3[i]= (TH1D*) unfold3.Hreco();

        hUnf1[i]->Scale(data_scale);
        hUnf2[i]->Scale(data_scale);
        hUnf3[i]->Scale(data_scale);
        hGen[i]->Scale(Mc_scale);
        mc[i]->Scale(Mc_scale);
        data[i]->Scale(data_scale);
        hSherpa[i]->Scale(1/MyWeight1);

 

    double bin = data[i]->GetNbinsX();
    for(int bb=1;bb<=bin;bb++){

      double acc = hUnf2[i]->GetBinWidth(bb) ;
     double scale_data =  data[i]->GetBinContent(bb)/acc;
     double scale_mc =  mc[i]->GetBinContent(bb)/acc;
     double scale_data_err =  data[i]->GetBinError(bb)/acc;
     double scale_mc_err =  mc[i]->GetBinError(bb)/acc;
      double scale_gen =  hGen[i]->GetBinContent(bb)/acc;
      double scale_unf1 = hUnf1[i]->GetBinContent(bb)/acc;
      double scale_unf2 = hUnf2[i]->GetBinContent(bb)/acc;
      double scale_unf3 = hUnf3[i]->GetBinContent(bb)/acc;
      double scale_sherpa =  hSherpa[i]->GetBinContent(bb)/acc;
      double scale_gen_err =  hGen[i]->GetBinError(bb)/acc;
      double scale_unf_err1 = hUnf1[i]->GetBinError(bb)/acc;
      double scale_unf_err2 = hUnf2[i]->GetBinError(bb)/acc;
      double scale_unf_err3 = hUnf3[i]->GetBinError(bb)/acc;
      double scale_sherpa_err =  hSherpa[i]->GetBinError(bb)/acc;
      data[i]->SetBinContent(bb,scale_data);
      data[i]->SetBinError(bb,scale_data_err);
      mc[i]->SetBinContent(bb,scale_mc);
      mc[i]->SetBinError(bb,scale_mc_err);
      hGen[i]->SetBinContent(bb,scale_gen);
      hGen[i]->SetBinError(bb,scale_gen_err);
      hUnf1[i]->SetBinContent(bb,scale_unf1);
      hUnf1[i]->SetBinError(bb,scale_unf_err1);
      hUnf2[i]->SetBinContent(bb,scale_unf2);
      hUnf2[i]->SetBinError(bb,scale_unf_err2);
      hUnf3[i]->SetBinContent(bb,scale_unf3);
      hUnf3[i]->SetBinError(bb,scale_unf_err3);
      hSherpa[i]->SetBinContent(bb,scale_sherpa);
      hSherpa[i]->SetBinError(bb,scale_sherpa_err);
    }


     hSherpa[i]->SetFillColor(kCyan);
    double a = hGen[i]->GetMaximum(100000000000000000000000000000);
    double b = hUnf2[i]->GetMaximum(1000000000000000000000000000000);   

    double a2 = hGen[i]->GetMinimum(0.000000000000000000000000000001);
    double b2 = hUnf2[i]->GetMinimum(0.000000000000000000000000000001);   
//    cout<<mc_bg[i]->GetMaximum(10000000000000))*6.<<endl;

    double MaxPlot_mc = a;
    double MaxPlot_data = b;
    double MinPlot_mc = a2;
    double MinPlot_data = b2;
    double MaxPlot_l =0;
    double MinPlot_l =0;

    if(MaxPlot_mc>MaxPlot_data) MaxPlot_l = MaxPlot_mc;
    if(MaxPlot_mc<MaxPlot_data) MaxPlot_l = MaxPlot_data;

   if(MinPlot_mc>MinPlot_data) MinPlot_l = MinPlot_data;
    if(MinPlot_mc<MinPlot_data) MinPlot_l = MinPlot_mc;

    if(MaxPlot_l>MaxPlot) MaxPlot = MaxPlot_l;
    if(MinPlot_l<MinPlot) MinPlot = MinPlot_l;
   // cout<<MinPlot_l<<"  "<<MinPlot<<endl;
if(unf1){ 
    Mont[i]=(TH1D*)hGen[i]->Clone("Mont[i]");
    Mont[i]->Divide(hGen[i], hUnf1[i],1.0,1.0);
}

if(unf2){ 
    Mont[i]=(TH1D*)hGen[i]->Clone("Mont[i]");
    Mont[i]->Divide(hGen[i], hUnf2[i],1.0,1.0);
}

if(unf3){ 
    Mont[i]=(TH1D*)hGen[i]->Clone("Mont[i]");
    Mont[i]->Divide(hGen[i], hUnf3[i],1.0,1.0);
}

    Mont2[i]=(TH1D*)hGen[i]->Clone("Mont2[i]");
    Mont2[i]->Divide(hGen[i], data[i],1.0,1.0);

  // for(int j =0; j<bin;j++)cout<<Mont2[i]->GetBinContent(j)<<endl;
  // cout<<endl;
  TCanvas *c3 = new TCanvas("c3","",900, 900);

  TPaveText *pt = new TPaveText(.09,.925,.70,0.98);
pt->SetFillColor(0);
   pt->SetBorderSize(0);
   pt->SetLineColor(0);
   pt->AddText("CMS Preliminary #sqrt{s} = 8 TeV, #int L dt = 19.8 fb^{-1} ");

pt->Draw();
  //char nam[50];
    gPad->SetTopMargin(0.02);
    gPad->SetBottomMargin(0.11);
    gPad->SetRightMargin(0.02);
  normalizeTH2D(hMat);
  sprintf(nam,"Reco %s",Xtitle);
  hMat->SetXTitle(nam);
  sprintf(nam,"Gen %s",Xtitle);
  hMat->GetYaxis()->SetTitleOffset(1.5);
  hMat->SetYTitle(nam);
  if(setlogres)gPad->SetLogx();
  if(setlogres)gPad->SetLogy();
  gPad->SetLogz();
  hMat->Draw("COLZTEXT");
  sprintf(nam,"histos_note_2201/Response_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.C",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
  c3->Print(nam);
  sprintf(nam,"histos_note_2201/Response_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.pdf",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
  c3->Print(nam);
  sprintf(nam,"histos_note_2201/Response_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.png",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
  c3->Print(nam);

  }

    //TLegend *leg1 = new TLegend(0.68,0.713719,0.880511,0.965385);
     TLegend *leg1 = new TLegend(0.15,0.15,0.400511,0.355385);
    //leg1 = new TLegend(0.831567,0.653719,0.980511,1.025385);
    leg1->SetBorderSize(0);
    leg1->SetFillColor(kWhite);
    leg1->SetTextSize(0.028);

    //TLegend *leg2 = new TLegend(0.68,0.713719,0.880511,0.965385);
   //  TLegend *leg2 = new TLegend(0.15,0.15,0.400511,0.355385);
    //leg1 = new TLegend(0.831567,0.653719,0.980511,1.025385);
   // leg2->SetBorderSize(0);
    //leg2->SetFillColor(kWhite);
    //leg2->SetTextSize(0.028);


//  double pt_range[6]={20.,30.,50.,100.,200.,1000.};

//  if(unf1 ){

   //cout<<"Burdayım ulenn"<<endl;

/*
    if(var1.find("ljet_pt_y")!= std::string::npos){
      sprintf(nam,"Bayes %.1lf<|#eta|<%.1lf (*10^{%d})",j_y_range[ii],j_y_range[ii+1],no_ranges-ii-1);
      leg1->AddEntry(hUnf1[ii], nam,"pe");
      //sprintf(nam,"Gen %.1lf<|#eta|<%.1lf (*10^{%d})",j_y_range[ii],j_y_range[ii+1],no_ranges-ii-1);
      //leg1->AddEntry(hGen[ii], nam,"l");
      sprintf(nam,"Data %.1lf<|#eta|<%.1lf (*10^{%d})",j_y_range[ii],j_y_range[ii+1],no_ranges-ii-1);
      leg2->AddEntry(data[ii], nam,"pe");
      //sprintf(nam,"MC %.1lf<|#eta|<%.1lf (*10^{%d})",j_y_range[ii],j_y_range[ii+1],no_ranges-ii-1);
      //leg2->AddEntry(mc[ii], nam,"l");
    }

    if(var1.find("ljet_pt_sljetpt")!= std::string::npos){
     cout<<"Burdayım ulenn"<<endl;
      sprintf(nam,"Bayes %d<P_T(j_2)<%d (*10^{%i})",j_pt_range[ii],j_pt_range[ii+1],no_ranges-ii-1);
      leg1->AddEntry(hUnf1[ii], nam,"pe");
      //sprintf(nam,"Gen %d<P_T(j_2)<%d (*10^{%i})",j_pt_range[ii],j_pt_range[ii+1],no_ranges-ii-1);
      //leg1->AddEntry(hGen[ii], nam,"l");
     sprintf(nam,"Data %d<P_T(j_2)<%d (*10^{%i})",j_pt_range[ii],j_pt_range[ii+1],no_ranges-ii-1);
      leg2->AddEntry(data[ii], nam,"pe");
      //sprintf(nam,"MC %d<P_T(j_2)<%d (*10^{%i})",j_pt_range[ii],j_pt_range[ii+1],no_ranges-ii-1);
      //leg2->AddEntry(mc[ii], nam,"l");
    }

*/

  leg1->AddEntry(hGen[0], "Truth","pel");
  leg1->AddEntry(data[0], "Measured","pel");
  leg1->AddEntry(hUnf1[0], "Unfold Bayes","pel");
  //leg1->AddEntry(hUnf2[0], "Unfold SVD","pel");
  //leg1->AddEntry(hUnf3[0], "Unfold Bin-by-Bin","pel");
   
    leg2 = new TLegend(0.811567,0.853719,0.930511,0.965385);
        leg2->SetBorderSize(0);
        leg2->SetFillColor(kWhite);
        leg2->SetTextSize(0.05);
        leg2->AddEntry(Mont2[0], "Truth/Measured","p");
	leg2->AddEntry(Mont[0], "Truth/Unfold Bayes","p");



  TCanvas *c1 = new TCanvas("c1","",700, 900);
 
   TPaveText *pt = new TPaveText(.09,.925,.70,0.98);
   pt->SetFillColor(0);
   pt->SetBorderSize(0);
   pt->SetLineColor(0);
   pt->AddText("CMS Preliminary #sqrt{s} = 8 TeV, #int L dt = 19.8 fb^{-1} ");

   pt->Draw();
  c1->cd(); 
  splithist(0.2);
  c1->cd(1); 

    gPad->SetLogy();


  if(setlogx)gPad->SetLogx();

  for(int i=0; i<1;i++){
    

    colorIt(hUnf1[i],kGreen,i+20);
    colorIt(hUnf2[i],kBlue,i+20);
    colorIt(hUnf3[i],kOrange+8,i+20);
    colorIt(data[i],kRed,i+20);

//    colorBG(hGen[i],((i+1)*12));
    hGen[i]->SetLineWidth(1.5);
    hGen[i]->SetLineColor(kBlack);
    if(i==0){
      hGen[i]->Draw("hhist");
      hGen[i]->SetMaximum(MaxPlot*10);
      hGen[i]->SetMinimum(MinPlot/100);
      //hGen[i]->SetXTitle(Xtitle); 
//hGen[i]->GetYaxis()->SetTitleOffset(0.5);

hGen[i]->SetYTitle(Ytitle);


    }
    else{
      hGen[i]->Draw("hhistsame");

    }
 //hSherpa[i]->Draw("e2same");

if(unf1)    hUnf1[i]->Draw("eX0Csame");
if(unf2)    hUnf2[i]->Draw("eX0Csame");
if(unf3)    hUnf3[i]->Draw("eX0Csame");

data[i]->Draw("eX0Csame");
 
  leg1->Draw("same");
  c1->cd(2);
    if(setlogx)gPad->SetLogx();
      //Mont[i]->GetYaxis()->SetNdivisions(0.1);
      //gStyle.SetGridWidth(5);
      gPad->SetGridy();
      colorIt(Mont[i],kGreen,i+20);
      Mont[i]->SetMinimum(0.5);
      Mont[i]->SetMaximum(2.5);
      Mont[i]->GetYaxis()->SetLabelSize(0.09);
      Mont[i]->GetXaxis()->SetLabelSize(0.09);
      Mont[i]->Draw("peX0C");
      Mont[i]->SetXTitle(Xtitle); 

      colorIt(Mont2[i],kRed,i+20);
      Mont2[i]->Draw("peX0Csame");
      //cout<<Mont2[i]->GetBinContent(1);
      Mont[i]->GetYaxis()->SetTitleOffset(0.5);
      Mont[i]->GetYaxis()->SetTitle("Ratio");
      leg2->Draw("same");
     fixsplithist(hGen[i],Mont[i]);
 }

  sprintf(nam,"histos_note_2201/Bayes_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.C",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
  c1->Print(nam);
  sprintf(nam,"histos_note_2201/Bayes_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.pdf",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
  c1->Print(nam);
  sprintf(nam,"histos_note_2201/Bayes_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.png",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
  c1->Print(nam);
/*
  TCanvas *c3 = new TCanvas("c3","",700, 900);
  c3->cd(); 
  TPaveText *pt = new TPaveText(.09,.925,.70,0.98);
pt->SetFillColor(0);
   pt->SetBorderSize(0);
   pt->SetLineColor(0);
   pt->AddText("CMS Preliminary #sqrt{s} = 8 TeV, #int L dt = 19.8 fb^{-1} ");

pt->Draw();

    gPad->SetTopMargin(0.02);
    gPad->SetBottomMargin(0.11);
    gPad->SetRightMargin(0.02);
  gPad->SetLogy();
  if(setlogx)gPad->SetLogx();

  for(int i=0; i<1;i++){
//    if(1==8 && i==6)continue;

    colorIt(data[i],kBlack,i+20);

    //colorBG(mc[i],((i+1)*12));
    mc[i]->SetLineWidth(1.5);
mc[i]->SetLineColor(kBlack);
    if(i==0){
      mc[i]->Draw("hhist");
      mc[i]->SetMaximum(MaxPlot*10);
      mc[i]->SetMinimum(MinPlot/100);
      mc[i]->SetXTitle(Xtitle);
      mc[i]->GetYaxis()->SetTitleOffset(1.5); 
      mc[i]->GetYaxis()->SetTitle(Ytitle);

    }
    else{
      mc[i]->Draw("hhistsame");
    }
     data[i]->Draw("eX0Csame");
  }
  leg2->Draw("same");

  sprintf(nam,"histos_note_2201/Detector_Level_%s_ismu_%d_QCDsubtracted_%d.C",varData.c_str(),isMu,qcd_sub);
  c3->Print(nam);
  sprintf(nam,"histos_note_2201/Detector_Level_%s_ismu_%d_QCDsubtracted_%d.pdf",varData.c_str(),isMu,qcd_sub);
  c3->Print(nam);
  sprintf(nam,"histos_note_2201/Detector_Level_%s_ismu_%d_QCDsubtracted_%d.png",varData.c_str(),isMu,qcd_sub);
  c3->Print(nam);
*/
  ofstream myfile;
  sprintf(nam,"histos_note_2201/Bayes_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.tex",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
  myfile.open (nam);
 if(plot==1){  
 myfile<<"\\begin{table}[hbtp]"<<endl;
  myfile<<"\\begin{center}"<<endl;
if(isMu==0)myfile<<"\\caption{Number of jets electron channel}"<<endl;
if(isMu==1)myfile<<"\\caption{Number of jets muon channel}"<<endl;
  myfile<<"\\begin{tabular}{|c|c|c|} \\hline"<<endl;
  myfile<<"Njet & $d \\sigma / d(NJet)$ & stat  \\\\ \\hline "<<endl;
	  for(int i=1;i<=hUnf1[0]->GetNbinsX();i++){
	    myfile<<i-1<<" &"<<hUnf1[0]->GetBinContent(i)<<" &"<<hUnf1[0]->GetBinError(i)<<"\\\\"<<endl;
	  }
  myfile<<"\\hline"<<endl;
  myfile<<"\\end{tabular}" <<endl;
  myfile<<"\\label{tab:njetunf}" <<endl;
     myfile<<"\\end{center}" <<endl;
  myfile<<"\\end{table}" <<endl;
  }
 double jetPt_Zinc1jet[23] = {20, 24, 30, 39, 49, 60, 72, 85, 100, 117, 136, 157, 187, 220, 258, 300, 350, 400, 450, 500, 590, 700, 1000};
  double jetPt_Zinc2jet[22] = {20, 24, 30, 39, 49, 60, 72, 85, 100, 117, 136, 157, 187, 220, 258, 300, 350, 400, 450, 500, 590, 800};

  if(plot==2){
 myfile<<"\\begin{table}[hbtp]"<<endl;
  myfile<<"\\begin{center}"<<endl;
if(isMu==0)myfile<<"\\caption{Leading jet pt electron channel}"<<endl;
if(isMu==1)myfile<<"\\caption{Leading jet pt muon channel}"<<endl;
  myfile<<"\\begin{tabular}{|c|c|c|} \\hline"<<endl;
  myfile<<"jet pt range & $d \\sigma / dPT$ & stat \\\\ \\hline "<<endl;
	  for(int i=1;i<=hUnf1[0]->GetNbinsX();i++){
	    myfile<<jetPt_Zinc1jet[i-1]<<"-"<<jetPt_Zinc1jet[i]<<" &"<<hUnf1[0]->GetBinContent(i)<<" &"<<hUnf1[0]->GetBinError(i)<<"\\\\"<<endl;
	  }
  myfile<<"\\hline"<<endl;
   myfile<<"\\end{tabular}" <<endl;
  myfile<<"\\label{tab:jet1PTunf}" <<endl;
     myfile<<"\\end{center}" <<endl;
  myfile<<"\\end{table}" <<endl;
  }
  if(plot==3){
 myfile<<"\\begin{table}[hbtp]"<<endl;
  myfile<<"\\begin{center}"<<endl;
if(isMu==0)myfile<<"\\caption{Second leading jet pt electron channel}"<<endl;
if(isMu==1)myfile<<"\\caption{Second leading jet pt muon channel}"<<endl;
  myfile<<"\\begin{tabular}{|c|c|c|} \\hline"<<endl;
  myfile<<"jet pt range & $d \\sigma / dPT$ & stat \\\\ \\hline "<<endl;
	  for(int i=1;i<=hUnf1[0]->GetNbinsX();i++){
	    myfile<<jetPt_Zinc2jet[i-1]<<"-"<<jetPt_Zinc2jet[i]<<" &"<<hUnf1[0]->GetBinContent(i)<<" &"<<hUnf1[0]->GetBinError(i)<<"\\\\"<<endl;
	  }
  myfile<<"\\hline"<<endl;
  myfile<<"\\end{tabular}" <<endl;
  myfile<<"\\label{tab:jet2PTunf}" <<endl;
     myfile<<"\\end{center}" <<endl;
  myfile<<"\\end{table}" <<endl;

  }
  if(plot==4 || plot==5){
  myfile<<"\\begin{table}[hbtp]"<<endl;
  myfile<<"\\begin{center}"<<endl;
if(isMu==0){
if(plot==4)  myfile<<"\\caption{Leading jet Y electron channel}"<<endl;
if(plot==5)  myfile<<"\\caption{Second leading jet Y electron channel}"<<endl;
}
if(isMu==1){
if(plot==4)  myfile<<"\\caption{Leading jet Y muon channel}"<<endl;
if(plot==5)  myfile<<"\\caption{Second leading jet Y muon channel}"<<endl;
}
  myfile<<"\\begin{tabular}{|c|c|c|} \\hline"<<endl;
  myfile<<"jet rapidity range & $d \\sigma / dY$ & stat \\\\ \\hline "<<endl;

  double low=0;
  double high=0;
	  for(int i=1;i<=hUnf1[0]->GetNbinsX();i++){
	    if(i<13){
	      low=-1*(2.6-(i*0.2));
	      high=low+0.2;
	      if(i==12) high=0;
	  }
	   if(i>12){
	     low=((i-13)*0.2);
	      high=low+0.2;
	  } 
	    myfile<<low<<" - "<<high<<" & "<<hUnf1[0]->GetBinContent(i)<<" &"<<hUnf1[0]->GetBinError(i)<<"\\\\"<<endl;
	  }
  myfile<<"\\hline"<<endl;
  myfile<<"\\end{tabular}" <<endl;
 if(plot==4)    myfile<<"\\label{tab:jet1Yunf}" <<endl;
 if(plot==5)    myfile<<"\\label{tab:jet2Yunf}" <<endl;
     myfile<<"\\end{center}" <<endl;
  myfile<<"\\end{table}" <<endl;
  }
   myfile.close();


}

