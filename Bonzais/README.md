Introduction
============

This folder contains the tools to make Bonzai (small ROOT tree including a preselection of events with a limited number of branches) from Baobab (ROOT tree with all the events of a CMS dataset with an extensive number of branches).

**OldFormat** For run 1 format and run 1 / run 2 format conversion.
**Pruner** Versatile skimmer using run 2 format.

In run 1 scheme Boabab and Bonzai have their own format. In run 2 scheme they share the same format, but the list of branches included in a Bonzai can be limited to a selected subset of branches.

Analysis code in the [https://gitlab.cern.ch/shears/shears/tree/master/DYJets][DYJets] directory support both Run 1 and Run 2 format, while the code in the [https://gitlab.cern.ch/shears/shears/tree/master/WJets][WJets] directory support only Run 2 format.



