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

// -- convert 2D (e.g. covariance/correlation matrices)
TH2D* Convert2D_TUnfoldOutput_MassAxis(TH1D* h_axis, TH2D* h2D_TUnfold) {
  Int_t nMassBin = h_axis->GetNbinsX();
  Int_t nMassBinNumX = h2D_TUnfold->GetNbinsX();
  Int_t nMassBinNumY = h2D_TUnfold->GetNbinsY();

  if( nMassBinNumX != nMassBinNumY )
    throw std::invalid_argument("[DYTool::Convert2D_TUnfoldOutput_MassAxis] The input matrix should be square!");

  if( nMassBinNumX != (nMassBin+2) ) {
    cout << "(nMassBin+2, nMassBinNumX) = (" << nMassBin+2 << ", " << nMassBinNumX <<"): inconsistent!" << endl;
    throw std::invalid_argument("[DYTool::Convert2D_TUnfoldOutput_MassAxis] Inconsistent number of bins");
  }

  // -- bin edges
  Double_t* arr_massBinEdge = new Double_t[nMassBin+1];
  for(Int_t i=0; i<nMassBin+1; ++i) { // -- up to nMassBin+1 (upper edge of the last bin)
    Int_t i_bin = i+1;
    arr_massBinEdge[i] = h_axis->GetXaxis()->GetBinLowEdge(i_bin);
  }

  TString histName = h2D_TUnfold->GetName();
  TH2D* h2D_return = new TH2D(histName+"_mAxis", "", nMassBin, arr_massBinEdge, nMassBin, arr_massBinEdge);

  for(Int_t i_x=0; i_x<nMassBinNumX; ++i_x) {
    Int_t i_binX = i_x+1;

    // -- i_bin = 1 --> i_massBin = 0 (underflow)
    // -- i_bin = 2 --> i_massBin = 1 (first bin)
    // -- ...
    // -- i_bin = nMassBinNum --> i_massBin = nMassBinNum-1 = nMassBin+1 (overflow)
    Int_t i_massBinX = i_binX-1;

    for(Int_t i_y=0; i_y<nMassBinNumY; ++i_y) {
      Int_t i_binY = i_y+1;
      Int_t i_massBinY = i_binY-1;

      Double_t value = h2D_TUnfold->GetBinContent(i_binX, i_binY);
      Double_t error = h2D_TUnfold->GetBinError(i_binX, i_binY);

      h2D_return->SetBinContent(i_massBinX, i_massBinY, value);
      h2D_return->SetBinError(i_massBinX, i_massBinY, error);
    }
  }

  delete[] arr_massBinEdge;

  return h2D_return;
}

// -- if h_axis is not provided: 
// -- find the mass-axis histogram in the .root file in DYTool::path_default
TH1D* Convert_TUnfoldOutput_MassAxis(TH1D* h_TUnfold) {
  TString fileName = DYTool::path_default+"/mm/16pre/dyjets-data.root";
  TString histName = "mass_wide_range_inc0jet";
  TH1D* h_axis = PlotTool::Get_Hist(fileName, histName);

  return Convert_TUnfoldOutput_MassAxis(h_axis, h_TUnfold);
}

