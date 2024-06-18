
include(ArtDictionary)

art_dictionary(
  DICTIONARY_LIBRARIES
  otsdaq-mu2e-tracker_ui
  artdaq-core-mu2e_Overlays
  mu2e-pcie-utils_CFOInterface
  mu2e-pcie-utils_DTCInterface
)

get_target_property(DictCompileOptions otsdaq-mu2e-tracker_ui_root_dict COMPILE_OPTIONS)

if(DictCompileOptions)
	string(REPLACE "-fsanitize=thread" "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-fsanitize=address" "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-ltsan" "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-lasan" "" DictCompileOptions "${DictCompileOptions}")

	set_target_properties(otsdaq-mu2e-tracker_ui_root_dict PROPERTIES COMPILE_OPTIONS "${DictCompileOptions}")
endif()

