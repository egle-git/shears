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
    float charge;     ///< Charge
    float iso;        ///< Relative isolation
    int id;           ///< Id
};
} // namespace physics

#endif // LEPTON_H
