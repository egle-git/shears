#ifndef WEIGHTS_H
#define WEIGHTS_H

#include "chains.h"
#include "histo_set.h"
#include "job.h"

#include <TTreeReader.h>
#include <TTreeReaderArray.h>

namespace physics
{

/**
 * \brief Analyzer for event weights.
 *
 * This class is responsible for the recording of processed event weights, lumiosity and cross
 * section. It does *not* mean that all histograms will be weighted automatically.
 */
class weights_analyzer : private virtual util::histo_set
{
    TTreeReaderArray<double> EvtWeights;

    long long _primary_events_in_chain = 0;
    long long _primary_events_total = 0;

    long long _processed_events = 0;
    long long _events_in_chain = 0;

    double _processed_weights_sum = 0;
    double _weights_sum_in_chain = 0;

    double _xsec;
    double _lumi;

  public:
    /// \brief Constructor.
    explicit weights_analyzer(util::job::info &info);

    /// \brief Call this for every processed event.
    void operator()();

    /// \brief Writes results to the current (ROOT) directory.
    void write();

    /// \brief Returns the size of the current weight vector.
    std::size_t size() { return EvtWeights.GetSize(); }

    /// \brief Returns the contents of the current weight vector at index \c i (checked).
    double at(std::size_t i) { return EvtWeights.At(i); }

    /// \brief Returns the contents of the current weight vector at index \c i (unchecked).
    double &operator[](std::size_t i) { return EvtWeights[i]; }
};
}

#endif // WEIGHTS_H
