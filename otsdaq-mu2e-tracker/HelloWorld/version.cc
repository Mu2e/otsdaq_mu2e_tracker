#include "config/version.h"
#include "xcept/version.h"
#include "xdaq/version.h"
#include "otsdaq-mu2e-tracker/HelloWorld/version.h"

GETPACKAGEINFO(mypackage)

void mypackage::checkPackageDependencies()
{
 CHECKDEPENDENCY(config);
 CHECKDEPENDENCY(xcept);
 CHECKDEPENDENCY(xdaq);
}

std::set<std::string, std::less<std::string>> mypackage::getPackageDependencies()
{
 std::set<std::string, std::less<std::string>> dependencies;
 ADDDEPENDENCY(dependencies,config);
 ADDDEPENDENCY(dependencies,xcept);
 ADDDEPENDENCY(dependencies,xdaq);
 return dependencies;
}
