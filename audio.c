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

#define Encode_Type PT_G726

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

/* Test API */
// static int IMP_Audio_Encode(void)
// {
// 	char *buf_pcm = NULL;
// 	int ret = -1;

// 	buf_pcm = (char *)malloc(AUDIO_SAMPLE_BUF_SIZE);
// 	if(buf_pcm == NULL) {
// 		IMP_LOG_ERR(TAG, "malloc audio pcm buf error\n");
// 		return -1;
// 	}

// 	/* audio encode create channel. */
// 	int AeChn = 0;
// 	IMPAudioEncChnAttr attr;
// 	// attr.type = handle_g711a; /* Use the My method to encoder. if use the system method is attr.type = PT_G711A; */
// 	attr.type = Encode_Type; /* Use the My method to encoder. if use the system method is attr.type = PT_G711A; */
// 	attr.bufSize = 20;
// 	ret = IMP_AENC_CreateChn(AeChn, &attr);
// 	if(ret != 0) {
// 		IMP_LOG_ERR(TAG, "imp audio encode create channel failed\n");
// 		return -1;
// 	}

// 	while(1) {
// 		ret = fread(buf_pcm, 1, AUDIO_SAMPLE_BUF_SIZE, file_pcm);
// 		if(ret < AUDIO_SAMPLE_BUF_SIZE)
// 			break;

// 		/* Send a frame to encode. */
// 		IMPAudioFrame frm;
// 		frm.virAddr = (uint32_t *)buf_pcm;
// 		frm.len = ret;
// 		ret = IMP_AENC_SendFrame(AeChn, &frm);
// 		if(ret != 0) {
// 			IMP_LOG_ERR(TAG, "imp audio encode send frame failed\n");
// 			return -1;
// 		}

// 		/* get audio encode frame. */
// 		IMPAudioStream stream;
// 		ret = IMP_AENC_PollingStream(AeChn, 1000);
// 		if (ret != 0) {
// 			IMP_LOG_ERR(TAG, "imp audio encode polling stream failed\n");
// 		}

// 		ret = IMP_AENC_GetStream(AeChn, &stream, BLOCK);
// 		if(ret != 0) {
// 			IMP_LOG_ERR(TAG, "imp audio encode get stream failed\n");
// 			return -1;
// 		}

// 		/* save the encode data to the file. */
// 		fwrite(stream.stream, 1, stream.len, file_g711);

// 		/* release stream. */
// 		ret = IMP_AENC_ReleaseStream(AeChn, &stream);
// 		if(ret != 0) {
// 			IMP_LOG_ERR(TAG, "imp audio encode release stream failed\n");
// 			return -1;
// 		}
// 	}

// 	// ret = IMP_AENC_UnRegisterEncoder(&handle_g711a);
// 	// if(ret != 0) {
// 	// 	IMP_LOG_ERR(TAG, "IMP_AENC_UnRegisterEncoder failed\n");
// 	// 	return -1;
// 	// }

// 	// ret = IMP_AENC_UnRegisterEncoder(&handle_g711u);
// 	// if(ret != 0) {
// 	// 	IMP_LOG_ERR(TAG, "IMP_AENC_UnRegisterEncoder failed\n");
// 	// 	return -1;
// 	// }

// 	/* destroy the encode channel. */
// 	ret = IMP_AENC_DestroyChn(AeChn);
// 	if(ret != 0) {
// 		IMP_LOG_ERR(TAG, "imp audio encode destroy channel failed\n");
// 		return -1;
// 	}

// 	fclose(file_pcm);
// 	fclose(file_g711);

// 	free(buf_pcm);
// 	return 0;
// }

// static int IMP_Audio_Decode(void)
// {
// 	char *buf_g711 = NULL;
// 	int ret = -1;

// 	buf_g711 = (char *)malloc(IMP_AUDIO_BUF_SIZE);
// 	if(buf_g711 == NULL) {
// 		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio g711 buf error\n", __func__);
// 		return -1;
// 	}

// 	FILE *file_pcm = fopen(IMP_AUDIO_PLAY_FILE, "wb");
// 	if(file_pcm == NULL) {
// 		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, IMP_AUDIO_PLAY_FILE);
// 		return -1;
// 	}

// 	FILE *file_g711 = fopen(IMP_AUDIO_ENCODE_FILE, "rb");
// 	if(file_g711 == NULL) {
// 		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, IMP_AUDIO_ENCODE_FILE);
// 		return -1;
// 	}

// 	// /* My g711a decoder Register. */
// 	// int handle_g711a = 0;
// 	// IMPAudioDecDecoder my_decoder;
// 	// sprintf(my_decoder.name, "%s", "MY_G711A");
// 	// my_decoder.openDecoder = NULL;
// 	// my_decoder.decodeFrm = MY_G711A_Decode_Frm;
// 	// my_decoder.getFrmInfo = NULL;
// 	// my_decoder.closeDecoder = NULL;

// 	// ret = IMP_ADEC_RegisterDecoder(&handle_g711a, &my_decoder);
// 	// if(ret != 0) {
// 	// 	IMP_LOG_ERR(TAG, "IMP_ADEC_RegisterDecoder failed\n");
// 	// 	return -1;
// 	// }

// 	// /* My g711u decoder Register. */
// 	// int handle_g711u = 0;
// 	// memset(&my_decoder, 0x0, sizeof(my_decoder));
// 	// sprintf(my_decoder.name, "%s", "MY_G711U");
// 	// my_decoder.openDecoder = NULL;
// 	// my_decoder.decodeFrm = MY_G711U_Decode_Frm;
// 	// my_decoder.getFrmInfo = NULL;
// 	// my_decoder.closeDecoder = NULL;

