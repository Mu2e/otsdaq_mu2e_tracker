cet_enable_asserts()

# Make sure tests have correct environment settings.
include(CetTest)
include(otsdaq::FEInterface)

# cet_test(TrkUi_import_t   USE_BOOST_UNIT
#     LIBRARIES   
#     otsdaq::FEInterface
# )

install_headers()
install_source()