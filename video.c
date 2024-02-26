#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_encoder.h>
#include <imp/imp_osd.h>
#include <imp/imp_utils.h>

#include "global_value.h"
#include "video-common.h"
#include "move.h"

#define TAG "video"

extern struct chn_conf chn[];

int mosdgrp = 0;
int bosdgrp = 0;
IMPRgnHandle *prHander;

static int osd_show(void)
{
	int ret, i;

	if (move_flag) {
		IMPOSDRgnAttr rect_rAttr;
		rect_rAttr.type = OSD_REG_RECT;
		rect_rAttr.rect.p0.x = move_det_xs;
		rect_rAttr.rect.p0.y = move_det_ys;
		rect_rAttr.rect.p1.x = move_det_xe - 1;
		rect_rAttr.rect.p1.y = move_det_ye - 1;
		rect_rAttr.fmt = PIX_FMT_MONOWHITE;
		rect_rAttr.data.lineRectData.color = OSD_RED;
		rect_rAttr.data.lineRectData.linewidth = 3;
		IMP_OSD_SetRgnAttr(prHander[TEST_RECT_INDEX], &rect_rAttr);
		ret = IMP_OSD_ShowRgn(prHander[TEST_RECT_INDEX], mosdgrp, 1);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
			return -1;
		}

		ret = IMP_OSD_ShowRgn(prHander[TEST_COVER_INDEX], mosdgrp, 0);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Test Cover error\n");
			return -1;
		}
	}

	IMPOSDRgnAttr cover_rAttr;
	for (i=0; i<GRID_COVER_INDEX; i++) {
		if (grid_cover_flag[i] == true){
			IMP_OSD_GetRgnAttr(prHander[2+RECT_INDEX+MOSAIC_INDEX+i], &cover_rAttr);
			printf("grid[%d] x:%d y:%d \n", i, cover_rAttr.mosaicAttr.x, cover_rAttr.mosaicAttr.y);
			ret = IMP_OSD_ShowRgn(prHander[2+RECT_INDEX+MOSAIC_INDEX+i], mosdgrp, 1);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
				return -1;
			}
			printf("grid %d On!\n", i);
		}
	}

	return 0;
}

int osd_grid_show(int selnum)
{
	int ret;

	if(selnum < 0 || selnum >= GRID_COVER_INDEX)
		printf("Invalid Grid Index : %d\n", selnum);

	IMPOSDRgnAttr cover_rAttr;
	if (grid_cover_flag[selnum] == true){
		IMP_OSD_GetRgnAttr(prHander[2+RECT_INDEX+MOSAIC_INDEX+selnum], &cover_rAttr);
		printf("grid[%d] x:%d y:%d \n", selnum, cover_rAttr.mosaicAttr.x , cover_rAttr.mosaicAttr.y);
		ret = IMP_OSD_ShowRgn(prHander[2+RECT_INDEX+MOSAIC_INDEX+selnum], mosdgrp, 1);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
			return -1;
		}
		printf("grid %d On!\n", selnum);
	}
	else {
		IMP_OSD_GetRgnAttr(prHander[2+RECT_INDEX+MOSAIC_INDEX+selnum], &cover_rAttr);
		printf("grid[%d] x:%d y:%d \n", selnum, cover_rAttr.mosaicAttr.x , cover_rAttr.mosaicAttr.y);
		ret = IMP_OSD_ShowRgn(prHander[2+RECT_INDEX+MOSAIC_INDEX+selnum], mosdgrp, 0);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
			return -1;
		}
		printf("grid %d Off!\n", selnum);
	}

	return 0;
}

int isd_crop(int x, int y, int w, int h, int cam) {
	IMPISPAutoZoom autozoom;
	int ret = -1;

	if (x < 0 || x > 1920) {
		printf("x range invaild!\n");
		return -1;
	}
	if (y < 0 || y > 1080) {
		printf("y range invaild!\n");
		return -1;
	}
	if (x+w < 0 || x+w > 1920) {
		printf("w range invaild!\n");
		return -1;
	}
	if (y+h < 0 || y+h > 1080) {
		printf("h range invaild!\n");
		return -1;
	}
	if (cam < 0 || cam > 1) {
		printf("Cam selecte error!\n");
		return -1;
	}

	memset(&autozoom, 0, sizeof(IMPISPAutoZoom));
	autozoom.zoom_chx_en[0] = 1;
	autozoom.zoom_left[0] = x;
	autozoom.zoom_top[0] = y;
	autozoom.zoom_width[0] = w;
	autozoom.zoom_height[0] = h;

	ret = IMP_ISP_Tuning_SetAutoZoom(IMPVI_MAIN+cam, &autozoom);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_ISP_Tuning_SetAutoZoom() error\n");
		return -1;
	}
	return 0;
}

