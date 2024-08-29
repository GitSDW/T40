/**********************************************************
 * Author        : kaiwang
 * Email         : kai.wang@ingenic.com
 * Last modified : 2020-09-17 20:03
 * Filename      : sample_uart.c
 * Description   : 
 * *******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <arpa/inet.h>
#include <linux/types.h>

#include "uart.h"
#include "spi.h"
#include "global_value.h"
#include "c_util.h"
#include "setting.h"
#include "audio.h"
#include "video.h"

const char default_path[] = "/dev/ttyS2";
int fd_uart;
bool ack_flag = false;

/**
* uart init
* */

// int uart_init(int baudrate,int databits,int stopbits,char parity)
// {
//     int i = 0;
//     /*
//      * 获取串口设备描述符
//      * Get serial device descriptor
//      * */    
//     dp("This is tty/usart demo.\n");
//     fd = open(path, O_RDWR);
//     if (fd < 0) {
//         dp("Fail to Open %s device\n", path);
//         return -1;
//     }
//     struct termios opt;
//     /*
//      * 清空串口接收缓冲区
//      * Clear the serial port receive buffer
//      * */
//     tcflush(fd, TCIOFLUSH);
//     tcgetattr(fd, &opt);
//     /**
//      * 设置串口波特率
//      * Set the serial port baud rate
//      * */
//     for(i = 0; i < sizeof(baud_rate)/sizeof(int) ;i++){
//         if(baud_rate_num[i] == baudrate){
//             cfsetospeed(&opt, baud_rate[i]);
//             cfsetispeed(&opt, baud_rate[i]);
//         }
//     }
//     /*
//      * 设置数据位数,先清除CSIZE数据位的内容
//      * Set the number of data bits, first clear the content of the CSIZE data bit
//      * */    
//     opt.c_cflag &= ~CSIZE;
//     switch (databits)
//     {
//     case 5:
//         opt.c_cflag |= CS5;
//         break;
//     case 6:
//         opt.c_cflag |= CS6;
//         break;
//     case 7:
//         opt.c_cflag |= CS7;
//         break;
//     case 8:
//         opt.c_cflag |= CS8;
//         break;
//     }
//     /* 
//      * 设置停止位
//      * Set stop bit
//      **/
//     switch (stopbits)
//     {
//     case 1:
//         opt.c_cflag &= ~CSTOPB;//1位 1 bit
//         break;
//     case 2:
//         opt.c_cflag |= CSTOPB;//2位 2 bit
//         break;
//     }
//     /*
//      * 设置校验位
//      * Set check digit
//      * */
//     switch (parity)
//     {
//     case 'n':
//     case 'N':
//         opt.c_cflag &= ~PARENB; // 不使用奇偶校验 
//         opt.c_iflag &= ~INPCK;  // 禁止输入奇偶检测
//         break;
//     case 'o':
//     case 'O':
//         opt.c_cflag |= PARENB;  // 启用奇偶效验 
//         opt.c_iflag |= INPCK;   // 启用输入奇偶检测 
//         opt.c_cflag |= PARODD ; // 设置为奇效验 
//         break;
//     case 'e':
//     case 'E':
//         opt.c_cflag |= PARENB;  // 启用奇偶效验
//         opt.c_iflag |= INPCK;   // 启用输入奇偶检测
//         opt.c_cflag &= ~PARODD; // 设置为偶效验
//         break;
//     }

//     /*
//      *更新配置
//      *Update configuration
//      **/    
//     tcsetattr(fd, TCSANOW, &opt);

//     dp("Device %s is set to %d bps,databits %d,stopbits %d,parity %c\n",path,baudrate,databits,stopbits,parity);
//     return 0;
// }

