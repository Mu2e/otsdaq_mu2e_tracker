#!/usr/bin/bash 

rn1=`printf "%06i" $1`
if [ x"$2" != "x" ] ; then rn2=$2 ; else rn2=$rn1 ; fi

for rn in `seq $rn1 $rn2` ; do
    irn=`printf "%06i" $rn`
    for f in `ls /scratch/mu2e/mu2etrk_mu2e_pasha_018/OutputData/raw.mu2e.trkvst.annex.${irn}_*` ; do 
        cmd="scp $f murat@mu2egpvm06:/srv/mu2e/data/projects/tracker/vst/datasets/raw.mu2e.trkvst.annex.art/."
        echo $cmd
        $cmd
        echo rc=$?
    done
done
