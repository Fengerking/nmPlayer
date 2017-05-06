/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010				*
*																		*
************************************************************************/
#define _ISOC99_SOURCE
#include <math.h>
#include "voH264EncGlobal.h"
#include "voH264EncME.h"
#include "voH264EncRC.h"
#include "voH264MC.h"
#if defined(VOARMV7)

extern void Hpel16x16SearchFilter_HV_ARMV7( VO_U8 *dsth, VO_U8 *dstv, VO_U8 *src,
                         VO_S32 stride );

#endif
static const int x_pos[8] = {-1,-2,-1,1,2,1,-1,-2};
static const int y_pos[8] = {-2,0,2,2,0,-2,-2,0};


#define MVD_COSTS( mx, my )\
  (mv_cost_x[(mx)<<2] + mv_cost_y[(my)<<2])
  
#define THREE_POINTS_COST( x0, x1, x2, y0, y1, y2, costs )\
{\
  (costs)[0] = SadBlock[block_type]( pCur, pSrc+(y0)*stride+(x0),FENC_STRIDE, stride,min_cost)\
			   + MVD_COSTS(best_my+y0,best_mx+x0);\
  (costs)[1] = SadBlock[block_type]( pCur, pSrc+(y1)*stride+(x1),FENC_STRIDE, stride,min_cost)\
			   + MVD_COSTS(best_my+y1,best_mx+x1);\
  (costs)[2] = SadBlock[block_type]( pCur, pSrc+(y2)*stride+(x2),FENC_STRIDE, stride,min_cost)\
			   + MVD_COSTS(best_my+y2,best_mx+x2);\
}

#define FOUR_POINTS_COST( x0, x1, x2, x3, y0, y1, y2, y3, costs )\
{\
  (costs)[0] = SadBlock[block_type]( pCur,  pSrc + (x0) + (y0)*stride,FENC_STRIDE, stride,min_cost )\
  			   + MVD_COSTS( best_mx+(x0), best_my+(y0) );\
  (costs)[1] = SadBlock[block_type]( pCur,  pSrc + (x1) + (y1)*stride,FENC_STRIDE, stride,min_cost )\
  			   + MVD_COSTS( best_mx+(x1), best_my+(y1) );\
  (costs)[2] = SadBlock[block_type]( pCur,  pSrc + (x2) + (y2)*stride,FENC_STRIDE, stride,min_cost )\
               + MVD_COSTS( best_mx+(x2), best_my+(y2) );\
  (costs)[3] = SadBlock[block_type]( pCur,  pSrc + (x3) + (y3)*stride,FENC_STRIDE, stride,min_cost )\
               + MVD_COSTS( best_mx+(x3), best_my+(y3) );\
}

#define PIXEL_SAD_X4( src_up,src_down,src_left,src_right,costs )\
{\
  (costs)[0] = SadBlock[block_type]( pCur,  (src_up),FENC_STRIDE, stride,min_cost );\
  (costs)[1] = SadBlock[block_type]( pCur,  (src_down),FENC_STRIDE, stride,min_cost );\
  (costs)[2] = SadBlock[block_type]( pCur,  (src_left),FENC_STRIDE, stride,min_cost );\
  (costs)[3] = SadBlock[block_type]( pCur,  (src_right),FENC_STRIDE, stride,min_cost );\
}


/* check if given vector is equal to any vector checked before */
static VOINLINE VO_S32 vector_repeats(VO_S16 (*mvc)[2], const VO_U32 i)
{
  VO_U32 j;
  for (j = 0; j < i; j++)
  {
	if (mvc[i][0] == mvc[j][0] &&  mvc[i][1] == mvc[j][1]) 
	  return 1; /* same vector has been checked already */
  }
  return 0;
}


#define MV_OUT_RANGE(mx,my) ( (mx < min_x) || (mx > max_x) || (my < min_y) || (my > max_y) )
#define EARLY_STOP(th,cost)\
{\
  if( (cost) < (th) )\
  	goto end_search;\
}

