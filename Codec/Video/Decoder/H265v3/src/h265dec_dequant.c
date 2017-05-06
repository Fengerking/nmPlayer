#include "h265dec_dequant.h"
#include "h265dec_vlc.h"
#include "h265dec_utils.h"
#include "stdlib.h"//YU_TBD
#include <stdio.h>
#include <string.h>   // for ios memcpy
#include <assert.h>


const VO_U32 g_sigLastScanCG32x32[ 64 ]=
{
  0, 8, 1, 16, 9, 2, 24, 17, 
  10, 3, 32, 25, 18, 11, 4, 40, 
  33, 26, 19, 12, 5, 48, 41, 34, 
  27, 20, 13, 6, 56, 49, 42, 35, 
  28, 21, 14, 7, 57, 50, 43, 36, 
  29, 22, 15, 58, 51, 44, 37, 30, 
  23, 59, 52, 45, 38, 31, 60, 53, 
  46, 39, 61, 54, 47, 62, 55, 63
};

const VO_U32 g_sigLastScan8x8[ 3 ][ 4 ] =
{
  {0, 2, 1, 3},
  {0, 1, 2, 3},
  {0, 2, 1, 3},
};

const VO_U32 g_sigLastScan16x16[ 3 ][ 16 ] =
{
  {0, 4, 1, 8, 5, 2, 12, 9, 6, 3, 13, 10, 7, 14, 11, 15},
  {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15},
  {0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15},
};

static const VO_S32 g_scalingListSize   [4] = {16,64,64,64};
static const VO_S32 g_scalingListNum[SCALING_LIST_SIZE_NUM]={6,6,6,2};
static const VO_U32 g_ScalingFactorSize  [4] = { 4, 8, 16,  32};
static const VO_S16 g_invQuantScales[6] =
{
    40,45,51,57,64,72
};

//TComScalingList
#if !FLAT_4x4_DSL
static VO_S32 g_quantIntraDefault4x4[16] =
{
    16,16,17,21,
    16,17,20,25,
    17,20,30,41,
    21,25,41,70
};
static VO_S32 g_quantInterDefault4x4[16] =
{
    16,16,17,21,
    16,17,21,24,
    17,21,24,36,
    21,24,36,57
};
#endif
static const VO_S32 g_quantTSDefault4x4[16] =
{
    16,16,16,16,
    16,16,16,16,
    16,16,16,16,
    16,16,16,16
};

static const VO_S32 g_quantIntraDefault8x8[64] =
{
    16,16,16,16,17,18,21,24,
    16,16,16,16,17,19,22,25,
    16,16,17,18,20,22,25,29,
    16,16,18,21,24,27,31,36,
    17,17,20,24,30,35,41,47,
    18,19,22,27,35,44,54,65,
    21,22,25,31,41,54,70,88,
    24,25,29,36,47,65,88,115
};

static const VO_S32 g_quantInterDefault8x8[64] =
{
    16,16,16,16,17,18,20,24,
    16,16,16,17,18,20,24,25,
    16,16,17,18,20,24,25,28,
    16,17,18,20,24,25,28,33,
    17,18,20,24,25,28,33,41,
    18,20,24,25,28,33,41,54,
    20,24,25,28,33,41,54,71,
    24,25,28,33,41,54,71,91
};
/** initialization process of scaling list array
 */
VO_S32 alloc_mMulLevelScale(H265_DEC_SLICE * p_slice)
{
  VO_S32 sizeId, matrixId, qp;
  for(sizeId = 0; sizeId < SCALING_LIST_SIZE_NUM; sizeId++)
  {
    for(matrixId = 0; matrixId < g_scalingListNum[sizeId]; matrixId++)
    {
      for(qp = 0; qp < SCALING_LIST_REM_NUM; qp++)
      {    
          //p_slice->MMulLevelScale [sizeId][matrixId][qp] = (VO_S16 *)malloc(sizeof(VO_S16) * g_ScalingFactorSize[sizeId] * g_ScalingFactorSize[sizeId]);
          p_slice->MMulLevelScale [sizeId][matrixId][qp] = (VO_S16 *)AlignMalloc( 
              p_slice->p_user_op_all->p_user_op, 
              p_slice->p_user_op_all->codec_id,
              sizeof(VO_S16) * g_ScalingFactorSize[sizeId] * g_ScalingFactorSize[sizeId],
              CACHE_LINE );
		  if (p_slice->MMulLevelScale [sizeId][matrixId][qp] == NULL)
		  {
			  return VO_ERR_OUTOF_MEMORY;
		  }
      }
    }
  }
  // alias list [3] as [1], must do it.
  //so get matrixId is easy, just  (p_slice->CuPredMode == MODE_INTRA ? 0 : 3) + cIdx;
  for(qp = 0; qp < SCALING_LIST_REM_NUM; qp++)
  {
    p_slice->MMulLevelScale [SCALING_LIST_32x32][3][qp] = p_slice->MMulLevelScale [SCALING_LIST_32x32][1][qp];
  }

  return VO_ERR_NONE;
}
/** destroy quantization matrix array
 */
