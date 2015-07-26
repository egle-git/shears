#include <iostream>
#include "BonzaiMaker.h"
#include "ArgParser.h"
#include "ConfigVJets.h"

using namespace std;

int main(int argc, char** argv)
{

    gROOT->ProcessLine("#include <vector>");
    ConfigVJets cfg;
  
  /*
    TString txtFile = cfg.getS("txtFile");
    TString lepSel = cfg.getS("lepSel");
    TString doWhat = cfg.getS("doWhat");
    TString anaSel = cfg.getS("anaSel");*/


    //--- Parse the arguments -----------------------------------------------------
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            TString currentArg = argv[i];
            if (currentArg.Contains("help") || currentArg.BeginsWith("-h")) {
                std::cout << "\nUsage: \n\t./runMyTree [anaSel=(DYJETS, WJETS, Wc)] [lepSel=(DMu, DE, SMu, SE, SMuSE)] [txtFile=("")]  [doWhat=(DATA, MC)] [outputDir=(local,eos)] [outputDir=(local,eos)] [maxNEvent=MAX or any number]  [--help]" << std::endl;
                std::cout << "\neg: ./runMyTree anaSel=DYJETS lepSel=DMu txtFile=data/DYJetsToLL_data_v5.txt doWhat=DATA outputDir=local maxNEvent=50" << std::endl;
                std::cout << "\nunspecified options will be read from vjets.cfg\n" << std::endl;
                return 0;
            }
            else if (currentArg.Contains("=")) {
	            TString key = currentArg(0, currentArg.Index("="));
                    TString val = currentArg(currentArg.Index("=")+1, currentArg.Length());

                  //  cout << key << " , " << val << "\n"; 
		
                    if (key != "" && val != "") cfg.set(key.Data(), val.Data()); 
		    else {
			std::cout << "Something wrong with arg " << currentArg << std::endl;
			return 0;
		    }
	    }
            //--- bad option ---
            else {
                std::cerr << "Warning: unknown option \"" << currentArg << "\"" << std::endl;
                std::cerr << "Please issue ./runMyTree --help for more information on possible options" << std::endl;
                return 0;
            }
          

            //--- possible options ---
            /*if (currentArg.BeginsWith("txtFile=")) {
                getArg(currentArg, txtFile);
            }
            else if (currentArg.BeginsWith("lepSel=")) {
                getArg(currentArg, lepSel);
            }
            else if (currentArg.BeginsWith("anaSel=")) {
                getArg(currentArg, anaSel);
            }
            else if (currentArg.BeginsWith("doWhat=")) {
                getArg(currentArg, doWhat);
            }*/
            //--- asking for help ---

          
         }
     }

  BonzaiMaker t(cfg);//, txtFile, anaSel, lepSel, doWhat);
  t.Loop();


  return 0;

}

