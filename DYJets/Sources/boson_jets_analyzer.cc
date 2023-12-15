#include "boson_jets_analyzer.h"

#include <algorithm>
#include <boost/optional/optional_io.hpp>


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
    auto high = std::lower_bound(bins.begin(), bins.end(), value);
    // if (high == bins.begin() || high == bins.end()) {
    //     // Out of bounds
    //     return "";
    // }
    if( high == bins.begin() ) return "UF"; // underflow
    if( high == bins.end() ) return "OF"; // overflow
    auto low = std::prev(high);
    return std::to_string(int(*low)) + "_" + std::to_string(int(*high));
}

/**
 * \brief Retrieves the trigger list for the current @ref sample.
 */
std::string get_triggers(util::job::info &info, const util::options &opt)
{
    std::string trigger_list = "";

    // look for trigger list specified for this sample
    if( info.sample.has_triggers() )
        trigger_list = info.sample.triggers();
    else if( opt.config["triggers"] ) // if not, use the general trigger list in .yml file
        trigger_list = opt.config["triggers"].as<std::string>();

    return trigger_list;
}

// save bin edges of the given axis as array
void get_binEdgeArray(TAxis* axis, Int_t nBin, Double_t* arr_binEdge) {
    for(int i=0; i<nBin+1; ++i) {
        int i_bin = i+1;
        arr_binEdge[i] = axis->GetBinLowEdge(i_bin);
    }
}

} // anonymous namespace