void MotionEstimation( H264ENC *pEncGlobal, VO_S16 (*mvc)[2], VO_S32 i_mvc ,H264ENC_L *pEncLocal)
{
  const VO_S32 block_type = BLOCK_16x16;
  VO_U8 *pCur = pEncGlobal->pCur[0];
  VO_U8 *pRef = pEncGlobal->pRef[0];
  const VO_S32 stride = pEncGlobal->i_stride[0];
  VO_S32 me_range = pEncGlobal->InternalParam.i_me_range;
  VO_S32 min_x = pEncLocal->min_MV_search[0];
  VO_S32 min_y = pEncLocal->min_MV_search[1];
  VO_S32 max_x = pEncLocal->max_MV_search[0];
  VO_S32 max_y = pEncLocal->max_MV_search[1];
  const VO_U16 *mv_cost_x = pEncLocal->pMVCost - pEncLocal->mvp[0];
  const VO_U16 *mv_cost_y = pEncLocal->pMVCost - pEncLocal->mvp[1];
  VO_S32 best_mx = AVSClip3( pEncLocal->mvp[0], min_x*4, max_x*4 );
  VO_S32 best_my = AVSClip3( pEncLocal->mvp[1], min_y*4, max_y*4 );
  VO_S32 pre_mx = ( best_mx + 2 ) >> 2;
  VO_S32 pre_my = ( best_my + 2 ) >> 2;
  VO_S32 min_cost = COST_MAX;
  VO_S32 tmp_mx,tmp_my;
  VO_S32 costs[8];
  VO_S32 i, pos;
  VO_S32 cur_cost;
  VO_U8 *pSrc;
  VO_S32 skip_th = pEncLocal->skip_th;
    
  //MVP not add mvd_cost
  min_cost = SadBlock[block_type]( pCur,&pRef[(pre_my)*stride+(pre_mx)], FENC_STRIDE, stride,min_cost);
  best_mx = pre_mx;
  best_my = pre_my;
  EARLY_STOP(skip_th,min_cost);
  //candidate mvs
  for( i = 0; i < i_mvc; i++ )
  {
	VO_S32 mv_x = (mvc[i][0] + 2) >> 2;
	VO_S32 mv_y = (mvc[i][1] + 2) >> 2;
	if(!(mv_x | mv_y) || !((mv_x-best_mx) | (mv_y-best_my)) || vector_repeats(mvc, i))
	{
	  continue;
	}
	else
	{
	  mv_x = AVSClip3( mv_x, min_x, max_x );
	  mv_y = AVSClip3( mv_y, min_y, max_y );
	  //COST_MV( mv_x, mv_y );
	  cur_cost = SadBlock[block_type]( pCur,&pRef[(mv_y)*stride+(mv_x)], FENC_STRIDE, stride,min_cost)+ MVD_COSTS(mv_x,mv_y);
	  MIN_SWAP3( min_cost, cur_cost, best_mx, mv_x, best_my, mv_y );
	}
  }
  //(0,0)pos
  cur_cost = SadBlock[block_type]( pCur,pRef, FENC_STRIDE, stride,min_cost)+ MVD_COSTS(0,0);
  MIN_SWAP3( min_cost, cur_cost, best_mx, 0, best_my, 0 );
  EARLY_STOP(skip_th,min_cost);
  //start search
  switch( pEncGlobal->me_method )
  {
    case VO_ME_DIA:
	  i = 0;
	  min_cost <<= 4;
	  do
	  {
	    pSrc = pRef + best_mx + best_my*stride;
	    FOUR_POINTS_COST( 0,0,-1,1,-1,1,0,0, costs );
	    MIN_SWAP1( min_cost, (costs[0]<<4)+1 );
	    MIN_SWAP1( min_cost, (costs[1]<<4)+3 );
	    MIN_SWAP1( min_cost, (costs[2]<<4)+4 );
	    MIN_SWAP1( min_cost, (costs[3]<<4)+12 );
	    if( !(min_cost&15) )
		  break;
	    best_mx -= (min_cost<<28)>>30;
	    best_my -= (min_cost<<30)>>30;
	    min_cost &= ~15;
	    if( MV_OUT_RANGE(best_mx, best_my) )
		  break;
	  } while( ++i < me_range );
	  min_cost >>= 4;
	  break;
	case VO_ME_HEX:
	  pos = 7;
	  // first hexagon 
	  pSrc = pRef + best_mx + best_my*stride;
	  THREE_POINTS_COST( -2,-1, 1, 0,  2, 2, costs   );
	  THREE_POINTS_COST(  2,1,  -1,0, -2,-2, costs+3 );
	  MIN_SWAP2( min_cost, costs[0], pos, 0 );
	  MIN_SWAP2( min_cost, costs[1], pos, 1 );
	  MIN_SWAP2( min_cost, costs[2], pos, 2 );
	  MIN_SWAP2( min_cost, costs[3], pos, 3 );
	  MIN_SWAP2( min_cost, costs[4], pos, 4 );
	  MIN_SWAP2( min_cost, costs[5], pos, 5 );
      EARLY_STOP(skip_th,min_cost);
	  if( pos != 7 )
	  {
	    best_mx += x_pos[pos+1];
	    best_my += y_pos[pos+1];
	    // only three points need to search
	    for ( i = 1;i < me_range/2 && (!MV_OUT_RANGE(best_mx, best_my));i++)
	    {
	      int pre_pos = (pos+6)%6;
		  pos = 7;
		  pSrc = pRef + best_mx + best_my*stride;
	      THREE_POINTS_COST( x_pos[pre_pos], x_pos[pre_pos+1],x_pos[pre_pos+2],y_pos[pre_pos],
	                         y_pos[pre_pos+1], y_pos[pre_pos+2],costs );      
	      MIN_SWAP2( min_cost, costs[0], pos, pre_pos-1 );
	      MIN_SWAP2( min_cost, costs[1], pos, pre_pos   );
	      MIN_SWAP2( min_cost, costs[2], pos, pre_pos+1 );
	      if( pos == 7 )
	        break;
	      best_mx += x_pos[pos+1];
	      best_my += y_pos[pos+1];
	    }
	  }
	  EARLY_STOP(skip_th,min_cost);
	  //Full search around
	  pSrc = pRef + best_mx + best_my*stride;
	  FOUR_POINTS_COST( 0,0,-1,1,-1,1,0,0, costs );
	  FOUR_POINTS_COST( -1,-1,1,1,-1,1,-1,1, costs+4 );
	  tmp_mx = best_mx;
	  tmp_my = best_my;
	  MIN_SWAP3( min_cost, costs[0], best_mx, tmp_mx,   best_my, tmp_my-1 );
      MIN_SWAP3( min_cost, costs[1], best_mx, tmp_mx,   best_my, tmp_my+1 );
      MIN_SWAP3( min_cost, costs[2], best_mx, tmp_mx-1, best_my, tmp_my );
      MIN_SWAP3( min_cost, costs[3], best_mx, tmp_mx+1, best_my, tmp_my );
	  MIN_SWAP3( min_cost, costs[4], best_mx, tmp_mx-1, best_my, tmp_my-1 );
      MIN_SWAP3( min_cost, costs[5], best_mx, tmp_mx-1, best_my, tmp_my+1 );
      MIN_SWAP3( min_cost, costs[6], best_mx, tmp_mx+1, best_my, tmp_my-1 );
      MIN_SWAP3( min_cost, costs[7], best_mx, tmp_mx+1, best_my, tmp_my+1 );
	  break;
	}
end_search:
	pEncLocal->mv[0] = (VO_S16)(best_mx << 2);
	pEncLocal->mv[1] = (VO_S16)(best_my << 2);
	pEncLocal->cost = min_cost;
    //compute the real cost 
    pEncLocal->cost_mv = mv_cost_x[ pEncLocal->mv[0] ] + mv_cost_y[ pEncLocal->mv[1] ];
    if( best_mx == pre_mx && best_my == pre_my  )
        pEncLocal->cost += pEncLocal->cost_mv;
	
	pEncLocal->nMBType = PL0;
	pEncLocal->nMBCost = pEncLocal->cost;
}


