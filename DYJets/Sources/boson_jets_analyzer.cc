#include "boson_jets_analyzer.h"

#include <algorithm>

#include "matched.h"

namespace physics
{

namespace /* anonymous */
{

/**
 * \brief Returns a tag of the form @c low_high where low and high are the bounds of the
 *        bin that contains @c value.
 *
 * Returns @c "" if the value doesn't fit in a bin.
 */
std::string make_tag(double value, const std::vector<double> &bins)
{
    auto low = std::lower_bound(bins.begin(), bins.end(), value);
    if (low == bins.end()) {
        // Out of bounds
        return "";
    }
    auto up = std::next(low);
    if (up == bins.end()) {
        // Out of bounds
        return "";
    }

    std::stringstream ss;
    ss << *low << "_" << *up;
    return ss.str();
}

} // anonymous namespace

boson_jets_analyzer::boson_jets_analyzer(util::job::info &info,
                                         const util::options &opt) :
    EvtRunNum(info.reader, "EvtRunNum"),
    _rng(std::random_device()()),
    _genleps(info, opt, histo_set),
    _triggers(info),
    _mask_eraBG(info, opt.config["triggers B-F"].as<std::string>()),
    _mask_eraH(info, opt.config["triggers G-H"].as<std::string>()),
    _muons(info, opt, histo_set),
    _electrons(info, opt, histo_set),
    _jets(info, opt),
    _pileup(info, opt),
    _btagger(opt,histo_set2D),
    _weights(info)
{
    if (opt.config["tables B-F"]) {
        _tables_eraBF = opt.config["tables B-F"].as<util::tables>();
    }
    if (opt.config["tables G-H"]) {
        _tables_eraGH = opt.config["tables G-H"].as<util::tables>();
    }

    if (!opt.config["b jet veto"]) {
        throw std::runtime_error("Missing mandatory section in config file: \"b jet veto\"");
    }

    util::set_value_safe(opt.config["b jet veto"], _bjet_veto, "use", "use b jet veto");

    if (opt.config["mass bins"]) {
        _mass_bins = opt.config["mass bins"].as<std::vector<double>>();
        std::sort(_mass_bins.begin(), _mass_bins.end());
    }

    _jets.declare_histograms(histo_set);
    _pileup.declare_histograms(histo_set);

    counter.declare("Total");
    counter.declare("Passing the trigger");
}

boson_jets_analyzer::~boson_jets_analyzer()
{}

void boson_jets_analyzer::operator()()
{
    _weights.process_event();
    counter.count("Total", weights().global_weight());

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

    std::vector<lepton> genleps = _genleps.get();
    std::vector<lepton> genleptons = find_gen_boson(genleps);

    util::matched<TLorentzVector> bosons;
    if (!genleptons.empty()) {
        // Gen boson found
        bosons.gen = std::accumulate(
            genleptons.begin(),
            genleptons.end(),
            TLorentzVector(),
            [](const TLorentzVector &p, const lepton &lep) { return p + lep.v; });

        _genleps.fill(histo_set, "geninc0jet_noweight", genleptons, weights());
    }

    /*
     * Handle the trigger
     */
    bool triggered = passes_trigger();
    if (!bosons.gen && !triggered) {
        // End early if nothing to do
        return;
    } else if (triggered) {
        counter.count("Passing the trigger", weights().global_weight());
    }

    /*
     * Read leptons and find the boson
     */
    std::vector<lepton> leptons;
    if (triggered) {
        std::vector<lepton> muons = _muons.get(weights().isdata(), rng());
        std::vector<lepton> electrons = _electrons.get();

        leptons = find_boson(muons, electrons);
        if (!leptons.empty()) {
            // Rec boson found
            bosons.rec = std::accumulate(
                leptons.begin(),
                leptons.end(),
                TLorentzVector(),
                [](const TLorentzVector &p, const lepton &lep) { return p + lep.v; });
        }
    }

    if (!bosons.gen && !bosons.rec) {
        // End early if nothing to do
        return;
    }

    /*
     * At this point at least one boson was found, either gen or rec.
     * Apply rec scale factors and load rec jets, then apply b tagging.
     */

    // Create lists of chosen muons and electrons to use for scale factors
    // If there's no rec boson, the lists are empty
    std::vector<lepton> chosen_muons, chosen_electrons;
    std::copy_if(leptons.begin(), leptons.end(), std::back_inserter(chosen_muons),
                 [](const lepton &lep) { return lep.pdgid == 13; });
    std::copy_if(leptons.begin(), leptons.end(), std::back_inserter(chosen_electrons),
                 [](const lepton &lep) { return lep.pdgid == 11; });

    // Apply lepton scale factors
    _muons.apply_sf(_weights, chosen_muons, tables());
    _electrons.apply_sf(_weights, chosen_electrons, tables());

    // Fill lepton control plots
    _muons.fill(histo_set, "inc0jet_noweight", chosen_muons, weights());
    _electrons.fill(histo_set, "inc0jet_noweight", chosen_electrons, weights());

    /*
     * Handle jets and pileup
     */
    util::matched<std::vector<jet>> jets;
    // TODO Gen jets
    if (bosons.rec) {
        jets.rec = _jets.get();
        _jets.veto(*jets.rec, leptons);

        // Calculate b efficiencies and apply scale factors
        if (_bjet_veto && _btagger.any(*jets.rec, _weights,histo_set2D)) {
            bosons.rec = boost::none;
            jets.rec = boost::none;
            if (!bosons.gen && !bosons.rec) {
                // End early if vetoed and no gen boson
                return;
            }
        }

        if (jets.rec) { // May have been zero'ed by b veto
            _jets.fill(histo_set, "inc0jet_noweight", *jets.rec, weights());
            _pileup.fill(histo_set, "inc0jet_noweight", weights());

            _pileup.reweight(_weights);
        }
    }

    /*
     * Apply lepton trigger scale factors
     */
    if (bosons.rec) {
        apply_trigger_sf(_weights, leptons);
    }

    /*
     * Fill histograms w.r.t. N_jets and invariant mass
     */
    auto mass_tags = bosons.apply(&TLorentzVector::M).apply(make_tag, _mass_bins);
    if (mass_tags.rec && !mass_tags.rec->empty()) {
        mass_tags.rec = "_mass" + *mass_tags.rec;
    }

    if (jets.rec) {
        auto &recjets = *jets.rec;

        // Exclusive
        if (recjets.size() < 3) {
            std::stringstream ss;
            ss << "exc" << recjets.size() << "jet";
            fill(ss.str(), leptons, recjets);
            fill(ss.str() + *mass_tags.rec, leptons, recjets);
        }

        // Inclusive
        for (unsigned njets = 0; njets < 3; ++njets) {
            std::stringstream ss;
            ss << "inc" << njets << "jet";
            fill(ss.str(), leptons, recjets);
            fill(ss.str() + *mass_tags.rec, leptons, recjets);
        }
    }
}

void boson_jets_analyzer::fill(const std::string &tag,
                               const std::vector<physics::lepton> &chosen_leptons,
                               const std::vector<physics::jet> &jets)
{
    _jets.fill(histo_set, tag, jets, weights());
    _pileup.fill(histo_set, tag, weights());

    // Create lists of chosen muons and electrons
    std::vector<lepton> chosen_muons, chosen_electrons;
    std::copy_if(chosen_leptons.begin(), chosen_leptons.end(), std::back_inserter(chosen_muons),
                 [](const lepton &lep) { return lep.pdgid == 13; });
    std::copy_if(chosen_leptons.begin(), chosen_leptons.end(), std::back_inserter(chosen_electrons),
                 [](const lepton &lep) { return lep.pdgid == 11; });

    // Fill lepton control plots
    _muons.fill(histo_set, tag, chosen_muons, weights());
    _electrons.fill(histo_set, tag, chosen_electrons, weights());
}

bool boson_jets_analyzer::passes_trigger()
{
    if (_weights.ismc()) {
        return _mask_eraH.passes(_triggers);
    }
    return era_select(_mask_eraBG, _mask_eraH).passes(_triggers);
}

void boson_jets_analyzer::write()
{
    counter.print();
    _weights.write(&histo_set);
    histo_set.write();
    histo_set2D.write();

}

} // namespace physics
