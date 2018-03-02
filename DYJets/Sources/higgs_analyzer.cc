#include "higgs_analyzer.h"

#include <algorithm>

#include <TFile.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "lepton.h"

higgs_analyzer::higgs_analyzer(util::job::info &info, const util::options &opt)
    : muons_analyzer(info.reader, opt),
      weights_analyzer(info)
{
    declare("mass", "Dilepton mass", 100, 0, 200);
}

void higgs_analyzer::operator()()
{
    using namespace physics;

    weights_analyzer::operator()();

    std::vector<lepton> muons = get_muons();

    if (muons.size() >= 2) {
        TLorentzVector pZ = muons[0].v + muons[1].v;
        float charge_check = muons[0].charge * muons[1].charge;

        if (charge_check >= 0) {
            return;
        }
        fill_muons(muons, "Zinc0jet");
        fill("mass", "Zinc0jet", pZ.M());
    }
}

void higgs_analyzer::write()
{
    weights_analyzer::write();
    histo_set::write();
}

po::options_description higgs_analyzer::options()
{
    return po::options_description("Physics options");
}
