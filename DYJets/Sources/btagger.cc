#include "btagger.h"

#include <cmath>

namespace physics
{

btagger::btagger(const util::options &opt)
{
    if (!opt.config["b jet veto"]) {
        throw std::runtime_error("Missing mandatory section in config file: \"b jet veto\"");
    }

    // Calibration file
    std::string calib_file = "EfficiencyTables/CSVv2_Moriond17_B_H.csv";
    if (opt.config["b jet veto"]["scale factor path"]) {
        calib_file = "EfficiencyTables/" +
                opt.config["b jet veto"]["scale factor path"].as<std::string>();
    }
    BTagCalibration calib("", calib_file);

    // Working point
    std::string bjet_cut = "loose";
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

bool btagger::any(const std::vector<jet> &jets, weights &w) const
{
    for (const auto &jet : jets) {
        apply_sf(jet, w);
    }
    return std::any_of(jets.begin(),
                       jets.end(),
                       [&](const jet &j) { return j.bdisc > _bjet_cut; });
}

void btagger::apply_sf(const jet &j, weights &w) const
{
    BTagEntry::JetFlavor flavor;
    if (std::abs(j.hadflav) == 5) {
        flavor = BTagEntry::FLAV_B;
    } else if (std::abs(j.hadflav) == 4) {
        flavor = BTagEntry::FLAV_C;
    } else {
        flavor = BTagEntry::FLAV_UDSG;
    }

    bool tagged = j.bdisc > _bjet_cut;
    double sf = _btag_calibration_reader.eval_auto_bounds(
        "central", flavor, std::abs(j.v.Eta()), j.v.Pt());
    double eff = _bjet_tag_eff[flavor];

    w.use_weight(tagged ? sf : (1 - sf * eff) / (1 - eff));
}

} // namespace physics
