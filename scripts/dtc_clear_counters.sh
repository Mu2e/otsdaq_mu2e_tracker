#!/usr/bin/bash
#------------------------------------------------------------------------------
# this is not a reset, just clear various DTC counters here
#------------------------------------------------------------------------------
dtc=-1; if [[ ".$1" != "." ]] ; then dtc=$1; fi

echo " "
echo "Clear Link0 counters:"
my_cntl -d $dtc write 0x9630 0x1 > /dev/null
my_cntl -d $dtc write 0x9650 0x1 > /dev/null
my_cntl -d $dtc write 0x9670 0x1 > /dev/null
my_cntl -d $dtc write 0x9690 0x1 > /dev/null
my_cntl -d $dtc write 0x9560 0x1 > /dev/null

echo "Clear Link1 counters:"
my_cntl -d $dtc write 0x9634 0x1 > /dev/null
my_cntl -d $dtc write 0x9654 0x1 > /dev/null
my_cntl -d $dtc write 0x9674 0x1 > /dev/null
my_cntl -d $dtc write 0x9694 0x1 > /dev/null
my_cntl -d $dtc write 0x9564 0x1 > /dev/null

echo "Clear Link2 counters:"
my_cntl -d $dtc write 0x9638 0x1 > /dev/null
my_cntl -d $dtc write 0x9658 0x1 > /dev/null
my_cntl -d $dtc write 0x9678 0x1 > /dev/null
my_cntl -d $dtc write 0x9698 0x1 > /dev/null
my_cntl -d $dtc write 0x9568 0x1 > /dev/null

#echo " "
#echo "Clear Link1 counters:"
#my_cntl write 0x9634 0x0 > /dev/null
#my_cntl write 0x9654 0x0 > /dev/null
#my_cntl write 0x9674 0x0 > /dev/null
#my_cntl write 0x9694 0x0 > /dev/null



