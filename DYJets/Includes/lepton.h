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
    bool passes_id;   ///< Does the lepton pass the Id cut?
    unsigned id;      ///< Id
    int pdgid;        ///< PDG ID (absolute value): electron = 11, muon = 13

    bool operator== (const lepton &other) const
    {
        return v == other.v
            && raw_v == other.raw_v
            && charge == other.charge
            && iso == other.iso
            && passes_id == other.passes_id
            && id == other.id
            && pdgid == other.pdgid;
    }
};

} // namespace physics

#endif // LEPTON_H
