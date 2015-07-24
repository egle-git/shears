#include <iostream>
#include <TString.h>
#include "ConfigVJets.h"
#include "FinalUnfoldZJets.h"

using namespace std;

int main(int argc, char **argv)
{
    TString srcDir = "Sources/";
    //vector<TString> sources;
    //sources.push_back(srcDir + "getFilesAndHistogramsZJets.cc+");
    //sources.push_back(srcDir + "functions.cc+");
    //sources.push_back(srcDir + "writeFastPlotsTex.cc+");
    //sources.push_back(srcDir + "unfoldingFunctions.cc+");
    //sources.push_back(srcDir + "UnfoldingSyst.cc+");
    //sources.push_back(srcDir + "FinalUnfoldZJets.cc+");

    ////--- Load shaared libraries ---
    //unsigned int nSources = sources.size();
    //for (unsigned int i(0); i < nSources; i++){
    //    cout << "Compiling " << sources[i] << endl;
    //    gROOT->ProcessLine(".L " + sources[i]);
    //}

    FinalUnfold(0, 1);


    return 0;
}
