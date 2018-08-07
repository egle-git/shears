#include "boson_jets_analyzer.h"

namespace physics
{

boson_jets_analyzer::boson_jets_analyzer(util::job::info &info,
                                         const util::options &opt) :
    EvtRunNum(info.reader, "EvtRunNum"),
    _rng(std::random_device()()),
    _weights(info)
{}

void boson_jets_analyzer::operator()()
{
    _weights.process_event();

    /*
     * Choose the right era for this event
     */
    const unsigned run_threshold = 278820u; // start of Run G
    const double run_lumi_fraction = 0.5493217216546642; // lumi fraction before run G

    std::uniform_real_distribution<> uniform(0.0, 1.0);

    if (weights().isdata()) {
        // Run number based era selection
        if (*EvtRunNum < run_threshold) {
            _era = 0;
        } else {
            _era = 1;
        }
    } else {
        // Monte-Carlo based era selection
        if (uniform(rng()) < run_lumi_fraction) {
            _era = 0;
        } else {
            _era = 1;
        }
    }

    analyze();
}

} // namespace physics
