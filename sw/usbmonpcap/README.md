## Usbmonpcap

The *usbmonpcap* command-line tool takes a previously captured binary file and converts the contents to a high-level USB Linux PCAP format (DLT_USB_LINUX) as per usbmon output (Linux SW USB capture).  
A lot of information from the original capture is lost, but the trade-off is that Wireshark dissectors can display useful high-level protocol details.

### Options
```
./usbmonpcap -h
Usage:
  --filename   | -f FILENAME   File to capture to (default: capture.bin)
  --output     | -o FILENAME   PCAP output file
  --usb-speed  | -u hs|fs|ls   USB speed for timing info (default: hs)
```
