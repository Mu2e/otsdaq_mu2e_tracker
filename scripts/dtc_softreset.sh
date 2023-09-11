echo " "

echo "DTC SoftReset" 
echo "============="

echo "Issue DTC Soft Reset by writing bit(31)=1 to 0x91000"
my_cntl write 0x9100 0x80000000 > /dev/null
echo " "

sleep 1

echo "Enable CFO Emulator:  "
my_cntl write 0x9100 0x00008000 > /dev/null  
echo " "

echo "After Soft Reset sequence, 0x9100 reads: " `my_cntl read 0x9100` 
echo " "
