#ifndef MET_H
#define MET_H

#include <TTreeReaderValue.h>
#include <TLorentzVector.h>

#include "job.h"
#include "lepton.h"

namespace util
{
class histo_set;
class options;
} // namespace util

namespace physics
{

class weights;

/// \brief Handles pileup
class met
{
    TTreeReaderValue<float> MET_pt;
    TTreeReaderValue<float> MET_phi;

  public:
    /// \brief Constructor
    explicit met(util::job::info &info, const util::options &opt, util::histo_set &h);

    /// \brief Returns the MET 4-vector
    TLorentzVector v();

    /// \brief Returns the transverse mass of the leading lepton + MET system
    float mt(const physics::lepton &l);

    /// \brief Fills MET histograms
    void fill(util::histo_set &h, const std::string &tag, const physics::lepton &l, const weights &w);
};
} // namespace physics

#endif // PILEUP_H
