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
#include "gpio.h"
#include "setting.h"
#include "c_util.h"

#define TAG "video"

extern struct chn_conf chn[];

int mosdgrp = 0;
int bosdgrp = 0;
IMPRgnHandle *prHander;

static int osd_show(void)
{
	int ret, i;

	// if (move_flag) {
	// 	IMPOSDRgnAttr rect_rAttr;
	// 	rect_rAttr.type = OSD_REG_RECT;
	// 	rect_rAttr.rect.p0.x = move_det_xs;
	// 	rect_rAttr.rect.p0.y = move_det_ys;
	// 	rect_rAttr.rect.p1.x = move_det_xe - 1;
	// 	rect_rAttr.rect.p1.y = move_det_ye - 1;
	// 	rect_rAttr.fmt = PIX_FMT_MONOWHITE;
	// 	rect_rAttr.data.lineRectData.color = OSD_RED;
	// 	rect_rAttr.data.lineRectData.linewidth = 3;
	// 	IMP_OSD_SetRgnAttr(prHander[TEST_RECT_INDEX], &rect_rAttr);
	// 	ret = IMP_OSD_ShowRgn(prHander[TEST_RECT_INDEX], mosdgrp, 1);
	// 	if (ret != 0) {
	// 		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
	// 		return -1;
	// 	}

	// 	ret = IMP_OSD_ShowRgn(prHander[TEST_COVER_INDEX], mosdgrp, 0);
	// 	if (ret != 0) {
	// 		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Test Cover error\n");
	// 		return -1;
	// 	}
	// }

	// for (i=0; i<27; i++) {
	// 	dp("grid[%d]:0x%02x\n", i, settings.user_grid[i]);
	// }


	IMPOSDRgnAttr cover_rAttr;
	if (settings.SF.bits.door_g) {
		for (i=0; i<GRID_COVER_INDEX; i++) {
			if (settings.door_grid[i/8]&(0x01 << (7-(i%8)))) {
			// if (grid_cover_flag[i] == true){
				IMP_OSD_GetRgnAttr(prHander[2+RECT_INDEX+MOSAIC_INDEX+i], &cover_rAttr);
				// dp("grid[%d] x:%d y:%d \n", i, cover_rAttr.mosaicAttr.x, cover_rAttr.mosaicAttr.y);
				ret = IMP_OSD_ShowRgn(prHander[2+RECT_INDEX+MOSAIC_INDEX+i], mosdgrp, 1);
				if (ret != 0) {
					IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
					return -1;
				}
				// dp("grid %d On!\n", i);
			}
		}
	}

	if (settings.SF.bits.user_g) {
		for (i=0; i<GRID_COVER_INDEX; i++) {
			if (settings.user_grid[i/8]&(0x01 << (7-(i%8)))) {
			// if (grid_cover_flag[i] == true){
				IMP_OSD_GetRgnAttr(prHander[2+RECT_INDEX+MOSAIC_INDEX+i], &cover_rAttr);
				// dp("grid[%d] x:%d y:%d \n", i, cover_rAttr.mosaicAttr.x, cover_rAttr.mosaicAttr.y);
				ret = IMP_OSD_ShowRgn(prHander[2+RECT_INDEX+MOSAIC_INDEX+i], mosdgrp, 1);
				if (ret != 0) {
					IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
					return -1;
				}
				// dp("grid %d On!\n", i);
			}
		}
	}

	return 0;
}

int osd_grid_show(int selnum)
{
	int ret;

	if(selnum < 0 || selnum >= GRID_COVER_INDEX)
		dp("Invalid Grid Index : %d\n", selnum);

	IMPOSDRgnAttr cover_rAttr;
	if (grid_cover_flag[selnum] == true){
		IMP_OSD_GetRgnAttr(prHander[2+RECT_INDEX+MOSAIC_INDEX+selnum], &cover_rAttr);
		dp("grid[%d] x:%d y:%d \n", selnum, cover_rAttr.mosaicAttr.x , cover_rAttr.mosaicAttr.y);
		ret = IMP_OSD_ShowRgn(prHander[2+RECT_INDEX+MOSAIC_INDEX+selnum], mosdgrp, 1);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
			return -1;
		}
		dp("grid %d On!\n", selnum);
	}
	else {
		IMP_OSD_GetRgnAttr(prHander[2+RECT_INDEX+MOSAIC_INDEX+selnum], &cover_rAttr);
		dp("grid[%d] x:%d y:%d \n", selnum, cover_rAttr.mosaicAttr.x , cover_rAttr.mosaicAttr.y);
		ret = IMP_OSD_ShowRgn(prHander[2+RECT_INDEX+MOSAIC_INDEX+selnum], mosdgrp, 0);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
			return -1;
		}
		dp("grid %d Off!\n", selnum);
	}

	return 0;
}

