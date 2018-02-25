#ifndef HIGGS_ANALYZER_H
#define HIGGS_ANALYZER_H

#include <stdexcept>

#include <boost/program_options/options_description.hpp>

#include <TH1D.h>
#include <TLorentzVector.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

namespace po = boost::program_options;

class higgs_analyzer
{
    TH1D histogram = TH1D("Mass", "Mass", 100, 0, 300);

    TTreeReaderValue<std::vector<float>> MuPt;
    TTreeReaderValue<std::vector<float>> MuEta;
    TTreeReaderValue<std::vector<float>> MuPhi;
    TTreeReaderValue<std::vector<float>> MuE;
    TTreeReaderValue<std::vector<float>> MuCh;
    TTreeReaderValue<std::vector<float>> MuPfIso;
    TTreeReaderValue<std::vector<unsigned>> MuIdTight;

    bool isZ(int charge, const TLorentzVector &p, TLorentzVector &pz)
    {
        if (charge == 0 && p.M() > 60 && p.M() < 110) {
            pz = p;
            return true;
        }
        return false;
    }

  public:
    higgs_analyzer(TTreeReader &reader)
        : MuPt(reader, "MuPt"),
          MuEta(reader, "MuEta"),
          MuPhi(reader, "MuPhi"),
          MuE(reader, "MuE"),
          MuCh(reader, "MuCh"),
          MuPfIso(reader, "MuPfIso"),
          MuIdTight(reader, "MuIdTight")
    {
    }

    void operator()()
    {
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
            TLorentzVector pHiggs;
            int charge[4];
            int chargeHiggs = 0;
            for (int i = 0, j = 0; i < 4; ++i, ++j) {
                float iso = MuPfIso->at(j);
                int id = MuIdTight->at(j);
                if (iso > 0.25 && (id & 1)) {
                    p[i].SetPtEtaPhiE(MuPt->at(j), MuEta->at(j), MuPhi->at(j), MuE->at(j));
                    pHiggs += p[i];
                    charge[i] = MuCh->at(j);
                    chargeHiggs += charge[i];
                    i++;
                }
            }
            if (chargeHiggs != 0) {
                return;
            }
            // Find Z
            bool hasz = false;
            TLorentzVector pz;
            hasz |= isZ(charge[0] + charge[1], p[0] + p[1], pz);
            hasz |= isZ(charge[0] + charge[2], p[0] + p[2], pz);
            hasz |= isZ(charge[0] + charge[3], p[0] + p[3], pz);
            hasz |= isZ(charge[1] + charge[2], p[1] + p[2], pz);
            hasz |= isZ(charge[1] + charge[3], p[1] + p[3], pz);
            hasz |= isZ(charge[2] + charge[3], p[2] + p[3], pz);
            if (!hasz) return;

            histogram.Fill(pz.M());
            throw std::invalid_argument("test");
        }
    }

    void write() { histogram.SaveAs("Mass.png"); }

    static po::options_description options() { return po::options_description("Physics options"); }
};

#endif // HIGGS_ANALYZER_H