// 	// ret = IMP_ADEC_RegisterDecoder(&handle_g711u, &my_decoder);
// 	// if(ret != 0) {
// 	// 	IMP_LOG_ERR(TAG, "IMP_ADEC_RegisterDecoder failed\n");
// 	// 	return -1;
// 	// }

// 	/* audio decoder create channel. */
// 	int adChn = 0;
// 	IMPAudioDecChnAttr attr;
// 	attr.type = Encode_Type;
// 	attr.bufSize = 20;
// 	attr.mode = ADEC_MODE_STREAM;
// 	ret = IMP_ADEC_CreateChn(adChn, &attr);
// 	if(ret != 0) {
// 		IMP_LOG_ERR(TAG, "imp audio decoder create channel failed\n");
// 		return -1;
// 	}

// 	ret = IMP_ADEC_ClearChnBuf(adChn);
// 	if(ret != 0) {
// 		IMP_LOG_ERR(TAG, "IMP_ADEC_ClearChnBuf failed\n");
// 		return -1;
// 	}

// 	while(1) {
// 		ret = fread(buf_g711, 1, IMP_AUDIO_BUF_SIZE/4, file_g711);
// 		if(ret < IMP_AUDIO_BUF_SIZE/4)
// 			break;

// 		/* Send a frame to decoder. */
// 		IMPAudioStream stream_in;
// 		stream_in.stream = (uint8_t *)buf_g711;
// 		stream_in.len = ret;
// 		ret = IMP_ADEC_SendStream(adChn, &stream_in, BLOCK);
// 		if(ret != 0) {
// 			IMP_LOG_ERR(TAG, "imp audio encode send frame failed\n");
// 			return -1;
// 		}

// 		/* get audio decoder frame. */
// 		IMPAudioStream stream_out;
// 		ret = IMP_ADEC_PollingStream(adChn, 1000);
// 		if(ret != 0) {
// 			IMP_LOG_ERR(TAG, "imp audio encode polling stream failed\n");
// 		}

// 		ret = IMP_ADEC_GetStream(adChn, &stream_out, BLOCK);
// 		if(ret != 0) {
// 			IMP_LOG_ERR(TAG, "imp audio decoder get stream failed\n");
// 			return -1;
// 		}

// 		/* save the decoder data to the file. */
// 		fwrite(stream_out.stream, 1, stream_out.len, file_pcm);

// 		/* release stream. */
// 		ret = IMP_ADEC_ReleaseStream(adChn, &stream_out);
// 		if(ret != 0) {
// 			IMP_LOG_ERR(TAG, "imp audio decoder release stream failed\n");
// 			return -1;
// 		}
// 	}

// 	// ret = IMP_ADEC_UnRegisterDecoder(&handle_g711a);
// 	// if(ret != 0) {
// 	// 	IMP_LOG_ERR(TAG, "IMP_ADEC_UnRegisterDecoder failed\n");
// 	// 	return -1;
// 	// }

// 	// ret = IMP_ADEC_UnRegisterDecoder(&handle_g711u);
// 	// if(ret != 0) {
// 	// 	IMP_LOG_ERR(TAG, "IMP_ADEC_UnRegisterDecoder failed\n");
// 	// 	return -1;
// 	// }

// 	/* destroy the decoder channel. */
// 	ret = IMP_ADEC_DestroyChn(adChn);
// 	if(ret != 0) {
// 		IMP_LOG_ERR(TAG, "imp audio decoder destroy channel failed\n");
// 		return -1;
// 	}

// 	fclose(file_pcm);
// 	fclose(file_g711);

// 	free(buf_g711);
// 	return 0;
// }

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
	// dp("sample:%d, numperfrm:%d\n", ao_attr.samplerate, ao_attr.numPerFrm);
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
		dp("enable audio hpf error.\n");
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

int Set_Mute(int onoff) {

	IMP_AI_SetVolMute(ao_devID, ao_chnID, onoff);

	IMP_AI_SetVolMute(ai_devID, ai_chnID, onoff);

	return 0;
}

int get_out_vol = 0;
int get_out_gain = 0;
int get_in_vol = 0;
int get_in_gain = 0;

int Get_Vol(void) {
	int ret = -1;
	/* step 5. Set audio channel volume. */
	ret = IMP_AI_GetVol(ai_devID, ai_chnID, &get_in_vol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record get volume failed\n");
		return ret;
	}

	IMP_LOG_INFO(TAG, "Audio In GetVol    vol : %d\n", get_in_vol);

	ret = IMP_AI_GetGain(ai_devID, ai_chnID, &get_in_gain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Get Gain failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio In GetGain    gain : %d\n", get_in_gain);

		/* Step 4: Set audio channel volume. */
	ret = IMP_AO_GetVol(ao_devID, ao_chnID, &get_out_vol);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Play get volume failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio Out GetVol    vol:%d\n", get_out_vol);

	ret = IMP_AO_GetGain(ao_devID, ao_chnID, &get_out_gain);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "Audio Record Get Gain failed\n");
		return ret;
	}
	IMP_LOG_INFO(TAG, "Audio Out GetGain    gain : %d\n", get_out_gain);

	dp("ai vol:%d ai gain:%d ao vol:%d ao gain:%d\n", get_in_vol, get_in_gain, get_out_vol, get_out_gain);

	return 0;
}


