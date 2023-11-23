#include "Common/ShearsComparator.h"
#include "Common/DYUncertainty.h"
#include "Common/DYPath.h"
#include "Pileup/estimate_unc_cov_pileup.cxx"

class UncEstimator_L1Pref : public UncEstimator_Pileup {
public:
  UncEstimator_L1Pref(TString channel) : 
  UncEstimator_Pileup(channel, "L1Pref") {}
};

void estimate_unc_cov_L1Pref() {
  UncEstimator_L1Pref estimator_ee("ee");
  estimator_ee.ProduceAndSave();

  UncEstimator_L1Pref estimator_mm("mm");
  estimator_mm.ProduceAndSave();
}