
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/

#ifndef _CC_CONSTANT_H__
#define _CC_CONSTANT_H__
#include "voccrender.h"

extern const unsigned char	*ccClip255;
extern const unsigned char	*ccClip63;
extern const unsigned char	*ccClip31;

extern const int	table_ConstY[256];
extern const int	table_ConstU1[256];
extern const int	table_ConstU2[256];
extern const int	table_ConstV1[256];
extern const int	table_ConstV2[256];

#define ConstV1 1673527
#define ConstV2 852492
#define ConstU1 2114977
#define ConstU2 411041
#define ConstY	1220542

//#define GDITHER_P0	  0
//#define GDITHER_P1	 0
//#define GDITHER_P2	 0
//#define GDITHER_P3	 0

#define RBDITHER_P0	  0
#define RBDITHER_P1	  0
#define RBDITHER_P2   0
#define RBDITHER_P3	 0

#define RDITHER_P0	  0
#define RDITHER_P1	  0
#define RDITHER_P2    0
#define RDITHER_P3	  0

#define GDITHER_P0	  0
#define GDITHER_P1	 0
#define GDITHER_P2	  0
#define GDITHER_P3	  0

#define BDITHER_P0	  0
#define BDITHER_P1	  0
#define BDITHER_P2    0
#define BDITHER_P3	  0

void init_scale(float x_scale, int out_width, int *scale_tab, int in_width);

typedef void (CC_CON)(unsigned char *y, unsigned char *u, unsigned char *v,
							 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							 unsigned char *out_buf, int out_stride, unsigned char *mb_skip);

typedef CC_CON * CC_CON_PTR;

extern void cc_s_yuv420_disable(unsigned char *y, unsigned char *u, unsigned char *v,
								int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
								unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_180(unsigned char *y, unsigned char *u, unsigned char *v,
								int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
								unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_r90(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_l90(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_disable_half(unsigned char *y, unsigned char *u, unsigned char *v,
								int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
								unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_r90_half(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_l90_half(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_disable_3quarter(unsigned char *y, unsigned char *u, unsigned char *v,
								int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
								unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_r90_3quarter(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_l90_3quarter(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_disable_1point5(unsigned char *y, unsigned char *u, unsigned char *v,
								int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
								unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_r90_1point5(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_l90_1point5(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_disable_double(unsigned char *y, unsigned char *u, unsigned char *v,
								int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
								unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_r90_double(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_yuv420_l90_double(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);


extern void cc_yuv420_disable(unsigned char *y, unsigned char *u, unsigned char *v,
							  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							  unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_yuv420_disable_180(unsigned char *y, unsigned char *u, unsigned char *v,
							  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							  unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_yuv420_r90(unsigned char *y, unsigned char *u, unsigned char *v,
						  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						  unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_yuv420_l90(unsigned char *y, unsigned char *u, unsigned char *v,
						  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						  unsigned char *out_buf, int out_stride, unsigned char *mb_skip);

extern void Yuv2yuv_L90(unsigned char *y, unsigned char *u, unsigned char *v,
							  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							  unsigned char *out_buf, int out_stride, unsigned char *mb_skip);

extern void Yuv2yuv_R90(unsigned char *y, unsigned char *u, unsigned char *v,
							  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							  unsigned char *out_buf, int out_stride, unsigned char *mb_skip);

extern void Yuv2yuv_180(unsigned char *y, unsigned char *u, unsigned char *v,
							  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							  unsigned char *out_buf, int out_stride, unsigned char *mb_skip);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void cc_rgb24_disable(unsigned char *y, unsigned char *u, unsigned char *v,
							 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							 unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_rgb24_l90(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_rgb24_r90(unsigned char *y, unsigned char *u, unsigned char *v,
						 int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						 unsigned char *out_buf, int out_stride, unsigned char *mb_skip);

extern void cc_s_rgb24_disable(unsigned char *y, unsigned char *u, unsigned char *v,
							   int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							   unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_rgb24_l90(unsigned char *y, unsigned char *u, unsigned char *v,
						   int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						   unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_rgb24_r90(unsigned char *y, unsigned char *u, unsigned char *v,
						   int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						   unsigned char *out_buf, int out_stride, unsigned char *mb_skip);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern void cc_rgb565_disable(unsigned char *y, unsigned char *u, unsigned char *v,
							  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							  unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_rgb565_l90(unsigned char *y, unsigned char *u, unsigned char *v,
						  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						  unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_rgb565_r90(unsigned char *y, unsigned char *u, unsigned char *v,
						  int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
						  unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_rgb565_disable(unsigned char *y, unsigned char *u, unsigned char *v,
								int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
								unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_rgb565_l90(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);
extern void cc_s_rgb565_r90(unsigned char *y, unsigned char *u, unsigned char *v,
							int in_width, int in_height, int * Pin_stride, int out_width, int out_height,
							unsigned char *out_buf, int out_stride, unsigned char *mb_skip);

CC_CON cc_s_yuv420_diable;
CC_CON cc_s_yuv420_r90;
CC_CON cc_s_yuv420_l90;
CC_CON cc_yuv420_diable;
CC_CON cc_yuv420_r90;
CC_CON cc_yuv420_l90;

CC_CON cc_rgb24_disable;
CC_CON cc_rgb24_l90;
CC_CON cc_rgb24_r90;
CC_CON cc_s_rgb24_disable;
CC_CON cc_s_rgb24_l90;
CC_CON cc_s_rgb24_r90;

CC_CON cc_rgb565_disable;
CC_CON cc_rgb565_l90;
CC_CON cc_rgb565_r90;
CC_CON cc_s_rgb565_disable;
CC_CON cc_s_rgb565_l90;
CC_CON cc_s_rgb565_r90;

extern const CC_CON_PTR cc_all[4][2][16];

typedef struct
{
	int in_width;
	int in_height;
	int in_stride;
	int out_width;
	int out_height;
	int out_stride;
	int *x_tab;
	int *y_tab;
	INTYPE in_type;
	CC_CON *cc;
}
CCHND;

#endif //_CC_CONSTANT_H__
