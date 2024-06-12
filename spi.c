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
        printf("can't open /dev/spidev1.0 ");
    }

    /*
     * spi mode 设置SPI 工作模式
     */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1){
        printf("can't set spi mode");
        return -1;
    }

    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1){
        printf("can't get spi mode");
        return -1;
    }

    /*
     * bits per word  设置一个字节的位数
     */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1){
        printf("can't set bits per word");
        return -1;
    }

    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1){
        printf("can't get bits per word");
        return -1;
    }

    /*
     * max speed hz  设置SPI 最高工作频率
     */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1){
        printf("can't set max speed hz");
        return -1;
    }

    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1){
        printf("can't get max speed hz");
        return -1;
    }

    // printf("spi mode: 0x%x\n", mode);
    // printf("bits per word: %d\n", bits);
    // printf("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);
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
        printf("fd:%d can't send spi message\n",fd);
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
        printf("fd:%d can't send spi message\n",fd);
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

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    if (ret < 1)
    {
        printf("fd:%d can't send spi message\n",fd);
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
        printf("fd:%d can't send spi message\n",fd);
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
        printf("File Length Over!! %d>1014\n", len);
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
                    printf("Protocol Make Fail!!\n");
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
                    printf("Protocol Make Fail!!\n");
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
        printf("File Length Over!! %d>1014\n", len);
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

int Make_Spi_Packet_live_rtp(uint8_t *tbuff, uint8_t *data, uint16_t len, uint8_t major, uint8_t minor, int64_t time, bool fm_end)
{
    int reserv_cnt = V_SEND_RESERV;
    // uint8_t endchar[4] = {0};

    if (len > FILE_READ_LENGTH_LIVE) {
        printf("File Length Over!! %d>1014\n", len);
        return -1;
    }
    memset(tbuff, 0xFF, SPI_SEND_LENGTH);

    RTPHeader header;
    static uint16_t seq_num0 = 0;
    static uint16_t seq_num3 = 0;

    static int64_t test_time = 0;
    int64_t cal_time = 0;

    static int64_t real_time_gap = 0;
    int64_t gap_time = 0;

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

    if (minor == STREAM_VEDIO_M) {
        gap_time = sample_gettimeus()-real_time_gap;
        cal_time = (time - test_time);
        if ((cal_time > 80000) && fm_end){
            printf("Real Main RTP GAP:%lld cal_time:%lld\n", gap_time, cal_time);
            if ((bitrate_change != 300) && fm_end) {
                bitrate_change = 300;
                Set_Target_Bit2(bitrate_change);
                bitrate_cnt = sample_gettimeus();
            }
        }
        real_time_gap = sample_gettimeus();
        test_time = time;
    }

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
                    memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                    // printf("tbuf: %d\n    0x%02x 0x%02x 0x%02x%02x \n    0x%02x%02x%02x%02x \n    0x%02x%02x%02x%02x \n", len,
                    //          tbuff[0+reserv_cnt+9], tbuff[1+reserv_cnt+9], tbuff[2+reserv_cnt+9],tbuff[3+reserv_cnt+9],
                    //          tbuff[4+reserv_cnt+9], tbuff[5+reserv_cnt+9], tbuff[6+reserv_cnt+9],tbuff[7+reserv_cnt+9],
                    //          tbuff[8+reserv_cnt+9], tbuff[9+reserv_cnt+9], tbuff[10+reserv_cnt+9],tbuff[11+reserv_cnt+9]);
                    
                    reserv_cnt += sizeof(RTPHeader);
                    memcpy(&tbuff[9+reserv_cnt], data, len);
                    // endchar[0] = 0x45;  //   'E'
                    // endchar[1] = 0x4E;  //   'N'
                    // endchar[2] = 0x44;  //   'D'
                    // endchar[3] = 0x03;  //   0x03
                    // memcpy(&tbuff[9+reserv_cnt+len], endchar, 4);
                    // printf("tbuf len:%d\n", len);
                    break;
                case STREAM_VEDIO_B:
                    memcpy(&tbuff[9+reserv_cnt], (uint8_t*)&header, sizeof(RTPHeader));
                    reserv_cnt += sizeof(RTPHeader);

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

extern pthread_mutex_t buffMutex_ao;

static int Recv_Spi_Packet_test(uint8_t *rbuff) {
    int i;

    for(i=0;i<1024;i++){
        if (rbuff[i] != i%256) {
            printf("X %d %d %d\n", i, rbuff[i], i%256);
            break;
        }
    }
    if (i == 1024)
        printf("O\n");

    return 0;
}


static int Recv_Spi_Packet_live(uint8_t *rbuff) {
    int index, len;
    uint8_t major, minor;
    int buff_space = 0;
    // static uint8_t data[10]= {0};
    int bad_cnt = 0;
    static int spicnt = 0;
    

    

#if 0
    static int filefd = 0;
    if (filefd == 0) {
        filefd = open("/dev/shm/test.pcm", O_RDWR | O_CREAT | O_TRUNC, 0777);
    }

    index = 0;
    major = rbuff[index+1];
    minor = rbuff[index+2];
    if (rbuff[index] != 0x02) {
        // printf("S\n");
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

    // printf("spi cnt:%d %d\n", rbuff[index+8], len);

    printf("M : 0x%02x m : 0x%02x len : %d seq : %d\n", major, minor, len, rbuff[index+8]);

    printf("rbuff:0x%02x 0x%02x 0x%02x 0x%02x\n", rbuff[1009], rbuff[1010], rbuff[1023], rbuff[1024]);

    // for (int i=0; i<1000; i++) {
    //     if (i%10 == 0) printf("\n");
    //     printf("0x%02x ", rbuff[index+9+i]);
    // }
    // printf("\n");
    
#else
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
        // printf("S\n");
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

    // printf("spi cnt:%d %d %d\n", spicnt, rbuff[index+8], len);

    if (len < 640){
        return -1;
    }

    
    if (spicnt == rbuff[index+8]) {
        // printf("C\n");
        return -1;
    }
    else {
        // if (spicnt != rbuff[index+8]-1) {

            // printf("spi cnt:%d %d\n", spicnt, rbuff[index+8]);
            // for (i=0; i<10; i++) {
                // printf("0x%02x ", rbuff[i]);
            // }
            // printf("\n");
        // }
        
        spicnt = rbuff[index+8];
        a_pkt_cnt = spicnt;
        
    }
    
    // for(i=0; i<10; i++) {
    //     if (data[i] != rbuff[index+9+i]) {
    //         break;
    //     }
    //     else if(i == 9) {
    //         printf("Same Data!!\n");
    //         return -1;
    //     }
    // }
    // memcpy(data, &rbuff[index+9], 10);

    switch(major) {
    case DTEST_BACK:

    case REC_BACK:
    case STREAMING_BACK:
        switch(minor) {
        case STREAM_AUDIO_B:
            // printf("audio dn len:%d\n", len);
            // len = 882;
            if(len > 0){
                pthread_mutex_lock(&buffMutex_ao);
                if (AO_Cir_Buff.RIndex != AO_Cir_Buff.WIndex) {
                    buff_space = (AO_Cir_Buff.RIndex - AO_Cir_Buff.WIndex - 1 + A_BUFF_SIZE) % (500*1024);
                }
                else buff_space = A_BUFF_SIZE;
                if (buff_space >= len) {
                    // for(int j = 0; j < len; ++j) {
                    //     AO_Cir_Buff.tx[AO_Cir_Buff.WIndex] = rbuff[index+9+j];
                    //     AO_Cir_Buff.WIndex = (AO_Cir_Buff.WIndex+1) % (500*1024);
                    //     if (AO_Cir_Buff.WIndex == AO_Cir_Buff.RIndex) {
                    //         AO_Cir_Buff.RIndex = (AO_Cir_Buff.RIndex+1) % (500*1024);
                    //     }
                    // }
                    memset(&AO_Cir_Buff.tx[AO_Cir_Buff.WIndex], 0x00, len);
                    memcpy(&AO_Cir_Buff.tx[AO_Cir_Buff.WIndex], &rbuff[index+9], len);
                    AO_Cir_Buff.WIndex = (AO_Cir_Buff.WIndex+len) % (500*1024);
                    // printf("[CIR_BUFF Audio Out]buff_space:%d WIndex:%d RIndex%d\n", buff_space, AO_Cir_Buff.WIndex, AO_Cir_Buff.RIndex);
                    // printf("M : 0x%02x m : 0x%02x len : %d seq : %d\n", major, minor, len, rbuff[index+8]);
                    // printf("Rindex : %d Windex : %d space : %d\n", AO_Cir_Buff.RIndex, AO_Cir_Buff.WIndex, buff_space);
                }
                else {
                    printf("AO Cir Buff Overflow!1\n");
                }
                pthread_mutex_unlock(&buffMutex_ao);
            }
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
//             printf("File Num set : %s\n", file2);
//         }
//         else if (num == 3) {
//             filed = open(file3, O_RDONLY);
//             printf("File Num set : %s\n", file3);
//         }
//         else if (num == 1) {
//             filed = open(file1, O_RDONLY);
//             printf("File Num set : %s\n", file1);
//         }
//         else {
//             printf("Error: -n option file number 1~3 value.\n");
//             return -1;
//         }
//     }
//     printf("set parse is : device %s ,speed %d ,delay %d ,bpw %d, mode %d\n",device,speed,delay,bits,mode);
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
//             // printf("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
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
        if (ret == 1) {
            // printf("Ready Busy Check!!\n");
            return ret;
        }
        else {
            if (i == (READY_BUSY_TIME*2000)-1) {
                printf("RB NACk %d\n", READY_BUSY_TIME);
                return ret;
            }
        }
    }
    return -1;
}

int spi_send_file(uint8_t minor, char *file)
{
    int filed = 0, ret = -1;
    // int dly = 3;
    struct stat file_info;
    int sz_file;
    int len = 0;
    int wcnt = 0;
    
    if ( 0 > stat(file, &file_info)) {
    	printf("File Size Not Check!!\n");
    	return -1;
    }

    filed = open(file, O_RDONLY);
    if (filed == -1) {
    	printf("File %s Open Fail!\n", file);
    	return -1;
    }
    sz_file = file_info.st_size;
    if (sz_file < 10*1024) {
        printf("File Size Low!:%d\n", sz_file);
        return -1;
    }
    printf("**********FILE SEND START CMD************\n");
    // printf("d %s,s %d,d %d,b %d,m %d,f %s, size:%d\n",device,speed,delay,bits,mode,file,sz_file);

    read_buff[0] = minor;
    read_buff[1] = (sz_file>>24)&0xFF;
    read_buff[2] = (sz_file>>16)&0xFF;
    read_buff[3] = (sz_file>>8)&0xFF;
    read_buff[4] = sz_file&0xFF;
    if (minor == REC_CLIP_F || minor == REC_CLIP_B) {
        len = 7;
        read_buff[5] = clip_cause_t.Major;
        read_buff[6] = clip_cause_t.Minor;
    }
    else if (minor == REC_BOX_ALM) {
        len = 7;
        read_buff[5] = clip_cause_t.Major;
        read_buff[6] = clip_cause_t.Minor;
    }
    else if (minor == REC_STREAMING_M ||minor == REC_STREAMING_B) {
        len = 7;
        read_buff[5] = CLIP_CAUSE_STREM;
        read_buff[6] = CLIP_STREAM_REC;
    }
    else if (minor == REC_SNAPSHOT) {
        len = 10;
        sprintf((char*)&read_buff[5], "%s.%s.%s", MAJOR_VER, MINOR_VER, CAHR_VER);
    
    }
    else {
        len = 5;
    }
    printf("Type1 : 0x%02x Type2 : 0x%02x\n", read_buff[5], read_buff[6]);
    Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_STR);
    // memset(tx_buff, 0, 1033);
    // memcpy(&tx_buff[6], read_buff,1);

    // if (!first_send) {
    //     spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    //     usleep(dly*1000);
    //     first_send = true;
    // }
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

    
    if (Ready_Busy_Check())
        printf("File Send Start!\n");
    else{
        printf("Fail to Start CMD\n");
        return -1;
    }

    do {
        ret = read(filed, read_buff, FILE_READ_LENGTH);
        if(ret != 0) {
            if (Ready_Busy_Check()){
                // printf("RB Checked!\n");
            }
            else{
                printf("F:%d\n", wcnt);
                return -1;
            }

            // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
            Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
            // printf("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                        // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
            // for (int i=0; i<1024; i++) {
                // printf(" 0x%02x", tx_buff[i]);
                // if (i%16 == 0) printf("\n");
            // }
        }
        wcnt++;
        // usleep(dly*1000);
    } while(ret != 0);

    if (Ready_Busy_Check()){
        // printf("RB Checked!\n");
    }
    else{
        printf("RB Check Fail!\n");
        return -1;
    }
    read_buff[0] = minor;
    Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_STREAM_END);
    // memset(tx_buff, 0, 1033);
    // memcpy(&tx_buff[6], read_buff,1);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    usleep(100*1000);
    printf("**********FILE SEND END CMD************\n");
    return ret;
}

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

    for (i=0; i<fcnt; i++) {

        sprintf(file, "/dev/shm/face_crop%d.jpg", i);
    
        if ( 0 > stat(file, &file_info)) {
            printf("File Size Not Check!!\n");
            return -1;
        }

        filed = open(file, O_RDONLY);
        if (filed == -1) {
            printf("File %s Open Fail!\n", file);
            return -1;
        }
        else {
            close(filed);
        }
        sz_file += file_info.st_size;
    }
    printf("**********FACE SEND START %d ************\n", sz_file);
    // printf("d %s,s %d,d %d,b %d,m %d,f %s, size:%d\n",device,speed,delay,bits,mode,file,sz_file);
    memset(read_buff, 0, 7);

    read_buff[0] = minor;
    read_buff[1] = (sz_file>>24)&0xFF;
    read_buff[2] = (sz_file>>16)&0xFF;
    read_buff[3] = (sz_file>>8)&0xFF;
    read_buff[4] = sz_file&0xFF;
    read_buff[5] = fcnt;
    read_buff[6] = 0;
    len = 7;

    if (Ready_Busy_Check()){
        // printf("File Send Start!\n");
    }
    else{
        printf("[RB ERR]Face Start CMD\n");
        return -1;
    }
    
    printf("Face File Count : %d\n", read_buff[5]);
    Make_Spi_Packet(tx_buff, read_buff, len, REC, REC_STREAM_STR);

    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    
    for (i=0; i<fcnt; i++) {
        memset(file, 0, 64);
        sprintf(file, "/dev/shm/face_crop%d.jpg", i);
        filed = open(file, O_RDONLY);
        if (filed == -1) {
            printf("File %s Open Fail!\n", file);
            return -1;
        }
        do {
            ret = read(filed, read_buff, FILE_READ_LENGTH);
            if(ret != 0) {
                if (Ready_Busy_Check()){
                    // printf("RB Checked!\n");
                }
                else{
                    printf("F[%d]:%d\n", i, wcnt);
                    return -1;
                }

                // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
                Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor);
                // memset(tx_buff, 0, 1024);
                // memcpy(&tx_buff[6], read_buff, ret);
                spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                // printf("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                            // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
                // for (int i=0; i<1024; i++) {
                    // printf(" 0x%02x", tx_buff[i]);
                    // if (i%16 == 0) printf("\n");
                // }
            }
            wcnt++;
            // usleep(dly*1000);
        } while(ret != 0);
        if (Ready_Busy_Check()){
            // printf("RB Checked!\n");
        }
        else{
            printf("FE[%d]:%d\n", i, wcnt);
            return -1;
        }
        read_buff[0] = i+1;
        Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_FACE_END);
        spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    }

    if (Ready_Busy_Check()){
        // printf("RB Checked!\n");
    }
    else{
        printf("[RB ERR]Face End CMD\n");
        return -1;
    }
    read_buff[0] = minor;
    Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_STREAM_END);
    // memset(tx_buff, 0, 1033);
    // memcpy(&tx_buff[6], read_buff,1);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    usleep(100*1000);
    printf("**********FACE SEND END ************\n");
    return ret;
}

