#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/prctl.h>
#include <math.h>
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/time.h>
#include <math.h>

#include <imp/imp_audio.h>
#include <imp/imp_log.h>
#include <imp/imp_osd.h>

// #include "audio.h"
#include "udp.h"
#include "global_value.h"

#define TAG "Audio"

#define AUDIO_SAMPLE_RATE 16000
// #define AUDIO_SAMPLE_RATE 14000
#define AUDIO_SAMPLE_TIME 20

#define AUDIO_SAMPLE_BUF_SIZE (AUDIO_SAMPLE_RATE * sizeof(short) * AUDIO_SAMPLE_TIME / 1000)

int set_ai_vol = 0;
int set_ai_gain = 0;

int set_ao_vol = 0;
int set_ao_gain = 0;

IMPAudioIOAttr ai_attr;
int ai_devID = 1;
int ai_chnID = 0;

// IMPAudioAgcConfig ai_agc_attr;

IMPAudioIOAttr ao_attr;
int ao_devID = 0;
int ao_chnID = 0;

// IMPAudioAgcConfig ao_agc_attr;

extern pthread_mutex_t buffMutex_ao;
extern pthread_mutex_t buffMutex_ai;

int Init_Audio_In(void)
{
	int ret = -1;

	// ai_attr.samplerate = AUDIO_SAMPLE_RATE_16000;
	ai_attr.samplerate = AUDIO_SAMPLE_RATE;
	ai_attr.bitwidth = AUDIO_BIT_WIDTH_16;
	ai_attr.soundmode = AUDIO_SOUND_MODE_MONO;
	ai_attr.frmNum = 20;
	ai_attr.numPerFrm = AUDIO_SAMPLE_BUF_SIZE;
	ai_attr.chnCnt = 1;

	ret = IMP_AI_SetPubAttr(ai_devID, &ai_attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ai %d ai_attr err: %d\n", ai_devID, ret);
		return ret;
	}

	memset(&ai_attr, 0x0, sizeof(ai_attr));
	ret = IMP_AI_GetPubAttr(ai_devID, &ai_attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ai %d ai_attr err: %d\n", ai_devID, ret);
		return ret;
	}

	// ai_agc_attr.TargetLevelDbfs = 20;
	// ai_agc_attr.CompressionGaindB = 15;


	// ret = IMP_AI_EnableAgc(&ai_attr, ai_agc_attr);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "set ai agc %d ai_attr err: %d\n", ai_devID, ret);
	// 	return ret;
	// }

	// ret =  IMP_AI_EnableHpf(&ai_attr);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "IMP_AI_EnableAgc: %d\n", ret);
	// 	return ret;
	// }


	// ret = IMP_AI_SetHpfCoFrequency(600);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "IMP_AO_SetHpfCoFrequency: %d\n", ret);
	// 	return ret;
	// }

	// ret = IMP_AI_EnableNs(&ai_attr, 1);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "IMP_AI_EnableNs: %d\n", ret);
	// 	return ret;
	// }


	IMP_LOG_INFO(TAG, "Audio In GetPubAttr samplerate : %d\n", ai_attr.samplerate);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr   bitwidth : %d\n", ai_attr.bitwidth);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  soundmode : %d\n", ai_attr.soundmode);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr     frmNum : %d\n", ai_attr.frmNum);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr  numPerFrm : %d\n", ai_attr.numPerFrm);
	IMP_LOG_INFO(TAG, "Audio In GetPubAttr     chnCnt : %d\n", ai_attr.chnCnt);

	/* step 2. enable AI device. */
	ret = IMP_AI_Enable(ai_devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "enable ai %d err\n", ai_devID);
		return ret;
	}

	/* step 3. set audio channel attribute of AI device. */
	
	IMPAudioIChnParam chnParam;
	chnParam.usrFrmDepth = 20;
	chnParam.aecChn = AUDIO_AEC_CHANNEL_FIRST_LEFT;
	ret = IMP_AI_SetChnParam(ai_devID, ai_chnID, &chnParam);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ai %d channel %d attr err: %d\n", ai_devID, ai_chnID, ret);
		return ret;
	}

	memset(&chnParam, 0x0, sizeof(chnParam));
	ret = IMP_AI_GetChnParam(ai_devID, ai_chnID, &chnParam);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ai %d channel %d attr err: %d\n", ai_devID, ai_chnID, ret);
		return ret;
	}

	IMP_LOG_INFO(TAG, "Audio In GetChnParam usrFrmDepth : %d\n", chnParam.usrFrmDepth);

	/* step 4. enable AI channel. */
	ret = IMP_AI_EnableChn(ai_devID, ai_chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record enable channel failed\n");
		return ret;
	}
	/*set aec profile path */
	ret = IMP_AI_Set_WebrtcProfileIni_Path("/config/profiles");
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Set AEC Path failed\n");
		return ret;
	} else {
		IMP_LOG_INFO(TAG, "Set AEC Path Ok\n");
	}

	//// test////////////////////////////////////////////////////////////////////////
	

	/* step 5. Set audio channel volume. */
	int chnVol = set_ai_vol;
	ret = IMP_AI_SetVol(ai_devID, ai_chnID, chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record set volume failed\n");
		return ret;
	}

	ret = IMP_AI_GetVol(ai_devID, ai_chnID, &chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record get volume failed\n");
		return ret;
	}

	IMP_LOG_INFO(TAG, "Audio In GetVol    vol : %d\n", chnVol);

	int aigain = set_ai_gain;
	ret = IMP_AI_SetGain(ai_devID, ai_chnID, aigain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Set Gain failed\n");
		return ret;
	}

	ret = IMP_AI_GetGain(ai_devID, ai_chnID, &aigain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Get Gain failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio In GetGain    gain : %d\n", aigain);

	ret = IMP_AI_EnableAec(ai_devID, ai_chnID, ao_devID, ao_chnID);
	// ret = IMP_AI_EnableAecRefFrame(ai_devID, ai_chnID, ao_devID, ao_chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record enable channel failed\n");
		return ret;
	}

	return 0;
}