void * IMP_Audio_Record_AEC_Thread(void *argv)
{
	int ret = -1;
	int buff_space = 0;
	int datasize = 0;
	uint8_t *buff_u8;

	uint8_t *buf_pcm = NULL;

	buf_pcm = malloc(AUDIO_SAMPLE_BUF_SIZE*2);
	if(buf_pcm == NULL) {
		IMP_LOG_ERR(TAG, "malloc audio pcm buf error\n");
		return NULL;
	}

	// int save_fd = 0;
    // save_fd = open("/tmp/mnt/sdcard/save_ai.g726", O_RDWR | O_CREAT | O_TRUNC, 0777);

    #ifdef __G726__
	/* audio encode create channel. */
		int AeChn = 0;
		IMPAudioEncChnAttr attr;
		// attr.type = handle_g711a; /* Use the My method to encoder. if use the system method is attr.type = PT_G711A; */
		attr.type = Encode_Type; /* Use the My method to encoder. if use the system method is attr.type = PT_G711A; */
		attr.bufSize = 20;
		ret = IMP_AENC_CreateChn(AeChn, &attr);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "imp audio encode create channel failed\n");
			return NULL;
		}
	#endif

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
		
		#ifdef __G726__
			ret = IMP_AENC_SendFrame(AeChn, &frm);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "imp audio encode send frame failed\n");
				return NULL;
			}

			/* get audio encode frame. */
			IMPAudioStream stream;
			ret = IMP_AENC_PollingStream(AeChn, 1000);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "imp audio encode polling stream failed\n");
			}

			ret = IMP_AENC_GetStream(AeChn, &stream, BLOCK);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "imp audio encode get stream failed\n");
				return NULL;
			}

			// dp("stream.len : %d buf size : %d timestamp : %lld\n", stream.len, AUDIO_SAMPLE_BUF_SIZE, stream.timeStamp);


			if (stream.len > 0){
				pthread_mutex_lock(&buffMutex_ai);
				if (AI_Cir_Buff.RIndex == AI_Cir_Buff.WIndex) {
					buff_space = 500*1024;
					AI_Cir_Buff.RIndex = 0;
					AI_Cir_Buff.WIndex = 0;
				}
				else {
					if (AI_Cir_Buff.WIndex >= AI_Cir_Buff.RIndex)
	                	datasize = (AI_Cir_Buff.WIndex - AI_Cir_Buff.RIndex) % (500*1024);
	            	else
		                datasize = 0;
					buff_space = (500*1024) - datasize;	
				}
				
				if (buff_space >= stream.len) {
					buff_u8 = (uint8_t*)stream.stream;
					memset (&AI_Cir_Buff.tx[AI_Cir_Buff.WIndex], 0, stream.len);
					memcpy (&AI_Cir_Buff.tx[AI_Cir_Buff.WIndex], buff_u8, stream.len);
					// ret = write(save_fd, &AI_Cir_Buff.tx[AI_Cir_Buff.WIndex], stream.len);
					AI_Cir_Buff.WIndex = (AI_Cir_Buff.WIndex+stream.len);
				}
				else {
					dp("AI Cir Buff Overflow!1\n");
				}
				pthread_mutex_unlock(&buffMutex_ai);
			}
		
			/* release stream. */
			ret = IMP_AENC_ReleaseStream(AeChn, &stream);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "imp audio encode release stream failed\n");
				return NULL;
			}
		#else
			// dp("frm.len : %d buf size : %d timestamp : %lld\n", frm.len, AUDIO_SAMPLE_BUF_SIZE, frm.timeStamp);

			if ((frm.len > 0) && (stream_state == 1)) {
				pthread_mutex_lock(&buffMutex_ai);
				if (AI_Cir_Buff.RIndex == AI_Cir_Buff.WIndex) {
					buff_space = 500*1024;
					AI_Cir_Buff.RIndex = 0;
					AI_Cir_Buff.WIndex = 0;
				}
				else {
					if (AI_Cir_Buff.WIndex >= AI_Cir_Buff.RIndex)
	                	datasize = (AI_Cir_Buff.WIndex - AI_Cir_Buff.RIndex) % (500*1024);
	            	else
		                datasize = 0;
					buff_space = (500*1024) - datasize;	
				}
				
				if (buff_space >= frm.len) {
					buff_u8 = (uint8_t*)frm.virAddr;
					// for(int j = 0; j < frm.len; ++j) {
					// 	AI_Cir_Buff.tx[AI_Cir_Buff.WIndex] = buff_u8[j];
					// 	AI_Cir_Buff.WIndex = (AI_Cir_Buff.WIndex+1) % (500*1024);
					// 	if (AI_Cir_Buff.WIndex == AI_Cir_Buff.RIndex) {
					// 		AI_Cir_Buff.RIndex = (AI_Cir_Buff.RIndex+1) % (500*1024);
					// 	}
					// }
					memset (&AI_Cir_Buff.tx[AI_Cir_Buff.WIndex], 0, frm.len);
					memcpy (&AI_Cir_Buff.tx[AI_Cir_Buff.WIndex], buff_u8, frm.len);
					// ret = write(save_fd, &AI_Cir_Buff.tx[AI_Cir_Buff.WIndex], frm.len);
					AI_Cir_Buff.WIndex = (AI_Cir_Buff.WIndex+frm.len);// % (500*1024);
					// dp("[CIR_BUFF Audio In]buff_space:%d WIndex:%d RIndex%d\n", buff_space, AI_Cir_Buff.WIndex, AI_Cir_Buff.RIndex);
				}
				else {
					dp("AI Cir Buff Overflow!1\n");
				}
				pthread_mutex_unlock(&buffMutex_ai);
			}
		#endif

		/* step 8. release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(ai_devID, ai_chnID, &frm);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio release frame data error\n");
			return NULL;
		}

	}while(!bStrem);

	dp("[Audio] AI Thread END!\n");
	pthread_exit(0);
}

extern int64_t sample_gettimeus(void);
extern void amp_on(void);
extern void amp_off(void);

void *IMP_Audio_Play_Thread(void *argv)
{
	unsigned char *buf = NULL;
	int ret = -1;
	int datasize = 0, definesize = 0;
	// int save_fd = 0;
    // save_fd = open("/tmp/mnt/sdcard/save_ao.g726", O_RDWR | O_CREAT | O_TRUNC, 0777);

	buf = (unsigned char *)malloc(1024);
	if (buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return NULL;
	}

	#ifdef __G726__
		int adChn = 0;
		IMPAudioDecChnAttr attr;
		attr.type = Encode_Type;
		attr.bufSize = 20;
		attr.mode = ADEC_MODE_STREAM;
		ret = IMP_ADEC_CreateChn(adChn, &attr);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "imp audio decoder create channel failed\n");
			return NULL;
		}

		ret = IMP_ADEC_ClearChnBuf(adChn);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_ADEC_ClearChnBuf failed\n");
			return NULL;
		}

		definesize = AUDIO_SAMPLE_BUF_SIZE/4;
	#else
		definesize = 320;
	#endif

	dp("Data Play Size : %d\n", definesize);

	IMPAudioOChnState play_status;
	play_status.chnBusyNum = 0;

	bool asflg = false;
	uint64_t as_time = 0;
	// bool ao_start_f = false;
	// int64_t ao_start_t = sample_gettimeus();

	do {
		if (bExit) break;

		if (ao_clear_flag) {
			dp("Clear Flag!!\n");
			ao_clear_flag = false;
			ret = IMP_AO_ClearChnBuf(ao_devID, ao_chnID);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_AO_ClearChnBuf error\n");
				return NULL;
			}
			AO_Cir_Buff.RIndex = AO_Cir_Buff.WIndex = 0;
			asflg = false;
		}

		ret = IMP_AO_QueryChnStat(ao_devID, ao_chnID, &play_status);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_AO_QueryChnStat error\n");
			dp("AO IMP_AO_QueryChnStat\n");
			return NULL;
		}

		// if (play_status.chnBusyNum) dp("buf busy : %d\n", play_status.chnBusyNum);

		if (play_status.chnBusyNum < 20) {
			pthread_mutex_lock(&buffMutex_ao);
			if (AO_Cir_Buff.RIndex != AO_Cir_Buff.WIndex) {
				asflg = true;
				as_time = sample_gettimeus();
				datasize = (AO_Cir_Buff.WIndex - AO_Cir_Buff.RIndex + A_BUFF_SIZE) % (500*1024);
				if (!audio_start_flag) {
					// dp("DataSize : %d > %d\n", datasize, definesize*20);
					#ifdef __G726__
						if (datasize > definesize*50){
							dp("AO Start!\n");
							audio_start_flag = true;
							ret = IMP_AO_ClearChnBuf(ao_devID, ao_chnID);
							if (ret != 0) {
								IMP_LOG_ERR(TAG, "IMP_AO_ClearChnBuf error\n");
								return NULL;
							}
							
						}
						datasize = 0;
					#else
						if (datasize > definesize*100){
							dp("AO Start!\n");
							audio_start_flag = true;
							ret = IMP_AO_ClearChnBuf(ao_devID, ao_chnID);
							if (ret != 0) {
								IMP_LOG_ERR(TAG, "IMP_AO_ClearChnBuf error\n");
								return NULL;
							}
						}
						datasize = 0;
					#endif
				}
				else if (datasize >= definesize) {
				// if (datasize >= definesize) {
					// datasize = datasize > AUDIO_SAMPLE_BUF_SIZE) ? AUDIO_SAMPLE_BUF_SIZE : datasize;
					// dp("[AO] DS:%d WI:%d RI:%d\n", datasize, AO_Cir_Buff.WIndex, AO_Cir_Buff.RIndex);
					datasize = definesize;
					memset (buf, 0, datasize);
					memcpy (buf, &AO_Cir_Buff.tx[AO_Cir_Buff.RIndex], datasize);
					AO_Cir_Buff.RIndex = (AO_Cir_Buff.RIndex+datasize) % (500*1024);
					if (AO_Cir_Buff.RIndex == AO_Cir_Buff.WIndex) {
						// dp("Buffer Clear!! : %d\n", datasize);
						AO_Cir_Buff.RIndex = AO_Cir_Buff.WIndex = 0;
					}
				}
				else if (datasize != 0){
					datasize = definesize;
					memset(buf, 0, datasize);
					memcpy(buf, &AO_Cir_Buff.tx[AO_Cir_Buff.RIndex], datasize);
					AO_Cir_Buff.RIndex = AO_Cir_Buff.WIndex = 0;
					audio_start_flag = false;
					dp("AO reinit!\n");
				}
			}
			pthread_mutex_unlock(&buffMutex_ao);
		}
		else {
			dp("AO Busy : %d\n", play_status.chnBusyNum);
			continue;
		}

		if (play_status.chnBusyNum != 0 && asflg)  {
			if ((sample_gettimeus() - as_time) > 200000){
				// dp("Audio Dummy Data Set!!\n");
				memset (buf, 0, definesize);
				datasize = definesize;
				as_time = sample_gettimeus();
			}
		}

		if (play_status.chnBusyNum == 0 && asflg) {
			// dp("Audio Chn Busy Clear!!\n");
			asflg = false;
		}

		if (datasize > 0 && play_status.chnBusyNum < 20) {
			#ifdef __G726__
				IMPAudioStream stream_in;
				stream_in.stream = (uint8_t *)buf;
				stream_in.len = datasize;
				ret = IMP_ADEC_SendStream(adChn, &stream_in, BLOCK);
				if(ret != 0) {
					IMP_LOG_ERR(TAG, "imp audio encode send frame failed\n");
					dp("ADEC IMP_ADEC_SendStream!\n");
					return NULL;
				}

				/* get audio decoder frame. */
				IMPAudioStream stream_out;
				ret = IMP_ADEC_PollingStream(adChn, 10000);
				if(ret != 0) {
					IMP_LOG_ERR(TAG, "imp audio encode polling stream failed\n");
					dp("ADEC IMP_ADEC_PollingStream!\n");
					return NULL;
				}

				ret = IMP_ADEC_GetStream(adChn, &stream_out, BLOCK);
				if(ret != 0) {
					IMP_LOG_ERR(TAG, "imp audio decoder get stream failed\n");
					dp("ADEC IMP_ADEC_GetStream!\n");
					return NULL;
				}

				

				/* Step 5: send frame data. */
				IMPAudioFrame frm;
				frm.virAddr = (uint32_t *)stream_out.stream;
				frm.len = stream_out.len;
			#else
				IMPAudioFrame frm;
				frm.virAddr = (uint32_t *)buf;
				frm.len = datasize;
			#endif

			// dp("AO DS : %d len : %d ChnBusy : %d\n", datasize, frm.len, play_status.chnBusyNum);
			
			ret = IMP_AO_SendFrame(ao_devID, ao_chnID, &frm, NOBLOCK);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "send Frame Data error\n");
				dp("AO IMP_AO_SendFrame\n");
				return NULL;
			}
			else {
				// udp_ao_rolling_dcnt();
				// usleep(20*1000);
				// dp("[AO] sindex:%d windex:%d dcnt:%d\n", Audio_Ao_Attr.sindex, Audio_Ao_Attr.windex, Audio_Ao_Attr.dcnt);
				// ret = write(save_fd, buf, datasize);
				datasize = 0;
			}
			
			#ifdef __G726__
				/* release stream. */
				ret = IMP_ADEC_ReleaseStream(adChn, &stream_out);
				if(ret != 0) {
					IMP_LOG_ERR(TAG, "imp audio decoder release stream failed\n");
					return NULL;
				}
			#endif

			// usleep(10*1000);
			IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
					play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);

		}

	}while(!bStrem);

	// close(save_fd);
	dp("[Audio] AO Thread END!\n");
	free(buf);
	pthread_exit(0);
}

