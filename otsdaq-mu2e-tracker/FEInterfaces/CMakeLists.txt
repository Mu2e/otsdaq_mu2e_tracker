
include(otsdaq::FEInterface)

cet_build_plugin(ROCTrackerInterface otsdaq::FEInterface 
    SOURCE ROCTrackerInterface_interface.cc ROCTrackerInterface_Ui.cc ROCTrackerInterface_Ui_defineFEMacros.cc
    LIBRARIES REG otsdaq-mu2e::ROCPolarFireCoreInterface
    otsdaq::RootUtilities
    otsdaq-mu2e-tracker::ParseAlignment
 )


install_headers()
install_source()
