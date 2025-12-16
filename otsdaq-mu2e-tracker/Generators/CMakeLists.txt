
include(artdaq::commandableGenerator)

cet_build_plugin(ToySim artdaq::commandableGenerator 
                 LIBRARIES REG 
                 artdaq-core-demo::Overlays
                 otsdaq-mu2e-tracker::Generators_ToyHW
)

add_subdirectory(ToyHW)

install_headers()
install_source()

# cet_build_plugin(TrackerVST artdaq::commandableGenerator 
#                  LIBRARIES REG 
#                  artdaq-core-mu2e::artdaq-core-mu2e_Overlays 
#                  canvas::canvas 
#                  mu2e-pcie-utils::DTCInterface
#                  ${XMLRPC_LIBRARIES}
# )

set(MIDAS_LIBS "")
if( DEFINED $ENV{MIDASSYS})
    message(MIDASSYS defined: $ENV{MIDASSYS})
    set(MIDAS_LIBS midas)
endif()

add_compile_definitions(USE_MIDAS)

# cet_build_plugin(TrackerBR artdaq::commandableGenerator 
#                  LIBRARIES REG
#                  otsdaq-mu2e-tracker::otsdaq-mu2e-tracker_Ui
#                  artdaq-core-mu2e::artdaq-core-mu2e_Overlays 
#                  canvas::canvas 
#                  mu2e_pcie_utils::DTCInterface
#                  ${XMLRPC_LIBRARIES}
#                  ${MIDAS_LIBS}
# #                  midas
# )

#target_include_directories(otsdaq-mu2e-tracker_Generators_TrackerVST_generator
#  PRIVATE ${XMLRPC_INCLUDE_DIRS})

#target_include_directories(otsdaq-mu2e-tracker_Generators_TrackerBR_generator
#  PRIVATE ${XMLRPC_INCLUDE_DIRS})
