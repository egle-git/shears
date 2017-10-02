Instructions to install ntuple producer code of Tupel_MiniAOD branch
--------------------------------------------------------------------

* Set up a CMSSW environment. Current code is for CMSSW_7_6_X miniaod format. The code has been tested with CMSSW_7_6_4
```
cmsrel CMSSW_7_6_4
cd CMSSW_7_6_4/src
cmsenv
```
* Clone the github repository to download the code
`git clone -b Tupel_MiniAOD git@github.com:UGent/Tupel`
* Apply the CMSSW patch you wil find in https://gitlab.cern.ch/shears/shears/blob/master/ntuple_production/cmssw-patches/CMSSW_7_6_3_patch2-mod.tgz (works also for the version 7.6.4)
```
tar xzf CMSSW_7_6_3_patch2-mod.tgz
```
* Compile the code
```
scram b -j 10
```

Instructions to run the ntuple producer
---------------------------------------

To produce ntuple we recommend to use the tools documented in the [ntuple production tool repository](https://gitlab.cern.ch/shears/shears/blob/master/ntuple_production/README.md).

For tests the simple configuration file [simple_run_cfg.py](https://github.com/UGent/Tupel/blob/Tupel_MiniAOD/Tupel/simple_run_cfg.py), located this directory, can be used.

`cmsRun simple_run_cfg.py`

A more sophisticated configuration file, grow_baobab_cfg.py, used by the ntuple production tools can be found in the [ntuple production tool repository](https://gitlab.cern.ch/shears/shears/blob/master/ntuple_production/).

