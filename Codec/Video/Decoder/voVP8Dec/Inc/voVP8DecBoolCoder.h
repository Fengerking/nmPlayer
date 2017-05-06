/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __VP8_DEC_BOOLCODER_H
#define __VP8_DEC_BOOLCODER_H

#include "voVP8GlobleDef.h"
//#include "voVP8Common.h"
#include "voVP8DecMBlock.h"

# define VP8_BD_VALUE_SIZE ((int)sizeof(VP8_BD_VALUE)*8)

#define COEF_BANDS 8
#define vp8_coef_tokens 12
#define MAX_ENTROPY_TOKENS vp8_coef_tokens
#define PREV_COEF_CONTEXTS 3
#define vp8_prob_half   128

#define VP8_NUMMBSPLITS 4

/* Coefficient token alphabet */

#define ZERO_TOKEN              0       //0         Extra Bits 0+0
#define ONE_TOKEN               1       //1         Extra Bits 0+1
#define TWO_TOKEN               2       //2         Extra Bits 0+1
#define THREE_TOKEN             3       //3         Extra Bits 0+1
#define FOUR_TOKEN              4       //4         Extra Bits 0+1
#define DCT_VAL_CATEGORY1       5       //5-6       Extra Bits 1+1
#define DCT_VAL_CATEGORY2       6       //7-10      Extra Bits 2+1
#define DCT_VAL_CATEGORY3       7       //11-26     Extra Bits 4+1
#define DCT_VAL_CATEGORY4       8       //11-26     Extra Bits 5+1
#define DCT_VAL_CATEGORY5       9       //27-58     Extra Bits 5+1
#define DCT_VAL_CATEGORY6       10      //59+       Extra Bits 11+1
#define DCT_EOB_TOKEN           11      //EOB       Extra Bits 0+0

typedef signed char vp8_tree_index;
//typedef unsigned char unsigned char;

//typedef unsigned char ENTROPY_CONTEXT;
typedef unsigned int  VP8_BD_VALUE;
typedef const int vp8_mbsplit[16];


typedef const vp8_tree_index vp8_tree[], *vp8_tree_p;
typedef const struct vp8_token_struct
{
    int value;
    int Len;
} vp8_token;

enum
{
    mv_max  = 1023,              /* max absolute value of a MV component */
    MVvals = (2 * mv_max) + 1,   /* # possible values "" */

    mvlong_width = 10,       /* Large MVs have 9 bit magnitudes */
    mvnum_short = 8,         /* magnitudes 0 through 7 */

    /* probability offsets for coding each MV component */

    mvpis_short = 0,         /* short (<= 7) vs long (>= 8) */
    MVPsign,                /* sign for non-zero */
    MVPshort,               /* 8 short values = 7-position tree */

    MVPbits = MVPshort + mvnum_short - 1, /* mvlong_width long value bits */
    MVPcount = MVPbits + mvlong_width    /* (with independent probabilities) */
};

typedef struct mv_context
{
    unsigned char prob[MVPcount];  /* often come in row, col pairs */
} MV_CONTEXT;

typedef struct
{
    const unsigned char *user_buffer_end;
    const unsigned char *user_buffer;
	unsigned int         pos;  //postition of user_buffer;
    VP8_BD_VALUE         value;  //the encoded 32 bits
    int                  count;  //
    unsigned int         range;
} BOOL_DECODER;


void vp8_default_bmode_probs(unsigned char p [VP8_BINTRAMODES-1]);
void vp8_coef_tree_initialize();
void vp8_entropy_mode_init();

# define VP8_LOTS_OF_BITS 0x40000000

#ifdef STABILITY
//static int handle_error( BOOL_DECODER *bc,int * pos,unsigned char* _bufend,unsigned char*_bufptr)
//{
//	*pos = (_bufend-_bufptr-1);
//	//return 0;
//}
#define VP8DX_BOOL_DECODER_FILL(_count,_value,_bufptr,_bufend, _pos) \
    do \
    { \
        int shift; \
        for(shift = VP8_BD_VALUE_SIZE - 8 - ((_count) + 8); shift >= 0; ) \
        { \
            if((_bufptr) >= (_bufend)) { \
                (_count) = VP8_LOTS_OF_BITS; \
                break; \
            } \
            (_count) += 8; \
			if(_pos>(unsigned int)(_bufend-_bufptr)) \
			_pos = _bufend-_bufptr-1;\
			else \
            (_value) |= (VP8_BD_VALUE)(_bufptr[(_pos)++]) << shift; \
            shift -= 8; \
        } \
    } \
    while(0)
#else
#define VP8DX_BOOL_DECODER_FILL(_count,_value,_bufptr,_bufend, _pos) \
    do \
    { \
        int shift; \
        for(shift = VP8_BD_VALUE_SIZE - 8 - ((_count) + 8); shift >= 0; ) \
        { \
            if((_bufptr) >= (_bufend)) { \
                (_count) = VP8_LOTS_OF_BITS; \
                break; \
            } \
            (_count) += 8; \
            (_value) |= (VP8_BD_VALUE)(_bufptr[(_pos)++]) << shift; \
            shift -= 8; \
        } \
    } \
    while(0)
