#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <imp/imp_log.h>
#include <imp/imp_common.h>
#include <imp/imp_system.h>
#include <imp/imp_framesource.h>
#include <imp/imp_ivs.h>
#include <imp/imp_osd.h>

#include <ivs/ivs_common.h>
#include <ivs/ivs_interface.h>
#include <ivs/ivs_inf_facepersonDet.h>


// #include "fdpd-common.h"
#include "global_value.h"

#define TAG "FDPD"

int sample_ivs_facepersondet_init(int grp_num) {
    int ret = 0;

    ret = IMP_IVS_CreateGroup(grp_num);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_IVS_CreateGroup(%d) failed\n", grp_num);
        return -1;
    }


    return 0;
}

int sample_ivs_facepersondet_exit(int grp_num) {
    int ret = 0;
    ret = IMP_IVS_DestroyGroup(grp_num);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_IVS_DestroyGroup(%d) failed\n", grp_num);
        return -1;
    }
    return 0;
}

int sample_ivs_facepersondet_start(int grp_num, int chn_num, IMPIVSInterface **interface) {
    //check ivs version
    //FACEPERSONDET_VERSION_NUM defined in ivs_inf_facepersondet.h .
    uint32_t facepersondet_ver = facepersondet_get_version_info();
    if(facepersondet_ver != FACEPERSONDET_VERSION_NUM){
        dp("The version numbers of head file and lib do not match, head file version: %08x, lib version: %08x\n", FACEPERSONDET_VERSION_NUM, facepersondet_ver);
        return -1;
    }
    //check ivs version
    int ret = 0;
    facepersondet_param_input_t param;

    memset(&param, 0, sizeof(facepersondet_param_input_t));
    param.frameInfo.width = 1920; //640;
    param.frameInfo.height = 1080; //360;

    param.skip_num = 1;      //skip num
    param.max_faceperson_box = 10;
    param.sense = 7;//Default:7 / Low sensitivity 0 ~ 10 High sensitivity;
    param.switch_track = true;
    param.enable_move = false;
    param.open_move_filter = false;
    param.model_path = "/tmp/mnt/sdcard/faceperson_det.bin";
    param.enable_perm = false;
    param.permcnt = 1;
    param.mod = 0;
    param.switch_stop_det = false;
    param.fast_update_params = true;

    param.perms[0].pcnt=6;
    param.perms[1].pcnt=5;
    param.perms[0].p = (IVSPoint *)malloc(6* sizeof(IVSPoint));
    param.perms[1].p = (IVSPoint *)malloc(5* sizeof(IVSPoint));

    int i ,j;
    for(i=0;i<param.permcnt;i++){
        switch(i){
        case 0:
        {
            for( j=0;j<param.perms[0].pcnt;j++){
                param.perms[0].p[j].x=(j%3)*70;
                param.perms[0].p[j].y=(j/3)*170;
            }
        }
        break;
        case 1:
        {
            for( j=0;j<param.perms[1].pcnt;j++){
                param.perms[1].p[j].x=(j%3)*70+160;
                param.perms[1].p[j].y=(j/3)*170;
            }
        }
        break;
        }
    }

    *interface = FacepersonDetInterfaceInit(&param);
    if (*interface == NULL) {
        IMP_LOG_ERR(TAG, "IMP_IVS_CreateGroup(%d) failed\n", grp_num);
        return -1;
    }

    ret = IMP_IVS_CreateChn(chn_num, *interface);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_IVS_CreateChn(%d) failed\n", chn_num);
        return -1;
    }

    ret = IMP_IVS_RegisterChn(grp_num, chn_num);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_IVS_RegisterChn(%d, %d) failed\n", grp_num, chn_num);
        return -1;
    }

    ret = IMP_IVS_StartRecvPic(chn_num);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_IVS_StartRecvPic(%d) failed\n", chn_num);
        return -1;
    }

    return 0;
}

int sample_ivs_facepersondet_stop(int chn_num, IMPIVSInterface *interface) {
    int ret = 0;

    ret = IMP_IVS_StopRecvPic(chn_num);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_IVS_StopRecvPic(%d) failed\n", chn_num);
        return -1;
    }
    sleep(1);

    ret = IMP_IVS_UnRegisterChn(chn_num);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_IVS_UnRegisterChn(%d) failed\n", chn_num);
        return -1;
    }

    ret = IMP_IVS_DestroyChn(chn_num);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "IMP_IVS_DestroyChn(%d) failed\n", chn_num);
        return -1;
    }

    FacepersonDetInterfaceExit(interface);

    return 0;
}

