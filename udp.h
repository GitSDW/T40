#ifndef __UDP_H_
#define __UDP_H_

#include <arpa/inet.h>

#ifdef __cplusplus
extern "C" {
#endif

int init_udp(uint8_t index, uint16_t host);
void deinit_udp(uint8_t index);
ssize_t udp_ai_send(uint8_t *udp_data, uint16_t len);
ssize_t udp_recv(uint8_t *data_buf);
void udp_ao_rolling_dcnt(void);
void *udp_recv_pthread(void *arg);
void *udp_send_pthread(void *arg);

#if __cplusplus
}
#endif

#endif /* __UDP_H_ */