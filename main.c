// C Basic Library
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

// T40 Library
#include <imp/imp_audio.h>
#include <imp/imp_log.h>

// C++ Library
#include "cxx_util.h"
#include "image_comparison.h"

// C library
#include "global_value.h"
#include "udp.h"
#include "audio.h"
#include "c_util.h"
#include "video.h"
#include "spi.h"
#include "fdpd.h"
#include "gpio.h"
#include "adc.h"
#include "uart.h"
#include "setting.h"

int bExit = 0;
int64_t dimming_s = 0;

int memory_init(void) {
	int i=0;
	AO_Cir_Buff.tx = (unsigned char *)malloc((size_t)512*1024);
	AO_Cir_Buff.WIndex = 0;
	AO_Cir_Buff.RIndex = 0;

	AI_Cir_Buff.tx = (unsigned char *)malloc((size_t)10*1024);
	AI_Cir_Buff.WIndex = 0;
	AI_Cir_Buff.RIndex = 0;

	// VM_Frame_Buff.tx[0] = (unsigned char *)malloc(V_BUFF_SIZE);
	// VM_Frame_Buff.tx[1] = (unsigned char *)malloc(V_BUFF_SIZE);
	// VM_Frame_Buff.tx[2] = (unsigned char *)malloc(V_BUFF_SIZE);
	// VM_Frame_Buff.tx[3] = (unsigned char *)malloc(V_BUFF_SIZE);
	// VM_Frame_Buff.tx[4] = (unsigned char *)malloc(V_BUFF_SIZE);
	// VM_Frame_Buff.len[0] = 0;
	// VM_Frame_Buff.len[1] = 0;
	// VM_Frame_Buff.len[2] = 0;
	// VM_Frame_Buff.len[3] = 0;
	// VM_Frame_Buff.len[4] = 0;
	for (i=0; i<20; i++) {
		VM_Frame_Buff.tx[i] = (unsigned char *)malloc(128*1024);
		VM_Frame_Buff.len[i] = 0;
	}
	VM_Frame_Buff.index = 0;
	VM_Frame_Buff.Rindex = 0;
	VM_Frame_Buff.cnt = 0;

	// VB_Frame_Buff.tx[0] = (unsigned char *)malloc(V_BUFF_SIZE);
	// VB_Frame_Buff.tx[1] = (unsigned char *)malloc(V_BUFF_SIZE);
	// VB_Frame_Buff.tx[2] = (unsigned char *)malloc(V_BUFF_SIZE);
	// VB_Frame_Buff.tx[3] = (unsigned char *)malloc(V_BUFF_SIZE);
	// VB_Frame_Buff.tx[4] = (unsigned char *)malloc(V_BUFF_SIZE);
	// VB_Frame_Buff.len[0] = 0;
	// VB_Frame_Buff.len[1] = 0;
	// VB_Frame_Buff.len[2] = 0;
	// VB_Frame_Buff.len[3] = 0;
	// VB_Frame_Buff.len[4] = 0;
	for (i=0; i<20; i++) {
		VB_Frame_Buff.tx[i] = (unsigned char *)malloc(128*1024);
		VB_Frame_Buff.len[i] = 0;
	}
	VB_Frame_Buff.index = 0;
	VB_Frame_Buff.Rindex = 0;
	VB_Frame_Buff.cnt = 0;

	// VB_Frame_Buff.buff = (unsigned char *)malloc(V_BUFF_SIZE);
	// VB_Frame_Buff.blen = 0;

	tx_buff = (unsigned char *)malloc(1040);;
	tx_tbuff = (unsigned char *)malloc(1040);;
	rx_buff = (unsigned char *)malloc(1040);;
	read_buff = (unsigned char *)malloc(1040);;


	return 0;
}

int memory_deinit(void) {
	free(AO_Cir_Buff.tx);
	free(AI_Cir_Buff.tx);
	// free(VM_Cir_Buff.tx);
	// free(VB_Cir_Buff.tx);

	return 0;
}

int global_value_init(void) {
	int i;
	char date_buf[12] = "240430110415";
	// buffMutex_vm = PTHREAD_MUTEX_INITIALIZER;
	// buffMutex_vb = PTHREAD_MUTEX_INITIALIZER;
	start_time=0;

	move_flag = false;
	move_det_xs = 0;
	move_det_ys = 0;
	move_det_xe = 0;
	move_det_ye = 0;
	main_motion_detect = 0;
	for (i=0; i<GRID_COVER_INDEX; i++) {
		grid_cover_flag[i] = false;
	}

	stream_state = 0;
	thumbnail_state = 0;
	face_cnt = 0;
	person_cnt = 0;
	fr_state = 0;
	polling_err_cnt = 0;
	rec_cnt = 0;
	save_pcm = 0;
	face_crop_cnt = 0;
	rec_total = 0;
	audio_timeout = 0;

	// rec_state = REC_READY;
	clip_rec_state = REC_READY;
	streaming_rec_state = REC_READY;
	rec_streaming_state = REC_READY;
	bell_rec_state = REC_READY;
	bl_state = BSS_WAIT;

	main_snap = false;
	box_snap = false;
	thumbnail_snap = false;
	face_snap = false;
	// main_rec_end = false;
	// box_rec_end = false;
	bStrem = false;
	pcm_in = false;
	pcm_out = false;

	Mosaic_En = false;
	fdpd_En = false;
	fdpd_ck = false;
	dot_En = false;
	rec_on = false;
	rec_mem_flag = false;
	rec_end = false;
	bell_flag = false;
	bell_snap_m = false;
	bell_snap_b = false;
	door_cap_flag = false;
	temp_flag = false;
	bell_call_flag = false;
	temp_unmount_flag = false;
	bell_stream_flag = false;
	netwrok_busy = false;
	// move_end = false;
	save_send_flag = false;
	cmd_end_flag = false;
	cfile_flag = false;
	bfile_flag = false;
	dimming = false;
	ota_flag = false;
	audio_start_flag = false;

	for(i=0;i<10;i++){
		fdpd_data[i].flag = false;
        fdpd_data[i].classid = 0;
        fdpd_data[i].ul_x = 0;
        fdpd_data[i].ul_y = 0;
        fdpd_data[i].br_x = 0;
        fdpd_data[i].br_y = 0;
	}

	snprintf(TimeStamp.date, 12, "%s", date_buf);
	TimeStamp.type[0] = 1;
	TimeStamp.type[1] = 1;

	memset(ip, 0, 30);
	return 0;
}

int gpio_init(void) {
	int ret = 0;

	ret = gpio_export(PORTB+17);	// Clip Gpio
	if(ret < 0){
		printf("Fail Export GPIO : %d\n", PORTB+17);
		return -1;
	}

	ret = gpio_set_dir(PORTB+17, GPIO_INPUT, GPIO_LOW);
	if(ret < 0){
		printf("Fail get dir GPIO : %d\n", PORTB+17);
		return -1;
	}



	ret = gpio_export(PORTB+18);	// Ready Busy Gpio
	if(ret < 0){
		printf("Fail Export GPIO : %d\n", PORTB+17);
		return -1;
	}

	ret = gpio_set_dir(PORTB+18, GPIO_INPUT, GPIO_LOW);
	if(ret < 0){
		printf("Fail get dir GPIO : %d\n", PORTB+17);
		return -1;
	}



	ret = gpio_export(PORTB+19);	// Stream Gpio
	if(ret < 0){
		printf("Fail Export GPIO : %d\n", PORTB+19);
		return -1;
	}

	ret = gpio_set_dir(PORTB+19, GPIO_INPUT, GPIO_LOW);
	if(ret < 0){
		printf("Fail get dir GPIO : %d\n", PORTB+19);
		return -1;
	}



	ret = gpio_export(PORTD+6);	// Box LED Gpio
	if(ret < 0){
		printf("Fail Export GPIO : %d\n", PORTD+6);
		return -1;
	}

	ret = gpio_set_dir(PORTD+6, GPIO_OUTPUT, GPIO_HIGH);
	// ret = gpio_set_dir(PORTD+6, GPIO_OUTPUT, GPIO_LOW);
	if(ret < 0){
		printf("Fail get dir GPIO : %d\n", PORTD+6);
		return -1;
	}

	ret = gpio_set_val(PORTD+6, 1);
	if(ret < 0){
		printf("Fail set Value GPIO : %d\n", PORTD+6);
		return -1;
	}


	// 
	ret = gpio_export(PORTD+21);	// Box LED Gpio
	if(ret < 0){
		printf("Fail Export GPIO : %d\n", PORTD+21);
		return -1;
	}

	ret = gpio_set_dir(PORTD+21, GPIO_OUTPUT, GPIO_LOW);
	// ret = gpio_set_dir(PORTD+6, GPIO_OUTPUT, GPIO_LOW);
	if(ret < 0){
		printf("Fail get dir GPIO : %d\n", PORTD+21);
		return -1;
	}

	// ret = gpio_export(PORTB+31);	// Box LED Gpio
	// if(ret < 0){
	// 	printf("Fail Export GPIO : %d\n", PORTB+19);
	// 	return -1;
	// }

	// ret = gpio_set_dir(PORTB+31, GPIO_OUTPUT, GPIO_HIGH);
	// // ret = gpio_set_dir(PORTB+31, GPIO_OUTPUT, GPIO_LOW);
	// if(ret < 0){
	// 	printf("Fail get dir GPIO : %d\n", PORTB+17);
	// 	return -1;
	// }

	// ret = gpio_set_val(PORTB+31, 1);
	// if(ret < 0){
	// 	printf("Fail set Value GPIO : %d\n", PORTB+31);
	// 	return -1;
	// }

	return 0;
}

void amp_on(void) {
	int ret = -1;

	ret = gpio_set_val(PORTD+21, 1);
	if(ret < 0){
		printf("Fail set Value GPIO : %d\n", PORTD+21);
	}
}

int gpio_deinit(void) {
	int ret = 0;

	ret = gpio_unexport(PORTB+17);
	if(ret < 0){
		printf("Fail Unexport GPIO : %d\n", PORTB+17);
		return -1;
	}

	ret = gpio_unexport(PORTB+18);
	if(ret < 0){
		printf("Fail Unexport GPIO : %d\n", PORTB+18);
		return -1;
	}

	ret = gpio_unexport(PORTB+19);
	if(ret < 0){
		printf("Fail Unexport GPIO : %d\n", PORTB+19);
		return -1;
	}

	ret = gpio_unexport(PORTD+6);
	if(ret < 0){
		printf("Fail Unexport GPIO : %d\n", PORTD+6);
		return -1;
	}

	return 0;
}

int start_up_mode(void){
	int gpio_917_0 = 0, gpio_917_1 = 1;

	gpio_917_0 = gpio_get_val(PORTB+17);
	if(gpio_917_0 < 0){
		printf("Fail get val GPIO : %d\n", PORTB+17);
		return -1;
	}
	gpio_917_1 = gpio_get_val(PORTB+19);
	if(gpio_917_1 < 0){
		printf("Fail get val GPIO : %d\n", PORTB+19);
		return -1;
	}

	if (gpio_917_0 == 1 && gpio_917_1 == 0)
		return 1;
	else if (gpio_917_0 == 0 && gpio_917_1 == 1)
		return 2;
	else if (gpio_917_0 == 1 && gpio_917_1 == 1)
		return 3;

	return 0;
}

int gpio_LED_dimming (int onoff) {
	static bool led_flag=false;
	int led_duty = 0;
	char file_sep[100] = {0};

	if (!led_flag) {
		system("echo 6 > /sys/class/pwm/pwmchip0/export");
		led_flag = true;
	}

	if (onoff == 0) {
		led_duty = 100;
		system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
		printf(file_sep);
		printf("\n");
		system(file_sep);
		system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
	}
	else if (onoff == 1) {
		led_duty = 0;
		system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
		printf(file_sep);
		printf("\n");
		system(file_sep);
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
		// system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
	}
	else {
		led_duty = 90;
		system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
		// printf(file_sep);
		// printf("\n");
		system(file_sep);
		system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
		
		dimming = true;
	}

	return 0;
}

int LED_dimming (int duty) {
	int led_duty = 0;
	char file_sep[100] = {0};

	led_duty = duty;
	system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
	memset(file_sep, 0, 100);
	sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
	// printf(file_sep);
	// printf("\n");
	system(file_sep);
	// system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
	// system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");

	return 0;
}

int gpio_LED_Set (int onoff) {
	static bool led_flag=false;
	int led_duty = 0, ret = 0;;
	char file_sep[100] = {0};

	if (!led_flag) {
		system("echo 6 > /sys/class/pwm/pwmchip0/export");
		led_flag = true;
	}

	if (onoff == 1) {
		led_duty = 0;
		system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
		printf(file_sep);
		printf("\n");
		system(file_sep);
		system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");

		ret = gpio_set_val(PORTD+6, 0);
		if(ret < 0){
			printf("Fail set Value GPIO : %d\n", PORTD+6);
			return -1;
		}

		light_on = true;
	}
	else if (onoff == 2) {
		led_duty = 90;
		system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
		printf(file_sep);
		printf("\n");
		system(file_sep);
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
		// system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		ret = gpio_set_val(PORTD+6, 1);
		if(ret < 0){
			printf("Fail set Value GPIO : %d\n", PORTD+6);
			return -1;
		}

		light_on = false;
	}
	else {
		led_duty = 100;
		system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
		printf(file_sep);
		printf("\n");
		system(file_sep);
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
		// system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		ret = gpio_set_val(PORTD+6, 1);
		if(ret < 0){
			printf("Fail set Value GPIO : %d\n", PORTD+6);
			return -1;
		}

		light_on = false;
	}

	return 0;
}



void func_reboot(void) {
	printf("Reboot Test\n");
	system("reboot");
}


void handler(int sig){
	if(sig == SIGINT) {
		printf("Exit Programe!!\n");
	}
}

void mv_cap(int mb, int cnt) {
	char sep[128] = {0};
	if (mb == 0)
		sprintf(sep, "cp /dev/shm/main%d.jpg /tmp/mnt/sdcard/main_cap%d.jpg", cnt, cnt);
	else
		sprintf(sep, "cp /dev/shm/box%d.jpg /tmp/mnt/sdcard/box_cap%d.jpg", cnt, cnt);
	system(sep);
	system("sync");
}



void *make_mp4_clip(void *argc) {
	int type, file_cnt;
	char file_sep[100] = {0};

	Make_File *mfd =(Make_File*)argc;
	type = mfd->type;
	file_cnt = mfd-> cnt;

	printf("[%s] Type : %d Cnt : %d\n", __func__, type, file_cnt);

	if (file_cnt > 0 && type == 0) {
		#ifdef __H265__			
			system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-0.h265 -c copy /dev/shm/main.mp4");
			system("rm /dev/shm/stream-0.h265");
			system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-3.h265 -c copy /dev/shm/box.mp4");
			system("rm /dev/shm/stream-3.h265");
		#else
			system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-0.h264 -c copy /dev/shm/main.mp4");
			system("rm /dev/shm/stream-0.h264");
			system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-3.h264 -c copy /dev/shm/box.mp4");
			system("rm /dev/shm/stream-3.h264");
		#endif
		// system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-4.h264 -c copy /dev/shm/box.mkv");
		for (int i=0; i<file_cnt; i++){
			if (i == 0) {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/main.mp4 -ss 0 -t 20 -c copy /dev/shm/main%d.mp4", i);
				printf("%s\n", file_sep);
				system(file_sep);
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/box.mp4 -ss 0 -t 20 -c copy /dev/shm/box%d.mp4", i);
				// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/box.mkv -ss 0 -t 12 -c copy /dev/shm/box%d.mkv", i);
				printf("%s\n", file_sep);
				system(file_sep);
			}
			else {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/main.mp4 -ss %d.4 -t 20 -c copy /dev/shm/main%d.mp4", (i*20)-1, i);
				printf("%s\n", file_sep);
				system(file_sep);
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/box.mp4 -ss %d.4 -t 20 -c copy /dev/shm/box%d.mp4", (i*20)-1, i);
				// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/box.mkv -ss %d.4 -t 12 -c copy /dev/shm/box%d.mkv", (i*12)-1, i);
				printf("%s\n", file_sep);
				system(file_sep);
			}
		}
		cfile_flag = true;
	}
	return (void*) 0;
}