void *IMP_Audio_Play_Thread_pcm(void *argv)
{
	unsigned char *buf = NULL;
	int ret = -1;
	int datasize = 0, definesize = 0;
	static bool amp_f = false;
	int amp_c = 0;

	// int save_fd = 0;
    // save_fd = open("/tmp/mnt/sdcard/save_ao.g726", O_RDWR | O_CREAT | O_TRUNC, 0777);

	buf = (unsigned char *)malloc(1024);
	if (buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return NULL;
	}

	definesize = 320;

	dp("Data Play Size : %d\n", definesize);

	IMPAudioOChnState play_status;
	play_status.chnBusyNum = 0;

	bool asflg = false;
	uint64_t as_time = 0;
	// bool ao_start_f = false;
	// int64_t ao_start_t = sample_gettimeus();

	do {
		if (bExit) break;

		if (ao_clear_flag) {
			dp("Clear Flag!!\n");
			ao_clear_flag = false;
			ret = IMP_AO_ClearChnBuf(ao_devID, ao_chnID);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "IMP_AO_ClearChnBuf error\n");
				return NULL;
			}
			AO_Cir_Buff.RIndex = AO_Cir_Buff.WIndex = 0;
			asflg = false;
		}

		ret = IMP_AO_QueryChnStat(ao_devID, ao_chnID, &play_status);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_AO_QueryChnStat error\n");
			dp("AO IMP_AO_QueryChnStat\n");
			return NULL;
		}

		// if (play_status.chnBusyNum) dp("buf busy : %d\n", play_status.chnBusyNum);

		if (play_status.chnBusyNum < 20) {
			pthread_mutex_lock(&buffMutex_ao);
			if (AO_Cir_Buff.RIndex != AO_Cir_Buff.WIndex) {
				asflg = true;
				as_time = sample_gettimeus();
				datasize = (AO_Cir_Buff.WIndex - AO_Cir_Buff.RIndex + A_BUFF_SIZE) % (500*1024);
				if (!audio_start_flag) {
					// dp("DataSize : %d > %d\n", datasize, definesize*20);
					if (datasize > definesize*100){
						dp("AO Start!\n");
						audio_start_flag = true;
						ret = IMP_AO_ClearChnBuf(ao_devID, ao_chnID);
						if (ret != 0) {
							IMP_LOG_ERR(TAG, "IMP_AO_ClearChnBuf error\n");
							return NULL;
						}
					}
					datasize = 0;
				}
				else if (datasize >= definesize) {
				// if (datasize >= definesize) {
					// datasize = datasize > AUDIO_SAMPLE_BUF_SIZE) ? AUDIO_SAMPLE_BUF_SIZE : datasize;
					// dp("[AO] DS:%d WI:%d RI:%d\n", datasize, AO_Cir_Buff.WIndex, AO_Cir_Buff.RIndex);
					datasize = definesize;
					memset (buf, 0, datasize);
					memcpy (buf, &AO_Cir_Buff.tx[AO_Cir_Buff.RIndex], datasize);
					AO_Cir_Buff.RIndex = (AO_Cir_Buff.RIndex+datasize) % (500*1024);
					if (AO_Cir_Buff.RIndex == AO_Cir_Buff.WIndex) {
						// dp("Buffer Clear!! : %d\n", datasize);
						AO_Cir_Buff.RIndex = AO_Cir_Buff.WIndex = 0;
					}
				}
				else if (datasize != 0){
					datasize = definesize;
					memset(buf, 0, datasize);
					memcpy(buf, &AO_Cir_Buff.tx[AO_Cir_Buff.RIndex], datasize);
					AO_Cir_Buff.RIndex = AO_Cir_Buff.WIndex = 0;
					audio_start_flag = false;
					// dp("AO reinit!\n");
				}
			}
			pthread_mutex_unlock(&buffMutex_ao);
		}
		else {
			// dp("AO Busy : %d\n", play_status.chnBusyNum);
			continue;
		}

		if (play_status.chnBusyNum != 0 && asflg)  {
			if ((sample_gettimeus() - as_time) > 200000){
				// dp("Audio Dummy Data Set!!\n");
				memset (buf, 0, definesize);
				datasize = definesize;
				as_time = sample_gettimeus();
			}
		}

		if (play_status.chnBusyNum == 0 && asflg) {
			// dp("Audio Chn Busy Clear!!\n");
			asflg = false;
		}

		if (datasize > 0 && play_status.chnBusyNum < 20) {
			IMPAudioFrame frm;
			frm.virAddr = (uint32_t *)buf;
			frm.len = datasize;

			// dp("AO DS : %d len : %d ChnBusy : %d\n", datasize, frm.len, play_status.chnBusyNum);
			
			ret = IMP_AO_SendFrame(ao_devID, ao_chnID, &frm, NOBLOCK);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "send Frame Data error\n");
				dp("AO IMP_AO_SendFrame\n");
				return NULL;
			}
			else {
				// udp_ao_rolling_dcnt();
				// usleep(20*1000);
				// dp("[AO] sindex:%d windex:%d dcnt:%d\n", Audio_Ao_Attr.sindex, Audio_Ao_Attr.windex, Audio_Ao_Attr.dcnt);
				// ret = write(save_fd, buf, datasize);
				datasize = 0;
			}


			if (!amp_f) {
				if (amp_c == 20) {
					Get_Vol();
					Set_Vol(-30,0,-30,0);
					Set_Mute(0);
					amp_off();
				}
				else if (amp_c == 22) {
					amp_f = true;
					amp_on();
					Set_Mute(1);
					Set_Vol(90,30,get_out_vol,get_out_gain);
				}
				amp_c++;
			}

			// usleep(10*1000);
			IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
					play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);

		}

	}while(!bStrem);

	// close(save_fd);
	dp("[Audio] AO Thread END!\n");
	free(buf);
	pthread_exit(0);
}

