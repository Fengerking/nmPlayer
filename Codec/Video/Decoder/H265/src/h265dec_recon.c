/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
/************************************************************************
* @file h265dec_recon.c
*
* H.265 intra & inter decompress implementation.
*
* @author  
************************************************************************/

#include "h265dec_config.h"
#include "h265dec_utils.h"
#include "h265dec_recon.h"
#include "h265dec_inter.h"
//#include "h265dec_pic.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "HMLog.h"

#define ZS_ENABLED 0

#define MAX_MC_CU_SIZE 64



static const VO_S32 intra_pred_angle[] = {
  32, 26, 21, 17, 13, 9, 5, 2, 0, -2, -5, -9, -13, -17, -21, -26, -32,
  -26, -21, -17, -13, -9, -5, -2, 0, 2, 5, 9, 13, 17, 21, 26, 32
};
static const VO_S32 inv_angle[] = {
  -4096, -1638, -910, -630, -482, -390, -315, -256, -315, -390, -482,
  -630, -910, -1638, -4096
};



//#define CC_INTERNAL_TRACE
#ifdef CC_INTERNAL_TRACE

static FILE   *g_internal_dbg;
static VO_U32 g_level;
static __inline void LogInit(VO_U32 uiLogLevel)
{
  g_internal_dbg = fopen("h265dec_recon_debug.txt", "a+");
  g_level = uiLogLevel;
  assert(g_internal_dbg != NULL);
}
static __inline void Log(VO_U32 uiLogLevel, VO_U8 *str, ...)
{
#if MC_ASM_ENABLED==0
  char * args;
  VO_U8 log_buf[1024];
  if (uiLogLevel < g_level)
  {
    va_start(args, str);
    vsprintf(log_buf, str, args);
    va_end(args);
    fprintf(g_internal_dbg,"%s",log_buf);
  }
#endif
}
static __inline void LogUninit()
{
  g_level = 0;
  if (g_internal_dbg)
  {
    fclose(g_internal_dbg);
  }
}

#endif // CC_INTERNAL_TRACE

// extern variables declaration put here!!!
//extern VO_U32 g_uiMaxCUWidth;
//extern VO_S32 g_uiMaxCUHeight;
//extern VO_U8  g_aucConvertTxtTypeToIdx[4];
//extern VO_S8  g_aucConvertToBit  [ MAX_CU_SIZE+1 ]; 
//extern VO_U32 g_auiRasterToZscan [ MAX_NUM_SPU_W*MAX_NUM_SPU_W ];
//extern VO_U32 g_auiZscanToRaster [ MAX_NUM_SPU_W*MAX_NUM_SPU_W ];
//extern VO_U32 g_auiRasterToPelX  [ MAX_NUM_SPU_W*MAX_NUM_SPU_W ];
//extern VO_U32 g_auiRasterToPelY  [ MAX_NUM_SPU_W*MAX_NUM_SPU_W ];

// extern methods declaration put here!!!
//extern VO_U32 GetPUAboveLeft( H265_DEC_SLICE *p_slice, VO_U32* uiALPartUnitIdx, VO_U32 uiCurrPartUnitIdx, VO_BOOL bEnforceSliceRestriction, VO_BOOL bEnforceDependentSliceRestriction, VO_BOOL MotionDataCompresssion );
//extern VO_U32 GetPUAbove( H265_DEC_SLICE *p_slice, VO_U32* uiAPartUnitIdx, VO_U32 uiCurrPartUnitIdx, VO_BOOL bEnforceSliceRestriction, VO_BOOL bEnforceDependentSliceRestriction, VO_BOOL MotionDataCompresssion, VO_BOOL planarAtLCUBoundary ,VO_BOOL bEnforceTileRestriction );
//extern VO_U32 GetPUAboveRightAdi( H265_DEC_SLICE *p_slice, VO_U32* uiARPartUnitIdx, VO_U32 uiPuWidth, VO_U32 uiCurrPartUnitIdx, VO_U32 uiPartUnitOffset, VO_BOOL bEnforceSliceRestriction, VO_BOOL bEnforceDependentSliceRestriction );
//extern VO_U32 GetPULeft( H265_DEC_SLICE *p_slice, VO_U32* uiLPartUnitIdx, VO_U32 uiCurrPartUnitIdx, VO_BOOL bEnforceSliceRestriction, VO_BOOL bEnforceDependentSliceRestriction, VO_BOOL bEnforceTileRestriction );
//extern VO_U32 GetPUBelowLeftAdi(H265_DEC_SLICE *p_slice,VO_U32* uiBLPartUnitIdx, VO_U32 uiPuHeight,  VO_U32 uiCurrPartUnitIdx, VO_U32 uiPartUnitOffset, VO_BOOL bEnforceSliceRestriction, VO_BOOL bEnforceDependentSliceRestriction );


// global variables definitions put here!!!

#if MC_ASM_ENABLED
static const VO_U8 mc_ext_left[4] = { 0, 3, 3, 2 };
static const VO_U8 mc_ext_right[4] = { 0, 3, 4, 4 };
static const VO_U8 mc_ext[4] = { 0, 6, 7, 6 };
#else
static const VO_U8 mc_ext_left[4] = { 0, 3, 3, 3 };
static const VO_U8 mc_ext_right[4] = { 0, 4, 4, 4 };
static const VO_U8 mc_ext[4] = { 0, 7, 7, 7 };
#endif
#if MC_ASM_ENABLED
// Luma Uni
const MC_FUNC arrFun_put_Luma[4][4]={
    { VO_put_MC_InterLuma_H0V0_neon,  VO_put_MC_InterLuma_H0V1_neon,  VO_put_MC_InterLuma_H0V2_neon, VO_put_MC_InterLuma_H0V3_neon},
    { VO_put_MC_InterLuma_H1V0_neon,  VO_put_MC_InterLuma_H1V1_neon,  VO_put_MC_InterLuma_H1V2_neon, VO_put_MC_InterLuma_H1V3_neon},
    { VO_put_MC_InterLuma_H2V0_neon,  VO_put_MC_InterLuma_H2V1_neon,  VO_put_MC_InterLuma_H2V2_neon, VO_put_MC_InterLuma_H2V3_neon},
    { VO_put_MC_InterLuma_H3V0_neon,  VO_put_MC_InterLuma_H3V1_neon,  VO_put_MC_InterLuma_H3V2_neon, VO_put_MC_InterLuma_H3V3_neon}
};

// Chroma Uni
const MC_CHROMA_FUNC arrFun_put_Chroma[2][2]={
    { VO_put_MC_InterChroma_H0V0_neon,  VO_put_MC_InterChroma_H0V1_neon},
    { VO_put_MC_InterChroma_H1V0_neon,  VO_put_MC_InterChroma_H1V1_neon}
};

// Luma Bi
const MC_BI_FUNC arrFun_avg_nornd_MC_Bi_Luma[4][4]={
    { VO_avg_nornd_MC_InterLuma_H0V0_neon,  VO_avg_nornd_MC_InterLuma_H0V1_neon,  VO_avg_nornd_MC_InterLuma_H0V2_neon, VO_avg_nornd_MC_InterLuma_H0V3_neon},
    { VO_avg_nornd_MC_InterLuma_H1V0_neon,  VO_avg_nornd_MC_InterLuma_H1V1_neon,  VO_avg_nornd_MC_InterLuma_H1V2_neon, VO_avg_nornd_MC_InterLuma_H1V3_neon},
    { VO_avg_nornd_MC_InterLuma_H2V0_neon,  VO_avg_nornd_MC_InterLuma_H2V1_neon,  VO_avg_nornd_MC_InterLuma_H2V2_neon, VO_avg_nornd_MC_InterLuma_H2V3_neon},
    { VO_avg_nornd_MC_InterLuma_H3V0_neon,  VO_avg_nornd_MC_InterLuma_H3V1_neon,  VO_avg_nornd_MC_InterLuma_H3V2_neon, VO_avg_nornd_MC_InterLuma_H3V3_neon}
};
const MC_BI_FUNC arrFun_avg_MC_Bi_Luma[4][4]={
    { VO_avg_MC_InterLuma_H0V0_neon,  VO_avg_MC_InterLuma_H0V1_neon,  VO_avg_MC_InterLuma_H0V2_neon, VO_avg_MC_InterLuma_H0V3_neon},
    { VO_avg_MC_InterLuma_H1V0_neon,  VO_avg_MC_InterLuma_H1V1_neon,  VO_avg_MC_InterLuma_H1V2_neon, VO_avg_MC_InterLuma_H1V3_neon},
    { VO_avg_MC_InterLuma_H2V0_neon,  VO_avg_MC_InterLuma_H2V1_neon,  VO_avg_MC_InterLuma_H2V2_neon, VO_avg_MC_InterLuma_H2V3_neon},
    { VO_avg_MC_InterLuma_H3V0_neon,  VO_avg_MC_InterLuma_H3V1_neon,  VO_avg_MC_InterLuma_H3V2_neon, VO_avg_MC_InterLuma_H3V3_neon}
};

// Chroma Bi
const MC_CHROMA_BI_FUNC arrFun_avg_nornd_MC_Bi_Chroma[2][2]={
    { VO_avg_nornd_MC_InterChroma_H0V0_neon,  VO_avg_nornd_MC_InterChroma_H0V1_neon},
    { VO_avg_nornd_MC_InterChroma_H1V0_neon,  VO_avg_nornd_MC_InterChroma_H1V1_neon}
};
const MC_CHROMA_BI_FUNC arrFun_avg_MC_Bi_Chroma[2][2]={
    { VO_avg_MC_InterChroma_H0V0_neon,  VO_avg_MC_InterChroma_H0V1_neon},
    { VO_avg_MC_InterChroma_H1V0_neon,  VO_avg_MC_InterChroma_H1V1_neon}
};

#endif
static const VO_S32 chroma_ext_left[2] = {0,1};
static const VO_S32 chroma_ext_right[2] = {0,2};
static const VO_S32 chroma_ext[2] = {0,3};

// VO_U32 g_scalingListSize   [4] = {16,64,256,1024};
// VO_U32 g_scalingListNum[SCALING_LIST_SIZE_NUM]={6,6,6,2};

/** initialization process of scaling list array
*/
//VO_S32  *g_aiDequantCoef[SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM][SCALING_LIST_DIR_NUM]; ///< array of dequantization matrix coefficient 4x4
//VO_S32 * g_aiDequantCoef[SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; ///< array of dequantization matrix coefficient 4x4  
//VO_S16 * g_aiDequantCoef[SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM][SCALING_LIST_REM_NUM]; ///< array of dequantization matrix coefficient 4x4  

/*VO_S32 SetQPforQuant( VO_S32 qpy, TextType eTxtType, VO_S32 qpBdOffset, VO_S32 chromaQPOffset)
{
  VO_S32 qpScaled;

  if(eTxtType == TEXT_LUMA)
  {
    qpScaled = qpy + qpBdOffset;
  }
  else
  {
    qpScaled = Clip3( -qpBdOffset, 57, qpy + chromaQPOffset );

    if(qpScaled < 0)
    {
      qpScaled = qpScaled + qpBdOffset;
    }
    else
    {
      qpScaled = g_aucChromaScale[ qpScaled ] + qpBdOffset;
    }
  }
  return qpScaled;
}*/
#if 0
static VO_BOOL DeriveTbAvail( H265_DEC_SLICE * p_slice, 
	//const VO_S32 xCb, 
	//const VO_S32 yCb, 
	//const VO_S32 nCbS,
	//const VO_S32 xPb,
	//const VO_S32 yPb, 
	//const VO_S32 nPbW, 
	//const VO_S32 nPbH, 
	const VO_S32 xNbY, 
	const VO_S32 yNbY, 
	//const VO_S32 partIdx,
	PicMvField **PbMvField )
{

	if ( xNbY < 0 || yNbY < 0 || xNbY > ( VO_S32 ) p_slice->p_sps->pic_width_in_luma_samples || yNbY > ( VO_S32 )p_slice->p_sps->pic_height_in_luma_samples )
		return VO_FALSE;
			
	NPbOffset = ( yNbY >> MinTbLog2SizeY ) * p_slice->PicWidthInMinTbsY + ( xNbY >> MinTbLog2SizeY );
	*PbMvField  = p_slice->p_motion_field + NPbOffset;

	if ( ( *PbMvField )->pred_mode != MODE_INTRA && p_slice->p_pps->constrained_intra_pred_flag)
		return VO_FALSE;
#if 1 //YU_TBD, support tile and slice, use tb_scan_table to replace slice number
	if ( //( xNbY >> MinTbLog2SizeY ) >= p_slice->tile_start_x && ( yNbY >> MinTbLog2SizeY ) >= p_slice->tile_start_y &&
	    p_slice->slice_number[ NPbOffset ] == p_slice->cur_slice_number ) 
	    return VO_TRUE;
#endif

    return VO_FALSE;
}

#endif

/*
static void DumpBlock( VO_U8* pucSrc, VO_U32 uiStride, VO_U32 uiWidth, VO_U32 uiHeight, VO_S8* pcName, VO_S32 iPOC, VO_U32 uiLCU, VO_U32 uiAbsIndex)
{
  FILE* fp;
  VO_U32 uiRow, uiCol;

  fp = fopen(pcName, "a+");
  fprintf(fp, "%d\t%d\t%d\n", iPOC, uiLCU, uiAbsIndex);

  for (uiRow = 0; uiRow < uiHeight; uiRow++)
  {
    for (uiCol = 0; uiCol < uiWidth; uiCol++)
    {
      fprintf(fp, "%d ", pucSrc[uiCol]);
    }
    fprintf(fp, "\n");
    pucSrc += uiStride;
  }

  fclose(fp);
}

static void DumpCoeff( VO_S32* piCoeff, VO_U32 uiWidth, VO_U32 uiHeight, VO_S8* pcName, VO_U32 uiLCU, VO_U32 uiAbsIndex)
{
  FILE* fp;
  VO_U32 uiNum;

  fp = fopen(pcName, "a+");
  fprintf(fp, "%d\t%d\n", uiLCU, uiAbsIndex);

  for (uiNum = 0; uiNum < uiWidth*uiHeight; uiNum++)
  {
    fprintf(fp, "%d ", piCoeff[uiNum]);
    if ((uiNum+1)%uiWidth == 0)
    {
      fprintf(fp, "\n");
    }
  }

  fclose(fp);
}
*/
//---------------------------Intra prediction

#define PIXEL_POS(x, y) ( (p_ref + (x) + pic_stride * (y) ) )
#define PIXEL_SPLAT_X4(x) ((x)*0x01010101U)
#define MAX_TB_SIZE 32
#define BIT_DEPTH 8

static VO_VOID H265_IntraPred_Planar( VO_U8 *p_cur, 
	const VO_U8 *top, 
	const VO_U8 *left,
    VO_S32 stride, 
	VO_S32 log2TrafoSize )
{
    VO_S32 x, y;
    VO_S32 nTbS = ( 1 << log2TrafoSize);
	VO_S32 shift = log2TrafoSize + 1 ;

	//YU_TBD: clean up getting addr
    for ( y = 0; y < nTbS; y++ ) {
        for ( x = 0; x < nTbS; x++ ) {
            *( p_cur++ ) = ( ( nTbS - 1 - x ) * left[ y ]  + ( x + 1 ) * top[ nTbS ] +
                         ( nTbS - 1 - y ) * top[ x ] + ( y + 1 ) * left[ nTbS ] + nTbS ) >> shift;
		}
		p_cur -= nTbS;
		p_cur += stride;
	}
}

static VO_VOID  H265_IntraPred_DC( VO_U8 *p_ref, 
	const VO_U8 *top, 
	const VO_U8 *left,
    VO_S32 pic_stride, 
	VO_S32 log2TrafoSize, 
	VO_S32 cIdx )
{
    VO_S32 i, j, x, y;
    VO_S32 nTbS = ( 1 << log2TrafoSize );
    VO_S32 dc = nTbS;
    VO_U32 a;

    for ( i = 0; i < nTbS; i++ )
        dc += left[i] + top[i];

    dc >>= log2TrafoSize + 1;

    a = PIXEL_SPLAT_X4( dc );

    for ( i = 0; i < nTbS; i++ )
        for ( j = 0; j < nTbS / 4; j++ )
			*((VO_U32*)PIXEL_POS(j * 4, i)) = a;

    if ( cIdx == 0 && nTbS < 32 ) {
        *( PIXEL_POS( 0, 0 ) ) = (left[0] + 2 * dc  + top[0] + 2) >> 2;
        for (x = 1; x < nTbS; x++)
            *( PIXEL_POS( x, 0 ) ) = (top[x] + 3 * dc + 2) >> 2;
        for (y = 1; y < nTbS; y++)
            *( PIXEL_POS( 0, y ) ) = (left[y] + 3 * dc + 2) >> 2;
    }
}