boson_jets_analyzer::boson_jets_analyzer(util::job::info &info,
                                         const util::options &opt) :
    run(info.reader, "run"),
    event(info.reader, "event"),
    L1PreFiringWeight_Nom(info.reader, "L1PreFiringWeight_Nom"),
    L1PreFiringWeight_Up(info.reader, "L1PreFiringWeight_Up"),
    L1PreFiringWeight_Dn(info.reader, "L1PreFiringWeight_Dn"),
    _genleps(info, opt, histo_set),
    _mask(info, get_triggers(info, opt)),
    _mask_sMu(info),
    _mask_dMu(info),
    _muons(info, opt, histo_set),
    _electrons(info, opt, histo_set),
    _jets(info, opt),
    _met(info, opt, histo_set),
    _pileup(info, opt),
    _btagger(opt,histo_set2D),
    _reweighing(info, opt),
    _weights(info),
    _ssUncEstimator(_weights, histo_set, histo_set2D)
{
    if (opt.config["tables"])
        _tables = opt.config["tables"].as<util::tables>();

    if (!opt.config["b jet veto"]) {
        throw std::runtime_error("Missing mandatory section in config file: \"b jet veto\"");
    }

    util::set_value_safe(opt.config["b jet veto"], _bjet_veto, "use", "use b jet veto");

    if (!opt.config["prefiring weights"]) {
        throw std::runtime_error("Missing mandatory section in config file: \"prefiring weights\"");
    }
    util::set_value_safe(opt.config["prefiring weights"], _applyPref_, "use", "use apply prefiring weights");
    util::set_value_safe(opt.config["prefiring weights"], _mode_pref, "mode", "mode for L1 prefiring weights (0: nominal, 1: up variation, -1: down variation");
    if( !(_mode_pref == 0 || _mode_pref == -1 || _mode_pref == 1) )
        throw std::runtime_error("mode for L1 prefiring weights should be 0, 1 or -1");


    if( opt.config["select the best muon trigger SF"] ) {
        const YAML::Node node = opt.config["select the best muon trigger SF"];

        util::set_value_safe(node, _select_bestMuonTrigSF, "use", "select the best muon trigger SF depending on leading muon pt and fired trigger");
        if( _select_bestMuonTrigSF ) {
            _mask_sMu = physics::trigger_mask(info, node["single muon triggers"].as<std::string>());
            _mask_dMu = physics::trigger_mask(info, node["double muon triggers"].as<std::string>());
            _pt_criteria_SMuDMu = node["pt criteria"].as<double>();

            _reject_lowQMu = node["reject low quality muon events"].as<bool>();
        }
    }
    else
        _select_bestMuonTrigSF = false;


    if (opt.config["mass bins"]) {
        _mass_bins = opt.config["mass bins"].as<std::vector<double>>();
        std::sort(_mass_bins.begin(), _mass_bins.end());

        // check whether DY mass cut is consistent with the mass bin edges
        const YAML::Node node_Z = opt.config["Z"];
        double minM = node_Z["low mass"].as<double>();
        double maxM = node_Z["high mass"].as<double>();

        if( _mass_bins[0] != minM ) {
            std::cout << "lowest mass bin edge (" << _mass_bins[0] << ") != minimum DY mass cut (" << minM << ") ... need to be fixed" << std::endl;
            throw std::runtime_error("mass bin edge and mass cut are inconsistent");
        }

        if( _mass_bins.back() != maxM ) {
            std::cout << "highest mass bin edge (" << _mass_bins.back() << ") != maximum DY mass cut (" << maxM << ") ... need to be fixed" << std::endl;
            throw std::runtime_error("mass bin edge and mass cut are inconsistent");
        }
    }

    if( opt.config["Select specific flavor in DYLL"] ) {
        YAML::Node node = opt.config["Select specific flavor in DYLL"];

        _selectDYLL = node["use"].as<bool>();
        _selectDYLL_flavor = node["flavor"].as<int>();

        if( _selectDYLL ) {
            std::cout << "Select dilepton events with specific flavor(pdgID = " << _selectDYLL_flavor << ") from DY->ll sample" << std::endl;
            std::cout << "Turn off this option if you are running on data or non-DY samples" << std::endl;
        }
    }

    if( opt.config["dilepton pt reweighting"] ) {
        YAML::Node node = opt.config["dilepton pt reweighting"];

        _apply_ptReweight = node["use"].as<bool>();
        _fileName_ptReweight = node["reweighting factor path"].as<std::string>();

        if( _apply_ptReweight ) {
            TString fullPath = "./EfficiencyTables/" + _fileName_ptReweight;
            std::cout << "dilepton pt reweighting is ON (should be used for DY MC only!)" << std::endl;
            std::cout << "--> it will use the reweighting factor in " << fullPath << std::endl;

            TH1::AddDirectory(kFALSE); // -- histogram can be used even after closing the originated TFile
            TFile* f = TFile::Open(fullPath);
            _h_ptReweight = (TH1D*)f->Get("h_weight_dimuonPt")->Clone();
            f->Close();
        }
    }

    _apply_triggerSF = opt.config["use trigger scale factors"].as<bool>();

    _apply_M100Cut = opt.config["use exclusive mass binned DYJets samples"].as<bool>();
    _sample_name = info.sample.name();

    _jets.declare_histograms(histo_set);
    _pileup.declare_histograms(histo_set);

    setup_TUnfoldBinning(opt);
    get_era(opt);

    _ssUncEstimator.set_TUnfoldBinning(_trueBinning, _recoBinning);
    _ssUncEstimator.set_era(_era);

    // register the histogram names for systematic variation
    // multiple histogram names can be added
    _ssUncEstimator.add_systHistName("mass_wide_range_inc0jet");
    // _ssUncEstimator.add_systHistName("pt_inc0jet");
    // _ssUncEstimator.add_systHistName("rapidity_inc0jet");

    counter.declare("Total");
    counter.declare("Passing the trigger");
    counter.declare("With at least one fake electron"); // TEMPORARY
    counter.declare("With at least one fake muon"); // TEMPORARY
}

boson_jets_analyzer::~boson_jets_analyzer()
{}

