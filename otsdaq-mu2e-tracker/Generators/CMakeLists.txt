
include(artdaq::commandableGenerator)

cet_build_plugin(CalorimeterVST    artdaq::commandableGenerator LIBRARIES 
					REG 
					mu2e_pcie_utils::DTCInterface
					artdaq_core_mu2e::artdaq-core-mu2e_Overlays 
					canvas::canvas)
cet_build_plugin(TrackerVST    artdaq::commandableGenerator LIBRARIES 
			       REG artdaq_core_mu2e::artdaq-core-mu2e_Overlays canvas::canvas mu2e_pcie_utils::DTCInterface)
# cet_build_plugin(TrackerVST001 artdaq::commandableGenerator LIBRARIES 
#			       REG artdaq_core_mu2e::artdaq-core-mu2e_Overlays canvas::canvas mu2e_pcie_utils::DTCInterface)
