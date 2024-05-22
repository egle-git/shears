#ifndef BTAGGER_H
#define BTAGGER_H

#include "BTagCalibrationStandalone.h"
#include "jets.h"
#include "job.h"
#include "options.h"
#include "tables.h"
#include "weights.h"
#include "histo_set2D.h"

namespace physics
{

/// \brief Wrapper around b-tagging utilities
class btagger
{
    int _era = -1 ;
    double _bjet_cut = -1 ;
    double _loose_cut = -1, _medium_cut = -1, _tight_cut = -1;
    std::string bjet_cut = " " ;
    BTagEntry::OperatingPoint wp = BTagEntry::OP_MEDIUM ;
    std::string calib_file = " " ;
    BTagCalibrationReader _btag_calibration_reader ;

  public:
    /// \brief Constructor
    explicit btagger(const util::options &opt, util::histo_set2D &h);

    /// \brief Checks whether any jet is a b jet
    bool any(const std::vector<jet> &jets) const;
    /// \brief Compute the b-tagging scale factors for the event
    double get_bVetoSF_event(const std::vector<jet> &jets, bool isData,
                            const util::tables &tab,
                            std::string _sys = "central",
                            std::string _flavor_for_sys = "None") const;

    bool fillHist_mcTruthEff() const { return _fillHist_jet_mcTruthEff; }

    /// \brief fill histograms for MC-truth efficiency of jet-tagging (used for b-tagging SF)
    void fill_eff(const jet &j, weights &w, util::histo_set2D &h) const;

  private:
    /// \brief turn on fill_eff()
    bool _fillHist_jet_mcTruthEff = false; 

    /// \brief Compute and the b-tagging scale factors for the given jet
    double get_bVetoSF_perJet(const jet &j, bool isData,
                            const util::tables &tab,
                            std::string _sys = "central",
                            std::string _flavor_for_sys = "None") const;

    void decide_jetFlavorInfo(const jet& j, std::string& tg, BTagEntry::JetFlavor& flavor) const;




};

} // namespace physics

#endif // BTAGGER_H
