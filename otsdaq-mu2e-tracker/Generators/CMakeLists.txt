link_directories($ENV{MESSAGEFACILITY_LIB})
link_directories($ENV{FHICLCPP_LIB})
link_directories($ENV{CETLIB_LIB})
link_directories($ENV{CETLIB_EXCEPT_LIB})
include(artdaq::commandableGenerator)

cet_build_plugin(ToySim artdaq::commandableGenerator LIBRARIES REG artdaq_core_demo::artdaq-core-demo_Overlays otsdaq_mu2e_tracker::otsdaq-mu2e-tracker_Generators_ToyHW )


add_subdirectory(ToyHW)

install_headers()
install_source()

#
#  cet_build_plugin(TrackerVST    artdaq::commandableGenerator LIBRARIES 
#  			       REG artdaq_core_mu2e::artdaq-core-mu2e_Overlays canvas::canvas mu2e_pcie_utils::DTCInterface)
#  #
#  cet_build_plugin(ToySim  artdaq::commandableGenerator LIBRARIES 
#  			       REG artdaq_core_mu2e::artdaq-core-mu2e_Overlays canvas::canvas)
#  #
#cet_build_plugin(TrackerVST artdaq::commandableGenerator LIBRARIES REG artdaq_core_demo::artdaq-core-demo_Overlays )
