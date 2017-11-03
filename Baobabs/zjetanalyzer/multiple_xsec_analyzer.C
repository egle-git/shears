#include <iomanip>
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH2.h"
#include "TH3.h"
#include "TRandom.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "math.h"
#include <fstream>
#include <string>
#include <iostream>
#include <TStyle.h>
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TChain.h"
#include "TF2.h"
#include "TLegend.h"
#include "TH1F.h"
#include "TPostScript.h"
#include "TLine.h"
#include "TEllipse.h"
#include "TMath.h"
#include "TLatex.h"
#include "TLorentzVector.h"
#include <vector>
#include "standalone_LumiReWeighting.h"
#include "functions.h"
#include "LHAPDF/LHAPDF.h"
//#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"


using namespace std;
void tree1r()
{
    LHAPDF::initPDFSet(1, "cteq6ll.LHpdf");
//    LHAPDF::initPDFSet(2, "MSTW2008nlo68cl.LHgrid");
    LHAPDF::initPDFSet(2, "CT10.LHgrid");
    LHAPDF::initPDFSet(3, "CT10as.LHgrid");
  //  LHAPDF::initPDFSet(4, "CT10.LHgrid");


int samesign;
int doUnf;
int scale;
int PUSys;  
int tight=0;
int muon=1;
int JERcorr;
int JERsys;

bool noJERCorr=false;//no effect in data
float Gthresh = 20.;


int ZPTweightcorr=0;
  table TableJESunc("JESUnce_FT_53_V21_AN4_Uncertainty_AK5PFchs.txt"); 

      //ofstream Yfan;
      //Yfan.open ("askYfan.txt");
      //ofstream Yfan2;
      //Yfan2.open ("askYfan2.txt");
for (int rind=0;rind<=125;rind++){
 for(int sysind=0;sysind<63;sysind++){
  for(int ssind=0;ssind<2;ssind++){
    gROOT->Reset();
    if(sysind==0) doUnf=0;
    if(sysind==0) continue;//only Unfolding samples
  //  if(sysind!=0) continue;//run only reco st.
    int CT10ind=-1;
    if(sysind>=8 &&sysind<61){
       LHAPDF::usePDFMember(2,sysind-8);
       CT10ind=sysind-8;
      
    }
    if(sysind==62){
       LHAPDF::usePDFMember(3,4);
     CT10ind=sysind-8;
    }
    if(sysind==63){
       LHAPDF::usePDFMember(3,6);
     CT10ind=sysind-8;
    }

    if(sysind>0) doUnf=1;
    
    if(sysind==2)scale=1;
    else if(sysind==3)scale=-1;
    else scale=0;

    if(sysind==4)PUSys=1;
    else if(sysind==5)PUSys=-1;
    else PUSys=0;

    if(sysind==6)JERsys=1;
    else if(sysind==7)JERsys=-1;
    else JERsys=0;



    if (ssind==0) samesign=0;
    else samesign=1;

    if(ssind==1)continue;
    //if(sysind!=1)continue; //run only on central value with JER smearing..
    gStyle->SetPalette(1);
    float DeltaR(float eta1, float eta2, float phi1, float phi2);
    float DeltaPhi(float phi1, float phi2);


 //   if(rind!=0||sysind>3)continue;//only data

    if(rind!=15||sysind==2||sysind==3||sysind>7)continue; //MG no PDF variations
    //if(rind!=15||sysind<8)continue; //MG with PDF variations
  //  if(rind==0 || rind>14||sysind>1)continue; //run only background
    //if(rind!=18 ||sysind!=1)continue; // sherpa_unweighted

    //if(rind!=16||sysind!=1)continue; //MG incl only
    //if(rind!=17 ||ssind!=0||sysind!=1)continue; // sherpa_weighted

    //if(rind!=19 ||ssind!=0||sysind!=1)continue; // nonu
    //if(rind<20 ||rind>120||ssind!=0||sysind!=1)continue; // pull sample
    //if(rind!=1 ||ssind!=0||sysind!=1)continue; // ttbar only
    //if(rind!=124 ||ssind!=0||sysind!=1)continue; // Filter eff sample
    //if(rind!=125 ||ssind!=0)continue; //DY Filter eff sample
    TChain myTree("analyzeBasicPat/MuonTree");

  /////////////////////////////

    bool data=false;
    bool signal =false;
    bool sherpa=false;
    bool mc=false;
    TString dataname;
    TString outFileRoot;

    if(muon==0){
    if(rind==0){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_2012ABCD.root");
      data=true;
      dataname = "DATA";
      outFileRoot="rootfiles/MD_DoubleElec_";
    } 
    if(rind==1){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_TTjets_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_TTjets_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_TTjets_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_tt_";
      dataname = "tt";
    }  
    if(rind==2){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_WWJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_WWJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_WWJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_wwj2L2Nu_";
      dataname = "wwj2L2Nu";
    }
    if(rind==3){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_WZJetsTo2L2Q_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_WZJetsTo2L2Q_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_WZJetsTo2L2Q_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_wzj2L2Q_";
      dataname = "wzj2L2Q";      
    } 
    if(rind==4){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_WZJetsTo3LNu_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_WZJetsTo3LNu_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_WZJetsTo3LNu_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_wzj3LNu_";
      dataname = "wzj3LNu";
    } 
    if(rind==5){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_ZZJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_ZZJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_ZZJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_zzj2L2Nu_";
      dataname = "zzj2L2Nu";
    }
    if(rind==6){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_ZZjetTo2L2Q_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_ZZjetTo2L2Q_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_ZZjetTo2L2Q_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_zzj2L2Q_";
      dataname = "zzj2L2Q";
}
    if(rind==7){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_ZZJetsTo4L_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_ZZJetsTo4L_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/EE_hadd_ZZJetsTo4L_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_zzj4L_";
      dataname = "zzj4L";
    }
    if(rind==8){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_T_tW.root");
      outFileRoot="rootfiles/MD_T_tW_";
      dataname = "T_tW";
    }
    if(rind==9){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_T_t.root");
      outFileRoot="rootfiles/MD_T_t_";
      dataname = "T_t";
    }
    if(rind==10){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_T_s.root");
      outFileRoot="rootfiles/MD_T_s_";
      dataname = "T_s";
    }
    if(rind==11){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_Tbar_tW.root");
      outFileRoot="rootfiles/MD_Tbar_tW_";
      dataname = "Tbar_tW";
    }
    if(rind==12){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_Tbar_t.root");
      outFileRoot="rootfiles/MD_Tbar_t_";
      dataname = "Tbar_t";
    }
    if(rind==13){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_Tbar_s.root");
      outFileRoot="rootfiles/MD_Tbar_s_";
      dataname = "Tbar_s";
    }
    if(rind==14){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_WJets.root");
      outFileRoot="rootfiles/MD_WJets_";
      dataname = "WJets";
    }
    if(rind==15){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_1102.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_exc1jet.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_exc2jet.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_exc3jet.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_exc4jet.root");

      signal =true; 
      outFileRoot="rootfiles/MD_DYJetstoLL_";
      dataname = "DYJetstoLL";
      JERcorr=1;
      mc=true;
    }
    if(rind==16){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/hadd_DYtoLL_1102.root");
      signal =false;
      outFileRoot="rootfiles/MD_DYJetstoLL_inclonly";
      dataname = "rootfiles/MD_DYJetstoLL_inclonly";
      JERcorr=1;
      mc=true;
    }
    if(rind==17){
      #include "Z2jetNLO4jetLO_MPI_crab_seed2_Weighted_enhanced_sherpa2.0.0_.h"
      outFileRoot="rootfiles/MD_Sherpa200_";
      sherpa=true;
      dataname = "SHERPA 200 Weighted";
    }
    if(rind==18){
      #include "Z2jetNLO4jetLO_MPI_crab_seed2_Unweighted_enhanced_sherpa2.0.0_.h"
      outFileRoot="rootfiles/MD_Sherpa200_unweighted_";
      sherpa=true;
      dataname = "SHERPA 200 UnWeighted";
    }
    if(rind==19){
//      #include "Z2jetNLO4jetLO_MPI_crab_seed2_Unweighted_enhanced_sherpa2.0.0_.h"
     myTree.Add("../DY_singlefile.root");
      outFileRoot="273events_";
      //sherpa=true;
      dataname = "273events_";
      JERcorr=1;
      mc=true;
    }

    if(rind==20){
     myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_1102.root");

      outFileRoot="Pulltest_0_";
      //sherpa=true;
      dataname = "Pulltest_";
      JERcorr=1;
    }

    if(rind>20 && rind<121){
     myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_1102.root");

      outFileRoot="Pulltest_";
      outFileRoot+=(rind-20);
      //sherpa=true;
      dataname = "Pulltest_";
      dataname+=(rind-20);
      //data=true;
      JERcorr=1;
    }
    if(rind==121){
     myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_1102.root");
      outFileRoot="DY_filter_";
      dataname = "DY_filter_";
    }
    if(rind==122){
     myTree.Add("../DY_singlefile_withfilters.root");
      outFileRoot="DY_withfilters_";
      dataname = "DY_withfilters_";
    }

    if(rind==123){
     myTree.Add("../tt_singlefile_wofilters.root");
      outFileRoot="tt_wofilters_";
      dataname = "tt_wofilters_";
    }
    if(rind==124){
//     myTree.Add("../tt_singlefile_withfilters.root");
      #include "ttlist.h"
      outFileRoot="tt_withfilters_";
      dataname = "tt_withfilters_";
    }
    if(rind==125){
//     myTree.Add("../tt_singlefile_withfilters.root");
      #include "dylist.h"
      outFileRoot="dy_withfilters_";
      dataname = "dy_withfilters_";
    }

 
    }

    if(muon==1){
    if(rind==0){

      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012A_22Jan2013_MuEle_112213_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012A_22Jan2013_MuEle_112213_1_.root");
 
     myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_2012B_Mu_2504_0_.root");
     myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_2012B_Mu_2504_1_.root");
     myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_2012B_Mu_2504_2_.root");

      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012C_22Jan2013_MuEle_112213_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012C_22Jan2013_MuEle_112213_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012C_22Jan2013_MuEle_112213_2_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012C_22Jan2013_MuEle_112213_3_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012C_22Jan2013_MuEle_112213_4_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012C_22Jan2013_MuEle_112213_5_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012C_22Jan2013_MuEle_112213_6_.root");


      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012D_22Jan2013_MuEle_112213_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012D_22Jan2013_MuEle_112213_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012D_22Jan2013_MuEle_112213_2_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012D_22Jan2013_MuEle_112213_3_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012D_22Jan2013_MuEle_112213_4_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/MuMu_hadd_DoubleMu2012D_22Jan2013_MuEle_112213_5_.root");

      data=true;
      dataname = "DATA";
      outFileRoot="rootfiles/MD_DoubleElec_";
    } 
    if(rind==1){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_TTjets_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_TTjets_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_TTjets_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_tt_";
      dataname = "tt";
    }  
    if(rind==2){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_WWJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_WWJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_WWJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_wwj2L2Nu_";
      dataname = "wwj2L2Nu";
    }
    if(rind==3){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_WZJetsTo2L2Q_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_WZJetsTo2L2Q_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_WZJetsTo2L2Q_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_wzj2L2Q_";
      dataname = "wzj2L2Q";      
    } 
    if(rind==4){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_WZJetsTo3LNu_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_WZJetsTo3LNu_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_WZJetsTo3LNu_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_wzj3LNu_";
      dataname = "wzj3LNu";
    } 
    if(rind==5){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_ZZJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_ZZJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_ZZJetsTo2L2Nu_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_zzj2L2Nu_";
      dataname = "zzj2L2Nu";
    }
    if(rind==6){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_ZZjetTo2L2Q_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_ZZjetTo2L2Q_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_ZZjetTo2L2Q_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_zzj2L2Q_";
      dataname = "zzj2L2Q";
}
    if(rind==7){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_ZZJetsTo4L_8TeV_Summer12_MuEle_112313_0_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_ZZJetsTo4L_8TeV_Summer12_MuEle_112313_1_.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/skimmed/MuMu_hadd_ZZJetsTo4L_8TeV_Summer12_MuEle_112313_2_.root");
      outFileRoot="rootfiles/MD_zzj4L_";
      dataname = "zzj4L";
    }
    if(rind==8){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_T_tW.root");
      outFileRoot="rootfiles/MD_T_tW_";
      dataname = "T_tW";
    }
    if(rind==9){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_T_t.root");
      outFileRoot="rootfiles/MD_T_t_";
      dataname = "T_t";
    }
    if(rind==10){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_T_s.root");
      outFileRoot="rootfiles/MD_T_s_";
      dataname = "T_s";
    }
    if(rind==11){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_Tbar_tW.root");
      outFileRoot="rootfiles/MD_Tbar_tW_";
      dataname = "Tbar_tW";
    }
    if(rind==12){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_Tbar_t.root");
      outFileRoot="rootfiles/MD_Tbar_t_";
      dataname = "Tbar_t";
    }
    if(rind==13){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_Tbar_s.root");
      outFileRoot="rootfiles/MD_Tbar_s_";
      dataname = "Tbar_s";
    }
    if(rind==14){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/2111/hadd_WJets.root");
      outFileRoot="rootfiles/MD_WJets_";
      dataname = "WJets";
    }
    if(rind==15){
       myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_1102.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_exc1jet.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_exc2jet.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_exc3jet.root");
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_exc4jet.root");
      signal =true;
      outFileRoot="rootfiles/MD_DYJetstoLL_";
      dataname = "DYJetstoLL";
      JERcorr=1;
      mc=true;
    }
    if(rind==16){
      myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_1102.root");
      signal =false;
      outFileRoot="rootfiles/MD_DYJetstoLL_inclonly";
      dataname = "rootfiles/MD_DYJetstoLL_inclonly";
      JERcorr=1;
      mc=true;
    }
    if(rind==17){
      #include "Z2jetNLO4jetLO_MPI_crab_seed2_Weighted_enhanced_sherpa2.0.0_.h"
      outFileRoot="rootfiles/MD_Sherpa200_";
      sherpa=true;
      dataname = "SHERPA 200 Weighted";
    }
    if(rind==18){
      #include "Z2jetNLO4jetLO_MPI_crab_seed2_Unweighted_enhanced_sherpa2.0.0_.h"
      outFileRoot="rootfiles/MD_Sherpa200_unweighted_";
      sherpa=true;
      dataname = "SHERPA 200 UnWeighted";
    }

    if(rind==19){
//      #include "Z2jetNLO4jetLO_MPI_crab_seed2_Unweighted_enhanced_sherpa2.0.0_.h"
     myTree.Add("../DYJetsToLL_ntuple.root");
      outFileRoot="nonu_";
      //sherpa=true;
      dataname = "NONU";
      JERcorr=1;
    }

    if(rind==20){
     myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_1102.root");

      outFileRoot="Pulltest_0_";
      //sherpa=true;
      dataname = "Pulltest_0_";
      JERcorr=1;
    }

   if(rind>20 && rind<121){
     myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_1102.root");

      outFileRoot="Pulltest_";
      outFileRoot+=(rind-20);
      //sherpa=true;
      dataname = "Pulltest_";
      dataname+=(rind-20);
      //data=true;
      JERcorr=1;
    }
    if(rind==121){
     myTree.Add("/afs/cern.ch/user/b/bbilin/eos/cms/store/group/phys_smp/ZPlusJets/8TeV/ntuples/0202/skimmed/hadd_DYtoLL_1102.root");
      outFileRoot="DY_filter_";
      dataname = "DY_filter_";
    }
    if(rind==122){
     myTree.Add("../DY_singlefile_withfilters.root");
      outFileRoot="DY_withfilters_";
      dataname = "DY_withfilters_";
    }

    if(rind==123){
     myTree.Add("../tt_singlefile_wofilters.root");
      outFileRoot="tt_wofilters_";
      dataname = "tt_wofilters_";
    }
    if(rind==124){
//     myTree.Add("../tt_singlefile_withfilters.root");
      #include "ttlist.h"
      outFileRoot="tt_withfilters_";
      dataname = "tt_withfilters_";
    }
    if(rind==125){
//     myTree.Add("../tt_singlefile_withfilters.root");
      #include "dylist.h"
      outFileRoot="dy_withfilters_";
      dataname = "dy_withfilters_";
    }
    }  



if(noJERCorr)JERcorr=0;//no JER corr for now
    
  TH1::AddDirectory(true);
  //double j_y_range[5]={0,1.2,2.4,3.6,4.7};
int jet_rap_ind=7;
//  double j_y_range[]={0,0.5,1.0,1.5,2.0,2.5,3.0,3.2,4.7};
  double j_y_range[]={0,0.5,1.0,1.5,2.0,2.5,3.2,4.7};
   double Z_y_range[]={0,0.9,1.5,2.1,2.5};
    double threed_j_y_range[4]={0,1.5,2.4,4.7};
    double pt_range[6]={20.,30.,50.,100.,200.,1000.};
   //int jet_pt_ind=4;
    double j_pt_range[]={30.,50.,100.,200.,1000.};


  Double_t        PU_npT;
  Double_t        PU_npIT;
  double           EvtInfo_NumVtx;
  double          MyWeight,weight_three;
   double nup;
  double          realdata;

/*
  Int_t          event;


  int          lumi;*/
  int          run;
  double HLT_Elec17_Elec8;
  std::vector<double> *id1_pdfInfo_=0;
  std::vector<double> *id2_pdfInfo_=0;
  std::vector<double> *x1_pdfInfo_=0;
  std::vector<double> *x2_pdfInfo_=0;
  std::vector<double> *scalePDF_pdfInfo_=0;

  std::vector<double> *Dr01LepPt=0;
  std::vector<double> *Dr01LepEta=0;
  std::vector<double> *Dr01LepPhi=0;
  std::vector<double> *Dr01LepE=0;
  std::vector<double> *Dr01LepM=0;
  std::vector<double> *Dr01LepId=0;
  std::vector<double> *Dr01LepStatus=0;

 std::vector<double>  *Bare01LepPt=0;
  std::vector<double> *Bare01LepEta=0;
  std::vector<double> *Bare01LepPhi=0;
  std::vector<double> *Bare01LepE=0;
  std::vector<double> *Bare01LepM=0;
  std::vector<double> *Bare01LepId=0;
  std::vector<double> *Bare01LepStatus=0;

  std::vector<double> *St01PhotonPt=0;
  std::vector<double> *St01PhotonEta=0;
  std::vector<double> *St01PhotonPhi=0;
  std::vector<double> *St01PhotonE=0;
  std::vector<double> *St01PhotonM=0;
  std::vector<double> *St01PhotonId=0;
  std::vector<double> *St01PhotonMomId=0;
  std::vector<double> *St01PhotonNumberMom=0;
  std::vector<double> *St01PhotonStatus=0;



  std::vector<double> *St03Pt=0;
  std::vector<double> *St03Eta=0;
  std::vector<double> *St03Phi=0;
  std::vector<double> *St03E=0;
  std::vector<double> *St03M=0;
  std::vector<double> *St03Id=0;
  std::vector<double> *St03Status=0;

  std::vector<double> *GjPt=0;
  std::vector<double> *Gjeta=0;
  std::vector<double> *Gjphi=0;
  std::vector<double> *GjE=0;
  std::vector<double> *GjPx=0;
  std::vector<double> *GjPy=0;
  std::vector<double> *GjPz=0;

  vector<double>  *patMuonEn_ =0; 
  vector<double>  *patMuonCharge_ =0;
  vector<double>  *patMuonPt_ =0;
  vector<double>  *patMuonEta_=0;
  vector<double>  *patMuonPhi_ =0;
      vector<double>  *patMuonCombId_ =0;
      vector<double>  *patMuonTrig_ =0;
      vector<double>  *patMuonDetIsoRho_ =0;
      vector<double>  *patMuonPfIsoDbeta_ =0;

  std::vector<double> *patElecTrig_ =0;
  std::vector<double> *patElecCharge_ =0;
  std::vector<double> *patElecEnergy_ =0;
  std::vector<double> *patElecEta_ =0;
  std::vector<double> *patElecScEta_ =0;
  std::vector<double> *patElecPhi_ =0;
//  std::vector<double> *patElecEcalEnergy_ =0;
  std::vector<double> *patElecPt_ =0;
  std::vector<double> *patElecPfIso_ =0;
  std::vector<double> *patElecPfIsodb_ =0;
  std::vector<double> *patElecPfIsoRho_ =0;
  std::vector<double>   *patElecMediumIDOff_ =0;
//  std::vector<double> *patElecMVATrigId_ =0;
//  std::vector<double> *patElecMVANonTrigId_ =0;

  vector<double>  *patJetPfAk05En_ =0;
  vector<double>  *patJetPfAk05Pt_ =0;
  vector<double>  *patJetPfAk05Eta_ =0;
  vector<double>  *patJetPfAk05Phi_ =0;
//  vector<double>  *patJetPfAk05JesUncert_ =0;
  vector<double>  *patJetPfAk05LooseId_ =0;
  vector<double>  *patJetPfAk05Et_ =0;
//  vector<double>  *patJetPfAk05HadEHF_ =0;
//  vector<double>  *patJetPfAk05EmEHF_ =0;
  vector<double>  *patJetPfAk05RawPt_ =0;

  vector<double>  *patJetPfAk05RawEn_ =0;
//  vector<double>  *patJetPfAk05jetBetaClassic_ =0;
//  vector<double>  *patJetPfAk05jetBeta_ =0;
//  vector<double>  *patJetPfAk05jetBetaStar_ =0;
//  vector<double>  *patJetPfAk05jetBetaStarClassic_ =0;
  vector<double>  *patJetPfAk05jetpuMVA_ =0;

  vector<bool>  *patJetPfAk05jetpukLoose_ =0;
  vector<bool>  *patJetPfAk05jetpukMedium_ =0;
  vector<bool>  *patJetPfAk05jetpukTight_ =0;
  std::vector<bool> *matchGjet=0;
  std::vector<double> *MGjPt=0;
  std::vector<double> *MGjeta=0;
  std::vector<double> *MGjphi=0;
  std::vector<double> *MGjE=0;

  vector<double> *patJetPfAk05chf_=0;
  vector<double> *patJetPfAk05nhf_=0;
  vector<double> *patJetPfAk05cemf_=0;
  vector<double> *patJetPfAk05nemf_=0;
  vector<double> *patJetPfAk05cmult_=0;
  vector<double> *patJetPfAk05nconst_=0;





  myTree.SetBranchAddress("id1_pdfInfo_",&id1_pdfInfo_);
   myTree.SetBranchAddress("id2_pdfInfo_",&id2_pdfInfo_);
   myTree.SetBranchAddress("x1_pdfInfo_",&x1_pdfInfo_);
   myTree.SetBranchAddress("x2_pdfInfo_",&x2_pdfInfo_);
   myTree.SetBranchAddress("scalePDF_pdfInfo_",&scalePDF_pdfInfo_);


  myTree.SetBranchAddress("MyWeight",&MyWeight);
if(!sherpa){
  myTree.SetBranchAddress("realdata",&realdata);
 /* myTree.SetBranchAddress("event",&event);

  myTree.SetBranchAddress("lumi",&lumi);*/
  if(muon)myTree.SetBranchAddress("run",&run);
  myTree.SetBranchAddress("PU_npT", &PU_npT);
  myTree.SetBranchAddress("PU_npIT", &PU_npIT);
  myTree.SetBranchAddress("nup", &nup);
  myTree.SetBranchAddress("EvtInfo_NumVtx", &EvtInfo_NumVtx);

  myTree.SetBranchAddress("patMuonEn_", &patMuonEn_);
  myTree.SetBranchAddress("patMuonCharge_", &patMuonCharge_);
  myTree.SetBranchAddress("patMuonPt_", &patMuonPt_);
  myTree.SetBranchAddress("patMuonEta_", &patMuonEta_);
  myTree.SetBranchAddress("patMuonPhi_", &patMuonPhi_);
  myTree.SetBranchAddress("patMuonCombId_", &patMuonCombId_);
  myTree.SetBranchAddress("patMuonTrig_", &patMuonTrig_);
  myTree.SetBranchAddress("patMuonDetIsoRho_", &patMuonDetIsoRho_);
  myTree.SetBranchAddress("patMuonPfIsoDbeta_", &patMuonPfIsoDbeta_);



  myTree.SetBranchAddress("patElecCharge_",&patElecCharge_);
  myTree.SetBranchAddress("patElecPt_",&patElecPt_);
  myTree.SetBranchAddress("patElecEnergy_",&patElecEnergy_);
//  myTree.SetBranchAddress("patElecEcalEnergy_",&patElecEcalEnergy_);
  myTree.SetBranchAddress("patElecEta_",&patElecEta_);
  myTree.SetBranchAddress("patElecScEta_",&patElecScEta_);
  myTree.SetBranchAddress("patElecPhi_",&patElecPhi_);
  myTree.SetBranchAddress("patElecPfIso_",&patElecPfIso_);
  myTree.SetBranchAddress("patElecPfIsodb_",&patElecPfIsodb_);
  myTree.SetBranchAddress("patElecPfIsoRho_",&patElecPfIsoRho_);
  myTree.SetBranchAddress("patElecMediumIDOff_",&patElecMediumIDOff_);
//  myTree.SetBranchAddress("patElecMVATrigId_",&patElecMVATrigId_);
//  myTree.SetBranchAddress("patElecMVANonTrigId_",&patElecMVANonTrigId_);
  myTree.SetBranchAddress("patElecTrig_",&patElecTrig_);

   myTree.SetBranchAddress("patJetPfAk05RawEn_",&patJetPfAk05RawEn_);
   //myTree.SetBranchAddress("patJetPfAk05HadEHF_",&patJetPfAk05HadEHF_);
   //myTree.SetBranchAddress("patJetPfAk05EmEHF_",&patJetPfAk05EmEHF_);
   myTree.SetBranchAddress("patJetPfAk05chf_",&patJetPfAk05chf_);
   myTree.SetBranchAddress("patJetPfAk05nhf_",&patJetPfAk05nhf_);
   myTree.SetBranchAddress("patJetPfAk05cemf_",&patJetPfAk05cemf_);
   myTree.SetBranchAddress("patJetPfAk05nemf_",&patJetPfAk05nemf_);
   myTree.SetBranchAddress("patJetPfAk05cmult_",&patJetPfAk05cmult_);
   myTree.SetBranchAddress("patJetPfAk05nconst_",&patJetPfAk05nconst_);
  myTree.SetBranchAddress("patJetPfAk05En_", &patJetPfAk05En_);
  myTree.SetBranchAddress("patJetPfAk05Pt_", &patJetPfAk05Pt_);
  myTree.SetBranchAddress("patJetPfAk05Eta_", &patJetPfAk05Eta_);
  myTree.SetBranchAddress("patJetPfAk05Phi_", &patJetPfAk05Phi_);
//  myTree.SetBranchAddress("patJetPfAk05JesUncert_", &patJetPfAk05JesUncert_);
  myTree.SetBranchAddress("patJetPfAk05LooseId_", &patJetPfAk05LooseId_);
  myTree.SetBranchAddress("patJetPfAk05Et_", &patJetPfAk05Et_);
//  myTree.SetBranchAddress("patJetPfAk05HadEHF_", &patJetPfAk05HadEHF_);
//  myTree.SetBranchAddress("patJetPfAk05EmEHF_", &patJetPfAk05EmEHF_);
  myTree.SetBranchAddress("patJetPfAk05RawPt_", &patJetPfAk05RawPt_);
//  myTree.SetBranchAddress("patJetPfAk05RawEn_", &patJetPfAk05RawEn_);
//  myTree.SetBranchAddress("patJetPfAk05jetBetaClassic_", &patJetPfAk05jetBetaClassic_);
//  myTree.SetBranchAddress("patJetPfAk05jetBeta_", &patJetPfAk05jetBeta_);
//  myTree.SetBranchAddress("patJetPfAk05jetBetaStar_", &patJetPfAk05jetBetaStar_);
//  myTree.SetBranchAddress("patJetPfAk05jetBetaStarClassic_", &patJetPfAk05jetBetaStarClassic_);
   myTree.SetBranchAddress("patJetPfAk05jetpuMVA_", &patJetPfAk05jetpuMVA_);
   myTree.SetBranchAddress("patJetPfAk05jetpukLoose_", &patJetPfAk05jetpukLoose_);
   myTree.SetBranchAddress("patJetPfAk05jetpukMedium_", &patJetPfAk05jetpukMedium_);
   myTree.SetBranchAddress("patJetPfAk05jetpukTight_", &patJetPfAk05jetpukTight_);
  myTree.SetBranchAddress("matchGjet",&matchGjet);
  myTree.SetBranchAddress("MGjPt",&MGjPt);
  myTree.SetBranchAddress("MGjeta",&MGjeta);
  myTree.SetBranchAddress("MGjphi",&MGjphi);
  myTree.SetBranchAddress("MGjE",&MGjE); 
   myTree.SetBranchAddress("HLT_Elec17_Elec8",&HLT_Elec17_Elec8);
}
//Gen jets//
  myTree.SetBranchAddress("GjPt",&GjPt);
  myTree.SetBranchAddress("Gjeta",&Gjeta);
  myTree.SetBranchAddress("Gjphi",&Gjphi);
  myTree.SetBranchAddress("GjE",&GjE);
  myTree.SetBranchAddress("GjPx",&GjPx);
  myTree.SetBranchAddress("GjPy",&GjPy);
  myTree.SetBranchAddress("GjPz",&GjPz); 
  //Dressed Leptons//
  myTree.SetBranchAddress("Dr01LepPt",&Dr01LepPt);
  myTree.SetBranchAddress("Dr01LepEta",&Dr01LepEta);
  myTree.SetBranchAddress("Dr01LepPhi",&Dr01LepPhi);
  myTree.SetBranchAddress("Dr01LepE",&Dr01LepE);
  myTree.SetBranchAddress("Dr01LepM",&Dr01LepM);
  myTree.SetBranchAddress("Dr01LepId",&Dr01LepId);
  myTree.SetBranchAddress("Dr01LepStatus",&Dr01LepStatus);

     myTree.SetBranchAddress("Bare01LepPt",&Bare01LepPt);
     myTree.SetBranchAddress("Bare01LepEta",&Bare01LepEta);
     myTree.SetBranchAddress("Bare01LepPhi",&Bare01LepPhi);
     myTree.SetBranchAddress("Bare01LepE",&Bare01LepE);
     myTree.SetBranchAddress("Bare01LepM",&Bare01LepM);
     myTree.SetBranchAddress("Bare01LepId",&Bare01LepId);
     myTree.SetBranchAddress("Bare01LepStatus",&Bare01LepStatus);

     myTree.SetBranchAddress("St01PhotonPt",&St01PhotonPt);
     myTree.SetBranchAddress("St01PhotonEta",&St01PhotonEta);
     myTree.SetBranchAddress("St01PhotonPhi",&St01PhotonPhi);
     myTree.SetBranchAddress("St01PhotonE",&St01PhotonE);
     myTree.SetBranchAddress("St01PhotonM",&St01PhotonM);
     myTree.SetBranchAddress("St01PhotonId",&St01PhotonId);
     myTree.SetBranchAddress("St01PhotonMomId",&St01PhotonMomId);
     myTree.SetBranchAddress("St01PhotonNumberMom",&St01PhotonNumberMom);
     myTree.SetBranchAddress("St01PhotonStatus",&St01PhotonStatus);


  myTree.SetBranchAddress("St03Pt",&St03Pt);
  myTree.SetBranchAddress("St03Eta",&St03Eta);
  myTree.SetBranchAddress("St03Phi",&St03Phi);
  myTree.SetBranchAddress("St03E",&St03E);
  myTree.SetBranchAddress("St03M",&St03M);
  myTree.SetBranchAddress("St03Id",&St03Id);
  myTree.SetBranchAddress("St03Status",&St03Status);



  if(sherpa)    myTree.SetBranchAddress("weight_three",&weight_three);


  ////Histogram booking///////////
  cout<<samesign<<endl;
  outFileRoot+="isMu_";
  outFileRoot+=(muon);
  outFileRoot+="_doUnf_";
  outFileRoot+=(doUnf);
  outFileRoot+="_isSS_";
  outFileRoot+=(samesign);
  outFileRoot+="_jetPt_";
  outFileRoot+=(Gthresh);
  outFileRoot+="_Tightjet_";
  outFileRoot+=(tight);
  outFileRoot+="_JES_";
  outFileRoot+=(scale);
  outFileRoot+="_JER_";
  outFileRoot+=(JERsys);
  outFileRoot+="_PU_";
  outFileRoot+=(PUSys);
  outFileRoot+="_ZPTweightcorr_";
  outFileRoot+=(ZPTweightcorr);
  outFileRoot+="_CT10ind_";
  outFileRoot+=(CT10ind);
  outFileRoot+="_18_05.root";
  TFile *theFile = new TFile (outFileRoot,"RECREATE");
//  TFile *theFile = new TFile ("test.root","RECREATE");


  cout<<"SYSID "<<sysind<<" doUnf "<<doUnf<<" scale "<<scale<<"  JERsys "<<JERsys<<" PUSys "<<PUSys<<endl;


  theFile->cd();

  #include "histos_MD.h"


  double cnt0 = 0.;
  double test = 0.;

  int puYear(2013);
    cout << "Pile Up Distribution: " << puYear << endl;
    standalone_LumiReWeighting puWeight(puYear), puUp(puYear,1), puDown(puYear,-1);

  int NZtotal=0;
  int nmin=0;
  Int_t nevent = myTree.GetEntries();

  
  if(rind==20 )nevent=6800000.;
  if(rind>20 &&rind<121){
    nmin=6800000. + (rind-20.)*70000.;
    nevent= 6800000. + (rind-19)*70000.;
  }  


  // nevent =1000;
  for (Int_t iev=nmin;iev<nevent;iev++) {
  
   //cout<< x1_pdfInfo_->size()<<" "<<scalePDF_pdfInfo_->size()<<"  "<<id1_pdfInfo_->size()<<endl;


//cout<<run<<" "<<lumi<<" "<<event<<endl;
   int Reco=0;
//    int Reco_BEF=0;
    int Gen=0;

    if (iev%100000 == 0) cout<<dataname<<" ===>>> "<<iev<<"/"<<nevent<<endl;
    myTree.GetEntry(iev);  
   //#include "alex_evnt.h"
   //Yfan2<<"event infor : "<<run<<" "<<event<<endl;
   //cout<<"IS IT IN THE FUCKING TREE??"<<endl;  
    double weight_add=1;
    double weight_pdf=1;
    if (x1_pdfInfo_->size()!=0){
     if(sysind>=8 &&sysind<61){
      double x1fx1 = LHAPDF::xfx(2, x1_pdfInfo_->at(0),scalePDF_pdfInfo_->at(0) , id1_pdfInfo_->at(0));
      double x2fx2 = LHAPDF::xfx(2, x2_pdfInfo_->at(0),scalePDF_pdfInfo_->at(0) , id2_pdfInfo_->at(0));
      double x1fx1c = LHAPDF::xfx(1, x1_pdfInfo_->at(0),scalePDF_pdfInfo_->at(0) , id1_pdfInfo_->at(0));
      double x2fx2c = LHAPDF::xfx(1, x2_pdfInfo_->at(0),scalePDF_pdfInfo_->at(0) , id2_pdfInfo_->at(0));
      weight_pdf= (x1fx1*x2fx2)/(x1fx1c*x2fx2c);
     }  
     if(sysind==62 ||sysind==63 ){
      double x1fx1 = LHAPDF::xfx(3, x1_pdfInfo_->at(0),scalePDF_pdfInfo_->at(0) , id1_pdfInfo_->at(0));
      double x2fx2 = LHAPDF::xfx(3, x2_pdfInfo_->at(0),scalePDF_pdfInfo_->at(0) , id2_pdfInfo_->at(0));
      double x1fx1c = LHAPDF::xfx(1, x1_pdfInfo_->at(0),scalePDF_pdfInfo_->at(0) , id1_pdfInfo_->at(0));
      double x2fx2c = LHAPDF::xfx(1, x2_pdfInfo_->at(0),scalePDF_pdfInfo_->at(0) , id2_pdfInfo_->at(0));
      weight_pdf= (x1fx1*x2fx2)/(x1fx1c*x2fx2c);

     }  



      //std::cout << x1fx1 <<"  "<<x2fx2<< std::endl;
    }  
    //Yfan<<event<<endl;
 //  if(event<0) Yfan<<run<<":"<<lumi<<":"<<event<<endl;  
  //cout<<event<<endl;
  //  #include "/afs/cern.ch/work/b/bbilin/public/tomextra.h"
  //  cout<<run<<":"<<lumi<<":"<<event<<endl;

    //if(event==1295935||  event==1297192) cout<<"AAAAAAA "<<HLT_Elec17_Elec8<<" "<<Dr01LepPt->size()<<endl<<endl<<endl<<endl;
    //if(iev>950)cout<<event<<endl;

//if(event==45040154|| event==45040334) cout<<run<<":"<<lumi<<":"<<event<<",";

  //  if(HLT_Elec17_Elec8!=1&& trig_eff_contr)continue;
 
//    double merge_weight1[5]={2.43334,0.401605,1.06595,0.127971,0.0927669};
//    double merge_weight2[5]={0.994716,1.39383,0.997847,1.00848,0.979974};
//    double merge_weight3[5]={0.996004,0.80321,0.998069,1.00034,1.00819};
//    double merge_weight4[5]={1.02167,1.05373,1.02798,0.943166,1.01159};
//    double merge_weight5[5]={0.991224,0.991236,0.99122,0.991242,0.947877};

double Ratio_Z_PT_inc[]= {1.01085 , 0.956019 , 0.959092 , 0.953853 , 0.960982 , 0.963662 , 0.975881 , 0.988958 , 1.00337 , 1.00512 , 1.00425 , 1.02567 , 1.04461 , 1.08428 , 1.1278 , 1.03515 , 1.10712 , 1.16709 , 1.24219 , 0.962722 , 1.15022 , 0.955797 , 0.955436 , 1.1662 , 1.17251 , 0.942143 , 0.789333 , 0.731394 , 1.61048 , 2.15411 , 0.951515 , 3.15155 , 0.613088 , 0.739693 , 2.39452 , 1.09414 , 0 , 0.320411 , 0.285376 , 0.284474 , 0.313535 }; //MC/data

//    double merge_weight[5]={2.44144,0.469618,1.08172,0.120696,0.0878833};
    double merge_weight[5]={1,0.192804,0.442983,0.049396,0.0360817};
    if(signal){
      double n_parton=nup-5;
      for(int n=0;n<5;n++){
        if(n_parton ==n ){
//          weight_add=merge_weight1[n]*merge_weight2[n]*merge_weight3[n]*merge_weight4[n];
          weight_add=merge_weight[n];
        }
      }
    }



  ///////////////////////////////
    double PUweight =1;
    double PUweightUp =1;
    double PUweightDn =1;

    if (PU_npT > 0) PUweight = PUweight * puWeight.weight(int(PU_npT))*weight_add*weight_pdf;
    if (PU_npT > 0) PUweightUp = PUweightUp * puUp.weight(int(PU_npT))*weight_add*weight_pdf;
    if (PU_npT > 0) PUweightDn = PUweightDn * puDown.weight(int(PU_npT))*weight_add*weight_pdf;

    if (sherpa) PUweight=MyWeight;
    if(sherpa)     test+= weight_three;
    //cout<<log10(PUweight)<<" "<<PUweight<<endl;
    if(PUweight>=0)h_weight_plus_sherpa->Fill(log10(PUweight));
    if(PUweight<0)h_weight_minus_sherpa->Fill(log10(-1*PUweight));

    if(PUSys==1)PUweight=PUweightUp;
    if(PUSys==-1)PUweight=PUweightDn;
//    if(!sherpa)PUweight=1.;
    ////////////////////////////////////////////////////////
//    double weight=1;
//    cout<<PUweight<<endl;
    cnt0+=PUweight;
    h_nup->Fill(nup-5,PUweight);

    int Gcommon = 0;
    int Gcommon_mu = 0;
    int Gcommon_st3e = 0;
    int Gcommon_st3mu = 0;
    float Gptcut = 20.;
    int Gindex1[10],Gindex2[10],Gindex1_mu[10],Gindex2_mu[10],Gindex1_st3e[10],Gindex2_st3e[10],Gindex1_st3mu[10],Gindex2_st3mu[10];

//    for (unsigned int j = 0; j < Bare01LepPt->size(); ++j){  
//      for (unsigned int jk = 0; jk < St01PhotonPt->size(); ++j){  
//        double dr_lp= DeltaR(Bare01LepEta->at(pf),St01PhotonEta->at(Genind1),Bare01Lepphi->at(pf),St01PhotonPhi->at(Genind1));
        
//        if(dr_lp<0.1)
 //     cout << " gen Leptons no FSR:  " << genLepNoFSR.pt  <<"   FSR" << tmpGenLep.Pt() << endl;
//      }
//    }

    for (unsigned int j = 0; j < Dr01LepPt->size(); ++j){  

      for (unsigned int jk = j; jk < Dr01LepPt->size(); ++jk){
       // if(fabs(Dr01LepId->at(j))==11)cout<<Dr01LepId->at(j)<<endl;
        if (j == jk) continue;
        if ( Dr01LepStatus->at(j) == 1 && Dr01LepStatus->at(jk) == 1 &&
	     (Dr01LepId->at(j)*Dr01LepId->at(jk)) == -121  //common1
            ){

	    h_nup_elec->Fill(nup-5,PUweight);
            }
        if ( Dr01LepStatus->at(j) == 1 && Dr01LepStatus->at(jk) == 1 &&
	     (Dr01LepId->at(j)*Dr01LepId->at(jk)) == -169 //common1
            ){
	    h_nup_mu->Fill(nup-5,PUweight);
            }
        if ( Dr01LepStatus->at(j) == 1 && Dr01LepStatus->at(jk) == 1 && (Bare01LepPt->at(j)>=0.25 && Bare01LepPt->at(jk)>=0.25) &&
	     (Dr01LepId->at(j)*Dr01LepId->at(jk)) == -121 && //common1
	     Dr01LepPt->at(j) > Gptcut && Dr01LepPt->at(jk) > Gptcut && //common2
	     fabs(Dr01LepEta->at(j)) <2.4 && fabs(Dr01LepEta->at(jk)) <2.4 //common3
            ){
            h_nup_elec_with_cuts->Fill(nup-5,PUweight);
            h_nup_elec_with_cuts_noweigt->Fill(nup-5);

          //cout<<"dielec"<<endl;
          Gindex1[Gcommon] = j;
          Gindex2[Gcommon] = jk;
          Gcommon++;
        }
       if ( Dr01LepStatus->at(j) == 1 && Dr01LepStatus->at(jk) == 1 && (Bare01LepPt->at(j)>=0.25 && Bare01LepPt->at(jk)>=0.25) &&
	     (Dr01LepId->at(j)*Dr01LepId->at(jk)) == -169 && //common1
	     Dr01LepPt->at(j) > Gptcut && Dr01LepPt->at(jk) > Gptcut && //common2
	     fabs(Dr01LepEta->at(j)) <2.4 && fabs(Dr01LepEta->at(jk)) <2.4 //common3
            ){
          //cout<<"dimu"<<endl;
            h_nup_mu_with_cuts->Fill(nup-5,PUweight);
            h_nup_mu_with_cuts_noweigt->Fill(nup-5);

	  Gindex1_mu[Gcommon_mu] = j;
          Gindex2_mu[Gcommon_mu] = jk;
          Gcommon_mu++;
        }
      }
    }
    int tau=0;
    for (unsigned int j = 0; j < St03Pt->size(); ++j){  

      if(St03Id->at(j)==15)tau=1;
      for (unsigned int jk = j; jk < St03Pt->size(); ++jk){
        if (j == jk) continue;
        if ( St03Status->at(j) == 3 && St03Status->at(jk) == 3 &&
	     (St03Id->at(j)*St03Id->at(jk)) == -121 && //common1
	     St03Pt->at(j) > Gptcut && St03Pt->at(jk) > Gptcut && //common2
	     fabs(St03Eta->at(j)) <2.4 && fabs(St03Eta->at(jk)) <2.4 //common3
            ){
          Gindex1_st3e[Gcommon_st3e] = j;
          Gindex2_st3e[Gcommon_st3e] = jk;
//          if(event==)
          Gcommon_st3e++;
        }

       if ( St03Status->at(j) == 3 && St03Status->at(jk) == 3 &&
	     (St03Id->at(j)*St03Id->at(jk)) == -169 && //common1
	     St03Pt->at(j) > Gptcut && St03Pt->at(jk) > Gptcut && //common2
	     fabs(St03Eta->at(j)) <2.4 && fabs(St03Eta->at(jk)) <2.4 //common3
            ){
          Gindex1_st3mu[Gcommon_st3mu] = j;
          Gindex2_st3mu[Gcommon_st3mu] = jk;
          Gcommon_st3mu++;
        }
      }
    }
   // if(tau==1)continue;


 //   cout<<Gcommon<<endl;

    float GMZElec = 0;
    float Gdielecpt =0;
    float Gdielecrapidity =0;
    float Gdielecphi =0;
    float GMZElec_mu = 0;
    float Gdielecpt_mu =0;
    float Gdielecrapidity_mu =0;
    float Gdielecphi_mu =0;
    float GMZElec_st3mu = 0;
    float Gdielecpt_st3mu =0;
    float Gdielecrapidity_st3mu =0;
    float Gdielecphi_st3mu =0;
    float GMZElec_st3e = 0;
    float Gdielecpt_st3e =0;
    float Gdielecrapidity_st3e =0;
    float Gdielecphi_st3e =0;

    int Gind1 = -99;
    int Gind2 = -99;
    int Gind1_mu = -99;
    int Gind2_mu = -99;
    int Gind1_st3mu = -99;
    int Gind2_st3mu = -99;
    int Gind1_st3e = -99;
    int Gind2_st3e = -99;

    for (int gg = 0;gg<Gcommon;++gg){
      Gind1 = Gindex1[gg];
      Gind2 = Gindex2[gg];
    }

    for (int gg = 0;gg<Gcommon_mu;++gg){
      Gind1_mu = Gindex1_mu[gg];
      Gind2_mu = Gindex2_mu[gg];
    }

    for (int gg = 0;gg<Gcommon_st3mu;++gg){
      Gind1_st3mu = Gindex1_st3mu[gg];
      Gind2_st3mu = Gindex2_st3mu[gg];
    }

    for (int gg = 0;gg<Gcommon_st3e;++gg){
      Gind1_st3e = Gindex1_st3e[gg];
      Gind2_st3e = Gindex2_st3e[gg];
    }


    if (Gind1 != -99 && Gind2 != -99){

      TLorentzVector m1;
      TLorentzVector m2;
      m1.SetPtEtaPhiE(Dr01LepPt->at(Gind1), Dr01LepEta->at(Gind1) ,Dr01LepPhi->at(Gind1),Dr01LepE->at(Gind1) );
      m2.SetPtEtaPhiE(Dr01LepPt->at(Gind2), Dr01LepEta->at(Gind2) ,Dr01LepPhi->at(Gind2),Dr01LepE->at(Gind2) );
      GMZElec= (m1+m2).M();

      if (GMZElec > 20.){
        Gdielecpt = (m1+m2).Perp(); 
        Gdielecrapidity = (m1+m2).Rapidity();
        Gdielecphi=(m1+m2).Phi();
        if (GMZElec > 71.&& GMZElec < 111. &&Gcommon_st3e>0){
         //   cout <<"gen info: "<<event<< "  PT1, Eta1, phi1    "<<Dr01LepPt->at(Gind1)<<"    "<< Dr01LepEta->at(Gind1) <<"    "<<Dr01LepPhi->at(Gind1)<<" Pt2, Eta2, phi2  "<<Dr01LepPt->at(Gind2)<<"    "<< Dr01LepEta->at(Gind2)<<"    "<<Dr01LepPhi->at(Gind2)<<"Bare Pt1 eta1  "<<Bare01LepPt->at(Gind1)<<"  "<<Bare01LepEta->at(Gind1)<<" Bare Pt2 eta2  "<<Bare01LepPt->at(Gind2)<<"  "<<Bare01LepEta->at(Gind2)<</*St03Pt->at(Gind1_st3e)<<"  "<<St03Eta->at(Gind1_st3e)<<"  "<<*/endl;
//St03Pt->at(Gind2_st3e)<<"  "<<St03Eta->at(Gind2_st3e)<<endl;
   
          h_GMZElec->Fill(GMZElec,PUweight);
	  h_GdiElecphi->Fill(Gdielecphi,PUweight);
	  h_GdiElecpt->Fill(Gdielecpt,PUweight);
	  h_GdiElecrapidity->Fill(Gdielecrapidity,PUweight);
        }
      }
    }

   if (Gind1_mu != -99 && Gind2_mu != -99){

      TLorentzVector m1;
      TLorentzVector m2;
      m1.SetPtEtaPhiE(Dr01LepPt->at(Gind1_mu), Dr01LepEta->at(Gind1_mu) ,Dr01LepPhi->at(Gind1_mu),Dr01LepE->at(Gind1_mu) );
      m2.SetPtEtaPhiE(Dr01LepPt->at(Gind2_mu), Dr01LepEta->at(Gind2_mu) ,Dr01LepPhi->at(Gind2_mu),Dr01LepE->at(Gind2_mu) );
      GMZElec_mu= (m1+m2).M();

      if (GMZElec_mu > 20.){
        Gdielecpt_mu = (m1+m2).Perp(); 
        Gdielecrapidity_mu = (m1+m2).Rapidity();
        Gdielecphi_mu=(m1+m2).Phi();
        if (GMZElec_mu > 71.&& GMZElec_mu < 111.){
          h_GMZElec_mu->Fill(GMZElec_mu,PUweight);
	  h_GdiElecphi_mu->Fill(Gdielecphi_mu,PUweight);
	  h_GdiElecpt_mu->Fill(Gdielecpt_mu,PUweight);
	  h_GdiElecrapidity_mu->Fill(Gdielecrapidity_mu,PUweight);
        }
      }
    }

   if (Gind1_st3mu != -99 && Gind2_st3mu != -99){

      TLorentzVector m1;
      TLorentzVector m2;
      m1.SetPtEtaPhiE(St03Pt->at(Gind1_st3mu), St03Eta->at(Gind1_st3mu) ,St03Phi->at(Gind1_st3mu),St03E->at(Gind1_st3mu) );
      m2.SetPtEtaPhiE(St03Pt->at(Gind2_st3mu), St03Eta->at(Gind2_st3mu) ,St03Phi->at(Gind2_st3mu),St03E->at(Gind2_st3mu) );
      GMZElec_st3mu= (m1+m2).M();

      if (GMZElec_st3mu > 20.){
        Gdielecpt_st3mu = (m1+m2).Perp(); 
        Gdielecrapidity_st3mu = (m1+m2).Rapidity();
        Gdielecphi_st3mu=(m1+m2).Phi();
        if (GMZElec_st3mu > 71.&& GMZElec_st3mu < 111.){
   
          h_GMZElec_st3mu->Fill(GMZElec_st3mu,PUweight);
	  h_GdiElecphi_st3mu->Fill(Gdielecphi_st3mu,PUweight);
	  h_GdiElecpt_st3mu->Fill(Gdielecpt_st3mu,PUweight);
	  h_GdiElecrapidity_st3mu->Fill(Gdielecrapidity_st3mu,PUweight);
        }
      }
    }

   if (Gind1_st3e != -99 && Gind2_st3e != -99){

      TLorentzVector m1;
      TLorentzVector m2;
      m1.SetPtEtaPhiE(St03Pt->at(Gind1_st3e), St03Eta->at(Gind1_st3e) ,St03Phi->at(Gind1_st3e),St03E->at(Gind1_st3e) );
      m2.SetPtEtaPhiE(St03Pt->at(Gind2_st3e), St03Eta->at(Gind2_st3e) ,St03Phi->at(Gind2_st3e),St03E->at(Gind2_st3e) );
      GMZElec_st3e= (m1+m2).M();

      if (GMZElec_st3e > 20.){
        Gdielecpt_st3e = (m1+m2).Perp(); 
        Gdielecrapidity_st3e = (m1+m2).Rapidity();
        Gdielecphi_st3e=(m1+m2).Phi();
        if (GMZElec_st3e > 71.&& GMZElec_st3e < 111.){
   
          h_GMZElec_st3e->Fill(GMZElec_st3e,PUweight);
	  h_GdiElecphi_st3e->Fill(Gdielecphi_st3e,PUweight);
	  h_GdiElecpt_st3e->Fill(Gdielecpt_st3e,PUweight);
	  h_GdiElecrapidity_st3e->Fill(Gdielecrapidity_st3e,PUweight);

        }
      }
    }


   // cout<<"AAAAAA"<<endl;
    double eff[6][5];
    double etarange[6]={0.,0.8,1.4442,1.556,2.0,2.5};
    double ptrange[6]={20.,30.,40.,50.,200.,1000.};
      eff[0][0]=0.986*0.982465; //new SF's
      eff[0][1]=1.002*0.987814;
      eff[0][2]=1.005*0.989732;
      eff[0][3]=1.004*0.989879;
      eff[0][4]=1.004*0.989879;
      eff[1][0]=0.959*0.993035;
      eff[1][1]=0.980*0.993157;
      eff[1][2]=0.988*0.992539;
      eff[1][3]=0.988*0.991053;
      eff[1][4]=0.988*0.991053;
      eff[2][0]=0.967*1.01586;
      eff[2][1]=0.950*0.985387;
      eff[2][2]=0.958*0.987332;
      eff[2][3]=0.966*0.974333;
      eff[2][4]=0.966*0.974333;
      eff[3][0]=0.941*0.988158;
      eff[3][1]=0.967*0.992502;
      eff[3][2]=0.992*0.991673;
      eff[3][3]=1.*0.989659;
      eff[3][4]=1.*0.989659;
      eff[4][0]=1.020*1.00156;
      eff[4][1]=1.021*1.00426;
      eff[4][2]=1.019*1.00514;
      eff[4][3]=1.022*0.997845;
      eff[4][4]=1.022*0.997845;

   ///////////////////////////////
    //double weight=1;
    //double weight_data=1;
    double ID_Weight_0=1;
    double ID_Weight_1=1;
    double Iso_Weight_0=1;
    double Iso_Weight_1=1;
    double trig_Weight=1;
    double weight_tmp=1;
    double eff1 = -99.;
    double eff2 = -99.;


    const Float_t etlow[12]   = {10,20,25,30,35,40,50,60,90,140,300,500};
    const Float_t etalow[5] = {0,0.9,1.2,2.1,2.4};

     double ID_eff[11][4] ={
        {0.984868,0.986855,1.01235,0.994},
        {0.988681,0.987375,1.00155,0.994},
        {0.993889,0.994212,0.999149,0.994},
        {0.994164,0.990593,0.997573,0.994},
        {0.994084,0.990353,0.996585,0.994},
        {0.99247,0.989641,0.997431,0.994},
        {0.990978,0.991311,0.997521,0.994},
        {0.990444,0.98631,0.993942,0.994},
        {1.00385,1.01191,1.01922,0.994},
        {1.02798,0.955563,1.01648,0.994},
        {1,1,0.608799,0.994}};

     double Iso_eff[11][4] ={
        {0.958891,0.966043,0.983887,1.027909},
        {0.987445,0.992279,0.999508,1.027909},
        {1.00085,1.003619,1.005638,1.027909},
        {0.999167,1.002070,1.004848,1.027909},
        {0.998584,1.001419,1.002408,1.027909},
        {0.99839,0.999835,1.000268,1.027909},
        {0.99882,1.000145,1.000349,1.027909},
        {1.000879,1.000320,1.000559,1.027909},
        {1.000783,0.999194,1.00012,1.027909},
        {1.001326,0.999715,1.001746,1.027909},
        {1.010996,1,1,1.027909}};

//////SF Trig Eff
     double trig_eff[4][4]={
        {0.97,0.93,0.94,0.98},
        {0.96,0.94,0.91,0.95},
        {0.95,0.95,0.92,0.91},
        {0.96,0.95,0.95,0.96}};


/*      eff[0][0]=1.004; //old SF's
      eff[0][1]=1.003;
      eff[0][2]=1.007;
      eff[0][3]=1.007;
      eff[1][0]=0.975;
      eff[1][1]=0.984;
      eff[1][2]=0.992;
      eff[1][3]=0.995;
      eff[2][0]=1.034;
      eff[2][1]=1.006;
      eff[2][2]=0.991;
      eff[2][3]=0.993;
      eff[3][0]=0.983;
      eff[3][1]=0.990;
      eff[3][2]=1.006;
      eff[3][3]=1.007;
      eff[4][0]=1.025;
      eff[4][1]=1.022;
      eff[4][2]=1.013;
      eff[4][3]=1.009;*/



    double sm=0.;
    if(samesign==1)sm=1.;
    if(samesign==0)sm=-1.;
  
    double eff_elec1[50]={1};
    double eff_elec2[50]={1};
     int common = 0;
    int select_dielec=0;
    int index1[100],index2[100];
    //if(doUnf)cout<<"NEW EVENT"<<endl;
    if(!sherpa){
      if(muon==0 &&HLT_Elec17_Elec8==1){
        for (unsigned int j = 0; j < patElecPt_->size(); ++j){ 
          //if(patElecPt_->at(j) > 18. )cout<<"event infor and trig : "<<event<<"  "<<patElecTrig_->at(j)<<"  pt, eta; iso "<<patElecPt_->at(j)<<" "<<patElecEta_->at(j)<<" "<<patElecPfIsoRho_->at(j)<<" SC eta: "<<patElecScEta_->at(j)<<" MEDIUM ID: "<<patElecMediumIDOff_->at(j)<<endl;
          for (unsigned int jk = j; jk < patElecPt_->size(); ++jk){
	    if (j == jk) continue;
            if (select_dielec==1) continue;
	    if ( (patElecCharge_->at(j)*patElecCharge_->at(jk)) == sm && 
	      patElecPt_->at(j) > 20. && patElecPt_->at(jk) > 20. && 
	      fabs(patElecScEta_->at(j)) <=2.4 && fabs(patElecScEta_->at(jk)) <=2.4 &&
              (
               !(fabs(patElecScEta_->at(j)) >1.4442 && fabs(patElecScEta_->at(j))<1.566) && 
	       !(fabs(patElecScEta_->at(jk)) >1.4442 && fabs(patElecScEta_->at(jk))<1.566)
              ) &&
              //(patElecTrig_->at(j) > 0 && patElecTrig_->at(jk) > 0) &&
              patElecPfIsoRho_->at(j)<(-1*sm*0.150) && patElecPfIsoRho_->at(jk)<(-1*sm*0.150) &&
              patElecMediumIDOff_->at(j) >0 && patElecMediumIDOff_->at(jk) >0
	    ){
              select_dielec=1;
              if(fabs(patElecScEta_->at(j)) >1.4442 && fabs(patElecScEta_->at(j))<1.566)cout<<"Elektron gap dışlanmamış!!"<<endl;
              TLorentzVector e1_tmp;
              TLorentzVector e2_tmp;
              e1_tmp.SetPtEtaPhiE(patElecPt_->at(j), patElecEta_->at(j) ,patElecPhi_->at(j),patElecEnergy_->at(j) );
              e2_tmp.SetPtEtaPhiE(patElecPt_->at(jk), patElecEta_->at(jk) ,patElecPhi_->at(jk),patElecEnergy_->at(jk) );

              float MZElec_tmp= (e1_tmp+e2_tmp).M();
              //if(event==21348648)cout<<"AAAAAAAAAAAAA "<<MZElec_tmp<<endl;
              //if(MZElec_tmp<71. || MZElec_tmp>111.)continue;            

              if((doUnf &&data) || (!doUnf && !data)){
                for(int eta = 0; eta<5 ; eta++){
                  for(int pt = 0; pt<5 ; pt++){
                    if(fabs(patElecScEta_->at(j)) >etarange[eta] && fabs(patElecScEta_->at(j))<etarange[eta+1]){
                      if(patElecPt_->at(j) > ptrange[pt] &&patElecPt_->at(j) < ptrange[pt+1] &&patElecPt_->at(j)<1000. ){
                        if(data)eff_elec1[common] = 1/eff[eta][pt];
                        if(!data) eff_elec1[common] = eff[eta][pt];
                        //cout<<"Lepton1 "<<common<<" "<<patElecScEta_->at(j)<<" "<<patElecPt_->at(j)<<" "<<eff[eta][pt]<<" "<<eff_elec1[common]<<endl;
                      }
	    	      /*else{
		        eff_elec1[common]=1.;
                      }*/
                    }
                    if(fabs(patElecScEta_->at(jk)) >etarange[eta] && fabs(patElecScEta_->at(jk))<etarange[eta+1]){
                      if(patElecPt_->at(jk) > ptrange[pt] &&patElecPt_->at(jk) < ptrange[pt+1]&&patElecPt_->at(jk)<1000.){
                        if(data)eff_elec2[common] = 1/eff[eta][pt];
                        if(!data) eff_elec2[common] = eff[eta][pt];
       	       	        //cout<<"Lepton2 "<<common<<" "<<patElecScEta_->at(jk)<<" "<<patElecPt_->at(jk)<<" "<<eff[eta][pt]<<" "<<eff_elec2[common]<<endl;
                      }
		      /*else{
		        eff_elec2[common]=1.;
                      }*/
                    }
                  }
                }
              }
              if((!doUnf &&data) || (doUnf && !data)){
                eff_elec1[common] = 1.;
                eff_elec2[common] = 1.;
              }

	      index1[common] = j;
	      index2[common] = jk;
	      common++;
	    }
          }
        }
      }
      eff1 =eff_elec1[0];
      eff2 =eff_elec2[0];
    }

    if(muon==1){

      double ptcut = 20;

      for (unsigned int j = 0; j < patMuonPt_->size(); ++j){

          //if(patMuonPt_->at(j) > 18. )Yfan2<<" trig "<<patMuonTrig_->at(j)<<"  pt, eta; iso "<<patMuonPt_->at(j)<<" "<<patMuonEta_->at(j)<<" "<<patMuonPfIsoDbeta_->at(j)<<" "<< patMuonCombId_->at(j)<<endl;
        for (unsigned int jk = j; jk < patMuonPt_->size(); ++jk){
          if (j == jk) continue;
          if ((patMuonCharge_->at(j)*patMuonCharge_->at(jk)) == sm &&
                patMuonPt_->at(j) > ptcut && patMuonPt_->at(jk) > ptcut && 
                fabs(patMuonEta_->at(j)) <=2.4 && fabs(patMuonEta_->at(jk)) <=2.4 &&
                patMuonTrig_->at(j) ==1  && patMuonTrig_->at(jk) == 1 &&
                patMuonPfIsoDbeta_->at(j)<0.2 && patMuonPfIsoDbeta_->at(jk)<0.2 &&
                patMuonCombId_->at(j) ==1 && patMuonCombId_->at(jk) ==1
               )
          {

            index1[common] = j;
            index2[common] = jk;
            common++;

          }
        }
      }

      int ind1 = -99;
      int ind2 = -99;
      if(common>0){
      ind1 = index1[0];
      ind2 = index2[0];}
      if (ind1 != -99 && ind2 != -99){

///////////////////////Eff//////////////////////////
        for (int ib=0; ib<11; ib++) {
          for (int ie=0; ie<4; ie++) {
          //cout<<"IS HERE111 "<<ind1<<" "<<ind2<<endl;
            if (patMuonPt_->at(ind1)>etlow[ib] && patMuonPt_->at(ind1)<etlow[ib+1]) {
              if (fabs(patMuonEta_->at(ind1))>etalow[ie] && fabs(patMuonEta_->at(ind1))<etalow[ie+1]) {
                ID_Weight_0 = ID_eff[ib][ie];
                Iso_Weight_0 = Iso_eff[ib][ie];
              }
            }

            if (patMuonPt_->at(ind2)>etlow[ib] && patMuonPt_->at(ind2)<etlow[ib+1]) {
              if (fabs(patMuonEta_->at(ind2))>etalow[ie] && fabs(patMuonEta_->at(ind2))<etalow[ie+1]) {
                ID_Weight_1 = ID_eff[ib][ie];
                Iso_Weight_1 = Iso_eff[ib][ie];
              }
            }
          }
        }
        for (int ib=0; ib<4; ib++) {
          for (int ie=0; ie<4; ie++) {
            if (fabs(patMuonEta_->at(ind1))>etalow[ib] && fabs(patMuonEta_->at(ind1))<etalow[ib+1]) {
              if (fabs(patMuonEta_->at(ind2))>etalow[ie] && fabs(patMuonEta_->at(ind2))<etalow[ie+1]) {
                trig_Weight = trig_eff[ib][ie];
              }
            }
          }
        }

        if((doUnf &&data) || (!doUnf && !data)){
          if(!data)weight_tmp = ID_Weight_0*Iso_Weight_0*ID_Weight_1*Iso_Weight_1*trig_Weight;
          if(data)weight_tmp = 1/(ID_Weight_0*Iso_Weight_0*ID_Weight_1*Iso_Weight_1*trig_Weight);
        }
      }
    }



    float MZ = 0;
    float Zpt =0;
    float Zphi =0;
    float Zrapidity =0;
    int ind1 = -99;
    int ind2 = -99;

   
    ind1 = index1[0];
    ind2 = index2[0];

     // if(doUnf&&common>0)cout<<common<<"  "<<eff1<<" "<<eff2<<" "<<1/eff1<<"  "<<1/eff2<<endl; 
    double weight=0;
    TLorentzVector l1;
    TLorentzVector l2;
    double GMZ=0.;
    double GZrapidity=0.;
    double GZpt=0.;
    int Genind1=99;
    int Genind2=99;
    if(muon==0){
      GMZ= GMZElec;
      GZrapidity=Gdielecrapidity;
      GZpt=Gdielecpt;
      Genind1=Gind1;
      Genind2=Gind2;
    }
    if(muon==1){
      GMZ= GMZElec_mu;
      GZrapidity=Gdielecrapidity_mu;
      GZpt=Gdielecpt_mu;
      Genind1=Gind1_mu;
      Genind2=Gind2_mu;
    }
    if (ind1 != -99 && ind2 != -99 &&common>0  /*&&common==1*/){
 //     cout<<"EFFone "<<eff1<<"  "<<l1.Pt()<<"  "<<l1.Eta()<<endl;
 //     cout<<"EFFtwo "<<eff2<<"  "<<l2.Pt()<<"  "<<l2.Eta()<<endl;
      if(muon==0){
        l1.SetPtEtaPhiE(patElecPt_->at(ind1), patElecEta_->at(ind1) ,patElecPhi_->at(ind1),patElecEnergy_->at(ind1) );
        l2.SetPtEtaPhiE(patElecPt_->at(ind2), patElecEta_->at(ind2) ,patElecPhi_->at(ind2),patElecEnergy_->at(ind2) );
        weight= PUweight*(eff1*eff2);
        //cout<<weight<<" "<< PUweight<<endl;
      }

      if(muon==1){
        l1.SetPtEtaPhiE(patMuonPt_->at(ind1), patMuonEta_->at(ind1) ,patMuonPhi_->at(ind1),patMuonEn_->at(ind1) );
        l2.SetPtEtaPhiE(patMuonPt_->at(ind2), patMuonEta_->at(ind2) ,patMuonPhi_->at(ind2),patMuonEn_->at(ind2) );
        weight= PUweight*weight_tmp;
      }
      MZ= (l1+l2).M();
      if ( MZ > 20.){

        //if(doUnf&&common>0)cout<<"weight "<<weight<<endl;
        Zpt= (l1+l2).Perp();           
	Zrapidity = (l1+l2).Rapidity();
        Zphi = (l1+l2).Phi();
        double weight_Ztmp=1;
      //  cout<<weight_Ztmp<<" "<<weight<<endl;
        if(!data && ZPTweightcorr==1){
          for (int ptindex=0; ptindex<30;ptindex++){
            if(Zpt>double (20.0 * ptindex) && Zpt<= double (20.0*(ptindex+1)) &&Ratio_Z_PT_inc[ptindex]!=0.){
	      weight_Ztmp=1/Ratio_Z_PT_inc[ptindex];
	      break;
	    }
          }
          weight*=weight_Ztmp;
        }
        //cout<<weight_Ztmp<<" "<<weight<<endl<<endl<<endl;
//cout<<weight<<" "<< PUweight<<endl;
	if (MZ>=71. && MZ<111.){
        // Yfan<<event<<":"<<weight<<endl;  
         //cout<<l1.Pt()<<"  "<<l2.Pt()<<endl;
         h_lpt->Fill(l1.Pt(),weight);
         h_leta->Fill(l1.Eta(),weight);
         h_lphi->Fill(l1.Phi(),weight);
         h_slpt->Fill(l2.Pt(),weight);
         h_sleta->Fill(l2.Eta(),weight);
         h_slphi->Fill(l2.Phi(),weight);
          h_MZ->Fill(MZ,weight);
	  h_Zphi->Fill(Zphi,weight);
	  h_Zpt->Fill(Zpt,weight);
	  h_Zrapidity->Fill(Zrapidity,weight);
          if(Zrapidity>1.7 && Zrapidity<=2.0){
	    h_Zpt_p0->Fill(Zpt,weight);
	    h_Zphi_p0->Fill(Zphi,weight);

            h_lelec_pt_p0->Fill(l1.Pt(),weight);
            h_lelec_eta_p0->Fill(l1.Eta(),weight);
            h_lelec_phi_p0->Fill(l1.Phi(),weight);

            h_slelec_pt_p0->Fill(l2.Pt(),weight);
            h_slelec_eta_p0->Fill(l2.Eta(),weight);
            h_slelec_phi_p0->Fill(l2.Phi(),weight);

	    if(fabs(l1.Eta())>fabs(l2.Eta())){
              h_felec_pt_p0->Fill(l1.Pt(),weight);
              h_felec_eta_p0->Fill(l1.Eta(),weight);
              h_felec_phi_p0->Fill(l1.Phi(),weight);

              h_celec_pt_p0->Fill(l2.Pt(),weight);
              h_celec_eta_p0->Fill(l2.Eta(),weight);
              h_celec_phi_p0->Fill(l2.Phi(),weight);
            }
	    if(fabs(l1.Eta())<fabs(l2.Eta())){
              h_felec_pt_p0->Fill(l2.Pt(),weight);
              h_felec_eta_p0->Fill(l2.Eta(),weight);
              h_felec_phi_p0->Fill(l2.Phi(),weight);

              h_celec_pt_p0->Fill(l1.Pt(),weight);
              h_celec_eta_p0->Fill(l1.Eta(),weight);
              h_celec_phi_p0->Fill(l1.Phi(),weight);

            }
          } 

          if(Zrapidity>2.0 && Zrapidity<=2.2){
	    h_Zpt_p1->Fill(Zpt,weight);
	    h_Zphi_p1->Fill(Zphi,weight);

            h_lelec_pt_p1->Fill(l1.Pt(),weight);
            h_lelec_eta_p1->Fill(l1.Eta(),weight);
            h_lelec_phi_p1->Fill(l1.Phi(),weight);

            h_slelec_pt_p1->Fill(l2.Pt(),weight);
            h_slelec_eta_p1->Fill(l2.Eta(),weight);
            h_slelec_phi_p1->Fill(l2.Phi(),weight);

	    if(fabs(l1.Eta())>fabs(l2.Eta())){
              h_felec_pt_p1->Fill(l1.Pt(),weight);
              h_felec_eta_p1->Fill(l1.Eta(),weight);
              h_felec_phi_p1->Fill(l1.Phi(),weight);

              h_celec_pt_p1->Fill(l2.Pt(),weight);
              h_celec_eta_p1->Fill(l2.Eta(),weight);
              h_celec_phi_p1->Fill(l2.Phi(),weight);
            }
	    if(fabs(l1.Eta())<fabs(l2.Eta())){
              h_felec_pt_p1->Fill(l2.Pt(),weight);
              h_felec_eta_p1->Fill(l2.Eta(),weight);
              h_felec_phi_p1->Fill(l2.Phi(),weight);

              h_celec_pt_p1->Fill(l1.Pt(),weight);
              h_celec_eta_p1->Fill(l1.Eta(),weight);
              h_celec_phi_p1->Fill(l1.Phi(),weight);

            }
          } 

          if(Zrapidity>2.2 && Zrapidity<=2.5){
	    h_Zpt_p2->Fill(Zpt,weight);
	    h_Zphi_p2->Fill(Zphi,weight);

            h_lelec_pt_p2->Fill(l1.Pt(),weight);
            h_lelec_eta_p2->Fill(l1.Eta(),weight);
            h_lelec_phi_p2->Fill(l1.Phi(),weight);

            h_slelec_pt_p2->Fill(l2.Pt(),weight);
            h_slelec_eta_p2->Fill(l2.Eta(),weight);
            h_slelec_phi_p2->Fill(l2.Phi(),weight);

	    if(fabs(l1.Eta())>fabs(l2.Eta())){
              h_felec_pt_p2->Fill(l1.Pt(),weight);
              h_felec_eta_p2->Fill(l1.Eta(),weight);
              h_felec_phi_p2->Fill(l1.Phi(),weight);

              h_celec_pt_p2->Fill(l2.Pt(),weight);
              h_celec_eta_p2->Fill(l2.Eta(),weight);
              h_celec_phi_p2->Fill(l2.Phi(),weight);
            }
	    if(fabs(l1.Eta())<fabs(l2.Eta())){
              h_felec_pt_p2->Fill(l2.Pt(),weight);
              h_felec_eta_p2->Fill(l2.Eta(),weight);
              h_felec_phi_p2->Fill(l2.Phi(),weight);

              h_celec_pt_p2->Fill(l1.Pt(),weight);
              h_celec_eta_p2->Fill(l1.Eta(),weight);
              h_celec_phi_p2->Fill(l1.Phi(),weight);

            }
          } 

          if(Zrapidity<-1.7 && Zrapidity>=-2.0){
	    h_Zpt_m0->Fill(Zpt,weight);
	    h_Zphi_m0->Fill(Zphi,weight);

            h_lelec_pt_m0->Fill(l1.Pt(),weight);
            h_lelec_eta_m0->Fill(l1.Eta(),weight);
            h_lelec_phi_m0->Fill(l1.Phi(),weight);

            h_slelec_pt_m0->Fill(l2.Pt(),weight);
            h_slelec_eta_m0->Fill(l2.Eta(),weight);
            h_slelec_phi_m0->Fill(l2.Phi(),weight);

	    if(fabs(l1.Eta())>fabs(l2.Eta())){
              h_felec_pt_m0->Fill(l1.Pt(),weight);
              h_felec_eta_m0->Fill(l1.Eta(),weight);
              h_felec_phi_m0->Fill(l1.Phi(),weight);

              h_celec_pt_m0->Fill(l2.Pt(),weight);
              h_celec_eta_m0->Fill(l2.Eta(),weight);
              h_celec_phi_m0->Fill(l2.Phi(),weight);
            }
	    if(fabs(l1.Eta())<fabs(l2.Eta())){
              h_felec_pt_m0->Fill(l2.Pt(),weight);
              h_felec_eta_m0->Fill(l2.Eta(),weight);
              h_felec_phi_m0->Fill(l2.Phi(),weight);

              h_celec_pt_m0->Fill(l1.Pt(),weight);
              h_celec_eta_m0->Fill(l1.Eta(),weight);
              h_celec_phi_m0->Fill(l1.Phi(),weight);

            }
          } 

          if(Zrapidity<-2.0 && Zrapidity>=-2.2){
	    h_Zpt_m1->Fill(Zpt,weight);
	    h_Zphi_m1->Fill(Zphi,weight);

            h_lelec_pt_m1->Fill(l1.Pt(),weight);
            h_lelec_eta_m1->Fill(l1.Eta(),weight);
            h_lelec_phi_m1->Fill(l1.Phi(),weight);

            h_slelec_pt_m1->Fill(l2.Pt(),weight);
            h_slelec_eta_m1->Fill(l2.Eta(),weight);
            h_slelec_phi_m1->Fill(l2.Phi(),weight);

	    if(fabs(l1.Eta())>fabs(l2.Eta())){
              h_felec_pt_m1->Fill(l1.Pt(),weight);
              h_felec_eta_m1->Fill(l1.Eta(),weight);
              h_felec_phi_m1->Fill(l1.Phi(),weight);

              h_celec_pt_m1->Fill(l2.Pt(),weight);
              h_celec_eta_m1->Fill(l2.Eta(),weight);
              h_celec_phi_m1->Fill(l2.Phi(),weight);

            }

	    if(fabs(l1.Eta())<fabs(l2.Eta())){
              h_felec_pt_m1->Fill(l2.Pt(),weight);
              h_felec_eta_m1->Fill(l2.Eta(),weight);
              h_felec_phi_m1->Fill(l2.Phi(),weight);

              h_celec_pt_m1->Fill(l1.Pt(),weight);
              h_celec_eta_m1->Fill(l1.Eta(),weight);
              h_celec_phi_m1->Fill(l1.Phi(),weight);

            }
          } 

          if(Zrapidity<-2.2 && Zrapidity>=-2.5){
	    h_Zpt_m2->Fill(Zpt,weight);
	    h_Zphi_m2->Fill(Zphi,weight);

            h_lelec_pt_m2->Fill(l1.Pt(),weight);
            h_lelec_eta_m2->Fill(l1.Eta(),weight);
            h_lelec_phi_m2->Fill(l1.Phi(),weight);

            h_slelec_pt_m2->Fill(l2.Pt(),weight);
            h_slelec_eta_m2->Fill(l2.Eta(),weight);
            h_slelec_phi_m2->Fill(l2.Phi(),weight);

	    if(fabs(l1.Eta())>fabs(l2.Eta())){
              h_felec_pt_m2->Fill(l1.Pt(),weight);
              h_felec_eta_m2->Fill(l1.Eta(),weight);
              h_felec_phi_m2->Fill(l1.Phi(),weight);

              h_celec_pt_m2->Fill(l2.Pt(),weight);
              h_celec_eta_m2->Fill(l2.Eta(),weight);
              h_celec_phi_m2->Fill(l2.Phi(),weight);

            }

	    if(fabs(l1.Eta())<fabs(l2.Eta())){
              h_felec_pt_m2->Fill(l2.Pt(),weight);
              h_felec_eta_m2->Fill(l2.Eta(),weight);
              h_felec_phi_m2->Fill(l2.Phi(),weight);

              h_celec_pt_m2->Fill(l1.Pt(),weight);
              h_celec_eta_m2->Fill(l1.Eta(),weight);
              h_celec_phi_m2->Fill(l1.Phi(),weight);

            }
          } 

//l1.Pt(), l1.Eta() ,l1.Phi(),patElecEnergy_->at(ind1) 
         if(Zrapidity<1.7 &&Zrapidity>=0.0 ){
	    h_Zpt_pc->Fill(Zpt,weight);
	    h_Zphi_pc->Fill(Zphi,weight);

            h_lelec_pt_pc->Fill(l1.Pt(),weight);
            h_lelec_eta_pc->Fill(l1.Eta(),weight);
            h_lelec_phi_pc->Fill(l1.Phi(),weight);

            h_slelec_pt_pc->Fill(l2.Pt(),weight);
            h_slelec_eta_pc->Fill(l2.Eta(),weight);
            h_slelec_phi_pc->Fill(l2.Phi(),weight);

	    if(fabs(l1.Eta())>fabs(l2.Eta())){
              h_felec_pt_pc->Fill(l1.Pt(),weight);
              h_felec_eta_pc->Fill(l1.Eta(),weight);
              h_felec_phi_pc->Fill(l1.Phi(),weight);

              h_celec_pt_pc->Fill(l2.Pt(),weight);
              h_celec_eta_pc->Fill(l2.Eta(),weight);
              h_celec_phi_pc->Fill(l2.Phi(),weight);
            }
	    if(fabs(l1.Eta())<fabs(l2.Eta())){
              h_felec_pt_pc->Fill(l2.Pt(),weight);
              h_felec_eta_pc->Fill(l2.Eta(),weight);
              h_felec_phi_pc->Fill(l2.Phi(),weight);

              h_celec_pt_pc->Fill(l1.Pt(),weight);
              h_celec_eta_pc->Fill(l1.Eta(),weight);
              h_celec_phi_pc->Fill(l1.Phi(),weight);
            }
          } 

         if(Zrapidity>-1.7 &&Zrapidity<=0.0 ){
	    h_Zpt_mc->Fill(Zpt,weight);
	    h_Zphi_mc->Fill(Zphi,weight);

            h_lelec_pt_mc->Fill(l1.Pt(),weight);
            h_lelec_eta_mc->Fill(l1.Eta(),weight);
            h_lelec_phi_mc->Fill(l1.Phi(),weight);

            h_slelec_pt_mc->Fill(l2.Pt(),weight);
            h_slelec_eta_mc->Fill(l2.Eta(),weight);
            h_slelec_phi_mc->Fill(l2.Phi(),weight);

	    if(fabs(l1.Eta())>fabs(l2.Eta())){
              h_felec_pt_mc->Fill(l1.Pt(),weight);
              h_felec_eta_mc->Fill(l1.Eta(),weight);
              h_felec_phi_mc->Fill(l1.Phi(),weight);

              h_celec_pt_mc->Fill(l2.Pt(),weight);
              h_celec_eta_mc->Fill(l2.Eta(),weight);
              h_celec_phi_mc->Fill(l2.Phi(),weight);
            }
	    if(fabs(l1.Eta())<fabs(l2.Eta())){
              h_felec_pt_mc->Fill(l2.Pt(),weight);
              h_felec_eta_mc->Fill(l2.Eta(),weight);
              h_felec_phi_mc->Fill(l2.Phi(),weight);

              h_celec_pt_mc->Fill(l1.Pt(),weight);
              h_celec_eta_mc->Fill(l1.Eta(),weight);
              h_celec_phi_mc->Fill(l1.Phi(),weight);

            }
          } 
	}
      }
    }

    //if(!data)weight= PUweight;
    //if(weight!=PUweight)cout<<PUweight<<"  "<<weight<<endl;		  
    TLorentzVector j1;
    TLorentzVector j2;
    TLorentzVector j1G;
    TLorentzVector j2G;
    TLorentzVector j3G;
    TLorentzVector j4G;
    TLorentzVector j5G;
    TLorentzVector j6G;

    TLorentzVector j1_BEF;
    TLorentzVector j2_BEF;
    TLorentzVector j3_BEF;
    TLorentzVector j4_BEF;
    TLorentzVector j1_BE;
    TLorentzVector j2_BE;
    TLorentzVector j3_BE;
    TLorentzVector j4_BE;
    TLorentzVector j1G_BEF;
    TLorentzVector j2G_BEF;
    TLorentzVector j1G_BE;
    TLorentzVector j2G_BE;



    std::vector<double> jPt;
    std::vector<double> jEta;
    std::vector<double> jPhi;
    std::vector<double> jE;
    std::vector<double> jY;

    std::vector<double> jPt_BEF;
    std::vector<double> jEta_BEF;
    std::vector<double> jPhi_BEF;
    std::vector<double> jE_BEF;
    std::vector<double> jY_BEF;

   /////////////Gen Jets///////////////////////

    int Gn_jet_30_n_bef=0;
    int Gn_jet_30_n=0;
    int Gn_jet_30_n_30=0;

    if (GMZ>=71 && GMZ<111 ){

 




      Gen=1;/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
      for (unsigned int pf=0;pf < GjPt->size();pf++){
	bool markpfjet = false;

        float deltar1 = DeltaR(Gjeta->at(pf),Dr01LepEta->at(Genind1),Gjphi->at(pf),Dr01LepPhi->at(Genind1));
        float deltar2 = DeltaR(Gjeta->at(pf),Dr01LepEta->at(Genind2),Gjphi->at(pf),Dr01LepPhi->at(Genind2));

        if (deltar1<0.5) markpfjet = true;
        if (deltar2<0.5) markpfjet = true;
        
        if (markpfjet) continue;

		/////// Gen Jet JpT 30///////////////////
	if (GjPt->at(pf) > Gthresh && fabs(Gjeta->at(pf))<4.7){             
          Gn_jet_30_n_bef++;	  
	  if(Gn_jet_30_n_bef==1){
	    j1G_BEF.SetPtEtaPhiE(GjPt->at(pf),Gjeta->at(pf),Gjphi->at(pf),GjE->at(pf));
          }
          if(Gn_jet_30_n_bef==2){
	    j2G_BEF.SetPtEtaPhiE(GjPt->at(pf),Gjeta->at(pf),Gjphi->at(pf),GjE->at(pf));
          }
        }

	if (GjPt->at(pf) > Gthresh && fabs(Gjeta->at(pf))<2.4){             
        if (GjPt->at(pf) > 30.)  Gn_jet_30_n_30++;
	  Gn_jet_30_n++;	 	  
	  if(Gn_jet_30_n==1){
	    j1G_BE.SetPtEtaPhiE(GjPt->at(pf),Gjeta->at(pf),Gjphi->at(pf),GjE->at(pf));
          }
          if(Gn_jet_30_n==2){
	    j2G_BE.SetPtEtaPhiE(GjPt->at(pf),Gjeta->at(pf),Gjphi->at(pf),GjE->at(pf));
          }
	  if(Gn_jet_30_n==3){
	    j3G.SetPtEtaPhiE(GjPt->at(pf),Gjeta->at(pf),Gjphi->at(pf),GjE->at(pf));
          }
          if(Gn_jet_30_n==4){
	    j4G.SetPtEtaPhiE(GjPt->at(pf),Gjeta->at(pf),Gjphi->at(pf),GjE->at(pf));
          }
	  if(Gn_jet_30_n==5){
	    j5G.SetPtEtaPhiE(GjPt->at(pf),Gjeta->at(pf),Gjphi->at(pf),GjE->at(pf));
          }
          if(Gn_jet_30_n==6){
	    j6G.SetPtEtaPhiE(GjPt->at(pf),Gjeta->at(pf),Gjphi->at(pf),GjE->at(pf));
          }
        }
      }/// end Gen jet loop
      //cout << event << "  genZmass : " << GMZ << "  genZpt : " << GZpt << " genNJets : " << Gn_jet_30_n_30 << endl;

    }
    /////////////Jets///////////////////////

    int n_jet_30_n_bef=0;
    int n_jet_30_n=0;
    int n_jet_30_n_30=0;

    if (MZ>=71. && MZ<111. ){


     // 

      Reco=1;/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
      h_nvtx->Fill(EvtInfo_NumVtx,weight);
      h_nvtx_noWeight->Fill(EvtInfo_NumVtx);
      for (unsigned int pf=0;pf < patJetPfAk05Pt_->size();pf++){
        int mgind=0;
     //   if(fabs(patJetPfAk05Eta_->at(pf))<3.2 &&fabs(patJetPfAk05Eta_->at(pf))>3.0)continue;

          bool markpfjet = false;
//REMOVE only resonance leptons...
/*
          for (unsigned int j = 0; j < patElecPt_->size(); ++j){  
            if ( patElecPt_->at(j) > 20. &&
	      fabs(patElecEta_->at(j)) <2.4 && 
              (
               !(fabs(patElecScEta_->at(j)) >1.4442 && fabs(patElecScEta_->at(j))<1.566) 
              ) &&
              //patElecTrig_->at(j) > 0 &&
              patElecMediumIDOff_->at(j) >0 
	      ){
 	       double deltar = DeltaR(patJetPfAk05Eta_->at(pf),patElecEta_->at(j),patJetPfAk05Phi_->at(pf),patElecPhi_->at(j));
               if (deltar<0.5) markpfjet = true;
            }
          }

          for (unsigned int j = 0; j < patMuonPt_->size(); ++j){  

           if( patMuonPt_->at(j) > 20. && 
	    fabs(patMuonEta_->at(j)) <2.4  &&
            patMuonTrig_->at(j) ==1   &&
            patMuonPfIsoDbeta_->at(j)<0.2  &&
	    patMuonCombId_->at(j) ==1){
              double deltar = DeltaR(patJetPfAk05Eta_->at(pf),patMuonEta_->at(j),patJetPfAk05Phi_->at(pf),patMuonPhi_->at(j));
              if(deltar<0.5) markpfjet = true;
            }
          }
*/
          double deltar1 = DeltaR(patJetPfAk05Eta_->at(pf),l1.Eta(),patJetPfAk05Phi_->at(pf),l1.Phi());
          double deltar2 = DeltaR(patJetPfAk05Eta_->at(pf),l2.Eta(),patJetPfAk05Phi_->at(pf),l2.Phi());
          h_deltar->Fill(deltar1);
          h_deltar->Fill(deltar2);
          //cout<<"JET LOOP: dr1 dr2 jetpt jeteta jetphi PUcut jetcut  "<<deltar1<<" "<<deltar2<<" "<<patJetPfAk05Pt_->at(pf)<<" "<<patJetPfAk05Eta_->at(pf)<<" "<<patJetPfAk05Phi_->at(pf)<<" "<<patJetPfAk05jetpuMVA_->at(pf)<<" "<<patJetPfAk05jetpukLoose_->at(pf)<<"   "<<patJetPfAk05LooseId_->at(pf)<<" "<<endl;
          if(deltar1<0.5 || deltar2<0.5) markpfjet = true;
          if (markpfjet) continue;

          //jecUnc->setJetEta(patJetPfAk05Eta_->at(pf));
          //jecUnc->setJetPt(patJetPfAk05Pt_->at(pf));


          double tempJetID=0;
          double chf= patJetPfAk05chf_->at(pf);
          double nhf= (patJetPfAk05nhf_->at(pf) * patJetPfAk05En_->at(pf))/patJetPfAk05RawEn_->at(pf);// buggy in data & BG
          if (mc) nhf= patJetPfAk05nhf_->at(pf);
        if(muon==1 &&run>=193834 && run<=196531){/*cout<<"BURDA Aq"<<endl;*/nhf= patJetPfAk05nhf_->at(pf);}
          //if(muon==1){/*cout<<"HEREEEEE"<<endl;*/nhf= patJetPfAk05nhf_->at(pf);}
          double cmult=patJetPfAk05cmult_->at(pf);
          double cemf= patJetPfAk05cemf_->at(pf);
          double nemf = patJetPfAk05nemf_->at(pf);
          double nconst = patJetPfAk05nconst_->at(pf);
//          if(chf+nhf+cemf+nemf<0.9)cout<<chf<<" "<<nhf<<" "<<cemf<<" "<<nemf<<" "<<patJetPfAk05Pt_->at(pf)<<" "<<patJetPfAk05Eta_->at(pf)<<endl;
//          cout<<patJetPfAk05nhf_->at(pf)<<" "<<nhf<<endl;

          //if(chf+nhf+cemf+nemf<0.9)cout<<chf+nhf+cemf+nemf<<"  "<<chf+patJetPfAk05nhf_->at(pf)+cemf+nemf<<endl;
          if( abs(patJetPfAk05Eta_->at(pf))<2.4){
            if((chf>0)&&(nhf<0.99)&&(cmult>0.0)&&(cemf<0.99)&&(nemf<0.99)&&(nconst>1)) tempJetID=1;
            if((chf>0)&&(nhf<0.95)&&(cmult>0.0)&&(cemf<0.99)&&(nemf<0.95)&&(nconst>1)) tempJetID=2;
            if((chf>0)&&(nhf<0.90)&&(cmult>0.0)&&(cemf<0.99)&&(nemf<0.90)&&(nconst>1)) tempJetID=3;
          }
          if( abs(patJetPfAk05Eta_->at(pf))>=2.4){
            if ((nhf<0.99)&&(nemf<0.99)&&(nconst>1))tempJetID=1;
            if ((nhf<0.95)&&(nemf<0.95)&&(nconst>1))tempJetID=2;
            if ((nhf<0.9)&&(nemf<0.9)&&(nconst>1))tempJetID=3;
          }
  


//Gen Jet Energy Resolution correction
         double eta_range[]= {0.0,0.5,1.1,1.7,2.3,5.0};
         double c[]={1.052,1.057,1.096,1.134,1.288};
         double cdown[]={0.990,1.001,1.032,1.042,1.089};
         double cup[]={1.115,1.114,1.161,1.228,1.488};
	 double new_pt=patJetPfAk05Pt_->at(pf);
	 double new_energy=patJetPfAk05En_->at(pf);
         
         if(new_pt>15. &&scale!=0){
           double jetEnergyCorr = TableJESunc.getEfficiency(patJetPfAk05Pt_->at(pf),patJetPfAk05Eta_->at(pf) ) ;
           //cout<<patJetPfAk05Pt_->at(pf)<<"  "<<patJetPfAk05Eta_->at(pf)<<"  "<< jetEnergyCorr<<endl;
           // cout<<patJetPfAk05Pt_->at(pf)/patJetPfAk05RawPt_->at(pf)<<"  "<<patJetPfAk05En_->at(pf)/patJetPfAk05RawEn_->at(pf)<<endl;
           new_pt *= (1. + scale* jetEnergyCorr);
           new_energy *= (1. + scale* jetEnergyCorr);
         }

         double deltar_min=0.5;
         int gjindex=-1;
	 if(new_pt>15. &&tempJetID>=1&&patJetPfAk05jetpukLoose_->at(pf)==1&&JERcorr==1){

           for (unsigned int pfg=0;pfg < GjPt->size();pfg++){
             if(GjPt->at(pfg) <10.)continue;
             float deltara=99.;
             float deltarb=99.;
             //cout<<Genind1<<" "<<Genind2<<endl;
             if (Genind1>=0 &&Genind2>=0){
              deltara = DeltaR(Gjeta->at(pfg),Dr01LepEta->at(Genind1),Gjphi->at(pfg),Dr01LepPhi->at(Genind1));
              deltarb = DeltaR(Gjeta->at(pfg),Dr01LepEta->at(Genind2),Gjphi->at(pfg),Dr01LepPhi->at(Genind2));}

             if (deltara<0.5) markpfjet = true;
             if (deltarb<0.5) markpfjet = true;
             float deltar1 = DeltaR(patJetPfAk05Eta_->at(pf),Gjeta->at(pfg),patJetPfAk05Phi_->at(pf),Gjphi->at(pfg));
             h_deltar_reco_gen_jet->Fill(deltar1,weight);
             if(deltar1<0.15 && deltar1<deltar_min){
               deltar_min=deltar1;

               for(int eta_ind=0; eta_ind<5;eta_ind++){
                 if(fabs(patJetPfAk05Eta_->at(pf))>eta_range[eta_ind] && fabs(patJetPfAk05Eta_->at(pf))<eta_range[eta_ind +1] ){
                    
                   if(JERsys==0)new_pt= std::max(0.0,GjPt->at(pfg) + c[eta_ind] *( new_pt-GjPt->at(pfg) ) );
                   if(JERsys==1)new_pt= std::max(0.0,GjPt->at(pfg) + cup[eta_ind] *( new_pt-GjPt->at(pfg) ) );
                   if(JERsys==-1)new_pt= std::max(0.0,GjPt->at(pfg)+ cdown[eta_ind] *( new_pt-GjPt->at(pfg) ) );
		   new_energy *= (new_pt/patJetPfAk05Pt_->at(pf));
                   gjindex=pfg;
	 	   //h_dE_gen_raw->Fill((patJetPfAk05RawPt_->at(pf)-GjPt->at(pfg))/GjPt->at(pfg),weight);

                 }
               }
             }
           }
           if(gjindex>-1){
             //cout<<patJetPfAk05Eta_->at(pf)<<" DELTAR "<<deltar_min<<" GJPT RAWPT CORRPT RESCORRPT "<<GjPt->at(gjindex)<<"  "<<patJetPfAk05RawPt_->at(pf)<<" "<<patJetPfAk05Pt_->at(pf)<<" "<<new_pt<<endl;
             float deltar11 = DeltaR(patJetPfAk05Eta_->at(pf),Gjeta->at(gjindex),patJetPfAk05Phi_->at(pf),Gjphi->at(gjindex));
             h_2dgenjet_recocorrjet->Fill(GjPt->at(gjindex),patJetPfAk05Pt_->at(pf),weight);
             h_2dgenjet_recorawjet->Fill(GjPt->at(gjindex),patJetPfAk05RawPt_->at(pf),weight);
             h_deltar_reco_gen_jet_matched->Fill(deltar11,weight);
             h_ptj_ratio_before->Fill(patJetPfAk05Eta_->at(pf),(patJetPfAk05Pt_->at(pf)/GjPt->at(gjindex)),weight);
             h_ptj_ratio2_before->Fill(GjPt->at(gjindex),(patJetPfAk05Pt_->at(pf)/GjPt->at(gjindex)),weight);
             h_ptj_ratio2Raw_before->Fill(GjPt->at(gjindex),(patJetPfAk05RawPt_->at(pf)/GjPt->at(gjindex)),weight);
             h_ptj_ratioRaw_before->Fill(patJetPfAk05Eta_->at(pf),(patJetPfAk05RawPt_->at(pf)/GjPt->at(gjindex)),weight);
             h_ptj_ratio_after->Fill(patJetPfAk05Eta_->at(pf),(new_pt/GjPt->at(gjindex)),weight);
             h_ptj_ratio2_after->Fill(GjPt->at(gjindex),(new_pt/GjPt->at(gjindex)),weight);
             h_ptj_ratioRaw_after->Fill(patJetPfAk05Eta_->at(pf),(new_pt/GjPt->at(gjindex)),weight);
             if(patJetPfAk05Pt_->at(pf)>50.&& GjPt->at(gjindex) >50.){
               h_ptj_ratio_before_50->Fill(patJetPfAk05Eta_->at(pf),(patJetPfAk05Pt_->at(pf)/GjPt->at(gjindex)),weight);
               h_ptj_ratioRaw_before_50->Fill(patJetPfAk05Eta_->at(pf),(patJetPfAk05RawPt_->at(pf)/GjPt->at(gjindex)),weight);
               h_ptj_ratio2_before_50->Fill(GjPt->at(gjindex),(patJetPfAk05Pt_->at(pf)/GjPt->at(gjindex)),weight);
               h_ptj_ratio_after_50->Fill(patJetPfAk05Eta_->at(pf),(new_pt/GjPt->at(gjindex)),weight);
               h_ptj_ratioRaw_after_50->Fill(patJetPfAk05Eta_->at(pf),(new_pt/GjPt->at(gjindex)),weight);
               h_ptj_ratio2_after_50->Fill(GjPt->at(gjindex),(new_pt/GjPt->at(gjindex)),weight);
               }
               if(patJetPfAk05Pt_->at(pf)<70.&& GjPt->at(gjindex) <70.){
                 h_ptj_ratio_before_50_70->Fill(patJetPfAk05Eta_->at(pf),(patJetPfAk05Pt_->at(pf)/GjPt->at(gjindex)),weight);
                 h_ptj_ratioRaw_before_50_70->Fill(patJetPfAk05Eta_->at(pf),(patJetPfAk05RawPt_->at(pf)/GjPt->at(gjindex)),weight);
               }
             if(patJetPfAk05Pt_->at(pf)>100.&& GjPt->at(gjindex) >100.){
               h_ptj_ratio_before_100->Fill(patJetPfAk05Eta_->at(pf),(patJetPfAk05Pt_->at(pf)/GjPt->at(gjindex)),weight);
               h_ptj_ratioRaw_before_100->Fill(patJetPfAk05Eta_->at(pf),(patJetPfAk05RawPt_->at(pf)/GjPt->at(gjindex)),weight);
               h_ptj_ratio2_before_100->Fill(GjPt->at(gjindex),(patJetPfAk05Pt_->at(pf)/GjPt->at(gjindex)),weight);
               h_ptj_ratio_after_100->Fill(patJetPfAk05Eta_->at(pf),(new_pt/GjPt->at(gjindex)),weight);
               h_ptj_ratio2_after_100->Fill(GjPt->at(gjindex),(new_pt/GjPt->at(gjindex)),weight);
               h_ptj_ratioRaw_after_100->Fill(patJetPfAk05Eta_->at(pf),(new_pt/GjPt->at(gjindex)),weight);
             }

	     h_dE_gen_corr->Fill((patJetPfAk05Pt_->at(pf)-GjPt->at(gjindex))/GjPt->at(gjindex),weight);
	     h_dE_gen_corr_rescorr->Fill((new_pt-GjPt->at(gjindex))/GjPt->at(gjindex),weight); 
           }
         }

 
       
          if(new_pt > Gthresh&&0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<4.7  
             ){  
//cout<<"AAAAAA "<<chf+nhf+cemf+nemf<<"  "<<chf<<" "<<nhf<<" "<<cemf<<" "<<nemf<<" "<<patJetPfAk05Pt_->at(pf)<<" "<<patJetPfAk05Eta_->at(pf)<<endl;         
            if ((tight!=1&& tempJetID>=1 && patJetPfAk05jetpukLoose_->at(pf)==1) ||  (tight==1&& tempJetID>=3 && patJetPfAk05jetpukTight_->at(pf)==1)){
  	      //cout<<chf+nhf+cemf+nemf<<"  "<<chf+patJetPfAk05nhf_->at(pf)+cemf+nemf<<endl;
              n_jet_30_n_bef++;

//            Reco_BEF=1;/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	      TLorentzVector j;
              j.SetPtEtaPhiE(new_pt,patJetPfAk05Eta_->at(pf),patJetPfAk05Phi_->at(pf),new_energy);
           //   cout<<j.Rapidity()<<"  "<<j.Eta()<<endl;
    	      if(n_jet_30_n_bef==1){
	        j1_BEF.SetPtEtaPhiE(j.Pt(),j.Eta(),j.Phi(),j.E());          
              }
              if(n_jet_30_n_bef==2){
	        j2_BEF.SetPtEtaPhiE(j.Pt(),j.Eta(),j.Phi(),j.E());
              }
              if(n_jet_30_n_bef==3){
	        j3_BEF.SetPtEtaPhiE(j.Pt(),j.Eta(),j.Phi(),j.E());
              }
              if(n_jet_30_n_bef==4){
	        j4_BEF.SetPtEtaPhiE(j.Pt(),j.Eta(),j.Phi(),j.E());
              }
            }  
          }


          if(new_pt > Gthresh&&0.0<fabs(patJetPfAk05Eta_->at(pf)) && fabs(patJetPfAk05Eta_->at(pf))<2.4&&
            ((tight!=1&& tempJetID>=1 && patJetPfAk05jetpukLoose_->at(pf)==1) ||  (tight==1&& tempJetID>=3 && patJetPfAk05jetpukTight_->at(pf)==1)) 
            ){
          //cout<<"AAAAAAA "<<chf+nhf+cemf+nemf<<endl;
	      if(new_pt > 30.)n_jet_30_n_30++;
	      n_jet_30_n++;
//            Reco_BEF=1;/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	    TLorentzVector j;
            j.SetPtEtaPhiE(new_pt,patJetPfAk05Eta_->at(pf),patJetPfAk05Phi_->at(pf),new_energy);
    	    if(n_jet_30_n==1){
	      j1_BE.SetPtEtaPhiE(j.Pt(),j.Eta(),j.Phi(),j.E());   
            }
            if(n_jet_30_n==2){
	      j2_BE.SetPtEtaPhiE(j.Pt(),j.Eta(),j.Phi(),j.E());
            }
            if(n_jet_30_n==3){
	      j3_BE.SetPtEtaPhiE(j.Pt(),j.Eta(),j.Phi(),j.E());
            }
            if(n_jet_30_n==4){
	      j4_BE.SetPtEtaPhiE(j.Pt(),j.Eta(),j.Phi(),j.E());
            }
          }  
      }

         //if ( event == 2626263 || event == 42432965  ){
	//  if (PRINTEVENT){
//                vector<jetStruct> tmpJets;
//                for (unsigned short i(0); i < nGoodJets; i++){
//                    if (jets[i].pt >= jetPtCutMin) tmpJets.push_back(jets[i]);
//                }
 //               unsigned short tempnGoodJets(tmpJets.size());
                NZtotal++;
               //cout << "event info: " << run << "  " << event << endl;
                //cout <<event<< "  Zmass : " << MZ << "  Zpt : " << Zpt << " NJets : " << n_jet_30_n_30   << endl;
/*                if (n_jet_30_n_30 > 0) cout << "JETS:"<< endl;
 //               for (unsigned short i(0); i < tempnGoodJets; i++)

if (n_jet_30_n_30>0) cout << " jet #" <<  1 << "  pt: " << j1_BE.Pt() << "  eta:"<<j1_BE.Eta() << "   " << endl;
if (n_jet_30_n_30>1) cout << " jet #" <<  2 << "  pt: " << j2_BE.Pt() << "  eta:"<<j2_BE.Eta() << "   " << endl;
if (n_jet_30_n_30>2) cout << " jet #" <<  3 << "  pt: " << j3_BE.Pt() << "  eta:"<<j3_BE.Eta() << "   " << endl;
if (n_jet_30_n_30>3) cout << " jet #" <<  4 << "  pt: " << j4_BE.Pt() << "  eta:"<<j4_BE.Eta() << "   " << endl;*/
                  //  cout << " jet #" << i + 1 << "  pt: " << tmpJets[i].pt << "  eta:"<<tmpJets[i].eta << "   " << endl;
                //cout << "-----------------------------------------------------------------------------------------"<< endl;
           // }




    }//END MASS WINDOW//

/*    if(n_jet_30_n_bef>0){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1_BEF.Rapidity())>j_y_range[i] && fabs(j1_BEF.Rapidity())<j_y_range[i+1]){
            //cout<<"test"<<endl;
            h_inc_ljet_pt_y[i]->Fill(j1_BEF.Pt(),weight);
	  }
        }
 
    }        

    if(Gn_jet_30_n_bef>0){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1G_BEF.Rapidity())>j_y_range[i] && fabs(j1G_BEF.Rapidity())<j_y_range[i+1]){
            
            h_inc_ljet_pt_y_Gen[i]->Fill(j1G_BEF.Pt(),weight);
          }
        }
    }        

    if(Gn_jet_30_n_bef>0 && n_jet_30_n_bef>0){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1G_BEF.Rapidity())>j_y_range[i] && fabs(j1G_BEF.Rapidity())<j_y_range[i+1]
           &&fabs( j1_BEF.Rapidity())>j_y_range[i] && fabs( j1_BEF.Rapidity())<j_y_range[i+1]){

            h_inc_ljet_pt_y_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);
          }
        }
    }*/
 
    // 3D Z plots for Z+1jet exclusive//

 //   double Z_y_range[5]={0,0.9,1.5,2.1,2.5};

 //   double threed_j_y_range[4]={0,1.5,2.4,4.7};

 //   double pt_range[6]={20.,30.,50.,100.,200.,1000.};

    if(n_jet_30_n_bef==1){

      for(int j=0; j<3;j++){
        for(int i = 0; i<5;i++){
          if(fabs(j1_BEF.Rapidity())>threed_j_y_range[j] && fabs(j1_BEF.Rapidity())<threed_j_y_range[j+1]){

            if(fabs(Zrapidity)>Z_y_range[i] && fabs(Zrapidity)<Z_y_range[i+1]){

              h_z_pt_y[i][j]->Fill(Zpt,weight);
            }
            if(Zpt>pt_range[i] && Zpt<pt_range[i+1]){
              h_z_y_pt[i][j]->Fill(fabs(Zrapidity),weight);
            }
          }
        }
      }
    }

    if(Gn_jet_30_n_bef==1){
      for(int j=0; j<3;j++){
        for(int i = 0; i<5;i++){

          if(fabs(j1G_BEF.Rapidity())>threed_j_y_range[j] && fabs(j1G_BEF.Rapidity())<threed_j_y_range[j+1]){

            if(fabs(GZrapidity)>Z_y_range[i] && fabs(GZrapidity)<Z_y_range[i+1]){

              h_z_pt_y_Gen[i][j]->Fill(GZpt,PUweight);
            }
            if(GZpt>pt_range[i] && GZpt<pt_range[i+1]){
              h_z_y_pt_Gen[i][j]->Fill(fabs(GZrapidity),PUweight);
            }
          }
        }
      }
    }


    if(n_jet_30_n_bef==1 && Gn_jet_30_n_bef==1){
      for(int j=0; j<3;j++){
        for(int i = 0; i<5;i++){
          if(fabs(j1_BEF.Rapidity())>threed_j_y_range[j] && fabs(j1_BEF.Rapidity())<threed_j_y_range[j+1]
           &&fabs(j1G_BEF.Rapidity())>threed_j_y_range[j] && fabs(j1G_BEF.Rapidity())<threed_j_y_range[j+1]){

            if(fabs(Zrapidity)>Z_y_range[i] && fabs(Zrapidity)<Z_y_range[i+1]
             &&fabs(GZrapidity)>Z_y_range[i] && fabs(GZrapidity)<Z_y_range[i+1]){
              h_z_pt_y_Gen_VS_reco[i][j]->Fill(Zpt,GZpt,weight);
            }
            if(Zpt>pt_range[i] && Zpt<pt_range[i+1] &&GZpt>pt_range[i] && GZpt<pt_range[i+1]){
              h_z_y_pt_Gen_VS_reco[i][j]->Fill(fabs(Zrapidity),fabs(GZrapidity),weight);
            }
          }
        }
      }
    }


    //2D & 3D leading jet plots for Z+1jet exclusive//
//    double j_y_range[9]={0,0.5,1.0,1.5,2.0,2.5,3.0,3.2,4.7};


   if(n_jet_30_n_bef==1){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1_BEF.Rapidity())>j_y_range[i] && fabs(j1_BEF.Rapidity())<j_y_range[i+1]){

            h_ljet_pt_y[i]->Fill(j1_BEF.Pt(),weight);

            if(fabs(Zrapidity)<1.0) h_ljet_pt_y_z0[i]->Fill(j1_BEF.Pt(),weight);
            if(fabs(Zrapidity)>1.0) h_ljet_pt_y_z1[i]->Fill(j1_BEF.Pt(),weight);

          }

          if(j1_BEF.Pt()>j_pt_range[i] &&j1_BEF.Pt()<j_pt_range[i+1]){

            h_ljet_y_pt[i]->Fill(fabs(j1_BEF.Rapidity()),weight);
            if(fabs(Zrapidity)<1.0) h_ljet_y_pt_z0[i]->Fill(fabs(j1_BEF.Rapidity()),weight);
            if(fabs(Zrapidity)>1.0) h_ljet_y_pt_z1[i]->Fill(fabs(j1_BEF.Rapidity()),weight);

          }
        }
    }        

    if(Gn_jet_30_n_bef==1){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1G_BEF.Rapidity())>j_y_range[i] && fabs(j1G_BEF.Rapidity())<j_y_range[i+1]){

            h_ljet_pt_y_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);

            if(fabs(GZrapidity)<1.0) h_ljet_pt_y_z0_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);
            if(fabs(GZrapidity)>1.0) h_ljet_pt_y_z1_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);

          }

          if(j1G_BEF.Pt()>j_pt_range[i] && j1G_BEF.Pt()<j_pt_range[i+1]){

            h_ljet_y_pt_Gen[i]->Fill(fabs(j1G_BEF.Rapidity()),PUweight);

            if(fabs(GZrapidity)<1.0) h_ljet_y_pt_z0_Gen[i]->Fill(fabs(j1G_BEF.Rapidity()),PUweight);
            if(fabs(GZrapidity)>1.0) h_ljet_y_pt_z1_Gen[i]->Fill(fabs(j1G_BEF.Rapidity()),PUweight);

          }
        }
    }        


    if(Gn_jet_30_n_bef==1 && n_jet_30_n_bef==1){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1G_BEF.Rapidity())>j_y_range[i] && fabs(j1G_BEF.Rapidity())<j_y_range[i+1]
           &&fabs(j1_BEF.Rapidity())>j_y_range[i] && fabs(j1_BEF.Rapidity())<j_y_range[i+1]){

            h_ljet_pt_y_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);

            if(fabs(Zrapidity)<1.0 &&fabs(GZrapidity)<1.0 ) h_ljet_pt_y_z0_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);
            if(fabs(Zrapidity)>1.0 &&fabs(GZrapidity)>1.0 ) h_ljet_pt_y_z1_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);

          }

          if(j1G_BEF.Pt()>j_pt_range[i] &&j1G_BEF.Pt()<j_pt_range[i+1]
           &&j1_BEF.Pt()>j_pt_range[i] &&j1_BEF.Pt()<j_pt_range[i+1]){

            h_ljet_y_pt_Gen_VS_reco[i]->Fill(fabs(j1_BEF.Rapidity()),fabs(j1G_BEF.Rapidity()),weight);

            if(fabs(Zrapidity)<1.0 &&fabs(GZrapidity)<1.0 ) h_ljet_y_pt_z0_Gen_VS_reco[i]->Fill(fabs(j1_BEF.Rapidity()),fabs(j1G_BEF.Rapidity()),weight);
            if(fabs(Zrapidity)>1.0 &&fabs(GZrapidity)>1.0 ) h_ljet_y_pt_z1_Gen_VS_reco[i]->Fill(fabs(j1_BEF.Rapidity()),fabs(j1G_BEF.Rapidity()),weight);

          }
        }
    }

