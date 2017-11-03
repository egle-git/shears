#if !defined(__CINT__) || defined(__MAKECINT__)

#include <iostream>
#include <algorithm>
#include "TMath.h"

using std::cout;
using std::endl;
using std::max;
using std::pow;
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

void splithist(double ratio=0.){
  TVirtualPad* pmain = TVirtualPad::Pad();
  
  double h = 1. - pmain->GetTopMargin() - pmain->GetBottomMargin();

  double xlow = 0.; //gStyle->GetPadLeftMargin();
  double xhigh = 0.98; // - gStyle->GetPadRightMargin();

  double ytop = 1.; //- gStyle->GetPadTopMargin();
  double ybot = 0.05; //gStyle->GetPadBottomMargin();
  double ymid = pmain->GetBottomMargin() + ratio * h; //ybot + ratio * (ytop-ybot);
  

  double yp1bot = ymid;
  double yp2top = ymid;

  TPad* p1 = new TPad(TString(pmain->GetName()) + "_1", pmain->GetTitle(),
		      xlow, ybot, xhigh, ytop);
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
  p1->SetBottomMargin(0.1);
  p1->SetRightMargin(0.02);
 
  p2->SetTopMargin(0.);
  p2->SetBottomMargin(0.3);
  p2->SetRightMargin(0.02);



  p2->Draw();
  p1->Draw();
  pmain->Modified();

  p1->cd();
}

