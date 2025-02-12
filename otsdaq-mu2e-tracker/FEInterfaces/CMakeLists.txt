
include(otsdaq::FEInterface)

cet_build_plugin(ROCTrackerInterface otsdaq::FEInterface LIBRARIES REG otsdaq-mu2e::ROCPolarFireCoreInterface
 )
 

install_headers()
install_source()