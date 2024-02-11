#
cet_build_plugin(TrackerDQM art::module LIBRARIES REG
art_root_io::TFileService_service
artdaq_core_mu2e::artdaq-core-mu2e_Data
ROOT::Hist
ROOT::Tree
ROOT::Core
ROOT::RIO
ROOT::Gui
)


# cet_build_plugin(ToyDump art::module LIBRARIES REG artdaq_core_demo::artdaq-core-demo_Overlays)
