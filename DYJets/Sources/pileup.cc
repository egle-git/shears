#include "pileup.h"

#include "histo_set.h"
#include "options.h"
#include "weights.h"

namespace physics
{

pileup::pileup(util::job::info &info, const util::options &opt)
    : Pileup_nTrueInt(info.init_optional_branch<decltype(Pileup_nTrueInt)>("Pileup_nTrueInt")),
      PV_npvsGood(info.reader, "PV_npvsGood"),
      _standalone_lrw(opt.config["pileup type"].as<std::string>(), 0),
      _standalone_lrw_dn(opt.config["pileup type"].as<std::string>(), -1),
      _standalone_lrw_up(opt.config["pileup type"].as<std::string>(), 1)
{
    util::set_value_safe(
        opt.config, _reweighing_enabled, "use pileup reweighing", "pileup reweighing toggle");
}

void pileup::reweight(weights &w)
{
    if (_reweighing_enabled && w.ismc()) {
        w.use_weight(_standalone_lrw.weight(**Pileup_nTrueInt));
        // w.use_gen_weight(_standalone_lrw.weight(**Pileup_nTrueInt)); # do not multiply PU weights to gen_weights yet
    }
}

double pileup::weight(int mode) {
    double weight = 1.0;

    if( mode == 0 )
        weight = _standalone_lrw.weight(**Pileup_nTrueInt);
    else if( mode == -1 )
        weight = _standalone_lrw_dn.weight(**Pileup_nTrueInt);
    else if( mode == 1 )
        weight = _standalone_lrw_up.weight(**Pileup_nTrueInt);

    return weight;
}

void pileup::declare_histograms(util::histo_set &h) const
{
    h.declare("nvtx", "Number of vertices;#Vtx", 60, 0.5, 60.5);
    h.declare("nTruePU", "Number of True PU;True PU", 99, 0, 99);
}

void pileup::fill(util::histo_set &h, const std::string &tag, const weights &w)
{
    h.fill("nvtx", tag, *PV_npvsGood, w.global_weight());
    if( w.ismc() )
        h.fill("nTruePU", tag, **Pileup_nTrueInt, w.global_weight());
}
} // namespace physics
