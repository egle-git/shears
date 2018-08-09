#ifndef JETS_H
#define JETS_H

#include <vector>

#include <TLorentzVector.h>
#include <TTreeReaderArray.h>

#include "histo_set.h"
#include "job.h"
#include "lepton.h"
#include "options.h"
#include "weights.h"

namespace physics
{

/// \brief Represents a jet.
class jet
{
  public:
    TLorentzVector v; ///< Four-momentum
    float id;         ///< Jet ID
    float puMva;      ///< Result of the pileup MVA
    float bdisc;      ///< b-tag ID score
};

/// \brief Handles jets.
class jets
{
    TTreeReaderArray<float> JetAk04Pt;
    TTreeReaderArray<float> JetAk04Eta;
    TTreeReaderArray<float> JetAk04Phi;
    TTreeReaderArray<float> JetAk04E;
    TTreeReaderArray<float> JetAk04Id;
    TTreeReaderArray<float> JetAk04PuMva;
    TTreeReaderArray<float> JetAk04BDiscCisvV2;


    double _pt_cut = 30;
    double _eta_cut = 2.4;
    double _pumva_cut = -0.2;
    double _deltar_cut = 0.4;

  public:
    /// \brief Constructor.
    explicit jets(util::job::info &info, const util::options &opt);

    /// \brief Configures the analyzer from user input.
    void configure(const util::options &opt);

    /**
     * \brief Retrieves a list of all jets in the current event.
     *
     * The list is already filtered according to config file options.
     */
    std::vector<jet> get();

    /// \brief Vetoes \c jets too close to one of the given \c leptons.
    void veto(std::vector<jet> &jets, const std::vector<lepton> &leptons) const;

    /// \brief Declares histograms filled by this class.
    void declare_histograms(util::histo_set &h);

    /**
     * \brief Fills muon control plots.
     * \param jets The list of jets in the event.
     * \param tag   A tag to pass to \ref util::histo_set
     */
    void fill(util::histo_set &h,
              const std::string &tag,
              const std::vector<jet> &jets,
              const weights &w);

    /// \brief Writes histograms to the current directory.
    void write();
};
} // namespace physics

#endif // JETS_H