//YU_TBD: need to review code
static VO_VOID  H265_IntraPred_Angular( VO_U8 *p_cur, 
	VO_U8 *top, 
	VO_U8 *left,
    VO_S32 dst_stride, 
	VO_S32 nTbS, 
	VO_S32 cIdx, 
	VO_S32 predModeIntra )
{
    VO_S32 x, y;
    VO_S32 angle = intra_pred_angle[ predModeIntra - 2 ];
    VO_U8 ref_array[ 3 * 32 + 1 ];
    VO_U8 *p_ref;
    VO_S32 last = (nTbS * angle) >> 5;
    VO_S32 inv_angle_mode_value;

    if ( predModeIntra >= 18 ) {
        p_ref = top - 1;

        if (/*angle < 0 &&*/ last < -1 ) {
            p_ref = ref_array + nTbS;
            inv_angle_mode_value = inv_angle[ predModeIntra - 11 ];

            for (x = last; x <= -1; x++)
                p_ref[x] = left[-1 + ((x * inv_angle_mode_value + 128) >> 8)]; 
            for (x = 0; x <= nTbS; x++)
                p_ref[x] = top[x - 1];
        }

        for ( y = 0; y < nTbS; y++ ) {
            VO_S32 idx = ((y + 1) * angle) >> 5;
            VO_S32 fact = ((y + 1) * angle) & 31;

            if (fact) {
                for (x = 0; x < nTbS; x++) {
                    p_cur[x + dst_stride * y] = ((32 - fact) * p_ref[x + idx + 1] + fact * p_ref[x + idx + 2] + 16) >> 5;
                }
            } else {
                for (x = 0; x < nTbS; x++) {
                    p_cur[x + dst_stride * y] = p_ref[x + idx + 1];
                }
            }
        }
        if ( predModeIntra == 26 && cIdx == 0 && nTbS < 32 ) {
            for (y = 0; y < nTbS; y++)
                p_cur[dst_stride * y] = Clip(top[0] + ((left[y] - left[-1]) >> 1));
        }
    } else {
        p_ref = left - 1;
        if (/*angle < 0 &&*/ last < -1) {
            p_ref = ref_array + nTbS;
            inv_angle_mode_value = inv_angle[predModeIntra-11];
            for (x = last; x <= -1; x++)
                p_ref[x] = top[-1 + ((x * inv_angle_mode_value  + 128) >> 8)];
            for (x = 0; x <= nTbS; x++)
                p_ref[x] = left[x - 1];

        }

        for ( x = 0; x < nTbS; x++ ) {
            VO_S32 idx = ((x + 1) * angle) >> 5;
            VO_S32 fact = ((x + 1) * angle) & 31;

            if ( fact ) {
                for ( y = 0; y < nTbS; y++ ) {
                    p_cur[ x + dst_stride * y ] = ( ( 32 - fact ) * p_ref[ y + idx + 1 ] + fact * p_ref[ y + idx + 2 ] + 16 ) >> 5;
                }
	        } else {
                for ( y = 0; y < nTbS; y++ ) {
                    p_cur[x + dst_stride * y] = p_ref[y + idx + 1];
                }
            }
        }
        if ( predModeIntra == 10 && cIdx == 0 && nTbS < 32 ) {
            for (x = 0; x < nTbS; x++)
                p_cur[x] = Clip(left[0] + ((top[x] - top[-1]) >> 1));
        }
    }
}



//---8.4.4.2.1	General intra sample prediction
VO_VOID H265_IntraPrediction( H265_DEC_SLICE *p_slice,
	VO_S32 xTbCmp, 
	VO_S32 yTbCmp, 
	VO_S32 log2TrafoSize, 
	VO_S32 predModeIntra,
	VO_S32 cIdx,
    VO_U32 available_flag)
{
#if 1

    VO_S32 i;
    VO_S32 shift = cIdx ? 1: 0; 
	VO_S32 xTbY = xTbCmp >> shift;
	VO_S32 yTbY = yTbCmp >> shift; 

    VO_S32 nTbS  = ( 1 << log2TrafoSize );
    VO_S32 nTbSY = nTbS << shift;

    VO_S32 pic_stride = p_slice->cur_pic->pic_stride[ cIdx ];
    VO_U8 *p_ref =  p_slice->cur_pic->pic_buf[ cIdx ] + xTbY + yTbY * pic_stride ;
	                    
#if defined(VOARMV7) && USE_INLINE_ASM
    VO_U8 left_array[2*MAX_TB_SIZE+9], filtered_left_array[2*MAX_TB_SIZE+16];
    VO_U8 top_array[2*MAX_TB_SIZE+9], filtered_top_array[2*MAX_TB_SIZE+16];
    VO_U8 *left = left_array + 8;
    VO_U8 *top = top_array + 8;
#else
    VO_U8 left_array[2*MAX_TB_SIZE+1], filtered_left_array[2*MAX_TB_SIZE+1];
    VO_U8 top_array[2*MAX_TB_SIZE+1], filtered_top_array[2*MAX_TB_SIZE+1];
    VO_U8 *left = left_array + 1;
    VO_U8 *top = top_array + 1;
#endif	
    
    VO_U8 *filtered_left = filtered_left_array + 1;
    VO_U8 *filtered_top = filtered_top_array + 1;

    
#if INTRA_PRED_LOG
  VO_U32 tmp_bl = available_flag & 0x1;
  VO_U32 tmp_l = available_flag & 0x2;
  VO_U32 tmp_tl = available_flag & 0x4;;
  VO_U32 tmp_t = available_flag & 0x8;
  VO_U32 tmp_tr = available_flag & 0x10;;
#endif
	//YU_TBD: should be tile boundary??
    VO_S32 bottom_left_size = ( MIN( yTbCmp + 2 * nTbSY, p_slice->p_sps->pic_height_in_luma_samples ) -
                              ( yTbCmp + nTbSY ) ) >> shift;
    VO_S32 top_right_size   = ( MIN( xTbCmp + 2 * nTbSY, p_slice->p_sps->pic_width_in_luma_samples ) -
                              ( xTbCmp + nTbSY ) ) >> shift;

	//Harry add
	//m_uiMaxCUHeight eqs to 1<<log2_luma_coding_block_size
// 	VO_U32 maxCUHeight = p_slice->p_sps->log2_luma_coding_block_size;
	VO_U32 bNeedXChange = (!p_slice->p_pps->pps_deblocking_filter_disabled_flag) &&
		yTbCmp > 0 && (!(yTbCmp<<(32 - p_slice->p_sps->log2_luma_coding_block_size))) && ( p_slice->fast_mode & VO_FM_DEBLOCK ) == 0;


	//----- Fill left and top with the available samples
    if ( available_flag & 0x2 ) {
        for ( i = 0; i < nTbS; i++ )
            left[ i ] = *( PIXEL_POS( -1, i ) );//YU_TBD:refine get source ans store p_cur
    }
    if ( available_flag & 0x1 ) {
        for ( i = 0; i < bottom_left_size; i++ ) {
            left[ nTbS + i ] = *( PIXEL_POS( -1, nTbS + i ) );//YU_TBD: refine get source ans store p_cur
		}
        for ( ; i < nTbS; i++ ) { //out_size picture
            left[ nTbS + i ] = *( PIXEL_POS( -1, nTbS + bottom_left_size - 1 ));//YU_TBD:refine get source ans store p_cur
        }
    }   

	{
		const VO_U8 * src_top;
		if (bNeedXChange) {
			VO_U32 row = ((VO_U32)yTbCmp)>>(p_slice->p_sps->log2_luma_coding_block_size);
			src_top = cIdx ? (cIdx == 1 ? (p_slice->m_puBufForIntraU[!(row & 1)] + xTbY) :
				(p_slice->m_puBufForIntraV[!(row & 1)] + xTbY)) : (p_slice->m_puBufForIntraY[!(row & 1)] + xTbY) ;
		}else{
			src_top = PIXEL_POS( 0, -1 );
		}

		if ( available_flag & 0x4 )
			left[ -1 ] = src_top[-1];

		//YU_TBD: try own copy
		if ( available_flag & 0x8 )
			CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, top, src_top, nTbS * sizeof( VO_U8 ) );

		if ( available_flag & 0x10 ) {
            VO_U8 lastAvailPosValue;
            VO_U8 *pTBuf;
			CopyMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, &top[ nTbS ], src_top + nTbS, top_right_size * sizeof( VO_U8 ) );
            lastAvailPosValue = src_top[ nTbS + top_right_size - 1];
            pTBuf = top + nTbS;
			for ( i = top_right_size; i < nTbS; i++ ) {//top_right_size??
				//top[ nTbS + i ] =  lastAvailPosValue;
                pTBuf[ i ] =  lastAvailPosValue;
            }
            /*memset(pTBuf, lastAvailPosValue,nTbS-top_right_size);*/
		}
		
	}
    //--- 8.4.4.2.2	Reference sample substitution process for intra sample prediction
    if ( !(available_flag & 0x1) ) {
        if ( (available_flag & 0x2) ) {
            //VO_EXTEND_DOWN( &left[ nTbS - 1 ], nTbS );
            //memset(left+nTbS, left[nTbS-1],nTbS);
#if defined(VOARMV7) && USE_INLINE_ASM
            VO_S32 width = nTbS;
            VO_U8 * pBuf = left+nTbS;
            __asm__ volatile
                (
                "   vdup.i8  d0, %2               \n\t"
                "   cmp  %1, #4                   \n\t"
                "   bne  1f                      \n\t"
                "   vst1.32 {d0[0]},[%0]           \n\t"
                "   b    2f                      \n\t"
                "1: subs      %1, %1, #8           \n\t"
                "   vstmia %0!, {d0}            \n\t"
                "   bgt        1b                  \n\t"
                "2:                     \n\t"
                :"+r"(pBuf), "+r"(width)
                : "r"(left[nTbS-1])
                :"memory", "d0"
                );
            
 
#else
            SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, left+nTbS,left[nTbS-1], nTbS); //width*width
#endif
            
        } else if ( (available_flag & 0x4) ) {
            //VO_EXTEND_DOWN( &left[ -1 ], 2 * nTbS );
            //memset(left, left[ -1 ],2 * nTbS);
#if defined(VOARMV7) && USE_INLINE_ASM
             VO_S32 width = nTbS<<1;
             VO_U8 * pBuf = left;
            __asm__ volatile
                (
                "   vdup.i8  d0, %2               \n\t"
                "1: subs      %1, %1, #8           \n\t"
                "   vstmia %0!, {d0}            \n\t"
                "   bgt        1b                  \n\t"
                :"+r"(pBuf), "+r"(width)
            : "r"(left[-1])
                :"memory", "d0"
                );

#else
            SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, left,left[-1], nTbS<<1); //width*width
#endif
            available_flag |= 0x2;
            //bl_avail = 1;//
        } else if ( available_flag & 0x8 ) {
            left[ -1 ] = top[ 0 ];
            //VO_EXTEND_DOWN( &left[ -1 ], 2 * nTbS );
            //memset(left-1, top[ 0 ],2 * nTbS+1);
#if defined(VOARMV7) && USE_INLINE_ASM
            VO_S32 width = nTbS<<1;
            VO_U8 * pBuf = left;
            __asm__ volatile
                (
                "   vdup.i8  d0, %2               \n\t"
                "1: subs      %1, %1, #8           \n\t"
                "   vstmia %0!, {d0}            \n\t"
                "   bgt        1b                  \n\t"
                :"+r"(pBuf), "+r"(width)
            : "r"(top[ 0 ])
                :"memory", "d0"
                );

#else
            SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, left,top[ 0 ], nTbS<<1); //width*width
#endif

            available_flag |= 0x6;
            //bl_avail = 1;//
        } else if ( available_flag & 0x10 ) {
            //VO_EXTEND_LEFT( &top[ nTbS ], nTbS );
            //memset(top, top[ nTbS ],nTbS);  
#if defined(VOARMV7) && USE_INLINE_ASM
            VO_S32 width = nTbS;
            VO_U8 * pBuf = top;
            __asm__ volatile
                (
                "   vdup.i8  d0, %2               \n\t"
                "   cmp  %1, #4                   \n\t"
                "   bne  1f                      \n\t"
                "   vst1.32 {d0[0]},[%0]           \n\t"
                "   b    2f                      \n\t"
                "1: subs      %1, %1, #8           \n\t"
                "   vstmia %0!, {d0}            \n\t"
                "   bgt        1b                  \n\t"
                "2:                                 \n\t"
                :"+r"(pBuf), "+r"(width)
            : "r"(top[ nTbS ])
                :"memory", "d0"
                );

#else
            SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, top,top[nTbS], nTbS); //width*width
#endif
            left[ -1 ] = top[ 0 ];
            //VO_EXTEND_DOWN( &left[ -1 ], 2 * nTbS);
            //memset(left-1, top[ 0 ],2 * nTbS+1);
#if defined(VOARMV7) && USE_INLINE_ASM
            width = nTbS<<1;
            pBuf = left;
            __asm__ volatile
                (
                "   vdup.i8  d0, %2               \n\t"
                "1: subs      %1, %1, #8           \n\t"
                "   vstmia %0!, {d0}            \n\t"
                "   bgt        1b                  \n\t"
                :"+r"(pBuf), "+r"(width)
            : "r"(top[ 0 ])
                :"memory", "d0"
                );

#else
            SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, left,top[ 0 ], nTbS<<1); //width*width
#endif
//             t_avail      = 1;
//             tl_avail = 1;
//             l_avail     = 1;
            available_flag |= 0xE;
        } else { // No samples available
            //top[0] = left[ -1 ] = ( 1 << ( BIT_DEPTH -  1 ) );
            //VO_EXTEND_RIGHT( &top[ 0 ], 2  * nTbS - 1 );
            //memset(top, ( 1 << ( BIT_DEPTH -  1 ) ), 2 * nTbS);
#if defined(VOARMV7) && USE_INLINE_ASM
            VO_S32 width = nTbS<<1;
            VO_U8 * pBuf = top;
            __asm__ volatile
                (
                "   vdup.i8  d0, %2               \n\t"
                "1: subs      %1, %1, #8           \n\t"
                "   vstmia %0!, {d0}            \n\t"
                "   bgt        1b                  \n\t"
                :"+r"(pBuf), "+r"(width)
            :"r"(128)
                :"memory", "d0"
                );

#else
            SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, top,128, nTbS<<1); //width*width
#endif
            left[ -1 ] = ( 1 << ( BIT_DEPTH -  1 ) );
            //VO_EXTEND_DOWN( &left[ -1 ], 2 * nTbS );
            //memset(left-1, ( 1 << ( BIT_DEPTH -  1 ) ),2 * nTbS+1);
#if defined(VOARMV7) && USE_INLINE_ASM
            width = nTbS<<1;
            pBuf = left;            
            __asm__ volatile
                (
                "   vdup.i8  d0, %2               \n\t"
                "1: subs      %1, %1, #8           \n\t"
                "   vstmia %0!, {d0}            \n\t"
                "   bgt        1b                  \n\t"
                :"+r"(pBuf), "+r"(width)
            :"r"(128)
                :"memory", "d0"
                );

#else
            SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, left,128, nTbS<<1); //width*width
