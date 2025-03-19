#
# cet_build_plugin(TrkFragmentAna art::module LIBRARIES REG
# art_root_io::TFileService_service
# artdaq_core_mu2e::artdaq-core-mu2e_Data
# ROOT::Hist
# ROOT::Tree
# ROOT::Core
# ROOT::RIO
# ROOT::Gui
# Offline::RecoDataProducts
# )

cet_build_plugin(CaloDigiAnalyzerExample art::module
    REG_SOURCE CaloDigiAnalyzerExample_module.cc
    LIBRARIES REG
     artdaq_core_mu2e::artdaq-core-mu2e_Data
     artdaq_core_mu2e::artdaq-core-mu2e_Data_dict
     artdaq_core_mu2e::artdaq-core-mu2e_Overlays
     artdaq::DAQdata
     art_root_io::TFileService_service
     Offline::DAQ
     ROOT::Hist
     ROOT::Tree
)


# cet_build_plugin(ToyDump art::module LIBRARIES REG artdaq_core_demo::artdaq-core-demo_Overlays)
