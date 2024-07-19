#!/bin/bash

# -- make ROOT recognize the header files defined 
# -- with a relative path w.r.t. the current directory
export ROOT_INCLUDE_PATH=${DY_ANALYZER_PATH}:$(pwd)
export PYTHONPATH=${DY_ANALYZER_PATH}:$(pwd)
export DYUNCPATH=$(pwd)/Uncertainty # -- used in Common/DYPath.h
export DYPPPATH=$(pwd) # -- PP = postprocess

echo "========================================"
echo "ROOT_INCLUDE_PATH: "$ROOT_INCLUDE_PATH
echo "PYTHONPATH: "$PYTHONPATH
echo "DYUNCPATH:  "$DYUNCPATH
echo "DYPPPATH:   "$DYPPPATH
echo "========================================"

