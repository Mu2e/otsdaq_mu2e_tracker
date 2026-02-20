
include(otsdaq::FEInterface)

cet_build_plugin(ROCTrackerInterface otsdaq::FEInterface
    SOURCE ROCTrackerInterface_interface.cc

    # ROCTrackerInterface_Ui_base.cc
    ROCTrackerInterface_Ui_base_defineFEMacros.cc
    # ROCTrackerInterface_Ui.cc
    ROCTrackerInterface_Ui_defineFEMacros.cc
    # ROCTrackerInterface_Ui_print.cc
    ROCTrackerInterface_Ui_print_defineFEMacros.cc
    # ROCTrackerInterface_Ui_ControlRoc.cc
    ROCTrackerInterface_Ui_ControlRoc_defineFEMacros.cc
    # ROCTrackerInterface_Ui_ProgramRoc.cc
    ROCTrackerInterface_Ui_ProgramRoc_defineFEMacros.cc

    LIBRARIES REG otsdaq-mu2e::ROCPolarFireCoreInterface
    otsdaq::RootUtilities
    otsdaq-mu2e-tracker::ParseAlignment
    otsdaq-mu2e-tracker::Ui
 )


install_headers()
install_source()