//Leading & Subleading jets in Z+jets sample

    //2D & 3D leading jet plots for Z+1jet exclusive//
//    double j_y_range[9]={0,0.5,1.0,1.5,2.0,2.5,3.0,3.2,4.7};

    if(n_jet_30_n_bef>0){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1_BEF.Rapidity())>j_y_range[i] && fabs(j1_BEF.Rapidity())<j_y_range[i+1]){
            //cout<<"test"<<endl;
            h_inc_ljet_pt_y[i]->Fill(j1_BEF.Pt(),weight);
            h_inc_ljet_pt_y_fb[i]->Fill(j1_BEF.Pt(),weight);
            h_inc_ljet_pt_y_fixbin[i]->Fill(j1_BEF.Pt(),weight);


            if(fabs(Zrapidity)<1.0) h_inc_ljet_pt_y_z0[i]->Fill(j1_BEF.Pt(),weight);
            if(fabs(Zrapidity)>1.0) h_inc_ljet_pt_y_z1[i]->Fill(j1_BEF.Pt(),weight);
            if(fabs(Zrapidity)<1.0) h_inc_ljet_pt_y_fb_z0[i]->Fill(j1_BEF.Pt(),weight);
            if(fabs(Zrapidity)>1.0) h_inc_ljet_pt_y_fb_z1[i]->Fill(j1_BEF.Pt(),weight);

          }

          if(j1_BEF.Pt()>j_pt_range[i] &&j1_BEF.Pt()<j_pt_range[i+1]){

            h_inc_ljet_y_pt[i]->Fill(fabs(j1_BEF.Rapidity()),weight);
            if(fabs(Zrapidity)<1.0) h_inc_ljet_y_pt_z0[i]->Fill(fabs(j1_BEF.Rapidity()),weight);
            if(fabs(Zrapidity)>1.0) h_inc_ljet_y_pt_z1[i]->Fill(fabs(j1_BEF.Rapidity()),weight);

          }
        }
    }        

    if(Gn_jet_30_n_bef>0){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1G_BEF.Rapidity())>j_y_range[i] && fabs(j1G_BEF.Rapidity())<j_y_range[i+1]){
            
            h_inc_ljet_pt_y_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);
            h_inc_ljet_pt_y_fb_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);

            if(fabs(GZrapidity)<1.0) h_inc_ljet_pt_y_z0_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);
            if(fabs(GZrapidity)>1.0) h_inc_ljet_pt_y_z1_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);
            if(fabs(GZrapidity)<1.0) h_inc_ljet_pt_y_fb_z0_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);
            if(fabs(GZrapidity)>1.0) h_inc_ljet_pt_y_fb_z1_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);
          }

          if(j1G_BEF.Pt()>j_pt_range[i] && j1G_BEF.Pt()<j_pt_range[i+1]){

            h_inc_ljet_y_pt_Gen[i]->Fill(fabs(j1G_BEF.Rapidity()),PUweight);

            if(fabs(GZrapidity)<1.0) h_inc_ljet_y_pt_z0_Gen[i]->Fill(fabs(j1G_BEF.Rapidity()),PUweight);
            if(fabs(GZrapidity)>1.0) h_inc_ljet_y_pt_z1_Gen[i]->Fill(fabs(j1G_BEF.Rapidity()),PUweight);

          }
        }
    }        

    if(Gn_jet_30_n_bef>0 && n_jet_30_n_bef>0){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1G_BEF.Rapidity())>j_y_range[i] && fabs(j1G_BEF.Rapidity())<j_y_range[i+1]
          &&fabs( j1_BEF.Rapidity())>j_y_range[i] && fabs( j1_BEF.Rapidity())<j_y_range[i+1]){

            h_inc_ljet_pt_y_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);
            h_inc_ljet_pt_y_fb_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);

            if(fabs(Zrapidity)<1.0 &&fabs(GZrapidity)<1.0 ) h_inc_ljet_pt_y_z0_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);
            if(fabs(Zrapidity)>1.0 &&fabs(GZrapidity)>1.0 ) h_inc_ljet_pt_y_z1_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);

          if(fabs(Zrapidity)<1.0 &&fabs(GZrapidity)<1.0 ) h_inc_ljet_pt_y_fb_z0_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);
            if(fabs(Zrapidity)>1.0 &&fabs(GZrapidity)>1.0 ) h_inc_ljet_pt_y_fb_z1_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);
          }

          if(j1G_BEF.Pt()>j_pt_range[i] &&j1G_BEF.Pt()<j_pt_range[i+1]
           &&j1_BEF.Pt()>j_pt_range[i] &&j1_BEF.Pt()<j_pt_range[i+1]){

            h_inc_ljet_y_pt_Gen_VS_reco[i]->Fill(fabs(j1_BEF.Rapidity()),fabs(j1G_BEF.Rapidity()),weight);

            if(fabs(Zrapidity)<1.0 &&fabs(GZrapidity)<1.0 ) h_inc_ljet_y_pt_z0_Gen_VS_reco[i]->Fill(fabs(j1_BEF.Rapidity()),fabs(j1G_BEF.Rapidity()),weight);
            if(fabs(Zrapidity)>1.0 &&fabs(GZrapidity)>1.0 ) h_inc_ljet_y_pt_z1_Gen_VS_reco[i]->Fill(fabs(j1_BEF.Rapidity()),fabs(j1G_BEF.Rapidity()),weight);

          }
        }
    }



    if(n_jet_30_n_bef>1){

      if(j1_BEF.Pt() > Gthresh&&0.0<fabs(j1_BEF.Rapidity()) && fabs(j1_BEF.Rapidity())<4.7){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1_BEF.Rapidity())>j_y_range[i] && fabs(j1_BEF.Rapidity())<j_y_range[i+1]){

            h_inc_sljet_pt_y[i]->Fill(j2_BEF.Pt(),weight);

            if(fabs(Zrapidity)<1.0) h_inc_sljet_pt_y_z0[i]->Fill(j2_BEF.Pt(),weight);
            if(fabs(Zrapidity)>1.0) h_inc_sljet_pt_y_z1[i]->Fill(j2_BEF.Pt(),weight);

          }

          if(j1_BEF.Pt()>j_pt_range[i] &&j1_BEF.Pt()<j_pt_range[i+1]){

            h_inc_sljet_y_pt[i]->Fill(fabs(j2_BEF.Rapidity()),weight);
            if(fabs(Zrapidity)<1.0) h_inc_sljet_y_pt_z0[i]->Fill(fabs(j2_BEF.Rapidity()),weight);
            if(fabs(Zrapidity)>1.0) h_inc_sljet_y_pt_z1[i]->Fill(fabs(j2_BEF.Rapidity()),weight);

          } 
          if(j2_BEF.Pt()>j_pt_range[i] &&j2_BEF.Pt()<j_pt_range[i+1]){
            h_inc_ljet_pt_sljetpt[i]->Fill(j1_BEF.Pt(),weight);
            //h_inc_ljet_pt_sljetpt_diff[i]->Fill(j1_BEF.Pt()-j2_BEF.Pt(),weight);
            //if(fabs(Zrapidity)<1.0) h_inc_ljet_pt_sljetpt_diff_z0[i]->Fill(j1_BEF.Pt()-j2_BEF.Pt(),weight);
            //if(fabs(Zrapidity)>1.0) h_inc_ljet_pt_sljetpt_diff_z1[i]->Fill(j1_BEF.Pt()-j2_BEF.Pt(),weight);
            if(fabs(Zrapidity)<1.0) h_inc_ljet_pt_sljetpt_z0[i]->Fill(j1_BEF.Pt(),weight);
            if(fabs(Zrapidity)>1.0) h_inc_ljet_pt_sljetpt_z1[i]->Fill(j1_BEF.Pt(),weight);

          }

        }
      }  
    }        

    if(Gn_jet_30_n_bef>1){

      if(j1G_BEF.Pt() > Gthresh&&0.0<fabs(j1G_BEF.Rapidity()) && fabs(j1G_BEF.Rapidity())<4.7){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1G_BEF.Rapidity())>j_y_range[i] && fabs(j1G_BEF.Rapidity())<j_y_range[i+1]){

            h_inc_sljet_pt_y_Gen[i]->Fill(j2G_BEF.Pt(),PUweight);

            if(fabs(GZrapidity)<1.0) h_inc_sljet_pt_y_z0_Gen[i]->Fill(j2G_BEF.Pt(),PUweight);
            if(fabs(GZrapidity)>1.0) h_inc_sljet_pt_y_z1_Gen[i]->Fill(j2G_BEF.Pt(),PUweight);

          }

          if(j1G_BEF.Pt()>j_pt_range[i] && j1G_BEF.Pt()<j_pt_range[i+1]){

            h_inc_sljet_y_pt_Gen[i]->Fill(fabs(j2G_BEF.Rapidity()),PUweight);

            if(fabs(GZrapidity)<1.0) h_inc_sljet_y_pt_z0_Gen[i]->Fill(fabs(j2G_BEF.Rapidity()),PUweight);
            if(fabs(GZrapidity)>1.0) h_inc_sljet_y_pt_z1_Gen[i]->Fill(fabs(j2G_BEF.Rapidity()),PUweight);

          }
          if(j2G_BEF.Pt()>j_pt_range[i] &&j2G_BEF.Pt()<j_pt_range[i+1]){
            h_inc_ljet_pt_sljetpt_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);
            if(fabs(GZrapidity)<1.0) h_inc_ljet_pt_sljetpt_z0_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);
            if(fabs(GZrapidity)>1.0) h_inc_ljet_pt_sljetpt_z1_Gen[i]->Fill(j1G_BEF.Pt(),PUweight);
           // h_inc_ljet_pt_sljetpt_diff_Gen[i]->Fill(j1_BEF.Pt()-j2_BEF.Pt(),weight);
           // if(fabs(Zrapidity)<1.0) h_inc_ljet_pt_sljetpt_diff_z0_Gen[i]->Fill(j1G_BEF.Pt()-j2G_BEF.Pt(),weight);
           // if(fabs(Zrapidity)>1.0) h_inc_ljet_pt_sljetpt_diff_z1_Gen[i]->Fill(j1G_BEF.Pt()-j2G_BEF.Pt(),weight);
          }
        }
      }
    }        


    if(Gn_jet_30_n_bef>1 && n_jet_30_n_bef>1){

      if(j1G_BEF.Pt() > Gthresh&&0.0<fabs(j1G_BEF.Rapidity()) && fabs(j1G_BEF.Rapidity())<4.7
       &&j1_BEF.Pt() > Gthresh&&0.0<fabs(j1_BEF.Rapidity()) && fabs(j1_BEF.Rapidity())<4.7){

        for(int i = 0; i<jet_rap_ind;i++){

          if(fabs(j1G_BEF.Rapidity())>j_y_range[i] && fabs(j1G_BEF.Rapidity())<j_y_range[i+1]
           &&fabs(j1_BEF.Rapidity())>j_y_range[i] && fabs(j1_BEF.Rapidity())<j_y_range[i+1]){

            h_inc_sljet_pt_y_Gen_VS_reco[i]->Fill(j2_BEF.Pt(),j2G_BEF.Pt(),weight);

            if(fabs(Zrapidity)<1.0 &&fabs(GZrapidity)<1.0 ) h_inc_sljet_pt_y_z0_Gen_VS_reco[i]->Fill(j2_BEF.Pt(),j2G_BEF.Pt(),weight);
            if(fabs(Zrapidity)>1.0 &&fabs(GZrapidity)>1.0 ) h_inc_sljet_pt_y_z1_Gen_VS_reco[i]->Fill(j2_BEF.Pt(),j2G_BEF.Pt(),weight);

          }

          if(j1G_BEF.Pt()>j_pt_range[i] &&j1G_BEF.Pt()<j_pt_range[i+1]
           &&j1_BEF.Pt()>j_pt_range[i] &&j1_BEF.Pt()<j_pt_range[i+1]){

            h_inc_sljet_y_pt_Gen_VS_reco[i]->Fill(fabs(j2_BEF.Rapidity()),fabs(j2G_BEF.Rapidity()),weight);

            if(fabs(Zrapidity)<1.0 &&fabs(GZrapidity)<1.0 ) h_inc_sljet_y_pt_z0_Gen_VS_reco[i]->Fill(fabs(j2_BEF.Rapidity()),fabs(j2G_BEF.Rapidity()),weight);
            if(fabs(Zrapidity)>1.0 &&fabs(GZrapidity)>1.0 ) h_inc_sljet_y_pt_z1_Gen_VS_reco[i]->Fill(fabs(j2_BEF.Rapidity()),fabs(j2G_BEF.Rapidity()),weight);

          }

          if(j2_BEF.Pt()>j_pt_range[i] &&j2_BEF.Pt()<j_pt_range[i+1] && j2G_BEF.Pt()>j_pt_range[i] &&j2G_BEF.Pt()<j_pt_range[i+1]){
            h_inc_ljet_pt_sljetpt_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);
            if(fabs(Zrapidity)<1.0 &&fabs(GZrapidity)<1.0) h_inc_ljet_pt_sljetpt_z0_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);
            if(fabs(Zrapidity)<1.0 &&fabs(GZrapidity)>1.0) h_inc_ljet_pt_sljetpt_z1_Gen_VS_reco[i]->Fill(j1_BEF.Pt(),j1G_BEF.Pt(),weight);

            //h_inc_ljet_pt_sljetpt_diff_Gen_VS_reco[i]->Fill(j1_BEF.Pt()-j2_BEF.Pt(),j1_BEF.Pt()-j2_BEF.Pt(),weight);
            //if(fabs(Zrapidity)<1.0) h_inc_ljet_pt_sljetpt_diff_z0_Gen_VS_reco[i]->Fill(j1_BEF.Pt()-j2_BEF.Pt(),j1G_BEF.Pt()-j2G_BEF.Pt(),weight);
            //if(fabs(Zrapidity)>1.0) h_inc_ljet_pt_sljetpt_diff_z1_Gen_VS_reco[i]->Fill(j1_BEF.Pt()-j2_BEF.Pt(),j1G_BEF.Pt()-j2G_BEF.Pt(),weight);
          }
        }
      }
    }





    if(n_jet_30_n==4){
//     cout<<(j1_BE+j2_BE+j3_BE+j4_BE).M()<<endl;
      h_mjjjj->Fill((j1_BE+j2_BE+j3_BE+j4_BE).M(),weight);

    }

  if(n_jet_30_n_bef==4){
 //    cout<<(j1_BEF+j2_BEF+j3_BEF+j4_BEF).M()<<endl;
      h_mjjjj_BEF->Fill((j1_BEF+j2_BEF+j3_BEF+j4_BEF).M(),weight);

    }
    
