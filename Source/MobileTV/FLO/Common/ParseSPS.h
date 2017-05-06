#ifndef _PARSESPS_H_
#define _PARSESPS_H_

#ifdef __cplusplus
extern "C" {
#endif

int GetSizeInfo(unsigned char *buf, int *width, int *height);
int IsH264ProfileSupported(unsigned char *buf);

#ifdef __cplusplus
}
#endif

#endif//_PARSESPS_H_