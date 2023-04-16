#ifndef BOSON_JETS_ANALYZER_H
#define BOSON_JETS_ANALYZER_H

#include <random>
#include <string>
#include <vector>

#include "BTagCalibrationStandalone.h"
#include "btagger.h"
#include "electrons.h"
#include "event_counter.h"
#include "genleps.h"
#include "histo_set.h"
#include "histo_set2D.h"
#include "jets.h"
#include "job.h"
#include "lepton.h"
#include "matched.h"
#include "muons.h"
#include "pileup.h"
#include "reweighing.h"
#include "tables.h"
#include "triggers.h"
#include "weights.h"

namespace physics
{

/// \brief Base class for boson-jets analyzers
class boson_jets_analyzer
{
    TTreeReaderValue<unsigned> run;
    TTreeReaderValue<unsigned long long> event;
    TTreeReaderValue<float> L1PreFiringWeight_Nom;
    TTreeReaderValue<float> L1PreFiringWeight_Up;
    TTreeReaderValue<float> L1PreFiringWeight_Dn;
protected:
    util::event_counter counter;
    util::histo_set histo_set;
    util::histo_set2D histo_set2D;

private:
    std::string _short_name, _long_name;

    util::tables _tables;

    trigger_mask _mask;

    // for the rejection of the low quality dimuon events
    bool _select_bestMuonTrigSF = true;
    trigger_mask _mask_sMu;
    trigger_mask _mask_dMu;
    double _pt_criteria_SMuDMu;

    genleps _genleps;
    muons _muons;
    electrons _electrons;
    jets _jets;
    pileup _pileup;
    btagger _btagger;

    bool _bjet_veto = true;
    bool _reject_lowQMu = false;

    bool _applyPref_ = false;
    int _mode_pref = 0; // 0: nominal, 1: up variation, -1: down variation

    bool _selectDYLL = false;
    int  _selectDYLL_flavor = -1;

    bool _apply_ptReweight = false;
    std::string _fileName_ptReweight;
    TH1D* _h_ptReweight;

    std::vector<double> _mass_bins;

    bool _apply_triggerSF = true;

    bool _apply_M100Cut = false;
    std::string _sample_name;

    physics::reweighing _reweighing;
    physics::weights _weights;

public:
    /// \brief Groups together the contents of a boson-jets event
    class event_contents
    {
      public:
        std::vector<lepton> leptons; ///< \brief Leptons making up the boson
        TLorentzVector boson_p;      ///< \brief Reconstructed boson 4-momentum
        TLorentzVector boson_p_LHE;      ///< \brief Reconstructed boson 4-momentum at LHE
        std::vector<jet> jets;       ///< \brief Jets in the event
        std::vector<jet> jets20;     ///< \brief Jets with 20 GeV cut

        /// \brief Returns the list of leptons making up the boson
        const std::vector<lepton> &get_leptons() const { return leptons; }

        /// \brief Returns the reconstructed boson 4-momentum
        TLorentzVector get_boson_p() const { return boson_p; }

        /// \brief Returns the list of jets
        const std::vector<jet> &get_jets() const { return jets; }
    };

    /// \brief Constructor
    boson_jets_analyzer(util::job::info &info, const util::options &opt);

    /// \brief Destructor
    virtual ~boson_jets_analyzer();

    /// \brief Entry point, called for every event
    virtual void operator()() final;

    /// \brief Function called at the end of the processing.
    virtual void write();

protected:
    /// \brief Applies trigger scale factors
    virtual void apply_trigger_sf(class weights &weights,
                                  const std::vector<physics::lepton> &leptons,
                                  bool use_smu_triggerSF) = 0;

    /// \brief Fills histograms
    virtual void fill(const util::matched<std::string> &tags,
                      const util::matched<event_contents> &evt);

    /**
     * \brief Reconstructs the boson candidate and returns its constituents.
     *
     * This method must be implemented in derived classes.
     */
    virtual std::vector<lepton> find_boson(const std::vector<lepton> &muons,
                                           const std::vector<lepton> &electrons) = 0;

    virtual std::vector<lepton> find_gen_boson(const std::vector<lepton> &genleps) = 0;

    /// \brief Checks whether the current event passes the trigger.
    virtual bool passes_trigger();

    /// \brief Fills histograms for an unfolded variable
    void fill_unfolded(const std::string &name,
                       const util::matched<std::string> &tags,
                       const util::matched<double> &value);

    util::tables tables() const { return _tables; }

    /// \brief Retrieves the weight information for the current event.
    const physics::weights &weights() const { return _weights; }

    bool check_lowQualityMuon(const std::vector<lepton> muons);
    bool check_whichTriggerSF(const std::vector<lepton> muons);
    bool _use_smu_triggerSF = false;
};

} // namespace physics

#endif // BOSON_JETS_ANALYZER_H