void *IMP_Audio_Play_Thread_g726(void *argv)
{
	unsigned char *buf = NULL;
	int ret = -1;
	int datasize = 0, definesize = 0;
	// int save_fd = 0;
    // save_fd = open("/tmp/mnt/sdcard/save_ao.g726", O_RDWR | O_CREAT | O_TRUNC, 0777);

	buf = (unsigned char *)malloc(1024);
	if (buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return NULL;
	}

	int adChn = 0;
	IMPAudioDecChnAttr attr;
	attr.type = Encode_Type;
	attr.bufSize = 20;
	attr.mode = ADEC_MODE_STREAM;
	ret = IMP_ADEC_CreateChn(adChn, &attr);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "imp audio decoder create channel failed\n");
		return NULL;
	}

	ret = IMP_ADEC_ClearChnBuf(adChn);
	if(ret != 0) {
		IMP_LOG_ERR(TAG, "IMP_ADEC_ClearChnBuf failed\n");
		return NULL;
	}

	definesize = AUDIO_SAMPLE_BUF_SIZE/4;
	

	dp("Data Play Size : %d\n", definesize);

	IMPAudioOChnState play_status;
	play_status.chnBusyNum = 0;

	bool asflg = false;
	uint64_t as_time = 0;
	// bool ao_start_f = false;
	// int64_t ao_start_t = sample_gettimeus();

	do {
		if (bExit) break;

		ret = IMP_AO_QueryChnStat(ao_devID, ao_chnID, &play_status);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "IMP_AO_QueryChnStat error\n");
			dp("AO IMP_AO_QueryChnStat\n");
			return NULL;
		}

		// if (play_status.chnBusyNum) dp("buf busy : %d\n", play_status.chnBusyNum);

		if (play_status.chnBusyNum < 20) {
			pthread_mutex_lock(&buffMutex_ao);
			if (AO_Cir_Buff.RIndex != AO_Cir_Buff.WIndex) {
				asflg = true;
				as_time = sample_gettimeus();
				datasize = (AO_Cir_Buff.WIndex - AO_Cir_Buff.RIndex + A_BUFF_SIZE) % (500*1024);
				if (!audio_start_flag) {
					// dp("DataSize : %d > %d\n", datasize, definesize*20);
					if (datasize > definesize*50){
						dp("AO Start!\n");
						audio_start_flag = true;
						ret = IMP_AO_ClearChnBuf(ao_devID, ao_chnID);
						if (ret != 0) {
							IMP_LOG_ERR(TAG, "IMP_AO_ClearChnBuf error\n");
							return NULL;
						}
					}
					datasize = 0;
				}
				else if (datasize >= definesize) {
				// if (datasize >= definesize) {
					// datasize = datasize > AUDIO_SAMPLE_BUF_SIZE) ? AUDIO_SAMPLE_BUF_SIZE : datasize;
					// dp("[AO] DS:%d WI:%d RI:%d\n", datasize, AO_Cir_Buff.WIndex, AO_Cir_Buff.RIndex);
					datasize = definesize;
					memset (buf, 0, datasize);
					memcpy (buf, &AO_Cir_Buff.tx[AO_Cir_Buff.RIndex], datasize);
					AO_Cir_Buff.RIndex = (AO_Cir_Buff.RIndex+datasize) % (500*1024);
					if (AO_Cir_Buff.RIndex == AO_Cir_Buff.WIndex) {
						// dp("Buffer Clear!! : %d\n", datasize);
						AO_Cir_Buff.RIndex = AO_Cir_Buff.WIndex = 0;
					}
				}
				else if (datasize != 0){
					datasize = definesize;
					memset(buf, 0, datasize);
					memcpy(buf, &AO_Cir_Buff.tx[AO_Cir_Buff.RIndex], datasize);
					AO_Cir_Buff.RIndex = AO_Cir_Buff.WIndex = 0;
					audio_start_flag = false;
					dp("AO reinit!\n");
				}
			}
			pthread_mutex_unlock(&buffMutex_ao);
		}
		else {
			dp("AO Busy : %d\n", play_status.chnBusyNum);
			continue;
		}

		if (play_status.chnBusyNum != 0 && asflg)  {
			if ((sample_gettimeus() - as_time) > 200000){
				// dp("Audio Dummy Data Set!!\n");
				memset (buf, 0, definesize);
				datasize = definesize;
				as_time = sample_gettimeus();
			}
		}

		if (play_status.chnBusyNum == 0 && asflg) {
			// dp("Audio Chn Busy Clear!!\n");
			asflg = false;
		}

		if (datasize > 0 && play_status.chnBusyNum < 20) {
			IMPAudioStream stream_in;
			stream_in.stream = (uint8_t *)buf;
			stream_in.len = datasize;
			ret = IMP_ADEC_SendStream(adChn, &stream_in, BLOCK);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "imp audio encode send frame failed\n");
				dp("ADEC IMP_ADEC_SendStream!\n");
				return NULL;
			}

			/* get audio decoder frame. */
			IMPAudioStream stream_out;
			ret = IMP_ADEC_PollingStream(adChn, 10000);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "imp audio encode polling stream failed\n");
				dp("ADEC IMP_ADEC_PollingStream!\n");
				return NULL;
			}

			ret = IMP_ADEC_GetStream(adChn, &stream_out, BLOCK);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "imp audio decoder get stream failed\n");
				dp("ADEC IMP_ADEC_GetStream!\n");
				return NULL;
			}

			

			/* Step 5: send frame data. */
			IMPAudioFrame frm;
			frm.virAddr = (uint32_t *)stream_out.stream;
			frm.len = stream_out.len;
			
			// dp("AO DS : %d len : %d ChnBusy : %d\n", datasize, frm.len, play_status.chnBusyNum);
			
			ret = IMP_AO_SendFrame(ao_devID, ao_chnID, &frm, NOBLOCK);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "send Frame Data error\n");
				dp("AO IMP_AO_SendFrame\n");
				return NULL;
			}
			else {
				// udp_ao_rolling_dcnt();
				// usleep(20*1000);
				// dp("[AO] sindex:%d windex:%d dcnt:%d\n", Audio_Ao_Attr.sindex, Audio_Ao_Attr.windex, Audio_Ao_Attr.dcnt);
				// ret = write(save_fd, buf, datasize);
				datasize = 0;
			}
			
			/* release stream. */
			ret = IMP_ADEC_ReleaseStream(adChn, &stream_out);
			if(ret != 0) {
				IMP_LOG_ERR(TAG, "imp audio decoder release stream failed\n");
				return NULL;
			}

			// usleep(10*1000);
			IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
					play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);

		}

	}while(!bStrem);

	// close(save_fd);
	dp("[Audio] AO Thread END!\n");
	free(buf);
	pthread_exit(0);
}

