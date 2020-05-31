#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <getopt.h>
#include "capture_bin.h"

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


typedef struct usbmon_packet {
    uint64_t id;        /*  0: URB ID - from submission to callback */
    unsigned char type; /*  8: 'S' or 'C' */
    unsigned char xfer_type; /*    ISO (0), Intr, Control, Bulk (3) */
#define USBPCAP_TRANSFER_ISOCHRONOUS    0
#define USBPCAP_TRANSFER_INTERRUPT  1
#define USBPCAP_TRANSFER_CONTROL    2
#define USBPCAP_TRANSFER_BULK       3

    unsigned char epnum;    /*     Endpoint number and transfer direction */
    unsigned char devnum;   /*     Device address */
    uint16_t busnum;    /* 12: Bus number */
    char flag_setup;    /* 14: Same as text */
    char flag_data;     /* 15: 0 or '=' */
    int64_t ts_sec;     /* 16: gettimeofday */
    int32_t ts_usec;    /* 24: gettimeofday */
    int status;     /* 28: */
    unsigned int length;    /* 32: Length of data (submitted or actual) */
    unsigned int len_cap;   /* 36: Delivered length */
    union {         /* 40: */
        unsigned char setup[8]; /* Only for Control S-type */
        struct iso_rec {        /* Only for ISO */
            int error_count;
            int numdesc;
        } iso;
    } s;
} __attribute__((packed)) t_usb_packet;

#define MAX_SETUP_SIZE 8
#define MAX_CTRL_SIZE  65536

