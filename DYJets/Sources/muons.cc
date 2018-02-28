#include "muons.h"

namespace physics
{

muons_analyzer::muons_analyzer(TTreeReader &reader)
    : MuPt(reader, "MuPt"),
      MuEta(reader, "MuEta"),
      MuPhi(reader, "MuPhi"),
      MuE(reader, "MuE"),
      MuCh(reader, "MuCh"),
      MuPfIso(reader, "MuPfIso"),
      MuIdTight(reader, "MuIdTight")
{
}

std::vector<lepton> muons_analyzer::get_muons()
{
    std::vector<lepton> muons;
    for (unsigned i = 0; i < MuPt.GetSize(); ++i) {
        lepton l;
        l.v.SetPtEtaPhiE(MuPt[i], MuEta[i], MuPhi[i], MuE[i]);
        l.charge = MuCh[i];
        l.iso = MuPfIso[i];
        l.id = MuIdTight[i];
        muons.push_back(l);
    }
    return muons;
}
}
