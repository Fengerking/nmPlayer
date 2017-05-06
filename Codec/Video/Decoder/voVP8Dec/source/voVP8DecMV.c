/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "voVP8DecMV.h"
#include "voVP8DecFrame.h"
#include "voVP8DecFindNearMV.h"
#include "voVP8DecReadBits.h"
#include "voVP8DecReadMode.h"

#ifdef LOG_ENABLE
//#define ANDROID_LOG 1
#ifdef VOANDROID
#define LOG_FILE "/data/local/log.txt"
#define LOG_TAG "VOPlayer"
#include <utils/Log.h>
#  define __VOLOG(fmt, args...) ({LOGD("->%d: %s(): " fmt, __LINE__, __FUNCTION__, ## args);})
#  define __VOLOGFILE(fmt, args...) ({FILE *fp =fopen(LOG_FILE, "a"); fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, ## args); fclose(fp);})
#elif VOWINXP
#define LOG_FILE "D:/log.txt"
#define LOG_TAG "VOPlayer"
#include "stdio.h"
#define __VOLOG(fmt, ...)  printf("[ %s ]->%d: %s(): "fmt"\n",LOG_TAG,__LINE__,__FUNCTION__,__VA_ARGS__);
#define __VOLOGFILE(fmt, ...) {FILE *fp =fopen(LOG_FILE, "a");fprintf(fp, "[ %s ]->%d: %s(): " fmt "\n", LOG_TAG, __LINE__, __FUNCTION__, __VA_ARGS__); fclose(fp);}
#endif
#else
#define __VOLOGFILE
#endif

typedef union
{
    unsigned int as_int;
    MV           as_mv;
} int_mv;

extern const vp8_tree_index vp8_mbsplit_tree[6];
//extern vp8_mbsplit vp8_mbsplits [VP8_NUMMBSPLITS];
const int vp8_mbsplit_count [VP8_NUMMBSPLITS] = { 2, 2, 4, 16};

const unsigned char vp8_mbsplit_probs [VP8_NUMMBSPLITS-1] = { 110, 111, 150};

const int vp8_mode_contexts[6][4] =
{
	{
        7,     1,     1,   143,
    },
    {
        // 1
        14,    18,    14,   107,
    },
    {
        // 2
        135,    64,    57,    68,
    },
    {
        // 3
        60,    56,   128,    65,
    },
    {
        // 4
        159,   134,   128,    34,
    },
    {
        // 5
        234,   188,   128,    28,
    },
};

const unsigned char vp8_sub_mv_ref_prob2 [SUBMVREF_COUNT][VP8_SUBMVREFS-1] =
{
    { 147, 136, 18 },
    { 106, 145, 1  },
    { 179, 121, 1  },
    { 223, 1  , 34 },
    { 208, 1  , 1  }
};

const MV_CONTEXT vp8_default_mv_context[2] =
{
    {{
        // row
        162,                                        // is short
        128,                                        // sign
        225, 146, 172, 147, 214,  39, 156,          // short tree
        128, 129, 132,  75, 145, 178, 206, 239, 254, 254 // long bits
    }},

    {{
        // same for column
        164,                                        // is short
        128,
        204, 170, 119, 235, 140, 230, 228,
        128, 130, 130,  74, 148, 180, 203, 236, 254, 254 // long bits

    }}
};

const vp8_tree_index vp8_mv_ref_tree[8] =
{
    -ZEROMV, 2,
    -NEARESTMV, 4,
    -NEARMV, 6,
    -NEWMV, -SPLITMV
};
const vp8_tree_index vp8_sub_mv_ref_tree[6] =
{
    -LEFT4X4, 2,
    -ABOVE4X4, 4,
    -ZERO4X4, -NEW4X4
};

const MV_CONTEXT vp8_mv_update_probs[2] =
{
    {{
        237,
        246,
        253, 253, 254, 254, 254, 254, 254,
        254, 254, 254, 254, 254, 250, 250, 252, 254, 254
    }},
    {{
        231,
        243,
        245, 253, 254, 254, 254, 254, 254,
        254, 254, 254, 254, 254, 251, 251, 254, 254, 254
    }}
};

