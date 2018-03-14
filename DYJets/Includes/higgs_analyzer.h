#ifndef HIGGS_ANALYZER_H
#define HIGGS_ANALYZER_H

#include <boost/program_options/options_description.hpp>

#include <TH1D.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "histo_set.h"
#include "jets.h"
#include "job.h"
#include "muons.h"
#include "pileup.h"
#include "tables.h"
#include "triggers.h"
#include "weights.h"

namespace po = boost::program_options;

/// \brief Implements a \f$ H \to 4l \f$ analysis.
class higgs_analyzer : private virtual util::histo_set
{
    TTreeReaderValue<unsigned> EvtRunNum;

    physics::jets _jets;
    physics::muons _muons;
    physics::pileup _pileup;

    util::tables _tables_eraBF;
    util::tables _tables_eraGH;

    physics::trigger_values _triggers;

    physics::trigger_mask _mask_eraBG;
    physics::trigger_mask _mask_eraH;

    physics::weights _weights;

  public:
    /// \brief Constructor.
    explicit higgs_analyzer(util::job::info &info, const util::options &opt);

    /// \brief Function called for every event.
    void operator()();

    /// \brief Checks whether the current event passes the trigger.
    bool passes_trigger();

    /// \brief Function called at the end of the processing.
    void write();

    /// \brief Returns the list of options supported by the analyzer.
    static po::options_description options();
};

#endif // HIGGS_ANALYZER_H
