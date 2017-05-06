
#include "block.h"
#include "global.h"
#include "mb_access.h"
//#include "transform8x8.h"
#include "transform.h"
#include "mc_prediction.h"
#include "intra8x8_pred.h"
#include "intra4x4_pred.h"
#include "intra16x16_pred.h"
#include "mv_prediction.h"
#include "mb_prediction.h"
#include "ifunctions.h"
extern int  get_colocated_info_8x8 (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *list1,int i1, int j1);
extern int  get_colocated_info_4x4 (StorablePicture *list1,int i1, int j1,H264DEC_L *pDecLocal);

static const byte decode_block_scan[16] = {0, 1, 4, 5, 2, 3, 6, 7, 8, 9, 12, 13, 10, 11, 14, 15};

int mb_pred_intra4x4(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture)
{
  Slice *currSlice = pDecLocal->p_Slice;
  int uv; 
  int ioff=0,joff=0;
  int block8x8;   // needed for ABT
//add by Really Yang 20110304 
  //VO_U8 *pSrc = currSlice->mb_pred[0];
  int dst_stride = pDecGlobal->iLumaStride<<pDecLocal->mb_field;
  VO_U8 *pDst = dec_picture->imgY+pDecLocal->pix_y*dst_stride+pDecLocal->pix_x;
  const VO_U8* nz = pDecGlobal->nz_coeff_frame + pDecLocal->mb_xy*24;
  VO_S16 *cof = pDecLocal->cof_yuv;
//#if USE_FRAME_THREAD
//  if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//  if (pDecGlobal->nThdNum > 1)
//#endif
//  {
//	cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//  }
  if(pDecLocal->mb_field)
  {
    pDst = dec_picture->imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride+pDecLocal->pix_x;
	//if(pDecLocal->mb.y&1)
	//  pDst -= 15*pDecGlobal->iLumaStride;
  }
//end of add
  for (block8x8 = 4; block8x8 != 0; block8x8--)
  {
    intrapred(pDecGlobal,pDecLocal, ioff,joff);
	if(*nz++)
		Idct4x4Add(cof, pDst, dst_stride );	
	else if(*cof)
	  Idct4x4DCAdd((*cof+32)>>6, pDst, dst_stride );
	cof += 16;

	intrapred(pDecGlobal,pDecLocal, ioff+4,joff);
	if(*nz++)
		Idct4x4Add(cof, pDst+4, dst_stride);
	else if(*cof)
	  Idct4x4DCAdd((*cof+32)>>6, pDst+4, dst_stride);
	cof += 16;

	intrapred(pDecGlobal,pDecLocal, ioff+8,joff);
	if(*nz++)
		Idct4x4Add(cof, pDst+8, dst_stride);
	else if(*cof)
	  Idct4x4DCAdd((*cof+32)>>6, pDst+8, dst_stride);
	cof += 16;

	intrapred(pDecGlobal,pDecLocal, ioff+12,joff);
	if(*nz++)
		Idct4x4Add(cof, pDst+12, dst_stride);
	else if(*cof)
	  Idct4x4DCAdd((*cof+32)>>6, pDst+12, dst_stride);
	cof += 16;
	joff += 4;
	pDst += dst_stride<<2;
  }

  pDecGlobal->intrapred_chroma(pDecGlobal,pDecLocal);// last argument is ignored, computes needed data for both uv channels

  dst_stride = pDecGlobal->iChromaStride<<pDecLocal->mb_field;
  for(uv = 0; uv < 2; uv++)
  {
	VO_U8 *pDst = dec_picture->imgUV[uv]+pDecLocal->pix_c_y*dst_stride+pDecLocal->pix_c_x;
	if(pDecLocal->mb_field)
    {
      pDst = dec_picture->imgUV[uv]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  //if(pDecLocal->mb.y&1)
	  //  pDst -= 7*pDecGlobal->iChromaStride;
    }
    if (pDecLocal->cbp >> 4 )
    {
		if(*nz++)
			Idct4x4Add(cof,pDst, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst, dst_stride );
		cof += 16;

		if(*nz++)
			Idct4x4Add(cof, pDst+4, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+4, dst_stride);
		cof += 16;

		if(*nz++)
			Idct4x4Add(cof, pDst+(dst_stride<<2), dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+(dst_stride<<2), dst_stride);
		cof += 16;

		if(*nz++)
			Idct4x4Add(cof, pDst+(dst_stride<<2)+4, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+(dst_stride<<2)+4, dst_stride);
		cof += 16;
    }
    //else
    //{      
	//  MCCopy8x8( pSrc, MB_BLOCK_SIZE, pDst, dst_stride);
    //}
  }

  //if (pDecLocal->cbp != 0)
  //  currSlice->is_reset_coeff = FALSE;
  return 1;
}

int mb_pred_intra8x8(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture)
{
  Slice *currSlice = pDecLocal->p_Slice;
	VO_S16 *cof = pDecLocal->cof_yuv;
  int uv;
  int dst_stride = pDecGlobal->iLumaStride<<pDecLocal->mb_field;
  VO_U8 *pDst = dec_picture->imgY+pDecLocal->pix_y*dst_stride+pDecLocal->pix_x;
  const VO_U8* nz = pDecGlobal->nz_coeff_frame + pDecLocal->mb_xy*24 + 16;
//#if USE_FRAME_THREAD
//  if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//  if (pDecGlobal->nThdNum > 1)
//#endif
//  {
//	  cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//  }
  if(pDecLocal->mb_field)
  {
    pDst = dec_picture->imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride+pDecLocal->pix_x;
	//if(pDecLocal->mb.y&1)
	//  pDst -= 15*pDecGlobal->iLumaStride;
  }

  intrapred8x8(pDecGlobal,pDecLocal, 0, 0);
  Idct8x8Add(cof, pDst, dst_stride );
  intrapred8x8(pDecGlobal,pDecLocal, 8, 0);
  Idct8x8Add(cof+64, pDst+8, dst_stride );
  intrapred8x8(pDecGlobal,pDecLocal, 0, 8);
  Idct8x8Add(cof+128, pDst+8*dst_stride, dst_stride );
  intrapred8x8(pDecGlobal,pDecLocal, 8, 8);
  Idct8x8Add(cof+192, pDst+8*dst_stride+8, dst_stride );

  cof += 256;
  pDecGlobal->intrapred_chroma(pDecGlobal,pDecLocal);
  dst_stride = pDecGlobal->iChromaStride<<pDecLocal->mb_field;
  for(uv = 0; uv < 2; uv++)
  {
	VO_U8 *pDst = dec_picture->imgUV[uv]+pDecLocal->pix_c_y*dst_stride+pDecLocal->pix_c_x;
	if(pDecLocal->mb_field)
    {
      pDst = dec_picture->imgUV[uv]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  //if(pDecLocal->mb.y&1)
	  //  pDst -= 7*pDecGlobal->iChromaStride;
    }
    if (pDecLocal->cbp >> 4 )
    {
		if(*nz++)
			Idct4x4Add(cof,pDst, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst, dst_stride );
		cof += 16;

		if(*nz++)
			Idct4x4Add(cof, pDst+4, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+4, dst_stride);
		cof += 16;

		if(*nz++)
			Idct4x4Add(cof, pDst+(dst_stride<<2), dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+(dst_stride<<2), dst_stride);
		cof += 16;

		if(*nz++)
			Idct4x4Add(cof, pDst+(dst_stride<<2)+4, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+(dst_stride<<2)+4, dst_stride);
		cof += 16;
    }
    //else
    //{      
	//  MCCopy8x8( pSrc, MB_BLOCK_SIZE, pDst, dst_stride);
    //}
  }
  //currSlice->is_reset_coeff = FALSE;
  return 1;
}

int mb_pred_intra16x16(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture)
{
  int uv;
  Slice *currSlice = pDecLocal->p_Slice;
  int jj;
  int dst_stride = pDecGlobal->iLumaStride<<pDecLocal->mb_field;
  VO_U8 *pDst = dec_picture->imgY+pDecLocal->pix_y*dst_stride+pDecLocal->pix_x;
  const VO_U8* nz = pDecGlobal->nz_coeff_frame + pDecLocal->mb_xy*24;
  VO_S16 *cof = pDecLocal->cof_yuv;
//#if USE_FRAME_THREAD
//  if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//  if (pDecGlobal->nThdNum > 1)
//#endif
//	{
//		cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//	}
  if(pDecLocal->mb_field)
  {
    pDst = dec_picture->imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride+pDecLocal->pix_x;
	//if(pDecLocal->mb.y&1)
	//  pDst -= 15*pDecGlobal->iLumaStride;
  }

  intrapred16x16(pDecGlobal,pDecLocal, pDecLocal->i16mode);
	//currMB->ipmode_DPCM = (char) currMB->i16mode; //For residual DPCM

  for (jj = 4; jj != 0; jj--)
  {
	  if(*nz++)
		  Idct4x4Add(cof, pDst, dst_stride );
	  else if(*cof)
		  Idct4x4DCAdd((*cof+32)>>6, pDst, dst_stride );
	  cof += 16;

	  if(*nz++)
		  Idct4x4Add(cof, pDst+4, dst_stride);
	  else if(*cof)
		  Idct4x4DCAdd((*cof+32)>>6, pDst+4, dst_stride);
	  cof += 16;

	  if(*nz++)
		  Idct4x4Add(cof, pDst+8, dst_stride);
	  else if(*cof)
		  Idct4x4DCAdd((*cof+32)>>6, pDst+8, dst_stride);
	  cof += 16;

	  if(*nz++)
		  Idct4x4Add(cof, pDst+12, dst_stride);
	  else if(*cof)
		  Idct4x4DCAdd((*cof+32)>>6, pDst+12, dst_stride);
	  cof += 16;

	  pDst += dst_stride<<2;
  }

  pDecGlobal->intrapred_chroma(pDecGlobal,pDecLocal);

  dst_stride = pDecGlobal->iChromaStride<<pDecLocal->mb_field;
  for(uv = 0; uv < 2; uv++)
  {
	VO_U8 *pDst = dec_picture->imgUV[uv]+pDecLocal->pix_c_y*dst_stride+pDecLocal->pix_c_x;
	if(pDecLocal->mb_field)
    {
      pDst = dec_picture->imgUV[uv]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  //if(pDecLocal->mb.y&1)
	  //  pDst -= 7*pDecGlobal->iChromaStride;
    }
    if (pDecLocal->cbp >> 4 )
    {
		if(*nz++)
			Idct4x4Add(cof,pDst, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst, dst_stride );
		cof += 16;

		if(*nz++)
			Idct4x4Add(cof, pDst+4, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+4, dst_stride);
		cof += 16;

		if(*nz++)
			Idct4x4Add(cof, pDst+(dst_stride<<2), dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+(dst_stride<<2), dst_stride);
		cof += 16;

		if(*nz++)
			Idct4x4Add(cof, pDst+(dst_stride<<2)+4, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+(dst_stride<<2)+4, dst_stride);
		cof += 16;
    }
  }

  return 1;
}




