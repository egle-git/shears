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
 bool sellep = 0;
if (argc<2){
	cout<<"Warning : Lack of args!"<<endl;
	cout<< "usage:"<<endl;
	cout<<"./maketptree --inputfile=blablain.root --outputfile=blablaout.root --lepton=muon(or electron)"<<endl;
 exit(0);
}
  //--- Parse the arguments -----------------------------------------------------
  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
        TString currentArg = argv[i];
        //--- possible options ---
	if(!currentArg.BeginsWith("--")){
	    cout<<"a command must contain arguments beginning with --"<<endl;
	    cout<<"usage:"<<endl;
	    cout<<"--input=test.root"<<endl;
	    cout<<"--output=out.root"<<endl;
	    cout<<"--lepton=muon"<<endl;
	    exit(0);
	}
        if (currentArg.BeginsWith("--input=")) {
            getArg(currentArg, InputFile);
        }
        else if (currentArg.BeginsWith("--output=")) {
            getArg(currentArg, OutputFile);
        }
        else if (currentArg.BeginsWith("--lepton=")) {
            getArg(currentArg, Lepton);
	    sellep = 1;
        }
    }
  }
if(!sellep){
	cout<<"You Must Select one kind of Lepton"<<endl;
	cout<<"Using \"--lepton=muon(or electron)\""<<endl;
	exit(0);
}
else if (!(Lepton=="electron"||Lepton=="muon")){
        cout<<"You Must Select one kind of Lepton"<<endl;
        cout<<"Using \"--lepton=muon(or electron)\""<<endl;
        exit(0);
}

EventTree mytree(InputFile, OutputFile, Lepton);
mytree.Loop();
return 0;

}