VO_VOID free_mMulLevelScale(H265_DEC_SLICE * p_slice)
{
  VO_S32 sizeId, matrixId, qp;
  for( sizeId = 0; sizeId < SCALING_LIST_SIZE_NUM; sizeId++)
  {
    for( matrixId = 0; matrixId < g_scalingListNum[sizeId]; matrixId++)
    {
      for( qp = 0; qp < SCALING_LIST_REM_NUM; qp++)
      {
        if(p_slice->MMulLevelScale [sizeId][matrixId][qp]) 
        {
//             free(p_slice->MMulLevelScale [sizeId][matrixId][qp]);
//             p_slice->MMulLevelScale [sizeId][matrixId][qp] = NULL;
            AlignFree(p_slice->p_user_op_all->p_user_op,
                p_slice->p_user_op_all->codec_id,
                p_slice->MMulLevelScale [sizeId][matrixId][qp]);         
        }
      }
    }
  }
}

//based on 7.4.5, however, multiply levelScale
//so when doing 8.6.3, one multiplication is ok
VO_VOID xSetMMulLevelScale(H265_DEC_SLICE * p_slice, stScalingList * pstScalingListEntity, VO_U32 sizeId, VO_U32 matrixId, VO_U32 qp)
{
    VO_U32 width = g_ScalingFactorSize[sizeId];
    VO_U32 height = g_ScalingFactorSize[sizeId];    
    VO_S16 *mMulLevelScale = p_slice->MMulLevelScale[sizeId][matrixId][qp];
    VO_S32 *ScalingList = pstScalingListEntity->ScalingList[sizeId][matrixId];   
    VO_S16 levelScale = g_invQuantScales[qp];
    VO_U32 i,j;
    VO_U32 scalingListSize = MIN(MAX_MATRIX_SIZE_NUM,(VO_S32)g_ScalingFactorSize[sizeId]);
    VO_U32 ratio = g_ScalingFactorSize[sizeId]/scalingListSize;

    for( j=0;j<height;j++)
    {
      for( i=0;i<width;i++)
      {
        mMulLevelScale[j*width + i] = (VO_S16)(levelScale * ScalingList[scalingListSize * (j / ratio) + i / ratio]);
      }
    }
    if(ratio > 1)
    {
      mMulLevelScale[0] = (VO_S16)(levelScale * pstScalingListEntity->m_scalingListDC[sizeId][matrixId]);
    }
}
/** set quantized matrix coefficient for decode
 */
VO_VOID setMMulLevelScale(H265_DEC_SLICE * p_slice, stScalingList * pstScalingListEntity)
{
  VO_S32 sizeId,matrixId;
  VO_U32 qp;

  for(sizeId=0;sizeId<SCALING_LIST_SIZE_NUM;sizeId++)
  {
    for(matrixId = 0; matrixId < g_scalingListNum[sizeId]; matrixId++)
    {
      for(qp=0;qp<SCALING_LIST_REM_NUM;qp++)
      {
        xSetMMulLevelScale(p_slice,pstScalingListEntity,sizeId,matrixId,qp);
      }
    }
  }
}

/** get default address of quantization matrix 
 * \param sizeId size index
 * \param matrixId list index
 * \returns pointer of quantization matrix
 */
