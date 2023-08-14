cet_make_library(LIBRARY_NAME HelloWorld
SOURCE
	HelloWorld.cc
	version.cc
LIBRARIES 
PUBLIC
	otsdaq::CoreSupervisors
                 artdaq::DAQdata
		 otsdaq::ConfigurationInterface
	otsdaq::XmlUtilities
	otsdaq::DesktopIconTable
	otsdaq::MessageFacility
	otsdaq::WebUsersUtilities
	otsdaq::FiniteStateMachine
	otsdaq::XDAQContextTable
	${XDAQ_BASIC_LIB_LIST}
	config # From XDAQ
	otsdaq::FECore
	otsdaq::SOAPUtilities
	otsdaq::CgiDataUtilities
		 otsdaq::ARTDAQTableBase
		 otsdaq::ARTDAQDataManager
	)

install_headers()
install_source()
