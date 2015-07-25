#include <iostream>
#include "ArgParser.h"
#include "ConfigVJets.h"
#include "Combination.h"


int main(int argc, char **argv)
{
    
    //--- Load configuration ---
    ConfigVJets cfg;
    //-----------------------------------------------------------------------

    //--- Settings ---
    // doWhat = "DATA", "BACKGROUND", "TAU", "DYJETS", 
    //          "WJETS", "ALL", "PDF", "SHERPA"

    TString unfoldDir  = cfg.getS("unfoldDir");
    TString combDir    = cfg.getS("combDir");
    TString algo       = cfg.getS("algo");
    int jetPtMin       = cfg.getI("jetPtMin", 30);
    int jetEtaMax      = cfg.getI("jetEtaMax", 24);
    bool diagXChanCov  = cfg.getB("diagXChanCov", false);
    bool fullXChanCov  = cfg.getB("fullXChanCov", false);
    bool fullSChanCov  = cfg.getB("fullSChanCov", false);
    bool modifiedSWA   = cfg.getB("modifiedSWA", false);
    TString generator1 = cfg.getS("generator1", "sherpa2");
    TString generator2 = cfg.getS("generator2", "amcatnlo");

    TString variable = "";
    bool doNormalized(false);

    //--- Parse the arguments -----------------------------------------------------
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            TString currentArg = argv[i];
            //--- possible options ---
            if (currentArg.BeginsWith("unfoldDir=")) {
                getArg(currentArg, unfoldDir);
            }
            else if (currentArg.BeginsWith("combDir=")) {
                getArg(currentArg, combDir);
            }
            else if (currentArg.BeginsWith("algo=")) {
                getArg(currentArg, algo);
            }
            else if (currentArg.BeginsWith("generator1=")) {
                getArg(currentArg, generator1);
            }
            else if (currentArg.BeginsWith("generator2=")) {
                getArg(currentArg, generator2);
            }
            else if (currentArg.BeginsWith("jetPtMin=")) {
                getArg(currentArg, jetPtMin);
            }
            else if (currentArg.BeginsWith("jetEtaMax=")) {
                getArg(currentArg, jetEtaMax);
            }
            else if (currentArg.BeginsWith("diagXChanCov=")) {
                getArg(currentArg, diagXChanCov);
            }
            else if (currentArg.BeginsWith("fullXChanCov=")) {
                getArg(currentArg, fullXChanCov);
            }
            else if (currentArg.BeginsWith("fullSChanCov=")) {
                getArg(currentArg, fullSChanCov);
            }
            else if (currentArg.BeginsWith("modifiedSWA=")) {
                getArg(currentArg, modifiedSWA);
            }
            else if (currentArg.BeginsWith("variable=")) {
                getArg(currentArg, variable);
            }
            else if (currentArg.BeginsWith("doNormalized=")) {
                getArg(currentArg, doNormalized);
            }
            //--- asking for help ---
            else if (currentArg.Contains("help") || currentArg.BeginsWith("-h")) {
                std::cout << "\nUsage: ./runCombination [unfoldDir=(path)] [combDir=(path)] [algo=(Bayes, SVD)] [jetPtMin=(int)] [jetEtaMax=(int*10)] ";
                std::cout << "[diagXChanCov=(1,0)] [fullXChanCov=(1,0)] [fullSChanCov=(1,0)] [modifiedSWA=(1,0)] [variable=(variableName)] [doNormalized=(0, 1)] [--help]" << std::endl;
                std::cout << "eg: ./runCombination fullXChanCov=0 jetEtaMax=24" << std::endl;
                std::cout << "unspecified options will be read from vjets.cfg\n" << std::endl;
                return 0;
            }
            //--- bad option ---
            else {
                std::cerr << "Warning: unknown option \"" << currentArg << "\"" << std::endl;
                std::cerr << "Please issue ./runCombination --help for more information on possible options" << std::endl;
                return 0;
            }
        }
    }

    if (!unfoldDir.EndsWith("/")) unfoldDir += "/";
    if (!combDir.EndsWith("/")) combDir += "/";

    Combination(unfoldDir, combDir, algo, jetPtMin, jetEtaMax, diagXChanCov, fullXChanCov, fullSChanCov, modifiedSWA, generator1, generator2, variable, doNormalized);
    return 0;
}
