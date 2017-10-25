Instructions to install ntuple producer code
--------------------------------------------

* Set up a CMSSW environment. Current code is for CMSSW_7_6_X miniaod format. The code has been tested with CMSSW_7_6_4
```
cmsrel CMSSW_7_6_4
cd CMSSW_7_6_4/src
cmsenv
```
* Compile the code
```
scram b -j 10
```

Instructions to run the ntuple producer
---------------------------------------

To produce ntuple we recommend to use the tools documented in the [ntuple production tool directory](../ntuple_production/).

For tests the simple configuration file [simple_run_cfg.py](./simple_run_cfg.py), located this directory, can be used.

`cmsRun simple_run_cfg.py`

A more sophisticated configuration file, grow_baobab_cfg.py, used by the ntuple production tools can be found in the [ntuple production tool directory](../ntuple_production/).

