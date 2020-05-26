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
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "usb_helpers.h"
#include "log_file_txt.h"

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------
#define TICKS_PER_HS_UFRAME        7500
#define TICKS_PER_FSLS_FRAME       60000
#define DLT_USB_2_0                288
#define DELTA_TO_NS(a)             ((a) * 4267)

//-----------------------------------------------------------------
// Locals
//-----------------------------------------------------------------
static FILE *_file;
static int _in_rst = -1;
static uint64_t _time_stamp;

//-----------------------------------------------------------------
// Types
//-----------------------------------------------------------------
typedef struct pcap_hdr_s
{
    uint32_t magic_number; /* magic number */
    uint16_t version_major; /* major version number */
    uint16_t version_minor; /* minor version number */
    int32_t  thiszone; /* GMT to local correction */
    uint32_t sigfigs; /* accuracy of timestamps */
    uint32_t snaplen; /* max length of captured packets, in octets */
    uint32_t network; /* data link type */
} __attribute__((packed)) pcap_hdr_t;

typedef struct pcaprec_hdr_s
{
    uint32_t ts_sec; /* timestamp seconds */
    uint32_t ts_usec; /* timestamp microseconds */
    uint32_t incl_len; /* number of octets of packet saved in file */
    uint32_t orig_len; /* actual length of packet */
} __attribute__((packed)) pcaprec_hdr_t;

//-----------------------------------------------------------------
// pcap_write: Write packet to PCAP file
//-----------------------------------------------------------------
static void pcap_write(uint8_t *data, int len, uint64_t ts_ns)
{
    pcaprec_hdr_t header;

    header.ts_sec = (ts_ns / 1000) / 1000000000;
    header.ts_usec = (ts_ns / 1000) % 1000000000;
    header.incl_len = len;
    header.orig_len = len;

    fwrite(&header, sizeof(pcaprec_hdr_t), 1, _file);
    fwrite(data, len, 1, _file);
}
//-----------------------------------------------------------------
// pcap_file_add_sof: Add start of frame token to log
//-----------------------------------------------------------------
int pcap_file_add_sof(uint32_t value, int is_hs)
{
    // HS:    125uS
    // FS/LS: 1mS
    _time_stamp += is_hs ? (125000) : (1000000);

    uint16_t frame_num = usb_get_sof_frame(value);

    uint8_t sof_data[3];
    uint8_t crc5 = usb_get_sof_crc5(value);

    sof_data[0] = PID_SOF;
    sof_data[1] = frame_num & 0xFF;
    sof_data[2] = (frame_num >> 8) & 0x7;
    sof_data[2]|= (crc5 << 3);

    pcap_write(sof_data, 3, _time_stamp);
    return 0;    
}
//-----------------------------------------------------------------
// pcap_file_add_rst: Add reset event to the log
//-----------------------------------------------------------------
int pcap_file_add_rst(uint32_t value, int is_hs)
{
    int in_rst = usb_get_rst_state(value);
    return 0;
}
//-----------------------------------------------------------------
// pcap_file_add_token: Add token (IN, OUT, SETUP, PING)
//-----------------------------------------------------------------
int pcap_file_add_token(uint32_t value)
{
    uint8_t token[3];

    uint8_t pid          = usb_get_pid(value);
    uint8_t device       = usb_get_token_device(value);
    uint8_t endpoint     = usb_get_token_endpoint(value);
    uint16_t delta_time  = usb_get_cycle_delta(value);
    uint8_t crc5         = usb_get_token_crc5(value);

    token[0] = pid;
    token[1] = device & 0x7F;
    token[1]|= (endpoint << 7) & 0x80;
    token[2] = (endpoint >> 1) & 0x7;
    token[2]|= (crc5 << 3);

    pcap_write(token, 3, _time_stamp + DELTA_TO_NS(usb_get_cycle_delta(value)));
    return 0;
}
//-----------------------------------------------------------------
// pcap_file_add_split: Add SPLIT
//-----------------------------------------------------------------
int pcap_file_add_split(uint32_t value)
{
    // TODO
    return 0;
}
//-----------------------------------------------------------------
// pcap_file_add_handshake: Add handshake (ACK, NAK, NYET)
//-----------------------------------------------------------------
int pcap_file_add_handshake(uint32_t value)
{
    uint8_t pid = usb_get_pid(value);
    pcap_write(&pid, 1, _time_stamp + DELTA_TO_NS(usb_get_cycle_delta(value)));
    return 0;
}
//-----------------------------------------------------------------
// pcap_file_add_data: Add data packet to log
//-----------------------------------------------------------------
int pcap_file_add_data(uint32_t value, uint8_t *data, int length)
{
    assert(length < 2048);
    uint8_t buffer[2048];
    buffer[0] = usb_get_pid(value);
    memcpy(&buffer[1], data, length);
    pcap_write(buffer, length+1, _time_stamp + DELTA_TO_NS(usb_get_cycle_delta(value)));

    return 0;
}
//-----------------------------------------------------------------
// pcap_file_create: Create & open empty log file
//-----------------------------------------------------------------
int pcap_file_create(const char *filename)
{
    _file = fopen(filename, "w");
    if (_file)
    {
        pcap_hdr_t header;
        header.magic_number  = 0xA1B23C4D;
        header.version_major = 2;
        header.version_minor = 4;
        header.thiszone      = 0;
        header.sigfigs       = 0;
        header.snaplen       = 2048;
        header.network       = DLT_USB_2_0;
        fwrite(&header, sizeof(pcap_hdr_t), 1, _file);
    }

    return _file == NULL ? -1 : 0;
}
//-----------------------------------------------------------------
// pcap_file_close: Close open file handle
//-----------------------------------------------------------------
int pcap_file_close(void)
{
    if (_file != NULL)
        fclose(_file);

    _file = NULL;

    return 0;
}
