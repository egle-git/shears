#ifndef ELECTRONS_H
#define ELECTRONS_H

#include <memory>
#include <vector>

#include <TTreeReaderArray.h>


#include "charge_misid.h"
#include "histo_set.h"
#include "job.h"
#include "lepton.h"
#include "options.h"
#include "tables.h"
#include "weights.h"

class Aepcor;

namespace physics
{

/// \brief Handles electrons.
class electrons
{
  public:
    /// \brief Represents electron IDs
    enum class id
    {
        none,   ///< \brief No check
        veto,   ///< \brief veto ID
        loose,  ///< \brief Loose ID
        medium, ///< \brief Medium ID
        tight,  ///< \brief Tight ID
        mva_wp80,
        mva_wp90,
    };

  private:
    TTreeReaderArray<float> Electron_pt;
    TTreeReaderArray<float> Electron_eta;
    TTreeReaderArray<float> Electron_phi;
    TTreeReaderArray<float> Electron_mass;
    TTreeReaderArray<int> Electron_charge;
    TTreeReaderArray<float> Electron_deltaEtaSC;
    TTreeReaderArray<int> Electron_cutBased;
    TTreeReaderArray<float>Electron_mvaFall17V2Iso;
    TTreeReaderArray<bool>Electron_mvaFall17V2Iso_WP80;
    TTreeReaderArray<bool>Electron_mvaFall17V2Iso_WP90;
    TTreeReaderArray<bool>Electron_mvaFall17V2Iso_WPL;
    TTreeReaderArray<float>Electron_eCorr;
    TTreeReaderArray<float>Electron_r9;
    

    double _pt_cut = 20;
    double _eta_cut = 2.4;
    double _iso_cut = 0.25;
    id _id_cut = id::mva_wp90;

    bool _id_sf_enabled = true;
    bool _reco_sf_enabled = true;
    bool _charge_misid_sf_enabled = false;

    physics::charge_misid _charge_misid;
    
    // -- "Rochester" correction of the electron version
    // -- unique_ptr gives error...
    std::shared_ptr<Aepcor> _eRoccor = nullptr;

  public:
    /// \brief Constructor.
    explicit electrons(util::job::info &info, const util::options &opt, util::histo_set &h);

    /// \brief Configures the analyzer from user input.
    void configure(const util::options &opt);

    /**
     * \brief Retrieves a list of all electrons in the current event.
     *
     * The list is already filtered according to config file options.
     */
    // std::vector<lepton> get( bool isData, int & nVetoElecs);
    std::vector<lepton> get(bool isData, const unsigned int runNum,
                            const vector<lepton>& vec_dressedGenLep,
                            const vector<lepton>& vec_postFSRGenLep,
                            int & nVetoElecs);    

    /**
     * \brief Reweighs an event to take scale factors into account.
     *
     * If enabled in the configuration file, the following tables will be used:
     *
     * - `muon id`
     * - `muon isolation`
     * - `muon tracking`
     *
     * It is the user responsibility to load them. An exception is thrown if they're not present.
     *
     * \param w Weights to be reweighed
     * \param electrons List of electrons to take into account
     * \param tab Tables to get the scale factors from
     *
     * \throws std::out_of_range if a table is enabled and not present.
     */
    void apply_sf(weights &w, const std::vector<lepton> &electrons, const util::tables &tab) const;

    /** \brief Applies the electron charge misID correction SF to a MC event
     * \param weights Weights to be reweighed
     * \param electrons List of electrons to take into account
     * \param genleps List of generator level leptons to be used for gen-matching
     */
    virtual void apply_charge_misid_sf(physics::weights &weights,
                                       const std::vector<physics::lepton> &_electrons,
                                       const std::vector<physics::lepton> &_genleps);

    /**
     * \brief Fills muon control plots.
     * \param electrons The list of electrons in the event.
     * \param tag       A tag to pass to \ref util::histo_set
     */
    void fill(util::histo_set &h,
              const std::string &tag,
              const std::vector<lepton> &electrons,
              const weights &w);

    /// \brief Writes histograms to the current directory.
    void write();

    bool _eRoccor_enabled = false;
    void apply_energyCorr_smp22010(lepton& l, 
                                   const bool isData, const double factorToRawE,
                                   const unsigned int runNum, const double r9, 
                                   const vector<lepton>& vec_dressedGenLep,
                                   const vector<lepton>& vec_postFSRGenLep);

    lepton matchedGenLepton(const lepton& l, const vector<lepton>& vec_genLep);
};
} // namespace physics

#endif // ELECTRONS_H
