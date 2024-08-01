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

// typedef struct SETTING_ALL {
//  	uint8_t led;
//  	uint8_t bell_type;
//  	uint8_t spk_vol;
//  	uint8_t per_face;
//  	uint8_t door_g;
//  	uint8_t user_g;
//  	uint8_t move_ex;
//  	uint8_t flicker;
//  	uint8_t move_sensitivty;
//  	uint8_t backlight;

//  	uint8_t door_grid[27];
//  	uint8_t user_grid[27];
//  	uint8_t move_ex_s_x[2];
//  	uint8_t move_ex_s_y[2];
//  	uint8_t move_ex_e_x[2];
//  	uint8_t move_ex_e_y[2];
// }Setting_All;

enum Setting_All {
	SA_LED   		= 0,	// 0
	SA_BELL_TYPE 	= 1,  	// 1
	SA_SPK_VOL    	= 2,
	SA_FACE_MOSAIC	= 3,
	SA_DOOR 		= 4,
	SA_USER			= 5,
	SA_MOVE_EX		= 6,
	SA_FLICKER		= 7,
	SA_MOVE_SENSI	= 8,
	SA_BACKLIGHT	= 9,
	SA_DOOR_GRID	= 10,
	SA_USER_GRID	= 37,
	SA_MOVE_EX_S_X	= 64,
	SA_MOVE_EX_S_Y	= 66,
	SA_MOVE_EX_E_X	= 68,
	SA_MOVE_EX_E_Y	= 70,
};


void *uart_thread(void *argc);
void *device_live_thread(void * argc);
void uart_tx_test (void);
int device_end(uint8_t major);
int make_file_start(uint8_t major);
int face_end(uint8_t major);
int streaming_rec_end(uint8_t cus);
int rec_enable_ack(void);

#if __cplusplus
}
#endif

#endif /* __UART_H__ */