//
#include "otsdaq-mu2e-tracker/HelloWorld/HelloWorld.hh"


  xdaq::Application* instantiate_executive(xdaq::ApplicationStub* stub) {
    return new xdaq::Application(stub);
  }

namespace mypackage {

  xdaq::Application* HelloWorld::instantiate(xdaq::ApplicationStub* stub) {
    return new HelloWorld(stub);
  }

  HelloWorld::HelloWorld(xdaq::ApplicationStub * s): ots::CoreSupervisorBase(s) {

    LOG4CPLUS_INFO(this->getApplicationLogger(), "Hello World!");
  }

  HelloWorld::~HelloWorld() {
  }

}
