#ifndef __GLOBAL_VALUE_H__
#define __GLOBAL_VALUE_H__

#include <pthread.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// #define __PHILL_REQ__

#ifndef __PHILL_REQ__
	#define MAJOR_VER	"0"
	#define MINOR_VER	"d"
	#define CAHR_VER	"8"
#else
	#define MAJOR_VER	"0"
	#define MINOR_VER	"z"
	#define CAHR_VER	"e"
#endif

#define DeBug 			1

#if DeBug
	#define		dp(...)	printf(__VA_ARGS__)
#else
	#define  	dp(...)
#endif

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
	unsigned char *tx[20];
	int len[20];
	int index;
	int Rindex;
	int cnt;
	int64_t ftime[20];
	// unsigned char *buff;
	// int blen;
} Buff2_t;

Buff2_t VM_Frame_Buff;
Buff2_t VB_Frame_Buff;


typedef struct CIRCULAR_BUFF3
{
	unsigned char *tx[256];
	int DE[256];
	int LEN[256];
	int TTSEN[256];
} Buff_t3;

typedef struct FILE_TIMESTAMP
{
	char 		date[13];
	uint8_t  	type[2];
} File_TimeStamp;

File_TimeStamp TimeStamp;

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

// #define __H265__

// #define V_SEND_SIZE 1004 // 1014 -> 1004
#ifdef __H265__
	#define V_SEND_SIZE 					1004
#else
	// #define V_SEND_SIZE 					868
	// #define V_SEND_SIZE 					992
	// #define V_SEND_SIZE 					978 // Ver 0.5.l
	// #define V_SEND_SIZE 					673 // Ver 0.c.o
	#define V_SEND_SIZE 					649 // Ver 0.c.q
	// #define V_SEND_SIZE 					489 // Ver 0.c.s
	// #define V_SEND_SIZE 					974	// Test Pack Error!
#endif


#define V_SEND_RESERV 				5
#define V_BUFF_SIZE 				256*1024
#define MAX_REC_TIME 				65000000	// 60 * 1000 * 1000 usec
#define MAX_CLIP_TIME				75000000
#define ROAMING_PER_TIME 			35000000	// 60 * 1000 * 1000 usec
// #define THUMBNAIL_TIME 				5500000
#define FACE_FIND_END_TIME 			6000000
#define BELL_START_TIME				5000000
#define BELL_TIME_MIN				34000000
#define TEMP_TIME_MIN				6000000
#define CLIP_CLOSE_TIME  			1500000
#define LIVE_MESSAGE_TIME  			5000000	// 1 * 1000 * 1000 usec

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

Buff_t3 AO_Seq_Buff;

#define A_SEND_SIZE 640
#define A_BUFF_SIZE 500*1024
#define A_BUFF_MARGIN 100*1024

int main_motion_detect;
// int min_pixel_x, min_pixel_y;
// int max_pixel_x, max_pixel_y;
// bool ex_flag;
// int min_pixel_x2, min_pixel_y2;
// int max_pixel_x2, max_pixel_y2;
// bool ex_on;

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
	int cnt;
} Fdpd_Data_t;

int face_cnt_mosaic;
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



typedef struct SIMIL_T
{
	double correl;
	double chisqr;
	double intersect;
	double bhattacharyya;
	double kl_div;
} Simil_t;

Simil_t osim_t;
Simil_t bsim_t;

// Thum_Data_t mosaic_data;
Thum_Data_t thum_face_data;

// typedef struct STREAMING_REC
// {
// 	int64_t rec_time[20];
// 	int rec_fcnt[20];
// } Streaming_Rec_t;

// Streaming_Rec_t str_rec_t;

typedef struct CLIP_CAUSE
{
	uint8_t Major;
	uint8_t Minor;
} Clip_Cause_t;

typedef struct  {
    uint8_t version_padding_extension_cc;
    uint8_t marker_payload_type;
    uint8_t sequence_number[2];
    uint8_t timestamp[4];
    uint8_t ssrc[4];
}RTPHeader;

Clip_Cause_t clip_cause_t;

#define START_CHECK_TIME 4000000
#define READY_BUSY_TIME 5

typedef struct  {
    char name[15][128];
    uint8_t cnt;
}SaveFile;

SaveFile Save_movie1;
SaveFile Save_movie2;

typedef struct  {
    double focus;
    double sharpness;
}Focus_Sharpness;

typedef struct  {
    double type;
    double cnt;
    int index;
}Make_File;

uint8_t *tx_buff;
uint8_t *tx_tbuff;
uint8_t *rx_buff;
uint8_t *read_buff;
uint8_t ota_type_u;
uint8_t stream_tag[10];

int64_t start_time;
uint32_t ExpVal;
int64_t rec_total;
int64_t rec_time_s;
int64_t rec_each_time[10];
int64_t audio_timeout;
int64_t mosaic_time[10];
int64_t frame_ck, frame_ck2;
int64_t cap_test_time;
int64_t setting_end_time, setting_end_delay;