//edit by Really Yang 20110329
int mb_pred_p_inter8x8(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture)
{
  int block8x8;   // needed for ABT
  int i=0, j=0,k;  
  VO_S8 *b8mode = pDecLocal->b8mode;
  VO_S8 *b8pdir = pDecLocal->b8pdir;
  for (block8x8=0; block8x8<4; block8x8++)
  {
    int mv_mode  = b8mode[block8x8];
    int pred_dir = b8pdir[block8x8];

    int k_start = (block8x8 << 2);
    int k_inc = (mv_mode == SMB8x4) ? 2 : 1;
    int k_end = (mv_mode == SMB8x8) ? k_start + 1 : ((mv_mode == SMB4x4) ? k_start + 4 : k_start + k_inc + 1);


    for (k = k_start; k < k_end; k += k_inc)
    {
      i =  (decode_block_scan[k] & 3);
      j = ((decode_block_scan[k] >> 2) & 3);
      perform_mc(pDecGlobal,pDecLocal, pred_dir, i, j,mv_mode-1);
    }        
  }


  return 1;
}
VO_S32 mb_pred_b_temporal(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type)
{
  short ref_idx;
  int refList;
  StorablePicture* ref_pic;
  int i, j;
  int block8x8;   
  int stride_4x4 = 4;
  int stride_8x8 = 4;
  Slice *currSlice = pDecLocal->p_Slice;
  //int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
  StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0];
  StorablePicture **list1 = currSlice->listX[currSlice->slice_number][LIST_1];
  //StorablePicture** ref_pic_list0 = pDecGlobal->dec_picture->ref_pic[0]+pDecLocal->mb_xy*16;
  //StorablePicture** ref_pic_list1 = pDecGlobal->dec_picture->ref_pic[1]+pDecLocal->mb_xy*16;
  VO_S32* mv_cache0 = pDecGlobal->dec_picture->mv[0]+pDecLocal->mb_xy*16;
  VO_S32* mv_cache1 = pDecGlobal->dec_picture->mv[1]+pDecLocal->mb_xy*16;
  VO_S8* ref_cache0 = pDecGlobal->dec_picture->ref_idx[0]+pDecLocal->mb_xy*16;
  VO_S8* ref_cache1 = pDecGlobal->dec_picture->ref_idx[1]+pDecLocal->mb_xy*16;
  VO_S8 colocated_ref_list0,colocated_ref_list1;
  StorablePicture* colocated_ref_pic0,*colocated_ref_pic1;
  VO_S16 pmvl0[2], pmvl1[2];
  VO_S16* colocated_mv0,*colocated_mv1;
  VO_S32 colocated_type[2];
  VO_S32 is_8x8 = IS_8X8(*mb_type);
  VO_U32 sub_type;
  int cmv_x,cmv_y;
  VO_S32 mv0,mv1;
  VO_S32 ref0,ref1;
  VO_S32 block_xy = pDecLocal->mb_xy*16;
  VO_S32 mb_xy = pDecLocal->mb_xy;
  VO_S32 index_shift = 0;
  if(list0[0] == NULL||list1[0] == NULL)
  {
    return 0;
  }
#if USE_FRAME_THREAD
  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
  {
	  //wait for list1[0],mv=(?,?) progress ready
	  FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
	  const VO_S32 max_y = pDecLocal->mb.y*MB_BLOCK_SIZE + MB_BLOCK_SIZE-1;		//???interlace need fix
      //const VO_S32 max_y = 10000;
	  VO_S32 errFlag = WaitForProgress(pInfo, list1[0], max_y, 0);
      
	  if(errFlag)
	  {
		  //ref is error
		  pInfo->errFlag = errFlag;
		  return errFlag;
	  }
  }
#endif
                                         
      colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

      sub_type = VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT; 
      if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
	  {
         *mb_type   |= VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT;
      }
	  else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
	  {
         *mb_type   |= VO_L0L1|VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
      }
	  else
	  {
         if(!currSlice->active_sps->direct_8x8_inference_flag)
		 {
           sub_type = VO_8x8|VO_P0L0|VO_P0L1|VO_DIRECT; 
         }
         *mb_type   |= VO_8x8|VO_L0L1;
      }
  
 
  if(IS_16X16(*mb_type))
  {
    
	M32(&ref_cache1[0]) = M32(&ref_cache1[4]) = M32(&ref_cache1[8]) = M32(&ref_cache1[12]) = 0;
	//ref_pic_list1[0] = ref_pic_list1[1] = ref_pic_list1[2] = ref_pic_list1[3] = 
	//ref_pic_list1[4] = ref_pic_list1[5] = ref_pic_list1[6] = ref_pic_list1[7] = 
	//ref_pic_list1[8] = ref_pic_list1[9] = ref_pic_list1[10] = ref_pic_list1[11] = 
	//ref_pic_list1[12] = ref_pic_list1[13] = ref_pic_list1[14] = ref_pic_list1[15] = list1[0];

	if(IS_INTRA0(colocated_type[0]))
	{
      ref0=mv0=mv1=0;
    }
	else
	{
	  VO_S32 mapped_idx=0;
      VO_S32 iref;
	  VO_S32 col_indexR = mb_xy*16;	  
	  colocated_ref_list0 = list1[0]->ref_idx[0][col_indexR];
	  colocated_ref_list1 = list1[0]->ref_idx[1][col_indexR];
	  //colocated_ref_pic0 = list1[0]->ref_pic[0][col_indexR];
	  colocated_ref_pic0 = list1[0]->listX[currSlice->slice_number][0][colocated_ref_list0];
	  //colocated_ref_pic1 = list1[0]->ref_pic[1][col_indexR];
	  colocated_ref_pic1 = list1[0]->listX[currSlice->slice_number][1][colocated_ref_list1];
	  colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col_indexR];
	  colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col_indexR];
	  if(colocated_ref_list0 <= -1)
	  {
		ref_pic = colocated_ref_pic1;
		cmv_x = colocated_mv1[0];
		cmv_y = colocated_mv1[1];
	  }
	  else
	  {
		ref_pic = colocated_ref_pic0;
		cmv_x = colocated_mv0[0];
		cmv_y = colocated_mv0[1];		
	  }
	  if(ref_pic == NULL)
	  {
	    return 0;
	  }
	  for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[currSlice->slice_number][LIST_0]);iref++)
      {
        if(currSlice->listX[currSlice->slice_number][LIST_0][iref] == ref_pic)
        {
          mapped_idx = iref;            
          break;
        }
        else //! invalid index. Default to zero even though this case should not happen
        {
          mapped_idx=INVALIDINDEX;
        }
      }
	  if(INVALIDINDEX == mapped_idx)
      {
        mv0 = 0;
        mv1 = 0;
		ref0 = 0;
      }
      else
      {
        int mv_scale = currSlice->mvscale[LIST_0][mapped_idx];

        if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0][mapped_idx]->is_long_term)
        {
	      pmvl0[0] = cmv_x;
		  pmvl0[1] = cmv_y;
		  mv0 = M32(pmvl0);
          mv1 = 0;
        }
        else
        {
		  pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
		  pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
          pmvl1[0] = (short) (pmvl0[0] - cmv_x);
	      pmvl1[1] = (short) (pmvl0[1] - cmv_y);
		  mv0 = M32(pmvl0);
          mv1 = M32(pmvl1);
        }

		ref0 = (char) mapped_idx;
	  }
	}
	M32(&ref_cache0[0]) = M32(&ref_cache0[4]) = M32(&ref_cache0[8]) = M32(&ref_cache0[12]) = ref0*0x01010101;
	//ref_pic_list0[0] = ref_pic_list0[1] = ref_pic_list0[2] = ref_pic_list0[3] = 
	//ref_pic_list0[4] = ref_pic_list0[5] = ref_pic_list0[6] = ref_pic_list0[7] = 
	//ref_pic_list0[8] = ref_pic_list0[9] = ref_pic_list0[10] = ref_pic_list0[11] = 
	//ref_pic_list0[12] = ref_pic_list0[13] = ref_pic_list0[14] = ref_pic_list0[15] = list0[ref0];
	mv_cache0[0] = mv_cache0[1] = mv_cache0[2] = mv_cache0[3] = 
	mv_cache0[4] = mv_cache0[5] = mv_cache0[6] = mv_cache0[7] = 
	mv_cache0[8] = mv_cache0[9] = mv_cache0[10] = mv_cache0[11] = 
	mv_cache0[12] = mv_cache0[13] = mv_cache0[14] = mv_cache0[15] = mv0;
	mv_cache1[0] = mv_cache1[1] = mv_cache1[2] = mv_cache1[3] = 
	mv_cache1[4] = mv_cache1[5] = mv_cache1[6] = mv_cache1[7] = 
	mv_cache1[8] = mv_cache1[9] = mv_cache1[10] = mv_cache1[11] = 
	mv_cache1[12] = mv_cache1[13] = mv_cache1[14] = mv_cache1[15] = mv1;
  }
  else
  {
    block_xy = mb_xy*16;
    for (block8x8=0; block8x8<4; block8x8++)
    {      
      int k = (block8x8 << 2);
      int col_indexR;
	  VO_S32 mapped_idx=0;
      VO_S32 iref;
	  VO_S32 mv_scale;
	  VO_S16* colocated_mv;
      i =  (decode_block_scan[k] & 3);
      j = ((decode_block_scan[k] >> 2) & 3);
	  if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
        continue;
      pDecLocal->b8type[block8x8] = sub_type;
	  ref_cache1[j*4+i]=ref_cache1[j*4+i+1]=ref_cache1[(j+1)*4+i]=ref_cache1[(j+1)*4+i+1]=0;
	  //ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= list1[0];
	  if(IS_INTRA0(colocated_type[0]))
	  {
        //ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
	    mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=0;
	    mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=0;
	    ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=0;
		continue;
      }
	  col_indexR = currSlice->active_sps->direct_8x8_inference_flag?block_xy+RSD(j)*4+RSD(i):block_xy+j*4+i;
	  colocated_ref_list0 = list1[0]->ref_idx[0][col_indexR];
	  colocated_ref_list1 = list1[0]->ref_idx[1][col_indexR];
	  //colocated_ref_pic0 = list1[0]->ref_pic[0][col_indexR];
	  //colocated_ref_pic1 = list1[0]->ref_pic[1][col_indexR];
	  colocated_ref_pic0 = list1[0]->listX[currSlice->slice_number][0][colocated_ref_list0];
      colocated_ref_pic1 = list1[0]->listX[currSlice->slice_number][1][colocated_ref_list1];
	  colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col_indexR];
	  colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col_indexR];
	  if(colocated_ref_list0 <= -1)
	  {
		ref_pic = colocated_ref_pic1;
		colocated_mv = colocated_mv1;
	  }
	  else
	  {
		ref_pic = colocated_ref_pic0;
		colocated_mv = colocated_mv0;		
	  }
	  if(ref_pic == NULL)
	  {
	    return 0;
	  }
	  for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[currSlice->slice_number][LIST_0]);iref++)
      {
        if(currSlice->listX[currSlice->slice_number][LIST_0][iref] == ref_pic)
        {
          mapped_idx = iref;            
          break;
        }
        else //! invalid index. Default to zero even though this case should not happen
        {
          mapped_idx=INVALIDINDEX;
        }
      }
	  if(INVALIDINDEX == mapped_idx)
      {
        //ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
	    mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=0;
	    mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=0;
	    ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=0;
		continue;
	  }
	  mv_scale = currSlice->mvscale[LIST_0][mapped_idx];
	  ref0 = (char) mapped_idx;
	  ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=ref0;
	  //ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[ref0];
	  if(IS_SUB_8X8(sub_type))
	  {
	    cmv_x = colocated_mv[0];
		cmv_y = colocated_mv[1];
	    if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0][mapped_idx]->is_long_term)
        {
	      pmvl0[0] = cmv_x;
		  pmvl0[1] = cmv_y;
		  mv0 = M32(pmvl0);
          mv1 = 0;
        }
        else
        {
		  pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
		  pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
          pmvl1[0] = (short) (pmvl0[0] - cmv_x);
	      pmvl1[1] = (short) (pmvl0[1] - cmv_y);
		  mv0 = M32(pmvl0);
          mv1 = M32(pmvl1);
        }
		mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=mv0;
	    mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=mv1;
	  }
	  else
	  {
	    VO_S32 block4x4,index;
		for(block4x4=0; block4x4<4; block4x4++)
		{
		  index = (block4x4&2)*2+(block4x4&1);
		  cmv_x = colocated_mv[index*2+0];
		  cmv_y = colocated_mv[index*2+1];
		  if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0][mapped_idx]->is_long_term)
          {
	        pmvl0[0] = cmv_x;
		    pmvl0[1] = cmv_y;
		    mv0 = M32(pmvl0);
            mv1 = 0;
          }
          else
          {
		    pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
		    pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
            pmvl1[0] = (short) (pmvl0[0] - cmv_x);
	        pmvl1[1] = (short) (pmvl0[1] - cmv_y);
		    mv0 = M32(pmvl0);
            mv1 = M32(pmvl1);
          }
		  mv_cache0[j*4+i+index]=mv0;
		  mv_cache1[j*4+i+index]=mv1;
		}
	  }
    }
  }
  return 1;
}