void boson_jets_analyzer::operator()()
{
    // check before counting this event
    if( _selectDYLL ) {
        if( !_genleps.IsGivenFlavorDileptonEvent(_selectDYLL_flavor) ) return;
    }

    // check before counting this event
    // if( _apply_M100Cut && _sample_name == "DYJets_M-50to100" ) {
    //     if( DileptonMass_LHE() > 100 ) return;
    // }

    if( _apply_M100Cut ) {
        if( _sample_name == "DYJets_M-50to100" && 
            DileptonMass_LHE() > 100 ) return;
    }

    _weights.process_event();
    _reweighing.reweigh(_weights);
    counter.count("Total", weights().global_weight());

    // Event
    util::matched<event_contents> evt;
    std::vector<lepton> genleps = {};

    // distribution for all events in MC: get the true PU distribution
    if( !weights().isdata() )
        _pileup.fill(histo_set, "inc0jet_fullEvent", weights());

    // fill the generator level histograms only when the sample is MC
    if( !weights().isdata() ) {
        genleps = _genleps.get();
        std::vector<lepton> genleptons = find_gen_boson(genleps);

        if (!genleptons.empty()) {
            // Gen boson found
            evt.gen = event_contents();
            evt.gen->leptons = genleptons;
            evt.gen->boson_p = std::accumulate(
                genleptons.begin(),
                genleptons.end(),
                TLorentzVector(),
                [](const TLorentzVector &p, const lepton &lep) { return p + lep.v; });

            _genleps.fill(histo_set, "geninc0jet_noweight", genleptons, weights());
        }
    }

    /*
     * Handle the trigger
     */
    bool triggered = passes_trigger();
    if (!evt.gen && !triggered) {
        // End early if nothing to do
        return;
    } else if (triggered) {
        counter.count("Passing the trigger", weights().global_weight());
    }

    /*
     * Pre-firing weight
     */
    if( _weights.ismc() && _applyPref_ ) {
        switch (_mode_pref) {
        case 0:
            _weights.use_weight(*L1PreFiringWeight_Nom);
            // cout << "l1 prefiring weight: " << *L1PreFiringWeight_Nom << endl;
            break;
        case 1:
            _weights.use_weight(*L1PreFiringWeight_Up);
            break;
        case -1:
            _weights.use_weight(*L1PreFiringWeight_Dn);
            break;
        default:
            throw std::runtime_error("mode for L1 prefiring weights should be 0, 1 or -1");
            break;
        }
    }

    /*
     * Read leptons and find the boson
     */
    if (triggered) {
        int nVetoMuons=0;
        int nVetoElecs=0;

        // final state (post-FSR) gen-muons/electrons used for the Rochester correction
        std::vector<lepton> genleps_finalState;
        if( _weights.ismc() ) genleps_finalState = _genleps.get_leptons_finalState();
        else                  genleps_finalState.clear(); // data: no gen-leptons

        // -- dressed leptons, without any pt or eta cut (for the electron energy correction)
        std::vector<lepton> genleps_dressed_noCut;
        if( _weights.ismc() ) genleps_dressed_noCut = _genleps.get_leptons_dressed_noCut();
        else                  genleps_dressed_noCut.clear(); // data: no gen-leptons

        std::vector<lepton> muons = _muons.get(weights().isdata(), genleps_finalState, nVetoMuons);
        std::vector<lepton> electrons = _electrons.get(weights().isdata(), *run,
                                                       genleps_dressed_noCut, genleps_finalState, nVetoElecs);

        std::vector<lepton> leptons = find_boson(muons, electrons);
        if (!leptons.empty()&&(nVetoMuons+nVetoElecs)<=2) {
            // Rec boson found
            evt.rec = event_contents();
            evt.rec->leptons = leptons;
            evt.rec->boson_p = std::accumulate(
                leptons.begin(),
                leptons.end(),
                TLorentzVector(),
                [](const TLorentzVector &p, const lepton &lep) { return p + lep.v; });
        }
    }

    if (!evt.gen && !evt.rec) {
        // End early if nothing to do
        return;
    }

    // remove events with m(reco) < 10 GeV in both data and MC
    // because we only have the DY MC down to m=10 GeV, but data can go down below GeV
    // without this cut, the # underflow events in the response matrix could be significant different between data and MC
    if( evt.rec && evt.rec->boson_p.M() < 10 ) return;

    /*
     * At this point at least one boson was found, either gen or rec.
     * Apply rec scale factors and load rec jets, then apply b tagging.
     */

    // Create lists of chosen muons and electrons to use for scale factors
    if (evt.rec) {
        std::vector<lepton> chosen_muons, chosen_electrons;
        std::copy_if(evt.rec->leptons.begin(),
                     evt.rec->leptons.end(),
                     std::back_inserter(chosen_muons),
                     [](const lepton &lep) { return lep.pdgid == 13; });
        std::copy_if(evt.rec->leptons.begin(),
                     evt.rec->leptons.end(),
                     std::back_inserter(chosen_electrons),
                     [](const lepton &lep) { return lep.pdgid == 11; });

        if( _select_bestMuonTrigSF && _reject_lowQMu ) {
            Bool_t isLowQMuEvent = check_lowQualityMuon(chosen_muons);
            if( isLowQMuEvent ) return; // -- reject the event with low quality muons
        }

        if( _apply_ptReweight && _weights.ismc() &&
            (chosen_muons.size() >= 2 || chosen_electrons.size() >= 2) ) {

            auto pt = evt.apply(&event_contents::get_boson_p).apply((double (TLorentzVector::*)() const) &TLorentzVector::Pt);
            Int_t theBin = _h_ptReweight->GetXaxis()->FindBin(*pt.rec);

            Double_t weight = 1.0;
            if( theBin == 0 || theBin == _h_ptReweight->GetNbinsX()+1 ) weight = 1.0; // -- under or overflow: do not apply the weight
            else                                                        weight = _h_ptReweight->GetBinContent( theBin );

            _weights.use_weight(weight);
        }

        // Apply lepton scale factors
        _muons.apply_sf(_weights, chosen_muons, tables());
        _electrons.apply_sf(_weights, chosen_electrons, tables());

        // Apply weights for background estimation
        if (_weights.ismc())
        {
            std::vector<int> matches; // temporary
            _electrons.apply_charge_misid_sf(_weights, chosen_electrons, _genleps.get_leptons_finalState(), matches); // temporary
            if (matches.size() > 1) // temporary
                if (matches[0] < 0 || matches[1] < 0) // temporary
                    counter.count("With at least one fake electron", weights().global_weight()); // temporary
            if (chosen_muons.size()) { // temporary
                for (auto & mu : chosen_muons) { // temporary
                    auto genlep = _muons.matchedGenLepton(mu, _genleps.get_leptons_finalState()); // temporary
                    if (genlep.v.Pt() == 0 && genlep.v.Eta() == 0 && genlep.v.Phi() == 0) // temporary
                        counter. count("With at least one fake muon", weights().global_weight()); // temporary
                }
            } // temporary
        }
        reweight_backgrounds(_weights, _sample_name, evt.rec->get_boson_p(), _met.v());

        // Fill lepton control plots
        _muons.fill(histo_set, "inc0jet_noweight", chosen_muons, weights());
        _electrons.fill(histo_set, "inc0jet_noweight", chosen_electrons, weights());
        _met.fill(histo_set, "inc0jet_noweight", evt.rec->leptons[0], weights());

        // Check the pt of the leading muon to know which SF we will use
        if( _select_bestMuonTrigSF )
            _use_smu_triggerSF = check_whichTriggerSF(chosen_muons);
        else
            _use_smu_triggerSF = true; // always use single muon trigger SF
    }

    /*
     * Handle jets and pileup
     */
    if (evt.rec) {
        std::vector<lepton> l ;
        if (evt.gen) l = evt.gen->leptons;
        evt.rec->jets = _jets.get(weights().isdata(),l);
        evt.rec->jets20 = _jets.get(weights().isdata(), l, 20); // For b veto
        _jets.veto(evt.rec->jets, evt.rec->leptons);
        _jets.veto(evt.rec->jets20, evt.rec->leptons);

        // Calculate b efficiencies and apply scale factors
        if (_bjet_veto && _btagger.any(evt.rec->jets20, _weights, histo_set2D, tables())) {
            evt.rec = boost::none;
            if (!evt.gen && !evt.rec) {
                // End early if vetoed and no gen boson
                return;
            }
        }

        if (evt.rec) { // May have been zero'ed by b veto
            _jets.fill(histo_set, "inc0jet_noweight", evt.rec->jets, weights());
            _pileup.fill(histo_set, "inc0jet_noweight", weights());

            _pileup.reweight(_weights);
        }
    }
    if (evt.gen) {
        evt.gen->jets = _jets.getGen();
        _jets.veto(evt.gen->jets, evt.gen->leptons);
    }

    /*
     * Apply lepton trigger scale factors
     */
    if (evt.rec && _apply_triggerSF) {
        apply_trigger_sf(_weights, evt.rec->leptons, _use_smu_triggerSF);
    }

    // systematic variation information
    if( _weights.ismc() ) {
        _ssUncEstimator.add_systInfo("L1Pref", *L1PreFiringWeight_Nom, *L1PreFiringWeight_Up, *L1PreFiringWeight_Dn);
        _ssUncEstimator.add_systInfo("pileup", _pileup.weight(0), _pileup.weight(1), _pileup.weight(-1));
    }

    /*
     * Fill histograms w.r.t. N_jets and invariant mass
     */
    auto mass_tags = evt.apply(&event_contents::get_boson_p)
                        .apply(&TLorentzVector::M)
                        .apply(make_tag, _mass_bins);
    
    util::matched<std::string> mass_tags_fullRange;
    std::string str_fullRange = std::to_string(int(_mass_bins.front())) + "_" + std::to_string(int(_mass_bins.back())); // 50to1000

    if (mass_tags.rec && mass_tags.rec->empty()) {
        mass_tags.rec           = boost::none;
        mass_tags_fullRange.rec = boost::none;
    } else if (mass_tags.rec) {
        if( *mass_tags.rec != "UF" && *mass_tags.rec != "OF" )
            mass_tags_fullRange.rec = "mass" + str_fullRange;
        mass_tags.rec = "mass" + *mass_tags.rec;
    }
    if (mass_tags.gen && mass_tags.gen->empty()) {
        mass_tags.gen           = boost::none;
        mass_tags_fullRange.gen = boost::none;
    } else if (mass_tags.gen) {
        if( *mass_tags.gen != "UF" && *mass_tags.gen != "OF" )
            mass_tags_fullRange.gen = "mass" + str_fullRange;
        mass_tags.gen = "mass" + *mass_tags.gen;
    }        

    auto njets = evt.apply(&event_contents::get_jets)
                    .apply(&std::vector<jet>::size);

    // comment out filling histograms for exclusive jet cases: turn it on below lines if needed later
    // {
    //     // Exclusive
    //     util::matched<std::string> tags; // eg "exc1jet"
    //     util::matched<std::string> tags_mass; // eg "exc1jet_mass50_71"
    //     util::matched<std::string> tags_mass_fullRange; // eg "exc1jet_mass50_1000"

    //     if (njets.rec && *njets.rec < 3) {
    //         tags.rec = "exc" + std::to_string(*njets.rec) + "jet";
    //         if (mass_tags.rec)
    //             tags_mass.rec           = *tags.rec + "_" + *mass_tags.rec;
    //         if (mass_tags_fullRange.rec)
    //             tags_mass_fullRange.rec = *tags.rec + "_" + *mass_tags_fullRange.rec;
    //     }
    //     if (njets.gen && *njets.gen < 3) {
    //         tags.gen = "exc" + std::to_string(*njets.gen) + "jet";
    //         if (mass_tags.gen)
    //             tags_mass.gen           = *tags.gen + "_" + *mass_tags.gen;
    //         if (mass_tags_fullRange.gen)
    //             tags_mass_fullRange.gen = *tags.gen + "_" + *mass_tags_fullRange.gen;
    //     }

    //     if (tags.gen || tags.rec) {
    //         fill(tags, evt);
    //         if (tags_mass.gen || tags_mass.rec)
    //             fill(tags_mass, evt);
    //         if (tags_mass_fullRange.gen || tags_mass_fullRange.rec)
    //             fill(tags_mass_fullRange, evt);
    //     }
    // }

    // Inclusive
    // nj < 3 --> nj < 1 (only save inc0jet case): increase it if needed later
    for (std::size_t nj = 0; nj < 1; ++nj) {
        // Exclusive
        util::matched<std::string> tags; // eg "inc1jet"
        util::matched<std::string> tags_mass; // eg "inc1jet_mass50_71"
        util::matched<std::string> tags_mass_fullRange; // eg "inc1jet_mass50_1000"

        if (njets.rec && *njets.rec >= nj) {
            tags.rec = "inc" + std::to_string(nj) + "jet";
            if (mass_tags.rec)
                tags_mass.rec           = *tags.rec + "_" + *mass_tags.rec;
            if (mass_tags_fullRange.rec)
                tags_mass_fullRange.rec = *tags.rec + "_" + *mass_tags_fullRange.rec;
        }
        if (njets.gen && *njets.gen >= nj) {
            tags.gen = "inc" + std::to_string(nj) + "jet";
            if (mass_tags.gen)
                tags_mass.gen           = *tags.gen + "_" + *mass_tags.gen;
            if (mass_tags_fullRange.gen)
                tags_mass_fullRange.gen = *tags.gen + "_" + *mass_tags_fullRange.gen;
        }

        if (tags.gen || tags.rec) {
            fill(tags, evt);
            if (tags_mass.gen || tags_mass.rec)
                fill(tags_mass, evt);
            if (tags_mass_fullRange.gen || tags_mass_fullRange.rec)
                fill(tags_mass_fullRange, evt);
        }
    }

    // clear systInfo of this event at the end of the event process so that systInfo of the next event can be filled
    if( _weights.ismc() )
        _ssUncEstimator.clear_systInfo();
}