int stream_state;
int thumbnail_state;

int face_cnt, person_cnt;
int fr_state;
int polling_err_cnt;
int rec_cnt;

int clip_rec_state;
int streaming_rec_state;
int rec_streaming_state;
int bell_rec_state;

int boot_mode;
int face_crop_cnt;
int Rec_type;
int bl_state;

int spk_vol_buf;
int spk_gain_buf;
int bell_vol_buf;
int bell_gain_buf;
int send_retry;


int brt_st_stat;
int dim_st_stat;
int bled_st_stat;
int video_st_stat;
int mic_st_stat;
int shfo_st_stat;
int bellend_sound;

// int net_wait_st;

bool main_snap;
bool box_snap;
bool thumbnail_snap;
bool face_snap;
bool roaming_person;

bool bStrem;

bool bUart;
bool bLive;
bool bLiveFile;

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
bool bell_flag;
bool bell_snap_m;
bool bell_snap_b;
bool door_cap_flag;
bool temp_flag;
bool bell_call_flag;
bool temp_unmount_flag;
bool bell_stream_flag;
bool netwrok_busy;
bool save_send_flag;
// bool move_end;
bool cmd_end_flag;
bool cfile_flag, cfile_flag1, cfile_flag2;
bool bfile_flag, bfile_flag1, bfile_flag2;
bool dimming;
bool bOTA, ota_flag;
bool audio_start_flag;
bool file_21_flag;
bool rfile_flag[10];
bool mem_full_flag;
bool av_off_flag;
bool send_retry_flag;
bool bell_rerecode_flag;
bool audio_spi_flag;
bool dn_g726_falg;
bool get_audio;
bool set_audio;
bool bDimming;
bool Dimming_end;
bool bBLed;
bool ao_clear_flag;
bool rebell;
bool bMove, move_start_flag;
bool face_end_f[5];
bool DLedT, BLedT, BMicT;
bool TestReset;
bool netwaitstar, netwaitrecover;
bool bpflag;
bool bell_play_flag;
bool bell_play_wait_flag;

///////// tts ////////////////
int last_recv_seq;
int64_t last_recv_time;
bool last_recv_flag;
bool tts_start_falg;
/////////////////////////////

extern int bExit;

enum FR_STATE {
  FR_WAIT = 0,  // 0
  FR_START,   	// 1
  FR_POINTSET, 	// 2
  FR_SNAPSHOT,  // 3
  FR_SUCCESS,	// 4
  FR_END,		// 5
  FR_SEND,		// 6
};

enum THUMB_STATE {
  THUMB_WAIT = 0,	// 0
  THUMB_START,   	// 1
  THUMB_SNAPSHOT,  	// 2
  // THUMB_SUCCESS,	// 3
  THUMB_END,		// 3
};

enum BELL_SNAP_STATE {
  BSS_WAIT 			= 0,	// 0
  BSS_START,   				// 1
  BSS_SEND,  					// 2
  // THUMB_SUCCESS,		// 3
  BSS_END,					// 3
  BSS_MAKE,
};

enum TEMP_SNAP_STATE {
  TSS_WAIT 			= 0,	// 0
  TSS_START,   				// 1
  TSS_SEND,  					// 2
  // THUMB_SUCCESS,		// 3
  TSS_END,					// 3
};

enum REC_STATE {
	REC_READY = 0,	// 0
	REC_START,			// 1
	REC_ING,				// 2
	REC_STOP,				// 3
	REC_WAIT,				// 4
	REC_MP4MAKE,		// 5
	REC_SPISEND,
	REC_SENDEND,
	REC_RECONNECT,
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

enum REC_TYPE {
	CLIP_REC   		= 0,	// 0
	BELL_REC 		= 1,  	// 1
	MAKE_FILE		= 2,
	SEND_FILE 		= 3,
	STRM_REC 		= 4,
	BELL_REREC 		= 5,
	// NET_WAIT		= 6,
};

// enum NET_WAIT_STATE {
// 	NET_WAIT_START	 = 0,	// 0
// 	NET_WAIT_WAIT	 = 1,  	// 1
// 	NET_WAIT_RECOVER = 2,
// 	NET_WAIT_TIMTOUT = 3,
// };

// #define __TEST_FAKE_VEDIO__
// #define __STREAMING_CMD__
// #define __TEST_CMD__
// #define __BOX_ALGORITH__
#define __G726__
#define __FILE_SEND_CHANGE__
#define __PRIVERCE_SIZE_UP__
#define __FRAME_SYNC__
#define __IOT_CORE__
#define __AUDIOE_SEQ__

#ifdef __TEST_FAKE_VEDIO__
	int led_cnt;
#endif



#if __cplusplus
}
#endif

#endif /* __GLOBAL_VALUE_H__ */