#endif
        }
    }
    if ( !(available_flag & 0x2) ) {
        //VO_EXTEND_UP( &left[ nTbS ], nTbS );
         //memset(left, left[ nTbS ],nTbS);
#if defined(VOARMV7) && USE_INLINE_ASM
        VO_S32 width = nTbS;
        VO_U8 * pBuf = left;
        __asm__ volatile
            (
            "   vdup.i8  d0, %2               \n\t"
            "   cmp  %1, #4                   \n\t"
            "   bne  1f                      \n\t"
            "   vst1.32 {d0[0]},[%0]           \n\t"
            "   b    2f                      \n\t"
            "1: subs      %1, %1, #8           \n\t"
            "   vstmia %0!, {d0}            \n\t"
            "   bgt        1b                  \n\t"
            "2:                                 \n\t"
            :"+r"(pBuf), "+r"(width)
            :"r"(left[ nTbS ])
            :"memory", "d0"
            );

#else
        SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, left,left[ nTbS ], nTbS); //width*width
#endif
        //l_avail = 1;//
    }

    if ( !(available_flag & 0x4) ) {
        left[ -1 ] = left[ 0 ];
        //tl_avail = 1;//
    }

    if ( !(available_flag & 0x8) ) {
        //top[ 0 ] = left[ -1 ];
        //VO_EXTEND_RIGHT( &top[ 0 ], nTbS - 1 );
        //memset(top, left[ -1 ], nTbS);
#if defined(VOARMV7) && USE_INLINE_ASM
        VO_S32 width = nTbS;
        VO_U8 * pBuf = top;
        __asm__ volatile
            (
            "   vdup.i8  d0, %2               \n\t"
            "   cmp  %1, #4                   \n\t"
            "   bne  1f                      \n\t"
            "   vst1.32 {d0[0]},[%0]           \n\t"
            "   b    2f                      \n\t"
            "1: subs      %1, %1, #8           \n\t"
            "   vstmia %0!, {d0}            \n\t"
            "   bgt        1b                  \n\t"
            "2:                             \n\t"
            :"+r"(pBuf), "+r"(width)
            :"r"(left[ -1 ])
            :"memory", "d0"
            );

#else
        SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, top,left[ -1 ], nTbS); //width*width
#endif
        //t_avail = 1;//
    }

    if ( !(available_flag & 0x10) ){
        //VO_EXTEND_RIGHT( &top[ nTbS - 1 ], nTbS );
        //memset(top+nTbS, top[nTbS-1], nTbS);
#if defined(VOARMV7) && USE_INLINE_ASM
        VO_S32 width = nTbS;
        VO_U8 * pBuf = top+nTbS;
        __asm__ volatile
            (
            "   vdup.i8  d0, %2               \n\t"
            "   cmp  %1, #4                   \n\t"
            "   bne  1f                      \n\t"
            "   vst1.32 {d0[0]},[%0]           \n\t"
            "   b    2f                      \n\t"
            "1: subs      %1, %1, #8           \n\t"
            "   vstmia %0!, {d0}            \n\t"
            "   bgt        1b                  \n\t"
            "2:                                \n\t"
            :"+r"(pBuf), "+r"(width)
            : "r"(top[ nTbS-1 ])
            :"memory", "d0"
            );

#else
        SetMem(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, top+nTbS,top[ nTbS-1 ], nTbS); //width*width
#endif

    }

    top[ -1 ] = left[ -1 ];



#if INTRA_PRED_LOG
    printIntraPredictionInput(predModeIntra,cIdx,top,left,nTbS,tmp_bl,tmp_l,tmp_tl,tmp_t,tmp_tr);
#endif

    //--- 8.4.4.2.3	Filtering process of neighbouring samples
    if ( cIdx == 0 && predModeIntra != INTRA_DC && nTbS != 4 ) {//YU_TBD: check cIdx==0???
        static const VO_S32 intraHorVerDistThres[ 3 ] = { 7, 1, 0 };
        VO_S32 minDistVerHor = MIN( ABS( ( VO_S32 ) predModeIntra - 26 ), ABS( ( VO_S32 )predModeIntra - 10 ) );

        if ( minDistVerHor > intraHorVerDistThres[ log2TrafoSize - 3 ] ) {//YU_TBD: check log2TrafoSize
            VO_S32 thresold = 1 << ( BIT_DEPTH - 5 );

            if ( p_slice->p_sps->sps_strong_intra_smoothing_enable_flag && log2TrafoSize == 5 &&
                ABS( top[ -1 ] + top[ 63 ] - 2 * top[ 31 ]) < thresold &&
                ABS( left[ -1 ] + left[ 63 ] - 2 * left[ 31 ]) < thresold ) {
                // We can't just overwrite values in top because it could be a pointer into src
                filtered_top[-1] = top[ -1 ];
                filtered_top[63] = top[ 63 ];

                for ( i = 0; i < 63; i++ ) {
                    filtered_top[i] = ((64 - (i + 1))*top[-1] + (i + 1) * top[63] + 32) >> 6;
                }
                for (i = 0; i < 63; i++) {
                    left[i] = ((64 - (i + 1))*left[-1] + (i + 1) * left[63] + 32) >> 6;
                }
                top = filtered_top;
            } else {
#if  defined(VOARMV7) && USE_INLINE_ASM
               VO_U8 * volatile left_tmp = &left[-1];
               VO_U8 * volatile top_tmp = &top[-1];			   
			   VO_U8 * volatile filtered_left_tmp = filtered_left;
			   VO_U8 * volatile filtered_top_tmp =	filtered_top;

			   switch(log2TrafoSize)
			   	{
			   	    case 2:
						 __asm__ volatile
						   (
							  "    vld1.8			 d0, [%1]					\n\t"
							  " 	vext.8			 d1, d0, d0, #1 		  \n\t"
							  " 	vext.8			 d2, d0, d0, #2 		  \n\t"
							  " 	vhadd.u8	   d3, d0, d2				  \n\t"
							  " 	vrhadd.u8	   d3, d3, d1				  \n\t"
							  " 	 vst1.8 		 d3, [%0]					\n\t"
							  :
							  :"r"(filtered_left_tmp), "r"(left_tmp)
						:"memory", "q0", "q1"
							);
						   __asm__ volatile
						   (
							  "    vld1.8			 d0, [%1]					\n\t"
							  " 	vext.8			 d1, d0, d0, #1 		  \n\t"
							  " 	vext.8			 d2, d0, d0, #2 		  \n\t"
							  " 	vhadd.u8	   d3, d0, d2				  \n\t"
							  " 	vrhadd.u8	   d3, d3, d1				  \n\t"
							  " 	vst1.8			  d3, [%0]					 \n\t"
							  :
							  :"r"(filtered_top_tmp), "r"(top_tmp)
						:"memory", "q0", "q1"
							);						  
						
						break;
					case 3:
							__asm__ volatile
							  (
								 "vld1.8	 {q0, q1}, [%1]     		  \n\t"
								 "vext.8	 q2, q0, q1, #1 	     	  \n\t"
								 "vext.8	 q3, q0, q1, #2 		      \n\t"
								 "vhadd.u8   q3, q0, q3			     	  \n\t"
								 "vrhadd.u8  q3, q3, q2				      \n\t"
								 "vst1.8	 {q3}, [%0]			          \n\t"
								 :
								 :"r"(filtered_left_tmp), "r"(left_tmp)
								 :"memory", "q0", "q1", "q2", "q3"
							  );
							 __asm__ volatile
							   (
								  "vld1.8	  {q0, q1}, [%1]			   \n\t"
								  "vext.8	  q2, q0, q1, #1			   \n\t"
								  "vext.8	  q3, q0, q1, #2			   \n\t"
								  "vhadd.u8   q3, q0, q3				   \n\t"
								  "vrhadd.u8  q3, q3, q2				   \n\t"
								  "vst1.8	  {q3}, [%0]				   \n\t"
								  :
								  :"r"(filtered_top_tmp), "r"(top_tmp)
								  :"memory", "q0", "q1", "q2", "q3"
							   );
						
						break;
					case 4:
						     __asm__ volatile
							  (
								 "vld1.8	 {q0, q1}, [%1]!     		  \n\t"
								 "vld1.8	 {q2},     [%1]     		  \n\t"
								 "vext.8	 q3, q0, q1, #1 	     	  \n\t"
								 "vext.8	 q4, q0, q1, #2 		      \n\t"
								 "vext.8	 q5, q1, q2, #1 	     	  \n\t"
								 "vext.8	 q6, q1, q2, #2 		      \n\t"
								 "vhadd.u8   q4, q0, q4			     	  \n\t"
								 "vrhadd.u8  q4, q4, q3				      \n\t"
								 "vhadd.u8   q6, q1, q6			     	  \n\t"
								 "vrhadd.u8  q5, q6, q5				      \n\t"
								 "vst1.8	 {q4, q5}, [%0]			      \n\t"
								 :
								 :"r"(filtered_left_tmp), "r"(left_tmp)
								 :"memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6"
							  );

							 __asm__ volatile
							  (
								 "vld1.8	 {q0, q1}, [%1]!     		  \n\t"
								 "vld1.8	 {q2},     [%1]     		  \n\t"
								 "vext.8	 q3, q0, q1, #1 	     	  \n\t"
								 "vext.8	 q4, q0, q1, #2 		      \n\t"
								 "vext.8	 q5, q1, q2, #1 	     	  \n\t"
								 "vext.8	 q6, q1, q2, #2 		      \n\t"
								 "vhadd.u8   q4, q0, q4			     	  \n\t"
								 "vrhadd.u8  q4, q4, q3				      \n\t"
								 "vhadd.u8   q6, q1, q6			     	  \n\t"
								 "vrhadd.u8  q5, q6, q5				      \n\t"
								 "vst1.8	 {q4, q5}, [%0]			      \n\t"
								 :
								 :"r"(filtered_top_tmp), "r"(top_tmp)
								 :"memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6"
							  );	  	 
						break;
					case 5:
                              __asm__ volatile
							  (
								 "vld1.8	 {q0, q1}, [%1]!     		  \n\t"
								 "vld1.8	 {q2, q3}, [%1]!     		  \n\t"
								 "vld1.8	 {q4},     [%1]     		  \n\t"
								 "vext.8	 q5,  q0, q1, #1 	     	  \n\t"
								 "vext.8	 q6,  q0, q1, #2 		      \n\t"
								 "vext.8	 q7,  q1, q2, #1 	     	  \n\t"
								 "vext.8	 q8,  q1, q2, #2 		      \n\t"
                                 "vext.8	 q9,  q2, q3, #1 	     	  \n\t"
								 "vext.8	 q10, q2, q3, #2 		      \n\t"
								 "vext.8	 q11, q3, q4, #1 	     	  \n\t"
								 "vext.8	 q12, q3, q4, #2 		      \n\t"								 
								 "vhadd.u8   q6, q0, q6			     	  \n\t"
								 "vrhadd.u8  q6, q6, q5				      \n\t"
								 "vhadd.u8   q8, q1, q8			     	  \n\t"
								 "vrhadd.u8  q7, q8, q7				      \n\t"
								 "vhadd.u8   q10, q2, q10		     	  \n\t"
								 "vrhadd.u8  q10, q10, q9			      \n\t"
								 "vhadd.u8   q12, q3, q12		     	  \n\t"
								 "vrhadd.u8  q11, q12, q11			      \n\t"
								 "vst1.8	 {q6, q7}, [%0]!		      \n\t"
								 "vst1.8	 {q10, q11}, [%0] 		      \n\t"
								 :
								 :"r"(filtered_left_tmp), "r"(left_tmp)
								 :"memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11"
							  );

							  __asm__ volatile
							  (
								 "vld1.8	 {q0, q1}, [%1]!     		  \n\t"
								 "vld1.8	 {q2, q3}, [%1]!     		  \n\t"
								 "vld1.8	 {q4},     [%1]     		  \n\t"
								 "vext.8	 q5,  q0, q1, #1 	     	  \n\t"
								 "vext.8	 q6,  q0, q1, #2 		      \n\t"
								 "vext.8	 q7,  q1, q2, #1 	     	  \n\t"
								 "vext.8	 q8,  q1, q2, #2 		      \n\t"
                                 "vext.8	 q9,  q2, q3, #1 	     	  \n\t"
								 "vext.8	 q10, q2, q3, #2 		      \n\t"
								 "vext.8	 q11, q3, q4, #1 	     	  \n\t"
								 "vext.8	 q12, q3, q4, #2 		      \n\t"								 
								 "vhadd.u8   q6, q0, q6			     	  \n\t"
								 "vrhadd.u8  q6, q6, q5				      \n\t"
								 "vhadd.u8   q8, q1, q8			     	  \n\t"
								 "vrhadd.u8  q7, q8, q7				      \n\t"
								 "vhadd.u8   q10, q2, q10		     	  \n\t"
								 "vrhadd.u8  q10, q10, q9			      \n\t"
								 "vhadd.u8   q12, q3, q12		     	  \n\t"
								 "vrhadd.u8  q11, q12, q11			      \n\t"
								 "vst1.8	 {q6, q7}, [%0]!		      \n\t"
								 "vst1.8	 {q10, q11}, [%0] 		      \n\t"
								 :
								 :"r"(filtered_top_tmp), "r"(top_tmp)
								 :"memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11"
							  );
						break;
			   	}

                filtered_left[2*nTbS-1] = left[2*nTbS-1];
                filtered_top[2*nTbS-1]  = top[2*nTbS-1];
                filtered_top[-1] = filtered_left[-1] = (left[0] + 2*left[-1] + top[0] + 2) >> 2;				
#else
                filtered_left[2*nTbS-1] = left[2*nTbS-1];
                filtered_top[2*nTbS-1]  = top[2*nTbS-1];
                for ( i = 2*nTbS-2; i >= 0; i-- ) {
                    filtered_left[i] = (left[i+1] + 2*left[i] + left[i-1] + 2) >> 2;
                }
                filtered_top[-1] = filtered_left[-1] = (left[0] + 2*left[-1] + top[0] + 2) >> 2;
                for (i = 2*nTbS-2; i >= 0; i--) {
                    filtered_top[i] = (top[i+1] + 2*top[i] + top[i-1] + 2) >> 2;
                }
#endif				
                left = filtered_left;
                top = filtered_top;
            }
        }
    }

    switch ( predModeIntra ) {
    case INTRA_PLANAR:
        H265_IntraPred_Planar( p_ref, top, left, pic_stride, log2TrafoSize );
        break;
    case INTRA_DC:
        H265_IntraPred_DC( p_ref, top, left, pic_stride, log2TrafoSize, cIdx );
        break;
    default:
#if INTRA_ASM_ENABLED      
    	if ( predModeIntra >= 18 ) {
    		H265_IntraPred_Angular_mode_GE18_ASM(p_ref,top,left,pic_stride,nTbS,cIdx,predModeIntra);
        } else {
            H265_IntraPred_Angular_mode_LT18_ASM(p_ref,top,left,pic_stride,nTbS,cIdx,predModeIntra);
        }
#else
        
        H265_IntraPred_Angular( p_ref, top, left, pic_stride, nTbS, cIdx, predModeIntra );
#endif
        break;
    }

#endif
#if INTRA_PRED_LOG
    printIntraPredictionOutput(cIdx, p_ref, pic_stride, log2TrafoSize);  
#endif
}

static VO_S32 xCheckIdenticalMotion (  H265_DEC_SLICE *p_slice , 
	PicMvField *motion_field )
{
	VO_S32 iRefIdxL0 = motion_field->m_iRefIdx[ 0 ];
    VO_S32 iRefIdxL1 = motion_field->m_iRefIdx[ 1 ];

    if ( iRefIdxL0 >= 0 && iRefIdxL1 >= 0 ) {
        VO_S32 RefPOCL0 = p_slice->ref_pic_list[ REF_PIC_LIST_0 ][ iRefIdxL0 ]->m_iPOC;
        VO_S32 RefPOCL1 = p_slice->ref_pic_list[ REF_PIC_LIST_1 ][ iRefIdxL1 ]->m_iPOC;

        if ( RefPOCL0 == RefPOCL1 && motion_field->m_acMv[ 0 ] == motion_field->m_acMv[ 1 ] ) {
            return 0; //ui
        }
    }

	//YU_TBD: iRefIdxL0 >= 0 && iRefIdxL1 >= 0 and inter_dir is identical??
    if ( motion_field->inter_dir != 3 ) {
        return motion_field->m_iRefIdx[0] >= 0 ? 0 : 1;
    }
    return -1; //bi prediction
}



#if 0

