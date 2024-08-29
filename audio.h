#ifndef __AUDIO_H__
#define __AUDIO_H__


#ifdef __cplusplus
extern "C" {
#endif

#define WRITE_FILE

int Init_Audio_In(void);
int Deinit_Audio_In(void);
int Init_Audio_Out(void);
int Deinit_Audio_Out(void);
int Set_Vol(int ai_vol, int ai_gain, int ao_vol, int ao_gain);
int Get_Vol(void);

void *IMP_Audio_Record_AEC_Thread(void *argv);
void *IMP_Audio_Play_Thread(void *argv);
void *IMP_Audio_Play_Thread_pcm(void *argv);
void *IMP_Audio_Play_Thread_g726(void *argv);
void ao_file_play_thread(void *argv);
void ao_file_play_thread_mute(void *argv);
void ao_file_play_pcm_thread(void *argv);
void IMP_Audio_Test_In_Thread(void);
void IMP_Audio_Test_Out_Thread(void);
void IMP_Audio_Test_InOut_Thread(void);

#if __cplusplus
}
#endif

#endif /* __AUDIO_H__ */