const VO_S32* getDefaultScalingList(VO_U32 sizeId, VO_U32  matrixId)
{
  const VO_S32 *src = 0;
  switch(sizeId)
  {
    case SCALING_LIST_4x4:
      src = g_quantTSDefault4x4;
      break;
    case SCALING_LIST_8x8:
      src = (matrixId<3) ? g_quantIntraDefault8x8 : g_quantInterDefault8x8;
      break;
    case SCALING_LIST_16x16:
      src = (matrixId<3) ? g_quantIntraDefault8x8 : g_quantInterDefault8x8;
      break;
    case SCALING_LIST_32x32:
      src = (matrixId<1) ? g_quantIntraDefault8x8 : g_quantInterDefault8x8;
      break;
    default:
      assert(0);
      src = NULL;
      break;
  }
  return src;
}



// VO_VOID alloc_ScalingList(H265_DEC_SLICE * p_slice, stScalingList *pstScalingListEntity )
// {
//     VO_S32 sizeId;
//     VO_S32 matrixId;
// 
//     for ( sizeId = 0; sizeId < SCALING_LIST_SIZE_NUM; sizeId++ ) {
//         for ( matrixId = 0; matrixId < g_scalingListNum[ sizeId ]; matrixId++ ) {
//             if ( pstScalingListEntity->ScalingList[ sizeId ][ matrixId ] != NULL ) {
//                 //free(pstScalingListEntity->ScalingList[sizeId][matrixId]);
//                 AlignFree(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id,
//                     pstScalingListEntity->ScalingList[sizeId][matrixId]);
//             }  
//             //pstScalingListEntity->ScalingList[sizeId][matrixId] = (VO_S32 *) malloc(sizeof(VO_S32) *g_scalingListSize[sizeId]);
//             pstScalingListEntity->ScalingList[sizeId][matrixId] = (VO_S32 *) AlignMalloc(
//                 p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id,
//                 sizeof(VO_S32) *g_scalingListSize[sizeId],
//                 CACHE_LINE);
//         }
//     }  
// }
VO_S32 alloc_ScalingList(H265_DEC* const p_dec, stScalingList *pstScalingListEntity )
{
    VO_S32 sizeId;
    VO_S32 matrixId;

    for ( sizeId = 0; sizeId < SCALING_LIST_SIZE_NUM; sizeId++ ) {
        for ( matrixId = 0; matrixId < g_scalingListNum[ sizeId ]; matrixId++ ) {
            if ( pstScalingListEntity->ScalingList[ sizeId ][ matrixId ] != NULL ) {
                //free(pstScalingListEntity->ScalingList[sizeId][matrixId]);
                AlignFree(p_dec->user_op_all.p_user_op,
                    p_dec->user_op_all.codec_id,
                    pstScalingListEntity->ScalingList[sizeId][matrixId]);
            }  
            //pstScalingListEntity->ScalingList[sizeId][matrixId] = (VO_S32 *) malloc(sizeof(VO_S32) *g_scalingListSize[sizeId]);
            pstScalingListEntity->ScalingList[sizeId][matrixId] = (VO_S32 *) AlignMalloc(
                p_dec->user_op_all.p_user_op,
                p_dec->user_op_all.codec_id,
                sizeof(VO_S32) *g_scalingListSize[sizeId],
                CACHE_LINE);
			if (pstScalingListEntity->ScalingList[sizeId][matrixId] == NULL)
			{
				return VO_ERR_OUTOF_MEMORY;
        }
    }  
}
	return VO_ERR_NONE;
}

/** destroy quantization matrix array
 */
//TComScalingList.destroy
VO_VOID free_ScalingList(H265_DEC *p_dec,stScalingList *pstScalingListEntity)
{
  VO_S32 sizeId;
  VO_S32 matrixId;
  for(sizeId = 0; sizeId < SCALING_LIST_SIZE_NUM; sizeId++)
  {
    for(matrixId = 0; matrixId < g_scalingListNum[sizeId]; matrixId++)
    {
      if(pstScalingListEntity->ScalingList[sizeId][matrixId])
      {
//           free(pstScalingListEntity->ScalingList[sizeId][matrixId]);
//           pstScalingListEntity->ScalingList[sizeId][matrixId] = NULL;
          AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, pstScalingListEntity->ScalingList[sizeId][matrixId]);
		  pstScalingListEntity->ScalingList[sizeId][matrixId] = NULL;
      }
    }
  }
}

