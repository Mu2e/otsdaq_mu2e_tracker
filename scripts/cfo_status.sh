#!/usr/bin/bash

cfo=$CFOLIB_CFO ; if [[ -n $1 ]] ; then cfo=$1 ; fi

echo "CFO version                                (reg 0x9004) : "`my_cntl -d $cfo read 0x9004`
echo "Kernel driver version                      (reg 0x9030) : "`my_cntl -d $cfo read 0x9030`
echo "CFO control register                       (reg 0x9100) : "`my_cntl -d $cfo read 0x9100`
echo "SERDES loopback enable                     (reg 0x9108) : "`my_cntl -d $cfo read 0x9108`
echo "CFO link enable register                   (reg 0x9114) : "`my_cntl -d $cfo read 0x9114`
echo "SERDES PLL locked                          (reg 0x9128) : "`my_cntl -d $cfo read 0x9128`
echo "SERDES RX  status                          (reg 0x9134) : "`my_cntl -d $cfo read 0x9134`
echo "SERDES RX  done                            (reg 0x9138) : "`my_cntl -d $cfo read 0x9138`
echo "SERDES RX CDR lock register                (reg 0x9140) : "`my_cntl -d $cfo read 0x9140`
echo "Enable Beam ON  mode register              (reg 0x9148) : "`my_cntl -d $cfo read 0x9148`
echo "Enable Beam OFF mode register              (reg 0x914c) : "`my_cntl -d $cfo read 0x914c`
echo "40 MHz clock marker count register         (reg 0x9154) : "`my_cntl -d $cfo read 0x9154`
echo "Number of DTCs on each link                (reg 0x918c) : "`my_cntl -d $cfo read 0x918c`
echo "Beam OFF event window interval time        (reg 0x91a0) : "`my_cntl -d $cfo read 0x91a0`
echo " "
echo "Receive  byte count link 0                 (reg 0x9200) : "`my_cntl -d $cfo read 0x9200`
echo "Receive  byte count link 1                 (reg 0x9208) : "`my_cntl -d $cfo read 0x9204`
echo "Receive  byte count link 2                 (reg 0x9208) : "`my_cntl -d $cfo read 0x9208`
echo "Receive  byte count link 3                 (reg 0x920c) : "`my_cntl -d $cfo read 0x920c`
echo "Receive  byte count link 4                 (reg 0x9210) : "`my_cntl -d $cfo read 0x9210`
echo "Receive  byte count link 5                 (reg 0x9214) : "`my_cntl -d $cfo read 0x9214`
echo "Receive  byte count link 6                 (reg 0x9218) : "`my_cntl -d $cfo read 0x9218`
echo "Receive  byte count link 7                 (reg 0x921c) : "`my_cntl -d $cfo read 0x921c`
echo " "
echo "Transmit byte count link 0                 (reg 0x9240) : "`my_cntl -d $cfo read 0x9240`
echo "Transmit byte count link 1                 (reg 0x9244) : "`my_cntl -d $cfo read 0x9244`
echo "Transmit byte count link 2                 (reg 0x9248) : "`my_cntl -d $cfo read 0x9248`
echo " "
echo "Transmit byte   count link 7               (reg 0x925c) : "`my_cntl -d $cfo read 0x925c`
echo " "
echo "Transmit packet count link 0               (reg 0x9260) : "`my_cntl -d $cfo read 0x9260`
echo "Transmit packet count link 1               (reg 0x9264) : "`my_cntl -d $cfo read 0x9264`
echo "DDR3 Beam On  Base Address Register        (reg 0x930c) : "`my_cntl -d $cfo read 0x930c`
echo "DDR3 Beam Off Base Address Register        (reg 0x9310) : "`my_cntl -d $cfo read 0x9310`

echo "Cable Delay Value Link 0                   (reg 0x9360) : "`my_cntl -d $cfo read 0x9360`
echo "Cable Delay Value Link 1                   (reg 0x9364) : "`my_cntl -d $cfo read 0x9364`
echo "..."
echo "Cable Delay Value Link 7                   (reg 0x937c) : "`my_cntl -d $cfo read 0x937c`

# echo "number of DREQs                            (reg 0x91AC) : "`my_cntl -d $cfo read 0x91AC`
# echo "Null HB after last valid one               (reg 0x91BC) : "`my_cntl -d $cfo read 0x91BC`
# echo "Clk marker deltaT                          (reg 0x91F4) : "`my_cntl -d $cfo read 0x91F4`
# echo "CFO emulator enable of CLK and EWM markers (reg 0x91F8) : "`my_cntl -d $cfo read 0x91F8`
# echo "DCS timeout                                (reg 0x93e0) : "`my_cntl -d $cfo read 0x93e0`
# echo " "
#------------------------------------------------------------------------------
# these ones have to be set to smth non-zero, suggest 0xFFFFFFFF
#------------------------------------------------------------------------------
# echo "Emulator event mode                        (reg 0x91c0) : "`my_cntl -d $cfo read 0x91c0`
# echo "Emulator event mode                        (reg 0x91c4) : "`my_cntl -d $cfo read 0x91c4`
