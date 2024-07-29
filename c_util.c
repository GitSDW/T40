#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h> 
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>

#include "global_value.h"

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

int64_t set_time = 0;
int64_t cnt_time = 0;

int check_delay_time(void)
{
    if (set_time != 0 && cnt_time != 0) {
        if ((sample_gettimeus()-set_time) >= cnt_time) {
            set_time = 0;
            cnt_time = 0;
            return 1;
        }
        else {
            return 0;
        }
    }
    else {
        // printf("Not Set Timer!\n");
        return 1;
    }
}

void set_delay_time(int64_t ti)
{
    if (set_time == 0 && cnt_time == 0 && ti != 0) {
        set_time = sample_gettimeus();
        cnt_time = ti;

    }
    else {
        if (check_delay_time() == 1)
        {
            set_time = sample_gettimeus();
            cnt_time = ti;
            // printf("Set Timer:%lld\n", cnt_time);
        }
        else {
            // printf("Already Set!\n");
        }
    }
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

int file_ck(const char* filename) {

    // access 함수를 사용하여 파일이 존재하는지 확인
    if (access(filename, F_OK) != -1) {
        printf("File Exist.\n");
        return 1;
    } else {
        printf("Not Found File.\n");
        return 0;
    }

}



#define FOLDER1_PATH "/maincam"
#define FOLDER2_PATH "/boxcam"
#define MAX_FILES 22

// 파일명에서 날짜를 추출하는 함수
char* extract_date_from_filename(const char* filename) {
    char* date = (char*)malloc(13 * sizeof(char)); // 12자리 날짜 + NULL 문자
    if (date == NULL) {
        perror("Malloc Error!\n");
        exit(EXIT_FAILURE);
    }
    strncpy(date, filename, 12);
    date[12] = '\0'; // NULL 문자 추가
    return date;
}

// 가장 오래된 파일 삭제하는 함수
void delete_oldest_file(const char* folder_path) {
    DIR* dir;
    struct dirent* entry;
    struct stat file_stat;
    char oldest_file_name[256];
    time_t oldest_time = time(NULL);

    // 폴더 열기
    dir = opendir(folder_path);
    if (dir == NULL) {
        perror("Folder Open Fail");
        exit(EXIT_FAILURE);
    }

    // 폴더 내 파일 탐색
    while ((entry = readdir(dir)) != NULL) {
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", folder_path, entry->d_name);

        // 파일 상태 가져오기
        if (stat(full_path, &file_stat) == -1) {
            perror("file status get fail!");
            exit(EXIT_FAILURE);
        }

        // 파일이 regular file이고, 수정 시간이 현재 가장 오래된 파일보다 오래된 경우 업데이트
        if (S_ISREG(file_stat.st_mode) && difftime(file_stat.st_mtime, oldest_time) < 0) {
            oldest_time = file_stat.st_mtime;
            strcpy(oldest_file_name, entry->d_name);
        }
    }

    closedir(dir);

    // 가장 오래된 파일 삭제
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "%s/%s", folder_path, oldest_file_name);
    if (remove(full_path) != 0) {
        perror("Fail to Delete!");
        exit(EXIT_FAILURE);
    }
    printf("Oldest file del: %s\n", oldest_file_name);
}

int old_file_del(void) {
    int file_count = 0;

    // 폴더1 내 파일 확인
    DIR* folder1 = opendir(FOLDER1_PATH);
    if (folder1 == NULL) {
        perror("Folder Open Fail1");
        exit(EXIT_FAILURE);
    }
    struct dirent* entry1;
    while ((entry1 = readdir(folder1)) != NULL) {
        // 파일명에서 날짜 추출
        char* date = extract_date_from_filename(entry1->d_name);
        printf("File1: %s, Date: %s\n", entry1->d_name, date);
        free(date);
        file_count++;
    }
    closedir(folder1);

    // 폴더2 내 파일 확인
    DIR* folder2 = opendir(FOLDER2_PATH);
    if (folder2 == NULL) {
        perror("Folder Open Fail2");
        exit(EXIT_FAILURE);
    }
    struct dirent* entry2;
    while ((entry2 = readdir(folder2)) != NULL) {
        // 파일명에서 날짜 추출
        char* date = extract_date_from_filename(entry2->d_name);
        printf("File2: %s, Date: %s\n", entry2->d_name, date);
        free(date);
        file_count++;
    }
    closedir(folder2);

    // 파일 개수가 최대 파일 개수를 초과하는 경우 가장 오래된 파일 삭제
    if (file_count > MAX_FILES) {
        printf("File Num over %d. Oldest file delete.\n", MAX_FILES);
        delete_oldest_file(FOLDER1_PATH);
        delete_oldest_file(FOLDER2_PATH);
    }

    return 0;
}


int file_name_get(SaveFile *filelist, int topbot) {
    DIR *dir;
    struct dirent *ent;
    int cnt=0;

    // 폴더 경로
    const char *folder_path1 = FOLDER1_PATH;
    const char *folder_path2 = FOLDER2_PATH;

    // 폴더 열기
    if ((topbot == 0) && ((dir = opendir(folder_path1)) != NULL)) {
        // 파일 개수 세기
        int file_count = 0;
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                file_count++;
            }
        }

        // 파일 개수 출력
        // printf("File Count: %d\n", file_count);

        // 다시 폴더 처음부터 열기
        rewinddir(dir);

        // 파일명 순서대로 출력
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                sprintf(filelist->name[cnt], "%s", ent->d_name);
                // printf("%d%s\n", cnt, ent->d_name);
                cnt++;
            }
        }
        filelist->cnt = cnt;
        closedir(dir);
    } 
    else if ((topbot == 1) && ((dir = opendir(folder_path2)) != NULL)) {
        // 파일 개수 세기
        int file_count = 0;
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                file_count++;
            }
        }

        // 파일 개수 출력
        // printf("File Count: %d\n", file_count);

        // 다시 폴더 처음부터 열기
        rewinddir(dir);

        // 파일명 순서대로 출력
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                sprintf(filelist->name[cnt], "%s", ent->d_name);
                // printf("%d%s\n", cnt, ent->d_name);
                cnt++;
            }
        }
        filelist->cnt = cnt;
        closedir(dir);
    } 
    else {
        // 폴더 열기 실패
        perror("Folder Open Error!");
        filelist->cnt = 0;
        return EXIT_FAILURE;
    }

    return 0;
}

#define BUFF_SIZE 1024
int md5_get(char *filepath, char *md5_char) 
{
    char  buff[BUFF_SIZE];
    FILE *fp;

    sprintf(buff, "md5sum %s", filepath);
    printf("filepath:%s\n", buff);

    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        printf("File Not exist!\n");
        return -1;
    }

    fp = popen(buff, "r");
    if (NULL == fp)
    {
        printf("md5 fail!\n");
        perror("popen() 실패");
        return -1;
    }

    while (fgets(buff, BUFF_SIZE, fp))

    for(int i=0; i<1024; i++) {
        if (buff[i] != ' ') {
            md5_char[i] = buff[i];
            // printf("i:%d char:%c\n", i, md5_char[i]);
        }
        else {
            md5_char[i] = 0;
            break;
        }
    }

    printf("hash : %s\n", md5_char);

    pclose(fp);

    return 0;
}



