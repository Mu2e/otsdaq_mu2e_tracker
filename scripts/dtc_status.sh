#!/usr/bin/bash

dtc=$DTCLIB_DTC ; if [[ -n $1 ]] ; then dtc=$1 ; fi

echo "DTC version                                (reg 0x9004) : "`my_cntl -d $dtc read 0x9004`
echo "DTC enables                                (reg 0x9100) : "`my_cntl -d $dtc read 0x9100`
echo "fiber locked                               (reg 0x9140) : "`my_cntl -d $dtc read 0x9140`
echo "DTC timeout                                (reg 0x9144) : "`my_cntl -d $dtc read 0x9144`
echo "Number of EVB nodes                        (reg 0x9158) : "`my_cntl -d $dtc read 0x9158`
echo "Data timeout length                        (reg 0x9188) : "`my_cntl -d $dtc read 0x9188`
echo "EW marker deltaT                           (reg 0x91A8) : "`my_cntl -d $dtc read 0x91A8`
echo "number of DREQs                            (reg 0x91AC) : "`my_cntl -d $dtc read 0x91AC`
echo "Null HB after last valid one               (reg 0x91BC) : "`my_cntl -d $dtc read 0x91BC`
echo "Clk marker deltaT                          (reg 0x91F4) : "`my_cntl -d $dtc read 0x91F4`
echo "CFO emulator enable of CLK and EWM markers (reg 0x91F8) : "`my_cntl -d $dtc read 0x91F8`
echo "DCS timeout                                (reg 0x93e0) : "`my_cntl -d $dtc read 0x93e0`
echo " "
#------------------------------------------------------------------------------
# these ones have to be set to smth non-zero, suggest 0xFFFFFFFF
#------------------------------------------------------------------------------
echo "Emulator event mode                        (reg 0x91c0) : "`my_cntl -d $dtc read 0x91c0`
echo "Emulator event mode                        (reg 0x91c4) : "`my_cntl -d $dtc read 0x91c4`