int uart_init(void) {
    struct termios tty;

    // 시리얼 포트 열기
    fd_uart = open("/dev/ttyS2", O_RDWR);
    if (fd_uart < 0) {
        perror("Unable to open serial port");
        return -1;
    }

    /*
     * 清空串口接收缓冲区
     * Clear the serial port receive buffer
     * */
    // tcflush(fd_uart, TCIOFLUSH);
    tcgetattr(fd_uart, &tty);

    // 현재 시리얼 포트 설정 가져오기
    // if(tcgetattr(fd, &tty) != 0) {
    //     perror("tcgetattr");
    //     return -1;
    // }

    // 시리얼 통신 속도 설정 (2560000 bps)
    // cfsetospeed(&tty, B2500000);
    // cfsetispeed(&tty, B2500000);
    // cfsetospeed(&tty, B115200);
    // cfsetispeed(&tty, B115200);
    // B115200 B921600 B460800 B576000 B921600

    // 통신 속성 설정
    tty.c_cflag = B115200;
    tty.c_cflag |= CS8;      // 8 비트 데이터 비트 크기
    // tty.c_cflag &= ~CSIZE;   // 데이터 비트 크기 비트를 지웁니다.
    tty.c_cflag |= CLOCAL; // 읽기를 활성화하고 로컬 라인을 유지합니다.
    tty.c_cflag |= CREAD; // 읽기를 활성화하고 로컬 라인을 유지합니다.
    tty.c_iflag = IGNPAR;
    tty.c_oflag = 0;
    tty.c_lflag = 0;
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 1;


    // 설정 적용
    if (tcsetattr(fd_uart, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        return -1;
    }

    // tty.c_cflag |= (CLOCAL | CREAD); // 로컬 제어 및 읽기 활성화
    // tty.c_cflag &= ~PARENB; // 패리티 비트 비활성화
    // tty.c_cflag &= ~CSTOPB; // 1개의 스탑 비트 설정
    // tty.c_cflag &= ~CSIZE; // 데이터 비트의 크기를 지정하기 위해 비트를 지운다.
    // tty.c_cflag |= CS8; // 8개의 데이터 비트
    // tcsetattr(fd_uart, TCSANOW, &tty); // 옵션 설정 적용

   

    return 1;
}

/**
 * @brief 串口发送函数
 *
 *
 * */

pthread_mutex_t uart_vm = PTHREAD_MUTEX_INITIALIZER;




const int uart_send(int fd,uint8_t *send_buf,int data_len)
{
    int len = 0;

    pthread_mutex_lock(&uart_vm);
    // dp("UART:");
    // for(int i=0; i<data_len; i++) {
    //     dp("0x%02x ", send_buf[i]);
    // }
    // dp("\n");
    len = write(fd,send_buf,data_len);
    if(len == data_len){
        pthread_mutex_unlock(&uart_vm);
        usleep(100*1000);
        return len;
    } else{
        tcflush(fd,TCIOFLUSH);
        pthread_mutex_unlock(&uart_vm);
        return -1;
    }
    pthread_mutex_unlock(&uart_vm);
    return 0;
}

uint8_t ack_major = 0;
uint8_t ack_minor = 0;

int Make_Uart_Ack(uint8_t *tbuff, int len, uint8_t *data, uint8_t major, uint8_t minor)
{
    memset(tbuff, 0, len+10);
    tbuff[0] = 0x02;
    tbuff[1] = (major & 0x7F) & 0xFF;
    tbuff[2] = (minor | 0x80) & 0xFF;
    tbuff[3] = (len>>8) & 0xFF;
    tbuff[4] = len & 0xFF;
    tbuff[5] = 0x00;
    tbuff[6] = 0x00;
    tbuff[7] = 0x00;
    tbuff[8] = 0x00;
    if (len > 0) {
        memcpy(&tbuff[9], data, len);
    }
    tbuff[9+len] = 0x03;

    return 10+len;
}



int Make_Packet_uart(uint8_t *tbuff, uint8_t *data, uint16_t len, uint8_t major, uint8_t minor)
{
    memset(tbuff, 0, len+10);
    tbuff[0] = 0x02;
    tbuff[1] = major & 0xFF;
    tbuff[2] = minor & 0xFF;
    tbuff[3] = (len>>8) & 0xFF;
    tbuff[4] = len & 0xFF;
    tbuff[5] = 0x00;
    tbuff[6] = 0x00;
    tbuff[7] = 0x00;
    tbuff[8] = 0x00;

    switch(major){
        case DTEST:
            switch(minor){
                case TEST_START:
                    break;
                case TEST_STOP:
                    break;
                default:
                    return -1;
                    break;
            }
            break;
        case REC:
            switch(minor){
                case REC_DEV_START:
                    break;
                case REC_STREAM_STR:
                    memcpy(&tbuff[9], data, len);
                    break;
                case REC_CLIP_F:
                case REC_CLIP_B:
                    memcpy(&tbuff[9], data, len);
                    break;
                case REC_FACE:
                    break;
                case REC_BOX_ALM:
                    break;
                case REC_STREAM_END:
                    break;
                case REC_ACK:
                    break;
                case REC_DEV_STOP:
                    break;
                default:
                    return -1;
                    break;
            }
            break;
    case STREAMING:
            switch(minor){
                case STREAM_START:
                    break;
                case STREAM_REV:
                    break;
                case STREAM_VEDIO_M:
                    memcpy(&tbuff[9], data, len);
                    break;
                case STREAM_VEDIO_B:
                    memcpy(&tbuff[9], data, len);
                    break;
                case STREAM_FACE:
                    break;
                case STREAM_AUDIO_F:
                    memcpy(&tbuff[9], data, len);
                    break;
                case STREAM_STOP:
                    break;
                default:
                    return -1;
                    break;
            }
            break;
        break;
    case SETTING:
        switch(minor) {
            case SET_FILE_START:
                // dp("Start Pck Make!\n");
                break;
            case SET_FILE_SEND:
                // dp("Start Pck Make!\n");
                break;
            case SET_FILE_END:
                // dp("Start Pck Make!\n");
                break;
        }
        break;
    default:
        return -1;
        break;
    }
    tbuff[len + 9] = 0x03;
    // tbuff[1023] = 0x03;
    return 10+len;
}

extern int gpio_LED_Set(int onoff);
extern int gpio_LED_dimming (int onoff);
int door_set_fail(void);



static int Recv_Uart_Packet_live(uint8_t *rbuff) {
    int index, len, value_buf, ack_len, res;
    uint8_t major, minor, buf_8;
    int bad_cnt = 0;
    uint8_t *uart_tx;
    uint8_t ack_data[256];
    // static int64_t rec_time_s = 0;
    int64_t rec_time_e = 0;
    uint32_t br_buf;
    char* effect_file = NULL;

    index = 0;

    major = rbuff[index+1];
    minor = rbuff[index+2];
    if (rbuff[index] != 0x02) {
        // dp("S\n");
        bad_cnt++;
    }
    
    buf_8 = major&0x80;
    if (buf_8 == 0) {
        // dp("M\n");
        bad_cnt++;
    } 

    // if (minor != 0x07) {
    //     bad_cnt++;
    // }

    if (bad_cnt > 1) {
        return -1;
    }

    len = rbuff[index+3]*256 + rbuff[index+4];

    dp("PKT Len : %d\n", len);

    ack_flag = false;

    ack_major = major;
    ack_minor = minor;

    switch(major) {
    case REC_BACK:
        switch(minor) {
        case UREC_BELL:
            if (settings.bell_type == 0) effect_file = "/tmp/mnt/sdcard/effects/bell1.wav";
            else if (settings.bell_type == 1) effect_file = "/tmp/mnt/sdcard/effects/bell2.wav";
            else if (settings.bell_type == 2) effect_file = "/tmp/mnt/sdcard/effects/bell3.wav";
            else if (settings.bell_type == 2) effect_file = "/tmp/mnt/sdcard/effects/bell4.wav";
            dp("play : %s\n", effect_file);
            // ao_file_play_thread(effect_file);
            // clip_cause_t.Major = CLIP_CAUSE_BOX;
            // clip_cause_t.Minor = CLIP_BOX_OCCUR;
            // bell_rec_state = REC_START;

            if (bellend_sound == 0) bellend_sound++;

            if (stream_state == 1) {
                rebell = true;
                dp("Bell Stream : %d\n", stream_state);
                ao_file_play_thread(effect_file);
                break;
            }
            
            ack_len = 0;
            // ack_flag = true;
            // dp("av_off : %d\n", av_off_flag);
            if (av_off_flag) {
                if (bl_state == BSS_WAIT) {
                    dp("Bell Flag!!\n");
                    bell_flag = true;
                    bell_call_flag = false;
                    // bell_rec_state = REC_START;
                }
            }
            else if (bell_rerecode_flag) {
                rec_enable_ack();
            }
            else {
                dp("ReBell!\n");
            }

            if (!get_audio) {
                get_audio = true;
                // if (rbuff[index+9] > 0)     dn_g726_falg = false;
                // else                        dn_g726_falg = true;
                dn_g726_falg = true;
            }

            ao_file_play_thread(effect_file);
        break;
        case UREC_BELL_MUTE:
            // clip_cause_t.Major = CLIP_CAUSE_BOX;
            // clip_cause_t.Minor = CLIP_BOX_OCCUR;
            // bell_rec_state = REC_START;
            bell_flag = true;
            bell_call_flag = false;
            ack_len = 0;
            // ack_flag = true;
        break;
        case UREC_RETRY:
            dp("File Send Retry!!\n");
            send_retry_flag = true;
        break;
        case  UREC_FACE:
            if (rbuff[index+9] == 1) {
                dp("Streaming Rec Face Detection!\n");
                clip_cause_t.Major = CLIP_CAUSE_FACE;
                clip_cause_t.Minor = CLIP_FACE_DET; 
            }
            ack_len = 0;
            // ack_flag = true;
        break;
        case UREC_STREAM:
            if (boot_mode == 1) {
                #ifndef __STREAMING_CMD__
                    Set_Vol(90,30,spk_vol_buf,spk_gain_buf);
                #endif
                stream_state = 1;
                rec_streaming_state = REC_START;
                bell_stream_flag = true;
                audio_spi_flag = true;
                if (Rec_type == BELL_REC || Rec_type == BELL_REREC) {
                    bell_call_flag = true;
                }
                ack_len = 0;
                dp("Stream start!\n");
                file_21_flag = false;
                // ack_flag = true;
            }
            else if (boot_mode == 2) {
                stream_state = 1;
                if (streaming_rec_state != REC_READY)
                    streaming_rec_state = REC_RECONNECT;
            }

            if (!get_audio) {
                get_audio = true;
                // if (rbuff[index+9] > 0)     dn_g726_falg = false;
                // else                        dn_g726_falg = true;
                dn_g726_falg = true;
            }
        break;

        case UREC_STREAM_END:
            if (boot_mode == 1) {
                stream_state = 0;
                rec_streaming_state = REC_STOP;
                ack_len = 0;
                // ack_flag = true;
                rec_end = true;
                if (bellend_sound == 1) bellend_sound++;
                if (bell_stream_flag) {
                    bell_stream_flag = false;
                }
            }
            else if (boot_mode == 2) {
                stream_state = 0;
                rec_end = true;
                streaming_rec_state = REC_ING;
            }
        break;
        case UREC_TEMPER:
            if (rbuff[index+9] == 0)
                effect_file = "/tmp/mnt/sdcard/effects/dev_takeoff.wav";
            else if (rbuff[index+9] == 1)
                effect_file = "/tmp/mnt/sdcard/effects/dev_takeon.wav";
            dp("play : %s\n", effect_file);
            Set_Vol(90,30,(10 * 1) + 55,15);
            ao_file_play_thread(effect_file);

            // clip_cause_t.Major = CLIP_CAUSE_MOUNT;
            // clip_cause_t.Minor = CLIP_MOUNT_DISMT;
            if (Rec_type != BELL_REC)
                bell_flag = true;
            temp_flag = true;
            if (rbuff[index+9] > 0)     temp_unmount_flag = true;
            else                        temp_unmount_flag = 0;
            ack_len = 0;
            // ack_flag = true;
        break;
        
        case REC_DATE:
            if (!get_audio) {
                get_audio = true;
                // if (rbuff[index+9] > 0)     dn_g726_falg = false;
                // else                        dn_g726_falg = true;
                dn_g726_falg = true;
            }

            snprintf(TimeStamp.date, 13, "%s\n", (char*)&rbuff[index+9+1]);
            dp("1 dn_g726_falg: %d Date : %s\n", dn_g726_falg, TimeStamp.date);
            ack_len = 0;
            // ack_flag = true;
        break;
        }
    break;
    case STREAMING_BACK:
        switch(minor) {
        case USTREAM_LIGHT:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                gpio_LED_Set(value_buf);
                ack_len = 0;
                // ack_flag = true;
            }

            dp("Light %d\n", value_buf);
        break;
        case USTREAM_REC_S:
            if (boot_mode == 0x00 || boot_mode == 0x03) {
                break;
            }
            dp("Streaming Rec Start!\n");

            if (rec_on) {
                ack_len = 1;
                ack_data[0] = 2;
                // ack_flag = true;
            }
            else if (rec_mem_flag) {
                ack_len = 1;
                ack_data[0] = 3;
                // ack_flag = true;
                streaming_rec_end(CAUSE_MEM);
            }
            else {
                rec_time_s = sample_gettimeus();
                // if (boot_mode == 0x01) {
                //     Rec_type = STRM_REC;
                // }
                streaming_rec_state = REC_START;
                rec_on = true;
                stream_tag[rec_cnt] = CLIP_CAUSE_STREM;
                rec_cnt++;
                dp("Rec Start : %d\n", rec_cnt);
                ack_len = 1;
                ack_data[0] = 1;
                // ack_flag = true;
            }
        break;
        case USTREAM_REC_E:
            if (boot_mode == 0x00 || boot_mode == 0x03) {
                break;
            }
            if (Rec_type == BELL_REC) {
                rec_streaming_state = REC_STOP;
                rec_end = true;
            }
            else {
                dp("Streaming Rec End!\n");
                rec_time_e = sample_gettimeus()-rec_time_s;
                dp("Rec Filecnt : %d Time : %lld total : %lld\n", rec_cnt, rec_time_e, rec_total);
                rec_each_time[rec_cnt-1] = rec_time_e;
                rec_total += rec_time_e;
                streaming_rec_state = REC_STOP;
                rec_on = false;
                ao_clear_flag = true;
                ack_len = 0;
                audio_spi_flag = false;

                if (bellend_sound == 1) bellend_sound++;
                
                if (rec_total > 57000000) {
                    usleep(10*1000);
                    streaming_rec_end(CAUSE_MEM);
                }
                else if (rec_cnt >= 9) {
                    usleep(10*1000);
                    streaming_rec_end(CAUSE_FILE);
                }

            }
            ack_len = 0;
            // ack_flag = true;
        break;
        case USTREAM_BELCAL:
            if (boot_mode == 0x00 || boot_mode == 0x03) {
                break;
            }
            dp("Streaming Bell Call Start!\n");
            if (rec_on) {
                ack_len = 1;
                ack_data[0] = 2;
                // ack_flag = true;
            }
            else if (rec_mem_flag) {
                ack_len = 1;
                ack_data[0] = 3;
                // ack_flag = true;
                streaming_rec_end(CAUSE_MEM);
            }
            else {
                #ifndef __STREAMING_CMD__
                    Set_Vol(90,30,spk_vol_buf,spk_gain_buf);
                #endif
                rec_time_s = sample_gettimeus();
                // if (boot_mode == 0x01) {
                //     Rec_type = STRM_REC;
                // }
                audio_spi_flag = true;
                if (Rec_type == BELL_REC) {
                     bell_call_flag = true;
                }
                else {
                    streaming_rec_state = REC_START;
                    rec_on = true;
                    stream_tag[rec_cnt] = CLIP_CAUSE_BELL;
                    rec_cnt++;
                    dp("Bell Start : %d\n", rec_cnt);
                }

                if (!get_audio) {
                    get_audio = true;
                    // if (rbuff[index+9] > 0)     dn_g726_falg = false;
                    // else                        dn_g726_falg = true;
                    dn_g726_falg = true;
                }

                if (bellend_sound == 1) bellend_sound++;

                ack_len = 1;
                ack_data[0] = 1;
                // ack_flag = true;
            }
        break;
        case USTREAM_F_SEND:
            stream_state = 0;
            rec_streaming_state = REC_STOP;
            bell_stream_flag = false;
            ack_len = 0;
            // ack_flag = true;
            rec_end = true;

        break;
        case USTREAM_BITRATE:
            if (rbuff[index+9] > 0 && rbuff[index+9] < 6) {
                br_buf = rbuff[index+9]*100;
                Set_Target_Bit(br_buf);
            }
            ack_len = 0;
            // ack_flag = true;
        break;
        case USTREAM_BLE_LT:
            dp("Ble Light Set!\n");
            gpio_LED_dimming(rbuff[index+9]);
            ack_len = 0;
            // ack_flag = true;
        break;
        case STREAM_DATE:
            if (!get_audio) {
                get_audio = true;
                // if (rbuff[index+9] > 0)     dn_g726_falg = false;
                // else                        dn_g726_falg = true;
                dn_g726_falg = true;
            }

            snprintf(TimeStamp.date, 13, "%s\n", (char*)&rbuff[index+9+1]);
            dp("2 dn_g726_falg : %d Date : %s\n", dn_g726_falg, TimeStamp.date);
            ack_len = 0;
            // ack_flag = true;
        break;
        }
    break;
    case SETTING_BACK:
        switch(minor) {
        case SET_LED:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.led = value_buf;
                Setting_Save();
                dp("Auto LED Setting Saved! %d\n", settings.SF.bits.led);
                system("sync");
                ack_len = 0;
                // ack_flag = true;
                cmd_end_flag = true;
            }
        break;
        case SET_BELL_VOL:
            if (len > 0) {
                value_buf = rbuff[index+9];
                settings.bell_type = value_buf;
                Setting_Save();
                dp("Bell Type Setting Saved! %d\n", settings.bell_type);
                system("sync");
                ack_len = 0;
                // ack_flag = true;

                if (settings.bell_type == 0) effect_file = "/tmp/mnt/sdcard/effects/bell1.wav";
                else if (settings.bell_type == 1) effect_file = "/tmp/mnt/sdcard/effects/bell2.wav";
                else if (settings.bell_type == 2) effect_file = "/tmp/mnt/sdcard/effects/bell3.wav";
                else if (settings.bell_type == 2) effect_file = "/tmp/mnt/sdcard/effects/bell4.wav";
                dp("play : %s\n", effect_file);
                ao_file_play_thread(effect_file);
                cmd_end_flag = true;


            }
        break;
        case SET_SPK_VOL:
            if (len > 0) {
                value_buf = rbuff[index+9];
                settings.spk_vol = value_buf;
                Setting_Save();
                dp("Speaker Vol Setting Saved! %d\n", settings.spk_vol);
                system("sync");
                ack_len = 0;
                // ack_flag = true;
                cmd_end_flag = true;
            }
        break;
        case SET_BACK_LIGHT:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.backlight = value_buf;
                Setting_Save();
                dp("Back Light Setting Saved! %d\n", settings.SF.bits.backlight);
                system("sync");
                ack_len = 0;
                // ack_flag = true;
                cmd_end_flag = true;
            }
        break;
        case SET_FLICKER:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.flicker = value_buf;
                Setting_Save();
                dp("Flicker Setting Saved! %d\n", settings.SF.bits.flicker);
                system("sync");
                ack_len = 0;
                // ack_flag = true;
                cmd_end_flag = true;
            }
        break;
        case SET_MOVE_SENSI:
            if (len > 0) {
                value_buf = rbuff[index+9];
                settings.move_sensitivty = value_buf;
                Setting_Save();
                dp("Move Sensitivity Setting Saved! %d\n", settings.move_sensitivty);
                system("sync");
                ack_len = 0;
                // ack_flag = true;
                cmd_end_flag = true;
            }
        break;
        case SET_EX_ONOF:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.move_ex = value_buf;
                Setting_Save();
                dp("Move Ex On/Off Setting Saved! %d\n", settings.SF.bits.move_ex);
                system("sync");
                ack_len = 0;
                // ack_flag = true;
                cmd_end_flag = true;
            }
        break;
        case SET_FACE_MOSAIC:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.per_face = value_buf;
                Setting_Save();
                dp("Face Mosaic Setting Saved! %d\n", settings.SF.bits.per_face);
                system("sync");
                ack_len = 0;
                // ack_flag = true;
                cmd_end_flag = true;
            }
        break;
        case SET_DOOR_ONOF:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.door_g = value_buf;
                Setting_Save();
                dp("Door Grid On/Off Setting Saved! %d\n", settings.SF.bits.door_g);
                system("sync");
                ack_len = 0;
                // ack_flag = true;
                cmd_end_flag = true;
            }
        break;
        case SET_USER_ONOF:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.user_g = value_buf;
                Setting_Save();
                dp("User Grid On/Off Setting Saved! %d\n", settings.SF.bits.user_g);
                system("sync");
                ack_len = 0;
                // ack_flag = true;
                cmd_end_flag = true;
            }
        break;
        case SET_EX_AREA:
            settings.move_ex_s_x = (rbuff[index+9]*0x100)+(rbuff[index+9+1]);
            settings.move_ex_s_y = (rbuff[index+9+2]*0x100)+(rbuff[index+9+3]);
            settings.move_ex_e_x = (rbuff[index+9+4]*0x100)+(rbuff[index+9+5]);
            settings.move_ex_e_y = (rbuff[index+9+6]*0x100)+(rbuff[index+9+7]);
            Setting_Save();
            ack_len = 0;
            // ack_flag = true;
            cmd_end_flag = true;
        break;
        case SET_DOOR_GRID:
            memcpy(settings.door_grid, &rbuff[index+9], 27);
            Setting_Save();
            ack_len = 0;
            // ack_flag = true;
            cmd_end_flag = true;
        break;
        case SET_USER_GRID:
            memcpy(settings.user_grid, &rbuff[index+9], 27);
            Setting_Save();
            ack_len = 0;
            // ack_flag = true;

            cmd_end_flag = true;
        break;
        case SET_DOOR_CAP:
            if (boot_mode != 0x03)  {
                usleep(10*1000);
                door_set_fail();
                break;
            }
            door_cap_flag = true;
            ack_len = 0;
            // ack_flag = true;
        break;
        case SET_DATE:
            if (rbuff[index+9] > 0)     netwrok_busy = true;
            else                        netwrok_busy = false;

            snprintf(TimeStamp.date, 13, "%s\n", (char*)&rbuff[index+9+1]);
            dp("Set Busy : %d Date : %s\n", netwrok_busy, TimeStamp.date);
            ack_len = 0;
            // ack_flag = true;
        break;
        case SET_SAVE_SEND:
            save_send_flag = true;
            ack_len = 0;
            // ack_flag = true;
        break;
        case SET_FACTORY:
            Setting_Reinit();
            Set_Vol(90,30,(10 * 1) + 55,15);
            effect_file = "/tmp/mnt/sdcard/effects/factory.wav";
            dp("play : %s\n", effect_file);
            ao_file_play_thread(effect_file);
            
            ack_len = 0;
            // ack_flag = true;
            cmd_end_flag = true;
        break;
        case SET_BLE_LT:
            dp("Ble Light Set!\n");
            int cmdbuf = 0;
            if (rbuff[index+9] == 0) cmdbuf = 0;
            else if (rbuff[index+9] == 1) cmdbuf = 1;
            else if (rbuff[index+9] == 2) cmdbuf = 2;
            else  cmdbuf = 2;
            gpio_LED_dimming(cmdbuf);
            if (rbuff[index+9] == 1) {
                ao_file_play_thread_mute("/tmp/mnt/sdcard/effects/pairing.wav");
                Set_Vol(90,30,(10 * 1) + 55,15);
                effect_file = "/tmp/mnt/sdcard/effects/dev_start.wav";
                dp("play : %s\n", effect_file);
                ao_file_play_thread(effect_file);
            }
            else if (rbuff[index+9] == 2) {
                ao_file_play_thread_mute("/tmp/mnt/sdcard/effects/pairing.wav");
                Set_Vol(90,30,(10 * 1) + 55,15);
                effect_file = "/tmp/mnt/sdcard/effects/pairing.wav";
                dp("play : %s\n", effect_file);
                ao_file_play_thread(effect_file);
            }
            ack_len = 0;
            // ack_flag = true;
        break;
        case SET_OTA_START:
            bOTA = false;
            ota_type_u = rbuff[index+9];
            ota_flag = true;
            ack_len = 0;
            // ack_flag = true;
        break;
        case SET_DEV_START:
            ack_len = 0;
            // ack_flag = true;
            Set_Vol(90,30,(10 * 1) + 55,15);
            effect_file = "/tmp/mnt/sdcard/effects/dev_start.wav";
            dp("play : %s\n", effect_file);
            ao_file_play_thread(effect_file);
            
            cmd_end_flag = true;
        break;
        case SET_DEV_OFF:
            ack_len = 0;
            // ack_flag = true;
            Set_Vol(90,30,(10 * 1) + 55,15);
            effect_file = "/tmp/mnt/sdcard/effects/dev_end.wav";
            dp("play : %s\n", effect_file);
            ao_file_play_thread(effect_file);
            
            sleep(5);
            cmd_end_flag = true;
        break;
        case SET_FACTORY_SND:
            ack_len = 0;
            // ack_flag = true;

            effect_file = "/tmp/mnt/sdcard/effects/factory.wav";
            dp("play : %s\n", effect_file);
            ao_file_play_thread(effect_file);
            // sleep(5);
            // cmd_end_flag = true;
        break;
        case SET_DEV_BATT:
            ack_len = 0;
            // ack_flag = true;
            
            if (rbuff[index+9] == 1) effect_file = "/tmp/mnt/sdcard/effects/batt01.wav";
            else if (rbuff[index+9] == 2) effect_file = "/tmp/mnt/sdcard/effects/batt02.wav";
            else if (rbuff[index+9] == 3) effect_file = "/tmp/mnt/sdcard/effects/batt03.wav";
            else if (rbuff[index+9] == 4) effect_file = "/tmp/mnt/sdcard/effects/batt04.wav";
            else if (rbuff[index+9] == 5) effect_file = "/tmp/mnt/sdcard/effects/batt05.wav";
            else if (rbuff[index+9] == 6) effect_file = "/tmp/mnt/sdcard/effects/batt06.wav";
            else if (rbuff[index+9] == 7) effect_file = "/tmp/mnt/sdcard/effects/batt07.wav";
            else if (rbuff[index+9] == 8) effect_file = "/tmp/mnt/sdcard/effects/batt08.wav";
            else if (rbuff[index+9] == 9) effect_file = "/tmp/mnt/sdcard/effects/batt09.wav";
            else if (rbuff[index+9] == 10) effect_file = "/tmp/mnt/sdcard/effects/batt10.wav";
            else if (rbuff[index+9] == 11) effect_file = "/tmp/mnt/sdcard/effects/batt11.wav";
            else if (rbuff[index+9] == 12) effect_file = "/tmp/mnt/sdcard/effects/batt12.wav";
            else if (rbuff[index+9] == 13) effect_file = "/tmp/mnt/sdcard/effects/batt13.wav";
            else if (rbuff[index+9] == 14) effect_file = "/tmp/mnt/sdcard/effects/batt14.wav";
            else if (rbuff[index+9] == 15) effect_file = "/tmp/mnt/sdcard/effects/batt15.wav";
            else if (rbuff[index+9] == 16) effect_file = "/tmp/mnt/sdcard/effects/batt16.wav";
            else if (rbuff[index+9] == 17) effect_file = "/tmp/mnt/sdcard/effects/batt17.wav";
            else if (rbuff[index+9] == 18) effect_file = "/tmp/mnt/sdcard/effects/batt18.wav";
            else if (rbuff[index+9] == 19) effect_file = "/tmp/mnt/sdcard/effects/batt19.wav";
            else if (rbuff[index+9] == 20) effect_file = "/tmp/mnt/sdcard/effects/batt20.wav";
            else effect_file = "/tmp/mnt/sdcard/effects/batt01.wav";
            dp("play : %s\n", effect_file);
            ao_file_play_thread(effect_file);
            sleep(5);
            cmd_end_flag = true;
        break;
        case SET_SET_TOTAL:
            // SA = (Setting_All*)&rbuff[index+9];
            // rbuff[index+9+0];
            // rbuff[index+9+1];
            // rbuff[index+9+2];
            // rbuff[index+9+3];
            // rbuff[index+9+4];
            // rbuff[index+9+5];
            // rbuff[index+9+6];
            // rbuff[index+9+7];
            // rbuff[index+9+8];
            // rbuff[index+9+9];
            // rbuff[index+9+10];
            // rbuff[index+9+10+27];
            // rbuff[index+9+10+27+27];
            settings.SF.bits.led = rbuff[index+9+SA_LED];
            settings.bell_type =  rbuff[index+9+SA_BELL_TYPE];
            settings.spk_vol =  rbuff[index+9+SA_SPK_VOL];
            settings.SF.bits.per_face =  rbuff[index+9+SA_FACE_MOSAIC];
            settings.SF.bits.door_g = rbuff[index+9+SA_DOOR];
            settings.SF.bits.user_g = rbuff[index+9+SA_USER];
            settings.SF.bits.move_ex = rbuff[index+9+SA_MOVE_EX];
            settings.SF.bits.flicker = rbuff[index+9+SA_FLICKER];
            settings.move_sensitivty = rbuff[index+9+SA_MOVE_SENSI];
            settings.SF.bits.backlight = rbuff[index+9+SA_BACKLIGHT];

            dp("led:%d belltype:%d spkvol:%d facemosaic:%d\n", settings.SF.bits.led, settings.bell_type, settings.spk_vol, settings.SF.bits.per_face);
            dp("door:%d user:%d moveex:%d flicker:%d sensi:%d backlight:%d\n", settings.SF.bits.door_g, settings.SF.bits.user_g, settings.SF.bits.move_ex, settings.SF.bits.flicker,
                                                                                    settings.move_sensitivty, settings.SF.bits.backlight);


            memcpy(settings.door_grid , &rbuff[index+9+SA_DOOR_GRID], 27);
            memcpy(settings.user_grid , &rbuff[index+9+SA_USER_GRID], 27);
            settings.move_ex_s_x = (rbuff[index+9+SA_MOVE_EX_S_X]*0x100)+(rbuff[index+9+SA_MOVE_EX_S_X+1]);
            settings.move_ex_s_y = (rbuff[index+9+SA_MOVE_EX_S_Y]*0x100)+(rbuff[index+9+SA_MOVE_EX_S_Y+1]);
            settings.move_ex_e_x = (rbuff[index+9+SA_MOVE_EX_E_X]*0x100)+(rbuff[index+9+SA_MOVE_EX_E_X+1]);
            settings.move_ex_e_y = (rbuff[index+9+SA_MOVE_EX_E_Y]*0x100)+(rbuff[index+9+SA_MOVE_EX_E_Y+1]);
            dp("doorgrid:");
            for(int k=0; k<27; k++) {
                dp("0x%02x ", settings.door_grid[k]);
            }
            dp("\n");
            dp("usergrid:");
            for(int k=0; k<27; k++) {
                dp("0x%02x ", settings.user_grid[k]);
            }
            dp("\n");
            dp("moveexarea: \n     start X : %d Y : %d\n     end : X : %d Y : %d\n", settings.move_ex_s_x, settings.move_ex_s_y, settings.move_ex_e_x, settings.move_ex_e_y);


            Setting_Save();
            ack_len = 0;
            ack_flag = true;
            cmd_end_flag = true;
        break;
        }
    break;
    case DTEST_BACK:
        switch(minor) {
        case TEST_BRIGTH:
            dp("Bright Test!!\n");
            brt_st_stat = 1;
        break;
        case TEST_DIMMING:
            dp("Dimmint Test!!\n");
            int index_D = rbuff[index+9];
            if (index_D == 1) {
                dim_st_stat = 1;     
            }
            else if (index_D == 0) {
                if (dim_st_stat != 0)
                    dim_st_stat = 3;
            }
            else {
                dp("[DIMMING]Invalid Index\n");
            }
        break;
        case TEST_BLED:
            dp("Bottom Led Test!!\n");
            int index_B = rbuff[index+9];
            if (index_B == 1) {
                bled_st_stat  = 1;
            }
            else if (index_B == 0) {
                if (bled_st_stat != 0)
                    bled_st_stat  = 3;
            }
            else {
                dp("[BLT]Invalid Index\n");
            }
        break;
        case TEST_SPK:
            dp("Sound Test For 1KHz.\n");
            Set_Vol(90,30,spk_vol_buf,spk_gain_buf);
            effect_file = "/tmp/mnt/sdcard/effects/dev_takeoff.wav";
            dp("play : %s\n", effect_file);
            ao_file_play_thread(effect_file);
        break;
        case TEST_MIC:
            dp("MIC Test!!\n");
            mic_st_stat  = 1;
        break;
        case TEST_VEDIO:
            dp("Video Test!!\n");
            video_st_stat = 1;
        break;
        case TEST_CLARITY:
            dp("Sharpness & FocusTest!!\n");
            shfo_st_stat = 1;
        break;
        }
    break;
    default:
        return -1;
    break;        
    }

    if (ack_flag) {
        usleep(50*1000);
        uart_tx = malloc(ack_len+10);
        res = Make_Uart_Ack(uart_tx, ack_len, ack_data, ack_major, ack_minor);
        uart_send(fd_uart, uart_tx, res);
        dp("Ack Major:0x%02x Minor0x%02x\n", uart_tx[1], uart_tx[2]);
        ack_flag = false;
        free(uart_tx);
    }

    return 0;
}


