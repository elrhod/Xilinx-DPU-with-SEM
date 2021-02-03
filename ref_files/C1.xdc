set_property IOSTANDARD LVCMOS33 [get_ports status_classification_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_correction_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_detect_only_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_diagnostic_scan_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_essential_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_uncorrectable_0]

######################################################################
##### ELR: for SEM debug
######################################################################
set_property IOSTANDARD LVCMOS33 [get_ports ICAP_CLK_O]

set_property IOSTANDARD LVCMOS33 [get_ports monitor_txfull_o]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_rxempty_o]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_txwrite_o]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_rxread_o]

set_property IOSTANDARD LVCMOS33 [get_ports ZYNQ_TX_O]
set_property IOSTANDARD LVCMOS33 [get_ports ZYNQ_RX_O]

# SEM RX data from helper block
set_property IOSTANDARD LVCMOS33 [get_ports monitor_RXDATA_O[0]]  
set_property IOSTANDARD LVCMOS33 [get_ports monitor_RXDATA_O[1]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_RXDATA_O[2]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_RXDATA_O[3]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_RXDATA_O[4]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_RXDATA_O[5]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_RXDATA_O[6]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_RXDATA_O[7]]

# SEM TX data to helper block
set_property IOSTANDARD LVCMOS33 [get_ports monitor_TXDATA_O[0]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_TXDATA_O[1]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_TXDATA_O[2]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_TXDATA_O[3]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_TXDATA_O[4]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_TXDATA_O[5]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_TXDATA_O[6]]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_TXDATA_O[7]]

set_property PACKAGE_PIN G8 [get_ports ICAP_CLK_O]

set_property PACKAGE_PIN H7 [get_ports monitor_txfull_o]
set_property PACKAGE_PIN J7 [get_ports monitor_rxempty_o]
set_property PACKAGE_PIN G7 [get_ports monitor_txwrite_o]
set_property PACKAGE_PIN J6 [get_ports monitor_rxread_o]

set_property PACKAGE_PIN H8 [get_ports ZYNQ_TX_O]
set_property PACKAGE_PIN H6 [get_ports ZYNQ_RX_O]

set_property PACKAGE_PIN L8 [get_ports monitor_RXDATA_O[0]]  
set_property PACKAGE_PIN K8 [get_ports monitor_RXDATA_O[1]]
set_property PACKAGE_PIN K9 [get_ports monitor_RXDATA_O[2]]
set_property PACKAGE_PIN J9 [get_ports monitor_RXDATA_O[3]]

##### ELR: for SEM debug
######################################################################
set_property IOSTANDARD LVCMOS33 [get_ports Fifo1_WR_en]
#set_property PACKAGE_PIN J7 [get_ports Fifo1_WR_en]

set_property PACKAGE_PIN D5 [get_ports status_heartbeat_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_heartbeat_0]
set_property PACKAGE_PIN D6 [get_ports status_initialization_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_initialization_0]
set_property PACKAGE_PIN A5 [get_ports status_injection_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_injection_0]
set_property PACKAGE_PIN B5 [get_ports status_observation_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_observation_0]

set_property PACKAGE_PIN F6 [get_ports status_classification_0]
set_property PACKAGE_PIN E5 [get_ports status_correction_0]
set_property PACKAGE_PIN F5 [get_ports status_detect_only_0]
set_property PACKAGE_PIN F4 [get_ports status_diagnostic_scan_0]
set_property PACKAGE_PIN E4 [get_ports status_essential_0]
set_property PACKAGE_PIN D4 [get_ports status_uncorrectable_0]

set_property BITSTREAM.SEU.ESSENTIALBITS yes [current_design]


