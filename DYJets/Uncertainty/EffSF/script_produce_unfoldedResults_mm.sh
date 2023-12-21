#!bin/bash

root -l -b -q 'produce_unfoldedResults_mm.cxx("ID")'
echo "\nID: done\n"

root -l -b -q 'produce_unfoldedResults_mm.cxx("ISO")'
echo "\nISO: done\n"

root -l -b -q 'produce_unfoldedResults_mm.cxx("STRIG")'
echo "\nSTRIG: done\n"

root -l -b -q 'produce_unfoldedResults_mm.cxx("DTRIG")'
echo "\nDTRIG: done\n"

# hadd Unfolded_SystVar_EffSF_mm.root Unfolded_SystVar_EffSF_mm_*.root