#include "global.h"
#include "block.h"
#include "mbuffer.h"
#include "transform.h"
#include "ifunctions.h"

 void itrans_2(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)
{
  //VideoParameters *p_Vid = currMB->p_Vid;
  int j;

  //int transform_pl = (pDecGlobal->separate_colour_plane_flag != 0) ? PLANE_Y /*p_Vid->colour_plane_id*/ : pl;
  int qp_scaled = pDecLocal->qp_scaled[0];

  int qp_per = pDecGlobal->qp_per_matrix[ qp_scaled ];
  int qp_rem = pDecGlobal->qp_rem_matrix[ qp_scaled ];      

  int invLevelScale = pDecGlobal->InvLevelScale4x4_Intra[0][qp_rem][0][0];
  short M4[16];
  VO_S16 *cof = pDecLocal->cof_yuv;
//#if USE_FRAME_THREAD
//  if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//  if (pDecGlobal->nThdNum > 1)
//#endif
//  {
//	  cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//  }
  // horizontal
  for (j=0; j < 4;++j) 
  {
    int ypos = (j<<2)*MB_BLOCK_SIZE;
	int jpos = j<<2;
    M4[jpos]=cof[ypos];
    M4[jpos+1]=cof[ypos+MB_BLOCK_SIZE];
    M4[jpos+2]=cof[ypos+(MB_BLOCK_SIZE<<1)];
    M4[jpos+3]=cof[ypos+MB_BLOCK_SIZE*3];
  }

  ihadamard4x4(M4);

  // vertical
  for (j=0; j < 4;++j) 
  {
    int ypos = (j<<2)*MB_BLOCK_SIZE;
	int jpos = j<<2;
    cof[ypos+0]  = (short)rshift_rnd((( M4[jpos] * invLevelScale) << qp_per), 6);
    cof[ypos+MB_BLOCK_SIZE]  = (short)rshift_rnd((( M4[jpos+1] * invLevelScale) << qp_per), 6);
    cof[ypos+(MB_BLOCK_SIZE<<1)]  = (short)rshift_rnd((( M4[jpos+2] * invLevelScale) << qp_per), 6);
    cof[ypos+MB_BLOCK_SIZE*3] = (short)rshift_rnd((( M4[jpos+3] * invLevelScale) << qp_per), 6);
  }

}

void iTransformChroma(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)// smb is for si sp
{
	Slice *currSlice = pDecLocal->p_Slice;
	StorablePicture *dec_picture = pDecGlobal->dec_picture;
	VO_S32 dst_stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iChromaStride<<1:pDecGlobal->iChromaStride;
	const VO_U8* nz = pDecGlobal->nz_coeff_frame + pDecLocal->mb_xy*24 + 16;
	const VO_S32 offset = pDecLocal->pix_c_y*(dst_stride>>pDecLocal->mb_field)+pDecLocal->pix_c_x;
	int uv; 
	VO_S16 *cof = pDecLocal->cof_yuv + MB_BLOCK_SIZE*MB_BLOCK_SIZE;
//#if USE_FRAME_THREAD
//	if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//	if (pDecGlobal->nThdNum > 1)
//#endif
//	{
//		cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//	}

	if ((pDecLocal->cbp>>4))
	{
		for(uv = 0; uv < 2; ++uv)
		{
			VO_U8 *pDst = dec_picture->imgUV[uv]+offset;
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
}

void iTransformLuma(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal)// smb is for si sp
{
	Slice *currSlice = pDecLocal->p_Slice;
  StorablePicture *dec_picture = pDecGlobal->dec_picture;
  imgpel *curr_img = dec_picture->imgY; //since pl always  = PLANY ,there is no need to do 
  int jj;
  //VO_U8* pSrc = currSlice->mb_pred[0];
  VO_S32 dst_stride = (currSlice->field_pic_flag||pDecLocal->mb_field)?pDecGlobal->iLumaStride<<1:pDecGlobal->iLumaStride;
  VO_U8* pDst = curr_img+pDecLocal->pix_y*(dst_stride>>pDecLocal->mb_field)+pDecLocal->pix_x;
  const VO_U8* nz = pDecGlobal->nz_coeff_frame + pDecLocal->mb_xy*24;
  VO_S16 *cof = pDecLocal->cof_yuv;
//#if USE_FRAME_THREAD
//  if (pDecGlobal->interlace&&pDecGlobal->nThdNum > 1)
//#else
//  if (pDecGlobal->nThdNum > 1)
//#endif
//  {
//	  cof += pDecLocal->mb.x*MB_BLOCK_SIZE*MB_BLOCK_SIZE*3/2;
//  }

  if(!IS_8x8DCT(pDecGlobal->mb_type0_frame_buffer[pDecLocal->mb_xy]))
  {
	for (jj = 4; jj != 0; jj--)
	{
#if 0
		if((*nz++))
			Idct4x4Add(cof, pDst, dst_stride );
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst, dst_stride);
		cof += 16;

		if((*nz++))
			Idct4x4Add(cof, pDst+4, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+4, dst_stride);
		cof += 16;

		if((*nz++))
			Idct4x4Add(cof, pDst+8, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+8, dst_stride);
		cof += 16;

		if((*nz++))
			Idct4x4Add(cof, pDst+12, dst_stride);
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+12, dst_stride);
		cof += 16;
#else
		if((*nz > 1)||(*nz==1&&*cof==0))
			Idct4x4Add(cof, pDst, dst_stride );
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst, dst_stride);
		nz++;
		cof += 16;

		if((*nz > 1)||(*nz==1&&*cof==0))
			Idct4x4Add(cof, pDst+4, dst_stride );
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+4, dst_stride);
		nz++;
		cof += 16;

		if((*nz > 1)||(*nz==1&&*cof==0))
			Idct4x4Add(cof, pDst+8, dst_stride );
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+8, dst_stride);
		nz++;
		cof += 16;

		if((*nz > 1)||(*nz==1&&*cof==0))
			Idct4x4Add(cof, pDst+12, dst_stride );
		else if(*cof)
			Idct4x4DCAdd((*cof+32)>>6, pDst+12, dst_stride);
		nz++;
		cof += 16;
