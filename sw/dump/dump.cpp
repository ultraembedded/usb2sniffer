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
        m_file = f;
    }
    bool on_sof(uint16_t frame_num)
    {
        fprintf(m_file, "SOF - Frame %d\n", frame_num);
        return true;
    }
    bool on_rst(bool in_rst)
    {
        fprintf(m_file, "USB RST = %d\n", in_rst);
        return true;
    }
    bool on_token(uint8_t pid, uint8_t dev, uint8_t ep)
    {
        fprintf(m_file, "%s Device %d Endpoint %d\n", capture_bin::get_pid_str(pid), dev, ep);
        return true;
    }
    bool on_split(uint8_t hub_addr, bool complete)
    {
        fprintf(m_file, "%s Hub Addr %d\n", complete ? "CSPLIT" : "SSPLIT", hub_addr);
        return true;
    }
    bool on_handshake(uint8_t pid)
    {
        fprintf(m_file, "  %s\n", capture_bin::get_pid_str(pid));
        return true;
    }
    bool on_data(uint8_t pid, uint8_t *data, int length)
    {
        fprintf(m_file, "  %s: Length %d\n", capture_bin::get_pid_str(pid), length-2);

        fprintf(m_file, "  ");
        for (int i=0;i<length-2;i++)
        {
            fprintf(m_file, "%02x ", data[i]);

            if (!((i+1) & 0xF) || ((i+1) == (length-2)))
                fprintf(m_file, "\n  ");
        }

        uint16_t exp_crc = capture_bin::calc_crc16(data, length-2);
        exp_crc = (exp_crc >> 8) | (exp_crc << 8);

        fprintf(m_file, "CRC = %02x%02x (Expected = %04x)\n", data[length-2], data[length-1], exp_crc);
        return true;
    }

protected:
    FILE *m_file;
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
