/*
 * sample-move.c
 *
 * Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 */

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <ivs/ivs_common.h>
#include <ivs/ivs_interface.h>
#include <ivs/ivs_inf_move.h>
#include <imp/imp_ivs.h>

// #include "move-common.h"
#include "video-common.h"
#include "global_value.h"
#include "setting.h"
#include "c_util.h"

#define TAG "MOVE"

int sample_ivs_move_init(int grp_num){
	int ret = 0;

	ret = IMP_IVS_CreateGroup(grp_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_CreateGroup(%d) failed\n", grp_num);
		return -1;
	}

	return 0;
}

int sample_ivs_move_exit(int grp_num){
	int ret = 0;

	ret = IMP_IVS_DestroyGroup(grp_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_DestroyGroup(%d) failed\n", grp_num);
		return -1;
	}
	return 0;
}

int sample_ivs_move_start(int grp_num, int chn_num, IMPIVSInterface **interface){
	int ret = 0;
	float lvl_buf = 0.5;
    //check ivs version
    //MOVE_VERSION_NUM defined in ivs_inf_move.h.
    uint32_t move_ver = move_get_version_info();
    if(move_ver != MOVE_VERSION_NUM){
        dp("The version numbers of head file and lib do not match, head file version: %08x, lib version: %08x\n", MOVE_VERSION_NUM, move_ver);        
        return -1;
    }
    //check ivs version    
	move_param_input_t param;
	memset(&param, 0, sizeof(move_param_input_t));

	if (!move_start_flag) {
		dp("Move Start Set!!\n");
		if (settings.move_sensitivty == 4)
			param.sense = 3;
		else
			param.sense = settings.move_sensitivty;

		lvl_buf = 0.1 + (settings.move_sensitivty * 0.2);
		// param.level=0.5; // 0.3 -> 0.8
		param.level = lvl_buf;
	}
	else {
		dp("Move End Set!!\n");
		param.sense = 0;
		param.level = 0.1;
	}

	// param.sense = 0;
	param.frameInfo.width = 1920;
	param.frameInfo.height = 1080;
	param.min_h = 50;
	param.min_w = 50;

	
	param.timeon = 110; 
	param.timeoff = 0;  
	param.light = 0;    
	param.isSkipFrame = 0;

	/* param.det_w = 160; */
	/* param.det_h = 90; */
	
	param.det_w = 50;
	param.det_h = 50;
	
	param.permcnt = 1;
	param.perms[0].fun = 0;
	/* param.perms[0].fun = 1; */
	param.perms[0].pcnt=4;

	param.perms[0].p = (IVSPoint *)malloc(12* sizeof(int));

	param.perms[0].p[0].x=1;
	param.perms[0].p[0].y=1;
	param.perms[0].p[1].x=1919-1;
	param.perms[0].p[1].y=1;
	param.perms[0].p[2].x=1919-1;
	param.perms[0].p[2].y=1079-1;
	param.perms[0].p[3].x=1;
	param.perms[0].p[3].y=1079-1;

	// param.perms[0].p[0].x=move_det_xs;
	// param.perms[0].p[0].y=move_det_ys;
	// param.perms[0].p[1].x=move_det_xe-1;
	// param.perms[0].p[1].y=move_det_ys;
	// param.perms[0].p[2].x=move_det_xe-1;
	// param.perms[0].p[2].y=move_det_ye-1;
	// param.perms[0].p[3].x=move_det_xs;
	// param.perms[0].p[3].y=move_det_ye-1;

	// param.perms[0].p[0].x=move_det_xs;
	// param.perms[0].p[0].y=0;

	// param.perms[0].p[1].x=move_det_xe-1;
	// param.perms[0].p[1].y=0;

	// param.perms[0].p[2].x=move_det_xe-1;
	// param.perms[0].p[2].y=move_det_ye-1;

	// param.perms[0].p[3].x=0;
	// param.perms[0].p[3].y=move_det_ye-1;

	// param.perms[0].p[4].x=0;
	// param.perms[0].p[4].y=move_det_ys;

	// param.perms[0].p[5].x=move_det_xs;
	// param.perms[0].p[5].y=move_det_ys;


	*interface = MoveInterfaceInit(&param);
	if (*interface == NULL) {
		IMP_LOG_ERR(TAG, "IMP_IVS_CreateGroup(%d) failed\n", grp_num);
		return -1;
	}

	ret = IMP_IVS_CreateChn(chn_num, *interface);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_CreateChn(%d) failed\n", chn_num);
		return -1;
	}

	ret = IMP_IVS_RegisterChn(grp_num, chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_RegisterChn(%d, %d) failed\n", grp_num, chn_num);
		return -1;
	}

	ret = IMP_IVS_StartRecvPic(chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_StartRecvPic(%d) failed\n", chn_num);
		return -1;
	}

	return 0;
}

