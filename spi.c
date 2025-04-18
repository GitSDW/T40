/**********************************************************
 * Author        : kaiwang
 * Email         : kai.wang@ingenic.com
 * Last modified : 2020-09-15 21:25
 * Filename      : spi.c
 * Description   : 
 * *******************************************************/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/ioctl.h>
#include <linux/spi/spidev.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "global_value.h"
#include "spi.h"
#include "gpio.h"
#include "c_util.h"

char *device = "/dev/spidev1.0";
static  uint32_t mode = SPI_MODE_0;
static  uint8_t bits = 8;
static  uint32_t speed = 25*1000*1000;
static  uint16_t delay = 0;
int fd = 0;
/*
 * 初始化SPI
 */
int spi_init(void)
{
    int ret = 0;
    /*打开 SPI 设备*/
    fd = open(device, O_RDWR);
    if (fd < 0)
    {
        dp("can't open /dev/spidev1.0 ");
    }

    /*
     * spi mode 设置SPI 工作模式
     */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1){
        dp("can't set spi mode");
        return -1;
    }

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1){
        dp("can't get spi mode");
        return -1;
    }

    /*
     * bits per word  设置一个字节的位数
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1){
        dp("can't set bits per word");
        return -1;
    }

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1){
        dp("can't get bits per word");
        return -1;
    }

    /*
     * max speed hz  设置SPI 最高工作频率
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1){
        dp("can't set max speed hz");
        return -1;
    }

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1){
        dp("can't get max speed hz");
        return -1;
    }

    // dp("spi mode: 0x%x\n", mode);
    // dp("bits per word: %d\n", bits);
    // dp("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);
    return 0;
}

void spi_deinit(void)
{
    close(fd);
}
/**
 * spi 单字节发送
 * */
int  spi_write_byte(int fd,unsigned char data)
{
    int ret = 0;
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)&data,
        .rx_buf = 0,//SPI Can't support synchronous transfer
        .len = 1,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        dp("fd:%d can't send spi message\n",fd);
        return -1;
    }
    return 0;
}
/**
 * spi 单字节读
 * */
int  spi_read_byte(int fd,unsigned char data_cmd,unsigned char data)
{
    int ret = 0;
    struct spi_ioc_transfer tr[2];
    memset(tr,0,2*sizeof(struct spi_ioc_transfer));
    tr[0].tx_buf = (unsigned long)&data_cmd;
    tr[0].len = 1;
    tr[0].bits_per_word = bits,

    tr[1].tx_buf = 0;
    tr[1].rx_buf = (unsigned long)&data;
    tr[1].len = 1;
    tr[1].bits_per_word = bits,

    ret = ioctl(fd, SPI_IOC_MESSAGE(2), tr);
    if (ret < 1)
    {
        dp("fd:%d can't send spi message\n",fd);
        return -1;
    }
    return 0;
}

// struct spi_ioc_transfer spi_transfer = {
//     .tx_buf = 0,
//     .rx_buf = 0,
//     .len = 1,
//     .speed_hz = 20000000,
//     .delay_usecs = 1,
//     .bits_per_word = 8,
// };

// int spi_read(uint8_t *data, size_t len)
// {
//     spi_transfer.rx_buf = (unsigned long)data;
//     spi_transfer.len = len;

//     if (ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_transfer) < 0)
//     {
//         perror("Error during SPI transfer");
//         return -1;
//     }

//     return 0;
// }

/**
 * spi 多字节发送
 * */
int  spi_write_bytes(int fd,unsigned char *pdata_buff,unsigned int len)
{
    int ret = 0;
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)pdata_buff,
        .len = len,//SPI Can't support synchronous transfer
    };

    if (send_retry_flag) {
        dp("bud[9]:0x%02x\n", pdata_buff[9]);
    }

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        dp("fd:%d can't send spi message\n",fd);
        return -1;
    }
    return 0;
}

int  spi_rw_bytes(int fd,unsigned char *tx_buff,unsigned char *rx_buff,unsigned int len)
{
    int ret = 0;
    memset(rx_buff, 0x00, 1024);
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx_buff,
        .rx_buf = (unsigned long)rx_buff,
        .len = len,//SPI Can't support synchronous transfer
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        dp("fd:%d can't send spi message\n",fd);
        return -1;
    }
    return 0;
}

#define SPI_SEND_LENGTH 1024
#define SPI_SEND_HEDER_LEN 10
#define FILE_READ_LENGTH_LIVE SPI_SEND_LENGTH - SPI_SEND_HEDER_LEN
#define FILE_READ_LENGTH 900
// #define FILE_READ_LENGTH 450

uint8_t a_pkt_cnt = 0;


int Make_Spi_Packet(uint8_t *tbuff, uint8_t *data, uint16_t len, uint8_t major, uint8_t minor)
{
    if (len > FILE_READ_LENGTH) {
        dp("File Length Over!! %d>1014\n", len);
        return -1;
    }
    memset(tbuff, 0xFF, SPI_SEND_LENGTH);
    tbuff[0+V_SEND_RESERV] = 0x02;
    tbuff[1+V_SEND_RESERV] = major & 0xFF;
    tbuff[2+V_SEND_RESERV] = minor & 0xFF;
    tbuff[3+V_SEND_RESERV] = (len>>8) & 0xFF;
    tbuff[4+V_SEND_RESERV] = len & 0xFF;
    tbuff[5+V_SEND_RESERV] = 0x00;
    tbuff[6+V_SEND_RESERV] = 0x00;
    tbuff[7+V_SEND_RESERV] = 0x00;
    tbuff[8+V_SEND_RESERV] = 0x00;

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
                case REC_CLIP_F:
                case REC_CLIP_B:
                	// for (int i=0; i<len; i++){
                		// tbuff[9+i] = (i%10)+1;
                	// }
                	memcpy(&tbuff[9+V_SEND_RESERV], data, len);
                    break;
                case REC_FACESHOT:

                case REC_FACE:
                case REC_BOX_ALM:
                case REC_SNAPSHOT:
                    memcpy(&tbuff[9+V_SEND_RESERV], data, len);
                    break;
                case REC_STREAM_END:
                    memcpy(&tbuff[9+V_SEND_RESERV], data, len);
                    break;
                case REC_ACK:
                    break;
                case REC_DEV_STOP:
                    break;
                case REC_STREAMING_M:
                case REC_STREAMING_B:
                    memcpy(&tbuff[9+V_SEND_RESERV], data, len);
                    break;
                case REC_BELL_SNAP_M:
                case REC_BELL_SNAP_B:
                case REC_TEMP_SNAP_M:
                case REC_TEMP_SNAP_B:
                case REC_DOOR_SNAP:
                case REC_FACE_END:
                    memcpy(&tbuff[9+V_SEND_RESERV], data, len);
                    break;
                default:
                    dp("Protocol Make Fail!!\n");
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
                case STREAM_VEDIO_B:
                	memcpy(&tbuff[9+V_SEND_RESERV], data, len);
                    break;
                case STREAM_FACE:
                    break;
                case STREAM_AUDIO_F:
                	memcpy(&tbuff[9+V_SEND_RESERV], data, len);
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
            switch(minor){
                case SET_FILE_START:
                case SET_FILE_SEND:
                case SET_FILE_END:
                    memcpy(&tbuff[9+V_SEND_RESERV], data, len);
                break;
                default:
                    dp("Protocol Make Fail!!\n");
                    return -1;
                break;
            }
        break;
        default:
            return -1;
        break;
    }
    // tbuff[len + 8] = 0x03;
    tbuff[1023-V_SEND_RESERV] = 0x03;
    return 0;
}

int Make_Spi_Packet_live(uint8_t *tbuff, uint8_t *data, uint16_t len, uint8_t major, uint8_t minor)
{
    int reserv_cnt = V_SEND_RESERV;   

    if (len > FILE_READ_LENGTH_LIVE) {
        dp("File Length Over!! %d>1014\n", len);
        return -1;
    }
    memset(tbuff, 0xFF, SPI_SEND_LENGTH);

    tbuff[0+reserv_cnt] = 0x02;
    tbuff[1+reserv_cnt] = major & 0xFF;
    tbuff[2+reserv_cnt] = minor & 0xFF;
    tbuff[3+reserv_cnt] = (len>>8) & 0xFF;
    tbuff[4+reserv_cnt] = len & 0xFF;
    tbuff[5+reserv_cnt] = 0x00;
    tbuff[6+reserv_cnt] = 0x00;
    tbuff[7+reserv_cnt] = 0x00;
    // tbuff[8+reserv_cnt] = 0x00;
    tbuff[8+reserv_cnt] = a_pkt_cnt;

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
                    memcpy(&tbuff[9+reserv_cnt], data, len);
                    break;
                case REC_CLIP_F:
                case REC_CLIP_B:
                    memcpy(&tbuff[9+reserv_cnt], data, len);
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
                    memcpy(&tbuff[9+reserv_cnt], data, len);

                    break;
                case STREAM_VEDIO_B:
                    memcpy(&tbuff[9+reserv_cnt], data, len);
                    break;
                case STREAM_FACE:
                    break;
                case STREAM_AUDIO_F:
                    memcpy(&tbuff[9+reserv_cnt], data, len);
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
        break;
    default:
        return -1;
        break;
    }
    // tbuff[len + 8] = 0x03;
    tbuff[1023-V_SEND_RESERV] = 0x03;
    return 0;
}

int bitrate_change = 500;
int64_t bitrate_cnt = 0;

extern int Set_Target_Bit2(uint32_t targetbit);

int Make_Spi_Packet_live_rtp(uint8_t *tbuff, uint8_t *data, uint16_t len, uint8_t major, uint8_t minor, int64_t time, bool fm_end, bool countinue_flag, bool header_ex)
{
    int reserv_cnt = V_SEND_RESERV;
    // uint8_t endchar[4] = {0};
    // int test_data = 0;

    if (len > FILE_READ_LENGTH_LIVE) {
        dp("File Length Over!! %d>1014\n", len);
        return -1;
    }
    memset(tbuff, 0xFF, SPI_SEND_LENGTH);

    RTPHeader header;
    static uint16_t seq_num0 = 0;
    static uint16_t seq_num3 = 0;

    // static int64_t test_time = 0;
    // static int64_t time_check = 0;
    // int64_t cal_time = 0;

    // static int64_t real_time_gap = 0;
    // int64_t gap_time = 0;

    #ifdef __IOT_CORE__
        if ((countinue_flag&&!header_ex) || !countinue_flag) {
            header.version_padding_extension_cc = 0x80;
            if (fm_end)
                header.marker_payload_type = 0xE1;
            else
                header.marker_payload_type = 0x61;

            if (minor == STREAM_VEDIO_M) {
                header.sequence_number[0] = (seq_num0&0xFF00) >> 8;;
                header.sequence_number[1] = seq_num0&0xFF;
            }
            else if (minor == STREAM_VEDIO_B) {
                header.sequence_number[0] = (seq_num3&0xFF00) >> 8;;
                header.sequence_number[1] = seq_num3&0xFF;
            }
            // header.timestamp = sample_gettimeus();
            header.timestamp[0] = (time&0xFF000000) >> 24;
            header.timestamp[1] = (time&0x00FF0000) >> 16;
            header.timestamp[2] = (time&0x0000FF00) >> 8;
            header.timestamp[3] = (time&0x000000FF);

            // if (minor == STREAM_VEDIO_M) {
            //     if (time_check > time) {
            //         dp("t: %lld -> %lld\n", time_check, time);
            //     }
            //     time_check = time;
            // }

            // if (minor == STREAM_VEDIO_M) {
            //     header.timestamp[0] = 0x51;
            //     header.timestamp[1] = 0x79;
            //     header.timestamp[2] = 0x42;
            //     header.timestamp[3] = 0x00;
            // }
            // else if (minor == STREAM_VEDIO_B) {
            //     header.timestamp[0] = 0x51;
            //     header.timestamp[1] = 0x79;
            //     header.timestamp[2] = 0x42;
            //     header.timestamp[3] = 0x01;
            // }
            // header.ssrc = 123465879;
            header.ssrc[0] = 0x20;
            header.ssrc[1] = 0x24;
            header.ssrc[2] = major;
            header.ssrc[3] = minor;

            // memcpy(&tbuff[reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
            if (minor == STREAM_VEDIO_M) seq_num0++;
            else if (minor == STREAM_VEDIO_B) seq_num3++;

            len += sizeof(RTPHeader); 
            // if (STREAM_VEDIO_M) dp("R\n");
        }
    #else
        header.version_padding_extension_cc = 0x80;
        if (fm_end)
            header.marker_payload_type = 0xE1;
        else
            header.marker_payload_type = 0x61;

        if (minor == STREAM_VEDIO_M) {
            header.sequence_number[0] = (seq_num0&0xFF00) >> 8;;
            header.sequence_number[1] = seq_num0&0xFF;
        }
        else if (minor == STREAM_VEDIO_B) {
            header.sequence_number[0] = (seq_num3&0xFF00) >> 8;;
            header.sequence_number[1] = seq_num3&0xFF;
        }
        // header.timestamp = sample_gettimeus();
        header.timestamp[0] = (time&0xFF000000) >> 24;
        header.timestamp[1] = (time&0x00FF0000) >> 16;
        header.timestamp[2] = (time&0x0000FF00) >> 8;
        header.timestamp[3] = (time&0x000000FF);
        // if (minor == STREAM_VEDIO_M) {
        //     header.timestamp[0] = 0x51;
        //     header.timestamp[1] = 0x79;
        //     header.timestamp[2] = 0x42;
        //     header.timestamp[3] = 0x00;
        // }
        // else if (minor == STREAM_VEDIO_B) {
        //     header.timestamp[0] = 0x51;
        //     header.timestamp[1] = 0x79;
        //     header.timestamp[2] = 0x42;
        //     header.timestamp[3] = 0x01;
        // }
        // header.ssrc = 123465879;
        header.ssrc[0] = 0x20;
        header.ssrc[1] = 0x24;
        header.ssrc[2] = major;
        header.ssrc[3] = minor;

        // memcpy(&tbuff[reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
        if (minor == STREAM_VEDIO_M) seq_num0++;
        else if (minor == STREAM_VEDIO_B) seq_num3++;

        len += sizeof(RTPHeader); 
        // if (STREAM_VEDIO_M) dp("R\n");
    #endif

    tbuff[0+reserv_cnt] = 0x02;
    tbuff[1+reserv_cnt] = major & 0xFF;
    tbuff[2+reserv_cnt] = minor & 0xFF;
    tbuff[3+reserv_cnt] = (len>>8) & 0xFF;
    tbuff[4+reserv_cnt] = len & 0xFF;
    tbuff[5+reserv_cnt] = 0x00;
    tbuff[6+reserv_cnt] = 0x00;
    #ifdef __IOT_CORE__
        if (fm_end) {
            // if (header_ex) tbuff[7+reserv_cnt] = 2;
            // else tbuff[7+reserv_cnt] = 1;
            // tbuff[7+reserv_cnt] = 1;
            if (!header_ex) {
                if (len < (V_SEND_SIZE+sizeof(RTPHeader)))  tbuff[7+reserv_cnt] = 1;
                else                    tbuff[7+reserv_cnt] = 2;
            }
            else {
                tbuff[7+reserv_cnt] = 1;
                
            }
        }
        else {
            if (!header_ex) {
                tbuff[7+reserv_cnt] = 2;
            }
            else {
                tbuff[7+reserv_cnt] = 1;
                
            }
        }

        // test_data = seq_num0 & 0xFF;

        // if (!countinue_flag) tbuff[7+reserv_cnt] = 3;

        // if (header_ex) tbuff[7+reserv_cnt] = 2;
        // else tbuff[7+reserv_cnt] = 1;
    #else
        tbuff[7+reserv_cnt] = 0x01;
    #endif
    
    // tbuff[8+reserv_cnt] = 0x00;
    tbuff[8+reserv_cnt] = a_pkt_cnt;


    // if (minor == STREAM_VEDIO_M) {
    //     // #ifdef __IOT_CORE__
    //         // dp("C:%d L:%d S:%d\n", tbuff[7+reserv_cnt], len, seq_num0);
    //     // #endif
    //     // gap_time = sample_gettimeus()-real_time_gap;
    //     cal_time = (time - test_time);
    //     if ((cal_time > 80000) && fm_end){
    //         // dp("Real Main RTP GAP:%lld cal_time:%lld\n", gap_time, cal_time);
    //         if ((bitrate_change != 300) && fm_end) {
    //             bitrate_change = 300;
    //             dp("Real Main cal_time:%lld\n", cal_time);
    //             // Set_Target_Bit2(bitrate_change);
    //             // bitrate_cnt = sample_gettimeus();
    //         }
    //         bitrate_cnt = sample_gettimeus();
    //     }
    //     // real_time_gap = sample_gettimeus();
    //     test_time = time;
    // }

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
                    memcpy(&tbuff[9+reserv_cnt], data, len);
                    break;
                case REC_CLIP_F:
                case REC_CLIP_B:
                	memcpy(&tbuff[9+reserv_cnt], data, len);
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
                    #ifdef __IOT_CORE__
                        if ((countinue_flag&&!header_ex) || !countinue_flag) {
                            memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                            reserv_cnt += sizeof(RTPHeader);
                            memcpy(&tbuff[9+reserv_cnt], data, len);
                            // memset(&tbuff[9+reserv_cnt], test_data, len);
                        }
                        else {
                            // memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                            // reserv_cnt += sizeof(RTPHeader);
                            memcpy(&tbuff[9+reserv_cnt], data, len);
                            // memset(&tbuff[9+reserv_cnt], test_data, len);
                        }
                    #else
                        memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                        reserv_cnt += sizeof(RTPHeader);
                        memcpy(&tbuff[9+reserv_cnt], data, len);
                    #endif
                    break;
                case STREAM_VEDIO_B:
                    #ifdef __IOT_CORE__
                        if ((countinue_flag&&!header_ex) || !countinue_flag) {
                            memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                            reserv_cnt += sizeof(RTPHeader);
                        	memcpy(&tbuff[9+reserv_cnt], data, len);
                        }
                        else {
                            // memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                            // reserv_cnt += sizeof(RTPHeader);
                            memcpy(&tbuff[9+reserv_cnt], data, len);
                        }
                    #else
                        memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                        reserv_cnt += sizeof(RTPHeader);
                        memcpy(&tbuff[9+reserv_cnt], data, len);
                    #endif
                    break;
                case STREAM_FACE:
                    break;
                case STREAM_AUDIO_F:
                	memcpy(&tbuff[9+reserv_cnt], data, len);
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
        break;
    default:
        return -1;
        break;
    }
    // tbuff[len + 8] = 0x03;
    tbuff[1023-V_SEND_RESERV] = 0x03;
    return 0;
}


