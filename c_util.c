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
#include <time.h>

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

char** listFilesInDirectory(const char* directory, const char* filename) {
    DIR *dir;
    struct dirent *entry;
    char **fileList = malloc(15 * sizeof(char*));
    int count = 0;
    int filecmp_cnt = 10;

    if ((dir = opendir(directory)) == NULL) {
        perror("opendir");
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // 디렉토리가 아니라면
            filecmp_cnt = strcmp(entry->d_name, filename);
            if (filecmp_cnt == 95) {
                // printf("%s %s %d\n", entry->d_name, filename, filecmp_cnt);
                fileList[count] = malloc(strlen(entry->d_name) + 1);
                strcpy(fileList[count], entry->d_name);
                count++;
            }
        }
    }

    closedir(dir);
    fileList[count] = NULL; // 리스트 끝을 나타내는 NULL 추가
    return fileList;
}

int FileShow(const char* directory, const char* filename) {
    int ret = -1, cnt = 0, result = 0;
    char file_name[20];
    char **files = listFilesInDirectory(directory, filename);
    if (files == NULL) {
        fprintf(stderr, "Failed to list files in directory: %s\n", directory);
        return -1;
    }

    // 파일 리스트 출력
    for (int i = 0; files[i] != NULL; ++i) {
        printf("%s\n", files[i]);
        for (int j=10; j > 0; j--) {
            sprintf(file_name, "%s_%d.mp4", filename, j);
            ret = strcmp(files[i], file_name);
            if (ret == 0) {
                printf("%s[%d]:%d %s %s\n", __func__, i, ret, files[i], file_name);
                if (result < j) {
                    result = j;
                }
            }
        }
        free(files[i]);
        cnt++;
    }
    free(files);
    return result;
}


int SetTime(int year, int month, int day, int hour, int min, int sec) {
    struct tm *local_time;
    time_t t;

    // 현재 시간을 가져옴
    t = time(NULL);
    local_time = localtime(&t);

    if (local_time == NULL) {
        perror("시간 가져오기 실패");
        return 1;
    }
    setenv("TZ", "Asia/Seoul", 1);

    tzset();

    // 시간을 설정할 값으로 변경
    // printf("%s : %d %d %d %d %d %d\n", __func__, year, month, day, hour, min, sec);
    local_time->tm_year = year - 1900; // 년도는 1900년을 기준으로 설정
    local_time->tm_mon  = month - 1; // 월은 0부터 시작하므로 4월은 3
    local_time->tm_mday = day;
    local_time->tm_hour = hour;
    local_time->tm_min  = min;
    local_time->tm_sec  = sec;

    t = mktime(local_time);
    // 설정한 시간을 시스템 시간으로 설정
    if (stime(&t) != 0) {
    // if (stime((time_t *)local_time) != 0) {
    // if (stime(&current_time) != 0) {
        perror("Time Set Fail!");
        return 1;
    }

    printf("Set Time: %s", asctime(local_time));

    return 0;
}

int GetTime() {
    time_t current_time;
    struct tm *local_time;

    // 현재 시간 가져오기
    time(&current_time);
    local_time = localtime(&current_time);

    // 시간 출력
    printf("Now Time: %s", asctime(local_time));

    return 0;
}

#define SHUTDOWN_FILE "/tmp/mnt/sdcard/shutdown_time.txt"

// 종료 시간을 파일에 기록하는 함수
void record_shutdown_time() {
    time_t current_time;
    FILE *file;

    // 현재 시간 가져오기
    time(&current_time);

    // 파일 열기
    file = fopen(SHUTDOWN_FILE, "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", SHUTDOWN_FILE);
        exit(1);
    }

    // 시간 파일에 기록
    fprintf(file, "%ld\n", (long)current_time);
    fclose(file);
}

// 부팅 시간 파일에서 시간을 읽어와 시스템 시간 설정하는 함수
void set_system_time_from_file() {
    time_t startup_time;
    FILE *file;

    // 파일 열기
    file = fopen(SHUTDOWN_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file %s\n", SHUTDOWN_FILE);
        SetTime(2024, 4, 5, 11, 50, 0);
        record_shutdown_time();
        return;
    }

    // 파일에서 시간 읽기
    if (fscanf(file, "%ld", &startup_time) != 1) {
        fprintf(stderr, "Error reading from file %s\n", SHUTDOWN_FILE);
    }
    fclose(file);

    // 시스템 시간 설정
    if (stime(&startup_time) != 0) {
        fprintf(stderr, "Error setting system time\n");
    }

    printf("System time has been set from file.\n");
}