//-----------------------------------------------------------------
// Command line options
//-----------------------------------------------------------------
#define GETOPTS_ARGS "f:o:u:h"

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
    fprintf (stderr,"  --filename   | -f FILENAME   File to capture to (default: capture.bin)\n");
    fprintf (stderr,"  --output     | -o FILENAME   PCAP output file\n");
    fprintf (stderr,"  --usb-speed  | -u hs|fs|ls   USB speed for timing info (default: hs)\n");
    exit(-1);
}
//-----------------------------------------------------------------
// convert_usb_linux: Convert binary log to USB Linux PCAP
//-----------------------------------------------------------------
class convert_usb_linux: public capture_bin
{
public:
    convert_usb_linux()
    {
        m_file         = NULL;
        m_token_pid    = 0;
        m_token_dev    = 0;
        m_token_ep     = 0;
        m_data_pid     = 0;
        m_data_len     = 0;
        m_hshake       = 0;
        m_id           = 0;
        m_ctrl_pending = false;
    }
    //-----------------------------------------------------------------
    // create: Create a PCAP file
    //-----------------------------------------------------------------
    bool create(const char *filename)
    {
        m_file = fopen(filename, "wb");
        if (m_file)
        {
            pcap_hdr_t header;
            header.magic_number  = 0xA1B23C4D;
            header.version_major = 2;
            header.version_minor = 4;
            header.thiszone      = 0;
            header.sigfigs       = 0;
            header.snaplen       = 2048;
            header.network       = 189; // DLT_USB_LINUX
            fwrite(&header, sizeof(pcap_hdr_t), 1, m_file);
        }

        return m_file == NULL ? false : true;
    }
    //-----------------------------------------------------------------
    // close: Close file handle
    //-----------------------------------------------------------------
    void close(void)
    {
        if (m_file)
            fclose(m_file);

        m_file = NULL;
    }
    //-----------------------------------------------------------------
    // write: Write packet to PCAP file
    //-----------------------------------------------------------------
    bool write(uint8_t *data, int len, uint64_t ts_ns)
    {
        pcaprec_hdr_t header;

        header.ts_sec = (ts_ns / 1000) / 1000000000;
        header.ts_usec = (ts_ns / 1000) % 1000000000;
        header.incl_len = len;
        header.orig_len = len;

        fwrite(&header, sizeof(pcaprec_hdr_t), 1, m_file);
        fwrite(data, len, 1, m_file);
        return true;
    }
    //-----------------------------------------------------------------
    // output_ctrl: Output control frame
    //-----------------------------------------------------------------
    void output_ctrl(bool ok)
    {
        m_ctrl_pending = false;

        uint8_t frame[MAX_CTRL_SIZE+sizeof(t_usb_packet)];
        t_usb_packet *packet = (t_usb_packet *)frame;
        uint8_t *payload = &frame[sizeof(t_usb_packet)];

        // Request (SETUP)
        packet->id         = m_id++;
        packet->type       = 'S';
        packet->xfer_type  = USBPCAP_TRANSFER_CONTROL;
        packet->epnum      = m_token_ep + (m_ctrl_in ? 0x80 : 0x00);
        packet->devnum     = m_token_dev;
        packet->busnum     = 0;
        packet->flag_setup = 0;
        packet->flag_data  = '<';
        packet->ts_sec     = 0;
        packet->ts_usec    = 0;
        packet->status     = 0;
        packet->length     = m_ctrl_length;
        packet->len_cap    = 0;

        memcpy(packet->s.setup, m_ctrl_setup, MAX_SETUP_SIZE);
        write(frame, sizeof(t_usb_packet), 0);

        // Payload / response / status
        if (ok)
        {
            packet->type       = 'C';
            packet->xfer_type  = USBPCAP_TRANSFER_CONTROL;
            packet->epnum      = m_token_ep + (m_ctrl_in ? 0x80 : 0x00);
            packet->devnum     = m_token_dev;
            packet->busnum     = 0;
            packet->flag_setup = 0;
            packet->flag_data  = 0;
            packet->ts_sec     = 0;
            packet->ts_usec    = 0;
            packet->status     = 0;
            packet->length     = m_ctrl_offset;
            packet->len_cap    = m_ctrl_offset;

            memset(packet->s.setup, 0, MAX_SETUP_SIZE);
            if (m_ctrl_offset)
                memcpy(payload, m_ctrl_payload, m_ctrl_offset);

            write(frame, sizeof(t_usb_packet) + m_ctrl_offset, 0);   
        }
    }
    //-----------------------------------------------------------------
    // output_ctrl: Output bulk frame
    //-----------------------------------------------------------------
    void output_bulk(uint16_t data_len, bool ok)
    {
        uint8_t frame[MAX_CTRL_SIZE+sizeof(t_usb_packet)];
        t_usb_packet *packet = (t_usb_packet *)frame;
        uint8_t *payload = &frame[sizeof(t_usb_packet)];

        packet->id         = m_id++;
        packet->type       = 'C';
        packet->xfer_type  = USBPCAP_TRANSFER_BULK;
        packet->epnum      = m_token_ep | ((m_token_pid == PID_IN) ? 0x80 : 0);
        packet->devnum     = m_token_dev;
        packet->busnum     = 0;
        packet->flag_setup = '-';
        packet->flag_data  = '>';
        packet->ts_sec     = 0;
        packet->ts_usec    = 0;
        packet->status     = ok ? 0 : -1;
        packet->length     = 0;
        packet->len_cap    = data_len;

        memset(packet->s.setup, 0, MAX_SETUP_SIZE);
        if (data_len)
            memcpy(payload, m_data, data_len);
        write(frame, sizeof(t_usb_packet) + data_len, 0);   
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
        return true;
    }
    bool on_token(uint8_t pid, uint8_t dev, uint8_t ep)
    {
        // Buffered data - likely to be ISO
        if (m_data_len)
            process_frame();

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
    FILE *   m_file;

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

    uint64_t m_id;
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
    tUsbSpeed speed = USB_SPEED_HS;

    int option_index = 0;
    while ((c = getopt_long (argc, argv, GETOPTS_ARGS, long_options, &option_index)) != -1)
    {
        switch(c)
        {
            case 'f': // Filename
                filename = optarg;
                break;
            case 'o': // Filename
                dst_file = optarg;
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

    if (help || filename == NULL || dst_file == NULL)
    {
        help_options();
        return -1;
    }

    convert_usb_linux pcap;
    if (!pcap.create(dst_file))
    {
        fprintf(stderr, "ERROR: Could not create file\n");
        return -1;
    }

    bool ok = pcap.open(filename, speed == USB_SPEED_HS);
    pcap.close();

    return ok ? 0: -1;
}