int Make_Spi_Packet_live_rtp_b(uint8_t *tbuff, uint8_t *data, uint16_t len, uint8_t major, uint8_t minor, int64_t time, bool fm_end, bool countinue_flag, bool header_ex)
{
    int reserv_cnt = V_SEND_RESERV;
    // uint8_t endchar[4] = {0};
    // int test_data = 0;

    if (len > FILE_READ_LENGTH_LIVE) {
        dp("File Length Over!! %d>1014\n", len);
        return -1;
    }
    memset(tbuff, 0xFF, SPI_SEND_LENGTH);

    RTPHeader header;
    static uint16_t seq_num0 = 0;
    static uint16_t seq_num3 = 0;

    // static int64_t test_time = 0;
    // static int64_t time_check = 0;
    // int64_t cal_time = 0;

    // static int64_t real_time_gap = 0;
    // int64_t gap_time = 0;

    #ifdef __IOT_CORE__
        if ((countinue_flag&&!header_ex) || !countinue_flag) {
            header.version_padding_extension_cc = 0x80;
            if (fm_end)
                header.marker_payload_type = 0xE1;
            else
                header.marker_payload_type = 0x61;

            if (minor == STREAM_VEDIO_M) {
                header.sequence_number[0] = (seq_num0&0xFF00) >> 8;;
                header.sequence_number[1] = seq_num0&0xFF;
            }
            else if (minor == STREAM_VEDIO_B) {
                header.sequence_number[0] = (seq_num3&0xFF00) >> 8;;
                header.sequence_number[1] = seq_num3&0xFF;
            }
            // header.timestamp = sample_gettimeus();
            header.timestamp[0] = (time&0xFF000000) >> 24;
            header.timestamp[1] = (time&0x00FF0000) >> 16;
            header.timestamp[2] = (time&0x0000FF00) >> 8;
            header.timestamp[3] = (time&0x000000FF);

            // if (minor == STREAM_VEDIO_M) {
            //     if (time_check > time) {
            //         dp("t: %lld -> %lld\n", time_check, time);
            //     }
            //     time_check = time;
            // }

            // if (minor == STREAM_VEDIO_M) {
            //     header.timestamp[0] = 0x51;
            //     header.timestamp[1] = 0x79;
            //     header.timestamp[2] = 0x42;
            //     header.timestamp[3] = 0x00;
            // }
            // else if (minor == STREAM_VEDIO_B) {
            //     header.timestamp[0] = 0x51;
            //     header.timestamp[1] = 0x79;
            //     header.timestamp[2] = 0x42;
            //     header.timestamp[3] = 0x01;
            // }
            // header.ssrc = 123465879;
            header.ssrc[0] = 0x20;
            header.ssrc[1] = 0x24;
            header.ssrc[2] = major;
            header.ssrc[3] = minor;

            // memcpy(&tbuff[reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
            if (minor == STREAM_VEDIO_M) seq_num0++;
            else if (minor == STREAM_VEDIO_B) seq_num3++;

            len += sizeof(RTPHeader); 
            // if (STREAM_VEDIO_M) dp("R\n");
        }
    #else
        header.version_padding_extension_cc = 0x80;
        if (fm_end)
            header.marker_payload_type = 0xE1;
        else
            header.marker_payload_type = 0x61;

        if (minor == STREAM_VEDIO_M) {
            header.sequence_number[0] = (seq_num0&0xFF00) >> 8;;
            header.sequence_number[1] = seq_num0&0xFF;
        }
        else if (minor == STREAM_VEDIO_B) {
            header.sequence_number[0] = (seq_num3&0xFF00) >> 8;;
            header.sequence_number[1] = seq_num3&0xFF;
        }
        // header.timestamp = sample_gettimeus();
        header.timestamp[0] = (time&0xFF000000) >> 24;
        header.timestamp[1] = (time&0x00FF0000) >> 16;
        header.timestamp[2] = (time&0x0000FF00) >> 8;
        header.timestamp[3] = (time&0x000000FF);
        // if (minor == STREAM_VEDIO_M) {
        //     header.timestamp[0] = 0x51;
        //     header.timestamp[1] = 0x79;
        //     header.timestamp[2] = 0x42;
        //     header.timestamp[3] = 0x00;
        // }
        // else if (minor == STREAM_VEDIO_B) {
        //     header.timestamp[0] = 0x51;
        //     header.timestamp[1] = 0x79;
        //     header.timestamp[2] = 0x42;
        //     header.timestamp[3] = 0x01;
        // }
        // header.ssrc = 123465879;
        header.ssrc[0] = 0x20;
        header.ssrc[1] = 0x24;
        header.ssrc[2] = major;
        header.ssrc[3] = minor;

        // memcpy(&tbuff[reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
        if (minor == STREAM_VEDIO_M) seq_num0++;
        else if (minor == STREAM_VEDIO_B) seq_num3++;

        len += sizeof(RTPHeader); 
        // if (STREAM_VEDIO_M) dp("R\n");
    #endif

    tbuff[0+reserv_cnt] = 0x02;
    tbuff[1+reserv_cnt] = major & 0xFF;
    tbuff[2+reserv_cnt] = minor & 0xFF;
    tbuff[3+reserv_cnt] = (len>>8) & 0xFF;
    tbuff[4+reserv_cnt] = len & 0xFF;
    tbuff[5+reserv_cnt] = 0x00;
    tbuff[6+reserv_cnt] = 0x00;
    #ifdef __IOT_CORE__
        if (fm_end) {
            // if (header_ex) tbuff[7+reserv_cnt] = 2;
            // else tbuff[7+reserv_cnt] = 1;
            // tbuff[7+reserv_cnt] = 1;
            if (!header_ex) {
                if (len < (V_SEND_SIZE+sizeof(RTPHeader)))  tbuff[7+reserv_cnt] = 1;
                else                    tbuff[7+reserv_cnt] = 2;
            }
            else {
                tbuff[7+reserv_cnt] = 1;
                
            }
        }
        else {
            if (!header_ex) {
                tbuff[7+reserv_cnt] = 2;
            }
            else {
                tbuff[7+reserv_cnt] = 1;
                
            }
        }

        // test_data = seq_num0 & 0xFF;

        // if (!countinue_flag) tbuff[7+reserv_cnt] = 3;

        // if (header_ex) tbuff[7+reserv_cnt] = 2;
        // else tbuff[7+reserv_cnt] = 1;
    #else
        tbuff[7+reserv_cnt] = 0x01;
    #endif
    
    // tbuff[8+reserv_cnt] = 0x00;
    tbuff[8+reserv_cnt] = a_pkt_cnt;


    // if (minor == STREAM_VEDIO_M) {
    //     // #ifdef __IOT_CORE__
    //         // dp("C:%d L:%d S:%d\n", tbuff[7+reserv_cnt], len, seq_num0);
    //     // #endif
    //     // gap_time = sample_gettimeus()-real_time_gap;
    //     cal_time = (time - test_time);
    //     if ((cal_time > 80000) && fm_end){
    //         // dp("Real Main RTP GAP:%lld cal_time:%lld\n", gap_time, cal_time);
    //         if ((bitrate_change != 300) && fm_end) {
    //             bitrate_change = 300;
    //             dp("Real Main cal_time:%lld\n", cal_time);
    //             // Set_Target_Bit2(bitrate_change);
    //             // bitrate_cnt = sample_gettimeus();
    //         }
    //         bitrate_cnt = sample_gettimeus();
    //     }
    //     // real_time_gap = sample_gettimeus();
    //     test_time = time;
    // }

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
                    memcpy(&tbuff[9+reserv_cnt], data, len);
                    break;
                case REC_CLIP_F:
                case REC_CLIP_B:
                    memcpy(&tbuff[9+reserv_cnt], data, len);
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
                    #ifdef __IOT_CORE__
                        if ((countinue_flag&&!header_ex) || !countinue_flag) {
                            memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                            reserv_cnt += sizeof(RTPHeader);
                            memcpy(&tbuff[9+reserv_cnt], data, len);
                            // memset(&tbuff[9+reserv_cnt], test_data, len);
                        }
                        else {
                            // memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                            // reserv_cnt += sizeof(RTPHeader);
                            memcpy(&tbuff[9+reserv_cnt], data, len);
                            // memset(&tbuff[9+reserv_cnt], test_data, len);
                        }
                    #else
                        memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                        reserv_cnt += sizeof(RTPHeader);
                        memcpy(&tbuff[9+reserv_cnt], data, len);
                    #endif
                    break;
                case STREAM_VEDIO_B:
                    #ifdef __IOT_CORE__
                        if ((countinue_flag&&!header_ex) || !countinue_flag) {
                            memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                            reserv_cnt += sizeof(RTPHeader);
                            memcpy(&tbuff[9+reserv_cnt], data, len);
                        }
                        else {
                            // memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                            // reserv_cnt += sizeof(RTPHeader);
                            memcpy(&tbuff[9+reserv_cnt], data, len);
                        }
                    #else
                        memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                        reserv_cnt += sizeof(RTPHeader);
                        memcpy(&tbuff[9+reserv_cnt], data, len);
                    #endif
                    break;
                case STREAM_FACE:
                    break;
                case STREAM_AUDIO_F:
                    memcpy(&tbuff[9+reserv_cnt], data, len);
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
        break;
    default:
        return -1;
        break;
    }
    // tbuff[len + 8] = 0x03;
    tbuff[1023-V_SEND_RESERV] = 0x03;
    return 0;
}

extern pthread_mutex_t buffMutex_ao;

// static int Recv_Spi_Packet_test(uint8_t *rbuff) {
//     int i;

//     for(i=0;i<1024;i++){
//         if (rbuff[i] != i%256) {
//             dp("X %d %d %d\n", i, rbuff[i], i%256);
//             break;
//         }
//     }
//     if (i == 1024)
//         dp("O\n");

//     return 0;
// }

int AIN_CNT = 0;

int64_t voice_timeout = 0;
// int last_send_seq = 0;