void *make_mp4_bell(void *argc) {
	int type, file_cnt;
	char file_sep[100] = {0};

	Make_File *mfd =(Make_File*)argc;
	type = mfd->type;
	file_cnt = mfd-> cnt;

	printf("[%s] Type : %d Cnt : %d\n", __func__, type, file_cnt);

	if (file_cnt > 0 && type == 1) {
		#ifdef __H265__			
			system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell-0.h265 -c copy /dev/shm/bell_m.mp4");
			system("rm /dev/shm/bell-0.h265");
			system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell-3.h265 -c copy /dev/shm/bell_b.mp4");
			system("rm /dev/shm/bell-3.h265");
		#else
			system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell-0.h264 -c copy /dev/shm/bell_m.mp4");
			system("rm /dev/shm/bell-0.h264");
			system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell-3.h264 -c copy /dev/shm/bell_b.mp4");
			system("rm /dev/shm/bell-3.h264");
		#endif
		// system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell-4.h264 -c copy /dev/shm/bell_b.mkv");
		for (int i=0; i<file_cnt; i++){
			if (i == 0) {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_m.mp4 -ss 0 -t 20 -c copy /dev/shm/bell_m%d.mp4", i);
				printf("%s\n", file_sep);
				system(file_sep);
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_b.mp4 -ss 0 -t 20 -c copy /dev/shm/bell_b%d.mp4", i);
				// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_b.mkv -ss 0 -t 12 -c copy /dev/shm/bell_b%d.mkv", i);
				printf("%s\n", file_sep);
				system(file_sep);
			}
			else {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_m.mp4 -ss %d.4 -t 20 -c copy /dev/shm/bell_m%d.mp4", (i*20)-1, i);
				printf("%s\n", file_sep);
				system(file_sep);
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_b.mp4 -ss %d.4 -t 20 -c copy /dev/shm/bell_b%d.mp4", (i*20)-1, i);
				// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_b.mkv -ss %d.4 -t 12 -c copy /dev/shm/bell_b%d.mkv", (i*12)-1, i);
				printf("%s\n", file_sep);
				system(file_sep);
			}
		}
		bfile_flag = true;
	}

	return (void*) 0;
}


int clip_total(void);
int clip_total_fake(void);
int stream_total(void);
int Setting_Total(void);

