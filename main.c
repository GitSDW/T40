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

	spk_vol_buf = 80;
	spk_gain_buf = 14;

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
	cfile_flag1 = false;
	cfile_flag2 = false;
	bfile_flag = false;
	bfile_flag1 = false;
	bfile_flag2 = false;
	dimming = false;
	ota_flag = false;
	audio_start_flag = false;
	file_21_flag = false;
	av_off_flag = false;
	audio_spi_flag = false;
	dn_g726_falg = false;
	get_audio = false;
	set_audio = false;
	bBLed = false;
	ao_clear_flag = false;
	bLive = false;
	bLiveFile = false;
	rebell = false;

	for(i=0;i<10;i++){
		fdpd_data[i].flag = false;
        fdpd_data[i].classid = 0;
        fdpd_data[i].ul_x = 0;
        fdpd_data[i].ul_y = 0;
        fdpd_data[i].br_x = 0;
        fdpd_data[i].br_y = 0;
        mosaic_time[i] = 0;
	}

	snprintf(TimeStamp.date, 12, "%s", date_buf);
	TimeStamp.type[0] = 1;
	TimeStamp.type[1] = 1;

	memset(ip, 0, 30);
	return 0;
}

int gpio_active_low(unsigned int pin) {
	int ret = -1;

	// dp("low active : %d\n", pin);

	ret = gpio_export(pin);	// Box LED Gpio
	if(ret < 0){
		dp("Fail Export GPIO : %d\n", pin);
		return -1;
	}

	ret = gpio_set_dir(pin, GPIO_OUTPUT, GPIO_HIGH);
	if(ret < 0){
		dp("Fail set dir GPIO : %d\n",pin);
		return -1;
	}

	return 0;
}

void *unused_pin_low(void *argc) {
	for (unsigned int k=0; k<128; k++) {
			
		if (
				// Port A
				k == PORTA+4 || k == PORTA+17 || k == PORTA+18 || k == PORTA+21 || k == PORTA+22 ||	
				k == PORTA+23 || k == PORTA+24 || k == PORTA+25 || k == PORTA+26 || k == PORTA+27 || k == PORTA+28 ||
				// Port B
				k == PORTB+0 || k == PORTB+1 || k == PORTB+2 || k == PORTB+3 || k == PORTB+4 || k == PORTB+5 ||
				k == PORTB+17 || k == PORTB+18 || k == PORTB+19 || k == PORTB+21 || k == PORTB+22 || k == PORTB+23 || 
				k == PORTB+24 || k == PORTB+25 || k == PORTB+26 || k == PORTB+27 || k == PORTB+28 || k == PORTB+29 || 
				k == PORTB+30 || k == PORTB+31 ||
				// Port C
				k == PORTC+0 || k == PORTC+1 || k == PORTC+2 || k == PORTC+3 || k == PORTC+4 || k == PORTC+5 ||
				k == PORTC+12 || k == PORTC+27 || k == PORTC+28 || k == PORTC+29 || k == PORTC+30 ||
				// Port D
				k == PORTD+6 || k == PORTD+21 || k == PORTD+22 || k >= PORTD+27
			) 
			{
				continue;
			}
		gpio_active_low(k) ;
	}
}

int gpio_init(void) {
	int ret = 0;

	ret = gpio_export(PORTB+17);	// Clip Gpio
	if(ret < 0){
		dp("Fail Export GPIO : %d\n", PORTB+17);
		return -1;
	}

	ret = gpio_set_dir(PORTB+17, GPIO_INPUT, GPIO_LOW);
	if(ret < 0){
		dp("Fail get dir GPIO : %d\n", PORTB+17);
		return -1;
	}



	ret = gpio_export(PORTB+18);	// Ready Busy Gpio
	if(ret < 0){
		dp("Fail Export GPIO : %d\n", PORTB+17);
		return -1;
	}

	ret = gpio_set_dir(PORTB+18, GPIO_INPUT, GPIO_LOW);
	if(ret < 0){
		dp("Fail get dir GPIO : %d\n", PORTB+17);
		return -1;
	}



	ret = gpio_export(PORTB+19);	// Stream Gpio
	if(ret < 0){
		dp("Fail Export GPIO : %d\n", PORTB+19);
		return -1;
	}

	ret = gpio_set_dir(PORTB+19, GPIO_INPUT, GPIO_LOW);
	if(ret < 0){
		dp("Fail get dir GPIO : %d\n", PORTB+19);
		return -1;
	}



	ret = gpio_export(PORTD+6);	// Box LED Gpio
	if(ret < 0){
		dp("Fail Export GPIO : %d\n", PORTD+6);
		// return -1;
	}

	ret = gpio_set_dir(PORTD+6, GPIO_OUTPUT, GPIO_HIGH);
	// ret = gpio_set_dir(PORTD+6, GPIO_OUTPUT, GPIO_LOW);
	if(ret < 0){
		dp("Fail get dir GPIO : %d\n", PORTD+6);
		return -1;
	}

	ret = gpio_set_val(PORTD+6, 1);
	if(ret < 0){
		dp("Fail set Value GPIO : %d\n", PORTD+6);
		return -1;
	}


	// 
	ret = gpio_export(PORTD+21);	// Box LED Gpio
	if(ret < 0){
		dp("Fail Export GPIO : %d\n", PORTD+21);
		// return -1;
	}

	ret = gpio_set_dir(PORTD+21, GPIO_OUTPUT, GPIO_LOW);
	// ret = gpio_set_dir(PORTD+6, GPIO_OUTPUT, GPIO_LOW);
	if(ret < 0){
		dp("Fail get dir GPIO : %d\n", PORTD+21);
		return -1;
	}

	// ret = gpio_export(PORTB+31);	// Box LED Gpio
	// if(ret < 0){
	// 	dp("Fail Export GPIO : %d\n", PORTB+19);
	// 	return -1;
	// }

	// ret = gpio_set_dir(PORTB+31, GPIO_OUTPUT, GPIO_LOW);
	// if(ret < 0){
	// 	dp("Fail get dir GPIO : %d\n", PORTB+31);
	// 	return -1;
	// }

	// ret = gpio_set_val(PORTB+31, 1);
	// if(ret < 0){
	// 	dp("Fail set Value GPIO : %d\n", PORTB+31);
	// 	return -1;
	// }
	 
	pthread_t tid_gpio;
	ret = pthread_create(&tid_gpio, NULL, unused_pin_low, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[GPIO]", "[ERROR] %s: pthread_create unused_pin_low failed\n", __func__);
		return -1;
	}

	return 0;
}

bool en_ck = false;

void amp_on(void) {
	int ret = -1;

	if (!en_ck) {
		ret = gpio_set_val(PORTD+21, 1);
		if(ret < 0){
			dp("Fail set Value GPIO : %d\n", PORTD+21);
		}

		// Set_Vol(90,25,spk_vol_buf,spk_gain_buf);
		en_ck = true;
		dp("Set AMP On!\n");
	}
}

void amp_off(void) {
	int ret = -1;

	if (en_ck) {
		ret = gpio_set_val(PORTD+21, 0);
		if(ret < 0){
			dp("Fail set Value GPIO : %d\n", PORTD+21);
		}

		// Set_Vol(90,25,spk_vol_buf,spk_gain_buf);
		en_ck = false;
		dp("Set AMP On!\n");
	}
}


int gpio_deinit(void) {
	int ret = 0;

	ret = gpio_unexport(PORTB+17);
	if(ret < 0){
		dp("Fail Unexport GPIO : %d\n", PORTB+17);
		return -1;
	}

	ret = gpio_unexport(PORTB+18);
	if(ret < 0){
		dp("Fail Unexport GPIO : %d\n", PORTB+18);
		return -1;
	}

	ret = gpio_unexport(PORTB+19);
	if(ret < 0){
		dp("Fail Unexport GPIO : %d\n", PORTB+19);
		return -1;
	}

	ret = gpio_unexport(PORTD+6);
	if(ret < 0){
		dp("Fail Unexport GPIO : %d\n", PORTD+6);
		return -1;
	}

	return 0;
}

