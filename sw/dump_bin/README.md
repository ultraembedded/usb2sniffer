## Dump Binary

The *dump_bin* command-line tool takes a USB capture and allows extraction of data for a specific device/endpoint to be copied to a new binary file.

### Options
```
./dump_bin -h
Usage:
  --filename   | -f FILENAME   File to capture to (default: capture.bin)
  --device     | -d DEVNUM     Match only this device ID
  --endpoint   | -e EPNUM      Match only this endpoint
  --output     | -o FILENAME   Output file
```
