

/**
   \class    standalone_LumiReWeighting standalone_LumiReWeighting.h "PhysicsTools/Utilities/interface/standalone_LumiReWeighting.h"
   \brief    Class to provide lumi weighting for analyzers to weight "flat-to-N" MC samples to data
   This class will trivially take two histograms:
   1. The generated "flat-to-N" distributions from a given processing (or any other generated input)
   2. A histogram generated from the "estimatePileup" macro here:
   https://twiki.cern.ch/twiki/bin/view/CMS/LumiCalc#How_to_use_script_estimatePileup
   and produce weights to convert the input distribution (1) to the latter (2).
   \author Salvatore Rappoccio, modified by Mike Hildreth
  
*/
#ifndef standalone_LumiReWeighting_h
#define standalone_LumiReWeighting_h
#include "TH1.h"
#include "TFile.h"
#include <string>
#include "TH1.h"
#include "TFile.h"
#include <vector>
#include <TROOT.h>
#include <iostream>
#include <map>


class standalone_LumiReWeighting {
 public:

  standalone_LumiReWeighting(int year=2016,int mode=0,int nBin=50); // 0: central, -1: down, +1: up
  virtual ~standalone_LumiReWeighting();
  double weight( int npv) ;
  void weightOOT_init();

  //Make a map to use for accessing different arrays using a string  
  std::map<std::string,double*> pileupMap;
  
 protected:

  TH1D*      weights_;
};

//Set the pileup numbers here

double Data_2016_50Bin[50] = {6.47328e-06,2.2704e-05,6.25767e-05,8.48423e-05,0.000121463,0.000163068,
			     0.000234489,0.00066196,0.00119512,0.0021577,0.00479954,0.0097697,
			     0.0163688,0.0238365,0.0319656,0.0405244,0.0478945,0.0529775,0.0558863,
			     0.0573049,0.0581333,0.0585749,0.0580522,0.0563036,0.0536791,0.0505037,
			     0.0468805,0.0429096,0.0387376,0.0344764,0.0301956,0.025971,0.0219089,
			     0.0181256,0.0147155,0.0117309,0.00918171,0.00704638,0.00528755,
			     0.00386476,0.00273991,0.00187658,0.00123743,0.000783447,0.000475257,
			     0.000275812,0.000152974,8.10415e-05,4.10129e-05,1.98489e-05};

double Data_2016_75Bin[75] = {238796,837457,2.2762e+06,3.08988e+06,4.41555e+06,5.92832e+06,6.9546e+06,
			      1.28649e+07, 3.52032e+07,7.8591e+07,1.76697e+08,3.59658e+08,6.02165e+08,
			      8.75557e+08,1.1726e+09, 1.48464e+09,1.74864e+09,1.92425e+09,2.02409e+09,
			      2.07661e+09,2.10911e+09,2.12551e+09, 2.10558e+09,2.04057e+09,1.94292e+09,
			      1.82438e+09,1.68946e+09,1.54309e+09,1.39144e+09, 1.23854e+09,1.08614e+09,
			      9.36073e+08,7.91566e+08,6.56562e+08,5.34424e+08,4.27117e+08, 3.35104e+08,
			      2.57724e+08,1.93751e+08,1.41831e+08,1.00671e+08,6.90139e+07,4.55401e+07, 
			      2.88475e+07,1.75063e+07,1.01626e+07,5.63778e+06,2.98728e+06,1.512e+06,
			      731845,339822, 152545,67404.8,30489.7,15152.1,8975.91,6496.15,5434.8,
			      4889.96,4521.72,4208.46,3909.76, 3614.27,3320.72,3031.1,2748.24,2474.98,
			      2213.82,1966.82,1735.55,1521.11,1324.15,1144.9, 983.22,838.668};