int Deinit_Audio_In(void)
{
	int ret = -1;

	ret = IMP_AI_DisableAec(ai_devID, ai_chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_AI_DisableAecRefFrame\n");
		return ret;
	}
	sleep(3);
	ret = IMP_AI_DisableChn(ai_devID, ai_chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio channel disable error\n");
		return ret;
	}

	/* step 9. disable the audio devices. */
	ret = IMP_AI_Disable(ai_devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio device disable error\n");
		return ret;
	}
	return 0;
}


int Init_Audio_Out(void)
{
	int ret = -1;

	// ao_attr.samplerate = AUDIO_SAMPLE_RATE_16000;
	ao_attr.samplerate = AUDIO_SAMPLE_RATE;
	ao_attr.bitwidth = AUDIO_BIT_WIDTH_16;
	ao_attr.soundmode = AUDIO_SOUND_MODE_MONO;
	ao_attr.frmNum = 20;
	// ao_attr.numPerFrm = 640;
	ao_attr.numPerFrm = AUDIO_SAMPLE_BUF_SIZE;
	// printf("sample:%d, numperfrm:%d\n", ao_attr.samplerate, ao_attr.numPerFrm);
	ao_attr.chnCnt = 1;

	ret = IMP_AO_SetPubAttr(ao_devID, &ao_attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "set ao %d ao_attr err: %d\n", ao_devID, ret);
		return ret;
	}

	memset(&ao_attr, 0x0, sizeof(ao_attr));
	ret = IMP_AO_GetPubAttr(ao_devID, &ao_attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "get ao %d ao_attr err: %d\n", ao_devID, ret);
		return ret;
	}

	// ao_agc_attr.TargetLevelDbfs = 20;
	// ao_agc_attr.CompressionGaindB = 10;


	// ret = IMP_AO_EnableAgc(&ao_attr, ao_agc_attr);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "set ai agc %d ai_attr err: %d\n", ao_devID, ret);
	// 	return ret;
	// }

	// ret = IMP_AO_SetHpfCoFrequency(400);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "IMP_AO_SetHpfCoFrequency: %d\n", ret);
	// 	return ret;
	// }

	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr samplerate:%d\n", ao_attr.samplerate);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr   bitwidth:%d\n", ao_attr.bitwidth);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr  soundmode:%d\n", ao_attr.soundmode);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr     frmNum:%d\n", ao_attr.frmNum);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr  numPerFrm:%d\n", ao_attr.numPerFrm);
	IMP_LOG_INFO(TAG, "Audio Out GetPubAttr     chnCnt:%d\n", ao_attr.chnCnt);

	/* Step 2: enable AO device. */
	ret = IMP_AO_Enable(ao_devID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "enable ao %d err\n", ao_devID);
		return ret;
	}

	/* Step 3: enable AI channel. */
	
	ret = IMP_AO_EnableChn(ao_devID, ao_chnID);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio play enable channel failed\n");
		return ret;
	}

	/* Step 4: Set audio channel volume. */
	int chnVol = set_ao_vol;
	ret = IMP_AO_SetVol(ao_devID, ao_chnID, chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play set volume failed\n");
		return ret;
	}

	ret = IMP_AO_GetVol(ao_devID, ao_chnID, &chnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play get volume failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio Out GetVol    vol:%d\n", chnVol);

	int aogain = set_ao_gain;
	ret = IMP_AO_SetGain(ao_devID, ao_chnID, aogain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Set Gain failed\n");
		return ret;
	}

	ret = IMP_AO_GetGain(ao_devID, ao_chnID, &aogain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Get Gain failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio Out GetGain    gain : %d\n", aogain);

	ret = IMP_AO_EnableHpf(&ao_attr);
	if(ret != 0) {
		printf("enable audio hpf error.\n");
		IMP_LOG_INFO(TAG, "enable audio hpf error.\n");
		return ret;
	}


	return 0;
}

int Deinit_Audio_Out(void)
{
	int ret = -1;
	
	ret = IMP_AO_FlushChnBuf(ao_devID, ao_chnID);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_AO_FlushChnBuf error\n");
		return ret;
	}
	/* Step 6: disable the audio channel. */
	ret = IMP_AO_DisableChn(ao_devID, ao_chnID);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "Audio channel disable error\n");
		return ret;
	}

	/* Step 7: disable the audio devices. */
	ret = IMP_AO_Disable(ao_devID);
	if (ret != 0) {
		IMP_LOG_ERR(TAG, "Audio device disable error\n");
		return ret;
	}
	return 0;
}