void boson_jets_analyzer::fill(const util::matched<std::string> &tags,
                               const util::matched<event_contents> &evt)
{
    if (tags.rec && evt.rec) {
        _jets.fill(histo_set, *tags.rec, evt.rec->jets, weights());
        _pileup.fill(histo_set, *tags.rec, weights());

        // Create lists of chosen muons and electrons
        std::vector<lepton> chosen_muons, chosen_electrons;
        std::copy_if(evt.rec->leptons.begin(), evt.rec->leptons.end(), std::back_inserter(chosen_muons),
                    [](const lepton &lep) { return lep.pdgid == 13; });
        std::copy_if(evt.rec->leptons.begin(), evt.rec->leptons.end(), std::back_inserter(chosen_electrons),
                    [](const lepton &lep) { return lep.pdgid == 11; });

        // Fill lepton control plots
        _muons.fill(histo_set, *tags.rec, chosen_muons, weights());
        _electrons.fill(histo_set, *tags.rec, chosen_electrons, weights());

        // Fill MET control plots
        _met.fill(histo_set, *tags.rec, evt.rec->leptons[0], weights());
        histo_set2D.fill("MET_vs_mass", *tags.rec, _met.v().Pt(), evt.rec->get_boson_p().M(), weights().global_weight());
    }
    if (tags.gen && evt.gen) {
        _genleps.fill(histo_set, *tags.gen, evt.gen->leptons, weights());
    }
}

