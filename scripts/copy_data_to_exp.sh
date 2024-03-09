#!/usr/bin/bash 
#------------------------------------------------------------------------------
# copies data from local /scratch/mu2e/.. area on an online machine 
# to offline /exp/mu2e/data/projects/...
# so far , used only for the tracker
#
# call format: copy_data_to_exp.sh run1 run2 [doit] 
#
# if "doit" is undefined, the script only prints the commands to be executed
#------------------------------------------------------------------------------
rn1=`printf "%06i" $1`
if [ ".$2" != "." ] ; then rn2=$2 ; else rn2=$rn1 ; fi

doit=$3

echo rn1=$rn1 rn2=$rn2 doit=$doit

for rn in `seq $rn1 $rn2` ; do
    irn=`printf "%06i" $rn`
    for f in `ls /scratch/mu2e/$DAQ_USER_STUB/$TFM_CONFIG_NAME/data/raw.mu2e.trkvst.annex.${irn}_*` ; do 
        cmd="scp $f murat@mu2egpvm06:/exp/mu2e/data/projects/tracker/vst/datasets/raw.mu2e.trkvst.annex.art/."
        echo "$cmd"
        if [ ".$doit" != "." ] ; then 
            # echo doit=$doit
            $cmd ; echo rc=$? ; 
        fi
    done
done
