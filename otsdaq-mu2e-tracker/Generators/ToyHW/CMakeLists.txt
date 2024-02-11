
link_directories($ENV{TRACE_LIB})
link_directories($ENV{MESSAGEFACILITY_LIB})
link_directories($ENV{FHICLCPP_LIB})
link_directories($ENV{CETLIB_LIB})
link_directories($ENV{CETLIB_EXCEPT_LIB})
#
cet_make(
        LIB_LIBRARIES
        fhiclcpp
        cetlib
        cetlib_except
        MF_MessageLogger
        artdaq_DAQdata
        artdaq_core::artdaq-core_Data
        artdaq_core_demo::artdaq-core-demo_Overlays    
	      ${Boost_SYSTEM_LIBRARY}
        )
        
install_headers()
install_source()
# artdaq_core_mu2e::artdaq-core-mu2e_Overlays
#