static int Recv_Spi_Packet_live(uint8_t *rbuff) {
    int index, len;
    uint8_t major, minor;
    int buff_space = 0;
    // static uint8_t data[10]= {0};
    int bad_cnt = 0;
    static int spicnt = 0;
    static int seqcnt = 0;
    static int seqbuffsend = 0; 
    static int sendseq = 256-16;
    int send_seq_cnt = 0;

    static int err_test_Cnt = 0;
    // static bool amp = false;
    

    

#if 0
    static int filefd = 0;
    if (filefd == 0) {
        filefd = open("/dev/shm/test.pcm", O_RDWR | O_CREAT | O_TRUNC, 0777);
    }

    index = 0;
    major = rbuff[index+1];
    minor = rbuff[index+2];
    if (rbuff[index] != 0x02) {
        // dp("S\n");
        // return -1;
        bad_cnt++;
    }
    
    if (major != 0x82) {
        bad_cnt++;
    } 

    if (minor != 0x07) {
        bad_cnt++;
    }

    if (bad_cnt > 1) {
        return -1;
    }

    len = rbuff[index+3]*256 + rbuff[index+4];

    // dp("spi cnt:%d %d\n", rbuff[index+8], len);

    dp("M : 0x%02x m : 0x%02x len : %d seq : %d\n", major, minor, len, rbuff[index+8]);

    dp("rbuff:0x%02x 0x%02x 0x%02x 0x%02x\n", rbuff[1009], rbuff[1010], rbuff[1023], rbuff[1024]);

    // for (int i=0; i<1000; i++) {
    //     if (i%10 == 0) dp("\n");
    //     dp("0x%02x ", rbuff[index+9+i]);
    // }
    // dp("\n");
    
#else
    // static int filefd = 0;
    // if (filefd == 0) {
    //     filefd = open("/tmp/mnt/sdcard/test.pcm", O_RDWR | O_CREAT | O_TRUNC, 0777);
    // }

    index = 0;
    // for (int i=0; i<10; i++) {
    //     if (rbuff[index] == 0x02) {
    //         index = i;
    //         break;
    //     }
    // }
    major = rbuff[index+1];
    minor = rbuff[index+2];
    if (rbuff[index] != 0x02) {
        // dp("S\n");
        // return -1;
        bad_cnt++;
    }
    
    if (major != 0x82) {
        bad_cnt++;
    } 

    if (minor != 0x07) {
        bad_cnt++;
    }

    if (bad_cnt > 1) {
        return -1;
    }

    len = rbuff[index+3]*256 + rbuff[index+4];

    // dp("spi cnt:%d %d %d\n", spicnt, rbuff[index+8], len);

    if (len < 200){
        return -1;
    }

    
    if (spicnt == rbuff[index+8]) {
        // dp("C\n");
        return -1;
    }
    else {
        // if (spicnt != rbuff[index+8]-1) {

            // dp("spi cnt:%d %d\n", spicnt, rbuff[index+8]);
            // for (i=0; i<10; i++) {
                // dp("0x%02x ", rbuff[i]);
            // }
            // dp("\n");
        // }
        
        spicnt = rbuff[index+8];
        a_pkt_cnt = spicnt;
        
    }
    
    // for(i=0; i<10; i++) {
    //     if (data[i] != rbuff[index+9+i]) {
    //         break;
    //     }
    //     else if(i == 9) {
    //         dp("Same Data!!\n");
    //         return -1;
    //     }
    // }
    // memcpy(data, &rbuff[index+9], 10);

    // dp("spi cnt:%d len : %d\n", spicnt, len);

    if(audio_timeout == 0)
        audio_timeout = sample_gettimeus();

    switch(major) {
    case DTEST_BACK:

    case REC_BACK:
    case STREAMING_BACK:
        switch(minor) {
        case STREAM_AUDIO_B:
            #ifndef __AUDIOE_SEQ__
                if(len > 0){
                    pthread_mutex_lock(&buffMutex_ao);
                    if (AO_Cir_Buff.RIndex != AO_Cir_Buff.WIndex) {
                        buff_space = (AO_Cir_Buff.RIndex - AO_Cir_Buff.WIndex - 1 + A_BUFF_SIZE) % (A_BUFF_SIZE);
                    }
                    else buff_space = A_BUFF_SIZE;
                    if (buff_space >= len) {
                        memset(&AO_Cir_Buff.tx[AO_Cir_Buff.WIndex], 0x00, len);
                        memcpy(&AO_Cir_Buff.tx[AO_Cir_Buff.WIndex], &rbuff[index+9], len);
                        // write(filefd, &rbuff[index+9], len);
                        AO_Cir_Buff.WIndex = (AO_Cir_Buff.WIndex+len) % (500*1024);
                        // dp("[SPIAO]buff_space:%d WIndex:%d RIndex%d\n", buff_space, AO_Cir_Buff.WIndex, AO_Cir_Buff.RIndex);
                        // dp("M : 0x%02x m : 0x%02x len : %d seq : %d\n", major, minor, len, rbuff[index+8]);
                        // dp("Rindex : %d Windex : %d space : %d\n", AO_Cir_Buff.RIndex, AO_Cir_Buff.WIndex, buff_space);
                        // AIN_CNT++;
                        // if (AIN_CNT>10) {
                        //     AIN_CNT = 0;
                        //     dp("AIN\n");    
                        // }
                    }
                    else {
                        dp("AO Cir Buff Overflow!1\n");
                    }
                    pthread_mutex_unlock(&buffMutex_ao);
                }
            #else
                if(len > 0){
                    
                    seqcnt = rbuff[index+9+1]*256 + rbuff[index+9+2];
                    memset(AO_Seq_Buff.tx[seqcnt%256], 0x00, 1024);
                    memcpy(AO_Seq_Buff.tx[seqcnt%256], &rbuff[index+9+3], len-3);
                    AO_Seq_Buff.DE[seqcnt%256] = true;
                    AO_Seq_Buff.LEN[seqcnt%256] = len-3;
                    AO_Seq_Buff.TTSEN[seqcnt%256] = rbuff[index+9];
                    dp("ALST:%d ASEQ:%d ADE:%d ALEN:%d len:%d\n", AO_Seq_Buff.TTSEN[seqcnt%256], seqcnt, AO_Seq_Buff.DE[seqcnt%256], AO_Seq_Buff.LEN[seqcnt%256], len);

                    // if (seqcnt == 0) ao_clear_flag = true;

                    if (AO_Seq_Buff.TTSEN[seqcnt%256] == 1) {
                        tts_start_falg = true;
                        last_recv_time = sample_gettimeus();
                        last_recv_seq = seqcnt%256;
                    }
                    else if (AO_Seq_Buff.TTSEN[seqcnt%256] == 2 && last_recv_flag == false && last_recv_seq != 0) {
                        // dp("TTS Buffer Load Start!!\n");
                        last_recv_flag = true;
                        last_recv_time = sample_gettimeus();
                        last_recv_seq += 1;
                    }
                    else if (AO_Seq_Buff.TTSEN[seqcnt%256] == 0) {
                        if (((seqcnt+256-16)%256) > sendseq)
                            send_seq_cnt = ((seqcnt+256-16)%256)-sendseq;
                        else 
                            send_seq_cnt = 256-sendseq+((seqcnt+256-16)%256);

                        if (send_seq_cnt > 16) send_seq_cnt = 0;
                        // if(send_seq_cnt > 1) dp("Err:%d cnt:%d\n", err_test_Cnt++, seqcnt);
                        // dp("C:%d B:%d A:%d\n", send_seq_cnt, ((seqcnt+256-16)%256), sendseq);
                        for (int i = 0; i < send_seq_cnt; i++) {

                            
                            if (AO_Seq_Buff.DE[(sendseq+i)%256]) {
                                dp("SEND BSEQ:%d\n", (sendseq+i)%256);
                                if (AO_Cir_Buff.RIndex != AO_Cir_Buff.WIndex) {
                                    buff_space = (AO_Cir_Buff.RIndex - AO_Cir_Buff.WIndex - 1 + A_BUFF_SIZE) % (A_BUFF_SIZE);
                                }
                                else buff_space = A_BUFF_SIZE;
                                if (buff_space >= len) {
                                    pthread_mutex_lock(&buffMutex_ao);
                                    if (AO_Seq_Buff.TTSEN[sendseq+i] == 0) {
                                        memset(&AO_Cir_Buff.tx[AO_Cir_Buff.WIndex], 0x00, len);
                                        memcpy(&AO_Cir_Buff.tx[AO_Cir_Buff.WIndex], AO_Seq_Buff.tx[(sendseq+i)%256], AO_Seq_Buff.LEN[(sendseq+i)%256]);
                                    }
                                    AO_Cir_Buff.WIndex = (AO_Cir_Buff.WIndex+AO_Seq_Buff.LEN[(sendseq+i)%256]) % (500*1024);
                                    AO_Seq_Buff.DE[(sendseq+i)%256] = false;
                                    AO_Seq_Buff.LEN[(sendseq+i)%256] = 0;
                                    pthread_mutex_unlock(&buffMutex_ao);
                                }
                                else {
                                    dp("AO Cir Buff Overflow!1\n");
                                }
                            }
                            else {
                                // dp("ERROR BSEQ:%d\n", (sendseq+i)%256);
                                // dp("E%d\n", (sendseq+i)%256);
                            }
                            
                        }
                        sendseq = ((seqcnt+256-16)%256);
                    }

                    // if (seqbuffsend != ((seqcnt%256)/16)) {
                    //     seqbuffsend = (seqcnt%256)/16;
                    //     // dp("BSEQ:%d, BDE:%d BLEN:%d WINDED:%d\n", (seqcnt+256-16)%256, AO_Seq_Buff.DE[(seqcnt+256-16)%256], AO_Seq_Buff.LEN[(seqcnt+256-16)%256], AO_Cir_Buff.WIndex);
                    //     for (int i =0; i<16; i++) {
                            
                    //         if (AO_Seq_Buff.DE[i%256]) {

                    //             pthread_mutex_lock(&buffMutex_ao);
                    //             if (AO_Cir_Buff.RIndex != AO_Cir_Buff.WIndex) {
                    //                 buff_space = (AO_Cir_Buff.RIndex - AO_Cir_Buff.WIndex - 1 + A_BUFF_SIZE) % (A_BUFF_SIZE);
                    //             }
                    //             else buff_space = A_BUFF_SIZE;
                    //             if (buff_space >= len) {
                    //                 memset(&AO_Cir_Buff.tx[AO_Cir_Buff.WIndex], 0x00, len);
                    //                 // memcpy(&AO_Cir_Buff.tx[AO_Cir_Buff.WIndex], &rbuff[index+9], len);
                    //                 memcpy(&AO_Cir_Buff.tx[AO_Cir_Buff.WIndex], AO_Seq_Buff.tx[(seqcnt+256-16+i)%256], AO_Seq_Buff.LEN[(seqcnt+256-16+i)%256]);
                    //                 AO_Cir_Buff.WIndex = (AO_Cir_Buff.WIndex+AO_Seq_Buff.LEN[(seqcnt+256-16+i)%256]) % (500*1024);
                    //                 AO_Seq_Buff.DE[(seqcnt+256-16+i)%256] = false;
                    //                 AO_Seq_Buff.LEN[(seqcnt+256-16+i)%256] = 0;
                    //                 // dp("[SPIAO]buff_space:%d WIndex:%d RIndex%d\n", buff_space, AO_Cir_Buff.WIndex, AO_Cir_Buff.RIndex);
                    //                 // dp("M : 0x%02x m : 0x%02x len : %d seq : %d\n", major, minor, len, rbuff[index+8]);
                    //                 // dp("Rindex : %d Windex : %d space : %d\n", AO_Cir_Buff.RIndex, AO_Cir_Buff.WIndex, buff_space);
                    //                 // AIN_CNT++;
                    //                 // if (AIN_CNT>10) {
                    //                 //     AIN_CNT = 0;
                    //                 //     dp("AIN\n");    
                    //                 // }
                    //             }
                    //             else {
                    //                 dp("AO Cir Buff Overflow!1\n");
                    //             }
                    //             pthread_mutex_unlock(&buffMutex_ao);
                    //         }
                    //         else {
                    //             dp("ERROR BSEQ:%d\n", (seqcnt+256-16+i)%256);
                    //         }
                    //     }
                    // }



                    
                }
            #endif

        break;
        }
    break;
    case SETTING_BACK:
        
    break;
    default:
        return -1;
    break;        
    }
#endif

    return 0;
}


// uint8_t tx_buff[1040] = {0};
// uint8_t tx_tbuff[1040] = {0};
// uint8_t rx_buff[1040] = {0};
// uint8_t read_buff[1040] = {0};
/**
 * SPI功能使用，支持标准Linux接口
 * sample_spi.c目的是测试spi功能,如果使用请按照具体开发需要更改
 * */
// static bool first_send = false;

// int spi_send_clip(int dly, int num)
// {
    
//     int filed = 0, ret = -1;
//     char file1[] = "/mcam/clip1.mp4";
//     char file2[] = "/mcam/clip2.mp4";
//     char file3[] = "/mcam/clip3.mp4";

//     if (num > 0 && num < 4) {
//         if (num == 2) {
//             filed = open(file2, O_RDONLY);
//             dp("File Num set : %s\n", file2);
//         }
//         else if (num == 3) {
//             filed = open(file3, O_RDONLY);
//             dp("File Num set : %s\n", file3);
//         }
//         else if (num == 1) {
//             filed = open(file1, O_RDONLY);
//             dp("File Num set : %s\n", file1);
//         }
//         else {
//             dp("Error: -n option file number 1~3 value.\n");
//             return -1;
//         }
//     }
//     dp("set parse is : device %s ,speed %d ,delay %d ,bpw %d, mode %d\n",device,speed,delay,bits,mode);
//     filed = open(file1, O_RDONLY);
//     read_buff[0] = 1;
//     Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_STREAM_STR);
//     // memset(tx_buff, 0, 1033);
//     // memcpy(&tx_buff[6], read_buff,1);

//     // if (!first_send) {
//     //     spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
//     //     usleep(dly*1000);
//     //     first_send = true;
//     // }

//     spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
//     usleep(dly*1000);
//     do {
//         ret = read(filed, read_buff, FILE_READ_LENGTH);
//         if(ret != 0) {
//             Make_Spi_Packet(tx_buff, read_buff, ret, REC, REC_CLIP_F);
//             // memset(tx_buff, 0, 1024);
//             // memcpy(&tx_buff[6], read_buff, ret);
//             spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
//             // dp("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
//                         // tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3], tx_buff[4], tx_buff[1023]);
//         }
//         usleep(dly*1000);
//     } while(ret != 0);
//     read_buff[0] = 1;
//     Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_STREAM_END);
//     // memset(tx_buff, 0, 1033);
//     // memcpy(&tx_buff[6], read_buff,1);
//     spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
//     usleep(dly*1000);

//     return ret;
// }

int Ready_Busy_Check(void) {
    int ret = -1;

    for (int i=0; i<(READY_BUSY_TIME*2000); i++) {
        usleep(5*100);
        ret = gpio_get_val(PORTB+18);
        if (stream_state == 1 && bl_state != BSS_SEND) {
            return -2;
        }
        if (bell_flag) {
            return -3;
        }
        if (ret == 1) {
            // dp("Ready Busy Check!!\n");
            return ret;
        }
        else {
            if (i == (READY_BUSY_TIME*2000)-1) {
                dp("RB NACk %d\n", READY_BUSY_TIME);
                return ret;
            }
        }
    }
    return -1;
}

int file_exsist_size_check(char *file)
{
    int filed = -1;
    struct stat file_info;
    int sz_file;

    filed = open(file, O_RDONLY);
    if (filed == -1) {
        dp("File %s Open Fail!\n", file);
        return -1;
    }
    else {
        close(filed);
        dp("%s Exsist!!\n", file);
        if ( 0 > stat(file, &file_info)) {
            dp("File Size Not Check!!\n");
            return -1;
        }
        sz_file = file_info.st_size;
        if (sz_file <= 0) {
            dp("File Zero!:%d\n", sz_file);
            return -1;
        }
        else {
            dp("File Check:%d\n", sz_file);
            return sz_file;
        }
    }
}

int spi_send_file(uint8_t minor, char *file, uint8_t recnum, uint8_t clipnum, uint8_t camnum)
{
    int filed = 0, ret = -1;
    // int dly = 3;
    struct stat file_info;
    int sz_file;
    int len = 0;
    int wcnt = 0;

    stream_state = 0;
    usleep(300*1000);
    
    if ( 0 > stat(file, &file_info)) {
    	dp("File Size Not Check!!\n");
    	return -1;
    }

    filed = open(file, O_RDONLY);
    if (filed == -1) {
    	dp("File %s Open Fail!\n", file);
    	return -1;
    }
    sz_file = file_info.st_size;
    if (sz_file < 10*1024) {
        dp("File Size Low!:%d\n", sz_file);
        return -1;
    }
    dp("**********FILE SEND START CMD************\n");
    // dp("d %s,s %d,d %d,b %d,m %d,f %s, size:%d\n",device,speed,delay,bits,mode,file,sz_file);
    memset(tx_buff, 0xff, SPI_SEND_LENGTH);
    dp("%s dummy send!\n", __func__);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

    if (Ready_Busy_Check() > 0){
        // dp("RB Checked!\n");
    }
    else{
        dp("F1:%d\n",wcnt);
        return -1;
    }
    len = 10;

    read_buff[0] = minor;
    read_buff[1] = (sz_file>>24)&0xFF;
    read_buff[2] = (sz_file>>16)&0xFF;
    read_buff[3] = (sz_file>>8)&0xFF;
    read_buff[4] = sz_file&0xFF;
    if (minor == REC_CLIP_F || minor == REC_CLIP_B) {
        read_buff[5] = clip_cause_t.Major;
        read_buff[6] = clip_cause_t.Minor;
    }
    else if (minor == REC_BOX_ALM) {
        read_buff[5] = clip_cause_t.Major;
        read_buff[6] = clip_cause_t.Minor;
    }
    else if (minor == REC_STREAMING_M ||minor == REC_STREAMING_B) {
        read_buff[5] = CLIP_CAUSE_STREM;
        read_buff[6] = CLIP_STREAM_REC;
    }
    else {
        memset(&read_buff[5], 0, 5);
    }
    read_buff[7] = recnum;
    read_buff[8] = clipnum;
    read_buff[9] = camnum;

    if (Ready_Busy_Check() > 0)
        dp("File Send Start!\n");
    else{
        dp("Fail to Start CMD\n");
        return -1;
    }
    dp("Type1 : 0x%02x Type2 : 0x%02x\n", read_buff[5], read_buff[6]);
    Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_STR);
    // dp("S 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
    // if (tx_buff[6] != REC || tx_buff[7] != REC_STREAM_END) {
    //     dp("S1 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
    //     Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_END);
    //     dp("S2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
    // }
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    
    do {
        ret = read(filed, read_buff, FILE_READ_LENGTH);
        if(ret != 0) {
            if (Ready_Busy_Check() > 0){
                // dp("RB Checked!\n");
            }
            else{
                dp("F:%d\n", wcnt);
                return -1;
            }

            // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
            Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
            // dp("D 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
            //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            // if (tx_buff[6] != REC || tx_buff[7] != REC_STREAM_END) {
            //     dp("S1 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
            //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            //     Make_Spi_Packet(tx_buff, read_buff, len, REC, minor);
            //     dp("S2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
            //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            // }
            spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
            // dp("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                        // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
            // for (int i=0; i<1024; i++) {
                // dp(" 0x%02x", tx_buff[i]);
                // if (i%16 == 0) dp("\n");
            // }
        }
        wcnt++;
        // usleep(dly*1000);
    } while(ret != 0);

    if (Ready_Busy_Check() > 0){
        // dp("RB Checked!\n");
    }
    else{
        dp("RB Check Fail!\n");
        return -1;
    }
    read_buff[0] = minor;
    Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_STREAM_END);
    // dp("E 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
    // if (tx_buff[6] != REC || tx_buff[7] != REC_STREAM_END) {
    //     dp("S1 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
    //     Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_END);
    //     dp("S2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
    // }
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    usleep(100*1000);
    dp("**********FILE SEND END CMD************\n");
    return ret;
}

