#include "ValidationTool.h"
#include "AccProducer.h"

void producePlot_acc() {
  // -- validation first
  // ValidationTool validationTool("./output");

  // TString shearsPath_base = "/Users/kplee/Research/Analysis/Logbook/231025_update_subtractDYFake/Summary";  
  // validationTool.Set_ShearsPath(shearsPath_base+"/UncAndCov_All_ee.root", shearsPath_base+"/UncAndCov_All_mm.root");

  // validationTool.Validate();

  // -- produce the acceptance results & uncertainties
  AccProducer producer("./output");
  producer.Set_OutputFileName("DYAcceptance.root");
  producer.ProduceAndSave();
}