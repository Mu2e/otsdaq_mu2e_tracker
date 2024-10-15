
include(artdaq::commandableGenerator)

cet_build_plugin(ToySim artdaq::commandableGenerator 
                 LIBRARIES REG 
                 artdaq_core_demo::artdaq-core-demo_Overlays
                 otsdaq_mu2e_tracker::otsdaq-mu2e-tracker_Generators_ToyHW
)

add_subdirectory(ToyHW)

install_headers()
install_source()

cet_build_plugin(TrackerVST artdaq::commandableGenerator 
                 LIBRARIES REG 
                 artdaq_core_mu2e::artdaq-core-mu2e_Overlays 
                 canvas::canvas 
                 mu2e_pcie_utils::DTCInterface
                 ${XMLRPC_LIBRARIES}
)

cet_build_plugin(TrackerBR artdaq::commandableGenerator 
                 LIBRARIES REG
                 otsdaq_mu2e_tracker::otsdaq-mu2e-tracker_Ui
                 artdaq_core_mu2e::artdaq-core-mu2e_Overlays 
                 canvas::canvas 
                 mu2e_pcie_utils::DTCInterface
                 ${XMLRPC_LIBRARIES}
)

target_include_directories(otsdaq-mu2e-tracker_Generators_TrackerVST_generator
  PRIVATE ${XMLRPC_INCLUDE_DIRS})

target_include_directories(otsdaq-mu2e-tracker_Generators_TrackerBR_generator
  PRIVATE ${XMLRPC_INCLUDE_DIRS})