#ifdef __FILE_SEND_CHANGE__
    int file_size_get(char *path)
    {
        struct stat file_info;
        int size_ret = 0;

        if (0 > stat(path, &file_info)) {
            dp("File Not Check!\n");
            return -1;
        }

        size_ret = file_info.st_size;

        return size_ret;
    }

    extern int AV_Off_Thread(void);

    int spi_send_total_clip(FileSend *fs)
    {
        struct stat file_info1, file_info2;
        int ret = -1, cnt = 0, scnt = 0;
        int sz_file[6] = {0}, total_size = 0;
        int len = 0;
        uint8_t wcnt = 0;
        int filed[6] = {0};
        char file[128] = {0};

        dp("m:%d t1:%d t2:%d fn:%d fc:%d\n", fs->minor, fs->tag1, fs->tag2, fs->filenum, fs->filecnt);

        stream_state = 0;
        usleep(300*1000);

        for (cnt=0; cnt<(fs->filecnt); cnt++) {
            #if 1
                memset(file, 0, 128);
                if (fs->tag1 == CLIP_CAUSE_BELL || fs->tag1 == CLIP_CAUSE_MOUNT) {
                    sprintf(file, "/dev/shm/bell_m%d.mp4", cnt);
                }
                else {
                    sprintf(file, "/dev/shm/main%d.mp4", cnt);
                }
            
                
                if ( 0 > stat(file, &file_info1)) {
                    dp("File Size Not Check!! :%s\n", file);
                    break;
                }

                filed[(cnt*2)] = open(file, O_RDONLY);
                if (filed[(cnt*2)] == -1) {
                    dp("File %s Open Fail!\n", file);
                    break;
                }
                // dp("file[%d]:%s\n", (cnt*2), file);
                
                if (file_info1.st_size < 10*1024) {
                    // dp("File Size Low!:%ld\n", file_info1.st_size);
                    break;
                }

                memset(file, 0, 128);
                if (fs->tag1 == CLIP_CAUSE_BELL || fs->tag1 == CLIP_CAUSE_MOUNT) {
                    sprintf(file, "/dev/shm/bell_b%d.mp4", cnt);
                }
                else {
                    sprintf(file, "/dev/shm/box%d.mp4", cnt);
                }
            
                if ( 0 > stat(file, &file_info2)) {
                    dp("File Size Not Check!! :%s\n", file);
                    break;
                }

                filed[(cnt*2)+1] = open(file, O_RDONLY);
                if (filed[(cnt*2)+1] == -1) {
                    dp("File %s Open Fail!\n", file);
                    break;
                }
                // dp("file[%d]:%s\n", (cnt*2)+1, file);
                
                if (file_info2.st_size < 10*1024) {
                    // dp("File Size Low!:%ld\n", file_info2.st_size);
                    break;
                }

            #else
                memset(file, 0, 128);
                sprintf(file, "/tmp/mnt/sdcard/maintest.mp4", cnt);
                            
                if ( 0 > stat(file, &file_info1)) {
                    dp("File Size Not Check!! :%s\n", file);
                    break;
                }

                filed[(cnt*2)] = open(file, O_RDONLY);
                if (filed[(cnt*2)] == -1) {
                    dp("File %s Open Fail!\n", file);
                    break;
                }
                // dp("file[%d]:%s\n", (cnt*2), file);
                
                if (file_info1.st_size < 10*1024) {
                    // dp("File Size Low!:%ld\n", file_info1.st_size);
                    break;
                }

                memset(file, 0, 128);
                sprintf(file, "/tmp/mnt/sdcard/bottest.mp4", cnt);
            
                if ( 0 > stat(file, &file_info2)) {
                    dp("File Size Not Check!! :%s\n", file);
                    break;
                }

                filed[(cnt*2)+1] = open(file, O_RDONLY);
                if (filed[(cnt*2)+1] == -1) {
                    dp("File %s Open Fail!\n", file);
                    break;
                }
                // dp("file[%d]:%s\n", (cnt*2)+1, file);
                
                if (file_info2.st_size < 10*1024) {
                    // dp("File Size Low!:%ld\n", file_info2.st_size);
                    break;
                }
            #endif

            sz_file[(cnt*2)]    += file_info1.st_size;
            if (sz_file[(cnt*2)]%FILE_READ_LENGTH == 0) {
                total_size++;
            }
            sz_file[(cnt*2)+1]  += file_info2.st_size;
            if (sz_file[(cnt*2)+1]%FILE_READ_LENGTH == 0) {
                total_size++;
            }
            total_size += (file_info1.st_size+file_info2.st_size);

        }

        if (cnt == 0) return -1;
      
        dp("**********FILE SEND START CMD : %d ************\n", cnt);
        // dp("d %s,s %d,d %d,b %d,m %d,f %s, size:%d\n",device,speed,delay,bits,mode,file,sz_file);
        memset(tx_buff, 0xff, SPI_SEND_LENGTH);
        dp("%s dummy send!\n", __func__);
        spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

        if (Ready_Busy_Check() > 0){
            // dp("RB Checked!\n");
        }
        else{
            dp("F1:%d\n",wcnt);
            return -1;
        }
        memset(tx_buff, 0xff, SPI_SEND_LENGTH);

        len = 8;

        read_buff[0] = fs->minor;
        read_buff[1] = (total_size>>24)&0xFF;
        read_buff[2] = (total_size>>16)&0xFF;
        read_buff[3] = (total_size>>8)&0xFF;
        read_buff[4] = total_size&0xFF;
        read_buff[5] = cnt*2;
        read_buff[6] = fs->tag1;
        read_buff[7] = fs->tag2;
        dp("S 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            read_buff[0], read_buff[1], read_buff[2], read_buff[3],
            read_buff[4], read_buff[5], read_buff[6], read_buff[7]);
        Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_STR);
        // dp("S 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
        //             tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
        //             tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
        // if (tx_buff[6] != REC || tx_buff[7] != REC_STREAM_STR) {
        //     dp("S1 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
        //             tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
        //             tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
        //     Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_STR);
        //     dp("S2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
        //             tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
        //             tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
        // }
        // if (Ready_Busy_Check() > 0){
        //     // dp("File Send Start!\n");
        // }
        // else{
        //     dp("FS\n");
        //     return -1;
        // }
        // if (stream_state == 1) {
        if (stream_state == 1 && bell_flag) {
            dp("SPI Faile : Streaming\n");
            return 2;
        }
        spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

        for (scnt=0; scnt<cnt; scnt++) {
            dp("SendFile:%d-1\n", scnt+1);
            wcnt = 0;
            dp("rcnt : %d, cflg1 : %d cflag2 : %d bflag : %d\n", cnt-scnt, cfile_flag1, cfile_flag2, bfile_flag);
            if ((cnt - scnt == 1) && (cfile_flag1 && cfile_flag2 && !bfile_flag) && (!bell_rerecode_flag)) {
                dp("Total : %d, Cnt : %d AV Thread Off!\n", cnt, scnt+1);
                AV_Off_Thread();
            }
            do {
                ret = read(filed[(scnt*2)], read_buff, FILE_READ_LENGTH);
                // dp("RC:%d\n", ret);
                if(ret != 0) {
                    // if (stream_state == 1) {
                    if (stream_state == 1 || bell_flag) {
                        dp("SPI Faile : Streaming\n");
                        return 2;
                    }
                    else if (Ready_Busy_Check() > 0){
                        // dp("RB Checked!\n");
                    }
                    else{
                        dp("F:%d\n", wcnt);
                        return -1;
                    }
                    Make_Spi_Packet(tx_buff, read_buff, ret, REC, fs->minor);
                    tx_buff[8+V_SEND_RESERV] = wcnt&0xFF;
                    // dp("D1 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
                    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
                    // if (tx_buff[6] != REC || tx_buff[7] != fs->minor) {
                    //     dp("D11 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
                    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
                    //     Make_Spi_Packet(tx_buff, read_buff, ret, REC, fs->minor);
                    //     tx_buff[8+V_SEND_RESERV] = wcnt&0xFF;
                    //     dp("D12 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
                    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
                    // }
                    spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                    
                    wcnt++;
                }
                
                // usleep(dly*1000);
            } while(ret != 0);

            if (sz_file[(scnt*2)]%FILE_READ_LENGTH == 0) {
                // if (stream_state == 1) {
                if (stream_state == 1 || bell_flag) {
                    dp("SPI Faile : Streaming\n");
                    return 2;
                }
                else if (Ready_Busy_Check() > 0){
                    // dp("RB Checked!\n");
                }
                else{
                    dp("F:%d\n", wcnt);
                    return -1;
                }
                dp("Data 0 Send!!\n");
                Make_Spi_Packet(tx_buff, read_buff, 1, REC, fs->minor);
                tx_buff[8+V_SEND_RESERV] = wcnt&0xFF;
                spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                // dp("E2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                // tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
                // tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            }
            
            len = 9;
            read_buff[0] = fs->minor;
            read_buff[1] = (sz_file[(scnt*2)]>>24)&0xFF;
            read_buff[2] = (sz_file[(scnt*2)]>>16)&0xFF;
            read_buff[3] = (sz_file[(scnt*2)]>>8)&0xFF;
            read_buff[4] = sz_file[(scnt*2)]&0xFF;
            read_buff[5] = (scnt*2)+1;
            read_buff[6] = fs->filenum;
            read_buff[7] = scnt+1;
            read_buff[8] = 1;
            dp("File Size : %d pckcnt : %d wcnt : %d\n", sz_file[(scnt*2)], sz_file[(scnt*2)]/FILE_READ_LENGTH, wcnt);
            dp("E1 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                read_buff[0], read_buff[1], read_buff[2], read_buff[3],
                read_buff[4], read_buff[5], read_buff[6], read_buff[7], read_buff[8]);
            Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_END);
            // dp("E1 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
            //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            // if (tx_buff[6] != REC || tx_buff[7] != REC_STREAM_END) {
            //     dp("E11 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
            //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            //     Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_END);
            //     dp("E12 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
            //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            // }
            // memset(tx_buff, 0, 1033);
            // memcpy(&tx_buff[6], read_buff,1);
            // if (stream_state == 1) {
            if (stream_state == 1 || bell_flag) {
                dp("SPI Faile : Streaming\n");
                return 2;
            }
            else if (Ready_Busy_Check() > 0){
                // dp("RB Checked!\n");
            }
            else{
                dp("FE\n");
                return -1;
            }
            spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

            dp("SendFile:%d-2\n", scnt+1);
            wcnt = 0;
            do {
                ret = read(filed[(scnt*2)+1], read_buff, FILE_READ_LENGTH);
                // dp("RC:%d\n", ret);
                if(ret != 0) {
                    // if (stream_state == 1) {
                    if (stream_state == 1 || bell_flag) {
                        dp("SPI Faile : Streaming\n");
                        return 2;
                    }
                    else if (Ready_Busy_Check() > 0){
                        // dp("RB Checked!\n");
                    }
                    else{
                        dp("F:%d\n", wcnt);
                        return -1;
                    }
                    Make_Spi_Packet(tx_buff, read_buff, ret, REC, fs->minor+1);
                    tx_buff[8+V_SEND_RESERV] = wcnt&0xFF;
                    // dp("D2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
                    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
                    // if (tx_buff[6] != REC || tx_buff[7] != fs->minor+1) {
                    //     dp("D21 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
                    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
                    //     Make_Spi_Packet(tx_buff, read_buff, ret, REC, fs->minor+1);
                    //     tx_buff[8+V_SEND_RESERV] = wcnt&0xFF;
                    //     dp("D22 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                    //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
                    //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
                    // }
                    spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                    
                    wcnt++;
                }
                
                // usleep(dly*1000);
            } while(ret != 0);

            if (sz_file[(scnt*2)+1]%FILE_READ_LENGTH == 0) {
                // if (stream_state == 1) {
                if (stream_state == 1 || bell_flag) {
                    dp("SPI Faile : Streaming\n");
                    return 2;
                }
                else if (Ready_Busy_Check() > 0){
                    // dp("RB Checked!\n");
                }
                else{
                    dp("F:%d\n", wcnt);
                    return -1;
                }
                dp("Data 0 Send!!\n");
                Make_Spi_Packet(tx_buff, read_buff, 1, REC, fs->minor+1);
                tx_buff[8+V_SEND_RESERV] = wcnt&0xFF;
                spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                // dp("E2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                // tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
                // tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            }
            
            len = 9;
            read_buff[0] = fs->minor;
            read_buff[1] = (sz_file[(scnt*2)+1]>>24)&0xFF;
            read_buff[2] = (sz_file[(scnt*2)+1]>>16)&0xFF;
            read_buff[3] = (sz_file[(scnt*2)+1]>>8)&0xFF;
            read_buff[4] = sz_file[(scnt*2)+1]&0xFF;
            read_buff[5] = (scnt*2)+2;
            read_buff[6] = fs->filenum;
            read_buff[7] = scnt+1;
            read_buff[8] = 2;
            dp("File Size : %d pckcnt : %d wcnt : %d\n", sz_file[(scnt*2)+1], sz_file[(scnt*2)+1]/FILE_READ_LENGTH, wcnt);
            dp("E2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                read_buff[0], read_buff[1], read_buff[2], read_buff[3],
                read_buff[4], read_buff[5], read_buff[6], read_buff[7], read_buff[8]);
            Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_END);
            // dp("E2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
            //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            // if (tx_buff[6] != REC || tx_buff[7] != REC_STREAM_END) {
            //     dp("E21 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
            //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            //     Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_END);
            //     dp("E22 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            //         tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
            //         tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            // }
            // memset(tx_buff, 0, 1033);
            // memcpy(&tx_buff[6], read_buff,1);
            if (stream_state == 1) {
                dp("SPI Faile : Streaming\n");
                return 2;
            }
            else if (Ready_Busy_Check() > 0){
                // dp("RB Checked!\n");
            }
            else{
                dp("FE\n");
                return -1;
            }
            spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
            
        }
        dp("**********FILE SEND END CMD************\n");
        usleep(100*1000);
        return ret;
    }

    int spi_send_total_stream_clip(FileSend *fs)
    {
        struct stat file_info1, file_info2;
        int ret = -1, cnt = 0, scnt = 0;
        int sz_file[6] = {0}, total_size = 0;
        int len = 0;
        int wcnt = 0;
        int filed[6] = {0};
        char file[128] = {0};
        // uint8_t seq = 0;

        dp("m:%d t1:%d t2:%d fn:%d fc:%d\n", fs->minor, fs->tag1, fs->tag2, fs->filenum, fs->filecnt);

        stream_state = 0;
        usleep(300*1000);

        for (cnt=0; cnt<(fs->filecnt); cnt++) {
            memset(file, 0, 128);
            sprintf(file, "/dev/shm/rec0_%d_%d.mp4", fs->filenum+1, cnt);
            
            if ( 0 > stat(file, &file_info1)) {
                dp("File Size Not Check!!\n");
                break;
            }

            filed[(cnt*2)] = open(file, O_RDONLY);
            if (filed[(cnt*2)] == -1) {
                dp("File %s Open Fail!\n", file);
                break;
            }
            // dp("file[%d]:%s\n", (cnt*2), file);
            
            if (file_info1.st_size < 10*1024) {
                // dp("File Size Low!:%ld\n", file_info1.st_size);
                break;
            }

            memset(file, 0, 128);
            sprintf(file, "/dev/shm/rec3_%d_%d.mp4", fs->filenum+1, cnt);
        
            if ( 0 > stat(file, &file_info2)) {
                dp("File Size Not Check!!\n");
                break;
            }

            filed[(cnt*2)+1] = open(file, O_RDONLY);
            if (filed[(cnt*2)+1] == -1) {
                dp("File %s Open Fail!\n", file);
                break;
            }
            // dp("file[%d]:%s\n", (cnt*2)+1, file);
            
            if (file_info2.st_size < 10*1024) {
                // dp("File Size Low!:%ld\n", file_info2.st_size);
                break;
            }

            // sz_file[(cnt*2)]    += file_info1.st_size;
            // sz_file[(cnt*2)+1]  += file_info2.st_size;
            sz_file[(cnt*2)]    += file_info1.st_size;
            if (sz_file[(cnt*2)]%SPI_SEND_LENGTH == 0) {
                total_size++;
            }
            sz_file[(cnt*2)+1]  += file_info2.st_size;
            if (sz_file[(cnt*2)+1]%SPI_SEND_LENGTH == 0) {
                total_size++;
            }
            total_size += (file_info1.st_size+file_info2.st_size);

        }

        if (cnt == 0) return -1;
      
        dp("**********FILE SEND START CMD : %d ************\n", cnt);
        // dp("d %s,s %d,d %d,b %d,m %d,f %s, size:%d\n",device,speed,delay,bits,mode,file,sz_file);
        memset(tx_buff, 0xff, SPI_SEND_LENGTH);
        dp("%s dummy send!\n", __func__);
        spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

        if (Ready_Busy_Check() > 0){
            // dp("RB Checked!\n");
        }
        else{
            dp("F1:%d\n",wcnt);
            return -1;
        }

        memset(tx_buff, 0xff, SPI_SEND_LENGTH);

        len = 8;

        read_buff[0] = fs->minor;
        read_buff[1] = (total_size>>24)&0xFF;
        read_buff[2] = (total_size>>16)&0xFF;
        read_buff[3] = (total_size>>8)&0xFF;
        read_buff[4] = total_size&0xFF;
        read_buff[5] = cnt*2;
        read_buff[6] = fs->tag1;
        read_buff[7] = fs->tag2;
        dp("S 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            read_buff[0], read_buff[1], read_buff[2], read_buff[3],
            read_buff[4], read_buff[5], read_buff[6], read_buff[7]);
        Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_STR);
      
        // if (Ready_Busy_Check() > 0){
        //     // dp("File Send Start!\n");
        // }
        // else{
        //     dp("FS\n");
        //     return -1;
        // }
        // if (stream_state == 1) {
        if (stream_state == 1 || bell_flag) {
            dp("SPI Faile : Streaming\n");
            return 2;
        }
            
        spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

        for (scnt=0; scnt<cnt; scnt++) {
            dp("SendFile:%d-1\n", scnt+1);
            wcnt = 0;
            do {
                ret = read(filed[(scnt*2)], read_buff, FILE_READ_LENGTH);
                // dp("RC:%d\n", ret);
                if(ret != 0) {
                    // if (stream_state == 1) {
                    if (stream_state == 1 || bell_flag) {
                        dp("SPI Faile : Streaming\n");
                        return 2;
                    }
                    else if (Ready_Busy_Check() > 0){
                        // dp("RB Checked!\n");
                    }
                    else{
                        dp("F:%d\n", wcnt);
                        return -1;
                    }
                    Make_Spi_Packet(tx_buff, read_buff, ret, REC, fs->minor);
                    tx_buff[8+V_SEND_RESERV] = wcnt&0xFF;
                    spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                }
                wcnt++;
                // usleep(dly*1000);
            } while(ret != 0);

            if (sz_file[(scnt*2)]%SPI_SEND_LENGTH == 0) {
                // if (stream_state == 1) {
                if (stream_state == 1 || bell_flag) {
                    dp("SPI Faile : Streaming\n");
                    return 2;
                }
                else if (Ready_Busy_Check() > 0){
                    // dp("RB Checked!\n");
                }
                else{
                    dp("F:%d\n", wcnt);
                    return -1;
                }
                dp("Data 0 Send!!\n");
                Make_Spi_Packet(tx_buff, read_buff, 1, REC, fs->minor);
                tx_buff[8+V_SEND_RESERV] = wcnt&0xFF;
                spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                // dp("E2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                // tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
                // tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            }
            
            len = 9;
            read_buff[0] = fs->minor;
            read_buff[1] = (sz_file[(scnt*2)]>>24)&0xFF;
            read_buff[2] = (sz_file[(scnt*2)]>>16)&0xFF;
            read_buff[3] = (sz_file[(scnt*2)]>>8)&0xFF;
            read_buff[4] = sz_file[(scnt*2)]&0xFF;
            read_buff[5] = (scnt*2)+1;
            read_buff[6] = fs->filenum;
            read_buff[7] = scnt+1;
            read_buff[8] = 1;
            dp("E1 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                read_buff[0], read_buff[1], read_buff[2], read_buff[3],
                read_buff[4], read_buff[5], read_buff[6], read_buff[7], read_buff[8]);
            Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_END);
            // memset(tx_buff, 0, 1033);
            // memcpy(&tx_buff[6], read_buff,1);
            if (stream_state == 1) {
                dp("SPI Faile : Streaming\n");
                return 2;
            }
            else if (Ready_Busy_Check() > 0){
                // dp("RB Checked!\n");
            }
            else{
                dp("FE\n");
                return -1;
            }
            spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

            dp("SendFile:%d-2\n", scnt+1);
            wcnt = 0;
            do {
                ret = read(filed[(scnt*2)+1], read_buff, FILE_READ_LENGTH);
                // dp("RC:%d\n", ret);
                if(ret != 0) {
                    // if (stream_state == 1) {
                    if (stream_state == 1 || bell_flag) {
                        dp("SPI Faile : Streaming\n");
                        return 2;
                    }
                    else if (Ready_Busy_Check() > 0){
                        // dp("RB Checked!\n");
                    }
                    else{
                        dp("F:%d\n", wcnt);
                        return -1;
                    }
                    Make_Spi_Packet(tx_buff, read_buff, ret, REC, fs->minor+1);
                    tx_buff[8+V_SEND_RESERV] = wcnt&0xFF;
                    spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                }
                wcnt++;
                // usleep(dly*1000);
            } while(ret != 0);

            if (sz_file[(scnt*2)+1]%SPI_SEND_LENGTH == 0) {
                // if (stream_state == 1) {
                if (stream_state == 1 || bell_flag) {
                    dp("SPI Faile : Streaming\n");
                    return 2;
                }
                else if (Ready_Busy_Check() > 0){
                    // dp("RB Checked!\n");
                }
                else{
                    dp("F:%d\n", wcnt);
                    return -1;
                }
                dp("Data 0 Send!!\n");
                Make_Spi_Packet(tx_buff, read_buff, 1, REC, fs->minor+1);
                tx_buff[8+V_SEND_RESERV] = wcnt&0xFF;
                spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                // dp("E2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                // tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3],
                // tx_buff[4], tx_buff[5], tx_buff[6], tx_buff[7], tx_buff[8]);
            }

            len = 9;
            read_buff[0] = fs->minor;
            read_buff[1] = (sz_file[(scnt*2)+1]>>24)&0xFF;
            read_buff[2] = (sz_file[(scnt*2)+1]>>16)&0xFF;
            read_buff[3] = (sz_file[(scnt*2)+1]>>8)&0xFF;
            read_buff[4] = sz_file[(scnt*2)+1]&0xFF;
            read_buff[5] = (scnt*2)+2;
            read_buff[6] = fs->filenum;
            read_buff[7] = scnt+1;
            read_buff[8] = 2;
            dp("E2 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
                read_buff[0], read_buff[1], read_buff[2], read_buff[3],
                read_buff[4], read_buff[5], read_buff[6], read_buff[7], read_buff[8]);
            Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_END);
            // memset(tx_buff, 0, 1033);
            // memcpy(&tx_buff[6], read_buff,1);
            // if (stream_state == 1) {
            if (stream_state == 1 || bell_flag) {
                dp("SPI Faile : Streaming\n");
                return 2;
            }
            else if (Ready_Busy_Check() > 0){
                // dp("RB Checked!\n");
            }
            else{
                dp("FE\n");
                return -1;
            }
            spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
        }
        dp("**********FILE SEND END CMD************\n");
        usleep(100*1000);
        return ret;
    }

