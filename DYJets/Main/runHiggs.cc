#include <algorithm>
#include <iostream>

#include <TChain.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "catalog.h"
#include "timer.h"

bool isZ(int charge, double mass) { return charge == 0 && mass > 60 && mass < 110; }

int main(int argc, char **argv)
{
    std::string fileName("Bonzais-DoubleMuon-all-VJetPruner-FourLep.txt");
    std::string bonzaiDir(
        "/store/group/phys_smp/AnalysisFramework/Bonzai/13TeV_2016/Data/v6/Catalogs/");

    catalog c(fileName, bonzaiDir, -1);

    std::shared_ptr<TTree> chain = c.event_chain();

    TTreeReader reader(chain.get());
    TTreeReaderValue<std::vector<float>> MuPt(reader, "MuPt");
    TTreeReaderValue<std::vector<float>> MuEta(reader, "MuEta");
    TTreeReaderValue<std::vector<float>> MuPhi(reader, "MuPhi");
    TTreeReaderValue<std::vector<float>> MuE(reader, "MuE");
    TTreeReaderValue<std::vector<float>> MuCh(reader, "MuCh");
    TTreeReaderValue<std::vector<float>> MuPfIso(reader, "MuPfIso");
    TTreeReaderValue<std::vector<unsigned>> MuIdTight(reader, "MuIdTight");

    TH1D histogram("Mass", "Mass", 100, 0, 300);

    timer time(reader.GetEntries(true));
    time.start();
    while (reader.Next()) {
        time.next();

        int nGoodLep = 0;
        for (unsigned i = 0; i < MuPt->size(); ++i) {
            float iso = MuPfIso->at(i);
            int id = MuIdTight->at(i);
            if (iso > 0.25 && (id & 1)) {
                nGoodLep++;
            }
        }
        if (nGoodLep == 4) {
            TLorentzVector p[4];
            double charge[4];
            for (int i = 0, j = 0; i < 4; ++i, ++j) {
                for (; MuPfIso->at(j) <= 0.25 && !(MuIdTight->at(i) & 1); ++j) {
                    // Do nothing
                }
                p[i].SetPtEtaPhiE(MuPt->at(j), MuEta->at(j), MuPhi->at(j), MuE->at(j));
                charge[i] = MuCh->at(j);
            }
            if (std::accumulate(charge, charge + 4, 0) != 0) {
                continue;
            }
            // Find Z
            bool hasz = false;
            hasz |= isZ(charge[0] + charge[1], (p[0] + p[1]).M());
            hasz |= isZ(charge[0] + charge[2], (p[0] + p[2]).M());
            hasz |= isZ(charge[0] + charge[3], (p[0] + p[3]).M());
            hasz |= isZ(charge[1] + charge[2], (p[1] + p[2]).M());
            hasz |= isZ(charge[1] + charge[3], (p[1] + p[3]).M());
            hasz |= isZ(charge[2] + charge[3], (p[2] + p[3]).M());
            if (!hasz) continue;

            TLorentzVector hp = std::accumulate(p, p + 4, TLorentzVector());
            histogram.Fill(hp.M());
        }
    }
    time.stop();

    histogram.SaveAs("Mass.cpp");

    return 0;
}
