#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "setting.h"

// CRC 테이블 초기화
void crc_init() {
    uint32_t crc;
    int i, j;

    for (i = 0; i < 256; i++) {
        crc = i;
        for (j = 8; j > 0; j--) {
            if (crc & 1)
                crc = (crc >> 1) ^ CRC_POLYNOMIAL;
            else
                crc >>= 1;
        }
        crc_table[i] = crc;
    }
}

// CRC 값을 계산
uint32_t calculateCRC(FILE *file) {
    uint32_t crc = 0xFFFFFFFFL;
    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            crc = (crc >> 8) ^ crc_table[(crc ^ buffer[i]) & 0xFF];
        }
    }

    return crc ^ 0xFFFFFFFFL;
}

// 파일의 CRC 값을 저장
void saveCRC(uint32_t crc, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(&crc, sizeof(uint32_t), 1, file);
        fclose(file);
    } else {
        printf("Error: Unable to open file for writing.\n");
    }
}

// 저장된 CRC 값을 불러옴
bool loadCRC(uint32_t *crc, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        fread(crc, sizeof(uint32_t), 1, file);
        fclose(file);
        return true;
    } else {
        printf("Error: Unable to open file for reading.\n");
        return false;
    }
}

// 설정 값을 파일에 저장
void saveSettings(Settings *settings, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(settings, sizeof(Settings), 1, file);
        fclose(file);
    } else {
        printf("Error: Unable to open file for writing.\n");
    }
}

// 파일에서 설정값 불러오기
void loadSettings(Settings *settings, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        fread(settings, sizeof(Settings), 1, file);
        fclose(file);
    } else {
        // 파일이 없는 경우 기본값으로 설정
        settings->height = 0;
        settings->width = 0;
        settings->length = 0;
        settings->exposure = 0;
        printf("Warning: Unable to open file for reading. Loading default settings.\n");
    }
}

void Setting_Reinit(void) {
    printf("Setting Reinit.\n");
    // 기본 설정값 사용
    settings.height = 0;
    settings.width = 0;
    settings.length = 0;
    settings.exposure = 0;
    // 파일에 설정값 저장
    saveSettings(&settings, filename);
    // 파일의 CRC 값 저장
    saveCRC(calculateCRC(fopen(filename, "rb")), "/tmp/mnt/sdcard/crc.dat");
    printf("ReInit settings saved.\n");
}

void Setting_Init(void) {
    crc_init();
    
     // 파일 경로
    const char *filename = "/tmp/mnt/sdcard/settings.dat";

    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        uint32_t crc = calculateCRC(file);
        fclose(file);

        uint32_t savedCRC;
        if (loadCRC(&savedCRC, "/tmp/mnt/sdcard/crc.dat")) {
            // 저장된 CRC 값과 계산된 CRC 값 비교
            if (savedCRC == crc) {
                printf("File integrity check: OK\n");
                // 파일에서 설정값 불러오기
                loadSettings(&settings, filename);
            } else {
                printf("CRC check: FAILED\n");
                fclose(file);
                Setting_Reinit();
            }
        } else {
            printf("CRC file not found.\n");
            fclose(file);
            Setting_Reinit();
        }
    } else {
        printf("Settings file not found.\n");
        Setting_Reinit();
    }

    // 현재 설정값 출력
    printf("Current settings:\n");
    printf("Height: %d\n", settings.height);
    printf("Width: %d\n", settings.width);
    printf("Length: %d\n", settings.length);
    printf("Exposure: %d\n", settings.exposure);

}

int main() {
    crc_init();

    Settings settings;

    // 파일 경로
    const char *filename = "/tmp/mnt/sdcard/settings.dat";

    // 파일의 CRC 값 계산
    FILE *file = fopen(filename, "rb");
    if (file != NULL) {
        uint32_t crc = calculateCRC(file);
        fclose(file);

        // 저장된 CRC 값 불러오기
        uint32_t savedCRC;
        if (loadCRC(&savedCRC, "/tmp/mnt/sdcard/crc.dat")) {
            // 저장된 CRC 값과 계산된 CRC 값 비교
            if (savedCRC == crc) {
                printf("File integrity check: OK\n");
                // 파일에서 설정값 불러오기
                loadSettings(&settings, filename);
            } else {
                printf("File integrity check: FAILED\n");
                printf("Settings file is corrupted.\n");
                // 기본 설정값 사용
                settings.height = 0;
                settings.width = 0;
                settings.length = 0;
                settings.exposure = 0;
            }
        } else {
            printf("CRC file not found.\n");
            // 파일에 설정값 저장
            saveSettings(&settings, filename);
            // 계산된 CRC 값을 저장
            saveCRC(crc, "/tmp/mnt/sdcard/crc.dat");
            printf("Initial settings saved.\n");
        }
    } else {
        printf("Settings file not found.\n");
        // 기본 설정값 사용
        settings.height = 0;
        settings.width = 0;
        settings.length = 0;
        settings.exposure = 0;
        // 파일에 설정값 저장
        saveSettings(&settings, filename);
        // 파일의 CRC 값 저장
        saveCRC(calculateCRC(fopen(filename, "rb")), "/tmp/mnt/sdcard/crc.dat");
        printf("Initial settings saved.\n");
    }

    // 현재 설정값 출력
    printf("Current settings:\n");
    printf("Height: %d\n", settings.height);
    printf("Width: %d\n", settings.width);
    printf("Length: %d\n", settings.length);
    printf("Exposure: %d\n", settings.exposure);

    return 0;
}