int spi_send_file_dual(uint8_t minor1, uint8_t minor2, char *file1, char *file2)
{
    int filed1 = 0, filed2 = 0, ret = -1;
    // int dly = 3;
    struct stat file_info1, file_info2;
    int sz_file;
    int len = 0;
    int wcnt = 0;
 
    
    if ( 0 > stat(file1, &file_info1)) {
        printf("File1 Size Not Check!!\n");
        return -1;
    }

    

    if ( 0 > stat(file2, &file_info2)) {
        printf("File2 Size Not Check!!\n");
        return -1;
    }

    
    sz_file = file_info1.st_size + file_info2.st_size;

    if (sz_file < 10*1024) {
        printf("File Size Low!:%d\n", sz_file);
        return -1;
    }
    printf("**********FILE SEND START CMD************\n");
    // printf("d %s,s %d,d %d,b %d,m %d,f %s, size:%d\n",device,speed,delay,bits,mode,file,sz_file);

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

    
    if (Ready_Busy_Check())
        printf("File Send Start!\n");
    else{
        printf("Fail to Start1 CMD\n");
        return -1;
    }

    filed1 = open(file1, O_RDONLY);
    if (filed1 == -1) {
        printf("File1 %s Open Fail!\n", file1);
        return -1;
    }

    do {
        ret = read(filed1, read_buff, FILE_READ_LENGTH);
        if(ret != 0) {
            if (Ready_Busy_Check()){
                // printf("RB Checked!\n");
            }
            else{
                printf("F1:%d\n",wcnt);
                return -1;
            }

            // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor1);
            Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor1);
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
            // printf("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                        // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
            // for (int i=0; i<1024; i++) {
                // printf(" 0x%02x", tx_buff[i]);
                // if (i%16 == 0) printf("\n");
            // }
        }
        wcnt++;
        // usleep(dly*1000);
    } while(ret != 0);

    close(filed1);

    // if (Ready_Busy_Check())
    //     printf("File Send Start!\n");
    // else{
    //     printf("Fail to Start2 CMD\n");
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
        printf("File2 %s Open Fail!\n", file2);
        return -1;
    }

    wcnt = 0;

    do {
        ret = read(filed2, read_buff, FILE_READ_LENGTH);
        if(ret != 0) {
            if (Ready_Busy_Check()){
                // printf("RB Checked!\n");
            }
            else{
                printf("F2:%d\n",wcnt);
                return -1;
            }

            // Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor1);
            Make_Spi_Packet(tx_buff, read_buff, ret, REC, minor2);
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
            // printf("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                        // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
            // for (int i=0; i<1024; i++) {
                // printf(" 0x%02x", tx_buff[i]);
                // if (i%16 == 0) printf("\n");
            // }
        }
        wcnt++;
        // usleep(dly*1000);
    } while(ret != 0);

    close(filed2);

    if (Ready_Busy_Check()){
        // printf("RB Checked!\n");
    }
    else{
        printf("RB Check Fail!\n");
        return -1;
    }
    read_buff[0] = minor2;
    Make_Spi_Packet(tx_buff, read_buff, 1, REC, REC_STREAM_END);
    // memset(tx_buff, 0, 1033);
    // memcpy(&tx_buff[6], read_buff,1);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    usleep(100*1000);
    printf("**********FILE SEND END CMD************\n");
    return ret;
}


