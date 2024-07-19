// -- to produce the input python code for DY Acceptance calculation (on miniAOD)

#include "WeightInfo.h"

TString Get_RequestName(TString era, TString channel, TString sampleTag) {
  // theRequestName = "%s_m%s_%s" % (channel, massRange, era)
  if( !sampleTag.Contains("DYJets_") ) {
    throw std::invalid_argument(sampleTag + " is not a DY sample!");
    return "";
  }

  vector<TString> split = DYTool::SplitString(sampleTag, "-");
  TString massRange = split[1];

  return TString::Format("%s_m%s_%s", channel.Data(), massRange.Data(), era.Data());
}

void WriteValue(ofstream &file, TString& requestName, TString valueType, vector<Double_t>& vec_value) {
  TString listName = TString::Format("%s_%s", requestName.Data(), valueType.Data());
  file << "\"" << listName << "\" : [";

  Int_t nValue = (Int_t)vec_value.size();
  for(Int_t i=0; i<nValue; ++i) {
    Double_t value = vec_value[i];

    if( i == nValue-1 ) file << value << "]," << endl; // -- last element
    else                file << value << ", ";
  }
}

void MakePyScript_WeightInfo() {
  // WeightInfo info("ee", "16pre", "DYJets_M-1500to2000", "PDFVar", 50);
  // cout << info.nEvent_HugeWRatio() << ", " << info.Frac_HugeWRatio() << endl;

  // -- init. the file
  TString outputName = "WeightInfo_DYSample.py";
  ofstream file(outputName);
  if( !file.is_open() )
    throw std::runtime_error("file is not open!");
  file << "dic_weightInfo = {" << endl;

  vector<TString> vec_channel = {"ee", "mm"};
  vector<TString> vec_era = {"16pre", "16post", "17", "18"};

  for(const auto& channel : vec_channel ) {
    for(const auto& era: vec_era ) {
      EraWeightInfo info(channel, era);

      std::map<TString, SampleWeightInfo>& map_sampleInfo = info.InfoMap();

      for(const auto& sampleInfo : map_sampleInfo ) {
        TString sampleTag = sampleInfo.second.SampleTag();
        if( !sampleTag.Contains("DYJets_") ) continue; // -- only for DY samples

        TString requestName = Get_RequestName(era, channel, sampleTag);

        TH1D* h_mean = sampleInfo.second.Hist("mean");
        TH1D* h_sigma = sampleInfo.second.Hist("sigma");

        vector<Double_t> vec_mean;
        vector<Double_t> vec_sigma;

        for(Int_t i=0; i<100; ++i) { // -- 100 sets; only for PDF Hessian sets
          Int_t i_bin = i+1;

          Double_t mean  = h_mean->GetBinContent(i_bin);
          Double_t sigma = h_sigma->GetBinContent(i_bin);
          vec_mean.push_back( mean );
          vec_sigma.push_back( sigma );
        }

        WriteValue(file, requestName, "mean",  vec_mean);
        WriteValue(file, requestName, "sigma", vec_sigma);
      } // -- iteration over sample

      // break;
    } // -- iteration over era

    // break;
  } // -- iteration over channel

  file << "}" << endl;
  file.close();
}