int main(int argc, char **argv) {
	int ret = 0, mode = 0;
    bool up_streming_flag = false;
    bool dn_streming_flag = false;
    // bool aec_test_flag = false;
    bool camera_test_flag = false;
    bool fdpd_flag = false;
    bool adc_flag = false;
    // bool led_flag = false;
    // char file_sep[100] = {0};
    int gval = 0;
    int spk_vol_buf = 80;
    int spk_gain_buf = 14;

    memory_init();
	global_value_init();

	Setting_Init();
	spk_vol_buf = (10 * settings.spk_vol) + 55;
	if (settings.spk_vol == 4){
		spk_gain_buf = 15;
	}
	else
		spk_gain_buf = 15;
    Mosaic_En = settings.SF.bits.per_face;

   
#ifndef	__TEST_FAKE_VEDIO__
	video_init();
#endif

	Init_Audio_In();
	Init_Audio_Out();
	ret = gpio_init();
	if(ret < 0){
		printf("Fail GPIO Init\n");
		return -1;
	}
	// set_system_time_from_file();

	

    pthread_t tid_ao, tid_ai;//, tid_aio_aec;
    pthread_t tid_udp_in, tid_udp_out, tid_spi;
    pthread_t tid_stream, tid_snap, tid_move, tim_osd, tid_fdpd, adc_thread_id;
    pthread_t tid_uart;
    printf("Ver : %s.%s.%s\n", MAJOR_VER, MINOR_VER, CAHR_VER);

    if (settings.SF.bits.move_ex){
 		move_flag = true;
 		move_det_xs = settings.move_ex_s_x;
		move_det_ys = settings.move_ex_s_y;
		move_det_xe = settings.move_ex_e_x;
		move_det_ye = settings.move_ex_e_y;
		printf("Move Ex : %d %d %d %d\n", settings.move_ex_s_x, settings.move_ex_s_y, settings.move_ex_e_x, settings.move_ex_e_y);
    }

    Set_Vol(100,25,spk_vol_buf,spk_gain_buf);

    // isd_crop(0, 0, 1920, 1080, 0);

    isd_crop(286, 322, 1350, 758, 1);

    printf("expval : %d\n", ExpVal);



    boot_mode = mode = start_up_mode();
    // printf("Mode : %d\n", mode);
    // mode = 2;
    if (mode == 1){
    	// printf("Clip Mode!!\n");
    	// if (ExpVal > 1000) gpio_LED_Set(1);
    #ifndef __PHILL_REQ__
    	ret = clip_total();
    #else
    	ret = clip_total_fake();
    #endif
    	if(ret < 0){
        	printf("Clip Mode error\n");
        	return 0;
    	}
    }
    else if(mode == 2){
    	// printf("Streming Mode!!\n");
    	// if (ExpVal > 1000) gpio_LED_Set(1);
    	ret = stream_total();
    	if(ret < 0){
    		printf("Strem Mode Error\n");
    		return 0;
    	}
    }
    else if(mode == 3){
    	// printf("Setting Mode!!\n");
    	Setting_Total();
    }

    #ifdef __PHILL_REQ__
    	ret = clip_total();
    #endif

	while (!bExit && (mode == 0 || mode == 3))
	{
		int cmd = 255;
		int jpg_index = 255;

		printf("cmd 1  Package Find Test.\n");
		// printf("cmd 2  Audio Up Streaming Start!\n");
		// printf("cmd 3  Audio Down Streaming test!\n");
		printf("cmd 4  Audio Effects test!\n");
		// printf("cmd 5  Audio In-Out AEC Test!\n");
		printf("cmd 6  Audio IN/Out Vol, Gain Set!\n");
		printf("cmd 7  LED Test!\n");
		printf("cmd 8  Camera Test!\n");
		printf("cmd 9  Grid Test!\n");
		printf("cmd 10 snap shot!\n");
		printf("cmd 11 thumbnail Test!\n");
		printf("cmd 12 GPIO Mode Test!\n");
		printf("cmd 13 adc test\n");
		printf("cmd 14 Spi Read Test!\n");
		printf("cmd 15 Spi Loop Test!\n");
		printf("cmd 16 Streming Mode!!\n");
		printf("cmd 17 Clip Mode!!\n");
		printf("cmd 18 Fake SPI Test!\n");
		printf("cmd 19 Face Clip Test!!\n");
		printf("cmd 20 PCM Save Start/End\n");
		printf("cmd 21 Box LED ON/OFF\n");
		printf("cmd 22 Box Camera Crop Test\n");
		printf("cmd 23 Distortion Test\n");
		printf("cmd 24 Flicker Test\n");
		printf("cmd 25 Moasic On/Off\n");
		printf("cmd 26 Uart Test\n");
		printf("cmd 27 ISP integration time Test\n");
		printf("cmd 28 Dot Test\n");
		printf("cmd 29 Back Light Compresion\n");
		printf("cmd 30 Setting Value Test\n");
		printf("cmd 31 Set/Get Time\n");
		printf("cmd 32 Save Time to file\n");
		printf("cmd 90 Reset Test\n");
		printf("cmd 99 : exit\n");

		cmd = scanf_cmd();
		ST_Flush();
		
		if (cmd == 1) {
			char *before_img = NULL;
    		char *after_img  = NULL;
    		char *sistic_img = "/dev/shm/corimg1.jpg";
			int threshold = 70;
			double sim = 0.0;
			const char* folderPath = "/dev/shm"; // 탐색할 폴더 경로 설정
    		int fileCount;
    		char** jpgFiles = NULL;
			printf("cmd 1 Package Find Test.\n");
			/////////////// jpg file select. ///////////////////////////
			jpgFiles = listJPGFiles(folderPath, &fileCount);
			if (fileCount == 0) {
     	   		printf("No JPG files found in the specified folder.\n");
        		continue;
    		}

    		displayFileList(jpgFiles, fileCount);

    		printf("Before JPG Index : ");
    		jpg_index = scanf_index();
    		if (jpg_index >= 0 && jpg_index < fileCount) {
    			before_img = jpgFiles[jpg_index];
    		}
    		else {
    			printf("Invalid index.\n");
    			continue;
    		}

    		printf("After JPG Index : ");
    		jpg_index = scanf_index();
    		if (jpg_index >= 0 && jpg_index < fileCount) {
    			after_img = jpgFiles[jpg_index];
    		}
    		else {
    			printf("Invalid index.\n");
    			continue;
    		}

    		printf("Threshold Value(0~100) : ");
    		threshold = scanf_index();
    		if (threshold < 0 || threshold > 100) {
    			printf("Threshold Value Invalid!!\n");
    			continue;
    		}

    		printf("b:%s a:%s t:%d\n", before_img, after_img, threshold);


    		//////////////// Package Fine Start ////////////////////////////////
    		sim = calculateSimilarity(before_img, after_img);
    		if (sim < 0.95) {
    			ret = package_sistic(before_img, after_img);
				if(ret < 0) {
					printf("package_sistic Fail!\n");
					return ret;
				}

				ret = package_find(sistic_img, after_img, 65);
				if(ret < 0) {
					printf("package_find Fail!\n");
					return ret;
				}
	    		else {
        			printf("Box Count : %d\n", ret);
        			if (ret == 0) {
        	    		sim = calculateSimilarity(sistic_img, after_img);
    	        		// printf("similarity:%f %\n", sim);
    	        		// std::cout << "Similarity:" << sim << "\n" << std::ends;
            			
        			}
    			}
    		}
    		else {
    			printf("Chang Not Find!\n");
    			printf("Similarity:%f\n", sim);
    		}
    		freeFileList(jpgFiles, fileCount);
		}
		// else if (cmd == 2) {
		// 	if (!up_streming_flag) {
		// 	printf("cmd 2 Audio Up Streaming Start!\n");
		// 	up_streming_flag = true;
		// 		// pthread_attr_init(&ai_attr);
		// 		// pthread_attr_setinheritsched(&ai_attr, PTHREAD_EXPLICIT_SCHED);
		// 		// pthread_attr_setschedpolicy(&ai_attr, SCHED_RR);

		// 		// struct sched_param ao_param;
		// 		// ao_param.sched_priority = 7;			// thread Importance
		// 		// pthread_attr_setschedparam(&ai_attr, &ao_param);

		// 		// ret = pthread_create(&tid_ai, &ai_attr, IMP_Audio_Play_Thread, NULL);
    	// 		ret = pthread_create(&tid_ai, NULL, IMP_Audio_Record_AEC_Thread, NULL);
		// 		if(ret != 0) {
		// 			IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
		// 			return -1;
		// 		}

				
		// 	}
		// }
		// else if (cmd == 3) {
		// 	if (!dn_streming_flag) {
		// 		printf("cmd 3 Audio Down Streaming test!\n");
		// 		dn_streming_flag = true;
		// 		// pthread_attr_t ao_attr, ai_attr, udp_in_attr, udp_out_attr;

    	// 		// pthread_attr_init(&udp_out_attr);
		// 		// pthread_attr_setinheritsched(&udp_out_attr, PTHREAD_EXPLICIT_SCHED);
		// 		// pthread_attr_setschedpolicy(&udp_out_attr, SCHED_RR);

		// 		// struct sched_param ao_udp_param;
		// 		// ao_udp_param.sched_priority = 5;		// thread Importance
		// 		// pthread_attr_setschedparam(&udp_out_attr, &ao_udp_param);

    	// 		// ret = pthread_create(&tid_udp_in, &udp_out_attr, udp_recv_pthread, NULL);
    	// 		ret = pthread_create(&tid_udp_in, NULL, udp_recv_pthread, NULL);
		// 		if(ret != 0) {
		// 			IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create udp_recv_pthread failed\n", __func__);
		// 			return -1;
		// 		}

		// 		// pthread_attr_init(&ao_attr);
		// 		// pthread_attr_setinheritsched(&ao_attr, PTHREAD_EXPLICIT_SCHED);
		// 		// pthread_attr_setschedpolicy(&ao_attr, SCHED_RR);

		// 		// struct sched_param ao_param;
		// 		// ao_param.sched_priority = 7;			// thread Importance
		// 		// pthread_attr_setschedparam(&ao_attr, &ao_param);

		// 		// ret = pthread_create(&tid_ao, &ao_attr, IMP_Audio_Play_Thread, NULL);
    	// 		ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread, NULL);
		// 		if(ret != 0) {
		// 			IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
		// 			return -1;
		// 		}
		// 	}
		// }
		else if (cmd == 4) {
			char* effect_file = NULL;
    		int effect_index = 255;
			printf("cmd 4 Audio Effects test!\n");
			printf("Effect #(0~6) : ");
			effect_index = scanf_index();
			if (effect_index >= 0 && effect_index < 7) {
				if (effect_index == 0) effect_file = "/tmp/mnt/sdcard/effects/start_16.wav";
				else if (effect_index == 1) effect_file = "/tmp/mnt/sdcard/effects/end_16.wav";
				else if (effect_index == 2) effect_file = "/tmp/mnt/sdcard/effects/takeon_16.wav";
				else if (effect_index == 3) effect_file = "/tmp/mnt/sdcard/effects/takeoff_16.wav";
				else if (effect_index == 4) effect_file = "/tmp/mnt/sdcard/effects/ready_16.wav";
				else if (effect_index == 5) effect_file = "/tmp/mnt/sdcard/effects/start2c.wav";
				else if (effect_index == 6) effect_file = "/tmp/mnt/sdcard/effects/start3c.wav";
				printf("play : %s\n", effect_file);
				ao_file_play_thread(effect_file);
			}
			else if (effect_index >=7 && effect_index < 9) {
				if (effect_index == 7) effect_file = "/tmp/mnt/sdcard/effects/save_ao.pcm";
				else if (effect_index == 8) effect_file = "/tmp/mnt/sdcard/effects/test_out.pcm";
				printf("play : %s\n", effect_file);
				ao_file_play_pcm_thread(effect_file);
			}
			else {
				printf("Invalid #!\n");
				continue;
			}
			
		}
		// else if (cmd == 5) {
		// 	if (!aec_test_flag) {
		// 		printf("cmd 5 Audio In-Out AEC Test!\n");
		// 		aec_test_flag = true;
		// 		// pthread_attr_init(&ao_attr);
		// 		// pthread_attr_setinheritsched(&ao_attr, PTHREAD_EXPLICIT_SCHED);
		// 		// pthread_attr_setschedpolicy(&ao_attr, SCHED_RR);

		// 		// struct sched_param ao_param;
		// 		// ao_param.sched_priority = 7;			// thread Importance
		// 		// pthread_attr_setschedparam(&ao_attr, &ao_param);

		// 		// ret = pthread_create(&tid_aioint , &ao_attr, IMP_Audio_Play_Thread, NULL);
    	// 		ret = pthread_create(&tid_aio_aec, NULL, IMP_Audio_InOut_AEC_Thread, NULL);
		// 		if(ret != 0) {
		// 			IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_InOut_AEC_Thread failed\n", __func__);
		// 			return -1;
		// 		}
		// 	}
		// }
		else if (cmd == 6) {
			printf("cmd 6 Audio IN/Out Vol, Gain Set!\n");
			Get_Vol();
			int ai_vol = 100, ai_gain = 20, ao_vol = 100, ao_gain = 20;
			printf("Audio In Vol:");
			ai_vol = scanf_index();
			if (ai_vol < 0 || ai_vol > 100) {
				printf("Invalid Value!!\n");
				continue;
			}
			printf("Audio In gain:");
			ai_gain = scanf_index();
			if (ai_gain < 0 || ai_gain > 30) {
				printf("Invalid Value!!\n");
				continue;
			}
			printf("Audio Out Vol:");
			ao_vol = scanf_index();
			if (ao_vol < 0 || ao_vol > 100) {
				printf("Invalid Value!!\n");
				continue;
			}
			printf("Audio Out gain:");
			ao_gain = scanf_index();
			if (ao_gain < 0 || ao_gain > 30) {
				printf("Invalid Value!!\n");
				continue;
			}

			printf("Set Ai vol:%d\n", ai_vol);
			printf("Set Ai gain:%d\n", ai_gain);
			printf("Set Ao vol:%d\n", ao_vol);
			printf("Set Ao gain:%d\n", ao_gain);
			Set_Vol(ai_vol, ai_gain, ao_vol, ao_gain);
		}
		else if (cmd == 7) {
			// int led_duty = 0;
			// printf("cmd 7 LED Test!\n");
			// if (!led_flag) {
			// 	system("echo 6 > /sys/class/pwm/pwmchip0/export");
				
			// 	led_flag = true;
			// }
			// printf("Duty?");
			// led_duty = scanf_index();
			// system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
			// memset(file_sep, 0, 100);
			// sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
			// printf(file_sep);
			// printf("\n");
			// system(file_sep);
			// system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
			// system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");

			gpio_LED_Set(1);
		}
		else if (cmd == 8) {
			int sel = 0;
			printf("cmd 8 Camera Test!\n");
			printf("Select Data Send Type(1:UDP, 2:SPI):");
			sel = scanf_index();
			if (sel == 1) {
				printf("Input IP:");
				scanf_string(ip);
				printf("ip:%s %d\n", ip, strlen(ip));
				ret = pthread_create(&tid_udp_out, NULL, udp_send_pthread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create udp_send_pthread failed\n", __func__);
					return -1;
				}
			}
    		else if(sel == 2) {
    			printf("Select Type(1:Main, 2:Box, 3:Audio 4:All):");
				data_sel = scanf_index();
				if (data_sel <= 0 || data_sel > 4) {
					printf("Invalid Type!\n");
					continue;
				}
				//////////////// SPI Init ////////////////////////////////////////////////////////////////
				ret = spi_init();
    			if(ret < 0){
    			    printf("spi init error\n");
    			    return 0;
    			}

				if (data_sel <= 0 || data_sel > 4) {
					printf("Invalid Type!\n");
					return -1;
				}

				// pthread_attr_t spi_send_attr;

    			// pthread_attr_init(&spi_send_attr);
				// pthread_attr_setinheritsched(&spi_send_attr, PTHREAD_EXPLICIT_SCHED);
				// pthread_attr_setschedpolicy(&spi_send_attr, SCHED_RR);

				// struct sched_param spi_send_param;
				// spi_send_param.sched_priority = 5;		// thread Importance
				// pthread_attr_setschedparam(&spi_send_attr, &spi_send_param);

    			// ret = pthread_create(&tid_spi, &spi_send_attr, spi_send_stream, NULL);

    			ret = pthread_create(&tid_spi, NULL, spi_send_stream, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create spi_send_stream failed\n", __func__);
					return -1;
				}

				
				//////////////////////////////////////////////////////////////////////////////////////////
			}
			else {
				printf("Invalid Selection!\n");
				continue;
			}


			if (!camera_test_flag) {
				stream_state = 1;
				clip_rec_state = REC_START;
				// rec_stop = false;
			#ifndef __TEST_FAKE_VEDIO__
				ret = pthread_create(&tim_osd, NULL, OSD_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create OSD_thread failed\n", __func__);
					return -1;
				}
				 
				ret = pthread_create(&tid_stream, NULL, get_video_stream_user_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_stream_user_thread failed\n", __func__);
					return -1;
				}

				// pthread_t tid_clip;
				// ret = pthread_create(&tid_clip, NULL, get_video_clip_user_thread, NULL);
				// if(ret != 0) {
				// 	IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_clip_user_thread failed\n", __func__);
				// 	return -1;
				// }

				ret = pthread_create(&tid_snap, NULL, get_snap_stream_user_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_snap_stream_user_thread failed\n", __func__);
					return -1;
				}

				ret = pthread_create(&tid_move, NULL, move_detecte_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create move_detecte_thread failed\n", __func__);
					return -1;
				}

				ret = pthread_create(&tid_fdpd, NULL, fdpd_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create fdpd_thread failed\n", __func__);
					return -1;
				}

			#else
				ret = pthread_create(&tid_stream, NULL, get_video_stream_test_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_stream_user_thread failed\n", __func__);
					return -1;
				}
			#endif
				
				

				if (sel == 1) {
					ret = pthread_create(&tid_udp_in, NULL, udp_recv_pthread, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create udp_recv_pthread failed\n", __func__);
						return -1;
					}
				}


				dn_streming_flag = true;
    			ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
					return -1;
				}

				up_streming_flag = true;
				ret = pthread_create(&tid_ai, NULL, IMP_Audio_Record_AEC_Thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
					return -1;
				}

				camera_test_flag = true;
			}


		}
		else if (cmd == 9) {
			int grid;
			printf("cmd 9 Grid Test!\n");
			printf("Select Grid Number(0~%d):", GRID_COVER_INDEX);
			grid = scanf_index();

			if(!grid_cover_flag[grid]){
				grid_cover_flag[grid] = true;
			}
			else {
				grid_cover_flag[grid] = false;
			}
			osd_grid_show(grid);
		}
		else if (cmd == 10) {
			int snap;
			printf("main : 1\n");
			printf("secoend : 2\n");
			printf("cmd 10 snap shot!\n");
			snap = scanf_index();
			if (snap == 1) {
				main_snap = true;
			}
			else if (snap == 2) {
				box_snap = true;
			}
			else {
				printf("Invalid Selection!\n");
			}
		}
		else if (cmd == 11) {
			printf("cmd 11 thumbnail Test!\n");
			thum_face_data.flag[0] = true;
			thum_face_data.x[0] = 100;
			thum_face_data.y[0] = 200;
			thum_face_data.ex[0] = 500;
			thum_face_data.ey[0] = 600;
			thumbnail_make(thum_face_data);
		}
		else if (cmd == 12) {
			int mode;
			printf("cmd 12 GPIO Mode Test!\n");
			mode = start_up_mode();
			printf("mode : %d (1:clip/2:stream/3:setting/0:test)\n", mode);
		}
		
		// else if (cmd == 13) {
		// 	char file_sep[100] = {0};
		// 	system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-1.h265 -c copy /dev/shm/main.mp4");
		// 	system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-4.h265 -c copy /dev/shm/box.mp4");
		// 	for (int i=0; i<4; i++){
		// 		memset(file_sep, 0, 100);
		// 		sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/main.mp4 -ss %d.4 -t 12 -c copy /dev/shm/main%d.mp4", (i*12)-1, i);
		// 		system(file_sep);
		// 		memset(file_sep, 0, 100);
		// 		sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/box.mp4 -ss %d.4 -t 12 -c copy /dev/shm/box%d.mp4", (i*12)-1, i);
		// 		system(file_sep);
		// 	}
		// 	system("sync");
		// }
		else if (cmd == 13) {
			if (adc_flag == false) {
				printf("cmd 13 adc test\n");
				system("echo 54 > /sys/class/gpio/export");
				system("echo out > /sys/class/gpio/gpio54/direction");
				system("echo 1 > /sys/class/gpio/gpio54/value");
				// /* get value thread */
				adc_init();
				
				adc_flag = true;
			}
			ret = pthread_create(&adc_thread_id, NULL, adc_get_voltage_thread, NULL);
			if (ret != 0) {
				printf("error: pthread_create error!!!!!!");
				return -1;
			}
    	}
    	else if (cmd == 14) {
    		printf("cmd 14 : spi read test");
    		ret = gpio_get_val(PORTB+18);
			if(ret < 0){
				printf("Ready Busy GPIO not Set : %d\n", PORTB+18);
			}
    		ret = spi_init();
    		if(ret < 0){
    		    printf("spi init error\n");
    		    return 0;
    		}
			test_spi_rw();
		}
		else if (cmd == 15) {
			int sdly;
			printf("cmd 15 : spi loop test");
			// printf("Delay:");
			sdly = 1;
			// sdly = scanf_index();
			ret = spi_init();
    		if(ret < 0){
    		    printf("spi init error\n");
    		    return 0;
    		}
			test_spi_onekbytes(sdly);
		}
		else if (cmd == 16) {
			printf("cmd 17 Streming Mode!!\n");
    		ret = stream_total();
    		if(ret < 0){
    			printf("Strem Mode Error\n");
    			return 0;
    		}
		}
		else if (cmd == 17) {
			printf("cmd 18 Clip Mode!!\n");
    		ret = clip_total();
    		if(ret < 0){
        		printf("Clip Mode error\n");
        		return 0;
    		}
		}
		else if (cmd == 18) {
			printf("cmd 16 Fake SPI Test!\n");
			data_sel = 4;
			//////////////// SPI Init ////////////////////////////////////////////////////////////////
			ret = spi_init();
   			if(ret < 0){
   			    printf("spi init error\n");
   			    return 0;
   			}
			if (data_sel <= 0 || data_sel > 4) {
				printf("Invalid Type!\n");
				return -1;
			}
  			ret = pthread_create(&tid_spi, NULL, spi_test_send_stream, NULL);
			if(ret != 0) {
				IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create spi_send_stream failed\n", __func__);
				return -1;
			}
			//////////////////////////////////////////////////////////////////////////////////////////
			if (!camera_test_flag) {
				stream_state = 1;
				clip_rec_state = REC_START;
				// rec_stop = false;
				ret = pthread_create(&tim_osd, NULL, OSD_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create OSD_thread failed\n", __func__);
					return -1;
				}
				 
				ret = pthread_create(&tid_stream, NULL, get_video_stream_user_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_stream_user_thread failed\n", __func__);
					return -1;
				}
				
				// ret = pthread_create(&tid_clip, NULL, get_video_clip_user_thread, NULL);
				// if(ret != 0) {
				// 	IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_clip_user_thread failed\n", __func__);
				// 	return -1;
				// }

				ret = pthread_create(&tid_snap, NULL, get_snap_stream_user_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_snap_stream_user_thread failed\n", __func__);
					return -1;
				}

				ret = pthread_create(&tid_move, NULL, move_detecte_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create move_detecte_thread failed\n", __func__);
					return -1;
				}

				ret = pthread_create(&tid_fdpd, NULL, fdpd_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create fdpd_thread failed\n", __func__);
					return -1;
				}
				
				camera_test_flag = true;

				// up_streming_flag = true;
    			// ret = pthread_create(&tid_ai, NULL, IMP_Audio_Record_AEC_Thread, NULL);
				// if(ret != 0) {
				// 	IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
				// 	return -1;
				// }
			}

		}
		else if (cmd == 19) {
			printf("cmd 19 Face Clip Test!!\n");
			spi_send_fake_file(REC_CLIP_F);
			usleep(15000*1000);
			spi_send_fake_file(REC_CLIP_B);
			usleep(15000*1000);
		}
		else if (cmd == 20) {
			printf("cmd 20 PCM Save Start/End\n");
			if (save_pcm == 0){
				printf("PCM Save Start!\n");
				save_pcm = 1;
			}
			else if(save_pcm == 1){
				printf("PCM Save End!\n");
				save_pcm = 2;
				while (pcm_in == 0) {};
				if (pcm_in){
					save_pcm = 3;
					system("cp /dev/shm/test_in.pcm /tmp/mnt/sdcard/effects");
					system("cp /dev/shm/test_out.pcm /tmp/mnt/sdcard/effects");
					system("sync");
					printf("pcm Test Copy end!\n");
				}
			}
		}
		else if (cmd == 21) {
			printf("cmd 21 Box LED ON/OFF\n");
			ret = gpio_set_val(PORTD+6, gval);
			if(ret < 0){
				printf("Fail set Value GPIO : %d\n", PORTD+6);
				return -1;
			}
			if (gval == 0) gval = 1;
			else gval = 0;
		}
		else if (cmd == 22) {
			int x, y, w, h, c;
			printf("cmd 22 Box Camera Crop Test\n");
			printf("Set X:");
			x = scanf_index();
			printf("Set Y:");
			y = scanf_index();
			printf("Set Width:");
			w = scanf_index();
			printf("Set Height:");
			h = scanf_index();
			printf("Cam select[0:main/1:box]:");
			c = scanf_index();

			isd_crop(x, y, w, h, c);
		}
		else if (cmd == 23) {
			int x, y, w, h, str, c;
			printf("cmd 23 Distortion Test\n");
			printf("Set Center X:");
			x = scanf_index();
			printf("Set Center Y:");
			y = scanf_index();
			printf("Set Width:");
			w = scanf_index();
			printf("Set Height:");
			h = scanf_index();
			printf("Set strength:");
			str = scanf_index();
			printf("Cam select[0:main/1:box]:");
			c = scanf_index();

			isd_distortion(x, y, w, h, str, c);
		}
		else if (cmd == 24) {
			int freq, mode;
			printf("cmd 24 Flicker Test\n");
			printf("Set Flicker Freq:");
			freq = scanf_index();
			printf("0:disable 1:normal 2:auto 3:value dep\n");
			printf("Set Flicker Mode:");
			mode = scanf_index();

			isp_filcker (freq, mode);
			isp_filcker_get();
		}
		else if (cmd == 25) {
			printf("cmd 25 Moasic On/Off\n");
			if (!Mosaic_En) Mosaic_En = true;
			else Mosaic_En = false;
		}
		else if (cmd == 26) {
			int type = 0;
			printf("cmd 26 Uart Test\n");
			printf("1: rx Thread 2: Tx Test\n");
			type = scanf_index();
			if (type == 1) {
				ret = pthread_create(&tid_uart, NULL, uart_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create uart_thread failed\n", __func__);
					return -1;
				}
			}
			else if (type == 2) {
				uart_tx_test();
			}
			else {
				printf("Tyep Invaild.\n");
			}
		}
		else if (cmd == 27) {
			uint32_t isp_igtime = 132;
			int getset = 0;
			printf("cmd 27 ISP integration time Test\n");
			printf("time Get/Set(0/1):");
			getset = scanf_index();
			if (getset > 0) {
				printf("Value:");
				isp_igtime = (uint32_t)scanf_index();
			}
			isp_igtime = isp_integration_time(getset, isp_igtime);
			
		}
		else if (cmd == 28) {
			printf("cmd 28 Dot Test\n");
			if (!dot_En) dot_En = true;
			else dot_En = false;
		}
		else if (cmd == 29) {
			int getset = 0;
			uint8_t val = 0;
			printf("cmd 29 Back Light Compresion\n");
			getset = scanf_index();
			if (getset > 0) {
				printf("Value:");
				val = scanf_index();
			}
			SceneceSet(getset, val);
		}
		else if (cmd == 30) {
			printf("cmd 30 Setting Value Test\n");
			printf("Vol:");
			settings.spk_vol = scanf_index();
			Setting_Save();
		}
		else if (cmd == 31) {
			int sget = 0;
			int year = 2024;
			int month = 4;
			int day = 5;
			int hour = 9;
			int min = 31;
			int sec = 0;
			printf("cmd 31 Set/Get Time\n");
			printf("Time Set:1/Time Get :2\n");
			printf("Set/Get : ");
			sget = scanf_index();
			if (sget < 1 || sget > 2) {
				printf("Invaild Set/Get Value!\n");
				continue;
			}
			if (sget == 1) {
				printf("Year:");
				year = scanf_index();
				if (year < 1900 || year > 2100) {
					printf("Invalid Set Year!\n");
				}
				printf("month:");
				month = scanf_index();
				if (month < 1 || month > 12) {
					printf("Invalid Set month!\n");
				}
				printf("day:");
				day = scanf_index();
				if (day < 1 || day > 31) {
					printf("Invalid Set day!\n");
				}
				printf("hour:");
				hour = scanf_index();
				if (hour < 0 || hour > 23) {
					printf("Invalid Set hour!\n");
				}
				printf("min:");
				min = scanf_index();
				if (min < 0 || min > 59) {
					printf("Invalid Set Year!\n");
				}
				printf("sec:");
				sec = scanf_index();
				if (sec < 0 || sec > 59) {
					printf("Invalid Set sec!\n");
				}
				// printf("Set Date : %d %d %d %d %d %d\n", year, month, day, hour, min, sec);
				SetTime(year, month, day, hour, min, sec);
			}
			else if(sget == 2) {
				GetTime();
			}

		}
		else if (cmd == 32) {
			printf("cmd 32 Save Time to file\n");
			record_shutdown_time();
		}
		else if (cmd == 33) {
			uint32_t targetbit = 500;
			printf("cmd 33 Target Bit Change!\n");
			targetbit = scanf_index();
			Set_Target_Bit(targetbit);
		}
		else if (cmd == 34) {
			char filepath1[] = "/tmp/mnt/sdcard/isc.zip";
			char md5_hash_code1[33] = {0};
			// // test_hash222(filepath1);
			// test_md5(filepath1);
			// system("cp /tmp/mnt/sdcard/isc.zip /dev/shm/isc.zip");
			// char filepath2[] = "/dev/shm/isc.zip";
			// test_md5(filepath2);
			md5_get(filepath1, md5_hash_code1);
		}
		else if (cmd == 35) {
			printf("cmd 35 Save File Test!\n");

			system("ubi_mount");

			file_name_get(&Save_movie1, 0);

			for(int i=0;i<Save_movie1.cnt;i++) {
				// char sepp[128];
				printf("File[%d]:%s\n", i, Save_movie1.name[i]);
				// sprintf(sepp, "/maincam/%s", Save_movie.name[i]);
				spi_send_save_file("/maincam/", Save_movie1.name[i]);
			}

		}
		else if (cmd == 36) {
			int max_sharp = 0, max_foucs = 0;
			double avr_sharp, avr_focus;
			char file_name[128];
			Focus_Sharpness fs_t[10];

			for (int q=0; q<10; q++) {
				main_snap = true;
				while (main_snap);

				memset(file_name , 0x00, 128);
				sprintf(file_name, "/dev/shm/main%d.jpg", q);
				// sharpness[q] = Sharpness_cal(file_name);
				ret = focus_and_sharpness_cal(file_name, (Focus_Sharpness2*)&fs_t[q]);
				// printf("now:%f max:%f\n", sharpness[q],  sharpness[max_sharp]);
				if (fs_t[q].sharpness > fs_t[max_sharp].sharpness){
					max_sharp = q;
					// printf("Max Q : %d\n", max_sharp);
				}

				if (fs_t[q].focus > fs_t[max_foucs].focus){
					max_foucs = q;
					// printf("Max Q : %d\n", max_sharp);
				}
			}

			avr_sharp = 0;
			avr_focus = 0;

			for (int q=0; q<10; q++){
				if (q != max_sharp) {
					avr_sharp += fs_t[q].sharpness;
					// printf("shapness add %d : %f\n", q, avr_sharp);
				}

				if (q != max_foucs) {
					avr_focus += fs_t[q].focus;
					// printf("focus add %d : %f\n", q, avr_focus);
				}
			}
			printf("Avrage Focus : %f\n", avr_focus/9);
			printf("Avrage Sharpness : %f\n", avr_sharp/9);
		}
		else if (cmd == 37) {
			int dim_val = 0;
			int64_t dimming_e = 0;
			int dimming_val = 90;
			bool dimming_up = true;
			printf("cmd 37 Dimming Test\n");
			printf("Type : ");
			dim_val = scanf_index();
			gpio_LED_dimming(dim_val);
			while (dim_val == 2) {
				if (dimming) {
					dimming_e =  sample_gettimeus() - dimming_s;
					if ((dimming_e > 30000) & dimming_up) {
						dimming_s = sample_gettimeus();
						dimming_val -= 1;
						LED_dimming (dimming_val);
						if (dimming_val <= 10) {
							dimming_up = false;
						}
					}
					else if ((dimming_e > 30000) & !dimming_up) {
						dimming_s = sample_gettimeus();
						dimming_val += 1;
						LED_dimming (dimming_val);
						if (dimming_val >= 90) {
							dimming_up = true;
						}
					}
				}
			}
		}
		else if (cmd == 90) {
			printf("cmd 90 Reset Test\n");
			system("reboot");
		}
		else if (cmd == 99) {
			printf("Exiting Program! Plz Wait!\n");
			bExit = 1;
			bStrem = true;
			signal(SIGINT, handler);
			// break;
		}
		else {
			printf("Not Defined Cmd!\n");
		}
		printf("cmd:%d\n", cmd);
		
		// if (save_pcm == 2) {
		// 	printf("pcm cp %d %d\n", pcm_in);
		// 	if (pcm_in){
		// 		save_pcm = 3;
		// 		system("cp /dev/shm/test_in.pcm /tmp/mnt/sdcard/effects");
		// 		system("cp /dev/shm/test_out.pcm /tmp/mnt/sdcard/effects");
		// 		system("sync");
		// 		printf("pcm Test Copy end!\n");
		// 	}
		// }
	}

	sleep(1);
	
	if (up_streming_flag) {
		sleep(0.5);
		// ret = pthread_kill(tid_ai, 0);
		// if (ret == 0) {
			// pthread_cancel(tid_ai);
		// }
		// else {
			pthread_join(tid_ai, NULL);
		// }
	}

	// sleep(0.5);
	// ret = pthread_kill(tid_udp_out, 0);
	// if (ret == 0) {
	// 	pthread_cancel(tid_udp_out);
	// }
	// else {
	// 	pthread_join(tid_udp_out, NULL);
	// }

	sleep(0.5);
	// ret = pthread_kill(tid_spi, 0);
	// if (ret == 0) {
		// pthread_cancel(tid_spi);
	// }
	// else {
		pthread_join(tid_spi, NULL);
	// }
	
	if (dn_streming_flag) {
		// sleep(0.5);
		// ret = pthread_kill(tim_osd, 0);
		// if (ret == 0) {
			// pthread_cancel(tim_osd);
		// }
		// else {
			pthread_join(tim_osd, NULL);
		// }

		// ret = pthread_kill(tid_udp_in, 0);
		// if (ret == 0) {
			// pthread_cancel(tid_udp_in);
		// }
		// else {
			pthread_join(tid_udp_in, NULL);
		// }

		// ret = pthread_kill(tid_ao, 0);
		// if (ret == 0) {
			// pthread_cancel(tid_ao);
		// }
		// else {
			pthread_join(tid_ao, NULL);
		// }
	}

	// if (aec_test_flag) {
		// ret = pthread_kill(tid_aio_aec, 0);
		// if (ret == 0) {
			// pthread_cancel(tid_aio_aec);
		// }
		// else {
			// pthread_join(tid_aio_aec, NULL);
		// }
	// }

	if (camera_test_flag) {
		// ret = pthread_kill(tid_stream, 0);
		// if (ret == 0) {
			// pthread_cancel(tid_stream);
		// }
		// else {
			pthread_join(tid_stream, NULL);
			// pthread_join(tid_clip, NULL);
			
		// }
	}

	if (fdpd_flag) {
		pthread_join(tid_fdpd, NULL);
	}

	if (adc_flag){
		pthread_join(adc_thread_id, NULL);
		adc_deinit();
	}

	Deinit_Audio_In();
	Deinit_Audio_Out();
	video_deinit();
	memory_deinit();
	spi_deinit();

	return 0;
}



