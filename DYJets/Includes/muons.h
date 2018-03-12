#ifndef MUONS_H
#define MUONS_H

#include <vector>

#include <TTreeReaderArray.h>

#include "histo_set.h"
#include "job.h"
#include "lepton.h"
#include "options.h"
#include "weights.h"

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

    double _pt_cut = 20;
    double _eta_cut = 2.4;
    double _iso_cut = 0.25;

  public:
    /// \brief Constructor.
    explicit muons_analyzer(util::job::info &info, const util::options &opt);

    /// \brief Configures the analyzer from user input.
    void configure(const util::options &opt);

    /**
     * \brief Retrieves a list of all muons in the current event.
     *
     * The list is already filtered according to config file options.
     */
    std::vector<lepton> get_muons();

    /**
     * \brief Fills muon control plots.
     * \param muons The list of muons in the event.
     * \param tag   A tag to pass to \ref util::histo_set
     */
    void fill_muons(const std::vector<lepton> &muons,
                    const weights &w,
                    const std::string &tag = "");

    /// \brief Writes histograms to the current directory.
    void write();
};
} // namespace physics

#endif // MUONS_H
