## Clock signal
set_property -dict { PACKAGE_PIN J19    IOSTANDARD LVCMOS33 } [get_ports { clk100_i }];
create_clock -add -name sys_clk_pin -period 10.00 -waveform {0 5} [get_ports { clk100_i }];

set_property INTERNAL_VREF 0.750 [get_iobanks 35]

## RGB LEDs
set_property -dict { PACKAGE_PIN W2    IOSTANDARD LVCMOS33 } [get_ports { led0_r_o }];
set_property -dict { PACKAGE_PIN Y1    IOSTANDARD LVCMOS33 } [get_ports { led0_g_o }];
set_property -dict { PACKAGE_PIN W1    IOSTANDARD LVCMOS33 } [get_ports { led0_b_o }];
set_property -dict { PACKAGE_PIN AA1   IOSTANDARD LVCMOS33 } [get_ports { led1_r_o }];
set_property -dict { PACKAGE_PIN AB1   IOSTANDARD LVCMOS33 } [get_ports { led1_g_o }];
set_property -dict { PACKAGE_PIN Y2    IOSTANDARD LVCMOS33 } [get_ports { led1_b_o }];

## ftdi:0.clk
set_property PACKAGE_PIN D17 [get_ports ftdi_clk]
set_property IOSTANDARD LVCMOS33 [get_ports ftdi_clk]
## ftdi:0.rst
set_property PACKAGE_PIN K22 [get_ports ftdi_rst]
set_property IOSTANDARD LVCMOS33 [get_ports ftdi_rst]
set_property SLEW FAST [get_ports ftdi_rst]
## ftdi:0.data
set_property PACKAGE_PIN A16 [get_ports {ftdi_data[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[0]}]
set_property SLEW FAST [get_ports {ftdi_data[0]}]
## ftdi:0.data
set_property PACKAGE_PIN F14 [get_ports {ftdi_data[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[1]}]
set_property SLEW FAST [get_ports {ftdi_data[1]}]
## ftdi:0.data
set_property PACKAGE_PIN A15 [get_ports {ftdi_data[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[2]}]
set_property SLEW FAST [get_ports {ftdi_data[2]}]
## ftdi:0.data
set_property PACKAGE_PIN F13 [get_ports {ftdi_data[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[3]}]
set_property SLEW FAST [get_ports {ftdi_data[3]}]
## ftdi:0.data
set_property PACKAGE_PIN A14 [get_ports {ftdi_data[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[4]}]
set_property SLEW FAST [get_ports {ftdi_data[4]}]
## ftdi:0.data
set_property PACKAGE_PIN E14 [get_ports {ftdi_data[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[5]}]
set_property SLEW FAST [get_ports {ftdi_data[5]}]
## ftdi:0.data
set_property PACKAGE_PIN A13 [get_ports {ftdi_data[6]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[6]}]
set_property SLEW FAST [get_ports {ftdi_data[6]}]
## ftdi:0.data
set_property PACKAGE_PIN E13 [get_ports {ftdi_data[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[7]}]
set_property SLEW FAST [get_ports {ftdi_data[7]}]
## ftdi:0.data
set_property PACKAGE_PIN B13 [get_ports {ftdi_data[8]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[8]}]
set_property SLEW FAST [get_ports {ftdi_data[8]}]
## ftdi:0.data
set_property PACKAGE_PIN C15 [get_ports {ftdi_data[9]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[9]}]
set_property SLEW FAST [get_ports {ftdi_data[9]}]
## ftdi:0.data
set_property PACKAGE_PIN C13 [get_ports {ftdi_data[10]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[10]}]
set_property SLEW FAST [get_ports {ftdi_data[10]}]
## ftdi:0.data
set_property PACKAGE_PIN C14 [get_ports {ftdi_data[11]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[11]}]
set_property SLEW FAST [get_ports {ftdi_data[11]}]
## ftdi:0.data
set_property PACKAGE_PIN B16 [get_ports {ftdi_data[12]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[12]}]
set_property SLEW FAST [get_ports {ftdi_data[12]}]
## ftdi:0.data
set_property PACKAGE_PIN E17 [get_ports {ftdi_data[13]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[13]}]
set_property SLEW FAST [get_ports {ftdi_data[13]}]
## ftdi:0.data
set_property PACKAGE_PIN B15 [get_ports {ftdi_data[14]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[14]}]
set_property SLEW FAST [get_ports {ftdi_data[14]}]
## ftdi:0.data
set_property PACKAGE_PIN F16 [get_ports {ftdi_data[15]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[15]}]
set_property SLEW FAST [get_ports {ftdi_data[15]}]
## ftdi:0.data
set_property PACKAGE_PIN A20 [get_ports {ftdi_data[16]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[16]}]
set_property SLEW FAST [get_ports {ftdi_data[16]}]
## ftdi:0.data
set_property PACKAGE_PIN E18 [get_ports {ftdi_data[17]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[17]}]
set_property SLEW FAST [get_ports {ftdi_data[17]}]
## ftdi:0.data
set_property PACKAGE_PIN B20 [get_ports {ftdi_data[18]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[18]}]
set_property SLEW FAST [get_ports {ftdi_data[18]}]
## ftdi:0.data
set_property PACKAGE_PIN F18 [get_ports {ftdi_data[19]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[19]}]
set_property SLEW FAST [get_ports {ftdi_data[19]}]
## ftdi:0.data
set_property PACKAGE_PIN D19 [get_ports {ftdi_data[20]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[20]}]
set_property SLEW FAST [get_ports {ftdi_data[20]}]
## ftdi:0.data
set_property PACKAGE_PIN D21 [get_ports {ftdi_data[21]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[21]}]
set_property SLEW FAST [get_ports {ftdi_data[21]}]
## ftdi:0.data
set_property PACKAGE_PIN E19 [get_ports {ftdi_data[22]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[22]}]
set_property SLEW FAST [get_ports {ftdi_data[22]}]
## ftdi:0.data
set_property PACKAGE_PIN E21 [get_ports {ftdi_data[23]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[23]}]
set_property SLEW FAST [get_ports {ftdi_data[23]}]
## ftdi:0.data
set_property PACKAGE_PIN A21 [get_ports {ftdi_data[24]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[24]}]
set_property SLEW FAST [get_ports {ftdi_data[24]}]
## ftdi:0.data
set_property PACKAGE_PIN B21 [get_ports {ftdi_data[25]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[25]}]
set_property SLEW FAST [get_ports {ftdi_data[25]}]
## ftdi:0.data
set_property PACKAGE_PIN A19 [get_ports {ftdi_data[26]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[26]}]
set_property SLEW FAST [get_ports {ftdi_data[26]}]
## ftdi:0.data
set_property PACKAGE_PIN A18 [get_ports {ftdi_data[27]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[27]}]
set_property SLEW FAST [get_ports {ftdi_data[27]}]
## ftdi:0.data
set_property PACKAGE_PIN F20 [get_ports {ftdi_data[28]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[28]}]
set_property SLEW FAST [get_ports {ftdi_data[28]}]
## ftdi:0.data
set_property PACKAGE_PIN F19 [get_ports {ftdi_data[29]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[29]}]
set_property SLEW FAST [get_ports {ftdi_data[29]}]
## ftdi:0.data
set_property PACKAGE_PIN B18 [get_ports {ftdi_data[30]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[30]}]
set_property SLEW FAST [get_ports {ftdi_data[30]}]
## ftdi:0.data
set_property PACKAGE_PIN B17 [get_ports {ftdi_data[31]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_data[31]}]
set_property SLEW FAST [get_ports {ftdi_data[31]}]
## ftdi:0.be
set_property PACKAGE_PIN K16 [get_ports {ftdi_be[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_be[0]}]
set_property SLEW FAST [get_ports {ftdi_be[0]}]
## ftdi:0.be
set_property PACKAGE_PIN L16 [get_ports {ftdi_be[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_be[1]}]
set_property SLEW FAST [get_ports {ftdi_be[1]}]
## ftdi:0.be
set_property PACKAGE_PIN G20 [get_ports {ftdi_be[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_be[2]}]
set_property SLEW FAST [get_ports {ftdi_be[2]}]
## ftdi:0.be
set_property PACKAGE_PIN H20 [get_ports {ftdi_be[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ftdi_be[3]}]
set_property SLEW FAST [get_ports {ftdi_be[3]}]
## ftdi:0.rxf_n
set_property PACKAGE_PIN M13 [get_ports ftdi_rxf_n]
set_property IOSTANDARD LVCMOS33 [get_ports ftdi_rxf_n]
## ftdi:0.txe_n
set_property PACKAGE_PIN L13 [get_ports ftdi_txe_n]
set_property IOSTANDARD LVCMOS33 [get_ports ftdi_txe_n]
## ftdi:0.rd_n
set_property PACKAGE_PIN K19 [get_ports ftdi_rd_n]
set_property IOSTANDARD LVCMOS33 [get_ports ftdi_rd_n]
set_property SLEW FAST [get_ports ftdi_rd_n]
## ftdi:0.wr_n
set_property PACKAGE_PIN M15 [get_ports ftdi_wr_n]
set_property IOSTANDARD LVCMOS33 [get_ports ftdi_wr_n]
set_property SLEW FAST [get_ports ftdi_wr_n]
## ftdi:0.oe_n
set_property PACKAGE_PIN L21 [get_ports ftdi_oe_n]
set_property IOSTANDARD LVCMOS33 [get_ports ftdi_oe_n]
set_property SLEW FAST [get_ports ftdi_oe_n]
## ftdi:0.siwua
set_property PACKAGE_PIN M16 [get_ports ftdi_siwua]
set_property IOSTANDARD LVCMOS33 [get_ports ftdi_siwua]
set_property SLEW FAST [get_ports ftdi_siwua]

create_clock -period 10.000 -name clock_ftdi -add [get_ports ftdi_clk]
set_clock_groups -name clock_ftdi_grp -asynchronous -group [get_clocks clock_ftdi]

# USB switch
set_property PACKAGE_PIN Y8 [get_ports ulpi_sw_s]
set_property IOSTANDARD LVCMOS33 [get_ports ulpi_sw_s]
set_property PACKAGE_PIN Y9 [get_ports ulpi_sw_oe_n]
set_property IOSTANDARD LVCMOS33 [get_ports ulpi_sw_oe_n]

# ULPI Interface (USB Host)
set_property PACKAGE_PIN W19 [get_ports ulpi0_clk60_i]
set_property IOSTANDARD LVCMOS33 [get_ports ulpi0_clk60_i]
set_property PACKAGE_PIN AB18 [get_ports {ulpi0_data_io[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ulpi0_data_io[0]}]
set_property SLEW FAST [get_ports {ulpi0_data_io[0]}]
set_property PACKAGE_PIN AA18 [get_ports {ulpi0_data_io[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ulpi0_data_io[1]}]
set_property SLEW FAST [get_ports {ulpi0_data_io[1]}]
set_property PACKAGE_PIN AA19 [get_ports {ulpi0_data_io[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ulpi0_data_io[2]}]
set_property SLEW FAST [get_ports {ulpi0_data_io[2]}]
set_property PACKAGE_PIN AB20 [get_ports {ulpi0_data_io[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ulpi0_data_io[3]}]
set_property SLEW FAST [get_ports {ulpi0_data_io[3]}]
set_property PACKAGE_PIN AA20 [get_ports {ulpi0_data_io[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ulpi0_data_io[4]}]
set_property SLEW FAST [get_ports {ulpi0_data_io[4]}]
set_property PACKAGE_PIN AB21 [get_ports {ulpi0_data_io[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ulpi0_data_io[5]}]
set_property SLEW FAST [get_ports {ulpi0_data_io[5]}]
set_property PACKAGE_PIN AA21 [get_ports {ulpi0_data_io[6]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ulpi0_data_io[6]}]
set_property SLEW FAST [get_ports {ulpi0_data_io[6]}]
set_property PACKAGE_PIN AB22 [get_ports {ulpi0_data_io[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ulpi0_data_io[7]}]
set_property SLEW FAST [get_ports {ulpi0_data_io[7]}]
set_property PACKAGE_PIN W21 [get_ports ulpi0_dir_i]
set_property IOSTANDARD LVCMOS33 [get_ports ulpi0_dir_i]
set_property PACKAGE_PIN Y22 [get_ports ulpi0_stp_o]
set_property IOSTANDARD LVCMOS33 [get_ports ulpi0_stp_o]
set_property SLEW FAST [get_ports ulpi0_stp_o]
set_property PACKAGE_PIN W22 [get_ports ulpi0_nxt_i]
set_property IOSTANDARD LVCMOS33 [get_ports ulpi0_nxt_i]
set_property PACKAGE_PIN V20 [get_ports ulpi0_reset_o]
set_property IOSTANDARD LVCMOS33 [get_ports ulpi0_reset_o]
set_property SLEW FAST [get_ports ulpi0_reset_o]

create_clock -period 16.667 -name ulpi0_clk -add [get_ports ulpi0_clk60_i]
set_clock_groups -name ulpi0_clk_grp -asynchronous -group [get_clocks ulpi0_clk]