//CENTRAL X-CHECK PLOTS
    if(Gen==1 && Reco== 1){
      h_n_jet_30_Gen_VS_reco->Fill(n_jet_30_n_30,Gn_jet_30_n_30,weight);
      if(Gn_jet_30_n>0 && n_jet_30_n>0){
        h_1jetpt_Gen_VS_reco->Fill(j1_BE.Pt(),j1G_BE.Pt(),weight);
        if(j1_BE.Pt()>30.&&j1G_BE.Pt()>30.)h_1jetY_Gen_VS_reco->Fill(fabs(j1_BE.Rapidity()),fabs(j1G_BE.Rapidity()),weight);
      }
      if(Gn_jet_30_n>1 && n_jet_30_n>1){
        h_2jetpt_Gen_VS_reco->Fill(j2_BE.Pt(),j2G_BE.Pt(),weight);
       if(j2_BE.Pt()>30.&&j2G_BE.Pt()>30.) h_2jetY_Gen_VS_reco->Fill(fabs(j2_BE.Rapidity()),fabs(j2G_BE.Rapidity()),weight);
      }
    }

    if(Reco==1){ 
      h_n_jet_30_unfold->Fill(n_jet_30_n_30,weight);
      if(n_jet_30_n>0) {
	h_1jetpt->Fill(j1_BE.Pt(),weight);
        if(j1_BE.Pt()>30.)h_1jetY->Fill(fabs(j1_BE.Rapidity()),weight);
      }	
      if(n_jet_30_n>1){
	h_2jetpt->Fill(j2_BE.Pt(),weight);
	if(j2_BE.Pt()>30.)h_2jetY->Fill(fabs(j2_BE.Rapidity()),weight);
      }
      if(n_jet_30_n==0){
          h_MZ_0->Fill(MZ,weight);
        h_Zphi_0->Fill(Zphi,weight);
        h_Zpt_0->Fill(Zpt,weight);
        h_Zrapidity_0->Fill(Zrapidity,weight);
      }
      if(n_jet_30_n==1){
          h_MZ_1->Fill(MZ,weight);
        h_Zphi_1->Fill(Zphi,weight);
        h_Zpt_1->Fill(Zpt,weight);
        h_Zrapidity_1->Fill(Zrapidity,weight);
      }
      if(n_jet_30_n==2){
          h_MZ_2->Fill(MZ,weight);
        h_Zphi_2->Fill(Zphi,weight);
        h_Zpt_2->Fill(Zpt,weight);
        h_Zrapidity_2->Fill(Zrapidity,weight);
      }
      if(n_jet_30_n==3){
          h_MZ_3->Fill(MZ,weight);
        h_Zphi_3->Fill(Zphi,weight);
        h_Zpt_3->Fill(Zpt,weight);
        h_Zrapidity_3->Fill(Zrapidity,weight);
      }
      if(n_jet_30_n==4){
          h_MZ_4->Fill(MZ,weight);
        h_Zphi_4->Fill(Zphi,weight);
        h_Zpt_4->Fill(Zpt,weight);
        h_Zrapidity_4->Fill(Zrapidity,weight);
      }
    }
   //if(event==2698963 || event==5806473){cout<<"EVENT "<<event<<" ISGEN "<<Gen<<" NGENJET "<<Gn_jet_30_n<<" LEP COLL SIZE "<<Dr01LepPt->size()<< " JET COLL SIZE "<< GjPt->size()<<endl;}
   if(Gen==1){
	h_n_jet_30_gen->Fill(Gn_jet_30_n_30,PUweight);

	if(Gn_jet_30_n>0){
        h_1Gjetpt->Fill(j1G_BE.Pt(),PUweight);
	if(j1G_BE.Pt()>30.)h_1GjetY->Fill(fabs(j1G_BE.Rapidity()),PUweight);
	}
        if(Gn_jet_30_n>1){
        h_2Gjetpt->Fill(j2G_BE.Pt(),PUweight);
        if(j2G_BE.Pt()>30.)h_2GjetY->Fill(fabs(j2G_BE.Rapidity()),PUweight);
        }		
	
        if(Gn_jet_30_n>2){
          h_3Gjetpt->Fill(j3G.Pt(),PUweight);
          if(j3G.Pt()>30.)h_3GjetY->Fill(j3G.Eta(),PUweight);
        }	
        if(Gn_jet_30_n>3){
          h_4Gjetpt->Fill(j4G.Pt(),PUweight);
          if(j4G.Pt()>30.)h_4GjetY->Fill(j4G.Eta(),PUweight);
        }	
        if(Gn_jet_30_n>4){
          h_5Gjetpt->Fill(j5G.Pt(),PUweight);
          if(j5G.Pt()>30.)h_5GjetY->Fill(j5G.Eta(),PUweight);
        }	
        if(Gn_jet_30_n>5){
          h_6Gjetpt->Fill(j6G.Pt(),PUweight);
          if(j6G.Pt()>30.)h_6GjetY->Fill(j6G.Eta(),PUweight);
        }		
   }



    if(n_jet_30_n_bef>0 && j1_BEF.Pt()>30.){
       
      h_ljet_rapidity->Fill(j1_BEF.Rapidity(),weight);
      h_ljet_eta->Fill(j1_BEF.Eta(),weight);
      if(n_jet_30_n_bef>1 &&j2_BEF.Pt()>30.){
        h_sljet_rapidity->Fill(j2_BEF.Rapidity(),weight);
        h_sljet_eta->Fill(j2_BEF.Eta(),weight);
      }
      if(EvtInfo_NumVtx<=5){
        h_ljet_rapidity_0->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_0->Fill(j1_BEF.Eta(),weight);
      }

      if(EvtInfo_NumVtx>5 &&EvtInfo_NumVtx<=10 ){
        h_ljet_rapidity_1->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_1->Fill(j1_BEF.Eta(),weight);
      }
      if(EvtInfo_NumVtx>10 &&EvtInfo_NumVtx<=15 ){
        h_ljet_rapidity_2->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_2->Fill(j1_BEF.Eta(),weight);
      }

      if(EvtInfo_NumVtx>15 &&EvtInfo_NumVtx<=20 ){
        h_ljet_rapidity_3->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_3->Fill(j1_BEF.Eta(),weight);
      }
      if(EvtInfo_NumVtx>20 &&EvtInfo_NumVtx<=25 ){
        h_ljet_rapidity_4->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_4->Fill(j1_BEF.Eta(),weight);
      }
  
      if(EvtInfo_NumVtx>25 &&EvtInfo_NumVtx<=30 ){
        h_ljet_rapidity_5->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_5->Fill(j1_BEF.Eta(),weight);
      }
      if(EvtInfo_NumVtx>30 &&EvtInfo_NumVtx<=35 ){
        h_ljet_rapidity_6->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_6->Fill(j1_BEF.Eta(),weight);
      }

      if(EvtInfo_NumVtx>35 &&EvtInfo_NumVtx<=40 ){
        h_ljet_rapidity_7->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_7->Fill(j1_BEF.Eta(),weight);
      }
      if(EvtInfo_NumVtx>40 &&EvtInfo_NumVtx<=45 ){
        h_ljet_rapidity_8->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_8->Fill(j1_BEF.Eta(),weight);
      }
      if(EvtInfo_NumVtx>45 &&EvtInfo_NumVtx<=50 ){
        h_ljet_rapidity_9->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_9->Fill(j1_BEF.Eta(),weight);
      }
      if(EvtInfo_NumVtx>50 &&EvtInfo_NumVtx<=55 ){
        h_ljet_rapidity_10->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_10->Fill(j1_BEF.Eta(),weight);
      }
      if(EvtInfo_NumVtx>55 &&EvtInfo_NumVtx<=60 ){
        h_ljet_rapidity_11->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_11->Fill(j1_BEF.Eta(),weight);
      }
      if(EvtInfo_NumVtx>60 &&EvtInfo_NumVtx<=65 ){
        h_ljet_rapidity_12->Fill(j1_BEF.Rapidity(),weight);
        h_ljet_eta_12->Fill(j1_BEF.Eta(),weight);
      }
    }


