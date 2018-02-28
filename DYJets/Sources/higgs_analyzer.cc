#include "higgs_analyzer.h"

#include <algorithm>

#include <TFile.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "lepton.h"

higgs_analyzer::higgs_analyzer(TTreeReader &reader) : muons_analyzer(reader)
{
    declare("mass", "Dilepton mass", 100, 0, 200);
}

void higgs_analyzer::operator()()
{
    using namespace physics;

    std::vector<lepton> muons = get_muons();

    muons.erase(std::remove_if(muons.begin(),
                               muons.end(),
                               [](const lepton &mu) {
                                   return mu.v.Pt() < 7 || mu.iso >= 0.25 ||
                                          std::abs(mu.v.Eta()) > 2.4;
                               }),
                muons.end());

    if (muons.size() >= 2) {
        TLorentzVector pZ = muons[0].v + muons[1].v;
        float charge_check = muons[0].charge * muons[1].charge;

        if (charge_check >= 0) {
            return;
        }
        fill("mass", "Zinc0jet", pZ.M());
    }
}

void higgs_analyzer::write()
{
    TFile output("higgs_output.root", "RECREATE");
    histo_set::write();
    output.Close();
}

po::options_description higgs_analyzer::options()
{
    return po::options_description("Physics options");
}
