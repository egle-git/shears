2016-Data Efficiency-Tables : 

All Muon efficiency-corrections for 2016-Data are based on the recommendations of Muon-POG for ICHEP 2016: 
Those recommendations are meant for 2016BCD. 2016EFG(H) have to be tested, and if necessary, updated.
https://twiki.cern.ch/twiki/bin/view/CMS/MuonWorkInProgressAndPagResults
"runMakeTable_2016.C" was used to create the efficiency-tables for Muon-ID, -ISO, -Trigger .

In addition to the Trigger-, TightID and ISO-corrections there is also a Tracking-correction for 2016 Data provided by the Tracking-POG: 
https://twiki.cern.ch/twiki/bin/view/CMS/MuonReferenceEffsRun2 
"runMakeTable_TGraph_2016.C" was used to create the muon tracking-efficiency table


Jet-Energy-Corrections were provided by JETMET-group: 
https://github.com/cms-jet/JECDatabase/tree/master/tarballs
"runMakeJECTable.C" was used to convert the original .txt -files into the format used by shears. 