#include "met.h"

#include <boost/math/constants/constants.hpp>

#include "histo_set.h"
#include "options.h"
#include "weights.h"

namespace physics
{

met::met(util::job::info &info, const util::options &opt, util::histo_set &h)
    : MET_pt(info.reader, "MET_pt"),
      MET_phi(info.reader, "MET_phi")
{
    const double pi = boost::math::constants::pi<double>();

    h.declare("MET_pt", "MET p_{T};p_{T}^{miss} [GeV]", 30, 0, 300);
    h.declare("MET_phi", "MET #phi;MET #phi", 24, -pi, pi);
    h.declare("MT", "Transverse mass;M_{T}(l_{1}, MET) [GeV]", 25, 0, 500);
}

TLorentzVector met::v()
{
    TLorentzVector vec;
    vec.SetPtEtaPhiM(*MET_pt, 0, *MET_phi, 0);
    return vec;
}

float met::mt(const physics::lepton &l)
{
    return std::abs((v()+l.v).Mt());
}

void met::fill(util::histo_set &h, const std::string &tag, const physics::lepton &l, const weights &w)
{
    h.fill("MET_pt", tag, *MET_pt, w.global_weight());
    h.fill("MET_phi", tag, *MET_phi, w.global_weight());
    h.fill("MT", tag, mt(l), w.global_weight());
}
} // namespace physics