void boson_jets_analyzer::fill_unfolded(const std::string &name,
                                        const util::matched<std::string> &tags,
                                        const util::matched<double> &value)
{

    // to fill TUnfold histograms
    // for now, it only works for mass binned histograms
    // need to be generalized later to fill pt & rapidity histograms as well
    bool isMass = (name == "mass_wide_range");
    Int_t binNum_true = -1;
    Int_t binNum_reco = -1;

    // Fill 1D distributions
    if (tags.rec && value.rec) {
        histo_set.fill(name, *tags.rec, *value.rec, weights().global_weight());

        if( isMass ) {
            binNum_reco = _recoBinning->GetGlobalBinNumber(*value.rec, _era);
            histo_set.fill("TUnfold1DReco", *tags.rec, binNum_reco, weights().global_weight());
            histo_set.fill("mass_wide_range_fineBin", *tags.rec, *value.rec, weights().global_weight());
        }
    }
    if (tags.gen && value.gen) {
        histo_set.fill(name, *tags.gen + "-gen", *value.gen, weights().gen_weight());

        if( isMass ) {
            binNum_true = _trueBinning->GetGlobalBinNumber(*value.gen);
            histo_set.fill("TUnfold1DTrue", *tags.gen, binNum_true, weights().gen_weight());
            histo_set.fill("mass_wide_range_fineBin", *tags.gen + "-gen", *value.gen, weights().gen_weight());
        }
    }

    // Fill response matrix
    if (tags.rec && tags.gen && value.rec && value.gen && tags.rec == tags.gen) {
        // both gen and reco exist and have the same tag
        // First fill both gen and reco with the global_weight
        histo_set2D.fill(name,
                         *tags.rec + "-matrix",
                         *value.rec,
                         *value.gen, weights().global_weight());
        // Now fill again subtracting the global weight from gen weight
        // And placing the event in the reco underflow bin
        // as explained in the TUnfold manual
        // https://www.desy.de/~sschmitt/TUnfold/tunfold_manual_v17.9.pdf, page 10
        histo_set2D.fill(name,
                         *tags.rec + "-matrix",
                         -10000.0,// underflow bin
                         *value.gen, weights().gen_weight()-weights().global_weight());

        if( isMass ) {
            histo_set2D.fill("TUnfold2DMig", *tags.rec, binNum_reco, binNum_true, weights().global_weight());
            histo_set2D.fill("TUnfold2DMig", *tags.rec, 0,           binNum_true, weights().gen_weight()-weights().global_weight());
        }
    }
    else{
        if(tags.rec && value.rec){
            // there is no gen corresponding to the reco event
            // reco event gets global_weight
            histo_set2D.fill(name,
                             *tags.rec + "-matrix",
                             *value.rec,
                             -10000.0, weights().global_weight());

            // fill the reco underflow bin with the (gen_weight - global_weight) according to the manual above
            histo_set2D.fill(name,
                             *tags.rec + "-matrix",
                             -10000.0,
                             -10000.0, weights().gen_weight()-weights().global_weight());

            if( isMass ) {
                histo_set2D.fill("TUnfold2DMig", *tags.rec, binNum_reco, 0, weights().global_weight());
                histo_set2D.fill("TUnfold2DMig", *tags.rec, 0,           0, weights().gen_weight()-weights().global_weight());
            }
        }
        if(tags.gen && value.gen){
            // there is no reco event
            // gen event gets gen_weight
            histo_set2D.fill(name,
                             *tags.gen + "-matrix",
                             -10000.0,
                             *value.gen, weights().gen_weight());

            if( isMass ) {
                histo_set2D.fill("TUnfold2DMig", *tags.gen, 0, binNum_true, weights().gen_weight());
            }
        }
    }
}