int isd_distortion(int cx, int cy, int w, int h, int streng, int cam) {
	IMPISPHLDCAttr hldc;
	int ret = -1;

	if (cx < 0 || cx > 1920) {
		printf("x range invaild!\n");
		return -1;
	}
	if (cy < 0 || cy > 1080) {
		printf("y range invaild!\n");
		return -1;
	}
	// if (cx-(w/2) < 0 || cx+(w/2) > 1920) {
	// 	printf("w range invaild! %d %d\n", cx-(w/2), cx+(w/2));
	// 	return -1;
	// }
	// if (cy-(h/2) < 0 || cy+(h/2) > 1080) {
	// 	printf("h range invaild!\n");
	// 	return -1;
	// }
	if (streng < 0 || streng > 255) {
		printf("strength range invaild [0~255]!\n");
		return -1;
	}
	if (cam < 0 || cam > 1) {
		printf("Cam selecte error!\n");
		return -1;
	}
	
	hldc.strength = streng;     			/**< Distortion correction intensity [range: 0 to 255, default: 128]*/
    hldc.width = w;          		/**< Image width */
    hldc.height = h;         		/**< Image height */
    hldc.center_w = cx;       /**< Image distortion horizontal optical center range:[width/2-120, width/2+120] */
    hldc.center_h = cy;      /**< Image distortion vertical optical center range:[height/2-120, height/2+120] */

	ret = IMP_ISP_Tuning_SetHLDCAttr(IMPVI_MAIN+cam, &hldc);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_ISP_Tuning_SetHLDCAttr() error\n");
		return -1;
	}
	return 0;
}

int isp_filcker (int freq, int mode) {
	IMPISPAntiflickerAttr flickerAttr;

	memset(&flickerAttr, 0, sizeof(IMPISPAntiflickerAttr));
	flickerAttr.freq = freq;
	flickerAttr.mode = mode;
	IMP_ISP_Tuning_SetAntiFlickerAttr(IMPVI_MAIN, &flickerAttr);
	IMP_ISP_Tuning_SetAntiFlickerAttr(IMPVI_MAIN+1, &flickerAttr);
}

// int isp_wdr (int state, int cam) {
// 	int ret = -1;
// 	IMPISPTuningOpsMode mode;

// 	if (state = 0) {
// 		mode = IMPISP_TUNING_OPS_MODE_DISABLE;
// 	}
// 	else {
// 		mode = IMPISP_TUNING_OPS_MODE_ENABLE;
// 	}

// 	ret = IMP_ISP_WDR_ENABLE(IMPVI_MAIN+cam, &mode);
// 	if (ret != 0) {
// 		IMP_LOG_ERR(TAG, "IMP_ISP_WDR_ENABLE() error\n");
// 		return -1;
// 	}
// 	return 0;
// }

IMPCell osdcell;

