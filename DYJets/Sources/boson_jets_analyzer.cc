#include "boson_jets_analyzer.h"

namespace physics
{

boson_jets_analyzer::boson_jets_analyzer(util::job::info &info, const util::options &opt)
    : EvtRunNum(info.reader, "EvtRunNum"),
      EvtNum(info.reader, "EvtNum"),
      _rng(std::random_device()()),
      _genleps(info, opt, histo_set),
      _triggers(info),
      _mask_eraBG(info, opt.config["triggers B-F"].as<std::string>()),
      _mask_eraH(info, opt.config["triggers G-H"].as<std::string>()),
      _muons(info, opt, histo_set),
      _electrons(info, opt, histo_set),
      _jets(info, opt),
      _pileup(info, opt),
      _btagger(opt, histo_set2D),
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

    _jets.declare_histograms(histo_set);
    _pileup.declare_histograms(histo_set);

    counter.declare("Total");
    counter.declare("Passing the trigger");
}

boson_jets_analyzer::~boson_jets_analyzer() {}

void boson_jets_analyzer::operator()()
{
    // if(!(*EvtNum==317714931 || *EvtNum==318251268 || *EvtNum==14697305 || *EvtNum==15162076 ||
    // *EvtNum==36896977)) return;
    _weights.process_event();
    counter.count("Total", weights().global_weight());
    double w0 = weights().global_weight();
    /*
     * Choose the right era for this event
     */
    const unsigned run_threshold = 278820u;              // start of Run G
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

    if (!genleptons.empty()) {
        _genleps.fill(histo_set, "genZinc0jet_noweight", genleptons, weights());
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
    std::vector<lepton> muons = _muons.get(weights().isdata(), rng(),genleps);
    std::vector<lepton> electrons = _electrons.get();

    std::vector<lepton> leptons = find_boson(muons, electrons);
    if (leptons.empty()) {
        return;
    } // means that a boson is found

    // Create lists of chosen muons and electrons
    std::vector<lepton> chosen_muons, chosen_electrons;
    std::copy_if(leptons.begin(),
                 leptons.end(),
                 std::back_inserter(chosen_muons),
                 [](const lepton &lep) { return lep.pdgid == 13; });
    std::copy_if(leptons.begin(),
                 leptons.end(),
                 std::back_inserter(chosen_electrons),
                 [](const lepton &lep) { return lep.pdgid == 11; });

    // Apply lepton scale factors
    _muons.apply_sf(_weights, chosen_muons, tables());
    _electrons.apply_sf(_weights, chosen_electrons, tables());
    double w1 = weights().global_weight() / w0;

    // Fill lepton control plots
    _muons.fill(histo_set, "Zinc0jet_noweight", chosen_muons, weights());
    _electrons.fill(histo_set, "Zinc0jet_noweight", chosen_electrons, weights());

    /*
     * Handle jets and pileup
     */
    std::vector<jet> jets = _jets.get(weights().isdata());
    _jets.veto(jets, leptons);

    // Calculate b efficiencies and apply scale factors
    if (_bjet_veto && _btagger.any(jets, _weights,histo_set2D,tables())) {
        return;
    }

    _jets.fill(histo_set, "Zinc0jet_noweight", jets, weights());
    _pileup.fill(histo_set, "Zinc0jet_noweight", weights());

    _pileup.reweight(_weights);
    double w2 = weights().global_weight() / (w0 * w1);

    /*
     * Apply lepton trigger scale factors
     */
    apply_trigger_sf(_weights, leptons);
    double w3 = weights().global_weight() / (w0 * w1 * w2);

    /*
     * Fill histograms w.r.t. N_jets and invariant mass
     */
    TLorentzVector boson_p;
    for (const auto &lepton : leptons) {
        boson_p += lepton.v;
    }
    if (chosen_muons.size() == 0) return;
    // std::cout<< chosen_muons[0].v.pt()<<" "<<  chosen_muons[0].v.eta() <<" "
    // <<chosen_muons[1].v.pt()<<" "<<  chosen_muons[1].v.eta()<<" "<<
    double boson_mass = boson_p.M();

    if (boson_mass > 71 && boson_mass < 111) {
        // std::cout<<"RUN_EVENT "<<*EvtRunNum<<" "<<*EvtNum<<"  "<<w0<<" "<<w1<<" "<<w2<<" "<<w3<<"
        // "<<_weights.global_weight()<<endl;
        // std::cout << "RUN_EVENT " << *EvtRunNum << " " << *EvtNum << " " << w1 << " " << w3 << "
        // "
        //          << w2 << " " << _weights.global_weight() << endl;
        // std::cout<< chosen_muons[0].v.Pt()<<" "<<  chosen_muons[0].v.Eta() <<" "
        // <<chosen_muons[1].v.Pt()<<" "<<  chosen_muons[1].v.Eta()<<" "<<chosen_muons[0].iso<<"
        // "<<chosen_muons[1].iso<<std::endl;
    }

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
    std::copy_if(chosen_leptons.begin(),
                 chosen_leptons.end(),
                 std::back_inserter(chosen_muons),
                 [](const lepton &lep) { return lep.pdgid == 13; });
    std::copy_if(chosen_leptons.begin(),
                 chosen_leptons.end(),
                 std::back_inserter(chosen_electrons),
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
