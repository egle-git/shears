#! /usr/bin/env bash
ls
shopt -s expand_aliases

. /cvmfs/cms.cern.ch/cmsset_default.sh

# Override architecture for SCRAM
export SCRAM_ARCH=slc6_amd64_gcc53

cd HZZ2l2nu

nbOfDiff=`ls result_* | grep png | wc -l`
wget "https://slack.com/api/chat.postMessage?token=$1&channel=slackci&text=nb%20of%20differences%20found=$nbOfDiff&pretty=1&username=shearsDocker"

tar cfvz allComparisons.tar.gz result_*
