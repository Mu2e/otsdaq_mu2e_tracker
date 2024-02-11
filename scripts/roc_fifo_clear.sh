if [ $# -lt 1 ]; then
    echo "link_enable command expects one argument: LINK_N0"
fi

LINK=$1


echo " "
#reset link and reconfigure
echo "Clearing ROC FIFOs for link $LINK"

rocUtil -a 13 -w 1 -l $LINK write_register > /dev/null
rocUtil -a 13 -w 0 -l $LINK write_register > /dev/null

echo "Check if ROCFIFOs are empty (register 18 should return 0xf00)"
echo "register 18 reads:" `rocUtil -a 18 -l $LINK simple_read`

