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

    if (!opt.config["b jet veto"]) {
        throw std::runtime_error("Missing mandatory section in config file: \"b jet veto\"");
    }

    util::set_value_safe(opt.config["b jet veto"], _bjet_veto, "use", "use b jet veto");

    // Calibration file
    std::string calib_file = "EfficiencyTables/CSVv2_Moriond17_B_H.csv";
    if (opt.config["b jet veto"]["scale factor path"]) {
        calib_file = "EfficiencyTables/" +
                opt.config["b jet veto"]["scale factor path"].as<std::string>();
    }
    BTagCalibration calib("", calib_file);

    // Working point
    std::string bjet_veto_cut = "loose";
    BTagEntry::OperatingPoint wp = BTagEntry::OP_LOOSE;
    if (opt.config["b jet veto"]["working point"]) {
        bjet_veto_cut = opt.config["b jet veto"]["working point"].as<std::string>();
    }
    if (bjet_veto_cut == "loose") {
        _bjet_veto_cut = 0.5426;
        wp = BTagEntry::OP_LOOSE;
        _bjet_tag_eff = { 0.848, 0.383, 0.095 };
    } else if (bjet_veto_cut == "medium") {
        _bjet_veto_cut = 0.8484;
        wp = BTagEntry::OP_MEDIUM;
        _bjet_tag_eff = { 0.674, 0.123, 0.010 };
    } else if (bjet_veto_cut == "tight") {
        _bjet_veto_cut = 0.9535;
        wp = BTagEntry::OP_TIGHT;
        _bjet_tag_eff = { 0.484, 0.016, 0.001 };
    } else {
        throw std::runtime_error("Unkown b jet veto working point: " + bjet_veto_cut);
    }

    _btag_calibration_reader = BTagCalibrationReader(wp, "central", {"up", "down"});
    _btag_calibration_reader.load(calib, BTagEntry::FLAV_B, "comb");
    _btag_calibration_reader.load(calib, BTagEntry::FLAV_C, "comb");
    _btag_calibration_reader.load(calib, BTagEntry::FLAV_UDSG, "comb");

    _jets.declare_histograms(histo_set);
    _pileup.declare_histograms(histo_set);

    counter.declare("Total");
    counter.declare("Passing the trigger");
    counter.declare("Total b-flavor jets");
    counter.declare("Total c-flavor jets");
    counter.declare("Total udsg-flavor jets");
    counter.declare("Total b-flavor jets, b-tagged");
    counter.declare("Total c-flavor jets, b-tagged");
    counter.declare("Total udsg-flavor jets, b-tagged");
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

    /*
     * Handle the trigger
     */
    if (!passes_trigger()) {
        return;
    }
    counter.count("Passing the trigger", weights().global_weight());

    /*
     * Read leptons and find the boson
     */
    std::vector<lepton> muons = _muons.get(weights().isdata(), rng());
    std::vector<lepton> electrons = _electrons.get();

    std::vector<lepton> leptons = find_boson(muons, electrons);
    if (leptons.empty()) {
        return;
    }// means that a boson is found

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

    // Calculate b efficiencies and apply scale factors
    if (_bjet_veto) {
         for (const auto &jet : jets){
             BTagEntry::JetFlavor flavor;
             bool tagged = jet.bdisc > _bjet_veto_cut;
            if(fabs(jet.hadflav)==5){
              counter.count("Total b-flavor jets", weights().global_weight());
                if (tagged) {
                    counter.count("Total b-flavor jets, b-tagged", weights().global_weight());
                }
              flavor = BTagEntry::FLAV_B;
           }
           else if(fabs(jet.hadflav)==4){
              counter.count("Total c-flavor jets", weights().global_weight());
                if (tagged) {
                    counter.count("Total c-flavor jets, b-tagged", weights().global_weight());
                }
              flavor = BTagEntry::FLAV_C;
           }
           else{
              counter.count("Total udsg-flavor jets", weights().global_weight());
                if (tagged) {
                    counter.count("Total udsg-flavor jets, b-tagged", weights().global_weight());
                }
                flavor = BTagEntry::FLAV_UDSG;
           }
            double sf = _btag_calibration_reader.eval_auto_bounds(
                "central", flavor, std::abs(jet.v.Eta()), jet.v.Pt());
            double eff = _bjet_tag_eff[flavor];

            _weights.use_weight(tagged ? sf : (1 - sf * eff) / (1 - eff));
        }

        // Veto events with b jets
        if (std::any_of(jets.begin(),
                        jets.end(),
                        [&](const jet &j) { return j.bdisc > _bjet_veto_cut; })) {
            return;
        }
    }

    _jets.fill(histo_set, "Zinc0jet_noweight", jets, weights());
    _pileup.fill(histo_set, "Zinc0jet_noweight", weights());

    _pileup.reweight(_weights);

    /*
     * Apply lepton trigger scale factors
     */
    apply_trigger_sf(_weights, leptons);

    /*
     * Fill histograms w.r.t. N_jets and invariant mass
     */
    TLorentzVector boson_p;
    for (const auto &lepton : leptons) {
        boson_p += lepton.v;
    }
    double boson_mass = boson_p.M();
    std::string mass_tag;
    if (boson_mass > 50 && boson_mass < 71) {
        mass_tag = "_mass50_71";
    } else if (boson_mass > 71 && boson_mass < 111) {
        mass_tag = "_mass71_111";
    } else if (boson_mass > 111 && boson_mass < 130) {
        mass_tag = "_mass111_130";
    } else if (boson_mass > 130 && boson_mass < 170) {
        mass_tag = "_mass130_170";
    } else if (boson_mass > 170 && boson_mass < 250) {
        mass_tag = "_mass170_250";
    } else if (boson_mass > 250 && boson_mass < 320) {
        mass_tag = "_mass250_320";
    }

    // Exclusive
    if (jets.size() < 3) {
        std::stringstream ss;
        ss << "Zexc" << jets.size() << "jet";
        fill(ss.str(), leptons, jets);
        fill(ss.str() + mass_tag, leptons, jets);
    }

    // Inclusive
    for (unsigned njets = 0; njets < 3; ++njets) {
        std::stringstream ss;
        ss << "Zinc" << njets << "jet";
        fill(ss.str(), leptons, jets);
        fill(ss.str() + mass_tag, leptons, jets);
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
    return era_select(_mask_eraBG, _mask_eraH).passes(_triggers);
}

void boson_jets_analyzer::write()
{
    counter.print();
    _weights.write(&histo_set);
    histo_set.write();
}

} // namespace physics