// Lambda = pow(2,qp/6-2) 
const VO_U8 LambdaTab[52] = 
{
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,  
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
  0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x04, 
  0x04, 0x04, 0x05, 0x06, 0x06, 0x07, 0x08, 0x09,
  0x0a, 0x0b, 0x0d, 0x0e, 0x0f, 0x12, 0x14, 0x17,
  0x19, 0x1d, 0x20, 0x24, 0x28, 0x2d, 0x33, 0x39,
  0x40, 0x48, 0x51, 0x5b  
};

// lambda2 = pow(Lambda,2) * 0.9 * 256 
const VO_S32 Lambda2Tab[52] = {
  0x00000E, 0x000012, 0x000016, 0x00001C, 0x000024, 0x00002d, 0x000039, 0x000048, 
  0x00005B, 0x000073, 0x000091, 0x0000B6, 0x0000E6, 0x000122, 0x00016D, 0x0001CC, 
  0x000244, 0x0002DB, 0x000399, 0x000489, 0x0005B6, 0x000733, 0x000912, 0x000B6D, 
  0x000E66, 0x001224, 0x0016DB, 0x001CCC, 0x002449, 0x002DB7, 0x003999, 0x004892, 
  0x005B6F, 0x007333, 0x009124, 0x00B6DE, 0x00E666, 0x012249, 0x016DBC, 0x01CCCC, 
  0x024492, 0x02DB79, 0x039999, 0x048924, 0x05B6F2, 0x073333, 0x091249, 0x0B6DE5, 
  0x0E6666, 0x122492, 0x16DBCB, 0x1ccccc                                  
};





