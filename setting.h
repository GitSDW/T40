#ifndef __SETTING_H__
#define __SETTING_H__

#ifdef __cplusplus
extern "C"{
#endif	// __cplusplus

typedef union
{
    uint16_t bytes;
    struct
    {
        uint16_t led        : 1;
        uint16_t backlight  : 1;
        uint16_t flicker    : 1;
        uint16_t move_ex    : 1;
        uint16_t per_face   : 1;
        uint16_t door_g     : 1;
        uint16_t user_g     : 1;
        uint16_t reseved1   : 1;
        uint16_t reseved2   : 8;
    } bits;
} SETTING_FLAGS;

// 파일에 저장할 데이터 구조체 정의
typedef struct {
    int spk_vol;
    int bell_type;
    int move_sensitivty;
    int move_ex_s_x;
    int move_ex_s_y;
    int move_ex_e_x;
    int move_ex_e_y;
    uint8_t door_grid[28];
    uint8_t user_grid[28];
    SETTING_FLAGS SF;
} Settings;

#define CRC_POLYNOMIAL 0xEDB88320L
#define BUFFER_SIZE 1024

// CRC 테이블 생성
uint32_t crc_table[256];

Settings settings;
// Settings load_settings;


void Setting_Reinit(void);
void Setting_Init(void);
int Setting_Save(void);


#ifdef __cplusplus
}
#endif	// __cplusplus

#endif	// __SETTING_H__