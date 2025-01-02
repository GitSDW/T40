#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "setting.h"
#include "global_value.h"

const char *setting_file = "/tmp/mnt/sdcard/settings.dat";
const char *setting_crc = "/tmp/mnt/sdcard/crc.dat";
const char *default_file = "/tmp/mnt/sdcard/dsetting.dat";
const char *default_crc = "/tmp/mnt/sdcard/dcrc.dat";

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
        dp("Error: Unable to open file for writing.\n");
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
        dp("Error: Unable to open file for reading.\n");
        return false;
    }
}

// 설정 값을 파일에 저장
int saveSettings(Settings *settings, const char *filename) {
    dp("save! :%s\n", filename);
    FILE *file = fopen(filename, "wb");
    dp("open!\n");
    if (file != NULL) {
        fwrite(settings, sizeof(Settings), 1, file);
        fclose(file);
        return 1;
    } else {
        dp("Error: Unable to open file for writing.\n");
        return -1;
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
        settings->spk_vol = 0;
        dp("Warning: Unable to open file for reading. Loading default settings.\n");
    }
}

void Setting_Reinit(void) {
    dp("Setting Reinit.\n");
    // 기본 설정값 사용
    settings.spk_vol = 2;
    settings.bell_type = 0;
    settings.move_sensitivty = 4;
    settings.move_ex_s_x = 0;
    settings.move_ex_s_y = 0;
    settings.move_ex_e_x = 0;
    settings.move_ex_e_y = 0;
    for (int i=0; i<28; i++) {
        settings.door_grid[i] = 0;
        settings.user_grid[i] = 0;
    }

    settings.SF.bits.led = 1;
    settings.SF.bits.backlight = 0;
    settings.SF.bits.flicker = 0;
    settings.SF.bits.move_ex = 0;
    settings.SF.bits.per_face = 1;
    settings.SF.bits.door_g = 0;
    settings.SF.bits.user_g = 0;

    // 파일에 설정값 저장
    dp("Save Setting!\n");
    saveSettings(&settings, setting_file);
    // 파일의 CRC 값 저장
    dp("CRC Save!!\n");
    saveCRC(calculateCRC(fopen(setting_file, "rb")), setting_crc);
    dp("ReInit settings saved.\n");
}

void Setting_Init(void) {
    int reset_flag = false;
    crc_init();
    
     // 파일 경로
    

    FILE *file = fopen(setting_file, "rb");
    if (file != NULL) {
        uint32_t crc = calculateCRC(file);
        fclose(file);

        uint32_t savedCRC;
        if (loadCRC(&savedCRC, setting_crc)) {
            // 저장된 CRC 값과 계산된 CRC 값 비교
            if (savedCRC == crc) {
                dp("File integrity check: OK\n");
                // 파일에서 설정값 불러오기
                loadSettings(&settings, setting_file);
            } else {
                dp("CRC check: FAILED\n");
                // fclose(file);
                Setting_Reinit();
            }
        } else {
            dp("CRC file not found.\n");
            // fclose(file);
            Setting_Reinit();
        }
    } else {
        dp("Settings file not found.\n");
        Setting_Reinit();
    }

    if (settings.spk_vol > 4) {
        reset_flag = true;
    } 
    else if (settings.bell_type > 2) {
        reset_flag = true;
    }
    else if (settings.move_sensitivty > 4) {
        reset_flag = true;
    }

    if (reset_flag) {
        dp("Settings Value Invalid.\n");
        Setting_Reinit();
    }

    // 현재 설정값 출력
    dp("Current settings:\n");
    dp("led:%d belltype:%d spkvol:%d facemosaic:%d\n", settings.SF.bits.led, settings.bell_type, settings.spk_vol, settings.SF.bits.per_face);
    dp("door:%d user:%d moveex:%d flicker:%d sensi:%d backlight:%d\n", settings.SF.bits.door_g, settings.SF.bits.user_g, settings.SF.bits.move_ex, settings.SF.bits.flicker,
                                                                                                                        settings.move_sensitivty, settings.SF.bits.backlight);
    for(int k=0; k<27; k++) {
        dp("0x%02x ", settings.door_grid[k]);
    }
    dp("\n");
    dp("usergrid:");
    for(int k=0; k<27; k++) {
        dp("0x%02x ", settings.user_grid[k]);
    }
    dp("\n");
    dp("moveexarea: \n     start X : %d Y : %d\n     end : X : %d Y : %d\n", settings.move_ex_s_x, settings.move_ex_s_y, settings.move_ex_e_x, settings.move_ex_e_y);

    dp("Setting Load Success!!\n");
}

int Setting_Save(void) {
    int ret = 0;
    ret = saveSettings(&settings, setting_file);
    if (ret < 0) {
        dp("Fail Setting Saved!\n");
        return -1;
    }
    // 파일의 CRC 값 저장
    saveCRC(calculateCRC(fopen(setting_file, "rb")), setting_crc);

    return 0;
}

// int main() {
//     crc_init();

//     Settings settings;

//     // 파일 경로
//     const char *filename = "/tmp/mnt/sdcard/settings.dat";

//     // 파일의 CRC 값 계산
//     FILE *file = fopen(filename, "rb");
//     if (file != NULL) {
//         uint32_t crc = calculateCRC(file);
//         fclose(file);

//         // 저장된 CRC 값 불러오기
//         uint32_t savedCRC;
//         if (loadCRC(&savedCRC, "/tmp/mnt/sdcard/crc.dat")) {
//             // 저장된 CRC 값과 계산된 CRC 값 비교
//             if (savedCRC == crc) {
//                 dp("File integrity check: OK\n");
//                 // 파일에서 설정값 불러오기
//                 loadSettings(&settings, filename);
//             } else {
//                 dp("File integrity check: FAILED\n");
//                 dp("Settings file is corrupted.\n");
//                 // 기본 설정값 사용
//                 settings.height = 0;
//                 settings.width = 0;
//                 settings.length = 0;
//                 settings.exposure = 0;
//             }
//         } else {
//             dp("CRC file not found.\n");
//             // 파일에 설정값 저장
//             saveSettings(&settings, filename);
//             // 계산된 CRC 값을 저장
//             saveCRC(crc, "/tmp/mnt/sdcard/crc.dat");
//             dp("Initial settings saved.\n");
//         }
//     } else {
//         dp("Settings file not found.\n");
//         // 기본 설정값 사용
//         settings.height = 0;
//         settings.width = 0;
//         settings.length = 0;
//         settings.exposure = 0;
//         // 파일에 설정값 저장
//         saveSettings(&settings, filename);
//         // 파일의 CRC 값 저장
//         saveCRC(calculateCRC(fopen(filename, "rb")), "/tmp/mnt/sdcard/crc.dat");
//         dp("Initial settings saved.\n");
//     }

//     // 현재 설정값 출력
//     dp("Current settings:\n");
//     dp("Height: %d\n", settings.height);
//     dp("Width: %d\n", settings.width);
//     dp("Length: %d\n", settings.length);
//     dp("Exposure: %d\n", settings.exposure);

//     return 0;
// }