void ao_file_play_thread(void *argv)
{
	unsigned char *buf = NULL;
	int size = 0;
	int ret = -1;
	int old_chnbusy=0, old_busy_cnt=0;
	bool stop_flag = false;
	int amp_c = 0;
	static bool amp_f = false;
	// int total = 0;

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

	size = fread(buf, 1, 80, play_file);
	if (size < 80)
		return;
	else dp("wav header read!!\n");

	do {
		if (bExit) break;

		if (!stop_flag) {
			size = fread(buf, 1, AUDIO_SAMPLE_BUF_SIZE, play_file);
			// if (size < AUDIO_SAMPLE_BUF_SIZE)
				// break;
			if (size == 0)
				break;

			/* Step 5: send frame data. */
			IMPAudioFrame frm;
			frm.virAddr = (uint32_t *)buf;
			frm.len = size;
			ret = IMP_AO_SendFrame(ao_devID, ao_chnID, &frm, BLOCK);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "send Frame Data error\n");
				return;
			}
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
					dp("busynum:%d\n", play_status.chnBusyNum);
				}
				if (play_status.chnBusyNum > 18) stop_flag = true;
				else if (play_status.chnBusyNum > 5) stop_flag = false;
			}
		}

		usleep(18*1000);

		if (!amp_f) {
			if (amp_c == 20) {
				Get_Vol();
				Set_Vol(-30,0,-30,0);
				Set_Mute(0);
				amp_off();
			}
			else if (amp_c == 22) {
				amp_f = true;
				amp_on();
				Set_Mute(1);
				Set_Vol(90,30,get_out_vol,get_out_gain);
			}
			amp_c++;
		}

		IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
				play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);
	}while (1);

	Set_Vol(90,30,spk_vol_buf,spk_gain_buf);

	dp("[Audio File] Thread End!\n");
	fclose(play_file);
	free(buf);
	
	// pthread_exit(0);
}

