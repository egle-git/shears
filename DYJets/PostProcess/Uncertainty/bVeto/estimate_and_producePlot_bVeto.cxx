#include "UncEstimator_bVeto.h"
#include "UncPlotProducer_bVeto.h"

void estimate_and_producePlot_bVeto(TString channel) {
  UncEstimator_bVeto estimator(channel);
  estimator.EstimateAndSave();

  UncPlotProducer_bVeto producer(channel);
  // producer.Set_InputFileName(estimator.OuputFileName());
  producer.Set_InputFileName("Unfolded_And_Uncertainty_bVeto_"+channel+".root");
  producer.Produce();
}

void estimate_and_producePlot_bVeto() {
  estimate_and_producePlot_bVeto("ee");
  estimate_and_producePlot_bVeto("mm");
}