/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/


//#include <stdlib.h>
//#include <string.h>
//#include <math.h>

#include "voMpeg4Postprocess.h"
#include "voMpegBuf.h"
#if ENABLE_DEBLOCK


/* Some useful (and fast) macros
   Note that the MIN/MAX macros assume signed shift - if your compiler
   doesn't do signed shifts, use the default MIN/MAX macros from global.h */

#define FAST_MAX(x,y) ((x) - ((((x) - (y))>>(32 - 1)) & ((x) - (y))))
#define FAST_MIN(x,y) ((x) + ((((y) - (x))>>(32 - 1)) & ((y) - (x))))
#define FAST_ABS(x) ((((VO_S32)(x)) >> 31) ^ ((VO_S32)(x))) - (((VO_S32)(x)) >> 31)
#define ABS(X)    (((X)>0)?(X):-(X)) 


void
image_postproc(POSTPROC *tbls, VO_IMGYUV * img, VO_S32 ExWidth,
				VO_U32 *quant_map, VO_S32 nMBWidth, VO_S32 nMBHeight, VO_S32 mb_stride,
				VO_S32 brightness, VO_S32 pf_mode)
{
	const VO_S32 edged_width2 = ExWidth /2;
	VO_S32 i,j;
	VO_S32 quant;
	VO_S32 dering = pf_mode & 0x2;//TBD

	/* luma: j,i in block units */
	if (pf_mode&0x1)// TBD
	{
		for (j = 1; j < nMBHeight*2; j++){		/* horizontal deblocking */
			for (i = 0; i < nMBWidth*2; i++){
				quant = quant_map[(j+0)/2*mb_stride + (i/2)];
				deblock8x8_h(tbls, img->y + j*8*ExWidth + i*8, ExWidth, quant, dering);
			}
		}

		for (j = 0; j < nMBHeight*2; j++){		/* vertical deblocking */
			for (i = 1; i < nMBWidth*2; i++){
				quant = quant_map[(j+0)/2*mb_stride + (i/2)];
				deblock8x8_v(tbls, img->y + j*8*ExWidth + i*8, ExWidth, quant, dering);
			}
		}
	}


	/* chroma */
	if (pf_mode&0x1)//TBD
	{		
		for (j = 1; j < nMBHeight; j++){		/* horizontal deblocking */
			for (i = 0; i < nMBWidth; i++)
			{
				quant = quant_map[(j+0)*mb_stride + i];
				deblock8x8_h(tbls, img->u + j*8*edged_width2 + i*8, edged_width2, quant, dering);
				deblock8x8_h(tbls, img->v + j*8*edged_width2 + i*8, edged_width2, quant, dering);
			}
		}
		
		for (j = 0; j < nMBHeight; j++){	/* vertical deblocking */	
			for (i = 1; i < nMBWidth; i++)
			{
				quant = quant_map[(j+0)*mb_stride + i];
				deblock8x8_v(tbls, img->u + j*8*edged_width2 + i*8, edged_width2, quant, dering);
				deblock8x8_v(tbls, img->v + j*8*edged_width2 + i*8, edged_width2, quant, dering);
			}
		}
	}

//	if (!bvop)
//		tbls->prev_quant = quant_map[0];//mbs->quant; TBD
}

/******************************************************************************/

void init_deblock(POSTPROC *tbls)
{
	VO_S32 i;

	for(i = -255; i < 256; i++) {
		tbls->mpeg4_thresh_tbl[i + 255] = 0;
		if(ABS(i) < THR1)
			tbls->mpeg4_thresh_tbl[i + 255] = 1;
		tbls->mpeg4_abs_tbl[i + 255] = ABS(i);
	}
}

#define LOAD_DATA_HOR(x) \
		/* Load pixel addresses and data for filtering */ \
	    s[0] = *(v[0] = img - 5*stride + x); \
		s[1] = *(v[1] = img - 4*stride + x); \
		s[2] = *(v[2] = img - 3*stride + x); \
		s[3] = *(v[3] = img - 2*stride + x); \
		s[4] = *(v[4] = img - 1*stride + x); \
		s[5] = *(v[5] = img + 0*stride + x); \
		s[6] = *(v[6] = img + 1*stride + x); \
		s[7] = *(v[7] = img + 2*stride + x); \
		s[8] = *(v[8] = img + 3*stride + x); \
		s[9] = *(v[9] = img + 4*stride + x);