VO_S32 mb_pred_b_temporal_interlace(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type)
{
	short ref_idx;
	int refList;
	StorablePicture* ref_pic;
	int i, j;
	int block8x8;   
	int stride_4x4 = 4;
	int stride_8x8 = 4;
	Slice *currSlice = pDecLocal->p_Slice;
	int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
	StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0 + list_offset];
	StorablePicture **list1 = currSlice->listX[currSlice->slice_number][LIST_1 + list_offset];
	StorablePicture** ref_pic_list0 = pDecGlobal->dec_picture->ref_pic[0]+pDecLocal->mb_xy*16;
	StorablePicture** ref_pic_list1 = pDecGlobal->dec_picture->ref_pic[1]+pDecLocal->mb_xy*16;
	VO_S32* mv_cache0 = pDecGlobal->dec_picture->mv[0]+pDecLocal->mb_xy*16;
	VO_S32* mv_cache1 = pDecGlobal->dec_picture->mv[1]+pDecLocal->mb_xy*16;
	VO_S8* ref_cache0 = pDecGlobal->dec_picture->ref_idx[0]+pDecLocal->mb_xy*16;
	VO_S8* ref_cache1 = pDecGlobal->dec_picture->ref_idx[1]+pDecLocal->mb_xy*16;
	VO_S8 colocated_ref_list0,colocated_ref_list1;
	StorablePicture* colocated_ref_pic0,*colocated_ref_pic1;
	VO_S16 pmvl0[2], pmvl1[2];
	VO_S16* colocated_mv0,*colocated_mv1;
	VO_S32 colocated_type[2];
	VO_S32 is_8x8 = IS_8X8(*mb_type);
	VO_U32 sub_type;
	int cmv_x,cmv_y;
	VO_S32 mv0,mv1;
	VO_S32 ref0,ref1;
	VO_S32 block_xy = pDecLocal->mb_xy*16;
	VO_S32 mb_xy = pDecLocal->mb_xy;
	VO_S32 index_shift = 0;
	if(list0[0] == NULL||list1[0] == NULL)
	{
		return 0;
	}
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
		//wait for list1[0],mv=(?,?) progress ready
		FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
		const VO_S32 max_y = pDecLocal->mb.y*MB_BLOCK_SIZE + MB_BLOCK_SIZE-1+MB_BLOCK_SIZE*(!!IS_INTERLACED(*mb_type));		//???interlace need fix
		//const VO_S32 max_y = 10000;
		VO_S32 errFlag = WaitForProgress(pInfo, list1[0], max_y, 0);

		if(errFlag)
		{
			//ref is error
			pInfo->errFlag = errFlag;
			return errFlag;
		}
	}
