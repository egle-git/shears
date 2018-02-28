#ifndef HIGGS_ANALYZER_H
#define HIGGS_ANALYZER_H

#include <boost/program_options/options_description.hpp>

#include <TH1D.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include "histo_set.h"
#include "muons.h"

namespace po = boost::program_options;

/// \brief Implements a \f$ H \to 4l \f$ analysis.
class higgs_analyzer : private util::histo_set, private physics::muons_analyzer
{
  public:
    /**
     * \brief Constructor.
     * \param reader A \c TTreeReader that will iterate over all events.
     */
    explicit higgs_analyzer(TTreeReader &reader);

    /// \brief Function called for every event.
    void operator()();

    /// \brief Function called at the end of the processing.
    void write();

    /// \brief Returns the list of options supported by the analyzer.
    static po::options_description options();
};

#endif // HIGGS_ANALYZER_H
