## Dump

The *dump* command-line tool takes a previously captured binary file and displays the contents.
Data transactions which are ACK'd or STALL'd are displayed, all other traffic is not.

### Example
```
$ ./dump -f ../capture/capture.bin
USB Device Reset
SETUP Device 0 Endpoint 0
  DATA0: Length 8
  80 06 00 01 00 00 40 00 
  ACK
IN Device 0 Endpoint 0
  DATA1: Length 8
  12 01 00 02 02 00 00 08 
  ACK
OUT Device 0 Endpoint 0
  DATA1: Length 0
  ACK
USB Device Reset
SETUP Device 0 Endpoint 0
  DATA0: Length 8
  00 05 24 00 00 00 00 00 
  ACK
IN Device 0 Endpoint 0
  DATA1: Length 0
  ACK
SETUP Device 36 Endpoint 0
  DATA0: Length 8
  80 06 00 01 00 00 12 00 
  ACK
IN Device 36 Endpoint 0
  DATA1: Length 8
  12 01 00 02 02 00 00 08 
  ACK
IN Device 36 Endpoint 0
  DATA0: Length 8
  50 1d 49 61 01 01 00 00 
  ACK
IN Device 36 Endpoint 0
  DATA1: Length 2
  00 01 
  ACK
```

### Options
```
./dump -h
Usage:
  --filename   | -f FILENAME   Input file name (e.g. capture.bin)
  --usb-speed  | -u hs|fs|ls   USB speed for timing info (default: hs)
```