#endif
	  pDst += dst_stride<<2;
	}
  }
  else // 8x8 inverse transform
  {  
    Idct8x8Add(cof, pDst, dst_stride );
    Idct8x8Add(cof+64, pDst+8, dst_stride );
    Idct8x8Add(cof+128, pDst+8*dst_stride, dst_stride );
    Idct8x8Add(cof+192, pDst+8*dst_stride+8, dst_stride );
  }
}
#if !defined(VOARMV7)
//add by Really Yang 20110303
void MCCopy4x4_c( VO_U8 *src, VO_S32 i_src_stride, VO_U8 *dst, VO_S32 i_dst_stride )
{
	CP32(dst,src);
    src += i_src_stride;
    dst += i_dst_stride;
	CP32(dst,src);
	src += i_src_stride;
    dst += i_dst_stride;
	CP32(dst,src);
    src += i_src_stride;
    dst += i_dst_stride;
	CP32(dst,src); 
}
void MCCopy8x8_c( VO_U8 *src, VO_S32 i_src_stride, VO_U8 *dst, VO_S32 i_dst_stride )
{
    CP64(dst,src);
    src += i_src_stride;
    dst += i_dst_stride;
	CP64(dst,src);
    src += i_src_stride;
    dst += i_dst_stride;
	CP64(dst,src);
    src += i_src_stride;
    dst += i_dst_stride;
	CP64(dst,src);
	src += i_src_stride;
    dst += i_dst_stride;
	CP64(dst,src);
    src += i_src_stride;
    dst += i_dst_stride;
	CP64(dst,src);
    src += i_src_stride;
    dst += i_dst_stride;
	CP64(dst,src);
    src += i_src_stride;
    dst += i_dst_stride;
	CP64(dst,src);
}
static void MCCopy16x8_c( VO_U8 *src, VO_S32 i_src_stride, VO_U8 *dst, VO_S32 i_dst_stride )
{
	VO_U32 y;
	for(y = 8;y!=0;y-=4)
	{
    	CP64(dst,src);
		CP64(dst+8,src+8);
    	src += i_src_stride;
    	dst += i_dst_stride;
		CP64(dst,src);
		CP64(dst+8,src+8);
    	src += i_src_stride;
    	dst += i_dst_stride;
		CP64(dst,src);
		CP64(dst+8,src+8);
    	src += i_src_stride;
    	dst += i_dst_stride;
		CP64(dst,src);
		CP64(dst+8,src+8);
		src += i_src_stride;
    	dst += i_dst_stride;
	}
}
void MCCopy16x16_c( VO_U8 *src, VO_S32 i_src_stride, VO_U8 *dst, VO_S32 i_dst_stride )
{
  VO_U32 y;
  for(y = 16;y!=0;y-=4)
  {
    CP64(dst,src);
	CP64(dst+8,src+8);
    src += i_src_stride;
    dst += i_dst_stride;
	CP64(dst,src);
	CP64(dst+8,src+8);
    src += i_src_stride;
    dst += i_dst_stride;
	CP64(dst,src);
	CP64(dst+8,src+8);
    src += i_src_stride;
    dst += i_dst_stride;
	CP64(dst,src);
	CP64(dst+8,src+8);
	src += i_src_stride;
    dst += i_dst_stride;
  }
}
#endif

//end of add