double Data_2016_75Bin_up[75] = {368012, 795684, 2.23314e+06, 2.81877e+06, 4.09679e+06, 5.45164e+06, 
				 6.46557e+06,9.08515e+06, 2.38236e+07, 5.40126e+07, 1.15968e+08, 
				 2.45968e+08,4.43247e+08, 6.79951e+08, 9.38361e+08, 1.22239e+09, 
				 1.5117e+09, 1.74649e+09,1.89874e+09, 1.97905e+09, 2.01631e+09, 
				 2.0397e+09, 2.05246e+09, 2.03468e+09,1.97679e+09, 1.88876e+09, 
				 1.78137e+09, 1.65887e+09, 1.52502e+09, 1.38538e+09,1.24436e+09, 
				 1.10409e+09, 9.65732e+08, 8.31136e+08, 7.0302e+08, 5.84275e+08, 
				 4.77177e+08, 3.82967e+08, 3.01854e+08, 2.3329e+08, 1.76329e+08, 
				 1.29915e+08, 9.29848e+07, 6.4448e+07, 4.31402e+07, 2.78291e+07, 
				 1.7273e+07, 1.03039e+07, 5.90373e+06, 3.24882e+06, 1.71837e+06, 
				 875325, 431263, 207253, 98725.4, 47968.7, 24840, 14401.9, 9606.95, 
				 7272.61, 6011.04, 5229.59, 4675.1, 4236.42, 3862.1, 3526.81, 3217.5, 
				 2927.42, 2653.28, 2393.73, 2148.47, 1917.75, 1702, 1501.63, 1316.91};

double Data_2016_75Bin_dn[75]={404870, 1.19368e+06, 2.48715e+06, 3.63587e+06, 5.01113e+06, 6.69087e+06,
			       8.14958e+06, 2.00073e+07, 5.19315e+07, 1.19723e+08, 2.72659e+08, 
			       5.12976e+08, 8.02162e+08, 1.11991e+09, 1.46933e+09, 1.79485e+09, 
			       2.02364e+09, 2.14872e+09,
			       2.2039e+09, 2.23361e+09, 2.25007e+09, 2.22729e+09, 2.15269e+09, 
			       2.04119e+09, 1.90607e+09, 1.75258e+09, 1.58741e+09, 1.41816e+09,
			       1.24912e+09, 1.08222e+09, 9.19843e+08, 7.65786e+08, 6.24191e+08,
			       4.98216e+08, 3.89369e+08, 2.97616e+08,
			       2.21904e+08,1.60766e+08,1.12666e+08,7.60491e+07, 4.92587e+07, 3.05274e+07,
			       1.80628e+07, 1.01892e+07, 5.47571e+06, 2.80384e+06, 1.36997e+06, 641115,
			       289724, 128577, 57889.2, 27912.9, 15375.6, 10018.7, 7551.4, 6251.27,
			       5441.32, 4853.3, 4376.11, 3960.33, 3582.53, 3231.29, 2901.32, 2590.59,
			       2298.73, 2026.15, 1773.48, 1541.25, 1329.74, 1138.86, 968.204, 817.048,
			       684.391, 569.028, 469.605};


//Spring MC
double MC_2016_50Bin[50] = {0.000829312873542, 0.00124276120498,  0.00339329181587,  0.00408224735376,
			    0.00383036590008,  0.00659159288946,  0.00816022734493,  0.00943640833116,
			    0.0137777376066,   0.017059392038,    0.0213193035468,   0.0247343174676,
			    0.0280848773878,   0.0323308476564,   0.0370394341409,   0.0456917721191,
			    0.0558762890594,   0.0576956187107,   0.0625325287017,   0.0591603758776,
			    0.0656650815128,   0.0678329011676,   0.0625142146389,   0.0548068448797,
			    0.0503893295063,   0.040209818868,    0.0374446988111,   0.0299661572042,
			    0.0272024759921,   0.0219328403791,   0.0179586571619,   0.0142926728247,
			    0.00839941654725,  0.00522366397213,  0.00224457976761,  0.000779274977993,
			    0.000197066585944, 7.16031761328e-05, 0.0,   0.0, 0.0,   0.0,
			    0.0,   0.0,  0.0,   0.0,  0.0,   0.0,  0.0,  0.0};

