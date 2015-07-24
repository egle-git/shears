#!/bin/bash

i=0
date
for lepSel in DMu DE; do
    for doWhat in 0 1 3 4; do
	cat <<EOF > cfg.$i
doWhat = $doWhat
do10000events = 0
doCentral = 1
doSysRunning = 1
lepSel = $lepSel
EOF
	export VJETS_CONFIG=cfg.$i
	root -q -l -b runZJets.cc > log.$i 2>&1 &
	i=$((i+1))
    done
done

wait
date