int isd_crop(int x, int y, int w, int h, int cam) {
	IMPISPAutoZoom autozoom;
	int ret = -1;

	if (x < 0 || x > 1920) {
		dp("x range invaild!\n");
		return -1;
	}
	if (y < 0 || y > 1080) {
		dp("y range invaild!\n");
		return -1;
	}
	if (x+w < 0 || x+w > 1920) {
		dp("w range invaild!\n");
		return -1;
	}
	if (y+h < 0 || y+h > 1080) {
		dp("h range invaild!\n");
		return -1;
	}
	if (cam < 0 || cam > 1) {
		dp("Cam selecte error!\n");
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

	IMPISPHLDCAttr hldc;
	if (cam == 0)
		hldc.strength = 70;     			/**< Distortion correction intensity [range: 0 to 255, default: 128]*/
	else
		hldc.strength = 128;     			/**< Distortion correction intensity [range: 0 to 255, default: 128]*/
    hldc.width = w;          		/**< Image width */
    hldc.height = h;         		/**< Image height */
    hldc.center_w = x+(w/2);       /**< Image distortion horizontal optical center range:[width/2-120, width/2+120] */
    hldc.center_h = y+(h/2);      /**< Image distortion vertical optical center range:[height/2-120, height/2+120] */

	IMP_ISP_Tuning_SetHLDCAttr(IMPVI_MAIN+cam, &hldc);
	return 0;
}

int isd_distortion(int cx, int cy, int w, int h, int streng, int cam) {
	IMPISPHLDCAttr hldc;
	int ret = -1;

	if (cx < 0 || cx > 1920) {
		dp("x range invaild!\n");
		return -1;
	}
	if (cy < 0 || cy > 1080) {
		dp("y range invaild!\n");
		return -1;
	}
	// if (cx-(w/2) < 0 || cx+(w/2) > 1920) {
	// 	dp("w range invaild! %d %d\n", cx-(w/2), cx+(w/2));
	// 	return -1;
	// }
	// if (cy-(h/2) < 0 || cy+(h/2) > 1080) {
	// 	dp("h range invaild!\n");
	// 	return -1;
	// }
	if (streng < 0 || streng > 255) {
		dp("strength range invaild [0~255]!\n");
		return -1;
	}
	if (cam < 0 || cam > 1) {
		dp("Cam selecte error!\n");
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

	return 0;
}

int isp_filcker_get (void) {
	IMPISPAntiflickerAttr flickerAttr;

	memset(&flickerAttr, 0, sizeof(IMPISPAntiflickerAttr));
	// flickerAttr.freq = freq;
	// flickerAttr.mode = mode;
	IMP_ISP_Tuning_GetAntiFlickerAttr(IMPVI_MAIN, &flickerAttr);
	dp("flicker main mode : %d freq : %d\n", flickerAttr.mode, flickerAttr.freq);
	memset(&flickerAttr, 0, sizeof(IMPISPAntiflickerAttr));
	IMP_ISP_Tuning_GetAntiFlickerAttr(IMPVI_MAIN+1, &flickerAttr);
	dp("flicker box mode : %d freq : %d\n", flickerAttr.mode, flickerAttr.freq);

	return 0;
}

uint32_t init_igtime = 0;

uint32_t isp_integration_time(int getset, uint32_t value) {
	IMPISPAEExprInfo expose_inf;
	IMP_ISP_Tuning_GetAeExprInfo(IMPVI_MAIN, &expose_inf);
	dp("Expose mode:%d Value:%d\n", expose_inf.AeMode, expose_inf.ExposureValue);
	dp("igtime:%d again:%d dgain:%d idgain%d totaldb:%d\n", expose_inf.AeIntegrationTime, expose_inf.AeAGain, expose_inf.AeDGain, expose_inf.AeIspDGain, expose_inf.TotalGainDb);
	dp("igmode:%d amode:%d dmode:%d\n", expose_inf.AeIntegrationTimeMode, expose_inf.AeAGainManualMode, expose_inf.AeDGainManualMode);
	dp("tgain:%d tgainshort:%d \n", expose_inf.TotalGainDb, expose_inf.TotalGainDbShort);

	if (getset > 0) {
		if (value == 0) {
			expose_inf.AeMode = IMPISP_TUNING_OPS_TYPE_AUTO;
			expose_inf.AeIntegrationTimeMode = IMPISP_TUNING_OPS_TYPE_AUTO;
			expose_inf.AeIntegrationTime = init_igtime;
		}
		else {
			expose_inf.AeMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
			expose_inf.AeIntegrationTimeMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
			expose_inf.AeIntegrationTime = value;
		}

		IMP_ISP_Tuning_SetAeExprInfo(IMPVI_MAIN, &expose_inf);
		IMP_ISP_Tuning_SetAeExprInfo(IMPVI_MAIN+1, &expose_inf);

	// expose_inf.AeAGainManualMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
	// expose_inf.AeAGain = 1;
	// expose_inf.AeDGainManualMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
	// expose_inf.AeDGain = 1;
	// expose_inf.AeIspDGainManualMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
	// expose_inf.AeIspDGain = 100;
	

	IMP_ISP_Tuning_SetAeExprInfo(IMPVI_MAIN, &expose_inf);
	}
	else {
		dp("min : %d %d %d\n", expose_inf.AeMinIntegrationTime, expose_inf.AeMinAGain, expose_inf.AeMinDgain);
		dp("max : %d %d %d\n", expose_inf.AeMaxIntegrationTime, expose_inf.AeMaxAGain, expose_inf.AeMaxDgain);
	}
	return expose_inf.AeMinIntegrationTime;
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

uint8_t SceneceSet(int getset, uint8_t val) {
	IMPISPAEScenceAttr sceneattr;

	IMP_ISP_Tuning_GetAeScenceAttr(IMPVI_MAIN, &sceneattr);
	dp("Sceneattr get :\n");
	dp("	HLC EN:%d HLC ST:%d\n", sceneattr.AeBLCEn, sceneattr.AeBLCStrength);
	dp("	BLC EN:%d BLC ST:%d\n", sceneattr.AeHLCEn, sceneattr.AeHLCStrength);
	dp("	TAR EN:%d TAR CP:%d\n", sceneattr.AeTargetCompEn, sceneattr.AeTargetComp);
	dp("	STA EN:%d STA Ev:%d\n", sceneattr.AeStartEn ,sceneattr.AeStartEv);
	dp("	luma  :%d lm sce:%d\n", sceneattr.luma ,sceneattr.luma_scence);
	dp("	stable:%d target:%d aemean:%d\n", sceneattr.stable , sceneattr.target , sceneattr.ae_mean);
	if (getset == 10) {
		if (val == 0) {
			sceneattr.AeBLCEn = TISP_AE_SCENCE_GLOBAL_ENABLE;
			sceneattr.AeBLCStrength = 0;
			sceneattr.AeHLCEn = TISP_AE_SCENCE_GLOBAL_ENABLE;
			sceneattr.AeHLCStrength = 0;
			sceneattr.AeTargetCompEn = IMP_ISP_AE_SCENCE_DISABLE;
			sceneattr.AeTargetComp = 0;
			sceneattr.AeStartEn = IMP_ISP_AE_SCENCE_DISABLE;
			sceneattr.AeStartEv = 0;
		}
	}
	else if (getset == 1) {
			sceneattr.AeBLCEn = TISP_AE_SCENCE_GLOBAL_ENABLE;
			sceneattr.AeBLCStrength = val;
	}
	else if (getset == 2) {
			sceneattr.AeHLCEn = TISP_AE_SCENCE_GLOBAL_ENABLE;
			sceneattr.AeHLCStrength = val;
	}
	else if (getset == 3) {
			sceneattr.AeTargetCompEn = TISP_AE_SCENCE_GLOBAL_ENABLE;
			sceneattr.AeTargetComp = val;
	}
	else if (getset == 4) {
			sceneattr.AeStartEn = TISP_AE_SCENCE_GLOBAL_ENABLE;
			sceneattr.AeStartEv = val;
	}

	if (getset > 0) {
		dp("Sceneattr set : BackLight EN:%d Backlight Str:%d\n", sceneattr.AeBLCEn, sceneattr.AeBLCStrength);
		IMP_ISP_Tuning_SetAeScenceAttr(IMPVI_MAIN, &sceneattr);
	}
	return val;
}

uint8_t BLC_User(void) {
	IMPISPAEScenceAttr sceneattr;
	IMPISPAEExprInfo expose_inf;
	uint32_t ae_mean = 0;
	static uint32_t AeIntegrationTime = 0;
	static bool auto_flag = true;

	IMP_ISP_Tuning_GetAeScenceAttr(IMPVI_MAIN, &sceneattr);
	IMP_ISP_Tuning_GetAeExprInfo(IMPVI_MAIN, &expose_inf);

	ae_mean = sceneattr.ae_mean;
	// dp("Sceneattr get aemean:%d\n", sceneattr.ae_mean);
	// dp("aeintergrationtime : %d\n", expose_inf.AeIntegrationTime);

	if (expose_inf.AeMode == IMPISP_TUNING_OPS_TYPE_AUTO) {
		AeIntegrationTime = expose_inf.AeIntegrationTime;
		auto_flag = false;
	}
	else {
		auto_flag = true;
	}

	if (ae_mean > 120) {
		if (ae_mean > 220){
			AeIntegrationTime = AeIntegrationTime/2;
		}
		else if (ae_mean > 160){
			AeIntegrationTime -= 10;
		}
		else{
			AeIntegrationTime -= 2;
		}
		expose_inf.AeMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
		expose_inf.AeIntegrationTimeMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
		expose_inf.AeIntegrationTime = AeIntegrationTime;
		IMP_ISP_Tuning_SetAeExprInfo(IMPVI_MAIN, &expose_inf);
		IMP_ISP_Tuning_SetAeExprInfo(IMPVI_MAIN+1, &expose_inf);
		
		dp("Set Value : %d %d\n", AeIntegrationTime, ae_mean);
	}
	else if (ae_mean < 30 && auto_flag) {
		expose_inf.AeMode = IMPISP_TUNING_OPS_TYPE_AUTO;
		expose_inf.AeIntegrationTimeMode = IMPISP_TUNING_OPS_TYPE_AUTO;
		// expose_inf.AeIntegrationTime = AeIntegrationTime = 120;
		IMP_ISP_Tuning_SetAeExprInfo(IMPVI_MAIN, &expose_inf);
		IMP_ISP_Tuning_SetAeExprInfo(IMPVI_MAIN+1, &expose_inf);
		dp("Auto Set! : %d\n", ae_mean);
	}

	return AeIntegrationTime;
}

uint8_t cam_mean(void) {
	IMPISPAEScenceAttr sceneattr;
	// IMPISPAEExprInfo expose_inf;
	uint32_t ae_mean1 = 0, ae_mean2 = 0;

	IMP_ISP_Tuning_GetAeScenceAttr(IMPVI_MAIN, &sceneattr);
	// IMP_ISP_Tuning_GetAeExprInfo(IMPVI_MAIN, &expose_inf);
	ae_mean1 = sceneattr.ae_mean;

	IMP_ISP_Tuning_GetAeScenceAttr(IMPVI_MAIN+1, &sceneattr);

	ae_mean2 = sceneattr.ae_mean;
	// dp("Sceneattr get aemean:%d\n", sceneattr.ae_mean);

	dp("[mean]cam1:%d cam2:%d\n", ae_mean1, ae_mean2);

	return 1;
}

int Set_Target_Bit(uint32_t targetbit) {
	IMPEncoderAttrRcMode encecmode;

	IMP_Encoder_GetChnAttrRcMode(CH0_INDEX, &encecmode);
	dp("Before uTargetBitRate:%d\n", encecmode.attrCbr.uTargetBitRate);
	encecmode.attrCbr.uTargetBitRate = targetbit;
	IMP_Encoder_SetChnAttrRcMode(CH0_INDEX, &encecmode);

	IMP_Encoder_GetChnAttrRcMode(CH0_INDEX, &encecmode);
	dp("After uTargetBitRate:%d\n", encecmode.attrCbr.uTargetBitRate);

	// dp("enc rc mode:%d\n", encecmode.rcMode);
	// dp("enc rc CBR uTargetBitRate:%d\n", encecmode.attrCbr.uTargetBitRate);
	// dp("enc rc CBR iInitialQP:%d\n", encecmode.attrCbr.iInitialQP);
	// dp("enc rc CBR iMinQP:%d\n", encecmode.attrCbr.iMinQP);
	// dp("enc rc CBR iMaxQP:%d\n", encecmode.attrCbr.iMaxQP);
	// dp("enc rc CBR iIPDelta:%d\n", encecmode.attrCbr.iIPDelta);
	// dp("enc rc CBR iPBDelta:%d\n", encecmode.attrCbr.iPBDelta);
	// dp("enc rc CBR eRcOptions:%d\n", encecmode.attrCbr.eRcOptions);
	// dp("enc rc CBR uMaxPictureSize:%d\n", encecmode.attrCbr.uMaxPictureSize);

	return 0;
}

int Set_Target_Bit2(uint32_t targetbit) {
	IMPEncoderAttrRcMode encecmode;

	IMP_Encoder_GetChnAttrRcMode(CH0_INDEX, &encecmode);
	// dp("Before uTargetBitRate:%d\n", encecmode.attrCbr.uTargetBitRate);
	encecmode.attrCbr.uTargetBitRate = targetbit;
	IMP_Encoder_SetChnAttrRcMode(CH0_INDEX, &encecmode);
	IMP_Encoder_GetChnAttrRcMode(CH0_INDEX, &encecmode);

	// dp("enc rc mode:%d\n", encecmode.rcMode);
	// dp("enc rc CBR uTargetBitRate:%d\n", encecmode.attrCbr.uTargetBitRate);
	// dp("enc rc CBR iInitialQP:%d\n", encecmode.attrCbr.iInitialQP);
	// dp("enc rc CBR iMinQP:%d\n", encecmode.attrCbr.iMinQP);
	// dp("enc rc CBR iMaxQP:%d\n", encecmode.attrCbr.iMaxQP);
	// dp("enc rc CBR iIPDelta:%d\n", encecmode.attrCbr.iIPDelta);
	// dp("enc rc CBR iPBDelta:%d\n", encecmode.attrCbr.iPBDelta);
	// dp("enc rc CBR eRcOptions:%d\n", encecmode.attrCbr.eRcOptions);
	// dp("enc rc CBR uMaxPictureSize:%d\n", encecmode.attrCbr.uMaxPictureSize);

	return 0;
}

uint8_t bright_set(uint8_t bright) {
	uint8_t mbright, bbright;

	IMP_ISP_Tuning_GetBrightness(IMPVI_MAIN, &mbright);
	IMP_ISP_Tuning_GetBrightness(IMPVI_MAIN+1, &bbright);

	dp("Get Bright : %d %d\n", mbright, bbright);

	if (bright != 0) {
		mbright = bright;
		bbright = bright;

		IMP_ISP_Tuning_SetBrightness(IMPVI_MAIN, &mbright);
		IMP_ISP_Tuning_SetBrightness(IMPVI_MAIN+1, &bbright);

		dp("Set Bright : %d\n", mbright);
	}

	return mbright;
}


uint32_t AeIntegrationTime_1;

uint32_t Get_Brightness(void) {
	IMPISPAEScenceAttr sceneattr;
	IMPISPAEExprInfo expose_inf;
	uint8_t ae_mean = 0;
	uint32_t expose_val = 0;
	uint32_t AeIntegrationTime = AeIntegrationTime_1;

	// expose_inf.AeMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
	// expose_inf.AeIntegrationTimeMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
	// expose_inf.AeIntegrationTime = AeIntegrationTime;
	// IMP_ISP_Tuning_SetAeExprInfo(IMPVI_MAIN, &expose_inf);

	IMP_ISP_Tuning_GetAeExprInfo(IMPVI_MAIN, &expose_inf);
	expose_val = expose_inf.ExposureValue;
	dp("Expose Value:%d\n", expose_val);
	IMP_ISP_Tuning_GetBrightness(IMPVI_MAIN, &ae_mean);
	dp("mean Value:%d\n", ae_mean);

	return expose_val;
}

IMPCell osdcell;

int video_init(void) {
	int ret = -1;

	osdcell.deviceID = DEV_ID_OSD;
	osdcell.groupID = mosdgrp;
	osdcell.outputID = 0;

	// dp("HEVC:0x%02x\n", IMP_ENC_PROFILE_AVC_HIGH);

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

	// ret = sample_framesource_i2dopr(CH3_INDEX);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "sample_framesource_i2dopr(%d) error !\n", chn[CH3_INDEX].index);
	// 	return -1;
	// }

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
	// 	dp("grid[%d] x:%d y:%d \n", i, cover_rAttr.mosaicAttr.x, cover_rAttr.mosaicAttr.y);
	// }

	IMPISPHVFLIP hvf;
	hvf = IMPISP_FLIP_HV_MODE;
	IMP_ISP_Tuning_SetHVFLIP(IMPVI_MAIN, &hvf);		// Main Cam Flip
	// hvf = IMPISP_FLIP_SHIV_MODE;
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
	IMPISPHLDCAttr hldc;
	hldc.strength = 180;     			/**< Distortion correction intensity [range: 0 to 255, default: 128]*/
    hldc.width = 1920;          		/**< Image width */
    hldc.height = 1080;         		/**< Image height */
    hldc.center_w = hldc.width/2;       /**< Image distortion horizontal optical center range:[width/2-120, width/2+120] */
    hldc.center_h = hldc.height/2;      /**< Image distortion vertical optical center range:[height/2-120, height/2+120] */

	IMP_ISP_Tuning_SetHLDCAttr(IMPVI_MAIN, &hldc);
	///////////////////////////////////////////////////////////////////

	///////////////////////////// Night Mode //////////////////////////
	// if (Night_Mode) {
	// 	IMP_ISP_StartNightMode(IMPVI_MAIN);	// Night Mode ?
	// 	IMP_ISP_StartNightMode(IMPVI_MAIN+1);	// Night Mode ?
	// }
	///////////////////////////////////////////////////////////////////

	///////////////////////////// Expose Change ///////////////////////
	IMPISPAEExprInfo expose_inf;
	IMP_ISP_Tuning_GetAeExprInfo(IMPVI_MAIN, &expose_inf);
	// dp("Expose mode:%d Value:%d\n", expose_inf.AeMode, expose_inf.ExposureValue);
	// dp("igtime:%d again:%d dgain:%d idgain:%d\n", expose_inf.AeIntegrationTime, expose_inf.AeAGain, expose_inf.AeDGain, expose_inf.AeIspDGain);
	// dp("igmode:%d amode:%d dmode:%d\n", expose_inf.AeIntegrationTimeMode, expose_inf.AeAGainManualMode, expose_inf.AeDGainManualMode);

	ExpVal = expose_inf.ExposureValue;
	uint32_t AeIntegrationTime_1 = expose_inf.AeIntegrationTime;

	dp("AeIntegrationTime : %d\n", AeIntegrationTime_1);

	if (settings.SF.bits.flicker) {
		dp("Flicker Mode : On\n");
		// init_igtime = expose_inf.AeIntegrationTime;
		expose_inf.AeMinIntegrationTimeMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
		expose_inf.AeMinIntegrationTime = 250;
		// expose_inf.AeMinAGainMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
		// expose_inf.AeMinAGain = 0;
		// expose_inf.AeMinDgainMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
		// expose_inf.AeMinDgain = 0;

		// expose_inf.AeMode = IMPISP_TUNING_OPS_TYPE_AUTO;
		// expose_inf.AeIntegrationTimeMode = ISP_CORE_EXPR_UNIT_US;
		// expose_inf.AeIntegrationTime = 159;
		// expose_inf.AeAGainManualMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
		// expose_inf.AeAGain = 1;
		// expose_inf.AeDGainManualMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
		// expose_inf.AeDGain = 1;
		// expose_inf.AeIspDGainManualMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
		// expose_inf.AeIspDGain = 100;

		// expose_inf.AeMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
		// expose_inf.AeIntegrationTimeMode = IMPISP_TUNING_OPS_TYPE_MANUAL;
		// expose_inf.AeIntegrationTime = 90;

		IMP_ISP_Tuning_SetAeExprInfo(IMPVI_MAIN, &expose_inf);
	}
	else {
		dp("Flicker Mode : Off\n");
	}


	// IMPISPTuningOpsMode mode = IMPISP_TUNING_OPS_MODE_ENABLE;

	// IMP_ISP_WDR_ENABLE(IMPVI_MAIN, &mode);

	// IMPISPWdrOutputMode wdroutmode = IMPISP_WDR_OUTPUT_MODE_SHORT_FRAME;

	// IMP_ISP_Tuning_SetWdrOutputMode(IMPVI_MAIN, &wdroutmode);
	// IMP_ISP_Tuning_GetWdrOutputMode(IMPVI_MAIN, &wdroutmode);
	// dp("outputmode:%d\n", wdroutmode);

	// IMPISPWBAttr awbattr;

	// IMP_ISP_Tuning_GetAwbAttr(IMPVI_MAIN, &awbattr);

	// awbattr.mode = ISP_CORE_WB_MODE_DAY_LIGHT;
	// // awbattr.gain_val.rgain = 500;
	// // awbattr.gain_val.bgain = 500;

	// IMP_ISP_Tuning_SetAwbAttr(IMPVI_MAIN, &awbattr);

	// IMP_ISP_Tuning_GetAwbAttr(IMPVI_MAIN, &awbattr);

	// dp("mode:%d ct:%d ", awbattr.mode, awbattr.ct);
	// dp("r:%d b:%d\n",
	// 				awbattr.gain_val.rgain, awbattr.gain_val.bgain);

	IMPISPAEScenceAttr sceneattr;

	IMP_ISP_Tuning_GetAeScenceAttr(IMPVI_MAIN, &sceneattr);

	// dp("Sceneattr : BackLight EN:%d Backlight Str:%d\n", sceneattr.AeBLCEn, sceneattr.AeBLCStrength);

	// sceneattr.AeBLCEn = TISP_AE_SCENCE_ROI_ENABLE;
	// sceneattr.AeBLCStrength = 8;

	sceneattr.AeBLCEn = TISP_AE_SCENCE_GLOBAL_ENABLE;
	sceneattr.AeBLCStrength = 10;
	sceneattr.AeHLCEn = TISP_AE_SCENCE_GLOBAL_ENABLE;
	sceneattr.AeHLCStrength = 10;

	ret = IMP_ISP_Tuning_SetAeScenceAttr(IMPVI_MAIN, &sceneattr);
	if (ret < 0) {
		dp("ScenceAttr Set Fail!\n");
	}

	// IMPEncoderAttrRcMode encecmode;

	// IMP_Encoder_GetChnAttrRcMode(CH0_INDEX, &encecmode);
	// dp("enc rc mode:%d\n", encecmode.rcMode);
	// dp("enc rc CBR uTargetBitRate:%d\n", encecmode.attrCbr.uTargetBitRate);
	// dp("enc rc CBR iInitialQP:%d\n", encecmode.attrCbr.iInitialQP);
	// dp("enc rc CBR iMinQP:%d\n", encecmode.attrCbr.iMinQP);
	// dp("enc rc CBR iMaxQP:%d\n", encecmode.attrCbr.iMaxQP);
	// dp("enc rc CBR iIPDelta:%d\n", encecmode.attrCbr.iIPDelta);
	// dp("enc rc CBR iPBDelta:%d\n", encecmode.attrCbr.iPBDelta);
	// dp("enc rc CBR eRcOptions:%d\n", encecmode.attrCbr.eRcOptions);
	// dp("enc rc CBR uMaxPictureSize:%d\n", encecmode.attrCbr.uMaxPictureSize);


	// IMP_Encoder_SetChnAttrRcMode(CH0_INDEX, encecmode);

	// uint32_t fps_num, fps_den;
	// IMP_ISP_Tuning_GetSensorFPS(IMPVI_MAIN,&fps_num, &fps_den);

	// dp("1 fps_num:%d fps_den:%d\n", fps_num, fps_den);

	// fps_num = 60;
	// fps_den = 2;
	// IMP_ISP_Tuning_SetSensorFPS(IMPVI_MAIN,&fps_num, &fps_den);

	// IMP_ISP_Tuning_GetSensorFPS(IMPVI_MAIN,&fps_num, &fps_den);

	// dp("2 fps_num:%d fps_den:%d\n", fps_num, fps_den);

	// Mosaic_En = settings.SF.bits.per_face;

	// Set_Target_Bit(500);
	///////////////////////////////////////////////////////////////////

	uint8_t mbright, bbright;

	IMP_ISP_Tuning_GetBrightness(IMPVI_MAIN, &mbright);
	IMP_ISP_Tuning_GetBrightness(IMPVI_MAIN+1, &bbright);

	dp("Main Bright : %d %d\n", mbright, bbright);

	// mbright += 20;
	// bbright += 20;

	// IMP_ISP_Tuning_SetBrightness(IMPVI_MAIN, &mbright);
	// IMP_ISP_Tuning_SetBrightness(IMPVI_MAIN+1, &bbright);

	// uint8_t contrast = 110;

	// IMP_ISP_Tuning_SetContrast(IMPVI_MAIN, &contrast);
	// IMP_ISP_Tuning_SetContrast(IMPVI_MAIN+1, &contrast);

	// uint8_t saturation = 200;

	// IMP_ISP_Tuning_SetBcshHue(IMPVI_MAIN, &saturation);
	// IMP_ISP_Tuning_SetBcshHue(IMPVI_MAIN+1, &saturation);

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

#ifdef __TEST_FAKE_VEDIO__

void *get_video_stream_test_thread(void *args)
{
	extern pthread_mutex_t buffMutex_vm;
	start_time = sample_gettimeus();
	// sample_get_video_stream_test();
	uint8_t *test_buf=NULL;
	int ret = -1;

	test_buf = (uint8_t*)malloc(50*1024);

	int size = 0;
	int len = 0;
	bool gval = false;

	led_cnt = 0;

	FILE *test_file1 = fopen("/tmp/mnt/sdcard/stream-0.h265", "rb");
	if (test_file1 == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, test_file1);
		return ((void*) 0);
	}
	FILE *test_file2 = fopen("/tmp/mnt/sdcard/stream-3.h265", "rb");
	if (test_file2 == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, test_file2);
		return ((void*) 0);
	}

	do {
		size = fread(test_buf, 1, 40*1000, test_file1);
		if (size == 0){
			fclose(test_file1);
			test_file1 = fopen("/tmp/mnt/sdcard/stream-0.h265", "rb");
			if (test_file1 == NULL) {
				IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, test_file1);
				return ((void*) 0);
			}
		}

		// dp("size:%d 0:0x%02x 1:0x%02x 899:%02x\n", size, test_buf[0], test_buf[1], test_buf[899]);

		if(VM_Frame_Buff.cnt >= 10){
			dp("VM Frame Buffer Full!\n");
			return ((void*) 0);
		}
		pthread_mutex_lock(&buffMutex_vm);
		memset(VM_Frame_Buff.tx[VM_Frame_Buff.index], 0, 256*1024);

		len = size;
		memcpy(VM_Frame_Buff.tx[VM_Frame_Buff.index]+len, test_buf, len);

		VM_Frame_Buff.len[VM_Frame_Buff.index] = len;
		VM_Frame_Buff.index = (VM_Frame_Buff.index+1)%10;
		VM_Frame_Buff.cnt++;
		pthread_mutex_unlock(&buffMutex_vm);

		usleep(100*1000);

		led_cnt++;

		if (led_cnt > 10) {
			led_cnt = 0;
			ret = gpio_set_val(PORTD+6, gval);
			if (!gval) gval = true;
			else gval = false;
		}
	}while (1);

	fclose(test_file1);
	fclose(test_file2);
	free(test_buf);

	return ((void*) 0);
}

#endif

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

extern void func_reboot(void);

void *OSD_thread(void *args)
{
	int ret;

	int64_t total_time = 0;
	int64_t oldt_time = 0;//, BLC_time = 0;//, mean_time = 0;;
	int mosaic_test_cnt = 0;

	// int f_cnt=0;
	
	// int mosaic_x = 300, mosaic_y = 300;
	// int state = 0;

	// IMPOSDRgnAttr rect_rAttr;
	// IMPOSDRgnAttr cover_rAttr;
	IMPOSDRgnAttr mosaic_rAttr;
	// int mosaic_index = 0;
	
	// int64_t now_mosaic_time = 0;
	// int64_t mtime = 0;
	// uint64_t test_time = 0;
	// IMPOSDGrpRgnAttr gAPMosaic;

	ret = osd_show();
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "OSD show error\n");
		return NULL;
	}

	set_delay_time(200*1000);
	
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

		// if (ex_flag) {
		// 	ex_flag = false;
		// 	IMPOSDRgnAttr rect_rAttr;
		// 	rect_rAttr.type = OSD_REG_RECT;
		// 	rect_rAttr.rect.p0.x = min_pixel_x;
		// 	rect_rAttr.rect.p0.y = min_pixel_y;
		// 	rect_rAttr.rect.p1.x = max_pixel_x - 1;
		// 	rect_rAttr.rect.p1.y = max_pixel_y - 1;
		// 	rect_rAttr.fmt = PIX_FMT_MONOWHITE;
		// 	rect_rAttr.data.lineRectData.color = OSD_RED;
		// 	rect_rAttr.data.lineRectData.linewidth = 1;
		// 	IMP_OSD_SetRgnAttr(prHander[TEST_RECT_INDEX], &rect_rAttr);
		// 	ret = IMP_OSD_ShowRgn(prHander[TEST_RECT_INDEX], mosdgrp, 1);
		// 	if (ret != 0) {
		// 		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
		// 		return NULL;
		// 	}
		// }

		// if (ex_on) {
		// 	ex_on = false;
		// 	IMPOSDRgnAttr rect_rAttr;
		// 	rect_rAttr.type = OSD_REG_RECT;
		// 	rect_rAttr.rect.p0.x = min_pixel_x2;
		// 	rect_rAttr.rect.p0.y = min_pixel_y2;
		// 	rect_rAttr.rect.p1.x = max_pixel_x2 - 1;
		// 	rect_rAttr.rect.p1.y = max_pixel_y2 - 1;
		// 	rect_rAttr.fmt = PIX_FMT_MONOWHITE;
		// 	rect_rAttr.data.lineRectData.color = OSD_YELLOW;
		// 	rect_rAttr.data.lineRectData.linewidth = 1;
		// 	IMP_OSD_SetRgnAttr(prHander[1+1+RECT_INDEX], &rect_rAttr);
		// 	ret = IMP_OSD_ShowRgn(prHander[1+1+RECT_INDEX], mosdgrp, 1);
		// 	if (ret != 0) {
		// 		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
		// 		return NULL;
		// 	}
		// }

		// if (check_delay_time()) {
		// 	if (fdpd_ck && Mosaic_En) {
		// 		fdpd_ck = false;
				
		// 		for (int j=0; j<10; j++) {
		// 			if (fdpd_data[j].flag && fdpd_data[j].classid == 0 && fdpd_data[j].trackid >= 0) {
		// 			// if (fdpd_data[j].flag && fdpd_data[j].classid == 0) {

		// 				index = fdpd_data[j].trackid % 10;
		// 				// x_cal = fdpd_data[j].ul_x - 40;
		// 				// x_cal = (x_cal/10)*10;
		// 				if (fdpd_data[j].ul_x > 40)
		// 					x_cal = fdpd_data[j].ul_x - 40;
		// 				else
		// 					x_cal = fdpd_data[j].ul_x;
		// 				x_cal = x_cal - (x_cal%10);

		// 				// y_cal = fdpd_data[j].ul_y - 40;
		// 				// y_cal = (y_cal/10)*10;
		// 				if (fdpd_data[j].ul_y > 40)
		// 					y_cal = fdpd_data[j].ul_y - 40;
		// 				else
		// 					y_cal = fdpd_data[j].ul_y;
		// 				y_cal = y_cal - (y_cal%10);

		// 				w_cal = fdpd_data[j].br_x - fdpd_data[j].ul_x;
		// 				h_cal = fdpd_data[j].br_y - fdpd_data[j].ul_y;
		// 				if (w_cal < 40 || h_cal < 40) {
		// 					// w_cal = 80;
		// 					// h_cal = 80;
		// 					continue;
		// 				}
		// 				else {
		// 					w_cal = w_cal - (w_cal%40) + 160;
		// 					h_cal = h_cal - (h_cal%40) + 160;
		// 				}
		// 				if(x_cal + w_cal >= 1920) x_cal = 1920 - w_cal - 1;
		// 				if(y_cal + h_cal >= 1080) y_cal = 1080 - h_cal - 1;

		// 				mosaic_rAttr.type = OSD_REG_MOSAIC;
		// 				mosaic_rAttr.mosaicAttr.x = x_cal;
		// 				mosaic_rAttr.mosaicAttr.y = y_cal;
		// 				mosaic_rAttr.mosaicAttr.mosaic_width = w_cal;
		// 				mosaic_rAttr.mosaicAttr.mosaic_height = h_cal;
		// 				mosaic_rAttr.mosaicAttr.frame_width = FIRST_SENSOR_WIDTH;
		// 				mosaic_rAttr.mosaicAttr.frame_height = FIRST_SENSOR_HEIGHT;
		// 				mosaic_rAttr.mosaicAttr.mosaic_min_size = 40;

		// 				// dp("mosaic[%d] x:%d y:%d w:%d h:%d\n", j, x_cal, y_cal, w_cal, h_cal);

		// 				ret = IMP_OSD_SetRgnAttr(prHander[2+RECT_INDEX+index], &mosaic_rAttr);
		// 				if (ret != 0) {
		// 					IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Mosaic error\n");
		// 					return NULL;
		// 				}
		// 				// else {
		// 				// 	set_delay_time(200*1000);

		// 				// }
		// 				if (mosaic_time[index] == 0) {
		// 					ret = IMP_OSD_ShowRgn(prHander[2+RECT_INDEX+index], mosdgrp, 1);
		// 					if (ret != 0) {
		// 						IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Mosaic error\n");
		// 						return NULL;
		// 					}
		// 				}

		// 				fdpd_data[j].flag = false;
		// 				mosaic_time[index] = sample_gettimeus();
		// 				// dp("Face Mosaic[%d] En tiem : %lld\n", index, mosaic_time[index]);
		// 			}
		// 		}
		// 	}
		// // }

		// face_mo_cnt = 0;
		// for (int k=0; k<10; k++) {
		// 	if (mosaic_time[k] != 0) {
		// 		// dp("c");
		// 		face_mo_cnt++;
		// 	}
		// }

		// if (check_delay_time()) {
 		// 	if (Mosaic_En) {
		// 		for (int j=0; j<10; j++) {
		// 			if (fdpd_data[j].flag && fdpd_data[j].classid == 0 && fdpd_data[j].trackid >= 0) {
		// 				IMPOSDRgnAttr rect_rAttr;
		// 				int cover_w, cover_h;
		// 				int cover_sx, cover_sy;
		// 				int cover_ex, cover_ey;
		// 				// rect_rAttr.type = OSD_REG_RECT;
		// 				// rect_rAttr.rect.p0.x = fdpd_data[j].ul_x;
		// 				// rect_rAttr.rect.p0.y = fdpd_data[j].ul_y;
		// 				// rect_rAttr.rect.p1.x = fdpd_data[j].br_x - 1;
		// 				// rect_rAttr.rect.p1.y = fdpd_data[j].br_y - 1;
		// 				// rect_rAttr.fmt = PIX_FMT_MONOWHITE;
		// 				// rect_rAttr.data.lineRectData.color = OSD_GREEN;
		// 				// rect_rAttr.data.lineRectData.linewidth = 1;

		// 				cover_w = (fdpd_data[j].br_x - fdpd_data[j].ul_x)/3;
		// 				cover_h = (fdpd_data[j].br_y - fdpd_data[j].ul_y)/3;
		// 				if (fdpd_data[j].ul_x > cover_w) cover_sx = fdpd_data[j].ul_x - cover_w;
		// 				else cover_sx = 0;
		// 				if (fdpd_data[j].ul_y > cover_h) cover_sy = fdpd_data[j].ul_y - cover_h;
		// 				else cover_sy = 0;

		// 				if (fdpd_data[j].br_x < (1920-cover_w)) cover_ex = fdpd_data[j].br_x + cover_w;
		// 				else cover_ex = 1920-1;
		// 				if (fdpd_data[j].br_y < (1080-cover_h)) cover_ey = fdpd_data[j].br_y + cover_h;
		// 				else cover_ey = 1080-1;

		// 				rect_rAttr.type = OSD_REG_COVER;
		// 				rect_rAttr.rect.p0.x = cover_sx;
		// 				rect_rAttr.rect.p0.y = cover_sy;
		// 				rect_rAttr.rect.p1.x = cover_ex;
		// 				rect_rAttr.rect.p1.y = cover_ey;
		// 			    rect_rAttr.fmt = PIX_FMT_BGRA;
		// 				    //  OSD_IPU_BLACK   = 0xff000000, /**< black */
		// 				    //  OSD_IPU_WHITE   = 0xffffffff, /**< white*/
		// 				    //  OSD_IPU_RED     = 0xffff0000, /**< red */
		// 				    //  OSD_IPU_GREEN   = 0xff00ff00, /**< green */
		// 				    //  OSD_IPU_BLUE    = 0xff0000ff, /**< blue */
		// 				rect_rAttr.data.coverData.color = 0xff777777;
		// 				IMP_OSD_SetRgnAttr(prHander[2+j], &rect_rAttr);
		// 				ret = IMP_OSD_ShowRgn(prHander[2+j], mosdgrp, 1);
		// 				if (ret != 0) {
		// 					IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
		// 					return NULL;
		// 				}
		// 				else {
		// 					mosaic_time[j] = sample_gettimeus();
		// 				}
		// 				dis_cnt[j] = 0;
		// 			}
		// 			else {
		// 				dis_cnt[j]++;
		// 				if ((dis_cnt[j] > 20) && (face_mo_cnt > face_cnt_mosaic)) {
		// 					dis_cnt[j] = 0;
		// 					ret = IMP_OSD_ShowRgn(prHander[2+j], mosdgrp, 0);
		// 					if (ret != 0) {
		// 						IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Test Cover error\n");
		// 						return NULL;
		// 					}
		// 					mosaic_time[j] = 0;
		// 				}
		// 			}
		// 		}
		// 		// usleep(25*1000);
		// 		set_delay_time(5*1000);
		// 	}
		// 	else {
		// 		for (int j=0; j<10; j++) {
		// 			if (fdpd_data[j].flag && fdpd_data[j].classid == 0 && fdpd_data[j].trackid >= 0) {
		// 				IMPOSDRgnAttr rect_rAttr;
		// 				rect_rAttr.type = OSD_REG_RECT;
		// 				rect_rAttr.rect.p0.x = fdpd_data[j].ul_x;
		// 				rect_rAttr.rect.p0.y = fdpd_data[j].ul_y;
		// 				rect_rAttr.rect.p1.x = fdpd_data[j].br_x - 1;
		// 				rect_rAttr.rect.p1.y = fdpd_data[j].br_y - 1;
		// 				rect_rAttr.fmt = PIX_FMT_MONOWHITE;
		// 				rect_rAttr.data.lineRectData.color = OSD_GREEN;
		// 				rect_rAttr.data.lineRectData.linewidth = 1;
		// 				IMP_OSD_SetRgnAttr(prHander[2+j], &rect_rAttr);
		// 				ret = IMP_OSD_ShowRgn(prHander[2+j], mosdgrp, 1);
		// 				if (ret != 0) {
		// 					IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
		// 					return NULL;
		// 				}
		// 				dis_cnt[j] = 0;
		// 			}
		// 			else {
		// 				dis_cnt[j]++;
		// 				if (dis_cnt[j] > 100) {
		// 					dis_cnt[j] = 0;
		// 					ret = IMP_OSD_ShowRgn(prHander[2+j], mosdgrp, 0);
		// 					if (ret != 0) {
		// 						IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Test Cover error\n");
		// 						return NULL;
		// 					}
		// 				}
		// 			}
		// 		}
		// 		set_delay_time(5*1000);
		// 	}
		// }
		// else {
		// 	face_mo_cnt = 0;
		// 	for (int k=0; k<10; k++) {
		// 		if (mosaic_time[k] != 0) {
		// 			// dp("c");
		// 			face_mo_cnt++;
		// 		}
		// 	}
		// }

		// if (!Mosaic_En) {
		// 	if (!rect_flag) rect_flag = true;
		// 	for (int j=0; j<10; j++) {
		// 		if (fdpd_data[j].flag && fdpd_data[j].classid == 0 && fdpd_data[j].trackid >= 0) {
		// 			IMPOSDRgnAttr rect_rAttr;
		// 			rect_rAttr.type = OSD_REG_RECT;
		// 			rect_rAttr.rect.p0.x = fdpd_data[j].ul_x;
		// 			rect_rAttr.rect.p0.y = fdpd_data[j].ul_y;
		// 			rect_rAttr.rect.p1.x = fdpd_data[j].br_x - 1;
		// 			rect_rAttr.rect.p1.y = fdpd_data[j].br_y - 1;
		// 			rect_rAttr.fmt = PIX_FMT_MONOWHITE;
		// 			rect_rAttr.data.lineRectData.color = OSD_GREEN;
		// 			rect_rAttr.data.lineRectData.linewidth = 1;
		// 			IMP_OSD_SetRgnAttr(prHander[2+j], &rect_rAttr);
		// 			ret = IMP_OSD_ShowRgn(prHander[2+j], mosdgrp, 1);
		// 			if (ret != 0) {
		// 				IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
		// 				return NULL;
		// 			}

					
		// 		}
		// 		else {
		// 			ret = IMP_OSD_ShowRgn(prHander[2+j], mosdgrp, 0);
		// 			if (ret != 0) {
		// 				IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Test Cover error\n");
		// 				return NULL;
		// 			}
		// 		}
		// 	}
		// }

		// if (rect_flag && Mosaic_En && (clip_rec_state < REC_MP4MAKE) && 
		// 								(streaming_rec_state < REC_MP4MAKE) && 
		// 								(bell_rec_state < REC_MP4MAKE)) 
		// {
		// 	rect_flag = false;
		// 	for (int j=0; j<10; j++) {
		// 		ret = IMP_OSD_ShowRgn(prHander[2+j], mosdgrp, 0);
		// 		if (ret != 0) {
		// 			IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Test Cover error\n");
		// 			return NULL;
		// 		}
		// 	}
		// } 
		// usleep(5*1000);

		// if (check_delay_time()) {
		// 	mosaic_test_cnt++;
		// 	mosaic_rAttr.type = OSD_REG_MOSAIC;
		// 	mosaic_rAttr.mosaicAttr.x = (mosaic_test_cnt*40)%1720;
		// 	mosaic_rAttr.mosaicAttr.y = (mosaic_test_cnt*40)%800;
		// 	mosaic_rAttr.mosaicAttr.mosaic_width = 200;
		// 	mosaic_rAttr.mosaicAttr.mosaic_height = 280;
		// 	mosaic_rAttr.mosaicAttr.frame_width = FIRST_SENSOR_WIDTH;
		// 	mosaic_rAttr.mosaicAttr.frame_height = FIRST_SENSOR_HEIGHT;
		// 	mosaic_rAttr.mosaicAttr.mosaic_min_size = 40;

		// 	ret = IMP_OSD_SetRgnAttr(prHander[2+RECT_INDEX], &mosaic_rAttr);
		// 	if (ret != 0) {
		// 		IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Mosaic error\n");
		// 		return NULL;
		// 	}
		// 	set_delay_time(200*1000);
		// }


		for (int i=0; i<10; i++) {
			if (mosaic_time[i] != 0){
				if ((sample_gettimeus()-mosaic_time[i]) > 6000000) {
					mosaic_time[i] = 0;
					#if 0
						ret = IMP_OSD_ShowRgn(prHander[2+i], mosdgrp, 0);
						if (ret != 0) {
							IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Test Cover error\n");
							return NULL;
						}
					#else
						ret = IMP_OSD_ShowRgn(prHander[2+RECT_INDEX+i], mosdgrp, 0);
						if (ret != 0) {
							IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Test Cover error\n");
							return NULL;
						}
					#endif
				}
			}
		}

		if (streaming_rec_state == REC_MP4MAKE || 
			(clip_rec_state == REC_MP4MAKE && bell_rec_state == REC_MP4MAKE) ||
			(clip_rec_state == REC_MP4MAKE && bell_rec_state == REC_READY)) {
			dp("OSD Thread END!\n");
			return ((void*) 0);
		}



		total_time = sample_gettimeus() - start_time;
		if (total_time/1000000 != oldt_time) {
			oldt_time = total_time/1000000;
			if (dot_En) {
				dp(".\n");
			}
			// isp_integration_time(0, 0);
			// SceneceSet(0, 0);
		}
		// if (total_time/200000 != BLC_time && settings.SF.bits.backlight) {
		// 	BLC_time = total_time/200000;
		// 	BLC_User();
		// }
		if (polling_err_cnt > 6) {
			func_reboot();
		}

		// if (total_time/200000 != mean_time) {
		// 	mean_time = total_time/200000;
		// 	cam_mean();
		// }
	} while(!bStrem);

	return ((void*) 0);
}

