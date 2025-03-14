//
#include <vector>

#include "otsdaq-mu2e-tracker/Nt/DaqEvent.hh"
// #include "DaqEvent.hh"

// ClassImp(DaqEvent)

//-----------------------------------------------------------------------------
DaqEvent::DaqEvent() { // : TObject () {
  nsd = 0;
  sd = new TClonesArray("DaqStrawDigi",100);
  
}

//-----------------------------------------------------------------------------
DaqEvent::~DaqEvent() {
  sd->Delete();
  delete sd;
}
