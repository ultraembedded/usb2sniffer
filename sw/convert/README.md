## Convert

The *convert* command-line tool takes a previously captured binary file and converts to PCAP, text file, or to a USB file format output file.

By default, the output file created is called 'capture.pcap'.

### Example
```
$ ./convert -f ../capture/capture.bin -o capture.txt
$ more capture.txt
SOF - Frame 12
SOF - Frame 12
SSPLIT Hub Addr 12
OUT Device 30 Endpoint 1
  DATA0: Length 176
  8d ff 13 ff 7b ff 09 ff 5e ff fa fe 3c ff e3 fe 
  17 ff bf fe ee fe 8e fe cd fe 6c fe b4 fe 5d fe 
  a1 fe 5a fe 97 fe 5f fe 94 fe 62 fe 94 fe 5d fe 
  8b fe 53 fe 84 fe 59 fe 92 fe 74 fe a3 fe 8b fe 
  a3 fe 91 fe 99 fe 8d fe 8f fe 84 fe 8f fe 7b fe 
  98 fe 7b fe 96 fe 7c fe 8b fe 79 fe 8d fe 7b fe 
  90 fe 75 fe 89 fe 63 fe 84 fe 56 fe 84 fe 57 fe 
  85 fe 60 fe 79 fe 5c fe 5e fe 42 fe 4f fe 32 fe 
  50 fe 30 fe 49 fe 2a fe 3e fe 21 fe 39 fe 1d fe 
  37 fe 17 fe 32 fe 0b fe 32 fe 09 fe 3d fe 17 fe 
  4f fe 26 fe 5e fe 30 fe 5f fe 33 fe 59 fe 38 fe 
  CRC = 6603 (Expected = 6603)
SOF - Frame 13
SOF - Frame 13
SOF - Frame 13
SOF - Frame 13
SOF - Frame 13
SOF - Frame 13
SOF - Frame 13
SOF - Frame 13
SSPLIT Hub Addr 12
```

### Options
```
./convert -h
Usage:
  --filename   | -f FILENAME   Input file name (e.g. capture.bin)
  --output     | -o FILENAME   Output file in either .txt, .pcap, .usb format (default: capture.pcap)
  --usb-speed  | -u hs|fs|ls   USB speed for timing info (default: hs)
```