int clip_total(void) {
	int ret = 0;
	int file_cnt = 0, file_cnt2 = 0;
	int fpdp_cnt = 0;
	bool start_flag = false;
	bool ubi_flag = false;
	bool spi_th_flag = false;
	
	int64_t end_time = 0, total_time = 0;
	int64_t start_time2 = 0, end_time2 = 0, total_time2 = 0;
	char file_path[128] = {0};
	char file_sep[100] = {0};

	uint8_t major_buf1, major_buf2;


	pthread_t tid_ao, tid_ai;
    // pthread_t tid_udp_in, tid_udp_out, tid_spi;
    pthread_t tid_stream, tid_snap, tid_move, tim_osd, tid_fdpd;
    pthread_t tid_uart;//, tid_live;


    // Init_Audio_Out();
	// Init_Audio_In();
	// Set_Vol(100, 20, 80, 15);

    pthread_t tid_spi;
	//////////////// SPI Init ////////////////////////////////////////////////////////////////
	ret = spi_init();
    if(ret < 0){
        printf("spi init error\n");
        return 0;
    }

    
	//////////////////////////////////////////////////////////////////////////////////////////


    thumbnail_state = 0;
    stream_state = 0;
	clip_rec_state = REC_START;
	// rec_stop = false;
	ret = pthread_create(&tim_osd, NULL, OSD_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create OSD_thread failed\n", __func__);
		return -1;
	}
				 
	ret = pthread_create(&tid_stream, NULL, get_video_stream_user_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_stream_user_thread failed\n", __func__);
		return -1;
	}
				
	// ret = pthread_create(&tid_clip, NULL, get_video_clip_user_thread, NULL);
	// if(ret != 0) {
	// 	IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_clip_user_thread failed\n", __func__);
	// 	return -1;
	// }

	ret = pthread_create(&tid_move, NULL, move_detecte_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create move_detecte_thread failed\n", __func__);
		return -1;
	}

	ret = pthread_create(&tid_fdpd, NULL, fdpd_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create fdpd_thread failed\n", __func__);
		return -1;
	}

	ret = pthread_create(&tid_snap, NULL, get_snap_stream_user_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_snap_stream_user_thread failed\n", __func__);
		return -1;
	}

	ret = pthread_create(&tid_uart, NULL, uart_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create uart_thread failed\n", __func__);
		return -1;
	}

	// ret = pthread_create(&tid_live, NULL, device_live_thread, NULL);
	// if(ret != 0) {
		// IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create device_live_thread failed\n", __func__);
		// return -1;
	// }

	if (settings.SF.bits.led) {
	    if (ExpVal > 10000) { // Noh Change 1000 -> 10000 20240530
	    	gpio_LED_Set(1);
	    }
	    else {
	    	gpio_LED_Set(2);
	    }
	}

	usleep(300*1000);

	do {
		if (start_flag == false) {
		#ifndef __PHILL_REQ__
			// if ((face_cnt > 0) || (person_cnt > 0) || (main_motion_detect > 1) || bell_flag || temp_flag) {
			if ((face_cnt > 0) || (main_motion_detect > 1) || bell_flag || temp_flag) {
				printf("fc : %d Motion : %d\n", face_cnt, main_motion_detect);
				printf("Start REC!!\n");
				start_flag = true;
				roaming_person = false;
				// start_time = sample_gettimeus();
				clip_cause_t.Major = CLIP_CAUSE_MOVE;
				clip_cause_t.Minor = CLIP_MOVE_MOVE;
				Rec_type = CLIP_REC;
				end_time = start_time + 5000000;
			}
			else if ((sample_gettimeus() - start_time) > START_CHECK_TIME) {
				device_end(STREAMING);
				printf("Not Detection!! Device Turn Off.\n");
				break;
			}
		#else
			printf("Start REC!!\n");
			start_flag = true;
			roaming_person = false;
			// start_time = sample_gettimeus();
			clip_cause_t.Major = CLIP_CAUSE_MOVE;
			clip_cause_t.Minor = CLIP_MOVE_MOVE;
			Rec_type = CLIP_REC;
			end_time = start_time + 5000000;
		#endif
		}
		else {
			if (Rec_type == CLIP_REC){
				total_time = sample_gettimeus() - start_time;

				// if(total_time > THUMBNAIL_TIME) {
					// printf("Thumb : %d fr : %d\n", thumbnail_state, fr_state);
				// }

				if((total_time > THUMBNAIL_TIME) && (fr_state == FR_WAIT) &&
					(thumbnail_state == THUMB_WAIT || thumbnail_state == THUMB_END)) {
					printf("Thumb state : %d\n", thumbnail_state);
					fr_state++;
				}
				else if(fr_state == FR_SNAPSHOT) {
					fr_state++;
					printf("Face Data Send!!\n");
					ret = facecrop_make(facial_data);
					if (ret < 0 && fpdp_cnt < 5) {
						printf("Facial Fail. Retry.\n");
						memset(file_sep, 0, 100);
						sprintf(file_sep, "rm /dev/shm/face.jpg");
						printf("%s\n", file_sep);
						system(file_sep);
						fr_state = FR_WAIT;
						fpdp_cnt++;
					}
					else if (fpdp_cnt >= 5){
						fr_state = FR_END; // facecrop_make fail x 5
					}
				}
				else if(fr_state == FR_SUCCESS) {
					if (face_crop_cnt < 5)
						fr_state = FR_WAIT;
					else{
						printf("Face Cnt : %d\n", face_crop_cnt);
						fr_state = FR_END;
					}
				}
				else if (fr_state != FR_END && total_time > FACE_FIND_END_TIME) {
					fr_state = FR_END;	// fr_state 5 / Time out
					// Make File Send
					if (Ready_Busy_Check() && face_crop_cnt > 0){
						spi_send_file_face(REC_FACESHOT, face_crop_cnt);
					}
					face_end(REC);
				}

				if ((total_time > THUMBNAIL_TIME) && (thumbnail_state == THUMB_WAIT) &&
					(fr_state == FR_WAIT || fr_state == FR_END || fr_state == FR_START)) {	// thumbnail
					printf("Thumbnail Make Start!!\n");
					thumbnail_snap = true;
					thumbnail_state = THUMB_START;
				}
				if (thumbnail_state == THUMB_SNAPSHOT) {
	   				thumbnail_make(thum_face_data);
	   				thumbnail_state = THUMB_END;
	   				if (!netwrok_busy) {
		   				if (Ready_Busy_Check()){
							printf("Thumbnail Send!\n");
							memset(file_path, 0, 64);
							sprintf(file_path, "/dev/shm/thumbnail_last.jpg");
							spi_send_file(REC_SNAPSHOT, file_path, 0, 0, 0);
						}
					}
					// else {
					// 	printf("Fail to Thumbnail Send.\n");
					// }
				}

				if (!spi_th_flag && (total_time > FACE_FIND_END_TIME) && (stream_state == 1)) {
					printf("Clip Streaming Start!!\n");
					spi_th_flag = true;
					data_sel = 4;
					if (data_sel <= 0 || data_sel > 4) {
						printf("Invalid Type!\n");
						return -1;
					}
					ret = pthread_create(&tid_spi, NULL, spi_send_stream, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create spi_send_stream failed\n", __func__);
						return -1;
					}
					ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
						return -1;
					}

					ret = pthread_create(&tid_ai, NULL, IMP_Audio_Record_AEC_Thread, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
						return -1;
					}
					if (rec_streaming_state == REC_START) rec_streaming_state = REC_ING;
				}

				if (start_flag && (face_cnt != 0) && !roaming_person) {
					printf("Face Check!\n");
					roaming_person = true;
				}

				if ((total_time > ROAMING_PER_TIME) && roaming_person && 
					clip_cause_t.Major == CLIP_CAUSE_MOVE && clip_cause_t.Minor == CLIP_MOVE_MOVE) {
					printf("Roaming Person Tag!! : %lld\n", total_time);
					clip_cause_t.Minor = CLIP_MOVE_PER;
				}

				if (rec_streaming_state == REC_STOP) {
					rec_streaming_state = REC_MP4MAKE;
					printf("CLIP END:Streaming End! %lld\n", total_time);
					clip_rec_state = REC_STOP;
					box_snap = true;
					Rec_type = MAKE_FILE;
				}

				if ((file_cnt == 0) && (bell_flag)) {	// Bell Push -> Clip Stop
					printf("CLIP END:Bell!\n");
					clip_rec_state = REC_STOP;
					bell_rec_state = REC_START;
					bell_flag = false;
					Rec_type = BELL_REC;
					bl_state = BSS_START;
					bell_snap_m = true;
					bell_snap_b = true;
					start_time2 = end_time2 = sample_gettimeus();
				}

				if ((total_time > MAX_REC_TIME) && (clip_rec_state == REC_ING)) {	// 60Sec Time Over -> Clip Stop
					// rec_stop = true;
					printf("CLIP END:Time Over! %lld\n", total_time);
					clip_rec_state = REC_STOP;
					box_snap = true;
					Rec_type = MAKE_FILE;
					file_cnt = 3;
				}
				
			#ifndef __PHILL_REQ__
				if ((file_cnt == 0) && (total_time > FACE_FIND_END_TIME) && (clip_rec_state < REC_STOP)) {	// Face or Motion Not Found -> Clip Stop
					if ((person_cnt == 0) && (main_motion_detect == 0)) {
						if ((sample_gettimeus() - end_time) > CLIP_CLOSE_TIME) {
							printf("CLIP END:Move End!\n");
							// rec_stop = true;
							clip_rec_state = REC_STOP;
							box_snap = true;
							Rec_type = MAKE_FILE;
							// if (total_time < 23000000) {
							// 	file_cnt = 1;
							// }
							// else if (total_time < 43000000) {
							// 	file_cnt = 2;
							// }
							// else if (total_time >= 43000000) {
							// 	file_cnt = 3;
							// }
							// else {
								// printf("file Count Error!\n");
								// file_cnt = 3;
							// }
							printf("Detection End! REC END. file cnt : %d\n", file_cnt);
						}
					}
					else {
						// printf("face:%d, person:%d, move:%d\n", face_cnt, person_cnt, main_motion_detect);
						end_time = sample_gettimeus();
					}
				}
			#endif

				
			}
			else if (Rec_type == BELL_REC){


				
				total_time2 = sample_gettimeus() - start_time2;
				if (total_time2%10000000 == 0){
					printf("Rec T:%d time : %lld\n", Rec_type, total_time2);
				}

				if (!bell_snap_m && !bell_snap_b && bl_state < BSS_SEND) bl_state = BSS_SEND;

				if (bl_state == BSS_SEND) {
					if (temp_flag) {
						major_buf1 = REC_TEMP_SNAP_M;
						major_buf2 = REC_TEMP_SNAP_B;
					}
					else {
						major_buf1 = REC_BELL_SNAP_M;
						major_buf2 = REC_BELL_SNAP_B;
					}

					if (!netwrok_busy) {
						if (Ready_Busy_Check()){
							printf("Bell/Temp Dual JPG Send!\n");
							memset(file_path, 0, 64);
							sprintf(file_path, "/dev/shm/bell_m.jpg");
							memset(file_sep, 0, 64);
							sprintf(file_sep, "/dev/shm/bell_b.jpg");
							spi_send_file_dual(major_buf1, major_buf2, file_path, file_sep);
						}
						else {
							printf("Fail to Dual Bell JPG Send.\n");
						}

					}

					face_end(REC);

					stream_state = 1;
					data_sel = 4;

					bl_state = BSS_END;
				}

				if (!spi_th_flag && (stream_state == 1)) {
					printf("Bell Streaming Start!!\n");
					spi_th_flag = true;
					data_sel = 4;
					if (data_sel <= 0 || data_sel > 4) {
						printf("Invalid Type!\n");
						return -1;
					}
					ret = pthread_create(&tid_spi, NULL, spi_send_stream, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create spi_send_stream failed\n", __func__);
						return -1;
					}
					ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
						return -1;
					}

					ret = pthread_create(&tid_ai, NULL, IMP_Audio_Record_AEC_Thread, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
						return -1;
					}
				}

				if ((rec_streaming_state == REC_STOP) && (total_time2 > BELL_TIME_MIN)) {
					rec_streaming_state = REC_MP4MAKE;
					printf("BELL END:Steaming End! %lld\n", total_time);
					bell_rec_state = REC_STOP;
					if (bell_stream_flag == false){
						Rec_type = MAKE_FILE;
						// box_snap = true;
					}
				}

				if ((total_time2 > MAX_REC_TIME) && (bell_rec_state == REC_ING) && (bell_rec_state < REC_STOP)) {	// 60Sec Time Over -> Clip Stop
					// rec_stop = true;
					printf("BELL END:Time Over! %lld\n", total_time);
					bell_rec_state = REC_STOP;
					if (bell_stream_flag == false){
						Rec_type = MAKE_FILE;
						// box_snap = true;
					}
				}

				if ((total_time2 > BELL_TIME_MIN) && (person_cnt == 0) && (main_motion_detect == 0) && (bell_rec_state < REC_STOP)) {
					if ((sample_gettimeus() - end_time2) > CLIP_CLOSE_TIME) {
						printf("BELL END:Move End! %lld\n", total_time);
						bell_rec_state = REC_STOP;
						if (bell_stream_flag == false) {
							Rec_type = MAKE_FILE;
							// box_snap = true;
						}
					}
				}
				else {
					end_time2 = sample_gettimeus();
				}

				if ((bell_rec_state == REC_STOP) && (bell_stream_flag == false)) {
					stream_state = 0;
					Rec_type = MAKE_FILE;
					// box_snap = true;
				}
			}
		}


		if(Rec_type == MAKE_FILE) {

			if (stream_state == 1) {
				continue;
			}

			bStrem = true;

			if (spi_th_flag) {
				spi_th_flag = false;
				if (clip_rec_state < REC_STOP){
					clip_rec_state = REC_STOP;
					box_snap = true;
				}
				// bExit = true;
				// pthread_join(tid_spi, NULL);
				ret = pthread_kill(tid_spi, 0);
				if (ret == 0) {
					pthread_cancel(tid_spi);
				}
				printf("Clip Rec Mode : %d\n", clip_rec_state);
			}

			if ((clip_rec_state == REC_WAIT && bell_rec_state == REC_WAIT) ||
				(clip_rec_state == REC_WAIT && bell_rec_state == REC_READY))
			{
				if (clip_rec_state == REC_WAIT) clip_rec_state = REC_MP4MAKE;
				if (bell_rec_state == REC_WAIT) bell_rec_state = REC_MP4MAKE;

				if (total_time < 23000000) {
					file_cnt = 1;
				}
				else if (total_time < 43000000) {
					file_cnt = 2;
				}
				else if (total_time >= 43000000) {
					file_cnt = 3;
				}
				printf("Detection End! REC END. file cnt : %d\n", file_cnt);

				if (total_time2 < 2000000){
					file_cnt2 = 0;
				}
				else if (total_time2 < 23000000) {
					file_cnt2 = 1;
				}
				else if (total_time2 < 43000000) {
					file_cnt2 = 2;
				}
				else if (total_time2 >= 43000000) {
					file_cnt2 = 3;
				}
				printf("Detection End! BELL END. file cnt : %d\n", file_cnt2);
			}
			else continue;

			while(box_snap);

			system("sync");

			make_file_start(REC);

			pthread_t tid_makefile;
			Make_File mfd1;
			mfd1.type = 0;
			mfd1.cnt = file_cnt;
			cfile_flag = false;
			ret = pthread_create(&tid_makefile, NULL, make_mp4_clip,(void*)&mfd1);
			if(ret != 0) {
				IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
				return -1;
			}
			if (file_cnt2 > 0) {
				Make_File mfd2;
				mfd2.type = 1;
				mfd2.cnt = file_cnt2;
				bfile_flag = false;
				ret = pthread_create(&tid_makefile, NULL, make_mp4_bell,(void*)&mfd2);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
					return -1;
				}
			}

			// if (main_rec_end && box_rec_end) {
			if ((clip_rec_state == REC_MP4MAKE && bell_rec_state == REC_MP4MAKE) ||
				(clip_rec_state == REC_MP4MAKE && bell_rec_state == REC_READY)) {

				int box_n=0, box_o=0, box_b=0;
				box_n = open("/dev/shm/box0.jpg", O_RDONLY);
	   			if (box_n == -1) {
	   				printf("File /dev/shm/box0.jpg Open Fail!\n");
	   			}
	   			else {
	   				struct stat file_info;
					if ( 0 > stat("/dev/shm/box0.jpg", &file_info)) {
    					printf("File Size Not Check!!\n");
    					close(box_n);
    					box_n = -1;
    				}
    				else {
	   					close(box_n);
	   				}
	   			}

	   			box_o = open("/tmp/mnt/sdcard/box_origin.jpg", O_RDONLY);
	   			if (box_o == -1) {
	   				printf("File /tmp/mnt/sdcard/box_origin.jpg Open Fail!\n");
	   			}
	   			else {
	   				struct stat file_info;
					if ( 0 > stat("/tmp/mnt/sdcard/box_origin.jpg", &file_info)) {
    					printf("File Size Not Check!!\n");
    					close(box_o);
    					box_o = -1;
    				}
    				else {
	   					close(box_o);
	   				}
	   			}

	   			box_b = open("/tmp/mnt/sdcard/box_before.jpg", O_RDONLY);
	   			if (box_b == -1) {
	   				printf("File /tmp/mnt/sdcard/box_before.jpg Open Fail!\n");
	   			}
	   			else {
	   				struct stat file_info;
					if ( 0 > stat("/tmp/mnt/sdcard/box_before.jpg", &file_info)) {
    					printf("File Size Not Check!!\n");
    					close(box_b);
    					box_b = -1;
    				}
    				else {
	   					close(box_b);
	   				}
	   			}

	   			char *before_img = "/tmp/mnt/sdcard/box_before.jpg";
				char *after_img  = "/dev/shm/box0.jpg";
				char *sistic_img = "/dev/shm/corimg1.jpg";
				char *orgin_img = "/tmp/mnt/sdcard/box_origin.jpg";
				int threshold = 80;
				// double bsim = 0.0;
				// double osim = 0.0;
				// double asim = 0.0;
				int64_t cv_stime = sample_gettimeus();
				int64_t cv_etime = 0;
				// if (box_n != -1 && box_o != -1 && box_b != -1) {

				if (box_n != -1 && box_b != -1) {
					#ifdef __BOX_ALGORITH__
						test_box_al();
					#endif
    				printf("box Sistic!\n");
    				ret = package_sistic(before_img, after_img);
					if(ret < 0) {
						printf("package_sistic Fail!\n");
						// return ret;
					}
					printf("box Find!\n");
					ret = package_find(sistic_img, after_img, threshold);
					if(ret < 0) {
						printf("package_find Fail!\n");
						// return ret;
					} 
	    			else {
        				printf("Box Count : %d\n", ret);
        				cv_etime = sample_gettimeus();
    					printf("CV Total Time:%lld\n", cv_etime-cv_stime);
    					if (ret > 0) {

    						#if 0
    						if (box_o != -1) {
			        			osim = calculateSimilarity(orgin_img, after_img);
			        		}
			        		else {
			        			osim = 0.1;
			        		}

							bsim = calculateSimilarity(orgin_img, before_img);

							// asim = calculateSimilarity(before_img, after_img);

							if (bsim == 0) bsim = 1.0;

    						if (osim < bsim) {
    							if (clip_cause_t.Major == CLIP_CAUSE_MOVE) {
									clip_cause_t.Major = CLIP_CAUSE_BOX;
									clip_cause_t.Minor = CLIP_BOX_DISAP;
									system("cp /dev/shm/box0.jpg /tmp/mnt/sdcard/box_origin2.jpg");
								}
							}
							else {
								if (clip_cause_t.Major == CLIP_CAUSE_MOVE) {
									clip_cause_t.Major = CLIP_CAUSE_BOX;
									clip_cause_t.Minor = CLIP_BOX_OCCUR;
								}
							}
							#else
							int sim_cnt = 0;

    						if (box_o != -1) {
			        			ret = calculateSimilarity2(orgin_img, after_img, (Simil_t2*)&osim_t);
			        			if (ret < 0) {
			        				printf("Original vs After similarity cal fail!\n");
			        			}

				        		ret = calculateSimilarity2(orgin_img, before_img, (Simil_t2*)&bsim_t);
				        		if (ret < 0) {
			        				printf("Original vs Before similarity cal fail!\n");
			        			}

			        			if (osim_t.bhattacharyya < 0.4 && bsim_t.bhattacharyya < 0.4) {

					        		if (osim_t.correl 			> bsim_t.correl) 		sim_cnt++;
					        		if (osim_t.chisqr 			< bsim_t.chisqr) 		sim_cnt+=2;
					        		if (osim_t.intersect 		> bsim_t.intersect) 	sim_cnt++;
					        		if (osim_t.bhattacharyya 	< bsim_t.bhattacharyya) sim_cnt+=2;
					        		if (osim_t.kl_div 			< bsim_t.kl_div) 		sim_cnt++;

					        		printf("similarity count : %d\n", sim_cnt);
					        		if (sim_cnt > 4) {
		    							if (clip_cause_t.Major == CLIP_CAUSE_MOVE) {
											clip_cause_t.Major = CLIP_CAUSE_BOX;
											clip_cause_t.Minor = CLIP_BOX_DISAP;
											system("cp /dev/shm/box0.jpg /tmp/mnt/sdcard/box_origin2.jpg");
										}
									}
									else {
										if (clip_cause_t.Major == CLIP_CAUSE_MOVE) {
											clip_cause_t.Major = CLIP_CAUSE_BOX;
											clip_cause_t.Minor = CLIP_BOX_OCCUR;
										}
									}
								}
								else {
									printf("Similarity Value Low -> Not Find!\n");
									clip_cause_t.Major = CLIP_CAUSE_BOX;
									clip_cause_t.Minor = CLIP_BOX_OCCUR;
								}
							}
			        		else {
			        			clip_cause_t.Major = CLIP_CAUSE_BOX;
								clip_cause_t.Minor = CLIP_BOX_OCCUR;
			        		}

							#endif

							// printf("osim:%f bsim:%f asim:%f cnt:%d\n", osim, bsim, asim, ret);
							// printf("osim:%f bsim:%f cnt:%d\n", osim, bsim, ret);
	    					if (!netwrok_busy) {
	        					if (Ready_Busy_Check()){
									printf("Box Send!\n");
									memset(file_path, 0, 64);
									sprintf(file_path, "/dev/shm/box_result.jpg");
									printf("box send!\n");
									
									spi_send_file(REC_BOX_ALM, file_path, 0, 0, 0);
								}
							}
						}
	    			}
	    		}

	    		system("cp /tmp/mnt/sdcard/box_before.jpg /tmp/mnt/sdcard/box_before2.jpg");
	    		system("cp /dev/shm/box0.jpg /tmp/mnt/sdcard/box_before.jpg");
	    		system("cp /dev/shm/box_result.jpg /tmp/mnt/sdcard/box_before3.jpg");

				if (face_snap == false) bStrem = true;

				// printf("MP4 Make!\n");

				// system("sync");

				// if (file_cnt > 0) {
				// 	#ifdef __H265__			
				// 		system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-0.h265 -c copy /dev/shm/main.mp4");
				// 		system("rm /dev/shm/stream-0.h265");
				// 		system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-3.h265 -c copy /dev/shm/box.mp4");
				// 		system("rm /dev/shm/stream-3.h265");
				// 	#else
				// 		system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-0.h264 -c copy /dev/shm/main.mp4");
				// 		system("rm /dev/shm/stream-0.h264");
				// 		system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-3.h264 -c copy /dev/shm/box.mp4");
				// 		system("rm /dev/shm/stream-3.h264");
				// 	#endif
				// 	// system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/stream-4.h264 -c copy /dev/shm/box.mkv");
				// 	for (int i=0; i<file_cnt; i++){
				// 		if (i == 0) {
				// 			memset(file_sep, 0, 100);
				// 			sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/main.mp4 -ss 0 -t 20 -c copy /dev/shm/main%d.mp4", i);
				// 			printf("%s\n", file_sep);
				// 			system(file_sep);
				// 			memset(file_sep, 0, 100);
				// 			sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/box.mp4 -ss 0 -t 20 -c copy /dev/shm/box%d.mp4", i);
				// 			// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/box.mkv -ss 0 -t 12 -c copy /dev/shm/box%d.mkv", i);
				// 			printf("%s\n", file_sep);
				// 			system(file_sep);
				// 		}
				// 		else {
				// 			memset(file_sep, 0, 100);
				// 			sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/main.mp4 -ss %d.4 -t 20 -c copy /dev/shm/main%d.mp4", (i*20)-1, i);
				// 			printf("%s\n", file_sep);
				// 			system(file_sep);
				// 			memset(file_sep, 0, 100);
				// 			sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/box.mp4 -ss %d.4 -t 20 -c copy /dev/shm/box%d.mp4", (i*20)-1, i);
				// 			// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/box.mkv -ss %d.4 -t 12 -c copy /dev/shm/box%d.mkv", (i*12)-1, i);
				// 			printf("%s\n", file_sep);
				// 			system(file_sep);
				// 		}
						
				// 	}
				// }

				// if (file_cnt2 > 0) {
				// 	#ifdef __H265__			
				// 		system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell-0.h265 -c copy /dev/shm/bell_m.mp4");
				// 		system("rm /dev/shm/bell-0.h265");
				// 		system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell-3.h265 -c copy /dev/shm/bell_b.mp4");
				// 		system("rm /dev/shm/bell-3.h265");
				// 	#else
				// 		system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell-0.h264 -c copy /dev/shm/bell_m.mp4");
				// 		system("rm /dev/shm/bell-0.h264");
				// 		system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell-3.h264 -c copy /dev/shm/bell_b.mp4");
				// 		system("rm /dev/shm/bell-3.h264");
				// 	#endif
				// 	// system("/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell-4.h264 -c copy /dev/shm/bell_b.mkv");
				// 	for (int i=0; i<file_cnt2; i++){
				// 		if (i == 0) {
				// 			memset(file_sep, 0, 100);
				// 			sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_m.mp4 -ss 0 -t 20 -c copy /dev/shm/bell_m%d.mp4", i);
				// 			printf("%s\n", file_sep);
				// 			system(file_sep);
				// 			memset(file_sep, 0, 100);
				// 			sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_b.mp4 -ss 0 -t 20 -c copy /dev/shm/bell_b%d.mp4", i);
				// 			// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_b.mkv -ss 0 -t 12 -c copy /dev/shm/bell_b%d.mkv", i);
				// 			printf("%s\n", file_sep);
				// 			system(file_sep);
				// 		}
				// 		else {
				// 			memset(file_sep, 0, 100);
				// 			sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_m.mp4 -ss %d.4 -t 20 -c copy /dev/shm/bell_m%d.mp4", (i*20)-1, i);
				// 			printf("%s\n", file_sep);
				// 			system(file_sep);
				// 			memset(file_sep, 0, 100);
				// 			sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_b.mp4 -ss %d.4 -t 20 -c copy /dev/shm/bell_b%d.mp4", (i*20)-1, i);
				// 			// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/bell_b.mkv -ss %d.4 -t 12 -c copy /dev/shm/bell_b%d.mkv", (i*12)-1, i);
				// 			printf("%s\n", file_sep);
				// 			system(file_sep);
				// 		}
						
				// 	}
				// }

				while (file_cnt > 0 && cfile_flag == false) ;
				while (file_cnt2 > 0 && bfile_flag == false) ;


				////// Fine Spi Send /////////
				if (file_cnt > 0 && cfile_flag) { 
					// uint32_t nowtime = 0;
					int save_cnt=0;
					char file_name[20];

					for (int i=0; i<file_cnt; i++) {
						sprintf(file_name, "main%d", i);
						
						if (!netwrok_busy) {
							if (Ready_Busy_Check()){
								printf("File %d-1 Start!\n", i+1);
								memset(file_path, 0, 64);
								sprintf(file_path, "/dev/shm/main%d.mp4", i);
								spi_send_file(REC_CLIP_F, file_path, 0, i+1, 1);
								}
							else {
								printf("Fail to Send %d-1\n", i+1);
							}
							
							if (Ready_Busy_Check()){
								printf("File %d-2 Start!\n", i+1);
								memset(file_path, 0, 64);
								sprintf(file_path, "/dev/shm/box%d.mp4", i);
								// sprintf(file_path, "/dev/shm/box%d.mkv", i);
								spi_send_file(REC_CLIP_B, file_path, 0, i+1, 2);
								}
							else {
								printf("Fail to Send %d-2\n", i+1);
							}
						}
						// else {
						// 	// nowtime = sample_gettimeus();
						// 	if (!ubi_flag) {
						// 		ubi_flag = true;
						// 		if (file_ck("/tmp/mnt/sdcard/nandformat")) {
						// 			system("ubi_mount");
						// 		}
						// 		else {
						// 			system("ubi_mk");
						// 			system("echo ubiformat > /tmp/mnt/sdcard/nandformat");
						// 		}
						// 	}
						// 	save_cnt = FileShow("/maincam", file_name);
						// 	printf("file cnt : %s.mp4 %d\n", file_name, save_cnt);

						// 	save_cnt++;
						// 	memset(file_path, 0, 128);
						// 	sprintf(file_path, "cp /dev/shm/main%d.mp4 /maincam/main%d_%d.mp4", i, i, save_cnt);
						// 	system(file_path);
						// 	sleep(1);
						// 	memset(file_path, 0, 128);
						// 	sprintf(file_path, "cp /dev/shm/box%d.mp4 /boxcam/box%d_%d.mp4", i, i, save_cnt);
						// 	system(file_path);
						// 	sleep(1);
						// }
						else {
							// nowtime = sample_gettimeus();
							if (!ubi_flag) {
								ubi_flag = true;
								if (file_ck("/tmp/mnt/sdcard/nandformat")) {
									system("ubi_mount");
								}
								else {
									system("ubi_mk");
									system("echo ubiformat > /tmp/mnt/sdcard/nandformat");
								}
							}
							// save_cnt = FileShow("/maincam", file_name);
							// printf("file cnt : %s.mp4 %d\n", file_name, save_cnt);

							TimeStamp.type[0] = clip_cause_t.Major;
							TimeStamp.type[1] = clip_cause_t.Minor;

							save_cnt++;
							memset(file_path, 0, 128);
							// sprintf(file_path, "cp /dev/shm/main%d.mp4 /maincam/main%d_%d.mp4", i, i, save_cnt);
							sprintf(file_path, "cp /dev/shm/main%d.mp4 /maincam/%s_%02d_00_%02x%02x.mp4", 
												i, TimeStamp.date, i+1, TimeStamp.type[0], TimeStamp.type[1]);
							system(file_path);
							sleep(1);
							memset(file_path, 0, 128);
							// sprintf(file_path, "cp /dev/shm/box%d.mp4 /boxcam/box%d_%d.mp4", i, i, save_cnt);
							sprintf(file_path, "cp /dev/shm/box%d.mp4 /boxcam/%s_%02d_01_%02x%02x.mp4", 
												i, TimeStamp.date, i+1, TimeStamp.type[0], TimeStamp.type[1]);
							system(file_path);
							sleep(1);
							old_file_del();
						}
					}
				}


				if (file_cnt2 > 0 && bfile_flag) { 
					int save_cnt=0;
					char file_name[20];

					if (temp_flag) {
						clip_cause_t.Major = CLIP_CAUSE_MOUNT;
						if (temp_unmount_flag)
							clip_cause_t.Minor = CLIP_MOUNT_DISMT;
						else
							clip_cause_t.Minor = CLIP_MOUNT_MOUNT;
					}
					else {
						clip_cause_t.Major = CLIP_CAUSE_BELL;
						if (bell_call_flag)
							clip_cause_t.Minor = CLIP_BELL_CALL;
						else
							clip_cause_t.Minor = CLIP_BELL_BELL;
					}

					for (int i=0; i<file_cnt2; i++) {
						sprintf(file_name, "bell_m%d", i);
						if (!netwrok_busy) {
							if (Ready_Busy_Check()){
								printf("File %d-1 Start!\n", i+1);
								memset(file_path, 0, 64);
								sprintf(file_path, "/dev/shm/bell_m%d.mp4", i);
								spi_send_file(REC_CLIP_F, file_path, 0, i+1, 1);
								}
							else {
								printf("Fail to Send %d-1\n", i+1);
							}
							
							if (Ready_Busy_Check()){
								printf("File %d-2 Start!\n", i+1);
								memset(file_path, 0, 64);
								sprintf(file_path, "/dev/shm/bell_b%d.mp4", i);
								// sprintf(file_path, "/dev/shm/box%d.mkv", i);
								spi_send_file(REC_CLIP_B, file_path, 0, i+1, 2);
								}
							else {
								printf("Fail to Send %d-2\n", i+1);
							}
						}
						// else {
						// 	// nowtime = sample_gettimeus();
						// 	if (!ubi_flag) {
						// 		ubi_flag = true;
						// 		if (file_ck("/tmp/mnt/sdcard/nandformat")) {
						// 			system("ubi_mount");
						// 		}
						// 		else {
						// 			system("ubi_mk");
						// 			system("echo ubiformat > /tmp/mnt/sdcard/nandformat");
						// 		}
						// 	}
						// 	save_cnt = FileShow("/maincam", file_name);
						// 	printf("file cnt : %s.mp4 %d\n", file_name, save_cnt);

						// 	save_cnt++;
						// 	memset(file_path, 0, 128);
						// 	sprintf(file_path, "cp /dev/shm/bell_m%d.mp4 /maincam/bell_m%d_%d.mp4", i, i, save_cnt);
						// 	system(file_path);
						// 	sleep(1);
						// 	memset(file_path, 0, 128);
						// 	sprintf(file_path, "cp /dev/shm/bell_b%d.mp4 /boxcam/bell_b%d_%d.mp4", i, i, save_cnt);
						// 	system(file_path);
						// 	sleep(1);
						// }
						else {
							// nowtime = sample_gettimeus();
							if (!ubi_flag) {
								ubi_flag = true;
								if (file_ck("/tmp/mnt/sdcard/nandformat")) {
									system("ubi_mount");
								}
								else {
									system("ubi_mk");
									system("echo ubiformat > /tmp/mnt/sdcard/nandformat");
								}
							}
							// save_cnt = FileShow("/maincam", file_name);
							// printf("file cnt : %s.mp4 %d\n", file_name, save_cnt);

							old_file_del();

							TimeStamp.type[0] = clip_cause_t.Major;
							TimeStamp.type[1] = clip_cause_t.Minor;

							save_cnt++;
							memset(file_path, 0, 128);
							// sprintf(file_path, "cp /dev/shm/main%d.mp4 /maincam/main%d_%d.mp4", i, i, save_cnt);
							sprintf(file_path, "cp /dev/shm/bell_m%d.mp4 /maincam/%s_%02d_00_%02x%02x.mp4", 
												i, TimeStamp.date, i+1, TimeStamp.type[0], TimeStamp.type[1]);
							system(file_path);
							sleep(1);
							memset(file_path, 0, 128);
							// sprintf(file_path, "cp /dev/shm/box%d.mp4 /boxcam/box%d_%d.mp4", i, i, save_cnt);
							sprintf(file_path, "cp /dev/shm/bell_b%d.mp4 /boxcam/%s_%02d_01_%02x%02x.mp4", 
												i, TimeStamp.date, i+1, TimeStamp.type[0], TimeStamp.type[1]);
							system(file_path);
							sleep(1);
							old_file_del();
						}
					}
				}

				system("sync");

				// system("rm /tmp/mnt/sdcard/mp4/*");

				// system("cp /dev/shm/*.mp4 /tmp/mnt/sdcard/mp4");

				device_end(REC);
				printf("File Send End!!\n");
				break;
			}
		}
	}while(1);

	bExit = true;
	bUart = true;
	bStrem = true;

	pthread_join(tim_osd, NULL);
	pthread_join(tid_stream, NULL);
	// pthread_join(tid_clip, NULL);
	pthread_join(tid_move, NULL);
	pthread_join(tid_fdpd, NULL);
	pthread_join(tid_snap, NULL);
	pthread_join(tid_uart, NULL);
	// pthread_join(tid_live, NULL);

	return 0;
}

