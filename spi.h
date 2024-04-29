#ifndef __SPI_H__
#define __SPI_H__

#include <arpa/inet.h>

#ifdef __cplusplus
extern "C"{
#endif	// __cplusplus


typedef enum {
	DTEST          	= 0x00,
	REC 			= 0x01,
	STREAMING  		= 0x02,
	SETTING  		= 0x03,
	DTEST_BACK 		= 0x80,
	REC_BACK 		= 0x81,
	STREAMING_BACK  = 0x82,
	SETTING_BACK 	= 0x83,
} MAJOR;

typedef enum {
	STREAM_START  	= 0x01,
	STREAM_REV  	= 0x02,
	STREAM_VEDIO_M	= 0x03,
	STREAM_VEDIO_B  = 0x04,
	STREAM_FACE  	= 0x05,
	STREAM_AUDIO_F  = 0x06,
	STREAM_AUDIO_B  = 0x07,  // Backward

	USTREAM_REC_S	= 0x41,
	USTREAM_REC_E 	= 0x42,
	// USTREAM_MIC_VOL	= 0x43,
	USTREAM_LIGHT 	= 0x44,
	USTREAM_F_SEND	= 0x45,

	STREAM_ACK  	= 0x80,
	STREAM_STOP  	= 0x7F,
} MINOR_STREAMING;

typedef enum {
	REC_DEV_START  	= 0x01,
	REC_STREAM_STR  = 0x02,
	REC_CLIP_F 		= 0x03,
	REC_CLIP_B  	= 0x04,  // or Bottom Capture
	REC_FACE  		= 0x05,
	REC_BOX_ALM  	= 0x06,
	REC_SNAPSHOT    = 0x07,
	REC_FACESHOT  	= 0x08,
	REC_STREAMING_M = 0x09,
	REC_STREAMING_B = 0x0A,
	REC_BELL_SNAP_M	= 0x0B,
	REC_BELL_SNAP_B	= 0x0C,
	REC_TEMP_SNAP_M = 0x0D,
	REC_TEMP_SNAP_B = 0x0E,
	REC_DOOR_SNAP  	= 0x10,

 	REC_STREAM_END	= 0x0F,  // Not Used, Backward

	UREC_BELL 		= 0x41,
	UREC_FACE		= 0x42,	
	UREC_STREAM		= 0x43,
	UREC_TEMPER 	= 0x44,
	UREC_STREAM_END = 0x45,

	REC_ACK  		= 0x80,
	REC_DEV_STOP  	= 0x7F,
} MINOR_REC;

typedef enum {
	SET_START  		= 0x01,

	SET_LED			= 0x41,
	SET_BELL_VOL	= 0x42,
	SET_SPK_VOL		= 0x43,
	SET_BACK_LIGHT	= 0x44,
	SET_FLICKER		= 0x45,
	SET_MOVE_SENSI	= 0x46,
	SET_EX_ONOF		= 0x47,
	SET_FACE_MOSAIC = 0x48,
	SET_DOOR_ONOF	= 0x49,
	SET_USER_ONOF	= 0x4A,
	SET_EX_AREA		= 0x4B,
	SET_DOOR_GRID	= 0x4C,
	SET_USER_GRID	= 0x4D,
	SET_DOOR_CAP 	= 0x4E,

	SET_STOP  		= 0x7F,
} MINOR_SETTING;

typedef enum {
	TEST_START  	= 0x01,

	TEST_STOP  		= 0x7F,
} MINOR_TEST;






typedef enum {
	UREC_START  		= 0x01,
	

	UREC_STOP  		= 0x7F
} MINOR_UART_REC;




int spi_init(void);
void spi_deinit(void);
int spi_send_clip(int dly, int num);
int spi_send_file(uint8_t minor, char *file);
int spi_send_fake_file(uint8_t minor);
void *spi_send_stream (void *arg);
void *spi_test_send_stream (void *arg);
int spi_device_off(uint8_t minor);
void test_spi_rw(void);
void test_spi_onekbytes(int dly);
int Ready_Busy_Check(void);
int spi_send_file_dual(uint8_t minor1, uint8_t minor2, char *file1, char *file2);

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// __SPI_H__