#endif
	if(IS_INTERLACED(list1[0]->mb_type_buffer[mb_xy]))
	{ 
		if(!IS_INTERLACED(*mb_type))
		{                   
			mb_xy= pDecLocal->mb.x + ((pDecLocal->mb.y&~1) + pDecGlobal->mbaff_dis)*pDecGlobal->PicWidthInMbs;
			stride_8x8 = 0;
		}
		else
		{
			mb_xy += pDecGlobal->field_offset; 
		}
		colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

		sub_type = VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT; 
		if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
		{
			*mb_type   |= VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT;
		}
		else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
		{
			*mb_type   |= VO_L0L1|VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
		}
		else
		{
			if(!currSlice->active_sps->direct_8x8_inference_flag)
			{
				sub_type = VO_8x8|VO_P0L0|VO_P0L1|VO_DIRECT; 
			}
			*mb_type   |= VO_8x8|VO_L0L1;
		}
	}
	else
	{                                              
		if(IS_INTERLACED(*mb_type))
		{                    
			mb_xy= pDecLocal->mb.x + (pDecLocal->mb.y&~1)*pDecGlobal->PicWidthInMbs;
			colocated_type[0] = list1[0]->mb_type_buffer[mb_xy];
			colocated_type[1] = list1[0]->mb_type_buffer[mb_xy+pDecGlobal->PicWidthInMbs];
			stride_8x8 = 2+4*4;
			stride_4x4 = pDecGlobal->PicWidthInMbs*16+8;

			sub_type = VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT; 

			if((colocated_type[0] & VO_INTRA_OR_16X16)
				&& (colocated_type[1] & VO_INTRA_OR_16X16)&& !is_8x8)
			{
				*mb_type   |= VO_16x8 |VO_L0L1|VO_DIRECT;
			}
			else
			{
				*mb_type   |= VO_8x8|VO_L0L1;
			}
		}
		else
		{                                          
			colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

			sub_type = VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT; 
			if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
			{
				*mb_type   |= VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT;
			}
			else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
			{
				*mb_type   |= VO_L0L1|VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
			}
			else
			{
				if(!currSlice->active_sps->direct_8x8_inference_flag)
				{
					sub_type = VO_8x8|VO_P0L0|VO_P0L1|VO_DIRECT; 
				}
				*mb_type   |= VO_8x8|VO_L0L1;
			}
		}
	}

	if(!stride_8x8)
	{
		if(pDecLocal->mb.y&1)
			index_shift = 2*stride_4x4;
	}

	if(IS_INTERLACED(*mb_type) != IS_INTERLACED(colocated_type[0]))
	{
		VO_S32 shift_y  = 2*!IS_INTERLACED(*mb_type);
		block_xy = mb_xy*16;
		for(block8x8=0; block8x8<4; block8x8++)
		{
			const int x8 = block8x8&1;
			const int y8 = block8x8>>1;
			int k = (block8x8 << 2);
			int col_indexR;
			VO_S32 mapped_idx=0;
			VO_S32 iref;
			VO_S32 mv_scale;
			VO_S16* colocated_mv;
			i =  (decode_block_scan[k] & 3);
			j = ((decode_block_scan[k] >> 2) & 3);
			if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
				continue;
			pDecLocal->b8type[block8x8] = sub_type;
			ref_cache1[j*4+i]=ref_cache1[j*4+i+1]=ref_cache1[(j+1)*4+i]=ref_cache1[(j+1)*4+i+1]=0;
			ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= list1[0];
			if(IS_INTRA0(colocated_type[y8]))
			{
				ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
				mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=0;
				mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=0;
				ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=0;
				continue;
			}
			col_indexR = block_xy+x8*3+y8*stride_4x4+index_shift;
			colocated_ref_list0 = list1[0]->ref_idx[0][col_indexR];
			colocated_ref_list1 = list1[0]->ref_idx[1][col_indexR];
			colocated_ref_pic0 = list1[0]->ref_pic[0][col_indexR];
			colocated_ref_pic1 = list1[0]->ref_pic[1][col_indexR];
			//colocated_ref_pic0 = list1[0]->listX[currSlice->slice_number][0 + list_offset][colocated_ref_list0];
			//colocated_ref_pic1 = list1[0]->listX[currSlice->slice_number][1 + list_offset][colocated_ref_list1];
			colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col_indexR];
			colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col_indexR];
			if(colocated_ref_list0 <= -1)
			{
				ref_pic = colocated_ref_pic1;
				colocated_mv = colocated_mv1;
			}
			else
			{
				ref_pic = colocated_ref_pic0;
				colocated_mv = colocated_mv0;		
			}
			if(ref_pic == NULL)
			{
				return 0;
			}
			for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[currSlice->slice_number][LIST_0 + list_offset]);iref++)
			{
				if(currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][iref]->top_field == ref_pic || 
					currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][iref]->bottom_field == ref_pic ||
					currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][iref]->frame == ref_pic)
				{
					mapped_idx = iref;            
					break;
				}
				else //! invalid index. Default to zero even though this case should not happen
				{
					mapped_idx=INVALIDINDEX;
				}
			}
			if(INVALIDINDEX == mapped_idx)
			{
				ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
				mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=0;
				mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=0;
				ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=0;
				continue;
			}
			mv_scale = currSlice->mvscale[LIST_0 + list_offset][mapped_idx];
			ref0 = (char) mapped_idx;
			ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=ref0;
			ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[ref0];

			cmv_x = colocated_mv[0];
			cmv_y = (colocated_mv[1]<<shift_y)/2;
			if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][mapped_idx]->is_long_term)
			{
				pmvl0[0] = cmv_x;
				pmvl0[1] = cmv_y;
				mv0 = M32(pmvl0);
				mv1 = 0;
			}
			else
			{
				pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
				pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
				pmvl1[0] = (short) (pmvl0[0] - cmv_x);
				pmvl1[1] = (short) (pmvl0[1] - cmv_y);
				mv0 = M32(pmvl0);
				mv1 = M32(pmvl1);
			}
			mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=mv0;
			mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=mv1;
		}
		return 1;
	}
	if(IS_16X16(*mb_type))
	{

		M32(&ref_cache1[0]) = M32(&ref_cache1[4]) = M32(&ref_cache1[8]) = M32(&ref_cache1[12]) = 0;
		ref_pic_list1[0] = ref_pic_list1[1] = ref_pic_list1[2] = ref_pic_list1[3] = 
			ref_pic_list1[4] = ref_pic_list1[5] = ref_pic_list1[6] = ref_pic_list1[7] = 
			ref_pic_list1[8] = ref_pic_list1[9] = ref_pic_list1[10] = ref_pic_list1[11] = 
			ref_pic_list1[12] = ref_pic_list1[13] = ref_pic_list1[14] = ref_pic_list1[15] = list1[0];

		if(IS_INTRA0(colocated_type[0]))
		{
			ref0=mv0=mv1=0;
		}
		else
		{
			VO_S32 mapped_idx=0;
			VO_S32 iref;
			VO_S32 col_indexR = mb_xy*16;	  
			colocated_ref_list0 = list1[0]->ref_idx[0][col_indexR];
			colocated_ref_list1 = list1[0]->ref_idx[1][col_indexR];
			colocated_ref_pic0 = list1[0]->ref_pic[0][col_indexR];
			//colocated_ref_pic0 = list1[0]->listX[currSlice->slice_number][0 + list_offset][colocated_ref_list0];
			colocated_ref_pic1 = list1[0]->ref_pic[1][col_indexR];
			//colocated_ref_pic1 = list1[0]->listX[currSlice->slice_number][1 + list_offset][colocated_ref_list1];
			colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col_indexR];
			colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col_indexR];
			if(colocated_ref_list0 <= -1)
			{
				ref_pic = colocated_ref_pic1;
				cmv_x = colocated_mv1[0];
				cmv_y = colocated_mv1[1];
			}
			else
			{
				ref_pic = colocated_ref_pic0;
				cmv_x = colocated_mv0[0];
				cmv_y = colocated_mv0[1];		
			}
			if(ref_pic == NULL)
			{
				return 0;
			}
			for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[currSlice->slice_number][LIST_0 + list_offset]);iref++)
			{
				if(currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][iref] == ref_pic)
				{
					mapped_idx = iref;            
					break;
				}
				else //! invalid index. Default to zero even though this case should not happen
				{
					mapped_idx=INVALIDINDEX;
				}
			}
			if(INVALIDINDEX == mapped_idx)
			{
				mv0 = 0;
				mv1 = 0;
				ref0 = 0;
			}
			else
			{
				int mv_scale = currSlice->mvscale[LIST_0 + list_offset][mapped_idx];

				if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][mapped_idx]->is_long_term)
				{
					pmvl0[0] = cmv_x;
					pmvl0[1] = cmv_y;
					mv0 = M32(pmvl0);
					mv1 = 0;
				}
				else
				{
					pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
					pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
					pmvl1[0] = (short) (pmvl0[0] - cmv_x);
					pmvl1[1] = (short) (pmvl0[1] - cmv_y);
					mv0 = M32(pmvl0);
					mv1 = M32(pmvl1);
				}

				ref0 = (char) mapped_idx;
			}
		}
		M32(&ref_cache0[0]) = M32(&ref_cache0[4]) = M32(&ref_cache0[8]) = M32(&ref_cache0[12]) = ref0*0x01010101;
		ref_pic_list0[0] = ref_pic_list0[1] = ref_pic_list0[2] = ref_pic_list0[3] = 
			ref_pic_list0[4] = ref_pic_list0[5] = ref_pic_list0[6] = ref_pic_list0[7] = 
			ref_pic_list0[8] = ref_pic_list0[9] = ref_pic_list0[10] = ref_pic_list0[11] = 
			ref_pic_list0[12] = ref_pic_list0[13] = ref_pic_list0[14] = ref_pic_list0[15] = list0[ref0];
		mv_cache0[0] = mv_cache0[1] = mv_cache0[2] = mv_cache0[3] = 
			mv_cache0[4] = mv_cache0[5] = mv_cache0[6] = mv_cache0[7] = 
			mv_cache0[8] = mv_cache0[9] = mv_cache0[10] = mv_cache0[11] = 
			mv_cache0[12] = mv_cache0[13] = mv_cache0[14] = mv_cache0[15] = mv0;
		mv_cache1[0] = mv_cache1[1] = mv_cache1[2] = mv_cache1[3] = 
			mv_cache1[4] = mv_cache1[5] = mv_cache1[6] = mv_cache1[7] = 
			mv_cache1[8] = mv_cache1[9] = mv_cache1[10] = mv_cache1[11] = 
			mv_cache1[12] = mv_cache1[13] = mv_cache1[14] = mv_cache1[15] = mv1;
	}
	else
	{
		block_xy = mb_xy*16;
		for (block8x8=0; block8x8<4; block8x8++)
		{      
			int k = (block8x8 << 2);
			int col_indexR;
			VO_S32 mapped_idx=0;
			VO_S32 iref;
			VO_S32 mv_scale;
			VO_S16* colocated_mv;
			i =  (decode_block_scan[k] & 3);
			j = ((decode_block_scan[k] >> 2) & 3);
			if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
				continue;
			pDecLocal->b8type[block8x8] = sub_type;
			ref_cache1[j*4+i]=ref_cache1[j*4+i+1]=ref_cache1[(j+1)*4+i]=ref_cache1[(j+1)*4+i+1]=0;
			ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= list1[0];
			if(IS_INTRA0(colocated_type[0]))
			{
				ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
				mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=0;
				mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=0;
				ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=0;
				continue;
			}
			col_indexR = currSlice->active_sps->direct_8x8_inference_flag?block_xy+RSD(j)*4+RSD(i):block_xy+j*4+i;
			colocated_ref_list0 = list1[0]->ref_idx[0][col_indexR];
			colocated_ref_list1 = list1[0]->ref_idx[1][col_indexR];
			colocated_ref_pic0 = list1[0]->ref_pic[0][col_indexR];
			colocated_ref_pic1 = list1[0]->ref_pic[1][col_indexR];
			//colocated_ref_pic0 = list1[0]->listX[currSlice->slice_number + list_offset][0][colocated_ref_list0];
			//colocated_ref_pic1 = list1[0]->listX[currSlice->slice_number + list_offset][1][colocated_ref_list1];
			colocated_mv0 = (VO_S16*)&list1[0]->mv[0][col_indexR];
			colocated_mv1 = (VO_S16*)&list1[0]->mv[1][col_indexR];
			if(colocated_ref_list0 <= -1)
			{
				ref_pic = colocated_ref_pic1;
				colocated_mv = colocated_mv1;
			}
			else
			{
				ref_pic = colocated_ref_pic0;
				colocated_mv = colocated_mv0;		
			}
			if(ref_pic == NULL)
			{
				return 0;
			}
			for (iref = 0; iref < imin(currSlice->num_ref_idx_active[LIST_0], currSlice->listXsize[currSlice->slice_number][LIST_0 + list_offset]);iref++)
			{
				if(currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][iref] == ref_pic)
				{
					mapped_idx = iref;            
					break;
				}
				else //! invalid index. Default to zero even though this case should not happen
				{
					mapped_idx=INVALIDINDEX;
				}
			}
			if(INVALIDINDEX == mapped_idx)
			{
				ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[0];
				mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=0;
				mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=0;
				ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=0;
				continue;
			}
			mv_scale = currSlice->mvscale[LIST_0 + list_offset][mapped_idx];
			ref0 = (char) mapped_idx;
			ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=ref0;
			ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= list0[ref0];
			if(IS_SUB_8X8(sub_type))
			{
				cmv_x = colocated_mv[0];
				cmv_y = colocated_mv[1];
				if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][mapped_idx]->is_long_term)
				{
					pmvl0[0] = cmv_x;
					pmvl0[1] = cmv_y;
					mv0 = M32(pmvl0);
					mv1 = 0;
				}
				else
				{
					pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
					pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
					pmvl1[0] = (short) (pmvl0[0] - cmv_x);
					pmvl1[1] = (short) (pmvl0[1] - cmv_y);
					mv0 = M32(pmvl0);
					mv1 = M32(pmvl1);
				}
				mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=mv0;
				mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=mv1;
			}
			else
			{
				VO_S32 block4x4,index;
				for(block4x4=0; block4x4<4; block4x4++)
				{
					index = (block4x4&2)*2+(block4x4&1);
					cmv_x = colocated_mv[index*2+0];
					cmv_y = colocated_mv[index*2+1];
					if (mv_scale == 9999 || currSlice->listX[currSlice->slice_number][LIST_0 + list_offset][mapped_idx]->is_long_term)
					{
						pmvl0[0] = cmv_x;
						pmvl0[1] = cmv_y;
						mv0 = M32(pmvl0);
						mv1 = 0;
					}
					else
					{
						pmvl0[0] = (short) ((mv_scale * cmv_x + 128 ) >> 8);
						pmvl0[1] = (short) ((mv_scale * cmv_y + 128 ) >> 8);
						pmvl1[0] = (short) (pmvl0[0] - cmv_x);
						pmvl1[1] = (short) (pmvl0[1] - cmv_y);
						mv0 = M32(pmvl0);
						mv1 = M32(pmvl1);
					}
					mv_cache0[j*4+i+index]=mv0;
					mv_cache1[j*4+i+index]=mv1;
				}
			}
		}
	}
	return 1;
}


