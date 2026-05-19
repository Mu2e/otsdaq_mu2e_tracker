
include(otsdaq::FEInterface)

cet_build_plugin(ROCTrackerInterface otsdaq::FEInterface
    SOURCE ROCTrackerInterface_interface.cc

    LIBRARIES REG otsdaq-mu2e::ROCPolarFireCoreInterface
    otsdaq::RootUtilities
    otsdaq-mu2e-tracker::ParseAlignment
    otsdaq-mu2e-tracker::Ui
 )


install_headers()
install_source()
