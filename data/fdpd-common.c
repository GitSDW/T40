/*
 * sample-common.c
 *
 * Copyright (C) 2014 Ingenic Semiconductor Co.,Ltd
 */

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_isp.h>
#include <imp/imp_framesource.h>

#include "fdpd-common.h"

#define TAG "fdpd-Common"

IMPSensorInfo Def_Sensor_Info[1] = {
    {
        FIRST_SNESOR_NAME,
        TX_SENSOR_CONTROL_INTERFACE_I2C,
        {FIRST_SNESOR_NAME, FIRST_I2C_ADDR, FIRST_I2C_ADAPTER_ID},
        FIRST_RST_GPIO,
        FIRST_PWDN_GPIO,
        FIRST_POWER_GPIO,
        FIRST_SENSOR_ID,
        FIRST_VIDEO_INTERFACE,
        FIRST_MCLK,
        FIRST_DEFAULT_BOOT
    },
};

IMPSensorInfo sensor_info[1];
int sensor_main_width = 1920;
int sensor_main_height = 1080;
int sensor_sub_width = 1920;
int sensor_sub_height = 1080;