VO_S32 mb_pred_b_spatial(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type)
{
  VO_S32 l_ref[2];
  VO_S32 pmv[2]={0};
  VO_S32 block8x8,list;
  int stride_4x4 = 4;
  int stride_8x8 = 4;
  Slice *currSlice = pDecLocal->p_Slice;
  //int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
  StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0];
  StorablePicture **list1 = currSlice->listX[currSlice->slice_number][LIST_1];
  //StorablePicture** ref_pic_list0 = pDecGlobal->dec_picture->ref_pic[0]+pDecLocal->mb_xy*16;
  //StorablePicture** ref_pic_list1 = pDecGlobal->dec_picture->ref_pic[1]+pDecLocal->mb_xy*16;
  VO_S32* mv_cache0 = pDecGlobal->dec_picture->mv[0]+pDecLocal->mb_xy*16;
  VO_S32* mv_cache1 = pDecGlobal->dec_picture->mv[1]+pDecLocal->mb_xy*16;
  VO_S8* ref_cache0 = pDecGlobal->dec_picture->ref_idx[0]+pDecLocal->mb_xy*16;
  VO_S8* ref_cache1 = pDecGlobal->dec_picture->ref_idx[1]+pDecLocal->mb_xy*16;
  VO_S32 ref0,ref1,mv0, mv1;
  StorablePicture* ref_pic[2];
  int pred_dir = 0;
  VO_U16 mov_flag = 0;
  VO_S32 colocated_type[2];
  VO_S32 is_8x8 = IS_8X8(*mb_type);
  VO_U32 sub_type;
  VO_S32 mb_xy = pDecLocal->mb_xy;
  VO_S32 index_shift = 0;
  if(list0[0] == NULL||list1[0] == NULL)
  {
    return 0;
  }
  for(list=0; list<2; list++)
  {
    VO_S32 l0_refA, l0_refB, l0_refC;
    VO_S8* ref_cache0 = &pDecLocal->ref_cache[list][0];
    VO_S32 n = pDecLocal->mbAvailC ? 8 : 3;
    VO_S16 *C= (VO_S16 *)&pDecLocal->mv_cache[list][n];
    l0_refA  = ref_cache0[11];
    l0_refB = ref_cache0[4];
    l0_refC = ref_cache0[n];

    l_ref[list] =VOMIN3((unsigned) l0_refA, (unsigned) l0_refB, (unsigned) l0_refC);
    if(l_ref[list] >= 0)
    {
      VO_S16 * const A= (VO_S16 *)&pDecLocal->mv_cache[list][ 12 - 1 ];
      VO_S16 * const B= (VO_S16 *)&pDecLocal->mv_cache[list][ 12 - 8 ];

      int match_count= (l0_refA==l_ref[list]) + (l0_refB==l_ref[list]) + (l0_refC==l_ref[list]);
      if(match_count > 1)
	  {
	    VO_S16 *p = (VO_S16 *)&pmv[list];
        p[0]= imedian(A[0], B[0], C[0]);
		p[1]= imedian(A[1], B[1], C[1]);
      }
	  else 
	  {
        if(l0_refA==l_ref[list])
          pmv[list]= M32(A);
        else if(l0_refB==l_ref[list])
          pmv[list]= M32(B);
        else
          pmv[list]= M32(C);
      }
    }
	else
	{
      VO_S32 mask= ~(VO_L0 << (2*list));
      pmv[list] = 0;
      l_ref[list] = -1;
      if(!is_8x8)
        *mb_type &= mask;
      sub_type &= mask;
	  
    }
  }
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
		//wait for list1[0],mv=(?,?) progress ready
		FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
		const VO_S32 max_y = pDecLocal->mb.y*MB_BLOCK_SIZE + MB_BLOCK_SIZE-1; 	  //???interlace need fix
        //const VO_S32 max_y = 10000;
		VO_S32 errFlag = WaitForProgress(pInfo, list1[0], max_y, 0);
		
		if(errFlag)
		{
			//ref is error
			pInfo->errFlag = errFlag;
			return errFlag;
		}
	}
#endif
  //if(!currSlice->active_sps->direct_8x8_inference_flag)
  {
    if (l_ref[0] < 0 && l_ref[1] < 0)
    {
	  pred_dir=2;
	  //ref_pic[0] = list0[0];
      //ref_pic[1] = list1[0];
    }
	else if (l_ref[1] <= -1)
	{
	  pred_dir=0;
	  //ref_pic[0] = list0[l_ref[0]];
      //ref_pic[1] = NULL;
	}
	else if (l_ref[0] <= -1)
	{
	  pred_dir=1;
	  //ref_pic[0] = NULL;
      //ref_pic[1] = list1[l_ref[1]];
	}
	else
	{
	  pred_dir=2;
	  //ref_pic[0] = list0[l_ref[0]];
      //ref_pic[1] = list1[l_ref[1]];
	}
    pDecLocal->b8pdir[0] = pDecLocal->b8pdir[1] = pDecLocal->b8pdir[2] = pDecLocal->b8pdir[3] = pred_dir;
  }
  if(l_ref[0] < 0 && l_ref[1] < 0)
  {
    l_ref[0] = l_ref[1] = 0;
    if(!is_8x8)
      *mb_type |= VO_L0L1;
    sub_type |= VO_L0L1;
  }
  if(!(is_8x8|pmv[0]|pmv[1]))
  {
    M32(ref_cache0)=M32(ref_cache0+4)=M32(ref_cache0+8)=M32(ref_cache0+12)=((VO_U8)l_ref[0])*0x01010101;
	M32(ref_cache1)=M32(ref_cache1+4)=M32(ref_cache1+8)=M32(ref_cache1+12)=((VO_U8)l_ref[1])*0x01010101;
	//ref_pic_list0[0]=ref_pic_list0[1]=ref_pic_list0[2]=ref_pic_list0[3]=
	//ref_pic_list0[4]=ref_pic_list0[5]=ref_pic_list0[6]=ref_pic_list0[7]=
	//ref_pic_list0[8]=ref_pic_list0[9]=ref_pic_list0[10]=ref_pic_list0[11]=
	//ref_pic_list0[12]=ref_pic_list0[13]=ref_pic_list0[14]=ref_pic_list0[15]=ref_pic[0];
	//ref_pic_list1[0]=ref_pic_list1[1]=ref_pic_list1[2]=ref_pic_list1[3]=
	//ref_pic_list1[4]=ref_pic_list1[5]=ref_pic_list1[6]=ref_pic_list1[7]=
	//ref_pic_list1[8]=ref_pic_list1[9]=ref_pic_list1[10]=ref_pic_list1[11]=
	//ref_pic_list1[12]=ref_pic_list1[13]=ref_pic_list1[14]=ref_pic_list1[15]=ref_pic[1];
	mv_cache0[0]=mv_cache0[1]=mv_cache0[2]=mv_cache0[3]=
	mv_cache0[4]=mv_cache0[5]=mv_cache0[6]=mv_cache0[7]=
	mv_cache0[8]=mv_cache0[9]=mv_cache0[10]=mv_cache0[11]=
	mv_cache0[12]=mv_cache0[13]=mv_cache0[14]=mv_cache0[15]=pmv[0];
    mv_cache1[0]=mv_cache1[1]=mv_cache1[2]=mv_cache1[3]=
	mv_cache1[4]=mv_cache1[5]=mv_cache1[6]=mv_cache1[7]=
	mv_cache1[8]=mv_cache1[9]=mv_cache1[10]=mv_cache1[11]=
	mv_cache1[12]=mv_cache1[13]=mv_cache1[14]=mv_cache1[15]=pmv[1];
	pDecLocal->direct_mode[0] = 16;
    *mb_type= (*mb_type & ~(VO_8x8|VO_16x8|VO_8x16|VO_P1L0|VO_P1L1))|VO_16x16|VO_DIRECT;
    return 1;
  }
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
		//wait for list1[0],mv=(?,?) progress ready
		FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
		const VO_S32 max_y = pDecLocal->mb.y*MB_BLOCK_SIZE + MB_BLOCK_SIZE-1+MB_BLOCK_SIZE*(!!IS_INTERLACED(*mb_type)); 	  //???interlace need fix
		VO_S32 errFlag = WaitForProgress(pInfo, list1[0], max_y, 0);
		
		if(errFlag)
		{
			//ref is error
			pInfo->errFlag = errFlag;
			return errFlag;
		}
	}
