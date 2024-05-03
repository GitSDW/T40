#ifndef __IMAGE_COMPARISON_H__
#define __IMAGE_COMPARISON_H__

#include "global_value.h"

#ifdef __cplusplus
extern "C" {
#endif


int package_find(char *imgpath1, char *imgpath2, int thhold);
int package_sistic(char *imgpath1, char *imgpath2);
double calculateSimilarity(char *imgpath1, char *imgpath2);
// int thumbnail_make(int x, int y, int width, int height);
int thumbnail_make(Thum_Data_t cont);
int facecrop_make(Fdpd_Data_t cont);

#if __cplusplus
}
#endif

#endif /* __IMAGE_COMPARISON_H__ */