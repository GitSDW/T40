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
//     printf("This is tty/usart demo.\n");
//     fd = open(path, O_RDWR);
//     if (fd < 0) {
//         printf("Fail to Open %s device\n", path);
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

//     printf("Device %s is set to %d bps,databits %d,stopbits %d,parity %c\n",path,baudrate,databits,stopbits,parity);
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
const int uart_send(int fd,uint8_t *send_buf,int data_len)
{
    int len = 0;
    len = write(fd,send_buf,data_len);
    if(len == data_len){
        return len;
    } else{
        tcflush(fd,TCIOFLUSH);
        return -1;
    }
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
            case SET_START:
                // printf("Start Pck Make!\n");
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

static int Recv_Uart_Packet_live(uint8_t *rbuff) {
    int index, len, value_buf, ack_len, res;
    uint8_t major, minor, buf_8;
    int bad_cnt = 0;
    uint8_t *uart_tx;
    uint8_t ack_data[256];
    static int64_t rec_time_s = 0;
    int64_t rec_time_e = 0;
    char* effect_file = NULL;

        
    index = 0;

    major = rbuff[index+1];
    minor = rbuff[index+2];
    if (rbuff[index] != 0x02) {
        // printf("S\n");
        bad_cnt++;
    }
    
    buf_8 = major&0x80;
    if (buf_8 == 0) {
        // printf("M\n");
        bad_cnt++;
    } 

    // if (minor != 0x07) {
    //     bad_cnt++;
    // }

    if (bad_cnt > 1) {
        return -1;
    }

    len = rbuff[index+3]*256 + rbuff[index+4];

    printf("PKT Len : %d\n", len);

    ack_major = major;
    ack_minor = minor;

    switch(major) {
    case DTEST_BACK:

    case REC_BACK:
        switch(minor) {
        case UREC_BELL:
            
            if (settings.bell_type == 0) effect_file = "/tmp/mnt/sdcard/effects/ready_16.wav";
            else if (settings.bell_type == 1) effect_file = "/tmp/mnt/sdcard/effects/start2c.wav";
            else if (settings.bell_type == 2) effect_file = "/tmp/mnt/sdcard/effects/start3c.wav";;
            printf("play : %s\n", effect_file);
            ao_file_play_thread(effect_file);
            clip_cause_t.Major = CLIP_CAUSE_BOX;
            clip_cause_t.Minor = CLIP_BOX_OCCUR;
            bell_rec_state = REC_START;
            ack_len = 0;
            ack_flag = true;
        break;
        case  UREC_FACE:
            if (rbuff[index+9] == 1) {
                printf("Streaming Rec Face Detection!\n");
                clip_cause_t.Major = CLIP_CAUSE_FACE;
                clip_cause_t.Minor = CLIP_FACE_DET; 
            }
            ack_len = 0;
            ack_flag = true;
        break;
        case UREC_STREAM:
            stream_state = 1;
            ack_len = 0;
            ack_flag = true;
        break;

        case UREC_STREAM_END:
            stream_state = 0;
            ack_len = 0;
            ack_flag = true;
        break;
        case UREC_TEMPER:
            effect_file = "/tmp/mnt/sdcard/effects/end1c.wav";
            printf("play : %s\n", effect_file);
            ao_file_play_thread(effect_file);
            clip_cause_t.Major = CLIP_CAUSE_MOUNT;
            clip_cause_t.Minor = CLIP_MOUNT_DISMT;
            ack_len = 0;
            ack_flag = true;
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
                ack_flag = true;
            }

            printf("Light %d\n", value_buf);
        break;
        case USTREAM_REC_S:
            printf("Streaming Rec Start!\n");
            if (rec_on) {
                ack_len = 1;
                ack_data[0] = 2;
                ack_flag = true;
            }
            else if (rec_mem_flag) {
                ack_len = 1;
                ack_data[0] = 3;
                ack_flag = true;
            }
            else {
                rec_time_s = sample_gettimeus();
                streaming_rec_state = REC_START;
                rec_on = true;
                rec_cnt++;
                ack_len = 1;
                ack_data[0] = 1;
                ack_flag = true;
            }
        break;
        case USTREAM_REC_E:
            printf("Streaming Rec End!\n");
            rec_time_e = sample_gettimeus()-rec_time_s;
            printf("Rec Time : %lld total : %lld\n", rec_time_e, rec_total);
            rec_total += rec_time_e;
            streaming_rec_state = REC_STOP;
            rec_on = false;
            ack_len = 0;
            ack_flag = true;
        break;
        case USTREAM_F_SEND:
            rec_end = true;
            ack_len = 0;
            ack_flag = true;
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
                printf("Auto LED Setting Saved! %d\n", settings.SF.bits.led);
                system("sync");
                ack_len = 0;
                ack_flag = true;
            }
        break;
        case SET_BELL_VOL:
            if (len > 0) {
                value_buf = rbuff[index+9];
                settings.bell_type = value_buf;
                Setting_Save();
                printf("Bell Type Setting Saved! %d\n", settings.bell_type);
                system("sync");
                ack_len = 0;
                ack_flag = true;

                if (settings.bell_type == 0) effect_file = "/tmp/mnt/sdcard/effects/ready_16.wav";
                else if (settings.bell_type == 1) effect_file = "/tmp/mnt/sdcard/effects/start2c.wav";
                else if (settings.bell_type == 2) effect_file = "/tmp/mnt/sdcard/effects/start3c.wav";;
                printf("play : %s\n", effect_file);
                ao_file_play_thread(effect_file);
            }
        break;
        case SET_SPK_VOL:
            if (len > 0) {
                value_buf = rbuff[index+9];
                settings.spk_vol = value_buf;
                Setting_Save();
                printf("Speaker Vol Setting Saved! %d\n", settings.spk_vol);
                system("sync");
                ack_len = 0;
                ack_flag = true;
            }
        break;
        case SET_BACK_LIGHT:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.backlight = value_buf;
                Setting_Save();
                printf("Back Light Setting Saved! %d\n", settings.SF.bits.backlight);
                system("sync");
                ack_len = 0;
                ack_flag = true;
            }
        break;
        case SET_FLICKER:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.flicker = value_buf;
                Setting_Save();
                printf("Flicker Setting Saved! %d\n", settings.SF.bits.flicker);
                system("sync");
                ack_len = 0;
                ack_flag = true;
            }
        break;
        case SET_MOVE_SENSI:
            if (len > 0) {
                value_buf = rbuff[index+9];
                settings.move_sensitivty = value_buf;
                Setting_Save();
                printf("Move Sensitivity Setting Saved! %d\n", settings.move_sensitivty);
                system("sync");
                ack_len = 0;
                ack_flag = true;
            }
        break;
        case SET_EX_ONOF:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.move_ex = value_buf;
                Setting_Save();
                printf("Move Ex On/Off Setting Saved! %d\n", settings.SF.bits.move_ex);
                system("sync");
                ack_len = 0;
                ack_flag = true;
            }
        break;
        case SET_FACE_MOSAIC:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.per_face = value_buf;
                Setting_Save();
                printf("Face Mosaic Setting Saved! %d\n", settings.SF.bits.per_face);
                system("sync");
                ack_len = 0;
                ack_flag = true;
            }
        break;
        case SET_DOOR_ONOF:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.door_g = value_buf;
                Setting_Save();
                printf("Door Grid On/Off Setting Saved! %d\n", settings.SF.bits.door_g);
                system("sync");
                ack_len = 0;
                ack_flag = true;
            }
        break;
        case SET_USER_ONOF:
            if (len > 0) {
                if (rbuff[index+9] > 0)     value_buf = 1;
                else                        value_buf = 0;
                settings.SF.bits.user_g = value_buf;
                Setting_Save();
                printf("User Grid On/Off Setting Saved! %d\n", settings.SF.bits.user_g);
                system("sync");
                ack_len = 0;
                ack_flag = true;
            }
        break;
        case SET_EX_AREA:
        break;
        case SET_DOOR_GRID:
        break;
        case SET_USER_GRID:
        break;
        }
    break;
    default:
        return -1;
    break;        
    }

    if (ack_flag) {
        uart_tx = malloc(ack_len+10);
        res = Make_Uart_Ack(uart_tx, ack_len, ack_data, ack_major, ack_minor);
        uart_send(fd_uart, uart_tx, res);
        printf("Ack Major:0x%02x Minor0x%02x\n", uart_tx[1], uart_tx[2]);
        ack_flag = false;
        free(uart_tx);
    }

    return 0;
}