// if(Gen==1 &&Reco==0) cout<<weight<<endl;
    if(Gen==1 && Reco!= 1){
      h_n_jet_30_unselected->Fill(Gn_jet_30_n_30,weight);
    }


  }// end of event loop

  TTree *weight_tree;
  weight_tree = new TTree("tree","tree");
  weight_tree->Branch("nevent",&nevent);

  if(!sherpa)weight_tree->Branch("cnt0",&cnt0);
  if(sherpa)weight_tree->Branch("cnt0",&test);

  weight_tree->Fill();

  weight_tree->Print();
  theFile->Write();
  theFile->Close();
  
}//sys loop
}//ss loop
}//file loop

}//end void  

int main(int argc,char **argv){
  tree1r();
}


float DeltaR(float eta1, float eta2, float phi1, float phi2)
{
  float deta = eta2 - eta1;
  float dphi = phi2 - phi1;
  if (fabs(dphi) > 3.14) dphi = 6.28 - fabs(dphi);
  float DELTAR = sqrt(pow(dphi,2)+pow(deta,2))*1.0;
  return DELTAR;
}
float DeltaPhi(float phi1, float phi2)
{
  float dphi = phi2 - phi1;
  if (fabs(dphi) > 3.14) dphi = 6.28 - fabs(dphi);
  return dphi;
}




