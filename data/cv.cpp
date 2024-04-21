#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <string>
#include <iostream>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/spi/spidev.h>

#include <opencv2/opencv.hpp>

#include "st_common.h"
#include "def.h"

static MI_BOOL bExit = FALSE;

static const char *device = "/dev/spidev1.0";
static uint8_t mode = 0;    /* Use full-duplex mode for SPI communication, set CPOL = 0 and CPHA = 0*/
static uint8_t bits = 8;    /* 8-bit read/write, MSB first*/
// static uint32_t speed = 60*1000*1000;   /*Set transmit speed 60Mbps*/
uint32_t speed = 5*1024*1024;   /*Set transmit speed 20Mbps by test*/
static uint16_t delay = 0;
static int g_SPI_Fd = 0;

static void pabort(const char *s)
{
    perror(s);
    abort();
}

int SPI_Transfer(const uint8_t *TxBuf, uint8_t *RxBuf, uint32_t len)
{
    int ret;
    int fd = g_SPI_Fd;
    struct spi_ioc_transfer tr ={
        .tx_buf = (unsigned long) TxBuf,
        .rx_buf = (unsigned long) RxBuf,
        .len =len,
        .delay_usecs = delay,
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

    if (ret < 1)
        perror("can't send spi message\n");
    else
    {
        #if SPI_DEBUG
        int i;
        printf("nsend spi message Succeed\n");
        printf("nSPI Send [Len:%d]: \n", len);
        for (i = 0; i < len; i++)
        {
        if (i % 8 == 0)
        printf("nt\n");
        printf("0x%02X \n", TxBuf[i]);
        }
        printf("n");
        printf("SPI Receive [len:%d]:\n", len);
        for (i = 0; i < len; i++)
        {
        if (i % 8 == 0)
        printf("nt\n");
        printf("0x%02X \n", RxBuf[i]);
        }
        printf("\n");
        #endif
    }
    return ret;
}

/**
* Function: transmit data
* Input port parameter:
* TxBuf -> Transmit data start address
* len -> Transmit data length
* Return value: 0 for success
* Developer: Lzy 2013－5－22
*/

int SPI_Write(uint8_t *TxBuf, uint32_t len)
{
    int ret;
    int fd = g_SPI_Fd;
    ret = write(fd, TxBuf, len);
    if (ret < 0)
        perror("SPI Write error\n");
    else
    {
        #if SPI_DEBUG
        int i;
        printf("SPI Write [Len:%d]: \n", len);
        for (i = 0; i < len; i++)
        {
            if (i % 8 == 0)
            printf("\n\t");
            printf("0x%02X \n", TxBuf[i]);
        }
        printf("\n");
        #endif
    }
    return ret;
}

/**
* Function: Receive data
* Output port parameter:
* RxBuf -> Receive data buffer
* rtn -> Receive data length
* Return value: >=0 for success
* Developer: Lzy 2013－5－22
*/

int SPI_Read(uint8_t *RxBuf, int len)
{
    int ret;
    int fd = g_SPI_Fd;
    ret = read(fd, RxBuf, len);
    if (ret < 0)
        printf("SPI Read error\n");
    else
    {
        #if SPI_DEBUG
        int i;
        printf("SPI Read [len:%d]:\n", len);
        for (i = 0; i < len; i++)
        {
            if (i % 8 == 0)
            printf("\n\t");
            printf("0x%02X \n", RxBuf[i]);
        }
        printf("\n");
        #endif
    }
    return ret;
}

/**
* Function: Open and initialize device
* Input port parameter:
* Output port parameter:
* Return value: 0 means SPI device opened and 0XF1 means SPI device opened but some other error happened
* Developer: Lzy 2013－5－22
*/

int SPI_Open(void)
{
    int fd;
    int ret = 0;
    if (g_SPI_Fd != 0) /* Device opened */
        return 0xF1;
    fd = open(device, O_RDWR);
    if (fd < 0)
        pabort("can't open device\n");
    else
        printf("SPI - Open Succeed. Start Init SPI...\n");
    g_SPI_Fd = fd;
    /*
    * spi mode
    */
    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    if (ret == -1)
        pabort("can't set spi mode\n");
    ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
    if (ret == -1)
        pabort("can't get spi mode\n");
    /*
    * bits per word
    */
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't set bits per word\n");
    ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    if (ret == -1)
        pabort("can't get bits per word\n");
    /*
    * max speed hz
    */
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't set max speed hz\n");
    ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        pabort("can't get max speed hz\n");
    printf("spi mode: %d\n", mode);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d KHz (%d MHz)\n", speed / 1000, speed / 1000 / 1000);
    return ret;
}

/**
* Function: Close SPI module
*/

int SPI_Close(void)
{
    int fd = g_SPI_Fd;
    if (fd == 0) /* Whether SPI has been opened*/
        return 0;
    close(fd);
    g_SPI_Fd = 0;
    return 0;
}

/**
* Function: Self transmit/receive test program
* Fail if the received data are not identical with the transmitted data
* Description:
* Need to short the input and output pins on hardware
* Developer: Lzy 2013－5－22
*/

int SPI_LookBackTest(void)
{
    int ret;
    uint32_t i;
    const uint32_t BufSize = 1500;
    uint8_t *rx, *tx;

    tx = (uint8_t*)malloc((size_t)BufSize);
    rx = (uint8_t*)malloc((size_t)BufSize);
        // uint8_t tx[BufSize], rx[BufSize];
    bzero(rx, sizeof(rx));
    for (i = 0; i < BufSize; i++)
        tx[i] = i%0x100;
    printf("nSPI - LookBack Mode Test...\n");
    ret = SPI_Transfer(tx, rx, BufSize);
    // ret = SPI_Write(tx, BufSize);
    if (ret > 1)
    {
        ret = memcmp(tx, rx, BufSize);
        if (ret != 0)
        {
            printf("tx:\n");
            for (i = 0; i < BufSize; i++)
            {
                printf("%03d ", tx[i]);
                if (i%20 == 0) printf("\n");
            }
            printf("\n");
            printf("rx:\n");
            for (i = 0; i < BufSize; i++)
            {
                printf("%03d ", rx[i]);
                if (i%20 == 0) printf("\n");
            }
        printf("\n");
        perror("LookBack Mode Test error\n");
        }
        else
            printf("SPI - LookBack Mode OK\n");
    }
    free(tx);
    free(rx);
    return ret;
}

#define SPI_TEST_WMODE 1
#define SPI_TEST_RMODE 2

int SPI_RWTEST(uint8_t mode, uint32_t len)
{
    int ret;
    uint32_t i;
    uint32_t BufSize = len;
    uint8_t *rx, *tx;
    static uint8_t exdata = 0;

    exdata++;
    if (mode == 1) {
        // printf("SPI - Write Test...\n");
        tx = (uint8_t*)malloc((size_t)BufSize);
        for (i = 0; i < BufSize; i++)
            tx[i] = exdata & 0xFF;
        ret = SPI_Write(tx, BufSize);
        if (ret > 1) {
            // printf("tx:\n");
            // for (i = 0; i < BufSize; i++)
            // {
            //     printf("%03d ", tx[i]);
            //     if (i%20 == 0) printf("\n");
            // }
            // printf("\n");
        }
        else {
            printf("Fails Send SPI\n");
        }
        printf("Tx : %d len : %d\n", exdata, BufSize);
        free(tx);
    }
    else if (mode == 2) {
        printf("SPI - Read Test...\n");
         rx = (uint8_t*)malloc((size_t)BufSize);
        bzero(rx, sizeof(rx));
        ret = SPI_Read(rx, BufSize);
        if (ret > 1) {
            printf("rx:\n");
            for (i = 0; i < BufSize; i++)
            {
                printf("%03d ", rx[i]);
                if (i%20 == 0) printf("\n");
            }
            printf("\n");
        }
        free(rx);
    }
    else {
        printf("Mode Error!!");
        return 0;
    }
    
    return ret;
}

int SPI_RWTEST2(uint8_t mode, uint8_t *buff, uint32_t len)
{
    int ret;
    uint32_t i;
    uint32_t BufSize = len;
    uint8_t *rx, *tx;
    static uint8_t exdata = 0;

    exdata++;
    if (mode == 1) {
        // printf("SPI - Write Test...\n");
        tx = (uint8_t*)malloc((size_t)BufSize);
        // for (i = 0; i < BufSize; i++)
            // tx[i] = exdata & 0xFF;
        memset(tx, 0, BufSize);
        memcpy(tx, buff, BufSize);
        ret = SPI_Write(tx, BufSize);
        if (ret > 1) {
            // printf("tx:\n");
            // for (i = 0; i < BufSize; i++)
            // {
            //     printf("%03d ", tx[i]);
            //     if (i%20 == 0) printf("\n");
            // }
            // printf("\n");
        }
        else {
            printf("Fails Send SPI\n");
        }
        // printf("Tx: STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x len:%d\n", 
                          // tx[0],    tx[1],tx[2],  tx[3],tx[4],   BufSize);
        free(tx);
    }
    else if (mode == 2) {
        printf("SPI - Read Test...\n");
         rx = (uint8_t*)malloc((size_t)BufSize);
        bzero(rx, sizeof(rx));
        ret = SPI_Read(rx, BufSize);
        if (ret > 1) {
            printf("rx:\n");
            for (i = 0; i < BufSize; i++)
            {
                printf("%03d ", rx[i]);
                if (i%20 == 0) printf("\n");
            }
            printf("\n");
        }
        free(rx);
    }
    else {
        printf("Mode Error!!");
        return 0;
    }
    
    return ret;
}

void ST_Flush(void)
{
    char c;

    while((c = getchar()) != '\n' && c != EOF);
}

int motion_detecte(void)
{
    std::string image1_path = "/tmp/before.jpg";
    std::string image2_path = "/tmp/after.jpg";

    cv::Mat frame1 = cv::imread(image1_path);
    cv::Mat frame2 = cv::imread(image2_path);

    if (frame1.empty() || frame2.empty()) {
        std::cerr << "Can't Open File!" << std::ends;
        return -1;
    }

    cv::Mat diff;
    cv::cvtColor(frame1, frame1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(frame2, frame2, cv::COLOR_BGR2GRAY);
    cv::absdiff(frame1, frame2, diff);

    // cv::imwrite("/tmp/diff_b.jpg", frame1);
    // cv::imwrite("/tmp/diff_a.jpg", frame2);

    // cv::cvtColor(diff, diff, cv::COLOR_BGR2GRAY);

    int threshold_value = 30;
    cv::Mat thresholded;
    cv::threshold(diff, thresholded, threshold_value, 255, cv::THRESH_BINARY);

    // cv::imwrite("/tmp/diff.jpg", thresholded);

    int nonzero_pixels = cv::countNonZero(thresholded);

    return nonzero_pixels;
}

int package_detecte(void)
{
    std::string image1_path = "/customer/before.jpg";
    std::string image2_path = "/customer/after.jpg";

    cv::Mat img1 = cv::imread(image1_path);
    cv::Mat img2 = cv::imread(image2_path);

    if (img1.empty() || img2.empty()) {
        std::cerr << "Can't Open File!" << std::ends;
        return -1;
    }
    
    cv::Mat gray1, gray2;
    cv::cvtColor(img1, gray1, cv::COLOR_BGR2GRAY);
    cv::cvtColor(img2, gray2, cv::COLOR_BGR2GRAY);

    cv::equalizeHist(gray1, gray1);
    cv::equalizeHist(gray2, gray2);

    cv::GaussianBlur(gray1, gray1, cv::Size(5, 5), 0);
    cv::GaussianBlur(gray2, gray2, cv::Size(5, 5), 0);

    cv::imwrite("/tmp/img1_gaus.jpg", gray1);
    cv::imwrite("/tmp/img2_gaus.jpg", gray2);

    cv::Mat edges1, edges2;
    cv::Canny(gray1, edges1, 50, 150);
    cv::Canny(gray2, edges2, 50, 150);

    cv::Mat retimg1(img1.size(), img1.type(), cv::Scalar(0, 0, 0));
    cv::Mat retimg2(img2.size(), img2.type(), cv::Scalar(0, 0, 0));

    img1.copyTo(retimg1, edges1);
    img2.copyTo(retimg2, edges2);
    
    cv::imwrite("/tmp/img1_outline.jpg", retimg1);
    cv::imwrite("/tmp/img2_outline.jpg", retimg2);

    cv::Mat diff_image;
    cv::absdiff(retimg1, retimg2, diff_image);

    cv::Mat bin_img;
    cv::threshold(diff_image, bin_img, 30, 255, cv::THRESH_BINARY);

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(bin_img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for(size_t i = 0; i< contours.size(); i++) {
        cv::Rect boundingRect = cv::boundingRect(contours[i]);

        // if (boundingRect.width >= 50 || boundingRect.height > 50) {
            // if (boundingRect.x >= 480 && boundingRect.x <= 960) {
                // if (boundingRect.y >= 270 && boundingRect.y <= 540) {
                    cv::rectangle(img2, boundingRect, cv::Scalar(0, 255, 0), 2);
                // }
            // }
        // }
    }

    // cv::imwrite("/tmp/result.jpg", img2);

    return 0;
    
}

size_t
get_malloc_maxsize(int nbits, int verbose)
{
    size_t prev_size=0, next_size, size;
    void *buf;
    size_t i;

    if (verbose) printf("Step-I)\n");
    for (i=nbits; i <= sizeof(size_t)*8; i++) {
        size = (((1 << (i-1)) - 1) << 1) | 0x1; 
        if (verbose) printf("\ttesting size:0x%08X(%d-bits) ... ", size, i);

        if ((buf=malloc((size_t) size)) == NULL) {
            if (verbose) printf(" fail!\n");
            next_size = size;
            break;
        }
        if (verbose) printf(" o.k.\n");
        free(buf);
        prev_size = size;
    }

    if (!prev_size) {
        if (verbose)
            printf(" step-I) The limit size of malloc() is less than 1(MiB)\n");
        return 0;
    }
    if (verbose) printf("The valid malloc size=%u\n\n", prev_size); 

    if (verbose) printf("Step-II)\n");
    while (prev_size+1 < next_size) {
        size = prev_size + ((next_size - prev_size) >> 1);
        if (verbose) printf("\ttesting size:%u ...", size);

        if ((buf=malloc((size_t) size)) == NULL) {
            next_size = size;
            if (verbose) printf(" fail!\n");
        } else {
            prev_size = size;
            free(buf);
            if (verbose) printf(" o.k.\n");
        }
    }
    return prev_size;
}

// static const char *device = "/dev/spidev1.0";
// static uint8_t mode = 0;
// static uint8_t bits = 8;
// static uint32_t speed = 60*1000*1000;
// static uint16_t delay = 0;
// static int g_SPI_Fd = 0;
// static void pabort(const char *s)
// {
//     perror(s);
//     abort();
// }

// int SPI_Transfer(const uint8_t *TxBuf, uint8_t *RxBuf, int len)
// {
//     int ret;
//     int fd = g_SPI_Fd;
//     struct spi_ioc_transfer tr = {
//         .tx_buf = (unsigned long) TxBuf,
//         .rx_buf = (unsigned long) RxBuf,
//         .len = len,
//         .delay_usecs = delay,
//     };

//     ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

//     if (ret < 1)
//         perror("can't send spi message\n");
//     else
//     {
//         #if SPI_DEBUG
//         int i;
//         printf("nsend spi message Succeed\n");
//         #endif
//     }
//     return ret;
// }

typedef struct SPI_PROTOCOL
{
    uint8_t *buff;
    uint8_t *data;
    uint8_t major;
    uint8_t minor;
    uint32_t len;
    bool nextdata;
}Spi_Protocol;

static Spi_Protocol Spi_Ptl_Attr;

#define SPI_SEND_LENGTH 1033
#define FILE_READ_LENGTH 1024

int M21231ake_Spi_Packet(Spi_Protocol *attr)
{
    memset(attr->buff, 0, SPI_SEND_LENGTH);
    attr->buff[0] = 0x02;
    attr->buff[1] = attr->major & 0xFF;
    attr->buff[2] = attr->minor & 0xFF;
    attr->buff[3] = (attr->len>>8) & 0xFF;
    attr->buff[4] = attr->len & 0xFF;
    attr->buff[5] = 0x00;
    attr->buff[6] = 0x00;
    attr->buff[7] = attr->nextdata & 0xFF;
    switch(attr->major){
        case DTEST:
            switch(attr->minor){
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
            switch(attr->minor){
                case REC_START:
                    break;
                case REC_RESERVED:
                case REC_CLIP_F:
                    memcpy(&attr->buff[8], attr->data, attr->len);
                    break;
                case REC_CLIP_B:
                    break;
                case REC_FACE:
                    break;
                case REC_BOX_ALM:
                    break;
                case REC_STREAM_END:
                    break;
                case REC_ACK:
                    break;
                case REC_STOP:
                    break;
                default:
                    return -1;
                    break;
            }
            break;
    case STREAMING:
            switch(attr->minor){
                case STREAM_START:
                    break;
                case STREAM_REV:
                    break;
                case STREAM_VEDIO_M:
                    memcpy(&attr->buff[8], attr->data, attr->len);
                    break;
                case STREAM_VEDIO_B:
                    memcpy(&attr->buff[8], attr->data, attr->len);
                    break;
                case STREAM_FACE:
                    break;
                case STREAM_AUDIO_F:
                    memcpy(&attr->buff[8], attr->data, attr->len);
                    break;
                case REC_STOP:
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
    // attr->buff[attr->len + 8] = 0x03;
    attr->buff[1032] = 0x03;
    return 0;
}

int main(int argc, char **argv)
{
    // int motion=0, verbose=1, test_cnt=0;
    // size_t maxsize;
    int delay = 0, ret = 0;
    uint32_t clk = 5;

    int fd = 0;
    uint8_t *rd_buf = (uint8_t*)malloc(FILE_READ_LENGTH);
    uint8_t *tx_buf = (uint8_t*)malloc(SPI_SEND_LENGTH);


    char file1[] = "/customer/clip1.mp4";
    char file2[] = "/customer/clip2.mp4";
    char file3[] = "/customer/clip3.mp4";
    int filenum = 0;

    if (argc > 1) {
        for (int i = 1; i< argc; i++) {
            if(std::string(argv[i]) == "-d") {
                if (i + 1 < argc) {
                    delay = std::stod(argv[i + 1]);

                    continue;
                }
                else {
                    std::cerr << "Error: -d option reuires a value." << std::ends;
                    return 1;
                }
            }
            else if(std::string(argv[i]) == "-c") {
                if (i + 1 < argc) {
                    clk = std::stod(argv[i + 1]);
                    speed = clk * 100 * 1024;
                    printf("speed option set : %d\n", speed);
                    continue;
                }
                else {
                    std::cerr << "Error: -c option reuires a value." << std::ends;
                    return 1;
                } 
            }
            else if(std::string(argv[i]) == "-n") {
                if (i + 1 < argc) {
                    filenum = std::stod(argv[i + 1]);
                    if (filenum == 2) {
                        fd = open(file2, O_RDONLY);
                        printf("File Num set : %s\n", file2);
                    }
                    else if (filenum == 3) {
                        fd = open(file3, O_RDONLY);
                        printf("File Num set : %s\n", file3);
                    }
                    else if (filenum == 1) {
                        fd = open(file1, O_RDONLY);
                        printf("File Num set : %s\n", file1);
                    }
                    else {
                        std::cerr << "Error: -n option file number 1~3 value." << std::ends;
                        return 1;
                    }
                    continue;
                }
                else {
                    std::cerr << "Error: -n option file number 1~3 value." << std::ends;
                    return 1;
                } 
            }
        }
    }

    SPI_Open();

    rd_buf[0] = 1;
    Spi_Ptl_Attr.buff = tx_buf;
    Spi_Ptl_Attr.data = rd_buf;
    Spi_Ptl_Attr.major = REC;
    Spi_Ptl_Attr.minor = REC_RESERVED;
    Spi_Ptl_Attr.len = 1;
    Make_Spi_Packet(&Spi_Ptl_Attr);
    SPI_RWTEST2(SPI_TEST_WMODE, tx_buf, SPI_SEND_LENGTH); // data start
    printf("File Send Start\n");
    if (delay > 0)
            usleep(delay * 1000);
    while(!bExit)
    {
        // MI_U32 u32Select = 0xff;
        // printf("select 1: Motion Detection. /tmp/before.jpg /tmp/after.jpg\n");
        // printf("select 2: package Detection. /customer/before.jpg /customer/after.jpg\n");
        // printf("select 3: mallock Test.\n");
        // printf("select 12: exit\n");
        // scanf("%d", &u32Select);
        // ST_Flush();
        // if(u32Select == 1)
        // {
        //     motion = motion_detecte();
        //     printf("Difference Pixel : %d\n", motion);
        // }
        // else if(u32Select == 2)
        // {
        //     package_detecte();
        // }
        // else if(u32Select == 3)
        // {
        //     maxsize = get_malloc_maxsize(20, verbose); // start at 20MB
        //     if (!maxsize) {
        //         printf("The limit size of malloc() is less than 1(MiB)\n");
        //     } else {
        //         printf("The limit size of malloc() : %u(0x%08X)\n", maxsize, maxsize); 
        //     }
        // }
        // else if(u32Select == 4)
        // {
        
        // SPI_RWTEST(SPI_TEST_WMODE, len);
        memset(rd_buf, 0, FILE_READ_LENGTH);
        ret = read(fd, rd_buf, FILE_READ_LENGTH);
        // printf("ret:%d\n", ret);
        if (ret < 0){
            printf("Read Fail.!\n");
            return 1;
        }
        else if (ret == 0){
            rd_buf[0] = 1;
            Spi_Ptl_Attr.buff = tx_buf;
            Spi_Ptl_Attr.data = rd_buf;
            Spi_Ptl_Attr.major = REC;
            Spi_Ptl_Attr.minor = REC_STREAM_END;
            Spi_Ptl_Attr.len = 1;
            Make_Spi_Packet(&Spi_Ptl_Attr);
            SPI_RWTEST2(SPI_TEST_WMODE, tx_buf, SPI_SEND_LENGTH); // data end
            printf("File Read End!!\n");
            return 0;
        }   
        else {
            Spi_Ptl_Attr.buff = tx_buf;
            Spi_Ptl_Attr.data = rd_buf;
            Spi_Ptl_Attr.major = REC;
            Spi_Ptl_Attr.minor = REC_CLIP_F;
            Spi_Ptl_Attr.len = ret;
            Make_Spi_Packet(&Spi_Ptl_Attr);
            // printf("STX:0x%02x CMD:0x%02x%02x LEN:0x%02x%02x ETX:0x%02x\n", 
                        // tx_buf[0], tx_buf[1], tx_buf[2], tx_buf[3], tx_buf[4], tx_buf[1032]);
            SPI_RWTEST2(SPI_TEST_WMODE, tx_buf, SPI_SEND_LENGTH); // data send
        }

        // SPI_LookBackTest();
        
        // }
        // else if(u32Select == 12)
        // {
        //     bExit = TRUE;
        // }
        if (delay > 0)
            usleep(delay * 1000);
        // sleep(1);

        // SPI_RWTEST(SPI_TEST_RMODE);

        // sleep(1);
    }

    SPI_Close();

    return 0;
}

