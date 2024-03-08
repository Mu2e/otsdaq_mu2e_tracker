
#
cet_make_library(
        SOURCE
        ToyHW.cc
        LIBRARIES PUBLIC
        artdaq::DAQdata
        artdaq_core::artdaq-core_Data
        artdaq_core_demo::artdaq-core-demo_Overlays
        )
        
install_headers()
install_source()
# artdaq_core_mu2e::artdaq-core-mu2e_Overlays
#