int clip_total_fake(void) {
	// bool 		ubi_flag 		= false;
	char 		file_path[128] 	= {0};
	int 		ret 			= 0;
	int 		file_cnt 		= 0;

	//////////////// SPI Init ////////////////////////////////////////////////////////////////
	ret = spi_init();
    if(ret < 0){
        printf("spi init error\n");
        return 0;
    }
	//////////////////////////////////////////////////////////////////////////////////////////

	Rec_type = MAKE_FILE;

	do {
		if(Rec_type == MAKE_FILE) {

			bStrem = true;

			file_cnt = 3;

			////// Fine Spi Send /////////
			if (file_cnt > 0) { 
				// uint32_t nowtime = 0;
				// int save_cnt=0;
				char file_name[20];

				for (int i=0; i<file_cnt; i++) {
					sprintf(file_name, "main%d", i);
					
					if (!netwrok_busy) {
						if (Ready_Busy_Check()){
							printf("File %d-1 Start!\n", i+1);
							memset(file_path, 0, 64);
							sprintf(file_path, "/tmp/mnt/sdcard/mp4/main%d.mp4", i);
							spi_send_file(REC_CLIP_F, file_path, 0, i+1, 1);
							}
						else {
							printf("Fail to Send %d-1\n", i+1);
						}
						
						if (Ready_Busy_Check()){
							printf("File %d-2 Start!\n", i+1);
							memset(file_path, 0, 64);
							sprintf(file_path, "/tmp/mnt/sdcard/mp4/box%d.mp4", i);
							// sprintf(file_path, "/dev/shm/box%d.mkv", i);
							spi_send_file(REC_CLIP_B, file_path, 0, i+1, 1);
							}
					}
				}
			}

			system("sync");

			// system("rm /tmp/mnt/sdcard/mp4/*");

			// system("cp /dev/shm/*.mp4 /tmp/mnt/sdcard/mp4");

			device_end(REC);
			printf("File Send End!!\n");
			break;
		}
	}while(1);

	bExit = true;

	// pthread_join(tid_uart, NULL);

	return 0;
}


