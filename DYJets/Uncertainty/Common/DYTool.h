#include "SimplePlotTools.h"
#include "DYPath.h"
#include "DYOutput.h"

namespace DYTool {

// -- convert TUnfoldOutput (axis: mass bin number) to the histogram with the mass binning
// ---- h_axis: empty histogram with the mass binning (template)
// ---- h_TUnfold: TUnfold output (i.e. axis = mass bin number)
TH1D* Convert_TUnfoldOutput_MassAxis(TH1D* h_axis, TH1D* h_TUnfold) {
  Int_t nMassBin = h_axis->GetNbinsX();
  Int_t nMassBinNum = h_TUnfold->GetNbinsX();
  if( nMassBinNum != (nMassBin+2) ) {
    cout << "(nMassBin+2, nMassBinNum) = (" << nMassBin+2 << ", " << nMassBinNum <<"): inconsistent!" << endl;
    throw std::invalid_argument("[DYTool::Convert_TUnfoldOutput_MassAxis] Inconsistent number of bins");
  }

  TH1D* h_return = (TH1D*)h_axis->Clone();
  h_return->Reset("ICES");

  for(Int_t i=0; i<nMassBinNum; ++i) {
    Int_t i_bin = i+1;
    Double_t value = h_TUnfold->GetBinContent(i_bin);
    Double_t error = h_TUnfold->GetBinError(i_bin);

    // -- i_bin = 1 --> i_massBin = 0 (underflow)
    // -- i_bin = 2 --> i_massBin = 1 (first bin)
    // -- ...
    // -- i_bin = nMassBinNum --> i_massBin = nMassBinNum-1 = nMassBin+1 (overflow)
    Int_t i_massBin = i_bin-1;
    h_return->SetBinContent(i_massBin, value);
    h_return->SetBinError(i_massBin, error);
  }

  return h_return;
}

// -- if h_axis is not provided: 
// -- find the mass-axis histogram in the .root file in DYTool::path_default
TH1D* Convert_TUnfoldOutput_MassAxis(TH1D* h_TUnfold) {
  TString fileName = DYTool::path_default+"/mm/16pre/dyjets-data.root";
  TString histName = "mass_wide_range_inc0jet";
  TH1D* h_axis = PlotTool::Get_Hist(fileName, histName);

  return Convert_TUnfoldOutput_MassAxis(h_axis, h_TUnfold);
}

TH1D* Convert_TUnfoldOutput_DSigmaDM(TH1D* h_axis, TH1D* h_TUnfold, Double_t lumi) {
  TH1D* h_mass = Convert_TUnfoldOutput_MassAxis(h_axis, h_TUnfold);
  TH1D* h_dSigmadM = PlotTool::DivideEachBin_ByBinWidth(h_mass);
  h_dSigmadM->Scale( 1.0 / lumi );

  return h_dSigmadM;
}

// -- if h_axis is not provided: 
// -- find the mass-axis histogram in the .root file in DYTool::path_default
TH1D* Convert_TUnfoldOutput_DSigmaDM(TH1D* h_TUnfold, Double_t lumi) {
  TH1D* h_mass = Convert_TUnfoldOutput_MassAxis(h_TUnfold);
  TH1D* h_dSigmadM = PlotTool::DivideEachBin_ByBinWidth(h_mass);
  h_dSigmadM->Scale( 1.0 / lumi );

  return h_dSigmadM;
}

vector<TString> SplitString(TString theString, TString delimeter) {
  vector<TString> vec_str;

  TObjArray *objArr_token = theString.Tokenize(delimeter);
  for(Int_t i=0; i< objArr_token->GetEntries(); ++i) {
    vec_str.push_back( ((TObjString *)(objArr_token->At(i)))->String() );
  }

  return vec_str;
}

void Set_Fake(TString channel, DYRun2Result* result) {
  if( channel != "ee" && channel != "mm" )
    throw std::invalid_argument("[DYTool::Set_Fake] channel = "+channel+" is not supported!");

  for(const auto& info : DYTool::vec_fakeLepBkgInfo) {
    if( info.channel != channel ) continue;
    result->Set_Fake(info.era, info.fileName, info.histName);
  }
}

}; // -- end of namespace DYTool