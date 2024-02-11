#!/usr/bin/bash

dtc=-1;
if [[ ".$1" != "." ]] ; then dtc=$1; fi
 
my_cntl -d $dtc write 0x9100 0x80000000 > /dev/null # reset DTC  reset serdes osc
my_cntl -d $dtc write 0x9100 0x00008000 > /dev/null # Turn on CFO Emulation Mode for Serdes Reset
my_cntl -d $dtc write 0x9118 0xffff00ff > /dev/null # SERDES resets (don't reset PLL bits)
my_cntl -d $dtc write 0x9118 0x00000000 > /dev/null # clear SERDES resets on link 0

sleep 1

echo "SERDES Reset Done after reset DTC=$dtc: "
my_cntl -d $dtc read 0x9138
