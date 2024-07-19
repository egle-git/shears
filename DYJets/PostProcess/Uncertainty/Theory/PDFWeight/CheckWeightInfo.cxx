#include "WeightInfo.h"

void CheckWeightInfo() {
  // WeightInfo info("ee", "16pre", "DYJets_M-1500to2000", "PDFVar", 50);
  // cout << info.nEvent_HugeWRatio() << ", " << info.Frac_HugeWRatio() << endl;

  vector<TString> vec_channel = {"ee", "mm"};
  vector<TString> vec_era = {"16pre", "16post", "17", "18"};

  for(const auto& channel : vec_channel ) {
    for(const auto& era: vec_era ) {
      EraWeightInfo info(channel, era);

      TString fileName = TString::Format("WeightInfo_%s_%s.root", channel.Data(), era.Data());
      TFile* f_output = TFile::Open(fileName, "RECREATE");
      info.Save(f_output);
      f_output->Close();
    }
  }
}