int start_up_mode(void){
	int gpio_917_0 = 0, gpio_917_1 = 1;

	gpio_917_0 = gpio_get_val(PORTB+17);
	if(gpio_917_0 < 0){
		dp("Fail get val GPIO : %d\n", PORTB+17);
		return -1;
	}
	gpio_917_1 = gpio_get_val(PORTB+19);
	if(gpio_917_1 < 0){
		dp("Fail get val GPIO : %d\n", PORTB+19);
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
		system("echo 100000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 1000*(led_duty));
		dp(file_sep);
		dp("\n");
		system(file_sep);
		system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
	}
	else if (onoff == 1) {
		led_duty = 0;
		system("echo 100000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 1000*(led_duty));
		dp(file_sep);
		dp("\n");
		system(file_sep);
		system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
		// system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
	}
	else {
		led_duty = 20;
		system("echo normal > /sys/class/pwm/pwmchip0/pwm6/polarity");
		system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		system("echo 100000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 1000*(led_duty));
		dp(file_sep);
		dp("\n");
		system(file_sep);
		// system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
		
		dimming = true;
	}

	return 0;
}

int LED_dimming (int duty) {
	int led_duty = 0;
	char file_sep[100] = {0};

	led_duty = duty;
	system("echo 100000 > /sys/class/pwm/pwmchip0/pwm6/period");
	memset(file_sep, 0, 100);
	sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 500*(led_duty));
	// dp(file_sep);
	// dp("\n");
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
		

		led_duty = 99;
		system("echo inversed > /sys/class/pwm/pwmchip0/pwm6/polarity");
		system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
		dp(file_sep);
		dp("\n");
		system(file_sep);
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");

		ret = gpio_set_val(PORTD+6, 0);
		if(ret < 0){
			dp("Fail set Value GPIO : %d\n", PORTD+6);
			return -1;
		}

		light_on = true;
	}
	else if (onoff == 2) {
		led_duty = 10;
		system("echo inversed > /sys/class/pwm/pwmchip0/pwm6/polarity");
		system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
		dp(file_sep);
		dp("\n");
		system(file_sep);
		system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
		// ret = gpio_set_val(PORTD+6, 1);
		// if(ret < 0){
		// 	dp("Fail set Value GPIO : %d\n", PORTD+6);
		// 	return -1;
		// }

		light_on = false;
	}
	else {
		
		ret = gpio_set_val(PORTD+6, 1);
		if(ret < 0){
			dp("Fail set Value GPIO : %d\n", PORTD+6);
			return -1;
		}

		system("echo 0 > /sys/class/pwm/pwmchip0/pwm6/enable");
		led_duty = 0;
		system("echo inversed > /sys/class/pwm/pwmchip0/pwm6/polarity");
		system("echo 1000000 > /sys/class/pwm/pwmchip0/pwm6/period");
		memset(file_sep, 0, 100);
		sprintf(file_sep, "echo %d > /sys/class/pwm/pwmchip0/pwm6/duty_cycle", 10000*(led_duty));
		dp(file_sep);
		dp("\n");
		system(file_sep);
		
		system("echo 1 > /sys/class/pwm/pwmchip0/pwm6/enable");
		



		light_on = false;
	}

	return 0;
}




void func_reboot(void) {
	dp("Reboot Test\n");
	system("reboot");
}


void handler(int sig){
	if(sig == SIGINT) {
		dp("Exit Programe!!\n");
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
	char file_sep[128] = {0};
	int raw1, raw2, pack1, pack2;

	Make_File *mfd =(Make_File*)argc;
	type = mfd->type;
	file_cnt = mfd-> cnt;

	dp("[%s] Type : %d Cnt : %d\n", __func__, type, file_cnt);

	int main_filed = 0, bottom_filed = 0;

	main_filed = open("/dev/shm/stream-0.h264", O_RDONLY);
    if (main_filed < 0) {
        dp("Fail Main Clip!!\n");
        return (void*) 0;
    }
    else {
    	close(main_filed);
    }
    
    raw1 = file_size_get("/dev/shm/stream-0.h264");

    bottom_filed = open("/dev/shm/stream-3.h264", O_RDONLY);
    if (bottom_filed < 0) {
        dp("Fail bottom Clip!!\n");
        return (void*) 0;
    }
    else {
    	close(bottom_filed);
    }

    raw2 = file_size_get("/dev/shm/stream-3.h264");
    if (file_cnt == 1) {
    	#ifdef __H265__			
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-0.h265 -ss 0 -t 20 -c copy /dev/shm/main0.mp4");
			system("rm /dev/shm/stream-0.h265");
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-3.h265 -ss 0 -t 20 -c copy /dev/shm/box0.mp4");
			system("rm /dev/shm/stream-3.h265");
		#else
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-0.h264 -ss 0 -t 20 -c copy /dev/shm/main0.mp4");
			pack1 = file_size_get("/dev/shm/main0.mp4");
			system("rm /dev/shm/stream-0.h264");
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-3.h264 -ss 0 -t 20 -c copy /dev/shm/box0.mp4");
			pack2 = file_size_get("/dev/shm/box0.mp4");
			system("rm /dev/shm/stream-3.h264");
		#endif

		dp("main : raw : %d pack : %d delta : %d\n", raw1, pack1, pack1-raw1);
		dp("bott : raw : %d pack : %d delta : %d\n", raw2, pack2, pack2-raw2);

		cfile_flag = true;
    }
	else if (file_cnt > 0 && type == 0) {
		#ifdef __H265__			
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-0.h265 -c copy /dev/shm/main.mp4");
			system("rm /dev/shm/stream-0.h265");
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-3.h265 -c copy /dev/shm/box.mp4");
			system("rm /dev/shm/stream-3.h265");
		#else
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-0.h264 -c copy /dev/shm/main.mp4");
			pack1 = file_size_get("/dev/shm/main.mp4");
			system("rm /dev/shm/stream-0.h264");
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-3.h264 -c copy /dev/shm/box.mp4");
			pack2 = file_size_get("/dev/shm/box.mp4");
			system("rm /dev/shm/stream-3.h264");
		#endif

		dp("main : raw : %d pack : %d delta : %d\n", raw1, pack1, pack1-raw1);
		dp("bott : raw : %d pack : %d delta : %d\n", raw2, pack2, pack2-raw2);
		// system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-4.h264 -c copy /dev/shm/box.mkv");
		for (int i=0; i<file_cnt; i++){
			if (i == 0) {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/main.mp4 -ss 0 -t 20 -c copy /dev/shm/main%d.mp4", i);
				// dp("%s\n", file_sep);
				system(file_sep);
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/box.mp4 -ss 0 -t 20 -c copy /dev/shm/box%d.mp4", i);
				// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/box.mkv -ss 0 -t 12 -c copy /dev/shm/box%d.mkv", i);
				// dp("%s\n", file_sep);
				system(file_sep);
			}
			else {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/main.mp4 -ss %d.4 -t 20 -c copy /dev/shm/main%d.mp4", (i*20)-1, i);
				// dp("%s\n", file_sep);
				system(file_sep);
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/box.mp4 -ss %d.4 -t 20 -c copy /dev/shm/box%d.mp4", (i*20)-1, i);
				// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/box.mkv -ss %d.4 -t 12 -c copy /dev/shm/box%d.mkv", (i*12)-1, i);
				// dp("%s\n", file_sep);
				system(file_sep);
			}
		}
		cfile_flag = true;
	}
	return (void*) 0;
}

void *make_mp4_clip1(void *argc) {
	int type, file_cnt;
	char file_sep[128] = {0};
	int raw1, pack1;

	Make_File *mfd =(Make_File*)argc;
	type = mfd->type;
	file_cnt = mfd-> cnt;

	dp("[%s] Type : %d Cnt : %d\n", __func__, type, file_cnt);

	int main_filed = 0;

	main_filed = open("/dev/shm/stream-0.h264", O_RDONLY);
    if (main_filed < 0) {
        dp("Fail Main Clip!!\n");
        return (void*) 0;
    }
    else {
    	close(main_filed);
    }
    
    raw1 = file_size_get("/dev/shm/stream-0.h264");

    if (file_cnt == 1) {
    	#ifdef __H265__			
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-0.h265 -ss 0 -t 20 -c copy /dev/shm/main0.mp4");
			system("rm /dev/shm/stream-0.h265");
		#else
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-0.h264 -ss 0 -t 20 -c copy /dev/shm/main0.mp4");
			pack1 = file_size_get("/dev/shm/main0.mp4");
			system("rm /dev/shm/stream-0.h264");
		#endif

		dp("main : raw : %d pack : %d delta : %d\n", raw1, pack1, pack1-raw1);

		cfile_flag1 = true;
    }
	else if (file_cnt > 0 && type == 0) {
		#ifdef __H265__			
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-0.h265 -c copy /dev/shm/main.mp4");
			system("rm /dev/shm/stream-0.h265");
		#else
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-0.h264 -c copy /dev/shm/main.mp4");
			pack1 = file_size_get("/dev/shm/main.mp4");
			system("rm /dev/shm/stream-0.h264");
		#endif

		dp("main : raw : %d pack : %d delta : %d\n", raw1, pack1, pack1-raw1);
		// system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-4.h264 -c copy /dev/shm/box.mkv");
		for (int i=0; i<file_cnt; i++){
			if (i == 0) {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/main.mp4 -ss 0 -t 20 -c copy /dev/shm/main%d.mp4", i);
				// dp("%s\n", file_sep);
				system(file_sep);
			}
			else {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/main.mp4 -ss %d.4 -t 20 -c copy /dev/shm/main%d.mp4", (i*20)-1, i);
				// dp("%s\n", file_sep);
				system(file_sep);
			}
		}
		cfile_flag1 = true;
	}
	return (void*) 0;
}

void *make_mp4_clip2(void *argc) {
	int type, file_cnt;
	char file_sep[128] = {0};
	int raw2, pack2;

	Make_File *mfd =(Make_File*)argc;
	type = mfd->type;
	file_cnt = mfd-> cnt;

	dp("[%s] Type : %d Cnt : %d\n", __func__, type, file_cnt);

	int bottom_filed = 0;

    bottom_filed = open("/dev/shm/stream-3.h264", O_RDONLY);
    if (bottom_filed < 0) {
        dp("Fail bottom Clip!!\n");
        return (void*) 0;
    }
    else {
    	close(bottom_filed);
    }

    raw2 = file_size_get("/dev/shm/stream-3.h264");

    if (file_cnt == 1) {
    	#ifdef __H265__			
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-3.h265 -ss 0 -t 20 -c copy /dev/shm/box0.mp4");
			system("rm /dev/shm/stream-3.h265");
		#else
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-3.h264 -ss 0 -t 20 -c copy /dev/shm/box0.mp4");
			pack2 = file_size_get("/dev/shm/box0.mp4");
			system("rm /dev/shm/stream-3.h264");
		#endif

		dp("bott : raw : %d pack : %d delta : %d\n", raw2, pack2, pack2-raw2);

		cfile_flag2 = true;
    }
	else if (file_cnt > 0 && type == 0) {
		#ifdef __H265__			
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-3.h265 -c copy /dev/shm/box.mp4");
			system("rm /dev/shm/stream-3.h265");
		#else
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/stream-3.h264 -c copy /dev/shm/box.mp4");
			pack2 = file_size_get("/dev/shm/box.mp4");
			system("rm /dev/shm/stream-3.h264");
		#endif

		dp("bott : raw : %d pack : %d delta : %d\n", raw2, pack2, pack2-raw2);

		for (int i=0; i<file_cnt; i++){
			if (i == 0) {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/box.mp4 -ss 0 -t 20 -c copy /dev/shm/box%d.mp4", i);
				// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/box.mkv -ss 0 -t 12 -c copy /dev/shm/box%d.mkv", i);
				// dp("%s\n", file_sep);
				system(file_sep);
			}
			else {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/box.mp4 -ss %d.4 -t 20 -c copy /dev/shm/box%d.mp4", (i*20)-1, i);
				// dp("%s\n", file_sep);
				system(file_sep);
			}
		}
		cfile_flag2 = true;
	}
	return (void*) 0;
}

void *make_mp4_bell(void *argc) {
	int type, file_cnt;
	char file_sep[128] = {0};

	Make_File *mfd =(Make_File*)argc;
	type = mfd->type;
	file_cnt = mfd-> cnt;

	dp("[%s] Type : %d Cnt : %d\n", __func__, type, file_cnt);

	if (file_cnt > 0 && type == 1) {
		#ifdef __H265__			
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/bell-0.h265 -c copy /dev/shm/bell_m.mp4");
			system("rm /dev/shm/bell-0.h265");
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/bell-3.h265 -c copy /dev/shm/bell_b.mp4");
			system("rm /dev/shm/bell-3.h265");
		#else
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/bell-0.h264 -c copy /dev/shm/bell_m.mp4");
			system("rm /dev/shm/bell-0.h264");
			system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/bell-3.h264 -c copy /dev/shm/bell_b.mp4");
			system("rm /dev/shm/bell-3.h264");
		#endif
		// system("/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/bell-4.h264 -c copy /dev/shm/bell_b.mkv");
		for (int i=0; i<file_cnt; i++){
			if (i == 0) {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/bell_m.mp4 -ss 0 -t 20 -c copy /dev/shm/bell_m%d.mp4", i);
				// dp("%s\n", file_sep);
				system(file_sep);
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/bell_b.mp4 -ss 0 -t 20 -c copy /dev/shm/bell_b%d.mp4", i);
				// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/bell_b.mkv -ss 0 -t 12 -c copy /dev/shm/bell_b%d.mkv", i);
				// dp("%s\n", file_sep);
				system(file_sep);
			}
			else {
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/bell_m.mp4 -ss %d.4 -t 20 -c copy /dev/shm/bell_m%d.mp4", (i*20)-1, i);
				// dp("%s\n", file_sep);
				system(file_sep);
				memset(file_sep, 0, 100);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/bell_b.mp4 -ss %d.4 -t 20 -c copy /dev/shm/bell_b%d.mp4", (i*20)-1, i);
				// sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/bell_b.mkv -ss %d.4 -t 12 -c copy /dev/shm/bell_b%d.mkv", (i*12)-1, i);
				// dp("%s\n", file_sep);
				system(file_sep);
			}
		}
		bfile_flag = true;
	}

	return (void*) 0;
}

void *make_mp4_streamrec(void *argc) {
	int type, file_cnt, index;
	char file_sep[128] = {0};
    char file_path[128] = {0};

	Make_File *mfd =(Make_File*)argc;
	type = mfd->type;
	file_cnt = mfd->cnt;
	index = mfd->index;

	dp("[%s] Type : %d Cnt : %d index : %d\n", __func__, type, file_cnt, index);

	if (file_cnt > 0) {
		if (file_cnt == 1) {
			#ifdef __H265__
				memset(file_sep, 0, 256);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec-0-%d.h265 -ss 0 -t 20 -c copy /dev/shm/rec0_%d_0.mp4", index+1, index+1);
				system(file_sep);
				memset(file_sep, 0, 256);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec-3-%d.h265 -ss 0 -t 20 -c copy /dev/shm/rec3_%d_0.mp4", index+1, index+1);
				system(file_sep);
			#else
				memset(file_sep, 0, 256);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec-0-%d.h264 -ss 0 -t 20 -c copy /dev/shm/rec0_%d_0.mp4", index+1, index+1);
				system(file_sep);
				memset(file_sep, 0, 256);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec-3-%d.h264 -ss 0 -t 20 -c copy /dev/shm/rec3_%d_0.mp4", index+1, index+1);
				system(file_sep);
			#endif
			rfile_flag[index] = true;
		}
		else {
			#ifdef __H265__
				memset(file_sep, 0, 256);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec-0-%d.h265 -c copy /dev/shm/rec0_%d.mp4", index+1, index+1);
				system(file_sep);
				memset(file_sep, 0, 256);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec-3-%d.h265 -c copy /dev/shm/rec1_%d.mp4", index+1, index+1);
				system(file_sep);
			#else
				memset(file_sep, 0, 256);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec-0-%d.h264 -c copy /dev/shm/rec0_%d.mp4", index+1, index+1);
				system(file_sep);
				memset(file_sep, 0, 256);
				sprintf(file_sep, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec-3-%d.h264 -c copy /dev/shm/rec3_%d.mp4", index+1, index+1);
				system(file_sep);
			#endif

			for (int j=0; j<file_cnt; j++) {
				if (j == 0) {
					memset(file_path, 0, 100);
					sprintf(file_path, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec0_%d.mp4 -ss 0 -t 20 -c copy /dev/shm/rec0_%d_%d.mp4", index+1, index+1, j);
					// dp("%s\n", file_path);
					system(file_path);
					memset(file_path, 0, 100);
					sprintf(file_path, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec3_%d.mp4 -ss 0 -t 20 -c copy /dev/shm/rec3_%d_%d.mp4", index+1, index+1, j);
					// dp("%s\n", file_path);
					system(file_path);
				}
				else {
					memset(file_path, 0, 100);
					sprintf(file_path, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec0_%d.mp4 -ss %d.4 -t 20 -c copy /dev/shm/rec0_%d_%d.mp4", index+1, (j*20)-1, index+1, j);
					// dp("%s\n", file_path);
					system(file_path);
					memset(file_path, 0, 100);
					sprintf(file_path, "/tmp/mnt/sdcard/./ffmpeg -y -loglevel quiet -i /dev/shm/rec3_%d.mp4 -ss %d.4 -t 20 -c copy /dev/shm/rec3_%d_%d.mp4", index+1, (j*20)-1, index+1, j);
					// dp("%s\n", file_path);
					system(file_path);
				}
			}
			rfile_flag[index] = true;
		}
	}

	return (void*) 0;
}

void *AV_Off(void *argc) {
	dp("b\n");
	bStrem = true;
	// usleep(50*1000);
	video_deinit();
	// Deinit_Audio_In();
	dp("a\n");

	return (void*) 0;
}


int clip_total(void);
int clip_total_fake(void);
int stream_total(int mode);
int Setting_Total(void);
void *dimming_test(void *argc);
void *bottom_led_test(void *argc);
extern void *sample_soft_photosensitive_ctrl(void *p);

int main(int argc, char **argv) {
	int ret = 0, mode = 0;
    // bool camera_test_flag = false;
    // bool adc_flag = false;
    // int gval = 0;
    // #ifdef __TEST_CMD__
    // 	bool ao_init = false;
    // #endif
    int64_t realvedio_time = 0;
    bool realvedio_flag = false;

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
		dp("Fail GPIO Init\n");
		return -1;
	}
    // pthread_t tid_ao, tid_ai;//, tid_aio_aec;
    // pthread_t tid_udp_in, tid_udp_out, tid_spi;
    // pthread_t tid_stream, tid_snap, tid_move, tim_osd, tid_fdpd, adc_thread_id;
    
    dp("Ver : %s.%s.%s\n", MAJOR_VER, MINOR_VER, CAHR_VER);

    if (settings.SF.bits.move_ex){
 		move_flag = true;
 		move_det_xs = settings.move_ex_s_x;
		move_det_ys = settings.move_ex_s_y;
		move_det_xe = settings.move_ex_e_x+1;
		move_det_ye = settings.move_ex_e_y+1;
		// dp("Move Ex : %d %d %d %d\n", move_det_xs, move_det_ys, move_det_xe, move_det_ye);
    }

    Set_Vol(90,25,spk_vol_buf,spk_gain_buf);



    // isd_crop(400, 460, 1100, 620, 1);

    // isd_crop(400, 450, 1120, 630, 1);

    // isd_crop(416, 468, 1088, 612, 1);

    isd_crop(408, 459, 1104, 612, 1);

    boot_mode = mode = start_up_mode();
    // dp("Mode : %d\n", mode);
    // mode = 2;
    if (mode == 1){
    	ret = clip_total();
    	if(ret < 0){
        	dp("Clip Mode error\n");
        	return 0;
    	}
    	else if (ret == 2) {
    		ret = stream_total(ret);
	    	if(ret < 0){
	    		dp("Strem Mode Error\n");
	    		return 0;
	    	}
    	}
    }
    else if(mode == 2){
    	dp("Streming Mode!!\n");
    	ret = stream_total(0);
    	if(ret < 0){
    		dp("Strem Mode Error\n");
    		return 0;
    	}
    }
    else if(mode == 3){
    	dp("Setting Mode!!\n");
    	Setting_Total();
    }



    pthread_t tid_uart, tid_snap;
    ret = pthread_create(&tid_uart, NULL, uart_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create uart_thread failed\n", __func__);
		return -1;
	}

	ret = pthread_create(&tid_snap, NULL, get_snap_stream_user_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_snap_stream_user_thread failed\n", __func__);
		return -1;
	}

	while (!bExit && (mode == 0 || mode == 3))
	{

		#ifdef __TEST_CMD__
			int cmd = 255;
			// int jpg_index = 255;

			dp("cmd 1  Sound Test.\n");
			dp("cmd 3  Get Brightness!\n");
			dp("cmd 4  SPI Test!!\n");
			dp("cmd 5  DIMMING Test!!\n");
			dp("cmd 6  Bottom LED Test!!\n");
			dp("cmd 7  Sound Test For 1KHz.\n");
			dp("cmd 8  MIC Test!!\n");
			dp("cmd 9  RealTime Stream(Vedio)!!\n");
			dp("cmd 10 Vedio Image clarity!!\n");
			
			dp("cmd 90 Reset Test\n");
			dp("cmd 99 exit\n");

			cmd = scanf_cmd();
			ST_Flush();
			
			if (cmd == 1) {
				int ao_vol, ao_gain, ao_index;
				int play_cnt = 1;
				char* effect_file = NULL;

				dp("cmd 1  Sound Test.\n");

				dp("Test Audio Out Volume(0~120) : ");
				ao_vol = scanf_index();
				if (ao_vol < 0 || ao_vol > 120) {
					dp("Volume Set Fail!!\n");
					continue;
				}
				dp("Test Audio Out Gain(0~31) : ");
				ao_gain = scanf_index();
				if (ao_gain < 0 || ao_gain > 31) {
					dp("Gain Set Fail!!\n");
					continue;
				}
				

				dp("Test Audio Number(1~5) : ");
				ao_index = scanf_index();
				if (ao_index < 1 || ao_index > 5) {
					dp("Audio Index Set Fail!!\n");
					continue;
				}

				dp("Number og Repeat Play : ");
				play_cnt = scanf_index();
				if (play_cnt < 1) play_cnt = 1;

				if (ao_index == 1) effect_file = "/tmp/mnt/sdcard/effects/test1.wav";
	            else if (ao_index == 2) effect_file = "/tmp/mnt/sdcard/effects/test2.wav";
	            else if (ao_index == 3) effect_file = "/tmp/mnt/sdcard/effects/test3.wav";
	            else if (ao_index == 4) effect_file = "/tmp/mnt/sdcard/effects/test4.wav";
	            else if (ao_index == 5) effect_file = "/tmp/mnt/sdcard/effects/test5.wav";
	            dp("play : %s\n", effect_file);
	            for (int a=0; a<play_cnt; a++) {
	            	Set_Vol(90,25,ao_vol,ao_gain);
	            	ao_file_play_thread(effect_file);
	            }
	            dp("Audio Out Test End!!\n");
				continue;
			}
			else if (cmd == 3) {
				dp("cmd 3  Get Brightness!\n");
				uint16_t black_brt, white_brt;

				// dp("Start Brightness : %d\n", ExpVal);

				black_brt = Get_Brightness();

				dp("Get Brightness : %d\n", black_brt);

				usleep(500*1000);

				gpio_LED_Set(1);

				usleep(500*1000);

				// dp("Start Brightness : %d\n", ExpVal);

				white_brt = Get_Brightness();

				dp("Get Brightness : %d\n", white_brt);

				usleep(500*1000);

				gpio_LED_Set(0);

				brightness_ack(black_brt, white_brt);
				
			}
			else if (cmd == 4) {
				dp("cmd 4  SPI Test!!\n");
			}
			else if (cmd == 5) {
				dp("cmd 5  DIMMING Test!!\n");
				static pthread_t tid_dimming;
				int index = 0;
				dp("Dimming On/Off(1/0) : ");
				index = scanf_index();
				if (index == 0) {
					if (!bDimming) {
						bDimming = true;
						pthread_join(tid_dimming, NULL);
					}
				}
				else if (index == 1) {
					bDimming = false;
					ret = pthread_create(&tid_dimming, NULL, dimming_test, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create dimming_test failed\n", __func__);
						return -1;
					}
				}
				else {
					dp("Invalid Index\n");
				}
			}
			else if (cmd == 6) {
				dp("cmd 6  Bottom LED Test!!\n");
				static pthread_t tid_BLed;
				int index = 0;
				dp("Bottom LED On/Off(1/0) : ");
				index = scanf_index();
				if (index == 0) {
					if (!bBLed) {
						bBLed = true;
						pthread_join(tid_BLed, NULL);
					}
				}
				else if (index == 1) {
					bBLed = false;
					ret = pthread_create(&tid_BLed, NULL, bottom_led_test, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create bottom_led_test failed\n", __func__);
						return -1;
					}
				}
				else {
					dp("Invalid Index\n");
				}
			}
			else if (cmd == 7) {
				char* effect_file = NULL;

				dp("cmd 7  Sound Test For 1KHz.\n");

				Set_Vol(90,25,spk_vol_buf,spk_gain_buf);

				effect_file = "/tmp/mnt/sdcard/effects/test6.wav";
	            
	            dp("play : %s\n", effect_file);
	            ao_file_play_thread(effect_file);
	            dp("Audio Out Test End!!\n");
				continue;
			}
			else if (cmd == 8) {
				static bool get_record = false;
				dp("cmd 8  MIC Test!!\n");
				IMP_Audio_Test_InOut_Thread();
			}
			else if (cmd == 9) {
				dp("cmd 9  RealTime Stream(Vedio)!!\n");
			}
			else if (cmd == 10) {
				dp("cmd 10 Focus & Sharpness Test\n");
				int max_sharp = 0, max_foucs = 0;
				double avr_sharp, avr_focus;
				char file_name[128];
				Focus_Sharpness fs_t[10];
				uint16_t sh_m, fo_m, sh_b, fo_b;

				for (int q=0; q<10; q++) {
					main_snap = true;
					while (main_snap);

					memset(file_name , 0x00, 128);
					sprintf(file_name, "/dev/shm/main%d.jpg", q);
					// sharpness[q] = Sharpness_cal(file_name);
					ret = focus_and_sharpness_cal(file_name, (Focus_Sharpness2*)&fs_t[q]);
					// dp("now:%f max:%f\n", sharpness[q],  sharpness[max_sharp]);
					if (fs_t[q].sharpness > fs_t[max_sharp].sharpness){
						max_sharp = q;
						// dp("Max Q : %d\n", max_sharp);
					}

					if (fs_t[q].focus > fs_t[max_foucs].focus){
						max_foucs = q;
						// dp("Max Q : %d\n", max_sharp);
					}
				}

				avr_sharp = 0;
				avr_focus = 0;

				for (int q=0; q<10; q++){
					if (q != max_sharp) {
						avr_sharp += fs_t[q].sharpness;
						// dp("shapness add %d : %f\n", q, avr_sharp);
					}

					if (q != max_foucs) {
						avr_focus += fs_t[q].focus;
						// dp("focus add %d : %f\n", q, avr_focus);
					}
				}
				dp("Avrage Focus : %f\n", avr_focus/9);
				dp("Avrage Sharpness : %f\n", avr_sharp/9);
				fo_m = doubleToUnsignedShort(avr_focus/9);
				sh_m = doubleToUnsignedShort(avr_sharp*10/9);
				dp("F:%d S:%d\n", fo_m, sh_m);

				for (int q=0; q<10; q++) {
					box_snap = true;
					while (box_snap);

					memset(file_name , 0x00, 128);
					sprintf(file_name, "/dev/shm/box%d.jpg", q);
					// sharpness[q] = Sharpness_cal(file_name);
					ret = focus_and_sharpness_cal(file_name, (Focus_Sharpness2*)&fs_t[q]);
					// dp("now:%f max:%f\n", sharpness[q],  sharpness[max_sharp]);
					if (fs_t[q].sharpness > fs_t[max_sharp].sharpness){
						max_sharp = q;
						// dp("Max Q : %d\n", max_sharp);
					}

					if (fs_t[q].focus > fs_t[max_foucs].focus){
						max_foucs = q;
						// dp("Max Q : %d\n", max_sharp);
					}
				}

				avr_sharp = 0;
				avr_focus = 0;

				for (int q=0; q<10; q++){
					if (q != max_sharp) {
						avr_sharp += fs_t[q].sharpness;
						// dp("shapness add %d : %f\n", q, avr_sharp);
					}

					if (q != max_foucs) {
						avr_focus += fs_t[q].focus;
						// dp("focus add %d : %f\n", q, avr_focus);
					}
				}
				dp("Avrage Focus : %f\n", avr_focus/9);
				dp("Avrage Sharpness : %f\n", avr_sharp/9);
				fo_b = doubleToUnsignedShort(avr_focus/9);
				sh_b = doubleToUnsignedShort(avr_sharp*10/9);
				dp("F:%d S:%d\n", fo_b, sh_b);
			}
			else if (cmd == 90) {
				dp("cmd 90 Reset Test\n");
				system("reboot");
			}
			else if (cmd == 99) {
				dp("Exiting Program! Plz Wait!\n");
				bExit = 1;
				bStrem = true;
				signal(SIGINT, handler);
				// break;
			}
			else {
				dp("Not Defined Cmd!\n");
			}
			dp("cmd:%d\n", cmd);
			
			// if (save_pcm == 2) {
			// 	dp("pcm cp %d %d\n", pcm_in);
			// 	if (pcm_in){
			// 		save_pcm = 3;
			// 		system("cp /dev/shm/test_in.pcm /tmp/mnt/sdcard/effects");
			// 		system("cp /dev/shm/test_out.pcm /tmp/mnt/sdcard/effects");
			// 		system("sync");
			// 		dp("pcm Test Copy end!\n");
			// 	}
			// }
		#else
			pthread_t tid_dimming, tid_spi, tid_stream, tid_move, tid_fdpd;
			int max_sharp = 0, max_foucs = 0;
			double avr_sharp, avr_focus;
			char file_name[128];
			Focus_Sharpness fs_t[10];
			uint16_t sh_m, fo_m, sh_b, fo_b;
			uint16_t black_brt, white_brt;

			if (brt_st_stat > 0) {
				if (brt_st_stat == 1) {
					dp("cmd 3  Get Brightness!\n");
					// uint32_t get_expval;

					// dp("Start Brightness : %d\n", ExpVal);

					usleep(3000*1000);

					black_brt = Get_Brightness();

					dp("Get Brightness : %d\n", black_brt);

					usleep(500*1000);

					gpio_LED_Set(1);

					usleep(500*1000);

					// dp("Start Brightness : %d\n", ExpVal);

					usleep(3000*1000);

					white_brt = Get_Brightness();

					dp("Get Brightness : %d\n", white_brt);

					usleep(500*1000);

					gpio_LED_Set(0);

					brt_st_stat = 2;
				}
				else if (brt_st_stat == 2) {
					// printf("b:%d w:%d\n", black_brt, white_brt);
					brightness_ack(black_brt, white_brt);
					brt_st_stat = 0;
				}
			}

			if (dim_st_stat > 0) {
				if (dim_st_stat == 3) {
					if (!bDimming) {
						bDimming = true;
						pthread_join(tid_dimming, NULL);
						dim_st_stat = 0;
					}
				}
				else if (dim_st_stat == 1) {
					dp("cmd 5  Dimming Test!!\n");
					bDimming = false;
					ret = pthread_create(&tid_dimming, NULL, dimming_test, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create dimming_test failed\n", __func__);
						return -1;
					}
					dim_st_stat = 2;
				}
				// else {
					// dp("Invalid Index\n");
				// }
			}

			pthread_t tid_BLed;
			if (bled_st_stat > 0) {
				if (bled_st_stat == 3) {
					if (!bBLed) {
						bBLed = true;
						pthread_join(tid_BLed, NULL);
						bled_st_stat = 0;
					}
				}
				else if (bled_st_stat == 1) {
					dp("cmd 6  Bottom LED Test!!\n");
					bBLed = false;
					ret = pthread_create(&tid_BLed, NULL, bottom_led_test, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create bottom_led_test failed\n", __func__);
						return -1;
					}
					bled_st_stat = 2;
				}
				// else {
					// dp("Invalid Index\n");
				// }
			}

			if (mic_st_stat > 0) {
				dp("cmd 8  MIC Test!!\n");
				if (mic_st_stat == 1) {
					IMP_Audio_Test_InOut_Thread();
					mic_st_stat = 0;
				}
			}

			if (video_st_stat > 0) {

				if (video_st_stat == 1) {
					gpio_LED_Set(1);

					ret = spi_init();
				    if(ret < 0){
				        dp("spi init error\n");
				        return 0;
				    }

				    data_sel = 4;
					if (data_sel <= 0 || data_sel > 4) {
						dp("Invalid Type!\n");
						return -1;
					}

					ret = pthread_create(&tid_spi, NULL, spi_send_stream, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create spi_send_stream failed\n", __func__);
						return -1;
					}

					ret = pthread_create(&tid_stream, NULL, get_video_stream_user_thread, NULL);
					if(ret != 0) {
						IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_stream_user_thread failed\n", __func__);
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
					stream_state = 1;
					video_st_stat = 0;
					realvedio_time = sample_gettimeus();


				}
			}
			if (polling_err_cnt > 2 && !realvedio_flag) {
				realvedio_flag = true;
			}
			if ( (realvedio_time != 0) && (sample_gettimeus()-realvedio_time) > 2000000 ) {
				realvedio_time = 0;
				uint8_t reval;
				if (realvedio_flag) reval = 0;
				else 				reval = 1;
				realvedio_ack(reval);
			}

			if (shfo_st_stat > 0) {
				static int try_cnt = 0;

				if (shfo_st_stat == 1) {
					gpio_LED_Set(1);
					// ret = gpio_set_val(PORTD+6, 0);
				    // if(ret < 0) {
					// 	dp("Fail set Value GPIO : %d\n", PORTD+6);
					// 	return ((void*)0);
					// }
					dp("cmd 10 Focus & Sharpness Test\n");
					memset(fs_t, 0, sizeof(Focus_Sharpness));
					for (int q=0; q<10; q++) {
						main_snap = true;
						while (main_snap);

						memset(file_name , 0x00, 128);
						sprintf(file_name, "/dev/shm/main%d.jpg", q+(try_cnt*10));
						// sharpness[q] = Sharpness_cal(file_name);
						ret = focus_and_sharpness_cal(file_name, (Focus_Sharpness2*)&fs_t[q]);
						// dp("now:%f max:%f\n", sharpness[q],  sharpness[max_sharp]);
						if (fs_t[q].sharpness > fs_t[max_sharp].sharpness){
							max_sharp = q;
							// dp("Max Q : %d\n", max_sharp);
						}

						if (fs_t[q].focus > fs_t[max_foucs].focus){
							max_foucs = q;
							// dp("Max Q : %d\n", max_sharp);
						}
					}

					avr_sharp = 0;
					avr_focus = 0;

					for (int q=0; q<10; q++){
						if (q != max_sharp) {
							avr_sharp += fs_t[q].sharpness;
							// dp("shapness add %d : %f\n", q, avr_sharp);
						}

						if (q != max_foucs) {
							avr_focus += fs_t[q].focus;
							// dp("focus add %d : %f\n", q, avr_focus);
						}
					}
					dp("Avrage Focus : %f\n", avr_focus/9);
					dp("Avrage Sharpness : %f\n", avr_sharp/9);
					fo_m = doubleToUnsignedShort(avr_focus/9);
					sh_m = doubleToUnsignedShort(avr_sharp*10/9);
					dp("F:%d S:%d\n", fo_m, sh_m);

					memset(fs_t, 0, sizeof(Focus_Sharpness));
					for (int q=0; q<10; q++) {
						box_snap = true;
						while (box_snap);

						memset(file_name , 0x00, 128);
						sprintf(file_name, "/dev/shm/box%d.jpg", q+(try_cnt*10));
						// sharpness[q] = Sharpness_cal(file_name);
						ret = focus_and_sharpness_cal(file_name, (Focus_Sharpness2*)&fs_t[q]);
						// dp("now:%f max:%f\n", sharpness[q],  sharpness[max_sharp]);
						if (fs_t[q].sharpness > fs_t[max_sharp].sharpness){
							max_sharp = q;
							// dp("Max Q : %d\n", max_sharp);
						}

						if (fs_t[q].focus > fs_t[max_foucs].focus){
							max_foucs = q;
							// dp("Max Q : %d\n", max_sharp);
						}
					}

					avr_sharp = 0;
					avr_focus = 0;

					for (int q=0; q<10; q++){
						if (q != max_sharp) {
							avr_sharp += fs_t[q].sharpness;
							// dp("shapness add %d : %f\n", q, avr_sharp);
						}

						if (q != max_foucs) {
							avr_focus += fs_t[q].focus;
							// dp("focus add %d : %f\n", q, avr_focus);
						}
					}
					dp("Avrage Focus : %f\n", avr_focus/9);
					dp("Avrage Sharpness : %f\n", avr_sharp/9);
					fo_b = doubleToUnsignedShort(avr_focus/9);
					sh_b = doubleToUnsignedShort(avr_sharp*10/9);
					dp("F:%d S:%d\n", fo_b, sh_b);
					shfo_st_stat = 2;
				}
				else if (shfo_st_stat == 2) {
					// gpio_LED_dimming(1);
					// ret = gpio_set_val(PORTD+6, 1);
				    // if(ret < 0) {
					// 	dp("Fail set Value GPIO : %d\n", PORTD+6);
					// 	return ((void*)0);
					// }
					try_cnt++;
					gpio_LED_Set(0);
					sharp_focus_ack(fo_m, sh_m, fo_b, sh_b);
					shfo_st_stat = 0;
				}
			}
		#endif 
	}

	sleep(1);
	
	// pthread_join(tid_ai, NULL);
	// pthread_join(tid_spi, NULL);
	// pthread_join(tim_osd, NULL);
	// pthread_join(tid_udp_in, NULL);
	// pthread_join(tid_ao, NULL);
	// pthread_join(tid_stream, NULL);
	// pthread_join(tid_fdpd, NULL);

	// if (adc_flag){
	// 	pthread_join(adc_thread_id, NULL);
	// 	adc_deinit();
	// }

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
	bool box_send_flag = false;
	bool face_send_flag = false;
	
	int64_t end_time = 0, total_time = 0;
	int64_t start_time2 = 0, end_time2 = 0, total_time2 = 0;
	int64_t start_time3 = 0, total_time3 = 0;
	int64_t forced_live = 0;
	// char file_path[128] = {0};
	char file_sep[100] = {0};
	int file_each_cnt[10] = {0};

	uint8_t major_buf1, major_buf2;

	int64_t rec_time_e = 0;
    int64_t rec_now = 0;
    int64_t retry_time = 0;

	pthread_t tid_ao, tid_ai;
    pthread_t tid_stream, tid_snap, tid_move, tim_osd, tid_fdpd;
    pthread_t tid_uart, tid_live;
    pthread_t tid_vmod;
    pthread_t tid_dimming;

    int64_t make_start = 0;

    int64_t dimming_e = 0;
    int dimming_val = 40;
    bool dimming_up = true;
    bool amp_f = false;
    bool redimming = false;


    // Init_Audio_Out();
	// Init_Audio_In();
	// Set_Vol(100, 20, 80, 15);

    pthread_t tid_spi;
	//////////////// SPI Init ////////////////////////////////////////////////////////////////
	ret = spi_init();
    if(ret < 0){
        dp("spi init error\n");
        return 0;
    }

    data_sel = 4;
	if (data_sel <= 0 || data_sel > 4) {
		dp("Invalid Type!\n");
		return -1;
	}
	ret = pthread_create(&tid_spi, NULL, spi_send_stream, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create spi_send_stream failed\n", __func__);
		return -1;
	}
				
	//////////////////////////////////////////////////////////////////////////////////////////

    thumbnail_state = 0;
    stream_state = 0;
	clip_rec_state = REC_START;
	av_off_flag = true;
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

	ret = pthread_create(&tid_live, NULL, device_live_thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create device_live_thread failed\n", __func__);
		return -1;
	}

	// ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread, NULL);
	// if(ret != 0) {
	// 	IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
	// 	return -1;
	// }

	ret = pthread_create(&tid_ai, NULL, IMP_Audio_Record_AEC_Thread, NULL);
	if(ret != 0) {
		IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
		return -1;
	}

	if (settings.SF.bits.backlight) {
		ret = pthread_create(&tid_vmod, NULL, sample_soft_photosensitive_ctrl, NULL);
		if(ret != 0) {
			IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create sample_soft_photosensitive_ctrl failed\n", __func__);
			return -1;
		}
	}

	// usleep(100*1000);

	
	do {
		if (!amp_f && stream_state == 1) {
			amp_f = true;
			amp_on();
		}

		if (get_audio && !set_audio) {
			set_audio = true;
			if (dn_g726_falg) {
				dp("Set G726 Audo Setting!! But PCM!!\n");
				// ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread_g726, NULL);
				// if(ret != 0) {
				// 	IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
				// 	return -1;
				// }
				ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread_pcm, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
					return -1;
				}
			}
			else {
				dp("Set PCM Audo Setting!!\n");
				ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread_pcm, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
					return -1;
				}
			}
		}

		if (start_flag == false) {
		#ifndef __PHILL_REQ__
			if ((face_cnt > 0) || (main_motion_detect > 1) || bell_flag || temp_flag) {
			// if ((face_cnt > 0) || (person_cnt > 0) || (main_motion_detect > 1) || bell_flag || temp_flag) {
			// if ((main_motion_detect > 1) || bell_flag || temp_flag) {
				dp("fc : %d Motion : %d bell : %d temp : %d\n", face_cnt, main_motion_detect, bell_flag, temp_flag);
				dp("Start REC!!\n");
				start_flag = true;
				roaming_person = false;
				// start_time = sample_gettimeus();
				clip_cause_t.Major = CLIP_CAUSE_MOVE;
				clip_cause_t.Minor = CLIP_MOVE_MOVE;
				Rec_type = CLIP_REC;
				end_time = start_time + START_CHECK_TIME;
				ExpVal = Get_Brightness();
				if (settings.SF.bits.led) {
				    if (ExpVal > 30000) { // Noh Change 1000 -> 10000 20240530 -> 70000 20240712
			    		gpio_LED_Set(1);
				    }
				    else {
			    		gpio_LED_Set(2);
				    }
				}
				else {
					gpio_LED_Set(2);
				}
			}
			else if ((sample_gettimeus() - start_time) > START_CHECK_TIME) {
				// dp("Main_Motion : %d\n", main_motion_detect);
				device_end(STREAMING);
				dp("Not Detection!! Device Turn Off.\n");
				break;
			}
			else if (stream_state == 1) {
				dp("Streaming Mode Change!\n");
				clip_rec_state = REC_STOP;
				boot_mode = 2;
				ret = stream_total(2);
		    	if(ret < 0){
		    		dp("Strem Mode Error\n");
		    		return 0;
		    	}
		    	break;
			}
		#else
			dp("Start REC!!\n");
			start_flag = true;
			roaming_person = false;
			// start_time = sample_gettimeus();
			clip_cause_t.Major = CLIP_CAUSE_MOVE;
			clip_cause_t.Minor = CLIP_MOVE_MOVE;
			Rec_type = CLIP_REC;
			end_time = start_time + START_CHECK_TIME;
			ExpVal = Get_Brightness();
			if (settings.SF.bits.led) {
			    if (ExpVal > 70000) { // Noh Change 1000 -> 10000 20240530 -> 70000 20240712
		    		gpio_LED_Set(1);
			    }
			    else {
		    		gpio_LED_Set(2);
			    }
			}
			else {
				gpio_LED_Set(2);
			}
		#endif
		}
		else {
			if (Rec_type == CLIP_REC){
				total_time = sample_gettimeus() - start_time;

				if((fr_state == FR_WAIT) &&
					(thumbnail_state == THUMB_WAIT || thumbnail_state == THUMB_END)) {
					dp("Thumb state : %d\n", thumbnail_state);
					fr_state++;
				}
				else if(fr_state == FR_SNAPSHOT) {
					fr_state++;
					dp("Face Data Send!!\n");
					ret = facecrop_make(facial_data);
					if (ret < 0 && fpdp_cnt < 5) {
						dp("Facial Fail. Retry.\n");
						memset(file_sep, 0, 100);
						sprintf(file_sep, "rm /dev/shm/face.jpg");
						dp("%s\n", file_sep);
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
						dp("Face Cnt : %d\n", face_crop_cnt);
						fr_state = FR_END;
					}
				}
				else if (fr_state != FR_END && total_time > FACE_FIND_END_TIME) {
				// else if (total_time > FACE_FIND_END_TIME) {
					fr_state = FR_END;	// fr_state 5 / Time out
					// Make File Send
					if (stream_state == 0) {
						#if 0
							if (Ready_Busy_Check() > 0 && face_crop_cnt > 0){
								send_retry_flag = false;
								ret = spi_send_file_face(REC_FACESHOT, face_crop_cnt);
								if (ret < 0) {
									retry_time = sample_gettimeus();
									while (!send_retry_flag) {
										if (send_retry_flag) {
											spi_send_file_face(REC_FACESHOT, face_crop_cnt);
										}
										if ((sample_gettimeus() - retry_time) > 15000000) {
											dp("Motion Cap Faile!\n");
											break;
										}
									}
								}
								#ifdef __PHILL_REQ__
									Set_Vol(90,25,spk_vol_buf,spk_gain_buf);
					            	ao_file_play_thread("/tmp/mnt/sdcard/effects/bell4.wav");
					            #endif
							}
						#else
							if (Ready_Busy_Check() > 0 && thumbnail_state == THUMB_END) {
								send_retry_flag = false;
								uint8_t snap_minor = 0;
								if (face_crop_cnt > 0) snap_minor = REC_FACESHOT;
								else snap_minor = REC_SNAPSHOT;
								bLiveFile = true;
								ret = spi_send_file_face(snap_minor, face_crop_cnt);
								if (ret < 0) {
									int secss = 0;
									retry_time = sample_gettimeus();
									while (1) {
										if (stream_state == 1) {
											dp("streaming!!\n");
											break;
										}
										if (send_retry_flag) {
											spi_send_file_face(snap_minor, face_crop_cnt);
											break;
										}
										if ((sample_gettimeus() - retry_time)%1000000 == 0){
											secss++;
											dp("sec:%d\n", secss);
										}
										if ((sample_gettimeus() - retry_time) > 15000000) {
											dp("Motion Cap Faile!\n");
											break;
										}
									}
								}
								bLiveFile = false;

								if (face_crop_cnt > 0) {
									// system("cp /dev/shm/face*.jpg /tmp/mnt/sdcard");
									#ifdef __PHILL_REQ__
										Set_Vol(90,25,spk_vol_buf,spk_gain_buf);
						            	ao_file_play_thread("/tmp/mnt/sdcard/effects/bell1.wav");
						            #endif
								}
							}
						#endif

						if (!face_send_flag) {
							face_send_flag = true;
							face_end(REC);
						}
					}
				}

				// if ((total_time > THUMBNAIL_TIME) && (thumbnail_state == THUMB_WAIT) &&
				// 	(fr_state == FR_WAIT || fr_state == FR_END || fr_state == FR_START)) {	// thumbnail
				// 	dp("Thumbnail Make Start!!\n");
				// 	thumbnail_snap = true;
				// 	thumbnail_state = THUMB_START;
				// }
				if (thumbnail_state == THUMB_WAIT) {	// thumbnail
					dp("Thumbnail Make Start!!\n");
					thumbnail_snap = true;
					thumbnail_state = THUMB_START;
				}
				if (thumbnail_state == THUMB_SNAPSHOT) {
	   				thumbnail_make(thum_face_data);
	   				thumbnail_state = THUMB_END;

	   				// if (Ready_Busy_Check() > 0){
					// 	dp("Thumbnail Send!\n");
					// 	memset(file_path, 0, 64);
					// 	sprintf(file_path, "/dev/shm/thumbnail_last.jpg");
					// 	send_retry_flag = false;
					// 	ret = spi_send_file(REC_SNAPSHOT, file_path, 0, 0, 0);
					// 	if (ret < 0) {
					// 		retry_time = sample_gettimeus();
					// 		while (!send_retry_flag) {
					// 			if (send_retry_flag) {
					// 				spi_send_file_face(REC_SNAPSHOT, face_crop_cnt);
					// 			}
					// 			if ((sample_gettimeus() - retry_time) > 15000000) {
					// 				dp("Face Data Fail!\n");
					// 				break;
					// 			}
					// 		}
					// 	}
					// }
					
					// else {
					// 	dp("Fail to Thumbnail Send.\n");
					// }
				}

				#ifdef __PHILL_REQ__
					if ((total_time > 15000000) && (clip_rec_state == REC_ING)) {	// 15Sec Time Over -> Clip Stop
						// rec_stop = true;
						dp("CLIP END:Time Over! %lld\n", total_time);
						clip_rec_state = REC_STOP;
						// box_snap = true;
						Rec_type = MAKE_FILE;
						file_cnt = 3;
					}
				#endif


				if (start_flag && (face_cnt != 0) && !roaming_person) {
					dp("Face Check!\n");
					roaming_person = true;
				}

				if ((total_time > ROAMING_PER_TIME) && roaming_person && 
					clip_cause_t.Major == CLIP_CAUSE_MOVE && clip_cause_t.Minor == CLIP_MOVE_MOVE) {
					dp("Roaming Person Tag!! : %lld\n", total_time);
					clip_cause_t.Minor = CLIP_MOVE_PER;
				}

				if (rec_streaming_state == REC_STOP) {
					rec_streaming_state = REC_MP4MAKE;
					dp("CLIP END:Streaming End! %lld\n", total_time);
					clip_rec_state = REC_STOP;
					// box_snap = true;
					Rec_type = MAKE_FILE;
				}

				if ((file_cnt == 0) && (bell_flag)) {	// Bell Push -> Clip Stop
					dp("CLIP END:Bell!\n");
					clip_rec_state = REC_STOP;
					
					bell_flag = false;
					Rec_type = BELL_REC;
					bl_state = BSS_START;
					// if (fr_state != FR_END) {
						bell_snap_m = true;
						bell_snap_b = true;
					// }
				}

			#ifdef __FRAME_SYNC__
				if (((frame_ck > 1180) || (total_time > MAX_CLIP_TIME)) && (clip_rec_state == REC_ING)) {	// 60Sec Time Over -> Clip Stop
			#else
				if ((total_time > MAX_REC_TIME) && (clip_rec_state == REC_ING)) {	// 60Sec Time Over -> Clip Stop
			#endif
				
					// rec_stop = true;
					clip_rec_state = REC_STOP;
					dp("CLIP END:Time Over! %lld\n", total_time);
					// box_snap = true;
					Rec_type = MAKE_FILE;
					file_cnt = 3;
				}

				if (rec_on) {
					clip_rec_state = REC_STOP;
					dp("CLIP END:Stream REC! %lld\n", total_time);
					Rec_type = STRM_REC;
				}
				
			#ifndef __PHILL_REQ__
				if ((file_cnt == 0) && (total_time > BELL_START_TIME+1000000) && (clip_rec_state < REC_STOP)) {	// Face or Motion Not Found -> Clip Stop
					if ((face_cnt == 0) && (person_cnt == 0) && (main_motion_detect == 0)) {
						if ((sample_gettimeus() - end_time) > CLIP_CLOSE_TIME) {
							dp("CLIP END:Move End!\n");
							// rec_stop = true;
							clip_rec_state = REC_STOP;
							// box_snap = true;
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
								// dp("file Count Error!\n");
								// file_cnt = 3;
							// }
							dp("Detection End! REC END. file cnt : %d\n", file_cnt);
						}
					}
					else {
						// dp("face:%d, person:%d, move:%d\n", face_cnt, person_cnt, main_motion_detect);
						end_time = sample_gettimeus();
					}
				}
			#endif

				
			}
			else if (Rec_type == BELL_REC) {
				if (start_time2 == 0)
					start_time2 = end_time2 = sample_gettimeus();
				total_time2 = sample_gettimeus() - start_time2;
				if (start_time3 != 0) {
					total_time3 = sample_gettimeus() - start_time3;
				}
				if (total_time2%10000000 == 0){
					dp("Rec T:%d time : %lld\n", Rec_type, total_time2);
				}

				if (dimming) {
					dimming_e =  sample_gettimeus() - dimming_s;
					if ((dimming_e > 100000) & dimming_up) {
						dimming_s = sample_gettimeus();
						if (dimming_val > 15)
							dimming_val -= 2;
						else
							dimming_val -= 1;
						// dimming_val -= 2;
						LED_dimming (dimming_val);
						if (dimming_val <= 5) {
							dp("tup\n");
							dimming_up = false;
						}
						// dp("dimming val : %d %lld\n", dimming_val, dimming_e);
					}
					else if ((dimming_e > 100000) & !dimming_up) {
						dimming_s = sample_gettimeus();
						if (dimming_val > 15)
							dimming_val += 2;
						else
							dimming_val += 1;
						// dimming_val += 2;
						LED_dimming (dimming_val);
						if (dimming_val >= 30) {
							dp("tdn\n");
							dimming_up = true;
						}
						// dp("dimming val : %d %lld\n", dimming_val, dimming_e);
					}
				}

				if ( (!bell_snap_m && !bell_snap_b) && bl_state < BSS_SEND ) {
					bl_state = BSS_SEND;
					
					bell_rec_state = REC_START;

					
				}

				if (bl_state == BSS_SEND) {
					if (temp_flag) {
						major_buf1 = REC_TEMP_SNAP_M;
						major_buf2 = REC_TEMP_SNAP_B;
					}
					else {
						major_buf1 = REC_BELL_SNAP_M;
						major_buf2 = REC_BELL_SNAP_B;
					}

					av_off_flag = false;

					// if (fr_state != FR_END) {
					bLiveFile = true;
						if (Ready_Busy_Check() > 0){
							dp("Bell/Temp Dual JPG Send!\n");
							// memset(file_path, 0, 64);
							// sprintf(file_path, "/dev/shm/bell_m.jpg");
							// memset(file_sep, 0, 64);
							// sprintf(file_sep, "/dev/shm/bell_b.jpg");
							// box_resize("/dev/shm/bell_b.jpg", "dev/shm/bell_b_r.jpg");
							// ret = spi_send_file_dual(major_buf1, major_buf2, "/dev/shm/bell_m.jpg", "dev/shm/bell_b_r.jpg");
							ret = spi_send_file_dual(major_buf1, major_buf2, "/dev/shm/bell_m.jpg", "/dev/shm/bell_b.jpg");
							if (ret < 0) {
								int secss = 0;
								retry_time = sample_gettimeus();
								while (1) {
									if (stream_state == 1) {
										dp("streaming!!\n");
										break;
									}
									if (send_retry_flag) {
										spi_send_file_dual(major_buf1, major_buf2, "/dev/shm/bell_m.jpg", "/dev/shm/bell_b.jpg");
										break;
									}
									if ((sample_gettimeus() - retry_time)%1000000 == 0){
										secss++;
										dp("sec:%d\n", secss);
									}
									if ((sample_gettimeus() - retry_time) > 15000000) {
										dp("Motion Cap Faile!\n");
										break;
									}
								}
							}
						}
						else {
							dp("Fail to Dual Bell JPG Send.\n");
						}
					// }
					bLiveFile = false;

					if (!face_send_flag) {
						face_send_flag = true;
						face_end(REC);
					}

					stream_state = 1;
					// data_sel = 4;

					gpio_LED_dimming(2);
					dimming_val = 20;

					bl_state = BSS_END;
				}

				if (bl_state == BSS_END) {
					
					if (total_time2 > BELL_TIME_MIN && dimming && !redimming) {
						dimming = false;
						gpio_LED_dimming(1);
						if (bellend_sound == 1)
							ao_file_play_thread("/tmp/mnt/sdcard/effects/bellend.wav");
					}

					if (total_time2 > BELL_TIME_MIN && rebell) {
						rebell = false;
						bell_snap_m = true;
						bell_snap_b = true;
						stream_state = 0;

						start_time3 = sample_gettimeus();

						while(!bell_snap_m && !bell_snap_b);

						bLiveFile = true;
						if (Ready_Busy_Check() > 0){
							dp("Bell/Temp Dual JPG ReSend!\n");
							// memset(file_path, 0, 64);
							// sprintf(file_path, "/dev/shm/bell_m.jpg");
							// memset(file_sep, 0, 64);
							// sprintf(file_sep, "/dev/shm/bell_b.jpg");
							// box_resize("/dev/shm/bell_b.jpg", "dev/shm/bell_b_r.jpg");
							// ret = spi_send_file_dual(major_buf1, major_buf2, "/dev/shm/bell_m.jpg", "dev/shm/bell_b_r.jpg");
							ret = spi_send_file_dual(major_buf1, major_buf2, "/dev/shm/bell_m.jpg", "/dev/shm/bell_b.jpg");
							if (ret < 0) {
								int secss = 0;
								retry_time = sample_gettimeus();
								while (1) {
									if (stream_state == 1) {
										dp("streaming!!\n");
										break;
									}
									if (send_retry_flag) {
										spi_send_file_dual(major_buf1, major_buf2, "/dev/shm/bell_m.jpg", "/dev/shm/bell_b.jpg");
										break;
									}
									if ((sample_gettimeus() - retry_time)%1000000 == 0){
										secss++;
										dp("sec:%d\n", secss);
									}
									if ((sample_gettimeus() - retry_time) > 15000000) {
										dp("Motion Cap Faile!\n");
										break;
									}
								}
							}
						}
						else {
							dp("Fail to Dual Bell JPG Send.\n");
						}

						gpio_LED_dimming(2);
						dimming_val = 20;
						redimming = true;

						bLiveFile = false;

						stream_state = 1;
					}
					else {
						rebell = false;
					}

					if (rec_on) {
						bell_rec_state = REC_STOP;
						bell_rerecode_flag = true;
						dp("BELL END:Stream REC! %lld\n", total_time);
						Rec_type = STRM_REC;
					}

					if ((rec_streaming_state == REC_STOP) && (total_time2 > 8000000)) {
						bell_rerecode_flag = true;
						rec_streaming_state = REC_MP4MAKE;
						dp("BELL END:Steaming End! %lld\n", total_time);
						bell_rec_state = REC_STOP;
						if (bell_stream_flag == false){
							Rec_type = MAKE_FILE;
							// box_snap = true;
						}
						if (bellend_sound == 2)  {
			                ao_file_play_thread("/tmp/mnt/sdcard/effects/bellend.wav");
			                bellend_sound++;
						}
						else {
							dp("bellend : %d\n", bellend_sound);
						}
						continue;
					}

					if ((total_time2 > MAX_REC_TIME) && (bell_rec_state == REC_ING) && (bell_rec_state < REC_STOP)) {	// 60Sec Time Over -> Clip Stop
						bell_rerecode_flag = true;
						// rec_stop = true;
						dp("BELL END:Time Over! %lld\n", total_time2);
						bell_rec_state = REC_STOP;
						// dp("0:%d t3:%lld s3:%lld\n", bell_stream_flag, total_time3, start_time3);
						if (bell_stream_flag == false) {
							if (start_time3 != 0) {
								if (total_time3 > BELL_TIME_MIN) {
									// dp("1 t3:%lld s3:%lld\n", total_time3, start_time3);
									Rec_type = MAKE_FILE;
								}
								else {
									// dp("2 t3:%lld s3:%lld\n", total_time3, start_time3);
								}
							}
							else {
								// dp("3 t3:%lld s3:%lld\n", total_time3, start_time3);
								Rec_type = MAKE_FILE;
							}
							// box_snap = true;
						}

						if (bellend_sound == 2)  {
			                ao_file_play_thread("/tmp/mnt/sdcard/effects/bellend.wav");
			                bellend_sound++;
						}
						else {
							dp("bellend : %d\n", bellend_sound);
						}
					}

					if ((total_time2 > TEMP_TIME_MIN) && (temp_flag)) {
						bell_rerecode_flag = true;
						dp("BELL END:Temp End! %lld\n", total_time2);
						bell_rec_state = REC_STOP;
						if (bell_stream_flag == false) {
							Rec_type = MAKE_FILE;
							// box_snap = true;
						}
					}

					if ((total_time2 > BELL_TIME_MIN) && (person_cnt == 0) && (main_motion_detect == 0) && (bell_rec_state < REC_STOP)) {
						if ((sample_gettimeus() - end_time2) > CLIP_CLOSE_TIME) {
							bell_rerecode_flag = true;
							dp("BELL END:Move End! %lld\n", total_time2);
							bell_rec_state = REC_STOP;
							if (bell_stream_flag == false) {
								Rec_type = MAKE_FILE;
								// box_snap = true;
							}

							if (bellend_sound == 2)  {
				                ao_file_play_thread("/tmp/mnt/sdcard/effects/bellend.wav");
				                bellend_sound++;
							}
							// else {
							// 	dp("bellend : %d\n", bellend_sound);
							// }
						}
					}
					else {
						end_time2 = sample_gettimeus();
					}

					if (bell_rec_state == REC_STOP || bell_rec_state == REC_WAIT) {
						if (start_time3 != 0) {
							// dp("4 t3:%lld s3:%lld\n", total_time3, start_time3);
							if (total_time3 > BELL_TIME_MIN) {
								dp("5 t3:%lld s3:%lld\n", total_time3, start_time3);
								stream_state = 0;
								bell_rerecode_flag = true;
								Rec_type = MAKE_FILE;
								if (bellend_sound == 1)  {
					                ao_file_play_thread("/tmp/mnt/sdcard/effects/bellend.wav");
					                bellend_sound++;
								}
							}
							
						}
						else if (bell_stream_flag == false) {
							dp("6 t3:%lld s3:%lld\n", total_time3, start_time3);
							stream_state = 0;
							bell_rerecode_flag = true;
							Rec_type = MAKE_FILE;
						}
					}
				}
			}
		}


		if(Rec_type == MAKE_FILE) {

			if (rec_on) {
				dp("MAKEFILE:Stream REC! %lld\n", total_time);
				Rec_type = STRM_REC;
			}

			if (stream_state == 1) continue;

			dimming = false;

			// if ((clip_rec_state == REC_WAIT && bell_rec_state == REC_WAIT) ||
			// 	(clip_rec_state == REC_WAIT && bell_rec_state == REC_READY))
			// {
			// 	if (clip_rec_state == REC_WAIT) clip_rec_state = REC_MP4MAKE;
			// 	if (bell_rec_state == REC_WAIT) bell_rec_state = REC_MP4MAKE;

			// 	if (total_time < 23000000) {
			// 		file_cnt = 1;
			// 	}
			// 	else if (total_time < 43000000) {
			// 		file_cnt = 2;
			// 	}
			// 	else if (total_time >= 43000000) {
			// 		file_cnt = 3;
			// 	}
			// 	dp("Detection End! REC END. file cnt : %d\n", file_cnt);

			// 	if (total_time2 < 2000000){
			// 		file_cnt2 = 0;
			// 	}
			// 	else if (total_time2 < 23000000) {
			// 		file_cnt2 = 1;
			// 	}
			// 	else if (total_time2 < 43000000) {
			// 		file_cnt2 = 2;
			// 	}
			// 	else if (total_time2 >= 43000000) {
			// 		file_cnt2 = 3;
			// 	}
			// 	dp("Detection End! BELL END. file cnt : %d\n", file_cnt2);
			// }
			// else {
			// 	// dp("clip:%d bell:%d\n", clip_rec_state, bell_rec_state);
			// 	if (clip_rec_state != REC_MP4MAKE) {
			// 		printf("Clip Rec State Error!\n");
			// 		continue;
			// 	}

			// 	if (bell_rec_state != REC_READY && bell_rec_state != REC_MP4MAKE) {
			// 		printf("Bell Rec State Error! : %d\n", bell_rec_state);
			// 		continue;
			// 	}
			// }


			if (clip_rec_state == REC_WAIT) {
				if (clip_rec_state == REC_WAIT) clip_rec_state = REC_MP4MAKE;

				if (total_time < 23000000) {
					file_cnt = 1;
				}
				else if (total_time < 43000000) {
					file_cnt = 2;
				}
				else if (total_time >= 43000000) {
					file_cnt = 3;
				}
				dp("Detection End! REC END. file cnt : %d\n", file_cnt);
			}
			else {
				if (clip_rec_state != REC_MP4MAKE) {
					// printf("Clip Rec State Error!\n");
					continue;
				}
			}


			if (bell_rec_state == REC_WAIT) {
				if (bell_rec_state == REC_WAIT) bell_rec_state = REC_MP4MAKE;

				if (total_time2 < 3000000){
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
				dp("Detection End! BELL END. file cnt : %d\n", file_cnt2);
			}
			else {
				if (bell_rec_state != REC_READY && bell_rec_state != REC_MP4MAKE) {
					// printf("Bell Rec State Error! : %d\n", bell_rec_state);
					continue;
				}
			}

			make_start = sample_gettimeus();

			box_snap = true;

			while(box_snap);

			system("sync");

			

			// #ifndef __PHILL_REQ__
				gpio_LED_Set(0);
			// #endif

			// pthread_t tid_makefile;
			// Make_File mfd1;
			// mfd1.type = 0;
			// mfd1.cnt = file_cnt;
			// cfile_flag = false;
			// ret = pthread_create(&tid_makefile, NULL, make_mp4_clip,(void*)&mfd1);
			// if(ret != 0) {
			// 	IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
			// 	return -1;
			// }

			pthread_t tid_makefile1, tid_makefile2, tid_makefile;
			dp("Thumnail state : %d\n", thumbnail_state);
			if (!cfile_flag1 && !cfile_flag2) {			
				if (thumbnail_state == THUMB_END) {
					Make_File mfd1;
					mfd1.type = 0;
					mfd1.cnt = file_cnt;
					cfile_flag1 = cfile_flag2 = false;

					ret = pthread_create(&tid_makefile1, NULL, make_mp4_clip1,(void*)&mfd1);
					if(ret != 0) {
						IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
						return -1;
					}
					ret = pthread_create(&tid_makefile2, NULL, make_mp4_clip2,(void*)&mfd1);
					if(ret != 0) {
						IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
						return -1;
					}
				}
				else {
					cfile_flag1 = cfile_flag2 = true;
					file_cnt = 0;
					dp("Final Clip File Cnt : %d\n", file_cnt);
				}
			}



			if (!bfile_flag) {			
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
				else {
					bfile_flag = true;
				}
			}



			// if (main_rec_end && box_rec_end) {
			if ((clip_rec_state == REC_MP4MAKE && bell_rec_state == REC_MP4MAKE) ||
				(clip_rec_state == REC_MP4MAKE && bell_rec_state == REC_READY)) {

	    		#ifndef __PHILL_REQ__
	    			if (!box_send_flag && stream_state != 1 && !bell_flag)  {
			    		// memset(file_path, 0, 64);
						// sprintf(file_path, "/dev/shm/box0.jpg");
						box_resize("/dev/shm/box0.jpg", "dev/shm/box_r.jpg");

						// memset(file_path, 0, 64);
						// sprintf(file_path, "/dev/shm/box_r.jpg");

						if (stream_state != 1) {

							if (!file_21_flag) {
								file_21_flag = true;
								make_file_start(REC);
								bLive = true;
							}
						
			    			ret = spi_send_file(REC_BOX_ALM, "/dev/shm/box_r.jpg", 0, 0, 0);
			    			if (ret < 0) {
								int secss = 0;
								retry_time = sample_gettimeus();
								while (1) {
									if (stream_state == 1) {
										dp("streaming!!\n");
										break;
									}
									if (send_retry_flag) {
										spi_send_file(REC_BOX_ALM, "/dev/shm/box_r.jpg", 0, 0, 0);
										break;
									}
									if ((sample_gettimeus() - retry_time)%1000000 == 0){
										secss++;
										dp("sec:%d\n", secss);
									}
									if ((sample_gettimeus() - retry_time) > 15000000) {
										dp("Motion Cap Faile!\n");
										break;
									}
								}
							}
			    			box_send_flag = true;
			    			usleep(200*1000);
			    		}
		    		}
	    		#endif

				while (!cfile_flag1 || !cfile_flag2 || !bfile_flag || (stream_state == 1)) 
				{
					if (rec_on) {
						dp("SEND FILE:Stream REC! %lld\n", total_time);
						Rec_type = STRM_REC;
						break;
					}

					if (bell_flag) {
						break;
					}
				}

				// if (cfile_flag2 && (file_cnt2 > 0)) {
				// 	system ("cp /dev/shm/bell_m0.mp4 /tmp/mnt/sdcard");
				// 	system ("cp /dev/shm/bell_b0.mp4 /tmp/mnt/sdcard");
				// }
				

				// system("sync");

				dp("mp4 Time : %lld\n", sample_gettimeus() - make_start);

				if (bell_flag) {
					bell_flag = false;
					bell_snap_m = bell_snap_b = true;
					// while(!bell_snap_m && !bell_snap_b) ;
					dp("Bell ReStart!!\n");
					Rec_type = BELL_REREC;
					bfile_flag = false;

					bl_state = BSS_START;

					start_time2 = end_time2 = sample_gettimeus();
				}
				else {
					Rec_type = SEND_FILE;
				}
			}
		}

		if (Rec_type == SEND_FILE) {
			if (rec_on) {
				dp("SEND FILE:Stream REC! %lld\n", total_time);
				Rec_type = STRM_REC;
				continue;
			}

			if (bell_flag) {
				bell_flag = false;
				bell_snap_m = bell_snap_b = true;
				while(!bell_snap_m && !bell_snap_b) ;
				dp("Bell ReStart!!\n");
				Rec_type = BELL_REREC;
				bfile_flag = false;

				bl_state = BSS_START;

				start_time2 = end_time2 = sample_gettimeus();
				continue;
			}

			if (stream_state == 1) continue;

			bLive = true;

			if (file_cnt == 1 && cfile_flag1 && cfile_flag2 && file_cnt2 == 0) {
				pthread_t tid_avoff;

				av_off_flag = false;

				ret = pthread_create(&tid_avoff, NULL, AV_Off, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create AV_Off failed\n", __func__);
					return -1;
				}
				
			}
			// dp("%d %d %d\n", file_cnt, cfile_flag1, cfile_flag2);

			////// Fine Spi Send /////////
			if (file_cnt > 0 && cfile_flag1 && cfile_flag2) { 
				if (!file_21_flag) {
					file_21_flag = true;
					make_file_start(REC);
				}
				dp("Clip file send\n");

				#ifdef __FILE_SEND_CHANGE__
					FileSend fs;
					fs.minor = REC_CLIP_F;
					fs.tag1 = clip_cause_t.Major;
					fs.tag2 = clip_cause_t.Minor;
					fs.filenum = 0;
					fs.filecnt = file_cnt;
					send_retry_flag = false;
					// ret = spi_send_total_clip(&fs);
					// if(ret < 0) {
					// 	dp("File Send Fail!!\n");
					// }
					// else if (ret == 2) { 
					// 	// make_file_start(REC);
					// 	dp("file send stop : streaming\n");
					// 	// continue;
					// }
					// else {
					// 	file_cnt = 0;
					// }

					// if (Ready_Busy_Check() > 0){
						ret = spi_send_total_clip(&fs);
						if(ret < 0) {
							if (ret != -2) { 
								dp("File Send Fail!!\n");
								if (send_retry < 3) {
									int secss = 0;
									retry_time = sample_gettimeus();
									while(1) {
										if (stream_state == 1) {
											dp("streaming!!\n");
											break;
										}
										if (send_retry_flag) {
											send_retry++;
											dp("Clip Retry : %d\n", send_retry);
											break;
										}
										if ((sample_gettimeus() - retry_time)%1000000 == 0){
											secss++;
											dp("sec:%d\n", secss);
										}
										if ((sample_gettimeus() - retry_time) > 15000000) {
											dp("Clip Retry timeout!");
											send_retry = 4;
											break;
										}
									}
									if (send_retry <= 3) continue;
								}
							}
						}
						else if (ret == 2) {
							// make_file_start(REC);
							dp("file send stop : streaming\n");
							// continue;
						}
						else {
							file_cnt = 0;
							send_retry = 0;
						}
					// }
					// else {
						// dp("Start RB Fail!\n");
					// }

					if (stream_state == 1 || bell_flag) {
						continue;
					}
				#else
					int save_cnt=0;
					char file_name[20];

					for (int i=0; i<file_cnt; i++) {
						sprintf(file_name, "main%d", i);
						
						if (!netwrok_busy) {
							if (Ready_Busy_Check() > 0){
								dp("File %d-1 Start!\n", i+1);
								memset(file_path, 0, 64);
								sprintf(file_path, "/dev/shm/main%d.mp4", i);
								spi_send_file(REC_CLIP_F, file_path, 0, i+1, 1);
								}
							else {
								dp("Fail to Send %d-1\n", i+1);
							}
							
							if (Ready_Busy_Check() > 0){
								dp("File %d-2 Start!\n", i+1);
								memset(file_path, 0, 64);
								sprintf(file_path, "/dev/shm/box%d.mp4", i);
								// sprintf(file_path, "/dev/shm/box%d.mkv", i);
								spi_send_file(REC_CLIP_B, file_path, 0, i+1, 2);
								}
							else {
								dp("Fail to Send %d-2\n", i+1);
							}
							
						}
						
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
							// dp("file cnt : %s.mp4 %d\n", file_name, save_cnt);

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
				#endif
			}


			if (file_cnt2 > 0 && bfile_flag) { 
				dp("Bell file send");
				
				if (!file_21_flag) {
					file_21_flag = true;
					make_file_start(REC);
				}

				if (temp_flag) {
					clip_cause_t.Major = CLIP_CAUSE_MOUNT;
					if (temp_unmount_flag)
						clip_cause_t.Minor = CLIP_MOUNT_MOUNT;
					else
						clip_cause_t.Minor = CLIP_MOUNT_DISMT;
				}
				else {
					clip_cause_t.Major = CLIP_CAUSE_BELL;
					if (bell_call_flag)
						clip_cause_t.Minor = CLIP_BELL_CALL;
					else
						clip_cause_t.Minor = CLIP_BELL_BELL;
				}
				#ifdef __FILE_SEND_CHANGE__
					FileSend fs;
					fs.minor = REC_CLIP_F;
					fs.tag1 = clip_cause_t.Major;
					fs.tag2 = clip_cause_t.Minor;
					fs.filenum = 0;
					fs.filecnt = file_cnt2;
					send_retry_flag = false;
					if (Ready_Busy_Check() > 0){
						ret = spi_send_total_clip(&fs);
						if(ret < 0) {
							if (ret != -2) { 
								dp("File Send Fail!!\n");
								if (send_retry < 3) {
									int secss = 0;
									retry_time = sample_gettimeus();
									while(1) {
										if (stream_state == 1) {
											dp("streaming!!\n");
											break;
										}
										if (send_retry_flag) {
											send_retry++;
											dp("Clip Retry : %d\n", send_retry);
											break;
										}
										if ((sample_gettimeus() - retry_time)%1000000 == 0){
											secss++;
											dp("sec:%d\n", secss);
										}
										if ((sample_gettimeus() - retry_time) > 15000000) {
											dp("Clip Retry timeout!");
											send_retry = 4;
											break;
										}
									}
									if (send_retry <= 3) continue;
								}
							}
						}
						else if (ret == 2) {
							// make_file_start(REC);
							dp("file send stop : streaming\n");
							// continue;
						}
						else {
							file_cnt2 = 0;
							send_retry = 0;
						}
					}
					else {
						dp("Start RB Fail!\n");
					}

					if (stream_state == 1 || bell_flag) {
						continue;
					}
				#else
					int save_cnt=0;
					char file_name[20];

					for (int i=0; i<file_cnt2; i++) {
						sprintf(file_name, "bell_m%d", i);
						if (!netwrok_busy) {
							if (Ready_Busy_Check() > 0){
								dp("File %d-1 Start!\n", i+1);
								memset(file_path, 0, 64);
								sprintf(file_path, "/dev/shm/bell_m%d.mp4", i);
								spi_send_file(REC_CLIP_F, file_path, 0, i+1, 1);
								}
							else {
								dp("Fail to Send %d-1\n", i+1);
							}
							
							if (Ready_Busy_Check() > 0){
								dp("File %d-2 Start!\n", i+1);
								memset(file_path, 0, 64);
								sprintf(file_path, "/dev/shm/bell_b%d.mp4", i);
								// sprintf(file_path, "/dev/shm/box%d.mkv", i);
								spi_send_file(REC_CLIP_B, file_path, 0, i+1, 2);
								}
							else {
								dp("Fail to Send %d-2\n", i+1);
							}
						}
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
							// dp("file cnt : %s.mp4 %d\n", file_name, save_cnt);

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
				#endif
			}

			if (streaming_rec_state == REC_SPISEND) {
				// for (int i=0; i<rec_cnt; i++) {
				int i = 0;

				if (!file_21_flag) {
					file_21_flag = true;
					make_file_start(REC);
				}
					FileSend fs;
					if (stream_tag[i] == CLIP_CAUSE_STREM) {
						fs.minor = REC_STREAMING_M;
						fs.tag1 = CLIP_CAUSE_STREM;
						fs.tag2 = CLIP_STREAM_REC;
					}
					else {
						fs.minor = REC_CLIP_F;
						fs.tag1 = CLIP_CAUSE_BELL;
						fs.tag2 = CLIP_BELL_CALL;
					}
					fs.filenum = i;
					fs.filecnt = file_each_cnt[i];
					send_retry_flag = false;
					if (Ready_Busy_Check() > 0) { 
						ret = spi_send_total_stream_clip(&fs);
						if(ret < 0) {
							if (ret != -2) { 
								dp("File Send Fail!!\n");
								if (send_retry < 3) {
									int secss = 0;
									retry_time = sample_gettimeus();
									while(1) {
										if (stream_state == 1) {
											dp("streaming!!\n");
											break;
										}
										if (send_retry_flag) {
											send_retry++;
											dp("Clip Retry : %d\n", send_retry);
											break;
										}
										if ((sample_gettimeus() - retry_time)%1000000 == 0){
											secss++;
											dp("sec:%d\n", secss);
										}
										if ((sample_gettimeus() - retry_time) > 15000000) {
											dp("Clip Retry timeout!");
											send_retry = 4;
											break;
										}
									}
									if (send_retry <= 3) continue;
								}
							}
						}
						else if (ret == 2) {
							dp("Stream Re:connect!\n");
						}
						else {
							streaming_rec_state = REC_SENDEND;
							send_retry = 0;
						}
					}
					else {
						dp("Start RB Faile!\n");
					}
				// }

				if (streaming_rec_state == REC_RECONNECT) {
					continue;
				}
				else {
					device_end(STREAMING);
					dp("Streaming Mode End!!\n");
					bUart = true;
					sleep(2);
					break;
				}
			}

			system("sync");

			// system("rm /tmp/mnt/sdcard/mp4/*");

			// system("cp /dev/shm/*.mp4 /tmp/mnt/sdcard/mp4");

			device_end(REC);
			dp("File Send End!!\n");

			dp("Make Time : %lld\n", sample_gettimeus() - make_start);

			break;
		}

		if (Rec_type == STRM_REC) {
			

			if (streaming_rec_state == REC_RECONNECT) {
				dp("Stream Re:Connect!\n");
				system("rm /dev/shm/*.mp4");
				streaming_rec_state = REC_WAIT;
				
			}
			else if (streaming_rec_state == REC_ING) {
				rec_now = sample_gettimeus() - rec_time_s;
				if (rec_total + rec_now >= 60000000) {
	            	rec_on = false;
	            	rec_total += rec_time_s;
	            	rec_mem_flag = true;
	            	streaming_rec_state = REC_STOP;
	            	mem_full_flag = true;
	            	streaming_rec_end(CAUSE_MEM);
	            	// stream_state = 0;
				}

				if (rec_end) {
					if (rec_cnt > 0) {
						rec_time_e = sample_gettimeus()-rec_time_s;
						dp("Rec Time : %lld total : %lld\n", rec_time_e, rec_total);
		            	rec_on = false;
		            	rec_each_time[rec_cnt-1] = rec_time_e;
		            	rec_total += rec_time_s;
		            	rec_mem_flag = true;
		            	streaming_rec_state = REC_STOP;
		            	mem_full_flag = true;
		            	streaming_rec_end(CAUSE_MEM);
		            }
		            else {
		            	device_end(STREAMING);
						dp("Streaming Mode End!!\n");
						bUart = true;
						sleep(2);
						break;
		            }
				}
			}
			else if (streaming_rec_state == REC_WAIT) {
				if (!rec_mem_flag) {
					rec_mem_flag = true;
					streaming_rec_end(CAUSE_MEM);
				}
				if (rec_end) {
					streaming_rec_state = REC_MP4MAKE;
					rec_end = false;
					stream_state = 0;
				}			
			}
			else if (streaming_rec_state == REC_MP4MAKE) {

				pthread_t tid_makerec[10];
				bool make_flag = false;
				Make_File mfd_rec[10];

				stream_state = 0;
				make_file_start(REC);

				if (rec_cnt > 10) rec_cnt = 10;
				for (int i=0; i<rec_cnt; i++) {
					mfd_rec[i].type = 0;
					mfd_rec[i].index = i;
					if (rec_each_time[i] < 23000000)
						mfd_rec[i].cnt = 1;
					else if(rec_each_time[i] < 43000000)
						mfd_rec[i].cnt = 2;
					else
						mfd_rec[i].cnt = 3;

					file_each_cnt[i] = mfd_rec[i].cnt;

					rfile_flag[i] = false;

					ret = pthread_create(&tid_makerec[i], NULL, make_mp4_streamrec,(void*)&mfd_rec[i]);
					if(ret != 0) {
						IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create make_mp4_streamrec %d failed\n", __func__, i);
						return -1;
					}
				}

				while(!make_flag) {
					for (int i=0; i<rec_cnt;) {
						// dp("flag[%d] %d\n", i, rfile_flag[i]);
						if (rfile_flag[i] == false) {
							i = rec_cnt;
						}
						else {
							i++;
							if (rec_cnt == i) {
								make_flag = true;
							}
						}
					}
				}
				dp("Stream REC State End!!\n");

				streaming_rec_state = REC_SPISEND;
				if (bell_flag) {
					bell_flag = false;
					bell_snap_m = bell_snap_b = true;
					// while(!bell_snap_m && !bell_snap_b) ;
					dp("Bell ReStart!!\n");
					Rec_type = BELL_REREC;
					bfile_flag = false;

					bl_state = BSS_START;

					start_time2 = end_time2 = sample_gettimeus();
				}
				else {
					Rec_type = MAKE_FILE;
				}
			}

			// if (!rec_on){
			// 	Rec_type = MAKE_FILE;
			// }
		}
		if (Rec_type == BELL_REREC) {
			// dp("******************************BELL RESTART!********************************\n");
			if (start_time2 == 0)
					start_time2 = end_time2 = sample_gettimeus();
			total_time2 = sample_gettimeus() - start_time2;
			if (total_time2%10000000 == 0){
				dp("Rec T:%d time : %lld\n", Rec_type, total_time2);
			}

			// if ((total_time2 - forced_live) > 3000000) {
			// 	forced_live = total_time2;
			// 	device_live(boot_mode);
			// }

			if (dimming) {
				dimming_e =  sample_gettimeus() - dimming_s;
				if ((dimming_e > 100000) & dimming_up) {
					dimming_s = sample_gettimeus();
					if (dimming_val > 15)
						dimming_val -= 2;
					else
						dimming_val -= 1;
					// dimming_val -= 2;
					LED_dimming (dimming_val);
					if (dimming_val <= 5) {
						dp("tup\n");
						dimming_up = false;
					}
					// dp("dimming val : %d %lld\n", dimming_val, dimming_e);
				}
				else if ((dimming_e > 100000) & !dimming_up) {
					dimming_s = sample_gettimeus();
					if (dimming_val > 15)
						dimming_val += 2;
					else
						dimming_val += 1;
					// dimming_val += 2;
					LED_dimming (dimming_val);
					if (dimming_val >= 30) {
						dp("tdn\n");
						dimming_up = true;
					}
					// dp("dimming val : %d %lld\n", dimming_val, dimming_e);
				}
			}

			if (!bell_snap_m && !bell_snap_b && bl_state < BSS_SEND) {
				bl_state = BSS_SEND;
				start_time2 = end_time2 = sample_gettimeus();
				bell_rec_state = REC_START;
			}

			if (bl_state == BSS_SEND) {
				if (temp_flag) {
					major_buf1 = REC_TEMP_SNAP_M;
					major_buf2 = REC_TEMP_SNAP_B;
				}
				else {
					major_buf1 = REC_BELL_SNAP_M;
					major_buf2 = REC_BELL_SNAP_B;
				}

				av_off_flag = false;

				if (Ready_Busy_Check() > 0){
					dp("Bell/Temp Dual JPG Send!\n");
					// memset(file_path, 0, 64);
					// sprintf(file_path, "/dev/shm/bell_m.jpg");
					// memset(file_sep, 0, 64);
					// sprintf(file_sep, "/dev/shm/bell_b.jpg");
					// box_resize("/dev/shm/bell_b.jpg", "dev/shm/bell_b_r.jpg");
					// spi_send_file_dual(major_buf1, major_buf2, "/dev/shm/bell_m.jpg", "dev/shm/bell_b_r.jpg");
					spi_send_file_dual(major_buf1, major_buf2, "/dev/shm/bell_m.jpg", "/dev/shm/bell_b.jpg");
				}
				else {
					dp("Fail to Dual Bell JPG Send.\n");
				}

				gpio_LED_dimming(2);
				dimming_val = 20;

				stream_state = 1;
				// data_sel = 4;
				file_21_flag = false;

				bl_state = BSS_END;
			}

			if (bl_state == BSS_END) {

				if (total_time2 > BELL_TIME_MIN && dimming && !redimming) {
					dimming = false;
					gpio_LED_dimming(1);
					ao_file_play_thread("/tmp/mnt/sdcard/effects/bellend.wav");
				}

				if (total_time2 > BELL_TIME_MIN && rebell) {
					rebell = false;
					bell_snap_m = true;
					bell_snap_b = true;
					stream_state = 0;

					total_time3 = sample_gettimeus();

					while(!bell_snap_m && !bell_snap_b);

					bLiveFile = true;
					if (Ready_Busy_Check() > 0){
						dp("Bell/Temp Dual JPG ReSend!\n");
						// memset(file_path, 0, 64);
						// sprintf(file_path, "/dev/shm/bell_m.jpg");
						// memset(file_sep, 0, 64);
						// sprintf(file_sep, "/dev/shm/bell_b.jpg");
						// box_resize("/dev/shm/bell_b.jpg", "dev/shm/bell_b_r.jpg");
						// ret = spi_send_file_dual(major_buf1, major_buf2, "/dev/shm/bell_m.jpg", "dev/shm/bell_b_r.jpg");
						ret = spi_send_file_dual(major_buf1, major_buf2, "/dev/shm/bell_m.jpg", "/dev/shm/bell_b.jpg");
						if (ret < 0) {
							int secss = 0;
							retry_time = sample_gettimeus();
							while (1) {
								if (stream_state == 1) {
									dp("streaming!!\n");
									break;
								}
								if (send_retry_flag) {
									spi_send_file_dual(major_buf1, major_buf2, "/dev/shm/bell_m.jpg", "/dev/shm/bell_b.jpg");
									break;
								}
								if ((sample_gettimeus() - retry_time)%1000000 == 0){
									secss++;
									dp("sec:%d\n", secss);
								}
								if ((sample_gettimeus() - retry_time) > 15000000) {
									dp("Motion Cap Faile!\n");
									break;
								}
							}
						}
					}
					else {
						dp("Fail to Dual Bell JPG Send.\n");
					}

					gpio_LED_dimming(2);
					dimming_val = 20;
					redimming = true;

					bLiveFile = false;

					stream_state = 1;
				}
				else {
					rebell = false;
				}
				
				if ((rec_streaming_state == REC_STOP) && (total_time2 > 5000000)) {
					bell_rerecode_flag = true;
					bell_rec_state = REC_STOP;
					dp("BELL END:Steaming End! %lld\n", total_time);
					if (bellend_sound == 2)  {
		                ao_file_play_thread("/tmp/mnt/sdcard/effects/bellend.wav");
		                bellend_sound++;
					}
					else {
						dp("bellend : %d\n", bellend_sound);
					}
				}

				if ((total_time2 > MAX_REC_TIME) && (bell_rec_state == REC_ING) && (bell_rec_state < REC_STOP)) {	// 60Sec Time Over -> Clip Stop
					bell_rerecode_flag = true;
					 bell_rec_state = REC_STOP;
					dp("BELL END:Time Over! %lld\n", total_time);
					if (bellend_sound == 2)  {
		                ao_file_play_thread("/tmp/mnt/sdcard/effects/bellend.wav");
		                bellend_sound++;
					}
					else {
						dp("bellend : %d\n", bellend_sound);
					}
				}

				if ((total_time2 > TEMP_TIME_MIN) && (temp_flag)) {
					bell_rerecode_flag = true;
					dp("BELL END:Temp End! %lld\n", total_time2);
					bell_rec_state = REC_STOP;
					if (bell_stream_flag == false) {
						Rec_type = MAKE_FILE;
						// box_snap = true;
					}
				}

				if ((total_time2 > BELL_TIME_MIN) && (person_cnt == 0) && (main_motion_detect == 0) && (bell_rec_state < REC_STOP)) {
					if ((sample_gettimeus() - end_time2) > CLIP_CLOSE_TIME) {
						bell_rerecode_flag = true;
						bell_rec_state = REC_STOP;
						dp("BELL END:Move End! %lld\n", total_time);
						if (bellend_sound == 2)  {
			                ao_file_play_thread("/tmp/mnt/sdcard/effects/bellend.wav");
			                bellend_sound++;
						}
						else {
							dp("bellend : %d\n", bellend_sound);
						}

						if (bellend_sound == 2)  {
			                ao_file_play_thread("/tmp/mnt/sdcard/effects/bellend.wav");
			                bellend_sound++;
						}
						else {
							dp("bellend : %d\n", bellend_sound);
						}
					}
				}
				else {
					end_time2 = sample_gettimeus();
				}

				if ((bell_rec_state == REC_STOP) && (bell_stream_flag == false)) {
					bell_rerecode_flag = true;
					stream_state = 0;
					bl_state = BSS_MAKE;
				}
			}

			if (bl_state == BSS_MAKE) {
				// dp("Make File!!\n");
				if (bell_rec_state == REC_WAIT)
				{
					if (bell_rec_state == REC_WAIT) bell_rec_state = REC_MP4MAKE;

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
					dp("Detection End! BELL END. file cnt : %d\n", file_cnt2);
				}
				else continue;
			
				make_file_start(REC);

				pthread_t tid_makefile;

				if (!bfile_flag) {			
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
					else {
						bfile_flag = true;
					}
				}

				while (!bfile_flag) ;

				dp("ReBell end!!\n");

				Rec_type = MAKE_FILE;
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
        dp("spi init error\n");
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
						if (Ready_Busy_Check() > 0){
							dp("File %d-1 Start!\n", i+1);
							memset(file_path, 0, 64);
							sprintf(file_path, "/tmp/mnt/sdcard/mp4/main%d.mp4", i);
							spi_send_file(REC_CLIP_F, file_path, 0, i+1, 1);
							}
						else {
							dp("Fail to Send %d-1\n", i+1);
						}
						
						if (Ready_Busy_Check() > 0){
							dp("File %d-2 Start!\n", i+1);
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
			dp("File Send End!!\n");
			break;
		}
	}while(1);

	bExit = true;

	// pthread_join(tid_uart, NULL);

	return 0;
}




#define STREAMING_SPI


int stream_total(int mode) {
	int ret = 0;
	// int file_cnt = 0;
	// bool start_flag = false;
	// int64_t total_time = 0;
	// int64_t oldt_time = 0;
	// char file_path[64] = {0};
#ifdef __STREAMING_CMD__
    // bool adc_flag = false;
    // bool led_flag = false;
    // int gval = 0;
#else
    int64_t rec_time_e = 0;
    int64_t rec_now = 0;
    int file_each_cnt[10] = {0};
#endif

	pthread_t tid_ao, tid_ai;
    pthread_t tid_stream, tid_move, tim_osd, tid_fdpd;// , tid_snap;
    pthread_t tid_uart;//, tid_live;
    pthread_t tid_vmod;

    // pthread_t adc_thread_id, tid_clip;

    // int64_t test_up_time;


#ifdef STREAMING_SPI
    pthread_t tid_spi;
	//////////////// SPI Init ////////////////////////////////////////////////////////////////
	if (mode != 2) {
		ret = spi_init();
	    if(ret < 0){
	        dp("spi init error\n");
	        return 0;
	    }
	

	    data_sel = 4;
		if (data_sel <= 0 || data_sel > 4) {
			dp("Invalid Type!\n");
			return -1;
		}
		ret = pthread_create(&tid_spi, NULL, spi_send_stream, NULL);
		if(ret != 0) {
			IMP_LOG_ERR("[Udp]", "[ERROR] %s: pthread_create spi_send_stream failed\n", __func__);
			return -1;
		}
	}
	//////////////////////////////////////////////////////////////////////////////////////////
#else
	pthread_t tid_udp_out;
	sprintf(ip, "192.168.0.100");
	dp("ip:%s %d\n", ip, strlen(ip));
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

    if (mode != 2) {
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

	// ret = pthread_create(&tid_snap, NULL, get_snap_stream_user_thread, NULL);
	// if(ret != 0) {
	// 	IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_snap_stream_user_thread failed\n", __func__);
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

		ret = pthread_create(&tid_uart, NULL, uart_thread, NULL);
		if(ret != 0) {
			IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create uart_thread failed\n", __func__);
			return -1;
		}

		#else

			ret = pthread_create(&tid_stream, NULL, get_video_stream_test_thread, NULL);
			if(ret != 0) {
				IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create get_video_stream_user_thread failed\n", __func__);
				return -1;
			}
		#endif
		

		// ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread, NULL);
		// if(ret != 0) {
		// 	IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
		// 	return -1;
		// }

		ret = pthread_create(&tid_ai, NULL, IMP_Audio_Record_AEC_Thread, NULL);
		if(ret != 0) {
			IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Record_AEC_Thread failed\n", __func__);
			return -1;
		}

		if (settings.SF.bits.backlight) {
			ret = pthread_create(&tid_vmod, NULL, sample_soft_photosensitive_ctrl, NULL);
			if(ret != 0) {
				IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create sample_soft_photosensitive_ctrl failed\n", __func__);
				return -1;
			}
		}

	}	

	// ret = pthread_create(&tid_live, NULL, device_live_thread, NULL);
	// if(ret != 0) {
		// IMP_LOG_ERR("[Camera]", "[ERROR] %s: pthread_create device_live_thread failed\n", __func__);
		// return -1;
	// }

	// system("rm /tmp/mnt/sdcard/main*.jpg");
	// system("rm /tmp/mnt/sdcard/box*.jpg");
	ExpVal = Get_Brightness();
	if (settings.SF.bits.led) {
	    if (ExpVal > 70000) { // Noh Change 1000 -> 10000 20240530 -> 70000 20240712
	    	gpio_LED_Set(1);
	    }
	    else {
	    	// gpio_LED_Set(0);
	    }
	}

	// usleep(1000*1000);

	// amp_on();

	do {

#ifdef __STREAMING_CMD__
		// total_time = sample_gettimeus() - start_time;
		int cmd = 255; 

		dp("cmd 1  Audio IN/Out Vol, Gain Set!\n");
		dp("cmd 90 Reset Test\n");
		dp("cmd 99 : exit\n");

		cmd = scanf_cmd();
		ST_Flush();

		if (cmd == 1) {
			dp("cmd 1 Audio IN/Out Vol, Gain Set!\n");
			
			Get_Vol();
			int ai_vol = 100, ai_gain = 20, ao_vol = 100, ao_gain = 20;
			dp("Audio In Vol(0~120) : ");
			ai_vol = scanf_index();
			if (ai_vol < 0 || ai_vol > 120) {
				dp("Invalid Value!!\n");
				continue;
			}
			dp("Audio In gain(0~31) : ");
			ai_gain = scanf_index();
			if (ai_gain < 0 || ai_gain > 31) {
				dp("Invalid Value!!\n");
				continue;
			}
			dp("Audio Out Vol(0~120) : ");
			ao_vol = scanf_index();
			if (ao_vol < 0 || ao_vol > 120) {
				dp("Invalid Value!!\n");
				continue;
			}
			dp("Audio Out gain(0~31) : ");
			ao_gain = scanf_index();
			if (ao_gain < 0 || ao_gain > 31) {
				dp("Invalid Value!!\n");
				continue;
			}

			dp("Set Ai vol:%d\n", ai_vol);
			dp("Set Ai gain:%d\n", ai_gain);
			dp("Set Ao vol:%d\n", ao_vol);
			dp("Set Ao gain:%d\n", ao_gain);
			Set_Vol(ai_vol, ai_gain, ao_vol, ao_gain);

			if (!set_audio) {
				set_audio = true;
				amp_on();
				ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread_pcm, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
					return -1;
				}
			}
		}
	
		else if (cmd == 90) {
			dp("cmd 90 Reset Test\n");
			system("reboot");
		}
		else if (cmd == 99) {
			dp("Exiting Program! Plz Wait!\n");
			bExit = 1;
			bStrem = true;
			signal(SIGINT, handler);
			// break;
		}
#else
		if (get_audio && !set_audio) {
			set_audio = true;
			if (dn_g726_falg) {
				dp("Set G726 Audo Setting!! But PCM\n");
				// ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread_g726, NULL);
				// if(ret != 0) {
				// 	IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
				// 	return -1;
				// }
				ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread_pcm, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
					return -1;
				}
			}
			else {
				dp("Set PCM Audo Setting!!\n");
				ret = pthread_create(&tid_ao, NULL, IMP_Audio_Play_Thread_pcm, NULL);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create IMP_Audio_Play_Thread failed\n", __func__);
					return -1;
				}
			}
		}

		if (streaming_rec_state == REC_READY) {
			if (rec_end) {
				device_end(STREAMING);
				dp("Streaming Mode End!!\n");
				bUart = true;
				sleep(2);
				break;
			}
		}
		else if (streaming_rec_state == REC_RECONNECT) {
			dp("Stream Re:Connect!\n");
			system("rm /dev/shm/*.mp4");
			streaming_rec_state = REC_WAIT;
			
		}
		else if (streaming_rec_state == REC_ING) {
			rec_now = sample_gettimeus() - rec_time_s;
			if (rec_total + rec_now >= 60000000) {
            	rec_on = false;
            	rec_total += rec_time_s;
            	rec_mem_flag = true;
            	streaming_rec_state = REC_STOP;
            	streaming_rec_end(CAUSE_MEM);
            	// stream_state = 0;
			}

			if (rec_end) {
				if (rec_cnt > 0) {
					rec_time_e = sample_gettimeus()-rec_time_s;
					dp("Rec Time : %lld total : %lld\n", rec_time_e, rec_total);
	            	rec_on = false;
	            	rec_each_time[rec_cnt-1] = rec_time_e;
	            	rec_total += rec_time_s;
	            	rec_mem_flag = true;
	            	streaming_rec_state = REC_STOP;
	            	streaming_rec_end(CAUSE_MEM);
	            }
	            else {
	            	device_end(STREAMING);
					dp("Streaming Mode End!!\n");
					bUart = true;
					sleep(2);
					break;
	            }
			}
		}
		else if (streaming_rec_state == REC_WAIT) {
			if (rec_end) {
				streaming_rec_state = REC_MP4MAKE;
				rec_end = false;
				stream_state = 0;
			}			
		}
		else if (streaming_rec_state == REC_MP4MAKE) {
			pthread_t tid_makerec[10];
			bool make_flag = false;
			Make_File mfd_rec[10];

			make_file_start(STREAMING);

			if (rec_cnt > 10) rec_cnt = 10;
			for (int i=0; i<rec_cnt; i++) {
				mfd_rec[i].type = 0;
				mfd_rec[i].index = i;
				if (rec_each_time[i] < 23000000)
					mfd_rec[i].cnt = 1;
				else if(rec_each_time[i] < 43000000)
					mfd_rec[i].cnt = 2;
				else
					mfd_rec[i].cnt = 3;

				file_each_cnt[i] = mfd_rec[i].cnt;

				rfile_flag[i] = false;

				ret = pthread_create(&tid_makerec[i], NULL, make_mp4_streamrec,(void*)&mfd_rec[i]);
				if(ret != 0) {
					IMP_LOG_ERR("[Audio]", "[ERROR] %s: pthread_create make_mp4_streamrec %d failed\n", __func__, i);
					return -1;
				}
			}

			while(!make_flag) {
				for (int i=0; i<rec_cnt;) {
					if (rfile_flag[i] == false) {
						i = rec_cnt;
					}
					else {
						i++;
						if (rec_cnt == i) {
							make_flag = true;
						}
					}
				}
			}
			if (streaming_rec_state == REC_MP4MAKE)
				streaming_rec_state = REC_SPISEND;
		}
		else if (streaming_rec_state == REC_SPISEND) {
			for (int i=0; i<rec_cnt; i++) {
				FileSend fs;
				if (stream_tag[i] == CLIP_CAUSE_STREM) {
					fs.minor = REC_STREAMING_M;
					fs.tag1 = CLIP_CAUSE_STREM;
					fs.tag2 = CLIP_STREAM_REC;
				}
				else {
					fs.minor = REC_CLIP_F;
					fs.tag1 = CLIP_CAUSE_BELL;
					fs.tag2 = CLIP_BELL_CALL;
				}
				fs.filenum = i;
				fs.filecnt = file_each_cnt[i];
				if (i == 0 || (Ready_Busy_Check() > 0)) { 
					ret = spi_send_total_stream_clip(&fs);
					if(ret < 0) {
						dp("File Send Fail!!\n");
					}
					else if (ret == 2) {
						dp("Stream Re:connect!\n");
					}
				}
				else {
					dp("Start RB Faile!\n");
				}
			}
			if (streaming_rec_state == REC_RECONNECT) {
				continue;
			}
			else {
				device_end(STREAMING);
				dp("Streaming Mode End!!\n");
				bUart = true;
				sleep(2);
				break;
			}
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
	// pthread_join(tid_snap, NULL);

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
    //     dp("spi init error\n");
    //     return 0;
    // }

    // data_sel = 4;
	// if (data_sel <= 0 || data_sel > 4) {
	// 	dp("Invalid Type!\n");
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
	// dp("ip:%s %d\n", ip, strlen(ip));
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
	dp("Thread Start!\n");

	do {
		if (dimming) {
			dimming_e =  sample_gettimeus() - dimming_s;
			if ((dimming_e > 15000) & dimming_up) {
				dimming_s = sample_gettimeus();
				dimming_val -= 1;
				LED_dimming (dimming_val);
				if (dimming_val <= 40) {
					dimming_up = false;
				}
			}
			else if ((dimming_e > 15000) & !dimming_up) {
				dimming_s = sample_gettimeus();
				dimming_val += 1;
				LED_dimming (dimming_val);
				if (dimming_val >= 198) {
					dimming_up = true;
				}
			}

			// dp("dimming val : %d\n", dimming_val);
		}

		if (door_cap_flag){
			if (!cap_start) {
				dp("[door] cap init!\n");
				ret = spi_init();
    			if(ret < 0){
    			    dp("spi init error\n");
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
				if (Ready_Busy_Check() > 0){
					dp("doorCap Send Start!\n");
					memset(file_path, 0, 128);
					sprintf(file_path, "/dev/shm/main0.jpg");
					spi_send_file(REC_DOOR_SNAP, file_path, 0, 0, 0);
				}
				else {
					dp("Fail to Send main0.jpg\n");
				}
				door1 = false;
			}

			if (!box_snap && door2) {
				dp("Box Origin File!!\n");
				system("cp /dev/shm/box0.jpg /tmp/mnt/sdcard/box_origin.jpg");
				system("cp /dev/shm/box0.jpg /tmp/mnt/sdcard/box_before.jpg");
				system("sync");
				door2 = false;
			}

			if (!door1 && !door2) {
				dp("[END] Door Shot!\n");
				device_end(SETTING);
				door_cap_flag = false;
			}
		}

		if (save_send_flag) {

			if (!ubi_start) {
				ret = spi_init();
    			if(ret < 0){
    			    dp("spi init error\n");
    			    return 0;
    			}
				system("ubi_mount");

				ubi_start = true;
			}

			usleep(500*1000);

			file_name_get(&Save_movie1, 0);
			file_name_get(&Save_movie2, 1);

			for(int i=0;i<Save_movie1.cnt;i++) {
				dp("File[%d]:%s\n", i, Save_movie1.name[i]);
				spi_send_save_file("/maincam/", Save_movie1.name[i]);
				dp("File[%d]:%s\n", i, Save_movie2.name[i]);
				spi_send_save_file("/boxcam/", Save_movie2.name[i]);
			}

			// system("rm /maincam/*");
			// system("rm /boxcam/*");

			system("sync");

			dp("[END] Save Send!\n");
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
			usleep(1000*1000);
			dp("[END] CMD Send!\n");
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

void *dimming_test(void *argc) {
	int64_t dimming_e = 0;
    int dimming_val = 90;
    bool dimming_up = true;

    gpio_LED_dimming(2);

	do {
		if (dimming) {
			dimming_e =  sample_gettimeus() - dimming_s;
			if ((dimming_e > 15000) & dimming_up) {
				dimming_s = sample_gettimeus();
				dimming_val -= 1;
				LED_dimming (dimming_val);
				if (Dimming_end) {
					bDimming = true;
					break;					
				}
				if (dimming_val <= 40) {
					dimming_up = false;
				}
			}
			else if ((dimming_e > 15000) & !dimming_up) {
				dimming_s = sample_gettimeus();
				dimming_val += 1;
				LED_dimming (dimming_val);
				if (dimming_val >= 198) {
					dimming_up = true;
				}
			}

			// dp("dimming val : %d\n", dimming_val);
		}
	} while(!bDimming);

	gpio_LED_dimming(0);

	return ((void*)0);
}

void *bottom_led_test(void *argc) {
	int64_t onoff_time = 0;
    bool bled_onoff = true;
    int ret = -1;

    ret = gpio_set_val(PORTD+6, bled_onoff);
    if(ret < 0){
		dp("Fail set Value GPIO : %d\n", PORTD+6);
		return ((void*)0);
	}

    onoff_time = sample_gettimeus();
	do {
		if ( (sample_gettimeus() - onoff_time) > 1000000 ) {
			onoff_time = sample_gettimeus();
			bled_onoff ^= 1;
			ret = gpio_set_val(PORTD+6, bled_onoff);
		    if(ret < 0){
				dp("Fail set Value GPIO : %d\n", PORTD+6);
				break;
			}
		}
		
	} while(!bBLed);

	ret = gpio_set_val(PORTD+6, 1);
    if(ret < 0){
		dp("Fail set Value GPIO : %d\n", PORTD+6);
		return ((void*)0);
	}

	return ((void*)0);
}