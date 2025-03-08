#
include_directories(
  $ENV{ROOTSYS}/include/root
  ${CMAKE_CURRENT_SOURCE_DIR}
  $ENV{SPACK_ENV}/otsdaq-mu2e-tracker
)

# link_directories($ENV{ROOTSYS}/lib)

set (dict "otsdaq-mu2e-tracker_Nt_dict")
ROOT_GENERATE_DICTIONARY(${dict} DaqEvent.hh DaqStrawDigi.hh LINKDEF Linkdef.h)

set_property(SOURCE ${dict}.cxx PROPERTY generated)

add_library(otsdaq-mu2e-tracker_Nt SHARED
            DaqEvent.cc DaqStrawDigi.cc ${dict}.cxx
)

target_link_libraries(otsdaq-mu2e-tracker_Nt ROOT::Core)

cet_add_compiler_flags(-g -O0)

install(TARGETS otsdaq-mu2e-tracker_Nt)
install_headers()
install_source()
