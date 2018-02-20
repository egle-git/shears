#! /usr/bin/env bash

set -e

tool=$1
tarball=$2

branch=$CI_COMMIT_REF_NAME
commit=${CI_COMMIT_SHA:0:8}

curl -sf -F "tarball=@$tarball" \
    "http://m5.iihe.ac.be:5000/shears/upload?branch=$branch&commit=$commit&tool=$tool"
