CROSS_COMPILE ?= mips-linux-gnu-

CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

T40_INCLUDE = -I../../../include -I.

OPENCV_INCLUDE = -I../../../../opencv/include

MXU_DIR ?= ../../../../MXU
MXU_LIB_DIR = $(MXU_DIR)/lib/uclibc

IVS_DIR ?= ../../../../IVS
IVS_LIB = -I../../../../IVS/include
IVS_LIB_DIR = $(IVS_DIR)/lib/uclibc

# CFLAGS = $(T40_INCLUDE) $(IVS_LIB) -O2 -Wall -march=mips32r2 -lpthread -lm -lrt -ldl -muclibc
CFLAGS = $(T40_INCLUDE) $(IVS_LIB) -Wall -Wl,-gc-sections -lpthread -lm -lrt -lstdc++ -muclibc -lopenssl
CXXFLAGS = $(OPENCV_INCLUDE) $(T40_INCLUDE) $(IVS_LIB) -Wall -g -std=c++11 -muclibc -lpthread -lm -lrt -ldl 
OPENCV_LIBS = -L../../../../opencv/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_features2d -lopencv_calib3d -lopencv_flann
SDK_LIB_DIR = ../../../lib/uclibc
LIBS = $(SDK_LIB_DIR)/libimp.a $(SDK_LIB_DIR)/libalog.a \
       $(OPENCV_LIBS) \
       $(MXU_LIB_DIR)/libmxu_merge.a  \
	   $(MXU_LIB_DIR)/libmxu_contrib.a $(MXU_LIB_DIR)/libmxu_video.a \
	   $(MXU_LIB_DIR)/libmxu_imgproc.a $(MXU_LIB_DIR)/libmxu_core.a \

VENUS_LIB = -L$(IVS_LIB_DIR) -lvenus
LDFLAG = -Wall -Wl,-gc-sections -lpthread -lm -lrt -lstdc++
LDFLAG += -muclibc

C_SRCS = main.c video-common.c video.c c_util.c udp.c audio.c spi.c move.c fdpd.c gpio.c adc.c uart.c # fdpd-common.c move-common.c
CPP_SRCS = cxx_util.cpp image_comparison.cpp 

C_OBJS = $(C_SRCS:.c=.o)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)	

TARGET = isc

all: $(TARGET)

$(TARGET): $(C_OBJS) $(CPP_OBJS)
	$(CXX) -o $@ $^ $(IVS_LIB_DIR)/libmove_inf.a $(IVS_LIB_DIR)/libfacepersonDet_inf.a $(VENUS_LIB) $(LIBS) $(CXXFLAGS)
	$(STRIP) $@
# 	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS) -lpthread
# $(IVS_LIB_DIR)/libfacepersonDet_inf.a 


# move: move-common.o move.o
# 	$(CC) -o $@ $^  $(IVS_LIB_DIR)/libmove_inf.a $(LIBS) $(LDFLAG)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(C_OBJS) $(CPP_OBJS) $(TARGET)