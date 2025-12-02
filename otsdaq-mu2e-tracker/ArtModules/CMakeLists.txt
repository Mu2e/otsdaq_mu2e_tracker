#
cet_build_plugin(TrackerDQM art::module LIBRARIES REG
art_root_io::TFileService_service
artdaq-core-mu2e::Data
ROOT::Hist
ROOT::Tree
ROOT::Core
ROOT::RIO
ROOT::Gui
Offline::RecoDataProducts
)

# cet_build_plugin(StrawDigiFilter art::module LIBRARIES REG
# art_root_io::TFileService_service
# artdaq-core-mu2e::Data
# ROOT::Hist
# ROOT::Tree
# ROOT::Core
# ROOT::RIO
# ROOT::Gui
# Offline::RecoDataProducts
# )

# cet_build_plugin(MakeDigiNtuple art::module LIBRARIES REG
# otsdaq-mu2e-tracker_Nt
# art_root_io::TFileService_service
# artdaq-core-mu2e::Data
# ROOT::Hist
# ROOT::Tree
# ROOT::Core
# ROOT::RIO
# ROOT::Gui
# Offline::RecoDataProducts
# )


# cet_build_plugin(ToyDump art::module LIBRARIES REG artdaq-core-demo::Overlays)
