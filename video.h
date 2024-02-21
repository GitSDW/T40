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
int isd_crop(int x, int y, int w, int h);

#if __cplusplus
}
#endif

#endif /* __VIDEO_H__ */