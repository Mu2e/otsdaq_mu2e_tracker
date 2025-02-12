
#
cet_make_library(
        SOURCE
        ToyHW.cc
        LIBRARIES PUBLIC
        artdaq::DAQdata
        artdaq-core::Data
        artdaq-core-demo::Overlays
        )
        
install_headers()
install_source()
# artdaq-core-mu2e::Overlays
#
