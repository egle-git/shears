#include "zfinder.h"

#include "logging.h"

#include <cmath>

namespace physics
{

dilepton::dilepton(const lepton &a, const lepton &b)
    : v(a.v + b.v), charge_product(a.charge * b.charge), a(a), b(b)
{
}

zfinder::zfinder(const util::options &opt, const std::string &name)
{
    if (!opt.config[name] || !opt.config[name].IsMap()) {
        util::logging::warn << "Couldn't find configuration for Z finder \"" << name << "\". "
                            << "Will use the defaults." << std::endl;
        return;
    }
    const YAML::Node node = opt.config["name"];

    if (node["charge mode"]) {
        std::string mode = node["charge mode"].as<std::string>();
        if (mode == "none") {
            _charge_mode = charge_mode::none;
        } else if (mode == "neutral") {
            _charge_mode = charge_mode::neutral;
        } else if (mode == "same sign") {
            _charge_mode = charge_mode::same_sign;
        } else {
            throw std::invalid_argument("Invalid charge mode for Z finder \"" + name + "\": " +
                                        mode);
        }
    }

    util::set_value_safe(node, _mass_low, "low mass", "low mass for Z finder \"" + name + "\"");
    util::set_value_safe(node, _mass_high, "high mass", "high mass for Z finder \"" + name + "\"");
}

std::vector<dilepton> zfinder::find(const std::vector<lepton> &inputs) const
{
    std::vector<dilepton> list;
    for (auto ita = inputs.cbegin(); ita != inputs.cend(); ++ita) {
        for (auto itb = ita + 1; itb != inputs.cend(); ++itb) {
            dilepton candidate(*ita, *itb);
            if (valid(candidate)) {
                list.push_back(candidate);
            }
        }
    }
    return list;
}

bool zfinder::valid(const dilepton &candidate) const
{
    if (_charge_mode == charge_mode::neutral && candidate.charge_product > 0) {
        return false;
    } else if (_charge_mode == charge_mode::same_sign && candidate.charge_product < 0) {
        return false;
    }
    double mass = candidate.v.M();
    return _mass_low < mass && mass < _mass_high;
}

} // namespace physics
