# SFs with fitting on MC distribution for TnP

* Default SF (EGM official SF): TnP efficiency was estimated by counting in Z peak region (60 < m < 120 GeV)
* New SF in this directory: MC efficiency is estimated by **fitting** the distribution as well
  * i.e. data efficiency is *unchanged* from the official SF
  * RECO and ID SF: produced by ourselves
  * TRIG SF: from the differential Afb analysis: [link](https://github.com/sansan9401/root2roc/tree/master/root/ElectronEff_v18)
  * It makes better agreement between data vs. MC in the high m(ee) region
  * Reference: discussions in the DY meetings
    * https://indico.cern.ch/event/1480268/
    * https://indico.cern.ch/event/1484434/