#endif

#if 0
    int spi_send_file_face(uint8_t minor, int fcnt)
    {
        int filed = 0, ret = -1;
        // int dly = 3;
        struct stat file_info;
        int sz_file=0;
        int len = 0;
        int wcnt = 0;
        char file[64];
        int i = 0;

        stream_state = 0;
        usleep(300*1000);

        for (i=0; i<fcnt; i++) {

            sprintf(file, "/dev/shm/face_crop%d.jpg", i);
        
            if ( 0 > stat(file, &file_info)) {
                dp("File Size Not Check!!\n");
                return -1;
            }

            filed = open(file, O_RDONLY);
            if (filed == -1) {
                dp("File %s Open Fail!\n", file);
                return -1;
            }
            else {
                close(filed);
            }
            sz_file += file_info.st_size;
        }
        dp("**********FACE SEND START %d ************\n", sz_file);
        // dp("d %s,s %d,d %d,b %d,m %d,f %s, size:%d\n",device,speed,delay,bits,mode,file,sz_file);
        memset(read_buff, 0, 7);

        read_buff[0] = minor;
        read_buff[1] = (sz_file>>24)&0xFF;
        read_buff[2] = (sz_file>>16)&0xFF;
        read_buff[3] = (sz_file>>8)&0xFF;
        read_buff[4] = sz_file&0xFF;
        read_buff[5] = fcnt;
        read_buff[6] = 0;
        len = 7;

        if (Ready_Busy_Check() > 0){
            // dp("File Send Start!\n");
        }
        else{
            dp("[RB ERR]Face Start CMD\n");
            return -1;
        }
        
        dp("Face File Count : %d\n", read_buff[5]);
        Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_STR);

        spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
        
        for (i=0; i<fcnt; i++) {
            memset(file, 0, 64);
            sprintf(file, "/dev/shm/face_crop%d.jpg", i);
            filed = open(file, O_RDONLY);
            if (filed == -1) {
                dp("File %s Open Fail!\n", file);
                return -1;
            }
            do {
                ret = read(filed, read_buff, FILE_READ_LENGTH);
                if(ret != 0) {
                    if (Ready_Busy_Check() > 0){
                        // dp("RB Checked!\n");
                    }
                    else{
                        dp("F[%d]:%d\n", i, wcnt);
                        return -1;
                    }

                    // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
                    Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
                    // memset(tx_buff, 0, 1024);
                    // memcpy(&tx_buff[6], read_buff, ret);
                    spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                    // dp("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                                // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
                    // for (int i=0; i<1024; i++) {
                        // dp(" 0x%02x", tx_buff[i]);
                        // if (i%16 == 0) dp("\n");
                    // }
                }
                wcnt++;
                // usleep(dly*1000);
            } while(ret != 0);
            if (Ready_Busy_Check() > 0){
                // dp("RB Checked!\n");
            }
            else{
                dp("FE[%d]:%d\n", i, wcnt);
                return -1;
            }
            read_buff[0] = i+1;
            Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_FACE_END);
            spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
        }

        if (Ready_Busy_Check() > 0){
            // dp("RB Checked!\n");
        }
        else{
            dp("[RB ERR]Face End CMD\n");
            return -1;
        }
        read_buff[0] = minor;
        Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_STREAM_END);
        // memset(tx_buff, 0, 1033);
        // memcpy(&tx_buff[6], read_buff,1);
        spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
        usleep(100*1000);
        dp("**********FACE SEND END ************\n");
        return ret;
    }

#else

    int spi_send_file_face(uint8_t minor, int fcnt)
    {
        int filed = 0, ret = -1;
        // int dly = 3;
        struct stat file_info;
        int sz_file=0;
        int len = 0;
        int wcnt = 0;
        char file[64];
        int i = 0;

        stream_state = 0;
        usleep(300*1000);

        sprintf(file, "/dev/shm/thumbnail_last.jpg");
        
        if ( 0 > stat(file, &file_info)) {
            dp("File Size Not Check!!\n");
            return -1;
        }

        filed = open(file, O_RDONLY);
        if (filed == -1) {
            dp("File %s Open Fail!\n", file);
            return -1;
        }
        else {
            close(filed);
        }
        sz_file += file_info.st_size;

        for (i=0; i<fcnt; i++) {

            sprintf(file, "/dev/shm/face_crop%d.jpg", i);
        
            if ( 0 > stat(file, &file_info)) {
                dp("File Size Not Check!!\n");
                return -1;
            }

            filed = open(file, O_RDONLY);
            if (filed == -1) {
                dp("File %s Open Fail!\n", file);
                return -1;
            }
            else {
                close(filed);
            }
            sz_file += file_info.st_size;
        }
        dp("**********FACE SEND START %d %d ************\n", sz_file, fcnt);
        
        // dp("d %s,s %d,d %d,b %d,m %d,f %s, size:%d\n",device,speed,delay,bits,mode,file,sz_file);
        memset(tx_buff, 0xff, SPI_SEND_LENGTH);
        dp("%s dummy send!\n", __func__);
        spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

        if (Ready_Busy_Check() > 0){
            // dp("RB Checked!\n");
        }
        else{
            dp("F1:%d\n",wcnt);
            return -1;
        }

        memset(read_buff, 0, 7);

        read_buff[0] = minor;
        read_buff[1] = (sz_file>>24)&0xFF;
        read_buff[2] = (sz_file>>16)&0xFF;
        read_buff[3] = (sz_file>>8)&0xFF;
        read_buff[4] = sz_file&0xFF;
        read_buff[5] = fcnt+1;
        read_buff[6] = 0;
        len = 7;

        // if (Ready_Busy_Check() > 0){
        //     // dp("File Send Start!\n");
        // }
        // else{
        //     dp("[RB ERR]Face Start CMD\n");
        //     return -1;
        // }
        
        dp("Face File Count : %d\n", read_buff[5]);
        Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_STR);
        dp("0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n", 
            read_buff[0], read_buff[1], read_buff[2], read_buff[3], read_buff[4], read_buff[5]);

        spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);


        memset(file, 0, 64);
        sprintf(file, "/dev/shm/thumbnail_last.jpg");
        filed = open(file, O_RDONLY);
        do {
            ret = read(filed, read_buff, FILE_READ_LENGTH);
            if(ret != 0) {
                if (Ready_Busy_Check() > 0){
                    // dp("RB Checked!\n");
                }
                else{
                    dp("F[%d]:%d\n", i, wcnt);
                    return -1;
                }

                // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
                Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
                // memset(tx_buff, 0, 1024);
                // memcpy(&tx_buff[6], read_buff, ret);
                spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                // dp("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                            // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
                // for (int i=0; i<1024; i++) {
                    // dp(" 0x%02x", tx_buff[i]);
                    // if (i%16 == 0) dp("\n");
                // }
            }
            wcnt++;
            // usleep(dly*1000);
        } while(ret != 0);
        if (Ready_Busy_Check() > 0){
            // dp("RB Checked!\n");
        }
        else{
            dp("FE[%d]:%d\n", i, wcnt);
            return -1;
        }
        read_buff[0] = 1;
        Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_FACE_END);
        spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
        dp("Thumnail shot!!\n");

        close(filed);
        
        for (i=0; i<fcnt; i++) {
            memset(file, 0, 64);
            sprintf(file, "/dev/shm/face_crop%d.jpg", i);
            filed = open(file, O_RDONLY);
            if (filed == -1) {
                dp("File %s Open Fail!\n", file);
                return -1;
            }
            do {
                ret = read(filed, read_buff, FILE_READ_LENGTH);
                if(ret != 0) {
                    if (Ready_Busy_Check() > 0){
                        // dp("RB Checked!\n");
                    }
                    else{
                        dp("F[%d]:%d\n", i, wcnt);
                        return -1;
                    }

                    // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
                    Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
                    // memset(tx_buff, 0, 1024);
                    // memcpy(&tx_buff[6], read_buff, ret);
                    spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                    // dp("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                                // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
                    // for (int i=0; i<1024; i++) {
                        // dp(" 0x%02x", tx_buff[i]);
                        // if (i%16 == 0) dp("\n");
                    // }
                }
                wcnt++;
                // usleep(dly*1000);
            } while(ret != 0);
            if (Ready_Busy_Check() > 0){
                // dp("RB Checked!\n");
            }
            else{
                dp("FE[%d]:%d\n", i, wcnt);
                return -1;
            }
            read_buff[0] = i+2;
            Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_FACE_END);
            spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
            dp("face shot %d!!\n", i);

            close(filed);
        }

        if (Ready_Busy_Check() > 0){
            // dp("RB Checked!\n");
        }
        else{
            dp("[RB ERR]Face End CMD\n");
            return -1;
        }
        read_buff[0] = minor;
        Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_STREAM_END);
        // memset(tx_buff, 0, 1033);
        // memcpy(&tx_buff[6], read_buff,1);
        spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
        usleep(100*1000);
        dp("**********FACE SEND END ************\n");
        return ret;
    }
#endif

int spi_send_file_dual(uint8_t minor1, uint8_t minor2, char *file1, char *file2)
{
    int filed1 = 0, filed2 = 0, ret = -1;
    // int dly = 3;
    struct stat file_info1, file_info2;
    int sz_file;
    int len = 0;
    int wcnt = 0;
 
    stream_state = 0;
    usleep(300*1000);
    
    if ( 0 > stat(file1, &file_info1)) {
        dp("File1 Size Not Check!!\n");
        return -1;
    }

    

    if ( 0 > stat(file2, &file_info2)) {
        dp("File2 Size Not Check!!\n");
        return -1;
    }

    
    sz_file = file_info1.st_size + file_info2.st_size;

    if (sz_file < 10*1024) {
        dp("File Size Low!:%d\n", sz_file);
        return -1;
    }
    dp("**********FILE SEND START CMD************\n");
    // dp("d %s,s %d,d %d,b %d,m %d,f %s, size:%d\n",device,speed,delay,bits,mode,file,sz_file);
    memset(tx_buff, 0xff, SPI_SEND_LENGTH);
    dp("%s dummy send!\n", __func__);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

    if (Ready_Busy_Check() > 0){
        // dp("RB Checked!\n");
    }
    else{
        dp("F1:%d\n",wcnt);
        return -1;
    }

    memset(tx_buff, 0xff, SPI_SEND_LENGTH);

    read_buff[0] = minor1;
    read_buff[1] = (sz_file>>24)&0xFF;
    read_buff[2] = (sz_file>>16)&0xFF;
    read_buff[3] = (sz_file>>8)&0xFF;
    read_buff[4] = sz_file&0xFF;
    len = 5;
   
    Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_STR);
    // memset(tx_buff, 0, 1033);
    // memcpy(&tx_buff[6], read_buff,1);

    // if (!first_send) {
    //     spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    //     usleep(dly*1000);
    //     first_send = true;
    // }
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

    
    if (Ready_Busy_Check() > 0)
        dp("File Send Start!\n");
    else{
        dp("Fail to Start1 CMD\n");
        return -1;
    }

    filed1 = open(file1, O_RDONLY);
    if (filed1 == -1) {
        dp("File1 %s Open Fail!\n", file1);
        return -1;
    }

    do {
        ret = read(filed1, read_buff, FILE_READ_LENGTH);
        if(ret != 0) {
            if (Ready_Busy_Check() > 0){
                // dp("RB Checked!\n");
            }
            else{
                dp("F1:%d\n",wcnt);
                return -1;
            }

            // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor1);
            Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor1);
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
            // dp("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                        // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
            // for (int i=0; i<1024; i++) {
                // dp(" 0x%02x", tx_buff[i]);
                // if (i%16 == 0) dp("\n");
            // }
        }
        wcnt++;
        // usleep(dly*1000);
    } while(ret != 0);

    close(filed1);

    // if (Ready_Busy_Check() > 0)
    //     dp("File Send Start!\n");
    // else{
    //     dp("Fail to Start2 CMD\n");
    //     return -1;
    // }

    // read_buff[0] = minor2;
    // read_buff[1] = (sz_file>>24)&0xFF;
    // read_buff[2] = (sz_file>>16)&0xFF;
    // read_buff[3] = (sz_file>>8)&0xFF;
    // read_buff[4] = sz_file&0xFF;
    // len = 5;
   
    // Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_STR);
    // // memset(tx_buff, 0, 1033);
    // // memcpy(&tx_buff[6], read_buff,1);

    // // if (!first_send) {
    // //     spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    // //     usleep(dly*1000);
    // //     first_send = true;
    // // }
    // spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);


    filed2 = open(file2, O_RDONLY);
    if (filed2 == -1) {
        dp("File2 %s Open Fail!\n", file2);
        return -1;
    }

    wcnt = 0;

    do {
        ret = read(filed2, read_buff, FILE_READ_LENGTH);
        if(ret != 0) {
            if (Ready_Busy_Check() > 0){
                // dp("RB Checked!\n");
            }
            else{
                dp("F2:%d\n",wcnt);
                return -1;
            }

            // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor1);
            Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor2);
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
            // dp("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                        // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
            // for (int i=0; i<1024; i++) {
                // dp(" 0x%02x", tx_buff[i]);
                // if (i%16 == 0) dp("\n");
            // }
        }
        wcnt++;
        // usleep(dly*1000);
    } while(ret != 0);

    close(filed2);

    if (Ready_Busy_Check() > 0){
        // dp("RB Checked!\n");
    }
    else{
        dp("RB Check Fail!\n");
        return -1;
    }
    read_buff[0] = minor2;
    Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_STREAM_END);
    // memset(tx_buff, 0, 1033);
    // memcpy(&tx_buff[6], read_buff,1);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    usleep(100*1000);
    dp("**********FILE SEND END CMD************\n");
    return ret;
}


