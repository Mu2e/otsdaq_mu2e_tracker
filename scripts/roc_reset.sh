#!/usr/bin/bash
#------------------------------------------------------------------------------
# reset ROC corresponding to a given link, assume default DTC ID ($DTCLIB_DTC)
#------------------------------------------------------------------------------
link=$1
if [[ ".$link" == "." ]] ; then 
    echo "ERROR: define link ID"
    return
elif [ $link -lt 0 -o  $link -gt 5 ] ; then
    echo "ERROR: link ID ranges from 0 to 5"
    return
fi

cmd="rocUtil -l $link -a 14 -w 0x1 write_register"
echo $cmd
$cmd
sleep 1