#define STREAMING_SPI

int stream_total(void) {
	int ret = 0;
	// int file_cnt = 0;
	// bool start_flag = false;
	// int64_t total_time = 0;
	// int64_t oldt_time = 0;
	// char file_path[64] = {0};
#ifdef __STREAMING_CMD__

	bool up_streming_flag = false;
    bool dn_streming_flag = false;

    bool adc_flag = false;
    bool led_flag = false;

    
    int gval = 0;
#endif
    // int64_t rec_time_s = 0;//, 
    int64_t rec_time_e = 0;
    int64_t rec_now = 0;
    int64_t dimming_e = 0;
    int dimming_val = 90;
    bool dimming_up = true;

    char file_sep[256] = {0};
    char file_path[128] = {0};

	pthread_t tid_ao, tid_ai;
    pthread_t tid_stream, tid_snap, tid_move, tim_osd, tid_fdpd;
    pthread_t tid_uart;//, tid_live;

    // pthread_t adc_thread_id, tid_clip;

    


#ifdef STREAMING_SPI
    pthread_t tid_spi;
	//////////////// SPI Init ////////////////////////////////////////////////////////////////
	ret = spi_init();
    if(ret < 0){
        printf("spi init error\n");
        return 0;
    }

    data_sel = 4;
	if (data_sel <= 0 || data_sel > 4) {
		printf("Invalid Type!\n");
		return -1;
	}
	ret = pthread_create(&tid_spi, NULL, spi_send_stream, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create spi_send_stream failed\n", __func__);
		return -1;
	}
	//////////////////////////////////////////////////////////////////////////////////////////
#else
	pthread_t tid_udp_out;
	sprintf(ip, "192.168.0.100");
	printf("ip:%s %d\n", ip, strlen(ip));
	ret = pthread_create(&tid_udp_out, NULL, udp_send_pthread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create udp_send_pthread failed\n", __func__);
		return -1;
	}
	//////////////////////////////////////////////////////////////////////////////////////////
#endif
    stream_state = 1;
    // clip_rec_state = REC_START;
	// rec_state = 0;
	// rec_stop = false;
	

#ifndef __TEST_FAKE_VEDIO__

	ret = pthread_create(&tim_osd, NULL, OSD_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create OSD_thread failed\n", __func__);
		return -1;
	}
				 
	ret = pthread_create(&tid_stream, NULL, get_video_stream_user_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_stream_user_thread failed\n", __func__);
		return -1;
	}

	// ret = pthread_create(&tid_clip, NULL, get_video_clip_user_thread, NULL);
	// if(ret != 0) {
	// 	IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_clip_user_thread failed\n", __func__);
	// 	return -1;
	// }

	ret = pthread_create(&tid_snap, NULL, get_snap_stream_user_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_snap_stream_user_thread failed\n", __func__);
		return -1;
	}

	ret = pthread_create(&tid_move, NULL, move_detecte_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create move_detecte_thread failed\n", __func__);
		return -1;
	}

	ret = pthread_create(&tid_fdpd, NULL, fdpd_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create fdpd_thread failed\n", __func__);
		return -1;
	}
#else

	ret = pthread_create(&tid_stream, NULL, get_video_stream_test_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_stream_user_thread failed\n", __func__);
		return -1;
	}
#endif			

	ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
		return -1;
	}

	ret = pthread_create(&tid_ai, NULL, IMP_Audio_Record_AEC_Thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
		return -1;
	}

	ret = pthread_create(&tid_uart, NULL, uart_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create uart_thread failed\n", __func__);
		return -1;
	}

	// ret = pthread_create(&tid_live, NULL, device_live_thread, NULL);
	// if(ret != 0) {
		// IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create device_live_thread failed\n", __func__);
		// return -1;
	// }

	// system("rm /tmp/mnt/sdcard/main*.jpg");
	// system("rm /tmp/mnt/sdcard/box*.jpg");

	if (settings.SF.bits.led) {
	    if (ExpVal > 10000) { // Noh Change 1000 -> 10000 20240530
	    	gpio_LED_Set(1);
	    }
	    else {
	    	gpio_LED_Set(0);
	    }
	}

	usleep(1000*1000);

	do {

#ifdef __STREAMING_CMD__
		// total_time = sample_gettimeus() - start_time;
		int cmd = 255; 

		printf("cmd 1  Audio Up Streaming Start!\n");
		printf("cmd 2  Audio Down Streaming test!\n");
		printf("cmd 3  Audio Effects test!\n");
		printf("cmd 4  Audio IN/Out Vol, Gain Set!\n");
		printf("cmd 5  Grid Test!\n");
		printf("cmd 6  snap shot!\n");
		printf("cmd 7  Rec Start!(60sec)\n");
		printf("cmd 8  PCM Save Start/End\n");
		printf("cmd 9  Box Camera Crop Test\n");
		printf("cmd 10 Distortion Test\n");
		printf("cmd 11 LED Test!\n");
		printf("cmd 12 adc test\n");
		printf("cmd 13 Box LED ON/OFF\n");
		printf("cmd 14 Moasic On/Off\n");
		printf("cmd 15 ISP integration time Test\n");
		printf("cmd 16 Dot Test\n");
		printf("cmd 17 Target Bit Change!\n");
		printf("cmd 18 streaming rec start\n");
		printf("cmd 19 streaming rec end\n");
		printf("cmd 20 streaming end & save file send\n");
		printf("cmd 21 Focus & Sharpness Test\n");
		printf("cmd 90 Reset Test\n");
		printf("cmd 99 : exit\n");

		cmd = scanf_cmd();
		ST_Flush();

		if (cmd == 1) {
			if (!up_streming_flag) {
				printf("cmd 1 Audio Up Streaming Start!\n");
				up_streming_flag = true;
   				ret = pthread_create(&tid_ai, NULL, IMP_Audio_Record_AEC_Thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
					return -1;
				}
			}
		}
		else if (cmd == 2) {
			if (!dn_streming_flag) {
				printf("cmd 2 Audio Down Streaming test!\n");
				dn_streming_flag = true;

	#ifdef STREAMING_SPI
				pthread_t tid_udp_in;
    			ret = pthread_create(&tid_udp_in, NULL, udp_recv_pthread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create udp_recv_pthread failed\n", __func__);
					return -1;
				}
	#endif
    			ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
					return -1;
				}
			}
		}
		else if (cmd == 3) {
			char* effect_file = NULL;
    		int effect_index = 255;
			printf("cmd 3 Audio Effects test!\n");
			printf("Effect #(0~6) : ");
			effect_index = scanf_index();
			if (effect_index >= 0 && effect_index < 7) {
				if (effect_index == 0) effect_file = "/tmp/mnt/sdcard/effects/end1c.wav";
				else if (effect_index == 1) effect_file = "/tmp/mnt/sdcard/effects/end2c.wav";
				else if (effect_index == 2) effect_file = "/tmp/mnt/sdcard/effects/end3c.wav";
				else if (effect_index == 3) effect_file = "/tmp/mnt/sdcard/effects/end4c.wav";
				else if (effect_index == 4) effect_file = "/tmp/mnt/sdcard/effects/start1c.wav";
				else if (effect_index == 5) effect_file = "/tmp/mnt/sdcard/effects/start2c.wav";
				else if (effect_index == 6) effect_file = "/tmp/mnt/sdcard/effects/start3c.wav";
			}
			else {
				printf("Invalid #!\n");
				continue;
			}
			printf("play : %s\n", effect_file);
			ao_file_play_thread(effect_file);
		}
		else if (cmd == 4) {
			printf("cmd 4 Audio IN/Out Vol, Gain Set!\n");
			Get_Vol();
			int ai_vol = 100, ai_gain = 20, ao_vol = 100, ao_gain = 20;
			printf("Audio In Vol:");
			ai_vol = scanf_index();
			if (ai_vol < 0 || ai_vol > 100) {
				printf("Invalid Value!!\n");
				continue;
			}
			printf("Audio In gain:");
			ai_gain = scanf_index();
			if (ai_gain < 0 || ai_gain > 30) {
				printf("Invalid Value!!\n");
				continue;
			}
			printf("Audio Out Vol:");
			ao_vol = scanf_index();
			if (ao_vol < 0 || ao_vol > 100) {
				printf("Invalid Value!!\n");
				continue;
			}
			printf("Audio Out gain:");
			ao_gain = scanf_index();
			if (ao_gain < 0 || ao_gain > 30) {
				printf("Invalid Value!!\n");
				continue;
			}

			printf("Set Ai vol:%d\n", ai_vol);
			printf("Set Ai gain:%d\n", ai_gain);
			printf("Set Ao vol:%d\n", ao_vol);
			printf("Set Ao gain:%d\n", ao_gain);
			Set_Vol(ai_vol, ai_gain, ao_vol, ao_gain);
		}
		else if (cmd == 5) {
			int grid;
			printf("cmd 5 Grid Test!\n");
			printf("Select Grid Number(0~%d):", GRID_COVER_INDEX);
			grid = scanf_index();

			if(!grid_cover_flag[grid]){
				grid_cover_flag[grid] = true;
			}
			else {
				grid_cover_flag[grid] = false;
			}
			osd_grid_show(grid);
		}
		else if (cmd == 6) {
			int snap;
			printf("cmd 6 Snap Shot Test!\n");
			printf("main : 1\n");
			printf("secoend : 2\n");
			snap = scanf_index();
			if (snap == 1) {
				main_snap = true;
			}
			else if (snap == 2) {
				box_snap = true;
			}
			else {
				printf("Invalid Selection!\n");
			}
		}
		else if (cmd == 7) {
			printf("cmd 7  Rec Start!(60sec)\n");
			start_time = sample_gettimeus();
			clip_rec_state = REC_START;
		}
		else if (cmd == 8) {
			printf("cmd 8 PCM Save Start/End\n");
			if (save_pcm == 0){
				printf("PCM Save Start!\n");
				save_pcm = 1;
			}
			else if(save_pcm == 1){
				printf("PCM Save End!\n");
				save_pcm = 2;
			}
		}
		else if (cmd == 9) {
			int x, y, w, h, c;
			printf("cmd 9 Box Camera Crop Test\n");
			printf("Set X:");
			x = scanf_index();
			printf("Set Y:");
			y = scanf_index();
			printf("Set Width:");
			w = scanf_index();
			printf("Set Height:");
			h = scanf_index();
			printf("Cam select[0:main/1:box]:");
			c = scanf_index();

			isd_crop(x, y, w, h, c);
		}
		else if (cmd == 10) {
			int x, y, w, h, str, c;
			printf("cmd 10 Distortion Test\n");
			printf("Set Center X:");
			x = scanf_index();
			printf("Set Center Y:");
			y = scanf_index();
			printf("Set Width:");
			w = scanf_index();
			printf("Set Height:");
			h = scanf_index();
			printf("Set strength:");
			str = scanf_index();
			printf("Cam select[0:main/1:box]:");
			c = scanf_index();

			isd_distortion(x, y, w, h, str, c);
		}
		else if (cmd == 11) {
			int led_duty = 0;
			printf("cmd 11 LED Test!\n");
			if (!led_flag) {
				system("echo 6 > /sys/class/pwm/pwmchip0/export");
				
				led_flag = true;
			}
			printf("Duty?");
			led_duty = scanf_index();
			system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
			memset(file_sep, 0, 100);
			sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
			printf(file_sep);
			printf("\n");
			system(file_sep);
			system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
			system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
		}
		// else if (cmd == 12) {
		// 	if (adc_flag == false) {
		// 		printf("cmd 12 adc test\n");
		// 		system("echo 54 > /sys/class/gpio/export");
		// 		system("echo out > /sys/class/gpio/gpio54/direction");
		// 		system("echo 1 > /sys/class/gpio/gpio54/value");
		// 		// /* get value thread */
		// 		adc_init();
				
		// 		adc_flag = true;
		// 	}
		// 	ret = pthread_create(&adc_thread_id, NULL, adc_get_voltage_thread, NULL);
		// 	if (ret != 0) {
		// 		printf("error: pthread_create error!!!!!!");
		// 		return -1;
		// 	}
    	// }
		else if (cmd == 13) {
			printf("cmd 13 Box LED ON/OFF\n");
			ret = gpio_set_val(PORTD+6, gval);
			if(ret < 0){
				printf("Fail set Value GPIO : %d\n", PORTD+6);
				return -1;
			}
			if (gval == 0) gval = 1;
			else gval = 0;
		}
		else if (cmd == 14) {
			printf("cmd 14 Moasic On/Off\n");
			if (!Mosaic_En) Mosaic_En = true;
			else Mosaic_En = false;
		}
		else if (cmd == 15) {
			uint32_t isp_igtime = 132;
			int getset = 0;
			printf("cmd 15 ISP integration time Test\n");
			printf("time Get/Set(0/1):");
			getset = scanf_index();
			if (getset > 0) {
				printf("Value:");
				isp_igtime = (uint32_t)scanf_index();
			}
			isp_igtime = isp_integration_time(getset, isp_igtime);
			
		}
		else if (cmd == 16) {
			printf("cmd 16 Dot Test\n");
			if (!dot_En) dot_En = true;
			else dot_En = false;
		}
		else if (cmd == 17) {
			uint32_t targetbit = 500;
			printf("cmd 17 Target Bit Change!\n");
			targetbit = scanf_index();
			Set_Target_Bit(targetbit);
		}
		else if (cmd == 18) {
			printf("cmd 18 streaming rec start\n");
			printf("Streaming Rec Start!\n");
            if (!rec_on) {
            	rec_time_s = sample_gettimeus();
                rec_cnt++;
                streaming_rec_state = REC_START;
            }
		}
		// else if (cmd == 19) {
		// 	printf("cmd 19 streaming rec end\n");
		// 	rec_time_e = sample_gettimeus()-rec_time_s;
		// 	printf("Rec Time : %lld\n", rec_time_e);
        //     streaming_rec_state = REC_STOP;
		// }
		else if (cmd == 20) {
			printf("cmd 20 streaming end & save file send\n");
			bExit = 1;
			bStrem = true;

			if (streaming_rec_state == REC_WAIT) streaming_rec_state = REC_MP4MAKE;
			else printf("streaming_rec_state Error:%d\n", streaming_rec_state);

			if (streaming_rec_state == REC_MP4MAKE) {
				for(int i=0; i<rec_cnt; i++) {
				#ifdef __H265__		
					memset(file_sep, 0, 256);
					sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec-0-%d.h265 -c copy /dev/shm/rec0_%d.mp4", i+1, i+1);
					system(file_sep);
					memset(file_sep, 0, 256);
					sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec-3-%d.h265 -c copy /dev/shm/rec1_%d.mp4", i+1, i+1);
					system(file_sep);
				#else
					memset(file_sep, 0, 256);
					sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec-0-%d.h264 -c copy /dev/shm/rec0_%d.mp4", i+1, i+1);
					system(file_sep);
					memset(file_sep, 0, 256);
					sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec-3-%d.h264 -c copy /dev/shm/rec1_%d.mp4", i+1, i+1);
					system(file_sep);
				#endif

					if (Ready_Busy_Check()){
						printf("rec0_%d.mp4 Start!\n", i+1);
						memset(file_path, 0, 128);
						sprintf(file_path, "/dev/shm/rec0_%d.mp4", i+1);
						spi_send_file(REC_STREAMING_M, file_path, 0, i+1, 1);
					}
					else {
						printf("Fail to Send rec0_%d.mp4\n", i+1);
					}
					
					if (Ready_Busy_Check()){
						printf("rec1_%d.mp4 Start!\n", i+1);
						memset(file_path, 0, 128);
						sprintf(file_path, "/dev/shm/rec1_%d.mp4", i+1);
						spi_send_file(REC_STREAMING_B, file_path, 0, i+1, 2);
					}
					else {
						printf("Fail to Send rec0_%d.mp4\n", i+1);
					}
				}
			}
			device_end(STREAMING);
			printf("Streaming Mode End!!\n");
			bUart = true;
			
		}
		else if (cmd == 21) {
			int snap, max_sharp = 0, max_foucs = 0;
			double avr_sharp, avr_focus;
			char file_name[128];
			Focus_Sharpness fs_t[10];

			printf("cmd 21 Focus & Sharpness Test\n");

			for (int q=0; q<10; q++) {
				main_snap = true;
				while (main_snap);

				memset(file_name , 0x00, 128);
				sprintf(file_name, "/dev/shm/main%d.jpg", q);
				// sharpness[q] = Sharpness_cal(file_name);
				ret = focus_and_sharpness_cal(file_name, (Focus_Sharpness2*)&fs_t[q]);
				// printf("now:%f max:%f\n", sharpness[q],  sharpness[max_sharp]);
				if (fs_t[q].sharpness > fs_t[max_sharp].sharpness){
					max_sharp = q;
					// printf("Max Q : %d\n", max_sharp);
				}

				if (fs_t[q].focus > fs_t[max_foucs].focus){
					max_foucs = q;
					// printf("Max Q : %d\n", max_sharp);
				}
			}

			avr_sharp = 0;
			avr_focus = 0;

			for (int q=0; q<10; q++){
				if (q != max_sharp) {
					avr_sharp += fs_t[q].sharpness;
					// printf("shapness add %d : %f\n", q, avr_sharp);
				}

				if (q != max_foucs) {
					avr_focus += fs_t[q].focus;
					// printf("focus add %d : %f\n", q, avr_focus);
				}
			}
			printf("Avrage Focus : %f\n", avr_focus/9);
			printf("Avrage Sharpness : %f\n", avr_sharp/9);
		}
		else if (cmd == 90) {
			printf("cmd 90 Reset Test\n");
			system("reboot");
		}
		else if (cmd == 99) {
			printf("Exiting Program! Plz Wait!\n");
			bExit = 1;
			bStrem = true;
			signal(SIGINT, handler);
			// break;
		}
