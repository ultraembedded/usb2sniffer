#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <getopt.h>
#include "capture_bin.h"

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
        m_file      = f;
        m_token_pid = 0;
        m_token_dev = 0;
        m_token_ep  = 0;
        m_data_pid  = 0;
        m_data_len  = 0;
        m_hshake    = 0;
    }
    
    void display(void)
    {
        if (m_token_pid == 0)
            return ;

        fprintf(m_file, "%s Device %d Endpoint %d\n", capture_bin::get_pid_str(m_token_pid), m_token_dev, m_token_ep);

        if (m_data_pid && m_data_len)
        {
            fprintf(m_file, "  %s: Length %d\n", capture_bin::get_pid_str(m_data_pid), m_data_len-2);

            fprintf(m_file, "  ");
            for (int i=0;i<m_data_len-2;i++)
            {
                fprintf(m_file, "%02x ", m_data[i]);

                if (!((i+1) & 0xF) || ((i+1) == (m_data_len-2)))
                    fprintf(m_file, "\n  ");
            }

            fprintf(m_file, "%s\n", capture_bin::get_pid_str(m_hshake));
        }

        m_data_len  = 0;
        m_token_pid = 0;
    }

    bool on_sof(uint16_t frame_num)
    {
        // Buffered data - likely to be ISO
        if (m_data_len)
            display();
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
            display();

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
            display();
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

    uint8_t m_token_pid;
    uint8_t m_token_dev;
    uint8_t m_token_ep;
    uint8_t m_data[MAX_PACKET_SIZE];
    uint8_t m_data_pid;
    int     m_data_len;
    uint8_t m_hshake;
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
