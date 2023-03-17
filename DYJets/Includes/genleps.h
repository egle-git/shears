#ifndef GENLEPS_H
#define GENLEPS_H

#include <memory>
#include <random>
#include <vector>

#include <TTreeReaderArray.h>
#include <optional>
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


    std::optional<TTreeReaderArray<float>> GenDressedLepton_pt;
    std::optional<TTreeReaderArray<float>> GenDressedLepton_eta;
    std::optional<TTreeReaderArray<float>> GenDressedLepton_phi;
    std::optional<TTreeReaderArray<float>> GenDressedLepton_mass;
    std::optional<TTreeReaderArray<int>> GenDressedLepton_pdgId;
    std::optional<TTreeReaderArray<bool>> GenDressedLepton_hasTauAnc;
    std::optional<TTreeReaderArray<int>> GenPart_status;
    std::optional<TTreeReaderArray<int>> GenPart_statusFlags;
    std::optional<TTreeReaderArray<float>> GenPart_pt;
    std::optional<TTreeReaderArray<float>> GenPart_eta;
    std::optional<TTreeReaderArray<float>> GenPart_phi;
    std::optional<TTreeReaderArray<float>> GenPart_mass;
    std::optional<TTreeReaderArray<int>> GenPart_pdgId;
    std::optional<TTreeReaderArray<float>> LHEPart_pt;
    std::optional<TTreeReaderArray<float>> LHEPart_eta;
    std::optional<TTreeReaderArray<float>> LHEPart_phi;
    std::optional<TTreeReaderArray<float>> LHEPart_mass;
    std::optional<TTreeReaderArray<int>> LHEPart_pdgId;

    double _pt_cut = 20;
    double _eta_cut = 2.4;

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

    bool IsGivenFlavorDileptonEvent(int pdgID);

    std::vector<lepton> get_leptons_isLHE();
    //float get_zzmas_isLHE();
    std::vector<lepton> get_leptons_finalState();

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
