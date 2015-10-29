Quick start
============

Clone the shears project to your working directory:

    git clone ssh://git@gitlab.cern.ch:7999/shears/shears.git

Once you have cloned the repository, you should move to the DYJets directory and build the code:

```
    $ cd DYJets
    $ make clean
    $ make
```

Unless you have already your own configuration file, copy the example:

```
cp example.cfg vjet.cfg
```

(vjet.cfg stored on git is not guaranteed to work out-of-the box and we recommend to use example.cfg).

To run the Z+jet analysis, execute:

```
    $ ./runZJets_newformat
```

With the default configuration file, the histograms will be store in the HistoFiles directory.

To use as input ntuple in the Run I format, run `runZJets` instead of `runZJets_newformat`.

```
    $ ./runZJets
```

Configuration file
==================

Configuration is stored in vjet.cfg. A commented example can be found in example.cfg


Code organization
=================

The DYJets directory contains several runXXX.cc files, which have been compiles in executable called runXXX. Each of these executable runs the code implemented in the class XXX with the corresponding name and defined in Includes/XXX.h and Sources/XXX.cc. 
