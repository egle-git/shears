#! /usr/bin/env bash

set -e

cmssw_location=$1
cmssw_version=CMSSW_8_0_32

[ -z "$cmssw_location" ] && cmssw_location="."

# Find where shears is located before cd'ing
script_location="$(dirname "$0")"
shears_location="$(readlink -e "$(dirname "$script_location")")"

# Go to the target location
cd "$cmssw_location"

# Install CMSSW
scramv1 project CMSSW "$cmssw_version" # cmsrel "$cmssw_version"
cd "$cmssw_version/src"
eval `scramv1 runtime -sh` # cmsenv

# Install dependencies
git cms-init

# MET
if false; then # FIXME: Disabled for CMSSW_8_0_30
  git cms-merge-topic HuguesBrun:METfiltersIn8027
fi

# Photons
if false; then # FIXME: Disabled for CMSSW_8_0_30
  git cms-merge-topic ikrav:egm_id_80X_v3_photons
fi

# Electrons
if true; then
  git cms-merge-topic cms-egamma:EGM_gain_v1
  git clone https://github.com/ECALELFS/ScalesSmearings.git \
      EgammaAnalysis/ElectronTools/data/ScalesSmearings \
      -b Moriond17_23Jan_v2 \
      --depth 0
fi

# Move shears to the CMSSW folder
mv "$shears_location" .
# And replace it with a soft link at the old location
ln -s "$(readlink -e ./shears)" "$shears_location"

# Build
scram b -j$(nproc)

