#
include_directories($ENV{ROOTSYS}/include)
link_directories($ENV{ROOTSYS}/lib)
#
cet_make_library(   aa     
        SOURCE    DtcGui_gui.cc  
                  DtcGui_gui_CfoTab.cc
                  DtcGui_gui_DtcTab.cc
                  DtcGui_gui_RocTab.cc
                  DtcGui_commands.cc  
        LIBRARIES PUBLIC
                  otsdaq_mu2e_tracker::otsdaq-mu2e-tracker_ui
                  ROOT::Gui
        )
        
install_headers()
install_source()
# artdaq_core_mu2e::artdaq-core-mu2e_Overlays
#

if($ENV{BUILD_ROOT_INTERFACE})
  message(-- otsdaq_mu2e_tracker/otsdaq-mu2e-tracker/gui::BUILD_ROOT_INTERFACE=$ENV{BUILD_ROOT_INTERFACE})
  add_subdirectory(root)
endif()
