
include(ArtDictionary)

art_dictionary(
  DICTIONARY_LIBRARIES
  otsdaq_mu2e_tracker::otsdaq-mu2e-tracker_gui
#  artdaq_core_mu2e::artdaq-core-mu2e_Overlays
#  mu2e_pcie_utils::CFOInterface
#  mu2e_pcie_utils::DTCInterface
)

set (dict_name "otsdaq-mu2e-tracker_gui_root_dict")

get_target_property(DictCompileOptions ${dict_name} COMPILE_OPTIONS)

if(DictCompileOptions)
	string(REPLACE "-fsanitize=thread" "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-fsanitize=address" "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-ltsan" "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-lasan" "" DictCompileOptions "${DictCompileOptions}")

	set_target_properties(${dict_name} PROPERTIES COMPILE_OPTIONS "${DictCompileOptions}")
endif()