int video_init(void) {
	int ret = -1;

	osdcell.deviceID = DEV_ID_OSD;
	osdcell.groupID = mosdgrp;
	osdcell.outputID = 0;

	/* Step.1 System init */
	ret = sample_system_init();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
		return -1;
	}

	/* Step.2 FrameSource init */
	ret = sample_framesource_init(CH0_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource CH0_INDEX init failed\n");
		return -1;
	}
	// ret = sample_framesource_init(CH1_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "FrameSource CH1_INDEX init failed\n");
	// 	return -1;
	// }
	ret = sample_framesource_init(CH2_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource CH2_INDEX init failed\n");
		return -1;
	}

	ret = sample_framesource_init(CH3_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource CH3_INDEX init failed\n");
		return -1;
	}
	// ret = sample_framesource_init(CH4_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "FrameSource CH4_INDEX init failed\n");
	// 	return -1;
	// }
	ret = sample_framesource_init(CH5_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource CH5_INDEX init failed\n");
		return -1;
	}

	/* Step.3 Encoder init */
	ret = IMP_Encoder_CreateGroup(chn[CH0_INDEX].index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", chn[CH0_INDEX].index);
		return -1;
	}
	ret = sample_encoder_init(CH0_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder init failed\n");
		return -1;
	}

	// ret = IMP_Encoder_CreateGroup(chn[CH1_INDEX].index);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", chn[CH1_INDEX].index);
	// 	return -1;
	// }
	// ret = sample_encoder_init(CH1_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "Encoder init failed\n");
	// 	return -1;
	// }

	ret = IMP_Encoder_CreateGroup(chn[CH2_INDEX].index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", chn[CH1_INDEX].index);
		return -1;
	}
	ret = sample_jpeg_init(CH2_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_jpeg_init init failed\n");
		return -1;
	}

	ret = IMP_Encoder_CreateGroup(chn[CH3_INDEX].index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", chn[CH3_INDEX].index);
		return -1;
	}
	ret = sample_encoder_init(CH3_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder init failed\n");
		return -1;
	}

	// ret = IMP_Encoder_CreateGroup(chn[CH4_INDEX].index);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", chn[CH4_INDEX].index);
	// 	return -1;
	// }
	// ret = sample_encoder_init(CH4_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "Encoder init failed\n");
	// 	return -1;
	// }

	ret = IMP_Encoder_CreateGroup(chn[CH5_INDEX].index);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_Encoder_CreateGroup(%d) error !\n", chn[CH5_INDEX].index);
		return -1;
	}
	ret = sample_jpeg_init(CH5_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_jpeg_init init failed\n");
		return -1;
	}


	// ret = sample_jpeg_init(CH2_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "Jpg Snap init failed\n");
	// 	return -1;
	// }

	// ret = sample_jpeg_init(CH5_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "Jpg Snap init failed\n");
	// 	return -1;
	// }

	/* Step.4 OSD init */
	if (IMP_OSD_CreateGroup(mosdgrp) < 0) {
		IMP_LOG_ERR(TAG, "IMP_OSD_CreateGroup(%d) error !\n", mosdgrp);
		return -1;
	}

	// prHander = sample_osd_init(mosdgrp);
	// if (prHander <= 0) {
	// 	IMP_LOG_ERR(TAG, "OSD init failed\n");
	// 	return -1;
	// }
	prHander = osd_init(mosdgrp);
	if (prHander <= 0) {
		IMP_LOG_ERR(TAG, "OSD init failed\n");
		return -1;
	}


	/* Step.5 Bind */
	
	ret = IMP_System_Bind(&chn[CH0_INDEX].framesource_chn, &osdcell);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind FrameSource channel0 and OSD failed\n");
		return -1;
	}

	ret = IMP_System_Bind(&osdcell, &chn[CH0_INDEX].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind OSD and Encoder failed\n");
		return -1;
	}
	// ret = IMP_System_Bind(&chn[CH0_INDEX].framesource_chn, &chn[CH0_INDEX].imp_encoder);
	// if (ret < 0) {
		// IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",CH0_INDEX);
		// return -1;
	// }



	// ret = IMP_System_Bind(&chn[CH1_INDEX].framesource_chn, &chn[CH1_INDEX].imp_encoder);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",CH1_INDEX);
	// 	return -1;
	// }
	ret = IMP_System_Bind(&chn[CH2_INDEX].framesource_chn, &chn[CH2_INDEX].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",CH1_INDEX);
		return -1;
	}

	ret = IMP_System_Bind(&chn[CH3_INDEX].framesource_chn, &chn[CH3_INDEX].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",CH3_INDEX);
		return -1;
	}
	// ret = IMP_System_Bind(&chn[CH4_INDEX].framesource_chn, &chn[CH4_INDEX].imp_encoder);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",CH4_INDEX);
	// 	return -1;
	// }
	ret = IMP_System_Bind(&chn[CH5_INDEX].framesource_chn, &chn[CH5_INDEX].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and Encoder failed\n",CH4_INDEX);
		return -1;
	}

	move_init(chn[CH0_INDEX].framesource_chn);

	/* Step.6 Stream On */
	ret = sample_framesource_streamon(CH0_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn CH0_INDEX failed\n");
		return -1;
	}
	// ret = sample_framesource_streamon(CH1_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "ImpStreamOn CH1_INDEX failed\n");
	// 	return -1;
	// }
	ret = sample_framesource_streamon(CH2_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn CH2_INDEX failed\n");
		return -1;
	}
	ret = sample_framesource_streamon(CH3_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn CH3_INDEX failed\n");
		return -1;
	}
	// ret = sample_framesource_streamon(CH4_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "ImpStreamOn CH4_INDEX failed\n");
	// 	return -1;
	// }
	ret = sample_framesource_streamon(CH5_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ImpStreamOn CH5_INDEX failed\n");
		return -1;
	}

	ret = IMP_ISP_EnableTuning();
    if(ret < 0){
        IMP_LOG_ERR(TAG, "IMP_ISP_EnableTuning failed\n");
        return -1;
    }

	// IMPOSDRgnAttr cover_rAttr;
	// for (int i = 0; i < GRID_COVER_INDEX; i++) {
	// 	IMP_OSD_GetRgnAttr(prHander[2+RECT_INDEX+MOSAIC_INDEX+i], &cover_rAttr);
	// 	printf("grid[%d] x:%d y:%d \n", i, cover_rAttr.mosaicAttr.x, cover_rAttr.mosaicAttr.y);
	// }

	IMPISPHVFLIP hvf;
	hvf = IMPISP_FLIP_HV_MODE;
	// IMP_ISP_Tuning_SetHVFLIP(IMPVI_MAIN, &hvf);		// Main Cam Flip
	// IMP_ISP_Tuning_SetHVFLIP(IMPVI_MAIN+1, &hvf);	// Box Cam Flip

	///////////////////////// Box Cam Crop ////////////////////////////
	// IMPISPAutoZoom autozoom;
	// memset(&autozoom, 0, sizeof(IMPISPAutoZoom));
	// autozoom.zoom_chx_en[0] = 1;
	// autozoom.zoom_left[0] = 640/2;
	// autozoom.zoom_top[0] = 360;
	// autozoom.zoom_width[0] = (int)(1920-(autozoom.zoom_left[0]*2));
	// autozoom.zoom_height[0] = (int)(1080-autozoom.zoom_top[0]);

	// ret = IMP_ISP_Tuning_SetAutoZoom(IMPVI_MAIN+1, &autozoom);
	///////////////////////////////////////////////////////////////////

	///////////////////////// Flicker ISP /////////////////////////////
	IMPISPAntiflickerAttr flickerAttr;
	memset(&flickerAttr, 0, sizeof(IMPISPAntiflickerAttr));
	flickerAttr.freq = 70;
	flickerAttr.mode = IMPISP_ANTIFLICKER_NORMAL_MODE;
	IMP_ISP_Tuning_SetAntiFlickerAttr(IMPVI_MAIN, &flickerAttr);
	IMP_ISP_Tuning_SetAntiFlickerAttr(IMPVI_MAIN+1, &flickerAttr);
	///////////////////////////////////////////////////////////////////

	///////////////////////////// LDC ISP /////////////////////////////
	// IMPISPHLDCAttr hldc;
	// hldc.strength = 150;     			/**< Distortion correction intensity [range: 0 to 255, default: 128]*/
    // hldc.width = 1920;          		/**< Image width */
    // hldc.height = 1080;         		/**< Image height */
    // hldc.center_w = hldc.width/2;       /**< Image distortion horizontal optical center range:[width/2-120, width/2+120] */
    // hldc.center_h = hldc.height/2;      /**< Image distortion vertical optical center range:[height/2-120, height/2+120] */

	// IMP_ISP_Tuning_SetHLDCAttr(IMPVI_MAIN, &hldc);
	///////////////////////////////////////////////////////////////////

	// if (Night_Mode) {
	// 	IMP_ISP_StartNightMode(IMPVI_MAIN);	// Noight Mode ?
	// 	IMP_ISP_StartNightMode(IMPVI_MAIN+1);	// Noight Mode ?
	// }

	return 0;
}

