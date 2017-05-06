/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#ifndef _VOH264ENC_GLOBAL_H_
#define _VOH264ENC_GLOBAL_H_

/****************************************************************************
 * Macros
 ****************************************************************************/
#define AVC_MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define AVC_MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define AVC_MIN3(a,b,c) AVC_MIN((a),AVC_MIN((b),(c)))



#define CHECKED_MALLOC( var, size )\
do {\
    var = voMalloc( size );\
    if( !var )\
        goto fail;\
} while( 0 )
#define CHECKED_MALLOCZERO( var, size )\
do {\
    CHECKED_MALLOC( var, size );\
    memset( var, 0, size );\
} while( 0 )
#define MEMORY_ALLOC( dst, buffer, size, used_size,type )\
{\
  dst = (type)(buffer + used_size);\
  used_size += size;\
}
#define CHECK_SIZE(size)\
{\
  size += 3;\
  size &= ~3;\
}
#define ALIGN(x,a) (((x)+((a)-1))&~((a)-1))

// arbitrary, but low because SATD scores are 1/4 normal
#define AVC_LOOKAHEAD_QP 12



/****************************************************************************
 * Includes
 ****************************************************************************/
#include "voH264EncID.h"
#include "voVCodecCommon.h"
#include "voMem.h"


/* Unions for type-punning.
 * Mn: load or store n bits, aligned, native-endian
 * CPn: copy n bits, aligned, native-endian
 * we don't use memcpy for CPn because memcpy's args aren't assumed to be aligned */
typedef union { VO_U16 i; VO_U8  c[2]; } VO_MAY_ALIAS UNION16_TYPE;
typedef union { VO_U32 i; VO_U16 b[2]; VO_U8  c[4]; } VO_MAY_ALIAS UNION32_TYPE;
typedef union { VO_U64 i; VO_U32 a[2]; VO_U16 b[4]; VO_U8 c[8]; } VO_MAY_ALIAS UNION64_TYPE;
#define M16(src) (((UNION16_TYPE*)(src))->i)
#define M32(src) (((UNION32_TYPE*)(src))->i)
#define M64(src) (((UNION64_TYPE*)(src))->i)
#define CP16(dst,src) M16(dst) = M16(src)
#define CP32(dst,src) M32(dst) = M32(src)
#define CP64(dst,src) M64(dst) = M64(src)

#include "voH264Encoder.h"
#include "voMPEGWriteBits.h"
#include "voH264IntraPredction.h"
#include "voH264EncSAD.h"
#include "voH264MC.h"
#include "voH264Deblock.h"
#include "voH264Frame.h"
#include "voH264Dct.h"
#include "voH264Quant.h"

/****************************************************************************
 * General functions
 ****************************************************************************/
/* voMalloc : will do or emulate a memalign
 * you have to use voFree for buffers allocated with voMalloc */
void *voMalloc( VO_S32 );
void  voFree( void * );


VO_S32 EncodeNAL( VO_U8 *dst, VO_S32 b_annexb, VO_NAL *nal );


static VOINLINE VO_U8 Clip255( VO_S32 x )
{
  return (VO_U8)(x&(~255) ? (-x)>>31 : x);
}

static VOINLINE VO_S32 AVSClip3( VO_S32 v, VO_S32 nMin, VO_S32 nMax )
{
  return ( (v < nMin) ? nMin : (v > nMax) ? nMax : v );
}

static VOINLINE double AVCClips3F( double v, double fMin, double fMax )
{
  return ( (v < fMin) ? fMin : (v > fMax) ? fMax : v );
}

typedef struct 
{
  VO_S32 nStartMB;
  VO_S32 nEndMB;

  VO_S32 nQP;
  /* deblocking filter */
  VO_S32 bDisableDeblockIdc;
  VO_S32 nAlphaC0Offset;
  VO_S32 nBetaOffset;

} H264SLICEHEADER;



/* From ffmpeg
 */
#define AVC_SCAN8_SIZE (6*8)
#define AVC_SCAN8_0 (4+1*8)

static const VO_S32 cache_pos[16+8+3] =
{
  //luma
  12, 13, 20, 21, 14, 15, 22, 23,
  28, 29, 36, 37, 30, 31, 38, 39,
  //chroma CB CR
   9, 10, 17, 18, 33, 34, 41, 42,
  // DC yuv
  44, 45, 46
};
/*
   0 1 2 3 4 5 6 7
 0
 1   u u   y y y y
 2   u u   y y y y
 3           y y y y
 4   v v   y y y y
 5   v v   y u v
*/

typedef struct RateControl   RateControl;

struct H264ENC_L
{
  // neighboring MBs 
  VO_U32 nAvailNeigh;
  VO_S32 nTopMBType;//YU_TBD remove it
  VO_S32 nLeftMBType;
  VO_S32 nTopLeftMBType;
  VO_S32 nTopRightMBType;

  VO_S8  TopLeftRef[4];
  // Current MB 
  VO_S32 nMBX;
  VO_S32 nMBY;
  VO_S32 nMBXY;
  VO_S32 nMV4;

  VO_S32 min_MV[2];
  VO_S32 max_MV[2];
  VO_S32 min_MV_search[2];
  VO_S32 max_MV_search[2];
  //analyse
  VO_S32 nQP;
  VO_S32 nChromaQP;
  VO_S32 nLastQP;  /* last qp */

  VO_U16 *pMVCost;
  VO_S32 nMBCost;
  VO_S32 nMBType;
  VO_S32 nLumaMode;
  VO_S32 nChormaMode;
  VO_S32 nLambda;
  VO_S16 mv[2];
  VO_S32 nLumaCBP;
  VO_S32 nChromaCBP;
  //hpel
  VO_S16 scratch_buffer[32*18];
  VO_S16 mvi[2];
  VO_S16 mvp[2];
  VO_S32 cost_mv;        /* nLambda * nbits for the chosen mv */
  VO_S32 cost;
  VO_ALIGNED_16( VO_U8 pix[4][32*18] ); //0H 1V 2C 3 oneMB 
  VO_S32 skip_th;
};

