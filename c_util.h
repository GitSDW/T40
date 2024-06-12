#ifndef __C_UTIL_H__
#define __C_UTIL_H__


#ifdef __cplusplus
extern "C" {
#endif

// typedef struct  {
//     char name[15][128];
//     uint8_t cnt;
// }SaveFile_cutil;

void ST_Flush(void);
int64_t sample_gettimeus(void);
char** listJPGFiles(const char* folderPath, int* fileCount);
void displayFileList(char** fileList, int fileCount);
void freeFileList(char** fileList, int fileCount);
int FileShow(const char* directory, const char* filename);
int SetTime(int year, int month, int day, int hour, int min, int sec);
int GetTime(void);
void record_shutdown_time();
void set_system_time_from_file();
int file_ck(const char* filename);
int old_file_del(void);
int file_name_get(SaveFile *filelist, int topbot);
int check_delay_time(void);
void set_delay_time(int64_t ti);


#if __cplusplus
}
#endif

#endif /* __AUDIO_H__ __C_UTIL_H__ */