int sample_ivs_move_stop(int chn_num, IMPIVSInterface *interface){
	int ret = 0;

	ret = IMP_IVS_StopRecvPic(chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_StopRecvPic(%d) failed\n", chn_num);
		return -1;
	}
	sleep(1);

	ret = IMP_IVS_UnRegisterChn(chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_UnRegisterChn(%d) failed\n", chn_num);
		return -1;
	}

	ret = IMP_IVS_DestroyChn(chn_num);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "IMP_IVS_DestroyChn(%d) failed\n", chn_num);
		return -1;
	}

	MoveInterfaceExit(interface);

	return 0;
}

IMPIVSInterface *inteface = NULL;

static void *sample_ivs_move_get_result_process(void *arg)
{
	int ret = 0;
	int chn_num = (int)arg;
	move_param_output_t *result = NULL;
	bool detect_flag = false;
	int detect_on_cnt = 0, detect_off_cnt = 0;
	// int detect_off_timer = 0;
	// int ex_w, ex_h, ex_v;
	// bool ex_check=false;
	// int det_ex_cnt = 0;
	// int det_ex_cnt2 = 0;
	int ex_cnt = 0;
	int nex_cnt = 0;
	int move_on_buf = 0;
	// int px_w, px_h;

	/* Step.6 start to ivs */
	ret = sample_ivs_move_start(0, 0, &inteface);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_start(0, 0) failed\n");
		return (void *)-1;
	}

	dp("Move Start!!\n");

	// move_end = true;
	detect_flag = false;

	// for (i = 0; i < 1000/*NR_FRAMES_TO_SAVE*/; i++) {
	do {
		ret = IMP_IVS_PollingResult(chn_num, IMP_IVS_DEFAULT_TIMEOUTMS);
		// ret = IMP_IVS_PollingResult(chn_num, 100);
        if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_IVS_PollingResult(%d, %d) failed\n", chn_num, IMP_IVS_DEFAULT_TIMEOUTMS);
			return (void *)-1;
		}
		ret = IMP_IVS_GetResult(chn_num, (void **)&result);
		if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_IVS_GetResult(%d) failed\n", chn_num);
            return (void *)-1;
        }

  		detect_flag = false;

		if (result->ret > 0) {
			
			for (int j=0; j<8; j++){
				if (result->rects[j].ul.x == 0 && result->rects[j].ul.y == 0 && result->rects[j].br.x == 0 && result->rects[j].br.y == 0){
					// dp("[%d]Not Detection Position!\n", j);
				}
				else if (((result->rects[j].ul.x >= move_det_xs && result->rects[j].ul.x <= move_det_xe) && (result->rects[j].ul.y >= move_det_ys && result->rects[j].ul.y <= move_det_ye)) &&
					( (result->rects[j].br.x >= move_det_xs && result->rects[j].br.x <= move_det_xe) && (result->rects[j].br.y >= move_det_ys && result->rects[j].br.y <= move_det_ye)))
				{
					if (!move_flag) {
						// dp("[%d]2\n", j);
						detect_flag = true;;
					}
					else {
						// dp("[%d]3\n", j);
						ex_cnt++;
						// det_ex_cnt = 0;
						// det_ex_cnt2++;
						// if (det_ex_cnt2 > 2) {
						// 	detect_flag = false;
						// 	det_ex_cnt2 = 0;
						// }
					}
				}
				else{
					// dp("[%d]4\n", j);
					if (!move_flag) {
						detect_flag = true;;
					}
					else {
						nex_cnt++;
					}
					// if (det_ex_cnt < 5) {
					// 	dp("[%d]4\n", j);
					// 	det_ex_cnt++;
					// }
					// else {
					// 	dp("[%d]5\n", j);
					// 	det_ex_cnt = 0;
					// 	detect_flag = true;
					// }
				}
			}
		}
		// else {
		// 	detect_flag = false;
		// }



		// dp("Move Polling:%lld result->ret:%d result->count:%d detect_flag:%d detect_on_cnt:%d detect_off_cnt:%d\n", sample_gettimeus(), result->ret, result->count, detect_flag, detect_on_cnt, detect_off_cnt);

		if (move_flag) {

			if ((ex_cnt+nex_cnt != 0) && (ex_cnt < (nex_cnt+1))) {
				detect_flag = true;
			}
			ex_cnt = 0;
			nex_cnt = 0;
		}

		if (!move_start_flag) move_on_buf = settings.move_sensitivty;
		else move_on_buf = 0;

		if (detect_flag){
			detect_on_cnt++;
			if(detect_on_cnt > (5-move_on_buf)) {
				// dp("move flag on!\n");
				detect_on_cnt = 0;
				detect_off_cnt = 0;
				// detect_off_timer = 0;
				main_motion_detect++;
			}
			// dp("Movement!! %d\n", detect_on_cnt);
		}
		else{
			detect_off_cnt++;
			if(detect_off_cnt > (2+move_on_buf)) {
				// dp("move flag off!\n");
				detect_on_cnt = 0;
				detect_off_cnt = 0;
				main_motion_detect = 0;
				// detect_off_timer++;
			}
			// dp("No movement!! %d\n", detect_off_timer);
			
		}


		//IMP_LOG_INFO(TAG, "frame[%d], result->retRoi(%d,%d,%d,%d)\n", i, result->retRoi[0], result->retRoi[1], result->retRoi[2], result->retRoi[3]);

		ret = IMP_IVS_ReleaseResult(chn_num, (void *)result);
        if (ret < 0) {
			IMP_LOG_ERR(TAG, "IMP_IVS_ReleaseResult(%d) failed\n", chn_num);
			return (void *)-1;
		}
	} while(!bMove);

	move_start_flag = true;

	return (void *)0;
}