int Set_Vol(int ai_vol, int ai_gain, int ao_vol, int ao_gain) {
	int ret = -1;
	/* step 5. Set audio channel volume. */
	int aichnVol = ai_vol;
	ret = IMP_AI_SetVol(ai_devID, ai_chnID, aichnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record set volume failed\n");
		return ret;
	}

	ret = IMP_AI_GetVol(ai_devID, ai_chnID, &aichnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record get volume failed\n");
		return ret;
	}

	IMP_LOG_INFO(TAG, "Audio In GetVol    vol : %d\n", aichnVol);

	int aigain = ai_gain;
	ret = IMP_AI_SetGain(ai_devID, ai_chnID, aigain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Set Gain failed\n");
		return ret;
	}

	ret = IMP_AI_GetGain(ai_devID, ai_chnID, &aigain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Get Gain failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio In GetGain    gain : %d\n", aigain);

		/* Step 4: Set audio channel volume. */
	int aochnVol = ao_vol;
	ret = IMP_AO_SetVol(ao_devID, ao_chnID, aochnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play set volume failed\n");
		return ret;
	}

	ret = IMP_AO_GetVol(ao_devID, ao_chnID, &aochnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play get volume failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio Out GetVol    vol:%d\n", aochnVol);

	int aogain = ao_gain;
	ret = IMP_AO_SetGain(ao_devID, ao_chnID, aogain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Set Gain failed\n");
		return ret;
	}

	ret = IMP_AO_GetGain(ao_devID, ao_chnID, &aogain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Get Gain failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio Out GetGain    gain : %d\n", aogain);

	return 0;
}

int Get_Vol(void) {
	int ret = -1;
	/* step 5. Set audio channel volume. */
	int aichnVol = 0;
	ret = IMP_AI_GetVol(ai_devID, ai_chnID, &aichnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record get volume failed\n");
		return ret;
	}

	IMP_LOG_INFO(TAG, "Audio In GetVol    vol : %d\n", aichnVol);

	int aigain = 0;
	ret = IMP_AI_GetGain(ai_devID, ai_chnID, &aigain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Get Gain failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio In GetGain    gain : %d\n", aigain);

		/* Step 4: Set audio channel volume. */
	int aochnVol = 0;
	ret = IMP_AO_GetVol(ao_devID, ao_chnID, &aochnVol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play get volume failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio Out GetVol    vol:%d\n", aochnVol);

	int aogain = 0;
	ret = IMP_AO_GetGain(ao_devID, ao_chnID, &aogain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Get Gain failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio Out GetGain    gain : %d\n", aogain);

	printf("ai vol:%d ai gain:%d ao vol:%d ao gain:%d\n", aichnVol, aigain, aochnVol, aogain);

	return 0;
}


void * IMP_Audio_Record_AEC_Thread(void *argv)
{
	int ret = -1;
	int buff_space = 0;
	uint8_t *buff_u8;
	// uint16_t len=0, rest_cnt=0;

	// /* step 1. set public attribute of AI device. */
	// int devID = 1;
	// IMPAudioIOAttr attr;
	// attr.samplerate = AUDIO_SAMPLE_RATE_16000;
	// attr.bitwidth = AUDIO_BIT_WIDTH_16;
	// attr.soundmode = AUDIO_SOUND_MODE_MONO;
	// attr.frmNum = 40;
	// attr.numPerFrm = 640;
	// attr.chnCnt = 1;
	// ret = IMP_AI_SetPubAttr(devID, &attr);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "set ai %d attr err: %d\n", devID, ret);
	// 	return NULL;
	// }

	// memset(&attr, 0x0, sizeof(attr));
	// ret = IMP_AI_GetPubAttr(devID, &attr);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "get ai %d attr err: %d\n", devID, ret);
	// 	return NULL;
	// }

	// IMP_LOG_INFO(TAG, "Audio In GetPubAttr samplerate : %d\n", attr.samplerate);
	// IMP_LOG_INFO(TAG, "Audio In GetPubAttr   bitwidth : %d\n", attr.bitwidth);
	// IMP_LOG_INFO(TAG, "Audio In GetPubAttr  soundmode : %d\n", attr.soundmode);
	// IMP_LOG_INFO(TAG, "Audio In GetPubAttr     frmNum : %d\n", attr.frmNum);
	// IMP_LOG_INFO(TAG, "Audio In GetPubAttr  numPerFrm : %d\n", attr.numPerFrm);
	// IMP_LOG_INFO(TAG, "Audio In GetPubAttr     chnCnt : %d\n", attr.chnCnt);

	// /* step 2. enable AI device. */
	// ret = IMP_AI_Enable(devID);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "enable ai %d err\n", devID);
	// 	return NULL;
	// }

	// /* step 3. set audio channel attribute of AI device. */
	// int chnID = 0;
	// IMPAudioIChnParam chnParam;
	// chnParam.usrFrmDepth = 40;
	// chnParam.aecChn = AUDIO_AEC_CHANNEL_FIRST_LEFT;
	// ret = IMP_AI_SetChnParam(devID, chnID, &chnParam);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "set ai %d channel %d attr err: %d\n", devID, chnID, ret);
	// 	return NULL;
	// }

	// memset(&chnParam, 0x0, sizeof(chnParam));
	// ret = IMP_AI_GetChnParam(devID, chnID, &chnParam);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "get ai %d channel %d attr err: %d\n", devID, chnID, ret);
	// 	return NULL;
	// }

	// IMP_LOG_INFO(TAG, "Audio In GetChnParam usrFrmDepth : %d\n", chnParam.usrFrmDepth);

	// /* step 4. enable AI channel. */
	// ret = IMP_AI_EnableChn(devID, chnID);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "Audio Record enable channel failed\n");
	// 	return NULL;
	// }
	// /*set aec profile path */
	// /*ret = IMP_AI_Set_WebrtcProfileIni_Path("/system/");
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "Set AEC Path failed\n");
	// 	return NULL;
	// } else {
	// 	IMP_LOG_INFO(TAG, "Set AEC Path Ok\n");
	// }*/

	// ret = IMP_AI_EnableAec(devID, chnID, 0, 0);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "Audio Record enable channel failed\n");
	// 	return NULL;
	// }


	/* step 5. Set audio channel volume. */
	// int chnVol = set_ai_vol;
	// ret = IMP_AI_SetVol(ai_devID, ai_chnID, chnVol);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "Audio Record set volume failed\n");
	// 	return NULL;
	// }

	// ret = IMP_AI_GetVol(ai_devID, ai_chnID, &chnVol);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "Audio Record get volume failed\n");
	// 	return NULL;
	// }

	// IMP_LOG_INFO(TAG, "Audio In GetVol    vol : %d\n", chnVol);

	// int aigain = set_ai_gain;
	// ret = IMP_AI_SetGain(ai_devID, ai_chnID, aigain);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "Audio Record Set Gain failed\n");
	// 	return NULL;
	// }

	// ret = IMP_AI_GetGain(ai_devID, ai_chnID, &aigain);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "Audio Record Get Gain failed\n");
	// 	return NULL;
	// }
	// IMP_LOG_INFO(TAG, "Audio In GetGain    gain : %d\n", aigain);

	do {
		if (bExit) break;

		/* step 6. get audio record frame. */
		ret = IMP_AI_PollingFrame(ai_devID, ai_chnID, 1000);
		if (ret != 0 ) {
			IMP_LOG_ERR(TAG, "Audio Polling Frame Data error\n");
		}
		IMPAudioFrame frm;
		ret = IMP_AI_GetFrame(ai_devID, ai_chnID, &frm, NOBLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
			return NULL;
		}

		/* step 7. Save the recording data to a file. */
		// fwrite(frm.virAddr, 1, frm.len, record_file);
		// len = frm.len;
		// if ((Audio_Ai_Attr.dcnt + len) > AO_BUFFER_SIZE-1){
		// 	printf("[Audio AI] Buffer Full Frame!!\n");
		// 	continue;
		// }
		// if ((Audio_Ai_Attr.windex + len) <= AO_BUFFER_SIZE-1) {
		// 	memset(Audio_Ai_Attr.tx + Audio_Ai_Attr.windex, 0, len);
		// 	memcpy(Audio_Ai_Attr.tx + Audio_Ai_Attr.windex, frm.virAddr, len);
		// 	Audio_Ai_Attr.windex += len;
		// }
		// else {
		// 	rest_cnt = (AO_BUFFER_SIZE-1) - Audio_Ai_Attr.windex;
		// 	memset(Audio_Ai_Attr.tx + Audio_Ai_Attr.windex, 0, rest_cnt);
		// 	memcpy(Audio_Ai_Attr.tx + Audio_Ai_Attr.windex, frm.virAddr, rest_cnt);
		// 	memset(Audio_Ai_Attr.tx, 0, ((AO_BUFFER_SIZE-1) -rest_cnt));
		// 	memcpy(Audio_Ai_Attr.tx, frm.virAddr + rest_cnt, (len-rest_cnt));
		// 	// printf("AO Rolling windex:%d\n", Audio_Ai_Attr.windex);
		// 	Audio_Ai_Attr.windex = len-rest_cnt;
		// }
		// udp_ai_rolling_dcnt();
		// // printf("[AI AUDIO] sindex:%d windex:%d dcnt%d\n", Audio_Ai_Attr.sindex, Audio_Ai_Attr.windex, Audio_Ai_Attr.dcnt);
		// if (Audio_Ai_Attr.windex >= AO_BUFFER_SIZE-1) {
		// 	if(Audio_Ai_Attr.windex > AO_BUFFER_SIZE-1) {
		// 		printf("AI Write Index Error!\n");
		// 	}
		// 	Audio_Ai_Attr.windex = 0;
		// }

		// pthread_mutex_lock(&buffMutex_ai);
		// buff_space = (AI_Cir_Buff.RIndex - AI_Cir_Buff.WIndex - 1 + A_BUFF_SIZE) % (500*1024);
		// if (buff_space >= frm.len) {
		// 	memcpy(&AI_Cir_Buff.tx[AI_Cir_Buff.WIndex], frm.virAddr, frm.len);
		// 	if ((AI_Cir_Buff.WIndex + frm.len) >= (500*1024))
		// 		AI_Cir_Buff.WIndex = 0;
		// 	else
		// 		AI_Cir_Buff.WIndex = (AI_Cir_Buff.WIndex + frm.len) % (500*1024);
		// 	// printf("[CIR_BUFF Audio Out]buff_space:%d WIndex:%d RIndex%d\n", buff_space, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
		// }
		// else {
		// 	printf("Buffer Overflow, discarding data.\n");
		// }
		// pthread_mutex_unlock(&buffMutex_ai);
		// double sum = 0.0, rms = 0;
		// uint16_t *pcmData = (uint16_t*)frm.virAddr;
		// int pcmDataLength = 442;

		// for (int i=0; i<pcmDataLength; i++){
		// 	sum += pcmData[i] * pcmData[i];
		// }

		// rms = sqrt(sum/pcmDataLength);

		// if(rms <= 7000000) {
		// 	// printf("AI RMS : %d\n", rms);
		// 	// continue;
		// }
		// else {
			if (frm.len > 0){
				pthread_mutex_lock(&buffMutex_ai);
				buff_space = (AI_Cir_Buff.RIndex - AI_Cir_Buff.WIndex - 1 + A_BUFF_SIZE) % (500*1024);
				if (buff_space >= frm.len) {
					buff_u8 = (uint8_t*)frm.virAddr;
					for(int j = 0; j < frm.len; ++j) {
						AI_Cir_Buff.tx[AI_Cir_Buff.WIndex] = buff_u8[j];
						AI_Cir_Buff.WIndex = (AI_Cir_Buff.WIndex+1) % (500*1024);
						if (AI_Cir_Buff.WIndex == AI_Cir_Buff.RIndex) {
							AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+1) % (500*1024);
						}
					}
					// printf("[CIR_BUFF Audio In]buff_space:%d WIndex:%d RIndex%d\n", buff_space, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
				}
				else {
					printf("AI Cir Buff Overflow!1\n");
				}
				pthread_mutex_unlock(&buffMutex_ai);
			}
		// }
		/* step 8. release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(ai_devID, ai_chnID, &frm);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio release frame data error\n");
			return NULL;
		}

	}while(1);

	// ret = IMP_AI_DisableAec(devID, chnID);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "IMP_AI_DisableAecRefFrame\n");
	// 	return NULL;
	// }
	// sleep(3);
	// ret = IMP_AI_DisableChn(devID, chnID);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "Audio channel disable error\n");
	// 	return NULL;
	// }

	// /* step 9. disable the audio devices. */
	// ret = IMP_AI_Disable(devID);
	// if(ret != 0) {
	// 	IMP_LOG_ERR(TAG, "Audio device disable error\n");
	// 	return NULL;
	// }
	printf("[Audio] AI Thread END!\n");
	pthread_exit(0);
}



void *IMP_Audio_Play_Thread(void *argv)
{
	unsigned char *buf = NULL;
	int ret = -1;
	int datasize = 0;
	// int len_cal, len;
	// int read_len = AUDIO_SAMPLE_BUF_SIZE;
	int old_chnbusy=0, old_busy_cnt=0;
	bool bufclear_flag = false;

	int save_fd = 0;
    save_fd = open("/vtmp/save_ao.pcm", O_RDWR | O_CREAT | O_TRUNC, 0777);

	buf = (unsigned char *)malloc(AUDIO_SAMPLE_BUF_SIZE);
	if (buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return NULL;
	}
	
	IMPAudioOChnState play_status;
	play_status.chnBusyNum = 0;
	do {
		if (bExit) break;

		ret = IMP_AO_QueryChnStat(ao_devID, ao_chnID, &play_status);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_AO_QueryChnStat error\n");
			return NULL;
		}

		pthread_mutex_lock(&buffMutex_ao);
		if (AO_Cir_Buff.RIndex != AO_Cir_Buff.WIndex) {
			datasize = (AO_Cir_Buff.WIndex - AO_Cir_Buff.RIndex + A_BUFF_SIZE) % (500*1024);
			if (datasize >= AUDIO_SAMPLE_BUF_SIZE) {
				// datasize = datasize > AUDIO_SAMPLE_BUF_SIZE) ? AUDIO_SAMPLE_BUF_SIZE : datasize;
				datasize = AUDIO_SAMPLE_BUF_SIZE;
				// printf("[CIR_BUFF_AO]AO_SIZE:%d datasize:%d WIndex:%d RIndex%d\n", AUDIO_SAMPLE_BUF_SIZE, datasize, AO_Cir_Buff.WIndex, AO_Cir_Buff.RIndex);
			}
			// else {
			// 	datasize = 0;
			// }
			for (int i = 0; i < datasize; ++i) {
				buf[i] = AO_Cir_Buff.tx[AO_Cir_Buff.RIndex];
				AO_Cir_Buff.RIndex = (AO_Cir_Buff.RIndex+1) % (500*1024);
			}
			if (AO_Cir_Buff.RIndex == AO_Cir_Buff.WIndex) {
				// printf("Buffer Clear!! : %d\n", datasize);
				AO_Cir_Buff.RIndex = AO_Cir_Buff.WIndex = 0;
			}
			// memset(buf, 0, datasize);
			// memcpy(buf, &AO_Cir_Buff.tx[AO_Cir_Buff.RIndex], datasize);
			// AO_Cir_Buff.RIndex = (AO_Cir_Buff.RIndex + datasize) % (500&1024);
		}
		pthread_mutex_unlock(&buffMutex_ao);

		// double sum = 0.0, rms = 0;
		// uint16_t *pcmData = (uint16_t*)buf;
		// int pcmDataLength = datasize/2;

		// for (int i=0; i<pcmDataLength; i++){
		// 	sum += pcmData[i] * pcmData[i];
		// }
 
		// rms = sqrt(sum/pcmDataLength);
		// // rms = sum/pcmDataLength;

		// // if (rms > 1000)
		// 	printf("AO RMS : %f %f %d\n", rms, sum);
		// else
			// continue;

		// printf("WIndex:%d busynum:%d\n", AO_Cir_Buff.WIndex, play_status.chnBusyNum);
		if (play_status.chnBusyNum > 0) {
			if (old_chnbusy != play_status.chnBusyNum) {
				old_chnbusy = play_status.chnBusyNum;
				// printf("busynum:%d cnt:%d\n", old_chnbusy, old_busy_cnt);
				old_busy_cnt = 0;
				bufclear_flag = false;
			}
			else {
				old_busy_cnt++;
				if (old_busy_cnt > 1000000) {
					if (!bufclear_flag) {
						bufclear_flag = true;
						IMP_AO_ClearChnBuf(ao_devID, ao_chnID);
						printf("AO Buf clear!\n");
					}
				}
			}
		}
		if (datasize > 0 && play_status.chnBusyNum < 18) {
			

			/* Step 5: send frame data. */
			IMPAudioFrame frm;
			frm.virAddr = (uint32_t *)buf;
			frm.len = datasize;
			ret = IMP_AO_SendFrame(ao_devID, ao_chnID, &frm, NOBLOCK);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "send Frame Data error\n");
				return NULL;
			}
			else {
				// udp_ao_rolling_dcnt();
				// usleep(30*1000);
				// printf("[AO] sindex:%d windex:%d dcnt:%d\n", Audio_Ao_Attr.sindex, Audio_Ao_Attr.windex, Audio_Ao_Attr.dcnt);
				ret = write(save_fd, buf, datasize);
				datasize = 0;
			}
			
			// ret = IMP_AO_QueryChnStat(ao_devID, ao_chnID, &play_status);
			// if(ret != 0) {
			// 	IMP_LOG_ERR(TAG, "IMP_AO_QueryChnStat error\n");
			// 	return NULL;
			// }


			IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
					play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);

		}
	}while(1);

	printf("[Audio] AO Thread END!\n");
	free(buf);
	pthread_exit(0);
}

void ao_file_play_thread(void *argv)
{
	unsigned char *buf = NULL;
	int size = 0;
	int ret = -1;
	int old_chnbusy=0, old_busy_cnt=0;

	buf = (unsigned char *)malloc(AUDIO_SAMPLE_BUF_SIZE);
	if (buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return;
	}

	FILE *play_file = fopen(argv, "rb");
	if (play_file == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, argv);
		return;
	}

	size = fread(buf, 1, 44, play_file);
	if (size < 44)
		return;
	else printf("wav header read!!\n");

	do {
		if (bExit) break;

		size = fread(buf, 1, AUDIO_SAMPLE_BUF_SIZE, play_file);
		if (size < AUDIO_SAMPLE_BUF_SIZE)
			break;
		if (size == 0)
			break;

		/* Step 5: send frame data. */
		IMPAudioFrame frm;
		frm.virAddr = (uint32_t *)buf;
		frm.len = size;
		ret = IMP_AO_SendFrame(ao_devID, ao_chnID, &frm, NOBLOCK);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "send Frame Data error\n");
			return;
		}

		IMPAudioOChnState play_status;
		ret = IMP_AO_QueryChnStat(ao_devID, ao_chnID, &play_status);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_AO_QueryChnStat error\n");
			return;
		}

		if (play_status.chnBusyNum > 0) {
			if (old_chnbusy != play_status.chnBusyNum) {
				old_chnbusy = play_status.chnBusyNum;
				old_busy_cnt = 0;
			}
			else {
				old_busy_cnt++;
				if (old_busy_cnt%10 == 0) {
					printf("busynum:%d\n", play_status.chnBusyNum);
				}
			}
		}

		IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
				play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);
	}while (1);

	printf("[Audio File] Thread End!\n");
	fclose(play_file);
	free(buf);
	// pthread_exit(0);
}

