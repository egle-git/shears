#include <iostream>
#include <thread>
#include <TString.h>
#include <TChain.h>
#include "Includes/ArgParser.h"
#include "Includes/EventTree.h"
#include "Includes/ObjectSelection.h"

using namespace std;

int main(int argc, char **argv)
{
TString InputFile = "../Bonzais/Pruner/test.root";
  TString OutputFile = "theOutputFile.root";
  TString Lepton = "";

if (argc<2){
	cout<<"Warning : Lack of args!"<<endl;
	cout<< "usage:"<<endl;
	cout<<"./maketptree InputFile=\"blablain.root\" OutputFile=\"blablaout.root\" Lepton=Mu(or El)"<<endl;
 exit(0);
}
  //--- Parse the arguments -----------------------------------------------------
  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
        TString currentArg = argv[i];
        //--- possible options ---
        if (currentArg.BeginsWith("InputFile=")) {
            getArg(currentArg, InputFile);
        }
        else if (currentArg.BeginsWith("OutputFile=")) {
            getArg(currentArg, OutputFile);
        }
        else if (currentArg.BeginsWith("Lepton=")) {
            getArg(currentArg, Lepton);
        }
    }
  }

EventTree mytree(InputFile, OutputFile, Lepton);
mytree.Loop();
return 0;

}