bool boson_jets_analyzer::passes_trigger()
{
    return _mask.passes();
}

void boson_jets_analyzer::write()
{
    counter.print();
    _weights.write(&histo_set);
    histo_set.write();
    histo_set2D.write();

}

bool boson_jets_analyzer::check_lowQualityMuon(const std::vector<lepton> muons) {
    bool flag = false;

    if( muons.size() < 2 ) // it is not a dimuon event: return false
        return flag;
    else { // dimuon event
        // first muon is always the leading muon (muon collection is sorted in decreasing pT after selection)
        if( muons[0].raw_v.Pt() < _pt_criteria_SMuDMu && _mask_sMu.passes() && !_mask_dMu.passes() )
            flag = true;
    }

    return flag;
}

bool boson_jets_analyzer::check_whichTriggerSF(const std::vector<lepton> muons) {
    // returns true == uses SMu trigger SF ; false == uses DiMu trigger SF
    // FIXME : For the moment, this function would apply DiMu trigger SF on the lowQuality events if they are not removed
    // FIXME : The trigger masks this function uses also depend on the switch for lowQuality events in yml file

    // -- not used if it is not a dimuon analysis
    if( muons.size() < 2 ) return false;

    bool smu_triggered = false;
    if ( muons[0].raw_v.Pt() > _pt_criteria_SMuDMu && _mask_sMu.passes()) smu_triggered = true;
    // this function will only be used for triggered events
    // if pt > _pt_criteria_SMuDMu and _mask_sMu doesn't pass then DiMu trigger SF is applied
    // if pt < _pt_criteria_SMuDMu then DiMu trigger SF is applied
    // note that if the lowQuality events are not removed this may cause a problem

    return smu_triggered;
}