// int main(int argc, char *argv[]) {
//     dp("faceperson detect\n");
//     int ret = 0;
//     IMPIVSInterface *inteface = NULL;
//     facepersondet_param_output_t *result = NULL;

//     /* Step.1 System init */
//     ret = sample_system_init();
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "IMP_System_Init() failed\n");
//         return -1;
//     }

//     /* Step.2 FrameSource init */
//     IMPFSChnAttr fs_chn_attr;
//     memset(&fs_chn_attr, 0, sizeof(IMPFSChnAttr));
//     fs_chn_attr.pixFmt = PIX_FMT_NV12;
//     /* fs_chn_attr.pixFmt = PIX_FMT_BGR24; */
//     fs_chn_attr.outFrmRateNum = SENSOR_FRAME_RATE;
//     fs_chn_attr.outFrmRateDen = 1;
//     /* fs_chn_attr.nrVBs = 2; */
//     fs_chn_attr.nrVBs = 2;
//     fs_chn_attr.type = FS_PHY_CHANNEL;

//     fs_chn_attr.crop.enable = 0;
//     fs_chn_attr.crop.top = 0;
//     fs_chn_attr.crop.left = 0;
//     fs_chn_attr.crop.width = sensor_main_width;
//     fs_chn_attr.crop.height = sensor_main_height;

//     fs_chn_attr.scaler.enable = 1;    /* ivs use the second framesource channel, need scale*/
//     fs_chn_attr.scaler.outwidth = sensor_sub_width;
//     fs_chn_attr.scaler.outheight = sensor_sub_height;

//     fs_chn_attr.picWidth = sensor_sub_width;
//     fs_chn_attr.picHeight = sensor_sub_height;

//     ret = sample_framesource_init(FS_SUB_CHN, &fs_chn_attr);
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "FrameSource init failed\n");
//         return -1;
//     }

//     /* Step.3 Encoder init */
//     ret = sample_ivs_facepersondet_init(0);
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "sample_ivs_init(0) failed\n");
//         return -1;
//     }

//     /* Step.4 Bind */
//     IMPCell framesource_cell = {DEV_ID_FS, FS_SUB_CHN, 0};
//     IMPCell ivs_cell = {DEV_ID_IVS, 0, 0};

//     ret = IMP_System_Bind(&framesource_cell, &ivs_cell);
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and ivs0 failed\n", FS_SUB_CHN);
//         return -1;
//     }

//     /* Step.5 Stream On */
//     IMP_FrameSource_SetFrameDepth(FS_SUB_CHN, 0);
//     ret = sample_framesource_streamon(FS_SUB_CHN);
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "ImpStreamOn failed\n");
//         return -1;
//     }

//     /* Step.6 Start to ivs */
//     ret = sample_ivs_facepersondet_start(0, 2, &inteface);
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "sample_ivs_facepersondet_start(0, 2) failed\n");
//         return -1;
//     }

//     /*Step.7 Get result*/
//     int j = 1;
//     while(true){
//         j++;
//         ret = IMP_IVS_PollingResult(2, IMP_IVS_DEFAULT_TIMEOUTMS);
//         if (ret < 0) {
//             IMP_LOG_ERR(TAG, "IMP_IVS_PollingResult(%d, %d) failed\n", 2, IMP_IVS_DEFAULT_TIMEOUTMS);
//             return -1;
//         }
//         ret = IMP_IVS_GetResult(2, (void **)&result);
//         if (ret < 0) {
//             IMP_LOG_ERR(TAG, "IMP_IVS_GetResult(%d) failed\n", 2);
//         return -1;
//         }

//         facepersondet_param_output_t* r = (facepersondet_param_output_t*)result;
//         if(j%20 == 0){
//             int ret;
//             facepersondet_param_input_t param;
//             ret = IMP_IVS_GetParam(2, &param);
//             if (ret < 0){
//                 IMP_LOG_ERR(TAG, "IMP_IVS_GetParam(%d) failed\n", 0);
//                 return -1;
//             }
//             param.fast_update_params = true;
//             param.switch_stop_det = !param.switch_stop_det;
//             ret = IMP_IVS_SetParam(2, &param);
//             if (ret < 0){
//                 IMP_LOG_ERR(TAG, "IMP_IVS_SetParam(%d) failed\n", 0);
//                 return -1;
//             }
//         }
//         if(r->count > 0) {
//             dp("r->count %d\n", r->count);
//             for(int i = 0; i < r->count; i++) {
//                 int track_id = r->faceperson[i].track_id;
//                 int class_id = r->faceperson[i].class_id;
//                 IVSRect* show_rect = &r->faceperson[i].show_box;

