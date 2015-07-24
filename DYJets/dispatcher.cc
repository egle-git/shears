#include <iostream>
#include <thread>
#include <unistd.h>
#include "ArgParser.h"

TString cwd;
void executeInThread(std::string executable, std::string option, std::string machine) 
{
    std::string command = executable + " " + option;
    if (machine != "") command = "ssh -o StrictHostKeyChecking=no " + machine + " \'source .bash_profile; cd " + cwd + "; " + command + "\'";  
    std::cout << command << std::endl;
    system(command.c_str());
}

int main(int argc, char **argv)
{

    bool doHisto = true;
    bool doRecoComp = true;
    bool doUnfold = true;
    bool doCombination = true;
    bool doCopyToMac = false;
    std::string machines[] = {"m5.iihe.ac.be", "m6.iihe.ac.be", "m7.iihe.ac.be", "m8.iihe.ac.be", "m9.iihe.ac.be"};
    char *pcwd = getcwd( NULL, 0 ); 
    cwd = TString(pcwd);
    free(pcwd);

    //--- Parse the arguments -----------------------------------------------------
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            TString currentArg = argv[i];
            //--- possible options ---
            if (currentArg.BeginsWith("doHisto=")) {
                getArg(currentArg, doHisto);
            }
            else if (currentArg.BeginsWith("doRecoComp=")) {
                getArg(currentArg, doRecoComp);
            }
            else if (currentArg.BeginsWith("doUnfold=")) {
                getArg(currentArg, doUnfold);
            }
            else if (currentArg.BeginsWith("doCombination=")) {
                getArg(currentArg, doCombination);
            }
            else if (currentArg.BeginsWith("doCopyToMac=")) {
                getArg(currentArg, doCopyToMac);
            }
            else if (currentArg.BeginsWith("lxplus")) {
                machines[0] = "lxplus0159.cern.ch";
                machines[1] = "lxplus0138.cern.ch";
                machines[2] = "lxplus0139.cern.ch";
                machines[3] = "lxplus0149.cern.ch";
                machines[4] = "lxplus0171.cern.ch";
            }
            //--- asking for help ---
            else if (currentArg.Contains("help") || currentArg.BeginsWith("-h")) {
                std::cout << "\nUsage: ./dispatcher [doHisto=(1,0)] [doRecoComp=(1, 0)] [doUnfold=(1, 0)] [doCombination=(1, 0)] [doCopyToMac=(0, 1)] ";
                std::cout << "[lxplus] [--help]" << std::endl;
                std::cout << "eg: ./dispatcher doHisto=0" << std::endl;
                std::cout << "unspecified options will be read from vjets.cfg\n" << std::endl;
                return 0;
            }
            //--- bad option ---
            else {
                std::cerr << "Warning: unknown option \"" << currentArg << "\"" << std::endl;
                std::cerr << "Please issue ./dispatcher -help for more information on possible options" << std::endl;
                return 0;
            }
        }
    }

    auto start = std::chrono::system_clock::now();
    std::cout << "Starting to dispatch" << std::endl;

    if (doHisto) {
        //--- Generate the histofiles ---
        std::thread DataThread[6];
        std::thread DYJetsThread[18];
        std::thread BGThread[18];
        std::thread TAUThread[18];
        std::thread WJETSThread[18];
        std::thread AMCATNLO[2];
        std::thread SHERPA14[2];
        std::thread SHERPA2[40];

        //--- first execute the DYJets ---
        for (int i = 0; i < 2; ++i) {
            std::string lep = "DE";
            if (i == 1) lep = "DMu";

            DataThread[i*3 + 0] = std::thread(executeInThread, "./runZJets", "doWhat=DATA lepSel=" + lep + " whichSyst=0", machines[0]);
            DataThread[i*3 + 1] = std::thread(executeInThread, "./runZJets", "doWhat=DATA lepSel=" + lep + " whichSyst=1", machines[1]);
            DataThread[i*3 + 2] = std::thread(executeInThread, "./runZJets", "doWhat=DATA lepSel=" + lep + " whichSyst=2", machines[2]);

            DYJetsThread[i*9 + 0] = std::thread(executeInThread, "./runZJets", "doWhat=DYJETS lepSel=" + lep + " whichSyst=0", machines[0]);
            DYJetsThread[i*9 + 1] = std::thread(executeInThread, "./runZJets", "doWhat=DYJETS lepSel=" + lep + " whichSyst=1", machines[1]);
            DYJetsThread[i*9 + 2] = std::thread(executeInThread, "./runZJets", "doWhat=DYJETS lepSel=" + lep + " whichSyst=2", machines[2]);
            DYJetsThread[i*9 + 3] = std::thread(executeInThread, "./runZJets", "doWhat=DYJETS lepSel=" + lep + " whichSyst=3", machines[3]);
            DYJetsThread[i*9 + 4] = std::thread(executeInThread, "./runZJets", "doWhat=DYJETS lepSel=" + lep + " whichSyst=4", machines[4]);
            DYJetsThread[i*9 + 5] = std::thread(executeInThread, "./runZJets", "doWhat=DYJETS lepSel=" + lep + " whichSyst=5", machines[0]);
            DYJetsThread[i*9 + 6] = std::thread(executeInThread, "./runZJets", "doWhat=DYJETS lepSel=" + lep + " whichSyst=6", machines[1]);
            DYJetsThread[i*9 + 7] = std::thread(executeInThread, "./runZJets", "doWhat=DYJETS lepSel=" + lep + " whichSyst=7", machines[2]);
            DYJetsThread[i*9 + 8] = std::thread(executeInThread, "./runZJets", "doWhat=DYJETS lepSel=" + lep + " whichSyst=8", machines[3]);

            BGThread[i*9 + 0] = std::thread(executeInThread, "./runZJets", "doWhat=BACKGROUND lepSel=" + lep + " whichSyst=0", machines[0]);
            BGThread[i*9 + 1] = std::thread(executeInThread, "./runZJets", "doWhat=BACKGROUND lepSel=" + lep + " whichSyst=1", machines[1]);
            BGThread[i*9 + 2] = std::thread(executeInThread, "./runZJets", "doWhat=BACKGROUND lepSel=" + lep + " whichSyst=2", machines[2]);
            BGThread[i*9 + 3] = std::thread(executeInThread, "./runZJets", "doWhat=BACKGROUND lepSel=" + lep + " whichSyst=3", machines[3]);
            BGThread[i*9 + 4] = std::thread(executeInThread, "./runZJets", "doWhat=BACKGROUND lepSel=" + lep + " whichSyst=4", machines[4]);
            BGThread[i*9 + 5] = std::thread(executeInThread, "./runZJets", "doWhat=BACKGROUND lepSel=" + lep + " whichSyst=5", machines[0]);
            BGThread[i*9 + 6] = std::thread(executeInThread, "./runZJets", "doWhat=BACKGROUND lepSel=" + lep + " whichSyst=6", machines[1]);
            BGThread[i*9 + 7] = std::thread(executeInThread, "./runZJets", "doWhat=BACKGROUND lepSel=" + lep + " whichSyst=7", machines[2]);
            BGThread[i*9 + 8] = std::thread(executeInThread, "./runZJets", "doWhat=BACKGROUND lepSel=" + lep + " whichSyst=8", machines[3]);

            TAUThread[i*9 + 0] = std::thread(executeInThread, "./runZJets", "doWhat=TAU lepSel=" + lep + " whichSyst=0", machines[0]);
            TAUThread[i*9 + 1] = std::thread(executeInThread, "./runZJets", "doWhat=TAU lepSel=" + lep + " whichSyst=1", machines[1]);
            TAUThread[i*9 + 2] = std::thread(executeInThread, "./runZJets", "doWhat=TAU lepSel=" + lep + " whichSyst=2", machines[2]);
            TAUThread[i*9 + 3] = std::thread(executeInThread, "./runZJets", "doWhat=TAU lepSel=" + lep + " whichSyst=3", machines[3]);
            TAUThread[i*9 + 4] = std::thread(executeInThread, "./runZJets", "doWhat=TAU lepSel=" + lep + " whichSyst=4", machines[4]);
            TAUThread[i*9 + 5] = std::thread(executeInThread, "./runZJets", "doWhat=TAU lepSel=" + lep + " whichSyst=5", machines[0]);
            TAUThread[i*9 + 6] = std::thread(executeInThread, "./runZJets", "doWhat=TAU lepSel=" + lep + " whichSyst=6", machines[1]);
            TAUThread[i*9 + 7] = std::thread(executeInThread, "./runZJets", "doWhat=TAU lepSel=" + lep + " whichSyst=7", machines[2]);
            TAUThread[i*9 + 8] = std::thread(executeInThread, "./runZJets", "doWhat=TAU lepSel=" + lep + " whichSyst=8", machines[3]);

            WJETSThread[i*9 + 0] = std::thread(executeInThread, "./runZJets", "doWhat=WJETS lepSel=" + lep + " whichSyst=0", machines[0]);
            WJETSThread[i*9 + 1] = std::thread(executeInThread, "./runZJets", "doWhat=WJETS lepSel=" + lep + " whichSyst=1", machines[1]);
            WJETSThread[i*9 + 2] = std::thread(executeInThread, "./runZJets", "doWhat=WJETS lepSel=" + lep + " whichSyst=2", machines[2]);
            WJETSThread[i*9 + 3] = std::thread(executeInThread, "./runZJets", "doWhat=WJETS lepSel=" + lep + " whichSyst=3", machines[3]);
            WJETSThread[i*9 + 4] = std::thread(executeInThread, "./runZJets", "doWhat=WJETS lepSel=" + lep + " whichSyst=4", machines[4]);
            WJETSThread[i*9 + 5] = std::thread(executeInThread, "./runZJets", "doWhat=WJETS lepSel=" + lep + " whichSyst=5", machines[0]);
            WJETSThread[i*9 + 6] = std::thread(executeInThread, "./runZJets", "doWhat=WJETS lepSel=" + lep + " whichSyst=6", machines[1]);
            WJETSThread[i*9 + 7] = std::thread(executeInThread, "./runZJets", "doWhat=WJETS lepSel=" + lep + " whichSyst=7", machines[2]);
            WJETSThread[i*9 + 8] = std::thread(executeInThread, "./runZJets", "doWhat=WJETS lepSel=" + lep + " whichSyst=8", machines[3]);

            AMCATNLO[i] = std::thread(executeInThread, "./runZJets", "doWhat=AMCATNLO lepSel=" + lep + " whichSyst=0", machines[3]);
            SHERPA14[i] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA lepSel=" + lep + " whichSyst=0", machines[4]);
        }

        //Join the data threads with the main thread
        for (int i = 0; i < 6; ++i) {
            DataThread[i].join();
        }

        //Join the threads with the main thread
        for (int i = 0; i < 18; ++i) {
            DYJetsThread[i].join();
            BGThread[i].join();
            TAUThread[i].join();
            WJETSThread[i].join();
        }

        for (int i = 0; i < 2; ++i) {
            std::string lep = "DE";
            if (i == 1) lep = "DMu";
            SHERPA2[i*20 + 0] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_500 lepSel="   + lep + " whichSyst=0", machines[0]);
            SHERPA2[i*20 + 1] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_1000 lepSel="  + lep + " whichSyst=0", machines[1]);
            SHERPA2[i*20 + 2] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_1500 lepSel="  + lep + " whichSyst=0", machines[2]);
            SHERPA2[i*20 + 3] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_2000 lepSel="  + lep + " whichSyst=0", machines[3]);
            SHERPA2[i*20 + 4] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_2500 lepSel="  + lep + " whichSyst=0", machines[4]);
            SHERPA2[i*20 + 5] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_3000 lepSel="  + lep + " whichSyst=0", machines[0]);
            SHERPA2[i*20 + 6] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_3500 lepSel="  + lep + " whichSyst=0", machines[1]);
            SHERPA2[i*20 + 7] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_4000 lepSel="  + lep + " whichSyst=0", machines[2]);
            SHERPA2[i*20 + 8] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_4500 lepSel="  + lep + " whichSyst=0", machines[3]);
            SHERPA2[i*20 + 9] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_11000 lepSel=" + lep + " whichSyst=0", machines[4]);
            SHERPA2[i*20 +10] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_11500 lepSel=" + lep + " whichSyst=0", machines[0]);
            SHERPA2[i*20 +11] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_12000 lepSel=" + lep + " whichSyst=0", machines[1]);
            SHERPA2[i*20 +12] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_12500 lepSel=" + lep + " whichSyst=0", machines[2]);
            SHERPA2[i*20 +13] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_13000 lepSel=" + lep + " whichSyst=0", machines[3]);
            SHERPA2[i*20 +14] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_13500 lepSel=" + lep + " whichSyst=0", machines[4]);
            SHERPA2[i*20 +15] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_14000 lepSel=" + lep + " whichSyst=0", machines[0]);
            SHERPA2[i*20 +16] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_14500 lepSel=" + lep + " whichSyst=0", machines[1]);
            SHERPA2[i*20 +17] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_15000 lepSel=" + lep + " whichSyst=0", machines[2]);
            SHERPA2[i*20 +18] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_15500 lepSel=" + lep + " whichSyst=0", machines[3]);
            SHERPA2[i*20 +19] = std::thread(executeInThread, "./runZJets", "doWhat=SHERPA2_16000 lepSel=" + lep + " whichSyst=0", machines[4]);
        }

        for (int i = 0; i < 40; ++i) {
            SHERPA2[i].join();
        }
        

        for (int i = 0; i < 2; ++i) {
            AMCATNLO[i].join();
            SHERPA14[i].join();
        }


        std::cout << "All jobs done" << std::endl;
    }

    if (doRecoComp) {

        //--- Now proceed to reco comparison ---
        std::thread recoComp[4];
        for (int i = 0; i < 2; ++i) {
            std::string lep = "DE";
            if (i == 1) lep = "DMu";
            recoComp[2*i + 0] = std::thread(executeInThread, "./runRecoComparison", "doPASPlots=1 lepSel=" + lep, "");
            recoComp[2*i + 1] = std::thread(executeInThread, "./runRecoComparison", "doPASPlots=0 lepSel=" + lep, "");
        }

        //Join the reco threads with the main thread
        for (int i = 0; i < 4; ++i) {
            recoComp[i].join();
        }
    }

    if (doUnfold) {
        //--- Now proceed to reco comparison ---
        std::thread unfolding[2];
        for (int i = 0; i < 2; ++i) {
            std::string lep = "DE";
            if (i == 1) lep = "DMu";
            unfolding[i] = std::thread(executeInThread, "./runUnfoldingZJets", "lepSel=" + lep, "");
        }

        //Join the reco threads with the main thread
        for (int i = 0; i < 2; ++i) {
            unfolding[i].join();
        }
    }

    if (doCombination) {
        executeInThread("./runCombination", "", "");
    }

    if (doCopyToMac) {

    }

    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << elapsed.count() << " ms\n";

    return 0;
}
