#ifndef __UnfoldingSyst__ 
#define __UnfoldingSyst__

#include <TH1.h>
#include <TH2.h>
#include <string>
#include <RooUnfoldResponse.h>

using namespace std;

TH2D* CovToCorr(const TH2D *h);
TH2D* CovToCorr2(const TH2D *h, string title);
TH2D* setCovariance(const TH2D *h, const TH1D *hCent, double error);
TH1D* getSumBG(string histoFilesDirectory, string leptonFlavor, string energy, string* DataHistoFilesNameArray, string syst, string direction, int JetPtMin, int JetPtMax, string variable);
TH1D* Unfold(TString unfAlg, RooUnfoldResponse* response, TH1D* hData, TH1D* hSumBG, int Kterm, TString hOutName, bool useOverFlow = true);
TH2D* Unfold2D(TString unfAlg, RooUnfoldResponse* response, TH2D* hData, TH2D* hSumBG, int Kterm, TString hOutName, bool useOverFlow = true);
TH2D* CovFromRoo(TString unfAlg, RooUnfoldResponse* response, TH1D* hData, TH1D* hSumBG, int Kterm, TString hOutName, int NToys, bool useOverFlow = true);
void hBinCon(TH1D* hData);
TH1D* SetSystErrorsMax(TH1D* hCent, TH1D* hUp, TH1D* hDown, string name);
TH1D* SetSystErrorsMean(TH1D* hData, double Error, string name);
TH1D* SetSystErrorsMean(TH1D* hCent, TH1D* hUp, TH1D* hDown, string name);
TH1D* SetSystErrorsMean(TH1D* hData, TH1D* hCent, TH1D* hUp, TH1D* hDown, string name);
TH1D* SetSystErrorsMean(TH1D* hData, TH1D* hCent, TH1D* hUp, TH1D* hDown, double Error, string name);
TH2D* SetResponseErrorsMax(TH2D* hCent, TH2D* hUp, TH2D* hDown);
TH2D* SetResponseErrorsMean(TH2D* hCent, TH2D* hUp, TH2D* hDown);
TH1D* ToyMCErrorsStat(TString unfAlg, TH1D *hdata, TH1D* hBack0[], RooUnfoldResponse *response, int kterm, TH2D *hCovariance, TH2D *hCorrelation, int nPseudos, int selection, bool useOverFlow = true, int NBkgGroups = 7);
#endif
