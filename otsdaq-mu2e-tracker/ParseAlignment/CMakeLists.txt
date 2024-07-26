
cet_make_library(
        SOURCE          
        Alignment.cc
        AlignmentChannel.cc
        AlignmentIteration.cc
        PrintLegacyTable.cc
	LIBRARIES
	cetlib_except
        mu2e_pcie_utils::DTCInterface
        )

install_headers()
install_source()
