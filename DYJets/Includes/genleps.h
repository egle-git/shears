#ifndef GENLEPS_H
#define GENLEPS_H

#include <memory>
#include <random>
#include <vector>

#include <TTreeReaderArray.h>

#include "histo_set.h"
#include "job.h"
#include "lepton.h"
#include "options.h"
#include "tables.h"
#include "weights.h"

class RoccoR;

namespace physics
{

/// \brief Handles muons.
class genleps
{
  public:

  private:
    TTreeReaderArray<float> GenLepPt;
    TTreeReaderArray<float> GenLepEta;
    TTreeReaderArray<float> GenLepPhi;
    TTreeReaderArray<float> GenLepE;
    TTreeReaderArray<int> GenLepId;
    TTreeReaderValue<std::vector<bool>> GenLepPrompt;
    TTreeReaderValue<std::vector<bool>> GenLepTauProd;


    double _pt_cut = 20;
    double _eta_cut = 2.4;
    double _iso_cut = 0.25;

  public:
    /// \brief Constructor.
    explicit genleps(util::job::info &info, const util::options &opt, util::histo_set &h);

    /// \brief Configures the analyzer from user input.
    void configure(const util::options &opt);

    /**
     * \brief Retrieves a list of all muons in the current event.
     *
     * The list is already filtered according to config file options.
     */
    std::vector<lepton> get();

    /**
     * \brief Fills gen control plots.
     * \param gen The list of gen in the event.
     * \param tag   A tag to pass to \ref util::histo_set
     */
    void fill(util::histo_set &h,
              const std::string &tag,
              const std::vector<lepton> &genleps,
              const weights &w);

    /// \brief Writes histograms to the current directory.
    void write();
};
} // namespace physics

#endif // GENLEPS_H
