#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/types.h>

void ST_Flush(void)
{
	char c;
	while((c = getchar()) != '\n' && c != EOF);
}

int64_t sample_gettimeus(void)
{
    struct timeval sttime;
    gettimeofday(&sttime,NULL);
    return (sttime.tv_sec  * 1000000 + (sttime.tv_usec));
}

// 동적 할당된 자원 해제 함수
void freeFileList(char** fileList, int fileCount) {
    for (int i = 0; i < fileCount; ++i) {
        free(fileList[i]);
    }
    free(fileList);
}

// JPG 파일 리스트 생성 함수
char** listJPGFiles(const char* folderPath, int* fileCount) {
    DIR* dir;
    struct dirent* entry;
    int count = 0;

    // 디렉터리 열기
    if ((dir = opendir(folderPath)) == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
    }

    // 파일 개수 세기
    while ((entry = readdir(dir)) != NULL) {
        if (strlen(entry->d_name) > 4 &&
            strcmp(entry->d_name + strlen(entry->d_name) - 4, ".jpg") == 0) {
            count++;
        }
    }

    // 메모리 할당
    char** fileList = (char**)malloc(count * sizeof(char*));
    if (fileList == NULL) {
        perror("Memory allocation error");
        closedir(dir);
        exit(EXIT_FAILURE);
    }

    // 다시 디렉터리를 열어서 파일 이름 저장
    rewinddir(dir);
    count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strlen(entry->d_name) > 4 &&
            strcmp(entry->d_name + strlen(entry->d_name) - 4, ".jpg") == 0) {
            fileList[count] = (char*)malloc(strlen(folderPath) + strlen(entry->d_name) + 2);
            if (fileList[count] == NULL) {
                perror("Memory allocation error");
                closedir(dir);
                freeFileList(fileList, count);
                exit(EXIT_FAILURE);
            }

            sprintf(fileList[count], "%s/%s", folderPath, entry->d_name);
            count++;
        }
    }

    closedir(dir);
    *fileCount = count;
    return fileList;
}

// 파일 리스트 출력 함수
void displayFileList(char** fileList, int fileCount) {
    printf("JPG Files in the Folder:\n");
    for (int i = 0; i < fileCount; ++i) {
        printf("[%d] %s\n", i, fileList[i]);
    }
}