int spi_send_fake_file(uint8_t minor)
{
    int ret = -1;
    int dly = 3;
    int sz_file;
    
    
    
    sz_file = 1024*600;
    // sz_file = 900*20 + 450;
    printf("**********SPI FILE SEND************\n");
    printf("d %s,s %d,d %d,b %d,m %d\n",device,speed,delay,bits,mode);

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
        // printf("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x 0x%02x 0x%02x 0x%02x\n", 
                    // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-2], tx_buff[1023-3], tx_buff[1023-4], tx_buff[1023-5]);
        // printf("data:0x%02x 0x%02x 0x%02x\n", tx_buff[9], tx_buff[9+20], tx_buff[9+40]);
        printf("Data PKT : %d\n", j);
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
    // printf("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
    //                     tx_buff[0], tx_buff[1], tx_buff[2], tx_buff[3], tx_buff[4], tx_buff[1023]);
    // printf("data:0x%02x 0x%02x 0x%02x\n", tx_tbuff[9], tx_tbuff[9+20], tx_tbuff[9+40]);
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
    
    // printf("s:%s", file);
    sprintf(filebuf, "%s%s", path, file);

    if ( 0 > stat(filebuf, &file_info)) {
        printf("File Size Not Check!!\n");
        return -1;
    }

    filed = open(filebuf, O_RDONLY);
    if (filed == -1) {
        printf("File %s Open Fail!\n", filebuf);
        return -1;
    }

    parse_filename(file, &fileinfo);

    sz_file = file_info.st_size;

    if (sz_file < 10*1024) {
        printf("File Size Low!:%d\n", sz_file);
        return -1;
    }

    printf("size:%d date:%s order:%d topbot:%d type1:%d type2:%d\n", 
        sz_file, fileinfo.date, fileinfo.order, fileinfo.top_bottom, fileinfo.type1, fileinfo.type2);

    printf("**********SAVE SEND START CMD************\n");
    // // printf("d %s,s %d,d %d,b %d,m %d,f %s, size:%d\n",device,speed,delay,bits,mode,file,sz_file);

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

    if (!Ready_Busy_Check()) {
        printf("[%s]SF\n", __func__);
        return -1;
    }
    
    Make_Spi_Packet(tx_buff, read_buff, len, SETTING, SET_FILE_START);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);

    do {
        ret = read(filed, read_buff, FILE_READ_LENGTH);
        if(ret != 0) {
            if (!Ready_Busy_Check()){
                printf("F:%d\n", wcnt);
                return -1;
            }

            Make_Spi_Packet(tx_buff, read_buff, ret, SETTING, SET_FILE_SEND);
            spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
        }
        wcnt++;
    } while(ret != 0);

    if (!Ready_Busy_Check()){
        printf("[%s]EF\n", __func__);
        return -1;
    }
    Make_Spi_Packet(tx_buff, read_buff, 0, SETTING, SET_FILE_END);
    spi_write_bytes(fd, tx_buff, SPI_SEND_LENGTH);
    usleep(100*1000);
    printf("**********FILE SEND END CMD************\n");
    return ret;
}

