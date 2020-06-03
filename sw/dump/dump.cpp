#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <getopt.h>
#include "capture_bin.h"
#include "usb_defs.h"

//-----------------------------------------------------------------
// Command line options
//-----------------------------------------------------------------
#define GETOPTS_ARGS "f:u:h"

static struct option long_options[] =
{
    {"filename",     required_argument, 0, 'f'},
    {"usb-speed",    required_argument, 0, 'u'},
    {"help",         no_argument,       0, 'h'},
    {0, 0, 0, 0}
};

static void help_options(void)
{
    fprintf (stderr,"Usage:\n");
    fprintf (stderr,"  --filename   | -f FILENAME   File to capture to (default: capture.bin)\n");
    fprintf (stderr,"  --usb-speed  | -u hs|fs|ls   USB speed for timing info (default: hs)\n");
    exit(-1);
}
//-----------------------------------------------------------------
// convert_txt: Convert binary log to text
//-----------------------------------------------------------------
class convert_txt: public capture_bin
{
public:
    convert_txt(FILE *f)
    {
        m_file         = f;
        m_token_pid    = 0;
        m_token_dev    = 0;
        m_token_ep     = 0;
        m_data_pid     = 0;
        m_data_len     = 0;
        m_hshake       = 0;
        m_ctrl_pending = false;
    }
    //-----------------------------------------------------------------
    // output_ctrl: Output control frame
    //-----------------------------------------------------------------
    void output_ctrl(bool ok)
    {
        m_ctrl_pending = false;

        fprintf(m_file, "SETUP Device %d Endpoint %d\n", m_token_dev, m_token_ep);
        fprintf(m_file, "  SETUP_PKT: ");
        for (int i=0;i<MAX_SETUP_SIZE;i++)
        {
            fprintf(m_file, "%02x ", m_ctrl_setup[i]);

            if (!((i+1) & 0xF) || ((i+1) == MAX_SETUP_SIZE))
                fprintf(m_file, "\n  ");
        }
        fprintf(m_file, "\r");

        struct usb_setup_pkt *setup = (struct usb_setup_pkt *)m_ctrl_setup;
        fprintf(m_file, "   mRequestType: 0x%02x (%s)\n", setup->bmRequestType, bmRequestTypeStr(setup->bmRequestType));
        fprintf(m_file, "   bRequest:     0x%02x (%s)\n", setup->bRequest, bRequestStr(setup->bmRequestType, setup->bRequest));
        fprintf(m_file, "   wValue:       0x%04x\n", setup->wValue);
        fprintf(m_file, "   wIndex:       0x%04x\n", setup->wIndex);
        fprintf(m_file, "   wLength:      0x%04x\n", setup->wLength);

        fprintf(m_file, "  STATUS: %s\n", ok ? "OK" : "ERROR");

        // Payload / response / status
        if (ok && m_ctrl_offset)
        {
            fprintf(m_file, "  PAYLOAD: %d bytes\n    ", m_ctrl_offset);
            for (int i=0;i<m_ctrl_offset;i++)
            {
                fprintf(m_file, "%02x ", m_ctrl_payload[i]);

                if (!((i+1) & 0xF) || ((i+1) == m_ctrl_offset))
                    fprintf(m_file, "\n    ");
            }
            fprintf(m_file, "\r");
        }
    }
    //-----------------------------------------------------------------
    // output_bulk: Output bulk frame
    //-----------------------------------------------------------------
    void output_bulk(uint16_t data_len, bool ok)
    {
        fprintf(m_file, "%s Device %d Endpoint %d\n", capture_bin::get_pid_str(m_token_pid), m_token_dev, m_token_ep);

        if (data_len)
        {
            fprintf(m_file, "  %s: Length %d\n", capture_bin::get_pid_str(m_data_pid), data_len);

            fprintf(m_file, "  ");
            for (int i=0;i<data_len;i++)
            {
                fprintf(m_file, "%02x ", m_data[i]);

                if (!((i+1) & 0xF) || ((i+1) == data_len))
                    fprintf(m_file, "\n  ");
            }

            fprintf(m_file, "%s\n", capture_bin::get_pid_str(m_hshake));
        }
    }
    //-----------------------------------------------------------------
    // process_frame: Determine payload setup (SETUP, BULK, ...)
    //-----------------------------------------------------------------
    void process_frame(void)
    {
        if (m_token_pid == 0)
            return ;

        // Subtract CRC
        uint16_t data_len = (m_data_len >= 2) ? (m_data_len - 2) : 0;

        // Data that's not part of a control transfer - assume it is bulk for now...
        if (!m_ctrl_pending && (m_token_pid == PID_IN || m_token_pid == PID_OUT) && m_hshake == PID_ACK && m_data_len >= 2)
            output_bulk(data_len, true);

        // Setup packet
        if (m_token_pid == PID_SETUP && data_len == 8 && m_token_ep == 0)
        {
            // Dump partial control packet out...
            if (m_ctrl_pending)
                output_ctrl(false);

            uint8_t bmRequestType = m_data[0];

            // Device to Host
            if (bmRequestType & 0x80)
                m_ctrl_in = true;
            // Host to Device
            else
                m_ctrl_in = false;

            // Length of control transfer
            m_ctrl_pending = true;
            m_ctrl_length  = (((uint16_t)m_data[7]) << 8) | m_data[6];
            m_ctrl_offset  = 0;
            memcpy(m_ctrl_setup, m_data, data_len);
        }

        // DATA
        if (m_ctrl_pending && (m_token_pid == PID_IN || m_token_pid == PID_OUT) && data_len > 0)
        {
            // Error - too much data...
            if ((data_len + m_ctrl_offset) > sizeof(m_ctrl_payload))
                output_ctrl(false);
            else
            {
                memcpy(&m_ctrl_payload[m_ctrl_offset], m_data, data_len);
                m_ctrl_offset += data_len;
            }
        }

        // ZLP - Status phase
        if (m_ctrl_pending && (m_token_pid == PID_IN || m_token_pid == PID_OUT) && data_len == 0)
            output_ctrl(m_hshake == PID_ACK);      

        m_data_len  = 0;
        m_token_pid = 0;
        m_hshake    = 0;
    }