int device_star(uint8_t major) {
    uint8_t *uart_tx;

    uart_tx = malloc(30);

    memset(uart_tx, 0, 10);
    uart_tx[0] = 0x02;
    uart_tx[1] = (major & 0x7F) & 0xFF;
    uart_tx[2] = 0x01;
    // uart_tx[3] = 0;
    // uart_tx[4] = 0; //5; 
    // uart_tx[5] = 0x00;
    // uart_tx[6] = 0x00;
    // uart_tx[7] = 0x00;

    sprintf((char*)&uart_tx[3], "%s.%s.%s", MAJOR_VER, MINOR_VER, CAHR_VER);

    uart_tx[8] = 0x00;
    
    // uart_tx[14] = 0x03;
    uart_tx[9] = 0x03;

    uart_send(fd_uart, uart_tx, 10);
    
    dp("Device Start Major:0x%02x\n", uart_tx[1]);
    // dp("%c%c%c%c%c\n",(char) uart_tx[9],(char) uart_tx[10],(char) uart_tx[11],(char) uart_tx[12],(char) uart_tx[13]);
    
    free(uart_tx);

    return 0;
}

int device_end(uint8_t major) {
    uint8_t *uart_tx;

    bLive = true;

    uart_tx = malloc(10);

    memset(uart_tx, 0, 10);
    uart_tx[0] = 0x02;
    uart_tx[1] = (major & 0x7F) & 0xFF;
    uart_tx[2] = 0x7F;
    uart_tx[3] = 0;
    uart_tx[4] = 0;
    uart_tx[5] = 0x00;
    uart_tx[6] = 0x00;
    uart_tx[7] = 0x00;
    uart_tx[8] = 0x00;
    uart_tx[9] = 0x03;

    uart_send(fd_uart, uart_tx, 10);

    usleep(100*1000);
    system("echo Program_End!! > /tmp/end_file");

    uart_send(fd_uart, uart_tx, 10);
    
    dp("Device End Major:0x%02x\n", uart_tx[1]);
    
    free(uart_tx);

    return 0;
}