//Summer MC
double MC_2016_75Bin[75]= {1.78653e-05,2.56602e-05,5.27857e-05,8.88954e-05,0.000109362,
			   0.000140973,0.000240998,0.00071209,0.00130121,0.00245255,
			   0.00502589,0.00919534,0.0146697,0.0204126,0.0267586,
			   0.0337697,0.0401478,0.0450159,0.0490577,0.0524855,
			   0.0548159,0.0559937,0.0554468,0.0537687,0.0512055,
			   0.0476713,0.0435312,0.0393107,0.0349812,0.0307413,
			   0.0272425,0.0237115,0.0208329,0.0182459,0.0160712,
			   0.0142498,0.012804,0.011571,0.010547,0.00959489,0.00891718,
			   0.00829292,0.0076195,0.0069806,0.0062025,0.00546581,
			   0.00484127,0.00407168,0.00337681,0.00269893,0.00212473,
			   0.00160208,0.00117884,0.000859662,0.000569085,0.000365431,
			   0.000243565,0.00015688,9.88128e-05,6.53783e-05,3.73924e-05,
			   2.61382e-05,2.0307e-05,1.73032e-05,1.435e-05,1.36486e-05,
			   1.35555e-05,1.37491e-05,1.34255e-05,1.33987e-05,1.34061e-05,
			   1.34211e-05,1.34177e-05,1.32959e-05,1.33287e-05};

standalone_LumiReWeighting::standalone_LumiReWeighting(int year,int mode,int nBin) {

  
  //Add the pileup distributions to the map
  pileupMap.insert( std::pair<std::string,double*>("Data_2016_50Bin",Data_2016_50Bin) );
  pileupMap.insert( std::pair<std::string,double*>("Data_2016_75Bin",Data_2016_75Bin) );
  pileupMap.insert( std::pair<std::string,double*>("Data_2016_75Bin_up",Data_2016_75Bin_up) );
  pileupMap.insert( std::pair<std::string,double*>("Data_2016_75Bin_dn",Data_2016_75Bin_dn) );
  pileupMap.insert( std::pair<std::string,double*>("MC_2016_50Bin",MC_2016_50Bin) );
  pileupMap.insert( std::pair<std::string,double*>("MC_2016_75Bin",MC_2016_75Bin) );
  
  std::vector<double> MC_distr;
  std::vector<double> Lumi_distr;

  MC_distr.clear();
  Lumi_distr.clear();

  printf("standalone_LumiReWeighting Mode = %d\n",mode);

  char tmpName[50];
  char tmpNameMC[50];
  snprintf(tmpNameMC,50,"%d_%dBin",year,nBin);
  switch(mode){
  case 0:
    snprintf(tmpName,50,"%d_%dBin",year,nBin);
    break;
  case 1:
    snprintf(tmpName,50,"%d_%dBin_up",year,nBin);
    break;
  case -1:
    snprintf(tmpName,50,"%d_%dBin_dn",year,nBin);
    break;
  default:
    printf("Mode for standalone lumi reweighting is incorrect\n");
    abort();
    break;
  }
  std::string dataName = "Data_";
  dataName += tmpName;
  std::string mcName = "MC_";
  mcName += tmpNameMC;

  std::cout<<"standalone_LumiReWeighting dataName = "<<dataName<<std::endl;
  std::cout<<"standalone_LumiReWeighting mcName = "<<mcName<<std::endl;

  for( int i=0; i< nBin; ++i) {  
    Lumi_distr.push_back( pileupMap[dataName][i] );
    MC_distr.push_back( pileupMap[mcName][i] );
  }

  // first, check they are the same size...
  if( MC_distr.size() != Lumi_distr.size() ){   
    std::cout << "MC_distr.size() = " << MC_distr.size() << std::endl;
    std::cout << "Lumi_distr.size() = " << Lumi_distr.size() << std::endl;
    std::cerr <<"ERROR:standalone_LumiReWeighting: input vectors have different sizes. Quitting... \n";
  }

  weights_ = new TH1D(Form("luminumer_%d",mode),
		      Form("luminumer_%d",mode),
		      nBin,0., double(nBin));

  weights_->SetBit(TH1::kIsAverage);

  TH1D* tmp = new TH1D(Form("lumidenom_%d",mode),
		       Form("lumidenom_%d",mode),
		       nBin,0., double(nBin));

  TH1D* den = new TH1D(Form("lumidenom_%d",mode),
		       Form("lumidenom_%d",mode),
		       nBin,0., double(nBin));

  den->SetBit(TH1::kIsAverage);

  for(int ibin = 1; ibin<nBin+1; ++ibin ) {
    weights_->SetBinContent(ibin, Lumi_distr[ibin-1]);
    den->SetBinContent(ibin,MC_distr[ibin-1]);
  }

  tmp = (TH1D*) weights_->Clone("tmp");

  // check integrals, make sure things are normalized

  double deltaH = weights_->Integral();
  if(fabs(1.0 - deltaH) > 0.02 ) { //*OOPS*...
    printf("Normalizing Data PU: %F\n",weights_->Integral());
    weights_->Scale( 1.0/ weights_->Integral() );
    tmp->Scale( 1.0/ tmp->Integral() );
  }
  double deltaMC = den->Integral();
  if(fabs(1.0 - deltaMC) > 0.02 ) {
    printf("Normalizing MC PU: %F\n",den->Integral());
    den->Scale(1.0/ den->Integral());
  }

 
  printf("Data norm = %F  |||  MC norm = %F  |||  tmp norm = %F\n",weights_->Integral(),den->Integral(),tmp->Integral());

  //std::cout << "      Data Input        MC Input         tmp Input" << std::endl;
  //for(int ibin = 1; ibin<nBin+1; ++ibin){
  //  std::cout <<ibin-1 << "     " << weights_->GetBinContent(ibin) <<"       "<< den->GetBinContent(ibin) << "         "<<tmp->GetBinContent(ibin)<<std::endl;
  //}

  weights_->Divide( den );
  for(int ibin = 1; ibin <= nBin; ++ibin){
    tmp->SetBinContent(ibin, tmp->GetBinContent(ibin) / den->GetBinContent(ibin));
  }
 
  double inte = 0;
  double int2 = 0;
  //printf("MC_distr       den\n");
  for(int ibin = 1; ibin <= nBin; ++ibin){
    //printf("%F  |||  %F\n",MC_distr[ibin-1],den->GetBinContent(ibin));
    inte += weights_->GetBinContent(ibin) * MC_distr[ibin-1];
    int2 += tmp->GetBinContent(ibin) * MC_distr[ibin-1];
  }

  std::cout << "PU weight normalisation: " << inte <<"   tmp="<<int2<<"\n";

//  weights_->Scale(1/weights_->Integral());//Bugra Bilin, added this to normalize weigts.

  //std::cout << "Reweighting: Computed Weights per In-Time Nint " << std::endl;


  //for(int ibin = 1; ibin<nBin+1; ++ibin){
    //std::cout << "   " << ibin-1 << " " << weights_->GetBinContent(ibin) << std::endl;
  //}

  //   weightOOT_init();
  //std::cout << "=======================================================================" << std::endl;

}

