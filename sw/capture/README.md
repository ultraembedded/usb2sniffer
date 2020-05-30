## Capture

The *capture* command-line tool configures and enables the capture device.

By default, all USB traffic is captured to 'capture.bin'.

Various filtering options are supported to allow a reduction in capture traffic volume;
* Filter by device address or endpoint.
* Filter out SOF frames.
* Filter out IN+NAK sequences.

### Example
```
$ ./capture 
USB capture started, press ENTER to stop...
Buffer: 172348 entries 0.26%
Dumping capture to file...
```

### Options
```
./capture -h
Usage:
  --device     | -d DEVNUM     Match only this device ID
  --endpoint   | -e EPNUM      Match only this endpoint
  --inverse    | -n            Inverse matching (exclude device / endpoint)
  --no-sof     | -s            Disable SOF collection (breaks timing info)
  --no-in-nak  | -i            Disable IN+NAK capture
  --filename   | -f FILENAME   File to capture to (default: capture.bin)
  --usb-speed  | -u hs|fs|ls   USB speed for timing info (default: hs)
```