#else
		if (dimming) {
			dimming_e =  sample_gettimeus() - dimming_s;
			if ((dimming_e > 30000) & dimming_up) {
				dimming_s = sample_gettimeus();
				dimming_val -= 1;
				LED_dimming (dimming_val);
				if (dimming_val <= 10) {
					dimming_up = false;
				}
			}
			else if ((dimming_e > 30000) & !dimming_up) {
				dimming_s = sample_gettimeus();
				dimming_val += 1;
				LED_dimming (dimming_val);
				if (dimming_val >= 90) {
					dimming_up = true;
				}
			}
		}
		
		if (streaming_rec_state >= REC_START && streaming_rec_state < REC_STOP) {
			rec_now = sample_gettimeus() - rec_time_s;
			if (rec_total + rec_now >= 60000000) {
				streaming_rec_state = REC_STOP;
            	rec_on = false;
            	rec_total += rec_time_s;
            	rec_mem_flag = true;
			}
		}
		if (rec_end) {
			if (streaming_rec_state == REC_START || streaming_rec_state == REC_ING) {
				rec_time_e = sample_gettimeus()-rec_time_s;
            	printf("Rec Time : %lld total : %lld\n", rec_time_e, rec_total);
            	rec_each_time[rec_cnt-1] = rec_time_e;
            	rec_total += rec_time_e;
            	streaming_rec_state = REC_STOP;
            	rec_on = false;
			}
			rec_end = false;
			printf("cmd 20 streaming end & save file send\n");
			bExit = 1;
			bStrem = true;

			while (streaming_rec_state != REC_WAIT);

			if (streaming_rec_state == REC_WAIT) streaming_rec_state = REC_MP4MAKE;
			else printf("streaming_rec_state Error:%d\n", streaming_rec_state);

			if (streaming_rec_state == REC_MP4MAKE) {
				int file_cnt_rec = 0;
				make_file_start(STREAMING);
				for(int i=0; i<rec_cnt; i++) {
				#ifdef __H265__
					memset(file_sep, 0, 256);
					sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec-0-%d.h265 -c copy /dev/shm/rec0_%d.mp4", i+1, i+1);
					system(file_sep);
					memset(file_sep, 0, 256);
					sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec-3-%d.h265 -c copy /dev/shm/rec1_%d.mp4", i+1, i+1);
					system(file_sep);
				#else
					memset(file_sep, 0, 256);
					sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec-0-%d.h264 -c copy /dev/shm/rec0_%d.mp4", i+1, i+1);
					system(file_sep);
					memset(file_sep, 0, 256);
					sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec-3-%d.h264 -c copy /dev/shm/rec3_%d.mp4", i+1, i+1);
					system(file_sep);
					sprintf(file_sep, "cp /dev/shm/rec0_%d.mp4 /tmp/mnt/sdcard/pcktest.mp4", i+1);
					system(file_sep);
				#endif

					if (rec_each_time[i] < 23000000)
						file_cnt_rec = 1;
					else if(rec_each_time[i] < 43000000)
						file_cnt_rec = 2;
					else
						file_cnt_rec = 3;


					for (int j=0; j<file_cnt_rec; j++) {

						if (j == 0) {
							memset(file_path, 0, 100);
							sprintf(file_path, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec0_%d.mp4 -ss 0 -t 20 -c copy /dev/shm/rec0_%d_%d.mp4", i+1, i+1, j);
							printf("%s\n", file_path);
							system(file_path);
							memset(file_path, 0, 100);
							sprintf(file_path, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec3_%d.mp4 -ss 0 -t 20 -c copy /dev/shm/rec3_%d_%d.mp4", i+1, i+1, j);
							printf("%s\n", file_path);
							system(file_path);
						}
						else {
							memset(file_path, 0, 100);
							sprintf(file_path, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec0_%d.mp4 -ss %d.4 -t 20 -c copy /dev/shm/rec0_%d_%d.mp4", i+1, (j*20)-1, i+1, j);
							printf("%s\n", file_path);
							system(file_path);
							memset(file_path, 0, 100);
							sprintf(file_path, "/tmp/mnt/sdcard/./ffmpeg -i /dev/shm/rec3_%d.mp4 -ss %d.4 -t 20 -c copy /dev/shm/rec3_%d_%d.mp4", i+1, (j*20)-1, i+1, j);
							printf("%s\n", file_path);
							system(file_path);
						}

						if (Ready_Busy_Check()){
							printf("rec0_%d_%d.mp4 Start!\n", i+1, j);
							memset(file_path, 0, 128);
							sprintf(file_path, "/dev/shm/rec0_%d_%d.mp4", i+1, j);
							spi_send_file(REC_STREAMING_M, file_path, i, j+1, 1);
							// spi_send_fake_file(REC_STREAMING_M);
						}
						else {
							printf("Fail to Send rec0_%d_%d.mp4\n", i+1, j);
						}
						
						if (Ready_Busy_Check()){
							printf("rec3_%d_%d.mp4 Start!\n", i+1, j);
							memset(file_path, 0, 128);
							sprintf(file_path, "/dev/shm/rec3_%d_%d.mp4", i+1, j);
							spi_send_file(REC_STREAMING_B, file_path, i, j+1, 2);
							// spi_send_fake_file(REC_STREAMING_B);
						}
						else {
							printf("Fail to Send rec3_%d_%d.mp4\n", i+1, j);
						}
					}
				}
				system("sync");
			}
			device_end(STREAMING);
			printf("Streaming Mode End!!\n");
			bUart = true;
			sleep(2);
			break;
		}

#endif
			
	}while(1);

	bExit = true;
	bUart = true;
	bStrem = true;

	pthread_join(tim_osd, NULL);
	pthread_join(tid_stream, NULL);
	// pthread_join(tid_clip, NULL);
	pthread_join(tid_move, NULL);
	pthread_join(tid_fdpd, NULL);
	pthread_join(tid_snap, NULL);

	pthread_join(tid_ai, NULL);
	pthread_join(tid_ao, NULL);
#ifdef STREAMING_SPI
	pthread_join(tid_spi, NULL);
#else
	pthread_join(tid_spi, NULL);
#endif
	pthread_join(tid_uart, NULL);
	// pthread_join(tid_live, NULL);


	return 0;
}


int Setting_Total(void) {
	int ret = 0;

	// pthread_t tid_ao, tid_ai;
    // pthread_t tid_stream, tid_clip, tid_snap, tid_move, tim_osd, tid_fdpd, adc_thread_id;
    pthread_t tid_uart, tid_snap, tid_ota;// ,tid_live;
    // pthread_t tid_spi;
    char file_path[64] = {0};

    bool door1=false, door2=false;
    bool cap_start = false, ubi_start = false;

    int64_t dimming_e = 0;
    int dimming_val = 90;
    bool dimming_up = true;
    bool ota_thread_flag = false;


#ifdef STREAMING_SPI
    // pthread_t tid_spi;
	// //////////////// SPI Init ////////////////////////////////////////////////////////////////
	// ret = spi_init();
    // if(ret < 0){
    //     printf("spi init error\n");
    //     return 0;
    // }

    // data_sel = 4;
	// if (data_sel <= 0 || data_sel > 4) {
	// 	printf("Invalid Type!\n");
	// 	return -1;
	// }
	// ret = pthread_create(&tid_spi, NULL, spi_send_stream, NULL);
	// if(ret != 0) {
	// 	IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create spi_send_stream failed\n", __func__);
	// 	return -1;
	// }
	//////////////////////////////////////////////////////////////////////////////////////////
#else
	// pthread_t tid_udp_out;
	// sprintf(ip, "192.168.0.100");
	// printf("ip:%s %d\n", ip, strlen(ip));
	// ret = pthread_create(&tid_udp_out, NULL, udp_send_pthread, NULL);
	// if(ret != 0) {
	// 	IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create udp_send_pthread failed\n", __func__);
	// 	return -1;
	// }
	//////////////////////////////////////////////////////////////////////////////////////////
#endif
    // stream_state = 1;
	// rec_state = 0;
	// rec_stop = false;
	

// 	ret = pthread_create(&tim_osd, NULL, OSD_thread, NULL);
// 	if(ret != 0) {
// 		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create OSD_thread failed\n", __func__);
// 		return -1;
// 	}
				 
// 	ret = pthread_create(&tid_stream, NULL, get_video_stream_user_thread, NULL);
// 	if(ret != 0) {
// 		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_stream_user_thread failed\n", __func__);
// 		return -1;
// 	}

// 	// ret = pthread_create(&tid_clip, NULL, get_video_clip_user_thread, NULL);
// 	// if(ret != 0) {
// 	// 	IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_clip_user_thread failed\n", __func__);
// 	// 	return -1;
// 	// }

// 	ret = pthread_create(&tid_snap, NULL, get_snap_stream_user_thread, NULL);
// 	if(ret != 0) {
// 		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_snap_stream_user_thread failed\n", __func__);
// 		return -1;
// 	}

// 	ret = pthread_create(&tid_move, NULL, move_detecte_thread, NULL);
// 	if(ret != 0) {
// 		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create move_detecte_thread failed\n", __func__);
// 		return -1;
// 	}

// 	ret = pthread_create(&tid_fdpd, NULL, fdpd_thread, NULL);
// 	if(ret != 0) {
// 		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create fdpd_thread failed\n", __func__);
// 		return -1;
// 	}
// #else

// 	ret = pthread_create(&tid_stream, NULL, get_video_stream_test_thread, NULL);
// 	if(ret != 0) {
// 		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_stream_user_thread failed\n", __func__);
// 		return -1;
// 	}
// #endif			

	// ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread, NULL);
	// if(ret != 0) {
	// 	IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
	// 	return -1;
	// }

	// ret = pthread_create(&tid_ai, NULL, IMP_Audio_Record_AEC_Thread, NULL);
	// if(ret != 0) {
	// 	IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
	// 	return -1;
	// }

	ret = pthread_create(&tid_uart, NULL, uart_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create uart_thread failed\n", __func__);
		return -1;
	}

	// ret = pthread_create(&tid_live, NULL, device_live_thread, NULL);
	// if(ret != 0) {
		// IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create device_live_thread failed\n", __func__);
		// return -1;
	// }



	// usleep(1000*1000);
	printf("Thread Start!\n");

	do {
		if (dimming) {
			dimming_e =  sample_gettimeus() - dimming_s;
			if ((dimming_e > 30000) & dimming_up) {
				dimming_s = sample_gettimeus();
				dimming_val -= 1;
				LED_dimming (dimming_val);
				if (dimming_val <= 10) {
					dimming_up = false;
				}
			}
			else if ((dimming_e > 30000) & !dimming_up) {
				dimming_s = sample_gettimeus();
				dimming_val += 1;
				LED_dimming (dimming_val);
				if (dimming_val >= 90) {
					dimming_up = true;
				}
			}
		}

		if (door_cap_flag){
			if (!cap_start) {
				printf("[door] cap init!\n");
				ret = spi_init();
    			if(ret < 0){
    			    printf("spi init error\n");
    			    return 0;
    			}

				ret = pthread_create(&tid_snap, NULL, get_snap_stream_user_thread, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_snap_stream_user_thread failed\n", __func__);
					return -1;
				}

				usleep(100*1000);

				main_snap = true;
				box_snap = true;
				door1 = door2 = true;
				cap_start = true;

			}

			if (!main_snap && door1 && cap_start) {
				usleep(500*1000);
				system("sync");
				if (Ready_Busy_Check()){
					printf("doorCap Send Start!\n");
					memset(file_path, 0, 128);
					sprintf(file_path, "/dev/shm/main0.jpg");
					spi_send_file(REC_DOOR_SNAP, file_path, 0, 0, 0);
				}
				else {
					printf("Fail to Send main0.jpg\n");
				}
				door1 = false;
			}

			if (!box_snap && door2) {
				printf("Box Origin File!!\n");
				system("cp /dev/shm/box0.jpg /tmp/mnt/sdcard/box_origin.jpg");
				system("cp /dev/shm/box0.jpg /tmp/mnt/sdcard/box_before.jpg");
				system("sync");
				door2 = false;
			}

			if (!door1 && !door2) {
				printf("[END] Door Shot!\n");
				device_end(SETTING);
				door_cap_flag = false;
			}
		}

		if (save_send_flag) {

			if (!ubi_start) {
				ret = spi_init();
    			if(ret < 0){
    			    printf("spi init error\n");
    			    return 0;
    			}
				system("ubi_mount");

				ubi_start = true;
			}

			usleep(500*1000);

			file_name_get(&Save_movie1, 0);
			file_name_get(&Save_movie2, 1);

			for(int i=0;i<Save_movie1.cnt;i++) {
				printf("File[%d]:%s\n", i, Save_movie1.name[i]);
				spi_send_save_file("/maincam/", Save_movie1.name[i]);
				printf("File[%d]:%s\n", i, Save_movie2.name[i]);
				spi_send_save_file("/boxcam/", Save_movie2.name[i]);
			}

			// system("rm /maincam/*");
			// system("rm /boxcam/*");

			system("sync");

			printf("[END] Save Send!\n");
			device_end(SETTING);
			save_send_flag = false;
		}
		if (ota_flag) {
			ret = pthread_create(&tid_ota, NULL, OTA_Thread, NULL);
			if(ret != 0) {
				IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create OTA_Thread failed\n", __func__);
				return -1;
			}
			ota_thread_flag = true;
			ota_flag = false;
		}

		if (cmd_end_flag) {
			system("sync");

			printf("[END] CMD Send!\n");
			device_end(SETTING);
			cmd_end_flag = false;
			break;
		}

			
	}while(1);

	bExit = true;
	bUart = true;
	bStrem = true;

	pthread_join(tid_uart, NULL);
	// pthread_join(tid_live, NULL);
	if (ota_thread_flag) pthread_join(tid_ota, NULL);

	return 0;
}