standalone_LumiReWeighting::~standalone_LumiReWeighting()
{
}



double standalone_LumiReWeighting::weight( int npv ) {
  int bin = weights_->GetXaxis()->FindBin( npv );
  return weights_->GetBinContent( bin );
}

#if 0
void standalone_LumiReWeighting::weightOOT_init() {

  // The following are poisson distributions with different means, where the maximum
  // of the function has been normalized to weight 1.0
  // These are used to reweight the out-of-time pileup to match the in-time distribution.
  // The total event weight is the product of the in-time weight, the out-of-time weight,
  // and a residual correction to fix the distortions caused by the fact that the out-of-time
  // distribution is not flat.

  static double weight_24[25] = {
    0,
    0,
    0,
    0,
    2.46277e-06,
    2.95532e-05,
    0.000104668,
    0.000401431,
    0.00130034,
    0.00342202,
    0.00818132,
    0.0175534,
    0.035784,
    0.0650836,
    0.112232,
    0.178699,
    0.268934,
    0.380868,
    0.507505,
    0.640922,
    0.768551,
    0.877829,
    0.958624,
    0.99939,
    1
  };

  static double weight_23[25] = {
    0,
    1.20628e-06,
    1.20628e-06,
    2.41255e-06,
    1.20628e-05,
    6.39326e-05,
    0.000252112,
    0.000862487,
    0.00244995,
    0.00616527,
    0.0140821,
    0.0293342,
    0.0564501,
    0.100602,
    0.164479,
    0.252659,
    0.36268,
    0.491427,
    0.627979,
    0.75918,
    0.873185,
    0.957934,
    0.999381,
    1,
    0.957738
  };

  static double weight_22[25] = {
    0,
    0,
    0,
    5.88636e-06,
    3.0609e-05,
    0.000143627,
    0.000561558,
    0.00173059,
    0.00460078,
    0.0110616,
    0.0238974,
    0.0475406,
    0.0875077,
    0.148682,
    0.235752,
    0.343591,
    0.473146,
    0.611897,
    0.748345,
    0.865978,
    0.953199,
    0.997848,
    1,
    0.954245,
    0.873688
  };

  static double weight_21[25] = {
    0,
    0,
    1.15381e-06,
    8.07665e-06,
    7.1536e-05,
    0.000280375,
    0.00107189,
    0.00327104,
    0.00809396,
    0.0190978,
    0.0401894,
    0.0761028,
    0.13472,
    0.216315,
    0.324649,
    0.455125,
    0.598241,
    0.739215,
    0.861866,
    0.953911,
    0.998918,
    1,
    0.956683,
    0.872272,
    0.76399
  };
 
 
  static double weight_20[25] = {
    0,
    0,
    1.12532e-06,
    2.58822e-05,
    0.000145166,
    0.000633552,
    0.00215048,
    0.00592816,
    0.0145605,
    0.0328367,
    0.0652649,
    0.11893,
    0.19803,
    0.305525,
    0.436588,
    0.581566,
    0.727048,
    0.8534,
    0.949419,
    0.999785,
    1,
    0.953008,
    0.865689,
    0.753288,
    0.62765
  }; 
  static double weight_19[25] = {
    0,
    0,
    1.20714e-05,
    5.92596e-05,
    0.000364337,
    0.00124994,
    0.00403953,
    0.0108149,
    0.025824,
    0.0544969,
    0.103567,
    0.17936,
    0.283532,
    0.416091,
    0.562078,
    0.714714,
    0.846523,
    0.947875,
    1,
    0.999448,
    0.951404,
    0.859717,
    0.742319,
    0.613601,
    0.48552
  };

  static double weight_18[25] = {
    0,
    3.20101e-06,
    2.88091e-05,
    0.000164319,
    0.000719161,
    0.00250106,
    0.00773685,
    0.0197513,
    0.0443693,
    0.0885998,
    0.159891,
    0.262607,
    0.392327,
    0.543125,
    0.69924,
    0.837474,
    0.943486,
    0.998029,
    1,
    0.945937,
    0.851807,
    0.729309,
    0.596332,
    0.467818,
    0.350434
  };

 
  static double weight_17[25] = {
    1.03634e-06,
    7.25437e-06,
    4.97443e-05,
    0.000340956,
    0.00148715,
    0.00501485,
    0.0143067,
    0.034679,
    0.0742009,
    0.140287,
    0.238288,
    0.369416,
    0.521637,
    0.682368,
    0.828634,
    0.939655,
    1,
    0.996829,
    0.94062,
    0.841575,
    0.716664,
    0.582053,
    0.449595,
    0.331336,
    0.234332
  };

 
  static double weight_16[25] = {
    4.03159e-06,
    2.41895e-05,
    0.000141106,
    0.00081942,
    0.00314565,
    0.00990662,
    0.026293,
    0.0603881,
    0.120973,
    0.214532,
    0.343708,
    0.501141,
    0.665978,
    0.820107,
    0.938149,
    1,
    0.99941,
    0.940768,
    0.837813,
    0.703086,
    0.564023,
    0.42928,
    0.312515,
    0.216251,
    0.14561
  };
 
 
  static double weight_15[25] = {
    9.76084e-07,
    5.07564e-05,
    0.000303562,
    0.00174036,
    0.00617959,
    0.0188579,
    0.047465,
    0.101656,
    0.189492,
    0.315673,
    0.474383,
    0.646828,
    0.809462,
    0.934107,
    0.998874,
    1,
    0.936163,
    0.827473,
    0.689675,
    0.544384,
    0.40907,
    0.290648,
    0.198861,
    0.12951,
    0.0808051
  };
 
 
  static double weight_14[25] = {
    1.13288e-05,
    0.000124617,
    0.000753365,
    0.00345056,
    0.0123909,
    0.0352712,
    0.0825463,
    0.16413,
    0.287213,
    0.44615,
    0.625826,
    0.796365,
    0.930624,
    0.999958,
    1,
    0.934414,
    0.816456,
    0.672939,
    0.523033,
    0.386068,
    0.269824,
    0.180342,
    0.114669,
    0.0698288,
    0.0406496
  };

 
  static double weight_13[25] = {
    2.54296e-05,
    0.000261561,
    0.00167018,
    0.00748083,
    0.0241308,
    0.0636801,
    0.138222,
    0.255814,
    0.414275,
    0.600244,
    0.779958,
    0.92256,
    0.999155,
    1,
    0.927126,
    0.804504,
    0.651803,
    0.497534,
    0.35976,
    0.245834,
    0.160904,
    0.0991589,
    0.0585434,
    0.0332437,
    0.0180159
  };

  static double weight_12[25] = {
    5.85742e-05,
    0.000627706,
    0.00386677,
    0.0154068,
    0.0465892,
    0.111683,
    0.222487,
    0.381677,
    0.5719,
    0.765001,
    0.915916,
    1,
    0.999717,
    0.921443,
    0.791958,
    0.632344,
    0.475195,
    0.334982,
    0.223666,
    0.141781,
    0.0851538,
    0.048433,
    0.0263287,
    0.0133969,
    0.00696683
  };

 
  static double weight_11[25] = {
    0.00015238,
    0.00156064,
    0.00846044,
    0.0310939,
    0.0856225,
    0.187589,
    0.343579,
    0.541892,
    0.74224,
    0.909269,
    0.998711,
    1,
    0.916889,
    0.77485,
    0.608819,
    0.447016,
    0.307375,
    0.198444,
    0.121208,
    0.070222,
    0.0386492,
    0.0201108,
    0.0100922,
    0.00484937,
    0.00222458
  };

  static double weight_10[25] = {
    0.000393044,
    0.00367001,
    0.0179474,
    0.060389,
    0.151477,
    0.302077,
    0.503113,
    0.720373,
    0.899568,
    1,
    0.997739,
    0.909409,
    0.75728,
    0.582031,
    0.415322,
    0.277663,
    0.174147,
    0.102154,
    0.0566719,
    0.0298642,
    0.0147751,
    0.00710995,
    0.00319628,
    0.00140601,
    0.000568796
  };

 
  static double weight_9[25] = {
    0.00093396,
    0.00854448,
    0.0380306,
    0.113181,
    0.256614,
    0.460894,
    0.690242,
    0.888781,
    1,
    0.998756,
    0.899872,
    0.735642,
    0.552532,
    0.382726,
    0.246114,
    0.147497,
    0.0825541,
    0.0441199,
    0.0218157,
    0.0103578,
    0.00462959,
    0.0019142,
    0.000771598,
    0.000295893,
    0.000111529
  };

 
  static double weight_8[25] = {
    0.00240233,
    0.0192688,
    0.0768653,
    0.205008,
    0.410958,
    0.65758,
    0.875657,
    0.999886,
    1,
    0.889476,
    0.711446,
    0.517781,
    0.345774,
    0.212028,
    0.121208,
    0.0644629,
    0.0324928,
    0.0152492,
    0.00673527,
    0.0028547,
    0.00117213,
    0.000440177,
    0.000168471,
    5.80689e-05,
    1.93563e-05
  };

  static double weight_7[25] = {
    0.00617233,
    0.0428714,
    0.150018,
    0.350317,
    0.612535,
    0.856525,
    0.999923,
    1,
    0.87544,
    0.679383,
    0.478345,
    0.303378,
    0.176923,
    0.0950103,
    0.0476253,
    0.0222211,
    0.00972738,
    0.00392962,
    0.0015258,
    0.000559168,
    0.000183928,
    6.77983e-05,
    1.67818e-05,
    7.38398e-06,
    6.71271e-07
  };
 
  static double weight_6[25] = {
    0.0154465,
    0.0923472,
    0.277322,
    0.55552,
    0.833099,
    0.999035,
    1,
    0.855183,
    0.641976,
    0.428277,
    0.256804,
    0.139798,
    0.0700072,
    0.0321586,
    0.0137971,
    0.00544756,
    0.00202316,
    0.000766228,
    0.000259348,
    8.45836e-05,
    1.80362e-05,
    8.70713e-06,
    3.73163e-06,
    6.21938e-07,
    0
  };
 
 
  static double weight_5[25] = {
    0.0382845,
    0.191122,
    0.478782,
    0.797314,
    1,
    0.997148,
    0.831144,
    0.59461,
    0.371293,
    0.205903,
    0.103102,
    0.0471424,
    0.0194997,
    0.00749415,
    0.00273709,
    0.000879189,
    0.000286049,
    0.000102364,
    1.70606e-05,
    3.98081e-06,
    2.27475e-06,
    0,
    0,
    0,
    0
  };
 
 
  static double weight_4[25] = {
    0.0941305,
    0.373824,
    0.750094,
    1,
    0.997698,
    0.800956,
    0.532306,
    0.304597,
    0.152207,
    0.0676275,
    0.0270646,
    0.00975365,
    0.00326077,
    0.00101071,
    0.000301781,
    7.41664e-05,
    1.58563e-05,
    3.58045e-06,
    1.02299e-06,
    0,
    5.11493e-07,
    0,
    0,
    0,
    0
  };
 
 
  static double weight_3[25] = {
    0.222714,
    0.667015,
    1,
    0.999208,
    0.750609,
    0.449854,
    0.224968,
    0.0965185,
    0.0361225,
    0.012084,
    0.00359618,
    0.000977166,
    0.000239269,
    6.29422e-05,
    1.16064e-05,
    1.78559e-06,
    0,
    4.46398e-07,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  };
 
  static double weight_2[25] = {
    0.499541,
    0.999607,
    1,
    0.666607,
    0.333301,
    0.13279,
    0.0441871,
    0.0127455,
    0.00318434,
    0.00071752,
    0.000132204,
    2.69578e-05,
    5.16999e-06,
    2.21571e-06,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  };
 
  static double weight_1[25] = {
    0.999165,
    1,
    0.499996,
    0.166868,
    0.0414266,
    0.00831053,
    0.00137472,
    0.000198911,
    2.66302e-05,
    2.44563e-06,
    2.71737e-07,
    2.71737e-07,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  };
 
  static double weight_0[25] = {
    1,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
  };

  double* WeightPtr = 0;

  for(int iint = 0; iint<25; ++iint){
    if(iint ==0) WeightPtr = weight_0;
    if(iint ==1) WeightPtr = weight_1;
    if(iint ==2) WeightPtr = weight_2;
    if(iint ==3) WeightPtr = weight_3;
    if(iint ==4) WeightPtr = weight_4;
    if(iint ==5) WeightPtr = weight_5;
    if(iint ==6) WeightPtr = weight_6;
    if(iint ==7) WeightPtr = weight_7;
    if(iint ==8) WeightPtr = weight_8;
    if(iint ==9) WeightPtr = weight_9;
    if(iint ==10) WeightPtr = weight_10;
    if(iint ==11) WeightPtr = weight_11;
    if(iint ==12) WeightPtr = weight_12;
    if(iint ==13) WeightPtr = weight_13;
    if(iint ==14) WeightPtr = weight_14;
    if(iint ==15) WeightPtr = weight_15;
    if(iint ==16) WeightPtr = weight_16;
    if(iint ==17) WeightPtr = weight_17;
    if(iint ==18) WeightPtr = weight_18;
    if(iint ==19) WeightPtr = weight_19;
    if(iint ==20) WeightPtr = weight_20;
    if(iint ==21) WeightPtr = weight_21;
    if(iint ==22) WeightPtr = weight_22;
    if(iint ==23) WeightPtr = weight_23;
    if(iint ==24) WeightPtr = weight_24;

  }
}
#endif

#endif