#define FENC_STRIDE 16
#define FDEC_STRIDE 32

struct H264ENC
{
  // encoder parameters 
#define CONFIG_OK 0xFFFFFFFF
  VO_U32 nConfig;
  INTER_PARAM    InternalParam;
  OUT_PARAM      OutParam;
  VO_U8 *buffer_total;

  VO_U32 buffer_used;
  /* bitstream output */
  struct
  {
    VO_S32 nNAL;
    VO_S32 nNALNum;
    VO_NAL	*nal;
    VO_S32 nBSSize;    /* size of pBitstream */
    VO_U8 *pBitstream;   /* will hold data for all nal */
    BS_TYPE bs;
  } out;

  VO_U8 *pOutBuf;
  VO_S32 nNALBufSize;

  /* frame number/poc */
  VO_S32 nGFrameNum;
  VO_S32 nLFrameNum;

  /* We use only one SPS and one PPS */
  H264LEVEL *level;
  VO_S32 log2_max_frame_num;

  // poc pic_order_cnt
  VO_S32 log2_max_poc_lsb;

  VO_S32 nIDRPicID;
	
  VO_S32 *dequant_table;
  VO_U16 *quant_table;     
  VO_U16 *quant_bias[2]; 

  VO_U16 *cost_mv[92];
  VO_U16 *cost_mv_fpel[92][4];

  const VO_U8 *pChromaQP; 
  // Slice header 
  H264SLICEHEADER sh;

  VO_S32 mb_width;
  VO_S32 mb_height;

  VO_S32 frame_cropping_flag;
  VO_S32 frame_crop_left_offset;
  VO_S32 frame_crop_right_offset;
  VO_S32 frame_crop_top_offset;
  VO_S32 frame_crop_bottom_offset;

  // YUV buffer 
  VO_S32 i_stride[3];
  VO_S32 i_width[3];
  VO_S32 height[3];
  VO_S32 mb_stride;
  VO_S32 mb_pic;                

  VO_S8  *nMBType;
  VO_S16 (*mv)[2];
  VO_S8  *ref;  

  VO_S32 pic_type;
  AVC_FRAME *fdec;
  AVC_FRAME *fref0;     /* ref list 0 */

#ifdef CAL_PSNR
  AVC_FRAME *orig;
#endif

  VO_S32 i_cost_est;
  VO_S32 pic_cost;

  VO_ALIGNED_16( VO_U8 compress_cache[24*FENC_STRIDE] );
  VO_ALIGNED_16( VO_U8 recon_cache[27*FDEC_STRIDE] );
  VO_U8 *pCur[3];// enc cache
  VO_U8 *pDec[3];// dec cache
  VO_U8 *pRef[4+2]; 
  VO_S8  *qp;                        
  VO_U8 (*mzc_mbrow)[16+4+4];  // nzc. for I_PCM set to 16 

  //DCT coeffs for current MB
  struct
  {
    VO_ALIGNED_16( VO_S16 luma_dc[16] );
    VO_ALIGNED_16( VO_S16 chroma_dc[2][4] );
    // for luma and chroma ac
    VO_ALIGNED_16( VO_S16 ac_4x4[16+8][16] );
  } dct;

  /* MB table and cache for current frame/mb */
  struct
  {
    VO_U8 *LastMBRowLine[3]; /* bottom pixels of the previous mb row, used for intra prediction after the framebuffer has been deblocked */
	VO_U8 top_left_pix[4]; /* only used 3*/
	VO_S32 b_skip_mc;

	VO_ALIGNED_4( VO_U8 exmb_nzc[AVC_SCAN8_SIZE] );
    //-1 if unused, -2 if unavailable 
	VO_ALIGNED_4( VO_S8 ref[AVC_SCAN8_SIZE] );
    // 0 if not available 
	VO_ALIGNED_16( VO_S16 mv[AVC_SCAN8_SIZE][2] );
	VO_ALIGNED_4( VO_S16 pskip_mv[2] );
	VO_ALIGNED_4( VO_S16 mvp[2] );
  } mb;

  RateControl *rc;

  VO_PTR phLicenseCheck;
  

  VO_S32 enc_quality;
  VO_S32 fastskip;
  VO_S32 me_method;
  VO_MEM_OPERATOR* memOperater;
  VO_U32 codec_id;
};
enum
{
    BLOCK_16x16 = 0,
    BLOCK_8x8   = 1,
    BLOCK_16x8  = 2,
    BLOCK_8x16  = 3,
    BLOCK_8x4   = 4,
    BLOCK_4x8   = 5,
    BLOCK_4x4   = 6,
    BLOCK_4x2   = 7,
    BLOCK_2x4   = 8,
    BLOCK_2x2   = 9,
};

#include "voH264EncMB.h"

//analyse flags
#define VO_ANALYSE_I4x4       0x0001  //Analyse i4x4 
#define VO_ANALYSE_I8x8       0x0002  //Analyse i8x8 (requires 8x8 transform) 
#define VO_ANALYSE_PSUB16x16  0x0010  //Analyse p16x8, p8x16 and p8x8 
#define VO_ANALYSE_PSUB8x8    0x0020  //Analyse p8x4, p4x8, p4x4 
#define VO_ANALYSE_HPEL   	  0x0100  //Analyse half pixel
#define VO_ME_DIA                  0
#define VO_ME_HEX                  1


#endif // _VOH264ENC_GLOBAL_H_