int video_deinit(void)
{
	int ret = -1;
	/* Exit sequence as follow */
	/* Step.a Stream Off */
	ret = sample_framesource_streamoff(CH0_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff CH0_INDEX failed\n");
		return -1;
	}
	// ret = sample_framesource_streamoff(CH1_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "FrameSource StreamOff CH1_INDEX failed\n");
	// 	return -1;
	// }
	ret = sample_framesource_streamoff(CH2_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff CH2_INDEX failed\n");
		return -1;
	}
	ret = sample_framesource_streamoff(CH3_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff CH3_INDEX failed\n");
		return -1;
	}
	// ret = sample_framesource_streamoff(CH4_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "FrameSource StreamOff CH4_INDEX failed\n");
	// 	return -1;
	// }
	ret = sample_framesource_streamoff(CH5_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource StreamOff CH5_INDEX failed\n");
		return -1;
	}

	/* Step.b UnBind */
	ret = IMP_System_UnBind(&osdcell, &chn[CH0_INDEX].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Bind OSD and Encoder failed\n");
		return -1;
	}

	ret = IMP_System_UnBind(&chn[CH0_INDEX].framesource_chn, &osdcell);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "UnBind FrameSource and OSD failed\n");
		return -1;
	}
	// ret = IMP_System_UnBind(&chn[CH0_INDEX].framesource_chn, &chn[CH0_INDEX].imp_encoder);
	// if (ret < 0) {
		// IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",CH0_INDEX);
		// return -1;
	// }


	// ret = IMP_System_UnBind(&chn[CH1_INDEX].framesource_chn, &chn[CH1_INDEX].imp_encoder);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",CH1_INDEX);
	// 	return -1;
	// }
	ret = IMP_System_UnBind(&chn[CH2_INDEX].framesource_chn, &chn[CH1_INDEX].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",CH2_INDEX);
		return -1;
	}
	
	ret = IMP_System_UnBind(&chn[CH3_INDEX].framesource_chn, &chn[CH3_INDEX].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",CH3_INDEX);
		return -1;
	}
	// ret = IMP_System_UnBind(&chn[CH4_INDEX].framesource_chn, &chn[CH4_INDEX].imp_encoder);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",CH4_INDEX);
	// 	return -1;
	// }
	ret = IMP_System_UnBind(&chn[CH5_INDEX].framesource_chn, &chn[CH1_INDEX].imp_encoder);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and Encoder failed\n",CH5_INDEX);
		return -1;
	}
	

	/* Step.c OSD exit */
	// ret = sample_osd_exit(prHander,mosdgrp);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "OSD exit failed\n");
	// 	return -1;
	// }
	ret = osd_exit(prHander,mosdgrp);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "OSD exit failed\n");
		return -1;
	}


	move_deinit(chn[CH0_INDEX].framesource_chn);

	/* Step.c Encoder exit */
	ret = sample_encoder_exit(CH0_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder exit CH0_INDEX failed\n");
		return -1;
	}
	// ret = sample_encoder_exit(CH1_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "Encoder exit CH1_INDEX failed\n");
	// 	return -1;
	// }
	ret = sample_jpeg_exit(CH2_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_jpeg_exit channel%d and Encoder failed\n",CH2_INDEX);
		return -1;
	}

	ret = sample_encoder_exit(CH3_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "Encoder exit CH3_INDEX failed\n");
		return -1;
	}
	// ret = sample_encoder_exit(CH4_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "Encoder exit CH4_INDEX failed\n");
	// 	return -1;
	// }
	ret = sample_jpeg_exit(CH5_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_jpeg_exit channel%d and Encoder failed\n",CH5_INDEX);
		return -1;
	}

	/* Step.d FrameSource exit */
	ret = sample_framesource_exit(CH0_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit CH0_INDEX failed\n");
		return -1;
	}
	// ret = sample_framesource_exit(CH1_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "FrameSource exit CH1_INDEX failed\n");
	// 	return -1;
	// }
	ret = sample_framesource_exit(CH2_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit CH2_INDEX failed\n");
		return -1;
	}
	ret = sample_framesource_exit(CH3_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit CH3_INDEX failed\n");
		return -1;
	}
	// ret = sample_framesource_exit(CH4_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "FrameSource exit CH4_INDEX failed\n");
	// 	return -1;
	// }
	ret = sample_framesource_exit(CH5_INDEX);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "FrameSource exit CH5_INDEX failed\n");
		return -1;
	}

	/* Step.e System exit */
	ret = sample_system_exit();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
		return -1;
	}

	return 0;
}

