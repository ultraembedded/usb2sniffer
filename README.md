# USB2Sniffer: High Speed USB 2.0 capture (alternate SW/FPGA bitstream for LambdaConcept USB2Sniffer)

Github: [https://github.com/ultraembedded/usb2sniffer](https://github.com/ultraembedded/usb2sniffer)

## Intro
This repo contains an alternative set of FPGA gateware and SW for performing USB2.0 capture with the [LambaConcept USB2Sniffer](http://blog.lambdaconcept.com/doku.php?id=products:usb_sniffer) FPGA board ([buy](http://shop.lambdaconcept.com/home/35-usb2-sniffer.html)).

The key difference between this and the stock FPGA gateware for this board is that this version contains more advanced USB protocol decoding within the FPGA, allowing for more powerful on-target device id / endpoint / SOF filtering.  
Capturing USB traffic is done via a command-line tool, and the resulting binary can be converted into several 3rd party output formats for display or can be parsed using the provided SW library.

![USB2Sniffer](docs/usb2sniffer_board.jpg)

## Getting Started

#### Cloning

To clone this project and its dependencies;

```
git clone --recursive https://github.com/ultraembedded/usb2sniffer.git

```

#### Usage

With the bitstream loaded onto the target board, the following sequence can be used to capture some USB traffic;

```
# Build the command line applications
cd sw
make

# Start a capture
./capture/capture -f /tmp/capture.bin

# Convert to PCAP
./convert/convert -f /tmp/capture.bin -o capture.pcap

# Convert to txt file
./convert/convert -f /tmp/capture.bin -o capture.txt

# Convert to USB file (readable using LcSniff)
./convert/convert -f /tmp/capture.bin -o capture.usb
```

#### LambdaConcept Lcsniff

After converting captured data from binary to .usb file format, the [LambdaConcept Lcsniff GUI](https://github.com/lambdaconcept/usb2sniffer-qt) can be used to view the captured data;

![Lcsniff](docs/lcsniff.png)

#### Wireshark - Raw USB (DLT_USB_2_0)

To view PCAPs with Wireshark, a recent version is required (>= 2019).

To get a recent dev build of Wireshark on Ubuntu/Debian/Mint;
```
sudo add-apt-repository ppa:wireshark-dev/stable
sudo apt-get update
sudo apt-get install wireshark
```

![Wireshark Raw](docs/wireshark.png)

#### Wireshark - Decoded data (DLT_USB_LINUX)

Wireshark has some more advanced protocol dissectors available if you are willing to lose some lower-level USB details;
```
./usbmonpcap/usbmonpcap -f /tmp/capture.bin -o capture.pcap
```

![Wireshark Decoded](docs/wireshark_usbmonpcap.png)


#### Example - Audio capture

As a demo, the following example shows how the USB2Sniffer can be used to capture USB audio traffic;
```
# Find the USB audio device number
$ lsusb
Bus 001 Device 025: ID 0d8c:013c C-Media Electronics, Inc. CM108 Audio Controller

# Start the capture
./sw/capture/capture -f capture_audio.bin

# [Play some audio through the USB device, stop the capture]

# Extract stream of bytes going to device 25 (audio controller) endpoint 1 (audio output)
./sw/dump_bin/dump_bin -f capture_audio.bin -o capture.pcm -d 25 -e 1

# Convert back to WAV file using FFMpeg
ffmpeg -ac 2 -f s16le -i capture.pcm output.wav

# Done..
```


## IP Designs Used

Most of the IP cores used in this project are designed by myself and available as easy to follow open-source Verilog modules.
The remainder (DDR3, CDC, PLL) are IP cores built with Xilinx Vivado.

| Name                   | Description                                                 | Provider |
| ---------------------- | ------------------------------------------------------------| -------- |
| usb_sniffer            | [USB Sniffer Core](https://github.com/ultraembedded/core_usb_sniffer) | - |
| ulpi_wrapper           | [ULPI PHY Interface](https://github.com/ultraembedded/core_ulpi_wrapper) | - |
| ft60x_axi              | [FTDI USB3.0 to AXI bus master](https://github.com/ultraembedded/core_ft60x_axi) | - |
| mig_axis.xci           | [MIG DDR3 Controller](https://github.com/ultraembedded/usb2sniffer/blob/master/cores/ddr/mig_axis.xci) | Xilinx |
| axi_cdc_buffer.xci     | [AXI4 Clock Domain Converter](https://github.com/ultraembedded/usb2sniffer/blob/master/cores/cdc/axi_cdc_buffer.xci) | Xilinx |
