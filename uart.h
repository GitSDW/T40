#ifndef __UART_H__
#define __UART_H__


#ifdef __cplusplus
extern "C" {
#endif

// int baud_rate[] = {B115200,B57600,B38400,B19200,B9600,B4800,B2400,B1200,B300};
// int baud_rate_num[] = {115200,57600,38400,19200,9600,4800,2400,1200,300};

// char buf[1024] = "Ingenic tty send test.\n";
char *path;

#define CSTOPB 0000100
/* c_cflag bit meaning */
#define PARENB 0000400
#define PARODD 0001000
/* c_iflag bits */
#define INPCK 0000020

#define CSIZE 0000060
#define CS5 0000000
#define CS6 0000020
#define CS7 0000040
#define CS8 0000060

void *uart_thread(void *argc);
void *device_live_thread(void * argc);
void uart_tx_test (void);
int device_end(uint8_t major);


#if __cplusplus
}
#endif

#endif /* __UART_H__ */