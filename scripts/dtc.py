#!/usr/bin/env python
# rely on $DTCLIB_DTC

import os, sys
import TRACE

sys.path.append(os.environ["PWD"]+'/build_slf7.x86_64/mu2e_pcie_utils/slf7.x86_64.e28.s128.prof/lib')
sys.path.append(os.environ["PWD"]+'/build_slf7.x86_64/mu2e_pcie_utils/dtcInterfaceLib/python')

import dtcInterfaceLib

class Dtc:
    def __init__(self):
        self.dtc = dtcInterfaceLib.DTC()

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

#    print(f'PYTHONHOME={os.environ["PYTHONHOME"]}')
#    print(f'PYTHONPATH={os.environ["PYTHONPATH"]}')

    dtc = Dtc()
    dtc.print_status();
