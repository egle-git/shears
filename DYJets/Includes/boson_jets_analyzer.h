#ifndef BOSON_JETS_ANALYZER_H
#define BOSON_JETS_ANALYZER_H

#include <vector>

#include "histo_set.h"
#include "job.h"
#include "lepton.h"

namespace physics
{

/// \brief Base class for boson-jets analyzers
class boson_jets_analyzer
{
public:
    /// \brief Constructor
    boson_jets_analyzer(util::job::info &info, const util::options &opt);

    /// \brief Destructor
    virtual ~boson_jets_analyzer() = default;

    /// \brief Entry point, called for every event
    virtual void operator()() final;

    /// \brief Function called for every event.
    virtual void analyze() = 0;

    /**
     * \brief Reconstructs the boson candidate and returns its constituents.
     *
     * This method must be implemented in derived classes.
     */
    virtual std::vector<lepton> find_boson(const std::vector<lepton> &muons,
                                           const std::vector<lepton> &electrons) = 0;
};

} // namespace physics

#endif // BOSON_JETS_ANALYZER_H
