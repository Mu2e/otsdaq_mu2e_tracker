#!/bin/bash

#source /mu2e/ups/setup
#setup pcie_linux_kernel_module v2_03_00 -qe19:prof:s96

my_cntl write 0xa000 0

ii=$((0xa004))

while [ $ii -le $((0xa3FC)) ]; do
  my_cntl write $ii 1
#  my_cntl write $ii 0
  ii=$(( $ii + 4 ))
done

