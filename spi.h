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
	TEST_START  	= 0x01,

	TEST_STOP  		= 0x7F
} MINOR_TEST;

typedef enum {
	REC_START  		= 0x01,
	REC_STREAM_STR  = 0x02,
	REC_CLIP_F 		= 0x03,
	REC_CLIP_B  	= 0x04,  // or Bottom Capture
	REC_FACE  		= 0x05,
	REC_BOX_ALM  	= 0x06,
	REC_SNAPSHOT    = 0x07,
	REC_FACESHOT  	= 0x08,
	REC_STREAM_END	= 0x0F,  // Not Used, Backward
	REC_ACK  		= 0x80,

	REC_STOP  		= 0x7F
} MINOR_REC;

typedef enum {
	STREAM_START  	= 0x01,
	STREAM_REV  	= 0x02,
	STREAM_VEDIO_M	= 0x03,
	STREAM_VEDIO_B  = 0x04,
	STREAM_FACE  	= 0x05,
	STREAM_AUDIO_F  = 0x06,
	STREAM_AUDIO_B  = 0x07,  // Backward
	// STREAM_AUDIO_R  = 8,  // Backward

	STREAM_STOP  	= 0x7F
} MINOR_STREAMING;

typedef enum {
	SET_START  		= 0x00,

	SET_STOP  		= 0x7F
} MINOR_SETTING;

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

#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// __SPI_H__