#ifndef LEPTON_H
#define LEPTON_H

#include <TLorentzVector.h>

/// \brief Namespace for physics objects.
namespace physics
{

/// \brief Represents a lepton.
class lepton
{
  public:
    TLorentzVector v; ///< Four-momentum
    TLorentzVector raw_v; ///< Four-momentum as provided by CMSSW (ie before applying corrections)
    float charge;     ///< Charge
    float iso;        ///< Relative isolation
    int id;           ///< Id
};
} // namespace physics

#endif // LEPTON_H