#endif

  
                               
      colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

      sub_type = VO_16x16|VO_DIRECT; 
      if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
	  {
         *mb_type   |= VO_16x16|VO_DIRECT;
      }
	  else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
	  {
         *mb_type   |=VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
      }
	  else
	  {
         if(!currSlice->active_sps->direct_8x8_inference_flag)
		 {
           sub_type += (VO_8x8-VO_16x16); 
         }
         *mb_type   |= VO_8x8;
      }


  
  if(IS_16X16(*mb_type))
  {
	VO_S32 index = mb_xy*16;
	VO_S32 ref_idx_list0 = list1[0]->ref_idx[0][index];
	VO_S32 ref_idx_list1 = list1[0]->ref_idx[1][index];
	VO_S16* mv_list0 = (VO_S16*)&list1[0]->mv[0][index];
	VO_S16* mv_list1 = (VO_S16*)&list1[0]->mv[1][index];
    //ref_pic[0]=ref_pic[1]=NULL;
    ref0=ref1=-1;
	mv0=mv1=0;
	M32(ref_cache0)=M32(ref_cache0+4)=M32(ref_cache0+8)=M32(ref_cache0+12)=((VO_U8)l_ref[0])*0x01010101;
	M32(ref_cache1)=M32(ref_cache1+4)=M32(ref_cache1+8)=M32(ref_cache1+12)=((VO_U8)l_ref[1])*0x01010101;
	//ref_pic_list0[0] = ref_pic_list0[1] = ref_pic_list0[2] = ref_pic_list0[3] = 
	//ref_pic_list0[4] = ref_pic_list0[5] = ref_pic_list0[6] = ref_pic_list0[7] = 
	//ref_pic_list0[8] = ref_pic_list0[9] = ref_pic_list0[10] = ref_pic_list0[11] = 
	//ref_pic_list0[12] = ref_pic_list0[13] = ref_pic_list0[14] = ref_pic_list0[15] = ref_pic[0];
	//ref_pic_list1[0] = ref_pic_list1[1] = ref_pic_list1[2] = ref_pic_list1[3] = 
	//ref_pic_list1[4] = ref_pic_list1[5] = ref_pic_list1[6] = ref_pic_list1[7] = 
	//ref_pic_list1[8] = ref_pic_list1[9] = ref_pic_list1[10] = ref_pic_list1[11] = 
	//ref_pic_list1[12] = ref_pic_list1[13] = ref_pic_list1[14] = ref_pic_list1[15] = ref_pic[1];
    if(!IS_INTRA0(colocated_type[0]) && !list1[0]->is_long_term
       && ((ref_idx_list0 == 0 && iabs(mv_list0[0]) <= 1 && iabs(mv_list0[1]) <= 1)
       || (ref_idx_list0  < 0 && ref_idx_list1 == 0 && iabs(mv_list1[0]) <= 1 && iabs(mv_list1[1]) <= 1)))
    {     
      if(l_ref[0] > 0)
      {
        mv0= pmv[0];
      }
      if(l_ref[1] > 0)
      {
        mv1= pmv[1];
      }
    }
	else
	{
      mv0= pmv[0];
      mv1= pmv[1];
    }
    
	mv_cache0[0] = mv_cache0[1] = mv_cache0[2] = mv_cache0[3] = 
	mv_cache0[4] = mv_cache0[5] = mv_cache0[6] = mv_cache0[7] = 
	mv_cache0[8] = mv_cache0[9] = mv_cache0[10] = mv_cache0[11] = 
	mv_cache0[12] = mv_cache0[13] = mv_cache0[14] = mv_cache0[15] = mv0;
	mv_cache1[0] = mv_cache1[1] = mv_cache1[2] = mv_cache1[3] = 
	mv_cache1[4] = mv_cache1[5] = mv_cache1[6] = mv_cache1[7] = 
	mv_cache1[8] = mv_cache1[9] = mv_cache1[10] = mv_cache1[11] = 
	mv_cache1[12] = mv_cache1[13] = mv_cache1[14] = mv_cache1[15] = mv1;
	
  }
  else
  {
    VO_S32 n=0;
    for(block8x8=0; block8x8<4; block8x8++)
	{
	  VO_S32 k = (block8x8 << 2);
      VO_S32 i =  (decode_block_scan[k] & 3);
      VO_S32 j = ((decode_block_scan[k] >> 2) & 3);
      VO_S32 index = currSlice->active_sps->direct_8x8_inference_flag?mb_xy*16+RSD(j)*4+RSD(i):mb_xy*16+j*4+i;
	  VO_S32 ref_idx_list0 = list1[0]->ref_idx[0][index];
	  VO_S32 ref_idx_list1 = list1[0]->ref_idx[1][index];
	  VO_S16* mv_list0 = (VO_S16*)&list1[0]->mv[0][index];
	  VO_S16* mv_list1 = (VO_S16*)&list1[0]->mv[1][index];
      if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
        continue;
      pDecLocal->b8type[block8x8] = sub_type;

      ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=l_ref[0];
      ref_cache1[j*4+i]=ref_cache1[j*4+i+1]=ref_cache1[(j+1)*4+i]=ref_cache1[(j+1)*4+i+1]=l_ref[1];
	  //ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= ref_pic[0];
	  //ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= ref_pic[1];
	  mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=pmv[0];
	  mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=pmv[1];

      if(!IS_INTRA0(colocated_type[0]) && !list1[0]->is_long_term && 
	  	( ref_idx_list0 == 0|| (ref_idx_list0 < 0 && ref_idx_list1 == 0)))
	  {
        const VO_S16 *mv8= ref_idx_list0 == 0 ? mv_list0 : mv_list1;
        if(IS_SUB_8X8(sub_type))
		{
          if(iabs(mv8[0]) <= 1 && iabs(mv8[1]) <= 1)
		  {
            if(l_ref[0] == 0)
            {
              mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=0;
            }
            if(l_ref[1] == 0)
            {
              mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=0;
            }
            n+=4;
          }
        }
		else
		{
          VO_S32 m=0;
		  VO_S32 block4x4,index;
		  VO_S16 *mv4;
          for(block4x4=0; block4x4<4; block4x4++)
		  {
		    index = (block4x4&2)*2+(block4x4&1);
            mv4 = &mv_list0[index*2];
            if(iabs(mv4[0]) <= 1 && iabs(mv4[1]) <= 1)
			{
              if(l_ref[0] == 0)
                mv_cache0[j*4+i+index]=0;
              if(l_ref[1] == 0)
                mv_cache1[j*4+i+index]=0;
              m++;
            }
          }
          if(!(m&3))
             pDecLocal->b8type[block8x8]+= VO_16x16 - VO_8x8;
          n+=m;
        }
      }
    }
    if(!is_8x8 && !(n&15))
      *mb_type= (*mb_type & ~(VO_8x8|VO_16x8|VO_8x16|VO_P1L0|VO_P1L1))|VO_16x16|VO_DIRECT;
  }
  return 1;
}


VO_S32 mb_pred_b_spatial_interlace(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32* mb_type)
{
	VO_S32 l_ref[2];
	VO_S32 pmv[2]={0};
	VO_S32 block8x8,list;
	int stride_4x4 = 4;
	int stride_8x8 = 4;
	Slice *currSlice = pDecLocal->p_Slice;
	int list_offset = (pDecLocal->mb_field&&!currSlice->field_pic_flag)? (pDecLocal->mb.y&1)?4:2: 0;
	StorablePicture **list0 = currSlice->listX[currSlice->slice_number][LIST_0+list_offset];
	StorablePicture **list1 = currSlice->listX[currSlice->slice_number][LIST_1+list_offset];
	StorablePicture** ref_pic_list0 = pDecGlobal->dec_picture->ref_pic[0]+pDecLocal->mb_xy*16;
	StorablePicture** ref_pic_list1 = pDecGlobal->dec_picture->ref_pic[1]+pDecLocal->mb_xy*16;
	VO_S32* mv_cache0 = pDecGlobal->dec_picture->mv[0]+pDecLocal->mb_xy*16;
	VO_S32* mv_cache1 = pDecGlobal->dec_picture->mv[1]+pDecLocal->mb_xy*16;
	VO_S8* ref_cache0 = pDecGlobal->dec_picture->ref_idx[0]+pDecLocal->mb_xy*16;
	VO_S8* ref_cache1 = pDecGlobal->dec_picture->ref_idx[1]+pDecLocal->mb_xy*16;
	VO_S32 ref0,ref1,mv0, mv1;
	StorablePicture* ref_pic[2];
	int pred_dir = 0;
	VO_U16 mov_flag = 0;
	VO_S32 colocated_type[2];
	VO_S32 is_8x8 = IS_8X8(*mb_type);
	VO_U32 sub_type;
	VO_S32 mb_xy = pDecLocal->mb_xy;
	VO_S32 index_shift = 0;
	if(list0[0] == NULL||list1[0] == NULL)
	{
		return 0;
	}
	for(list=0; list<2; list++)
	{
		VO_S32 l0_refA, l0_refB, l0_refC;
		VO_S8* ref_cache0 = &pDecLocal->ref_cache[list][0];
		VO_S32 n = pDecLocal->mbAvailC ? 8 : 3;
		VO_S16 *C= (VO_S16 *)&pDecLocal->mv_cache[list][n];
		l0_refA  = ref_cache0[11];
		l0_refB = ref_cache0[4];
		l0_refC = ref_cache0[n];

		l_ref[list] =VOMIN3((unsigned) l0_refA, (unsigned) l0_refB, (unsigned) l0_refC);
		if(l_ref[list] >= 0)
		{
			VO_S16 * const A= (VO_S16 *)&pDecLocal->mv_cache[list][ 12 - 1 ];
			VO_S16 * const B= (VO_S16 *)&pDecLocal->mv_cache[list][ 12 - 8 ];

			int match_count= (l0_refA==l_ref[list]) + (l0_refB==l_ref[list]) + (l0_refC==l_ref[list]);
			if(match_count > 1)
			{
				VO_S16 *p = (VO_S16 *)&pmv[list];
				p[0]= imedian(A[0], B[0], C[0]);
				p[1]= imedian(A[1], B[1], C[1]);
			}
			else 
			{
				if(l0_refA==l_ref[list])
					pmv[list]= M32(A);
				else if(l0_refB==l_ref[list])
					pmv[list]= M32(B);
				else
					pmv[list]= M32(C);
			}
		}
		else
		{
			VO_S32 mask= ~(VO_L0 << (2*list));
			pmv[list] = 0;
			l_ref[list] = -1;
			if(!is_8x8)
				*mb_type &= mask;
			sub_type &= mask;

		}
	}
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
		//wait for list1[0],mv=(?,?) progress ready
		FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
		const VO_S32 max_y = pDecLocal->mb.y*MB_BLOCK_SIZE + MB_BLOCK_SIZE-1+MB_BLOCK_SIZE*(!!IS_INTERLACED(*mb_type)); 	  //???interlace need fix
		//const VO_S32 max_y = 10000;
		VO_S32 errFlag = WaitForProgress(pInfo, list1[0], max_y, 0);

		if(errFlag)
		{
			//ref is error
			pInfo->errFlag = errFlag;
			return errFlag;
		}
	}
