#include "tables.h"

#include <cmath>
#include <fstream>

#ifdef DYJETS_NEW_API
#   include <yaml-cpp/yaml.h>
#endif // DYJETS_NEW_API

#include "logging.h"

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
    logging::debug << "Loading table: " << filename << std::endl;
    std::ifstream file(filename);
    if (!file) {
        throw std::invalid_argument("File " + filename + " doesn't exist");
    }
    double data[7];
    while (file) {
        for (int i(0); i < 7; i++) {
            file >> data[i];
        }
        _recd.push_back(record{data[2], data[3], data[0], data[1], data[4], data[5], data[6]});
    }

    double pt_lowest = 1e10;
    double pt_highest = -1e10;
    for( auto record : _recd ) {
        if( record.ptLow < pt_lowest ) pt_lowest = record.ptLow;
        if( record.ptHi > pt_highest ) pt_highest = record.ptHi;
    }

    _ptBinEdge_lowest = pt_lowest; // -- lowest pT bin edge
    _ptBinEdge_highest = pt_highest; // -- highest pT bin edge

    util::logging::debug << "[Table] (ptMin, ptMax) = (" << _ptBinEdge_lowest << ", " << _ptBinEdge_highest << ")" << std::endl;
}

// mode = 0: nominal
// mode = 1: up
// mode = -1: down
double table::getEfficiencyBase(double pt, double eta, int mode) const
{
    double eff = 1.0;
    bool isFound = false;

    if( pt < _ptBinEdge_lowest )  pt = _ptBinEdge_lowest + 0.001; // underflow: assign the first bin
    // if( pt > _ptBinEdge_highest ) pt = _ptBinEdge_highest - 0.001; // overflow: assign the last bin
    if( pt >= _ptBinEdge_highest ) pt = _ptBinEdge_highest - 0.001; // overflow: assign the last bin

    for (unsigned int i = 0; i != _recd.size(); i++) {
        if( (_recd[i]).belongTo(pt, eta) ) {
            if( mode == 0 )       eff = _recd[i].effi;
            else if( mode == 1 )  eff = _recd[i].effi + _recd[i].effiErrorHigh;
            else if( mode == -1 ) eff = _recd[i].effi - _recd[i].effiErrorLow;

            isFound = true;
            break;
        }
    }

    if( !isFound ) // it should not happen: for sanity check ...
        util::logging::warn << "no corresponding SF bin is found for (pt, eta) = (" << pt << ", " << eta << ") ... return 1.0" << std::endl;

    return eff;
}

double table::getEfficiency(double pt, double eta) const {
    return table::getEfficiencyBase(pt, eta, 0);
}

double table::getEfficiencyLow(double pt, double eta) const {
    return table::getEfficiencyBase(pt, eta, -1);
}

double table::getEfficiencyHigh(double pt, double eta) const {
    return table::getEfficiencyBase(pt, eta, 1);
}

} // namespace util

#ifdef DYJETS_NEW_API

/// \cond
namespace YAML
{

bool convert<util::table>::decode(const Node &node, util::table &table)
{
    if (node.IsScalar()) {
        table = util::table("EfficiencyTables/" + node.as<std::string>());
        return true;
    } else {
        return false;
    }
}
} // namespace YAML
/// \endcond

#endif // DYJETS_NEW_API