int device_end(uint8_t major) {
    uint8_t *uart_tx;

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
    
    printf("Device End Major:0x%02x\n", uart_tx[1]);
    
    free(uart_tx);
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

    printf("Uart Tx!\n");
    uart_send(fd_uart,uart2_buf, 0xff);
}

void *uart_thread(void *argc)
{
    int res, mode;
    // int set_baudrate = 2560000;
    // int set_databits = 8;
    // int set_stopbits = 1;
    // char set_parity = 'N';
    // path = (char *)default_path;
    uint8_t *uart_rx;

    uart_rx = malloc(1024);
    
    // printf("/dev/ttyS2 115200 8 1 N\n"); 

    /*
     * 串口初始化函数
     * uart init function
     * */
    // res = uart_init(set_baudrate,set_databits,set_stopbits,set_parity);
    res = uart_init();
    if(res < 0){
        printf("uart init failed \n");
    }
    uint8_t *uart_tx;
    uint8_t start_data[10] = {0};

    uart_tx = malloc(11);
    if (boot_mode == 0) mode = DTEST;
    else if (boot_mode == 1) mode = REC;
    else if (boot_mode == 2) mode = STREAMING;
    else if (boot_mode == 3) mode = SETTING;
    res = Make_Packet_uart(uart_tx, start_data, 0, mode, SET_START);
    uart_send(fd_uart, uart_tx, res);
    printf("Start Major:0x%02x Minor0x%02x\n", uart_tx[1], uart_tx[2]);
    // free(uart_tx);


    /*
     * 测试代码
     * test code
     */
    do {
        res = read(fd_uart, uart_rx, 512);
        printf("len : %d\n", res);
        if (res > 0) {
            printf("UART RX: ");
            for (int i=0; i<res; i++) {
                printf("0x%02x ", uart_rx[i]);
            }
            printf("\n");
            Recv_Uart_Packet_live(uart_rx);
        }

        

    }while (!bUart);

    // res = Make_Packet_uart(uart_tx, start_data, 0, mode, SET_STOP);
    // uart_send(fd_uart, uart_tx, res);
    // printf("Start Major:0x%02x Minor0x%02x\n", uart_tx[1], uart_tx[2]);

    free(uart_rx);

    close(fd_uart);
    printf("Uart FD Close\n");
    return 0;
}