void ao_file_play_thread_mute(void *argv)
{
	unsigned char *buf = NULL;
	int size = 0;
	int ret = -1;
	int old_chnbusy=0, old_busy_cnt=0;
	bool stop_flag = false;
	// int total = 0;

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

	size = fread(buf, 1, 80, play_file);
	if (size < 80)
		return;
	else dp("wav header read!!\n");

	amp_off();

	do {
		if (bExit) break;

		if (!stop_flag) {
			size = fread(buf, 1, AUDIO_SAMPLE_BUF_SIZE, play_file);
			// if (size < AUDIO_SAMPLE_BUF_SIZE)
				// break;
			if (size == 0)
				break;

			/* Step 5: send frame data. */
			IMPAudioFrame frm;
			frm.virAddr = (uint32_t *)buf;
			frm.len = size;
			ret = IMP_AO_SendFrame(ao_devID, ao_chnID, &frm, BLOCK);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "send Frame Data error\n");
				return;
			}
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
					dp("busynum:%d\n", play_status.chnBusyNum);
				}
				if (play_status.chnBusyNum > 18) stop_flag = true;
				else if (play_status.chnBusyNum > 5) stop_flag = false;
			}
		}

		usleep(18*1000);


		IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
				play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);
	}while (1);

	dp("[Audio File] Thread End!\n");
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

	dp("[Audio File] Thread End!\n");
	fclose(play_file);
	free(buf);
	// pthread_exit(0);
}


    

