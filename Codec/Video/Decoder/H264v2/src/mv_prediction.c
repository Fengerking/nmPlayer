#include "global.h"
#include "mbuffer.h"
#include "ifunctions.h"



#define AVC_MID3(a,b,c,p)\
{\
  (p) = ((a)-(b))&(((a)-(b))>>31);\
  (a) -= (p);\
  (b) += (p);\
  (b) -= ((b)-(c))&(((b)-(c))>>31);\
  (b) += ((a)-(b))&(((a)-(b))>>31);\
  (p) = (b);\
}\

static int get_topright_mv(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, VO_S16 **C, VO_S32 i, VO_S32 list, VO_S32 cache_width)
{
  int topright_ref= pDecLocal->ref_cache[list][ i - 8 + cache_width ];
  if(pDecLocal->p_Slice->mb_aff_frame_flag)
  {
    if(topright_ref == -2&& i >= 20 && (i&7)==4&& pDecLocal->ref_cache[list][11] != -2)
	{
      VO_S16 *mv;
      pDecLocal->mv_cache[list][10] = 0;
      *C = (VO_S16*)&pDecLocal->mv_cache[list][10];

      if(!pDecLocal->mb_field&& IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[pDecLocal->left_mb_xy[0]]))
	  {
        //SET_DIAG_MV(*2, >>1, h->left_mb_xy[0]+s->mb_stride, (s->mb_y&1)*2+(i>>5));
		VO_S32 xy = pDecLocal->left_mb_xy[0]+pDecGlobal->PicWidthInMbs;
		VO_S32 y4 = (pDecLocal->mb.y&1)*2+(i>>5);
		if(IS_INTRA0(pDecGlobal->mb_type0_frame_buffer[xy+(y4>>2)*pDecGlobal->PicWidthInMbs]))
		  return -1;
		mv = (VO_S16*)&pDecGlobal->dec_picture->mv[list][xy*16+3 + y4*4];
		(*C)[0] = mv[0];
		(*C)[1] = mv[1]*2;
		return pDecGlobal->dec_picture->ref_idx[list][xy*16+3 + y4*4]>>1;
      }
      if(pDecLocal->mb_field&& !IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[pDecLocal->left_mb_xy[0]]))
	  {
        //SET_DIAG_MV(/2, <<1, h->left_mb_xy[i>=36], ((i>>2))&3);
		VO_S32 xy = pDecLocal->left_mb_xy[i>=36];
		VO_S32 y4 = ((i>>2))&3;
		if(IS_INTRA0(pDecGlobal->mb_type0_frame_buffer[xy+(y4>>2)*pDecGlobal->PicWidthInMbs]))
		  return -1;
		mv = (VO_S16*)&pDecGlobal->dec_picture->mv[list][xy*16+3 + y4*4];
		(*C)[0] = mv[0];
		(*C)[1] = mv[1]/2;
		return pDecGlobal->dec_picture->ref_idx[list][xy*16+3 + y4*4]<<1;
      }
    }
  }

  if(topright_ref != -2)
  {
    *C = (VO_S16*)&pDecLocal->mv_cache[list][ i - 8 + cache_width ];
    return topright_ref;
  }
  else
  {
    *C = (VO_S16*)&pDecLocal->mv_cache[list][ i - 8 - 1 ];
    return pDecLocal->ref_cache[list][ i - 8 - 1 ];
  }
}