//                 int x0,y0,x1,y1;
//                 x0 = (int)show_rect->ul.x;
//                 y0 = (int)show_rect->ul.y;
//                 x1 = (int)show_rect->br.x;
//                 y1 = (int)show_rect->br.y;
//                 dp("faceperson location: class[%d] [%d, %d, %d, %d] \n", class_id,x0, y0, x1, y1);
//             }
//         }
//         ret = IMP_IVS_ReleaseResult(2, (void *)result);
//         if (ret < 0) {
//             IMP_LOG_ERR(TAG, "IMP_IVS_ReleaseResult(%d) failed\n", 2);
//         return -1;
//         }
//         usleep(50*1000);
//     }

//     /* Step.8 stop to ivs */
//     ret = sample_ivs_facepersondet_stop(2, inteface);
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "sample_ivs_facepersondet_stop(0) failed\n");
//         return -1;
//     }

//     /* Step.9 Stream Off */
//     ret = sample_framesource_streamoff(FS_SUB_CHN);
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "FrameSource StreamOff failed\n");
//         return -1;
//     }

//     /* Step.10 UnBind */
//     ret = IMP_System_UnBind(&framesource_cell, &ivs_cell);
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "UnBind FrameSource channel%d and ivs0 failed\n", FS_SUB_CHN);
//         return -1;
//     }

//     /* Step.11 ivs exit */
//     ret = sample_ivs_facepersondet_exit(0);
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "ivs mode exit failed\n");
//         return -1;
//     }

//     /* Step.12 FrameSource exit */
//     ret = sample_framesource_exit(FS_SUB_CHN);
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "FrameSource(%d) exit failed\n", FS_SUB_CHN);
//         return -1;
//     }

//     /* Step.13 System exit */
//     ret = sample_system_exit();
//     if (ret < 0) {
//         IMP_LOG_ERR(TAG, "sample_system_exit() failed\n");
//         return -1;
//     }

//     return 0;
// }
extern int mosdgrp;
extern int bosdgrp;
extern IMPRgnHandle *prHander;

