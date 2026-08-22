#!/usr/bin/env python
# rely on $DTCLIB_DTC

import os, sys, subprocess
import TRACE

result = subprocess.run(
    "spack find -p mu2e-pcie-utils | grep mu2e-pcie-utils | tail -n 1 | awk '{print $2}'", 
    shell=True,
    capture_output=True,
    text=True,
    check=True,
)

path = result.stdout.strip()
sys.path.append(path+'/lib')

print (sys.path)

from dtcInterfaceLib import *

class Dtc:
    def __init__(self):
        self.dtc = DTC(DTC_SimMode_Disabled,0,0x111111,"")

    def read_register(self,register):
        data = self.dtc.GetDevice().read_register(register,150)
        if (data[0] == 0): 
            return data[1]
        else: 
            print ("ERROR reading reg 0x%04x" % register)
            return None
        return

    def print_register(self,register,banner=""):
        data = self.read_register(register)
        print('0x%04x  | 0x%08x | %s' % (register,data,banner))
        return

    def print_status(self):
        print('register|   value    |  comment');
        print('--------+------------+---------------------------------')
        self.print_register(0x9004,"DTC version");
        self.print_register(0x9100,"DTC control register");
        self.print_register(0x9140,"SERDES RX CDR lock (locked fibers)")
        self.print_register(0x91a8,"CFO Emulation Heartbeat Interval " )
#------------------------------------------------------------------------------
# rely on os.environ['DTCLIB_DTC']
#------------------------------------------------------------------------------
if __name__ == "__main__":

    dtc = Dtc()
    dtc.print_status();
