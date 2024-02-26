/*
 * Ingenic SU ADC solution.
 *
 * Copyright (C) 2019 Ingenic Semiconductor Co.,Ltd
 * Author: Damon<jiansheng.zhang@ingenic.com>
 */

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>

#include "adc.h"

int adc_fd;
int vol_n;

int adc_init(void)
{
	int ret = 0;
	char path[ADC_PATH_LEN];

	sprintf(path, "%s", ADC_PATH);

	adc_fd = open(path, O_RDONLY);
	if(adc_fd < 0) {
		printf("sample_adc:open error !\n");
		ret = -1;
		close(adc_fd);
		return -1;
	}

	/* set reference voltage */
	vol_n = STD_VAL_VOLTAGE;
	ret = ioctl(adc_fd, ADC_SET_VREF, &vol_n);
	if(ret){
		printf("Failed to set reference voltage!\n");
		close(adc_fd);
		return -1;
	}

	ret = ioctl(adc_fd, ADC_ENABLE);
	if(ret){
		printf("Failed to enable adc!\n");
		close(adc_fd);
		return -1;	
	}

	return 0;
}


	// /* get value thread */
	// pthread_t adc_thread_id;
	// ret = pthread_create(&adc_thread_id, NULL, adc_get_voltage_thread, NULL);
	// if (ret != 0) {
	// 	printf("error: pthread_create error!!!!!!");
	// 	return -1;
	// }

	// pthread_join(adc_thread_id, NULL);

int adc_deinit(void){
	// int ret = 0;
	/* disable adc */
	// ret = ioctl(adc_fd, ADC_DISABLE);
	ioctl(adc_fd, ADC_DISABLE);
	close(adc_fd);
    return 0;
}

void *adc_get_voltage_thread(void *argc)
{
    long int value = 0;
	int ret = 0, cnt=0;;
    while(cnt < 5)
    // while(1)
    {
		// enable adc
		// ret = ioctl(adc_fd, ADC_ENABLE);
		// if(ret){
		// 	printf("Failed to enable adc!\n");
		// 	break;
		// }

		// get value
		ret = read(adc_fd, &value, sizeof(int));
		if(!ret){
			printf("Failed to read adc value!\n");
			break;
		}

		//printf get value
		printf("### adc value is : %ldmV ###\n", value / 10);

		// disable adc
		// ret = ioctl(adc_fd, ADC_DISABLE);
		// if(ret){
		// 	printf("Failed to disable adc!\n");
		// 	break;
		// }
		cnt++;

        sleep(0.01);
    }

    /* close adc_fd */
    // close(adc_fd);

	return NULL;
}