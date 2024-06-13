#
include_directories($ENV{ROOTSYS}/include)
link_directories($ENV{ROOTSYS}/lib)
#
cet_make_library(
  SOURCE    DtcGui_gui.cc  
  DtcGui_gui_CfoTab.cc
  DtcGui_gui_DtcTab.cc
  DtcGui_gui_RocTab.cc
  DtcGui_commands.cc  
  LIBRARIES PUBLIC
  otsdaq_mu2e_tracker::otsdaq-mu2e-tracker_Ui
  ROOT::Gui
  )


include(ArtDictionary)
art_dictionary(
  DICTIONARY_LIBRARIES
  otsdaq_mu2e_tracker::otsdaq-mu2e-tracker_Gui
#  artdaq_core_mu2e::artdaq-core-mu2e_Overlays
#  mu2e_pcie_utils::CFOInterface
#  mu2e_pcie_utils::DTCInterface
)

install_headers()
install_source()
# artdaq_core_mu2e::artdaq-core-mu2e_Overlays
#

set (dict_name "otsdaq-mu2e-tracker_Gui_dict")

get_target_property(DictCompileOptions ${dict_name} COMPILE_OPTIONS)

if(DictCompileOptions)
	string(REPLACE "-fsanitize=thread"  "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-fsanitize=address" "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-ltsan"             "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-lasan"             "" DictCompileOptions "${DictCompileOptions}")

	set_target_properties(${dict_name} PROPERTIES COMPILE_OPTIONS "${DictCompileOptions}")
endif()

# if($ENV{BUILD_ROOT_INTERFACE})
#   message(-- otsdaq_mu2e_tracker/otsdaq-mu2e-tracker/gui::BUILD_ROOT_INTERFACE=$ENV{BUILD_ROOT_INTERFACE})
#   add_subdirectory(root)
# endif()