#endif
	//if(!currSlice->active_sps->direct_8x8_inference_flag)
	{
		if (l_ref[0] < 0 && l_ref[1] < 0)
		{
			pred_dir=2;
			ref_pic[0] = list0[0];
			ref_pic[1] = list1[0];
		}
		else if (l_ref[1] <= -1)
		{
			pred_dir=0;
			ref_pic[0] = list0[l_ref[0]];
			ref_pic[1] = NULL;
		}
		else if (l_ref[0] <= -1)
		{
			pred_dir=1;
			ref_pic[0] = NULL;
			ref_pic[1] = list1[l_ref[1]];
		}
		else
		{
			pred_dir=2;
			ref_pic[0] = list0[l_ref[0]];
			ref_pic[1] = list1[l_ref[1]];
		}
		pDecLocal->b8pdir[0] = pDecLocal->b8pdir[1] = pDecLocal->b8pdir[2] = pDecLocal->b8pdir[3] = pred_dir;
	}
	if(l_ref[0] < 0 && l_ref[1] < 0)
	{
		l_ref[0] = l_ref[1] = 0;
		if(!is_8x8)
			*mb_type |= VO_L0L1;
		sub_type |= VO_L0L1;
	}
	if(!(is_8x8|pmv[0]|pmv[1]))
	{
		M32(ref_cache0)=M32(ref_cache0+4)=M32(ref_cache0+8)=M32(ref_cache0+12)=((VO_U8)l_ref[0])*0x01010101;
		M32(ref_cache1)=M32(ref_cache1+4)=M32(ref_cache1+8)=M32(ref_cache1+12)=((VO_U8)l_ref[1])*0x01010101;
		ref_pic_list0[0]=ref_pic_list0[1]=ref_pic_list0[2]=ref_pic_list0[3]=
			ref_pic_list0[4]=ref_pic_list0[5]=ref_pic_list0[6]=ref_pic_list0[7]=
			ref_pic_list0[8]=ref_pic_list0[9]=ref_pic_list0[10]=ref_pic_list0[11]=
			ref_pic_list0[12]=ref_pic_list0[13]=ref_pic_list0[14]=ref_pic_list0[15]=ref_pic[0];
		ref_pic_list1[0]=ref_pic_list1[1]=ref_pic_list1[2]=ref_pic_list1[3]=
			ref_pic_list1[4]=ref_pic_list1[5]=ref_pic_list1[6]=ref_pic_list1[7]=
			ref_pic_list1[8]=ref_pic_list1[9]=ref_pic_list1[10]=ref_pic_list1[11]=
			ref_pic_list1[12]=ref_pic_list1[13]=ref_pic_list1[14]=ref_pic_list1[15]=ref_pic[1];
		mv_cache0[0]=mv_cache0[1]=mv_cache0[2]=mv_cache0[3]=
			mv_cache0[4]=mv_cache0[5]=mv_cache0[6]=mv_cache0[7]=
			mv_cache0[8]=mv_cache0[9]=mv_cache0[10]=mv_cache0[11]=
			mv_cache0[12]=mv_cache0[13]=mv_cache0[14]=mv_cache0[15]=pmv[0];
		mv_cache1[0]=mv_cache1[1]=mv_cache1[2]=mv_cache1[3]=
			mv_cache1[4]=mv_cache1[5]=mv_cache1[6]=mv_cache1[7]=
			mv_cache1[8]=mv_cache1[9]=mv_cache1[10]=mv_cache1[11]=
			mv_cache1[12]=mv_cache1[13]=mv_cache1[14]=mv_cache1[15]=pmv[1];
		pDecLocal->direct_mode[0] = 16;
		*mb_type= (*mb_type & ~(VO_8x8|VO_16x8|VO_8x16|VO_P1L0|VO_P1L1))|VO_16x16|VO_DIRECT;
		return 1;
	}
#if USE_FRAME_THREAD
	if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
	{
		//wait for list1[0],mv=(?,?) progress ready
		FrameThdInfo *pInfo = pDecGlobal->pThdInfo;
		const VO_S32 max_y = pDecLocal->mb.y*MB_BLOCK_SIZE + MB_BLOCK_SIZE-1+MB_BLOCK_SIZE*(!!IS_INTERLACED(*mb_type)); 	  //???interlace need fix
		VO_S32 errFlag = WaitForProgress(pInfo, list1[0], max_y, 0);

		if(errFlag)
		{
			//ref is error
			pInfo->errFlag = errFlag;
			return errFlag;
		}
	}
#endif

	if(IS_INTERLACED(list1[0]->mb_type_buffer[mb_xy]))
	{ 
		if(!IS_INTERLACED(*mb_type))
		{                   
			mb_xy= pDecLocal->mb.x + ((pDecLocal->mb.y&~1) + pDecGlobal->mbaff_dis)*pDecGlobal->PicWidthInMbs;
			stride_8x8 = 0;
		}
		else
		{
			mb_xy += pDecGlobal->field_offset; 
		}
		colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

		sub_type = VO_16x16|VO_DIRECT; 
		if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
		{
			*mb_type   |= VO_16x16|VO_DIRECT;
		}
		else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
		{
			*mb_type   |=VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
		}
		else
		{
			if(!currSlice->active_sps->direct_8x8_inference_flag)
			{
				sub_type += (VO_8x8-VO_16x16); 
			}
			*mb_type   |= VO_8x8;
		}
	}
	else
	{                                              
		if(IS_INTERLACED(*mb_type))
		{                   
			mb_xy= pDecLocal->mb.x + (pDecLocal->mb.y&~1)*pDecGlobal->PicWidthInMbs;
			colocated_type[0] = list1[0]->mb_type_buffer[mb_xy];
			colocated_type[1] = list1[0]->mb_type_buffer[mb_xy+pDecGlobal->PicWidthInMbs];
			stride_8x8 = 2+4*4;
			stride_4x4 = pDecGlobal->PicWidthInMbs*16+8;

			sub_type = VO_16x16|VO_P0L0|VO_P0L1|VO_DIRECT; 

			if((colocated_type[0] & VO_INTRA_OR_16X16)
				&& (colocated_type[1] & VO_INTRA_OR_16X16)&& !is_8x8)
			{
				*mb_type   |= VO_16x8 |VO_L0L1|VO_DIRECT;
			}
			else
			{
				*mb_type   |= VO_8x8|VO_L0L1;
			}
		}
		else
		{                                          
			colocated_type[0] = colocated_type[1] = list1[0]->mb_type_buffer[mb_xy];

			sub_type = VO_16x16|VO_DIRECT; 
			if(!is_8x8 && (colocated_type[0] & VO_INTRA_OR_16X16))
			{
				*mb_type   |= VO_16x16|VO_DIRECT;
			}
			else if(!is_8x8 && (colocated_type[0] & (VO_16x8|VO_8x16)))
			{
				*mb_type   |=VO_DIRECT | (colocated_type[0] & (VO_16x8|VO_8x16));
			}
			else
			{
				if(!currSlice->active_sps->direct_8x8_inference_flag)
				{
					sub_type += (VO_8x8-VO_16x16); 
				}
				*mb_type   |= VO_8x8;
			}
		}
	}
	if(!stride_8x8)
	{
		if(pDecLocal->mb.y&1)
			index_shift = 2*stride_4x4;
	}
	if(IS_INTERLACED(*mb_type) != IS_INTERLACED(colocated_type[0]))
	{
		int n=0;
		for(block8x8=0; block8x8<4; block8x8++)
		{
			int x8 = block8x8&1;
			int y8 = block8x8>>1;
			VO_S32 k = (block8x8 << 2);
			VO_S32 i =  (decode_block_scan[k] & 3);
			VO_S32 j = ((decode_block_scan[k] >> 2) & 3);
			VO_S32 index = mb_xy*16+x8*3+y8*stride_4x4+index_shift;
			VO_S32 ref_idx_list0 = list1[0]->ref_idx[0][index];
			VO_S32 ref_idx_list1 = list1[0]->ref_idx[1][index];
			VO_S16* mv_list0 = (VO_S16*)&list1[0]->mv[0][index];
			VO_S16* mv_list1 = (VO_S16*)&list1[0]->mv[1][index];
			if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
				continue;
			pDecLocal->b8type[block8x8] = sub_type;

			ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=l_ref[0];
			ref_cache1[j*4+i]=ref_cache1[j*4+i+1]=ref_cache1[(j+1)*4+i]=ref_cache1[(j+1)*4+i+1]=l_ref[1];
			ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= ref_pic[0];
			ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= ref_pic[1];
			if(!IS_INTRA0(colocated_type[y8]) && !list1[0]->is_long_term
				&& ((ref_idx_list0 == 0 && iabs(mv_list0[0]) <= 1 && iabs(mv_list0[1]) <= 1)
				|| (ref_idx_list0  < 0 && ref_idx_list1 == 0 && iabs(mv_list1[0]) <= 1 && iabs(mv_list1[1]) <= 1)))
			{    
				mv0=mv1=0;
				if(l_ref[0] > 0)
				{
					mv0= pmv[0];
				}
				if(l_ref[1] > 0)
				{
					mv1= pmv[1];
				}
				n++;
			}
			else
			{
				mv0= pmv[0];
				mv1= pmv[1];
			}

			mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=mv0;
			mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=mv1;
		}
		if(!is_8x8 && !(n&3))
			*mb_type= (*mb_type & ~(VO_8x8|VO_16x8|VO_8x16|VO_P1L0|VO_P1L1))|VO_16x16|VO_DIRECT;
	}
	else if(IS_16X16(*mb_type))
	{
		VO_S32 index = mb_xy*16;
		VO_S32 ref_idx_list0 = list1[0]->ref_idx[0][index];
		VO_S32 ref_idx_list1 = list1[0]->ref_idx[1][index];
		VO_S16* mv_list0 = (VO_S16*)&list1[0]->mv[0][index];
		VO_S16* mv_list1 = (VO_S16*)&list1[0]->mv[1][index];
		//ref_pic[0]=ref_pic[1]=NULL;
		ref0=ref1=-1;
		mv0=mv1=0;
		M32(ref_cache0)=M32(ref_cache0+4)=M32(ref_cache0+8)=M32(ref_cache0+12)=((VO_U8)l_ref[0])*0x01010101;
		M32(ref_cache1)=M32(ref_cache1+4)=M32(ref_cache1+8)=M32(ref_cache1+12)=((VO_U8)l_ref[1])*0x01010101;
		ref_pic_list0[0] = ref_pic_list0[1] = ref_pic_list0[2] = ref_pic_list0[3] = 
			ref_pic_list0[4] = ref_pic_list0[5] = ref_pic_list0[6] = ref_pic_list0[7] = 
			ref_pic_list0[8] = ref_pic_list0[9] = ref_pic_list0[10] = ref_pic_list0[11] = 
			ref_pic_list0[12] = ref_pic_list0[13] = ref_pic_list0[14] = ref_pic_list0[15] = ref_pic[0];
		ref_pic_list1[0] = ref_pic_list1[1] = ref_pic_list1[2] = ref_pic_list1[3] = 
			ref_pic_list1[4] = ref_pic_list1[5] = ref_pic_list1[6] = ref_pic_list1[7] = 
			ref_pic_list1[8] = ref_pic_list1[9] = ref_pic_list1[10] = ref_pic_list1[11] = 
			ref_pic_list1[12] = ref_pic_list1[13] = ref_pic_list1[14] = ref_pic_list1[15] = ref_pic[1];
		if(!IS_INTRA0(colocated_type[0]) && !list1[0]->is_long_term
			&& ((ref_idx_list0 == 0 && iabs(mv_list0[0]) <= 1 && iabs(mv_list0[1]) <= 1)
			|| (ref_idx_list0  < 0 && ref_idx_list1 == 0 && iabs(mv_list1[0]) <= 1 && iabs(mv_list1[1]) <= 1)))
		{     
			if(l_ref[0] > 0)
			{
				mv0= pmv[0];
			}
			if(l_ref[1] > 0)
			{
				mv1= pmv[1];
			}
		}
		else
		{
			mv0= pmv[0];
			mv1= pmv[1];
		}

		mv_cache0[0] = mv_cache0[1] = mv_cache0[2] = mv_cache0[3] = 
			mv_cache0[4] = mv_cache0[5] = mv_cache0[6] = mv_cache0[7] = 
			mv_cache0[8] = mv_cache0[9] = mv_cache0[10] = mv_cache0[11] = 
			mv_cache0[12] = mv_cache0[13] = mv_cache0[14] = mv_cache0[15] = mv0;
		mv_cache1[0] = mv_cache1[1] = mv_cache1[2] = mv_cache1[3] = 
			mv_cache1[4] = mv_cache1[5] = mv_cache1[6] = mv_cache1[7] = 
			mv_cache1[8] = mv_cache1[9] = mv_cache1[10] = mv_cache1[11] = 
			mv_cache1[12] = mv_cache1[13] = mv_cache1[14] = mv_cache1[15] = mv1;

	}
	else
	{
		VO_S32 n=0;
		for(block8x8=0; block8x8<4; block8x8++)
		{
			VO_S32 k = (block8x8 << 2);
			VO_S32 i =  (decode_block_scan[k] & 3);
			VO_S32 j = ((decode_block_scan[k] >> 2) & 3);
			VO_S32 index = currSlice->active_sps->direct_8x8_inference_flag?mb_xy*16+RSD(j)*4+RSD(i):mb_xy*16+j*4+i;
			VO_S32 ref_idx_list0 = list1[0]->ref_idx[0][index];
			VO_S32 ref_idx_list1 = list1[0]->ref_idx[1][index];
			VO_S16* mv_list0 = (VO_S16*)&list1[0]->mv[0][index];
			VO_S16* mv_list1 = (VO_S16*)&list1[0]->mv[1][index];
			if(is_8x8 && !IS_DIRECT_VO(pDecLocal->b8type[block8x8]))
				continue;
			pDecLocal->b8type[block8x8] = sub_type;

			ref_cache0[j*4+i]=ref_cache0[j*4+i+1]=ref_cache0[(j+1)*4+i]=ref_cache0[(j+1)*4+i+1]=l_ref[0];
			ref_cache1[j*4+i]=ref_cache1[j*4+i+1]=ref_cache1[(j+1)*4+i]=ref_cache1[(j+1)*4+i+1]=l_ref[1];
			ref_pic_list0[j*4+i]=ref_pic_list0[j*4+i+1]=ref_pic_list0[(j+1)*4+i]=ref_pic_list0[(j+1)*4+i+1]= ref_pic[0];
			ref_pic_list1[j*4+i]=ref_pic_list1[j*4+i+1]=ref_pic_list1[(j+1)*4+i]=ref_pic_list1[(j+1)*4+i+1]= ref_pic[1];
			mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=pmv[0];
			mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=pmv[1];

			if(!IS_INTRA0(colocated_type[0]) && !list1[0]->is_long_term && 
				( ref_idx_list0 == 0|| (ref_idx_list0 < 0 && ref_idx_list1 == 0)))
			{
				const VO_S16 *mv8= ref_idx_list0 == 0 ? mv_list0 : mv_list1;
				if(IS_SUB_8X8(sub_type))
				{
					if(iabs(mv8[0]) <= 1 && iabs(mv8[1]) <= 1)
					{
						if(l_ref[0] == 0)
						{
							mv_cache0[j*4+i]=mv_cache0[j*4+i+1]=mv_cache0[(j+1)*4+i]=mv_cache0[(j+1)*4+i+1]=0;
						}
						if(l_ref[1] == 0)
						{
							mv_cache1[j*4+i]=mv_cache1[j*4+i+1]=mv_cache1[(j+1)*4+i]=mv_cache1[(j+1)*4+i+1]=0;
						}
						n+=4;
					}
				}
				else
				{
					VO_S32 m=0;
					VO_S32 block4x4,index;
					VO_S16 *mv4;
					for(block4x4=0; block4x4<4; block4x4++)
					{
						index = (block4x4&2)*2+(block4x4&1);
						mv4 = &mv_list0[index*2];
						if(iabs(mv4[0]) <= 1 && iabs(mv4[1]) <= 1)
						{
							if(l_ref[0] == 0)
								mv_cache0[j*4+i+index]=0;
							if(l_ref[1] == 0)
								mv_cache1[j*4+i+index]=0;
							m++;
						}
					}
					if(!(m&3))
						pDecLocal->b8type[block8x8]+= VO_16x16 - VO_8x8;
					n+=m;
				}
			}
		}
		if(!is_8x8 && !(n&15))
			*mb_type= (*mb_type & ~(VO_8x8|VO_16x8|VO_8x16|VO_P1L0|VO_P1L1))|VO_16x16|VO_DIRECT;
	}
	return 1;
}