int device_live(uint8_t major) {
    uint8_t *uart_tx;

    uart_tx = malloc(10);

    memset(uart_tx, 0, 10);
    uart_tx[0] = 0x02;
    uart_tx[1] = (major & 0x7F) & 0xFF;
    uart_tx[2] = 0x7D;
    uart_tx[3] = 0;
    uart_tx[4] = 0;
    uart_tx[5] = 0x00;
    uart_tx[6] = 0x00;
    uart_tx[7] = 0x00;
    uart_tx[8] = 0x00;
    uart_tx[9] = 0x03;

    uart_send(fd_uart, uart_tx, 10);
    
    // dp("L\n");
    
    free(uart_tx);

    return 0;
}


int make_file_start(uint8_t major) {
    uint8_t *uart_tx;

    uart_tx = malloc(10);

    memset(uart_tx, 0, 10);
    uart_tx[0] = 0x02;
    uart_tx[1] = (major & 0x7F) & 0xFF;
    uart_tx[2] = 0x21;
    uart_tx[3] = 0;
    uart_tx[4] = 0;
    uart_tx[5] = 0x00;
    uart_tx[6] = 0x00;
    uart_tx[7] = 0x00;
    uart_tx[8] = 0x00;
    uart_tx[9] = 0x03;

    uart_send(fd_uart, uart_tx, 10);

    usleep(100*1000);

    uart_send(fd_uart, uart_tx, 10);
    
    dp("MFS\n");
    
    free(uart_tx);

    return 0;
}

