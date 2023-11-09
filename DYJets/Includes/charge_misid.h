#ifndef CHARGE_MISID_H
#define CHARGE_MISID_H

#include "job.h"
#include "lepton.h"
#include "TH2D.h"

namespace physics
{

/**
 * \brief Holds the charge misidentification efficiency values.
 *
 * This class is used to correct the charge misidentification efficiency in MC
 * to allow for a proper data/MC comparison in the same-sign dielectron sample
 */
class charge_misid
{
private:
    std::unique_ptr<TH2D> _charge_misid_data;
    std::unique_ptr<TH2D> _charge_misid_mc;
    std::unique_ptr<TH2D> _charge_misid_sf;
    int _var = 0;

public:
    /// \brief Constructor.
  explicit charge_misid(const YAML::Node &node);

  /// \brief Empty constructor.
  charge_misid(){};

  /// \brief Returns the charge misidentification efficiency for a given electron's (pT, eta) in
  /// data.
  double get_charge_misid_data(const lepton &lep);

  /// \brief Returns the charge misidentification efficiency for a given electron's (pT, eta) in mc.
  double get_charge_misid_mc(const lepton &lep);

  /// \brief Returns the misID SF for the electron that had its charge misidentified (by checking
  /// with GEN)
  double get_sf(const lepton &lep);
};

} // namespace physics

#endif // CHARGE_MISID_H