TH2D* Convert2D_TUnfoldOutput_MassAxis(TH2D* h2D_TUnfold) {
  TString fileName = DYTool::path_default+"/mm/16pre/dyjets-data.root";
  TString histName = "mass_wide_range_inc0jet";
  TH1D* h_axis = PlotTool::Get_Hist(fileName, histName);

  return Convert2D_TUnfoldOutput_MassAxis(h_axis, h2D_TUnfold);
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

void Set_Acc(DYRun2Result* result, TString tag) {
  result->Set_Acc(DYTool::path_acc, "h_acc_"+tag);
}

void Make_Dir(TString dirPath) {
  bool recursive = kTRUE;
  if( gSystem->mkdir(dirPath.Data(), recursive) < 0 )
    throw std::runtime_error("Directory = " + dirPath + " cannot be created (already exists?)");
}

void Remove_NegativeBin(TH1D* h) {
  for(Int_t i=0; i<h->GetNbinsX(); ++i) {
    Int_t i_bin = i+1;
    Double_t value = h->GetBinContent(i_bin);
    if( value < 0 ) {
      printf("  [%03d bin] (%.3lf, %.3lf) -> value = %lf < 0 ... set it 0 (as well as its error)\n",
             i_bin, h->GetBinLowEdge(i_bin), h->GetBinLowEdge(i_bin+1), value);
      h->SetBinContent(i_bin, 0);
      h->SetBinError(i_bin, 0);
    }
  }
}

Double_t GetLumi(TString era) {
  if( era == "16pre" )  return LUMI_16pre;
  if( era == "16post" ) return LUMI_16post;
  if( era == "17" )     return LUMI_17;
  if( era == "18" )     return LUMI_18;
  if( era == "all" )    return LUMI_16pre+LUMI_16post+LUMI_17+LUMI_18;

  throw std::invalid_argument("[DYTool::GetLumi] era = "+era+" is not supported");

  return 0;
}

void Save_DYRun2Result_MassAxis(TFile *f_output, DYRun2Result* result, Bool_t isFPS = kFALSE, TString tag = "mAxis") {
  if( !tag.Contains("mAxis") )
    printf("[DYTool::Save_DYRun2Result_MassAxis] tag = %s does not have mAxis word; you may want to add it?\n", tag.Data());

  vector<TString> vec_dsigdmType = {"DY", "data"};

  // -- 1D histograms
  for( const auto& pair: result->Get_Map_AllEraHist() ) {
    TString histType = pair.first;
    if( histType.Contains("reco_") ) continue; // -- 4-era axis (no need to convert)

    TString histName = tag != "" ? 
      TString::Format("h_allEra_%s_%s", histType.Data(), tag.Data()) :
      TString::Format("h_allEra_%s", histType.Data());

    TH1D* h_allEra_mAxis = (TH1D*)pair.second->Clone();
    h_allEra_mAxis = Convert_TUnfoldOutput_MassAxis(h_allEra_mAxis);
    h_allEra_mAxis->SetName(histName);

    f_output->cd();
    h_allEra_mAxis->Write();    
  }

  // -- additional histograms: dsigma/dm (no "mAxis" tag as it is obvious that the axis is mass)
  for(const auto& type : vec_dsigdmType ) {
    TString histType_nEvent = (isFPS) ? "unfoldedFPS" : "unfolded";
    TH1D* h_dsigdm = result->Get_AllEra(histType_nEvent, type);
    h_dsigdm = Convert_TUnfoldOutput_DSigmaDM(h_dsigdm, GetLumi("all"));

    TString histName_dsigdm = "h_dsigdm_"+type;
    if( tag != "mAxis" ) histName_dsigdm += "_"+tag;
    h_dsigdm->SetName(histName_dsigdm);

    f_output->cd();
    h_dsigdm->Write();
  }

  // -- 2D histograms: 4-era axis for x: no need to convert
}

void Assign_RelUnc(TH1D* h_cv, TH1D* h_relUnc) {    
  if( h_relUnc->GetNbinsX() != h_cv->GetNbinsX() )
    throw std::invalid_argument("[ChannelResult::Assign_RelUnc] # bins are not consistent between cv vs. uncertainty histogram");

  Int_t nBin = h_cv->GetNbinsX();
  for(Int_t i=0; i<nBin; ++i) {
    Int_t i_bin = i+1;

    Double_t relUnc = h_relUnc->GetBinContent(i_bin);
    Double_t value = h_cv->GetBinContent(i_bin);
    Double_t absUnc = value * relUnc;

    h_cv->SetBinError(i_bin, absUnc);
  }
}

}; // -- end of namespace DYTool