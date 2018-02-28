#ifndef MUONS_H
#define MUONS_H

#include <TTreeReaderArray.h>

#include "lepton.h"

namespace physics
{

/// \brief A muon-only analysis.
class muons_analyzer
{
    TTreeReaderArray<float> MuPt;
    TTreeReaderArray<float> MuEta;
    TTreeReaderArray<float> MuPhi;
    TTreeReaderArray<float> MuE;
    TTreeReaderArray<float> MuCh;
    TTreeReaderArray<float> MuPfIso;
    TTreeReaderArray<unsigned> MuIdTight;

  public:
    /// \brief Constructor.
    explicit muons_analyzer(TTreeReader &reader);

    /// \brief Retrieves a list of all muons in the current event.
    std::vector<lepton> get_muons();
};
} // namespace physics

#endif // MUONS_H