double boson_jets_analyzer::DileptonMass_LHE() {
    double mass = -1.0;

    std::vector<lepton> genleps_isLHE = _genleps.get_leptons_isLHE();

    // does not check the lepton flavor
    // there should be only 2 leptons at LHE level if it is a DY sample
    if( genleps_isLHE.size() != 2 ) {
        throw std::runtime_error("# LHE leptons are not 2!");
        std::cout << "genleps_isLHE.size() = " << genleps_isLHE.size() << std::endl;
        return -1.0;
    }
    else {
        TLorentzVector vecP_dilepton = genleps_isLHE[0].v + genleps_isLHE[1].v;
        mass = vecP_dilepton.M();
        // std::cout << "[boson_jets_analyzer::DileptonMass_LHE] mass = " << mass << std::endl;
    }

    return mass;
}

void boson_jets_analyzer::setup_TUnfoldBinning(const util::options &opt) {

    // get the dilepton mass binning defined in .yml file
    vector<double> vec_binEdge = get_inputBin(opt, "mass_wide_range_inc0jet");

    // convert from vector to array
    int nBin = (int)vec_binEdge.size()-1;
    double* arr_binEdge = new double[nBin+1];
    std::copy(vec_binEdge.begin(), vec_binEdge.end(), arr_binEdge);

    // binning for era (reco only)
    //// era = 0: 2016, preAPV
    //// era = 1: 2016, postAPV
    //// era = 2: 2017
    //// era = 3: 2018
    const Int_t nEra = 4;
    Double_t arr_eraBin[nEra+1] = {-0.5, 0.5, 1.5, 2.5, 3.5};

    // printf("arr_binEdge\n");
    // for(Int_t i=0; i<nBin+1; i++)
    //     printf("%.1lf ", arr_binEdge[i]);
    // printf("\n");

    // define TUnfoldBinning
    // reco: two axes (mass, era)
    _recoBinningBase = new TUnfoldBinning("recoBinningBase");
    _recoBinning = _recoBinningBase->AddBinning("recoBinning");
    _recoBinning->AddAxis("mass", nBin, arr_binEdge,
                          true, // underflow bin
                          true // overflow bin
                          );

    _recoBinning->AddAxis("era", nEra, arr_eraBin,
                          false, // no underflow bin
                          false // no overflow bin
                          );

    // true: 1 axis (mass)
    _trueBinningBase = new TUnfoldBinning("trueBinningBase");
    _trueBinning = _trueBinningBase->AddBinning("trueBinning");
    _trueBinning->AddAxis("mass", nBin, arr_binEdge,
                          true, // underflow bin
                          true // overflow bin
                          );

    // create the histograms from TUnfoldBinning (1D)
    TH1D* h_reco = (TH1D*)_recoBinningBase->CreateHistogram("h_reco");
    TH1D* h_true = (TH1D*)_trueBinningBase->CreateHistogram("h_true");

    Int_t nBin_reco = h_reco->GetNbinsX();
    double* arr_binEdge_reco = new double[nBin_reco+1];
    get_binEdgeArray(h_reco->GetXaxis(), nBin_reco, arr_binEdge_reco);

    Int_t nBin_true = h_true->GetNbinsX();
    double* arr_binEdge_true = new double[nBin_true+1];
    get_binEdgeArray(h_true->GetXaxis(), nBin_true, arr_binEdge_true);

    histo_set.declare("TUnfold1DReco", "Reco dilepton mass;Reco mass bin number", nBin_reco, arr_binEdge_reco);
    histo_set.declare("TUnfold1DTrue", "True dilepton mass;True mass bin number", nBin_true, arr_binEdge_true);

    // also create 2D histogram from TUnfoldBinning
    // x-axis: reco
    // y-axis; true
    TH2D* h_respM = (TH2D*)TUnfoldBinning::CreateHistogramOfMigrations(_recoBinningBase, _trueBinningBase, "h_respM");

    Int_t nBinX = h_respM->GetNbinsX();
    double* arr_binEdgeX = new double[nBinX+1];
    get_binEdgeArray(h_respM->GetXaxis(), nBinX, arr_binEdgeX);

    Int_t nBinY = h_respM->GetNbinsY();
    double* arr_binEdgeY = new double[nBinY+1];
    get_binEdgeArray(h_respM->GetYaxis(), nBinY, arr_binEdgeY);

    histo_set2D.declare("TUnfold2DMig", "TUnfold2DMig", "Migration matrix", nBinX, arr_binEdgeX, nBinY, arr_binEdgeY);
}

