#include <iostream>
#include <TString.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TStyle.h>
#include <TROOT.h>
#include <TApplication.h>

using namespace std;



int main(int argc, char **argv) {
  //gROOT->SetBatch();
  bool DJALOG = true;

    if (argc < 3) {
        cout << "You need to provide lepSel and in/out dir." << endl;
        cout << "ex:  ./plotRespMatrix DMu testin testout" << endl;
        return 1;
    }

    //TApplication *myApp = new TApplication("myApp", &argc, argv);

    TString lepSel(argv[1]);
    //TString variable(argv[2]);
    TString inputDir(argv[2]);
    TString outputDir(argv[3]);
    TCanvas *cMad = new TCanvas("cMad", "MadGraph", 800, 800);
    std::vector<TString> variables;
    //Variables of Interest:
    variables.push_back("hresponseFirstJetAbsRapidity_Zinc1jet");
    variables.push_back("hresponseFirstJetPt_Zinc1jet");
    variables.push_back("hresponseSecondJetAbsRapidity_Zinc2jet");
    variables.push_back("hresponseSecondJetPt_Zinc2jet");
    variables.push_back("hresponseThirdJetAbsRapidity_Zinc3jet");
    variables.push_back("hresponseThirdJetPt_Zinc3jet");
    variables.push_back("hresponseFourthJetAbsRapidity_Zinc4jet");
    variables.push_back("hresponseFourthJetPt_Zinc4jet");
    variables.push_back("hresponseJetsHT_Zinc1jet");
    variables.push_back("hresponseJetsHT_Zinc2jet");
    variables.push_back("hresponseJetsHT_Zinc3jet");
    variables.push_back("hresponseZPt_Zinc0jet");
    variables.push_back("hresponseZPt_Zinc1jet");
    variables.push_back("hresponseJZB");
    variables.push_back("hresponseZNGoodJets_Zexc");
    /*
    //Others:
    variables.push_back("hhJetMatching_StatMatches");
    variables.push_back("hhJetMatching_StatFail");
    variables.push_back("hhJetMatching_StatMatches_Lep");
    variables.push_back("hhJetMatching_StatFail_Lep");
    variables.push_back("hhRecoGenJetMatchingMatrix");
    variables.push_back("hhJetMatching_FirstJetRap_Match");
    variables.push_back("hhJetMatching_FirstJetRap_NoMatch");
    variables.push_back("hhJetMatching_FirstJetNoMatch_Matrix");
    variables.push_back("hresponseFirstJetAbsRapidityMatch_Zinc1jet");
    variables.push_back("hresponseFirstJetPtMatch_Zinc1jet");
    variables.push_back("hresponseSecondJetAbsRapidityMatch_Zinc2jet");
    variables.push_back("hresponseSecondJetPtMatch_Zinc2jet");
    variables.push_back("hresponseThirdJetAbsRapidityMatch_Zinc3jet");
    variables.push_back("hresponseThirdJetPtMatch_Zinc3jet");
    //variables.push_back("");
    */
    gStyle->SetOptStat(0);
    gStyle->SetPaintTextFormat("4.0f");
    gStyle->SetPalette(57);





    TFile *fMad = new TFile(inputDir +"/" + lepSel + "_13TeV_DYJets_UNFOLDING_Syst_0.root");
    if(fMad->IsZombie() || !fMad->IsOpen()){
      printf("Problem with rootfile\n");
      return 0;
    }
    std::cout<<"File Open:"<<fMad->GetName()<<std::endl;


    for(size_t iVar=0;iVar<variables.size();iVar++){

      TString outputFileName = outputDir+"/" + lepSel + "_" + variables[iVar] + "_ResponseMatrix";

      if(DJALOG) printf("Getting histograms\n");
      TH1D *hMad = (TH1D*) fMad->Get(variables[iVar]);
      TH2D *hrespMad = 0;
      TH2D *hrespMadMatch = 0;
      if(variables[iVar]=="hresponseFirstJetAbsRapidity_Zinc1jet"){
	hrespMad = (TH2D*) fMad->Get(variables[iVar]);
	hrespMadMatch = (TH2D*) fMad->Get("hresponseFirstJetAbsRapidityMatch_Zinc1jet");
      }else{
	hrespMad = (TH2D*) fMad->Get(variables[iVar]);
      }

      TString title;
      TString xTitle;
      TString yTitle;
      if(hrespMad == 0){
	printf("Problem getting the histograms from the rootfile\n");
	std::cout<<"variables[iVar] = "<<variables[iVar]<<std::endl;
	continue;
      }
      if(hMad == 0){
	printf("Problem getting the histograms from the rootfile\n");
	std::cout<<"Variables[IVar] = "<<variables[iVar]<<std::endl;
	title = hrespMad->GetTitle();
	xTitle = "";
	yTitle = "";
	//return 0;
      }else{
	title = hMad->GetTitle();
	xTitle = hMad->GetXaxis()->GetTitle();
	yTitle = "gen " + xTitle;
      }
      xTitle = "Reco Jet";
      yTitle = "Gen Jet";
      /*
      double factor = -(33667.0-150.0)/34801.0;
      factor = -0.955;
      std::cout<<factor<<std::endl;
      if(variables[iVar]=="hresponseFirstJetAbsRapidity_Zinc1jet"){
	hrespMad->Add(hrespMad,hrespMadMatch,1.0,factor);
      }
      */
      
      TH2D *hrespNormMad = (TH2D*) hrespMad->Clone();


      /*    TFile *fShe = new TFile("../HistoFiles/" + lepSel + "_13TeV_DYJets_Sherpa_Bugra_1_13_UNFOLDING_dR_TrigCorr_0_Syst_0_JetPtMin_30_JetEtaMax_24.root");
	    TH1D *hShe = (TH1D*) fShe->Get(variables[iVar]);
	    TH2D *hrespShe = (TH2D*) fShe->Get(variables[iVar]);
	    TH2D *hrespNormShe = (TH2D*) hrespShe->Clone();
      */
      
      if(DJALOG) printf("Formatting the histograms\n");
      hrespNormMad->SetTitle("aMC@NLO+Pythia8 Resp. Matrix for " + title);
      hrespNormMad->GetXaxis()->SetTitle(xTitle);
      hrespNormMad->GetXaxis()->SetTitleOffset(1.4);
      hrespNormMad->GetYaxis()->SetTitle(yTitle);
      hrespNormMad->GetYaxis()->SetTitleOffset(1.6);
      hrespNormMad->GetZaxis()->SetTitle("");
      hrespNormMad->GetZaxis()->SetRangeUser(0,100);
      hrespNormMad->SetMarkerSize(1.0);
      /*
	hrespNormShe->SetTitle("Sherpa Resp. Matrix for " + title);
	hrespNormShe->GetXaxis()->SetTitle(xTitle);
	hrespNormShe->GetXaxis()->SetTitleOffset(1.4);
	hrespNormShe->GetYaxis()->SetTitle(yTitle);
	hrespNormShe->GetYaxis()->SetTitleOffset(1.6);
	hrespNormShe->GetZaxis()->SetTitle("");
	hrespNormShe->GetZaxis()->SetRangeUser(0,100);
      */
      if(DJALOG) printf("Copying and normalizing content\n");
      int nBinsX = hrespMad->GetNbinsX();
      int nBinsY = hrespMad->GetNbinsY();
      if(DJALOG) printf("Bins X = %d, Bins Y = %d\n",nBinsX, nBinsY); 
      //nBinsX = 10;
      //nBinsY = 10;
      for (int i=1; i < nBinsX+1; i++) {
        double totRowMad(0);
	//        double totRowShe(0);
        for (int j=1; j < nBinsY + 1; j++) {
	  totRowMad += hrespMad->GetBinContent(i, j);
	  //printf("%F\n",hrespMad->GetBinContent(i, j));
	  //            totRowShe += hrespShe->GetBinContent(j, i);
        }
	//printf("Total Column = %F\n",totRowMad);
        for (int j=1; j < nBinsY+1; j++) {
	  double binContentMad = hrespMad->GetBinContent(i, j);
	  //printf("(%d,%d) = %F   |",i,j,binContentMad);
	  //            double binContentShe = hrespShe->GetBinContent(j, i);
	  //hrespNormMad->SetBinContent(i, j, binContentMad);
	  hrespNormMad->SetBinContent(i, j, 100*binContentMad/totRowMad);
	  //            hrespNormShe->SetBinContent(j, i, 100*binContentShe/totRowShe);
        }
	//printf("\n");
      }

      if(DJALOG) printf("Creating the canvas\n");

      //cMad->Connect("Closed()", "TApplication", myApp,  "Terminate()"); 
      cMad->cd();
      
      TPad *padMad = new TPad("padMad", "padMad", 0, 0, 1, 1);
      padMad->SetRightMargin(0.12);
      padMad->SetLeftMargin(0.12);
      padMad->SetBottomMargin(0.12);
      if(variables[iVar].Index("Pt") >= 0 || variables[iVar].Index("HT") >= 0){
        padMad->SetLogy(0);
        padMad->SetLogx(0);
      }
      padMad->Draw();
      padMad->cd();
      printf("Drawing Histogrames\n");
      hrespNormMad->DrawCopy("colztext");
      hrespNormMad->Draw("COLZ TEXT");
      padMad->Draw();
      cMad->Update();
      cMad->cd();
      //cMad->SaveAs(outputFileName + "_MadGraph.png");
      printf("Printing PDF\n");
      cMad->Print(outputFileName + "_MadGraph.pdf","pdf");
      //cMad->Print("MadGraph.pdf","pdf");
    //cMad->SaveAs(outputFileName + "_MadGraph.ps");
    //cMad->SaveAs(outputFileName + "_MadGraph.eps");
    //cMad->SaveAs(outputFileName + "_MadGraph.root");
/*
    TCanvas *cShe = new TCanvas("cShe", "Sherpa", 800, 800);
    cShe->Connect("Closed()", "TApplication", myApp,  "Terminate()"); 
    cShe->cd();
    
    TPad *padShe = new TPad("padShe", "padShe", 0, 0, 1, 1);
    padShe->SetRightMargin(0.12);
    padShe->SetLeftMargin(0.12);
    padShe->SetBottomMargin(0.12);
    padShe->Draw();
    padShe->cd();
    hrespNormShe->DrawCopy("colztext");
    padShe->Draw();
    cShe->Update();
    cShe->cd();
    cShe->SaveAs(outputFileName + "_Sherpa.png");
    cShe->SaveAs(outputFileName + "_Sherpa.pdf");
    cShe->SaveAs(outputFileName + "_Sherpa.root");
*/
      cMad->Clear();
    }

    //myApp->Run();
    fMad->Close();
//    fShe->Close();
    return 0;
}
