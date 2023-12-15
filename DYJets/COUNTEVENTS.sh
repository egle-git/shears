#!/bin/bash

total=0

for file in ${1}/logs/log-???.txt; do
    if [[ -f "$file" ]]; then
        if grep -q -e 'bonzai_SingleMuon_data' -e 'bonzai_DoubleMuon_data' -e 'bonzai_DoubleEG_data' -e 'bonzai_EGamma_data' "$file"; then
        # if grep -q -e '_bonzai_DYJetsToMuMu_M-10to50' -e '_bonzai_DYJetsToEE_M-10to50' "$file"; then
        # if grep -q -e '_bonzai_TTTo2L2Nu' "$file"; then
            number=$(grep 'With a good Z boson' "$file" | awk '{print $9}')
            echo $file
            echo $number
            if [[ ! -z "$number" ]]; then
                total=$(echo "$total + $number" | bc)
            fi
        fi
    fi
done

echo "Total: $total"