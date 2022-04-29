#ifndef standalone_LumiReWeighting_h
#define standalone_LumiReWeighting_h

#include <map>
#include <string>
#include <TString.h>

class TH1D;

/**
   \class    standalone_LumiReWeighting standalone_LumiReWeighting.h
   "PhysicsTools/Utilities/interface/standalone_LumiReWeighting.h"
   \brief    Class to provide lumi weighting for analyzers to weight "flat-to-N" MC samples to data
   This class will trivially take two histograms:
   1. The generated "flat-to-N" distributions from a given processing (or any other generated input)
   2. A histogram generated from the "estimatePileup" macro here:
   https://twiki.cern.ch/twiki/bin/view/CMS/LumiCalc#How_to_use_script_estimatePileup
   and produce weights to convert the input distribution (1) to the latter (2).
   \author Salvatore Rappoccio, modified by Mike Hildreth

*/

// 2022.04.28 (Kyeongpil): get "type" as one of the arguments to use various types of weights

class standalone_LumiReWeighting
{
  public:
    standalone_LumiReWeighting(TString type = "2016_preAPV",
                               int mode = 0,
                               int nBin = 99); // 0: central, -1: down, +1: up
    virtual ~standalone_LumiReWeighting();
    double weight(int npv);

    // Make a map to use for accessing different arrays using a string
    std::map<std::string, double *> pileupMap;

  protected:
    TH1D *weights_;
};

#endif