    bool on_sof(uint16_t frame_num)
    {
        // Buffered data - likely to be ISO
        if (m_data_len)
            process_frame();
        return true;
    }
    bool on_rst(bool in_rst)
    {
        if (in_rst)
            fprintf(m_file, "USB Device Reset\n");
        return true;
    }
    bool on_token(uint8_t pid, uint8_t dev, uint8_t ep)
    {
        // Buffered data - likely to be ISO
        if (m_data_len)
            process_frame();

        //fprintf(m_file, "%s Device %d Endpoint %d\n", capture_bin::get_pid_str(pid), dev, ep);
        m_token_pid = pid;
        m_token_dev = dev;
        m_token_ep  = ep;
        return true;
    }
    bool on_split(uint8_t hub_addr, bool complete)
    {
        return true;
    }
    bool on_handshake(uint8_t pid)
    {
        m_hshake = pid;
        if (pid == PID_ACK || pid == PID_STALL)
            process_frame();
        return true;
    }
    bool on_data(uint8_t pid, uint8_t *data, int length)
    {
        memcpy(m_data, data, length);
        m_data_len = length;
        m_data_pid = pid;
        return true;
    }

protected:
    FILE *m_file;

    uint8_t  m_token_pid;
    uint8_t  m_token_dev;
    uint8_t  m_token_ep;
    uint8_t  m_data[MAX_PACKET_SIZE];
    uint8_t  m_data_pid;
    int      m_data_len;
    uint8_t  m_hshake;

    bool     m_ctrl_pending;
    uint16_t m_ctrl_length;
    uint8_t  m_ctrl_setup[MAX_SETUP_SIZE];
    uint8_t  m_ctrl_payload[MAX_CTRL_SIZE];
    int      m_ctrl_offset;
    bool     m_ctrl_in;
};

//-----------------------------------------------------------------
// main:
//-----------------------------------------------------------------
int main(int argc, char *argv[])
{
    int            c;
    int            help      = 0;
    const char *   filename  = NULL;
    tUsbSpeed speed = USB_SPEED_HS;

    int option_index = 0;
    while ((c = getopt_long (argc, argv, GETOPTS_ARGS, long_options, &option_index)) != -1)
    {
        switch(c)
        {
            case 'f': // Filename
                filename = optarg;
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

    if (help || filename == NULL)
    {
        help_options();
        return -1;
    }

    convert_txt txt(stdout);
    return txt.open(filename, speed == USB_SPEED_HS) ? 0 : -1;
}
