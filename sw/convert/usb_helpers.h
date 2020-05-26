#ifndef __USB_HELPERS_H__
#define __USB_HELPERS_H__

//-----------------------------------------------------------------
// Defines:
//-----------------------------------------------------------------
// Tokens
#define PID_OUT                    0xE1
#define PID_IN                     0x69
#define PID_SOF                    0xA5
#define PID_SETUP                  0x2D
#define PID_PING                   0xB4

// Data
#define PID_DATA0                  0xC3
#define PID_DATA1                  0x4B
#define PID_DATA2                  0x87
#define PID_MDATA                  0x0F

// Handshake
#define PID_ACK                    0xD2
#define PID_NAK                    0x5A
#define PID_STALL                  0x1E
#define PID_NYET                   0x96

// Special
#define PID_PRE                    0x3C
#define PID_ERR                    0x3C
#define PID_SPLIT                  0x78

// Rounded up...
#define MAX_PACKET_SIZE            2048

//--------------------------------------------------------------------
// Enums
//--------------------------------------------------------------------
typedef enum
{
    USB_SPEED_HS,
    USB_SPEED_FS,
    USB_SPEED_LS,
    USB_SPEED_MANUAL,
} tUsbSpeed;

//--------------------------------------------------------------------
// Prototypes
//--------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

uint8_t  usb_get_pid(uint32_t value);
uint16_t usb_get_crc16(uint8_t *buffer, int len);
uint16_t usb_get_cycle_delta(uint32_t value);
int      usb_get_rst_state(uint32_t value);
uint8_t  usb_get_token_device(uint32_t value);
uint8_t  usb_get_token_endpoint(uint32_t value);
uint8_t  usb_get_token_crc5(uint32_t value);
uint16_t usb_get_data_length(uint32_t value);
uint16_t usb_get_sof_frame(uint32_t value);
uint8_t  usb_get_sof_crc5(uint32_t value);
uint8_t  usb_get_split_hub_addr(uint32_t value);
uint8_t  usb_get_split_complete(uint32_t value);

char*    usb_get_pid_str(uint8_t pid);

#ifdef __cplusplus
}
#endif

#endif