int spi_device_off(uint8_t major)
{
    printf("off device %s ,speed %d ,delay %d ,bpw %d, mode %d\n",device,speed,delay,bits,mode);
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

void *spi_send_stream_bak (void *arg)
{
    
    int ret = -1;
    uint8_t *buf;
	int datasize = 0;
	int framesize = 0;
    bool frame_end = false;
    bool main_first = false;

	buf = (uint8_t*)malloc(2000);
  
    do {
        /////////// Vedio Main IN -> UDP Out //////////////////////////////////////////
        if (VM_Frame_Buff.cnt > 0) {
            framesize = VM_Frame_Buff.len[VM_Frame_Buff.Rindex];
            for(int i=0; framesize > 0; i++){
                pthread_mutex_lock(&buffMutex_vm);
                datasize = (framesize > V_SEND_SIZE) ? V_SEND_SIZE : framesize;
                // udp_vm_send(VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize);
                framesize -= datasize;
                // printf("cnt:%d, total:%d, dsize:%d\n", i, framesize, datasize);
                pthread_mutex_unlock(&buffMutex_vm);
                // printf("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
            #ifdef __H265__
                Make_Spi_Packet_live(tx_buff, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize, STREAMING, STREAM_VEDIO_M);
            #else
                if (framesize < V_SEND_SIZE)    frame_end = true;
                else                            frame_end = false;
                Make_Spi_Packet_live_rtp(tx_buff, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*i), 
                                            datasize, STREAMING, STREAM_VEDIO_M, VM_Frame_Buff.ftime[VM_Frame_Buff.Rindex], frame_end);

                // printf("end1:0x%02x 0x%02x 0x%02x 0x%02x\n", tx_buff[datasize+12+9], tx_buff[datasize+12+9+1], tx_buff[datasize+12+9+2], tx_buff[datasize+12+9+3]);
            #endif
                
                // memset(tx_buff, 0, 1024);
                // memcpy(&tx_buff[6], read_buff, ret);
                ///////////////// SPI Send //////////////////////////
                if (data_sel == 1 || data_sel == 4) {
                    // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                    ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                    if (ret != 0) {
                        printf("Fail Send SPI Data!\n");
                    }
                    else {
                        // printf("cnt:%d total:%d dsize%d\n", i, framesize, datasize);
                        // printf("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                            // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
                        // printf("V1\n");
                        if (!main_first) main_first = true;
                        usleep(1*1000);
                    }
                }
            }
            VM_Frame_Buff.Rindex = (VM_Frame_Buff.Rindex+1)%10;
            VM_Frame_Buff.cnt--;
        }
        //////////////////////////////////////////////////////////////////////////////
        Recv_Spi_Packet_live(rx_buff); 

		/////////// Vedio Box IN -> UDP Out //////////////////////////////////////////
		if (VB_Frame_Buff.cnt > 0 && main_first) {
			framesize = VB_Frame_Buff.len[VB_Frame_Buff.Rindex];
			for(int i=0; framesize > 0; i++){
				pthread_mutex_lock(&buffMutex_vb);
				datasize = (framesize > V_SEND_SIZE) ? V_SEND_SIZE : framesize;
				// udp_vm_send(VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize);
				framesize -= datasize;
				// printf("cnt:%d, total:%d, dsize:%d\n", i, framesize, datasize);
				pthread_mutex_unlock(&buffMutex_vb);
                // printf("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
            #ifdef __H265__
                Make_Spi_Packet_live(tx_buff, VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize, STREAMING, STREAM_VEDIO_B);
            #else
                if (framesize < V_SEND_SIZE)    frame_end = true;
                else                            frame_end = false;
                Make_Spi_Packet_live_rtp(tx_buff, VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), 
                                            datasize, STREAMING, STREAM_VEDIO_B, VB_Frame_Buff.ftime[VB_Frame_Buff.Rindex], frame_end);
            #endif
				
    	        // memset(tx_buff, 0, 1024);
        	    // memcpy(&tx_buff[6], read_buff, ret);
            	///////////////// SPI Send //////////////////////////
	            if (data_sel == 2 || data_sel == 4) {
    	        	// ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                    ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                    // ret = 0;
        	    	if (ret != 0) {
            			printf("Fail Send SPI Data!\n");
            		}
	            	else {
   	         		// printf("cnt:%d total:%d dsize%d\n", i, framesize, datasize);
                        // printf("V2\n");
    	        		usleep(1*1000);
        	    	}
            	}
			}
			VB_Frame_Buff.Rindex = (VB_Frame_Buff.Rindex+1)%10;
			VB_Frame_Buff.cnt--;
		}
		//////////////////////////////////////////////////////////////////////////////
        Recv_Spi_Packet_live(rx_buff);
       
		/////////// Audio IN -> UDP Out //////////////////////////////////////////////
		if (AI_Cir_Buff.RIndex != AI_Cir_Buff.WIndex) {
			pthread_mutex_lock(&buffMutex_ai);
			datasize = (AI_Cir_Buff.WIndex - AI_Cir_Buff.RIndex + A_BUFF_SIZE) % (500*1024);
			datasize = (datasize > A_SEND_SIZE) ? A_SEND_SIZE : datasize;
			for (int i = 0; i < datasize; ++i) {
				buf[i] = AI_Cir_Buff.tx[AI_Cir_Buff.RIndex];
				AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+1) % (500*1024);
				if (AI_Cir_Buff.RIndex == AI_Cir_Buff.GIndex){
					// printf("[CIR_BUFF_AI]datasize:%d WIndex:%d RIndex%d GIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex, AI_Cir_Buff.GIndex);
					datasize = i+1;
					break;
				}
			}
			// printf("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
			pthread_mutex_unlock(&buffMutex_ai);
			// udp_vm_send(buf, datasize);
			Make_Spi_Packet_live(tx_buff, buf, datasize, STREAMING, STREAM_AUDIO_F);
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            ///////////////// SPI Send //////////////////////////
            if (data_sel == 3 || data_sel == 4) {
            	// ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                // ret = 0;
            	if (ret != 0) {
            		printf("Fail Send SPI Data!\n");
            	}
            	else {
            		// printf("AUDIO Send Data : 0x%02X%02X\n", tx_buff[3], tx_buff[4]);
                    // printf("A\n");
	            	usleep(1*1000);
    	        }
			}
            /////////////////////////////////////////////////////
		}
		//////////////////////////////////////////////////////////////////////////////
        Recv_Spi_Packet_live(rx_buff);

        

		
    } while(!bStrem);
    
    return ((void*)0);
}




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
    // int delay_state = 0;
    int mv_delay = 2;
    // bool stream_start1 = false;

    buf = (uint8_t*)malloc(2000);
  
    do {
        /////////// Vedio Main IN -> UDP Out //////////////////////////////////////////
        // if (!stream_start1) {
            // if (VM_Frame_Buff.cnt > 7) stream_start1 = true;
        // }
        // else if (VM_Frame_Buff.cnt > 0) {
        if (VM_Frame_Buff.cnt > 0) {
            
            // if (VM_Frame_Buff.cnt >= 10) {
            //     if (delay_state != 1) {
            //         printf("Delay State 1!\n");
            //         mv_delay = 2;
            //         delay_state = 1;
            //     }
            // }
            // else if (VM_Frame_Buff.cnt <= 7) {
            //     if (delay_state != 2) {
            //         printf("Delay State 2!\n");
            //         Set_Target_Bit2(500);
            //         delay_state = 2;
            //     }
            // }
            // else if (VM_Frame_Buff.cnt <= 4){
            //     if (delay_state != 3) {
            //         printf("Delay State 3!\n");
            //         mv_delay = 3;
            //         delay_state = 3;
            //     }
            // }

            if (bitrate_change != 500 && (sample_gettimeus()-bitrate_cnt)>3000000) {
                bitrate_change = 500;
                Set_Target_Bit2(bitrate_change);
            }

            if (frame_ptr1 == 0)
                framesize1 = VM_Frame_Buff.len[VM_Frame_Buff.Rindex]-(V_SEND_SIZE*frame_ptr1);
            pthread_mutex_lock(&buffMutex_vm);
            datasize = (framesize1 > V_SEND_SIZE) ? V_SEND_SIZE : framesize1;
            // udp_vm_send(VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr1), datasize);
            framesize1 -= datasize;
            // printf("cnt:%d, total:%d, dsize:%d\n", frame_ptr1, framesize1, datasize);
            pthread_mutex_unlock(&buffMutex_vm);
            // printf("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
        #ifdef __H265__
            Make_Spi_Packet_live(tx_buff, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr1), datasize, STREAMING, STREAM_VEDIO_M);
        #else
            if (datasize < V_SEND_SIZE)    frame_end = true;
            else                            frame_end = false;
            Make_Spi_Packet_live_rtp(tx_buff, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr1), 
                                        datasize, STREAMING, STREAM_VEDIO_M, VM_Frame_Buff.ftime[VM_Frame_Buff.Rindex], frame_end);
        #endif
            // printf("[%d]cnt:%d total:%d dsize:%d\n", VM_Frame_Buff.Rindex, frame_ptr1, framesize1, datasize);
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            ///////////////// SPI Send //////////////////////////
            if (data_sel == 1 || data_sel == 4) {
                // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                if (ret != 0) {
                    printf("Fail Send SPI Data!\n");
                }
                else {
                    frame_ptr1++;
                    
                    // printf("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                        // tx_buff[0+5], tx_buff[1+5], tx_buff[2+5], tx_buff[3+5], tx_buff[4+5], tx_buff[1023-5]);
                    // printf("V1\n");
                    if (main_first < 10) main_first++;
                    usleep(mv_delay*1000);
                }
            }
            if (framesize1 <= 0) {
                VM_Frame_Buff.Rindex = (VM_Frame_Buff.Rindex+1)%20;
                VM_Frame_Buff.cnt--;
                frame_ptr1 = 0;
            }
        }
        //////////////////////////////////////////////////////////////////////////////
        Recv_Spi_Packet_live(rx_buff); 

        /////////// Vedio Box IN -> UDP Out //////////////////////////////////////////
        if (VB_Frame_Buff.cnt > 0 && (main_first >= 10)) {
            if (frame_ptr2 == 0)
                framesize2 = VB_Frame_Buff.len[VB_Frame_Buff.Rindex]-(V_SEND_SIZE*frame_ptr2);
            pthread_mutex_lock(&buffMutex_vb);
            datasize = (framesize2 > V_SEND_SIZE) ? V_SEND_SIZE : framesize2;
            // udp_vm_send(VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr2), datasize);
            framesize2 -= datasize;
            // printf("cnt:%d, total:%d, dsize:%d\n", frame_ptr2, framesize2, datasize);
            pthread_mutex_unlock(&buffMutex_vb);
            // printf("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
        #ifdef __H265__
            Make_Spi_Packet_live(tx_buff, VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr2), datasize, STREAMING, STREAM_VEDIO_B);
        #else
            if (datasize < V_SEND_SIZE)    frame_end = true;
            else                            frame_end = false;
            Make_Spi_Packet_live_rtp(tx_buff, VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*frame_ptr2), 
                                        datasize, STREAMING, STREAM_VEDIO_B, VB_Frame_Buff.ftime[VB_Frame_Buff.Rindex], frame_end);
        #endif
            
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            ///////////////// SPI Send //////////////////////////
            if (data_sel == 2 || data_sel == 4) {
                // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                // ret = 0;
                if (ret != 0) {
                    printf("Fail Send SPI Data!\n");
                }
                else {
                    frame_ptr2++;
                    // printf("cnt:%d total:%d dsize%d\n", frame_ptr2, framesize2, datasize);
                    // printf("V2\n");
                    usleep(mv_delay*1000);
                }
            }
            if (framesize2 <= 0) {
                VB_Frame_Buff.Rindex = (VB_Frame_Buff.Rindex+1)%20;
                VB_Frame_Buff.cnt--;
                frame_ptr2 = 0;
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

            if (datasize > 0) {
                pthread_mutex_lock(&buffMutex_ai);
                datasize = (datasize > A_SEND_SIZE) ? A_SEND_SIZE : datasize;
                // for (int i = 0; i < datasize; ++i) {
                //     buf[i] = AI_Cir_Buff.tx[AI_Cir_Buff.RIndex];
                //     AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+1) % (500*1024);
                //     if (AI_Cir_Buff.RIndex == AI_Cir_Buff.GIndex){
                //         // printf("[CIR_BUFF_AI]datasize:%d WIndex:%d RIndex%d GIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex, AI_Cir_Buff.GIndex);
                //         datasize = i+1;
                //         break;
                //     }
                // }
                // printf("Audio Rec Len : %d\n", datasize);
                printf("[CIR_BUFF_AI]datasize:%d WIndex:%d RIndex%d len\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
                memset(buf, 0, datasize);
                memcpy(buf, &AI_Cir_Buff.tx[AI_Cir_Buff.RIndex], datasize);
                AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+datasize) % (500*1024);
                // if (AI_Cir_Buff.RIndex == AI_Cir_Buff.WIndex) {
                //     AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+1) % (500*1024);
                // }
                pthread_mutex_unlock(&buffMutex_ai);
            }


            // printf("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
            
            // udp_vm_send(buf, datasize);
            Make_Spi_Packet_live(tx_buff, buf, datasize, STREAMING, STREAM_AUDIO_F);
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            ///////////////// SPI Send //////////////////////////
            if (data_sel == 3 || data_sel == 4) {
                // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                // ret = 0;
                if (ret != 0) {
                    printf("Fail Send SPI Data!\n");
                }
                else {
                    // printf("AUDIO Send Data : 0x%02X%02X\n", tx_buff[3+5], tx_buff[4+5]);
                    // printf("A\n");
                    usleep(mv_delay*1000);
                }
            }
            /////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////////
        Recv_Spi_Packet_live(rx_buff);

        

        
    } while(!bStrem);
    
    return ((void*)0);
}


