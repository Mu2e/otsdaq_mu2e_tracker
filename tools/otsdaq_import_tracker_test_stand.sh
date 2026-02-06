#!/bin/bash
# otsdaq/tools/otsdaq_import_tracker_test_stand.sh

# usage:
# otsdaq_import_tracker_test_stand.sh <import database URI> <import USER_DATA path> <import prepend base name (optional)>
#
# All group aliases and active groups are imported to current db and are prepended with a label if the optional arugment is provided.
# The steps of the import are as follows:
#	- delete directory content at tmp/export_system_aliases
#	- export (from import database URI and import USER_DATA args) group aliases and active groups to text/json version and store at tmp/export_system_aliases
#	- import (to current database URI and USER_DATA) all group directories found at tmp/export_system_aliases
#
# Developer note: must use terminal shell wrapper, because artdaq database URI is controlled by environment variable before running C++ main()


echo
echo "  |"
echo "  |"
echo "  |"
echo " _|_"
echo " \ /"
echo "  V "
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t ========================================================"
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t\t This script should be executed"
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t\t usage: otsdaq_import_tracker_test_stand.sh <import path to Ui> <output path to FEInterfaces>"
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t"
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t"
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t\t for example..."
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t\t\t otsdaq_import_tracker_test_stand.sh srcs/otsdaq-mu2e-tracker/Ui srcs/otsdaq-mu2e-tracker/FEInterfaces"
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t\t          or..."
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t\t\t otsdaq_import_tracker_test_stand.sh /home/otsdaq-mu2e-tracker/otsdaq-mu2e-tracker/Ui /home/otsdaq-mu2e-tracker/otsdaq-mu2e-tracker/FEInterfaces"
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t"
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t\t All UI functions are imported as FEInterace functions with preamble 'Ui'"
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t"
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t"

#return  >/dev/null 2>&1 #return is used if script is sourced


echo
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t Extracting parameters..."
echo

if [[ "x$1" == "x" || "x$2" == "x" ]]; then

	echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t Illegal parameters.. See above for usage."
	return  >/dev/null 2>&1 #return is used if script is sourced
	exit  #exit is used if script is run ./reset...
fi


echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t IMPORT_UI_PATH=$1"
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t IMPORT_FEINTERFACE_PATH=$2"

#####################
# now run import executable

otsdaq_import_tracker_test_stand $1 $2
#####################

echo
echo
echo -e `date +"%h%y %T"` "otsdaq_import_tracker_test_stand.sh:${LINENO}  \t Import of test stand functionality complete!"