void GetMotionVectorPredictorIn(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int n, int cache_width, short *pmv,short  ref_frame,int list)
{
  int top_ref=      pDecLocal->ref_cache[list][ n - 8 ];
  int left_ref=     pDecLocal->ref_cache[list][ n - 1 ];
  VO_S16 * A = (VO_S16*)&pDecLocal->mv_cache[list][ n - 1 ];
  VO_S16 * B = (VO_S16*)&pDecLocal->mv_cache[list][ n - 8 ];
  VO_S16 * C;
  int in_ref, match_count;

  in_ref= get_topright_mv(pDecGlobal,pDecLocal, &C, n, list, cache_width);
  match_count= (in_ref==ref_frame) + (top_ref==ref_frame) + (left_ref==ref_frame);
  if(match_count > 1)
  { 
    pmv[0] = imedian(A[0], B[0], C[0]);
    pmv[1] = imedian(A[1], B[1], C[1]);
  }
  else if(match_count==1)
  {
    if(left_ref==ref_frame)
	{
      pmv[0] = A[0];
      pmv[1] = A[1];
    }
	else if(top_ref==ref_frame)
	{
      pmv[0] = B[0];
      pmv[1] = B[1];
    }
	else
	{
      pmv[0] = C[0];
      pmv[1] = C[1];
    }
  }
  else
  {
    if(top_ref == -2 && in_ref == -2 && left_ref != -2)
    {
      pmv[0] = A[0];
      pmv[1] = A[1];
    }
	else
	{
      pmv[0] = imedian(A[0], B[0], C[0]);
      pmv[1] = imedian(A[1], B[1], C[1]);
    }
  }   
}


void GetMotionVectorPredictor16x8(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int n, int list, int ref, short *pmv)
{
  if(n==12)
  {
    int top_ref = pDecLocal->ref_cache[list][4];
    VO_S16 *B = (VO_S16*)&pDecLocal->mv_cache[list][4];

    if(top_ref == ref)
	{
      pmv[0]= B[0];
      pmv[1]= B[1];
      return;
    }
  }
  else
  {
    int left_ref = pDecLocal->ref_cache[list][27];
    VO_S16 *A = (VO_S16*)&pDecLocal->mv_cache[list][27];

    if(left_ref == ref)
	{
      pmv[0]= A[0];
      pmv[1]= A[1];
      return;
    }
  }

  GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,n, 4, pmv, ref,list);
}

void GetMotionVectorPredictor8x16(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,int n, int list, int ref, short *pmv)
{
  if(n==12)
  {
    int left_ref = pDecLocal->ref_cache[list][11];
    VO_S16 *A = (VO_S16*)&pDecLocal->mv_cache[list][11];

    if(left_ref == ref)
	{
      pmv[0]= A[0];
      pmv[1]= A[1];
      return;
    }
  }
  else
  {
    VO_S16 * C;
    VO_S32 in_ref= get_topright_mv(pDecGlobal,pDecLocal, &C, 14, list, 2);

    if(in_ref == ref)
	{
      pmv[0]= C[0];
      pmv[1]= C[1];
      return;
    }
  }

  GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,n, 2, pmv, ref,list);
}

void prepare_direct_params(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, MotionVector *pmvl0, MotionVector *pmvl1, VO_S8 *l0_rFrame, VO_S8 *l1_rFrame)
{
  char l0_refA, l0_refB, l0_refC;
  char l1_refA, l1_refB, l1_refC;
  VO_S8* ref_cache0 = &pDecLocal->ref_cache[0][0];
  VO_S8* ref_cache1 = &pDecLocal->ref_cache[1][0];
  int n = pDecLocal->mbAvailC ? 8 : 3;

  //if (!currSlice->mb_aff_frame_flag)
  {
    l0_refA  = ref_cache0[11];
    l1_refA  = ref_cache1[11];
    l0_refB = ref_cache0[4];
    l1_refB = ref_cache1[4];
    l0_refC = ref_cache0[n];
    l1_refC = ref_cache1[n];
  }

  *l0_rFrame = (VO_S8) imin(imin((unsigned char) l0_refA, (unsigned char) l0_refB), (unsigned char) l0_refC);
  *l1_rFrame = (VO_S8) imin(imin((unsigned char) l1_refA, (unsigned char) l1_refB), (unsigned char) l1_refC);

  if (*l0_rFrame >=0)
    //GetMotionVectorPredictor(pDecGlobal,pDecLocal, pmvl0,(VO_S16)*l0_rFrame,LIST_0);
    GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,12, 4, (VO_S16*)pmvl0,(VO_S16)*l0_rFrame,LIST_0);

  if (*l1_rFrame >=0)
    //GetMotionVectorPredictor(pDecGlobal,pDecLocal, pmvl1,(VO_S16)*l1_rFrame,LIST_1);
    GetMotionVectorPredictorIn(pDecGlobal,pDecLocal,12, 4, (VO_S16*)pmvl1,(VO_S16)*l1_rFrame,LIST_1);
}