// technical details: from histo_set::create_from_style
vector<double> boson_jets_analyzer::get_inputBin(const util::options &opt, std::string histName) {
    vector<double> vec_binEdge;

    std::string binning_file = "dyjets-binnings.yml";
    if (opt.config["binning file"]) {
        binning_file = opt.config["binning file"].as<std::string>();
    }

    YAML::Node config_bin = YAML::LoadFile(binning_file);
    util::style_list style(config_bin);
    std::string method = style.get<std::string>("binning", histName, "ignored");

    if( method == "uniform" ) {
        vec_binEdge.clear();
        int bin_count = style.get<int>("bin count", histName, 100);
        double binning_min = style.get<double>("binning min", histName);
        double binning_max = style.get<double>("binning max", histName);
        double binWidth = (binning_max - binning_min) / (double)bin_count;

        for(int i=0; i<bin_count+1; i++)
            vec_binEdge.push_back(binning_min + i*binWidth);
    }
    else if( method == "custom" ) {
        vec_binEdge = style.get<std::vector<double>>("bin edges", histName);
    }
    else {
        throw std::runtime_error("Undefined binning method '" + method +
                                 "' matched by histogram '" + histName + "'");

        return {0};
    }

    return vec_binEdge;
}

void boson_jets_analyzer::get_era(const util::options &opt) {
    std::string pileup_type = opt.config["pileup type"].as<std::string>();
    if( pileup_type == "2016_PreAPV" )       _era = 0;
    else if( pileup_type == "2016_PostAPV" ) _era = 1;
    else if( pileup_type == "2017" )         _era = 2;
    else if( pileup_type == "2018" )         _era = 3;
}

} // namespace physics