int face_end(uint8_t major) {
    static bool first_flag = false;
    uint8_t *uart_tx;

    if (first_flag)
        return 0;

    uart_tx = malloc(10);

    memset(uart_tx, 0, 10);
    uart_tx[0] = 0x02;
    uart_tx[1] = (major & 0x7F) & 0xFF;
    uart_tx[2] = 0x22;
    uart_tx[3] = 0;
    uart_tx[4] = 0;
    uart_tx[5] = 0x00;
    uart_tx[6] = 0x00;
    uart_tx[7] = 0x00;
    uart_tx[8] = 0x00;
    uart_tx[9] = 0x03;

    uart_send(fd_uart, uart_tx, 10);
    
    dp("FE\n");
    
    free(uart_tx);

    first_flag = true;

    return 0;
}

bool memflag = false;

int streaming_rec_end(uint8_t cus) {
    uint8_t *uart_tx;

    uart_tx = malloc(10);

    memset(uart_tx, 0, 10);
    uart_tx[0] = 0x02;
    uart_tx[1] = 0x02;
    if (cus == CAUSE_MEM)
        uart_tx[2] = 0x22;
    else if (cus == CAUSE_FILE)
        uart_tx[2] = 0x23;
    else
        return -1;
    uart_tx[3] = 0;
    uart_tx[4] = 0;
    uart_tx[5] = 0x00;
    uart_tx[6] = 0x00;
    uart_tx[7] = 0x00;
    uart_tx[8] = 0x00;
    uart_tx[9] = 0x03;

    if (!memflag) {
        memflag = true;
        uart_send(fd_uart, uart_tx, 10);
        dp("SRE\n");
    }
    
    free(uart_tx);

    return 0;
}

