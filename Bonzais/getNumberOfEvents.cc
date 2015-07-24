#include <iostream>
#include <fstream>

#include <TFile.h>
#include <TH1.h>

using namespace std;

void NumberOfEvents(string fileName);

void getNumberOfEvents()
{
    NumberOfEvents("MC8TeV_Tbar_s_channel");
    NumberOfEvents("MC8TeV_Tbar_t_channel");
    NumberOfEvents("MC8TeV_Tbar_tW_channel");
    NumberOfEvents("MC8TeV_T_s_channel");
    NumberOfEvents("MC8TeV_T_t_channel");
    NumberOfEvents("MC8TeV_T_tW_channel");
}


void NumberOfEvents(string fileName)
{

    string storageElement = "dcap://maite.iihe.ac.be:/pnfs/iihe/cms/store/user/aleonard/VJets_ntuples/";
    string line;
    ifstream myfile (string(fileName + ".txt").c_str());
    if (myfile.is_open())
    {
        long totalEntries = 0;
        while ( getline (myfile,line) )
        {
            cout << line << '\n';

            string path = storageElement + fileName + "/" + line;
            TFile *tmpFile = TFile::Open(path.c_str());
            tmpFile->cd("dataAnalyzer");
            TH1D *tmpHist = (TH1D*) gDirectory->Get("cutflow");
    
            cout << totalEntries  << " +=  " << tmpHist->GetBinContent(1) << endl;
            totalEntries += tmpHist->GetBinContent(1);

        }
        cout << "Total number of entries: " << totalEntries << endl << endl;
        myfile.close();
    }
    else cout << "Unable to open file"; 


}
