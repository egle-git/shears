Pruner, an application from the Shears framework
------------------------------------------------

Author: Ph. Gras CEA/IRFU Saclay<br>
Creation date: Jul, 15<br>
Based on Squeezer from AnaNaS (Jan. 3, 11)

Build the code:
---------------

`make clean`<br>
`make`


Description
-----------

Pruner is a tool to skim Shears ROOT tree and produce a smaller trees, called Bonzai, from a big one, a Boabab or a Bonzai. The skimming is done on both the list of tree leaves stored per events and on the events. The list of branches is typically provided in a text file, while the event selection is done with a c++ function. Event can also be selected from a list of run and event id read from a text file.

Usage:
-----

Either use the default pruner application or use the Pruner class in your c++ code.

To get help on the squeezer application, execute: 
`pruner --help` to get help. 

Custom event selection is written in a C++ class derived from the Pruner class.