int spi_send_fake_file(uint8_t minor)
{
    int ret = -1;
    int dly = 3;
    int sz_file;
    
    sz_file = 1024*600;
    // sz_file = 900*20 + 450;
    dp("**********SPI FILE SEND************\n");
    dp("d %s,s %d,d %d,b %d,m %d\n",device,speed,delay,bits,mode);

    memset(tx_buff, 0xff, SPI_SEND_LENGTH);
    dp("%s dummy send!\n", __func__);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

    if (Ready_Busy_Check() > 0){
        // dp("RB Checked!\n");
    }
    else{
        return -1;
    }

    memset(tx_buff, 0xff, SPI_SEND_LENGTH);

    read_buff[0] = minor;
    read_buff[1] = (sz_file>>24)&0xFF;
    read_buff[2] = (sz_file>>16)&0xFF;
    read_buff[3] = (sz_file>>8)&0xFF;
    read_buff[4] = sz_file&0xFF;
    Make_Spi_Packet(tx_buff, read_buff, 5, REC, REC_STREAM_STR);

    
    
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    usleep(500*1000);
    for(int j=0; j<600; j++) {
        for (int i=0; i<1024; i++) {
            // tx_tbuff[i] = ((i/20)%10) + 0x30;
            tx_buff[i] = i%0xFF;
        }
        // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
        // Make_Spi_Packet(tx_buff, tx_tbuff, 900, REC, minor);
        // memset(tx_buff, 0, 1024);
        // memcpy(&tx_buff[6], read_buff, ret);
        spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
        // dp("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x 0x%02x 0x%02x 0x%02x\n", 
                    // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-2], tx_buff[1023-3], tx_buff[1023-4], tx_buff[1023-5]);
        // dp("data:0x%02x 0x%02x 0x%02x\n", tx_buff[9], tx_buff[9+20], tx_buff[9+40]);
        dp("Data PKT : %d\n", j);
        usleep(dly*1000);
    }
    
    // for (int num = 0, i=0; i<900; i++) {
    //     tx_tbuff[i] = ((i/20)%10) + 0x30;
    // }
    // // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
    // Make_Spi_Packet(tx_buff, tx_tbuff, FILE_READ_LENGTH/2, REC, minor);
    // // memset(tx_buff, 0, 1024);
    // // memcpy(&tx_buff[6], read_buff, ret);

    // spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
    // dp("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
    //                     tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3], tx_buff[4], tx_buff[1023]);
    // dp("data:0x%02x 0x%02x 0x%02x\n", tx_tbuff[9], tx_tbuff[9+20], tx_tbuff[9+40]);
    // usleep(dly*1000);

    read_buff[0] = minor;
    Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_STREAM_END);
    // memset(tx_buff, 0, 1033);
    // memcpy(&tx_buff[6], read_buff,1);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    usleep(500*1000);

    return ret;
}

// 파일명을 파싱하여 구조체에 저장하는 함수
int parse_filename(char* filename, FileInfo* file_info) {
    int ret = -1;
    // 파일명에서 각 부분을 추출
    ret = sscanf(filename, "%12s_%2d_%2d_%2d%2d.mp4", file_info->date, &file_info->order, &file_info->top_bottom, &file_info->type1, &file_info->type2);

    return ret;
}

int spi_send_save_file(char *path, char *file)
{
    int filed = 0, ret = -1;
    // int dly = 3;
    struct stat file_info;
    int sz_file;
    int len = 0;
    int wcnt = 0;
    FileInfo fileinfo;
    char filebuf[128];
    
    // dp("s:%s", file);
    sprintf(filebuf, "%s%s", path, file);

    if ( 0 > stat(filebuf, &file_info)) {
        dp("File Size Not Check!!\n");
        return -1;
    }

    filed = open(filebuf, O_RDONLY);
    if (filed == -1) {
        dp("File %s Open Fail!\n", filebuf);
        return -1;
    }

    parse_filename(file, &fileinfo);

    sz_file = file_info.st_size;

    if (sz_file < 10*1024) {
        dp("File Size Low!:%d\n", sz_file);
        return -1;
    }

    dp("size:%d date:%s order:%d topbot:%d type1:%d type2:%d\n", 
        sz_file, fileinfo.date, fileinfo.order, fileinfo.top_bottom, fileinfo.type1, fileinfo.type2);

    dp("**********SAVE SEND START CMD************\n");
    // // dp("d %s,s %d,d %d,b %d,m %d,f %s, size:%d\n",device,speed,delay,bits,mode,file,sz_file);

    // Save File Date
    read_buff[0] = (char)fileinfo.date[0];
    read_buff[1] = (char)fileinfo.date[1];
    read_buff[2] = (char)fileinfo.date[2];
    read_buff[3] = (char)fileinfo.date[3];
    read_buff[4] = (char)fileinfo.date[4];
    read_buff[5] = (char)fileinfo.date[5];
    read_buff[6] = (char)fileinfo.date[6];
    read_buff[7] = (char)fileinfo.date[7];
    read_buff[8] = (char)fileinfo.date[8];
    read_buff[9] = (char)fileinfo.date[9];
    read_buff[10] = (char)fileinfo.date[10];
    read_buff[11] = (char)fileinfo.date[11];

    // Save File Order
    read_buff[12] = fileinfo.order&0xFF;

    // Save File Top Bottom
    read_buff[13] = fileinfo.top_bottom&0xFF;

    // Save File Type
    read_buff[14] = fileinfo.type1&0xFF;
    read_buff[15] = fileinfo.type2&0xFF;

    // Save File Size
    read_buff[16] = (sz_file>>24)&0xFF;
    read_buff[17] = (sz_file>>16)&0xFF;
    read_buff[18] = (sz_file>>8)&0xFF;
    read_buff[19] = sz_file&0xFF;

    len = 20;

    if (Ready_Busy_Check() < 1) {
        dp("[%s]SF\n", __func__);
        return -1;
    }
    
    Make_Spi_Packet(tx_buff, read_buff, len, SETTING, SET_FILE_START);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

    do {
        ret = read(filed, read_buff, FILE_READ_LENGTH);
        if(ret != 0) {
            if (Ready_Busy_Check() < 1){
                dp("F:%d\n", wcnt);
                return -1;
            }

            Make_Spi_Packet(tx_buff, read_buff, ret, SETTING, SET_FILE_SEND);
            spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
        }
        wcnt++;
    } while(ret != 0);

    if (Ready_Busy_Check() < 1){
        dp("[%s]EF\n", __func__);
        return -1;
    }
    Make_Spi_Packet(tx_buff, read_buff, 0, SETTING, SET_FILE_END);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    usleep(100*1000);
    dp("**********FILE SEND END CMD************\n");
    return ret;
}

int spi_device_off(uint8_t major)
{
    dp("off device %s ,speed %d ,delay %d ,bpw %d, mode %d\n",device,speed,delay,bits,mode);
    Make_Spi_Packet(tx_buff, read_buff, 0, major, REC_DEV_STOP);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    usleep(1*1000);
    
    return 0;
}

#if 0 // UDP Unused
pthread_mutex_t buffMutex_ao = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buffMutex_ai = PTHREAD_MUTEX_INITIALIZER;

pthread_mutex_t buffMutex_vm = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t buffMutex_vb = PTHREAD_MUTEX_INITIALIZER;
#else // UDP Used

extern pthread_mutex_t buffMutex_ai;

extern pthread_mutex_t buffMutex_vm;
extern pthread_mutex_t buffMutex_vb;
#endif

// void *spi_send_stream_bak (void *arg)
// {
    
//     int ret = -1;
//     uint8_t *buf;
// 	int datasize = 0;
// 	int framesize = 0;
//     bool frame_end = false;
//     bool main_first = false;

// 	buf = (uint8_t*)malloc(2000);
  
//     do {
//         /////////// Vedio Main IN -> UDP Out //////////////////////////////////////////
//         if (VM_Frame_Buff.cnt > 0) {
//             framesize = VM_Frame_Buff.len[VM_Frame_Buff.Rindex];
//             for(int i=0; framesize > 0; i++){
//                 pthread_mutex_lock(&buffMutex_vm);
//                 datasize = (framesize > V_SEND_SIZE) ? V_SEND_SIZE : framesize;
//                 // udp_vm_send(VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize);
//                 framesize -= datasize;
//                 // dp("cnt:%d, total:%d, dsize:%d\n", i, framesize, datasize);
//                 pthread_mutex_unlock(&buffMutex_vm);
//                 // dp("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
//             #ifdef __H265__
//                 Make_Spi_Packet_live(tx_buff, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize, STREAMING, STREAM_VEDIO_M);
//             #else
//                 if (framesize < V_SEND_SIZE)    frame_end = true;
//                 else                            frame_end = false;
//                 Make_Spi_Packet_live_rtp(tx_buff, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*i), 
//                                             datasize, STREAMING, STREAM_VEDIO_M, VM_Frame_Buff.ftime[VM_Frame_Buff.Rindex], frame_end);

//                 // dp("end1:0x%02x 0x%02x 0x%02x 0x%02x\n", tx_buff[datasize+12+9], tx_buff[datasize+12+9+1], tx_buff[datasize+12+9+2], tx_buff[datasize+12+9+3]);
//             #endif
                
//                 // memset(tx_buff, 0, 1024);
//                 // memcpy(&tx_buff[6], read_buff, ret);
//                 ///////////////// SPI Send //////////////////////////
//                 if (data_sel == 1 || data_sel == 4) {
//                     // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
//                     ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
//                     if (ret != 0) {
//                         dp("Fail Send SPI Data!\n");
//                     }
//                     else {
//                         // dp("cnt:%d total:%d dsize%d\n", i, framesize, datasize);
//                         // dp("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
//                             // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
//                         // dp("V1\n");
//                         if (!main_first) main_first = true;
//                         usleep(1*1000);
//                     }
//                 }
//             }
//             VM_Frame_Buff.Rindex = (VM_Frame_Buff.Rindex+1)%10;
//             VM_Frame_Buff.cnt--;
//         }
//         //////////////////////////////////////////////////////////////////////////////
//         Recv_Spi_Packet_live(rx_buff); 

// 		/////////// Vedio Box IN -> UDP Out //////////////////////////////////////////
// 		if (VB_Frame_Buff.cnt > 0 && main_first) {
// 			framesize = VB_Frame_Buff.len[VB_Frame_Buff.Rindex];
// 			for(int i=0; framesize > 0; i++){
// 				pthread_mutex_lock(&buffMutex_vb);
// 				datasize = (framesize > V_SEND_SIZE) ? V_SEND_SIZE : framesize;
// 				// udp_vm_send(VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize);
// 				framesize -= datasize;
// 				// dp("cnt:%d, total:%d, dsize:%d\n", i, framesize, datasize);
// 				pthread_mutex_unlock(&buffMutex_vb);
//                 // dp("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
//             #ifdef __H265__
//                 Make_Spi_Packet_live(tx_buff, VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize, STREAMING, STREAM_VEDIO_B);
//             #else
//                 if (framesize < V_SEND_SIZE)    frame_end = true;
//                 else                            frame_end = false;
//                 Make_Spi_Packet_live_rtp(tx_buff, VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), 
//                                             datasize, STREAMING, STREAM_VEDIO_B, VB_Frame_Buff.ftime[VB_Frame_Buff.Rindex], frame_end);
//             #endif
				
//     	        // memset(tx_buff, 0, 1024);
//         	    // memcpy(&tx_buff[6], read_buff, ret);
//             	///////////////// SPI Send //////////////////////////
// 	            if (data_sel == 2 || data_sel == 4) {
//     	        	// ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
//                     ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
//                     // ret = 0;
//         	    	if (ret != 0) {
//             			dp("Fail Send SPI Data!\n");
//             		}
// 	            	else {
//    	         		// dp("cnt:%d total:%d dsize%d\n", i, framesize, datasize);
//                         // dp("V2\n");
//     	        		usleep(1*1000);
//         	    	}
//             	}
// 			}
// 			VB_Frame_Buff.Rindex = (VB_Frame_Buff.Rindex+1)%10;
// 			VB_Frame_Buff.cnt--;
// 		}
// 		//////////////////////////////////////////////////////////////////////////////
//         Recv_Spi_Packet_live(rx_buff);
       
// 		/////////// Audio IN -> UDP Out //////////////////////////////////////////////
// 		if (AI_Cir_Buff.RIndex != AI_Cir_Buff.WIndex) {
// 			pthread_mutex_lock(&buffMutex_ai);
// 			datasize = (AI_Cir_Buff.WIndex - AI_Cir_Buff.RIndex + A_BUFF_SIZE) % (500*1024);
// 			datasize = (datasize > A_SEND_SIZE) ? A_SEND_SIZE : datasize;
// 			for (int i = 0; i < datasize; ++i) {
// 				buf[i] = AI_Cir_Buff.tx[AI_Cir_Buff.RIndex];
// 				AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+1) % (500*1024);
// 				if (AI_Cir_Buff.RIndex == AI_Cir_Buff.GIndex){
// 					// dp("[CIR_BUFF_AI]datasize:%d WIndex:%d RIndex%d GIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex, AI_Cir_Buff.GIndex);
// 					datasize = i+1;
// 					break;
// 				}
// 			}
// 			// dp("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
// 			pthread_mutex_unlock(&buffMutex_ai);
// 			// udp_vm_send(buf, datasize);
// 			Make_Spi_Packet_live(tx_buff, buf, datasize, STREAMING, STREAM_AUDIO_F);
//             // memset(tx_buff, 0, 1024);
//             // memcpy(&tx_buff[6], read_buff, ret);
//             ///////////////// SPI Send //////////////////////////
//             if (data_sel == 3 || data_sel == 4) {
//             	// ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
//                 ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
//                 // ret = 0;
//             	if (ret != 0) {
//             		dp("Fail Send SPI Data!\n");
//             	}
//             	else {
//             		// dp("AUDIO Send Data : 0x%02X%02X\n", tx_buff[3], tx_buff[4]);
//                     // dp("A\n");
// 	            	usleep(1*1000);
//     	        }
// 			}
//             /////////////////////////////////////////////////////
// 		}
// 		//////////////////////////////////////////////////////////////////////////////
//         Recv_Spi_Packet_live(rx_buff);

        

		
//     } while(!bStrem);
    
//     return ((void*)0);
// }

// int v1c = 0, v2c = 0, aoc = 0;