void *spi_test_send_stream (void *arg)
{
    
    int ret = -1;
    uint8_t *buf;
    int datasize = 0;
    int framesize = 0;

    buf = (uint8_t*)malloc(2000);
  
    // printf("set parse is : device %s ,speed %d ,delay %d ,bpw %d\n",device,speed,delay,bits);
    for(int i=0; i<1024; i++){
        tx_tbuff[i] = i%256;
    }

    do {
        /////////// Vedio Box IN -> UDP Out //////////////////////////////////////////
        if (VB_Frame_Buff.cnt > 0) {
            framesize = VB_Frame_Buff.len[VB_Frame_Buff.Rindex];
            for(int i=0; framesize > 0; i++){
                pthread_mutex_lock(&buffMutex_vb);
                datasize = (framesize > V_SEND_SIZE) ? V_SEND_SIZE : framesize;
                // udp_vm_send(VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize);
                framesize -= datasize;
                // printf("cnt:%d, total:%d, dsize:%d\n", i, framesize, datasize);
                pthread_mutex_unlock(&buffMutex_vb);
                // printf("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
                Make_Spi_Packet_live(tx_buff, VB_Frame_Buff.tx[VB_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize, STREAMING, STREAM_VEDIO_B);
                // memset(tx_buff, 0, 1024);
                // memcpy(&tx_buff[6], read_buff, ret);
                ///////////////// SPI Send //////////////////////////
                if (data_sel == 2 || data_sel == 4) {
                    // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                    // ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                    // ret = spi_write_bytes(fd,tx_tbuff, SPI_SEND_LENGTH);
                    ret = spi_rw_bytes(fd,tx_tbuff,rx_buff,SPI_SEND_LENGTH);
                    if (ret != 0) {
                        printf("Fail Send SPI Data!\n");
                    }
                    else {
                    // printf("MAIN Send Data : 0x%02X%02X\n", tx_buff[3], tx_buff[4]);
                        usleep(1*1000);
                    }
                }
            }
            VB_Frame_Buff.Rindex = (VB_Frame_Buff.Rindex+1)%5;
            VB_Frame_Buff.cnt--;
        }
        //////////////////////////////////////////////////////////////////////////////
        Recv_Spi_Packet_test(rx_buff);

        if (VM_Frame_Buff.cnt > 0) {
            framesize = VM_Frame_Buff.len[VM_Frame_Buff.Rindex];
            for(int i=0; framesize > 0; i++){
                pthread_mutex_lock(&buffMutex_vm);
                datasize = (framesize > V_SEND_SIZE) ? V_SEND_SIZE : framesize;
                // udp_vm_send(VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize);
                framesize -= datasize;
                // printf("cnt:%d, total:%d, dsize:%d\n", i, framesize, datasize);
                pthread_mutex_unlock(&buffMutex_vm);
                // printf("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, VM_Cir_Buff.WIndex, VM_Cir_Buff.RIndex);
                Make_Spi_Packet_live(tx_buff, VM_Frame_Buff.tx[VM_Frame_Buff.Rindex]+(V_SEND_SIZE*i), datasize, STREAMING, STREAM_VEDIO_M);
                // memset(tx_buff, 0, 1024);
                // memcpy(&tx_buff[6], read_buff, ret);
                ///////////////// SPI Send //////////////////////////
                if (data_sel == 1 || data_sel == 4) {
                    // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                    // ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                    // ret = spi_write_bytes(fd,tx_tbuff, SPI_SEND_LENGTH);
                    ret = spi_rw_bytes(fd,tx_tbuff,rx_buff,SPI_SEND_LENGTH);
                    if (ret != 0) {
                        printf("Fail Send SPI Data!\n");
                    }
                    else {
                        // printf("d:0x%02X%02X\n", tx_buff[3], tx_buff[4]);
                        usleep(1*1000);
                    }
                }
            }
            VM_Frame_Buff.Rindex = (VM_Frame_Buff.Rindex+1)%5;
            VM_Frame_Buff.cnt--;
        }
        //////////////////////////////////////////////////////////////////////////////
        Recv_Spi_Packet_test(rx_buff);
        
        /////////// Audio IN -> UDP Out //////////////////////////////////////////////
        if (AI_Cir_Buff.RIndex != AI_Cir_Buff.WIndex) {
            pthread_mutex_lock(&buffMutex_ai);
            datasize = (AI_Cir_Buff.WIndex - AI_Cir_Buff.RIndex + A_BUFF_SIZE) % (500*1024);
            datasize = (datasize > A_SEND_SIZE) ? A_SEND_SIZE : datasize;
            for (int i = 0; i < datasize; ++i) {
                buf[i] = AI_Cir_Buff.tx[AI_Cir_Buff.RIndex];
                AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+1) % (500*1024);
                if (AI_Cir_Buff.RIndex == AI_Cir_Buff.GIndex){
                    // printf("[CIR_BUFF_AI]datasize:%d WIndex:%d RIndex%d GIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex, AI_Cir_Buff.GIndex);
                    datasize = i+1;
                    break;
                }
            }
            // printf("[CIR_BUFF_VM]datasize:%d WIndex:%d RIndex%d\n", datasize, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
            pthread_mutex_unlock(&buffMutex_ai);
            // udp_vm_send(buf, datasize);
            Make_Spi_Packet_live(tx_buff, buf, datasize, STREAMING, STREAM_AUDIO_F);
            // memset(tx_buff, 0, 1024);
            // memcpy(&tx_buff[6], read_buff, ret);
            ///////////////// SPI Send //////////////////////////
            if (data_sel == 3 || data_sel == 4) {
                // ret = spi_write_bytes(fd,tx_buff, SPI_SEND_LENGTH);
                // ret = spi_rw_bytes(fd,tx_buff,rx_buff,SPI_SEND_LENGTH);
                // ret = spi_write_bytes(fd,tx_tbuff, SPI_SEND_LENGTH);
                ret = spi_rw_bytes(fd,tx_tbuff,rx_buff,SPI_SEND_LENGTH);
                if (ret != 0) {
                    printf("Fail Send SPI Data!\n");
                }
                else {
                    // printf("AUDIO Send Data : 0x%02X%02X\n", tx_buff[3], tx_buff[4]);
                    usleep(1*1000);
                }
            }
            /////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////////
        Recv_Spi_Packet_test(rx_buff);
    } while(!bStrem);
    
    return ((void*)0);
}



void test_spi_rw(void) {

    memset(rx_buff, 0x00, 1024);
    for(int i=0; i<1024; i++) {
        tx_buff[i] = i%255;
    }
    
    spi_rw_bytes(fd, tx_buff, rx_buff, 1024);

    printf("SPI RX:\n");
    for(int j=0; j<1024; j++) {
        printf("0x%02x ", rx_buff[j]);
        if (j%16 == 15)
            printf("\n");
    }

    // usleep(1*1000*1000);
}

void test_spi_onekbytes(int dly){
    int cnt=0;

    do {
        printf("Loop Test Cnt:%d\n", cnt++);
        for(int i=0; i<1024; i++) {
            tx_buff[i] = i%256;
        }
    
        spi_rw_bytes(fd, tx_buff, rx_buff, 1024);

        usleep(dly*1000);
    }while(1);
}

int ota_fd = 0;
uint16_t ota_seq = 0;
char md5_hash_code[32] = {0};

int OTA_Recv_Packet(uint8_t *rbuff) {
    int len, ret = -1, res = 0;
    uint8_t major, minor;
    uint16_t seq_buf;
    int buff_space = 0;
    int recv_type = 0;
    // static uint8_t data[10]= {0};
    int bad_cnt = 0;
    uint8_t *ota_buf;
    static uint16_t seqcnt = 0xFFFF;

#if 0
    static int filefd = 0;
    if (filefd == 0) {
        filefd = open("/dev/shm/test.pcm", O_RDWR | O_CREAT | O_TRUNC, 0777);
    }

    major = rbuff[1];
    minor = rbuff[2];
    if (rbuff[0] != 0x02) {
        // printf("S\n");
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

    // printf("spi cnt:%d %d\n", rbuff[8], len);

    printf("M : 0x%02x m : 0x%02x len : %d seq : %d\n", major, minor, len, rbuff[8]);

    printf("rbuff:0x%02x 0x%02x 0x%02x 0x%02x\n", rbuff[1009], rbuff[1010], rbuff[1023], rbuff[1024]);

    // for (int i=0; i<1000; i++) {
    //     if (i%10 == 0) printf("\n");
    //     printf("0x%02x ", rbuff[9+i]);
    // }
    // printf("\n");
    
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

    // printf("spi cnt:%d %d %d\n", spicnt, rbuff[8], len);

    if (rbuff[1023] != 0x03){
        printf("[OTA_Recv] Length Worng!\n");
        return -2;
    }

    

    if(minor == SET_FW_DATA) {
        seq_buf = rbuff[5]*256 + rbuff[6];
        if (seqcnt == seq_buf) {
            printf("[OTA_Recv] Seq Worng1! %d %d\n", seqcnt, seq_buf);
            return -3;
        }
        else {
            if (((seqcnt+1)%0x10000) == rbuff[8]) {
                seqcnt = rbuff[8];
                ota_seq = seqcnt;
            }
            else {
                printf("[OTA_Recv] Seq Worng2! %d %d\n", seqcnt, seq_buf);
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
                printf("OTA Start\n");
                recv_type = read_buff[9];
                memset(md5_hash_code, 0, 32);
                memcpy(md5_hash_code, &read_buff[10], 32);
                char print_buf[35] = {0};
                memcpy(print_buf, md5_hash_code, 32);
                print_buf[32] = '\n';
                printf("hash:%s", print_buf);
                ota_fd = open("/dev/shm/testfile", O_RDWR | O_CREAT | O_TRUNC, 0777);
                if (ota_fd < 0) {
                    printf("File Open Fail!\n");
                    return -4;
                }
                res = 1;
            break;
            case SET_FW_DATA:
                // ota_buf = malloc(1024);
                printf("OTA Data Seq : %d\n", ota_seq);
                // memset(ota_buf, 0, 1024);
                // memcpy(ota_buf, &read_buff[9], len);
                ret = write(ota_fd, &read_buff[9], len);
                if (ret < 0) {
                    printf("Write Fail\n");
                    return -4;
                }
                // free(ota_buf);
                res = 2;
            break;
            case SET_FW_END:
                printf("OTA End\n");
                if (ota_fd > 0) {
                    close(ota_fd);
                }
                res = 3;
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
        printf("State Error! : %d\n", state);
        return -1;
    }

    tbuff[1023] = 0x03;
   
    return 0;
}

void *OTA_Thread(void * argc) {
    int ota_state = OTA_STATE_READY;
    int64_t timeout_t = sample_gettimeus();
    int ret = -1;
    uint8_t err_state = OTA_END_SUCCESS;

    ret = spi_init();
    if(ret < 0){
        printf("[OTA]spi init error\n");
        return NULL;
    }

    // tx_buff
    // rx_buff

    do {
        if (ota_state == OTA_STATE_READY){
            printf("OTA Ready!\n");
            ota_state = OTA_STATE_START;
        }
        else if (ota_state == OTA_STATE_START) {
            printf("OTA Start!\n");
            ret = Make_OTA_Dummy_Packet(tx_buff, ota_type_u, ota_state, 0);
            if (ret < 0) {
                // ota_state = OTA_STATE_END;
                // err_state = OTA_END_DISCON;
            }
            ret = spi_rw_bytes(fd, tx_buff, rx_buff, SPI_SEND_LENGTH);
            if (ret != 0) {
                printf("Fail Send SPI Start!\n");
            }
            ret = OTA_Recv_Packet(rx_buff);
            if (ret == 1) {
                ota_state = OTA_STATE_DATA;
            }
            else {
                // ota_state = OTA_STATE_END;
                // err_state = OTA_END_DISCON;
            }
        }
        else if (ota_state == OTA_STATE_DATA) {
            printf("OTA Data!\n");
            ret = Make_OTA_Dummy_Packet(tx_buff, ota_type_u, ota_state, 0);
            if (ret < 0) {
                // err_state = OTA_END_DISCON;
            }
            ret = spi_rw_bytes(fd, tx_buff, rx_buff, SPI_SEND_LENGTH);
            if (ret != 0) {
                printf("Fail Send SPI Data!\n");
            }
            ret = OTA_Recv_Packet(rx_buff);
            if (ret == 2) {
            }
            else if (ret == 3) {
            // if (ret == 3) {
                ota_state = OTA_STATE_END;
            }
            else {
                // ota_state = OTA_STATE_END;
                // err_state = OTA_END_DISCON;
            }
        }
        else if (ota_state == OTA_STATE_END) {
            ret = Make_OTA_Dummy_Packet(tx_buff, ota_type_u, ota_state, err_state);
            if (ret < 0) {
                // err_state = OTA_END_DISCON;
            }
            ret = spi_rw_bytes(fd, tx_buff, rx_buff, SPI_SEND_LENGTH);
            if (ret != 0) {
                printf("Fail Send SPI End!\n");
            }

            ota_state = OTA_STATE_SHUTDN;
        }
        else {
            printf("OTA Finish!\n");
            cmd_end_flag = true;
            return NULL;
        }
        usleep(3*1000);
    } while (!bOTA);
    return NULL;
}