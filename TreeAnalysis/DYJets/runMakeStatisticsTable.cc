#include <iostream>
#include <TROOT.h>
#include <TString.h>
#include "ArgParser.h"
#include "ConfigVJets.h"
#include "makeStatisticsTable.h"

int main(int argc, char **argv)
{

    //--- Loads configuration -----------------------------------------------------
    ConfigVJets cfg;

    TString histoDir    = cfg.getS("histoDir");
    TString recoCompDir = cfg.getS("recoCompDir");
    TString lepSel      = cfg.getS("lepSel");
    int jetPtMin        = cfg.getI("jetPtMin");
    int jetEtaMax       = cfg.getI("jetEtaMax");
    
    //-----------------------------------------------------------------------------

    //--- Parse the arguments -----------------------------------------------------
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            TString currentArg = argv[i];
            //--- possible options ---
            if (currentArg.BeginsWith("histoDir=")) {
                getArg(currentArg, histoDir);
            }
            else if (currentArg.BeginsWith("recoCompDir=")) {
                getArg(currentArg, recoCompDir);
            }
            else if (currentArg.BeginsWith("lepSel=")) {
                getArg(currentArg, lepSel);
            }
            else if (currentArg.BeginsWith("jetPtMin=")) {
                getArg(currentArg, jetPtMin);
            }
            else if (currentArg.BeginsWith("jetEtaMax=")) {
                getArg(currentArg, jetEtaMax);
            }
            //--- asking for help ---
            else if (currentArg.Contains("help") || currentArg.BeginsWith("-h")) {
                std::cout << "\nUsage: \n\t./runRecoComparison [lepSel=(DMu, DE)] [jetPtMin=(int)] [jetEtaMax=(int*10)] [histoDir=(path)] [recoCompDir=(path)] [--help]" << std::endl;
                std::cout << "\neg: ./runRecoComparison lepSel=DMu jetEtaMax=24" << std::endl;
                std::cout << "\nunspecified options will be read from vjets.cfg\n" << std::endl;
                return 0;
            }
            //--- bad option ---
            else {
                std::cerr << "Warning: unknown option \"" << currentArg << "\"" << std::endl;
                std::cerr << "Please issue ./runRecoComparison --help for more information on possible options" << std::endl;
                return 0;
            }
        }
    }
    std::cout << "\n";
    
    if (!histoDir.EndsWith("/")) histoDir += "/";
    if (!recoCompDir.EndsWith("/")) recoCompDir += "/";

    makeStatisticsTable(lepSel, histoDir);


    return 0;
}