static void MBEstimationEnd( H264ENC *pEncGlobal, H264ENC_L *pEncLocal);



VO_S32 InitMVCost( H264ENC *pEncGlobal, VO_S32 qp )
{
    VO_S32 i;
    VO_S32 nLambda = LambdaTab[qp];
    if( pEncGlobal->cost_mv[nLambda] )
        return 0;

	//YU_TBD mv_range
    /* factor of 4 from qpel, 2 from sign, and 2 because mv can be opposite from mvp */
    //CHECKED_MALLOC( pEncGlobal->cost_mv[nLambda], (4*4*2048 + 1) * sizeof(VO_U16) );
    MEMORY_ALLOC( pEncGlobal->cost_mv[nLambda], pEncGlobal->buffer_total , (4*4*2048 + 1 + 1) * sizeof(VO_U16), pEncGlobal->buffer_used,VO_U16*);
    pEncGlobal->cost_mv[nLambda] += 2*4*2048;

    for( i = 0; i <= 2*4*2048; i++ ){
        pEncGlobal->cost_mv[nLambda][-i] =
        pEncGlobal->cost_mv[nLambda][i]  = (VO_U16)(nLambda * (log2f(i+1)*2 + 0.718f + !!i) + .5f);
    }


    return 0;
}



static void MBEstimationStart( H264ENC *pEncGlobal, H264ENC_L *pEncLocal)
{
  VO_S32 qp = pEncLocal->nQP;

  pEncLocal->nMBCost = COST_MAX;
  pEncLocal->nLambda = LambdaTab[qp];
  pEncLocal->nChromaQP = pEncGlobal->pChromaQP[qp];
 
  if( pEncGlobal->pic_type  == P_PIC_TYPE )
  {
	const VO_S32 search_range = (1 <<6);

	VO_S32 high = search_range - 1;
	VO_S32 low = -search_range;

	VO_S32 k;

	//YU_TBD check the mv range

	pEncLocal->min_MV[0] = 4*( -16*pEncLocal->nMBX - 24 );
	pEncLocal->max_MV[0] = 4*( 16*( pEncGlobal->mb_width - pEncLocal->nMBX - 1 ) + 24 );
	pEncLocal->min_MV[1] = 4*( -16*pEncLocal->nMBY - 24 );
	pEncLocal->max_MV[1] = 4*( 16*(pEncGlobal->mb_height - pEncLocal->nMBY - 1 ) + 24 );

	k = (VO_S32)(pEncGlobal->i_width[0] - (pEncLocal->nMBX<<4));
	pEncLocal->max_MV_search[0] = AVC_MIN(high, k);
	k = (VO_S32)(pEncGlobal->height[0] -  (pEncLocal->nMBY<<4));
	pEncLocal->max_MV_search[1] = AVC_MIN(high, k);

	k = (-(VO_S32)((pEncLocal->nMBX+1)<<4));
	pEncLocal->min_MV_search[0] = AVC_MAX(low, k);
	k = (-(VO_S32)((pEncLocal->nMBY+1)<<4));
	pEncLocal->min_MV_search[1] = AVC_MAX(low, k);
	pEncLocal->pMVCost = pEncGlobal->cost_mv[pEncLocal->nLambda];
    pEncGlobal->mb.b_skip_mc = 0;
  }
}