void IMP_Audio_Test_In_Thread(void)
{
	int ret = -1;
	int test_fd = 0;
	uint8_t *buf_pcm = NULL;
	int64_t mic_test_time = 0;

	test_fd = open("/dev/shm/mictest.pcm", O_RDWR | O_CREAT | O_TRUNC, 0777);

	buf_pcm = malloc(AUDIO_SAMPLE_BUF_SIZE*2);
	if(buf_pcm == NULL) {
		IMP_LOG_ERR(TAG, "malloc audio pcm buf error\n");
		return;
	}

	mic_test_time = sample_gettimeus();
	do {

		// dp("Test Audio Record Thread!! %lld\n", );
		/* step 6. get audio record frame. */
		ret = IMP_AI_PollingFrame(ai_devID, ai_chnID, 1000);
		if (ret != 0 ) {
			IMP_LOG_ERR(TAG, "Audio Polling Frame Data error\n");
		}
		IMPAudioFrame frm;
		ret = IMP_AI_GetFrame(ai_devID, ai_chnID, &frm, NOBLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
			return;
		}
	
		// dp("frm.len : %d buf size : %d timestamp : %lld\n", frm.len, AUDIO_SAMPLE_BUF_SIZE, frm.timeStamp);

		ret = write(test_fd, frm.virAddr, frm.len);

		/* step 8. release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(ai_devID, ai_chnID, &frm);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio release frame data error\n");
			return;
		}

		if ((sample_gettimeus() - mic_test_time) > 10000000) {
			break;
		}

	}while(!bStrem);

	close(test_fd);

	dp("[MIC_Record] END!\n");
	// pthread_exit(0);

}

void IMP_Audio_Test_Out_Thread(void)
{
	unsigned char *buf = NULL;
	int size = 0;
	int ret = -1;
	int old_chnbusy=0, old_busy_cnt=0;
	bool stop_flag = false;
	// int total = 0;

	buf = (unsigned char *)malloc(AUDIO_SAMPLE_BUF_SIZE);
	if (buf == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: malloc audio buf error\n", __func__);
		return;
	}

	FILE *play_file = fopen("/dev/shm/mictest.pcm", "rb");
	if (play_file == NULL) {
		IMP_LOG_ERR(TAG, "[ERROR] %s: fopen %s failed\n", __func__, "/dev/shm/mictest.pcm");
		return;
	}

	amp_on();

	do {
		if (bExit) break;

		if (!stop_flag) {
			size = fread(buf, 1, AUDIO_SAMPLE_BUF_SIZE, play_file);
			// if (size < AUDIO_SAMPLE_BUF_SIZE)
				// break;
			if (size == 0)
				break;

			/* Step 5: send frame data. */
			IMPAudioFrame frm;
			frm.virAddr = (uint32_t *)buf;
			frm.len = size;
			ret = IMP_AO_SendFrame(ao_devID, ao_chnID, &frm, BLOCK);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "send Frame Data error\n");
				return;
			}
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
					dp("busynum:%d\n", play_status.chnBusyNum);
				}
				if (play_status.chnBusyNum > 18) stop_flag = true;
				else if (play_status.chnBusyNum > 5) stop_flag = false;
			}
		}

		usleep(18*1000);

		IMP_LOG_INFO(TAG, "Play: TotalNum %d, FreeNum %d, BusyNum %d\n",
				play_status.chnTotalNum, play_status.chnFreeNum, play_status.chnBusyNum);
	}while (1);

	Set_Vol(90,30,spk_vol_buf,spk_gain_buf);

	dp("[Audio File] Thread End!\n");
	fclose(play_file);
	free(buf);
	

}



void IMP_Audio_Test_InOut_Thread(void)
{
	int ret = -1;
	int64_t mic_test_time = 0;

	// int size = 0;
	int old_chnbusy=0, old_busy_cnt=0;
	bool stop_flag = false;

	mic_test_time = sample_gettimeus();
	do {

		// dp("Test Audio Record Thread!! %lld\n", );
		/* step 6. get audio record frame. */
		ret = IMP_AI_PollingFrame(ai_devID, ai_chnID, 1000);
		if (ret != 0 ) {
			IMP_LOG_ERR(TAG, "Audio Polling Frame Data error\n");
		}
		IMPAudioFrame frm;
		ret = IMP_AI_GetFrame(ai_devID, ai_chnID, &frm, NOBLOCK);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio Get Frame Data error\n");
			return;
		}

		if (!stop_flag) {
			/* Step 5: send frame data. */
			ret = IMP_AO_SendFrame(ao_devID, ao_chnID, &frm, BLOCK);
			if (ret != 0) {
				IMP_LOG_ERR(TAG, "send Frame Data error\n");
				return;
			}
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
					dp("busynum:%d\n", play_status.chnBusyNum);
				}
				if (play_status.chnBusyNum > 18) stop_flag = true;
				else if (play_status.chnBusyNum > 5) stop_flag = false;
			}
		}

		// dp("frm.len : %d buf size : %d timestamp : %lld\n", frm.len, AUDIO_SAMPLE_BUF_SIZE, frm.timeStamp);

		/* step 8. release the audio record frame. */
		ret = IMP_AI_ReleaseFrame(ai_devID, ai_chnID, &frm);
		if(ret != 0) {
			IMP_LOG_ERR(TAG, "Audio release frame data error\n");
			return;
		}

		// if ((sample_gettimeus() - mic_test_time) > 10000000) {
		// 	break;
		// }

	}while(!BMicT);

	dp("[MIC_Record] END!\n");
	// pthread_exit(0);

}