int door_set_fail(void) {
    uint8_t *uart_tx;

    uart_tx = malloc(10);

    memset(uart_tx, 0, 10);
    uart_tx[0] = 0x02;
    uart_tx[1] = 0x03;
    uart_tx[2] = 0x02;
    uart_tx[3] = 0;
    uart_tx[4] = 0;
    uart_tx[5] = 0x00;
    uart_tx[6] = 0x00;
    uart_tx[7] = 0x00;
    uart_tx[8] = 0x00;
    uart_tx[9] = 0x03;

    uart_send(fd_uart, uart_tx, 10);
    
    dp("DSF\n");
    
    free(uart_tx);

    return 0;
}

int rec_enable_ack(void) {
    uint8_t *uart_tx;

    uart_tx = malloc(10);

    memset(uart_tx, 0, 10);
    uart_tx[0] = 0x02;
    uart_tx[1] = 0x01;
    uart_tx[2] = 0x11;
    uart_tx[3] = 0;
    uart_tx[4] = 0;
    uart_tx[5] = 0x00;
    uart_tx[6] = 0x00;
    uart_tx[7] = 0x00;
    uart_tx[8] = 0x00;
    uart_tx[9] = 0x03;

    uart_send(fd_uart, uart_tx, 10);
    
    dp("REA\n");
    
    free(uart_tx);

    return 0;
}

