#include "higgs_analyzer.h"

#include <TFile.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

higgs_analyzer::higgs_analyzer(TTreeReader &reader)
    : MuPt(reader, "MuPt"),
      MuEta(reader, "MuEta"),
      MuPhi(reader, "MuPhi"),
      MuE(reader, "MuE"),
      MuCh(reader, "MuCh"),
      MuPfIso(reader, "MuPfIso"),
      MuIdTight(reader, "MuIdTight")
{
}

void higgs_analyzer::operator()()
{
    std::vector<bool> good(MuPt->size());

    int nGoodLep = 0;
    for (unsigned i = 0; i < MuPt->size(); ++i) {
        float pt = MuPt->at(i);
        float eta = MuEta->at(i);
        float iso = MuPfIso->at(i);
        int id = MuIdTight->at(i);
        if (pt > 7 && iso < 0.25 && (id & 1) && std::abs(eta) <= 2.4) {
            good[i] = true;
            nGoodLep++;
        } else {
            good[i] = false;
        }
    }

    if (nGoodLep >= 2) {
        TLorentzVector pZ;
        float charge_check = 1;

        unsigned numLep = 0;
        for (unsigned i = 0; numLep < 2 && i < MuPt->size(); ++i) {
            if (good[i]) {
                numLep++;
                TLorentzVector p;
                p.SetPtEtaPhiE(MuPt->at(i), MuEta->at(i), MuPhi->at(i), MuE->at(i));
                pZ += p;
                charge_check *= MuCh->at(i);
            }
        }
        if (charge_check >= 0) {
            return;
        }
        histogram.Fill(pZ.M());
    }
}

void higgs_analyzer::write()
{
    TFile output("higgs_output.root", "RECREATE");

    histogram.Write();

    output.Close();
}

po::options_description higgs_analyzer::options()
{
    return po::options_description("Physics options");
}
