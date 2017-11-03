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

void      macro_xsec_1D_1012(

			     //std::string var1="__HISTNAME__",
			     //char *Xtitle="__TITLE__",
			     int rebin = 1,
			     bool setlogx=false,
			     int qcd_sub=0,
			     int isMu=0,
			     int ZPTweightcorr=0,                   
			     int JES=0,
			     bool pullTest=false
			     //bool is3d= __IS3D__,
			     //std::string var3d="__3DEND__",
		  
			     )
{
  gROOT->Reset();

 // std::string varData="n_jet_30_unfold";std::string varMC="n_jet_30_Gen_VS_reco";std::string varGen="n_jet_30_gen";char *Xtitle="N_{jet}";char *Ytitle="d#sigma/dN_{jet}";	    int plot=1; bool setlogres=false; int niter=3;
  //1st pT jet//
// std::string varData="jetpt_1";std::string varMC="jet1pt_Gen_VS_reco";std::string varGen="Gjetpt_1";char *Xtitle="p_{T}^{1st} [GeV]";char *Ytitle="d#sigma/dp_{T}^{1st}";int plot=2;bool   setlogres=true; int niter=9;if(isMu==1)niter=7;
  //2nd pT jet//
  //std::string varData="jetpt_2";std::string varMC="jet2pt_Gen_VS_reco";std::string varGen="Gjetpt_2";char *Xtitle="p_{T}^{2nd} [GeV]";char *Ytitle="d#sigma/dp_{T}^{2nd}";int plot=3;bool   setlogres=true; int niter=7; if(isMu==1)niter=9;
  //1st Y jet//
//  	    std::string varData="jetY_1";std::string varMC="jet1Y_Gen_VS_reco";std::string varGen="GjetY_1";char *Xtitle="#eta^{1st}";char *Ytitle="d#sigma/d#eta^{1st}";int plot=4; bool setlogres=false; int niter=6;if(isMu)niter=5;
  //2nd Y jet//
  std::string varData="jetY_2";std::string varMC="jet2Y_Gen_VS_reco";std::string varGen="GjetY_2";char *Xtitle="#eta^{2nd}";char *Ytitle="d#sigma/d#eta^{2nd}";int plot=5; bool setlogres=false; int niter=6;




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
  double lumi=19701;
  if(isMu==1)lumi=19545.;
  int num_files=16;
  int sys_no=12;
  if(pullTest)num_files=100;
  double scale_array[17];
  Double_t data_scale=1/lumi;

  //Double_t Mc_scale= 3531.8/74212048.;

  Double_t Mc_scale= 3531.8/30459425.;

  if(pullTest){
    data_scale=1;
    Mc_scale= 7./680.;

  }

  scale_array[0]=1.;
  // scale_array[1]=3531.5*lumi/74212048.; //MC
  scale_array[1]=3531.5*lumi/30459425.; //MC
  //   Double_t Mc_scale= 3531.5*lumi/74212048.;
  scale_array[2]=245*lumi/6923735.;//tt
  //Double_t tt_scale= 234*lumi/6923735.;
  scale_array[3]=54.838*0.10608*lumi/1928399.;//WWlNu
  //Double_t wwjLNu_scale= 54.838*0.10608*lumi/1928399.;
  scale_array[4]=33.21*0.068258*lumi/3215983.;//WZLQ
  //Double_t wzjLQ_scale= 33.21*0.068258*lumi/3215983.;
  scale_array[5]=33.21*0.032887*lumi/2017975.;//WZLNu
  //Double_t wzjLNu_scale=33.21*0.032887*lumi/2017975.;
 
  scale_array[6]=17.654*0.04039*lumi/964911.;//ZZLNU
  // Double_t zzjLNu_scale= 8.258*0.04039*lumi/964911.;
  scale_array[7]=17.654*0.010196*lumi/4807886.;//ZZL
  // Double_t zzjL_scale= 8.258*0.14118*lumi/4807886.;
  scale_array[8]=17.654*0.14118*lumi/1936727.;//ZZLQ
  // Double_t zzjLQ_scale= 8.258*0.010196*lumi/1936727.;

  /*   scale_array[6]=8.258*0.04039*lumi/964911.;//ZZLNU
  // Double_t zzjLNu_scale= 8.258*0.04039*lumi/964911.;
  scale_array[7]=8.258*0.010196*lumi/4807886.;//ZZL
  // Double_t zzjL_scale= 8.258*0.14118*lumi/4807886.;
  scale_array[8]=8.258*0.14118*lumi/1936727.;//ZZLQ
  // Double_t zzjLQ_scale= 8.258*0.010196*lumi/1936727.;
  */    scale_array[9]=3.79*lumi/259960.;
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
  //scale_array[15]=36864.*lumi/16777216.;
  //scale_array[15]=0.;
  Double_t wjets_scale =36864.*lumi/16777216.;
  //Double_t wjets_scale =0.;
  std::string filnam[]={"DoubleElec","DYJetstoLL","tt","wwj2L2Nu","wzj2L2Q","wzj3LNu","zzj2L2Nu","zzj4L","zzj2L2Q","T_s","T_tW","T_t","Tbar_s","Tbar_tW","Tbar_t","WJets"};
  char nam[5000];
  TFile *farray[200][200];
  TH1D *histarray[200][200][200];
  int ptthr=20;





  TString var;
  TString var1_[100];
  TString var2_[100];
  TString var3_[100];
  for(const int file_index=0;file_index<num_files;file_index++){
    for(int ss_index=0;ss_index<sys_no;ss_index++){    
      if(pullTest && ss_index>0)continue;
      for(int i=0 ; i<1;i++){

	//if(i==6)continue;
	var1_[i]= varData;
	//if(is3d) var1_[i]+=(var3d);
	//var1_[i]+=(i);
	//if(is3d) var1_[i]+="_";

	cout<<file_index<<endl;
	var= var1_[i];
     if(!pullTest){
	if(ss_index==1){
	  sprintf(nam,"../rootfiles/fake_ss.root"); //same sign fake not used!!
	}
	  if(file_index==0){
	    if(ss_index==0 ){
		sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_-1_18_05.root",filnam[file_index].c_str(),isMu,1,ss_index,ptthr,0,JES,ZPTweightcorr);
	    }
	    else if(ss_index==2 ){
	      sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_-1_18_05.root",filnam[file_index].c_str(),isMu,1,0,ptthr,0,1,ZPTweightcorr);
	    }//JES up
	    else if(ss_index==3 ){
	      sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_-1_18_05.root",filnam[file_index].c_str(),isMu,1,0,ptthr,0,-1,ZPTweightcorr);//JES down
	    }  
	  } 
	
	if(file_index==1){
	  if (ss_index==0){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_-1_18_05.root",isMu,1,ss_index,ptthr,0,0,ZPTweightcorr);
	  }
	  else if (ss_index==8){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_1_PU_0_ZPTweightcorr_%d_CT10ind_-1_18_05.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//JER+
	  }
	  else if(ss_index==9){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_-1_PU_0_ZPTweightcorr_%d_CT10ind_-1_18_05.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//JER-
	  }
	  else if(ss_index==10){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_1_ZPTweightcorr_%d_CT10ind_-1_18_05.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//PU+
	  }
	  else if (ss_index==11){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_-1_ZPTweightcorr_%d_CT10ind_-1_18_05.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//PU-
	  }
        }
	if(file_index>1&&ss_index!=1){
	  sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_-1_18_05.root",filnam[file_index].c_str(),isMu,1,0,20,0,0,ZPTweightcorr); 

        }    
      }

	if(pullTest){
	  if(file_index==1){
	    sprintf(nam,"../rootfiles/Pulltest_0_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_0_CT10ind_-1_18_05.root",isMu,1,ss_index,ptthr,0,JES);
	  }
	  else if(file_index==0){
	    sprintf(nam,"../rootfiles/Pulltest_%disMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_0_CT10ind_-1_18_05.root",1,isMu,1,ss_index,ptthr,0,JES);
	  }	
	  else sprintf(nam,"../rootfiles/Pulltest_%disMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_0_CT10ind_-1_18_05.root",file_index,isMu,1,ss_index,ptthr,0,JES);
	}

	cout<<nam<<endl;
	farray[file_index][ss_index]  = TFile::Open(nam);
	histarray[file_index][i][ss_index]  =(TH1D*)(farray[file_index][ss_index]->Get(var));
	histarray[file_index][i][ss_index]->Rebin(rebin);
	if(!pullTest){
                    
	  if((file_index>1)&&ss_index==4)histarray[file_index][i][ss_index]->Scale(scale_array[file_index]*1.026);//lumi+ 2.6%
	  else if((file_index>1)&&ss_index==5)histarray[file_index][i][ss_index]->Scale(scale_array[file_index]*0.974);//lumi-2.6%
	  else if((file_index>1)&&ss_index==6){
	    if(file_index==2)histarray[file_index][i][ss_index]->Scale(scale_array[file_index]*1.1);//xsec +10% ttbar
	    if(file_index>2 &&file_index<9)histarray[file_index][i][ss_index]->Scale(scale_array[file_index]*1.5);//xsec +15% VV                       
	  }
	  else if((file_index>1)&&ss_index==7){
	    if(file_index==2)histarray[file_index][i][ss_index]->Scale(scale_array[file_index]*0.9);//xsec -10% ttbar
	    if(file_index>2 &&file_index<9)histarray[file_index][i][ss_index]->Scale(scale_array[file_index]*0.5);//xsec -15% VV                       
	  }
	  else if((file_index>1)||(file_index==1&&ss_index==1))histarray[file_index][i][ss_index]->Scale(scale_array[file_index]); 
	}

      }
    }  
  }

  //cout<<"BURDA MI LA"<<endl;
  sprintf(nam,"../rootfiles/MD_Sherpa200_unweighted_isMu_0_doUnf_1_isSS_0_jetPt_%d_Tightjet_0_JES_0_JER_0_PU_0_ZPTweightcorr_0_CT10ind_-1_15_05.root",ptthr);
  TFile *sherpa = TFile::Open(nam);
  double SFeff=0.02;
  double unc[1][100][100];
  double unctot[1][100];
  TH1D *h_pull[1000];
  TH1D *h_pull_all= new TH1D ("h_pull_all","h_pull_all", 20, -3.,3.);
  for(int bb =0;bb<1000;bb++){
    sprintf(nam,"Pull_%i",bb);
    h_pull[bb] = new TH1D (nam,nam, 20, -3.,3.);}
  TH1D *mc_bg_ss[100];
  TH1D *tot_ss[100];
  TH1D *mc_bg[100][100];
  TH1D *data[100][100];
  TH1D *mc[100];
  TH1D *Mont[100][100];
  TH1D *Mont2[100];
  TH1D* hUnf1[100][100];
  TH1D* hUnf2[100][100];
  TH1D* hUnf3[100][100];
  TH1D *hGen[100];
  TH1D *hSherpa[100];
  TH1D *datatot;
  double MaxPlot=0;  
  double MinPlot=999.;  
  int nnn=1;
  if(pullTest)nnn=num_files;
  for(int i=0 ; i<1;i++){
    /*   data_tot=  (TH1D*) histarray[0][i][0]->Clone();
	 for(int jjj=2;jjj<=num_files;jjj++){
	 data_tot->Add(histarray[jjj][i][0]);
	 }*/
    for(int jjj=0;jjj<nnn;jjj++){
      if(pullTest && jjj==1)continue;

      hGen[i]     =(TH1D*)(farray[1][0]->Get(varGen.data()));
      hSherpa[i]  =(TH1D*)(sherpa->Get(varGen.data()));

      TTree *tree1 = (TTree *)sherpa->Get("tree");

      double MyWeight1;

      tree1->SetBranchAddress("cnt0",&MyWeight1);

      tree1->GetEntry(0);

      if (!pullTest){	mc_bg_ss[i]= (TH1D*)histarray[1][i][1]->Clone("mc_bg_ss");
	for(int file_index=2;file_index<num_files;file_index++){
	  mc_bg_ss[i]->Add(histarray[file_index][i][1]);
	}

	tot_ss[i]= (TH1D*)histarray[0][i][1]->Clone("tot_ss");
	tot_ss[i]->Add(mc_bg_ss[i],-1.0);
      
      
	mc_bg[i][0]= (TH1D*)histarray[2][i][0]->Clone("mc_bg");

	mc_bg[i][4]= (TH1D*)histarray[2][i][4]->Clone("mc_bg");//Lumi +
	mc_bg[i][5]= (TH1D*)histarray[2][i][5]->Clone("mc_bg");//Lumi -
	mc_bg[i][6]= (TH1D*)histarray[2][i][6]->Clone("mc_bg");//xsec +
	mc_bg[i][7]= (TH1D*)histarray[2][i][7]->Clone("mc_bg");//xsec -
	for(int file_index=3;file_index<num_files;file_index++){
	  mc_bg[i][0]->Add(histarray[file_index][i][0]);
	  mc_bg[i][4]->Add(histarray[file_index][i][4]);
	  mc_bg[i][5]->Add(histarray[file_index][i][5]);
	  mc_bg[i][6]->Add(histarray[file_index][i][6]);
	  mc_bg[i][7]->Add(histarray[file_index][i][7]);
	}
	if(qcd_sub==1)mc_bg[i][0]->Add(tot_ss[i]);
	for(int jj=1;jj<=mc_bg[i][0]->GetNbinsX();jj++)cout<<jj<<"  "<<mc_bg[i][0]->GetBinContent(jj)<<endl;
      }

      data[i][0]= (TH1D*) histarray[jjj][i][0]->Clone();
      if (!pullTest){	data[i][2]= (TH1D*) histarray[jjj][i][2]->Clone();//JES up
	data[i][3]= (TH1D*) histarray[jjj][i][3]->Clone();//JES down

	data[i][4]= (TH1D*) histarray[jjj][i][0]->Clone();
	data[i][5]= (TH1D*) histarray[jjj][i][0]->Clone();

	data[i][6]= (TH1D*) histarray[jjj][i][0]->Clone();
	data[i][7]= (TH1D*) histarray[jjj][i][0]->Clone();

	data[i][8]= (TH1D*) histarray[jjj][i][0]->Clone();
	data[i][9]= (TH1D*) histarray[jjj][i][0]->Clone();
	data[i][10]= (TH1D*) histarray[jjj][i][0]->Clone();
	data[i][11]= (TH1D*) histarray[jjj][i][0]->Clone();




	data[i][0]->Add(mc_bg[i][0],-1.0);

	data[i][2]->Add(mc_bg[i][0],-1.0);//SUBRTRACT BG - NOT YET JES UP & DOWN ON MC
	data[i][3]->Add(mc_bg[i][0],-1.0);
	data[i][4]->Add(mc_bg[i][4],-1.0);//SUBRTRACT xsec Lumi +||-
	data[i][5]->Add(mc_bg[i][5],-1.0);
	data[i][6]->Add(mc_bg[i][6],-1.0);//SUBRTRACT xsec Lumi +||-
	data[i][7]->Add(mc_bg[i][7],-1.0);

	data[i][8]->Add(mc_bg[i][0],-1.0);//JER +
	data[i][9]->Add(mc_bg[i][0],-1.0);//JER -
	data[i][10]->Add(mc_bg[i][0],-1.0);//PU+
	data[i][11]->Add(mc_bg[i][0],-1.0);//PU-
        
	//cout<<"ALADASDASDASDASDA  "<< mc_bg[i][0]->GetBinContent(1)<<"   "<<mc_bg[i][4]->GetBinContent(1)<<"   "<<mc_bg[i][5]->GetBinContent(1)<<"    "<<mc_bg[i][6]->GetBinContent(1)<<"   "<<mc_bg[i][7]->GetBinContent(1)<<endl<<endl; 
      }
      mc[i]= (TH1D*) histarray[1][i][0]->Clone();

      cout << "==================================== TRAIN ====================================" << endl;
      cout<<farray[1][0]<<"  "<<var<<" "<<varGen<<" "<<varMC<<endl<<endl;
      TH1D *hRec   =(TH1D*)(farray[1][0]->Get(var));
      TH1D *hgen   =(TH1D*)(farray[1][0]->Get(varGen.data()));
      TH2D *hMat   =(TH2D*)(farray[1][0]->Get(varMC.data())); 
      if(!pullTest){
	TH1D *hRecJERup   =(TH1D*)(farray[1][8]->Get(var));
	TH1D *hgenJERup   =(TH1D*)(farray[1][8]->Get(varGen.data()));
	TH2D *hMatJERup   =(TH2D*)(farray[1][8]->Get(varMC.data())); 

	TH1D *hRecJERdown   =(TH1D*)(farray[1][9]->Get(var));
	TH1D *hgenJERdown   =(TH1D*)(farray[1][9]->Get(varGen.data()));
	TH2D *hMatJERdown   =(TH2D*)(farray[1][9]->Get(varMC.data())); 

	TH1D *hRecPUup   =(TH1D*)(farray[1][10]->Get(var));
	TH1D *hgenPUup   =(TH1D*)(farray[1][10]->Get(varGen.data()));
	TH2D *hMatPUup   =(TH2D*)(farray[1][10]->Get(varMC.data())); 

	TH1D *hRecPUdown   =(TH1D*)(farray[1][11]->Get(var));
	TH1D *hgenPUdown   =(TH1D*)(farray[1][11]->Get(varGen.data()));
	TH2D *hMatPUdown   =(TH2D*)(farray[1][11]->Get(varMC.data())); 
      }



      cout<<"TEEEEEEEEEEEESSSSSSTTT  "<<i<<endl;
      for(int iiii=1;iiii<=hRec->GetNbinsX();iiii++)
	{
	  if(hRec->GetBinContent(iiii)>0)cout<<" AAAAAA "<<hRec->GetBinContent(iiii)/hgen->GetBinContent(iiii)<<endl;
	}
      RooUnfoldResponse response (hRec, hgen, hMat); 
      if(!pullTest){
	RooUnfoldResponse responseJERup (hRecJERup, hgenJERup, hMatJERup); 
	RooUnfoldResponse responsePUup (hRecPUup, hgenPUup, hMatPUup); 
	RooUnfoldResponse responseJERdown (hRecJERdown, hgenJERdown, hMatJERdown); 
	RooUnfoldResponse responsePUdown (hRecPUdown, hgenPUdown, hMatPUdown);                  
      }
      response.UseOverflow();
      if(!pullTest){
	responseJERup.UseOverflow();
	responsePUup.UseOverflow();
	responseJERdown.UseOverflow();
	responsePUdown.UseOverflow();
      }
      cout << "==================================== UNFOLD ===================================" <<i<<" "<<niter[i]<< endl;

      RooUnfoldBayes   unfold1 (&response, data[i][0], niter[i]);    // OR
      RooUnfoldSvd     unfold2 (&response, data[i][0], 3);   // OR
      RooUnfoldBinByBin unfold3 (&response, data[i][0]);



      if(!pullTest){
	//JESUP

	RooUnfoldBayes   unfold1JESup (&response, data[i][2], niter[i]);    // OR
	RooUnfoldSvd     unfold2JESup (&response, data[i][2], 3);   // OR
	RooUnfoldBinByBin unfold3JESup (&response, data[i][2]);

	//JESDOWN

	RooUnfoldBayes   unfold1JESdown (&response, data[i][3], niter[i]);    // OR
	RooUnfoldSvd     unfold2JESdown (&response, data[i][3], 3);   // OR
	RooUnfoldBinByBin unfold3JESdown (&response, data[i][3]);
 
	//LUMIUP

	RooUnfoldBayes   unfold1Lumiup (&response, data[i][4], niter[i]);    // OR
	RooUnfoldSvd     unfold2Lumiup (&response, data[i][4], 3);   // OR
	RooUnfoldBinByBin unfold3Lumiup (&response, data[i][4]);
	//LUMIDOWN

	RooUnfoldBayes   unfold1Lumidown (&response, data[i][5], niter[i]);    // OR
	RooUnfoldSvd     unfold2Lumidown (&response, data[i][5], 3);   // OR
	RooUnfoldBinByBin unfold3Lumidown (&response, data[i][5]);

	//XSECUP

	RooUnfoldBayes   unfold1Xsecup (&response, data[i][6], niter[i]);    // OR
	RooUnfoldSvd     unfold2Xsecup (&response, data[i][6], 3);   // OR
	RooUnfoldBinByBin unfold3Xsecup (&response, data[i][6]);
	//XSECDOWN

	RooUnfoldBayes   unfold1Xsecdown (&response, data[i][7], niter[i]);    // OR
	RooUnfoldSvd     unfold2Xsecdown (&response, data[i][7], 3);   // OR
	RooUnfoldBinByBin unfold3Xsecdown (&response, data[i][7]);                  

	RooUnfoldBayes   unfold1JERup (&responseJERup, data[i][8], niter[i]);    // OR
	RooUnfoldSvd     unfold2JERup (&responseJERup, data[i][8], 3);   // OR
	RooUnfoldBinByBin unfold3JERup (&responseJERup, data[i][8]);


	RooUnfoldBayes   unfold1JERdown (&responseJERdown, data[i][9], niter[i]);    // OR
	RooUnfoldSvd     unfold2JERdown (&responseJERdown, data[i][9], 3);   // OR
	RooUnfoldBinByBin unfold3JERdown (&responseJERdown, data[i][9]);


	RooUnfoldBayes   unfold1PUup (&responsePUup, data[i][10], niter[i]);    // OR
	RooUnfoldSvd     unfold2PUup (&responsePUup, data[i][10], 3);   // OR
	RooUnfoldBinByBin unfold3PUup (&responsePUup, data[i][10]);



	RooUnfoldBayes   unfold1PUdown (&responsePUdown, data[i][11], niter[i]);    // OR
	RooUnfoldSvd     unfold2PUdown (&responsePUdown, data[i][11], 3);   // OR
	RooUnfoldBinByBin unfold3PUdown (&responsePUdown, data[i][11]);

      }

      cout<<"BİİİİİİİİR"<<endl;
      cout<<jjj<<endl;
      hUnf1[i][0]= (TH1D*) unfold1.Hreco();
      hUnf2[i][0]= (TH1D*) unfold2.Hreco();
      hUnf3[i][0]= (TH1D*) unfold3.Hreco();


      if(!pullTest){   
	cout<<"İKİİİİİİİİ"<<endl;
	hUnf1[i][2]= (TH1D*) unfold1JESup.Hreco();
	hUnf2[i][2]= (TH1D*) unfold2JESup.Hreco();
	hUnf3[i][2]= (TH1D*) unfold3JESup.Hreco();     
	cout<<"ÜÜÜÜÜÜÜÇ"<<endl;
	hUnf1[i][3]= (TH1D*) unfold1JESdown.Hreco();
	hUnf2[i][3]= (TH1D*) unfold2JESdown.Hreco();
	hUnf3[i][3]= (TH1D*) unfold3JESdown.Hreco();
	cout<<"DÖÖÖÖÖÖRRT"<<endl;
	hUnf1[i][4]= (TH1D*) unfold1Lumiup.Hreco();
	hUnf2[i][4]= (TH1D*) unfold2Lumiup.Hreco();
	hUnf3[i][4]= (TH1D*) unfold3Lumiup.Hreco();     
	cout<<"BEEEEEEEŞ"<<endl;
	hUnf1[i][5]= (TH1D*) unfold1Lumidown.Hreco();
	hUnf2[i][5]= (TH1D*) unfold2Lumidown.Hreco();
	hUnf3[i][5]= (TH1D*) unfold3Lumidown.Hreco();
	cout<<"ALTIIIII"<<endl;
	hUnf1[i][6]= (TH1D*) unfold1Xsecup.Hreco();
	hUnf2[i][6]= (TH1D*) unfold2Xsecup.Hreco();
	hUnf3[i][6]= (TH1D*) unfold3Xsecup.Hreco();     
	cout<<"YEDİİİİİİ"<<endl;
	hUnf1[i][7]= (TH1D*) unfold1Xsecdown.Hreco();
	hUnf2[i][7]= (TH1D*) unfold2Xsecdown.Hreco();
	hUnf3[i][7]= (TH1D*) unfold3Xsecdown.Hreco(); 
	cout<<"SEKİİİİİİİz"<<endl;
	hUnf1[i][8]= (TH1D*) unfold1JERup.Hreco();
	hUnf2[i][8]= (TH1D*) unfold2JERup.Hreco();
	hUnf3[i][8]= (TH1D*) unfold3JERup.Hreco();     
	cout<<"DOKUUUUUUz"<<endl;
	hUnf1[i][9]= (TH1D*) unfold1JERdown.Hreco();
	hUnf2[i][9]= (TH1D*) unfold2JERdown.Hreco();
	hUnf3[i][9]= (TH1D*) unfold3JERdown.Hreco(); 
	cout<<"OOOOOONN"<<endl;
	hUnf1[i][10]= (TH1D*) unfold1PUup.Hreco();
	hUnf2[i][10]= (TH1D*) unfold2PUup.Hreco();
	hUnf3[i][10]= (TH1D*) unfold3PUup.Hreco();     
	cout<<"ONBİİİİİİRR"<<endl;
	hUnf1[i][11]= (TH1D*) unfold1PUdown.Hreco();
	hUnf2[i][11]= (TH1D*) unfold2PUdown.Hreco();
	hUnf3[i][11]= (TH1D*) unfold3PUdown.Hreco(); 

	hUnf1[i][2]->Scale(data_scale);
	hUnf2[i][2]->Scale(data_scale);
	hUnf3[i][2]->Scale(data_scale);

	hUnf1[i][3]->Scale(data_scale);
	hUnf2[i][3]->Scale(data_scale);
	hUnf3[i][3]->Scale(data_scale);

	hUnf1[i][4]->Scale(data_scale/1.026);
	hUnf2[i][4]->Scale(data_scale/1.026);
	hUnf3[i][4]->Scale(data_scale/1.026);

	hUnf1[i][5]->Scale(data_scale/0.974);
	hUnf2[i][5]->Scale(data_scale/0.974);
	hUnf3[i][5]->Scale(data_scale/0.974);


	hUnf1[i][6]->Scale(data_scale);
	hUnf2[i][6]->Scale(data_scale);
	hUnf3[i][6]->Scale(data_scale);

	hUnf1[i][7]->Scale(data_scale);
	hUnf2[i][7]->Scale(data_scale);
	hUnf3[i][7]->Scale(data_scale);

	hUnf1[i][8]->Scale(data_scale);
	hUnf2[i][8]->Scale(data_scale);
	hUnf3[i][8]->Scale(data_scale);

	hUnf1[i][9]->Scale(data_scale);
	hUnf2[i][9]->Scale(data_scale);
	hUnf3[i][9]->Scale(data_scale);

	hUnf1[i][10]->Scale(data_scale);
	hUnf2[i][10]->Scale(data_scale);
	hUnf3[i][10]->Scale(data_scale);

	hUnf1[i][11]->Scale(data_scale);
	hUnf2[i][11]->Scale(data_scale);
	hUnf3[i][11]->Scale(data_scale);

        }
      hUnf1[i][0]->Scale(data_scale);
      hUnf2[i][0]->Scale(data_scale);
      hUnf3[i][0]->Scale(data_scale);

      hGen[i]->Scale(Mc_scale);
      mc[i]->Scale(Mc_scale);
      data[i][0]->Scale(data_scale);
      hSherpa[i]->Scale(1/MyWeight1);

      cout<<"ALLAH AŞKIONA BURDA OLSUN"<<endl;

      double bin = data[i][0]->GetNbinsX();

      for(int bb=1;bb<=bin;bb++){
	if(pullTest)cout<<"DEBUG "<<data[i][0]->Integral()<<" "<<hUnf1[i][0]->GetBinContent(bb)<<"	"<<hgen->GetBinContent(bb)<<endl;
	if(pullTest)cout<<"PULLLLLLL "<<(hUnf1[i][0]->GetBinContent(bb)-hgen->GetBinContent(bb))/hUnf1[i][0]->GetBinError(bb)<<endl;

	// cout<<jjj<<"asdasdasdasdad"<<endl;

        if(pullTest)h_pull[bb-1]->Fill((hUnf1[i][0]->GetBinContent(bb)-hgen->GetBinContent(bb))/hUnf1[i][0]->GetBinError(bb));
        if(pullTest &&bb<=2)h_pull_all->Fill((hUnf1[i][0]->GetBinContent(bb)-hgen->GetBinContent(bb))/hUnf1[i][0]->GetBinError(bb));
	if (!pullTest){
	  double acc = hUnf1[i][0]->GetBinWidth(bb) ;
	  double scale_data =  data[i][0]->GetBinContent(bb)/acc;
	  double scale_mc =  mc[i]->GetBinContent(bb)/acc;
	  double scale_data_err =  data[i][0]->GetBinError(bb)/acc;
	  double scale_mc_err =  mc[i]->GetBinError(bb)/acc;
	  double scale_gen =  hGen[i]->GetBinContent(bb)/acc;
	  double scale_sherpa =  hSherpa[i]->GetBinContent(bb)/acc;
	  double scale_gen_err =  hGen[i]->GetBinError(bb)/acc;
	  double scale_sherpa_err =  hSherpa[i]->GetBinError(bb)/acc;
	  data[i][0]->SetBinContent(bb,scale_data);
	  data[i][0]->SetBinError(bb,scale_data_err);
	  hSherpa[i]->SetBinContent(bb,scale_sherpa);
	  hSherpa[i]->SetBinError(bb,scale_sherpa_err);

	  mc[i]->SetBinContent(bb,scale_mc);
	  mc[i]->SetBinError(bb,scale_mc_err);
	  hGen[i]->SetBinContent(bb,scale_gen);
	  hGen[i]->SetBinError(bb,scale_gen_err);

	  for(int jjjj=0;jjjj<sys_no;jjjj++){
	    if (jjjj==1)continue;
	    double scale_unf1 = hUnf1[i][jjjj]->GetBinContent(bb)/acc;
	    double scale_unf2 = hUnf2[i][jjjj]->GetBinContent(bb)/acc;
	    double scale_unf3 = hUnf3[i][jjjj]->GetBinContent(bb)/acc;

	    double scale_unf_err1 = hUnf1[i][jjjj]->GetBinError(bb)/acc;
	    double scale_unf_err2 = hUnf2[i][jjjj]->GetBinError(bb)/acc;
	    double scale_unf_err3 = hUnf3[i][jjjj]->GetBinError(bb)/acc;
 
	    hUnf1[i][jjjj]->SetBinContent(bb,scale_unf1);
	    hUnf1[i][jjjj]->SetBinError(bb,scale_unf_err1);
	    hUnf2[i][jjjj]->SetBinContent(bb,scale_unf2);
	    hUnf2[i][jjjj]->SetBinError(bb,scale_unf_err2);
	    hUnf3[i][jjjj]->SetBinContent(bb,scale_unf3);
	    hUnf3[i][jjjj]->SetBinError(bb,scale_unf_err3);
	  }
	}
      }
      cout<<"Buraya geliyon mu qaq"<<endl;

      if (!pullTest){	for(int binn=1;binn<=hUnf1[i][0]->GetNbinsX();binn++){
	  unctot[i][binn]=0;
	  for(int jjjj=0;jjjj<sys_no;jjjj++){
	    if (jjjj==0||jjjj==1 )continue;
	    if((jjjj%2)!=0)continue;
	    double a=fabs( (hUnf1[i][jjjj]  ->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn) ));
	    double b=fabs( (hUnf1[i][jjjj+1]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn) ));


	    if(a>b)unc[i][binn][jjjj]=a / hUnf1[i][0]->GetBinContent(binn) ;
	    else unc[i][binn][jjjj]= b/ hUnf1[i][0]->GetBinContent(binn) ;
	    // cout<<i<<"  "<<binn<<"  "<<jjjj<<"  "<<a<<"  "<<b<<" "<<sqrt(unctot[i][binn])<<endl;
	    unctot[i][binn] += pow(unc[i][binn][jjjj],2);
	    cout<<"sysind "<<jjjj<<" % "<< unc[i][binn][jjjj]<<endl;
	  }

	  unctot[i][binn]= sqrt(unctot[i][binn] + SFeff*SFeff /*+ pow(hUnf1[i][0]->GetBinError(binn)/hUnf1[i][0]->GetBinContent(binn),2)*/);
	  cout<<"Tot "<<unctot[i][binn]<<endl;
	}
      }


      if(pullTest)hGen[i]->Scale(1/Mc_scale);

      if (!pullTest){
  	hSherpa[i]->SetFillColor(kCyan);
	double a = hGen[i]->GetMaximum(100000000000000000000000000000);
	double b = hUnf1[i][0]->GetMaximum(1000000000000000000000000000000);   

	double a2 = hGen[i]->GetMinimum(0.000000000000000000000000000001);
	double b2 = hUnf1[i][0]->GetMinimum(0.000000000000000000000000000001);   
	//    cout<<mc_bg[i][0]->GetMaximum(10000000000000))*6.<<endl;

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
	  Mont[i][0]=(TH1D*)hGen[i]->Clone("Mont[i]");
	  Mont[i][0]->Divide(hGen[i], hUnf1[i][0],1.0,1.0);
	  Mont[i][2]=(TH1D*)hGen[i]->Clone("Mont[i]");
	  Mont[i][2]->Divide(hGen[i], hUnf1[i][2],1.0,1.0);
	  Mont[i][3]=(TH1D*)hGen[i]->Clone("Mont[i]");
	  Mont[i][3]->Divide(hGen[i], hUnf1[i][3],1.0,1.0);
	}
	/*
	  if(unf2){ 
	  Mont[i]=(TH1D*)hGen[i]->Clone("Mont[i]");
	  Mont[i]->Divide(hGen[i], hUnf2[i][0],1.0,1.0);
	  }

	  if(unf3){ 
	  Mont[i]=(TH1D*)hGen[i]->Clone("Mont[i]");
	  Mont[i]->Divide(hGen[i], hUnf3[i][0],1.0,1.0);
	  }*/

	Mont2[i]=(TH1D*)hGen[i]->Clone("Mont2[i]");
	Mont2[i]->Divide(hGen[i], data[i][0],1.0,1.0);

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
	sprintf(nam,"histos_note_1605/Response_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.C",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
	c3->Print(nam);
	sprintf(nam,"histos_note_1605/Response_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.pdf",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
	c3->Print(nam);
	sprintf(nam,"histos_note_1605/Response_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.png",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
	c3->Print(nam);

      }
    }
  }
  if (!pullTest){    //TLegend *leg1 = new TLegend(0.68,0.713719,0.880511,0.965385);
    TLegend *leg1 = new TLegend(0.15,0.15,0.400511,0.355385);
    //leg1 = new TLegend(0.831567,0.653719,0.980511,1.025385);
    leg1->SetBorderSize(0);
    leg1->SetFillColor(kWhite);
    leg1->SetTextSize(0.028);



    leg1->AddEntry(hGen[0], "Truth","pel");
    leg1->AddEntry(data[0][0], "Measured","pel");
    leg1->AddEntry(hUnf1[0][0], "Unfold Bayes","pel");
    //leg1->AddEntry(hUnf2[0], "Unfold SVD","pel");
    //leg1->AddEntry(hUnf3[0], "Unfold Bin-by-Bin","pel");
   
    leg2 = new TLegend(0.811567,0.853719,0.930511,0.965385);
    leg2->SetBorderSize(0);
    leg2->SetFillColor(kWhite);
    leg2->SetTextSize(0.05);
    leg2->AddEntry(Mont2[0], "Truth/Measured","p");
    leg2->AddEntry(Mont[0][0], "Truth/Unfold Bayes","p");



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
    

      colorIt(hUnf1[i][0],kGreen,i+20);
      colorIt(hUnf2[i][0],kBlue,i+20);
      colorIt(hUnf3[i][0],kOrange+8,i+20);
      colorIt(data[i][0],kRed,i+20);

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

      if(unf1)    hUnf1[i][0]->Draw("eX0Csame");
      if(unf2)    hUnf2[i][0]->Draw("eX0Csame");
      if(unf3)    hUnf3[i][0]->Draw("eX0Csame");

      data[i][0]->Draw("eX0Csame");
 
      leg1->Draw("same");
      c1->cd(2);
      if(setlogx)gPad->SetLogx();
      //Mont[i]->GetYaxis()->SetNdivisions(0.1);
      //gStyle.SetGridWidth(5);
      gPad->SetGridy();
      colorIt(Mont[i][0],kGreen,i+20);
      Mont[i][0]->SetMinimum(0.5);
      Mont[i][0]->SetMaximum(2.5);
      Mont[i][0]->GetYaxis()->SetLabelSize(0.09);
      Mont[i][0]->GetXaxis()->SetLabelSize(0.09);
      Mont[i][0]->Draw("peX0C");
      Mont[i][0]->SetXTitle(Xtitle); 

      colorIt(Mont2[i],kRed,i+20);
      Mont2[i]->Draw("peX0Csame");
      //cout<<Mont2[i]->GetBinContent(1);
      Mont[i][0]->GetYaxis()->SetTitleOffset(0.5);
      Mont[i][0]->GetYaxis()->SetTitle("Ratio");
      leg2->Draw("same");
      fixsplithist(hGen[i],Mont[i][0]);
    }

    sprintf(nam,"histos_note_1605/Bayes_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.C",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
    c1->Print(nam);
    sprintf(nam,"histos_note_1605/Bayes_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.pdf",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
    c1->Print(nam);
    sprintf(nam,"histos_note_1605/Bayes_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.png",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
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

      sprintf(nam,"histos_note_1605/Detector_Level_%s_ismu_%d_QCDsubtracted_%d.C",varData.c_str(),isMu,qcd_sub);
      c3->Print(nam);
      sprintf(nam,"histos_note_1605/Detector_Level_%s_ismu_%d_QCDsubtracted_%d.pdf",varData.c_str(),isMu,qcd_sub);
      c3->Print(nam);
      sprintf(nam,"histos_note_1605/Detector_Level_%s_ismu_%d_QCDsubtracted_%d.png",varData.c_str(),isMu,qcd_sub);
      c3->Print(nam);
    */


    ofstream myfile;
    sprintf(nam,"histos_note_1605/Bayes_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.tex",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
    myfile.open (nam);
    myfile.precision(4);

    myfile<<"\\begin{table}[hbtp]"<<endl;
    myfile<<"\\begin{center}"<<endl;
    //if(plot==1){  
    if(isMu==0)myfile<<"\\caption{Number of jets electron channel}"<<endl;
    if(isMu==1)myfile<<"\\caption{Number of jets muon channel}"<<endl;
    myfile<<"\\begin{tabular}{c|ccccccc} \\hline"<<endl;
    myfile<<"Njet & $d \\sigma / d(NJet)$ & stat & JES & Lumi & JER &PU& xsec \\\\ \\hline "<<endl;
    for(int i=1;i<=hUnf1[0][0]->GetNbinsX();i++){
      if(hUnf1[0][0]->GetBinContent(i)==0)continue;
      myfile<<(hUnf1[0][0]->GetBinCenter(i)-((hUnf1[0][0]->GetBinWidth(i))/2))
	    <<"-"
	    <<(hUnf1[0][0]->GetBinCenter(i)+((hUnf1[0][0]->GetBinWidth(i))/2))
	    << "  & "<<hUnf1[0][0]->GetBinContent(i)<<" &"<<hUnf1[0][0]->GetBinError(i)//Central value
	    << "  & "<<100*(hUnf1[0][2]->GetBinContent(i)-hUnf1[0][0]->GetBinContent(i))/hUnf1[0][0]->GetBinContent(i)<<"  "
	    <<100*(hUnf1[0][3]->GetBinContent(i)-hUnf1[0][0]->GetBinContent(i))/hUnf1[0][0]->GetBinContent(i)//JES

	    << "  & "<<100*(hUnf1[0][4]->GetBinContent(i)-hUnf1[0][0]->GetBinContent(i))/hUnf1[0][0]->GetBinContent(i)<<"  "
	    <<100*(hUnf1[0][5]->GetBinContent(i)-hUnf1[0][0]->GetBinContent(i))/hUnf1[0][0]->GetBinContent(i)//Lumi

	    << "  & "<<100*(hUnf1[0][8]->GetBinContent(i)-hUnf1[0][0]->GetBinContent(i))/hUnf1[0][0]->GetBinContent(i)<<"  "
	    <<100*(hUnf1[0][9]->GetBinContent(i)-hUnf1[0][0]->GetBinContent(i))/hUnf1[0][0]->GetBinContent(i)//JER

	    << "  & "<<100*(hUnf1[0][10]->GetBinContent(i)-hUnf1[0][0]->GetBinContent(i))/hUnf1[0][0]->GetBinContent(i)<<"  "
	    <<100*(hUnf1[0][11]->GetBinContent(i)-hUnf1[0][0]->GetBinContent(i))/hUnf1[0][0]->GetBinContent(i)//PU

	    << "  & "<<100*(hUnf1[0][6]->GetBinContent(i)-hUnf1[0][0]->GetBinContent(i))/hUnf1[0][0]->GetBinContent(i)<<"  "
	    <<100*(hUnf1[0][7]->GetBinContent(i)-hUnf1[0][0]->GetBinContent(i))/hUnf1[0][0]->GetBinContent(i)//xsec

	    <<"\\\\"<<endl;
    }
    myfile<<"\\hline"<<endl;
    myfile<<"\\end{tabular}" <<endl;
    myfile<<"\\label{tab:njetunf}" <<endl;
    myfile<<"\\end{center}" <<endl;
    myfile<<"\\end{table}" <<endl;

    myfile.close();

    ofstream myfile3;
    sprintf(nam,"histos_note_1605/Bayes_Symm_error_%s_ismu_%d_QCDsubtracted_%d_ZPTweightcorr_%d.tex",varData.c_str(),isMu,qcd_sub,ZPTweightcorr);
    myfile3.open (nam);
    myfile3.precision(4);

    myfile3<<"\\begin{table}[hbtp]"<<endl;
    myfile3<<"\\begin{center}"<<endl;

    if(isMu==0)myfile3<<"\\caption{Number of jets electron channel}"<<endl;
    if(isMu==1)myfile3<<"\\caption{Number of jets muon channel}"<<endl;

    myfile3<<"\\scriptsize{"<<endl;
    myfile3<<"\\begin{tabular}{c|cccccccc} \\hline"<<endl;
    myfile3<<"\\begin{tabular}{c|ccccccc} \\hline"<<endl;
    myfile3<<"Njet & $d \\sigma / d(NJet)$ & stat & JES & Lumi & JER &PU& xsec \\\\ \\hline "<<endl;

    for(int binn=1;binn<=hUnf1[0][0]->GetNbinsX();binn++){
      if(hUnf1[0][0]->GetBinContent(binn)==0)continue;
      myfile3<<(hUnf1[0][0]->GetBinCenter(binn)-((hUnf1[0][0]->GetBinWidth(binn))/2))
	     <<"-"
	     <<(hUnf1[0][0]->GetBinCenter(binn)+((hUnf1[0][0]->GetBinWidth(binn))/2))

	     << "  & "<<hUnf1[0][0]->GetBinContent(binn)
              
	     <<"  &  "<<100*hUnf1[0][0]->GetBinError(binn)/hUnf1[0][0]->GetBinContent(binn)//Central value

	     << "  & "<<100*unc[0][binn][2]//JES

	     << "  & "<<100*unc[0][binn][4]//Lumi

	     << "  & "<<100*unc[0][binn][8]//JER

	     << "  & "<<100*unc[0][binn][10]//PU

	     << "  & "<<100*unc[0][binn][6]//xsec

	     << "  & "<<100*unctot[0][binn]//tot
	     <<"\\\\"<<endl;
    }
    myfile3<<"\\hline"<<endl;
    myfile3<<"\\end{tabular}}" <<endl;
    myfile3<<"\\label{tab:njetunf}" <<endl;
    myfile3<<"\\end{center}" <<endl;
    myfile3<<"\\end{table}" <<endl;
  }
  if(pullTest){
    TCanvas *c6 = new TCanvas("c6","",700, 900);
    c6->cd();
    h_pull_all->Draw();
    sprintf(nam,"histos_note_1605/pull_all_%s_ismu_%d_ZPTweightcorr_%d_%d.pdf",varData.c_str(),isMu,ZPTweightcorr,i);
    c6->Print(nam);
    sprintf(nam,"histos_note_1605/pull_all_%s_ismu_%d_ZPTweightcorr_%d_%d.root",varData.c_str(),isMu,ZPTweightcorr,i);
    c6->Print(nam);
   for(int bbbb=0;bbbb<10;bbbb++){
      TCanvas *c5 = new TCanvas("c5","",700, 900);
      c5->cd();  
      h_pull[bbbb]->Draw();
    sprintf(nam,"histos_note_1605/pull_all_%s_ismu_%d_ZPTweightcorr_%d_%d_%d.pdf",varData.c_str(),isMu,ZPTweightcorr,i,bbbb);
      c5->Print(nam);
    sprintf(nam,"histos_note_1605/pull_all_%s_ismu_%d_ZPTweightcorr_%d_%d_%d.root",varData.c_str(),isMu,ZPTweightcorr,i,bbbb);
      c5->Print(nam);
    }



  }

}

