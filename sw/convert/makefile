###############################################################################
## Makefile
###############################################################################

# Target
TARGET     ?= convert

USB_SNIFF_PATH = ../../cores/usb_sniffer/src_c

# Options
CFLAGS      = -I$(USB_SNIFF_PATH)
LDFLAGS     = 
LIBS        = 

ARGS       += 

# Source Files
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC)) 

###############################################################################
# Rules
###############################################################################
all: $(TARGET)
    
clean:
	-rm $(OBJ) $(TARGET)

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJ)
	g++ $(LDFLAGS) $(OBJ) $(LIBS) -o $@
