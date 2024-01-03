#!/usr/bin/bash
#------------------------------------------------------------------------------
# call : dtc_counters.sh link
#------------------------------------------------------------------------------
LINK=$1

if [ $# -lt 1 ]; then
    echo "Need LINK argument"
    echo " "
    exit 2
elif [ $LINK -lt 0 -o $LINK -gt 5 ]; then
    echo "Bad LINK: range 0 to 5"
    echo " "
    exit 2
fi

declare -i DTCREQ
DTCREQ=0x9630
DTCREQ+=4*$LINK
#echo "DTCREQ =" $DTCREQ

declare -i HB
HB=0x9650
HB+=4*$LINK

declare -i HEAD
HEAD=0x9670
HEAD+=4*$LINK

declare -i PAY
PAY=0x9690
PAY+=4*$LINK

declare -i CRC
CRC=0x9560
CRC+=4*$LINK

echo "Link $LINK:"
echo "        #DTCReq    : " `my_cntl read $DTCREQ`
echo "        #HB        : " `my_cntl read $HB` 
echo "        #DataHeader: " `my_cntl read $HEAD`
echo "        #Payloads  : " `my_cntl read $PAY`
echo "        #CRC errors: " `my_cntl read $CRC`

#echo " "
#echo "Link 0 reports:"
#echo " #DTCReq:" `my_cntl read 0x9630`
#echo " "
#
#echo " #HB:    " `my_cntl read 0x9650`
#echo " "
#
#echo " #DataHeader:" `my_cntl read 0x9670`
#echo " "
#
#echo " #Payloads:  " `my_cntl read 0x9690`
#echo " "
#
#echo " #CRC errors:" `my_cntl read 0x9560`
#echo " "
#
#echo "Link 1 reports:"
#echo " #DTCReq:" `my_cntl read 0x9634`
#echo " "

#echo " #HB:    " `my_cntl read 0x9654`
#echo " "

#echo " #DataHeader:" `my_cntl read 0x9674`
#echo " "

#echo " #Payloads:  " `my_cntl read 0x9694`
#echo " "

#echo " #CRC errors:" `my_cntl read 0x9564`
#echo " "

#echo "Link 2 reports:"
#echo " #DTCReq:" `my_cntl read 0x9638`
#echo " "

#echo " #HB:    " `my_cntl read 0x9658`
#echo " "

#echo " #DataHeader:" `my_cntl read 0x9678`
#echo " "

#echo " #Payloads:  " `my_cntl read 0x9698`
#echo " "

#echo "Link 3 reports:"
#echo " #DTCReq:" `my_cntl read 0x963c`
#echo " "

#echo " #HB:    " `my_cntl read 0x965c`
#echo " "

#echo " #DataHeader:" `my_cntl read 0x967c`
#echo " "

#echo " #Payloads:  " `my_cntl read 0x969c`
#echo " "

#echo " #CRC errors:" `my_cntl read 0x956c`
#echo " "

#echo "Link 4 reports:"
#echo " #DTCReq:" `my_cntl read 0x9640`
#echo " "

#echo " #HB:    " `my_cntl read 0x9660`
#echo " "

#echo " #DataHeader:" `my_cntl read 0x9680`
#echo " "

#echo " #Payloads:  " `my_cntl read 0x96a0`
#echo " "

#echo " #CRC errors:" `my_cntl read 0x9570`
#echo " "

