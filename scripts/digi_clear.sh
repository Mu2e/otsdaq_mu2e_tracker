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
rocUtil -a 28 -w 16 -l $LINK write_register > /dev/null

# write 0 
rocUtil -a 27 -w 0 -l $LINK write_register > /dev/null
## toggle INIT
rocUtil -a 26 -w 1 -l $LINK write_register > /dev/null
rocUtil -a 26 -w 0 -l $LINK write_register > /dev/null

## write 1  
rocUtil -a 27 -w 1 -l $LINK write_register > /dev/null
# toggle INIT
rocUtil -a 26 -w 1 -l $LINK write_register > /dev/null
rocUtil -a 26 -w 0 -l $LINK write_register > /dev/null


echo "Writing 0 & 1 to  address=16 for CAL DIGIs"
rocUtil -a 25 -w 16 -l $LINK write_register > /dev/null

## write 0
rocUtil -a 24 -w 0 -l $LINK write_register > /dev/null
# toggle INIT
rocUtil -a 23 -w 1 -l $LINK write_register > /dev/null
rocUtil -a 23 -w 0 -l $LINK write_register > /dev/null

## write 1
rocUtil -a 24 -w 1 -l $LINK write_register > /dev/null
# toggle INIT
rocUtil -a 23 -w 1 -l $LINK write_register > /dev/null
rocUtil -a 23 -w 0 -l $LINK write_register > /dev/null

