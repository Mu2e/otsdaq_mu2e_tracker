
include(artdaq::commandableGenerator)

cet_build_plugin(TrackerVST    artdaq::commandableGenerator LIBRARIES REG artdaq_core_mu2e::Overlays canvas::canvas)
cet_build_plugin(TrackerVST001 artdaq::commandableGenerator LIBRARIES REG artdaq_core_mu2e::Overlays canvas::canvas)
  