/*
static VO_VOID ClipMv( H265_DEC_SLICE* p_slice, TComMv* pMv, VO_S32 xPb, VO_S32 yPb )
{
  VO_S32 iMvShift = 2;
  VO_S32 iOffset = 8;
  //VO_U32 uiCUPelX = p_slice->m_uiCUPelX;
  //VO_U32 uiCUPelY = p_slice->m_uiCUPelY;
  //VO_U32 uiAbsIdxInLCU = p_slice->m_uiAbsIdxInLCU;
  VO_S32 iHorMax = ( p_slice->p_sps->pic_width_in_luma_samples + iOffset - xPb - 1 ) << iMvShift; 
  VO_S32 iHorMin = ( -(VO_S32)p_slice->p_sps->max_cu_width - iOffset - xPb + 1 ) << iMvShift;

  VO_S32 iVerMax = ( p_slice->p_sps->pic_height_in_luma_samples + iOffset - yPb - 1 ) << iMvShift;
  VO_S32 iVerMin = ( -(VO_S32)p_slice->p_sps->max_cu_height - iOffset - yPb + 1 ) << iMvShift;

#ifdef CC_INTERNAL_TRACE
  //Log(6, "POC = %d LCU = %d ZOrderIdxInLCU = %d | ", p_slice->m_iPOC, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU);
  //Log(6, "iHorMax = %d iHorMin = %d iVerMax = %d iVerMin = %d m_iHor = %d m_iVer = %d | ",
        iHorMax, iHorMin, iVerMax, iVerMin, pMv->m_iHor, pMv->m_iVer);
#endif

  pMv->m_iHor = (VO_S16)Clip3(iHorMin, iHorMax, pMv->m_iHor);
  pMv->m_iVer = (VO_S16)Clip3(iVerMin, iVerMax, pMv->m_iVer);

#ifdef CC_INTERNAL_TRACE
  //Log(6, "m_iHor = %d m_iVer = %d\n", pMv->m_iHor, pMv->m_iVer);
#endif
}
*/


#if PAD_BUFF_ENABLED
#endif
#endif

static  VO_VOID FillRow1( VO_U8 *p_ref, 
	VO_U8 *fill, 
	VO_S32 left, 
	VO_S32 center,
	VO_S32 right )
{
    memcpy( fill, p_ref, center );
}

static  VO_VOID FillRow7( VO_U8 *p_ref, 
	VO_U8 *fill, 
	VO_S32 left, 
	VO_S32 center, 
	VO_S32 right )
{
   VO_U8 tmp;

   if ( left )
   	  tmp = *p_ref;

   for ( ;left; left-- )
     *fill++ = tmp;

   for ( ;center; center-- )
     *fill++ = *p_ref++;

   if ( right )
   	 tmp = p_ref[ -1 ];

   for ( ;right; right-- )
     *fill++ = tmp;   
}

static VO_VOID FillMagin(VO_U8 *p_ref, //source pic
	VO_S32 ref_stride, //source pic stride
	VO_U8 *fill,       //block buffer
	VO_S32 dst_stride, //block buffer stride
	VO_S32 x_start,    //start x
	VO_S32 y_start,    //start y
    VO_S32 pic_width,  //pic width
	VO_S32 pic_height, //pic height
	VO_S32 blk_w,      //block width
	VO_S32 blk_h )     //block height
{

   VO_VOID ( *fp )(VO_U8*, VO_U8*, VO_S32, VO_S32, VO_S32);
	   
   VO_S32 x_stop, y_stop;
   VO_S32 left, x, right;
   VO_S32 top, y, bottom;

   x_stop = x_start + blk_w;
   y_stop = y_start + blk_h;

   if ( x_start >= 0 && x_stop <= (VO_S32)pic_width )
        fp = FillRow1;
    else
        fp = FillRow7;

   if ( y_stop < 0 )
       y_start = -blk_h;

   if ( x_stop < 0 )
       x_start = -blk_w;

   if ( y_start > pic_height )
       y_start = pic_height;

   if ( x_start > pic_width )
       x_start = pic_width;

   x_stop = x_start + blk_w;
   y_stop = y_start + blk_h;

   if ( x_start > 0 )
       p_ref += x_start;

   if ( y_start > 0 )
       p_ref += y_start * ref_stride;

   left = x_start < 0 ? -x_start : 0;
   right = x_stop > pic_width ? x_stop - pic_width : 0;
   x = blk_w - left - right;

   top = y_start < 0 ? -y_start : 0;
   bottom = y_stop > pic_height ? y_stop - pic_height : 0;
   y = blk_h - top - bottom;

   for (; top; top-- ) {
       fp( p_ref, fill, left, x, right );
	   fill += dst_stride;
   }

   for ( ; y; y-- ) {
       fp( p_ref, fill, left, x, right );
	   p_ref += ref_stride;
	   fill += dst_stride;
   }

   p_ref -= ref_stride;

   for ( ; bottom; bottom-- ) {
      fp( p_ref, fill,  left, x, right );
      fill += dst_stride;
   }
}



#if 1//YU_TBD : support it
// Get weighted prediction table from pps and then do scale using specified bit-depth.
static VO_VOID GetWpScaling( H265_DEC_SLICE *p_slice, VO_S32 iRefIdx0, VO_S32 iRefIdx1, WP_SCALING_PARAM *w0, WP_SCALING_PARAM *w1 )
{
  H265_DEC_PPS* p_pps     = p_slice->p_pps;
  VO_U32        wpBiPred  = p_pps->weighted_bipred_flag;
  VO_BOOL       bBiDir    = (iRefIdx0 >=0 && iRefIdx1 >= 0);
  VO_BOOL       bUniDir   = !bBiDir;
  VO_U32        yuv       = 0;
  wpScalingParam *p_wp0, *p_wp1;

  if ( bUniDir || wpBiPred )
  { // explicit --------------------
    if ( iRefIdx0 >= 0 )
    {
      p_wp0 = p_slice->m_weightPredTable[REF_PIC_LIST_0][iRefIdx0];
    }
    if ( iRefIdx1 >= 0 )
    {
      p_wp1 = p_slice->m_weightPredTable[REF_PIC_LIST_1][iRefIdx1];
    }
  }
  else
  {
    assert(0);
  }

  if ( iRefIdx0 < 0 )
  {
    p_wp0 = NULL;
  }
  if ( iRefIdx1 < 0 )
  {
    p_wp1 = NULL;
  }

  if ( !bBiDir)
  {  // Unidir
    VO_U32 bitDepthMinus8;
    wpScalingParam*   pwp      = (iRefIdx0 >=0) ? p_wp0 : p_wp1;
    WP_SCALING_PARAM* pwpScale = (iRefIdx0 >=0) ? w0 : w1 ;
    assert(pwp!=NULL);
    assert(pwpScale!=NULL);
    for ( yuv=0 ; yuv<3 ; yuv++ )
    {
      bitDepthMinus8       = yuv ? p_slice->p_sps->bit_depth_chroma-8 : p_slice->p_sps->bit_depth_luma-8; 
      pwpScale[yuv].w      = pwp[yuv].weight;                                                    
      pwpScale[yuv].offset = pwp[yuv].offset * (1 << bitDepthMinus8);                                     
      pwpScale[yuv].shift  = pwp[yuv].log2_weight_denom + (IF_INTERNAL_PREC - bitDepthMinus8 - 8);
      pwpScale[yuv].round  = (pwpScale[yuv].shift) ? (1 << (pwpScale[yuv].shift-1)) : (0);  
    }
  }
  else
  { // Bi-Dir case
    VO_U32 bitDepthMinus8;
    for ( yuv=0 ; yuv<3 ; yuv++ )
    {
      bitDepthMinus8 = yuv ? p_slice->p_sps->bit_depth_chroma-8 : p_slice->p_sps->bit_depth_luma-8; 
      w0[yuv].w      = p_wp0[yuv].weight;
      w0[yuv].o      = p_wp0[yuv].offset * (1 << bitDepthMinus8);
      w1[yuv].w      = p_wp1[yuv].weight;
      w1[yuv].o      = p_wp1[yuv].offset * (1 << bitDepthMinus8);
      w0[yuv].offset = w0[yuv].o + w1[yuv].o;
      w0[yuv].shift  = p_wp0[yuv].log2_weight_denom + 1 + (IF_INTERNAL_PREC - bitDepthMinus8 - 8);
      w0[yuv].round  = (w0[yuv].shift) ? (1<<(w0[yuv].shift-1)) : (0);
      w1[yuv].offset = w0[yuv].offset;
      w1[yuv].shift  = w0[yuv].shift;
      w1[yuv].round  = w0[yuv].round;
    }
  }
}

static  VO_VOID xWeightedUnidir(VO_U8 *p_cur, VO_S32 dst_stride, VO_S32 height, VO_S32 width,  VO_S16 *p0, VO_S32 p_stride,  VO_S32 w0,   VO_S32 round, VO_S32 shift, VO_S32 offset)
{
#if USE_INLINE_ASM
    VO_S32 i, j;
    VO_U8  *tmp_dst;
    VO_S16 *tmp_p0;

    __asm__ volatile
    (
        "  vdup.16   q5, %0          \n\t"
        "  vdup.32   q6, %1          \n\t"
        "  vmvn      q6, q6          \n\t"
        "  vdup.32   q7, %2          \n\t"
        :
        :"r"(w0), "r"(shift-1), "r"(offset)
        :"memory", "q5", "q6", "q7"
    );

    if(width >= 8)
    {

        for(j = width; j > 0; j -= 8)
        {
            tmp_dst = p_cur;
            tmp_p0 = p0;

            p_cur += 8;
            p0  += 8;

            //	   printf("before p_cur %p p0 %p\n", p_cur, p0);
            for( i = height; i > 0; i -= 2)
            {
                __asm__ volatile
                (
                    "	vld1.8			{q12}, [%2], %3			   \n\t"
                    "	vld1.8			{q13}, [%2], %3			   \n\t"
                    "    vmull.s16       q0,  d24,  d10             \n\t"
                    "    vmull.s16       q1,  d25,  d10             \n\t"
                    "    vmull.s16       q2,  d26,  d10             \n\t"
                    "    vmull.s16       q3,  d27,  d10             \n\t"
                    "    vrshl.s32       q0,  q0,   q6              \n\t"
                    "    vrshl.s32       q1,  q1,   q6              \n\t"
                    "    vrshl.s32       q2,  q2,   q6              \n\t"
                    "    vrshl.s32       q3,  q3,   q6              \n\t"
                    "    vadd.s32        q0, q7                     \n\t"
                    "    vadd.s32        q1, q7                     \n\t"
                    "    vadd.s32        q2, q7                     \n\t"
                    "    vadd.s32        q3, q7                     \n\t"
                    "    vqmovun.s32     d0, q0              	   \n\t"
                    "    vqmovun.s32     d1, q1              	   \n\t"
                    "    vqmovun.s32     d2, q2              	   \n\t"
                    "    vqmovun.s32     d3, q3              	   \n\t"
                    "    cmp             %4, #8                     \n\t"
                    "    vqmovn.u16      d0, q0                     \n\t"
                    "    vqmovn.u16      d1, q1                     \n\t"
                    "    blt             1f                        \n\t"
                    "	vst1.8			d0, [%0], %1			   \n\t"
                    "	vst1.8			d1, [%0], %1			   \n\t"
                    "    b               2f                        \n\t"
                    "1:                                             \n\t"
                    "	vst1.32			d0[0], [%0], %1			   \n\t"
                    "	vst1.32			d1[0], [%0], %1			   \n\t"
                    "2:                                             \n\t"
                    :
                    :"r"(tmp_dst), "r"(dst_stride), "r"(tmp_p0), "r"(p_stride*2), "r"(j)
                    :"memory", "q0", "q1", "q2", "q3", "q5", "q6", "q7", "q12", "q13"
                );
            }
        }


        //	   printf("after %p p0 %p\n", p_cur, p0);
    }
    else
    {
        for(j = width; j > 0; j -= 4)
        {
            tmp_dst = p_cur;
            tmp_p0 = p0;

            p_cur += 4;
            p0  += 4;

            //	   printf("before p_cur %p p0 %p\n", p_cur, p0);
            for( i = height; i > 0; i -= 2)
            {
                __asm__ volatile
                (
                    "	vld1.8			{d24}, [%2], %3			   \n\t"
                    "	vld1.8			{d25}, [%2], %3			   \n\t"
                    "   vmull.s16       q0,  d24,  d10             \n\t"
                    "   vmull.s16       q1,  d25,  d10             \n\t"
                    "   vrshl.s32       q0,  q0,   q6              \n\t"
                    "   vrshl.s32       q1,  q1,   q6              \n\t"
                    "   vadd.s32        q0, q7                     \n\t"
                    "   vadd.s32        q1, q7                     \n\t"
                    "   vqmovun.s32     d0, q0              	   \n\t"
                    "   vqmovun.s32     d1, q1              	   \n\t"
                    "   cmp             %4, #4                     \n\t"
                    "   vqmovn.u16      d0, q0                     \n\t"
                    "   blt             1f                        \n\t"
                    "	vst1.32			d0[0], [%0], %1			   \n\t"
                    "	vst1.32			d0[1], [%0], %1			   \n\t"
                    "   b               2f                        \n\t"
                    "1:                                             \n\t"
                    "	vst1.16			d0[0], [%0], %1			   \n\t"
                    "	vst1.16			d0[2], [%0], %1			   \n\t"
                    "2:                                             \n\t"
                    :
                    :"r"(tmp_dst), "r"(dst_stride), "r"(tmp_p0), "r"(p_stride*2), "r"(j)
                    :"memory", "q0", "q1", "q5", "q6", "q7", "q12"
                );
            }
        }    
    }
	
#else
    VO_S32 i, j;
    VO_U8  *tmp_dst;
    VO_S16 *tmp_p0;
    //VO_S32 *tmp_p1;
    for( i = 0; i < height; i++)
    {
        tmp_dst = p_cur;
        tmp_p0 = p0;
        for( j = 0; j < width; j++)
        {
            *tmp_dst++ = (VO_U8)Clip(( ( w0 * (*tmp_p0++)  + round ) >> shift) + offset ) ;
        }

        p_cur += dst_stride;
        p0 += p_stride;
    }
#endif
}



