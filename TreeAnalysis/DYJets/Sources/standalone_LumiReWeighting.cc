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
#include "TH1.h"
#include "TFile.h"
#include <string>
#include "TH1.h"
#include "TFile.h"
#include <vector>
#include <TROOT.h>
#include <string>
#include <iostream>
#include "standalone_LumiReWeighting.h"
#include <math.h>

using namespace std;

standalone_LumiReWeighting::standalone_LumiReWeighting(TString leptonFlavor, int year,int mode) {

    //std::cout << "=======================================================================" << std::endl;

    std::vector<float> MC_distr;
    std::vector<float> Lumi_distr;

    MC_distr.clear();
    Lumi_distr.clear();

    if (leptonFlavor == "DMu"){
        cout << "Muons PU distribution" << endl;
    }
    else if (leptonFlavor == "DE"){
        cout << "Electrons PU distribution" << endl;
    }
    switch (mode)
    {
        case 0:
            //std::cout << "Using central value " << std::endl;
            break;
        case 1:
            //std::cout << "Using +1 sigma 5% value " << std::endl;
            break;
        case -1:
            //std::cout << "Using -1 sigma 5% value " << std::endl;
            break;
        default:
            //std::cout << "Using central value " << std::endl;
            break;
    } // end of switch

    Int_t NBins = 60;
    if (year == 2011 || year == 201117) NBins = 50;

    for( int i=0; i< NBins; ++i) {
        if (year == 2011)
        {
            switch (mode){
                case 0:
                    //Lumi_distr.push_back(Data2011[i]);
                    Lumi_distr.push_back(CENTRAL2011[i]);
                    break;
                case 1:
                    //Lumi_distr.push_back(Data2011Up[i]);
                    Lumi_distr.push_back(HIGH2011[i]);
                    break;
                case -1:
                    //Lumi_distr.push_back(Data2011Down[i]);
                    Lumi_distr.push_back(LOW2011[i]);
                    break;
                default:
                    //Lumi_distr.push_back(Data2011[i]);
                    Lumi_distr.push_back(CENTRAL2011[i]);
                    break;
            } // end of switch
            MC_distr.push_back(Fall2011[i]);
        }
        else if (year == 201117) {
            switch (mode){
                case 0:
                    Lumi_distr.push_back(Data2011_HLT17[i]);
                    break;
                case 1:
                    Lumi_distr.push_back(Data2011Up_HLT17[i]);
                    break;
                case -1:
                    Lumi_distr.push_back(Data2011Down_HLT17[i]);
                    break;
                default:
                    Lumi_distr.push_back(Data2011_HLT17[i]);
                    break;
            } // end of switch
            MC_distr.push_back(Fall2011[i]);
        }
        else if (year == 2012)
        {
            switch (mode){
                case 0:
                    Lumi_distr.push_back(Data2012[i]);
                    break;
                case 1:
                    Lumi_distr.push_back(Data2012Up[i]);
                    break;
                case -1:
                    Lumi_distr.push_back(Data2012Down[i]);
                    break;
                default:
                    Lumi_distr.push_back(Data2012[i]);
                    break;
            } // end of switch
            MC_distr.push_back(Summer2012_S10[i]);
        }
        else if ( year == 2013 ){
            if (leptonFlavor == "SMu" ){
                switch (mode){
                    case 0:
                        Lumi_distr.push_back(SMu2013_Central[i]);
                        break;
                    case 1:
                        Lumi_distr.push_back(SMu2013_High[i]);
                        break;
                    case -1:
                        Lumi_distr.push_back(SMu2013_Low[i]);
                        break;
                    default:
                        Lumi_distr.push_back(SMu2013_Central[i]);
                        break;
                }

            }
            if (leptonFlavor == "SMuE" || leptonFlavor == "TTMuE" ){
                switch (mode){
                    case 0:
                        Lumi_distr.push_back(SMuE2013_Central[i]);
                        break;
                    case 1:
                        Lumi_distr.push_back(SMuE2013_High[i]);
                        break;
                    case -1:
                        Lumi_distr.push_back(SMuE2013_Low[i]);
                        break;
                    default:
                        Lumi_distr.push_back(SMuE2013_Central[i]);
                        break;
                }

            }

            else  if (leptonFlavor == "SE"  ){
                switch (mode){
                    case 0:
                        Lumi_distr.push_back(SE2013_Central[i]);
                        break;
                    case 1:
                        Lumi_distr.push_back(SE2013_High[i]);
                        break;
                    case -1:
                        Lumi_distr.push_back(SE2013_Low[i]);
                        break;
                    default:
                        Lumi_distr.push_back(SE2013_Central[i]);
                        break;
                }

            }

            else if (leptonFlavor == "DMu" || leptonFlavor == "DE"){
                switch (mode){
                    case 0:
                        Lumi_distr.push_back(DMu2013_Central[i]);
                        break;
                    case 1:
                        Lumi_distr.push_back(DMu2013_High[i]);
                        break;
                    case -1:
                        Lumi_distr.push_back(DMu2013_Low[i]);
                        break;
                    default:
                        Lumi_distr.push_back(DMu2013_Central[i]);
                        break;
                }
            }
            MC_distr.push_back(Summer2012_S10[i]);
        }	
    } // end of loop over bins

    if( MC_distr.size() != Lumi_distr.size() ){   
        std::cout << "MC_distr.size() = " << MC_distr.size() << std::endl;
        std::cout << "Lumi_distr.size() = " << Lumi_distr.size() << std::endl;
        std::cerr <<"ERROR: standalone_LumiReWeighting: input vectors have different sizes. Quitting... \n";
    }

    weights_ = new TH1F(Form("luminumer_%d",mode),
            Form("luminumer_%d",mode),
            NBins,0., float(NBins));

    TH1F* den = new TH1F(Form("lumidenom_%d",mode),
            Form("lumidenom_%d",mode),
            NBins,0., float(NBins));

    for(int ibin = 1; ibin<NBins+1; ++ibin ) {
        weights_->SetBinContent(ibin, Lumi_distr[ibin-1]);
        den->SetBinContent(ibin,MC_distr[ibin-1]);
    }

    // check integrals, make sure things are normalized

    float deltaH = weights_->Integral();
    if(fabs(1.0 - deltaH) > 0.02 ) { //*OOPS*...
        weights_->Scale( 1.0/ weights_->Integral() );
    }
    float deltaMC = den->Integral();
    if(fabs(1.0 - deltaMC) > 0.02 ) {
        den->Scale(1.0/ den->Integral());
    }

    weights_->Divide( den );  // so now the average weight should be 1.0    

    //std::cout << "Reweighting: Computed Weights per In-Time Nint " << std::endl;



}

standalone_LumiReWeighting::~standalone_LumiReWeighting()
{
}

double standalone_LumiReWeighting::weight( int npv ) {
    int bin = weights_->GetXaxis()->FindBin( npv );
    return weights_->GetBinContent( bin );
}