void *fdpd_thread(void *args) 
{
    dp("faceperson detect\n");
    int ret = 0;
    int i;
    // uint64_t now_time;
    IMPIVSInterface *inteface = NULL;
    facepersondet_param_output_t *result = NULL;

    // int fd = -1, str_p = 0;
    int face_num=0, person_num=0, nodet_cnt=0;
    // char framefilename[64];
    // char *fp_data;
    int face_track[5] = {256};
    int fdpd_en_cnt = 0;
    // int mosaic_cnt[10] = {0};
    int64_t face_cnt_time = 0;
    IMPOSDRgnAttr rect_rAttr;

    
    // sprintf(framefilename, "/dev/shm/faceperson.data");
    // fd = open(framefilename, O_RDWR | O_CREAT, 0x644);
    // if (fd < 0) {
    //     IMP_LOG_ERR(TAG, "open %s failed\n", framefilename);
    // }
    // fp_data = (char*)malloc(1024);
    // for (i=0;i<10;i++){
    //     face[i] = (char*)malloc(20);
    // }

    /* Step.4 Bind */
    // IMPCell framesource_cell = {DEV_ID_FS, FS_SUB_CHN, 0};
    // IMPCell ivs_cell = {DEV_ID_IVS, 0, 0};

    // ret = IMP_System_Bind(&framesource_cell, &ivs_cell);
    // if (ret < 0) {
    //     IMP_LOG_ERR(TAG, "Bind FrameSource channel%d and ivs0 failed\n", FS_SUB_CHN);
    //     return NULL;
    // }

    /* Step.6 Start to ivs */
    ret = sample_ivs_facepersondet_start(0, 3, &inteface);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "sample_ivs_facepersondet_start(0, 2) failed\n");
        return NULL;
    }

    /*Step.7 Get result*/
    int j = 1;
    while(!bStrem){
        j++;
        ret = IMP_IVS_PollingResult(3, IMP_IVS_DEFAULT_TIMEOUTMS);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_IVS_PollingResult(%d, %d) failed\n", 3, IMP_IVS_DEFAULT_TIMEOUTMS);
            return NULL;
        }
        ret = IMP_IVS_GetResult(3, (void **)&result);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_IVS_GetResult(%d) failed\n", 3);
        return NULL;
        }

        facepersondet_param_output_t* r = (facepersondet_param_output_t*)result;
        if(r->count > 0) {
            face_cnt_mosaic = r->count;
            nodet_cnt = 0;
            // memset(fp_data, 0, 1024);
            // now_time = (sample_gettimeus() - start_time)/1000; // msec
            face_num = 0;
            person_num = 0;
            fdpd_ck = true;
            // dp("fdpd Check \n", fdpd_ck);
            for (i = 0; i < r->count; i++) {
                if(i < r->count) {
                    int class_id = r->faceperson[i].class_id;   // 0 : face 1: person(body)
                    int track_id = r->faceperson[i].track_id;
                    float confidence = r->faceperson[i].confidence;
                    IVSRect* show_rect = &r->faceperson[i].show_box;

                    face_cnt_time = sample_gettimeus();
                    fdpd_data[i].flag = true;
                    fdpd_data[i].classid = class_id;
                    fdpd_data[i].trackid = track_id;
                    fdpd_data[i].confidence = confidence;
                    fdpd_data[i].ul_x = (int)show_rect->ul.x;
                    fdpd_data[i].ul_y = (int)show_rect->ul.y;
                    fdpd_data[i].br_x = (int)show_rect->br.x;
                    fdpd_data[i].br_y = (int)show_rect->br.y;

                    // dp("confidence:%f\n", confidence);

                    if (fdpd_data[i].classid == 0 && Mosaic_En) {
                        #ifdef __PRIVERCE_SIZE_UP__
                            int cover_w, cover_h;
                            int cover_sx, cover_sy;
                            int cover_ex, cover_ey;

                            cover_w = (fdpd_data[i].br_x - fdpd_data[i].ul_x)/1;
                            cover_h = (fdpd_data[i].br_y - fdpd_data[i].ul_y)/2;
                            if (fdpd_data[i].ul_x > cover_w) cover_sx = fdpd_data[i].ul_x - cover_w;
                            else cover_sx = 0;
                            if (fdpd_data[i].ul_y > cover_h) cover_sy = fdpd_data[i].ul_y - cover_h;
                            else cover_sy = 0;

                            if (fdpd_data[i].br_x < (1920-cover_w)) cover_ex = fdpd_data[i].br_x + cover_w;
                            else cover_ex = 1920-1;
                            if (fdpd_data[i].br_y < (1080-cover_h)) cover_ey = fdpd_data[i].br_y + cover_h;
                            else cover_ey = 1080-1;

                            // dp("1 sx:%d sy:%d 2 sx:%d ey:%d\n", cover_sx, cover_sy, fdpd_data[i].ul_x, fdpd_data[i].ul_y);
                            // dp("1 ex:%d ey:%d 2 ex:%d ey:%d\n", cover_ex, cover_ey, fdpd_data[i].br_x, fdpd_data[i].br_y);

                            rect_rAttr.type = OSD_REG_COVER;
                            rect_rAttr.rect.p0.x = cover_sx;
                            rect_rAttr.rect.p0.y = cover_sy;
                            rect_rAttr.rect.p1.x = cover_ex;
                            rect_rAttr.rect.p1.y = cover_ey;
                        #else
                            rect_rAttr.type = OSD_REG_COVER;
                            rect_rAttr.rect.p0.x = fdpd_data[i].ul_x;
                            rect_rAttr.rect.p0.y = fdpd_data[i].ul_y;
                            rect_rAttr.rect.p1.x = fdpd_data[i].br_x;
                            rect_rAttr.rect.p1.y = fdpd_data[i].br_y;
                        #endif



                        rect_rAttr.fmt = PIX_FMT_BGRA;
                            //  OSD_IPU_BLACK   = 0xff000000, /**< black */
                            //  OSD_IPU_WHITE   = 0xffffffff, /**< white*/
                            //  OSD_IPU_RED     = 0xffff0000, /**< red */
                            //  OSD_IPU_GREEN   = 0xff00ff00, /**< green */
                            //  OSD_IPU_BLUE    = 0xff0000ff, /**< blue */
                        rect_rAttr.data.coverData.color = 0xff777777;
                        IMP_OSD_SetRgnAttr(prHander[2+i], &rect_rAttr);
                        ret = IMP_OSD_ShowRgn(prHander[2+i], mosdgrp, 1);
                        if (ret != 0) {
                            IMP_LOG_ERR(TAG, "IMP_OSD_ShowRgn() Cover error\n");
                            return NULL;
                        }
                        else {
                            mosaic_time[i] = sample_gettimeus();
                        }
                    }

                    // dp("fdpd cnt: %d/%d class : %d track %d confidence : %f \n", i, r->count, class_id, track_id, confidence);
                
                    if(class_id == 0 && confidence > 0) {
                        face_num++;
                        // dp("fdpd cnt: %d/%d class : %d track %d confidence : %f x : %d y : %d\n"
                            // , i, r->count, class_id, track_id, confidence, fdpd_data[i].ul_x, fdpd_data[i].ul_y);
                        // dp("fr:%d confidence:%f thumbnail_snap:%d\n", fr_state, confidence, thumbnail_snap);
                        // if ((fr_state == 1 && track_id > 0 && confidence > 0.85) &&
                        if ((fr_state == FR_START && confidence > 0.90 && !thumbnail_snap) &&
                            (((fdpd_data[i].ul_x+fdpd_data[i].br_x)/2) > 200) &&
                            (((fdpd_data[i].ul_x+fdpd_data[i].br_x)/2) < 1920 - 100) &&
                            (((fdpd_data[i].ul_y+fdpd_data[i].br_y)/2) > 200) &&
                            (((fdpd_data[i].ul_y+fdpd_data[i].br_y)/2) < 1080 - 100) &&
                            (face_track[0] != fdpd_data[i].trackid) &&
                            (face_track[1] != fdpd_data[i].trackid) &&
                            (face_track[2] != fdpd_data[i].trackid) &&
                            (face_track[3] != fdpd_data[i].trackid) &&
                            (face_track[4] != fdpd_data[i].trackid))
                            {
                            fr_state++;
                            facial_data.flag = fdpd_data[i].flag;
                            facial_data.classid = fdpd_data[i].classid;
                            facial_data.trackid = fdpd_data[i].trackid;
                            face_track[face_crop_cnt] = facial_data.trackid;
                            facial_data.confidence = fdpd_data[i].confidence;
                            facial_data.ul_x = fdpd_data[i].ul_x;
                            facial_data.ul_y = fdpd_data[i].ul_y;
                            facial_data.br_x = fdpd_data[i].br_x;
                            facial_data.br_y = fdpd_data[i].br_y;
                            face_snap = true;
                            dp("x:%d, y:%d, confidence:%f\n", facial_data.ul_x, facial_data.ul_y, facial_data.confidence);
                        }
                    }
                    else { 
                        if(r->faceperson[i].class_id == 1) {
                            person_num++;
                        }
                    }
                }
                else{
                    fdpd_data[i].flag = false;
                }
            }
            face_cnt = face_num;
            person_cnt = person_num;

            // if(face_num > 0) {
            //     str_p = sprintf(fp_data, "%06lld/%d", now_time, face_num);
            //     for(i=0; i<face_num; i++){
            //         str_p += sprintf(fp_data+str_p, "%s", face[i]);
            //     }
            //     str_p += sprintf(fp_data+str_p, "\n");
            //     // dp("%s", fp_data);
            //     if (write(fd, (void *)fp_data, str_p) != str_p)
            //        dp("write Len Err!");
            // }
        }
        else {
            nodet_cnt++;
            if(nodet_cnt%6 == 5){
                for(i=0; i<10; i++){
                    fdpd_data[i].flag = false;
                }
                face_num = 0;
                person_num = 0;
                face_cnt = face_num;
                person_cnt = person_num;
            }
            
        }
        ret = IMP_IVS_ReleaseResult(3, (void *)result);
        if (ret < 0) {
            IMP_LOG_ERR(TAG, "IMP_IVS_ReleaseResult(%d) failed\n", 3);
            return NULL;
        }
        // usleep(100*1000);
        if (!fdpd_En){
            fdpd_en_cnt++;
            if (fdpd_en_cnt>5) fdpd_En = true;
        }

        if ((face_cnt_time != 0) && ((sample_gettimeus()-face_cnt_time) > 2000000)) {
            face_cnt_time = 0;
            face_cnt_mosaic = 0;
        }
    }

    /* Step.8 stop to ivs */
    ret = sample_ivs_facepersondet_stop(3, inteface);
    if (ret < 0) {
        IMP_LOG_ERR(TAG, "sample_ivs_facepersondet_stop(0) failed\n");
        return NULL;
    }

    // close(fd);
    // free(fp_data);
    // for (i=0;i<10;i++){
    //     free(face[i]);
    // }

    return ((void*) 0);
}