static VO_VOID xWeightedBidir(VO_U8 *p_cur, VO_S32 dst_stride, VO_S32 height, VO_S32 width, VO_S16 *p0, VO_S16 *p1, VO_S32 p_stride,  VO_S32 w0,  VO_S32 w1,  VO_S32 round, VO_S32 shift, VO_S32 offset)
{
#if USE_INLINE_ASM
		VO_S32 i, j;
		VO_U8  *tmp_dst;
		VO_S16 *tmp_p0;
		VO_S16 *tmp_p1;
	
		__asm__ volatile
		(
			"  vdup.16	 q4, %0 		 \n\t"
			"  vdup.16	 q5, %1 		 \n\t"
			"  vdup.32	 q6, %2 		 \n\t"
			"  vmvn 	 q6, q6 		 \n\t"
			"  vdup.32	 q7, %3 		 \n\t"
			:
			:"r"(w0), "r"(w1), "r"(shift-1), "r"((offset << (shift - 1)))
			:"memory", "q5", "q6", "q7"
		);
	
		if(width >= 8)
		{
	
			for(j = width; j > 0; j -= 8)
			{
				tmp_dst = p_cur;
				tmp_p0 = p0;
				tmp_p1 = p1;
	
				p_cur += 8;
				p0	+= 8;
				p1  += 8;
	
				//	   printf("before p_cur %p p0 %p\n", p_cur, p0);
				for( i = height; i > 0; i -= 2)
				{
					__asm__ volatile
					(
						"	vld1.8			{q12}, [%2], %3 		   \n\t"
						"	vld1.8			{q13}, [%2], %3 		   \n\t"
						"	vld1.8			{q14}, [%4], %3 		   \n\t"
						"	vld1.8			{q15}, [%4], %3 		   \n\t"
						"	 vmull.s16		 q0,  d24,	d8  			\n\t"
						"	 vmull.s16		 q1,  d25,	d8  			\n\t"
						"	 vmull.s16		 q2,  d26,	d8  			\n\t"
						"	 vmull.s16		 q3,  d27,	d8  			\n\t"
						"	 vmlal.s16		 q0,  d28,	d10 			\n\t"
						"	 vmlal.s16		 q1,  d29,	d10 			\n\t"
						"	 vmlal.s16		 q2,  d30,	d10 			\n\t"
						"	 vmlal.s16		 q3,  d31,	d10 			\n\t"						
						"	 vadd.s32		 q0,  q7 					\n\t"
						"	 vadd.s32		 q1,  q7 					\n\t"
						"	 vadd.s32		 q2,  q7 					\n\t"
						"	 vadd.s32		 q3,  q7 					\n\t"
						"	 vrshl.s32		 q0,  q0,	q6				\n\t"
						"	 vrshl.s32		 q1,  q1,	q6				\n\t"
						"	 vrshl.s32		 q2,  q2,	q6				\n\t"
						"	 vrshl.s32		 q3,  q3,	q6				\n\t"
						"	 vqmovun.s32	 d0, q0 				   \n\t"
						"	 vqmovun.s32	 d1, q1 				   \n\t"
						"	 vqmovun.s32	 d2, q2 				   \n\t"
						"	 vqmovun.s32	 d3, q3 				   \n\t"
						"	 cmp			 %5, #8 					\n\t"
						"	 vqmovn.u16 	 d0, q0 					\n\t"
						"	 vqmovn.u16 	 d1, q1 					\n\t"
						"	 blt			 1f 					   \n\t"
						"	vst1.8			d0, [%0], %1			   \n\t"
						"	vst1.8			d1, [%0], %1			   \n\t"
						"	 b				 2f 					   \n\t"
						"1: 											\n\t"
						"	vst1.32 		d0[0], [%0], %1 		   \n\t"
						"	vst1.32 		d1[0], [%0], %1 		   \n\t"
						"2: 											\n\t"
						:
						:"r"(tmp_dst), "r"(dst_stride), "r"(tmp_p0), "r"(p_stride*2), "r"(tmp_p1), "r"(j)
						:"memory", "q0", "q1", "q2", "q3", "q5", "q6", "q7", "q12", "q13"
					);
				}
			}
	
	
			//	   printf("after %p p0 %p\n", p_cur, p0);
		}
		else
		{
			for(j = width; j > 0; j -= 4)
			{
				tmp_dst = p_cur;
				tmp_p0 = p0;
				tmp_p1 = p1;
	
				p_cur += 4;
				p0	+= 4;
				p1  += 4;
	
				//	   printf("before p_cur %p p0 %p\n", p_cur, p0);
				for( i = height; i > 0; i -= 2)
				{
					__asm__ volatile
					(
						"	vld1.8			{d24}, [%2], %3 		   \n\t"
						"	vld1.8			{d25}, [%2], %3 		   \n\t"
						"	vld1.8			{d28}, [%4], %3 		   \n\t"
						"	vld1.8			{d29}, [%4], %3 		   \n\t"
						"	 vmull.s16		 q0,  d24,	d8  			\n\t"
						"	 vmull.s16		 q1,  d25,	d8  			\n\t"
						"	 vmlal.s16		 q0,  d28,	d10 			\n\t"
						"	 vmlal.s16		 q1,  d29,	d10 			\n\t"
						"	 vadd.s32		 q0,  q7 					\n\t"
						"	 vadd.s32		 q1,  q7 					\n\t"
						"	 vrshl.s32		 q0,  q0,	q6				\n\t"
						"	 vrshl.s32		 q1,  q1,	q6				\n\t"
						"	 vqmovun.s32	 d0, q0 				   \n\t"
						"	 vqmovun.s32	 d1, q1 				   \n\t"
						"	 cmp			 %5, #4 					\n\t"
						"	 vqmovn.u16 	 d0, q0 					\n\t"
						"	 blt			 1f 					   \n\t"
						"	vst1.32			d0[0], [%0], %1			   \n\t"
						"	vst1.32			d0[1], [%0], %1			   \n\t"
						"	 b				 2f 					   \n\t"
						"1: 											\n\t"
						"	vst1.16 		d0[0], [%0], %1 		   \n\t"
						"	vst1.16 		d0[2], [%0], %1 		   \n\t"
						"2: 											\n\t"
						:
						:"r"(tmp_dst), "r"(dst_stride), "r"(tmp_p0), "r"(p_stride*2), "r"(tmp_p1), "r"(j)
						:"memory", "q0", "q1", "q2", "q3", "q5", "q6", "q7", "q12", "q13"
					);
				}
			}

		}
		
#else

    VO_S32 i, j;
    VO_U8  *tmp_dst;
    VO_S16 *tmp_p0;
    VO_S16 *tmp_p1;
    for( i = 0; i < height; i++)
    {
        tmp_dst = p_cur;
        tmp_p0 = p0;
        tmp_p1 = p1;
        for( j = 0; j < width; j++)
        {
            *tmp_dst++ = (VO_U8)Clip( ( w0 * (*tmp_p0++)  + w1 * (*tmp_p1++)  + round + (offset << (shift - 1))) >> shift ) ;
        }

        p_cur += dst_stride;
        p0 += p_stride;
        p1 += p_stride;
    }
#endif	
}


// luma weighted prediction
static VOINLINE VO_VOID xWeightedPredInterLumaBlk(H265_DEC_SLICE *p_slice, TComPic *refPic, TComMv *mv, VO_S32 xPb, VO_S32 yPb, VO_S32 width, VO_S32 height, WP_SCALING_PARAM *wp, TComPic *dstPic )
{

    VO_S32 ref_stride = refPic->pic_stride[0];
    VO_S32 xy_off    = yPb * ref_stride + xPb;
    VO_S32 refOffset = ( mv->m_iHor >> 2 ) + ( mv->m_iVer >> 2 ) * ref_stride;
    //  VO_U8 *p_ref       = GetLumaAddr(refPic, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr) + refOffset;
    VO_U8 *p_ref  = refPic->pic_buf[0] + xy_off + refOffset;

  VO_S32 cur_stride = dstPic->pic_stride[0];//dstPic->getStride();
#if REMOVE_CACHE_BUFF
    VO_U8 *p_cur  =  dstPic->pic_buf[0] + xy_off;
#if MC_ASM_ENABLED
    VO_S32   align_pos =	((VO_U32) p_cur)	& 15;
#endif
#endif

  VO_S32 xFrac     = mv->m_iHor & 0x3;
  VO_S32 yFrac     = mv->m_iVer & 0x3;

#if PAD_BUFF_ENABLED  
  VO_S32 xInt     = xPb; //GetLumaX(refPic, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, ref_stride);
  VO_S32 yInt     = yPb; //GetLumaY(refPic, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, ref_stride);

  
  VO_S32 ext_left = mc_ext_left[xFrac];
  VO_S32 ext_top =  mc_ext_left[yFrac];

  VO_S32 pic_width = refPic->m_iPicWidth;
  VO_S32 pic_height = refPic->m_iPicHeight;
  
  xInt += mv->m_iHor >> 2;
  yInt += mv->m_iVer >> 2;

#if USE_FRAME_THREAD
  if (p_slice->nThdNum > 1)
  {
	  VO_S32 max_y, max_x, ret;
	
	  max_y = yInt + height + 3; //+3 is depend on half length of sub-pixel filter
#if USE_LCU_SYNC
	  max_x = xInt + width + 3;
	  ret = WaitForProgress(refPic, max_y, max_y-(1<<p_slice->p_sps->log2_luma_coding_block_size), max_x);
#else
 	  ret = WaitForProgress(refPic, max_y);
#endif
	  if (ret)
	  {
		  //p_slice->cur_pic->errFlag = ret;
		  return;
	  }
  }
#endif


  if(xInt < ext_left || xInt > pic_width - width - mc_ext_right[xFrac] ||
  	 yInt < ext_top  || yInt > pic_height - height - mc_ext_right[yFrac])
  {
        VO_S32 offset = yInt * ref_stride + xInt;
        VO_U8 *fill = (VO_U8 *)p_slice->edge_emu_buff0;
        VO_S32   dst_stride = width + mc_ext[xFrac];
        VO_S32   fill_height = height + mc_ext[yFrac];

        FillMagin(p_ref - offset, 	ref_stride, fill, dst_stride, xInt - ext_left, yInt - ext_top, pic_width, pic_height, dst_stride, fill_height);

	 p_ref = fill + ext_left + ext_top * dst_stride;
	 ref_stride = dst_stride;
  }
#endif  

#if MC_ASM_ENABLED
  if (wp[0].offset == 0 && wp[0].shift == 6)
  {  
    arrFun_put_Luma[xFrac][yFrac](p_cur, cur_stride, p_ref, ref_stride, width, height);
  }
	else
	{
        arrFun_avg_nornd_MC_Bi_Luma[xFrac][yFrac](p_cur, cur_stride, p_ref, ref_stride, p_slice->asm_mc0_buff + align_pos, MAX_MC_CU_SIZE * 2, width, height);//lina
        xWeightedUnidir(p_cur, cur_stride, height, width, p_slice->asm_mc0_buff + align_pos, MAX_MC_CU_SIZE, wp[0].w, wp[0].round, wp[0].shift, wp[0].offset);
	}
#else
    MC_InterLumaWeighted(p_ref, ref_stride, p_cur, cur_stride, width, height, xFrac, yFrac, &wp[0]);
#endif

#if INTER_LOG
    printxPredInterLuma(p_cur, width, height, cur_stride);
#endif

}

// luma bi-weighted prediction
static VOINLINE VO_VOID xWeightedPredInterLumaBlkBi(H265_DEC_SLICE *p_slice, TComPic *refPic0, TComPic *refPic1, VO_S32 xPb, VO_S32 yPb, TComMv *mv0, TComMv *mv1, VO_S32 width, VO_S32 height, WP_SCALING_PARAM *wp0, WP_SCALING_PARAM *wp1, TComPic *dstPic )
{
    VO_S32 refStride0 = refPic0->pic_stride[0], refStride1 = refPic1->pic_stride[0];
    VO_S32 xy_off    = yPb * refStride0 + xPb;
    VO_S32 refOffset0 = ( mv0->m_iHor >> 2 ) + ( mv0->m_iVer >> 2 ) * refStride0, refOffset1 = ( mv1->m_iHor >> 2 ) + ( mv1->m_iVer >> 2 ) * refStride1;
    VO_U8 *ref0  = refPic0->pic_buf[0] + xy_off + refOffset0;
    VO_U8 *ref1  = refPic1->pic_buf[0] + xy_off + refOffset1;

    VO_S32 cur_stride  = dstPic->pic_stride[0];//dstPic->getStride();
#if REMOVE_CACHE_BUFF
    VO_U8 *p_cur        = dstPic->pic_buf[0] + xy_off;
#endif

  VO_S32 xFrac0     = mv0->m_iHor & 0x3,   xFrac1     = mv1->m_iHor & 0x3;
  VO_S32 yFrac0     = mv0->m_iVer & 0x3,   yFrac1     = mv1->m_iVer & 0x3;

#if PAD_BUFF_ENABLED
  VO_S32 x0_off     = xPb; //GetLumaX(refPic0, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride0);
  VO_S32 y0_off     = yPb; //GetLumaY(refPic0, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride0);

  VO_S32 ext0_left = mc_ext_left[xFrac0];
  VO_S32 ext0_top =  mc_ext_left[yFrac0];

  VO_S32 x1_off     = xPb; //GetLumaX(refPic1, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride1);
  VO_S32 y1_off     = yPb; //GetLumaY(refPic1, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride1);

  VO_S32 ext1_left = mc_ext_left[xFrac1];
  VO_S32 ext1_top =  mc_ext_left[yFrac1];
  

  VO_S32 pic_width = refPic0->m_iPicWidth;
  VO_S32 pic_height = refPic0->m_iPicHeight;

  
  x0_off += mv0->m_iHor >> 2;
  y0_off += mv0->m_iVer >> 2;

#if USE_FRAME_THREAD
  if (p_slice->nThdNum > 1)
  {
	  VO_S32 max_y, max_x, ret;

	  max_y = y0_off + height + 3; //+3 is depend on half length of sub-pixel filter

#if USE_LCU_SYNC
	  max_x = x0_off + width + 3;
	  ret = WaitForProgress(refPic0, max_y, max_y-(1<<p_slice->p_sps->log2_luma_coding_block_size), max_x);
#else
	  ret = WaitForProgress(refPic0, max_y);
#endif
	  if (ret)
	  {
		  //p_slice->cur_pic->errFlag = ret;
		  return;
	  }
  }
#endif

  if(x0_off < ext0_left || x0_off > pic_width - width - mc_ext_right[xFrac0] ||
  	 y0_off < ext0_top  || y0_off > pic_height - height - mc_ext_right[yFrac0])
  {
        VO_S32 offset = y0_off * refStride0 + x0_off;
        VO_U8 *fill = (VO_U8 *)p_slice->edge_emu_buff0;
        VO_S32   dst_stride = width + mc_ext[xFrac0];
        VO_S32   fill_height = height + mc_ext[yFrac0];

        FillMagin(ref0 - offset, 	refStride0, fill, dst_stride, x0_off - ext0_left, y0_off - ext0_top, pic_width, pic_height, dst_stride, fill_height);

	 ref0 = fill + ext0_left + ext0_top * dst_stride;
	 refStride0 = dst_stride;
  }

  x1_off += mv1->m_iHor >> 2;
  y1_off += mv1->m_iVer >> 2;

#if USE_FRAME_THREAD
  if (p_slice->nThdNum > 1)
  {
	  VO_S32 max_y, max_x, ret;

	  max_y = y1_off + height + 3; //+3 is depend on half length of sub-pixel filter

#if USE_LCU_SYNC
	  max_x = x1_off + width + 3;
	  ret = WaitForProgress(refPic1, max_y, max_y-(1<<p_slice->p_sps->log2_luma_coding_block_size), max_x);
#else
	  ret = WaitForProgress(refPic1, max_y);
#endif
	  if (ret)
	  {
		  //p_slice->cur_pic->errFlag = ret;
		  return;
	  }
  }
#endif

  if(x1_off < ext1_left || x1_off > pic_width - width - mc_ext_right[xFrac1] ||
  	 y1_off < ext1_top  || y1_off > pic_height - height - mc_ext_right[yFrac1])
  {
        VO_S32 offset = y1_off * refStride1 + x1_off;
        VO_U8 *fill = (VO_U8 *)p_slice->edge_emu_buff1;
        VO_S32   dst_stride = width + mc_ext[xFrac1];
        VO_S32   fill_height = height + mc_ext[yFrac1];

        FillMagin(ref1 - offset, 	refStride1, fill, dst_stride, x1_off - ext1_left, y1_off - ext1_top, pic_width, pic_height, dst_stride, fill_height);

	 ref1 = fill + ext1_left + ext1_top * dst_stride;
	 refStride1 = dst_stride;
  }
#endif  

  //MC_InterLumaWeightedBi(ref0, refStride0, ref1, refStride1, p_cur, cur_stride, width, height, xFrac0, yFrac0, xFrac1, yFrac1, &wp0[0], &wp1[0]);
  if (wp0[0].w == wp1[0].w && wp0[0].offset == 0 && wp0[0].shift == 7){

#if MC_ASM_ENABLED
    VO_S32   align_pos =  ((VO_U32) p_cur)  & 15;
    arrFun_avg_nornd_MC_Bi_Luma[xFrac0][yFrac0](p_cur, cur_stride, ref0, refStride0, p_slice->asm_mc0_buff + align_pos, MAX_MC_CU_SIZE * 2, width, height);
    arrFun_avg_MC_Bi_Luma[xFrac1][yFrac1](p_cur, cur_stride, ref1, refStride1, p_slice->asm_mc0_buff + align_pos, MAX_MC_CU_SIZE * 2, width, height);
#else
    MC_InterLumaBi(ref0, refStride0, ref1, refStride1, p_cur, cur_stride, width, height, xFrac0, yFrac0, xFrac1, yFrac1);
#endif
    }
    else
    {
#if MC_ASM_ENABLED
        VO_S32   align_pos =  ((VO_U32) p_cur)  & 15;
        arrFun_avg_nornd_MC_Bi_Luma[xFrac0][yFrac0](p_cur, cur_stride, ref0, refStride0, p_slice->asm_mc0_buff + align_pos, MAX_MC_CU_SIZE * 2, width, height);
        arrFun_avg_nornd_MC_Bi_Luma[xFrac1][yFrac1](p_cur, cur_stride, ref1, refStride1, p_slice->asm_mc1_buff + align_pos, MAX_MC_CU_SIZE * 2, width, height);
        xWeightedBidir(p_cur, cur_stride, height, width, p_slice->asm_mc0_buff + align_pos, p_slice->asm_mc1_buff + align_pos, MAX_MC_CU_SIZE, wp0->w, wp1->w, wp0->round, wp0->shift, wp0->offset);
#else
        MC_InterLumaWeightedBi(ref0, refStride0, ref1, refStride1, p_cur, cur_stride, width, height, xFrac0, yFrac0, xFrac1, yFrac1, &wp0[0], &wp1[0]);
#endif
    }

#if INTER_LOG
  printxPredInterLuma(p_cur, width, height, cur_stride);
#endif
	
}

