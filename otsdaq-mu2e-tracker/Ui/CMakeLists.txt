#
# include_directories($ENV{ROOTSYS}/include)
# link_directories($ENV{ROOTSYS}/lib)
#
cet_make_library(
        SOURCE          
        DtcInterface.cc CfoInterface.cc
        LIBRARIES PUBLIC
	otsdaq_mu2e_tracker::otsdaq-mu2e-tracker_ParseAlignment
        artdaq::DAQdata
        artdaq_core::artdaq-core_Data
        # artdaq-core-demo::artdaq-core-demo_Overlays
        cetlib_except
        mu2e_pcie_utils::CFOInterface
        mu2e_pcie_utils::DTCInterface
        ROOT::Core
        )
        
include(ArtDictionary)

art_dictionary(
  DICTIONARY_LIBRARIES
  otsdaq_mu2e_tracker::otsdaq-mu2e-tracker_Ui
  artdaq_core_mu2e::artdaq-core-mu2e_Overlays
  mu2e_pcie_utils::CFOInterface
  mu2e_pcie_utils::DTCInterface
)

get_target_property(DictCompileOptions otsdaq-mu2e-tracker_Ui_dict COMPILE_OPTIONS)

if(DictCompileOptions)
	string(REPLACE "-fsanitize=thread"  "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-fsanitize=address" "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-ltsan"             "" DictCompileOptions "${DictCompileOptions}")
	string(REPLACE "-lasan"             "" DictCompileOptions "${DictCompileOptions}")

	set_target_properties(otsdaq-mu2e-tracker_Ui_dict PROPERTIES COMPILE_OPTIONS "${DictCompileOptions}")
endif()

cet_add_compiler_flags(-g -O0)

# if($ENV{BUILD_ROOT_INTERFACE})
#  message(-- otsdaq_mu2e_tracker/otsdaq-mu2e-tracker/ui::BUILD_ROOT_INTERFACE=$ENV{BUILD_ROOT_INTERFACE})
#  add_subdirectory(root)
# endif()

install_headers()
install_source()
