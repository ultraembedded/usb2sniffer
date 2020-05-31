TARGET_HELP+= "Help:\n"
all: help

TARGET_HELP += "make create_mcs    - Build a MCS file for the onboard SPI-Flash containing the bitstream.\n"
create_mcs:
	vivado -mode tcl -source create_prom_mcs.tcl

TARGET_HELP += "make program_flash - Flash MCS file onto target board SPI-Flash\n"
program_flash:
	vivado -mode tcl -source program_flash.tcl

TARGET_HELP += "make clean         - Cleanup build files.\n"
clean:
	rm -rf usb2sniffer.*

help:
	@echo $(TARGET_HELP)