// chroma weighted prediction
static VOINLINE VO_VOID xWeightedPredInterChromaBlk(H265_DEC_SLICE *p_slice, TComPic *refPic,  TComMv *mv,  VO_S32 xPb, VO_S32 yPb,   VO_S32 width, VO_S32 height, WP_SCALING_PARAM *wp, TComPic *dstPic )
{
    VO_S32 ref_stride = refPic->pic_stride[1]; 
    VO_S32 cur_stride = dstPic->pic_stride[1]; 

  VO_S32 refOffset = ( mv->m_iHor >> 3 ) + ( mv->m_iVer >> 3 ) * ref_stride;
  VO_S32 xy_off    = yPb / 2  * ref_stride + xPb / 2;

    VO_U8 *refCb	   = refPic->pic_buf[1] + xy_off + refOffset; // GetCbAddr(refPic, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr) + refOffset;
    VO_U8 *refCr	   = refPic->pic_buf[2] + xy_off + refOffset; //GetCrAddr(refPic, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr) + refOffset;

#if REMOVE_CACHE_BUFF
    VO_U8 *dstCb	   = dstPic->pic_buf[1] + xy_off; // GetCbAddr(dstPic, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr); // get cb buffer pointer from inter cache.
    VO_U8 *dstCr	   = dstPic->pic_buf[2] + xy_off; // GetCrAddr(dstPic, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr); // get cr buffer pointer from inter cache.
#endif
#if MC_ASM_ENABLED
    VO_S32   align0_pos =  ((VO_U32) dstCb)  & 15;
    VO_S32   align1_pos =  ((VO_U32) dstCr)  & 15;
#endif

  VO_S32 xFrac     = mv->m_iHor & 0x7;
  VO_S32 yFrac     = mv->m_iVer & 0x7;

#if PAD_BUFF_ENABLED
  VO_S32 xInt     = xPb / 2; //GetChromaX(refPic, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, ref_stride);
  VO_S32 yInt     = yPb / 2; //GetChromaY(refPic, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, ref_stride);

  VO_S32 pic_width = refPic->m_iPicWidth / 2;
  VO_S32 pic_height = refPic->m_iPicHeight / 2;
  VO_S32 ext_left = chroma_ext_left[!!xFrac];
  VO_S32 ext_top =  chroma_ext_left[!!yFrac];
  
  xInt += mv->m_iHor >> 3;
  yInt += mv->m_iVer >> 3;

    if(xInt < ext_left || xInt > pic_width - width / 2 - chroma_ext_right[!!xFrac] ||
            yInt < ext_top  || yInt > pic_height - height / 2 - chroma_ext_right[!!yFrac])
    {
        VO_S32 offset = yInt * ref_stride + xInt;
        VO_U8 *fillCb = (VO_U8 *)p_slice->edge_emu_buff0;
        VO_U8 *fillCr = (VO_U8 *)p_slice->edge_emu_buff1;
        VO_S32   dst_stride = width / 2 + chroma_ext[!!xFrac];
        VO_S32   fill_height = height / 2 + chroma_ext[!!yFrac];

        FillMagin(refCb - offset,	ref_stride, fillCb, dst_stride, xInt - ext_left, yInt - ext_top, pic_width, pic_height, dst_stride, fill_height);
        FillMagin(refCr - offset,	ref_stride, fillCr, dst_stride, xInt - ext_left, yInt - ext_top, pic_width, pic_height, dst_stride, fill_height);

        refCb = fillCb + ext_left + ext_top * dst_stride;
        refCr = fillCr + ext_left + ext_top * dst_stride;
        ref_stride = dst_stride;
    }

#endif

#if MC_ASM_ENABLED
	if (wp[0].offset == 0 && wp[0].shift == 6)
	{
        arrFun_put_Chroma[!!xFrac][!!yFrac](dstCb, cur_stride, refCb, ref_stride, width >> 1, height >> 1, xFrac, yFrac);
        arrFun_put_Chroma[!!xFrac][!!yFrac](dstCr, cur_stride, refCr, ref_stride, width >> 1, height >> 1, xFrac, yFrac);	
	}
	else
	{
        arrFun_avg_nornd_MC_Bi_Chroma[!!xFrac][!!yFrac](dstCb, cur_stride, refCb, ref_stride, p_slice->asm_mc0_buff + align0_pos, MAX_MC_CU_SIZE, width >> 1, height >> 1, xFrac, yFrac);
        arrFun_avg_nornd_MC_Bi_Chroma[!!xFrac][!!yFrac](dstCr, cur_stride, refCr, ref_stride, p_slice->asm_mc1_buff + align1_pos, MAX_MC_CU_SIZE, width >> 1, height >> 1, xFrac, yFrac);
        xWeightedUnidir(dstCb, cur_stride, height >> 1, width >> 1, p_slice->asm_mc0_buff + align0_pos, MAX_MC_CU_SIZE / 2, wp[1].w, wp[1].round, wp[1].shift, wp[1].offset);
        xWeightedUnidir(dstCr, cur_stride, height >> 1, width >> 1, p_slice->asm_mc1_buff + align1_pos, MAX_MC_CU_SIZE / 2, wp[2].w, wp[2].round, wp[2].shift, wp[2].offset);	
	}
#else
    MC_InterChromaWeighted(refCb, ref_stride, dstCb, cur_stride, width >> 1, height >> 1, xFrac, yFrac, &wp[1]);
    MC_InterChromaWeighted(refCr, ref_stride, dstCr, cur_stride, width >> 1, height >> 1, xFrac, yFrac, &wp[2]);
#endif

#if INTER_LOG
    printxPredInterChroma(dstCb, dstCr, width>>1, height>>1, cur_stride );
#endif

}

// chroma bi-weighted prediction
//static VOINLINE VO_VOID xWeightedPredInterChromaBlkBi( H265_DEC_SLICE *p_slice, TComPic *refPic0, TComPic *refPic1, VO_S32 xPb, VO_S32 yPb,   TComMv *mv0, TComMv *mv1, VO_S32 width, VO_S32 height, WP_SCALING_PARAM *wp0, WP_SCALING_PARAM *wp1, TComPic *dstPic )
static VOINLINE VO_VOID xWeightedPredInterChromaBlkBi(H265_DEC_SLICE *p_slice, TComPic *refPic0, TComPic *refPic1, VO_S32 xPb, VO_S32 yPb,   TComMv *mv0, TComMv *mv1, VO_S32 width, VO_S32 height, WP_SCALING_PARAM *wp0, WP_SCALING_PARAM *wp1, TComPic *dstPic )
{

  VO_S32 refStride0 = refPic0->pic_stride[1];  
  VO_S32 refStride1 = refPic1->pic_stride[1];
  VO_S32 cur_stride = dstPic->pic_stride[1];

  VO_S32 refOffset0 = ( mv0->m_iHor >> 3 ) + ( mv0->m_iVer >> 3 ) * refStride0, refOffset1 = ( mv1->m_iHor >> 3 ) + ( mv1->m_iVer >> 3 ) * refStride1;

  VO_S32 xy_off    = yPb / 2  * refStride0 + xPb / 2;

  VO_U8 *refCb0     = refPic0->pic_buf[1] + xy_off + refOffset0;  //GetCbAddr(refPic0, uiCUAddr, uiAbsPartIdx) + refOffset0;
  VO_U8 *refCr0     = refPic0->pic_buf[2] + xy_off + refOffset0;  //GetCrAddr(refPic0, uiCUAddr, uiAbsPartIdx) + refOffset0;
  VO_U8 *refCb1     = refPic1->pic_buf[1] + xy_off + refOffset1; //GetCbAddr(refPic1, uiCUAddr, uiAbsPartIdx) + refOffset1;
  VO_U8 *refCr1     = refPic1->pic_buf[2] + xy_off + refOffset1;  //GetCrAddr(refPic1, uiCUAddr, uiAbsPartIdx) + refOffset1;

#if REMOVE_CACHE_BUFF
    VO_U8 *dstCb	  =  dstPic->pic_buf[1] + xy_off; //GetCbAddr(dstPic, uiCUAddr, uiAbsPartIdx);
    VO_U8 *dstCr	  =  dstPic->pic_buf[2] + xy_off; //GetCrAddr(dstPic, uiCUAddr, uiAbsPartIdx);
#endif
#if MC_ASM_ENABLED
	VO_S32	 align0_pos =  ((VO_U32) dstCb)  & 15;
	VO_S32	 align1_pos =  ((VO_U32) dstCr)  & 15;
#endif


  VO_S32 xFrac0     = mv0->m_iHor & 0x7,  xFrac1     = mv1->m_iHor & 0x7;
  VO_S32 yFrac0     = mv0->m_iVer & 0x7,  yFrac1     = mv1->m_iVer & 0x7;

#if PAD_BUFF_ENABLED
  VO_S32 x0_off     = xPb / 2; //GetChromaX(refPic0, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride0);
  VO_S32 y0_off     = yPb / 2; //GetChromaY(refPic0, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride0);

  VO_S32 x1_off     = xPb / 2; //GetChromaX(refPic0, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride1);
  VO_S32 y1_off     = yPb / 2; //GetChromaY(refPic0, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride1);

  VO_S32 pic_width = refPic0->m_iPicWidth / 2;
  VO_S32 pic_height = refPic0->m_iPicHeight / 2;
  VO_S32 ext_left = chroma_ext_left[!!xFrac0];
  VO_S32 ext_top =  chroma_ext_left[!!yFrac0];
  x0_off += mv0->m_iHor >> 3;
  y0_off += mv0->m_iVer >> 3;

    if(x0_off < ext_left || x0_off > pic_width - width / 2 - chroma_ext_right[!!xFrac0] ||
            y0_off < ext_top  || y0_off > pic_height - height / 2 - chroma_ext_right[!!yFrac0])
    {
        VO_S32 offset = y0_off * refStride0 + x0_off;
        VO_U8 *fillCb = (VO_U8 *)p_slice->edge_emu_buff0;
        VO_U8 *fillCr = (VO_U8 *)p_slice->edge_emu_buff1;
        VO_S32   dst_stride = width / 2 + chroma_ext[!!xFrac0];
        VO_S32   fill_height = height / 2 + chroma_ext[!!yFrac0];

        FillMagin(refCb0 - offset, 	refStride0, fillCb, dst_stride, x0_off - ext_left, y0_off - ext_top, pic_width, pic_height, dst_stride, fill_height);
        FillMagin(refCr0 - offset, 	refStride0, fillCr, dst_stride, x0_off - ext_left, y0_off - ext_top, pic_width, pic_height, dst_stride, fill_height);

        refCb0 = fillCb + ext_left + ext_top * dst_stride;
        refCr0 = fillCr + ext_left + ext_top * dst_stride;
        refStride0 = dst_stride;
    }


    x1_off += mv1->m_iHor >> 3;
    y1_off += mv1->m_iVer >> 3;
    ext_left = chroma_ext_left[!!xFrac1];
    ext_top =  chroma_ext_left[!!yFrac1];
    if(x1_off < ext_left || x1_off > pic_width - width / 2 - chroma_ext_right[!!xFrac1] ||
            y1_off < ext_top  || y1_off > pic_height - height / 2 - chroma_ext_right[!!yFrac1])
    {
        VO_S32 offset = y1_off * refStride1 + x1_off;
        VO_U8 *fillCb = (VO_U8 *)(p_slice->edge_emu_buff0 + (MAX_CU_SIZE / 2 + 7) * (MAX_CU_SIZE / 2 + 7));
        VO_U8 *fillCr = (VO_U8 *)(p_slice->edge_emu_buff1 + (MAX_CU_SIZE / 2 + 7) * (MAX_CU_SIZE / 2 + 7));
        VO_S32   dst_stride = width / 2 + chroma_ext[!!xFrac1];
        VO_S32   fill_height = height / 2 + chroma_ext[!!yFrac1];

        FillMagin(refCb1 - offset, 	refStride1, fillCb, dst_stride, x1_off - ext_left, y1_off - ext_top, pic_width, pic_height, dst_stride, fill_height);
        FillMagin(refCr1 - offset, 	refStride1, fillCr, dst_stride, x1_off - ext_left, y1_off - ext_top, pic_width, pic_height, dst_stride, fill_height);

        refCb1 = fillCb + ext_left + ext_top * dst_stride;
        refCr1 = fillCr + ext_left + ext_top * dst_stride;
        refStride1 = dst_stride;
    }
#endif

  //MC_InterChromaWeightedBi(refCb0, refStride0, refCb1, refStride1, dstCb, cur_stride, width>>1, height>>1, xFrac0, yFrac0, xFrac1, yFrac1, &wp0[1], &wp1[1]);
  //MC_InterChromaWeightedBi(refCr0, refStride0, refCr1, refStride1, dstCr, cur_stride, width>>1, height>>1, xFrac0, yFrac0, xFrac1, yFrac1, &wp0[2], &wp1[2]);
  if (wp0[1].w == wp1[1].w && wp0[1].offset == 0 && wp0[1].shift == 7){

    VO_S32 width_UV = width >> 1;
    VO_S32 height_UV = height >> 1;

#if MC_ASM_ENABLED
    VO_S32 align0_pos =  ((VO_U32) dstCb)  & 15;
    arrFun_avg_nornd_MC_Bi_Chroma[!!xFrac0][!!yFrac0](dstCb, cur_stride, refCb0, refStride0, p_slice->asm_mc0_buff + align0_pos, MAX_MC_CU_SIZE, width_UV, height_UV, xFrac0, yFrac0);
    arrFun_avg_MC_Bi_Chroma[!!xFrac1][!!yFrac1](dstCb, cur_stride, refCb1, refStride1, p_slice->asm_mc0_buff + align0_pos, MAX_MC_CU_SIZE, width_UV, height_UV, xFrac1, yFrac1);
#else
    MC_InterChromaBi(refCb0, refStride0, refCb1, refStride1, dstCb, cur_stride, width_UV, height_UV, xFrac0, yFrac0, xFrac1, yFrac1);
#endif

    }
    else
    {
        //printf("not equal.--cb\n");
 #if MC_ASM_ENABLED
		arrFun_avg_nornd_MC_Bi_Chroma[!!xFrac0][!!yFrac0](dstCb, cur_stride, refCb0, refStride0, p_slice->asm_mc0_buff + align0_pos, MAX_MC_CU_SIZE, width >> 1, height >> 1, xFrac0, yFrac0);
		arrFun_avg_nornd_MC_Bi_Chroma[!!xFrac1][!!yFrac1](dstCb, cur_stride, refCb1, refStride1, p_slice->asm_mc1_buff + align0_pos, MAX_MC_CU_SIZE, width >> 1, height >> 1, xFrac1, yFrac1);
    xWeightedBidir(dstCb, cur_stride, height >> 1, width >> 1, p_slice->asm_mc0_buff + align0_pos, p_slice->asm_mc1_buff + align0_pos, MAX_MC_CU_SIZE / 2, wp0[1].w, wp1[1].w, wp0[1].round, wp0[1].shift, wp0[1].offset);
#else       
    MC_InterChromaWeightedBi(refCb0, refStride0, refCb1, refStride1, dstCb, cur_stride, width >> 1, height >> 1, xFrac0, yFrac0, xFrac1, yFrac1, &wp0[1], &wp1[1]);
#endif
    }

  if (wp0[2].w == wp1[2].w && wp0[2].offset == 0 && wp0[2].shift == 7){
    VO_S32 width_UV = width >> 1;
    VO_S32 height_UV = height >> 1;
#if MC_ASM_ENABLED
    VO_S32 align1_pos =  ((VO_U32) dstCr)  & 15;
    arrFun_avg_nornd_MC_Bi_Chroma[!!xFrac0][!!yFrac0](dstCr, cur_stride, refCr0, refStride0, p_slice->asm_mc1_buff + align1_pos, MAX_MC_CU_SIZE, width_UV, height_UV, xFrac0, yFrac0);
    arrFun_avg_MC_Bi_Chroma[!!xFrac1][!!yFrac1](dstCr, cur_stride, refCr1, refStride1, p_slice->asm_mc1_buff + align1_pos, MAX_MC_CU_SIZE, width_UV, height_UV, xFrac1, yFrac1);
#else
    MC_InterChromaBi(refCr0, refStride0, refCr1, refStride1, dstCr, cur_stride, width_UV, height_UV, xFrac0, yFrac0, xFrac1, yFrac1);
#endif
    }
    else
    {
#if MC_ASM_ENABLED
        arrFun_avg_nornd_MC_Bi_Chroma[!!xFrac0][!!yFrac0](dstCr, cur_stride, refCr0, refStride0, p_slice->asm_mc0_buff + align1_pos, MAX_MC_CU_SIZE, width >> 1, height >> 1, xFrac0, yFrac0);
        arrFun_avg_nornd_MC_Bi_Chroma[!!xFrac1][!!yFrac1](dstCr, cur_stride, refCr1, refStride1, p_slice->asm_mc1_buff + align1_pos, MAX_MC_CU_SIZE, width >> 1, height >> 1, xFrac1, yFrac1);
        xWeightedBidir(dstCr, cur_stride, height >> 1, width >> 1, p_slice->asm_mc0_buff + align1_pos, p_slice->asm_mc1_buff + align1_pos, MAX_MC_CU_SIZE / 2, wp0[2].w, wp1[2].w, wp0[2].round, wp0[2].shift, wp0[2].offset);		
#else
        MC_InterChromaWeightedBi(refCr0, refStride0, refCr1, refStride1, dstCr, cur_stride, width >> 1, height >> 1, xFrac0, yFrac0, xFrac1, yFrac1, &wp0[2], &wp1[2]);
#endif
    }

#if INTER_LOG
  printxPredInterChroma(dstCb, dstCr, width>>1, height>>1, cur_stride );
#endif

}
#endif

