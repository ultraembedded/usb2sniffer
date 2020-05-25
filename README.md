# USB2.0 Capture Project (for LambdaConcept USB2Sniffer hardware)

Github: [https://github.com/ultraembedded/usb2sniffer](https://github.com/ultraembedded/usb2sniffer)

## Intro
This repo contains an alternative set of FPGA gateware and SW for performing USB2.0 capture with the [LambaConcept USB2Sniffer](http://blog.lambdaconcept.com/doku.php?id=products:usb_sniffer) FPGA board ([buy](http://shop.lambdaconcept.com/home/35-usb2-sniffer.html)).
![USB2Sniffer](docs/usb2sniffer_board.jpg)

## Getting Started

#### Cloning

To clone this project and its dependencies;

```
git clone --recursive https://github.com/ultraembedded/usb2sniffer.git

```

## IP Designs Used

Most of the IP cores used in this project are designed by myself and available as easy to follow open-source Verilog modules.
The remainder (DDR3, CDC, PLL) are IP cores built with Xilinx Vivado.

| Name                   | Description                                                 | Provider |
| ---------------------- | ------------------------------------------------------------| -------- |
| ulpi_wrapper           | [ULPI PHY Interface](https://github.com/ultraembedded/core_ulpi_wrapper) | - |
| ft60x_axi              | [FTDI USB3.0 to AXI bus master](https://github.com/ultraembedded/core_ft60x_axi) | - |
| mig_axis.xci           | [MIG DDR3 Controller](https://github.com/ultraembedded/usb2sniffer/blob/master/cores/ddr/mig_axis.xci) | Xilinx |
| axi_cdc_buffer.xci     | [AXI4 Clock Domain Converter](https://github.com/ultraembedded/usb2sniffer/blob/master/cores/cdc/axi_cdc_buffer.xci) | Xilinx |