#endif

#define VP8_MVREFS (1 + SPLITMV - NEARESTMV)
//struct vp8_token_struct vp8_mv_ref_encoding_array    [VP8_MVREFS];
//struct vp8_token_struct vp8_sub_mv_ref_encoding_array [VP8_SUBMVREFS];
//#define VP8_MVREFENCODINGS      (vp8_mv_ref_encoding_array - NEARESTMV)
//#define VP8_SUBMVREFENCODINGS   (vp8_sub_mv_ref_encoding_array - LEFT4X4)


typedef BOOL_DECODER vp8_reader;
extern const MV_CONTEXT vp8_default_mv_context[2];
extern const unsigned char vp8_coef_update_probs [4] [COEF_BANDS] [PREV_COEF_CONTEXTS] [vp8_coef_tokens-1];


#define vp8_read vp8dx_decode_bool
#define vp8_read_literal vp8_decode_value
#define vp8_read_bit( R) vp8dx_decode_bool_2(R) //vp8_read( R, vp8_prob_half)

int vp8dx_start_decode(BOOL_DECODER *br, const unsigned char *source, unsigned int source_sz) ;

extern const unsigned int vp8dx_bitreader_norm[256];
static INLINE void vp8dx_bool_decoder_fill_c(BOOL_DECODER *br)
{
    const unsigned char *bufptr;
    const unsigned char *bufend;
    VP8_BD_VALUE         value;
    int                  count;
	int                  pos;
    bufend = br->user_buffer_end;
    bufptr = br->user_buffer;
    value = br->value;
    count = br->count;
	pos    = br->pos;

	 //pos = bufend-bufptr;
		//return VPX_CODEC_CORRUPT_FRAME;

	/*if((bufptr) >= (bufend)) 
	{ 
		(count) = VP8_LOTS_OF_BITS; 
	}*/

     //VP8DX_BOOL_DECODER_FILL(count, value, bufptr, bufend, pos);
	 { 
        int shift; 
        for(shift = 16-count; shift >= 0; ) 
        { 
            (count) += 8;
#ifdef STABILITY
			if(pos>(int)(bufend-bufptr)) //stability
			{
				pos = (bufend-bufptr-1);
			}
			else
#endif
			{
				(value) |= (VP8_BD_VALUE)(bufptr[(pos)++]) << shift;
			}

            shift -= 8; 
        } 
    } 
    br->value = value;
    br->count = count;
	br->pos = pos;
}

static INLINE int vp8dx_decode_bool(BOOL_DECODER *br, int probability) {
    unsigned int bit = 0;
    VP8_BD_VALUE value;
    unsigned int split;
    VP8_BD_VALUE bigsplit;
    int count;
    unsigned int range;
	register unsigned int shift;

    value = br->value;
    count = br->count;
    range = br->range;

    split = 1 + (((range - 1) * probability) >> 8);
    bigsplit = (VP8_BD_VALUE)split << (VP8_BD_VALUE_SIZE - 8);

    range = split;

    if (value >= bigsplit)
    {
        range = br->range - split;
        value = value - bigsplit;
        bit = 1;
    }	

    if(range>=0x80)
    {
        br->value = value;
        br->range = range;
        return bit;
    }

    {
		shift = vp8dx_bitreader_norm[range];
        range <<= shift;
        value <<= shift;
        count -= shift;
    }
    br->value = value;
    br->count = count;
    br->range = range;
    if(count < 0)
		vp8dx_bool_decoder_fill_c(br);  //VPX_CODEC_OK

    return bit;
}

static INLINE int vp8dx_decode_bool_2(BOOL_DECODER *br) {
    unsigned int bit = 0;
    VP8_BD_VALUE value;
    unsigned int split;
    VP8_BD_VALUE bigsplit;
    int count;
    unsigned int range;
	register unsigned int shift;

    value = br->value;
    count = br->count;
    range = br->range;

    split = (range+1)>>1;//1 + (((range - 1) * 128) >> 8);
    bigsplit = (VP8_BD_VALUE)split << (VP8_BD_VALUE_SIZE - 8);

    range = split;

    if (value >= bigsplit)
    {
        range = br->range - split;
        value = value - bigsplit;
        bit = 1;
    }

    {
        shift = vp8dx_bitreader_norm[range];
        range <<= shift;
        value <<= shift;
        count -= shift;
    }
    br->value = value;
    br->count = count;
    br->range = range;
    if(count < 0)
        vp8dx_bool_decoder_fill_c(br);
    return bit;
}

static INLINE int vp8_decode_value(BOOL_DECODER *br, int bits)
{
  /*
   * Until optimized versions of this function are available, we
   * keep the implementation in the header to allow inlining.
   *
   *return DBOOLHUFF_INVOKE(br->rtcd, devalue)(br, bits);
   */
    int z = 0;
    int bit;

    for (bit = bits - 1; bit >= 0; bit--)
    {
        z |= (vp8dx_decode_bool_2(br) << bit);
    }
    return z;
}
#endif