VO_VOID xPredInterUni ( //H265_DEC *p_dec, 
	H265_DEC_SLICE* p_slice, 
	const VO_S32 xPb, 
	const VO_S32 yPb, 
	const VO_S32 nPbW, 
	const VO_S32 nPbH, 
	RefPicList eRefPicList, 
	PicMvField* p_motion_field )
{
    VO_S8 iRefIdx      = p_motion_field->m_iRefIdx[ eRefPicList ];  
    TComMv *cMv        = (TComMv*) &(p_motion_field->m_acMv[ eRefPicList ]);  

    TComPic* p_ref_pic = p_slice->ref_pic_list[ ( VO_S32 )( eRefPicList ) ][ iRefIdx ];  
	TComPic* p_cur_pic = p_slice->cur_pic;

    VO_S32 xInt = xPb + ( cMv->m_iHor >> 2 );
    VO_S32 yInt = yPb + ( cMv->m_iVer >> 2 );
    VO_S32 xFrac = cMv->m_iHor & 0x3;
    VO_S32 yFrac = cMv->m_iVer & 0x3;

    VO_S32 ref_stride = p_ref_pic->pic_stride[ 0 ];
    VO_S32 pic_width  = p_ref_pic->m_iPicWidth;
    VO_S32 pic_height = p_ref_pic->m_iPicHeight;

    VO_S32 xy_off     = yPb * ref_stride + xPb;

    VO_S32 cur_stride = p_cur_pic->pic_stride[ 0 ];
    VO_U8 *p_cur      = p_cur_pic->pic_buf[ 0 ] + xy_off;

    VO_S32 ext_left   = mc_ext_left[ xFrac ];
    VO_S32 ext_top    = mc_ext_left[ yFrac ];
	VO_U8 *p_ref;
    VO_U8 *refCb;
    VO_U8 *refCr;
    VO_U8 *dstCb;
    VO_U8 *dstCr;

    //ClipMv(p_slice,cMv, xPb, yPb); 


    VOASSERT( iRefIdx >= 0 );//YU_TBD: check refidx and inter_dir identical

    p_ref = p_ref_pic->pic_buf[ 0 ] + yInt * ref_stride + xInt ;

#if USE_FRAME_THREAD
    if (p_slice->nThdNum > 1)
    {
      VO_S32 max_y, max_x, ret;

      max_y = yInt + nPbH + 3; //+3 is depend on half length of sub-pixel filter
#if USE_LCU_SYNC
	  max_x = xInt + nPbW + 3;
	  ret = WaitForProgress(p_ref_pic, max_y, max_y-(1<<p_slice->p_sps->log2_luma_coding_block_size), max_x);
#else
	  ret = WaitForProgress(p_ref_pic, max_y);
#endif
	  if (ret)
	  {
		  //p_slice->cur_pic->errFlag = ret;
		  return;
	  }
    }
#endif

    if ( ( ( xInt - ext_left ) < 0 ) || ( ( xInt + nPbW + mc_ext_right[ xFrac ] ) > pic_width ) ||
        ( ( yInt - ext_top ) < 0 ) || ( ( yInt + nPbH + mc_ext_right[ yFrac ] ) > pic_height ) ) {

        VO_U8  *fill = (VO_U8 *)p_slice->edge_emu_buff0;
        VO_S32 dst_stride = nPbW + mc_ext[ xFrac ];
        VO_S32 fill_height = nPbH + mc_ext[ yFrac ];

        FillMagin( p_ref_pic->pic_buf[ 0 ] , ref_stride, fill, dst_stride, xInt - ext_left, 
			yInt - ext_top, pic_width, pic_height, dst_stride, fill_height );

        p_ref = fill + ext_left + ext_top * dst_stride;
        ref_stride = dst_stride;
    }
#if MC_ASM_ENABLED 
    arrFun_put_Luma[xFrac][yFrac](p_cur, cur_stride, p_ref, ref_stride, nPbW, nPbH);
#else
    MC_InterLuma( p_ref, ref_stride, p_cur, cur_stride, nPbW, nPbH, xFrac, yFrac );
#endif

#if INTER_LOG
    printxPredInterLuma(p_cur, nPbW, nPbH, cur_stride);
#endif
    ref_stride = p_ref_pic->pic_stride[ 1 ];
    cur_stride = p_cur_pic->pic_stride[ 1 ];

    xFrac     = cMv->m_iHor & 0x7;
    yFrac     = cMv->m_iVer & 0x7;
    xInt = ( xPb >> 1 ) + ( cMv->m_iHor >> 3 );
    yInt = ( yPb >> 1 ) + ( cMv->m_iVer >> 3 );

    xy_off    = ( yPb >> 1 )  * ref_stride + ( xPb >> 1 );
    refCb     = p_ref_pic->pic_buf[ 1 ] + yInt * ref_stride + xInt;
    refCr     = p_ref_pic->pic_buf[ 2 ] + yInt * ref_stride + xInt; 
    dstCb     = p_cur_pic->pic_buf[ 1 ] + xy_off; 
    dstCr     = p_cur_pic->pic_buf[ 2 ] + xy_off; 

 
    pic_width >>= 1;
    pic_height >>= 1;
    ext_left = chroma_ext_left[ !!xFrac ];
    ext_top =  chroma_ext_left[ !!yFrac ];

    if ( xInt < ext_left || xInt > pic_width - nPbW / 2 - chroma_ext_right[ !!xFrac ] ||
        yInt < ext_top  || yInt > pic_height - nPbH / 2 - chroma_ext_right[ !!yFrac ]) {
        
        //VO_S32 offset = yInt * ref_stride + xInt;
        VO_U8 *fillCb = p_slice->edge_emu_buff0;
        VO_U8 *fillCr = p_slice->edge_emu_buff1;
        VO_S32 dst_stride = nPbW / 2 + chroma_ext[ !!xFrac ];
        VO_S32 fill_height = nPbH / 2 + chroma_ext[ !!yFrac ];

        FillMagin(p_ref_pic->pic_buf[ 1 ], 	ref_stride, fillCb, dst_stride, xInt - ext_left,
			yInt - ext_top, pic_width, pic_height, dst_stride, fill_height);     
        FillMagin(p_ref_pic->pic_buf[ 2 ], 	ref_stride, fillCr, dst_stride, xInt - ext_left, 
			yInt - ext_top, pic_width, pic_height, dst_stride, fill_height);

        refCb = fillCb + ext_left + ext_top * dst_stride;
        refCr = fillCr + ext_left + ext_top * dst_stride;
        ref_stride = dst_stride;
    }
#if MC_ASM_ENABLED
    arrFun_put_Chroma[!!xFrac][!!yFrac](dstCb, cur_stride, refCb, ref_stride, nPbW >> 1, nPbH >> 1, xFrac, yFrac);
    arrFun_put_Chroma[!!xFrac][!!yFrac](dstCr, cur_stride, refCr, ref_stride, nPbW >> 1, nPbH >> 1, xFrac, yFrac);
#else
    MC_InterChroma( refCb, ref_stride, dstCb, cur_stride, nPbW >> 1, nPbH >> 1, xFrac, yFrac );
    MC_InterChroma( refCr, ref_stride, dstCr, cur_stride, nPbW >> 1, nPbH >> 1, xFrac, yFrac );
#endif
#if INTER_LOG
    printxPredInterChroma(dstCb, dstCr, nPbW >> 1, nPbH >> 1, cur_stride);
#endif 

}


