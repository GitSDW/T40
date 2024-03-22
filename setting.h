#ifndef __SETTING_H__
#define __SETTING_H__

#ifdef __cplusplus
extern "C"{
#endif	// __cplusplus

// 파일에 저장할 데이터 구조체 정의
typedef struct {
    int height;
    int width;
    int length;
    int exposure;
} Settings;

#define CRC_POLYNOMIAL 0xEDB88320L
#define BUFFER_SIZE 1024

// CRC 테이블 생성
uint32_t crc_table[256];

Settings settings;
// Settings load_settings;


void Setting_Reinit(void);
void Setting_Init(void);
void Setting_Save(void);


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// __SETTING_H__