#include <iostream>
#include <fstream>
#include <makeStatisticsTable.h>
#include <TString.h>
#include <TH1D.h>
#include <TFile.h>
#include <vector>

using namespace std;


int makeStatisticsTable(TString lepSel, TString histoDir)
{


    vector<TString> fileNames;
    fileNames.push_back(histoDir + lepSel + "_8TeV_WJetsALL_MIX_UNFOLDING_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    fileNames.push_back(histoDir + lepSel + "_8TeV_DYJetsToLL_FromTau_50toInf_UNFOLDING_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    fileNames.push_back(histoDir + lepSel + "_8TeV_ZZJets4L_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    fileNames.push_back(histoDir + lepSel + "_8TeV_Top_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    fileNames.push_back(histoDir + lepSel + "_8TeV_ZZJets2L2Nu_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    fileNames.push_back(histoDir + lepSel + "_8TeV_ZZJets2L2Q_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    fileNames.push_back(histoDir + lepSel + "_8TeV_WWJets2L2Nu_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    fileNames.push_back(histoDir + lepSel + "_8TeV_WZJets2L2Q_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    fileNames.push_back(histoDir + lepSel + "_8TeV_WZJets3LNu_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    fileNames.push_back(histoDir + lepSel + "_8TeV_TTJets_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    fileNames.push_back(histoDir + lepSel + "_8TeV_DYJetsToLL_MIX_50toInf_UNFOLDING_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");
    fileNames.push_back(histoDir + lepSel + "_8TeV_Data_dR_TrigCorr_1_Syst_0_JetPtMin_30_JetEtaMax_24.root");

    vector<TString> shortNames = {"WJets", "DYTauTau", "ZZJets4L", "Single Top", "ZZJets2L2Nu", "ZZJets2L2Q", "WWJets2L2Nu", "WZJets2L2Q", "WZJets3LNu", "TTJets", "DYJets", "Data"};

    vector<double> totalMC (9, 0.0);
    vector<double> data (9, 0.0);

    TString histoName = "ZNGoodJets_Zexc";
    TString table = "\\begin{table}[htb!]\n\\begin{center}\n";
    table += "\\caption{Number of events in data and MC samples for exclusive jet multiplicity in ";
    if (lepSel == "DMu") table += "muon channel after full selection.}\n";
    if (lepSel == "DE") table += "electron channel after full selection.}\n";

    table += "\\footnotesize{\n\\begin{tabular}{l|cccccccc}\n";
    table += "& $N_{\\text{jets}} = 0 $ & $N_{\\text{jets}} = 1 $ & $N_{\\text{jets}} = 2 $ & $N_{\\text{jets}} = 3 $ ";
    table += "& $N_{\\text{jets}} = 4 $ & $N_{\\text{jets}} = 5 $ & $N_{\\text{jets}} = 6 $ & $N_{\\text{jets}} = 7$ \\\\ \\hline\n";



    for (int i(0); i < 11; ++i) {
        TString fileName = fileNames[i];
        TFile *tmpFile = new TFile(fileName);
        TH1D *tmpHist = (TH1D*) tmpFile->Get(histoName);
        table += shortNames[i] + "  ";
        for (int j(1); j < 9; ++j) {
            double tmpContent(tmpHist->GetBinContent(j));
            totalMC[j-1] += tmpContent;
            TString tmpStr;
            tmpStr.Form("%.1f", tmpContent);
            table += " & " + tmpStr + "  ";

        }
        table += "\\\\ \n ";
        tmpFile->Close();
    }

    table += "\\hline \n";
    table += "TOTAL ";
    for (int k(0); k < 8; ++k) {
        TString tmpStr;
        tmpStr.Form("%.1f", totalMC[k]);
        table +=  " & " + tmpStr + "  ";
    }
    table += "\\\\ \\hline \n";
    TString fileName = fileNames[11];
    TFile *tmpFile = new TFile(fileName);
    TH1D *tmpHist = (TH1D*) tmpFile->Get(histoName);
    table += shortNames[11] + "  ";
    for (int j(1); j < 9; ++j) {
        double tmpContent(tmpHist->GetBinContent(j));
        TString tmpStr;
        tmpStr.Form("%d", (int)tmpContent);
        table +=  " & " + tmpStr + "  ";
        data[j-1] += tmpContent;
    }
    tmpFile->Close();


    table += "\\\\ \n Ratio ";
    for (int i(0); i < 8; ++i) {
        TString tmpStr;
        tmpStr.Form("%.4f", totalMC[i]/data[i]);
        table += " & " + tmpStr + " " ; 
    }
    if (lepSel=="DMu")table += "\n\\end{tabular}}\n\\label{tab:MuEventsMultiplicity}\n\\end{center}\n\\end{table}";
    if (lepSel=="DE") table += "\n\\end{tabular}}\n\\label{tab:EleEventsMultiplicity}\n\\end{center}\n\\end{table}";
    cout << table << endl;

    //--- create output directory if does not exist ---
    TString outputFileName = histoDir + lepSel + "_table";
    ofstream out(outputFileName + ".tex");
    out << table;
    out.close();
    return 0;
}