/* Prediction modes allowed for various combinations of neighbors. */
/* Terminated by a -1. */
/* In order, no neighbors, left, top, top/left, top/left/topleft */
static const VO_S8 PreditcIntraLuma16x16[5][5] =
{
  {INTRA_LUMA_DC_128, -1, -1, -1, -1},
  {INTRA_LUMA_DC_LEFT, INTRA_LUMA_H, -1, -1, -1},
  {INTRA_LUMA_DC_TOP, INTRA_LUMA_V, -1, -1, -1},
  {INTRA_LUMA_V, INTRA_LUMA_H, INTRA_LUMA_DC, -1, -1},
  {INTRA_LUMA_V, INTRA_LUMA_H, INTRA_LUMA_DC, INTRA_LUMA_P, -1},
};

static const VO_S8 PreditcIntraChorma8x8[5][5] =
{
  {INTRA_CHROMA_DC_128, -1, -1, -1, -1},
  {INTRA_CHROMA_DC_LEFT, INTRA_CHROMA_H, -1, -1, -1},
  {INTRA_CHROMA_DC_TOP, INTRA_CHROMA_V, -1, -1, -1},
  {INTRA_CHROMA_V, INTRA_CHROMA_H, INTRA_CHROMA_DC, -1, -1},
  {INTRA_CHROMA_V, INTRA_CHROMA_H, INTRA_CHROMA_DC, INTRA_CHROMA_P, -1},
};


static VOINLINE const VO_S8 *IntraLuma16x16ModeAvail( VO_S32 nAvailNeigh )
{
  VO_S32 idx = nAvailNeigh & (MB_TOP|MB_LEFT|MB_TOPLEFT);
  return PreditcIntraLuma16x16[(idx&MB_TOPLEFT)?4:idx];
}

static VOINLINE const VO_S8 *IntraChroma8x8ModeAvail( VO_S32 nAvailNeigh )
{
  VO_S32 idx = nAvailNeigh & (MB_TOP|MB_LEFT|MB_TOPLEFT);
  return PreditcIntraChorma8x8[(idx&MB_TOPLEFT)?4:idx];
}


static void MBEstimationIntraChroma( H264ENC *pEncGlobal, H264ENC_L *pEncLocal)
{
  const VO_S8 *mode;
  VO_S32 min_cost = COST_MAX;

  mode = IntraChroma8x8ModeAvail( pEncLocal->nAvailNeigh );

  for( ; *mode >= 0; mode++ )
  {
	VO_S32 cur_cost;
	VO_S32 cur_mode = *mode;

	PredictIntraChroma8x8[cur_mode]( pEncGlobal->pDec[1], FDEC_STRIDE,  pEncGlobal->pDec[1], FDEC_STRIDE );
	PredictIntraChroma8x8[cur_mode]( pEncGlobal->pDec[2], FDEC_STRIDE,  pEncGlobal->pDec[2], FDEC_STRIDE );
	
	cur_cost = SadBlock[BLOCK_8x8]( pEncGlobal->pDec[1],  pEncGlobal->pCur[1],FDEC_STRIDE, FENC_STRIDE,min_cost) +
			   SadBlock[BLOCK_8x8]( pEncGlobal->pDec[2],  pEncGlobal->pCur[2],FDEC_STRIDE, FENC_STRIDE,min_cost ) +
			   pEncLocal->nLambda * intra_chroma_mode_bits[cur_mode];

	MIN_SWAP2( min_cost, cur_cost, pEncLocal->nChormaMode, cur_mode );
  }
}

