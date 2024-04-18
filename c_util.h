#ifndef __C_UTIL_H__
#define __C_UTIL_H__


#ifdef __cplusplus
extern "C" {
#endif

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

#if __cplusplus
}
#endif

#endif /* __AUDIO_H__ __C_UTIL_H__ */