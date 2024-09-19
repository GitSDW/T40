#ifndef __MOVE_H__
#define __MOVE_H__


#ifdef __cplusplus
extern "C" {
#endif

int move_init(IMPCell source_framecell);
int move(void);
int move_deinit(IMPCell source_framecell);
int get_parm(void);

#if __cplusplus
}
#endif

#endif /* __MOVE_H__ */