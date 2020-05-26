#ifndef __LOG_FILE_PCAP_H__
#define __LOG_FILE_PCAP_H__

//--------------------------------------------------------------------
// Prototypes
//--------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

int pcap_file_create(const char *filename);
int pcap_file_close(void);
int pcap_file_add_sof(uint32_t value, int is_hs);
int pcap_file_add_rst(uint32_t value, int is_hs);
int pcap_file_add_token(uint32_t value);
int pcap_file_add_handshake(uint32_t value);
int pcap_file_add_data(uint32_t value, uint8_t *data, int length);

#ifdef __cplusplus
}
#endif

#endif
