#include "btagger.h"

#include <cmath>

namespace physics
{

btagger::btagger(const util::options &opt, util::histo_set2D &h)
{
    if (!opt.config["b jet veto"]) {
        throw std::runtime_error("Missing mandatory section in config file: \"b jet veto\"");
    }
    double binx[]={0,20,30,50,100,200,1000};
    h.declare("bjetPtEta", "bjet p_{T} [GeV]", "bjet eta ", 6, binx, 5,-2.5,2.5);

    // Calibration file
    std::string calib_file = "EfficiencyTables/CSVv2_Moriond17_B_H.csv";
    if (opt.config["b jet veto"]["scale factor path"]) {
        calib_file = "EfficiencyTables/" +
                opt.config["b jet veto"]["scale factor path"].as<std::string>();
    }
    BTagCalibration calib("", calib_file);

    // Working point
    bjet_cut = "loose";
    BTagEntry::OperatingPoint wp = BTagEntry::OP_LOOSE;
    if (opt.config["b jet veto"]["working point"]) {
        bjet_cut = opt.config["b jet veto"]["working point"].as<std::string>();
    }
    if (bjet_cut == "loose") {
        _bjet_cut = 0.5426;
        wp = BTagEntry::OP_LOOSE;
        _bjet_tag_eff = { 0.848, 0.383, 0.095 };
    } else if (bjet_cut == "medium") {
        _bjet_cut = 0.8484;
        wp = BTagEntry::OP_MEDIUM;
        _bjet_tag_eff = { 0.674, 0.123, 0.010 };
    } else if (bjet_cut == "tight") {
        _bjet_cut = 0.9535;
        wp = BTagEntry::OP_TIGHT;
        _bjet_tag_eff = { 0.484, 0.016, 0.001 };
    } else {
        throw std::runtime_error("Unkown b jet veto working point: " + bjet_cut);
    }

    _btag_calibration_reader = BTagCalibrationReader(wp, "central", {"up", "down"});
    _btag_calibration_reader.load(calib, BTagEntry::FLAV_B, "mujets");
    _btag_calibration_reader.load(calib, BTagEntry::FLAV_C, "mujets");
    _btag_calibration_reader.load(calib, BTagEntry::FLAV_UDSG, "mujets");
}

bool btagger::any(const std::vector<jet> &jets, weights &w, util::histo_set2D &h, const util::tables &t) const
{
   //cout<<"NEW EVENT"<<endl;
    double wu=-999.;
    for (const auto &jet : jets) { 
        apply_sf(jet, w, h,t,wu);
        fill_eff(jet, w, h,wu);
     //   cout<<jets.size()<<" "<<wu<<"  "<<w.global_weight()<<endl;
    }
    return std::any_of(jets.begin(),
                       jets.end(),
                       [&](const jet &j) { return j.bdisc > _bjet_cut; });
}

void btagger::fill_eff(const jet &j, weights &w, util::histo_set2D &h, double wu) const 
{
    if (w.isdata())return;
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
    bool tagged_loose = j.bdisc > 0.5426;
    bool tagged_medium = j.bdisc >0.8484 ;
    bool tagged_tight = j.bdisc >0.9535 ;
    h.fill("bjetPtEta", tg, j.v.Pt(),j.v.Eta(), wu);
    if(tagged_loose)    h.fill("bjetPtEta", tg+"_tagged_loose", j.v.Pt(),j.v.Eta(), wu);
    if(tagged_medium)    h.fill("bjetPtEta", tg+"_tagged_medium", j.v.Pt(),j.v.Eta(), wu);
    if(tagged_tight)    h.fill("bjetPtEta", tg+"_tagged_tight", j.v.Pt(),j.v.Eta(), wu);
}


void btagger::apply_sf(const jet &j, weights &w, util::histo_set2D &h,const util::tables &tab, double &wu ) const
{
    if (w.isdata())return;
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
    double eff = tab.at(tg + " "+bjet_cut+" eff").getEfficiency(j.v.Pt(), j.v.Eta());
    //std::cout <<tg + " "+bjet_cut+" eff "<<eff<<"  " <<j.v.Pt()<<" "<<j.v.Eta()<<std::endl;
    bool tagged = j.bdisc > _bjet_cut;
    if(tagged) tg+="_tagged";
    if(wu==-999.)wu=w.global_weight();
    //h.fill("bjetPtEta", tg, j.v.Pt(),j.v.Eta(), wu);

    double sf = _btag_calibration_reader.eval_auto_bounds(
        "central", flavor, std::abs(j.v.Eta()), j.v.Pt());
   // double eff = _bjet_tag_eff[flavor];

    w.use_weight(tagged ? sf : (1 - sf * eff) / (1 - eff));
//    cout<<tg<<" "<<tagged<<" "<<sf<<" "<< eff<<" "<<(tagged ? sf : (1 - sf * eff) / (1 - eff))<<endl;
    //cout<<wu<<"  "<<(tagged ? sf : (1 - sf * eff) / (1 - eff))<<"  "<<w.global_weight()<<endl;
}

} // namespace physics
