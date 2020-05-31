FTDI_LIB_PATH  = ../../cores/ft60x_axi/sw
USB_SNIFF_PATH = ../../cores/usb_sniffer/src_c

COMMON_SRC = $(USB_SNIFF_PATH)/capture_bin.cpp
CFLAGS     = -I$(USB_SNIFF_PATH)
LFLAGS     = 
LIBS       = 

TARGETS    = dump_bin
all: $(TARGETS)

$(TARGETS):
	g++ -o $@ $(CFLAGS) $(LFLAGS) $@.cpp $(COMMON_SRC) $(LIBS)

clean:
	-rm -rf $(TARGETS)