int brightness_ack(uint16_t black_brt, uint16_t wwhite_brt) {
    uint8_t *uart_tx;

    uart_tx = malloc(20);

    memset(uart_tx, 0, 20);
    uart_tx[0] = 0x02;
    uart_tx[1] = 0x00;
    uart_tx[2] = 0x03;
    uart_tx[3] = 0;
    uart_tx[4] = 4;
    uart_tx[5] = 0x00;
    uart_tx[6] = 0x00;
    uart_tx[7] = 0x00;
    uart_tx[8] = 0x00;

    uart_tx[9]  = (black_brt>>8) & 0xFF;
    uart_tx[10] = (black_brt) & 0xFF;

    uart_tx[11] = (wwhite_brt>>8) & 0xFF;
    uart_tx[12] = (wwhite_brt) & 0xFF;

    uart_tx[13] = 0x03;

    uart_send(fd_uart, uart_tx, 14);
    
    dp("Sharpness Focus Ack Packet\n");
    
    free(uart_tx);

    return 0;
}

int sharp_focus_ack(uint16_t mfo, uint16_t msh, uint16_t bfo, uint16_t bsh) {
    uint8_t *uart_tx;

    uart_tx = malloc(20);

    memset(uart_tx, 0, 20);
    uart_tx[0] = 0x02;
    uart_tx[1] = 0x00;
    uart_tx[2] = 0x0A;
    uart_tx[3] = 0;
    uart_tx[4] = 8;
    uart_tx[5] = 0x00;
    uart_tx[6] = 0x00;
    uart_tx[7] = 0x00;
    uart_tx[8] = 0x00;

    uart_tx[9]  = (mfo>>8) & 0xFF;
    uart_tx[10] = (mfo) & 0xFF;

    uart_tx[11] = (msh>>8) & 0xFF;
    uart_tx[12] = (msh) & 0xFF;

    uart_tx[13] = (bfo>>8) & 0xFF;
    uart_tx[14] = (bfo) & 0xFF;

    uart_tx[15] = (bsh>>8) & 0xFF;
    uart_tx[16] = (bsh) & 0xFF;

    uart_tx[17] = 0x03;

    uart_send(fd_uart, uart_tx, 18);
    
    dp("Sharpness Focus Ack Packet\n");
    
    free(uart_tx);

    return 0;
}

