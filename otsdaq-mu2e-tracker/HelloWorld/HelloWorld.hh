#ifndef _mypackage_HelloWorld_h_
#define _mypackage_HelloWorld_h_

#include "otsdaq/CoreSupervisors/CoreSupervisorBase.h"

namespace mypackage {

  class HelloWorld: public ots::CoreSupervisorBase {
  public:

    static xdaq::Application* instantiate(xdaq::ApplicationStub* s); // XDAQ_INSTANTIATOR();

    static xdaq::Application* instantiate_executive(xdaq::ApplicationStub* s); // XDAQ_INSTANTIATOR();

    HelloWorld(xdaq::ApplicationStub * s);
    virtual ~HelloWorld();
  };
}

#endif
