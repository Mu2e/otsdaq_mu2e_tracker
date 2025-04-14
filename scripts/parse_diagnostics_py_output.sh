#!/usr/bin/bash

cat thresholds.dat  | grep \"cal\" | awk -F , '{print $3}' | awk -F : '{print $2}' >| thresholds_cal.json
cat thresholds.dat  | grep \"hv\"  | awk -F , '{print $3}' | awk -F : '{print $2}' >| thresholds_hv.json
cat thresholds.dat  | grep \"cal\" | awk -F , '{print $4}' | awk -F : '{print $2}' >| gains_cal.json
cat thresholds.dat  | grep \"hv\"  | awk -F , '{print $4}' | awk -F : '{print $2}' >| gains_hv.json