static void IntraMBPrediction( H264ENC *pEncGlobal, H264ENC_L *pEncLocal)
{
  VO_U8  *pSrc = pEncGlobal->pCur[0];
  VO_U8  *pDst = pEncGlobal->pDec[0];
  VO_S32 min_cost = pEncLocal->nMBCost;
  VO_S32 best_mode;
  // only support I_16x16 
  const VO_S8 *pAvailMode = IntraLuma16x16ModeAvail( pEncLocal->nAvailNeigh );

  for( ; *pAvailMode >= 0; pAvailMode++ )
  {  	
	VO_S32 cur_cost;
	VO_S32 cur_mode = *pAvailMode;
    //printf("cur_mode:%d\n",cur_mode);
	PredictIntraLuma16x16[cur_mode]( pDst, FDEC_STRIDE, pDst, FDEC_STRIDE );
    //printf("sad\n");
	cur_cost = SadBlock[BLOCK_16x16]( pDst,  pSrc,FDEC_STRIDE, FENC_STRIDE,min_cost ) +
			pEncLocal->nLambda * intra_luma_mode_bits[cur_mode];
	//printf("SWAP\n");
	MIN_SWAP2(min_cost, cur_cost, best_mode, cur_mode );
  }
  //printf("SWAP\n");
  MIN_SWAP3(pEncLocal->nMBCost, min_cost, pEncLocal->nMBType, I16x16, pEncLocal->nLumaMode, best_mode);
}
//add by Really Yang 1119
static void MBHpelFilter(H264ENC *pEncGlobal, H264ENC_L *pEncLocal)
{
  const VO_S32 stride = pEncGlobal->i_stride[0];

  VO_S32 offset = (pEncLocal->mv[1]>>2)*stride+(pEncLocal->mv[0]>>2);
#if defined(VOARMV7)
  Hpel16x16SearchFilter_HV_ARMV7(pEncLocal->pix[0],pEncLocal->pix[1],
		                         pEncGlobal->pRef[0]+offset,stride);
#else
  Hpel16x16SearchFilter_HV_C(pEncLocal->pix[0],pEncLocal->pix[1],
		                     pEncGlobal->pRef[0]+offset,stride);
#endif
}

static void SubpelRefine(H264ENC *pEncGlobal, H264ENC_L *pEncLocal)
{
  const VO_S32 block_type = BLOCK_16x16;

  VO_S32 best_mx = pEncLocal->mv[0];
  VO_S32 best_my = pEncLocal->mv[1];
  VO_S32 mv_x = best_mx; 
  VO_S32 mv_y = best_my;
  VO_S32 min_cost = pEncLocal->cost;
  VO_U8 *pCur = pEncGlobal->pCur[0];

  const VO_U16 *p_cost_mvx = pEncLocal->pMVCost - pEncLocal->mv[0];
  const VO_U16 *p_cost_mvy = pEncLocal->pMVCost - pEncLocal->mv[1];
  
  VO_S32 costs[4];
  VO_S32 stride = 32;
  VO_U8 *src_top, *src_down, *src_left, *src_right;

  MBHpelFilter(pEncGlobal,pEncLocal);

  src_top = pEncGlobal->pRef[4];src_down = src_top + stride;
  src_left = pEncGlobal->pRef[3];src_right = src_left + 1;
  PIXEL_SAD_X4(src_top,src_down,src_left,src_right,costs);
  MIN_SWAP2( min_cost, costs[0] + p_cost_mvx[mv_x  ] + p_cost_mvy[mv_y-2], best_my, mv_y-2 );
  MIN_SWAP2( min_cost, costs[1] + p_cost_mvx[mv_x  ] + p_cost_mvy[mv_y+2], best_my, mv_y+2 );
  MIN_SWAP3( min_cost, costs[2] + p_cost_mvx[mv_x-2] + p_cost_mvy[mv_y  ], best_mx, mv_x-2, best_my, mv_y );
  MIN_SWAP3( min_cost, costs[3] + p_cost_mvx[mv_x+2] + p_cost_mvy[mv_y  ], best_mx, mv_x+2, best_my, mv_y );
	
  if( min_cost < pEncLocal->nMBCost )
  {
	pEncLocal->mv[0] = (VO_S16)best_mx;
	pEncLocal->mv[1] = (VO_S16)best_my;
	pEncLocal->nMBCost = min_cost;
  }
}
//end of add