int realvedio_ack(uint8_t success) {
    uint8_t *uart_tx;

    uart_tx = malloc(20);

    memset(uart_tx, 0, 20);
    uart_tx[0] = 0x02;
    uart_tx[1] = 0x00;
    uart_tx[2] = 0x09;
    uart_tx[3] = 0;
    uart_tx[4] = 1;
    uart_tx[5] = 0x00;
    uart_tx[6] = 0x00;
    uart_tx[7] = 0x00;
    uart_tx[8] = 0x00;

    uart_tx[9]  = success & 0xFF;
    
    uart_tx[10] = 0x03;

    uart_send(fd_uart, uart_tx, 18);
    
    dp("Srealvedio_ack Packet\n");
    
    free(uart_tx);

    return 0;
}


/*
 * 串口接收函数
 *
 * **/

int uart_receive(int fd,uint8_t *rev_buff,int data_len)
{
    int bytes = 0;
    int ret = 0;
    ioctl(fd,FIONREAD,&bytes);//不阻塞
    if(bytes > 0){
        ret = read(fd,rev_buff,data_len);
    }
    return ret;
}

void uart_tx_test (void) {
    uint8_t *uart2_buf;
    uart2_buf = malloc(512);

    // sprintf(uart2_buf, "0123456789");
    for (uint8_t i=0; i<0xFF; i++) {
        uart2_buf[i] = i+1;
    }

    dp("Uart Tx!\n");
    uart_send(fd_uart,uart2_buf, 0xff);
}



void *uart_thread(void *argc)
{
    int res, mode;
    int cmd_state = 0, len, len_p;
    // int set_baudrate = 2560000;
    // int set_databits = 8;
    // int set_stopbits = 1;
    // char set_parity = 'N';
    // path = (char *)default_path;
    uint8_t *uart_rx;
    uint8_t *cmd_rx;

    uart_rx = malloc(128);
    cmd_rx = malloc(128);
    
    // dp("/dev/ttyS2 115200 8 1 N\n"); 

    /*
     * 串口初始化函数
     * uart init function
     * */
    // res = uart_init(set_baudrate,set_databits,set_stopbits,set_parity);
    res = uart_init();
    if(res < 0){
        dp("uart init failed \n");
    }
    // uint8_t *uart_tx;
    // uint8_t start_data[10] = {0};

    // uart_tx = malloc(50);
    // if (boot_mode == 0) mode = DTEST;
    // else if (boot_mode == 1) mode = REC;
    // else if (boot_mode == 2) mode = STREAMING;
    // else if (boot_mode == 3) mode = SETTING;
    // // start_data[0] = (uint8_t)MAJOR_VER;
    // // start_data[1] = '.';
    // // start_data[2] = (uint8_t)MINOR_VER;
    // // start_data[3] = '.';
    // // start_data[4] = (uint8_t)CAHR_VER;
    // // start_data[5] = 0;
    // sprintf((char*)start_data, "%s.%s.%s\n", MAJOR_VER, MINOR_VER, CAHR_VER);
    // res = Make_Packet_uart(uart_tx, start_data, 5, mode, 1);
    // uart_send(fd_uart, uart_tx, res);
    // dp("Start Major:0x%02x Minor0x%02x\n", uart_tx[1], uart_tx[2]);
    // dp("Start LEN : 0x%02x%02x\\n", uart_tx[3], uart_tx[4]);
    // free(uart_tx);

    if (boot_mode == 0) mode = DTEST;
    else if (boot_mode == 1) mode = REC;
    else if (boot_mode == 2) mode = STREAMING;
    else if (boot_mode == 3) mode = SETTING;

    device_star(mode);

    /*
     * 测试代码
     * test code
     */
    do {
        res = read(fd_uart, uart_rx, 512);
        // dp("res : %d\n", res);
        if (res > 0) {
           
            dp("\n");
            if (uart_rx[0] == 0x02) {
                cmd_state = 1;
                len = (uart_rx[3] * 0x100) + (uart_rx[4]);
                memset(cmd_rx, 0x00, 128);
                memcpy(cmd_rx, uart_rx, res);
                if (res == len+10) {
                    cmd_state = 2;
                }
                len_p = res;
                // dp("0UART RX: ");
                // for (int i=0; i<res; i++) {
                //     dp("0x%02x ", cmd_rx[i]);
                // }
                // dp("\n");
                // dp("len : %d\n", len);
            }
            else if (cmd_state == 1) {
                if (len_p + res == len + 10) {
                    memcpy(&cmd_rx[len_p], uart_rx, res);
                    cmd_state = 2;
                }
                else {
                    memcpy(&cmd_rx[len_p], uart_rx, res);
                    len_p += res;
                }
                // dp("1UART RX: ");
                // for (int i=0; i<len_p; i++) {
                //     dp("0x%02x ", cmd_rx[i]);
                // }
                // dp("\n");
            }

            if (cmd_state == 2) {
                dp("\nUART RX[%d]: ", len);
                for (int i=0; i<len_p; i++) {
                    dp("0x%02x ", cmd_rx[i]);
                }
                dp("\n");
                Recv_Uart_Packet_live(cmd_rx);
                cmd_state = 0;
            }
        }
     }while (!bUart);

    // res = Make_Packet_uart(uart_tx, start_data, 0, mode, SET_STOP);
    // uart_send(fd_uart, uart_tx, res);
    // dp("Start Major:0x%02x Minor0x%02x\n", uart_tx[1], uart_tx[2]);

    free(uart_rx);

    close(fd_uart);
    dp("Uart FD Close\n");
    return 0;
}


void *device_live_thread(void * argc) {
    int mode;
    int64_t live_total = sample_gettimeus();
    bool live_flag = false;
    int LMS_cnt = 0;
    do {
        if ((!live_flag) && (sample_gettimeus()-live_total > LIVE_MESSAGE_TIME)) {
            // dp("LMS %d\n", LMS_cnt);
            LMS_cnt++;
            live_total = sample_gettimeus();
            live_flag =true;
            if (boot_mode == 0) mode = DTEST;
            else if (boot_mode == 1) mode = REC;
            else if (boot_mode == 2) mode = STREAMING;
            else if (boot_mode == 3) mode = SETTING;
            if (!bLiveFile)
                device_live(mode);
        }
        else {
            live_flag = false;
        }
    } while (!bLive);
    return 0;
}