/** decode quantization matrix
* \param scalingList quantization matrix information
*/
//VO_VOID TDecCavlc::parseScalingList(TComScalingList* scalingList)
VO_S32 scaling_list_data(stScalingList *pstScalingListEntity,BIT_STREAM *p_bs)
{
    VO_U32  scaling_list_pred_mode_flag, scaling_list_pred_matrix_id_delta, i;
    VO_U32 sizeId, matrixId, refMatrixId;   
    VO_S32 scaling_list_delta_coef;
    VO_S32 nextCoef, coefNum;
    VO_S32 scalingListDcCoefMinus8;
    VO_S32 * ptr;

    //for each size
    for(sizeId = 0; sizeId < SCALING_LIST_SIZE_NUM; sizeId++)
    {
        for(matrixId = 0; matrixId <  g_scalingListNum[sizeId]; matrixId++)
        {
            
            scaling_list_pred_mode_flag = READ_FLAG( p_bs, "scaling_list_pred_mode_flag");
            if(!scaling_list_pred_mode_flag) //Copy Mode
            {    
                //scaling_list_pred_matrix_id_delta equals to 0, the scaling list is inferred from the default scaling list 
                scaling_list_pred_matrix_id_delta = READ_UEV(p_bs , "scaling_list_pred_matrix_id_delta"); 
                if(matrixId<scaling_list_pred_matrix_id_delta)
                  VOH265ERROR(VO_H265_ERR_SCALINGLISTERROR);
                pstScalingListEntity->scaling_list_pred_matrix_id[sizeId][matrixId] = refMatrixId = matrixId-scaling_list_pred_matrix_id_delta;
                if( sizeId > SCALING_LIST_8x8 )
                {                    
                    pstScalingListEntity->m_scalingListDC[sizeId][matrixId] = (matrixId == refMatrixId)? 16 :
                        pstScalingListEntity->m_scalingListDC[sizeId][refMatrixId];
                }
                if(matrixId == refMatrixId){
                    ptr = (VO_S32 *)getDefaultScalingList(sizeId, matrixId);
                } else {
                    ptr = pstScalingListEntity->ScalingList[sizeId][refMatrixId];
                }
                memcpy(pstScalingListEntity->ScalingList[sizeId][matrixId],ptr,
                  sizeof(VO_S32)*g_scalingListSize[sizeId]);
            }
            else //DPCM Mode
            {
              const VO_U32* scan  = (sizeId == 0) ? g_sigLastScan16x16[SCAN_DIAG] :  g_sigLastScanCG32x32;
              VO_S32 *dst = pstScalingListEntity->ScalingList[sizeId][matrixId];
              nextCoef = SCALING_LIST_START_VALUE;
              coefNum = MIN(MAX_MATRIX_COEF_NUM,(VO_S32)g_scalingListSize[sizeId]); 
              if( sizeId > SCALING_LIST_8x8 ) {
                scalingListDcCoefMinus8 = READ_SEV( p_bs, "scaling_list_dc_coef_minus8"); 
                pstScalingListEntity->m_scalingListDC[sizeId][matrixId] = nextCoef = scalingListDcCoefMinus8 + 8;
              }
              for( i = 0; i < coefNum; i++) {
                scaling_list_delta_coef = READ_SEV(p_bs , "scaling_list_delta_coef");
                nextCoef = (nextCoef + scaling_list_delta_coef + 256 ) % 256;
                dst[scan[i]] = nextCoef;
              }
            }
        }
    }

    return VO_ERR_NONE;
}

/** set ScalingList as default value, Table 7-5
*/
VO_VOID setDefaultScalingList(stScalingList *pstScalingListEntity)
{
     VO_S32 sizeId, matrixId;
    for( sizeId = 0; sizeId < SCALING_LIST_SIZE_NUM; sizeId++)
    {
        for(matrixId=0;matrixId<g_scalingListNum[sizeId];matrixId++)
        {
           
            memcpy(pstScalingListEntity->ScalingList[sizeId][matrixId],
              getDefaultScalingList(sizeId,matrixId),sizeof(VO_S32)*g_scalingListSize[sizeId]);

            pstScalingListEntity->m_scalingListDC[sizeId][matrixId] = SCALING_LIST_DC;
        }
    }
}