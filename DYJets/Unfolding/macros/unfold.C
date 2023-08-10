#include "../include/Unfolding.hh"

void unfold(TString directory,TString channel)
{
    TH1::SetDefaultSumw2();
    TH2::SetDefaultSumw2();
    gROOT->SetBatch(true);
    gStyle->SetPalette(1);
    gStyle->SetOptStat(0);

    TString savePrefix = directory;
    savePrefix += "/";
    savePrefix += channel;
    savePrefix += "/plots/unfold";
    
    TString saveSuffix = "_";
    saveSuffix += channel;
    saveSuffix += ".png";

    TString file_name = directory;
    file_name += "/";
    file_name += channel;
    file_name += "/unfolding_histograms.root";
    TFile*loadFile = new TFile(file_name);

    TH1F*hReco   = (TH1F*)loadFile->Get("reco_mass");
    TH1F*hData   = (TH1F*)loadFile->Get("data");
    TH1F*hTrue   = (TH1F*)loadFile->Get("gen_mass");
    TH1F*hBack   = (TH1F*)loadFile->Get("backgrounds");
    TH2F*hMatrix = (TH2F*)loadFile->Get("matrix");

    // Closure test
    Unfold*unfClosure = new Unfold(hReco,hTrue,hMatrix,channel);

    Unfold::UnfoldType unfTUnfold = Unfold::TUNFOLD;
    unfClosure->EngageUnfolding(unfTUnfold);
    TH1F*hUnfClosure = unfClosure->ReturnUnfolded();
    TCanvas*c2 = unfClosure->plotUnfolded("c2","TUnfold Test",true);
    TString closureSave = savePrefix;
    closureSave += "_UnfoldedClosure";
    closureSave += saveSuffix;
    c2->SaveAs(closureSave);

    // Unfold data
    Unfold*unfData = new Unfold(hData,hTrue,hMatrix,channel);
    unfData->SetBackground(hBack);
    unfData->EngageUnfolding(unfTUnfold);
    TH1F*hUnfData = unfData->ReturnUnfolded();
    TCanvas*c5 = unfData->plotUnfolded("c5","TUnfold Test",true);
    TString unfoldSave = savePrefix;
    unfoldSave += "_UnfoldedData";
    unfoldSave += saveSuffix;
    c5->SaveAs(unfoldSave);
    
    //Get Output covariance matrix
    TH2*hOutCov = unfData->ReturnOutputCovariance();
    TString covSave = savePrefix;
    covSave += "_OutputCovariance";
    covSave += saveSuffix;
    TCanvas*cCov = new TCanvas("cCov","",0,0,1000,1000);
    cCov->SetRightMargin(0.15);
    cCov->SetLeftMargin(0.15);
    cCov->SetLogz();
    hOutCov->Draw("colz");
    cCov->SaveAs(covSave);

    //Get Output correlation matrix
    TH2*hOutCorr = unfData->ReturnCorrelationMatrix();
    TString corrSave = savePrefix;
    corrSave += "_Correlations";
    corrSave += saveSuffix;
    TCanvas*cCorr = new TCanvas("cCorr","",0,0,1000,1000);
    cCorr->SetRightMargin(0.15);
    cCorr->SetLeftMargin(0.15);
    hOutCorr->Draw("colz");
    cCorr->SaveAs(corrSave);

    // plot response matrix
    TH2F*hResponse = unfClosure->ReturnResponseMatrix();
    TString responseSave = savePrefix;
    responseSave += "_ResponseMatrix";
    responseSave += saveSuffix;
//    unfData->plotMatrix(hResponse,responseSave,true);
    TCanvas*cResp = new TCanvas("cResp","",0,0,1400,1000);
    cResp->SetRightMargin(0.15);
    cResp->SetLeftMargin(0.15);
    hResponse->Draw("colz");
    cResp->SaveAs(responseSave);

    // Save the histograms
    TString save_loc = directory;
    save_loc += "/";
    save_loc += channel;
    save_loc += "/unfolding_output.root";
    TFile*save_file = new TFile(save_loc,"recreate");
    hOutCov->SetName("unfolding covariance");
    hReco->SetName("reconstructed");
    hData->SetName("data");
    hTrue->SetName("true");
    hBack->SetName("backgrounds");
    hMatrix->SetName("migration matrix");
    hUnfClosure->SetName("unfold closure test");
    hUnfData->SetName("unfold data");

    hOutCov->Write();
    hReco->Write();
    hData->Write();
    hTrue->Write();
    hBack->Write();
    hMatrix->Write();
    hUnfClosure->Write();
    hUnfData->Write();
    save_file->Close();
}

