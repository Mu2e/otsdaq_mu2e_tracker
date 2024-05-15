#!/usr/bin/bash

dtc=$DTCLIB_DTC ; if [[ -n $1 ]] ; then dtc=$1 ; fi

echo "DTC version                                (0x9004) : "`my_cntl -d $dtc read 0x9004`
echo "DTC COntrol Register                       (0x9100) : "`my_cntl -d $dtc read 0x9100`
echo "fiber locked                               (0x9140) : "`my_cntl -d $dtc read 0x9140`
echo "DMA timeout preset                         (0x9144) : "`my_cntl -d $dtc read 0x9144`
echo "Number of EVB nodes                        (0x9158) : "`my_cntl -d $dtc read 0x9158`
echo "Data timeout length                        (0x9188) : "`my_cntl -d $dtc read 0x9188`
echo "EW marker deltaT                           (0x91A8) : "`my_cntl -d $dtc read 0x91A8`
echo "number of DREQs                            (0x91AC) : "`my_cntl -d $dtc read 0x91AC`
echo "Null HB after last valid one               (0x91BC) : "`my_cntl -d $dtc read 0x91BC`
echo "Clk marker deltaT                          (0x91F4) : "`my_cntl -d $dtc read 0x91F4`
echo "CFO emulator enable of CLK and EWM markers (0x91F8) : "`my_cntl -d $dtc read 0x91F8`
echo "DCS timeout                                (0x93e0) : "`my_cntl -d $dtc read 0x93e0`
echo " "
#------------------------------------------------------------------------------
# these ones have to be set to smth non-zero, suggest 0xFFFFFFFF
#------------------------------------------------------------------------------
echo "Emulator event mode                        (0x91c0) : "`my_cntl -d $dtc read 0x91c0`
echo "Emulator event mode                        (0x91c4) : "`my_cntl -d $dtc read 0x91c4`
