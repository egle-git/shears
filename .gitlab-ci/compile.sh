#! /usr/bin/env bash

eval $(.gitlab-ci/cmsenv.sh)

cd $1
make
