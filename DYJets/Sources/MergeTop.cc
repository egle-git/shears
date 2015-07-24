#include <iostream>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include <sstream>
#include <RooUnfoldResponse.h>
#include "MergeTop.h"

using namespace std;

void MergeTop(){
    // DMu 20 GeV
    //runMergeTop("DMu",0,20,0);
    //runMergeTop("DMu",1,20,0);
    //runMergeTop("DMu",-1,20,0);
    //runMergeTop("DMu",3,20,0);
    //runMergeTop("DMu",-3,20,0);	        

    // DMu 30 GeV
    //runMergeTop("DMu", 0, 30, 2.4, 0, "HistoFilesAugust");
    //runMergeTop("DMu",1,30,0);
    //runMergeTop("DMu",-1,30,0);
    //runMergeTop("DMu",3,30,0);
    //runMergeTop("DMu",-3,30,0);

    // DE 20 GeV
    //runMergeTop("DE",0,20,0);
    //runMergeTop("DE",1,20,0);
    //runMergeTop("DE",-1,20,0);
    //runMergeTop("DE",3,20,0);
    //runMergeTop("DE",-3,20,0);	        

    // DE 30 GeV
    //runMergeTop("DE",0,30,0);
    //runMergeTop("DE",1,30,0);
    //runMergeTop("DE",-1,30,0);
    //runMergeTop("DE",3,30,0);
    //runMergeTop("DE",-3,30,0);
}

void runMergeTop(TString lepSelection, int systematics, int lepPtCutMin, int lepEtaCutMax, int jetPtCutMin, int jetEtaCutMax, Long_t maxEvents, TString outDir)
{
    if (maxEvents >= 0) {
        outDir = "HistoFilesTest/";
        cout << "Doing test for 10000 events  => output directory has been changed to HistoFilesTest/" << endl;
    }

    TH1::SetDefaultSumw2();
    TH2::SetDefaultSumw2();

    cout << __FILE__ << endl;
    ostringstream strJetPtCutMin; strJetPtCutMin << jetPtCutMin;
    ostringstream strJetEtaCutMax; strJetEtaCutMax << jetEtaCutMax;
    TString syst;
    if (systematics == 0) syst = "Syst_0_";
    else if (systematics == 1) syst = "Syst_1_Up_"; 
    else if (systematics == -1) syst = "Syst_1_Down_"; 
    else if (systematics == 3) syst = "Syst_3_Up_"; 
    else if (systematics == -3) syst = "Syst_3_Down_"; 
    else if (systematics == 5) syst = "Syst_5_Up_"; 
    else if (systematics == -5) syst = "Syst_5_Down_"; 

    TString str1 = outDir + lepSelection +  "_8TeV_T_s_channel_dR_TrigCorr_1_" + syst + "JetPtMin_" + strJetPtCutMin.str() + "_JetEtaMax_" + strJetEtaCutMax.str() + ".root";
    TString str2 = outDir + lepSelection +  "_8TeV_T_t_channel_dR_TrigCorr_1_" + syst + "JetPtMin_" + strJetPtCutMin.str() + "_JetEtaMax_" + strJetEtaCutMax.str() + ".root";
    TString str3 = outDir + lepSelection +  "_8TeV_T_tW_channel_dR_TrigCorr_1_" + syst + "JetPtMin_" + strJetPtCutMin.str() + "_JetEtaMax_" + strJetEtaCutMax.str() + ".root";
    TString str4 = outDir + lepSelection +  "_8TeV_Tbar_s_channel_dR_TrigCorr_1_" + syst + "JetPtMin_" + strJetPtCutMin.str() + "_JetEtaMax_" + strJetEtaCutMax.str() + ".root";
    TString str5 = outDir + lepSelection +  "_8TeV_Tbar_t_channel_dR_TrigCorr_1_" + syst + "JetPtMin_" + strJetPtCutMin.str() + "_JetEtaMax_" + strJetEtaCutMax.str() + ".root";
    TString str6 = outDir + lepSelection +  "_8TeV_Tbar_tW_channel_dR_TrigCorr_1_" + syst + "JetPtMin_" + strJetPtCutMin.str() + "_JetEtaMax_" + strJetEtaCutMax.str() + ".root";
    TString strf = outDir + lepSelection +  "_8TeV_Top_dR_TrigCorr_1_" + syst + "JetPtMin_" + strJetPtCutMin.str() + "_JetEtaMax_" + strJetEtaCutMax.str() + ".root";
    cout << strf << endl;

    TFile *f1 = new TFile(str1);
    TFile *f2 = new TFile(str2);
    TFile *f3 = new TFile(str3);
    TFile *f4 = new TFile(str4);
    TFile *f5 = new TFile(str5);
    TFile *f6 = new TFile(str6);
    TFile *ff = new TFile(strf, "RECREATE");

    int nHist = f1->GetListOfKeys()->GetEntries();   
    for (int i(0); i < nHist; i++){
        TString hName = f1->GetListOfKeys()->At(i)->GetName();

        if (hName.Index("response") == 0){
            RooUnfoldResponse *r1 = (RooUnfoldResponse*) f1->Get(hName);
            RooUnfoldResponse *r2 = (RooUnfoldResponse*) f2->Get(hName);
            RooUnfoldResponse *r3 = (RooUnfoldResponse*) f3->Get(hName);
            RooUnfoldResponse *r4 = (RooUnfoldResponse*) f4->Get(hName);
            RooUnfoldResponse *r5 = (RooUnfoldResponse*) f5->Get(hName);
            RooUnfoldResponse *r6 = (RooUnfoldResponse*) f6->Get(hName);

            RooUnfoldResponse *rSum = (RooUnfoldResponse*) r1->Clone();
            rSum->Add(*r2);
            rSum->Add(*r3);
            rSum->Add(*r4);
            rSum->Add(*r5);
            rSum->Add(*r6);
            ff->cd();
            rSum->Write(hName);
        }
        else {
            TH1D *h1 = (TH1D*) f1->Get(hName); 
            TH1D *h2 = (TH1D*) f2->Get(hName); 
            TH1D *h3 = (TH1D*) f3->Get(hName); 
            TH1D *h4 = (TH1D*) f4->Get(hName); 
            TH1D *h5 = (TH1D*) f5->Get(hName); 
            TH1D *h6 = (TH1D*) f6->Get(hName); 

            TH1D *hSum = (TH1D*) h1->Clone();
            hSum->Add(h2);
            hSum->Add(h3);
            hSum->Add(h4);
            hSum->Add(h5);
            hSum->Add(h6);
            ff->cd();
            hSum->Write();
        }
    }

    f1->Close();
    f2->Close();
    f3->Close();
    f4->Close();
    f5->Close();
    f6->Close();
    ff->Close();

}
