


set_property IOSTANDARD LVCMOS33 [get_ports status_classification_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_correction_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_detect_only_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_diagnostic_scan_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_essential_0]
set_property IOSTANDARD LVCMOS33 [get_ports status_uncorrectable_0]

set_property IOSTANDARD LVCMOS33 [get_ports uart1_helper_rx_o ]
set_property IOSTANDARD LVCMOS33 [get_ports uart1_helper_tx_o ]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_txfull_o  ]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_rxempty_o ]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_txwrite_o ]
set_property IOSTANDARD LVCMOS33 [get_ports monitor_rxread_o  ]

set_property PACKAGE_PIN G8 [get_ports uart1_helper_rx_o ]
set_property PACKAGE_PIN G7 [get_ports uart1_helper_tx_o ]
set_property PACKAGE_PIN H6 [get_ports monitor_txfull_o  ]
set_property PACKAGE_PIN J7 [get_ports monitor_rxempty_o ]
set_property PACKAGE_PIN J9 [get_ports monitor_txwrite_o ]
set_property PACKAGE_PIN K8 [get_ports monitor_rxread_o  ]


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



create_pblock pblock_1
add_cells_to_pblock [get_pblocks pblock_1] [get_cells -quiet [list design_1_i/up_cnt_0]]
resize_pblock pblock_1 -add {SLICE_X67Y164:SLICE_X89Y191 DSP48E2_X7Y66:DSP48E2_X11Y75 RAMB18_X2Y66:RAMB18_X2Y75 RAMB36_X2Y33:RAMB36_X2Y37}
set_property EXCLUDE_PLACEMENT 1 [get_pblocks pblock_1]
create_pblock pblock_2
add_cells_to_pblock [get_pblocks pblock_2] [get_cells -quiet [list design_1_i/up_cnt_1]]
resize_pblock pblock_2 -add {SLICE_X66Y134:SLICE_X89Y158 DSP48E2_X7Y54:DSP48E2_X11Y61 RAMB18_X2Y54:RAMB18_X2Y61 RAMB36_X2Y27:RAMB36_X2Y30}
set_property EXCLUDE_PLACEMENT 1 [get_pblocks pblock_2]
create_pblock pblock_3
add_cells_to_pblock [get_pblocks pblock_3] [get_cells -quiet [list design_1_i/up_cnt_2]]
resize_pblock pblock_3 -add {SLICE_X66Y105:SLICE_X89Y130 DSP48E2_X7Y42:DSP48E2_X11Y51 RAMB18_X2Y42:RAMB18_X2Y51 RAMB36_X2Y21:RAMB36_X2Y25}
set_property EXCLUDE_PLACEMENT 1 [get_pblocks pblock_3]

set_property BITSTREAM.SEU.ESSENTIALBITS yes [current_design]


