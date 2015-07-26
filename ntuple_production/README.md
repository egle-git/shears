Scripts to produce Baobab Ntuples
=================================

Author: Ph. Gras. CEA/Saclay


Usage:
-----

1. Set crab 3 environment
2. Set up Tuple enviroment following instructions from https://github.com/UGent/Tupel/blob/Tupel\_MiniAOD/Tupel/readme.txt.
3. Run grow\_baobabs. See options in the next section.

Important: CMSSW environment must be set after the crab 3 environment. Otherwise you will inherit from a Python version too old for the Baobab production tools.

Different options of grow\_baobabs run:
--------------------------------------

You will need first to set the path to the file containing the catalog of datasets to process, in the format defined in [1]:

./grow\_baobabs.py --catalog datasets.txt

To submit the jobs to produce the Boabab ntuple of the datasets listed in the catalog file, runs:

./grow\_baoabas.py --new-jobs

To check running job and generate lumi section summary json file, use the --check option:

./grow\_baobabs.py --check

The production is tracked in the baobab\_prod.sqlite3 database file. For a production, it is important to run grow\_baobabs.py always in the same disk directory as this database file is picked up form the working directory.

To list the content of the production database file, used the --list option:

./grow\_baobabs.py --list

A [Crab 3](https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideCrab) directory is created for each GRID task (processing of a dataset, called also <it>job batch</i>). It is name crab\_ followed by the datasetname and the job batch id. Crab 3 commands can be used directly. Use the Crab option -d to specify the task directory.

simple\_grow\_baobabs
---------------------

simple\_grow\_baobabs is a simple script to submit the jobs to produce Baobab ntuples. It does not keep track of the production as grow\_baobabs does. For a central production, grow\_baobabs should be used.


[1] <https://twiki.cern.ch/twiki/bin/view/CMS/SmpVjBaobabProduction>