void colorIt(TH1D *hMidA, int kCyan, int style ){
  hMidA->SetMarkerColor(kCyan);
  hMidA->SetLineColor(kCyan);
  hMidA->SetMarkerStyle(style);//24
  //hMidA->SetMarkerSize(0.10);
  hMidA->SetFillColor(kCyan);
  hMidA->SetFillStyle(0);	
  hMidA->GetXaxis()->SetLabelSize(0.12);
  //hMidA->GetXaxis()->SetTitleSize(0.11);
  //hMidA->GetYaxis()->SetTitleSize(0.11);
  hMidA->GetYaxis()->SetTitleOffset(0.2);
  hMidA->GetYaxis()->SetLabelSize(0.12);
  //hMidA->SetTitleFont(42, "XYZ");
  //hMidA->SetLabelFont(42, "XYZ");
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

void      macro_tmp(std::string var1="__HISTNAME__",
		    char *Xtitle="__TITLE__",
		    int rebin = __NBIN__,
		    bool setlogx=__LOGX__,
		    bool is3d= __IS3D__,
		    std::string var3d="__3DEND__",
		    int isMu=1,
		    int JES=0,
		    bool pullTest=false,
		    int ZPTweightcorr=0,
                    double sum_sanity=0.,
                    double sum_sanityJESup=0.,
                    double sum_sanityJESdown=0.

		  
		    )
{
  gROOT->Reset();
  int no_ranges= __RANGES__;
  bool unf1=true;//bayes
  bool unf2=false;//svd
  bool unf3=false;//bin by bin
  //  double niter[] = {2,2,2,2,2,2,1};
  // double niter[] = {20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20};
  double niter[7]={9,7,6,4,5,4,9}; //fine bin
  double nitersh[7]={2,3,3,3,3,2,6}; //fine bin
  if(isMu==0){
  if(is3d &&var3d.find("z0")!= std::string::npos){
    cout<<"I am here"<<endl;
      niter[0]=8;
      niter[1]=6;
      niter[2]=5;
      niter[3]=3;
      niter[4]=2;
      niter[5]=5;
      niter[6]=8;

  }

  if(is3d &&var3d.find("z1")!= std::string::npos){
    cout<<"I am here too"<<endl;
      niter[0]=4;
      niter[1]=5;
      niter[2]=4;
      niter[3]=4;
      niter[4]=5;
      niter[5]=2;
      niter[6]=5;

  }
  }

 if(isMu==1){
      niter[0]=7;
      niter[1]=6;
      niter[2]=7;
      niter[3]=4;
      niter[4]=5;
      niter[5]=4;
      niter[6]=8;
  if(is3d &&var3d.find("z0")!= std::string::npos){
    cout<<"I am here"<<endl;
      niter[0]=8;
      niter[1]=5;
      niter[2]=7;
      niter[3]=2;
      niter[4]=3;
      niter[5]=5;
      niter[6]=7;

  }

  if(is3d &&var3d.find("z1")!= std::string::npos){
    cout<<"I am here too"<<endl;
      niter[0]=3;
      niter[1]=3;
      niter[2]=5;
      niter[3]=4;
      niter[4]=6;
      niter[5]=1;
      niter[6]=3;

  }
  }


double PDFu[8][15]={
{0.0234717,0.0324185,0.0386756,0.0433125,0.0467646,0.0493087,0.0513414,0.0532433,0.0552658,0.0575357,0.0600711,0.063425,0.0673205,0.0729707,0.0785479}, {0.02291,0.03158,0.0373144,0.0413749,0.04437,0.0464275,0.048162,0.049826,0.0516468,0.0538398,0.0566807,0.0602303,0.0640944,0.068504,0.074642},{0.0218692,0.0296614,0.0344241,0.0377268,0.0398315,0.0413142,0.0425967,0.0440219,0.0460046,0.0487408,0.0524162,0.0558291,0.0627105,0.0707554},{0.0203686,0.0266958,0.0302195,0.0323531,0.0336949,0.0348103,0.036128,0.0380282,0.0400696,0.0431544,0.0466808,0.0500774,0.0640701},{0.0177478,0.0224622,0.0248608,0.0263531,0.0276769,0.0293624,0.0313854,0.034102,0.0392494,0.144032,0.14639,0.156713},{0.0139845,0.0173835,0.0195211,0.0216535,0.0246931,0.0287883,0.0347309,0.0464391,0.0697117,0.0867079},
{0.0114905,0.01565,0.0219339,0.0307184,0.0414538,0.0690778,0.0943125,0.139329}
};

double PDFd[8][15]={
{0.0226199,0.030958,0.0365745,0.0406959,0.0438593,0.0462787,0.048359,0.0503212,0.0523421,0.0546899,0.0573221,0.060708,0.0646749,0.069796,0.0750188},
{0.0223037,0.0305052,0.0358938,0.0397739,0.0427233,0.0447714,0.0466544,0.0484148,0.0502672,0.0524811,0.0552936,0.0585362,0.062145,0.0660941,0.070977},
{0.0216893,0.0293399,0.0342862,0.0378441,0.0403074,0.0421294,0.0436632,0.0452204,0.0470922,0.0491248,0.0522112,0.0550742,0.0598298,0.064569},
{0.0207738,0.02773,0.032049,0.035014,0.0370775,0.0386404,0.0401988,0.0420246,0.0438771,0.0464074,0.0491356,0.0513914,0.0571785},
{0.0193511,0.0254553,0.0291269,0.0315471,0.0335283,0.0356741,0.0378885,0.0402876,0.0434266,0.0474433,0.0511187,0.0603206},
{0.01718,0.0223511,0.0255661,0.0286766,0.0322364,0.0359343,0.0399019,0.0457339,0.0560625,0.0637266},
{0.0145636,0.0205877,0.0269985,0.0337748,0.0403632,0.0508338,0.0620302,0.0863579}
};


  /*double niter[]={2,2,2,2,2,2,2,2,2,2,2, 	 	
    3,3,2,3,3,3,2,3,3,3,2,
    3,5,3,3,3,3,3,4,2,3,3,
    4,6,4,4,4,4,4,5,3,4,4,
    4,7,3,4,4,5,4,6,3,5,4,
    2,2,2,2,2,2,3,3,3,3,2,
    4,4,3,4,4,4,4,3,5,5,3};*/ //bin size bigger

#ifdef __CINT__
  gSystem->Load("libRooUnfold");
#endif
  gStyle->SetOptStat(kFALSE);
  gStyle->SetOptTitle(kFALSE);
  gROOT->SetBatch(kTRUE);
  TH1::SetDefaultSumw2();


  int num_files=16;
    //int sys_no=67;
  int sys_no=13;
  if(pullTest)num_files=101;
  double scale_array[17];

  double lumi=19701;
  if(isMu==1)lumi=19545.;

  Double_t data_scale=1./lumi;
  Double_t Mc_scale= 3531.5/30459425.;
  if(pullTest){
    data_scale=1;
    Mc_scale= 7./680.;//n_pull/n_tot (70k /6.8 m)

  }
  //       data_scale=1.;
  //       Mc_scale=1.;
  //  scale_array[1]=3531.5*lumi/74212048.; //MC
  scale_array[1]=3531.5*lumi/30459425.; //MC
  //   Double_t Mc_scale= 3531.5*lumi/30350000.;
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
  */
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


  std::string filnam[]={"DoubleElec","DYJetstoLL","tt","wwj2L2Nu","wzj2L2Q","wzj3LNu","zzj2L2Nu","zzj4L","zzj2L2Q","T_s","T_tW","T_t","Tbar_s","Tbar_tW","Tbar_t","WJets"};
  char nam[5000];
  TFile *farray[200][200];
  TH1D *histarray[200][200][200];
  int ptthr=20;
  TString var;
  TString var1_[200];
  TString var2_[200];
  TString var3_[200];
  for(const int file_index=0;file_index<num_files;file_index++){
    for(int ss_index=0;ss_index<sys_no;ss_index++){    
      if(pullTest && ss_index>0)continue;
      if(!pullTest){
	if(ss_index==1){
	  sprintf(nam,"../rootfiles/fake_ss.root"); //same sign fake not used!!
	}
	  if(file_index==0){
	    if(ss_index==0 ){
		sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_-1_22_05.root",filnam[file_index].c_str(),isMu,1,ss_index,ptthr,0,JES,ZPTweightcorr);
	    }
	    else if(ss_index==2 ){
	      sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_-1_22_05.root",filnam[file_index].c_str(),isMu,1,0,ptthr,0,1,ZPTweightcorr);
	    }//JES up
	    else if(ss_index==3 ){
	      sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_-1_22_05.root",filnam[file_index].c_str(),isMu,1,0,ptthr,0,-1,ZPTweightcorr);//JES down
	    }  
	  } 
	
	if(file_index==1){
	  if (ss_index==0){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_-1_22_05.root",isMu,1,ss_index,ptthr,0,0,ZPTweightcorr);
	  }
	  else if (ss_index==8){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_1_PU_0_ZPTweightcorr_%d_CT10ind_-1_22_05.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//JER+
	  }
	  else if(ss_index==9){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_-1_PU_0_ZPTweightcorr_%d_CT10ind_-1_22_05.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//JER-
	  }
	  else if(ss_index==10){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_1_ZPTweightcorr_%d_CT10ind_-1_22_05.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//PU+
	  }
	  else if (ss_index==11){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_-1_ZPTweightcorr_%d_CT10ind_-1_22_05.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//PU-
	  }
	  else if (ss_index==12){
	    sprintf(nam,"../rootfiles/Sherpa1.4_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_-1_07_06.root",isMu,1,0,ptthr,0,0,ZPTweightcorr);//Sherpa1.4
	  }
/*	  else if (ss_index>11){
	    sprintf(nam,"../rootfiles/MD_DYJetstoLL_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_%d_22_05.root",isMu,1,0,ptthr,0,0,ZPTweightcorr,ss_index-12);//PDF
	  }*/


        }
	if(file_index>1&&ss_index!=1){
	  sprintf(nam,"../rootfiles/MD_%s_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_%d_CT10ind_-1_22_05.root",filnam[file_index].c_str(),isMu,1,0,20,0,0,ZPTweightcorr); 

        }    
      }

      if(pullTest){
	if(file_index==1){
	  sprintf(nam,"../rootfiles/Pulltest_0_isMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_0_CT10ind_-1_22_05.root",isMu,1,ss_index,ptthr,0,JES);
	}
	else if(file_index==0){
	  sprintf(nam,"../rootfiles/Pulltest_%disMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_0_CT10ind_-1_22_05.root",1,isMu,1,ss_index,ptthr,0,JES);
	}	
	else sprintf(nam,"../rootfiles/Pulltest_%disMu_%d_doUnf_%d_isSS_%d_jetPt_%d_Tightjet_%d_JES_%d_JER_0_PU_0_ZPTweightcorr_0_CT10ind_-1_22_05.root",file_index,isMu,1,ss_index,ptthr,0,JES);
      }
      cout<<nam<<endl;
      farray[file_index][ss_index]  = TFile::Open(nam);
      for(int i=0 ; i<no_ranges;i++){

	//if(i==6)continue;
	var1_[i]= var1;
	if(is3d) var1_[i]+=(var3d);
	var1_[i]+=(i);
	//if(is3d) var1_[i]+="_";

	cout<<"BBB "<<var1_[i]<<endl;
	var= var1_[i];

	//if(is3d) var1_[i]+=(var3d);
	//var1_[i]+=(i);
	//if(is3d) var1_[i]+="_";

	cout<<file_index<<endl;
	var= var1_[i];
	histarray[file_index][i][ss_index]  =(TH1D*)(farray[file_index][ss_index]->Get(var));
	histarray[file_index][i][ss_index]->Rebin(rebin);
	if(!pullTest){
                    
	  if((file_index>1)&&ss_index==4)histarray[file_index][i][ss_index]->Scale(scale_array[file_index]*1.026);//lumi+ 2.2%
	  else if((file_index>1)&&ss_index==5)histarray[file_index][i][ss_index]->Scale(scale_array[file_index]*0.974);//lumi-2.2%
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

  sprintf(nam,"../rootfiles/MD_Sherpa200_unweighted_isMu_%d_doUnf_1_isSS_0_jetPt_%d_Tightjet_0_JES_0_JER_0_PU_0_ZPTweightcorr_0_CT10ind_-1_13_06.root",isMu,ptthr);
  //sprintf(nam,"../rootfiles/MD_Sherpa200_unweighted_combined_22_05.root");
  double sumfactor=1;
  TFile *sherpa = TFile::Open(nam);
  double unc[100][100][100];
  double unctot[100][100];
  double SFeff=0.02;
  TH1D *mc_bg_ss[100];
  TH1D *tot_ss[100];
  TH1D *mc_bg[100][100];
  //TH1D *bg_ww[100][100];
  TH1D *bg_wz[100][100];
  TH1D *bg_zz[100][100];
  //TH1D *bg_tt[100][100];
  TH1D *bg_st[100][100];
  TH1D *data[100][100];
  TH1D *mc[100];
  TH1D *Mont[100][100];
  TH1D *MontSh[100][100];
  TH1D *MontSh14[100][100];
  TH1D *Mont2[100];
  TH1D* hUnf1[100][100];
  TH1D* hUnf2[100][100];
  TH1D* hUnf3[100][100];
  TH1D *hGen[100];
  TH1D *hSherpa[100];
  TH1D *hSherpa14[100];
  double MaxPlot=0;  
  double MinPlot=999.;  
    TH1D *h_pull_all_all= new TH1D ("h_pull_all_all","h_pull_all_all", 20, -3.,3.);
  int  nnn=1;
  if(pullTest)nnn=num_files;
 
  int nnn=1;
  if(pullTest)nnn=num_files;

  for(int i=0 ; i<no_ranges;i++){
    int binn_minn=3;
    if(i>4)binn_minn=5;
    TH1D *h_pull[1000];
    TH1D *h_pull_all= new TH1D ("h_pull_all","h_pull_all", 20, -3.,3.);

    for(int bb =0;bb<1000;bb++){
      sprintf(nam,"Pull_%i",bb);
      h_pull[bb] = new TH1D (nam,nam, 20, -3.,3.);
    }

    for(int jjj=0;jjj<nnn;jjj++){
      if(pullTest && jjj==1)continue;
      gROOT->Reset();


      var1_[i]= var1;
      if(is3d) var1_[i]+=(var3d);
      var1_[i]+=(i);
      var= var1_[i];
 
      if(!is3d){

	var2_[i]= var1;
	var2_[i]+="Gen_";
	var2_[i]+=(i);  
	cout<<var2_[i]<<endl;

	var3_[i]= var1;
	var3_[i]+="Gen_VS_reco_";
	var3_[i]+=(i);  
	cout<<var3_[i]<<endl;

      }

      if(is3d){
	var2_[i]= var1;
	var2_[i]+="Gen_";
	var2_[i]+=(var3d);  
	var2_[i]+=(i);  
	cout<<var2_[i]<<endl;

	var3_[i]= var1;
	var3_[i]+="Gen_VS_reco_";
	var3_[i]+=(var3d);  
	var3_[i]+=(i);  
	cout<<var3_[i]<<endl;

      }


      TString varGen= var2_[i];
      TString varMC= var3_[i];
      hGen[i]     =(TH1D*)(farray[1][0]->Get(varGen));
      hSherpa[i]  =(TH1D*)(sherpa->Get(varGen));
      hSherpa14[i]  =(TH1D*)(farray[1][12]->Get(varGen));

      TTree *tree1 = (TTree *)sherpa->Get("tree");

      double MyWeight1;

      tree1->SetBranchAddress("cnt0",&MyWeight1);

      tree1->GetEntry(0);

                         TTree *tree2 = (TTree *)farray[1][12]->Get("tree");
                          double totw;
                          tree2->SetBranchAddress("cnt0",&totw);
                         tree2->GetEntry(0);
      if(!pullTest){
	mc_bg_ss[i]= (TH1D*)histarray[1][i][1]->Clone("mc_bg_ss");
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
        bg_wz[i][0]= (TH1D*)histarray[4][i][0]->Clone("mc_bg");
  	bg_wz[i][0]->Add(histarray[5][i][0]);

        bg_zz[i][0]= (TH1D*)histarray[6][i][0]->Clone("mc_bg");
  	bg_zz[i][0]->Add(histarray[7][i][0]);
  	bg_zz[i][0]->Add(histarray[8][i][0]);

        bg_st[i][0]= (TH1D*)histarray[9][i][0]->Clone("mc_bg");
  	bg_st[i][0]->Add(histarray[10][i][0]);
  	bg_st[i][0]->Add(histarray[11][i][0]);
  	bg_st[i][0]->Add(histarray[12][i][0]);
  	bg_st[i][0]->Add(histarray[13][i][0]);
  	bg_st[i][0]->Add(histarray[14][i][0]);

	//mc_bg[i]->Add(tot_ss[i]);
      }
      data[i][0]= (TH1D*) histarray[jjj][i][0]->Clone();
      if(!pullTest){
	data[i][2]= (TH1D*) histarray[jjj][i][2]->Clone();//JES up
	data[i][3]= (TH1D*) histarray[jjj][i][3]->Clone();//JES down

	data[i][4]= (TH1D*) histarray[jjj][i][0]->Clone();
	data[i][5]= (TH1D*) histarray[jjj][i][0]->Clone();

	data[i][6]= (TH1D*) histarray[jjj][i][0]->Clone();
	data[i][7]= (TH1D*) histarray[jjj][i][0]->Clone();

	data[i][8]= (TH1D*) histarray[jjj][i][0]->Clone();
	data[i][9]= (TH1D*) histarray[jjj][i][0]->Clone();
	data[i][10]= (TH1D*) histarray[jjj][i][0]->Clone();
	data[i][11]= (TH1D*) histarray[jjj][i][0]->Clone();
	data[i][12]= (TH1D*) histarray[jjj][i][0]->Clone();


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
	data[i][12]->Add(mc_bg[i][0],-1.0);//PU-

	//cout<<"ALADASDASDASDASDA  "<< mc_bg[i][0]->GetBinContent(1)<<"   "<<mc_bg[i][4]->GetBinContent(1)<<"   "<<mc_bg[i][5]->GetBinContent(1)<<"    "<<mc_bg[i][6]->GetBinContent(1)<<"   "<<mc_bg[i][7]->GetBinContent(1)<<endl<<endl; 
      }
      mc[i]= (TH1D*) histarray[1][i][0]->Clone();

      cout << "==================================== TRAIN ====================================" << endl;
      cout<<farray[1][0]<<"  "<<var<<" "<<varGen<<" "<<varMC<<endl<<endl;
      TH1D *hRec   =(TH1D*)(farray[1][0]->Get(var));
      TH1D *hgen   =(TH1D*)(farray[1][0]->Get(varGen));
      TH2D *hMat   =(TH2D*)(farray[1][0]->Get(varMC)); 
   if(i>4){//HF 
    hRec->SetBinContent(1,0);
    hRec->SetBinContent(2,0);
    hgen->SetBinContent(1,0);
    hgen->SetBinContent(2,0);
    for(int binn=1;binn<=hRec->GetNbinsX();binn++){
      hMat->SetBinContent(1,binn,0);
      hMat->SetBinContent(2,binn,0);
      hMat->SetBinContent(binn,1,0);
      hMat->SetBinContent(binn,2,0);
    }
   }
      if(!pullTest){
	TH1D *hRecJERup   =(TH1D*)(farray[1][8]->Get(var));
	TH1D *hgenJERup   =(TH1D*)(farray[1][8]->Get(varGen));
	TH2D *hMatJERup   =(TH2D*)(farray[1][8]->Get(varMC)); 

	TH1D *hRecJERdown   =(TH1D*)(farray[1][9]->Get(var));
	TH1D *hgenJERdown   =(TH1D*)(farray[1][9]->Get(varGen));
	TH2D *hMatJERdown   =(TH2D*)(farray[1][9]->Get(varMC)); 

	TH1D *hRecPUup   =(TH1D*)(farray[1][10]->Get(var));
	TH1D *hgenPUup   =(TH1D*)(farray[1][10]->Get(varGen));
	TH2D *hMatPUup   =(TH2D*)(farray[1][10]->Get(varMC)); 

	TH1D *hRecPUdown   =(TH1D*)(farray[1][11]->Get(var));
	TH1D *hgenPUdown   =(TH1D*)(farray[1][11]->Get(varGen));
	TH2D *hMatPUdown   =(TH2D*)(farray[1][11]->Get(varMC)); 

	TH1D *hRecsh   =(TH1D*)(farray[1][12]->Get(var));
	TH1D *hgensh   =(TH1D*)(farray[1][12]->Get(varGen));
	TH2D *hMatsh   =(TH2D*)(farray[1][12]->Get(varMC)); 

   if(i>4){//HF 
    hRecJERup->SetBinContent(1,0);
    hRecJERup->SetBinContent(2,0);
    hgenJERup->SetBinContent(1,0);
    hgenJERup->SetBinContent(2,0);
    hRecJERdown->SetBinContent(1,0);
    hRecJERdown->SetBinContent(2,0);
    hgenJERdown->SetBinContent(1,0);
    hgenJERdown->SetBinContent(2,0);
    hRecPUup->SetBinContent(1,0);
    hRecPUup->SetBinContent(2,0);
    hgenPUup->SetBinContent(1,0);
    hgenPUup->SetBinContent(2,0);
    hRecPUdown->SetBinContent(1,0);
    hRecPUdown->SetBinContent(2,0);
    hgenPUdown->SetBinContent(1,0);
    hgenPUdown->SetBinContent(2,0);
    hRecsh->SetBinContent(1,0);
    hRecsh->SetBinContent(2,0);
    hgensh->SetBinContent(1,0);
    hgensh->SetBinContent(2,0);

    for(int binn=1;binn<=hRecJERup->GetNbinsX();binn++){
      hMatJERup->SetBinContent(1,binn,0);
      hMatJERup->SetBinContent(2,binn,0);
      hMatJERup->SetBinContent(binn,1,0);
      hMatJERup->SetBinContent(binn,2,0);
      hMatJERdown->SetBinContent(1,binn,0);
      hMatJERdown->SetBinContent(2,binn,0);
      hMatJERdown->SetBinContent(binn,1,0);
      hMatJERdown->SetBinContent(binn,2,0);
      hMatPUup->SetBinContent(1,binn,0);
      hMatPUup->SetBinContent(2,binn,0);
      hMatPUup->SetBinContent(binn,1,0);
      hMatPUup->SetBinContent(binn,2,0);
      hMatPUdown->SetBinContent(1,binn,0);
      hMatPUdown->SetBinContent(2,binn,0);
      hMatPUdown->SetBinContent(binn,1,0);
      hMatPUdown->SetBinContent(binn,2,0);
      hMatsh->SetBinContent(1,binn,0);
      hMatsh->SetBinContent(2,binn,0);
      hMatsh->SetBinContent(binn,1,0);
      hMatsh->SetBinContent(binn,2,0);

    }
   }
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
	RooUnfoldResponse responsesh (hRecsh, hgensh, hMatsh);                                   
      }
      response.UseOverflow();
      if(!pullTest){
	responseJERup.UseOverflow();
	responsePUup.UseOverflow();
	responseJERdown.UseOverflow();
	responsePUdown.UseOverflow();
	responsesh.UseOverflow();
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


	RooUnfoldBayes   unfold1sh (&responsesh, data[i][12], nitersh[i]);    // OR
	RooUnfoldSvd     unfold2sh (&responsesh, data[i][12], 3);   // OR
	RooUnfoldBinByBin unfold3sh (&responsesh, data[i][12]);

      }

      cout<<"BİİİİİİİİR"<<endl;
      if(pullTest)cout<<jjj<<endl;
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
	cout<<"ONIKIIIII"<<endl;
	hUnf1[i][12]= (TH1D*) unfold1sh.Hreco();
	hUnf2[i][12]= (TH1D*) unfold2sh.Hreco();
	hUnf3[i][12]= (TH1D*) unfold3sh.Hreco(); 
      }

      if(!pullTest){
      hUnf1[i][0]->Scale(data_scale*pow(10,(no_ranges-i-1)));
      hUnf2[i][0]->Scale(data_scale*pow(10,(no_ranges-i-1)));
      hUnf3[i][0]->Scale(data_scale*pow(10,(no_ranges-i-1)));

	hUnf1[i][2]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf2[i][2]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf3[i][2]->Scale(data_scale*pow(10,(no_ranges-i-1)));

	hUnf1[i][3]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf2[i][3]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf3[i][3]->Scale(data_scale*pow(10,(no_ranges-i-1)));

	hUnf1[i][4]->Scale(data_scale*pow(10,(no_ranges-i-1))/1.026);
	hUnf2[i][4]->Scale(data_scale*pow(10,(no_ranges-i-1))/1.026);
	hUnf3[i][4]->Scale(data_scale*pow(10,(no_ranges-i-1))/1.026);

	hUnf1[i][5]->Scale(data_scale*pow(10,(no_ranges-i-1))/0.974);
	hUnf2[i][5]->Scale(data_scale*pow(10,(no_ranges-i-1))/0.974);
	hUnf3[i][5]->Scale(data_scale*pow(10,(no_ranges-i-1))/0.974);


	hUnf1[i][6]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf2[i][6]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf3[i][6]->Scale(data_scale*pow(10,(no_ranges-i-1)));

	hUnf1[i][7]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf2[i][7]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf3[i][7]->Scale(data_scale*pow(10,(no_ranges-i-1)));

	hUnf1[i][8]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf2[i][8]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf3[i][8]->Scale(data_scale*pow(10,(no_ranges-i-1)));

	hUnf1[i][9]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf2[i][9]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf3[i][9]->Scale(data_scale*pow(10,(no_ranges-i-1)));

	hUnf1[i][10]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf2[i][10]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf3[i][10]->Scale(data_scale*pow(10,(no_ranges-i-1)));

	hUnf1[i][11]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf2[i][11]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf3[i][11]->Scale(data_scale*pow(10,(no_ranges-i-1)));

	hUnf1[i][12]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf2[i][12]->Scale(data_scale*pow(10,(no_ranges-i-1)));
	hUnf3[i][12]->Scale(data_scale*pow(10,(no_ranges-i-1)));

      hGen[i]->Scale(Mc_scale*pow(10,(no_ranges-i-1)));
      mc[i]->Scale(Mc_scale*pow(10,(no_ranges-i-1)));
      data[i][0]->Scale(data_scale*pow(10,(no_ranges-i-1)));
      hSherpa[i]->Scale(pow(10,(no_ranges-i-1))/(sumfactor*MyWeight1));
      hSherpa14[i]->Scale(pow(10,(no_ranges-i-1))*3531.5/totw);
      }
      if(pullTest){

        hGen[i]->Scale(Mc_scale);
        mc[i]->Scale(Mc_scale); 
      }

      //cout<<"MyWeight1 "<<MyWeight1<<endl<<endl<<endl<<endl;
      double j_pt_range[]={30.,50.,100.,200.,1000.};
      double j_y_range[]={0,0.5,1.0,1.5,2.0,2.5,3.2,4.7};

      double bin = data[i][0]->GetNbinsX();
      for(int bb=3;bb<=bin;bb++){
	if(pullTest)cout<<"DEBUG "<<data[i][0]->Integral()<<" "<<hUnf1[i][0]->GetBinContent(bb)<<"	"<<hgen->GetBinContent(bb)<<endl;
	if(pullTest)cout<<"PULLLLLLL "<<(hUnf1[i][0]->GetBinContent(bb)-hgen->GetBinContent(bb))/hUnf1[i][0]->GetBinError(bb)<<endl;
	if(pullTest&&bb<=bin/2)h_pull[bb-1]->Fill((hUnf1[i][0]->GetBinContent(bb)-hgen->GetBinContent(bb))/hUnf1[i][0]->GetBinError(bb));
	if(pullTest &&bb<=bin/2)h_pull_all->Fill((hUnf1[i][0]->GetBinContent(bb)-hgen->GetBinContent(bb))/hUnf1[i][0]->GetBinError(bb));
	if(pullTest &&bb<=(bin/2))h_pull_all_all->Fill((hUnf1[i][0]->GetBinContent(bb)-hgen->GetBinContent(bb))/hUnf1[i][0]->GetBinError(bb));
	if(!pullTest){
	double acc2=1;
	double acc3=1;
	if(var1.find("ljet_pt_y")!= std::string::npos)acc2=2*(j_y_range[i+1]-j_y_range[i]);
	if(var1.find("ljet_pt_sljetpt")!= std::string::npos)acc2=(j_pt_range[i+1]-j_pt_range[i]);
	if(!is3d &&var3d.find("z1")!= std::string::npos)acc3=1.5;
	double acc = hUnf1[i][0]->GetBinWidth(bb)*acc2*acc3 ;
	double scale_data =  data[i][0]->GetBinContent(bb)/acc;
	double scale_mc =  mc[i]->GetBinContent(bb)/acc;
	double scale_data_err =  data[i][0]->GetBinError(bb)/acc;
	double scale_mc_err =  mc[i]->GetBinError(bb)/acc;
	double scale_gen =  hGen[i]->GetBinContent(bb)/acc;
	double scale_sherpa =  hSherpa[i]->GetBinContent(bb)/acc;
	double scale_gen_err =  hGen[i]->GetBinError(bb)/acc;
	double scale_sherpa_err =  hSherpa[i]->GetBinError(bb)/acc;
	double scale_sherpa14 =  hSherpa14[i]->GetBinContent(bb)/acc;
	double scale_sherpa_err14 =  hSherpa14[i]->GetBinError(bb)/acc;
	data[i][0]->SetBinContent(bb,scale_data);
	data[i][0]->SetBinError(bb,scale_data_err);
	hSherpa[i]->SetBinContent(bb,scale_sherpa);
	hSherpa[i]->SetBinError(bb,scale_sherpa_err);
        hSherpa14[i]->SetBinContent(bb,scale_sherpa14);
	hSherpa14[i]->SetBinError(bb,scale_sherpa_err14);
	mc[i]->SetBinContent(bb,scale_mc);
	mc[i]->SetBinError(bb,scale_mc_err);
	hGen[i]->SetBinContent(bb,scale_gen);
	hGen[i]->SetBinError(bb,scale_gen_err);


	  for(int jjjj=0;jjjj<sys_no;jjjj++){
	    if (jjjj==1)continue;
            if(jjjj==0&&i<5)sum_sanity+=hUnf1[i][0]->GetBinContent(bb)/pow(10,(no_ranges-i-1));
            if(jjjj==0&&i<5)sum_sanityJESup+=hUnf1[i][2]->GetBinContent(bb)/pow(10,(no_ranges-i-1));
            if(jjjj==0&&i<5)sum_sanityJESdown+=hUnf1[i][4]->GetBinContent(bb)/pow(10,(no_ranges-i-1));
            cout<<"SUM SANITY!!!!!! c + -  "<<sum_sanity<<"  "<<sum_sanityJESup<<"  "<<sum_sanityJESdown<<endl;
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

      cout<<"is it here"<<endl;

 
      if(!pullTest){
      double binnn= hUnf1[i][0]->GetNbinsX();
       double plmax= hUnf1[i][0]->GetBinCenter(binnn)+((hUnf1[i][0]->GetBinWidth(binnn))/2);
      hSherpa[i]->GetXaxis()->SetRangeUser(30.,plmax);
      hSherpa14[i]->GetXaxis()->SetRangeUser(30.,plmax);
      hGen[i]->GetXaxis()->SetRangeUser(30.,plmax);

        hUnf1[i][0]->GetXaxis()->SetRangeUser(30.,plmax);
        mc[i]->GetXaxis()->SetRangeUser(30.,plmax);
 
        data[i][0]->GetXaxis()->SetRangeUser(30.,plmax);
        if(i>4){
      hSherpa[i]->GetXaxis()->SetRangeUser(50.,plmax);
      hSherpa14[i]->GetXaxis()->SetRangeUser(50.,plmax);
      hGen[i]->GetXaxis()->SetRangeUser(50.,plmax);

        hUnf1[i][0]->GetXaxis()->SetRangeUser(50.,plmax);
        mc[i]->GetXaxis()->SetRangeUser(50.,plmax);
 
        data[i][0]->GetXaxis()->SetRangeUser(50.,plmax);
       }
      for(int jjjj=2;jjjj<sys_no;jjjj++){
        double binnnn= hUnf1[i][jjjj]->GetNbinsX();
        double plmaxx= hUnf1[i][jjjj]->GetBinCenter(binnnn)+((hUnf1[i][jjjj]->GetBinWidth(binnnn))/2);
        hUnf1[i][jjjj]->GetXaxis()->SetRangeUser(30.,plmax);
        if(i>4){
        hUnf1[i][jjjj]->GetXaxis()->SetRangeUser(50.,plmax);
       }
      }
      cout<<"Burda mı göçüyo aq"<<endl;
      for(int binn=1;binn<=hUnf1[i][0]->GetNbinsX();binn++){

	unctot[i][binn]=0;
	for(int jjjj=0;jjjj<sys_no-1;jjjj++){
	  if (jjjj==0||jjjj==1 )continue;
          if((jjjj%2)!=0)continue;
	  double a=fabs( (hUnf1[i][jjjj]  ->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn) ));
	  double b=fabs( (hUnf1[i][jjjj+1]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn) ));


	  if(a>b)unc[i][binn][jjjj]=a / hUnf1[i][0]->GetBinContent(binn) ;
	  else unc[i][binn][jjjj]= b/ hUnf1[i][0]->GetBinContent(binn) ;
	  cout<<i<<"  "<<binn<<"  "<<jjjj<<"  "<<a<<"  "<<b<<" "<<sqrt(unctot[i][binn])<<endl;
	  unctot[i][binn] += pow(unc[i][binn][jjjj],2);
	}
        if(hUnf1[i][0]->GetBinContent(binn)!=0)cout<<"Unf unc"<<fabs(hUnf1[i][12]  ->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/ hUnf1[i][0]->GetBinContent(binn)<<endl ;
        unc[i][binn][12]=fabs(hUnf1[i][12]  ->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/ hUnf1[i][0]->GetBinContent(binn);
	unctot[i][binn]= sqrt(unctot[i][binn] +unc[i][binn][12]*unc[i][binn][12]  + SFeff*SFeff );
	cout<<unctot[i][binn]<<endl;
      }
      }




      if(pullTest){
	hGen[i]->Scale(1/Mc_scale);
	mc[i]->Scale(1/Mc_scale);
      }
      //mc[i]->Scale(1/(Mc_scale*pow(10,(no_ranges-i-1))));
      if(!pullTest){
	double a = hGen[i]->GetMaximum(100000000000000000000000000000);
	double b = hUnf1[i][0]->GetMaximum(1000000000000000000000000000000);   

	double a2 = hGen[i]->GetMinimum(0.000000000000000000000000000001);
	double b2 = hUnf1[i][0]->GetMinimum(0.000000000000000000000000000001);   
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
	cout<<MinPlot_l<<"  "<<MinPlot<<endl;
	for(int jjjj=0;jjjj<sys_no;jjjj++){
	  if (jjjj==1)continue;
	  if(unf1){ 
	    Mont[i][jjjj]=(TH1D*)hGen[i]->Clone("Mont[i]");
	    Mont[i][jjjj]->Divide(hGen[i], hUnf1[i][jjjj],1.0,1.0);
	    MontSh[i][jjjj]=(TH1D*)hSherpa[i]->Clone("MontSh[i]");
	    MontSh[i][jjjj]->Divide(hSherpa[i], hUnf1[i][jjjj],1.0,1.0);
	    MontSh14[i][jjjj]=(TH1D*)hSherpa14[i]->Clone("MontSh14[i]");
	    MontSh14[i][jjjj]->Divide(hSherpa14[i], hUnf1[i][jjjj],1.0,1.0);

	  }

	  if(unf2){ 
	    Mont[i][jjjj]=(TH1D*)hGen[i]->Clone("Mont[i]");
	    Mont[i][jjjj]->Divide(hGen[i], hUnf2[i][jjjj],1.0,1.0);
	    MontSh[i][jjjj]=(TH1D*)hSherpa[i]->Clone("MontSh[i]");
	    MontSh[i][jjjj]->Divide(hSherpa[i], hUnf2[i][jjjj],1.0,1.0);
            MontSh14[i][jjjj]=(TH1D*)hSherpa14[i]->Clone("MontSh14[i]");
	    MontSh14[i][jjjj]->Divide(hSherpa14[i], hUnf2[i][jjjj],1.0,1.0);

	  }

	  if(unf3){ 
	    Mont[i][jjjj]=(TH1D*)hGen[i]->Clone("Mont[i]");
	    Mont[i][jjjj]->Divide(hGen[i], hUnf3[i][jjjj],1.0,1.0);
	    MontSh[i][jjjj]=(TH1D*)hSherpa[i]->Clone("MontSh[i]");
	    MontSh[i][jjjj]->Divide(hSherpa[i], hUnf3[i][jjjj],1.0,1.0);
            MontSh14[i][jjjj]=(TH1D*)hSherpa14[i]->Clone("MontSh14[i]");
	    MontSh14[i][jjjj]->Divide(hSherpa14[i], hUnf3[i][jjjj],1.0,1.0);

	  }
	}


	Mont2[i]=(TH1D*)mc[i]->Clone("Mont2[i]");
	Mont2[i]->Divide(mc[i], data[i][0],1.0,1.0);
	TLegend *leg10 = new TLegend(0.75,0.153719,0.880511,0.225385);
	leg10->SetBorderSize(0);
	leg10->SetFillStyle(0);
	leg10->SetFillColor(kWhite);
	leg10->SetTextSize(0.028);
	sprintf(nam," %.1lf<|y|<%.1lf ",j_y_range[i],j_y_range[i+1],no_ranges-i-1);
	leg10->AddEntry(Mont[i][0], nam,"");

	if(is3d&&var3d.find("z0")!= std::string::npos) leg10->AddEntry(Mont[i][0]," |y(Z)|<1.0  ","");
  
	if(is3d&&var3d.find("z1")!= std::string::npos) leg10->AddEntry(Mont[i][0],"1.0<|y(Z)|<2.5","");
 
	TPaveText *pt = new TPaveText(.11,.915,.70,0.96);
	pt->SetFillColor(0);
	pt->SetBorderSize(0);
	pt->SetLineColor(0);
	pt->AddText("CMS Preliminary #sqrt{s} = 8 TeV, #int L dt = 19.6 fb^{-1} ");

	TCanvas *c3 = new TCanvas("c3","",700, 900);
	pt->Draw();

	c3->cd();
	splithist(0.0);
	c3->cd(1);

	//char nam[50];
	//gPad->SetTopMargin(0.02);
	//gPad->SetBottomMargin(0.11);
	//gPad->SetRightMargin(0.02);
	normalizeTH2D(hMat);
	sprintf(nam,"Reco %s",Xtitle);
	hMat->GetXaxis()->SetTitleOffset(1.5);
	hMat->SetXTitle(nam);
	sprintf(nam,"Gen %s",Xtitle);
	hMat->GetYaxis()->SetTitleOffset(1.5);
	hMat->SetYTitle(nam);
        gPad->SetLogx();
        gPad->SetLogy();
	gPad->SetLogz();
	hMat->Draw("COLZTEXT");
	leg10->Draw("SAME");
	sprintf(nam,"histos_note_1605_md/Response__HISTNAME___%i_%s_ismu_%d_ZPTweightcorr_%d.C",i,var3d.c_str(),isMu,ZPTweightcorr);
	c3->Print(nam);
	sprintf(nam,"histos_note_1605_md/Response__HISTNAME___%i_%s_ismu_%d_ZPTweightcorr_%d.pdf",i,var3d.c_str(),isMu,ZPTweightcorr);
	c3->Print(nam);
	sprintf(nam,"histos_note_1605_md/Response__HISTNAME___%i_%s_ismu_%d_ZPTweightcorr_%d.png",i,var3d.c_str(),isMu,ZPTweightcorr);
	c3->Print(nam);


	ofstream myfile;
	sprintf(nam,"histos_note_1605_md/Table__HISTNAME___%i_%s_ismu_%d_ZPTweightcorr_%d.tex",i,var3d.c_str(),isMu,ZPTweightcorr);
	myfile.open (nam);
	myfile.precision(4);

	myfile<<"\\begin{sidewaystable}[hbtp]"<<endl;
	myfile<<"\\begin{center}"<<endl;
	if(isMu==0){
          if(!is3d)myfile<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<") electron channel}"<<endl;
 	  if(is3d&&var3d.find("z0")!= std::string::npos)myfile<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", $Y_Z <$ 1.0) electron channel}"<<endl;
 	  if(is3d&&var3d.find("z1")!= std::string::npos)myfile<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", 1.0 $< Y_Z <$ 2.5) electron channel}"<<endl;
        }
	if(isMu==1){
          if(!is3d)myfile<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" < $Y_j$ < "<<j_y_range[i+1] <<") muon channel}"<<endl;
 	  if(is3d&&var3d.find("z0")!= std::string::npos)myfile<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", $Y_Z <$ 1.0) muon channel}"<<endl;
 	  if(is3d&&var3d.find("z1")!= std::string::npos)myfile<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", 1.0 $< Y_Z <$ 2.5) muon channel}"<<endl;
        }
	myfile<<"\\scriptsize{"<<endl;
	myfile<<"\\begin{tabular}{c|ccccccc} \\hline"<<endl;
	if(!is3d&&var1.find("ljet_pt_y")!= std::string::npos)myfile<<"Jet Pt &$ d^{2} \\sigma/dP_{T}(j1) dy(j1) [pb/GeV] $& stat & JES & Lumi & JER &PU& xsec \\\\ \\hline "<<endl;
	if(!is3d&&var1.find("ljet_pt_sljet")!= std::string::npos) myfile<<"Jet Pt &$ d^{2} \\sigma/dP_{T}(j1)dP_{T}(j2) [pb/GeV^{2}] $& stat & JES & Lumi & JER &PU& xsec \\\\ \\hline "<<endl;
	if(is3d&&var1.find("ljet_pt_y")!= std::string::npos) myfile<<"Jet Pt &$ d^{3} \\sigma/dP_{T}(j1) dy(j1)dy(Z) [pb/GeV] & stat $& JES & Lumi & JER &PU& xsec \\\\ \\hline "<<endl;
	if(is3d&&var1.find("ljet_pt_sljet")!= std::string::npos)myfile<<"Jet Pt & $d^{3} \\sigma/dP_{T}(j1)dP_{T}(j2)dy(Z) [pb/GeV^{2}] $& stat & JES & Lumi & JER &PU& xsec \\\\ \\hline "<<endl;
	for(int binn=binn_minn;binn<=hUnf1[i][0]->GetNbinsX();binn++){
 
	  myfile<<(hUnf1[i][0]->GetBinCenter(binn)-((hUnf1[i][0]->GetBinWidth(binn))/2))
                <<"-"
                <<(hUnf1[i][0]->GetBinCenter(binn)+((hUnf1[i][0]->GetBinWidth(binn))/2))

                << "  & "<<hUnf1[i][0]->GetBinContent(binn)/pow(10,(no_ranges-i-1))<<" &"<<100*hUnf1[i][0]->GetBinError(binn)/hUnf1[i][0]->GetBinContent(binn)//Central value

		<< "  & "<<100*(hUnf1[i][2]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/hUnf1[i][0]->GetBinContent(binn)<<"  "
		<<100*(hUnf1[i][3]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/hUnf1[i][0]->GetBinContent(binn)//JES

		<< "  & "<<100*(hUnf1[i][4]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/hUnf1[i][0]->GetBinContent(binn)<<"  "
		<<100*(hUnf1[i][5]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/hUnf1[i][0]->GetBinContent(binn)//Lumi

		<< "  & "<<100*(hUnf1[i][8]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/hUnf1[i][0]->GetBinContent(binn)<<"  "
		<<100*(hUnf1[i][9]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/hUnf1[i][0]->GetBinContent(binn)//JER

		<< "  & "<<100*(hUnf1[i][10]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/hUnf1[i][0]->GetBinContent(binn)<<"  "
		<<100*(hUnf1[i][11]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/hUnf1[i][0]->GetBinContent(binn)//PU

		<< "  & "<<100*(hUnf1[i][6]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/hUnf1[i][0]->GetBinContent(binn)<<"  "
		<<100*(hUnf1[i][7]->GetBinContent(binn)-hUnf1[i][0]->GetBinContent(binn))/hUnf1[i][0]->GetBinContent(binn)//xsec

		<<"\\\\"<<endl;
        }
	myfile<<"\\hline"<<endl;
	myfile<<"\\end{tabular}}" <<endl;
	myfile<<"\\label{tab:njetunf}" <<endl;
	myfile<<"\\end{center}" <<endl;
	myfile<<"\\end{sidewaystable}" <<endl;


	ofstream myfile2;
	sprintf(nam,"histos_note_1605_md/Table_MC__HISTNAME___%i_%s_ismu_%d_ZPTweightcorr_%d.tex",i,var3d.c_str(),isMu,ZPTweightcorr);
	myfile2.open (nam);
	myfile2.precision(4);

	myfile2<<"\\begin{table}[hbtp]"<<endl;
	myfile2<<"\\begin{center}"<<endl;
	if(isMu==0){
          if(!is3d)myfile2<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<") electron channel}"<<endl;
 	  if(is3d&&var3d.find("z0")!= std::string::npos)myfile2<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", $Y_Z <$ 1.0) electron channel}"<<endl;
 	  if(is3d&&var3d.find("z1")!= std::string::npos)myfile2<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", 1.0 $< Y_Z <$ 2.5) electron channel}"<<endl;
        }
	if(isMu==1){
          if(!is3d)myfile2<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" < $Y_j$ < "<<j_y_range[i+1] <<") muon channel}"<<endl;
 	  if(is3d&&var3d.find("z0")!= std::string::npos)myfile2<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", $Y_Z <$ 1.0) muon channel}"<<endl;
 	  if(is3d&&var3d.find("z1")!= std::string::npos)myfile2<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", 1.0 $< Y_Z <$ 2.5) muon channel}"<<endl;
        }
	myfile2<<"\\scriptsize{"<<endl;
	myfile2<<"\\begin{tabular}{c|ccccccccc} \\hline"<<endl;
        myfile2<<"Jet Pt & tt &WW&WZ&ZZ&singe top & DY & Total & Data &Ratio \\\\ \\hline "<<endl;
      
	//myfile2<<"Njet & $d \\sigma / d(NJet)$ & stat & JES & Lumi & JER &PU& xsec \\\\ \\hline "<<endl;
	for(int binn=binn_minn;binn<=hUnf1[i][0]->GetNbinsX();binn++){

	  myfile2<<(hUnf1[i][0]->GetBinCenter(binn)-((hUnf1[i][0]->GetBinWidth(binn))/2))
		 <<"-"
		 <<(hUnf1[i][0]->GetBinCenter(binn)+((hUnf1[i][0]->GetBinWidth(binn))/2))

		 << "  & "<<histarray[2][i][0]->GetBinContent(binn)//ttbar
		 << "  & "<<histarray[3][i][0]->GetBinContent(binn)//WW
		 << "  & "<<bg_wz[i][0]->GetBinContent(binn)//WZ
		 << "  & "<<bg_zz[i][0]->GetBinContent(binn)//ZZ
		 << "  & "<<bg_st[i][0]->GetBinContent(binn)//single top
		 << "  & "<<hRec->GetBinContent(binn)*3531.5*lumi/30459425. //Central value
		 << "  & "<<mc_bg[i][0]->GetBinContent(binn) + hRec->GetBinContent(binn)*3531.5*lumi/30459425.
		 << "  & "<<histarray[0][i][0]->GetBinContent(binn)
		 << "  & "<<(mc_bg[i][0]->GetBinContent(binn) + hRec->GetBinContent(binn)*3531.5*lumi/30459425.)/histarray[0][i][0]->GetBinContent(binn)
		 <<"\\\\"<<endl;
        }
	myfile2<<"\\hline"<<endl;
	myfile2<<"\\end{tabular}}" <<endl;	
	myfile2<<"\\label{tab:njetunf}" <<endl;
	myfile2<<"\\end{center}" <<endl;
	myfile2<<"\\end{table}" <<endl;


	ofstream myfile3;
	sprintf(nam,"histos_note_1605_md/Table_symm_error__HISTNAME___%i_%s_ismu_%d_ZPTweightcorr_%d.tex",i,var3d.c_str(),isMu,ZPTweightcorr);
	myfile3.open (nam);
	myfile3.precision(4);

	myfile3<<"\\begin{table}[hbtp]"<<endl;
	myfile3<<"\\begin{center}"<<endl;
	if(isMu==0){
          if(!is3d)myfile3<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<") electron channel}"<<endl;
 	  if(is3d&&var3d.find("z0")!= std::string::npos)myfile3<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", $Y_Z <$ 1.0) electron channel}"<<endl;
 	  if(is3d&&var3d.find("z1")!= std::string::npos)myfile3<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", 1.0 $< Y_Z <$ 2.5) electron channel}"<<endl;
        }
	if(isMu==1){
          if(!is3d)myfile3<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<"  $<Y_j< $ "<<j_y_range[i+1] <<") muon channel}"<<endl;
 	  if(is3d&&var3d.find("z0")!= std::string::npos)myfile3<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", $Y_Z <$ 1.0) muon channel}"<<endl;
 	  if(is3d&&var3d.find("z1")!= std::string::npos)myfile3<<"\\caption{Leading jet pt ( "<<j_y_range[i]<<" $< Y_j <$ "<<j_y_range[i+1] <<", 1.0 $< Y_Z <$ 2.5) muon channel}"<<endl;
        }
	myfile3<<"\\scriptsize{"<<endl;
	myfile3<<"\\begin{tabular}{c|ccccccccc} \\hline"<<endl;

	if(!is3d&&var1.find("ljet_pt_y")!= std::string::npos)myfile3<<"Pt range & $ d^{2} \\sigma/dP_{T}(j1) dy(j1) [pb/GeV]$ & stat & JES & Lumi & JER &PU& xsec&unf&tot\\\\ \\hline "<<endl;
	if(!is3d&&var1.find("ljet_pt_sljet")!= std::string::npos)myfile3<<"Pt range & $ d^{2} \\sigma/dP_{T}(j1)dP_{T}(j2) [pb/GeV]$ & stat & JES & Lumi & JER &PU& xsec&unf &tot \\\\ \\hline "<<endl;
	if(is3d&&var1.find("ljet_pt_y")!= std::string::npos) myfile3<<"Pt range & $d^{3} \\sigma/dP_{T}(j1) dy(j1)dy(Z) [pb/GeV]$ & stat & JES & Lumi & JER &PU& xsec&unf &tot \\\\ \\hline "<<endl;
	if(is3d&&var1.find("ljet_pt_sljet")!= std::string::npos)myfile3<<"Pt range &$ d^{3} \\sigma/dP_{T}(j1)dP_{T}(j2)dy(Z) [pb/GeV^{2}]$ & stat & JES & Lumi & JER &PU& xsec &unf&tot \\\\ \\hline "<<endl;
	for(int binn=binn_minn;binn<=hUnf1[i][0]->GetNbinsX();binn++){
	  myfile3<<(hUnf1[i][0]->GetBinCenter(binn)-((hUnf1[i][0]->GetBinWidth(binn))/2))
		 <<"-"
		 <<(hUnf1[i][0]->GetBinCenter(binn)+((hUnf1[i][0]->GetBinWidth(binn))/2))

		 << "  & "<<hUnf1[i][0]->GetBinContent(binn)/pow(10,(no_ranges-i-1))
              
		 <<"  &  "<<100*hUnf1[i][0]->GetBinError(binn)/hUnf1[i][0]->GetBinContent(binn)//Central value

		 << "  & "<<100*unc[i][binn][2]//JES

		 << "  & "<<100*unc[i][binn][4]//Lumi

		 << "  & "<<100*unc[i][binn][8]//JER

		 << "  & "<<100*unc[i][binn][10]//PU

		 << "  & "<<100*unc[i][binn][6]//xsec

                 << "  & "<<100*unc[i][binn][12]//unf

		 << "  & "<<100*unctot[i][binn]//tot
		 <<"\\\\"<<endl;
        }
	myfile3<<"\\hline"<<endl;
	myfile3<<"\\end{tabular}}" <<endl;
	myfile3<<"\\label{tab:njetunf}" <<endl;
	myfile3<<"\\end{center}" <<endl;
	myfile3<<"\\end{table}" <<endl;

      }//!pulltest 

    }//jjj loop

    if(pullTest){
    TCanvas *c6 = new TCanvas("c6","",700, 900);
    c6->cd();
    h_pull_all->Draw();
    sprintf(nam,"histos_note_1605_md/pull_all___HISTNAME___%s_ismu_%d_ZPTweightcorr_%d_%d.root",var3d.c_str(),isMu,ZPTweightcorr,i);
    c6->Print(nam);
    sprintf(nam,"histos_note_1605_md/pull_all___HISTNAME___%s_ismu_%d_ZPTweightcorr_%d_%d.pdf",var3d.c_str(),isMu,ZPTweightcorr,i);
    c6->Print(nam);
    
    for(int bbbb=0;bbbb<5;bbbb++){
      TCanvas *c5 = new TCanvas("c5","",700, 900);
      c5->cd();  
      h_pull[bbbb]->Draw();
    sprintf(nam,"histos_note_1605_md/pull_all___HISTNAME___%s_ismu_%d_ZPTweightcorr_%d_%d_%d.pdf",var3d.c_str(),isMu,ZPTweightcorr,i,bbbb);
      c5->Print(nam);
    sprintf(nam,"histos_note_1605_md/pull_all___HISTNAME___%s_ismu_%d_ZPTweightcorr_%d_%d_%d.root",var3d.c_str(),isMu,ZPTweightcorr,i,bbbb);
      c5->Print(nam);
    }
    }

  }//i loop
    if(pullTest){
    TCanvas *c66 = new TCanvas("c66","",700, 900);
    c66->cd();
    h_pull_all_all->Draw();
    sprintf(nam,"histos_note_1605_md/pull_all___HISTNAME___%s_ismu_%d_ZPTweightcorr_%d.root",var3d.c_str(),isMu,ZPTweightcorr);
    c66->Print(nam);
    sprintf(nam,"histos_note_1605_md/pull_all___HISTNAME___%s_ismu_%d_ZPTweightcorr_%d.pdf",var3d.c_str(),isMu,ZPTweightcorr);
    c66->Print(nam);
    }
  if(!pullTest){
    //TLegend *leg1 = new TLegend(0.68,0.713719,0.880511,0.965385);
    TLegend *leg1 = new TLegend(0.62,0.65,0.839999,0.869999);
    //leg1 = new TLegend(0.831567,0.653719,0.980511,1.025385);
    leg1->SetBorderSize(0);
    //leg1->SetFillStyle(1);
    leg1->SetFillColor(kWhite);
    leg1->SetTextSize(0.028);

    //TLegend *leg2 = new TLegend(0.68,0.713719,0.880511,0.965385);
    TLegend *leg2 = new TLegend(0.65,0.60,0.939511,0.755385);
    //leg1 = new TLegend(0.831567,0.653719,0.980511,1.025385);
    leg2->SetBorderSize(0);
    leg2->SetFillStyle(0);
    leg2->SetFillColor(kWhite);
    leg2->SetTextSize(0.028);

    //TLegend *leg3 = new TLegend(0.15,0.15,0.400511,0.355385);
 

    //  double pt_range[6]={20.,30.,50.,100.,200.,1000.};

    //  if(unf1 ){

    cout<<"Burdayım ulenn"<<endl;
    for(int ii=0;ii<no_ranges;ii++){
      if(var1.find("ljet_pt_y")!= std::string::npos){
	sprintf(nam,"%.1lf<|y|<%.1lf (\\times 10^{%d})",j_y_range[ii],j_y_range[ii+1],no_ranges-ii-1);
	leg1->AddEntry(hUnf1[ii][0], nam,"pe");
	//sprintf(nam,"Gen %.1lf<|y|<%.1lf (*10^{%d})",j_y_range[ii],j_y_range[ii+1],no_ranges-ii-1);
	//leg1->AddEntry(hGen[ii], nam,"l");
	sprintf(nam,"%.1lf<|y|<%.1lf (\\times 10^{%d})",j_y_range[ii],j_y_range[ii+1],no_ranges-ii-1);
	leg2->AddEntry(data[ii][0], nam,"pe");


	//sprintf(nam,"MC %.1lf<|y|<%.1lf (*10^{%d})",j_y_range[ii],j_y_range[ii+1],no_ranges-ii-1);
	//leg2->AddEntry(mc[ii], nam,"l");
      }

      if(var1.find("ljet_pt_sljetpt")!= std::string::npos){
	cout<<"Burdayım ulenn"<<endl;
	sprintf(nam," %d<P_T(j_2)<%d (*10^{%i})",j_pt_range[ii],j_pt_range[ii+1],no_ranges-ii-1);
	leg1->AddEntry(hUnf1[ii][0], nam,"pe");
	//sprintf(nam,"Gen %d<P_T(j_2)<%d (*10^{%i})",j_pt_range[ii],j_pt_range[ii+1],no_ranges-ii-1);
	//leg1->AddEntry(hGen[ii], nam,"l");
	sprintf(nam," %d<P_T(j_2)<%d (*10^{%i})",j_pt_range[ii],j_pt_range[ii+1],no_ranges-ii-1);
	leg2->AddEntry(data[ii][0], nam,"pe");
	//sprintf(nam,"MC %d<P_T(j_2)<%d (*10^{%i})",j_pt_range[ii],j_pt_range[ii+1],no_ranges-ii-1);
	//leg2->AddEntry(mc[ii], nam,"l");
      }

    }   
    leg1->AddEntry(hGen[0], "MadGraph Z+ #leq 4j @LO","l");
    leg1->AddEntry( hSherpa[0], "Sherpa Z +1,2j @NLO, #leq 4j@LO","l");
    //leg1->AddEntry( hSherpa14[0], "Sherpa Z + #leq 4j@LO","l");
    leg2->AddEntry(mc[0]," MC","l");
    //}
    TLegend *leg12 = new TLegend(0.75,0.183719,0.980511,0.355385);
    leg12->SetBorderSize(0);
    leg12->SetFillStyle(0);
    leg12->SetFillColor(kWhite);
    leg12->SetTextSize(0.028);
    leg12->AddEntry(hSherpa[0],"|y(Z)|<1.0","");
    TLegend *leg13 = new TLegend(0.75,0.183719,0.980511,0.355385);
    leg13->SetBorderSize(0);
    leg13->SetFillStyle(0);
    leg13->SetFillColor(kWhite);
    leg13->SetTextSize(0.028);
    leg13->AddEntry(hSherpa[0],"1.0<|y(Z)|<2.5","");  


    TCanvas *c1 = new TCanvas("c1","",2000, 1600);
 

    //  TPaveText *pt = new TPaveText(.09,.925,.70,0.98);
    //  pt->SetFillColor(0);
    //  pt->SetBorderSize(0);
    //  pt->SetLineColor(0);
    //  pt->AddText("CMS Preliminary #sqrt{s} = 8 TeV, #int L dt = 19.6fb^{-1} ");

    pt->Draw();
    c1->cd();
    splithist(0.0);
    c1->cd(1);
    //  gPad->SetTopMargin(0.02);
    //  gPad->SetBottomMargin(0.11);
    //  gPad->SetRightMargin(0.02);
    gPad->SetLogy();


    if(setlogx)gPad->SetLogx();

    for(int i=0; i<no_ranges;i++){

      //  if(no_ranges==8 && i==6)continue;

      colorIt(hUnf1[i][0],kBlack,i+20);
      colorIt(hUnf2[i][0],kBlack,i+20);
      colorIt(hUnf3[i][0],kBlack,i+20);

      colorIt(hUnf1[i][12],kGreen,i+20);
      colorIt(hUnf2[i][12],kGreen,i+20);
      colorIt(hUnf3[i][12],kGreen,i+20);
      /*for(binn=1;binn<=hGen[i]->GetNbinsX();binn++){
      
      hUnf1[i][0]->SetBinError(binn,hUnf1[i][0]->GetBinContent(binn)*unctot[i][binn]); 
      }*/
      //    colorBG(hGen[i],((i+1)*12));
      hGen[i]->SetLineWidth(1.5);
      hGen[i]->SetLineColor(kBlack);
      //colorIt(hSherpa[i],kCyan,0);
      hSherpa[i]->SetLineColor(38);
      hSherpa[i]->SetFillColor(38);
      hSherpa14[i]->SetLineColor(kGreen);
      hSherpa14[i]->SetFillColor(kGreen);
      if(i==0){
	hSherpa[i]->Draw("e2");
	hSherpa[i]->SetMaximum(MaxPlot*10000);
	hSherpa[i]->SetMinimum(MinPlot/10);
	hSherpa[i]->SetXTitle(Xtitle); 
	hSherpa[i]->GetYaxis()->SetLabelSize(0.03);
	hSherpa[i]->GetXaxis()->SetLabelSize(0.03);
	hSherpa[i]->GetYaxis()->SetTitleOffset(1.5);
	if(!is3d&&var1.find("ljet_pt_y")!= std::string::npos) hSherpa[i]->GetYaxis()->SetTitle("d^{2} #sigma/dP_{T}(j1) dy(j1) [pb/GeV]");
	if(!is3d&&var1.find("ljet_pt_sljet")!= std::string::npos) hSherpa[i]->GetYaxis()->SetTitle("d^{2} #sigma/dP_{T}(j1)dP_{T}(j2) [pb/GeV^{2}]  ");
	if(is3d&&var1.find("ljet_pt_y")!= std::string::npos) hSherpa[i]->GetYaxis()->SetTitle("d^{3} #sigma/dP_{T}(j1) dy(j1)dy(Z) [pb/GeV]");
	if(is3d&&var1.find("ljet_pt_sljet")!= std::string::npos) hSherpa[i]->GetYaxis()->SetTitle("d^{3} #sigma/dP_{T}(j1)dP_{T}(j2)dy(Z) [pb/GeV^{2}]  ");

      }
      else{
	hSherpa[i]->Draw("e2same");

      }
     // hSherpa14[i]->Draw("e2same");
      hGen[i]->Draw("][hhistsame");
      if(unf1)    hUnf1[i][0]->Draw("eX0Csame");
      if(unf2)    hUnf2[i][0]->Draw("eX0Csame");
      if(unf3)    hUnf3[i][0]->Draw("eX0Csame");
      /*if(unf1)    hUnf1[i][12]->Draw("eX0Csame");
      if(unf2)    hUnf2[i][12]->Draw("eX0Csame");
      if(unf3)    hUnf3[i][12]->Draw("eX0Csame");*/
    }
    leg1->Draw("same");
    if(is3d&&var3d.find("z0")!= std::string::npos)leg12->Draw("same");
    if(is3d&&var3d.find("z1")!= std::string::npos)leg13->Draw("same");

    sprintf(nam,"histos_note_1605_md/Bayes___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d.C",var3d.c_str(),isMu,ZPTweightcorr);
    c1->Print(nam);
    sprintf(nam,"histos_note_1605_md/Bayes___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d.pdf",var3d.c_str(),isMu,ZPTweightcorr);
    c1->Print(nam);
    sprintf(nam,"histos_note_1605_md/Bayes___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d.png",var3d.c_str(),isMu,ZPTweightcorr);
    c1->Print(nam);

    for(int i=0; i<no_ranges;i++){
      TLegend *leg3 = new TLegend(0.15,0.683719,0.280511,0.855385);
      leg3->SetBorderSize(0);
      leg3->SetFillStyle(0);
      leg3->SetFillColor(kWhite);
      leg3->SetTextSize(0.028);

      if(!is3d&&var1.find("ljet_pt_y")!= std::string::npos)sprintf(nam," %.1lf<|y_{j}|<%.1lf ",j_y_range[i],j_y_range[i+1],no_ranges-i-1);
      if(!is3d&&var1.find("ljet_pt_sljet")!= std::string::npos) sprintf(nam," %.1lf<|y_{j}|<%.1lf   ",j_y_range[i],j_y_range[i+1],no_ranges-i-1);
      if(is3d&&var3d.find("z0")!= std::string::npos&&var1.find("ljet_pt_y")!= std::string::npos) sprintf(nam," %.1lf<|y_{j}|<%.1lf, |y(Z)|<1.0 ",j_y_range[i],j_y_range[i+1],no_ranges-i-1);
      if(is3d&&var3d.find("z0")!= std::string::npos&&var1.find("ljet_pt_sljet")!= std::string::npos) sprintf(nam," %.1lf<|y_{j}|<%.1lf, |y(Z)|<1.0 ",j_y_range[i],j_y_range[i+1],no_ranges-i-1);
      if(is3d&&var3d.find("z1")!= std::string::npos&&var1.find("ljet_pt_y")!= std::string::npos) sprintf(nam," %.1lf<|y_{j}|<%.1lf, 1.0<|y(Z)|<2.5  ",j_y_range[i],j_y_range[i+1],no_ranges-i-1);
      if(is3d&&var3d.find("z1")!= std::string::npos&&var1.find("ljet_pt_sljet")!= std::string::npos) sprintf(nam," %.1lf<|y{j}|<%.1lf, 1.0<|y(Z)|<2.5 ",j_y_range[i],j_y_range[i+1],no_ranges-i-1);


      //leg3->AddEntry(MontSh14[i][0],"Sherpa Z +<4j@LO","pe");

      TH1D* montunc= (TH1D*)Mont[i][0]->Clone("montunc");
      TH1D* montstat= (TH1D*)Mont[i][0]->Clone("montstat");
      TH1D* montpdfup= (TH1D*)MontSh[i][0]->Clone("montpdfup");
      TH1D* montpdfdn= (TH1D*)MontSh[i][0]->Clone("montpdfdn");
      TH1D* montpdfupstat= (TH1D*)MontSh[i][0]->Clone("montpdfupstat");
      TH1D* montpdfdnstat= (TH1D*)MontSh[i][0]->Clone("montpdfdnstat");
      TH1D* montstatmd= (TH1D*)Mont[i][0]->Clone("montstatmd");
      montstatmd->SetFillColor(kOrange-5);
      montpdfup->SetFillColor(38);
      montpdfdn->SetFillColor(38);
      montstatmd->SetFillStyle(1001);
      montpdfup->SetFillStyle(1001);
      montpdfdn->SetFillStyle(1001);
      montpdfupstat->SetFillColor(kBlue-6);
      montpdfdnstat->SetFillColor(kBlue-6);
      montpdfupstat->SetFillStyle(1001);
      montpdfdnstat->SetFillStyle(1001);

	montpdfdn->SetMarkerSize(0);
	montpdfup->SetMarkerSize(0);
	montpdfdnstat->SetMarkerSize(0);
	montpdfupstat->SetMarkerSize(0);
	montunc->SetMarkerSize(0);
	montstat->SetMarkerSize(0);
	montstat->SetLineColor(kBlack);
      montunc->SetFillStyle(3354);
      montunc->SetFillColor(kBlack);
      colorIt(Mont[i][0],kOrange+10,25);
      colorIt(Mont2[i],kRed,20);
      colorIt(Mont[i][12],kGreen,20);
      colorIt(MontSh[i][0],kBlue,25);
      colorIt(MontSh14[i][0],kGreen,25);
      leg3->AddEntry(Mont[i][0], nam,"");

      //leg3->AddEntry(montpdfup,"Statistical unc (gen)","F");
      //leg3->AddEntry(montpdfupstat,"Theory uncertainty (gen)","F");


      leg3->AddEntry(montstatmd, "MadGraph Z+<4j @LO","F");
      leg3->AddEntry(montpdfupstat,"Sherpa Z +1,2j @NLO, <4j@LO","F");
      leg3->AddEntry(montunc,"Total experimental unc","F");  
      for(int ijk=1; ijk<=montunc->GetNbinsX();ijk++){
	double aa= unctot[i][ijk];
        double aa2 = hUnf1[i][0]->GetBinError(ijk)/hUnf1[i][0]->GetBinContent(ijk);
        double aatot= sqrt(aa*aa + aa2*aa2);
	montunc->SetBinError(ijk,aatot);
	montunc->SetBinContent(ijk,1);
	montstat->SetBinError(ijk,hUnf1[i][0]->GetBinError(ijk)/hUnf1[i][0]->GetBinContent(ijk));
	montstat->SetBinContent(ijk,1);

        double stat=hSherpa[i]->GetBinError(ijk)/hSherpa[i]->GetBinContent(ijk);
        double pdfup=PDFu[i][ijk-1];

        double pdfdn=PDFd[i][ijk-1];
        double pdfup_stat= sqrt(pdfup*pdfup + stat*stat);
        double pdfdn_stat= sqrt(pdfdn*pdfdn + stat*stat);     
        cout<<i<<"  "<<ijk-1<<"  "<<PDFu[i][ijk-1]<<"  "<<PDFd[i][ijk-1]<<"  "<<stat<<"  "<<pdfup_stat<<"  "<<pdfdn_stat<<endl;
        montpdfup->SetBinContent(ijk,MontSh[i][0]->GetBinContent(ijk)+stat/2); 
        montpdfup->SetBinError(ijk,stat/2);
        montpdfdn->SetBinContent(ijk,MontSh[i][0]->GetBinContent(ijk)-stat/2);
        montpdfdn->SetBinError(ijk,stat/2);
        montpdfupstat->SetBinContent(ijk,MontSh[i][0]->GetBinContent(ijk)+pdfup_stat/2);
        montpdfupstat->SetBinError(ijk,pdfup_stat/2);
        montpdfdnstat->SetBinContent(ijk,MontSh[i][0]->GetBinContent(ijk)-pdfdn_stat/2);
        montpdfdnstat->SetBinError(ijk,pdfdn_stat/2);
        montstatmd->SetBinError(ijk,hGen[i]->GetBinError(ijk)/hGen[i]->GetBinContent(ijk));
	Mont[i][0]->SetBinError(ijk,hGen[i]->GetBinError(ijk)/hGen[i]->GetBinContent(ijk));
	MontSh[i][0]->SetBinError(ijk,hSherpa[i]->GetBinError(ijk)/hSherpa[i]->GetBinContent(ijk));
	MontSh14[i][0]->SetBinError(ijk,hSherpa14[i]->GetBinError(ijk)/hSherpa14[i]->GetBinContent(ijk));
       //cout<<"STAT ERROR ON MC "<<hGen[i]->GetBinContent(ijk)<<"  "<<hGen[i]->GetBinError(ijk)<<"  " <<hGen[i]->GetBinError(ijk)/hGen[i]->GetBinContent(ijk)<<endl;
      }



      TCanvas *c5 = new TCanvas("c2","",500, 500);
      TPaveText *pt = new TPaveText(.09,.925,.70,0.98);
      pt->SetFillColor(0);
      pt->SetBorderSize(0);
      pt->SetLineColor(0);
      pt->AddText("CMS Preliminary #sqrt{s} = 8 TeV, #int L dt = 19.6 fb^{-1} ");

      pt->Draw();
      c5->cd();
      splithist(0.0);
      c5->cd(1);

      if(setlogx)gPad->SetLogx();
      //Mont[i]->GetYaxis()->SetNdivisions(0.1);
      //gStyle.SetGridWidth(5);
      gPad->SetGridy();
      montpdfupstat->SetMinimum(0);
      montpdfupstat->SetMaximum(2.0);
      montpdfupstat->GetYaxis()->SetLabelSize(0.03);
      montpdfupstat->GetXaxis()->SetLabelSize(0.03);
      montpdfupstat->SetXTitle(Xtitle); 
      montpdfupstat->GetYaxis()->SetTitleOffset(1.5);
      montpdfupstat->GetYaxis()->SetTitle("Theory/Data");

      montpdfupstat->Draw("][ e3 ");
      montpdfdnstat->Draw("][ e3 SAME");
      montstatmd->Draw("][ e3 SAME");
      //montpdfup->Draw("][ e3 SAME");
      //montpdfdn->Draw("][ e3 SAME");
     //montunc->SetMinimum(0);
     //montunc->SetMaximum(2.0);
     //montunc->GetYaxis()->SetLabelSize(0.03);
     //montunc->GetXaxis()->SetLabelSize(0.03);
     //montunc->SetXTitle(Xtitle); 
     //montunc->GetYaxis()->SetTitleOffset(1.5);
     //montunc->GetYaxis()->SetTitle("Theory/Data");
      montunc->Draw("][ e3 SAME");

      montstat->Draw("e1eX0 same");
      Mont[i][0]->Draw("SAME peX0");
      //Mont[i][12]->Draw("SAME peX0");
      //Mont2[i][0]->Draw("SAME peX0");
      MontSh[i][0]->Draw("SAME peX0");
      //MontSh14[i][0]->Draw("SAME peX0");
     
      leg3->Draw("same");
      //colorIt(Mont2[i],kRed,i+20);
      //    Mont2[i]->Draw("peX0Csame");
      //gPad->SetTopMargin(0.90);
      sprintf(nam,"histos_note_1605_md/ratio_Bayes___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d_Range_%d.C",var3d.c_str(),isMu,ZPTweightcorr,i);
      c5->Print(nam);
      sprintf(nam,"histos_note_1605_md/ratio_Bayes___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d_Range_%d.pdf",var3d.c_str(),isMu,ZPTweightcorr,i);
      c5->Print(nam);
      sprintf(nam,"histos_note_1605_md/ratio_Bayes___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d_Range_%d.png",var3d.c_str(),isMu,ZPTweightcorr,i);
      c5->Print(nam);

      for(int jjjj=0;jjjj<sys_no-1;jjjj++){
        if (jjjj==0||jjjj==1 )continue;
	if((jjjj%2)!=0)continue;
	TLegend *leg5 = new TLegend(0.15,0.853719,0.280511,0.885385);
	leg5->SetBorderSize(0);
	leg5->SetFillStyle(0);
	leg5->SetFillColor(kWhite);
	leg5->SetTextSize(0.028);
	if(jjjj==2)leg5->AddEntry(Mont[i][jjjj], "JEC ","l");
	if(jjjj==4)leg5->AddEntry(Mont[i][jjjj], "Lumi ","l");
	if(jjjj==6)leg5->AddEntry(Mont[i][jjjj], "x-sec ","l");
	if(jjjj==8)leg5->AddEntry(Mont[i][jjjj], "JER ","l");
	if(jjjj==10)leg5->AddEntry(Mont[i][jjjj], "PU ","l");

	TCanvas *c6 = new TCanvas("c2","",500, 500);
	pt->Draw();
	c6->cd();
	splithist(0.0);
	c6->cd(1);
   
	if(setlogx)gPad->SetLogx();
	//Mont[i]->GetYaxis()->SetNdivisions(0.1);
	//gStyle.SetGridWidth(5);
	gPad->SetGridy();

	Mont[i][0]->SetMinimum(0);
	Mont[i][0]->SetMaximum(2.0);
	Mont[i][0]->GetYaxis()->SetLabelSize(0.03);
	Mont[i][0]->GetXaxis()->SetLabelSize(0.03);
	Mont[i][0]->Draw("E1");
	Mont[i][0]->Draw("SAME Lhist");
	Mont[i][0]->SetXTitle(Xtitle); 
	Mont[i][0]->GetYaxis()->SetTitleOffset(1.5);
	Mont[i][0]->GetYaxis()->SetTitle("Gen/Unfolded");
	Mont[i][jjjj]->SetLineColor(3*jjjj+2);
	Mont[i][jjjj+1]->SetLineColor(3*jjjj+2);
	Mont[i][jjjj]->Draw("SAME E1");
	Mont[i][jjjj]->Draw("SAME Lhist");
	Mont[i][jjjj+1]->Draw("SAME E1");
	Mont[i][jjjj+1]->Draw("SAME Lhist");
	leg3->Draw("same");
	leg5->Draw("same");



	sprintf(nam,"histos_note_1605_md/ratio_Bayes___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d_Range_%d_sys_%d.C",var3d.c_str(),isMu,ZPTweightcorr,i,jjjj);
	c6->Print(nam);
	sprintf(nam,"histos_note_1605_md/ratio_Bayes___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d_Range_%d_sys_%d.pdf",var3d.c_str(),isMu,ZPTweightcorr,i,jjjj);
	c6->Print(nam);
	sprintf(nam,"histos_note_1605_md/ratio_Bayes___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d_Range_%d_sys_%d.png",var3d.c_str(),isMu,ZPTweightcorr,i,jjjj);
	c6->Print(nam);     
      }
    }

    TCanvas *c3 = new TCanvas("c3","",1200, 900);
    c3->cd(); 
    pt->Draw();
    c3->cd();
    splithist(0.0);
    c3->cd(1);

    gPad->SetLogy();
    if(setlogx)gPad->SetLogx();

    for(int i=0; i<no_ranges;i++){
      //    if(no_ranges==8 && i==6)continue;

      colorIt(data[i][0],kBlack,i+20);

      //colorBG(mc[i],((i+1)*12));
      mc[i]->SetLineWidth(1.5);
      mc[i]->SetLineColor(kBlack);
      if(i==0){
	mc[i]->Draw("][hhist");
	mc[i]->SetMaximum(MaxPlot*10);
	mc[i]->SetMinimum(MinPlot/100);
	mc[i]->SetXTitle(Xtitle);
	mc[i]->GetYaxis()->SetTitleOffset(1.5); 
	if(!is3d&&var1.find("ljet_pt_y")!= std::string::npos) mc[i]->GetYaxis()->SetTitle("d^{2} #sigma/dP_{T}(j1) dy(j1) [pb/GeV]");
	if(!is3d&&var1.find("ljet_pt_sljet")!= std::string::npos) mc[i]->GetYaxis()->SetTitle("d^{2} #sigma/dP_{T}(j1)dP_{T}(j2) [pb/GeV^{2}]  ");
	if(is3d&&var1.find("ljet_pt_y")!= std::string::npos) mc[i]->GetYaxis()->SetTitle("d^{3} #sigma/dP_{T}(j1) dy(j1)dy(Z) [pb/GeV]");
	if(is3d&&var1.find("ljet_pt_sljet")!= std::string::npos) mc[i]->GetYaxis()->SetTitle("d^{3} #sigma/dP_{T}(j1)dP_{T}(j2)dy(Z) [pb/GeV^{2}]  ");
      }
      else{
	mc[i]->Draw("hhistsame");
      }
      data[i][0]->Draw("eX0Csame");
    }
    leg2->Draw("same");
    if(is3d&&var3d.find("z0")!= std::string::npos)leg12->Draw("same");
    if(is3d&&var3d.find("z1")!= std::string::npos)leg13->Draw("same");

    sprintf(nam,"histos_note_1605_md/Detector_Level___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d.C",var3d.c_str(),isMu,ZPTweightcorr);
    c3->Print(nam);
    sprintf(nam,"histos_note_1605_md/Detector_Level___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d.pdf",var3d.c_str(),isMu,ZPTweightcorr);
    c3->Print(nam);
    sprintf(nam,"histos_note_1605_md/Detector_Level___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d.png",var3d.c_str(),isMu,ZPTweightcorr);
    c3->Print(nam);



    TCanvas *c4 = new TCanvas("c4","",700, 1000);
    TPaveText *pt = new TPaveText(.09,.925,.70,0.98);
    pt->SetFillColor(0);
    pt->SetBorderSize(0);
    pt->SetLineColor(0);
    pt->AddText("CMS Preliminary #sqrt{s} = 8 TeV, #int L dt = 19.6 fb^{-1} ");

    pt->Draw();


    c4->Divide(1,no_ranges);

    for(int i=0; i<no_ranges;i++){
      c4->cd(i+1); 
      gPad->SetRightMargin(0.02);
      gPad->SetTopMargin(0.02);
      gPad->SetBottomMargin(0.24);

      if(setlogx)gPad->SetLogx();
      //   Mont2[i]->GetXaxis()->SetNdivisions(0.5);
      gPad->SetGridy();
      colorIt(Mont2[i],kBlack,i+20);
      Mont2[i]->SetMinimum(0);
      Mont2[i]->SetMaximum(2.0);
      Mont2[i]->GetYaxis()->SetLabelSize(0.09);
      Mont2[i]->GetXaxis()->SetLabelSize(0.09);
      Mont2[i]->Draw("peX0C");
      colorIt(Mont2[i],kRed,i+20);
      Mont2[i]->GetYaxis()->SetTitleOffset(0.5);
      Mont2[i]->GetYaxis()->SetTitle("MC/Data");
      //    Mont2[i]->Draw("peX0Csame");
      if(i==no_ranges-1)Mont2[i]->SetXTitle(Xtitle); 
    }

    sprintf(nam,"histos_note_1605_md/ratio_Detector___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d.C",var3d.c_str(),isMu,ZPTweightcorr);
    c4->Print(nam);
    sprintf(nam,"histos_note_1605_md/ratio_Detector___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d.pdf",var3d.c_str(),isMu,ZPTweightcorr);
    c4->Print(nam);
    sprintf(nam,"histos_note_1605_md/ratio_Detector___HISTNAME__%s_ismu_%d_ZPTweightcorr_%d.png",var3d.c_str(),isMu,ZPTweightcorr);
    c4->Print(nam);

  }

 

}