VO_VOID xPredInterBi ( H265_DEC_SLICE* p_slice, 
	VO_S32 xPb, 
	VO_S32 yPb, 
	VO_S32 nPbW, 
	VO_S32 nPbH, 
	TComPic* p_cur_pic, 
	PicMvField* p_motion_field)
{
    TComPic  *p_ref_pic[2] /*= {0, 0}*/;
    VO_S32    iRefIdx_value_0;
    VO_S32    iRefIdx_value_1;

    iRefIdx_value_0 = p_motion_field->m_iRefIdx[0]; 
    iRefIdx_value_1 = p_motion_field->m_iRefIdx[1]; 
    assert( iRefIdx_value_0 >= 0);
    assert( iRefIdx_value_1 >= 0 );
    p_ref_pic[0] = p_slice->ref_pic_list[0][iRefIdx_value_0];  
    p_ref_pic[1] = p_slice->ref_pic_list[1][iRefIdx_value_1];

  {
    TComMv *cMv0 = (TComMv*)&(p_motion_field->m_acMv[0]);  
    TComMv *cMv1 = (TComMv*)&(p_motion_field->m_acMv[1]);

    // Get motion vector according to reference index.
#if 0 //YU_TBD: support it
    //ClipMvNew(p_slice,cMv0, xPb, yPb);
    //ClipMvNew(p_slice,cMv1, xPb, yPb);
#endif
    {

      VO_S32 refStride0 = p_ref_pic[0]->pic_stride[0], refStride1 = p_ref_pic[1]->pic_stride[0];
      VO_S32 refOffset0 = ( cMv0->m_iHor >> 2 ) + ( cMv0->m_iVer >> 2 ) * refStride0;
      VO_S32 refOffset1 = ( cMv1->m_iHor >> 2 ) + ( cMv1->m_iVer >> 2 ) * refStride1;
      VO_S32 xy_off    = yPb * refStride0 + xPb;
      VO_U8 *ref0       = p_ref_pic[0]->pic_buf[0] + xy_off + refOffset0; 
      VO_U8 *ref1       = p_ref_pic[1]->pic_buf[0] + xy_off + refOffset1; 
      VO_U8 *refCb0;
      VO_U8 *refCr0;
      VO_U8 *refCb1;
      VO_U8 *refCr1;

      VO_U8 *dstCb;
      VO_U8 *dstCr;
      VO_S32 cur_stride  = p_cur_pic->pic_stride[0];
      VO_U8 *p_cur  = p_cur_pic->pic_buf[0] + xy_off;

      VO_S32 xFrac0     = cMv0->m_iHor & 0x3,   xFrac1     = cMv1->m_iHor & 0x3;
      VO_S32 yFrac0     = cMv0->m_iVer & 0x3,   yFrac1     = cMv1->m_iVer & 0x3;


#if 1//PAD_BUFF_ENABLED
      VO_S32 x0_off     = xPb; //GetLumaX(refPic0, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride0);
      VO_S32 y0_off     = yPb; //GetLumaY(refPic0, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride0);

      VO_S32 ext0_left = mc_ext_left[xFrac0];
      VO_S32 ext0_top =  mc_ext_left[yFrac0];

      VO_S32 x1_off     = xPb; //GetLumaX(refPic1, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride1);
      VO_S32 y1_off     = yPb; //GetLumaY(refPic1, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride1);

      VO_S32 ext1_left = mc_ext_left[xFrac1];
      VO_S32 ext1_top =  mc_ext_left[yFrac1];


      VO_S32 pic_width = p_ref_pic[0]->m_iPicWidth;
      VO_S32 pic_height = p_ref_pic[0]->m_iPicHeight;

      VO_S32 flag_fillMagin_UV_0 = 0;
      VO_S32 flag_fillMagin_UV_1 = 0;
      VO_S32 offset_UV_0;     
      VO_S32 offset_UV_1;
      VO_S32 dst_stride_UV_0;
      VO_S32 dst_stride_UV_1;
      VO_S32 fill_height_UV_0;
      VO_S32 fill_height_UV_1;
      VO_S32 refStride_UV_0;
      VO_S32 refStride_UV_1;
      VO_S32 width_UV;
      VO_S32 height_UV;


      x0_off += cMv0->m_iHor >> 2;
      y0_off += cMv0->m_iVer >> 2;

#if USE_FRAME_THREAD
      if (p_slice->nThdNum > 1)
      {
        VO_S32 max_y, max_x, ret;

        max_y = y0_off + nPbH + 3; //+3 is depend on half length of sub-pixel filter

#if USE_LCU_SYNC
		max_x = x0_off + nPbW + 3;
		ret = WaitForProgress(p_ref_pic[0], max_y, max_y-(1<<p_slice->p_sps->log2_luma_coding_block_size), max_x);
#else
		ret = WaitForProgress(p_ref_pic[0], max_y);
#endif
		if (ret)
		{
			//p_slice->cur_pic->errFlag = ret;
			return;
		}
      }
#endif

      if ( x0_off < ext0_left || x0_off > pic_width - nPbW - mc_ext_right[xFrac0] ||
           y0_off < ext0_top  || y0_off > pic_height - nPbH - mc_ext_right[yFrac0] ) {
            
          VO_S32 offset = y0_off * refStride0 + x0_off;
          VO_U8 *fill = (VO_U8 *)p_slice->edge_emu_buff0;
          VO_S32 dst_stride = nPbW + mc_ext[xFrac0];
          VO_S32 fill_height = nPbH + mc_ext[yFrac0];

          FillMagin(ref0 - offset, 	refStride0, fill, dst_stride, x0_off - ext0_left, y0_off - ext0_top, pic_width, pic_height, dst_stride, fill_height);

          ref0 = fill + ext0_left + ext0_top * dst_stride;
          refStride0 = dst_stride;
      }

      x1_off += cMv1->m_iHor >> 2;
      y1_off += cMv1->m_iVer >> 2;

#if USE_FRAME_THREAD
      if (p_slice->nThdNum > 1)
      {
        VO_S32 max_y, max_x, ret;

        max_y = y1_off + nPbH + 3; //+3 is depend on half length of sub-pixel filter

#if USE_LCU_SYNC
		max_x = x1_off + nPbW + 3;
		ret = WaitForProgress(p_ref_pic[1], max_y, max_y-(1<<p_slice->p_sps->log2_luma_coding_block_size), max_x);
#else
		ret = WaitForProgress(p_ref_pic[1], max_y);
#endif
		if (ret)
		{
			//p_slice->cur_pic->errFlag = ret;
			return;
		}
      }
#endif

            if(x1_off < ext1_left || x1_off > pic_width - nPbW - mc_ext_right[xFrac1] ||
                    y1_off < ext1_top  || y1_off > pic_height - nPbH - mc_ext_right[yFrac1])
            {
                VO_S32 offset = y1_off * refStride1 + x1_off;
                VO_U8 *fill = (VO_U8 *)p_slice->edge_emu_buff1;
                VO_S32   dst_stride = nPbW + mc_ext[xFrac1];
                VO_S32   fill_height = nPbH + mc_ext[yFrac1];

        FillMagin(ref1 - offset, 	refStride1, fill, dst_stride, x1_off - ext1_left, y1_off - ext1_top, pic_width, pic_height, dst_stride, fill_height);

        ref1 = fill + ext1_left + ext1_top * dst_stride;
        refStride1 = dst_stride;
      }
#endif  

#if MC_ASM_ENABLED
     {
         VO_S32   align_pos =  ((VO_U32) p_cur)  & 15;
         arrFun_avg_nornd_MC_Bi_Luma[xFrac0][yFrac0](p_cur, cur_stride, ref0, refStride0, p_slice->asm_mc0_buff + align_pos, MAX_MC_CU_SIZE * 2, nPbW, nPbH);
         arrFun_avg_MC_Bi_Luma[xFrac1][yFrac1](p_cur, cur_stride, ref1, refStride1, p_slice->asm_mc0_buff + align_pos, MAX_MC_CU_SIZE * 2, nPbW, nPbH);
     }
#else
      MC_InterLumaBi(ref0, refStride0, ref1, refStride1, p_cur, cur_stride, nPbW, nPbH, xFrac0, yFrac0, xFrac1, yFrac1);
#endif
#if INTER_LOG
      printxPredInterLuma(p_cur, nPbW, nPbH, cur_stride);    
#endif
      refStride_UV_0 = p_ref_pic[0]->pic_stride[1];
      refStride_UV_1 = p_ref_pic[1]->pic_stride[1];
      refStride0 = refStride_UV_0;
      refStride1 = refStride_UV_1;
      cur_stride  = p_cur_pic->pic_stride[1];

      xy_off    = (yPb >> 1)  * refStride0 + (xPb >> 1);
      refOffset0 = ( cMv0->m_iHor >> 3 ) + ( cMv0->m_iVer >> 3 ) * refStride0;
      refOffset1 = ( cMv1->m_iHor >> 3 ) + ( cMv1->m_iVer >> 3 ) * refStride1;

      refCb0     = p_ref_pic[0]->pic_buf[1] + xy_off + refOffset0; //GetCbAddr(refPic0, uiCUAddr, uiAbsPartIdx) + refOffset0;
      refCr0     = p_ref_pic[0]->pic_buf[2] + xy_off + refOffset0; //GetCrAddr(refPic0, uiCUAddr, uiAbsPartIdx) + refOffset0;
      refCb1     = p_ref_pic[1]->pic_buf[1] + xy_off + refOffset1; //GetCbAddr(refPic1, uiCUAddr, uiAbsPartIdx) + refOffset1;
      refCr1     = p_ref_pic[1]->pic_buf[2] + xy_off + refOffset1; //GetCrAddr(refPic1, uiCUAddr, uiAbsPartIdx) + refOffset1;

      dstCb      = p_cur_pic->pic_buf[1] + xy_off; //GetCbAddr(dstPic, uiCUAddr, uiAbsPartIdx);
      dstCr      = p_cur_pic->pic_buf[2] + xy_off; //GetCrAddr(dstPic, uiCUAddr, uiAbsPartIdx);

            xFrac0     = cMv0->m_iHor & 0x7;
            xFrac1     = cMv1->m_iHor & 0x7;
            yFrac0     = cMv0->m_iVer & 0x7;
            yFrac1     = cMv1->m_iVer & 0x7;


#if 1//PAD_BUFF_ENABLED
      x0_off     = xPb / 2; //GetChromaX(refPic0, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride0);
      y0_off     = yPb / 2; //GetChromaY(refPic0, p_slice->m_uiCUAddr, p_slice->m_uiAbsIdxInLCU + partAddr, refStride0);

      x1_off = x0_off;
      y1_off = y0_off;

      pic_width = pic_width / 2;
      pic_height = pic_height / 2;
//       ext0_left = chroma_ext_left[!!xFrac0];
//       ext0_top =  chroma_ext_left[!!yFrac0];
      ext0_left = !!xFrac0;
      ext0_top =  !!yFrac0;
      x0_off += cMv0->m_iHor >> 3;
      y0_off += cMv0->m_iVer >> 3;
      width_UV = nPbW >> 1;
      height_UV = nPbH >> 1;
      // Cb inter bi prediction.
      if(x0_off < ext0_left || x0_off > pic_width - width_UV -chroma_ext_right[!!xFrac0] ||
        y0_off < ext0_top  || y0_off > pic_height - height_UV -chroma_ext_right[!!yFrac0])
      {

        //VO_U8 *fillCb = (VO_U8 *)edge_emu_buff_cb0;
        //VO_U8 *fillCb = (VO_U8 *)edge_emu_buff0;
        offset_UV_0 = y0_off * refStride_UV_0 + x0_off;
        dst_stride_UV_0 = width_UV + chroma_ext[!!xFrac0];
        fill_height_UV_0 = height_UV + chroma_ext[!!yFrac0];

                flag_fillMagin_UV_0 = 1;
                FillMagin(refCb0 - offset_UV_0, 	refStride_UV_0, p_slice->edge_emu_buff0, dst_stride_UV_0, x0_off - ext0_left,
                          y0_off - ext0_top, pic_width, pic_height, dst_stride_UV_0, fill_height_UV_0);

                refCb0 = p_slice->edge_emu_buff0 + ext0_left + ext0_top * dst_stride_UV_0;
                refStride0 = dst_stride_UV_0;
            }


            x1_off += cMv1->m_iHor >> 3;
            y1_off += cMv1->m_iVer >> 3;
            //       ext1_left = chroma_ext_left[!!xFrac1];
            //       ext1_top =  chroma_ext_left[!!yFrac1];
            ext1_left = !!xFrac1;
            ext1_top =  !!yFrac1;
            if(x1_off < ext1_left || x1_off > pic_width - width_UV - chroma_ext_right[!!xFrac1] ||
                    y1_off < ext1_top  || y1_off > pic_height - height_UV - chroma_ext_right[!!yFrac1])
            {
                //VO_U8 *fillCb = (VO_U8 *)edge_emu_buff1;
                offset_UV_1 = y1_off * refStride_UV_1 + x1_off;
                dst_stride_UV_1 = width_UV + chroma_ext[!!xFrac1];
                fill_height_UV_1 = height_UV + chroma_ext[!!yFrac1];
                flag_fillMagin_UV_1 = 1;
                FillMagin(refCb1 - offset_UV_1, 	refStride_UV_1, p_slice->edge_emu_buff1, dst_stride_UV_1, x1_off - ext1_left,
                          y1_off - ext1_top, pic_width, pic_height, dst_stride_UV_1, fill_height_UV_1);

                refCb1 = p_slice->edge_emu_buff1 + ext1_left + ext1_top * dst_stride_UV_1;
                refStride1 = dst_stride_UV_1;
            }
#endif

#if MC_ASM_ENABLED
      {
          VO_U32 align0_pos =  ((VO_U32) dstCb)  & 15;
          arrFun_avg_nornd_MC_Bi_Chroma[!!xFrac0][!!yFrac0](dstCb, cur_stride, refCb0, refStride0, p_slice->asm_mc0_buff + align0_pos, MAX_MC_CU_SIZE, width_UV, height_UV, xFrac0, yFrac0);
          arrFun_avg_MC_Bi_Chroma[!!xFrac1][!!yFrac1](dstCb, cur_stride, refCb1, refStride1, p_slice->asm_mc0_buff + align0_pos, MAX_MC_CU_SIZE, width_UV, height_UV, xFrac1, yFrac1);
      }
#else
      MC_InterChromaBi(refCb0, refStride0, refCb1, refStride1, dstCb, cur_stride, width_UV, height_UV, xFrac0, yFrac0, xFrac1, yFrac1);
#endif
            // Cr inter bi prediction
            if (flag_fillMagin_UV_0)
            {
                //VO_U8 *fillCr = (VO_U8 *)edge_emu_buff0;
                FillMagin(refCr0 - offset_UV_0, 	refStride_UV_0, p_slice->edge_emu_buff0, dst_stride_UV_0, x0_off - ext0_left,
                          y0_off - ext0_top, pic_width, pic_height, dst_stride_UV_0, fill_height_UV_0);
                refCr0 = refCb0; // 由于fillCr==fillCb==edge_emu_buff0，所以才可以这样赋值
            }

            if(flag_fillMagin_UV_1)
            {
                //VO_U8 *fillCr = (VO_U8 *)edge_emu_buff1;
                FillMagin(refCr1 - offset_UV_1, 	refStride_UV_1, p_slice->edge_emu_buff1, dst_stride_UV_1, x1_off - ext1_left,
                          y1_off - ext1_top, pic_width, pic_height, dst_stride_UV_1, fill_height_UV_1);
                refCr1 = refCb1; // 由于fillCr==fillCb==edge_emu_buff1，所以才可以这样赋值
            }
#if MC_ASM_ENABLED
            {
                VO_U32 align1_pos =  ((VO_U32) dstCr)  & 15;
                arrFun_avg_nornd_MC_Bi_Chroma[!!xFrac0][!!yFrac0](dstCr, cur_stride, refCr0, refStride0, p_slice->asm_mc0_buff + align1_pos, MAX_MC_CU_SIZE, width_UV, height_UV, xFrac0, yFrac0);
                arrFun_avg_MC_Bi_Chroma[!!xFrac1][!!yFrac1](dstCr, cur_stride, refCr1, refStride1, p_slice->asm_mc0_buff + align1_pos, MAX_MC_CU_SIZE, width_UV, height_UV, xFrac1, yFrac1);
            }
#else
      MC_InterChromaBi(refCr0, refStride0, refCr1, refStride1, dstCr, cur_stride, width_UV, height_UV, xFrac0, yFrac0, xFrac1, yFrac1);
#endif

#if INTER_LOG
      printxPredInterChroma(dstCb, dstCr, width_UV, height_UV, cur_stride );
#endif
    }
   }

}

VO_VOID xInterWeightedPred_Bi ( 
	H265_DEC_SLICE* p_slice, 
	VO_S32 xPb, 
	VO_S32 yPb, 
	VO_S32 nPbW, 
	VO_S32 nPbH, 
	TComPic* p_cur_pic,
	PicMvField* p_motion_field)
{
  TComPic  *p_ref_pic[2] /*= {0, 0}*/;
  VO_S32    iRefIdx_value_0;
  VO_S32    iRefIdx_value_1;
  //VO_BOOL   biPred = VO_TRUE;

  iRefIdx_value_0 = p_motion_field->m_iRefIdx[0]; //  (p_mv_field + iRefList)->m_piRefIdx[uiAbsPartIdx];//pcCU->getCUMvField( eRefPicList )->getRefIdx( uiPartAddr );
  assert( iRefIdx_value_0 >= 0);
  p_ref_pic[0] = iRefIdx_value_0 >= 0 ? p_slice->ref_pic_list[0][iRefIdx_value_0] : 0;

  iRefIdx_value_1 = p_motion_field->m_iRefIdx[1]; //  (p_mv_field + iRefList)->m_piRefIdx[uiAbsPartIdx];//pcCU->getCUMvField( eRefPicList )->getRefIdx( uiPartAddr );
  assert( iRefIdx_value_1 >= 0 );
  p_ref_pic[1] = iRefIdx_value_1 >= 0 ? p_slice->ref_pic_list[1][iRefIdx_value_1] : 0;

  {
    TComMv *cMv0 = (TComMv*)&(p_motion_field->m_acMv[0]);  
    TComMv *cMv1 = (TComMv*)&(p_motion_field->m_acMv[1]);

    // Get motion vector according to reference index.

    //ClipMvNew(p_slice,cMv0, xPb, yPb);
    //ClipMvNew(p_slice,cMv1, xPb, yPb);

    //if (p_slice->p_pps->weighted_bipred_flag/* && p_slice->slice_type == B_SLICE*/)
    {
      WP_SCALING_PARAM wp0[3], wp1[3];
      GetWpScaling(p_slice, iRefIdx_value_0, iRefIdx_value_1, &wp0[0], &wp1[0]);
      xWeightedPredInterLumaBlkBi(p_slice, p_ref_pic[0], p_ref_pic[1], xPb, yPb, cMv0, cMv1, nPbW, nPbH, &wp0[0], &wp1[0], p_cur_pic);
      xWeightedPredInterChromaBlkBi(p_slice, p_ref_pic[0], p_ref_pic[1], xPb, yPb, cMv0, cMv1, nPbW, nPbH, &wp0[0], &wp1[0], p_cur_pic);
    }
  }
}

VO_VOID xInterWeightedPred_Uni ( H265_DEC_SLICE* p_slice, 
	VO_S32 xPb, 
	VO_S32 yPb, 
	VO_S32 nPbW, 
	VO_S32 nPbH, 
	RefPicList eRefPicList, 
	TComPic* p_cur_pic, 
	PicMvField* p_motion_field )
{
  VO_S8 iRefIdx             = p_motion_field->m_iRefIdx[eRefPicList]; // p_mv_field->m_piRefIdx[uiAbsPartIdx];                        // Get reference index from reference list.
  TComPic* p_ref_pic          = p_slice->ref_pic_list[(VO_S32)( eRefPicList )][iRefIdx];  // Get reference picture.
  TComMv *cMv =  (TComMv*)&(p_motion_field->m_acMv[eRefPicList]);  

  assert( iRefIdx >= 0 );

  //  ClipMv( p_slice, cMv );
  //ClipMvNew(p_slice,cMv, xPb, yPb); 

  // if (p_slice->p_pps->weighted_pred_flag)
  {
    WP_SCALING_PARAM wp_param[3];
    if (eRefPicList == REF_PIC_LIST_0)
    {
      GetWpScaling(p_slice, iRefIdx, -1, &wp_param[0], NULL);
    }
    else
    {
      GetWpScaling(p_slice, -1, iRefIdx, NULL, &wp_param[0]);
    }

        xWeightedPredInterLumaBlk(p_slice, p_ref_pic, cMv,  xPb, yPb, nPbW, nPbH, &wp_param[0], p_cur_pic);
        xWeightedPredInterChromaBlk(p_slice, p_ref_pic, cMv,  xPb, yPb, nPbW, nPbH, &wp_param[0], p_cur_pic);

    }
}


VO_S32 H265_MotionCompensation( H265_DEC_SLICE* p_slice,
	const VO_U32 xPb, 
	const VO_U32 yPb, 
	const VO_U32 nPbW, 
	const VO_U32 nPbH,
	PicMvField* p_PuMvField ) 
{
    if ( !p_slice->p_pps->weighted_bipred_flag ) {
		VO_S32 flag_Uni = xCheckIdenticalMotion( p_slice, p_PuMvField );
        if ( flag_Uni >= 0  ) {
            xPredInterUni( p_slice, xPb, yPb, nPbW, nPbH, (RefPicList)flag_Uni, p_PuMvField );
        } else {
            xPredInterBi( p_slice, xPb, yPb,  nPbW, nPbH, p_slice->cur_pic, p_PuMvField);      
        }
    } else {
        if ( p_PuMvField->inter_dir != 3 ) {
            VO_S32 flag_Uni = p_PuMvField->m_iRefIdx[0] >= 0 ? 0 : 1;

            xInterWeightedPred_Uni ( p_slice, xPb, yPb,  nPbW, nPbH, (RefPicList)flag_Uni, p_slice->cur_pic, p_PuMvField);
        } else {
            xInterWeightedPred_Bi( p_slice, xPb, yPb,  nPbW, nPbH, p_slice->cur_pic, p_PuMvField);  
        }
	}

	return VO_ERR_NONE;
}