int mb_pred_b_inter8x8(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *dec_picture)
{
  //char l0_rFrame = -1, l1_rFrame = -1;
  //VO_S16 pmvl0[2] = {0}, pmvl1[2] = {0};
  int block_size_x, block_size_y;
  int k,i,j;
  int block8x8;   // needed for ABT
  Slice *currSlice = pDecLocal->p_Slice;
  const VO_S8 *ref_cache0 = pDecGlobal->dec_picture->ref_idx[LIST_0]+pDecLocal->mb_xy*16;
  const VO_S8 *ref_cache1 = pDecGlobal->dec_picture->ref_idx[LIST_1]+pDecLocal->mb_xy*16;
  VO_S8 *b8mode = pDecLocal->b8mode;
  VO_S8 *b8pdir = pDecLocal->b8pdir;
  for (block8x8=0; block8x8<4; block8x8++)
  {
    int mv_mode  = b8mode[block8x8];
    int pred_dir = b8pdir[block8x8];
	i = (block8x8&1)<<1;
	j = (block8x8>>1)<<1;

    if ( mv_mode != 0 )
    {
      int k_start = (block8x8 << 2);
      int k_inc = (mv_mode == SMB8x4) ? 2 : 1;
      int k_end = (mv_mode == SMB8x8) ? k_start + 1 : ((mv_mode == SMB4x4) ? k_start + 4 : k_start + k_inc + 1);

      block_size_x = ( mv_mode == SMB8x4 || mv_mode == SMB8x8 ) ? SMB_BLOCK_SIZE : BLOCK_SIZE;
      block_size_y = ( mv_mode == SMB4x8 || mv_mode == SMB8x8 ) ? SMB_BLOCK_SIZE : BLOCK_SIZE;

      for (k = k_start; k < k_end; k += k_inc)
      {
        i =  (decode_block_scan[k] & 3);
        j = ((decode_block_scan[k] >> 2) & 3);
        perform_mc(pDecGlobal,pDecLocal, pred_dir, i, j,mv_mode-1);
      }
    }
    else
    {
      int k_start = (block8x8 << 2);
      int k_end = k_start;

      if (currSlice->active_sps->direct_8x8_inference_flag)
      {
        block_size_x = SMB_BLOCK_SIZE;
        block_size_y = SMB_BLOCK_SIZE;
        k_end ++;
      }
      else
      {
        block_size_x = BLOCK_SIZE;
        block_size_y = BLOCK_SIZE;
        k_end += BLOCK_MULTIPLE;
      }

      for (k = k_start; k < k_end; k ++)
      {
        i =  (decode_block_scan[k] & 3);
        j = ((decode_block_scan[k] >> 2) & 3);
		if (currSlice->direct_spatial_mv_pred_flag)
          {
		    if (ref_cache1[j*4+i] == -1) 
            {
              pred_dir = 0;
            }
            else if (ref_cache0[j*4+i] == -1) 
            {
              pred_dir = 1;
            }
            else               
            {
              pred_dir = 2;
            }
		  }
		if (currSlice->active_sps->direct_8x8_inference_flag)
		{
	      perform_mc(pDecGlobal,pDecLocal, pred_dir, i, j,3);
		}        	
		else
		{
		  
	      perform_mc(pDecGlobal,pDecLocal, pred_dir, i, j,6);
		}
      } 
    }
  }
 
  return 1;
}


int mb_pred_ipcm(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  int i, j, k;
  Slice *currSlice = pDecLocal->p_Slice;
  //VideoParameters *p_Vid = currMB->p_Vid;
  StorablePicture *dec_picture = pDecGlobal->dec_picture;
  int stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iLumaStride<<1:pDecGlobal->iLumaStride;
  VO_U8* dst = dec_picture->imgY+pDecLocal->pix_y*stride+pDecLocal->pix_x;
  VO_S16 *cof = pDecLocal->cof_yuv;
//#if USE_FRAME_THREAD
//  if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//  if (pDecGlobal->nThdNum > 1)
//#endif
//  {
//	  cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//  }
  if(pDecLocal->mb_field)
  {
    dst = dec_picture->imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride+pDecLocal->pix_x;
  }

  for(i = 0; i < MB_BLOCK_SIZE; ++i)
  {
    for(j = 0;j < MB_BLOCK_SIZE ; ++j)
    {
      dst[i*stride+j] = (VO_U8)(*cof++);
    }
  }
  stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iChromaStride<<1:pDecGlobal->iChromaStride;
  //if ((dec_picture->chroma_format_idc != YUV400) && (pDecGlobal->separate_colour_plane_flag == 0))
  {
    for (k = 0; k < 2; ++k)
    {
      dst = dec_picture->imgUV[k]+pDecLocal->pix_c_y*stride+pDecLocal->pix_c_x;
	  if(pDecLocal->mb_field)
	  {
		  dst = dec_picture->imgUV[k]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	  }
      for(i = 0; i < 8; ++i)
      {
        for(j = 0;j < 8; ++j)
        {
          dst[i*stride+j] = (VO_U8)(*cof++);  
        }
      }
    }
  }
  return 1;
}

