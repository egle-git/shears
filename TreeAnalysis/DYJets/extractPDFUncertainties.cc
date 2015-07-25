#include <iostream>
#include <sstream>
#include <TH1.h>
#include <TCanvas.h>
#include <TFile.h>
#include "variablesOfInterestVarWidth.h"

using namespace std;

void extractPDFUncertainties()
{
    //--- open all the files: 0 to 52 for CT10 ---
    TFile *fCT10[53] = {NULL};
    for (int i(0); i < 53; i++) { 
        stringstream name; name << "HistoFiles/DE_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR_EffiCorr_0_TrigCorr_1_Syst_0_JetPtMin_30_VarWidth_PDF_CT10.LHgrid_";
        name << i << ".root";

        cout << name.str() << endl;
        fCT10[i] = new TFile(name.str().c_str());
    }
    TFile *fOutput = new TFile("TestPDF.root", "RECREATE");
    //--------------------------------------------

    for (int i(-1); i < NVAROFINTERESTZJETS; i++) {
        TH1D *h[53] = {NULL};
        string hName = "genZNGoodJets_Zinc";
        if (i > -1) {    
            cout << VAROFINTERESTZJETS[i].name << endl;
            hName = "gen" + VAROFINTERESTZJETS[i].name;
        }
        for (int j(0); j < 53; j++) {
            h[j] = (TH1D*) fCT10[j]->Get(hName.c_str());
        }
        //h[0]->DrawCopy();
        TH1D *hErrUp = (TH1D*) h[0]->Clone(string("Up_" + hName).c_str());
        TH1D *hErrDown = (TH1D*) h[0]->Clone(string("Down_" + hName).c_str());

        int nBins = h[0]->GetNbinsX() + 1;
        for (int j(0); j <= nBins; j++) {
            double central = h[0]->GetBinContent(j);
            double diffUp2 = 0;
            double diffDown2 = 0;
            for (int k(1); k <= 51; k += 2) { 
                double up = h[k]->GetBinContent(j);
                double down = h[k+1]->GetBinContent(j);

                diffUp2 += pow(max(max(up - central, down - central), 0.), 2);
                diffDown2 += pow(max(max(central - up, central - down), 0.), 2);

            }

            if (central > 0) {
                hErrUp->SetBinContent(j, sqrt(diffUp2)/(1.645*central));
                hErrDown->SetBinContent(j, sqrt(diffDown2)/(1.645*central));
            }
            else {
                hErrUp->SetBinContent(j, 0.);
                hErrDown->SetBinContent(j, 0.);
            }
            hErrUp->SetBinError(j, 0);
            hErrDown->SetBinError(j, 0);
            cout << sqrt(diffUp2)/(1.645*central) << "  " << sqrt(diffDown2)/(1.645*central) << endl;

        }
        fOutput->cd();
        hErrUp->Write();
        hErrDown->Write();

    }

    for (int i(0); i < 53; i++) {
        fCT10[i]->Close();
    }
    fOutput->Close();



}
