#
include_directories($ENV{ROOTSYS}/include)
link_directories($ENV{ROOTSYS}/lib)
#
cet_make_library(       
        SOURCE          
        DtcInterface.cc CfoInterface.cc
        LIBRARIES PUBLIC
        artdaq::DAQdata
        artdaq_core::artdaq-core_Data
        artdaq_core_demo::artdaq-core-demo_Overlays
        CFOInterface DTCInterface
        ROOT::Core
        )
        
install_headers()
install_source()
# artdaq_core_mu2e::artdaq-core-mu2e_Overlays
#

if($ENV{BUILD_ROOT_INTERFACE})
  message(-- otsdaq_mu2e_tracker/otsdaq-mu2e-tracker/ui::BUILD_ROOT_INTERFACE=$ENV{BUILD_ROOT_INTERFACE})
  add_subdirectory(root)
endif()
