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

#if __cplusplus
}
#endif

#endif /* __AUDIO_H__ __C_UTIL_H__ */