void ao_file_play_pcm_thread(void *argv)
{
	unsigned char *buf = NULL;
	int size = 0;
	int ret = -1;


	buf = (unsigned char *)malloc(AUDIO_SAMPLE_BUF_SIZE);
	if (buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return;
	}

	FILE *play_file = fopen(argv, "rb");
	if (play_file == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, argv);
		return;
	}

	do {
		if (bExit) break;

		size = fread(buf, 1, AUDIO_SAMPLE_BUF_SIZE, play_file);
		if (size < AUDIO_SAMPLE_BUF_SIZE)
			break;
		if (size == 0)
			break;

		/* Step 5: send frame data. */
		IMPAudioFrame frm;
		frm.virAddr = (uint32_t *)buf;
		frm.len = size;
		ret = IMP_AO_SendFrame(ao_devID, ao_chnID, &frm, NOBLOCK);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "send Frame Data error\n");
			return;
		}

		IMPAudioOChnState play_status;
		ret = IMP_AO_QueryChnStat(ao_devID, ao_chnID, &play_status);
		if (ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_AO_QueryChnStat error\n");
			return;
		}

		IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
				play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);
	}while (1);

	printf("[Audio File] Thread End!\n");
	fclose(play_file);
	free(buf);
	// pthread_exit(0);
}

