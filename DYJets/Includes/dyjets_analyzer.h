#ifndef DYJETS_ANALYZER_H
#define DYJETS_ANALYZER_H

#include <boost/program_options/options_description.hpp>

#include <TH1D.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "boson_jets_analyzer.h"
#include "jets.h"
#include "job.h"
#include "pileup.h"
#include "zfinder.h"

namespace po = boost::program_options;

/// \brief Implements a \f$ Z \to 2l \f$ analysis.
class dyjets_analyzer : public physics::boson_jets_analyzer
{

    // TTreeReaderValue<unsigned long long> event;
    // TTreeReaderValue<unsigned> run;
    TTreeReaderValue<unsigned> luminosityBlock;

    // Used for background reweighting, check reweight_backgrounds()
    bool _reweight_emu_method = false;
    double _offset_emu_method = 1.0;
    double _slope_emu_method = 0.0;

    bool _reweight_same_sign_method = false;
    double _met_offset_ss_method = 2.0;
    double _met_slope_ss_method = 0.0;
    double _met_exp_offset_ss_method = 0.0;
    double _met_exp_slope_ss_method = 0.0;
    double _mass_offset_ss_method = 1.0;
    double _mass_slope_ss_method = 0.0;

  public:
    physics::zfinder _zfinder;

    /// \brief Constructor.
    explicit dyjets_analyzer(util::job::info &info, const util::options &opt);

    /// \brief Destructor.
    virtual ~dyjets_analyzer() = default;

    // Overridden from base class
    void apply_trigger_sf(physics::weights &weights,
                          const std::vector<physics::lepton> &leptons,
                          bool use_smu_triggerSF) override;
    /// \brief Fills histograms
    void fill(const util::matched<std::string> &tags,
              const util::matched<event_contents> &evt) override;

    // Overridden from base class
    std::vector<physics::lepton> find_boson(
        const std::vector<physics::lepton> &muons,
        const std::vector<physics::lepton> &electrons) override;

    std::vector<physics::lepton> find_gen_boson(
    const std::vector<physics::lepton> &genleps) override;

    /// \brief Returns the list of options supported by the analyzer.
    static po::options_description options();

    /// \brief Reweights the events depending on event properties (for emu method and same-sign method)
    void reweight_backgrounds(physics::weights &weights,
                              const std::string &sample_name,
                              const TLorentzVector &boson,
                              const TLorentzVector &met) override;
};

#endif // DYJETS_ANALYZER_H