#define LOAD_DATA_VER(x) \
		/* Load pixel addresses and data for filtering */ \
		s[0] = *(v[0] = img + x*stride - 5); \
		s[1] = *(v[1] = img + x*stride - 4); \
		s[2] = *(v[2] = img + x*stride - 3); \
		s[3] = *(v[3] = img + x*stride - 2); \
		s[4] = *(v[4] = img + x*stride - 1); \
		s[5] = *(v[5] = img + x*stride + 0); \
		s[6] = *(v[6] = img + x*stride + 1); \
		s[7] = *(v[7] = img + x*stride + 2); \
		s[8] = *(v[8] = img + x*stride + 3); \
		s[9] = *(v[9] = img + x*stride + 4);

#define APPLY_DERING(x) \
		*v[x] = (e[x] == 0) ? (			\
			(e[x-1] == 0) ? (			\
			(e[x+1] == 0) ? 			\
			((s[x-1]+s[x]*2+s[x+1])>>2)	\
			: ((s[x-1]+s[x])>>1) )		\
			: ((s[x]+s[x+1])>>1) )		\
			: s[x];	

#define APPLY_FILTER_CORE \
		/* First, decide whether to use default or DC-nOffset mode */ \
		\
		eq_cnt = 0; \
		\
		eq_cnt += tbls->mpeg4_thresh_tbl[s[0] - s[1] + 255]; \
		eq_cnt += tbls->mpeg4_thresh_tbl[s[1] - s[2] + 255]; \
		eq_cnt += tbls->mpeg4_thresh_tbl[s[2] - s[3] + 255]; \
		eq_cnt += tbls->mpeg4_thresh_tbl[s[3] - s[4] + 255]; \
		eq_cnt += tbls->mpeg4_thresh_tbl[s[4] - s[5] + 255]; \
		eq_cnt += tbls->mpeg4_thresh_tbl[s[5] - s[6] + 255]; \
		eq_cnt += tbls->mpeg4_thresh_tbl[s[6] - s[7] + 255]; \
		eq_cnt += tbls->mpeg4_thresh_tbl[s[7] - s[8] + 255]; \
		\
		if(eq_cnt < THR2) { /* Default mode */  \
			VO_S32 a30, a31, a32;					\
			VO_S32 diff, limit;					\
												\
			if(tbls->mpeg4_abs_tbl[(s[4] - s[5]) + 255] < quant) {			\
				a30 = ((s[3]<<1) - s[4] * 5 + s[5] * 5 - (s[6]<<1));	\
				a31 = ((s[1]<<1) - s[2] * 5 + s[3] * 5 - (s[4]<<1));	\
				a32 = ((s[5]<<1) - s[6] * 5 + s[7] * 5 - (s[8]<<1));	\
																		\
				diff = (5 * ((sign(a30) * min(FAST_ABS(a30), min(FAST_ABS(a31), FAST_ABS(a32)))) - a30) + 32) >> 6;	\
				limit = (s[4] - s[5]) / 2;	\
				\
				if (limit > 0)				\
					diff = (diff < 0) ? 0 : ((diff > limit) ? limit : diff);	\
				else	\
					diff = (diff > 0) ? 0 : ((diff < limit) ? limit : diff);	\
																				\
				*v[4] -= diff;	\
				*v[5] += diff;	\
			}	\
			if (dering) {	\
				e[0] = (tbls->mpeg4_abs_tbl[(s[0] - s[1]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[1] = (tbls->mpeg4_abs_tbl[(s[1] - s[2]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[2] = (tbls->mpeg4_abs_tbl[(s[2] - s[3]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[3] = (tbls->mpeg4_abs_tbl[(s[3] - s[4]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[4] = (tbls->mpeg4_abs_tbl[(s[4] - s[5]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[5] = (tbls->mpeg4_abs_tbl[(s[5] - s[6]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[6] = (tbls->mpeg4_abs_tbl[(s[6] - s[7]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[7] = (tbls->mpeg4_abs_tbl[(s[7] - s[8]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				e[8] = (tbls->mpeg4_abs_tbl[(s[8] - s[9]) + 255] > quant + DERING_STRENGTH) ? 1 : 0;	\
				\
				e[1] |= e[0];	\
				e[2] |= e[1];	\
				e[3] |= e[2];	\
				e[4] |= e[3];	\
				e[5] |= e[4];	\
				e[6] |= e[5];	\
				e[7] |= e[6];	\
				e[8] |= e[7];	\
				e[9]  = e[8];	\
				\
				APPLY_DERING(1)	\
				APPLY_DERING(2)	\
				APPLY_DERING(3)	\
				APPLY_DERING(4)	\
				APPLY_DERING(5)	\
				APPLY_DERING(6)	\
				APPLY_DERING(7)	\
				APPLY_DERING(8) \
			}	\
		}	\
		else {	/* DC-nOffset mode */	\
			VO_U8 p0, p9;	\
			VO_S32 min, max;	\
							\
			/* Now decide whether to apply smoothing filter or not */	\
			max = FAST_MAX(s[1], FAST_MAX(s[2], FAST_MAX(s[3], FAST_MAX(s[4], FAST_MAX(s[5], FAST_MAX(s[6], FAST_MAX(s[7], s[8])))))));	\
			min = FAST_MIN(s[1], FAST_MIN(s[2], FAST_MIN(s[3], FAST_MIN(s[4], FAST_MIN(s[5], FAST_MIN(s[6], FAST_MIN(s[7], s[8])))))));	\
			\
			if(((max-min)) < 2*quant) {	\
										\
				/* Choose edge pixels */	\
				p0 = (tbls->mpeg4_abs_tbl[(s[1] - s[0]) + 255] < quant) ? s[0] : s[1];	\
				p9 = (tbls->mpeg4_abs_tbl[(s[8] - s[9]) + 255] < quant) ? s[9] : s[8];	\
																\
				*v[1] = (VO_U8) ((6*p0 + (s[1]<<2) + (s[2]<<1) + (s[3]<<1) + s[4] + s[5] + 8) >> 4);	\
				*v[2] = (VO_U8) (((p0<<2) + (s[1]<<1) + (s[2]<<2) + (s[3]<<1) + (s[4]<<1) + s[5] + s[6] + 8) >> 4);	\
				*v[3] = (VO_U8) (((p0<<1) + (s[1]<<1) + (s[2]<<1) + (s[3]<<2) + (s[4]<<1) + (s[5]<<1) + s[6] + s[7] + 8) >> 4);	\
				*v[4] = (VO_U8) ((p0 + s[1] + (s[2]<<1) + (s[3]<<1) + (s[4]<<2) + (s[5]<<1) + (s[6]<<1) + s[7] + s[8] + 8) >> 4);	\
				*v[5] = (VO_U8) ((s[1] + s[2] + (s[3]<<1) + (s[4]<<1) + (s[5]<<2) + (s[6]<<1) + (s[7]<<1) + s[8] + p9 + 8) >> 4);	\
				*v[6] = (VO_U8) ((s[2] + s[3] + (s[4]<<1) + (s[5]<<1) + (s[6]<<2) + (s[7]<<1) + (s[8]<<1) + (p9<<1) + 8) >> 4);	\
				*v[7] = (VO_U8) ((s[3] + s[4] + (s[5]<<1) + (s[6]<<1) + (s[7]<<2) + (s[8]<<1) + (p9<<2) + 8) >> 4);	\
				*v[8] = (VO_U8) ((s[4] + s[5] + (s[6]<<1) + (s[7]<<1) + (s[8]<<2) + 6*p9 + 8) >> 4);	\
			}	\
		}	

void deblock8x8_h(POSTPROC *tbls, VO_U8 *img, VO_S32 stride, VO_S32 quant, VO_S32 dering)
{
	VO_S32 eq_cnt;
	VO_U8 *v[10];
	VO_S32 s[10];
	VO_S32 e[10];

	LOAD_DATA_HOR(0)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(1)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(2)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(3)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(4)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(5)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(6)
	APPLY_FILTER_CORE

	LOAD_DATA_HOR(7)
	APPLY_FILTER_CORE
}


void deblock8x8_v(POSTPROC *tbls, VO_U8 *img, VO_S32 stride, VO_S32 quant, VO_S32 dering)
{
	VO_S32 eq_cnt;
	VO_U8 *v[10];
	VO_S32 s[10];
	VO_S32 e[10];

	LOAD_DATA_VER(0)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(1)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(2)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(3)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(4)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(5)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(6)
	APPLY_FILTER_CORE

	LOAD_DATA_VER(7)
	APPLY_FILTER_CORE
}
#endif
