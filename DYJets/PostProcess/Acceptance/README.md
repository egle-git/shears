# Acceptance

* Input: the outputs from [a custom EDAnalyzer code](https://github.com/KyeongPil-Lee/GENTool/blob/accCorr_DYFullRun2/EDTool/plugins/DYAcceptanceProducer.cc), to run on miniAOD (to access the full phase space information)
  * The path to the input should be specified in `path_inputForAcc` in `Common/DYPath.h`



* Output1: **Acceptance** result and corresponding plots
  * Code: `producePlot_acc.cxx`
  * Output root file: `DYAcceptance.root`
    * The path to this root file should be specified in `path_acc` in `Common/DYPath.h` for the other codes to access the acceptance value
* Output 2: **theory predictions** with **various PDFs**
  * Code: `produce_theoryPrediction.cxx`
  * Output root file: `TheoryPrediction_VariousPDF.root`, `TheoryPrediction_VariousPDF_aboveM200.root`
    * The path to this root file shoud be specified in `path_theoryPred` in `Common/DYPath.h` for the other codes