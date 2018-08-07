#ifndef BOSON_JETS_ANALYZER_H
#define BOSON_JETS_ANALYZER_H

#include <random>
#include <vector>

#include "histo_set.h"
#include "job.h"
#include "lepton.h"
#include "weights.h"

namespace physics
{

/// \brief Base class for boson-jets analyzers
class boson_jets_analyzer
{
    TTreeReaderValue<unsigned> EvtRunNum;

    std::mt19937 _rng;
    int _era;

protected:
    // TODO Make private
    physics::weights _weights;

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

    physics::weights weights() const { return _weights; }
protected:
    /**
     * \brief Returns a reference to the random number generator used by the
     *        analyzer.
     */
    std::mt19937 &rng() { return _rng; }

    /// \brief Selects one of two values based on the current era.
    template <class T> T &era_select(T &eraBG, T &eraGH) const
    {
        return era() == 0 ? eraBG : eraGH;
    }

    /**
     * \brief Returns the current era (0 for eraBG, 1 for eraGH).
     *
     * For MC events, the era is chosen randomly.
     */
    int era() const { return _era; }
};

} // namespace physics

#endif // BOSON_JETS_ANALYZER_H
