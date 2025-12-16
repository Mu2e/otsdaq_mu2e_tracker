#define BOOST_TEST_MODULE TrkUi_import_t
#include <boost/test/unit_test.hpp>

#include "otsdaq/MessageFacility/MessageFacility.h"

#include "otsdaq/Macros/StringMacros.h"

BOOST_AUTO_TEST_SUITE(TrkUi_import_t)

BOOST_AUTO_TEST_CASE(Import)
{
	bool ok = false;

	std::string result = StringMacros::exec(
	    "otsdaq_import_tracker_test_stand.sh otsdaq-mu2e-tracker/Ui/ "
	    "otsdaq-mu2e-tracker/FEInterfaces/ > t.txt")

	    // __COUTV__(result);
	    if(result.find(
	           "There were NO ERRORS found while loading and exporting groups.") !=
	       std::string::npos) ok = true;

	BOOST_REQUIRE(ok);

	BOOST_REQUIRE_EQUAL(ok, true);
}

BOOST_AUTO_TEST_SUITE_END()
