FTDI_LIB_PATH  = ../../cores/ft60x_axi/sw
USB_SNIFF_PATH = ../../cores/usb_sniffer/src_c

COMMON_SRC = $(FTDI_LIB_PATH)/ftdi_axi_driver.cpp $(FTDI_LIB_PATH)/ftdi_ft60x.cpp
CFLAGS     = -I$(USB_SNIFF_PATH) -I$(FTDI_LIB_PATH) -I$(FTDI_LIB_PATH)/linux-x86_64
LFLAGS     = -L$(FTDI_LIB_PATH)/linux-x86_64
LIBS       = -l:libftd3xx.so

TARGETS    = capture
all: $(TARGETS)

$(TARGETS):
	g++ -o $@ $(CFLAGS) $(LFLAGS) $@.cpp $(COMMON_SRC) $(LIBS)

clean:
	-rm -rf $(TARGETS)