static void MBEstimationInter( H264ENC *pEncGlobal, H264ENC_L *pEncLocal )
{
  VO_S32 nSkip = 0;
  VO_U32 flags = pEncGlobal->InternalParam.i_analyse_flags;

  /* check skip, YU_TBD*/

  if( pEncLocal->nLeftMBType == PSKIP || pEncLocal->nTopMBType == PSKIP ||
      pEncLocal->nTopLeftMBType == PSKIP || pEncLocal->nTopRightMBType == PSKIP )
    nSkip = PredictSkipMB( pEncGlobal, pEncLocal );

  if(nSkip)
  {
	pEncLocal->nMBType = PSKIP;
	pEncLocal->nMBCost = 0;
  }
  else
  {
	VO_S32 i_mvc;
	VO_ALIGNED_4( VO_S16 mvc[8][2] );	
	//predict_mv	
	GetPredictMV( pEncGlobal, pEncLocal->mvp );
	//candidate_mv
	GetCandidateMV(pEncGlobal, mvc, &i_mvc, pEncLocal );
	//integer mv
	MotionEstimation( pEncGlobal, mvc, i_mvc, pEncLocal );
	//sub_pixel	
	if((flags & VO_ANALYSE_HPEL) )
	{
	  pEncLocal->mvi[0] = pEncLocal->mv[0];
	  pEncLocal->mvi[1] = pEncLocal->mv[1];

	  if(pEncLocal->nMBCost > pEncGlobal->fastskip)
	    SubpelRefine(pEncGlobal, pEncLocal);
	}
	CP32( pEncGlobal->mb.mvp, pEncLocal->mvp );
  }

}

static void MBEstimationEnd( H264ENC *pEncGlobal, H264ENC_L *pEncLocal )
{

  switch( pEncLocal->nMBType )
  {
    case I16x16:
      MBEstimationIntraChroma( pEncGlobal, pEncLocal );
      break;
    case PL0:
	  MBUpdateRef( pEncGlobal);
	  MBUpdateMV( pEncGlobal, pEncLocal->mv);
	  break;
    case PSKIP:
      MBUpdateRef( pEncGlobal);
      MBUpdateMV( pEncGlobal,pEncGlobal->mb.pskip_mv );
      break;
    default: 
	  break;
 
    }
}


void MBEstimation( H264ENC *pEncGlobal, H264ENC_L *pEncLocal )
{
  MBEstimationStart( pEncGlobal, pEncLocal);

  if(pEncGlobal->pic_type == P_PIC_TYPE)
  {
	MBEstimationInter( pEncGlobal, pEncLocal);
	if(pEncLocal->nMBType != PSKIP  && pEncLocal->nMBCost > pEncGlobal->fastskip)
	  IntraMBPrediction( pEncGlobal, pEncLocal );
  }
  else
  {
    IntraMBPrediction( pEncGlobal, pEncLocal);
  }
  pEncGlobal->i_cost_est += (pEncLocal->nMBCost >> 2); 
  MBEstimationEnd( pEncGlobal, pEncLocal );
}




