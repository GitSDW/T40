#ifndef __VIDEO_H__
#define __VIDEO_H__


#ifdef __cplusplus
extern "C" {
#endif

int video_init(void);
int video_deinit(void);
void *get_video_stream_user_thread(void *args);
void *get_video_clip_user_thread(void *args);
void *get_snap_stream_user_thread(void *args);
void *move_detecte_thread(void *args);
void *OSD_thread(void *args);
void udp_main_video_rolling_dcnt(void);
void udp_box_video_rolling_dcnt(void);
int osd_grid_show(int selnum);
int isd_crop(int x, int y, int w, int h, int cam);
int isd_distortion(int cx, int cy, int w, int h, int streng, int cam);
int isp_filcker (int freq, int mode);
// int isp_wdr (int state, int cam);
uint32_t isp_integration_time(int getset, uint32_t value);

#if __cplusplus
}
#endif

#endif /* __VIDEO_H__ */