void *spi_send_stream (void *arg)
{
    
    int ret = -1;
    uint8_t *buf;
    int datasize = 0;
    int framesize1 = 0;
    int frame_ptr1 = 0;
    int framesize2 = 0;
    int frame_ptr2 = 0;
    bool frame_end = false;
    int main_first = 0;
    int mv_delay = 2;
    // int old_ck = 0;
    // bool stream_start1 = false;
    int64_t time_check_2 = 0, old_tc2 = 0;

    
    bool str_ex_1 = true, str_ex_2 = true;
    bool continue_flag1 = false, continue_flag2 = false;
    #ifdef __IOT_CORE__
        int ndatasize = 0;
    #endif

    // int64_t interval_v1 = 0, savetime_v1 = sample_gettimeus(), avr_v1 = 0;
    // int cnt_v1 = 0;
    // int64_t interval_v2 = 0, savetime_v2 =  sample_gettimeus(), avr_v2 = 0;
    // int cnt_v2 = 0;
    // int64_t interval_ai = 0, savetime_ai =  sample_gettimeus(), avr_ai = 0;
    // int cnt_ai = 0;

    buf = (uint8_t*)malloc(2000);

    // int save_fd1 = 0;
    // save_fd1 = open("/tmp/mnt/sdcard/main1.h264", O_RDWR | O_CREAT | O_TRUNC, 0777);

    // int save_fd2 = 0;
    // save_fd2 = open("/tmp/mnt/sdcard/main2.h264", O_RDWR | O_CREAT | O_TRUNC, 0777);
  
    do {
        /////////// Vedio Main IN -> UDP Out //////////////////////////////////////////
        // if (!stream_start1) {
            // if (VM_Frame_Buff.cnt > 7) stream_start1 = true;
        // }
        // else if (VM_Frame_Buff.cnt > 0) {
        if (!bLive && stream_state == 1) {
            bLive = true;
        }
        
        if (VM_Frame_Buff.cnt > 0) {
            
            if (VM_Frame_Buff.cnt >= 14) {
                if (mv_delay != 1) {
                    dp("Delay State 1!\n");
                    mv_delay = 1;
                }
            }
            
            else if (VM_Frame_Buff.cnt <= 4){
                if (mv_delay != 2) {
                    dp("Delay State 2!\n");
                    mv_delay = 2;
                }
            }

            // if (bitrate_change != 500 && (sample_gettimeus()-bitrate_cnt)>3000000) {
            //     bitrate_change = 500;
            //     Set_Target_Bit2(bitrate_change);
            // }

            if (frame_ptr1 == 0){
                framesize1 = VM_Frame_Buff.len[VM_Frame_Buff.Rindex]-(V_SEND_SIZE*frame_ptr1);
                if (framesize1 > V_SEND_SIZE)       continue_flag1 = true;
                else                                continue_flag1 = false;
            }
            pthread_mutex_lock(&buffMutex_vm);
            datasize = (framesize1 > V_SEND_SIZE) ? V_SEND_SIZE : framesize1;
            // udp_vm_send(VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr1), datasize);
            framesize1 -= datasize;

            #ifdef __IOT_CORE__
                ndatasize = (framesize1 > V_SEND_SIZE) ? V_SEND_SIZE : framesize1;
            #endif
            // dp("cnt:%d, total:%d, dsize:%d\n", frame_ptr1, framesize1, datasize);
            
            // dp("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
            // ret = write(save_fd1, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr1), datasize);
        #ifdef __H265__
            Make_Spi_Packet_live(tx_buff, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr1), datasize, STREAMING, STREAM_VEDIO_M);
        #else
            #ifdef __IOT_CORE__
                if (datasize < V_SEND_SIZE)         frame_end = true;
                else {
                    if (ndatasize < V_SEND_SIZE)    frame_end = true;
                    else                            frame_end = false;
                }

                if (str_ex_1)                   str_ex_1 = false;
                else                            str_ex_1 = true;
            #else
                if (datasize < V_SEND_SIZE)     frame_end = true;
                else                            frame_end = false;
            #endif

            // if (datasize > 2)
            //     ret = write(save_fd1, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr1), datasize);

            Make_Spi_Packet_live_rtp(tx_buff, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr1), 
                                        datasize, STREAMING, STREAM_VEDIO_M, VM_Frame_Buff.ftime[VM_Frame_Buff.Rindex], frame_end, continue_flag1, str_ex_1);

            pthread_mutex_unlock(&buffMutex_vm);

            #ifdef __IOT_CORE__
                // if (tx_buff[5+9] == 0x80 && (tx_buff[5+9+1] == 0x61 || tx_buff[5+9+1] == 0xe1)) {
                //     dp("C:%d L:%d S:%d %x %x D:%x %x\n", tx_buff[V_SEND_RESERV+7], (tx_buff[V_SEND_RESERV+3]*256+tx_buff[V_SEND_RESERV+4]), 
                //                                         (tx_buff[V_SEND_RESERV+9+2]*256+tx_buff[V_SEND_RESERV+9+3]), tx_buff[V_SEND_RESERV+9+2], tx_buff[V_SEND_RESERV+9+3], 
                //                                         frame_end, str_ex_1);
                //     if (old_ck != (tx_buff[V_SEND_RESERV+9+2]*256+tx_buff[V_SEND_RESERV+9+3]-1)) {
                //         dp("Old Seq : %d / New Seq : %d\n", old_ck, (tx_buff[V_SEND_RESERV+9+2]*256+tx_buff[V_SEND_RESERV+9+3]));
                //     }
                //     old_ck = (tx_buff[V_SEND_RESERV+9+2]*256+tx_buff[V_SEND_RESERV+9+3]);
                // }
                // else {
                //     dp("C:%d L:%d D:%x %x\n", tx_buff[V_SEND_RESERV+7], (tx_buff[V_SEND_RESERV+3]*256+tx_buff[V_SEND_RESERV+4]), frame_end, str_ex_1);
                // }
            #endif
        #endif
            // dp("[%d]cnt:%d total:%d dsize:%d\n", VM_Frame_Buff.Rindex, frame_ptr1, framesize1, datasize);
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            ///////////////// SPI Send //////////////////////////
            // dp("%x %x %x %x %x %x %x %x %x %x %x %x\n", tx_buff[V_SEND_RESERV+10+0], tx_buff[V_SEND_RESERV+10+1], tx_buff[V_SEND_RESERV+10+2], tx_buff[V_SEND_RESERV+10+3], tx_buff[V_SEND_RESERV+10+4], tx_buff[V_SEND_RESERV+10+5]
                                                        // , tx_buff[V_SEND_RESERV+10+6], tx_buff[V_SEND_RESERV+10+7], tx_buff[V_SEND_RESERV+10+8], tx_buff[V_SEND_RESERV+10+9], tx_buff[V_SEND_RESERV+10+10], tx_buff[V_SEND_RESERV+10+11]);
            if (data_sel == 1 || data_sel == 4) {
                // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                if (stream_state == 1)
                    ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                else
                    ret = 0;
                if (ret != 0) {
                    dp("Fail Send SPI Data!\n");
                }
                else {
                    frame_ptr1++;

                    if (tx_buff[5+9] == 0x80 && (tx_buff[5+9+1] == 0x61 || tx_buff[5+9+1] == 0xe1)) 
                    {
                        time_check_2 = 0x00000000;
                        time_check_2 = tx_buff[5+9+4]&0x000000FF;
                        time_check_2 <<= 8;
                        time_check_2 |= tx_buff[5+9+5]&0x000000FF;
                        time_check_2 <<= 8;
                        time_check_2 |= tx_buff[5+9+6]&0x000000FF;
                        time_check_2 <<= 8;
                        time_check_2 |= tx_buff[5+9+7]&0x000000FF;
                        // time_check_2 = (0xFF000000&(tx_buff[5+9+4]<<24)) || (0x00FF0000&(tx_buff[5+9+5]<<16)) || (0x0000FF00&(tx_buff[5+9+6]<<8)) || (0x000000FF&tx_buff[5+9+7]);

                        if (old_tc2 > time_check_2) {
                            dp("RTP_TIME ERROR : %lld -> %lld\n", old_tc2, time_check_2);
                        }
                        else if ((time_check_2-old_tc2)>1000000) {
                            dp("%x %x %x %x %x %x %x %x %x %x %x %x\n",tx_buff[5+9+0],tx_buff[5+9+1],tx_buff[5+9+2],tx_buff[5+9+3]
                                                                        ,tx_buff[5+9+4],tx_buff[5+9+5],tx_buff[5+9+6],tx_buff[5+9+7]
                                                                        ,tx_buff[5+9+8],tx_buff[5+9+9],tx_buff[5+9+10],tx_buff[5+9+11]);
                        }
                        // else {
                            // dp("TIME CK : %lld\n", time_check_2);
                        // }

                        old_tc2 = time_check_2;
                    }

                    // if (cnt_v1 > 0) {
                    //     interval_v1 = (sample_gettimeus() - savetime_v1);
                    //     avr_v1 = (avr_v1 + interval_v1);
                    //     dp("[v1] interval : %lld, avr : %lld\n", interval_v1, (avr_v1/cnt_v1));
                    // }
                    // savetime_v1 = sample_gettimeus();
                    // cnt_v1++;

                    // dp("C:%x D:%x %x %x %x\n",tx_buff[5+7],tx_buff[5+9+0],tx_buff[5+9+1],tx_buff[5+9+2],tx_buff[5+9+3]);

                    // // if (datasize > 2) {
                        // if (tx_buff[5+9] == 0x80 && (tx_buff[5+9+1] == 0x61 || tx_buff[5+9+1] == 0xe1))
                        //     ret = write(save_fd2, &tx_buff[5+9+0], datasize+12);
                        // else
                        //     ret = write(save_fd2, &tx_buff[5+9+0], datasize);
                    // // }


                    if (main_first < 10) main_first++;
                    usleep(mv_delay*1000);
                }
            }
            if (framesize1 <= 0) {
                VM_Frame_Buff.Rindex = (VM_Frame_Buff.Rindex+1)%20;
                VM_Frame_Buff.cnt--;
                frame_ptr1 = 0;
                #ifdef __IOT_CORE__
                    str_ex_1 = true;
                    // dp("1C\n");
                #endif
            }
        }
        //////////////////////////////////////////////////////////////////////////////
        Recv_Spi_Packet_live(rx_buff); 

        /////////// Vedio Box IN -> UDP Out //////////////////////////////////////////
        if (VB_Frame_Buff.cnt > 0 && (main_first >= 10)) {
            if (frame_ptr2 == 0){
                framesize2 = VB_Frame_Buff.len[VB_Frame_Buff.Rindex]-(V_SEND_SIZE*frame_ptr2);
                if (framesize2 > V_SEND_SIZE)       continue_flag2 = true;
                else                                continue_flag2 = false;
            }
            pthread_mutex_lock(&buffMutex_vb);
            datasize = (framesize2 > V_SEND_SIZE) ? V_SEND_SIZE : framesize2;
            // udp_vm_send(VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr2), datasize);
            framesize2 -= datasize;
            // dp("cnt:%d, total:%d, dsize:%d\n", frame_ptr2, framesize2, datasize);
            #ifdef __IOT_CORE__
                ndatasize = (framesize2 > V_SEND_SIZE) ? V_SEND_SIZE : framesize2;
            #endif
            
            // dp("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
            // ret = write(save_fd2, VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr2), datasize);
        #ifdef __H265__
            Make_Spi_Packet_live(tx_buff, VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr2), datasize, STREAMING, STREAM_VEDIO_B);
        #else
            // if (datasize < V_SEND_SIZE)     frame_end = true;
            // else                            frame_end = false;
            #ifdef __IOT_CORE__
                if (datasize < V_SEND_SIZE)         frame_end = true;
                else {
                    if (ndatasize < V_SEND_SIZE)    frame_end = true;
                    else                            frame_end = false;
                }

                if (str_ex_2)                   str_ex_2 = false;
                else                            str_ex_2 = true;
            #else
                if (datasize < V_SEND_SIZE)     frame_end = true;
                else                            frame_end = false;
            #endif

            Make_Spi_Packet_live_rtp_b(tx_buff, VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr2), 
                                        datasize, STREAMING, STREAM_VEDIO_B, VB_Frame_Buff.ftime[VB_Frame_Buff.Rindex], frame_end, continue_flag2, str_ex_2);
            
            pthread_mutex_unlock(&buffMutex_vb);
            #ifdef __IOT_CORE__
                // if (tx_buff[5+9] == 0x80 && (tx_buff[5+9+1] == 0x61 || tx_buff[5+9+1] == 0xe1)) {
                //     dp("C:%d L:%d S:%d %x %x %x D:%x %x\n", tx_buff[V_SEND_RESERV+7], (tx_buff[V_SEND_RESERV+3]*256+tx_buff[V_SEND_RESERV+4]), 
                //                                         (tx_buff[V_SEND_RESERV+9+2]*256+tx_buff[V_SEND_RESERV+9+3]), tx_buff[V_SEND_RESERV+9+1], tx_buff[V_SEND_RESERV+9+2], tx_buff[V_SEND_RESERV+9+3], 
                //                                         frame_end, str_ex_2);
                //     if (old_ck != (tx_buff[V_SEND_RESERV+9+2]*256+tx_buff[V_SEND_RESERV+9+3]-1)) {
                //         dp("Old Seq : %d / New Seq : %d\n", old_ck, (tx_buff[V_SEND_RESERV+9+2]*256+tx_buff[V_SEND_RESERV+9+3]));
                //     }
                //     old_ck = (tx_buff[V_SEND_RESERV+9+2]*256+tx_buff[V_SEND_RESERV+9+3]);
                // }
                // else {
                //     dp("C:%d L:%d D:%x %x\n", tx_buff[V_SEND_RESERV+7], (tx_buff[V_SEND_RESERV+3]*256+tx_buff[V_SEND_RESERV+4]), frame_end, str_ex_2);
                // }
            #endif
        #endif
            
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            ///////////////// SPI Send //////////////////////////
            if (data_sel == 2 || data_sel == 4) {
                // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                if (stream_state == 1)
                    ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                else
                    ret = 0;
                // ret = 0;
                if (ret != 0) {
                    dp("Fail Send SPI Data!\n");
                }
                else {
                    frame_ptr2++;

                    // if (datasize > 2) {
                        // if (tx_buff[5+9] == 0x80 && (tx_buff[5+9+1] == 0x61 || tx_buff[5+9+1] == 0xe1))
                        //     ret = write(save_fd2, &tx_buff[5+9+0], datasize+12);
                        // else
                        //     ret = write(save_fd2, &tx_buff[5+9+0], datasize);
                    // }

                    usleep(mv_delay*1000);
                }
            }
            if (framesize2 <= 0) {
                VB_Frame_Buff.Rindex = (VB_Frame_Buff.Rindex+1)%20;
                VB_Frame_Buff.cnt--;
                frame_ptr2 = 0;
                #ifdef __IOT_CORE__
                    str_ex_2 = true;
                    // dp("2C\n");
                #endif
            }
        }
        //////////////////////////////////////////////////////////////////////////////
        Recv_Spi_Packet_live(rx_buff);
       
        /////////// Audio IN -> UDP Out //////////////////////////////////////////////
        if (AI_Cir_Buff.RIndex != AI_Cir_Buff.WIndex) {
            
            if (AI_Cir_Buff.WIndex >= AI_Cir_Buff.RIndex)
                datasize = (AI_Cir_Buff.WIndex - AI_Cir_Buff.RIndex) % (500*1024);
            else
                // datasize = (AI_Cir_Buff.WIndex - AI_Cir_Buff.RIndex + A_BUFF_SIZE) % (500*1024);
                datasize = 0;

            if (datasize >= A_SEND_SIZE) {
                pthread_mutex_lock(&buffMutex_ai);
                datasize = (datasize > A_SEND_SIZE) ? A_SEND_SIZE : datasize;
                // for (int i = 0; i < datasize; ++i) {
                //     buf[i] = AI_Cir_Buff.tx[AI_Cir_Buff.RIndex];
                //     AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+1) % (500*1024);
                //     if (AI_Cir_Buff.RIndex == AI_Cir_Buff.GIndex){
                //         // dp("[CIR_BUFF_AI]datasize:%d WIndex:%d RIndex%d GIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex, AI_Cir_Buff.GIndex);
                //         datasize = i+1;
                //         break;
                //     }
                // }
                // dp("Audio Rec Len : %d\n", datasize);
                // dp("[CIR_BUFF_AI]datasize:%d WIndex:%d RIndex%d len\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
                memset(buf, 0, datasize);
                memcpy(buf, &AI_Cir_Buff.tx[AI_Cir_Buff.RIndex], datasize);
                AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+datasize) % (500*1024);
                if (AI_Cir_Buff.RIndex == AI_Cir_Buff.WIndex) {
                //     AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+1) % (500*1024);
                    AI_Cir_Buff.RIndex = AI_Cir_Buff.WIndex = 0;
                }
                pthread_mutex_unlock(&buffMutex_ai);


                // dp("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
                
                // udp_vm_send(buf, datasize);
                
                // memset(tx_buff, 0, 1024);
                // memcpy(&tx_buff[6], read_buff, ret);
                ///////////////// SPI Send //////////////////////////
                if (data_sel == 3 || data_sel == 4) {
                    // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                    if (stream_state == 1 && audio_spi_flag){
                        Make_Spi_Packet_live(tx_buff, buf, datasize, STREAMING, STREAM_AUDIO_F);
                        ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                        if (ret != 0) {
                            dp("Fail Send SPI Data!\n");
                        }
                        else {
                            // dp("AUDIO Send Data : 0x%02X%02X\n", tx_buff[3+5], tx_buff[4+5]);
                            // dp("AOUT\n");
                            // ret = write(save_fd, buf, datasize);
                            // aoc++;
                            // if (aoc>10) {
                            //     aoc = 0;
                            //     dp("AO\n");    
                            // }
                            datasize = 0;
                            usleep(mv_delay*1000);
                        }
                    }
                    else
                        ret = 0;
                    // ret = 0;
                    
                }
            }


            /////////////////////////////////////////////////////
        }


        //////////////////////////////////////////////////////////////////////////////
        Recv_Spi_Packet_live(rx_buff);

        

        
    } while(!bStrem);
    
    // close(save_fd);

    return ((void*)0);
}


// void *spi_test_send_stream (void *arg)
// {
    
//     int ret = -1;
//     uint8_t *buf;
//     int datasize = 0;
//     int framesize = 0;

//     buf = (uint8_t*)malloc(2000);
  
//     // dp("set parse is : device %s ,speed %d ,delay %d ,bpw %d\n",device,speed,delay,bits);
//     for(int i=0; i<1024; i++){
//         tx_tbuff[i] = i%256;
//     }

