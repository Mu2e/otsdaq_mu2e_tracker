# ======================================================================
#  otsdaq-mu2e-tracker main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/otsdaq-mu2e-tracker/ups/setup_for_development
#  buildtool
# ======================================================================
cmake_minimum_required (VERSION 3.19 FATAL_ERROR)

# these are minimum required versions, not the actual product versions
find_package(cetmodules  3.16.00 REQUIRED)

project(otsdaq_mu2e_tracker VERSION 3.01.00)

include(CetCMakeEnv)
cet_cmake_env()
#------------------------------------------------------------------------------
# it looks that these packages should be looked for only after cet_cmake_env
#------------------------------------------------------------------------------
find_package(artdaq      3.12.07 REQUIRED)
find_package(artdaq_core 3.09.16 REQUIRED)
find_package(artdaq_core_demo 1.11.00 REQUIRED EXPORT)
find_package(otsdaq_mu2e 1.02.00 REQUIRED)

 # XDAQ Extra setup
 include_directories($ENV{XDAQ_INC} $ENV{XDAQ_INC}/linux)
link_directories($ENV{XDAQ_LIB})

find_package(Offline 10.00.00 REQUIRED)
include_directories($ENV{BTRK_INC} $ENV{KINKAL_INC})
link_directories($ENV{BTRK_LIB} $ENV{KINKAL_LIB})

cet_set_compiler_flags(
        DIAGS VIGILANT
        WERROR  
        NO_UNDEFINED
        EXTRA_FLAGS -pedantic -Wno-unused-parameter -pthread #-g -O0 -v # Equivalent to debug qualifier, for testing
)

#string(TOUPPER ${CMAKE_BUILD_TYPE} BTYPE_UC )
#if( ${BTYPE_UC} MATCHES "DEBUG" )
#  cet_add_compiler_flags(-fsanitize=address)
#endif()

#cet_report_compiler_flags()

#string(REGEX MATCHALL "[0-9][0-9]?[0-9]?" ART_VERSION_LIST "$ENV{ART_VERSION}")
#list(GET ART_VERSION_LIST 0 ART_MAJOR_VERSION)
#list(GET ART_VERSION_LIST 1 ART_MINOR_VERSION)
#list(GET ART_VERSION_LIST 2 ART_PATCH_VERSION)

#message("-DART_HEX_VERSION=0x${ART_MAJOR_VERSION}${ART_MINOR_VERSION}${ART_PATCH_VERSION}")
#set(ART_HEX_VERSION 0x${ART_MAJOR_VERSION}${ART_MINOR_VERSION}${ART_PATCH_VERSION})
#add_definitions(-DART_HEX_VERSION=0x${ART_MAJOR_VERSION}${ART_MINOR_VERSION}${ART_PATCH_VERSION})

include(BuildPlugins)

# Code
add_subdirectory(otsdaq-mu2e-tracker)

# Useful scripts
#add_subdirectory(tools)

# Test Programs
#add_subdirectory(test)

# Data
#FILE(COPY ConfigurationDataExamples DESTINATION Data)
#INSTALL(DIRECTORY ConfigurationDataExamples DESTINATION ${product}/${version}/Data)
#FILE(COPY XDAQConfigurations DESTINATION Data)
#INSTALL(DIRECTORY XDAQConfigurations DESTINATION ${product}/${version}/Data)
#FILE(COPY ServiceData DESTINATION Data)
#INSTALL(DIRECTORY ServiceData DESTINATION ${product}/${version}/Data)
#FILE(COPY Data DESTINATION .)
#INSTALL(DIRECTORY Data DESTINATION ${product}/${version})
#FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Data/ServiceData/LoginData)
#INSTALL(DIRECTORY DESTINATION ${product}/${version}/Data/ServiceData/LoginData)

cet_cmake_config()
