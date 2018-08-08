#include "boson_jets_analyzer.h"

namespace physics
{

boson_jets_analyzer::boson_jets_analyzer(util::job::info &info,
                                         const util::options &opt) :
    EvtRunNum(info.reader, "EvtRunNum"),
    _rng(std::random_device()()),
    _triggers(info),
    _mask_eraBG(info, opt.config["triggers B-F"].as<std::string>()),
    _mask_eraH(info, opt.config["triggers G-H"].as<std::string>()),
    _weights(info)
{
    if (opt.config["tables B-F"]) {
        _tables_eraBF = opt.config["tables B-F"].as<util::tables>();
    }
    if (opt.config["tables G-H"]) {
        _tables_eraGH = opt.config["tables G-H"].as<util::tables>();
    }

    counter.declare("Total");
    counter.declare("Passing the trigger");
}

void boson_jets_analyzer::operator()()
{
    _weights.process_event();
    counter.count("Total", weights().global_weight());

    /*
     * Handle the trigger
     */
    if (!passes_trigger()) {
        return;
    }
    counter.count("Passing the trigger", weights().global_weight());

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

bool boson_jets_analyzer::passes_trigger()
{
    return era_select(_mask_eraBG, _mask_eraH).passes(_triggers);
}

} // namespace physics