//     do {
//         /////////// Vedio Box IN -> UDP Out //////////////////////////////////////////
//         if (VB_Frame_Buff.cnt > 0) {
//             framesize = VB_Frame_Buff.len[VB_Frame_Buff.Rindex];
//             for(int i=0; framesize > 0; i++){
//                 pthread_mutex_lock(&buffMutex_vb);
//                 datasize = (framesize > V_SEND_SIZE) ? V_SEND_SIZE : framesize;
//                 // udp_vm_send(VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize);
//                 framesize -= datasize;
//                 // dp("cnt:%d, total:%d, dsize:%d\n", i, framesize, datasize);
//                 pthread_mutex_unlock(&buffMutex_vb);
//                 // dp("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
//                 Make_Spi_Packet_live(tx_buff, VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize, STREAMING, STREAM_VEDIO_B);
//                 // memset(tx_buff, 0, 1024);
//                 // memcpy(&tx_buff[6], read_buff, ret);
//                 ///////////////// SPI Send //////////////////////////
//                 if (data_sel == 2 || data_sel == 4) {
//                     // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
//                     // ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
//                     // ret = spi_write_bytes(fd,tx_tbuff, SPI_SEND_LENGTH);
//                     ret = spi_rw_bytes(fd,tx_tbuff,rx_buff,SPI_SEND_LENGTH);
//                     if (ret != 0) {
//                         dp("Fail Send SPI Data!\n");
//                     }
//                     else {
//                     // dp("MAIN Send Data : 0x%02X%02X\n", tx_buff[3], tx_buff[4]);
//                         usleep(1*1000);
//                     }
//                 }
//             }
//             VB_Frame_Buff.Rindex = (VB_Frame_Buff.Rindex+1)%5;
//             VB_Frame_Buff.cnt--;
//         }
//         //////////////////////////////////////////////////////////////////////////////
//         Recv_Spi_Packet_test(rx_buff);

//         if (VM_Frame_Buff.cnt > 0) {
//             framesize = VM_Frame_Buff.len[VM_Frame_Buff.Rindex];
//             for(int i=0; framesize > 0; i++){
//                 pthread_mutex_lock(&buffMutex_vm);
//                 datasize = (framesize > V_SEND_SIZE) ? V_SEND_SIZE : framesize;
//                 // udp_vm_send(VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize);
//                 framesize -= datasize;
//                 // dp("cnt:%d, total:%d, dsize:%d\n", i, framesize, datasize);
//                 pthread_mutex_unlock(&buffMutex_vm);
//                 // dp("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
//                 Make_Spi_Packet_live(tx_buff, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize, STREAMING, STREAM_VEDIO_M);
//                 // memset(tx_buff, 0, 1024);
//                 // memcpy(&tx_buff[6], read_buff, ret);
//                 ///////////////// SPI Send //////////////////////////
//                 if (data_sel == 1 || data_sel == 4) {
//                     // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
//                     // ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
//                     // ret = spi_write_bytes(fd,tx_tbuff, SPI_SEND_LENGTH);
//                     ret = spi_rw_bytes(fd,tx_tbuff,rx_buff,SPI_SEND_LENGTH);
//                     if (ret != 0) {
//                         dp("Fail Send SPI Data!\n");
//                     }
//                     else {
//                         // dp("d:0x%02X%02X\n", tx_buff[3], tx_buff[4]);
//                         usleep(1*1000);
//                     }
//                 }
//             }
//             VM_Frame_Buff.Rindex = (VM_Frame_Buff.Rindex+1)%5;
//             VM_Frame_Buff.cnt--;
//         }
//         //////////////////////////////////////////////////////////////////////////////
//         Recv_Spi_Packet_test(rx_buff);
        
//         /////////// Audio IN -> UDP Out //////////////////////////////////////////////
//         if (AI_Cir_Buff.RIndex != AI_Cir_Buff.WIndex) {
//             pthread_mutex_lock(&buffMutex_ai);
//             datasize = (AI_Cir_Buff.WIndex - AI_Cir_Buff.RIndex + A_BUFF_SIZE) % (500*1024);
//             datasize = (datasize > A_SEND_SIZE) ? A_SEND_SIZE : datasize;
//             for (int i = 0; i < datasize; ++i) {
//                 buf[i] = AI_Cir_Buff.tx[AI_Cir_Buff.RIndex];
//                 AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+1) % (500*1024);
//                 if (AI_Cir_Buff.RIndex == AI_Cir_Buff.GIndex){
//                     // dp("[CIR_BUFF_AI]datasize:%d WIndex:%d RIndex%d GIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex, AI_Cir_Buff.GIndex);
//                     datasize = i+1;
//                     break;
//                 }
//             }
//             // dp("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
//             pthread_mutex_unlock(&buffMutex_ai);
//             // udp_vm_send(buf, datasize);
//             Make_Spi_Packet_live(tx_buff, buf, datasize, STREAMING, STREAM_AUDIO_F);
//             // memset(tx_buff, 0, 1024);
//             // memcpy(&tx_buff[6], read_buff, ret);
//             ///////////////// SPI Send //////////////////////////
//             if (data_sel == 3 || data_sel == 4) {
//                 // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
//                 // ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
//                 // ret = spi_write_bytes(fd,tx_tbuff, SPI_SEND_LENGTH);
//                 ret = spi_rw_bytes(fd,tx_tbuff,rx_buff,SPI_SEND_LENGTH);
//                 if (ret != 0) {
//                     dp("Fail Send SPI Data!\n");
//                 }
//                 else {
//                     // dp("AUDIO Send Data : 0x%02X%02X\n", tx_buff[3], tx_buff[4]);
//                     usleep(1*1000);
//                 }
//             }
//             /////////////////////////////////////////////////////
//         }
//         //////////////////////////////////////////////////////////////////////////////
//         Recv_Spi_Packet_test(rx_buff);
//     } while(!bStrem);
    
//     return ((void*)0);
// }



void test_spi_rw(void) {

    memset(rx_buff, 0x00, 1024);
    for(int i=0; i<1024; i++) {
        tx_buff[i] = i%255;
    }
    
    spi_rw_bytes(fd, tx_buff, rx_buff, 1024);

    dp("SPI RX:\n");
    for(int j=0; j<1024; j++) {
        dp("0x%02x ", rx_buff[j]);
        if (j%16 == 15)
            dp("\n");
    }

    // usleep(1*1000*1000);
}

int ota_fd = 0;
uint16_t ota_seq = 0;
char md5_hash_code[33] = {0};
char md5_madk_hash[33] = {0};

int OTA_Recv_Packet(uint8_t *rbuff) {
    int len, ret = -1, res = 0;
    uint8_t major, minor;
    uint16_t seq_buf;
    int recv_type = 0;
    // static uint8_t data[10]= {0};
    int bad_cnt = 0;
    // uint8_t *ota_buf;
    static uint16_t seqcnt = 0xFFFF;
    bool hash_ck = true;

#if 0
    static int filefd = 0;
    if (filefd == 0) {
        filefd = open("/dev/shm/test.pcm", O_RDWR | O_CREAT | O_TRUNC, 0777);
    }

    major = rbuff[1];
    minor = rbuff[2];
    if (rbuff[0] != 0x02) {
        // dp("S\n");
        // return -1;
        bad_cnt++;
    }
    
    if (major != 0x82) {
        bad_cnt++;
    } 

    if (minor != 0x07) {
        bad_cnt++;
    }

    if (bad_cnt > 1) {
        return -1;
    }

    len = rbuff[3]*256 + rbuff[4];

    // dp("spi cnt:%d %d\n", rbuff[8], len);

    dp("M : 0x%02x m : 0x%02x len : %d seq : %d\n", major, minor, len, rbuff[8]);

    dp("rbuff:0x%02x 0x%02x 0x%02x 0x%02x\n", rbuff[1009], rbuff[1010], rbuff[1023], rbuff[1024]);

    // for (int i=0; i<1000; i++) {
    //     if (i%10 == 0) dp("\n");
    //     dp("0x%02x ", rbuff[9+i]);
    // }
    // dp("\n");
    
#else
    major = rbuff[1];
    minor = rbuff[2];

    if (rbuff[0] != 0x02) {
        bad_cnt++;
    }
    
    if (major != 0x83) {
        bad_cnt++;
    } 

    if (minor != SET_FW_START && minor != SET_FW_DATA && minor != SET_FW_END) {
        bad_cnt++;
    }

    if (bad_cnt > 1) {
        return -1;
    }

    len = rbuff[3]*256 + rbuff[4];

    // dp("spi cnt:%d %d %d\n", spicnt, rbuff[8], len);

    if (rbuff[1023] != 0x03){
        dp("[OTA_Recv] Length Worng!\n");
        return -2;
    }

    

    if(minor == SET_FW_DATA) {
        seq_buf = rbuff[5]*256 + rbuff[6];
        if (seqcnt == seq_buf) {
            dp("[OTA_Recv] Seq Worng1! %d %d\n", seqcnt, seq_buf);
            return -3;
        }
        else {
            if (seqcnt == 0xFFFF) seqcnt = 1;
            else seqcnt++;
            if (seqcnt == seq_buf) {
                seqcnt = seq_buf;
                ota_seq = seqcnt;
            }
            else {
                dp("[OTA_Recv] Seq Worng2! %d %d\n", seqcnt, seq_buf);
                return -3;
            }
            
        }
    }
    
    switch(major) {
    case DTEST_BACK:
    case REC_BACK:
    case STREAMING_BACK:
    break;
    case SETTING_BACK:
        switch(minor) {
            case SET_FW_START:
                dp("OTA Start\n");
                recv_type = rbuff[9];
                memset(md5_hash_code, 0, 32);
                memcpy(md5_hash_code, &rbuff[10], 32);
                // char print_buf[35] = {0};
                // memcpy(print_buf, md5_hash_code, 32);
                // print_buf[32] = '\n';
                // dp("hash:%s\n", md5_hash_code);
                // for (int i=0; i<32; i++) {
                //     dp ("0x%02x ",rbuff[10+i]);
                // }
                dp("\n");
                if (recv_type == 0) 
                    ota_fd = open("/dev/shm/isc.zip", O_RDWR | O_CREAT | O_TRUNC, 0777);
                else if (recv_type == 1) 
                    ota_fd = open("/dev/shm/tag.bin", O_RDWR | O_CREAT | O_TRUNC, 0777);
                else if (recv_type == 2) 
                    ota_fd = open("/dev/shm/uImage.lzo", O_RDWR | O_CREAT | O_TRUNC, 0777);
                else {
                    dp("OTA Start Error! Type!\n");
                    return -4;
                }
                if (ota_fd < 0) {
                    dp("File Open Fail!\n");
                    return -4;
                }
                res = 1;
            break;
            case SET_FW_DATA:
                // ota_buf = malloc(1024);
                dp("OTA Data Seq : %d len : %d\n", ota_seq, len);
                // memset(ota_buf, 0, 1024);
                // memcpy(ota_buf, &rbuff[9], len);
                ret = write(ota_fd, &rbuff[9], len);
                if (ret < 0) {
                    dp("Write Fail\n");
                    return -4;
                }
                // free(ota_buf);
                res = 2;
            break;
            case SET_FW_END:
                dp("OTA End\n");
                ret = md5_get("/dev/shm/isc.zip", md5_madk_hash);
                if (ret == 0) {
                    dp("Get Hash : %s\n", md5_hash_code);
                    for (int i=0; i<32; i++) {
                        if (md5_madk_hash[i] != md5_hash_code[i]) {
                            hash_ck = false;
                        }
                    }
                }
                else {
                    hash_ck = false;
                }
                recv_type = rbuff[9];
                if (!hash_ck) {
                    dp("Hash Check Fail!\n");
                    // system("rm /tmp/mnt/sdcard/isc.zip");
                    // system("cp /dev/shm/isc.zip /tmp/mnt/sdcard/isc.zip");
                    res = -5;
                }
                else {
                    if (recv_type == 0) {
                        int filed = 0;

                        system("unzip /dev/shm/isc.zip -d /dev/shm");

                        filed = open("/dev/shm/setup.sh", O_RDONLY);
                        if (filed == -1) {
                            dp("setup.sh not checked!\n");
                            system("rm /tmp/mnt/sdcard/isc_bak");
                            system("mv /tmp/mnt/sdcard/isc /tmp/mnt/sdcard/isc_bak");
                            system("cp /dev/shm/isc /tmp/mnt/sdcard/isc");
                        }
                        else {
                            dp("setup.sh check!!\b");
                            close(filed);
                            system("chmod 777 /dev/shm/setup.sh");
                            system("/dev/shm/./setup.sh");
                        }
                        // system("rm /tmp/mnt/sdcard/isc.zip");
                        // system("rm /tmp/mnt/sdcard/isc_bak");
                        // system("mv /tmp/mnt/sdcard/isc /tmp/mnt/sdcard/isc_bak");
                        // system("cp /dev/shm/isc.zip /tmp/mnt/sdcard/isc.zip");
                        // system("unzip -o /tmp/mnt/sdcard/isc.zip -d /tmp/mnt/sdcard");
                        
                        res = 3;
                    }
                }
                // else if (recv_type == 1) {

                // } 
                // else if (recv_type == 2) {

                // }
                // else {
                //     dp("OTA End Error! Type!\n");
                //     return -4;
                // }
                system("sync");
                if (ota_fd > 0) {
                    close(ota_fd);
                }
                
            break;
            default:
            break;
        }
    break;
    default:
        return -1;
    break;        
    }
#endif

    return res;
}



int Make_OTA_Dummy_Packet(uint8_t *tbuff, uint8_t type, uint8_t state, uint8_t endstate) {
    uint16_t len;

    memset (tbuff, 0, 1024);
    
    tbuff[0] = 0x02;
    tbuff[1] = SETTING;
    tbuff[5] = 0;
    tbuff[6] = 0;
    tbuff[7] = 0;
    tbuff[8] = 0;
    

    if (state == OTA_STATE_START) {
        len = 1;
        tbuff[2] = SET_START_DUMMY;
        tbuff[3] = (len&0xFF00) >> 8;
        tbuff[4] = len&0xFF;
        tbuff[9] = type;
    }
    else if (state == OTA_STATE_DATA) {
        len = 1;
        tbuff[2] = SET_DATA_DUMMY;
        tbuff[3] = (len&0xFF00) >> 8;
        tbuff[4] = len&0xFF;
        tbuff[5] = (ota_seq&0xFF00) >> 8;
        tbuff[6] = ota_seq&0xFF;
        tbuff[9] = type;

    }
    else if (state == OTA_STATE_END) {
        len = 2;
        tbuff[2] = SET_END_DUMMY;
        tbuff[3] = (len&0xFF00) >> 8;
        tbuff[4] = len&0xFF;
        tbuff[9] = type;
        tbuff[10] = endstate;
    }
    else {
        dp("State Error! : %d\n", state);
        return -1;
    }

    tbuff[1023] = 0x03;
   
    return 0;
}

void *OTA_Thread(void * argc) {
    int ota_state = OTA_STATE_READY;
    // int64_t timeout_t = sample_gettimeus();
    int ret = -1;
    uint8_t err_state = OTA_END_SUCCESS;
    // static int end_pkt_cnt = 0;

    ret = spi_init();
    if(ret < 0) {
        dp("[OTA]spi init error\n");
        return NULL;
    }

    // tx_buff
    // rx_buff

    do {
        if (ota_state == OTA_STATE_READY){
            dp("OTA Ready!\n");
            ota_state = OTA_STATE_START;
        }
        else if (ota_state == OTA_STATE_START) {
            dp("OTA Start!\n");
            // ret = Make_OTA_Dummy_Packet(tx_buff, ota_type_u, ota_state, 0);
            ret = Make_OTA_Dummy_Packet(tx_buff, 0, ota_state, 0);
            if (ret < 0) {
                // ota_state = OTA_STATE_END;
                // err_state = OTA_END_DISCON;
            }
            ret = spi_rw_bytes(fd, tx_buff, rx_buff, SPI_SEND_LENGTH);
            if (ret != 0) {
                dp("Fail Send SPI Start!\n");
            }
            // else {
            //     for (int i=0; i<20; i++) {
            //         dp("%02X ", tx_buff[i]);
            //     }
            //     dp("\n");
            // }
            ret = OTA_Recv_Packet(rx_buff);
            if (ret == 1) {
                ota_state = OTA_STATE_DATA;
                dp("OTA Data!\n");
            }
            else {
                // ota_state = OTA_STATE_END;
                // err_state = OTA_END_DISCON;
            }
        }
        else if (ota_state == OTA_STATE_DATA) {
            // dp("OTA Data!\n");
            ret = Make_OTA_Dummy_Packet(tx_buff, ota_type_u, ota_state, 0);
            if (ret < 0) {
                // err_state = OTA_END_DISCON;
            }
            ret = spi_rw_bytes(fd, tx_buff, rx_buff, SPI_SEND_LENGTH);
            if (ret != 0) {
                dp("Fail Send SPI Data!\n");
            }
            ret = OTA_Recv_Packet(rx_buff);
            if (ret == 2) {
            }
            else if (ret == 3) {
            // if (ret == 3) {
                ota_state = OTA_STATE_END;
            }
            else if (ret == -5){
                ota_state = OTA_STATE_END;
                err_state = OTA_END_DISCON;
            }
        }
        else if (ota_state == OTA_STATE_END) {
            ret = Make_OTA_Dummy_Packet(tx_buff, ota_type_u, ota_state, err_state);
            if (ret < 0) {
                // err_state = OTA_END_DISCON;
            }
            ret = spi_rw_bytes(fd, tx_buff, rx_buff, SPI_SEND_LENGTH);
            if (ret != 0) {
                dp("Fail Send SPI End!\n");
            }

            // end_pkt_cnt++;
            // if (end_pkt_cnt > 20)
                ota_state = OTA_STATE_SHUTDN;
        }
        else {
            dp("OTA Finish!\n");
            cmd_end_flag = true;
            return NULL;
        }
        usleep(3*1000);
    } while (!bOTA);
    return NULL;
}