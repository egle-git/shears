#ifndef BTAGGER_H
#define BTAGGER_H

#include "BTagCalibrationStandalone.h"
#include "jets.h"
#include "job.h"
#include "options.h"
#include "tables.h"
#include "weights.h"
#include "histo_set2D.h"

namespace physics
{

/// \brief Wrapper around b-tagging utilities
class btagger
{
    int _era = -1 ;
    double _bjet_cut = -1 ;
    double _loose_cut = -1, _medium_cut = -1, _tight_cut = -1;
    std::string bjet_cut = " " ;
    BTagEntry::OperatingPoint wp = BTagEntry::OP_MEDIUM ;
    std::string calib_file = " " ;
    BTagCalibrationReader _btag_calibration_reader ;

  public:
    /// \brief Constructor
    explicit btagger(const util::options &opt, util::histo_set2D &h);

    /// \brief Checks whether any jet is a b jet
    bool any(const std::vector<jet> &jets, weights &w, util::histo_set2D &h) const;
    /// \brief Apply the b-tagging scale factors on the event
    double get_and_apply_bveto_weight_full_event(const std::vector<jet> &jets, weights &w,
                                                util::histo_set2D &h, const util::tables &tab,
                                                std::string _sys = "central",
                                                std::string _flavor_for_sys = "None") const;

  private:
    /// \brief Compute and applies the b-tagging scale factors for the given jet
    double get_and_apply_bveto_weight(const jet &j, weights &w, util::histo_set2D &h,
                                    const util::tables &tab,
                                    std::string _sys = "central",
                                    std::string _flavor_for_sys = "None") const;
    /// \brief Compute and applies the b-tagging scale factors for the given jet
    void fill_eff(const jet &j, weights &w, util::histo_set2D &h) const;

};

} // namespace physics

#endif // BTAGGER_H
