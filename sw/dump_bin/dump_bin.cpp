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
#define GETOPTS_ARGS "f:o:d:e:h"

static struct option long_options[] =
{
    {"filename",     required_argument, 0, 'f'},
    {"output",       required_argument, 0, 'o'},
    {"device",       required_argument, 0, 'd'},
    {"endpoint",     required_argument, 0, 'e'},
    {"help",         no_argument,       0, 'h'},
    {0, 0, 0, 0}
};

static void help_options(void)
{
    fprintf (stderr,"Usage:\n");
    fprintf (stderr,"  --filename   | -f FILENAME   File to capture to (default: capture.bin)\n");
    fprintf (stderr,"  --device     | -d DEVNUM     Match only this device ID\n");
    fprintf (stderr,"  --endpoint   | -e EPNUM      Match only this endpoint\n");
    fprintf (stderr,"  --output     | -o FILENAME   Output file\n");
    exit(-1);
}
//-----------------------------------------------------------------
// convert_bin: Convert binary log to a binary stream
//-----------------------------------------------------------------
class convert_bin: public capture_bin
{
public:
    convert_bin(FILE *f, int match_dev, int match_ep)
    {
        m_file      = f;
        m_token_pid = 0;
        m_token_dev = 0;
        m_token_ep  = 0;
        m_data_pid  = 0;
        m_data_len  = 0;
        m_hshake    = 0;
        m_match_dev = match_dev;
        m_match_ep  = match_ep;
    }
    
    void process(void)
    {
        if (m_token_pid == 0)
            return ;

        // Filter by endpoint / address
        if (m_match_dev != -1 && m_match_dev != m_token_dev)
            return ;
        if (m_match_ep != -1 && m_match_ep != m_token_ep)
            return ;

        if (m_data_pid && m_data_len)
            fwrite(m_data, m_data_len-2, 1, m_file);

        m_data_len  = 0;
        m_token_pid = 0;
    }

    bool on_sof(uint16_t frame_num)
    {
        // Buffered data - likely to be ISO
        if (m_data_len)
            process();
        return true;
    }
    bool on_rst(bool in_rst)
    {
        return true;
    }
    bool on_token(uint8_t pid, uint8_t dev, uint8_t ep)
    {
        // Buffered data - likely to be ISO
        if (m_data_len)
            process();

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
        if (pid == PID_ACK)
            process();
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
    FILE *  m_file;

    uint8_t m_token_pid;
    uint8_t m_token_dev;
    uint8_t m_token_ep;
    uint8_t m_data[MAX_PACKET_SIZE];
    uint8_t m_data_pid;
    int     m_data_len;
    uint8_t m_hshake;

    int     m_match_dev;
    int     m_match_ep;
};

//-----------------------------------------------------------------
// main:
//-----------------------------------------------------------------
int main(int argc, char *argv[])
{
    int            c;
    int            help      = 0;
    const char *   filename  = NULL;
    const char *   dst_file  = NULL;
    int            dev_addr  = -1;
    int            endpoint  = -1;

    int option_index = 0;
    while ((c = getopt_long (argc, argv, GETOPTS_ARGS, long_options, &option_index)) != -1)
    {
        switch(c)
        {
            case 'f': // Filename
                filename = optarg;
                break;
            case 'd': // Device
                dev_addr = (int)strtoul(optarg, NULL, 0);
                break;
            case 'e': // Endpoint
                endpoint = (int)strtoul(optarg, NULL, 0);
                break;
            case 'o': // Dest
                dst_file = optarg;
                break;
            default:
                help = 1;
                break;
        }
    }

    if (help || filename == NULL || dst_file == NULL)
    {
        help_options();
        return -1;
    }

    FILE *fout = fopen(dst_file, "wb");
    if (fout)
    {
        convert_bin conv(fout, dev_addr, endpoint);
        bool ok = conv.open(filename, USB_SPEED_HS) ? 0 : -1;
        fclose(fout);
        return ok ? 0 : -1;
    }
    else
    {
        fprintf(stderr, "ERROR: Could not open file\n");
        return -1;
    }
}