void *get_video_stream_user_thread(void *args)
{
	start_time = sample_gettimeus();
	sample_get_video_stream_user();

	return ((void*) 0);
}

void *get_video_clip_user_thread(void *args)
{
	sample_get_video_clip_user();

	return ((void*) 0);
}

void *get_snap_stream_user_thread(void *args)
{
	sample_get_snap_stream_user();

	return ((void*) 0);
}

void *move_detecte_thread(void *args)
{
	move();

	return ((void*) 0);
}

void *OSD_thread(void *args)
{
	int ret;

	int x_cal=0, y_cal=0, w_cal=0, h_cal=0;
	int index = 0;
	// int f_cnt=0;
	
	// int mosaic_x = 300, mosaic_y = 300;
	// int state = 0;

	// IMPOSDRgnAttr rect_rAttr;
	// IMPOSDRgnAttr cover_rAttr;
	IMPOSDRgnAttr mosaic_rAttr;
	// int mosaic_index = 0;
	uint64_t mosaic_time[10] = {0};
	uint64_t mtime = 0;
	// uint64_t test_time = 0;
	// IMPOSDGrpRgnAttr gAPMosaic;

	ret = osd_show();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "OSD show error\n");
		return NULL;
	}
	
	do {
		if (main_motion_detect > 0) {
			ret = IMP_OSD_ShowRgn(prHander[TEST_COVER_INDEX], mosdgrp, 1);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
				return NULL;
			}
		}
		else {
			ret = IMP_OSD_ShowRgn(prHander[TEST_COVER_INDEX], mosdgrp, 0);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
				return NULL;
			}
		}

		if (fdpd_ck) {
			fdpd_ck = false;
			for (int j=0; j<10; j++) {
				if (fdpd_data[j].flag && fdpd_data[j].classid == 0 && fdpd_data[j].trackid >= 0) {
				// if (fdpd_data[j].flag && fdpd_data[j].classid == 0) {
					index = fdpd_data[j].trackid % 10;
					// x_cal = fdpd_data[j].ul_x - 40;
					// x_cal = (x_cal/10)*10;
					if (fdpd_data[j].ul_x > 40)
						x_cal = fdpd_data[j].ul_x - 40;
					else
						x_cal = fdpd_data[j].ul_x;
					x_cal = x_cal - (x_cal%10);

					// y_cal = fdpd_data[j].ul_y - 40;
					// y_cal = (y_cal/10)*10;
					if (fdpd_data[j].ul_y > 40)
						y_cal = fdpd_data[j].ul_y - 40;
					else
						y_cal = fdpd_data[j].ul_y;
					y_cal = y_cal - (y_cal%10);

					w_cal = fdpd_data[j].br_x - fdpd_data[j].ul_x;
					h_cal = fdpd_data[j].br_y - fdpd_data[j].ul_y;
					if (w_cal < 40 || h_cal < 40) {
						continue;
					}
					else {
						w_cal = w_cal - (w_cal%40) + 80;
						h_cal = h_cal - (h_cal%40) + 80;
					}
					if(x_cal + w_cal >= 1920) x_cal = 1920 - w_cal - 1;
					if(y_cal + h_cal >= 1080) y_cal = 1080 - h_cal - 1;

					mosaic_rAttr.type = OSD_REG_MOSAIC;
					mosaic_rAttr.mosaicAttr.x = x_cal;
					mosaic_rAttr.mosaicAttr.y = y_cal;
					mosaic_rAttr.mosaicAttr.mosaic_width = w_cal;
					mosaic_rAttr.mosaicAttr.mosaic_height = h_cal;
					mosaic_rAttr.mosaicAttr.frame_width = FIRST_SENSOR_WIDTH;
					mosaic_rAttr.mosaicAttr.frame_height = FIRST_SENSOR_HEIGHT;
					mosaic_rAttr.mosaicAttr.mosaic_min_size = 40;

					// printf("mosaic[%d] x:%d y:%d w:%d h:%d\n", j, x_cal, y_cal, w_cal, h_cal);

					ret = IMP_OSD_SetRgnAttr(prHander[2+RECT_INDEX+index], &mosaic_rAttr);
					if (ret != 0) {
						IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Mosaic error\n");
						return NULL;
					}
					if (mosaic_time[index] == 0) {
						ret = IMP_OSD_ShowRgn(prHander[2+RECT_INDEX+index], mosdgrp, 1);
						if (ret != 0) {
							IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Mosaic error\n");
							return NULL;
						}
					}

					fdpd_data[j].flag = false;
					mosaic_time[index] = sample_gettimeus();
					// printf("Face Mosaic[%d] En tiem : %lld\n", j, mosaic_time[j]);
				}
			}
		}

		for (int k=0; k<10; k++) {
			if (mosaic_time[k] != 0) {
				mtime = sample_gettimeus()-mosaic_time[k];
				if (mtime > 10000000) {
					ret = IMP_OSD_ShowRgn(prHander[2+RECT_INDEX+k], mosdgrp, 0);
					if (ret != 0) {
						IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Mosaic error\n");
						return NULL;
					}
					printf("Face Mosaic[%d] De tiem : %lld\n", k, mosaic_time[k]);
					mosaic_time[k] = 0;
				}
			}
		}
		// usleep(5*1000);
	} while(!bExit);

	return ((void*) 0);
}

