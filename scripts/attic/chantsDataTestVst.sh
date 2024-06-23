#!/usr/bin/bash
# assume that the DTC has already been setup

# Need this to source local scripts
       dir=`dirname $BASH_SOURCE`
script_dir=`cd $dir ; pwd -P`

## configure jitter attenuation, want to know which script I'm using
source $MU2E_PCIE_UTILS_DIR/dtcInterfaceLib/JAConfig.sh

echo "Finished configuring the jitter attenuator"
sleep 5

## Reset Crystal - 03/30/2021 obsolete
## reset freq reg to default 156.25MHz
#my_cntl write 0x9160 0x09502f90 >/dev/null
## Set RST_REG bit
#my_cntl write 0x9168 0x55870100 >/dev/null
#my_cntl write 0x916c 0x00000001 >/dev/null
#sleep 5
## change refclock to 200mhz
#mu2eUtil program_clock -C 0 -F 200000000
#sleep 5

## enable CFO Emulator EVM and CLK markers
my_cntl write 0x91f8 0x00003f3f >/dev/null

## enable RX and TX links for CFO + link 5, 4, 3, 2, 1, 0
#my_cntl write 0x9114 0x00007f7f >/dev/null
# not needed for CFO emulation?? For sure EWM happen anyway...
my_cntl write 0x9114 0x00000000 >/dev/null

## DMA timeout time (in 5 ns unit: 0x14141 = 0.41 ms)
my_cntl write 0x9144 0x00014141 >/dev/null

## set number of null heartbeats at start
my_cntl write 0x91BC 0x10 >/dev/null

## Send data
## *** 0x91f0 becomes reserved register from DTC2021Feb18_10: use 0x91a8 in its place ****
## event marker interval in 200 MHz clk cycles (value of 0 will disable) 
# my_cntl write 0x91f0 0x0 >/dev/null
## event marker interval in 200 MHz clk cycles (value of 0 will disable) 
#my_cntl write 0x91f4 0x4000000 >/dev/null
#my_cntl write 0x91f4 0x0 >/dev/null
## Emulator start interval time
#my_cntl write 0x91a8 0x4e20 >/dev/null    #value in OTSDAQ configuration, equal to 20,000

## new Event Table data from Rick
## edited by Monica to get rid of "pcie_linux_kernel_module" version conflict
## not needed after Eric updates library
source $script_dir/loadEventTableMonica.sh

#set num of EVB destination nodes
my_cntl write 0x9158 0x1 >/dev/null

## this "writes" are done in library now
##    enable stand-alone clock (no CFO) by turning on CF) emulation mode
#my_cntl write 0x9100 0x40808004 >/dev/null   
## disable transmission
## also needed to restart EWM after a DTC_Reset
#my_cntl write 0x9100 0x40808404 > /dev/null
# avoid enabling CFO emulator: should be enough to get EWM going from DTC2021_Sep29_17
my_cntl write 0x9100 0x808404 > /dev/null

## now reset the ROC. This assumes ROC link 0.
#rocUtil -a 0 -w 0 write_register
## if you are using ROC link 1, in principle this should work (not tested)
#rocUtil -l 0x10 -a 0 -w 0 write_register

# added on 04/20/2021 otherwise no data request is sent!
#set emulator event mode bits (for non-null heartbeats)
my_cntl write 0x91c0 0xffffffff
my_cntl write 0x91c4 0xffffffff

DTCLIB_SIM_ENABLE=N 

## needed after programming DTC 
## not needed in version May_05_10 even after first flashing...
#DTC_Reset
