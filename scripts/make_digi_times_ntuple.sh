#!/usr/bin/bash

rn=$1
logfile=print_digi_times_$rn.log
mu2e -c dev/otsdaq-mu2e-tracker/fcl/print_digi_times_$rn.fcl -s /data/tracker/vst/mu2etrk_pasha_029/data/raw.mu2e.trkvst.tstation.${rn}_000001.art -n 500 >| $logfile

cat $logfile | grep -v event:10 | grep -v Begin > run_${rn}_hit_times_001.txt

cat run_${rn}_hit_times_001.txt | sed 's/0x//' > run_${rn}_hit_times_002.txt
