#!/usr/bin/env bash

dtc=$1

# TX Firefly
my_cntl -d $dtc write 0x93a0 0x00000100
my_cntl -d $dtc write 0x9288 0x50160000
my_cntl -d $dtc write 0x928c 0x00000002
my_cntl -d $dtc write 0x93a0 0x00000000
txtempval=`my_cntl -d $dtc read 0x9288|grep 0x`
tempvalue=`echo "print(int($txtempval & 0xFF))"|python -`

echo "TX Firefly temperature: $tempvalue"

# TX/RX Firefly
my_cntl -d $dtc write 0x93a0 0x00000400
my_cntl -d $dtc write 0x92a8 0x50160000
my_cntl -d $dtc write 0x92ac 0x00000002
my_cntl -d $dtc write 0x93a0 0x00000000

txrxtempval=`my_cntl -d $dtc read 0x92a8|grep 0x`
tempvalue=`echo "print(int($txrxtempval & 0xFF))"|python -`

echo "TX/RX Firefly temperature: $tempvalue"