static int sample_ivs_move_get_result_start(int chn_num, pthread_t *ptid)
{
	if (pthread_create(ptid, NULL, sample_ivs_move_get_result_process, (void *)chn_num) < 0) {
		IMP_LOG_ERR(TAG, "create sample_ivs_get_result_process failed\n");
		return -1;
	}

	return 0;
}

static int sample_ivs_move_get_result_stop(pthread_t tid)
{
	pthread_join(tid, NULL);
	return 0;
}

IMPCell ivs_cell = {DEV_ID_IVS, 0, 0};


int move_init(IMPCell source_framecell)
{
    // dp("move init\n");
	int ret;
	

    /* Step.3 Encoder init */
	ret = sample_ivs_move_init(0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_init(0) failed\n");
		return -1;
	}

    /* Step.4 Bind */
    // IMPCell framesource_cell = {DEV_ID_FS, FS_MAIN_CHN, 0};
	// IMPCell ivs_cell = {DEV_ID_IVS, 0, 0};

    // ret = IMP_System_Bind(&framesource_cell, &ivs_cell);
    ret = IMP_System_Bind(&source_framecell, &ivs_cell);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and ivs0 failed\n", FS_MAIN_CHN);
        return -1;
    }

    // /* Step.6 start to ivs */
	// ret = sample_ivs_move_start(0, 0, &inteface);
	// if (ret < 0) {
	// IMP_LOG_ERR(TAG, "sample_ivs_start(0, 0) failed\n");
	// 	return -1;
	// }

	return 0;
}

int move(void)
{
    // dp("move\n");
	int ret;
	pthread_t ivs_tid;

	/* Step.7 start to get ivs result */
	ret = sample_ivs_move_get_result_start(0, &ivs_tid);
    if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_get_result_start failed\n");
        return -1;
    }

	/* Exit sequence as follow */
	/* Step.8 stop to get ivs result */
	ret = sample_ivs_move_get_result_stop(ivs_tid);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_get_result_stop failed\n");
		return -1;
	}
	dp("move join\n");

	return 0;
}


int move_deinit(IMPCell source_framecell)
{
    dp("move deinit\n");
	int ret;

	/* Step.9 stop to ivs */
	ret = sample_ivs_move_stop(0, inteface);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "sample_ivs_stop(0) failed\n");
		return -1;
	}

    // /* Step.10 Stream Off */
    // ret = sample_framesource_streamoff(FS_MAIN_CHN);
	// if (ret < 0) {
	// 	IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
	// 	return -1;
	// }

    /* Step.11 UnBind */
    ret = IMP_System_UnBind(&source_framecell, &ivs_cell);
	if (ret < 0) {
        IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and ivs0 failed\n", FS_MAIN_CHN);
		return -1;
	}

    /* Step.12 ivs exit */
	ret = sample_ivs_move_exit(0);
	if (ret < 0) {
		IMP_LOG_ERR(TAG, "ivs mode exit failed\n");
		return -1;
	}

	return 0;
}

int get_parm(void) {
	int ret = -1;
	move_param_input_t param;

	ret = IMP_IVS_GetParam(0, &param);
	if (ret < 0) {
		dp("Param Get Fail!!\n");
	}

	dp("Get Param Sens : %d\n", settings.move_sensitivty);
	dp("Get Param Lvls : %f\n", param.level);

	return 0;
}

int set_parm_end(void) {
	int ret = -1;
	move_param_input_t param;

	ret = IMP_IVS_GetParam(0, &param);
	if (ret < 0) {
		dp("Param Get Fail!!\n");
	}

	dp("Get Param Sens : %d\n", param.sense);
	dp("Get Param Lvls : %f\n", param.level);

	param.sense = 0;
	param.level = 0.1;

	param.min_h = 100;
	param.min_w = 100;

	ret = IMP_IVS_SetParam(0, &param);
	if (ret < 0) {
		dp("Param Get Fail!!\n");
	}

	dp("Set Param Sens : %d\n", param.sense);
	dp("Set Param Lvls : %f\n", param.level);

	return 0;
}