table::table()
{
}

table::table(string filename)
{
  ifstream file(filename.c_str());
//  cout << filename << endl;
//  if (file) cout << "OK" << endl;
//  else cout << "Not OK" << endl;
  double  pt1, pt2, eta1, eta2, effi, effiErrorLow, effiErrorHigh ;
/*  while( file >> eta1 >> eta2 >> pt1 >> pt2 >> effi >> effiErrorLow >> effiErrorHigh){
    cout << eta1 << "  " << eta2 << "  " << pt1 << "  " << pt2 << effi << "  " <<effiErrorLow << "  " << pt2<<effiErrorHigh <<endl;
    recd.push_back(record(pt1, pt2, eta1, eta2, effi,effiErrorLow, effiErrorHigh));
//    cout << "ef " << effi << endl;
  }
*/
  double data[7];
  while( file ){
      for (int i=0;i<7;i++)
      {
          file>>data[i];
          //cout << data[i] <<"  " ;
      }
      //cout << endl;
       recd.push_back(record(data[2],data[3],data[0],data[1],data[4],data[5],data[6]));

  }

}
record::record(): 
  ptLow(0), ptHi(0), etaLow(0), etaHi(0), effi(0),effiErrorLow(0), effiErrorHigh(0)
{
}

record::record(double pt1, double pt2, double eta1, double eta2, double eff, double effLow, double effHigh):
  ptLow(pt1), ptHi(pt2), etaLow(eta1), etaHi(eta2), effi(eff),effiErrorLow(effLow), effiErrorHigh(effHigh)
{
}

