#ifndef BTAGGER_H
#define BTAGGER_H

#include "BTagCalibrationStandalone.h"
#include "jets.h"
#include "job.h"
#include "options.h"
#include "weights.h"

namespace physics
{

/// \brief Wrapper around b-tagging utilities
class btagger
{
    double _bjet_cut = 0.5426;
    std::array<double, 3> _bjet_tag_eff;
    BTagCalibrationReader _btag_calibration_reader;

  public:
    /// \brief Constructor
    explicit btagger(const util::options &opt);

    /// \brief Checks whether any jet is a b jet and apply the b-tagging scale factors
    bool any(const std::vector<jet> &jets, weights &w) const;

  private:
    /// \brief Applies the b-tagging scale factors for the given jet
    void apply_sf(const jet &j, weights &w) const;
};

} // namespace physics

#endif // BTAGGER_H
