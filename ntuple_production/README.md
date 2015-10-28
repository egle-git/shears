Scripts to produce Baobab Ntuples
=================================

Author: Ph. Gras. CEA/Saclay

Usage:
-----

1. Set crab 3 environment
2. Set up Tuple environment following instructions from <https://github.com/UGent/Tupel/blob/Tupel_MiniAOD/Tupel/readme.txt>.
3. Run grow\_baobabs. See options in the next section.

Important: CMSSW environment must be set after the crab 3 environment. Otherwise you will inherit from a Python version too old for the Baobab production tools.

For central production operation, you should also read the instructions specific to the central production which can be in, https://twiki.cern.ch/twiki/bin/view/CMS/SmpVjBaobabProduction.

Main options of grow\_baobabs command:
-------------------------------------

Add the `shears/ntuple_production` to your command search path PATH. 

The data sets to process, the json file in case of real data and the EOS destination directory for the produce ntuple need to be listed in a file in the format defined in [1]. Follows an example of the file content:

```
# output directory: /store/group/phys_smp/AnalysisFramework/Test/Ntuple
# json file: /afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions15/13TeV/Cert_*-*_13TeV_PromptReco_Collisions15_25ns_JSON.txt

/SingleMuon/Run2015D-PromptReco-v4/MINIAOD
```

The output directory path must end with the command

`grow_baobabs --set-catalog-path datasets.txt`

To submit the jobs to produce the Boabab ntuple of the datasets listed in the catalog file, runs:

`grow_baoabas --new-jobs`

To check running job and generate luminosity section summary json file, use the --check option:

`grow_baobabs --check`

The production is tracked in the baobab\_prod.sqlite3` database file. For a production, it is important to run grow\_baobabs always in the same disk directory as this database file is picked up form the working directory.

To list the content of the production database file, used the --list option:

`grow_baobabs --list`

A [Crab 3](https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideCrab) directory is created for each GRID task (processing of a dataset, called also <it>job batch</i>). It is name crab\_ followed by the dataset name and the job batch id. Crab 3 commands can be used directly. Use the Crab option -d to specify the task directory.

More options for grow\_baobabs command
--------------------------------------

Some usual options: 

--no-submit, to be used with --new-jobs to only creates the crab configuration files and register the crab tasks in the grow\_baobabs database. The job can then be submitted in a second step with the --submit command or using directly the CRAB3 crab command;

--task, to limit command to a given task: see in the online help to which commands it applies;

--resubmit, to resubmit jobs;

--no-crab-report-for-mc, to be used with the --check command to disable the crab report retrieval for MC dataset, which can take ages;

More options are available, run `grow_baobabs --help` to get the online help.

The simple\_grow\_baobabs command
-------------------------------

The command simple\_grow\_baobabs is a simplified version of grow\_baobab, which can be used for private production. It does not keep track of the production as grow\_baobabs does and does have a built-in support for the integrated luminosity calucation. For a central production, grow\_baobabs should be used.

**Beware**: the destination directory, where the Baobab ntuples will be written to, is specified in the catalog file. If you copy a central production dataset.txt file for a private production, don't forget to edit the file and modify the *output directory* line.


The mgrow\_boababs command
-------------------------

This command will run grow\_baobabs with the provided arguments in each directory defined in the prodrc configuration file (dirs variable). Online help can be obtained by running `warden --help`

The warden command
------------------

Warden is a command to monitor a Shear ntuple production. Online help can be obtained by running `warden --help`

The list of monitored directory is defined in the prodrc file located in the same directory than warden.

Note 1: when the warden command is run, it creates two files, warden_summary and warden_last_log in the directory it is executing from.

Note 2: if warden is running in background (loop mode), you can stop independently under which login it is running, with the warden --stop command. The command will work on two conditions: the warden command is located in the same directory that when it was started and you have write access to this directory.

The getlumi command
-------------------

This command will retrieve the integrated luminosity corresponding to a list of luminosity section provided in a json file. It is wrapper to CMS standard commands and is used by the grow\_boababs command.  It will be updated to follow latest recommendation for integrated luminosity calculation. Online help can be obtained by running `getlumi --help`.

The new-era command
-------------------

This command is used to set up work space on AFS and EOS for SMP Shears ntuple production. Online help can be obtained by running `new-era --help`.


The cmslog-inputfiles command
----------------------------

This is a utility to retrieve the list of input files used by a crab job from its log file. Online help can be obtained by running `cmslog-inputfile --help`.

The dumpPSetpkl command
-----------------------

This is a utility to dump the CMSSW configuration store by Crab in  input/PSet.pkl. Online help can be obtained by running `dumpPSetpkl --help`.

Other files
----------

send_shears_emails: this script is used by the warden command to send announcement of new ntuple to the cms-shears-ntuple-announcement egroup.

prodrc: configuration file for warden and mgrow\_boababs

[1] <https://twiki.cern.ch/twiki/bin/view/CMS/SmpVjBaobabProduction>
