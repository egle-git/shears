#include "charge_misid.h"

#include <iostream>
#include <TFile.h>
#include <TH2D.h>

namespace physics
{

charge_misid::charge_misid(const YAML::Node &node)
{
    util::logging::info << "Setting up charge misid correction" << std::endl;

    std::string filename = node["charge misid sf location"].as<std::string>();
    util::logging::info << "File for charge misid SFs: " << filename << std::endl;
    std::unique_ptr<TFile> input(TFile::Open(filename.c_str()));
    if (input)
        util::logging::debug << "The file is open." << std::endl;
    else
        util::logging::warn << "The file was not found!" << std::endl;

    util::logging::debug << "   Getting histograms" << std::endl;
    _charge_misid_data = std::unique_ptr<TH2D>((TH2D*)(input->Get("cfdata")));
    if (_charge_misid_data == nullptr)
        throw std::runtime_error("Histogram 'cfdata' not found!");
    _charge_misid_mc = std::unique_ptr<TH2D>((TH2D*)(input->Get("cfmc")));
    if (_charge_misid_mc == nullptr)
        throw std::runtime_error("Histogram 'cfmc' not found!");
    _charge_misid_sf = std::unique_ptr<TH2D>((TH2D*)(input->Get("cfsf")));
    if (_charge_misid_sf == nullptr)
        throw std::runtime_error("Histogram 'cfsf' not found!");

    _charge_misid_data->SetDirectory(0);
    _charge_misid_mc->SetDirectory(0);
    _charge_misid_sf->SetDirectory(0);
    input->Close();

    if (_charge_misid_data != nullptr && _charge_misid_mc != nullptr && _charge_misid_sf != nullptr)
        util::logging::debug << "Histograms obtained successfully." << std::endl;
    else throw std::runtime_error("Histograms could not be obtained!");

    if (node["charge misid sf mode"]) _var = node["charge misid sf mode"].as<int>();
    if (_var == 0)
        util::logging::info << "Will be using nominal charge misid SF values." << std::endl;
    else if (_var == -1)
        util::logging::info << "Will be using a -1 sigma variation for charge misid SF values." << std::endl;
    else if (_var == 1)
        util::logging::info << "Will be using a +1 sigma variation for charge misid SF values." << std::endl;
    else throw std::runtime_error("Wrong charge misid SF mode value! Should be 0, 1, or -1!");
}

double charge_misid::get_charge_misid_data(const lepton &lep)
{
    double xbin = _charge_misid_data->GetXaxis()->FindBin(std::abs(lep.raw_v.Eta()));
    if (xbin == 0) xbin = 1;
    else if (xbin > _charge_misid_data->GetNbinsX()) xbin = _charge_misid_data->GetNbinsX();

    double ybin = _charge_misid_data->GetYaxis()->FindBin(lep.raw_v.Pt());
    if (ybin == 0) ybin = 1;
    else if (ybin > _charge_misid_data->GetNbinsY()) ybin = _charge_misid_data->GetNbinsY();

    return _charge_misid_data->GetBinContent(xbin, ybin);
}

double charge_misid::get_charge_misid_mc(const lepton &lep)
{
    double xbin = _charge_misid_mc->GetXaxis()->FindBin(std::abs(lep.raw_v.Eta()));
    if (xbin == 0) xbin = 1;
    else if (xbin > _charge_misid_mc->GetNbinsX()) xbin = _charge_misid_mc->GetNbinsX();

    double ybin = _charge_misid_mc->GetYaxis()->FindBin(lep.raw_v.Pt());
    if (ybin == 0) ybin = 1;
    else if (ybin > _charge_misid_mc->GetNbinsY()) ybin = _charge_misid_mc->GetNbinsY();

    return _charge_misid_mc->GetBinContent(xbin, ybin);
}

double charge_misid::get_sf(const lepton &lep, const int &var)
{
    double xbin = _charge_misid_sf->GetXaxis()->FindBin(std::abs(lep.raw_v.Eta()));
    if (xbin == 0) xbin = 1;
    else if (xbin > _charge_misid_sf->GetNbinsX()) xbin = _charge_misid_sf->GetNbinsX();

    double ybin = _charge_misid_sf->GetYaxis()->FindBin(lep.raw_v.Pt());
    if (ybin == 0) ybin = 1;
    else if (ybin > _charge_misid_sf->GetNbinsY()) ybin = _charge_misid_sf->GetNbinsY();

    // util::logging::debug << "SF weight = " << _charge_misid_sf->GetBinContent(xbin, ybin) << std::endl;
    double weight = _charge_misid_sf->GetBinContent(xbin, ybin);

    // var overrides _var for systematic variation if it has a reasonable value
    if (var == 1 || (var < -100 && _var == 1)) weight +=  _charge_misid_sf->GetBinError(xbin, ybin);
    else if (var == -1 || (var < -100 && _var == -1)) weight -=  _charge_misid_sf->GetBinError(xbin, ybin);

    
    return weight;
}


} // namespace physics
