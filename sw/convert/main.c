//-----------------------------------------------------------------
//                       USB Sniffer
//                           V0.1
//                     Ultra-Embedded.com
//                    Copyright 2015-2020
//
//               Email: admin@ultra-embedded.com
//
//                       License: LGPL
//-----------------------------------------------------------------
//
// Copyright (C) 2015 - 2020 Ultra-Embedded.com
//
// This source file may be used and distributed without         
// restriction provided that this copyright statement is not    
// removed from the file and that any derivative work contains  
// the original copyright notice and the associated disclaimer. 
//
// This source file is free software; you can redistribute it   
// and/or modify it under the terms of the GNU Lesser General   
// Public License as published by the Free Software Foundation; 
// either version 2.1 of the License, or (at your option) any   
// later version.
//
// This source is distributed in the hope that it will be       
// useful, but WITHOUT ANY WARRANTY; without even the implied   
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR      
// PURPOSE.  See the GNU Lesser General Public License for more 
// details.
//
// You should have received a copy of the GNU Lesser General    
// Public License along with this source; if not, write to the 
// Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
// Boston, MA  02111-1307  USA
//-----------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <getopt.h>
#include <stdint.h>

#include "log_format.h"
#include "usb_helpers.h"
#include "log_file.h"

//-----------------------------------------------------------------
// Command line options
//-----------------------------------------------------------------
#define GETOPTS_ARGS "f:o:h"

static struct option long_options[] =
{
    {"filename",     required_argument, 0, 'f'},
    {"output",       required_argument, 0, 'o'},
    {"usb-speed",    required_argument, 0, 'u'},
    {"help",         no_argument,       0, 'h'},
    {0, 0, 0, 0}
};

static void help_options(void)
{
    fprintf (stderr,"Usage:\n");
    fprintf (stderr,"  --filename   | -f FILENAME   Input file name (e.g. capture.bin)\n");
    fprintf (stderr,"  --output     | -o FILENAME   Output file in either .txt, .pcap, .usb format (default: capture.pcap)\n");
    fprintf (stderr,"  --usb-speed  | -u hs|fs|ls   USB speed for timing info (default: hs)\n");
    exit(-1);
}
//-----------------------------------------------------------------
// convert_file
//-----------------------------------------------------------------
static int convert_file(const char *src_file, const char *dst_file, tUsbSpeed speed)
{
    int err = 0;
    int i,j;
    uint32_t value = 0;
    uint32_t data = 0;
    uint8_t  usb_data[MAX_PACKET_SIZE];
    int      usb_idx;
    int      idx = 0;
    long     buf_size = 0;

    FILE *f = fopen(src_file, "rb");
    if (!f)
    {
        fprintf(stderr, "ERROR: Could not open input file\n");
        return -1;
    }

    // Get size of capture.bin
    fseek(f, 0, SEEK_END);
    buf_size = ftell(f);
    rewind(f);

    // No captured data...
    if (buf_size == 0)
    {
        fprintf(stderr, "ERROR: Empty capture file\n");
        fclose(f);
        return -1;
    }

    // Create log file
    if (log_file_create(dst_file) != 0)
    {
        fprintf(stderr, "ERROR: Could not create output file\n");
        fclose(f);
        return -1;
    }

    // Iterate through binary and convert to another file format
    while (idx < (buf_size / 4) && !err)
    {
        fread(&value, 4, 1, f);
        idx++;

        switch ((value >> LOG_CTRL_TYPE_L) & LOG_CTRL_CYCLE_MASK)
        {
            case LOG_CTRL_TYPE_SOF:
                log_file_add_sof(value, speed == USB_SPEED_HS);
                break;
            case LOG_CTRL_TYPE_RST:
                log_file_add_rst(value, speed == USB_SPEED_HS);
                break;
            case LOG_CTRL_TYPE_TOKEN:
                log_file_add_token(value);
                break;
            case LOG_CTRL_TYPE_SPLIT:
                log_file_add_split(value);
                break;
            case LOG_CTRL_TYPE_HSHAKE:
                log_file_add_handshake(value);
                break;
            case LOG_CTRL_TYPE_DATA:
            {
                uint32_t len = usb_get_data_length(value);
                if (len > MAX_PACKET_SIZE)
                {
                    fprintf(stderr, "ERROR: Corrupt capture file\n");
                    err = 1;
                    break;
                }

                usb_idx = 0;
                for (i = 0; i < len; i+= 4)
                {
                    fread(&data, 4, 1, f);
                    idx++;
                    
                    for (j=0;j<4 && usb_idx < len;j++)
                        usb_data[usb_idx++] = data >> (8 * j);
                }

                log_file_add_data(value, usb_data, len);
            }
            break;
            default:
                printf("ERROR: Unknown ID %x, corrupt capture file\n", value);
                err = 1;
                break;
        }
    }

    log_file_close();
    fclose(f);
    return err ? -1 : 0;
}
//-----------------------------------------------------------------
// main
//-----------------------------------------------------------------
int main(int argc, char *argv[])
{
    char *dst_file = "capture.pcap";
    char *src_file = NULL;
    int   c;
    int   help = 0;
    int   option_index = 0;
    tUsbSpeed speed = USB_SPEED_HS;

    while ((c = getopt_long (argc, argv, GETOPTS_ARGS, long_options, &option_index)) != -1)
    {
        switch(c)
        {
            case 'o': // Filename
                dst_file = optarg;
                break;
            case 'f': // Filename
                src_file = optarg;
                break;
            case 'u': // Speed
                if (strcmp(optarg, "hs") == 0)
                    speed = USB_SPEED_HS;
                else if (strcmp(optarg, "fs") == 0)
                    speed = USB_SPEED_FS;
                else if (strcmp(optarg, "ls") == 0)
                    speed = USB_SPEED_LS;
                else
                {
                    fprintf (stderr,"ERROR: Incorrect speed selection\n");
                    help = 1;
                }
                break;
            default:
                help = 1;
                break;
        }
    }

    if (help || src_file == NULL)
    {
        help_options();
        return -1;
    }

    return convert_file(src_file, dst_file, speed);
}