const vp8_tree_index vp8_small_mvtree [14] =
{
    2, 8,
    4, 6,
    -0, -1,
    -2, -3,
    10, 12,
    -4, -5,
    -6, -7
};
const unsigned char vp8_coef_update_probs [4] [COEF_BANDS] [PREV_COEF_CONTEXTS] [vp8_coef_tokens-1] =  // BLOCK_TYPES 4
{ 
    {
        {
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {176, 246, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {223, 241, 252, 255, 255, 255, 255, 255, 255, 255, 255, },
            {249, 253, 253, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 244, 252, 255, 255, 255, 255, 255, 255, 255, 255, },
            {234, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 246, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {239, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {254, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 248, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {251, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {251, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {254, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 254, 253, 255, 254, 255, 255, 255, 255, 255, 255, },
            {250, 255, 254, 255, 254, 255, 255, 255, 255, 255, 255, },
            {254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
    },
    {
        {
            {217, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {225, 252, 241, 253, 255, 255, 254, 255, 255, 255, 255, },
            {234, 250, 241, 250, 253, 255, 253, 254, 255, 255, 255, },
        },
        {
            {255, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {223, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {238, 253, 254, 254, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 248, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {249, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 253, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {247, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {252, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {253, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 254, 253, 255, 255, 255, 255, 255, 255, 255, 255, },
            {250, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
    },
    {
        {
            {186, 251, 250, 255, 255, 255, 255, 255, 255, 255, 255, },
            {234, 251, 244, 254, 255, 255, 255, 255, 255, 255, 255, },
            {251, 251, 243, 253, 254, 255, 254, 255, 255, 255, 255, },
        },
        {
            {255, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {236, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {251, 253, 253, 254, 254, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {254, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
    },
    {
        {
            {248, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {250, 254, 252, 254, 255, 255, 255, 255, 255, 255, 255, },
            {248, 254, 249, 253, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 253, 253, 255, 255, 255, 255, 255, 255, 255, 255, },
            {246, 253, 253, 255, 255, 255, 255, 255, 255, 255, 255, },
            {252, 254, 251, 254, 254, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 254, 252, 255, 255, 255, 255, 255, 255, 255, 255, },
            {248, 254, 253, 255, 255, 255, 255, 255, 255, 255, 255, },
            {253, 255, 254, 254, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 251, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {245, 251, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {253, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 251, 253, 255, 255, 255, 255, 255, 255, 255, 255, },
            {252, 253, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 252, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {249, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 254, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 255, 253, 255, 255, 255, 255, 255, 255, 255, 255, },
            {250, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
        {
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
            {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, },
        },
    },
};



unsigned char vp8_mbsplit_offset[4][16] = {
    { 0,  8,  0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0},
    { 0,  2,  0,  0,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0},
    { 0,  2,  8, 10,  0,  0,  0,  0,  0,  0,   0,  0,  0,  0,  0,  0},
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15}
};
unsigned char vp8_mbsplit_fill_count[4] = {8, 8, 4, 1};
unsigned char vp8_mbsplit_fill_offset[4][16] = {
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15},
    { 0,  1,  4,  5,  8,  9, 12, 13,  2,  3,   6,  7, 10, 11, 14, 15},
    { 0,  1,  4,  5,  2,  3,  6,  7,  8,  9,  12, 13, 10, 11, 14, 15},
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15}
};
unsigned char vp8_J_count[4][16] = {
    { 0,  8},
    { 0,  8},
    { 0,  4,  8,  12},
    { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15}
};


typedef enum
{
    SUBMVREF_NORMAL,
    SUBMVREF_LEFT_ZED,
    SUBMVREF_ABOVE_ZED,
    SUBMVREF_LEFT_ABOVE_SAME,
    SUBMVREF_LEFT_ABOVE_ZED
} sumvfref_t;

//int vp8_mv_cont(const MV *l, const MV *a)
//{
//    int lez = (l->row == 0 && l->col == 0);
//    int aez = (a->row == 0 && a->col == 0);
//    int lea = (l->row == a->row && l->col == a->col);
//
//    if (lea && lez)
//        return SUBMVREF_LEFT_ABOVE_ZED;
//
//    if (lea)
//        return SUBMVREF_LEFT_ABOVE_SAME;
//
//    if (aez)
//        return SUBMVREF_ABOVE_ZED;
//
//    if (lez)
//        return SUBMVREF_LEFT_ZED;
//
//    return SUBMVREF_NORMAL;
//}
INLINE int vp8_mv_cont(const MV *l, const MV *a)
{
	/* int lez = (l->row == 0 && l->col == 0);
     int aez = (a->row == 0 && a->col == 0);
     int lea = (l->row == a->row && l->col == a->col);*/

	int lez  = *((int*)l);// (*((int*)(&l->row)));
	int aez = *((int*)a);// (*((int*)(&a->row)));
	int lea  = (lez == aez);

	//int f1= (*((int*)(&l->row)));
	//int f2= (*((int*)(&a->row)));
	lez = !lez;
	aez = !aez;

	//if(lez!=f1 || aez!=f2)
	//	__VOLOGFILE("%d %d;%d %d ",lez,f1 ,aez,f2);


    if (lea && lez)
        return SUBMVREF_LEFT_ABOVE_ZED;

    if (lea)
        return SUBMVREF_LEFT_ABOVE_SAME;

    if (aez)
        return SUBMVREF_ABOVE_ZED;

    if (lez)
        return SUBMVREF_LEFT_ZED;

    return SUBMVREF_NORMAL;
}

#define CALCULATE_MV(yoffset) \
	mvrow = pModeInfo->bmi[yoffset].mv.as_mv.row \
   + pModeInfo->bmi[yoffset+1].mv.as_mv.row \
   + pModeInfo->bmi[yoffset+4].mv.as_mv.row\
   + pModeInfo->bmi[yoffset+5].mv.as_mv.row;\
	mvcol = pModeInfo->bmi[yoffset].mv.as_mv.col \
   + pModeInfo->bmi[yoffset+1].mv.as_mv.col \
   + pModeInfo->bmi[yoffset+4].mv.as_mv.col \
   + pModeInfo->bmi[yoffset+5].mv.as_mv.col; \
	mvrow = mvrow<0? (mvrow-4)/8:(mvrow+4)/8; \
	mvcol  = mvcol<0? (mvcol-4)/8:(mvcol+4)/8; 

int vp8_build_uvmvs(MACROBLOCKD *x,MODE_INFO *pModeInfo)
{
    if (pModeInfo->mbmi.mode == SPLITMV) 
    {
		int mvrow,mvcol;
		CALCULATE_MV(0);
#ifdef STABILITY
		vp8_clamp_mv_uv(mvrow,mvcol,pModeInfo->mbmi);
#endif
		x->block[20].bmi.mv.as_mv.row = x->block[16].bmi.mv.as_mv.row = mvrow ;
		x->block[20].bmi.mv.as_mv.col = x->block[16].bmi.mv.as_mv.col = mvcol;
#ifdef MV_POSITION
		x->block[20].bmi.mv_position  =   x->block[16].bmi.mv_position = (mvrow>>3)*x->pre.uv_stride + (mvcol>>3);
#endif

		CALCULATE_MV(2);
#ifdef STABILITY
		vp8_clamp_mv_uv(mvrow,mvcol,pModeInfo->mbmi);
#endif
		x->block[21].bmi.mv.as_mv.row = x->block[17].bmi.mv.as_mv.row = mvrow ;
		x->block[21].bmi.mv.as_mv.col = x->block[17].bmi.mv.as_mv.col = mvcol;
#ifdef MV_POSITION
		x->block[21].bmi.mv_position  =   x->block[17].bmi.mv_position = (mvrow>>3)*x->pre.uv_stride + (mvcol>>3);
#endif

		CALCULATE_MV(8);
#ifdef STABILITY
		vp8_clamp_mv_uv(mvrow,mvcol,pModeInfo->mbmi);
#endif
		x->block[22].bmi.mv.as_mv.row = x->block[18].bmi.mv.as_mv.row = mvrow ;
		x->block[22].bmi.mv.as_mv.col = x->block[18].bmi.mv.as_mv.col = mvcol;
#ifdef MV_POSITION
		x->block[22].bmi.mv_position  =   x->block[18].bmi.mv_position = (mvrow>>3)*x->pre.uv_stride + (mvcol>>3);
#endif

		CALCULATE_MV(10);
#ifdef STABILITY
		vp8_clamp_mv_uv(mvrow,mvcol,pModeInfo->mbmi);
#endif
		x->block[23].bmi.mv.as_mv.row = x->block[19].bmi.mv.as_mv.row = mvrow ;
		x->block[23].bmi.mv.as_mv.col = x->block[19].bmi.mv.as_mv.col = mvcol;
#ifdef MV_POSITION
		x->block[23].bmi.mv_position  =   x->block[19].bmi.mv_position = (mvrow>>3)*x->pre.uv_stride + (mvcol>>3);
#endif
    }
    else 
    {
		if(pModeInfo->mbmi.mode == ZEROMV )
		{
			x->block[16].bmi.mv.as_int = 0;
#ifdef MV_POSITION
		x->block[16].bmi.mv_position  =   0;
#endif
		}
		else
		{
			int mvrow =pModeInfo->mbmi.mv.as_mv.row;//x->mbmi.mv.as_mv.row;
			int mvcol =pModeInfo->mbmi.mv.as_mv.col;//x->mbmi.mv.as_mv.col;

			mvrow = mvrow<0? (mvrow-1)/2:(mvrow+1)/2;
			mvcol  = mvcol<0? (mvcol-1)/2:(mvcol+1)/2;
#ifdef STABILITY
			vp8_clamp_mv_uv(mvrow,mvcol,pModeInfo->mbmi);
#endif
			x->block[16].bmi.mv.as_mv.row = mvrow;  // U
			x->block[16].bmi.mv.as_mv.col = mvcol;
#ifdef MV_POSITION
		x->block[16].bmi.mv_position  = (mvrow>>3)*x->pre.uv_stride + (mvcol>>3);
#endif
		}
    }
	return 0;
}
int vp8_build_uvmvs_fullpixel(MACROBLOCKD *x,MODE_INFO *pModeInfo)
{
    int i, j;
    if (pModeInfo->mbmi.mode == SPLITMV)
    {
        for (i = 0; i < 2; i++)
        {
            for (j = 0; j < 2; j++)
            {
                int yoffset = i * 8 + j * 2;
                int uoffset = 16 + i * 2 + j;
                int voffset = 20 + i * 2 + j;
                int mvrow,mvcol;

                mvrow = pModeInfo->bmi[yoffset].mv.as_mv.row
                       + pModeInfo->bmi[yoffset+1].mv.as_mv.row
                       + pModeInfo->bmi[yoffset+4].mv.as_mv.row
                       + pModeInfo->bmi[yoffset+5].mv.as_mv.row;

				 mvcol = pModeInfo->bmi[yoffset].mv.as_mv.col
                       + pModeInfo->bmi[yoffset+1].mv.as_mv.col
                       + pModeInfo->bmi[yoffset+4].mv.as_mv.col
                       + pModeInfo->bmi[yoffset+5].mv.as_mv.col;

				mvrow = mvrow<0? (mvrow-4)/8:(mvrow+4)/8;
				mvcol  = mvcol<0? (mvcol-4)/8:(mvcol+4)/8;
				mvrow = mvrow & 0xfffffff8;
				mvcol  = mvcol & 0xfffffff8;

#ifdef STABILITY
				vp8_clamp_mv_uv(mvrow,mvcol,pModeInfo->mbmi);
#endif

				x->block[voffset].bmi.mv.as_mv.row = x->block[uoffset].bmi.mv.as_mv.row = mvrow;
				x->block[voffset].bmi.mv.as_mv.col = x->block[uoffset].bmi.mv.as_mv.col = mvcol;
#ifdef MV_POSITION
		x->block[voffset].bmi.mv_position  =   x->block[uoffset].bmi.mv_position = ((mvrow & 0xfffffff8)>>3)*x->pre.uv_stride + ((mvcol & 0xfffffff8)>>3);
#endif
            }
        }
    }
    else
    {
        int mvrow =pModeInfo->mbmi.mv.as_mv.row;//x->mbmi.mv.as_mv.row;
        int mvcol =pModeInfo->mbmi.mv.as_mv.col;//x->mbmi.mv.as_mv.col;

		mvrow = mvrow<0? (mvrow-1)/2:(mvrow+1)/2;
		mvcol  = mvcol<0? (mvcol-1)/2:(mvcol+1)/2;

		mvrow = mvrow & 0xfffffff8;
		mvcol  = mvcol & 0xfffffff8;

#ifdef STABILITY
		vp8_clamp_mv_uv(mvrow,mvcol,pModeInfo->mbmi);
#endif

		x->block[16].bmi.mv.as_mv.row = mvrow;  // U
		x->block[16].bmi.mv.as_mv.col = mvcol;		
#ifdef MV_POSITION
		x->block[16].bmi.mv_position  = ((mvrow & 0xfffffff8)>>3)*x->pre.uv_stride + ((mvcol & 0xfffffff8)>>3);
#endif
    }
	return 0;
}
static int read_mvcomponent(vp8_reader *r, const MV_CONTEXT *mvc)
{
    const unsigned char *const p = (const unsigned char *) mvc;
    int x = 0;

    if (vp8_read(r, p [mvpis_short]))  /* Large */
    {
		x += vp8_read(r, p [MVPbits]);
		x += vp8_read(r, p [MVPbits + 1]) << 1;
		x += vp8_read(r, p [MVPbits + 2]) << 2;

		x += vp8_read(r, p [MVPbits + 9]) << 9;
		x += vp8_read(r, p [MVPbits + 8]) << 8;
		x += vp8_read(r, p [MVPbits + 7]) << 7;
		x += vp8_read(r, p [MVPbits + 6]) << 6;
		x += vp8_read(r, p [MVPbits + 5]) << 5;
		x += vp8_read(r, p [MVPbits + 4]) << 4;

        if (!(x & 0xFFF0)  ||  vp8_read(r, p [MVPbits + 3]))
            x += 8;
    }
    else   /* small */
        x = vp8_treed_read(r, vp8_small_mvtree, p + MVPshort);

    if (x  &&  vp8_read(r, p [MVPsign]))
        x = -x;

    return x;
}
static void read_mv(vp8_reader *r, MV *mv, const MV_CONTEXT *mvc)
{
    mv->row = (short)(read_mvcomponent(r,   mvc) << 1);
    mv->col = (short)(read_mvcomponent(r, ++mvc) << 1);
}


static void read_mvcontexts(vp8_reader *bc, MV_CONTEXT *mvc)
{
    int i = 0;

    do
    {
        const unsigned char *up = vp8_mv_update_probs[i].prob;
        unsigned char *p = (unsigned char *)(mvc + i);
        unsigned char *const pstop = p + MVPcount;

        do
        {
            if (vp8_read(bc, *up++))
            {
                const unsigned char x = (unsigned char)vp8_read_literal(bc, 7);

                *p = x ? x << 1 : 1;
            }
        }
        while (++p < pstop);
    }
    while (++i < 2);
}

#define read_mv_ref(bc,p)    vp8_treed_read(bc, vp8_mv_ref_tree, p)

#define sub_mv_ref(bc,p)    vp8_treed_read(bc, vp8_sub_mv_ref_tree, p)


int decoder_mode_mvs_mbrow(VP8D_COMP *pbi,int mb_row)
{
	VP8_COMMON *const pc = & pbi->common;
	vp8_reader *const bc = & pbi->bc;
    MODE_INFO *mi = pc->mi +mb_row*(pc->mb_cols+1);
	const int mis = pc->mode_info_stride;
	 MV_CONTEXT *const mvc = pc->fc.mvc;
	const MV Zero = { 0, 0};
	unsigned char prob_intra = pbi->prob_intra;
	unsigned char prob_last = pbi->prob_last;
	unsigned char prob_gf = pbi->prob_gf;
	unsigned char prob_skip_false = pbi->prob_skip_false;
	int mb_col = -1;
		
        while (++mb_col < pc->mb_cols)
        {
            MB_MODE_INFO *const mbmi = & mi->mbmi;
            MV *const mv = & mbmi->mv.as_mv;
            VP8_COMMON *const pc = &pbi->common;
            MACROBLOCKD *xd = &pbi->mb;

            // mbmi->need_to_clamp_mvs = 0;
            // Distance of Mb to the various image edges.
            // These specified to 8th pel as they are always compared to MV values that are in 1/8th pel units

            // If required read in new segmentation data for this MB
            if (pbi->mb.update_mb_segmentation_map)
			{
                vp8_read_mb_features(bc, mbmi, &pbi->mb);
			}

            // Read the macroblock coeff skip flag if this feature is in use, else default to 0
            if (pc->mb_no_coeff_skip)
                mbmi->mb_skip_coeff = vp8_read(bc, prob_skip_false);
            else
                mbmi->mb_skip_coeff = 0;

            mbmi->uv_mode = DC_PRED;

            if ((mbmi->ref_frame = (MV_REFERENCE_FRAME) vp8_read(bc, prob_intra)))    /* inter MB */
            {
                //unsigned char mv_ref_p [VP8_MVREFS-1];
				int_mv            near_mvs[4];
                //MV nearest, nearby, best_mv;
				MV  mvLeft_Mb,mvAbove_Mb;			

                if (vp8_read(bc, prob_last))
                    mbmi->ref_frame = (MV_REFERENCE_FRAME)((int)mbmi->ref_frame + (int)(1 + vp8_read(bc, prob_gf)));

#ifdef FILTER_LEVEL 
			if(xd->mode_ref_lf_delta_enabled)
					mi->filter_level = xd->ref_lf_deltas[mbmi->ref_frame]; //zou 1.18
#endif

                //vp8_find_near_mvs(xd, mi, &nearest, &nearby, &best_mv, mv_ref_p, mbmi->ref_frame, pbi->common.ref_frame_sign_bias);
				{
					const MODE_INFO *above = mi - xd->mode_info_stride;
					const MODE_INFO *left = mi - 1;
					const MODE_INFO *aboveleft = above - 1;
					//int_mv            near_mvs[4];
					int_mv           *nearmv = near_mvs;
					int cnt[4];
					int             *cntx = cnt;
					enum {CNT_INTRA, CNT_NEAREST, CNT_NEAR, CNT_SPLITMV};
					nearmv[0].as_int = nearmv[1].as_int = nearmv[2].as_int = 0;
					cnt[0] = cnt[1] = cnt[2] = cnt[3] = 0;
					if (above->mbmi.ref_frame != INTRA_FRAME)
					{
						if (above->mbmi.mv.as_int)
						{
							(++nearmv)->as_int = above->mbmi.mv.as_int;
							if (pbi->common.ref_frame_sign_bias[above->mbmi.ref_frame] != pbi->common.ref_frame_sign_bias[mbmi->ref_frame])
							{
								nearmv->as_mv.row =  above->mbmi.mv.as_mv.row * -1;
								nearmv->as_mv.col =  above->mbmi.mv.as_mv.col  * -1;
							} 
							++cntx;
						}
						*cntx += 2;
					}

					/* Process left */
					if (left->mbmi.ref_frame != INTRA_FRAME)
					{
						if (left->mbmi.mv.as_int)
						{
							int_mv this_mv;
							this_mv.as_int = left->mbmi.mv.as_int;
							if (pbi->common.ref_frame_sign_bias[left->mbmi.ref_frame] != pbi->common.ref_frame_sign_bias[mbmi->ref_frame])
							{
								this_mv.as_mv.row =  left->mbmi.mv.as_mv.row * -1;
								this_mv.as_mv.col =  left->mbmi.mv.as_mv.col  * -1;
							}
							if (this_mv.as_int != nearmv->as_int)
							{
								(++nearmv)->as_int = this_mv.as_int;
								++cntx;
							}
							*cntx += 2;
						}
						else
							cnt[CNT_INTRA] += 2;
					}

					/* Process above left */
					if (aboveleft->mbmi.ref_frame != INTRA_FRAME)
					{
						if (aboveleft->mbmi.mv.as_int)
						{
							int_mv this_mv;
							this_mv.as_int = aboveleft->mbmi.mv.as_int;
							//mv_bias(aboveleft, refframe, &this_mv, ref_frame_sign_bias);
							if (pbi->common.ref_frame_sign_bias[aboveleft->mbmi.ref_frame] != pbi->common.ref_frame_sign_bias[mbmi->ref_frame])
							{
								this_mv.as_mv.row =  aboveleft->mbmi.mv.as_mv.row * -1;
								this_mv.as_mv.col =  aboveleft->mbmi.mv.as_mv.col  * -1;
							}
							if (this_mv.as_int != nearmv->as_int)
							{
								(++nearmv)->as_int = this_mv.as_int;
								++cntx;
							}
							*cntx += 1;
						}
						else
							cnt[CNT_INTRA] += 1;
					}

					/* If we have three distinct MV's ... */
					if (cnt[CNT_SPLITMV])
					{
						/* See if above-left MV can be merged with NEAREST */
						if (nearmv->as_int == near_mvs[CNT_NEAREST].as_int)
							cnt[CNT_NEAREST] += 1;
					}

					cnt[CNT_SPLITMV] = ((above->mbmi.mode == SPLITMV)
										+ (left->mbmi.mode == SPLITMV)) * 2
									   + (aboveleft->mbmi.mode == SPLITMV);

					/* Swap near and nearest if necessary */
					if (cnt[CNT_NEAR] > cnt[CNT_NEAREST])
					{
						int tmp;
						tmp = cnt[CNT_NEAREST];
						cnt[CNT_NEAREST] = cnt[CNT_NEAR];
						cnt[CNT_NEAR] = tmp;
						{
							register vp8_tree_index i = 0;
							while ((i = vp8_mv_ref_tree[ i + vp8_read(bc, vp8_mode_contexts[cnt[i>>1]][i>>1])]) > 0);
							mbmi->mode = -i;
						}

						if(mbmi->mode ==ZEROMV)
						{
							*mv = Zero;
#ifdef MV_POSITION
							mi->mbmi.mv_position =0;
#endif
#ifdef FILTER_LEVEL
							mi->filter_level += xd->mode_lf_deltas[1];
#endif
							/*mi++;
							continue;*/
							goto GETMV;
						}

						tmp = near_mvs[CNT_NEAREST].as_int;
						near_mvs[CNT_NEAREST].as_int = near_mvs[CNT_NEAR].as_int;
						near_mvs[CNT_NEAR].as_int = tmp;
					}
					else
					{
						{
							register vp8_tree_index i = 0;
							while ((i = vp8_mv_ref_tree[ i + vp8_read(bc, vp8_mode_contexts[cnt[i>>1]][i>>1])]) > 0);
							mbmi->mode = -i;
						}
						if(mbmi->mode ==ZEROMV)
						{
							*mv = Zero;
 #ifdef MV_POSITION
							mi->mbmi.mv_position =0;
#endif
#ifdef FILTER_LEVEL
							mi->filter_level +=  xd->mode_lf_deltas[1];
#endif
							/*mi++;
							continue;*/
							goto GETMV;
						}
					}

					/* Use near_mvs[0] to store the "best" MV */
					if (cnt[CNT_NEAREST] >= cnt[CNT_INTRA])
						near_mvs[CNT_INTRA] = near_mvs[CNT_NEAREST];

					/* Set up return values */
					/*best_mv = near_mvs[0].as_mv;
					nearest = near_mvs[CNT_NEAREST].as_mv;
					nearby = near_mvs[CNT_NEAR].as_mv;*/
				}
				//__VOLOGFILE("mbmi->mode : %d ",mbmi->mode);
                switch (mbmi->mode/* = read_mv_ref(bc, mv_ref_p)*/)
                {
				case SPLITMV:
                {
                    const int s = mbmi->partitioning = vp8_treed_read(bc, vp8_mbsplit_tree, vp8_mbsplit_probs);
                    const int num_p = vp8_mbsplit_count [s];
                    int j = 0;
#ifdef FILTER_LEVEL 
					mi->filter_level +=  xd->mode_lf_deltas[3];
#endif
					//__VOLOGFILE("SPLITMV : %d ",SPLITMV);
                    do  /* for each subset j */
                    {
						B_MODE_INFO  bmi;
                        MV *const mv = & bmi.mv.as_mv;
                        int k = -1;  /* first block in subset j */
                        int mv_contz;
						k = vp8_mbsplit_offset[s][j]; //zou 1230
						mvLeft_Mb =vp8_left_mv(mi, k);//vp8_left_bmi(mi, k)->mv.as_mv;
						mvAbove_Mb =  vp8_above_mv(mi, k, mis);//vp8_above_bmi(mi, k, mis)->mv.as_mv;
                        mv_contz = vp8_mv_cont(&mvLeft_Mb, &mvAbove_Mb);
						bmi.mode = (B_PREDICTION_MODE) sub_mv_ref(bc, vp8_sub_mv_ref_prob2 [mv_contz]);
						//__VOLOGFILE("bmi.mode:%d  mv_contz:%d ",bmi.mode,mv_contz);
                        switch (bmi.mode)
                        {
                        case NEW4X4:
                            read_mv(bc, mv, (const MV_CONTEXT *) mvc);  
							vp8_clamp_mv(near_mvs[0].as_mv, mbmi);
							mv->row += near_mvs[0].as_mv.row;
                            mv->col += near_mvs[0].as_mv.col;
							vp8_clamp_mv((*mv),mbmi);

							//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
							bmi.mv_position = (mv->row>>3)*xd->pre.y_stride + (mv->col>>3);
#endif
                            break;
                        case LEFT4X4:
                            *mv = mvLeft_Mb;
							//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
							bmi.mv_position = (mvLeft_Mb.row>>3)*xd->pre.y_stride + (mvLeft_Mb.col>>3);
#endif
                            break;
                        case ABOVE4X4:
                            *mv = mvAbove_Mb;
							//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
							bmi.mv_position = (mvAbove_Mb.row>>3)*xd->pre.y_stride + (mvAbove_Mb.col>>3);
#endif
                            break;
                        case ZERO4X4:
                            *mv = Zero;	
#ifdef MV_POSITION
							bmi.mv_position = 0;
#endif
                            break;
                        default:
                            break;
                        }                 
						{
							unsigned char *fill_offset;
							unsigned int fill_count = vp8_mbsplit_fill_count[s];
							fill_offset = &vp8_mbsplit_fill_offset[s][ vp8_J_count[s][j] ];
							do 
							{
								mi->bmi[ *fill_offset] = bmi;
								fill_offset++;
							}while (--fill_count);
						}
                    }
                    while (++j < num_p);
                }
                *mv = mi->bmi[15].mv.as_mv;
				//__VOLOGFILE("%d %d",mv->row,mv->col);
                break;  /* done with SPLITMV */
                case NEARMV:
					vp8_clamp_mv(near_mvs[2].as_mv, mbmi);
                    *mv = near_mvs[2].as_mv;
					//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
					mbmi->mv_position = (mv->row>>3)*xd->pre.y_stride + (mv->col>>3);
#endif
#ifdef FILTER_LEVEL 
					mi->filter_level +=  xd->mode_lf_deltas[2];
#endif
					break;
                case NEARESTMV:
					vp8_clamp_mv(near_mvs[1].as_mv, mbmi);
                    *mv = near_mvs[1].as_mv;
					//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
					mbmi->mv_position = (mv->row>>3)*xd->pre.y_stride + (mv->col>>3);
#endif
#ifdef FILTER_LEVEL 
					mi->filter_level +=  xd->mode_lf_deltas[2];
#endif
					break;
                /*case ZEROMV:
                   *mv = Zero;
					break;*/
                case NEWMV:
                    read_mv(bc, mv, (const MV_CONTEXT *) mvc);
					vp8_clamp_mv(near_mvs[0].as_mv, mbmi); 
                    mv->row += near_mvs[0].as_mv.row;
                    mv->col += near_mvs[0].as_mv.col;

					vp8_clamp_mv((*mv),mbmi);
					//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
					mbmi->mv_position = (mv->row>>3)*xd->pre.y_stride + (mv->col>>3);
#endif
#ifdef FILTER_LEVEL 
					mi->filter_level +=  xd->mode_lf_deltas[2];  
#endif
                    break;
                default:;
                }
            }
            else
            {
                /* MB is intra coded */
                int j = 0;

#ifdef FILTER_LEVEL 
			if(xd->mode_ref_lf_delta_enabled)
					mi->filter_level = xd->ref_lf_deltas[mbmi->ref_frame]; //zou 1.18
#endif
				//nu2++;
                do
                {
                    mi->bmi[j].mv.as_mv = Zero;
                }
                while (++j < 16);
                *mv = Zero;
                if ((mbmi->mode = (MB_PREDICTION_MODE) vp8_read_ymode(bc, pc->fc.ymode_prob)) == B_PRED)
                {
                    int j = 0;
                    do
                    {
                        mi->bmi[j].mode = (B_PREDICTION_MODE)vp8_read_bmode(bc, pc->fc.bmode_prob);
                    }
                    while (++j < 16);
#ifdef FILTER_LEVEL 
					mi->filter_level +=  xd->mode_lf_deltas[0];
#endif
                }
                mbmi->uv_mode = (MB_PREDICTION_MODE)vp8_read_uv_mode(bc, pc->fc.uv_mode_prob);
            }
GETMV:
            mi++;       // next macroblock
        }
        
		return 0;
}


void vp8_decode_mode_mvs_new(VP8D_COMP *pbi)
{
    VP8_COMMON *const pc = & pbi->common;
    vp8_reader *const bc = & pbi->bc;
    MV_CONTEXT *const mvc = pc->fc.mvc;

	pbi->prob_skip_false = 0;

    if (pc->mb_no_coeff_skip)
        pbi->prob_skip_false = (unsigned char)vp8_read_literal(bc, 8);

    pbi->prob_intra = (unsigned char)vp8_read_literal(bc, 8);
    pbi->prob_last  = (unsigned char)vp8_read_literal(bc, 8);
    pbi->prob_gf    = (unsigned char)vp8_read_literal(bc, 8);


    if (vp8_read_bit(bc))
    {
        int i = 0;
        do
        {
            pc->fc.ymode_prob[i] = (unsigned char) vp8_read_literal(bc, 8);
        }
        while (++i < 4);
    }
	
    if (vp8_read_bit(bc))
    {
        int i = 0;
        do
        {
            pc->fc.uv_mode_prob[i] = (unsigned char) vp8_read_literal(bc, 8);
        }
        while (++i < 3);
    }

    read_mvcontexts(bc, mvc);
}

void vp8_decode_mode_mvs(VP8D_COMP *pbi)
{
    const MV Zero = { 0, 0};

    VP8_COMMON *const pc = & pbi->common;
    vp8_reader *const bc = & pbi->bc;

    MODE_INFO *mi = pc->mi, *ms;
    const int mis = pc->mode_info_stride;

    MV_CONTEXT *const mvc = pc->fc.mvc;

    int mb_row = -1;
    unsigned char prob_intra;
    unsigned char prob_last;
    unsigned char prob_gf;
    unsigned char prob_skip_false = 0;

    if (pc->mb_no_coeff_skip)
        prob_skip_false = (unsigned char)vp8_read_literal(bc, 8);

	//prob_intra_last_gf = (unsigned int)vp8_read_literal(bc, 24)
    prob_intra = (unsigned char)vp8_read_literal(bc, 8);
    prob_last  = (unsigned char)vp8_read_literal(bc, 8);
    prob_gf    = (unsigned char)vp8_read_literal(bc, 8);

    ms = pc->mi - 1;


    if (vp8_read_bit(bc))
    {
        int i = 0;
        do
        {
            pc->fc.ymode_prob[i] = (unsigned char) vp8_read_literal(bc, 8);
        }
        while (++i < 4);
    }
	
    if (vp8_read_bit(bc))
    {
        int i = 0;
        do
        {
            pc->fc.uv_mode_prob[i] = (unsigned char) vp8_read_literal(bc, 8);
        }
        while (++i < 3);
    }

    read_mvcontexts(bc, mvc);

    while (++mb_row < pc->mb_rows)
    {
        int mb_col = -1;
        while (++mb_col < pc->mb_cols)
        {
            MB_MODE_INFO *const mbmi = & mi->mbmi;
            MV *const mv = & mbmi->mv.as_mv;
            VP8_COMMON *const pc = &pbi->common;
            MACROBLOCKD *xd = &pbi->mb;

            // mbmi->need_to_clamp_mvs = 0;
            // Distance of Mb to the various image edges.
            // These specified to 8th pel as they are always compared to MV values that are in 1/8th pel units

            // If required read in new segmentation data for this MB
            if (pbi->mb.update_mb_segmentation_map)
			{
                vp8_read_mb_features(bc, mbmi, &pbi->mb);
			}

            // Read the macroblock coeff skip flag if this feature is in use, else default to 0
            if (pc->mb_no_coeff_skip)
                mbmi->mb_skip_coeff = vp8_read(bc, prob_skip_false);
            else
                mbmi->mb_skip_coeff = 0;

            mbmi->uv_mode = DC_PRED;

            if ((mbmi->ref_frame = (MV_REFERENCE_FRAME) vp8_read(bc, prob_intra)))    /* inter MB */
            {
                //unsigned char mv_ref_p [VP8_MVREFS-1];
				int_mv            near_mvs[4];
                //MV nearest, nearby, best_mv;
				MV  mvLeft_Mb,mvAbove_Mb;			

                if (vp8_read(bc, prob_last))
                    mbmi->ref_frame = (MV_REFERENCE_FRAME)((int)mbmi->ref_frame + (int)(1 + vp8_read(bc, prob_gf)));

#ifdef FILTER_LEVEL 
			if(xd->mode_ref_lf_delta_enabled)
					mi->filter_level = xd->ref_lf_deltas[mbmi->ref_frame]; //zou 1.18
#endif

                //vp8_find_near_mvs(xd, mi, &nearest, &nearby, &best_mv, mv_ref_p, mbmi->ref_frame, pbi->common.ref_frame_sign_bias);
				{
					const MODE_INFO *above = mi - xd->mode_info_stride;
					const MODE_INFO *left = mi - 1;
					const MODE_INFO *aboveleft = above - 1;
					//int_mv            near_mvs[4];
					int_mv           *nearmv = near_mvs;
					int cnt[4];
					int             *cntx = cnt;
					enum {CNT_INTRA, CNT_NEAREST, CNT_NEAR, CNT_SPLITMV};
					nearmv[0].as_int = nearmv[1].as_int = nearmv[2].as_int = 0;
					cnt[0] = cnt[1] = cnt[2] = cnt[3] = 0;
					if (above->mbmi.ref_frame != INTRA_FRAME)
					{
						if (above->mbmi.mv.as_int)
						{
							(++nearmv)->as_int = above->mbmi.mv.as_int;
							if (pbi->common.ref_frame_sign_bias[above->mbmi.ref_frame] != pbi->common.ref_frame_sign_bias[mbmi->ref_frame])
							{
								nearmv->as_mv.row =  above->mbmi.mv.as_mv.row * -1;
								nearmv->as_mv.col =  above->mbmi.mv.as_mv.col  * -1;
							} 
							++cntx;
						}
						*cntx += 2;
					}

					/* Process left */
					if (left->mbmi.ref_frame != INTRA_FRAME)
					{
						if (left->mbmi.mv.as_int)
						{
							int_mv this_mv;
							this_mv.as_int = left->mbmi.mv.as_int;
							if (pbi->common.ref_frame_sign_bias[left->mbmi.ref_frame] != pbi->common.ref_frame_sign_bias[mbmi->ref_frame])
							{
								this_mv.as_mv.row =  left->mbmi.mv.as_mv.row * -1;
								this_mv.as_mv.col =  left->mbmi.mv.as_mv.col  * -1;
							}
							if (this_mv.as_int != nearmv->as_int)
							{
								(++nearmv)->as_int = this_mv.as_int;
								++cntx;
							}
							*cntx += 2;
						}
						else
							cnt[CNT_INTRA] += 2;
					}

					/* Process above left */
					if (aboveleft->mbmi.ref_frame != INTRA_FRAME)
					{
						if (aboveleft->mbmi.mv.as_int)
						{
							int_mv this_mv;
							this_mv.as_int = aboveleft->mbmi.mv.as_int;
							//mv_bias(aboveleft, refframe, &this_mv, ref_frame_sign_bias);
							if (pbi->common.ref_frame_sign_bias[aboveleft->mbmi.ref_frame] != pbi->common.ref_frame_sign_bias[mbmi->ref_frame])
							{
								this_mv.as_mv.row =  aboveleft->mbmi.mv.as_mv.row * -1;
								this_mv.as_mv.col =  aboveleft->mbmi.mv.as_mv.col  * -1;
							}
							if (this_mv.as_int != nearmv->as_int)
							{
								(++nearmv)->as_int = this_mv.as_int;
								++cntx;
							}
							*cntx += 1;
						}
						else
							cnt[CNT_INTRA] += 1;
					}

					/* If we have three distinct MV's ... */
					if (cnt[CNT_SPLITMV])
					{
						/* See if above-left MV can be merged with NEAREST */
						if (nearmv->as_int == near_mvs[CNT_NEAREST].as_int)
							cnt[CNT_NEAREST] += 1;
					}

					cnt[CNT_SPLITMV] = ((above->mbmi.mode == SPLITMV)
										+ (left->mbmi.mode == SPLITMV)) * 2
									   + (aboveleft->mbmi.mode == SPLITMV);

					/* Swap near and nearest if necessary */
					if (cnt[CNT_NEAR] > cnt[CNT_NEAREST])
					{
						int tmp;
						tmp = cnt[CNT_NEAREST];
						cnt[CNT_NEAREST] = cnt[CNT_NEAR];
						cnt[CNT_NEAR] = tmp;
						{
							register vp8_tree_index i = 0;
							while ((i = vp8_mv_ref_tree[ i + vp8_read(bc, vp8_mode_contexts[cnt[i>>1]][i>>1])]) > 0);
							mbmi->mode = -i;
						}

						if(mbmi->mode ==ZEROMV)
						{
							*mv = Zero;
#ifdef MV_POSITION
							mi->mbmi.mv_position =0;
#endif
#ifdef FILTER_LEVEL
							mi->filter_level += xd->mode_lf_deltas[1];
#endif
							/*mi++;
							continue;*/
							goto GETMV;
						}

						tmp = near_mvs[CNT_NEAREST].as_int;
						near_mvs[CNT_NEAREST].as_int = near_mvs[CNT_NEAR].as_int;
						near_mvs[CNT_NEAR].as_int = tmp;
					}
					else
					{
						{
							register vp8_tree_index i = 0;
							while ((i = vp8_mv_ref_tree[ i + vp8_read(bc, vp8_mode_contexts[cnt[i>>1]][i>>1])]) > 0);
							mbmi->mode = -i;
						}
						if(mbmi->mode ==ZEROMV)
						{
							*mv = Zero;
 #ifdef MV_POSITION
							mi->mbmi.mv_position =0;
#endif
#ifdef FILTER_LEVEL
							mi->filter_level +=  xd->mode_lf_deltas[1];
#endif
							/*mi++;
							continue;*/
							goto GETMV;
						}
					}

					/* Use near_mvs[0] to store the "best" MV */
					if (cnt[CNT_NEAREST] >= cnt[CNT_INTRA])
						near_mvs[CNT_INTRA] = near_mvs[CNT_NEAREST];

					/* Set up return values */
					/*best_mv = near_mvs[0].as_mv;
					nearest = near_mvs[CNT_NEAREST].as_mv;
					nearby = near_mvs[CNT_NEAR].as_mv;*/
				}
				//__VOLOGFILE("mbmi->mode : %d ",mbmi->mode);
                switch (mbmi->mode/* = read_mv_ref(bc, mv_ref_p)*/)
                {
				case SPLITMV:
                {
                    const int s = mbmi->partitioning = vp8_treed_read(bc, vp8_mbsplit_tree, vp8_mbsplit_probs);
                    const int num_p = vp8_mbsplit_count [s];
                    int j = 0;
#ifdef FILTER_LEVEL 
					mi->filter_level +=  xd->mode_lf_deltas[3];
#endif
					//__VOLOGFILE("SPLITMV : %d ",SPLITMV);
                    do  /* for each subset j */
                    {
						B_MODE_INFO  bmi;
                        MV *const mv = & bmi.mv.as_mv;
                        int k = -1;  /* first block in subset j */
                        int mv_contz;
						k = vp8_mbsplit_offset[s][j]; //zou 1230
						mvLeft_Mb =vp8_left_mv(mi, k);//vp8_left_bmi(mi, k)->mv.as_mv;
						mvAbove_Mb =  vp8_above_mv(mi, k, mis);//vp8_above_bmi(mi, k, mis)->mv.as_mv;
                        mv_contz = vp8_mv_cont(&mvLeft_Mb, &mvAbove_Mb);
						bmi.mode = (B_PREDICTION_MODE) sub_mv_ref(bc, vp8_sub_mv_ref_prob2 [mv_contz]);
						//__VOLOGFILE("bmi.mode:%d  mv_contz:%d ",bmi.mode,mv_contz);
                        switch (bmi.mode)
                        {
                        case NEW4X4:
                            read_mv(bc, mv, (const MV_CONTEXT *) mvc);  
							vp8_clamp_mv(near_mvs[0].as_mv, mbmi);
							mv->row += near_mvs[0].as_mv.row;
                            mv->col += near_mvs[0].as_mv.col;
							vp8_clamp_mv((*mv),mbmi);

							//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
							bmi.mv_position = (mv->row>>3)*xd->pre.y_stride + (mv->col>>3);
#endif
                            break;
                        case LEFT4X4:
                            *mv = mvLeft_Mb;
							//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
							bmi.mv_position = (mvLeft_Mb.row>>3)*xd->pre.y_stride + (mvLeft_Mb.col>>3);
#endif
                            break;
                        case ABOVE4X4:
                            *mv = mvAbove_Mb;
							//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
							bmi.mv_position = (mvAbove_Mb.row>>3)*xd->pre.y_stride + (mvAbove_Mb.col>>3);
#endif
                            break;
                        case ZERO4X4:
                            *mv = Zero;	
#ifdef MV_POSITION
							bmi.mv_position = 0;
#endif
                            break;
                        default:
                            break;
                        }                 
						{
							unsigned char *fill_offset;
							unsigned int fill_count = vp8_mbsplit_fill_count[s];
							fill_offset = &vp8_mbsplit_fill_offset[s][ vp8_J_count[s][j] ];
							do 
							{
								mi->bmi[ *fill_offset] = bmi;
								fill_offset++;
							}while (--fill_count);
						}
                    }
                    while (++j < num_p);
                }
                *mv = mi->bmi[15].mv.as_mv;
				//__VOLOGFILE("%d %d",mv->row,mv->col);
                break;  /* done with SPLITMV */
                case NEARMV:
					vp8_clamp_mv(near_mvs[2].as_mv, mbmi);
                    *mv = near_mvs[2].as_mv;
					//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
					mbmi->mv_position = (mv->row>>3)*xd->pre.y_stride + (mv->col>>3);
#endif
#ifdef FILTER_LEVEL 
					mi->filter_level +=  xd->mode_lf_deltas[2];
#endif
					break;
                case NEARESTMV:
					vp8_clamp_mv(near_mvs[1].as_mv, mbmi);
                    *mv = near_mvs[1].as_mv;
					//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
					mbmi->mv_position = (mv->row>>3)*xd->pre.y_stride + (mv->col>>3);
#endif
#ifdef FILTER_LEVEL 
					mi->filter_level +=  xd->mode_lf_deltas[2];
#endif
					break;
                /*case ZEROMV:
                   *mv = Zero;
					break;*/
                case NEWMV:
                    read_mv(bc, mv, (const MV_CONTEXT *) mvc);
					vp8_clamp_mv(near_mvs[0].as_mv, mbmi); 
                    mv->row += near_mvs[0].as_mv.row;
                    mv->col += near_mvs[0].as_mv.col;

					vp8_clamp_mv((*mv),mbmi);
					//__VOLOGFILE("%d %d",mv->row,mv->col);
#ifdef MV_POSITION
					mbmi->mv_position = (mv->row>>3)*xd->pre.y_stride + (mv->col>>3);
#endif
#ifdef FILTER_LEVEL 
					mi->filter_level +=  xd->mode_lf_deltas[2];  
#endif
                    break;
                default:;
                }
            }
            else
            {
                /* MB is intra coded */
                int j = 0;

#ifdef FILTER_LEVEL 
			if(xd->mode_ref_lf_delta_enabled)
					mi->filter_level = xd->ref_lf_deltas[mbmi->ref_frame]; //zou 1.18
#endif
				//nu2++;
                do
                {
                    mi->bmi[j].mv.as_mv = Zero;
                }
                while (++j < 16);
                *mv = Zero;
                if ((mbmi->mode = (MB_PREDICTION_MODE) vp8_read_ymode(bc, pc->fc.ymode_prob)) == B_PRED)
                {
                    int j = 0;
                    do
                    {
                        mi->bmi[j].mode = (B_PREDICTION_MODE)vp8_read_bmode(bc, pc->fc.bmode_prob);
                    }
                    while (++j < 16);
#ifdef FILTER_LEVEL 
					mi->filter_level +=  xd->mode_lf_deltas[0];
#endif
                }
                mbmi->uv_mode = (MB_PREDICTION_MODE)vp8_read_uv_mode(bc, pc->fc.uv_mode_prob);
            }
GETMV:
            mi++;       // next macroblock
			//mbd->mode_info_context++
        }
        mi++;           // skip left predictor each row
    }
}