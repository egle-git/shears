#ifndef MUONS_H
#define MUONS_H

#include <vector>

#include <TTreeReaderArray.h>

#include "histo_set.h"
#include "lepton.h"

namespace physics
{

/// \brief A muon-only analysis.
class muons_analyzer : private virtual util::histo_set
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

    /**
     * \brief Fills muon control plots.
     * \param muons The list of muons in the event.
     * \param tag   A tag to pass to \ref util::histo_set
     */
    void fill_muons(const std::vector<lepton> &muons, const std::string &tag = "");

    /// \brief Writes histograms to the current directory.
    void write();
};
} // namespace physics

#endif // MUONS_H
