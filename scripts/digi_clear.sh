if [ $# -lt 1 ]; then
    echo "link_enable command expects one argument: LINK_N0"
fi

LINK=$1


echo " "
#reset link and reconfigure
echo "Clearing DIGI FIFOs link $LINK"


# this will proceed in 3 steps each for HV and CAL DIGIs:
# 1) pass TWI address and data toTWI controller (fiber is enabled by default)
# 2) write TWI INIT high
# 3) write TWI INIT low

echo "Writing 0 & 1 to  address=16 for HV DIGIs"
rocUtil write_register -l $LINK -a 28 -w 16 > /dev/null

rocUtil write_register -l $LINK -a 27 -w  0 > /dev/null # write 0 
rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null ## toggle INIT 
rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null

rocUtil write_register -l $LINK -a 27 -w  1 > /dev/null # write 1  
rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null # toggle INIT
rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null

echo "Writing 0 & 1 to  address=16 for CAL DIGIs"
rocUtil write_register -l $LINK -a 25 -w 16 > /dev/null

rocUtil write_register -l $LINK -a 24 -w  0 > /dev/null # write 0
rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null

rocUtil write_register -l $LINK -a 24 -w  1 > /dev/null # write 1
rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null