void *IMP_Audio_InOut_AEC_Thread(void *argv)
{
	int ret = -1;
	unsigned char *buf = NULL;
	buf = (unsigned char *)malloc(AUDIO_SAMPLE_BUF_SIZE);
	if (buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return NULL;
	}

	do {
		if (bExit) break;

		/* step 6. get audio record frame. */
		ret = IMP_AI_PollingFrame(ai_devID, ai_chnID, 1000);
		if (ret != 0 ) {
			IMP_LOG_ERR(TAG, "Audio Polling Frame Data error\n");
		}
		IMPAudioFrame frm;
		ret = IMP_AI_GetFrame(ai_devID, ai_chnID, &frm, NOBLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
			return NULL;
		}

		/* step 7. Save the recording data to a file. */
		// fwrite(frm.virAddr, 1, frm.len, record_file);
		
		/* step 8. release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(ai_devID, ai_chnID, &frm);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio release frame data error\n");
			return NULL;
		}

		IMPAudioOChnState play_status;

		// printf("[AO] dcnt:%d size:%d\n", Audio_Ao_Attr.dcnt, AUDIO_SAMPLE_BUF_SIZE);
		/* Step 5: send frame data. */
		ret = IMP_AO_SendFrame(ao_devID, ao_chnID, &frm, NOBLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "send Frame Data error\n");
			return NULL;
		}

		ret = IMP_AO_QueryChnStat(ao_devID, ao_chnID, &play_status);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_AO_QueryChnStat error\n");
			return NULL;
		}
		IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
				play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);
	}while(1);

	printf("[Audio] AO Thread END!\n");
	free(buf);
	pthread_exit(0);
}
