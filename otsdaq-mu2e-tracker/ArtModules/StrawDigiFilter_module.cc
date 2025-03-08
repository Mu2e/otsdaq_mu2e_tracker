//
//  Filter for selecting events based on straw digis
//  Original author: Michael MacKenzie 03/2025
//
// framework
#include "art/Framework/Core/EDFilter.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Sequence.h"
// mu2e
// data
#include "Offline/RecoDataProducts/inc/StrawDigi.hh"

#include "TRACE/tracemf.h"
#include "otsdaq/Macros/CoutMacros.h"
#include "otsdaq/MessageFacility/MessageFacility.h"

// c++
#include <iostream>
#include <memory>
#include <map>


using namespace mu2e;
namespace ots
{
  class StrawDigiFilter : public art::EDFilter
  {
  public:
    struct Config{
      using Name    = fhicl::Name;
      using Comment = fhicl::Comment;
      fhicl::Atom<std::string>   strawDigiCollection  {Name("strawDigiCollection"), Comment("StrawDigiCollection label"), "StrawDigisFromArtdaqFragments" };
      fhicl::Atom<unsigned>      minNStrawDigis       {Name("minNStrawDigis")     , Comment("minNStrawDigis")            };
      fhicl::Atom<unsigned>      minNPlanes           {Name("minNPlanes")         , Comment("Minimum planes hit")     ,0 };
      fhicl::Atom<int>           debugLevel           {Name("debugLevel")         , Comment("Debug printout level")   ,0 };
      fhicl::Atom<bool>          noFilter             {Name("noFilter")           , Comment("Don't filter anything")  ,0 };
    };

    using Parameters = art::EDFilter::Table<Config>;

    explicit StrawDigiFilter(const Parameters& config);

  private:
    bool filter(art::Event& event) override;
    bool endRun(art::Run& run ) override;

    std::string   _sdTag;
    unsigned      _minndigis;
    unsigned      _minnplanes;
    int           _debug;
    bool          _noFilter;
    // counters
    unsigned      _nevt, _npass;
    const mu2e::StrawDigiCollection *_digis;
  };

  StrawDigiFilter::StrawDigiFilter(const Parameters& conf)
    : art::EDFilter{conf},
      _sdTag   (conf().strawDigiCollection()),
      _minndigis(conf().minNStrawDigis()),
      _minnplanes(conf().minNPlanes()),
      _debug   (conf().debugLevel()),
      _noFilter(conf().noFilter()),
      _nevt    (0),
      _npass   (0)
  {}

  bool StrawDigiFilter::filter(art::Event& event){
    ++_nevt;
    // find the collection
    art::Handle<mu2e::StrawDigiCollection> digisH;
    if (!event.getByLabel(_sdTag, digisH)) {
      TLOG(TLVL_ERROR) << "No straw digis found with tag:" << _sdTag.c_str();
      _digis = nullptr;
    } else {
      _digis = digisH.product();
    }

    const unsigned ndigis = (_digis) ? _digis->size() : 0;
    bool retval = ndigis >= _minndigis;

    if(_debug > 0) {
      TLOG(TLVL_DEBUG) << "ndigis:" << ndigis;
    }

    std::vector<uint16_t> planes;
    for(unsigned index = 0; index < ndigis; ++index) {
      const auto digi = _digis->at(index);
      const uint16_t plane = digi.strawId().plane();
      if(_debug > 0) {
	TLOG(TLVL_DEBUG+1) << "Straw ID:" << digi.strawId().asUint16() << ", plane:" << plane;
      }
      if(std::find(planes.begin(), planes.end(), plane) == planes.end()) planes.push_back(plane);
    }
    const unsigned nplanes = planes.size();
    retval &= nplanes >= _minnplanes;
    if (retval && _debug > 0){
      TLOG(TLVL_DEBUG) << "passed event:" << event.id();
    } else if(_debug > 1) {
      TLOG(TLVL_DEBUG) << "failed event:" << event.id();
    }
    return _noFilter || retval;
  }

  bool StrawDigiFilter::endRun( art::Run& run ) {
    if(_debug > 0){
      const float rate = (_nevt > 0) ? float(_npass)/float(_nevt) : 0.f;
      TLOG(TLVL_DEBUG) << "passed:" << _npass << " events out of:" << _nevt << " for a ratio of:" << rate;
    }
    return true;
  }
}
DEFINE_ART_MODULE(ots::StrawDigiFilter)
