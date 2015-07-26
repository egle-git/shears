Scripts to produce Baobab Ntuples
=================================

Author: Ph. Gras. CEA/Saclay


Usage:
-----

1. Set crab 3 environment
2. Set up Tuple enviroment following instructions from <https://github.com/UGent/Tupel/blob/Tupel_MiniAOD/Tupel/readme.txt>.
3. Run grow\_baobabs. See options in the next section.

Important: CMSSW environment must be set after the crab 3 environment. Otherwise you will inherit from a Python version too old for the Baobab production tools.

Different options of grow\_baobabs run:
--------------------------------------

You will need first to set the path to the file containing the catalog of datasets to process, in the format defined in [1]:

`./grow_baobabs.py --catalog datasets.txt`

To submit the jobs to produce the Boabab ntuple of the datasets listed in the catalog file, runs:

`./grow_baoabas.py --new-jobs`

To check running job and generate lumi section summary json file, use the --check option:

`./grow_baobabs.py --check`

The production is tracked in the baobab\_prod.sqlite3` database file. For a production, it is important to run grow\_baobabs.py always in the same disk directory as this database file is picked up form the working directory.

To list the content of the production database file, used the --list option:

`./grow_baobabs.py --list`

A [Crab 3](https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideCrab) directory is created for each GRID task (processing of a dataset, called also <it>job batch</i>). It is name crab\_ followed by the datasetname and the job batch id. Crab 3 commands can be used directly. Use the Crab option -d to specify the task directory.

The simple\_grow\_baobabs command
---------------------------------

The command simple\_grow\_baobabs is a simplified version of grow\_baobab, which can be used for private production. It does not keep track of the production as grow\_baobabs does and does have a built-in support for the integrated luminosity calucation. For a central production, grow\_baobabs should be used.

**Beware**: the destination directory, where the Baobab ntuples will be written to, is specified in the catalog file. If you copy a central production dataset.txt file for a private production, don't forget to edit the file and modify the *output directory* line.


[1] <https://twiki.cern.ch/twiki/bin/view/CMS/SmpVjBaobabProduction>
