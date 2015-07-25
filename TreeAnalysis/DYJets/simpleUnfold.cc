#include <iostream>
#include <TRandom.h>
#include <RooUnfoldResponse.h>
#include <RooUnfoldBayes.h>
#include <RooUnfoldSvd.h>
#include <RooUnfoldInvert.h>
#include <RooUnfoldBinByBin.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <vector>
#include <string>

void simpleUnfold()
{  
    cout << " in simpleUnfold " << endl;                                                                                                
    TFile *fout = new TFile("SimpleUnfold.root", "RECREATE");

    //string variable = "ZNGoodJets_Zexc";
    //string variable = "FirstJetPt_Zinc1jet";
    string variable = "FirstJetEta_Zinc1jet";
    //string variable = "SecondJetPt_2_Zinc2jet";
    //string variable = "JetsMass_Zinc2jet";
    //string variable = "tau_max_Zinc1jet_2";
    string genVariable = "gen" + variable; 
    string respVariable = "response" + variable;        
    string hrespVariable = "hresponse" + variable;        
    TFile *dataFile = new TFile("HistoFilesAugust/DMu_8TeV_Data_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");         
    TH1D *dataHist = (TH1D*) dataFile->Get(variable.c_str());           

    TFile *dyFile = new TFile("HistoFilesAugust/DMu_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    if (!dyFile) cout << "Problem with DY file" << endl;
    TH1D *genHist = (TH1D*) dyFile->Get(genVariable.c_str());           
    TH1D *mcHist = (TH1D*) dyFile->Get(variable.c_str());           
    TH2D *hresp = (TH2D*) dyFile->Get(hrespVariable.c_str());
    //RooUnfoldResponse *dyResp = (RooUnfoldResponse*) dyFile->Get(respVariable.c_str());
    //RooUnfoldResponse *dyResp = new RooUnfoldResponse(mcHist, genHist, hresp);
    //dyResp->UseOverflow();

    vector<string> bg;                                                                                                                  
    bg.push_back("HistoFilesAugust/DMu_8TeV_ZZJets2L2Nu_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    bg.push_back("HistoFilesAugust/DMu_8TeV_ZZJets2L2Q_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root"); 
    bg.push_back("HistoFilesAugust/DMu_8TeV_ZZJets4L_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");   
    bg.push_back("HistoFilesAugust/DMu_8TeV_WWJets2L2Nu_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");                 
    bg.push_back("HistoFilesAugust/DMu_8TeV_WZJets2L2Q_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");                  
    bg.push_back("HistoFilesAugust/DMu_8TeV_WZJets3LNu_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root"); 
    bg.push_back("HistoFilesAugust/DMu_8TeV_TTJets_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");     
    bg.push_back("HistoFilesAugust/DMu_8TeV_Top_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");        
    bg.push_back("HistoFilesAugust/DMu_8TeV_WJetsALL_MIX_UNFOLDING_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");      
    bg.push_back("HistoFilesAugust/DMu_8TeV_DYJetsToLL_FromTau_50toInf_UNFOLDING_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");      

    cout << "OK" << endl;


    int nBg = bg.size();           

    TH1D *bgHist = NULL;

    for (int i = 0; i < nBg; i++) {
        TFile *bgFile = new TFile(bg[i].c_str());       
        TH1D *tmp = (TH1D*) bgFile->Get(variable.c_str());              
        if (i == 0) {                                                                                                                   
            bgHist = (TH1D*) tmp->Clone();                                                                                              
            bgHist->SetDirectory(0);
        }                                                                                                                               
        else {
            bgHist->Add(tmp);      
        }
        bgFile->Close();                                                                                                                
    }

    TH1D *hRecDYPlusBg = (TH1D*) bgHist->Clone();
    hRecDYPlusBg->Add(mcHist);
    RooUnfoldResponse *dyResp = new RooUnfoldResponse(hRecDYPlusBg, genHist, hresp, true);
    //dyResp->UseOverflow();

    //bgHist->Draw("text");
    //dataHist->DrawCopy("text");     
    //dataHist->Add(bgHist, -1);     

    RooUnfoldBayes *unfold = new RooUnfoldBayes(dyResp, dataHist, 4);
    //RooUnfoldSvd *unfold = new RooUnfoldSvd(dyResp, dataHist, 6);
    //RooUnfoldInvert *unfold = new RooUnfoldInvert(dyResp, dataHist);
    //RooUnfoldBinByBin *unfold = new RooUnfoldBinByBin(dyResp, dataHist);
    TH1D *dataUnfolded = (TH1D*) unfold->Hreco();       
    //TH1D *chi2 = (TH1D*) unfold->Chi2OfChange;
    //chi2->Draw();

    cout << dataUnfolded->GetBinContent(2)/19584. << endl;
    dataUnfolded->Scale(1./19584);
    dataHist->Scale(1./19584.);
    genHist->Scale(1./19584.);
    mcHist->Scale(1./19584.);
    dataHist->Divide(dataUnfolded);
    mcHist->Divide(genHist);
    //genHist->Divide(dataUnfolded);
    
    dataUnfolded->Draw("e");    
    dataHist->SetLineStyle(kDashed);    
    mcHist->SetLineStyle(kDashed);    
    dataHist->Draw("esame");    
    genHist->Draw("samehist");    
    mcHist->Draw("samehist");

    int nBins = dataUnfolded->GetNbinsX();
    cout << endl;
    for (int i(1); i <= nBins; i++) {
        cout << " --- " << dataUnfolded->GetBinContent(i)*1.0 <<  "  "  << dataUnfolded->GetBinError(i)*1.0/dataUnfolded->GetBinWidth(i) << endl;
        cout << dataUnfolded->GetBinContent(i)*1.0/dataUnfolded->GetBinWidth(i) <<  "  "  << dataUnfolded->GetBinError(i)*1.0/dataUnfolded->GetBinWidth(i) << endl;
    }
    cout << endl;
    fout->Close();

}  
