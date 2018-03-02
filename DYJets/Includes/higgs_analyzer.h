#ifndef HIGGS_ANALYZER_H
#define HIGGS_ANALYZER_H

#include <boost/program_options/options_description.hpp>

#include <TH1D.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "histo_set.h"
#include "job.h"
#include "muons.h"
#include "weights.h"

namespace po = boost::program_options;

/// \brief Implements a \f$ H \to 4l \f$ analysis.
class higgs_analyzer : private virtual util::histo_set,
                       private virtual physics::muons_analyzer,
                       private virtual physics::weights_analyzer
{
  public:
    /// \brief Constructor.
    explicit higgs_analyzer(util::job::info &info, const util::options &opt);

    /// \brief Function called for every event.
    void operator()();

    /// \brief Function called at the end of the processing.
    void write();

    /// \brief Returns the list of options supported by the analyzer.
    static po::options_description options();
};

#endif // HIGGS_ANALYZER_H
