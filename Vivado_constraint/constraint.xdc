## Clock signal
set_property -dict { PACKAGE_PIN E3 IOSTANDARD LVCMOS33 } [get_ports { sys_clock }];
create_clock -add -name sys_clk_pin -period 10.00 -waveform {0 5} [get_ports {sys_clock}];
## Reset (CPU_RESET)
set_property -dict { PACKAGE_PIN C12 IOSTANDARD LVCMOS33 } [get_ports { reset }];
##USB-RS232 Interface
set_property -dict { PACKAGE_PIN C4 IOSTANDARD LVCMOS33 } [get_ports { usb_uart_rxd }];
set_property -dict { PACKAGE_PIN D4 IOSTANDARD LVCMOS33 } [get_ports { usb_uart_txd }];
