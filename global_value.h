#ifndef __GLOBAL_VALUE_H__
#define __GLOBAL_VALUE_H__

#include <pthread.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAJOR_VER	"0"
#define MINOR_VER	"2"	
#define CAHR_VER	"p"

typedef struct CIRCULAR_BUFF
{
	unsigned char *tx;
	int WIndex;
	int RIndex;
	int GIndex;
} Buff_t;

// udp.h
#define RECV_INDEX	0 	// Audio Out
#define SEND_INDEX	1 	// Audio In

typedef struct CIRCULAR_BUFF2
{
	unsigned char *tx[10];
	int len[10];
	int index;
	int Rindex;
	int cnt;
	// unsigned char *buff;
	// int blen;
} Buff2_t;

Buff2_t VM_Frame_Buff;
Buff2_t VB_Frame_Buff;

// typedef struct VUDP_Attr
// {
// 	unsigned char *tx;
// 	unsigned char *buff;
// 	int windex;
// 	int sindex;
// 	int dcnt;
// }VUDP_Attr_t;

// VUDP_Attr_t Video_Main_Attr;
// VUDP_Attr_t Video_Box_Attr;

// Buff_t VM_Cir_Buff;
// Buff_t VB_Cir_Buff;

bool move_flag;
int move_det_xs, move_det_ys, move_det_xe, move_det_ye;

// #define V_SEND_SIZE 1004 // 1014 -> 1004
#define V_SEND_SIZE 992
#define V_SEND_RESERV 5
#define V_BUFF_SIZE 256*1024
#define MAX_REC_TIME 60000000	// 60 * 1000 * 1000 usec
#define ROAMING_PER_TIME 30000000	// 60 * 1000 * 1000 usec
#define THUMBNAIL_TIME 5000000
#define FACE_FIND_END_TIME 10000000

int data_sel;
char ip[30];

// audio.h


// typedef struct AUDP_Attr
// {
// 	unsigned char *tx;
// 	int windex;
// 	int sindex;
// 	int dcnt;
// }AUDP_Attr_t;

// AUDP_Attr_t Audio_Ao_Attr;
// AUDP_Attr_t Audio_Ai_Attr;

Buff_t AO_Cir_Buff;
Buff_t AI_Cir_Buff;

#define A_SEND_SIZE 882
#define A_BUFF_SIZE 500*1024
#define A_BUFF_MARGIN 100*1024

int main_motion_detect;
// int min_pixel_x, min_pixel_y;
// int max_pixel_x, max_pixel_y;

#define GRID_PIXEL_W		80
#define GRID_PIXEL_H		120
#define GRID_COVER_INDEX 	(1920/GRID_PIXEL_W)*(1080/GRID_PIXEL_H)
// #define GRID_COVER_INDEX 	576
#define MOSAIC_INDEX 		10
#define RECT_INDEX 			10
#define TEST_RECT_INDEX 	0
#define TEST_COVER_INDEX 	1

bool grid_cover_flag[GRID_COVER_INDEX];

typedef struct FDPD_DATA
{
	bool flag;
	int classid;
	int trackid;
	float confidence;
	int ul_x;
	int ul_y;
	int br_x;
	int br_y;
} Fdpd_Data_t;

Fdpd_Data_t fdpd_data[10];
Fdpd_Data_t facial_data;

typedef struct THUM_SNAP
{
	bool flag[10];
	int x[10];
	int y[10];
	int ex[10];
	int ey[10];
} Thum_Data_t;

// Thum_Data_t mosaic_data;
Thum_Data_t thum_face_data;

typedef struct STREAMING_REC
{
	int64_t rec_time[20];
	int rec_fcnt[20];
} Streaming_Rec_t;

Streaming_Rec_t str_rec_t;

typedef struct CLIP_CAUSE
{
	uint8_t Major;
	uint8_t Minor;
} Clip_Cause_t;

Clip_Cause_t clip_cause_t;

#define START_CHECK_TIME 5000000
#define READY_BUSY_TIME 5

int64_t start_time;
uint32_t ExpVal;

int stream_state;
int thumbnail_state;

int face_cnt, person_cnt;
int fr_state;
int polling_err_cnt;
int rec_cnt;

// int rec_state;
int clip_rec_state;
int streaming_rec_state;
int bell_rec_state;

int boot_mode;

// bool rec_stop;
// bool main_rec_end;
// bool box_rec_end;
bool main_snap;
bool box_snap;
bool thumbnail_snap;
bool face_snap;
bool roaming_person;

bool bStrem;

// pcm test
int save_pcm;
bool pcm_in, pcm_out;
bool Mosaic_En, fdpd_En, fdpd_ck;
bool dot_En;

// uart control
bool light_on;
bool rec_on;
bool rec_mem_flag;
bool rec_end;


extern int bExit;

enum FR_STATE {
  FR_WAIT = 0,  // 0
  FR_START,   	// 1
  FR_POINTSET, 	// 2
  FR_SNAPSHOT,  // 3
  FR_SUCCESS,	// 4
  FR_END,		// 5
};

enum THUMB_STATE {
  THUMB_WAIT = 0,	// 0
  THUMB_START,   	// 1
  THUMB_SNAPSHOT,  	// 2
  // THUMB_SUCCESS,	// 3
  THUMB_END,		// 3
};

enum REC_STATE {
	REC_READY = 0,	// 0
	REC_START,			// 1
	REC_ING,				// 2
	REC_STOP,				// 3
	REC_WAIT,				// 4
	REC_MP4MAKE,		// 5
};

enum CLIP_CAUSE_ENUM {
	CLIP_CAUSE_REV   = 0,	// 0
	CLIP_CAUSE_MOVE  = 1,	// 1
	CLIP_CAUSE_BOX   = 2,	// 2
	CLIP_CAUSE_BELL  = 3,	// 3
	CLIP_CAUSE_FACE  = 4,	// 4
	CLIP_CAUSE_STREM = 5,	// 5
	CLIP_CAUSE_MOUNT = 6, // 6
};

enum CLIP_MOVE {
	CLIP_MOVE_REV   = 0,	// 0
	CLIP_MOVE_MOVE  = 1,  // 1
	CLIP_MOVE_PER		= 2,  // 2
};

enum CLIP_BOX {
	CLIP_BOX_REV   = 0,	// 0
	CLIP_BOX_OCCUR = 1,  // 1
	CLIP_BOX_DISAP = 2,  // 2
};

enum CLIP_STRM {
	CLIP_STRM_REV   = 0,	// 0
	CLIP_STRM_REC   = 1,  // 1
};

enum CLIP_FACE {
	CLIP_FACE_REV   = 0,	// 0
	CLIP_FACE_DET   = 1,  // 1
};

enum CLIP_STREAM {
	CLIP_STREAM_REV   = 0,	// 0
	CLIP_STREAM_REC   = 1,  // 1
};

enum CLIP_BELL {
	CLIP_BELL_REV   = 0,	// 0
	CLIP_BELL_CALL = 1,  // 1
	CLIP_BELL_BELL = 2,  // 2
};


enum CLIP_MOUNT {
	CLIP_MOUNT_REV   = 0,	// 0
	CLIP_MOUNT_MOUNT = 1,  // 1
	CLIP_MOUNT_DISMT = 2,  // 2
};


// #define __TEST_FAKE_VEDIO__
// #define __STREAMING_CMD__

#ifdef __TEST_FAKE_VEDIO__
	int led_cnt;
#endif

#if __cplusplus
}
#endif

#endif /* __GLOBAL_VALUE_H__ */