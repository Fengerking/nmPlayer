/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/

#ifndef _VOH264ENC_MB_H_
#define _VOH264ENC_MB_H_

enum macroblock_position_e
{
    MB_LEFT     = 0x01,
    MB_TOP      = 0x02,
    MB_TOPRIGHT = 0x04,
    MB_TOPLEFT  = 0x08,

    MB_PRIVATE  = 0x10,

    ALL_NEIGHBORS = 0xf,
};



/* XXX nMBType isn't the one written in the bitstream -> only internal usage */
#define IS_INTRA(type) ( (type) == I4x4 || (type) == I8x8 || (type) == I16x16 || (type) == IPCM )
#define IS_SKIP(type)  ( (type) == PSKIP || (type) == BSKIP )

enum MB_TYPE
{
  I4x4 = 0,
  I8x8 = 1,
  I16x16 = 2,
  IPCM = 3,
  PL0 = 4,
  P8x8 = 5,
  PSKIP = 6,
  BDIRECT = 7,
  BL0L0 = 8,
  BL0L1 = 9,
  BL0BI = 10,
  BL1L0 = 11,
  BL1L1 = 12,
  BL1BI = 13,
  BBIL0 = 14,
  BBIL1 = 15,
  BBIBI = 16,
  B8x8 = 17,
  BSKIP = 18,
  MAX_MB_TYPE = 19
};


static const VO_U8 MBTypeFix[MAX_MB_TYPE] =
{
  I4x4, I4x4, I16x16, IPCM, PL0, P8x8, PSKIP,
  BDIRECT, BL0L0, BL0L1, BL0BI, BL1L0, BL1L1,
  BL1BI, BBIL0, BBIL1, BBIBI, B8x8, BSKIP
};

/*
enum mb_partition_e
{
    
    D_L0_4x4          = 0,
    D_L0_8x4          = 1,
    D_L0_4x8          = 2,
    D_L0_8x8          = 3,

  
    D_L1_4x4          = 4,
    D_L1_8x4          = 5,
    D_L1_4x8          = 6,
    D_L1_8x8          = 7,

    D_BI_4x4          = 8,
    D_BI_8x4          = 9,
    D_BI_4x8          = 10,
    D_BI_8x8          = 11,
    D_DIRECT_8x8      = 12,


    D_8x8             = 13,
    D_16x8            = 14,
    D_8x16            = 15,
    D_16x16           = 16,
    AVC_PARTTYPE_MAX = 17,
};
*/
static const VO_U8 BlkIndex[16] =
{
    0, 1, 4, 5, 2, 3, 6, 7, 8, 9, 12, 13, 10, 11, 14, 15
};


static const VO_U8 QP_chroma[52+12*2] =
{
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   0,  0,  0,  1,  2,  3,  4,  5,  6,  7,
   8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
  18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 
  28, 29, 29, 30, 31, 32, 32, 33, 34, 34, 
  35, 35, 36, 36, 37, 37, 37, 38, 38, 38, 
  39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 
  39, 39, 39, 39, 39, 39,
};

enum CavlcCtxBlockCat
{
  CAVLC_LUMA_DC   = 0,
  CAVLC_LUMA_AC   = 1,
  CAVLC_LUMA_4x4  = 2,
  CAVLC_CHROMA_DC = 3,
  CAVLC_CHROMA_AC = 4,
  CAVLC_LUMA_8x8  = 5,
};

void EncodeMBStart( H264ENC *pEncGlobal, VO_S32 nMBX, VO_S32 nMBY ,H264ENC_L *pEncLocal);
void EncodeMBEnd( H264ENC *pEncGlobal ,H264ENC_L *pEncLocal);


/* GetPredictMV:
 *      set mvp with predicted mv for D_16x16 block
 *      pEncGlobal->mb. need only valid values from other blocks */
void GetPredictMV( H264ENC *pEncGlobal, VO_S16 mvp[2] );
/* PredicMVSkip:
 *      set mvp with predicted mv for P_SKIP
 *      pEncGlobal->mb. need only valid values from other blocks */
void PredicMVSkip( H264ENC *pEncGlobal, VO_S16 mv[2] );

/* GetCandidateMV:
 *      set mvc with D_16x16 prediction.
 *      uses all neighbors, even those that didn't end up using this ref.
 *      pEncGlobal->mb. need only valid values from other blocks */
void GetCandidateMV( H264ENC *pEncGlobal, VO_S16 mvc[8][2], VO_S32 *i_mvc , H264ENC_L *pEncLocal);
void InterMBCompesation( H264ENC *pEncGlobal , H264ENC_L *pEncLocal);
#define MBUpdateMV( a,mv ) UpdateMBMV( a, M32( mv ) )
static VO_ALWAYS_INLINE void UpdateMBMV( H264ENC *pEncGlobal, VO_U32 mv )
{
  VO_U32 *d = (VO_U32 *)&pEncGlobal->mb.mv[AVC_SCAN8_0];
  VO_S32 dy;
  for( dy = 0; dy < 4; dy++ )
  {
	M32( d+8*dy+0 ) = mv;
	M32( d+8*dy+1 ) = mv;
	M32( d+8*dy+2 ) = mv;
	M32( d+8*dy+3 ) = mv;
  }
}

static VO_ALWAYS_INLINE void MBUpdateRef( H264ENC *pEncGlobal)
{
  VO_U32 *d = (VO_U32 *)&pEncGlobal->mb.ref[AVC_SCAN8_0];
  M32( d+0 ) = 0;
  M32( d+2 ) = 0;
  M32( d+4 ) = 0;
  M32( d+6 ) = 0;
}


static VOINLINE VO_S32 PredictMBNZC( H264ENC *pEncGlobal, VO_S32 idx )
{
  const VO_S32 za = pEncGlobal->mb.exmb_nzc[cache_pos[idx] - 1];
  const VO_S32 zb = pEncGlobal->mb.exmb_nzc[cache_pos[idx] - 8];

  VO_S32 i_ret = za + zb;

  if( i_ret < 0x80 )
  {
    i_ret = ( i_ret + 1 ) >> 1;
  }
  return i_ret & 0x7f;
}


extern const VO_S32 Lambda2Tab[52];
extern const VO_U8 LambdaTab[52];



VO_S32 PredictSkipMB( H264ENC *pEncGlobal, H264ENC_L *pEncLocal);

void MBCompensation      ( H264ENC *pEncGlobal, H264ENC_L *pEncLocal);

void WriteMBCavlc ( H264ENC *pEncGlobal , H264ENC_L *pEncLocal);

void GetChromaResidual( H264ENC *pEncGlobal, H264ENC_L *pEncLocal);


#endif //_VOH264ENC_MB_H_

