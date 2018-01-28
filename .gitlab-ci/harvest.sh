#! /usr/bin/env bash
ls
shopt -s expand_aliases

. /cvmfs/cms.cern.ch/cmsset_default.sh

# Override architecture for SCRAM
export SCRAM_ARCH=slc6_amd64_gcc53

cd HZZ2l2nu
set nbOfDiff = `ls result_* | wc -l`
wget "https://slack.com/api/chat.postMessage?token=$1&pretty=1&username=shearsDocker"

tar cfvz allComparisons.tar.gz result_*
