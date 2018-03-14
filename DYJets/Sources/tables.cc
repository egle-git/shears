#include "tables.h"

#include <cmath>
#include <fstream>

namespace util
{

bool table::record::belongTo(double pt, double eta) const
{
    return (pt < ptHi && pt >= ptLow) && (eta < etaHi && eta >= etaLow);
}

bool table::record::equalTo(int num) const
{
    return (std::abs(num - etaLow) < 0.5); // etaLow means the first value
}

table::table(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file) {
        throw std::invalid_argument("File " + filename + "doesn't exist");
    }
    double data[7];
    while (file) {
        for (int i(0); i < 7; i++) {
            file >> data[i];
        }
        recd.push_back(record{data[2], data[3], data[0], data[1], data[4], data[5], data[6]});
    }
}

double table::getEfficiency(double pt, double eta) const
{
    double hiPtBin = 0;
    for (unsigned int i = 0; i != recd.size(); i++) {
        // if finds the proper bin, then return the efficiency
        if ((recd[i]).belongTo(pt, eta)) return recd[i].effi;
        // else store the average pt of the current bin efficency but do not return and try the next
        // bin
        if ((recd[i]).belongTo(0.5 * (recd[i].ptHi + recd[i].ptLow), eta)) hiPtBin = recd[i].effi;
    }
    return hiPtBin;
}
double table::getEfficiencyLow(double pt, double eta) const
{
    double hiPtBin = 0;
    for (unsigned int i = 0; i != recd.size(); i++) {
        if ((recd[i]).belongTo(pt, eta)) return recd[i].effi - recd[i].effiErrorLow;
        if ((recd[i]).belongTo(350, eta)) hiPtBin = recd[i].effi;
    }
    return hiPtBin;
}
double table::getEfficiencyHigh(double pt, double eta) const
{
    double hiPtBin = 0;
    for (unsigned int i = 0; i != recd.size(); i++) {
        if ((recd[i]).belongTo(pt, eta)) return recd[i].effi + recd[i].effiErrorHigh;
        if ((recd[i]).belongTo(350, eta)) hiPtBin = recd[i].effi;
    }
    return hiPtBin;
}

double table::getTTbarSF(int NJets) const
{
    for (unsigned int i = 0; i != recd.size(); i++) {
        if ((recd[i]).equalTo(NJets)) return recd[i].etaHi;
    }
    return 1;
}

double table::getTTbarSFLow(int NJets) const
{
    for (unsigned int i = 0; i != recd.size(); i++) {
        if ((recd[i]).equalTo(NJets)) return recd[i].etaHi - recd[i].ptLow;
    }
    return 1;
}

double table::getTTbarSFHigh(int NJets) const
{
    for (unsigned int i = 0; i != recd.size(); i++) {
        if ((recd[i]).equalTo(NJets)) return recd[i].etaHi + recd[i].ptLow;
    }
    return 1;
}
} // namespace util
