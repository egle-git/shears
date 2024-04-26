#include "btagger.h"

#include <cmath>

namespace physics
{

btagger::btagger(const util::options &opt, util::histo_set2D &h)
{
    if (!opt.config["b jet veto"]) {
        throw std::runtime_error("Missing mandatory section in config file: \"b jet veto\"");
    }
    const YAML::Node node = opt.config["b jet veto"];

    // Get era and corresponding working point
    std::string pileup_type = opt.config["pileup type"].as<std::string>();
    if( pileup_type == "2016_PreAPV" )       _era = 0;
    else if( pileup_type == "2016_PostAPV" ) _era = 1;
    else if( pileup_type == "2017" )         _era = 2;
    else if( pileup_type == "2018" )         _era = 3;

    // Efficiency tables
    double bins_pt[]={50,70,100,140,200,300,600,1000};
    double bins_eta[]={-2.5,-1.3,0,1.3,2.5};
    h.declare("bjetPtEta", "bjet p_{T} [GeV]", "bjet eta ", 7, bins_pt, 4, bins_eta);

    // Working point for DeepJet tagger
    if( node["working point"] ) {
        bjet_cut = node["working point"].as<std::string>();
        wp = BTagEntry::OP_MEDIUM;
        if(_era == 0) { // 2016preAPV
            _loose_cut = 0.0508, _medium_cut = 0.2598, _tight_cut = 0.6502;
        }
        if(_era == 1) { // 2016postV
            _loose_cut = 0.0480, _medium_cut = 0.2489, _tight_cut = 0.6377;
        }
        if(_era == 2) { // 2017
            _loose_cut = 0.0532, _medium_cut = 0.3040, _tight_cut = 0.7476;
        }
        if(_era == 3) { // 2018
            _loose_cut = 0.0490, _medium_cut = 0.2783, _tight_cut = 0.7100;
        }
        if( bjet_cut == "loose" )         _bjet_cut = _loose_cut, wp = BTagEntry::OP_LOOSE;
        else if( bjet_cut == "medium" )   _bjet_cut = _medium_cut, wp = BTagEntry::OP_MEDIUM;
        else if( bjet_cut == "tight" )    _bjet_cut = _tight_cut, wp = BTagEntry::OP_TIGHT;
        else                      throw std::invalid_argument("Unknown bjet cut : \"" + bjet_cut  + "\"");
    }

    // Calibration file
    calib_file = "EfficiencyTables/2018/wp_deepJet.csv";
    if ( node["scale factor path"] ) {
        calib_file = "EfficiencyTables/" +
                node["scale factor path"].as<std::string>();
    }
    BTagCalibration calib("", calib_file);

    _btag_calibration_reader = BTagCalibrationReader(wp, "central", {"up_correlated", "down_correlated",
            "up_uncorrelated","down_uncorrelated"});
    _btag_calibration_reader.load(calib, BTagEntry::FLAV_UDSG, "incl");
    _btag_calibration_reader.load(calib, BTagEntry::FLAV_C, "mujets");
    _btag_calibration_reader.load(calib, BTagEntry::FLAV_B, "mujets");
}

bool btagger::any(const std::vector<jet> &jets, weights &w, util::histo_set2D &h) const
{
    //for (const auto &jet : jets) {
    //    fill_eff(jet, w, h);
    //}
    return std::any_of(jets.begin(),
                       jets.end(),
                       [&](const jet &j) { return j.bdisc > _bjet_cut; });
}

void btagger::fill_eff(const jet &j, weights &w, util::histo_set2D &h) const
{
    if (w.isdata()) return;
    std::string tg="";
    BTagEntry::JetFlavor flavor;
    if (std::abs(j.hadflav) == 5) {
        flavor = BTagEntry::FLAV_B;
        tg="bjet";
    } else if (std::abs(j.hadflav) == 4) {
        flavor = BTagEntry::FLAV_C;
        tg="cjet";
    } else {
        flavor = BTagEntry::FLAV_UDSG;
        tg="udsgjet";
    }
    bool tagged_loose = j.bdisc > _loose_cut;
    bool tagged_medium = j.bdisc > _medium_cut;
    bool tagged_tight = j.bdisc > _tight_cut;
    h.fill("bjetPtEta", tg, j.raw_v.Pt(),j.raw_v.Eta(), w.global_weight());
    if(tagged_loose)    h.fill("bjetPtEta", tg+"_tagged_loose", j.raw_v.Pt(),j.raw_v.Eta(), w.global_weight());
    if(tagged_medium)    h.fill("bjetPtEta", tg+"_tagged_medium", j.raw_v.Pt(),j.raw_v.Eta(), w.global_weight());
    if(tagged_tight)    h.fill("bjetPtEta", tg+"_tagged_tight", j.raw_v.Pt(),j.raw_v.Eta(), w.global_weight());
}

double btagger::get_and_apply_bveto_weight_full_event(const std::vector<jet> &jets, weights &w, util::histo_set2D &h, const util::tables &t, std::string _sys, std::string _flavor_for_sys) const
{
    double bveto_final_weight_full_event = 1.;
    for (const auto &jet : jets) {
        bveto_final_weight_full_event *= get_and_apply_bveto_weight(jet, w, h, t, _sys, _flavor_for_sys);
    }
    return bveto_final_weight_full_event;
}

double btagger::get_and_apply_bveto_weight(const jet &j, weights &w, util::histo_set2D &h,const util::tables &tab, std::string _sys, std::string _flavor_for_sys) const
{
    if (w.isdata()) return 0;

    BTagEntry::JetFlavor flavor;
    std::string tg="";
    double eff = -1, sf = -1, bveto_final_weight = -1;

    if (std::abs(j.hadflav) == 5) {
        flavor = BTagEntry::FLAV_B;
        tg="bjet";
    } else if (std::abs(j.hadflav) == 4) {
        flavor = BTagEntry::FLAV_C;
        tg="cjet";
    } else {
        flavor = BTagEntry::FLAV_UDSG;
        tg="udsgjet";
    }

    //std::cout << "NEW JET --------------" << std::endl;
    //std::cout << "Jet pt : " << j.raw_v.Pt() << "; Jet eta : " << j.raw_v.Eta() << "; Jet flavor : " << tg << std::endl;

    // Evaluate scale factor
    sf = _btag_calibration_reader.eval_auto_bounds("central", flavor, std::abs(j.raw_v.Eta()), j.raw_v.Pt());

    // DEFAULT case
    if ( _sys == "central" ) {
        eff = tab.at(tg + " "+bjet_cut+" eff").getEfficiency(j.raw_v.Pt(), j.raw_v.Eta());
    }
    // Systematics on the MC eff
    else if ( _sys == "MC_eff_high" || _sys == "MC_eff_low" ) {
        if ( _sys == "MC_eff_low" ) {
            eff = tab.at(tg + " "+bjet_cut+" eff").getEfficiencyLow(j.raw_v.Pt(), j.raw_v.Eta());
        } else if ( _sys == "MC_eff_high" ) {
            eff = tab.at(tg + " "+bjet_cut+" eff").getEfficiencyHigh(j.raw_v.Pt(), j.raw_v.Eta());
        }
    }
    // Systematics on the POG SF
    else if (_sys == "up_correlated" || _sys == "down_correlated" || _sys == "up_uncorrelated"
            || _sys == "down_uncorrelated") {
        eff = tab.at(tg + " "+bjet_cut+" eff").getEfficiency(j.raw_v.Pt(), j.raw_v.Eta());
        if ( _flavor_for_sys == "heavy" && (tg=="bjet" || tg=="cjet")) {
            sf = _btag_calibration_reader.eval_auto_bounds(_sys, flavor, std::abs(j.raw_v.Eta()), j.raw_v.Pt());
        } else if ( _flavor_for_sys == "light" && tg == "udsgjet" ) {
            sf = _btag_calibration_reader.eval_auto_bounds(_sys, flavor, std::abs(j.raw_v.Eta()), j.raw_v.Pt());
        }
    }
    else throw std::invalid_argument("Invalid systematics for bveto : \"" + _sys + "\"");

    bveto_final_weight = (1 - sf * eff) / (1 - eff);

    //cout << "This jet weight is : " << bveto_final_weight << endl;
    //cout << "End of JET --------------" << endl;
    return bveto_final_weight;
}

} // namespace physics
