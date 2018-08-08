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
    _muons(info, opt, histo_set),
    _electrons(info, opt, histo_set),
    _jets(info, opt),
    _pileup(info, opt),
    _weights(info)
{
    if (opt.config["tables B-F"]) {
        _tables_eraBF = opt.config["tables B-F"].as<util::tables>();
    }
    if (opt.config["tables G-H"]) {
        _tables_eraGH = opt.config["tables G-H"].as<util::tables>();
    }

    _jets.declare_histograms(histo_set);
    _pileup.declare_histograms(histo_set);

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

    /*
     * Read leptons and find the boson
     */
    std::vector<lepton> muons = _muons.get(weights().isdata(), rng());
    std::vector<lepton> electrons = _electrons.get();

    std::vector<lepton> leptons = find_boson(muons, electrons);
    if (leptons.empty()) {
        return;
    }

    // Create lists of chosen muons and electrons
    std::vector<lepton> chosen_muons, chosen_electrons;
    std::copy_if(leptons.begin(), leptons.end(), std::back_inserter(chosen_muons),
                 [](const lepton &lep) { return lep.pdgid == 13; });
    std::copy_if(leptons.begin(), leptons.end(), std::back_inserter(chosen_electrons),
                 [](const lepton &lep) { return lep.pdgid == 11; });

    // Apply lepton scale factors
    _muons.apply_sf(_weights, chosen_muons, tables());
    _electrons.apply_sf(_weights, chosen_electrons, tables());

    // Fill lepton control plots
    _muons.fill(histo_set, "Zinc0jet_noweight", chosen_muons, weights());
    _electrons.fill(histo_set, "Zinc0jet_noweight", chosen_electrons, weights());

    /*
     * Handle jets and pileup
     */
    std::vector<jet> jets = _jets.get();
    _jets.veto(jets, leptons);

    _jets.fill(histo_set, "Zinc0jet_noweight", jets, weights());
    _pileup.fill(histo_set, "Zinc0jet_noweight", weights());

    _pileup.reweight(_weights);

    /*
     * Apply lepton trigger scale factors
     */
    apply_trigger_sf(_weights, leptons);

    /*
     * Fill histograms w.r.t. N_jets
     */
    // Exclusive
    if (jets.size() < 3) {
        std::stringstream ss;
        ss << "Zexc" << jets.size() << "jet";
        fill(ss.str(), leptons, jets);
    }

    // Inclusive
    for (unsigned njets = 0; njets < 3; ++njets) {
        std::stringstream ss;
        ss << "Zinc" << njets << "jet";
        fill(ss.str(), leptons, jets);
    }
}

bool boson_jets_analyzer::passes_trigger()
{
    return era_select(_mask_eraBG, _mask_eraH).passes(_triggers);
}

} // namespace physics