bool record::belongTo(double pt, double eta)
{
  return (pt < ptHi && pt >= ptLow) && (eta < etaHi && eta >= etaLow);
}
double table::getEfficiency(double pt, double eta){
    double hiPtBin= 0;
    for (unsigned int i=0; i != recd.size(); i++) {
        if((recd[i]).belongTo(pt, eta)) return recd[i].effi;
        if((recd[i]).belongTo(190, eta)) hiPtBin = recd[i].effi;
    }
    return hiPtBin;
}
double table::getEfficiencyLow(double pt, double eta){
    double hiPtBin= 0;
    for (unsigned int i=0; i != recd.size(); i++) {
        if((recd[i]).belongTo(pt, eta)) return recd[i].effi-recd[i].effiErrorLow;
        if((recd[i]).belongTo(190, eta)) hiPtBin = recd[i].effi;
    }
    return hiPtBin;
}
double table::getEfficiencyHigh(double pt, double eta){
    double hiPtBin= 0;
    for (unsigned int i=0; i != recd.size(); i++) {
        if((recd[i]).belongTo(pt, eta)) return recd[i].effi+recd[i].effiErrorHigh;
        if((recd[i]).belongTo(190, eta)) hiPtBin = recd[i].effi;
    }
    return hiPtBin;
}

