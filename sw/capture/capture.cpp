#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <getopt.h>
#include <sys/select.h>

#include "ftdi_axi_driver.h"
#include "ftdi_ft60x.h"
#include "usb_capture.h"

//-----------------------------------------------------------------
// Command line options
//-----------------------------------------------------------------
#define GETOPTS_ARGS "d:e:nisf:h"

static struct option long_options[] =
{
    {"device",       required_argument, 0, 'd'},
    {"endpoint",     required_argument, 0, 'e'},
    {"inverse",      no_argument,       0, 'n'},
    {"no-sof",       no_argument,       0, 's'},
    {"no-in-nak",    no_argument,       0, 'i'},
    {"filename",     required_argument, 0, 'f'},
    {"help",         no_argument,       0, 'h'},
    {0, 0, 0, 0}
};

static void help_options(void)
{
    fprintf (stderr,"Usage:\n");
    fprintf (stderr,"  --device     | -d DEVNUM     Match only this device ID\n");
    fprintf (stderr,"  --endpoint   | -e EPNUM      Match only this endpoint\n");
    fprintf (stderr,"  --inverse    | -n            Inverse matching (exclude device / endpoint)\n");
    fprintf (stderr,"  --no-sof     | -s            Disable SOF collection (breaks timing info)\n");
    fprintf (stderr,"  --no-in-nak  | -i            Disable IN+NAK capture\n");
    fprintf (stderr,"  --filename   | -f FILENAME   File to capture to (default: capture.bin)\n");
    exit(-1);
}
//-----------------------------------------------------------------
// user_abort_check
//-----------------------------------------------------------------
static int user_abort_check(void)
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return (FD_ISSET(0, &fds));
}
//-----------------------------------------------------------------
// main:
//-----------------------------------------------------------------
int main(int argc, char *argv[])
{
    int            c;
    int            help      = 0;
    const char *   filename  = "capture.bin";
    int            dev_addr = -1;
    int            endpoint = -1;
    int            disable_sof = 0;
    int            disable_in_nak = 0;
    int            inverse_match = 0;    

    int option_index = 0;
    while ((c = getopt_long (argc, argv, GETOPTS_ARGS, long_options, &option_index)) != -1)
    {
        switch(c)
        {
            case 'd': // Device
                dev_addr = (int)strtoul(optarg, NULL, 0);
                break;
            case 'e': // Endpoint
                endpoint = (int)strtoul(optarg, NULL, 0);
                break;
            case 's': // Drop SOF
                disable_sof = 1;
                break;
            case 'i': // Drop IN + NAK
                disable_in_nak = 1;
                break;
            case 'f': // Filename
                filename = optarg;
                break;
            case 'n':
                inverse_match = 1;
                break;
            default:
                help = 1;
                break;
        }
    }

    if (help)
    {
        help_options();
        return -1;
    }

    // Open the port
    ftdi_ft60x port;
    if (!port.open(0))
        return -1;

    // Reset target state machines
    ftdi_axi_driver driver(&port);
    driver.send_drain(1000);
    port.sleep(10000);

    usb_capture capture(&driver);

    // Configure Buffer
    uint32_t buffer_size = (256 * 1024 * 1024);
    capture.configure_buffer(0, buffer_size);

    // Disable capture
    capture.stop_capture();

    // Configure device
    capture.match_device(dev_addr, inverse_match);
    capture.match_endpoint(endpoint, inverse_match);
    capture.drop_sof(disable_sof);
    capture.drop_in_nak(disable_in_nak);

    // Start device
    capture.start_capture();

    printf("USB capture started, press ENTER to stop...\n");
    while (1)
    {
        // Check for data loss - this should not happen....
        if (capture.has_lost_data())
        {
            fprintf(stderr, "\nERROR: Data lost, abort\n");
            port.close();
            exit(-1);
        }
        else if (capture.triggered())
        {
            uint32_t value = capture.get_level();
            double buffer_fill = ((double)value / buffer_size) * 100.0;
            printf("\rBuffer: %d entries %.2f%%", value / 4, buffer_fill);
            fflush(stdout);

            if (value == buffer_size)
            {
                printf("\nBuffer full");
                break;
            }
        }

        if (user_abort_check())
            break;

        port.sleep(100000);
    }
    printf("\n");

    printf("Dumping capture to file...\n");
    uint32_t size = capture.get_level();
    uint32_t *buffer = new uint32_t[size/4];
    driver.read(0, (uint8_t*)buffer, size);

    FILE *fout = fopen(filename, "wb");
    if (fout)
    {
        fwrite (buffer, 4, size/4, fout);
        fclose(fout);
    }
    else
    {
        fprintf(stderr, "ERROR: Could not create file\n");
        port.close();
        exit(-1);
    }

    port.close();
    return 0;
}
