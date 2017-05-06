#include "global.h"
#include "loopfilter.h"
#include "loop_filter.h"
#include "ifunctions.h"
#include "stdio.h"
#include "stdlib.h"
//extern void DeblockMb              (VideoParameters *p_Vid, StorablePicture *p, int MbQAddr);
extern void DeblockChromaV_ARMV7(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockIntraChromaV_ARMV7(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );
extern void DeblockChromaH_ARMV7(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockIntraChromaH_ARMV7(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );
extern void DeblockLumaV_ARMV7(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockIntraLumaV_ARMV7(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );
extern void DeblockLumaH_ARMV7(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockIntraLumaH_ARMV7(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );


#ifdef VOSSSE3

#ifdef __cplusplus
extern "C" {
#endif 	

extern void vo_deblock_v_luma_8_sse2_aligned_16(VO_U8 *pix, VO_S32 stride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0);
extern void vo_deblock_h_luma_8_sse2_aligned_16(VO_U8 *pix, VO_S32 stride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0);
extern void vo_deblock_v_luma_intra_8_sse2_aligned_16(VO_U8 *pix, VO_S32 stride, VO_S32 alpha, VO_S32 beta);
extern void vo_deblock_h_luma_intra_8_sse2_aligned_16(VO_U8 *pix, VO_S32 stride, VO_S32 alpha, VO_S32 beta);

extern void vo_deblock_v_chroma_8_mmxext_aligned_16(VO_U8 *pix, VO_S32 stride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0);
extern void vo_deblock_h_chroma_8_mmxext_aligned_16(VO_U8 *pix, VO_S32 stride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0);
extern void vo_deblock_v_chroma_intra_8_mmxext_aligned_16(VO_U8 *pix, VO_S32 stride, VO_S32 alpha, VO_S32 beta);			
extern void vo_deblock_h_chroma_intra_8_mmxext_aligned_16(VO_U8 *pix, VO_S32 stride, VO_S32 alpha, VO_S32 beta);

DECLARE_ALIGNED_EXT(16, const xmm_reg,  vo_pb_1)    = { 0x0101010101010101ULL, 0x0101010101010101ULL };
DECLARE_ALIGNED_EXT(16, const xmm_reg,  vo_pb_3)    = { 0x0303030303030303ULL, 0x0303030303030303ULL };
DECLARE_ALIGNED_EXT(16, const xmm_reg,  vo_pb_A1)   = { 0xA1A1A1A1A1A1A1A1ULL, 0xA1A1A1A1A1A1A1A1ULL };

#ifdef __cplusplus
}
#endif 

#endif

static inline VO_U8 Clip255( VO_S32 x )
{
	return (VO_U8)(x&(~255) ? (-x)>>31 : x);
}

#if defined(VOARMV6)
//extern void DeblockChromaV_ARMV6(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockChromaV_ARMV6(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );

//extern void DeblockIntraChromaV_ARMV6(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );
extern void DeblockIntraChromaV_ARMV6(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );


//extern void DeblockChromaH_ARMV6(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockChromaH_ARMV6(VO_U8 *pix1,  VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );

//extern void DeblockIntraChromaH_ARMV6(VO_U8 *pix1, VO_U8 *pix2, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );
extern void DeblockIntraChromaH_ARMV6(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );


extern void DeblockLumaV_ARMV6(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockIntraLumaV_ARMV6(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );
extern void DeblockLumaH_ARMV6(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta, VO_S8 *tc0 );
extern void DeblockIntraLumaH_ARMV6(VO_U8 *pix1, VO_S32 ystride, VO_S32 alpha, VO_S32 beta );
#endif


extern void set_loop_filter_functions_mbaff();
static const VO_U8 alpha_table[76] =
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x05,0x06,0x07,
  0x08,0x09,0x0a,0x0c,0x0d,0x0f,0x11,0x14,0x16,0x19,0x1c,
  0x20,0x24,0x28,0x2d,0x32,0x38,0x3f,0x47,0x50,0x5a,0x65,
  0x71,0x7f,0x90,0xa2,0xb6,0xcb,0xe2,0xff,0xff,0xff,0xff,
  0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
static const VO_U8 beta_table[76] =
{
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x02,0x03,0x03,
  0x03,0x03,0x04,0x04,0x04,0x06,0x06,0x07,0x07,0x08,0x08,
  0x09,0x09,0x0a,0x0a,0x0b,0x0b,0x0c,0x0c,0x0d,0x0d,0x0e,
  0x0e,0x0f,0x0f,0x10,0x10,0x11,0x11,0x12,0x12,0x12,0x12,
  0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12,0x12
};
static const VO_S8 skip_table[76][4] =
{
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x00},
  {0xff,0x00,0x00,0x00},{0xff,0x00,0x00,0x01},{0xff,0x00,0x00,0x01},{0xff,0x00,0x00,0x01},
  {0xff,0x00,0x00,0x01},{0xff,0x00,0x01,0x01},{0xff,0x00,0x01,0x01},{0xff,0x01,0x01,0x01},
  {0xff,0x01,0x01,0x01},{0xff,0x01,0x01,0x01},{0xff,0x01,0x01,0x01},{0xff,0x01,0x01,0x02},
  {0xff,0x01,0x01,0x02},{0xff,0x01,0x01,0x02},{0xff,0x01,0x01,0x02},{0xff,0x01,0x02,0x03},
  {0xff,0x01,0x02,0x03},{0xff,0x02,0x02,0x03},{0xff,0x02,0x02,0x04},{0xff,0x02,0x03,0x04},
  {0xff,0x02,0x03,0x04},{0xff,0x03,0x03,0x05},{0xff,0x03,0x04,0x06},{0xff,0x03,0x04,0x06},
  {0xff,0x04,0x05,0x07},{0xff,0x04,0x05,0x08},{0xff,0x04,0x06,0x09},{0xff,0x05,0x07,0x0a},
  {0xff,0x06,0x08,0x0b},{0xff,0x06,0x08,0x0d},{0xff,0x07,0x0a,0x0e},{0xff,0x08,0x0b,0x10},
  {0xff,0x09,0x0c,0x12},{0xff,0x0a,0x0d,0x14},{0xff,0x0b,0x0f,0x17},{0xff,0x0d,0x11,0x19},
  {0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},
  {0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},
  {0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19},{0xff,0x0d,0x11,0x19}
};

VO_S32 update_deblock_cache(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal,VO_S32 mbaddr,VO_S32 mb_x)
{
  Slice *currSlice = pDecGlobal->pCurSlice;
  VO_S32 mb_xy,cbp;
  VO_S32 top_xy, left_xy[2];
  BlockPos mb; 
  VO_S32 mb_field,left_mb_field;
  VO_S32 mb_aff_frame_flag = currSlice->mb_aff_frame_flag;
  VO_S32* mb_type0_frame_buffer = pDecGlobal->mb_type0_frame_buffer;
  VO_S32 mb_type,top_type,left_type0,left_type1;
  //int list_offset;
  if (mb_aff_frame_flag)
  {
	mb = pDecGlobal->PicPos[mbaddr>>1];
	mb.y = (VO_S16)((mbaddr&1)+(mb.y<<1));
	mb_xy = mb.y*pDecGlobal->PicWidthInMbs+mb.x;	
  }
  else
  {
    mb.y = pDecGlobal->mb_y;
    mb.x = mb_x;
    mb_xy = mbaddr;
  }
  
  //mb_type = mb_type0_frame_buffer[mb_xy];
  pDecLocal->mb_field = mb_field = !!IS_INTERLACED(mb_type0_frame_buffer[mb_xy]);
  top_xy     = mb_xy  - (pDecGlobal->PicWidthInMbs << mb_field);
  left_xy[1] = left_xy[0] = mb_xy-1;
  left_mb_field = mb.x!=0?!!IS_INTERLACED(mb_type0_frame_buffer[left_xy[0]]):0; 
  //list_offset = IS_INTERLACED(mb_type0_frame_buffer[mb_xy])? (mb.y&1)?4:2: 0;
  if(mb_aff_frame_flag)
  {
    
    if(mb.y&1)
	{
      if (left_mb_field != mb_field) 
        left_xy[0] -= pDecGlobal->PicWidthInMbs;
    }
	else
	{
      if(top_xy>=0&&mb_field&&!IS_INTERLACED(mb_type0_frame_buffer[top_xy]))
        top_xy += pDecGlobal->PicWidthInMbs;
      if (left_mb_field != mb_field)
        left_xy[1] += pDecGlobal->PicWidthInMbs;
    }
  }
  mb_type = mb_type0_frame_buffer[mb_xy];
  top_type = top_xy>=0?mb_type0_frame_buffer[top_xy]:0;
  left_type0 = mb_type0_frame_buffer[left_xy[0]];
  left_type1 = mb_type0_frame_buffer[left_xy[1]];
  pDecLocal->mb_xy = mb_xy;
  pDecLocal->top_mb_xy = top_xy;
  pDecLocal->left_mb_xy[0] = left_xy[0];
  pDecLocal->left_mb_xy[1] = left_xy[1];
  pDecLocal->mb = mb;
  
#if(FAST_FILTER)
  if (!mb_aff_frame_flag)
  {
    VO_S32 nQP = pDecGlobal->qp_frame_buffer[mbaddr];
    VO_S32 nQPThresh = pDecGlobal->DFQPThresh;
    if(nQP <= nQPThresh && (mb.x == 0 ||  ((nQP +  pDecGlobal->qp_frame_buffer[left_xy[0]] + 1) >> 1) <= nQPThresh) &&
    (mb.y == 0 || ((nQP +  pDecGlobal->qp_frame_buffer[top_xy] + 1) >> 1) <= nQPThresh))
       return 1;
  }
#endif
  
  pDecLocal->top_type    = top_xy>=0?IS_INTRA0(top_type):0;
  pDecLocal->left_type[0]= IS_INTRA0(left_type0);
  pDecLocal->left_type[1]= IS_INTRA0(left_type1);
  pDecLocal->is_intra_block = IS_INTRA0(mb_type);
  pDecLocal->mb_type0 = mb_type;
  if(pDecLocal->is_intra_block)
    return 0;

  pDecLocal->cbp = cbp = pDecGlobal->cbp_frame_buffer[mb_xy];
  {
  	VO_U8 *nz_coeff_frame = pDecGlobal->nz_coeff_frame;
    VO_U8 *nz_coeff = pDecLocal->nz_coeff;
  CP32(nz_coeff+12, nz_coeff_frame+mb_xy*24);
  CP32(nz_coeff+20, nz_coeff_frame+mb_xy*24+4);
  CP32(nz_coeff+28, nz_coeff_frame+mb_xy*24+8);
  CP32(nz_coeff+36, nz_coeff_frame+mb_xy*24+12);
  
  if(mb.y!=0&&top_xy>=0)
    CP32(nz_coeff+4, nz_coeff_frame+top_xy*24+12);

  if(mb.x!=0)
  {
	nz_coeff[11]= nz_coeff_frame[left_xy[0]*24+3];
	nz_coeff[19]= nz_coeff_frame[left_xy[0]*24+7];
	nz_coeff[27]= nz_coeff_frame[left_xy[0]*24+11];
	nz_coeff[35]= nz_coeff_frame[left_xy[0]*24+15];
  }
  
  assert(currSlice->Transform8x8Mode == currSlice->active_pps->transform_8x8_mode_flag);
  if(currSlice->Transform8x8Mode &&!currSlice->active_pps->entropy_coding_mode_flag)
  {
    VO_U8* nz_coeff = pDecLocal->nz_coeff;
    if(top_xy>=0&&IS_8x8DCT(mb_type0_frame_buffer[top_xy]))
	{
      nz_coeff[4]=nz_coeff[5]= pDecGlobal->cbp_frame_buffer[top_xy] & 4;
      nz_coeff[6]=nz_coeff[7]= pDecGlobal->cbp_frame_buffer[top_xy] & 8;
    }
	
    if(IS_8x8DCT(mb_type0_frame_buffer[left_xy[0]]))
      nz_coeff[11]=nz_coeff[19]= pDecGlobal->cbp_frame_buffer[left_xy[0]]&2; 

    if(IS_8x8DCT(mb_type0_frame_buffer[left_xy[1]]))
      nz_coeff[27]=nz_coeff[35]= pDecGlobal->cbp_frame_buffer[left_xy[1]]&8; 

    if(IS_8x8DCT(mb_type))
	{
      nz_coeff[12]=nz_coeff[13]=nz_coeff[20]=nz_coeff[21]= cbp & 1;  
      nz_coeff[14]=nz_coeff[15]=nz_coeff[22]=nz_coeff[23]= cbp & 2;
      nz_coeff[28]=nz_coeff[29]=nz_coeff[36]=nz_coeff[37]= cbp & 4;
      nz_coeff[30]=nz_coeff[31]=nz_coeff[38]=nz_coeff[39]= cbp & 8;
    }
  }
  }
  //load ref_pic and mv for current MB
  {
    VO_S32* mv_out;
	//VO_S8* ref_out;
	VO_S32 i;
	StorablePicture** pic_buffer_list;
	VO_S32* mv_cache = pDecLocal->mv_cache[0];
	StorablePicture** pic_cache = pDecLocal->pic_cache[0];
	//StorablePicture **list = currSlice->listX[LIST_0+list_offset];
	//if(mb_type&(VO_16x16|VO_16x8|VO_8x16|VO_8x8|VO_DIRECT))
	//{
    if((mb.y>1||(mb.y==1&&!mb_field))&&USES_LIST(top_type,0))
    {
      mv_out = pDecGlobal->dec_picture->mv[0]+top_xy*16+12;
	  //ref_out = pDecGlobal->dec_picture->ref_idx[0]+top_xy*16+12;
      
	  mv_cache[4]=mv_out[0];
	  mv_cache[5]=mv_out[1];
	  mv_cache[6]=mv_out[2];
	  mv_cache[7]=mv_out[3];
	  if(pDecGlobal->interlace)
	  {
	    pic_buffer_list = pDecGlobal->dec_picture->ref_pic[0]+top_xy*16+12;
	    pic_cache[4]=pic_buffer_list[0];//ref_out[0]>=0?list[ref_out[0]]:0;
	    pic_cache[5]=pic_buffer_list[1];//ref_out[1]>=0?list[ref_out[1]]:0;
	    pic_cache[6]=pic_buffer_list[2];//ref_out[2]>=0?list[ref_out[2]]:0;
	    pic_cache[7]=pic_buffer_list[3];//ref_out[3]>=0?list[ref_out[3]]:0;
	  }
	  else
	  {
	    VO_S8 *ref = pDecGlobal->dec_picture->ref_idx[0]+top_xy*16+12;
		VO_S32 slice_number = pDecGlobal->slice_number_frame[top_xy];
		pic_cache[4]=ref[0]>=0?currSlice->listX[slice_number][0][ref[0]]:NULL;
	    pic_cache[5]=ref[1]>=0?currSlice->listX[slice_number][0][ref[1]]:NULL;
	    pic_cache[6]=ref[2]>=0?currSlice->listX[slice_number][0][ref[2]]:NULL;
	    pic_cache[7]=ref[3]>=0?currSlice->listX[slice_number][0][ref[3]]:NULL;
	  }
    }
	else
	{
	  M64(&mv_cache[4])=M64(&mv_cache[6])=0;
	  pic_cache[4]=pic_cache[5]=pic_cache[6]=pic_cache[7]=NULL;
	}
	if(mb.x!=0&&USES_LIST(left_type0,0))
    {
      mv_out = pDecGlobal->dec_picture->mv[0]+left_xy[0]*16;
	  mv_cache[11]=mv_out[3];
	  mv_cache[19]=mv_out[7];
	  mv_cache[27]=mv_out[11];
	  mv_cache[35]=mv_out[15];
	  if(pDecGlobal->interlace)
	  {
	    pic_buffer_list = pDecGlobal->dec_picture->ref_pic[0]+left_xy[0]*16;
	    pic_cache[11]=pic_buffer_list[3];
	    pic_cache[19]=pic_buffer_list[7];
	    pic_cache[27]=pic_buffer_list[11];
	    pic_cache[35]=pic_buffer_list[15];
	  }
	  else
	  {
	    VO_S8 *ref = pDecGlobal->dec_picture->ref_idx[0]+left_xy[0]*16;
		VO_S32 slice_number = pDecGlobal->slice_number_frame[left_xy[0]];
		pic_cache[11]=ref[3]>=0?currSlice->listX[slice_number][0][ref[3]]:NULL;
	    pic_cache[19]=ref[7]>=0?currSlice->listX[slice_number][0][ref[7]]:NULL;
	    pic_cache[27]=ref[11]>=0?currSlice->listX[slice_number][0][ref[11]]:NULL;
	    pic_cache[35]=ref[15]>=0?currSlice->listX[slice_number][0][ref[15]]:NULL;
	  }
    }
	else
	{
	   mv_cache[11]= mv_cache[19]= mv_cache[27]= mv_cache[35]=0;
	   pic_cache[11]=pic_cache[19]=pic_cache[27]=pic_cache[35]=NULL;
	}
	//}
	mv_out = pDecGlobal->dec_picture->mv[0]+mb_xy*16;
    if(pDecGlobal->interlace)
	{
      pic_buffer_list = pDecGlobal->dec_picture->ref_pic[0]+mb_xy*16;
	  for(i = 0;i<4;i++)
      {
  	    mv_cache[12+(i<<3)] = *mv_out++;
	    mv_cache[13+(i<<3)] = *mv_out++;
	    mv_cache[14+(i<<3)] = *mv_out++;
	    mv_cache[15+(i<<3)] = *mv_out++;
	    pic_cache[12+(i<<3)] = *pic_buffer_list++;
	    pic_cache[13+(i<<3)] = *pic_buffer_list++;
	    pic_cache[14+(i<<3)] = *pic_buffer_list++;
	    pic_cache[15+(i<<3)] = *pic_buffer_list++;
	  }
    }
	else
	{
	  VO_S8 *ref = pDecGlobal->dec_picture->ref_idx[0]+mb_xy*16;
	  VO_S32 slice_number = pDecGlobal->slice_number_frame[mb_xy];
	  for(i = 0;i<4;i++)
      {
  	    mv_cache[12+(i<<3)] = *mv_out++;
	    mv_cache[13+(i<<3)] = *mv_out++;
	    mv_cache[14+(i<<3)] = *mv_out++;
	    mv_cache[15+(i<<3)] = *mv_out++;
	    pic_cache[12+(i<<3)] = ref[0]>=0?currSlice->listX[slice_number][0][ref[0]]:NULL;
	    pic_cache[13+(i<<3)] = ref[1]>=0?currSlice->listX[slice_number][0][ref[1]]:NULL;
	    pic_cache[14+(i<<3)] = ref[2]>=0?currSlice->listX[slice_number][0][ref[2]]:NULL;
	    pic_cache[15+(i<<3)] = ref[3]>=0?currSlice->listX[slice_number][0][ref[3]]:NULL;
		ref+=4;
	  }
	}
  }
  if(pDecGlobal->type == B_SLICE)
  {
    VO_S32* mv_out;
	VO_S32 i;
	StorablePicture** pic_buffer_list;
	VO_S32* mv_cache = pDecLocal->mv_cache[1];
	StorablePicture** pic_cache = pDecLocal->pic_cache[1];
	//if(mb_type&(VO_16x16|VO_16x8|VO_8x16|VO_8x8|VO_DIRECT))
	//{
    if((mb.y>1||(mb.y==1&&!mb_field))&&USES_LIST(top_type,1))
    {
      mv_out = pDecGlobal->dec_picture->mv[1]+top_xy*16+12;      
	  mv_cache[4]=mv_out[0];
	  mv_cache[5]=mv_out[1];
	  mv_cache[6]=mv_out[2];
	  mv_cache[7]=mv_out[3];
	  if(pDecGlobal->interlace)
	  {
	    pic_buffer_list = pDecGlobal->dec_picture->ref_pic[1]+top_xy*16+12;
	    pic_cache[4]=pic_buffer_list[0];//ref_out[0]>=0?list[ref_out[0]]:0;
	    pic_cache[5]=pic_buffer_list[1];//ref_out[1]>=0?list[ref_out[1]]:0;
	    pic_cache[6]=pic_buffer_list[2];//ref_out[2]>=0?list[ref_out[2]]:0;
	    pic_cache[7]=pic_buffer_list[3];//ref_out[3]>=0?list[ref_out[3]]:0;
	  }
	  else
	  {
	    VO_S8 *ref = pDecGlobal->dec_picture->ref_idx[1]+top_xy*16+12;
		VO_S32 slice_number = pDecGlobal->slice_number_frame[top_xy];
		pic_cache[4]=ref[0]>=0?currSlice->listX[slice_number][1][ref[0]]:NULL;
	    pic_cache[5]=ref[1]>=0?currSlice->listX[slice_number][1][ref[1]]:NULL;
	    pic_cache[6]=ref[2]>=0?currSlice->listX[slice_number][1][ref[2]]:NULL;
	    pic_cache[7]=ref[3]>=0?currSlice->listX[slice_number][1][ref[3]]:NULL;
	  }
    }
	else
	{
	  M64(&mv_cache[4])=M64(&mv_cache[6])=0;
	  pic_cache[4]=pic_cache[5]=pic_cache[6]=pic_cache[7]=NULL;
	}
	if(mb.x!=0&&USES_LIST(left_type1,1))
    {
      mv_out = pDecGlobal->dec_picture->mv[1]+left_xy[0]*16;
	  mv_cache[11]=mv_out[3];
	  mv_cache[19]=mv_out[7];
	  mv_cache[27]=mv_out[11];
	  mv_cache[35]=mv_out[15];
	  if(pDecGlobal->interlace)
	  {
	    pic_buffer_list = pDecGlobal->dec_picture->ref_pic[1]+left_xy[0]*16;
	    pic_cache[11]=pic_buffer_list[3];
	    pic_cache[19]=pic_buffer_list[7];
	    pic_cache[27]=pic_buffer_list[11];
	    pic_cache[35]=pic_buffer_list[15];
	  }
	  else
	  {
	    VO_S8 *ref = pDecGlobal->dec_picture->ref_idx[1]+left_xy[0]*16;
		VO_S32 slice_number = pDecGlobal->slice_number_frame[left_xy[0]];
		pic_cache[11]=ref[3]>=0?currSlice->listX[slice_number][1][ref[3]]:NULL;
	    pic_cache[19]=ref[7]>=0?currSlice->listX[slice_number][1][ref[7]]:NULL;
	    pic_cache[27]=ref[11]>=0?currSlice->listX[slice_number][1][ref[11]]:NULL;
	    pic_cache[35]=ref[15]>=0?currSlice->listX[slice_number][1][ref[15]]:NULL;
	  }
    }
	else
	{
	   mv_cache[11]= mv_cache[19]= mv_cache[27]= mv_cache[35]=0;
	   pic_cache[11]=pic_cache[19]=pic_cache[27]=pic_cache[35]=NULL;
	}
	//}
	mv_out = pDecGlobal->dec_picture->mv[1]+mb_xy*16;
    if(pDecGlobal->interlace)
	{
      pic_buffer_list = pDecGlobal->dec_picture->ref_pic[1]+mb_xy*16;
	  for(i = 0;i<4;i++)
      {
  	    mv_cache[12+(i<<3)] = *mv_out++;
	    mv_cache[13+(i<<3)] = *mv_out++;
	    mv_cache[14+(i<<3)] = *mv_out++;
	    mv_cache[15+(i<<3)] = *mv_out++;
	    pic_cache[12+(i<<3)] = *pic_buffer_list++;
	    pic_cache[13+(i<<3)] = *pic_buffer_list++;
	    pic_cache[14+(i<<3)] = *pic_buffer_list++;
	    pic_cache[15+(i<<3)] = *pic_buffer_list++;
	  }
    }
	else
	{
	  VO_S8 *ref = pDecGlobal->dec_picture->ref_idx[1]+mb_xy*16;
	  VO_S32 slice_number = pDecGlobal->slice_number_frame[mb_xy];
	  for(i = 0;i<4;i++)
      {
  	    mv_cache[12+(i<<3)] = *mv_out++;
	    mv_cache[13+(i<<3)] = *mv_out++;
	    mv_cache[14+(i<<3)] = *mv_out++;
	    mv_cache[15+(i<<3)] = *mv_out++;
	    pic_cache[12+(i<<3)] = ref[0]>=0?currSlice->listX[slice_number][1][ref[0]]:NULL;
	    pic_cache[13+(i<<3)] = ref[1]>=0?currSlice->listX[slice_number][1][ref[1]]:NULL;
	    pic_cache[14+(i<<3)] = ref[2]>=0?currSlice->listX[slice_number][1][ref[2]]:NULL;
	    pic_cache[15+(i<<3)] = ref[3]>=0?currSlice->listX[slice_number][1][ref[3]]:NULL;
		ref+=4;
	  }
	}
  }
  return 0;
}


#if !(defined(VOARMV7)||defined(VOARMV6))
static void DeblockLuma( VO_U8 *src, VO_S32 step, VO_S32 stride, VO_S32 alpha, VO_S32 beta, VO_S8 *skip )
{
  VO_U32 i, j;
  VO_S32 l2,l1,l0,r0,r1,r2;
  VO_S32 delta_luma;
  VO_S32 threshold;
  for( i = 0; i < 4; i++ )
  {
	if( skip[i] < 0 )
	{
	  src += 4*stride;
	}
	else
	{
	  for( j = 4; j != 0; j-- )
	  {
		l2 = src[-3*step];l1 = src[-2*step];l0 = src[-1*step];
		r0 = src[ 0*step];r1 = src[ 1*step];r2 = src[ 2*step];

		if( iabs( l0 - r0 ) < alpha && iabs( l1 - l0 ) < beta && iabs( r1 - r0 ) < beta )
		{
		  threshold = skip[i];					
		  if( iabs( l2 - l0 ) < beta )
		  {
			src[-2*step] = (VO_U8)(l1 + iClip3( -skip[i], skip[i],(( l2 + ((l0 + r0 + 1) >> 1)) >> 1) - l1  ));
			threshold++;
		  }
		  if( iabs( r2 - r0 ) < beta )
		  {
			src[ 1*step] = (VO_U8)(r1 + iClip3( -skip[i], skip[i],(( r2 + ((l0 + r0 + 1) >> 1)) >> 1) - r1  ));
			threshold++;
		  }
		  delta_luma = iClip3( -threshold, threshold,(((r0 - l0 ) << 2) + (l1 - r1) + 4) >> 3 );
		  src[-1*step] = Clip255( l0 + delta_luma );	
		  src[ 0*step] = Clip255( r0 - delta_luma );	
		}
		src += stride;
	  }
	}
  }
}
#endif
static void DeblockLumaH( H264DEC_G *pDecGlobal, VO_U8 *src, VO_S32 src_stride,VO_U8 *strength, VO_S32 nQP )
{
  VO_S32 index_alpha = nQP + pDecGlobal->DFAlphaC0Offset+12;
  VO_S32 alpha = alpha_table[index_alpha];
  VO_S32 beta  = beta_table[nQP + pDecGlobal->DFBetaOffset+12];
  VO_S8 skip[4];
	
  if( alpha && beta )
  {
	const VO_S8* st = skip_table[index_alpha];
	skip[0] = st[strength[0]];
	skip[1] = st[strength[1]];
	skip[2] = st[strength[2]];
	skip[3] = st[strength[3]];
#if defined(VOARMV7)
	DeblockLumaH_ARMV7( src, src_stride, alpha, beta, skip );
#elif defined(VOARMV6)
	DeblockLumaH_ARMV6( src, src_stride, alpha, beta, skip );
#elif defined(VOSSSE3)
	vo_deblock_h_luma_8_sse2_aligned_16(src, src_stride, alpha, beta, skip);
#else
	DeblockLuma( src, 1, src_stride, alpha, beta, skip);
#endif
  }
}
static void DeblockLumaV( H264DEC_G *pDecGlobal, VO_U8 *src, VO_S32 src_stride,VO_U8 *strength, VO_S32 nQP )
{
  VO_S32 index_alpha = nQP + pDecGlobal->DFAlphaC0Offset+12;
  VO_S32 alpha = alpha_table[index_alpha];
  VO_S32 beta  = beta_table[nQP + pDecGlobal->DFBetaOffset+12];
  VO_S8 skip[4];

  if( alpha && beta )
  {
    const VO_S8* st = skip_table[index_alpha];
    skip[0] = st[strength[0]];
	skip[1] = st[strength[1]];
    skip[2] = st[strength[2]];
	skip[3] = st[strength[3]];
#if defined(VOARMV7)
	DeblockLumaV_ARMV7( src, src_stride, alpha, beta, skip );
#elif defined(VOARMV6)
	DeblockLumaV_ARMV6( src, src_stride, alpha, beta, skip );
#elif defined(VOSSSE3)
	vo_deblock_v_luma_8_sse2_aligned_16(src, src_stride, alpha, beta, skip);
#else
    DeblockLuma( src, src_stride, 1, alpha, beta, skip );
#endif
  }
}
#if !(defined(VOARMV7)||defined(VOARMV6))
static void DeblockChroma( VO_U8 *src, VO_S32 step, VO_S32 stride, VO_S32 alpha, VO_S32 beta, VO_S8 *skip )
{
  VO_U32 i, j;
  VO_S32 l1,l0,r0,r1;
  for( i = 0; i < 4; i++ )
  {
    if( skip[i] <= 0 )
    {
      src += 2*stride;
    }
	else
    {
	  for( j = 2; j != 0; j-- )
	  {
	    l1 = src[-2*step];l0 = src[-1*step];
	    r0 = src[ 0*step];r1 = src[ 1*step];

	    if( iabs( l0 - r0 ) < alpha && iabs( l1 - l0 ) < beta && iabs( r1 - r0 ) < beta )
	    {
	      VO_S32 delta = iClip3( -skip[i], skip[i],(((r0 - l0 ) << 2) + (l1 - r1) + 4) >> 3 );
	      src[-1*step] = Clip255( l0 + delta );    
	      src[ 0*step] = Clip255( r0 - delta );    
	    }
	    src += stride;
	  }
    }
  }
}
#endif
static void DeblockChromaH( H264DEC_G *pDecGlobal, VO_U8 *src1,VO_U8 *src2, VO_S32 src_stride,VO_U8 *strength, VO_S32 nQP )
{
  VO_S32 index_alpha = nQP + pDecGlobal->DFAlphaC0Offset+12;
  VO_S32 alpha = alpha_table[index_alpha];
  VO_S32 beta  = beta_table[nQP + pDecGlobal->DFBetaOffset+12];
  VO_S8 skip[4];

  if( alpha && beta )
  {
   	const VO_S8* st = skip_table[index_alpha];
   	skip[0] = st[strength[0]]+1;
	skip[1] = st[strength[1]]+1;
   	skip[2] = st[strength[2]]+1;
	skip[3] = st[strength[3]]+1;
#if defined(VOARMV7)
	DeblockChromaH_ARMV7( src1,src2, src_stride, alpha, beta, skip );
#elif defined(VOARMV6)
	DeblockChromaH_ARMV6( src1, src_stride, alpha, beta, skip );
       DeblockChromaH_ARMV6( src2, src_stride, alpha, beta, skip );
#elif defined(VOSSSE3)
	vo_deblock_h_chroma_8_mmxext_aligned_16(src1, src_stride, alpha, beta, skip);
	vo_deblock_h_chroma_8_mmxext_aligned_16(src2, src_stride, alpha, beta, skip);
#else
    DeblockChroma( src1, 1, src_stride, alpha, beta, skip );
	DeblockChroma( src2, 1, src_stride, alpha, beta, skip );
#endif
  }
}
static void DeblockChromaV( H264DEC_G *pDecGlobal, VO_U8 *src1,VO_U8 *src2, VO_S32 src_stride,VO_U8 *strength, VO_S32 nQP )
{
    VO_S32 index_alpha = nQP + pDecGlobal->DFAlphaC0Offset+12;
	VO_S32 alpha = alpha_table[index_alpha];
	VO_S32 beta  = beta_table[nQP + pDecGlobal->DFBetaOffset+12];
    VO_S8 skip[4];

    if( alpha && beta )
    {
	    const VO_S8* st = skip_table[index_alpha];
    	skip[0] = st[strength[0]]+1;
		skip[1] = st[strength[1]]+1;
    	skip[2] = st[strength[2]]+1;
		skip[3] = st[strength[3]]+1;
#if defined(VOARMV7)
		DeblockChromaV_ARMV7( src1,src2, src_stride, alpha, beta, skip );
#elif defined(VOARMV6)
		DeblockChromaV_ARMV6( src1,src_stride, alpha, beta, skip );
        DeblockChromaV_ARMV6( src2,src_stride, alpha, beta, skip );
#elif defined(VOSSSE3)
		vo_deblock_v_chroma_8_mmxext_aligned_16(src1, src_stride, alpha, beta, skip);
		vo_deblock_v_chroma_8_mmxext_aligned_16(src2, src_stride, alpha, beta, skip);
#else
    	DeblockChroma( src1, src_stride, 1, alpha, beta, skip );
		DeblockChroma( src2, src_stride, 1, alpha, beta, skip );
#endif
    }
}
#if !(defined(VOARMV7)||defined(VOARMV6))
static void DeblocIntraLuma( VO_U8 *src, VO_S32 step, VO_S32 stride, VO_S32 alpha, VO_S32 beta )
{
  VO_U32 i;
  VO_S32 l3,l2,l1,l0,r0,r1,r2,r3;

  for( i = 16; i != 0; i-- )
  {
	l2 = src[-3*step];l1 = src[-2*step];l0 = src[-1*step];
	r0 = src[ 0*step];r1 = src[ 1*step];r2 = src[ 2*step];
	
	if( iabs( l0 - r0 ) < alpha && iabs( l1 - l0 ) < beta && iabs( r1 - r0 ) < beta )
	{
	  if(iabs( l0 - r0 ) < ((alpha >> 2) + 2) )
	  {
		if( iabs( l2 - l0 ) < beta ) 
		{
		  l3 = src[-4*step];
		  src[-1*step] = (VO_U8)(( l2 + 2*l1 + 2*l0 + 2*r0 + r1 + 4 ) >> 3);
		  src[-2*step] = (VO_U8)(( l2 + l1 + l0 + r0 + 2 ) >> 2);
		  src[-3*step] = (VO_U8)(( 2*l3 + 3*l2 + l1 + l0 + r0 + 4 ) >> 3);
		}
		else 
		  src[-1*step] = (VO_U8)(( 2*l1 + l0 + r1 + 2 ) >> 2);
		if( iabs( r2 - r0 ) < beta ) 
		{
		  r3 = src[3*step];
		  src[0*step] = (VO_U8)(( l1 + 2*l0 + 2*r0 + 2*r1 + r2 + 4 ) >> 3);
		  src[1*step] = (VO_U8)(( l0 + r0 + r1 + r2 + 2 ) >> 2);
		  src[2*step] = (VO_U8)(( 2*r3 + 3*r2 + r1 + r0 + l0 + 4 ) >> 3);
		}
		else /* q0' */
		  src[0*step] = (VO_U8)(( 2*r1 + r0 + l1 + 2 ) >> 2);
	  }
	  else /* p0', q0' */
	  {
		src[-1*step] = (VO_U8)(( 2*l1 + l0 + r1 + 2 ) >> 2);
		src[ 0*step] = (VO_U8)(( 2*r1 + r0 + l1 + 2 ) >> 2);
	  }
	}
	src += stride;
  }
}
#endif
static void DeblocIntraLumaH( H264DEC_G *pDecGlobal, VO_U8 *src, VO_S32 src_stride, VO_S32 nQP )
{
  VO_S32 alpha = alpha_table[nQP + pDecGlobal->DFAlphaC0Offset+12];
  VO_S32 beta  = beta_table[nQP + pDecGlobal->DFBetaOffset+12];
  if( alpha && beta )
  {
#if defined(VOARMV7)
	DeblockIntraLumaH_ARMV7( src, src_stride, alpha, beta);
#elif defined(VOARMV6)
	DeblockIntraLumaH_ARMV6( src, src_stride, alpha, beta);
#elif defined(VOSSSE3)
	vo_deblock_h_luma_intra_8_sse2_aligned_16(src,src_stride, alpha, beta);
	//DeblocIntraLuma( src, 1, src_stride, alpha, beta );
#else
	DeblocIntraLuma( src, 1, src_stride, alpha, beta );
#endif
  }
}
static void DeblocIntraLumaV( H264DEC_G *pDecGlobal, VO_U8 *src, VO_S32 src_stride, VO_S32 nQP )
{
  VO_S32 alpha = alpha_table[nQP + pDecGlobal->DFAlphaC0Offset+12];
  VO_S32 beta  = beta_table[nQP + pDecGlobal->DFBetaOffset+12];
  if( alpha && beta)
  {
#if defined(VOARMV7)
	DeblockIntraLumaV_ARMV7( src, src_stride, alpha, beta);
#elif defined(VOARMV6)
	DeblockIntraLumaV_ARMV6( src, src_stride, alpha, beta);
#elif defined(VOSSSE3)
	vo_deblock_v_luma_intra_8_sse2_aligned_16(src,src_stride, alpha, beta);
	//DeblocIntraLuma( src, src_stride,1 , alpha, beta );
#else
	DeblocIntraLuma( src, src_stride,1 , alpha, beta );
#endif
  }
}
#if !(defined(VOARMV7)||defined(VOARMV6))
static void DeblocIntraChroma( VO_U8 *src, VO_S32 step, VO_S32 stride, VO_S32 alpha, VO_S32 beta )
{
  VO_U32 i;
  VO_S32 l1,l0,r0,r1;
  for( i = 8; i != 0; i-- )
  {
    l1 = src[-2*step];l0 = src[-1*step];
    r0 = src[ 0*step];r1 = src[ 1*step];

    if( iabs( l0 - r0 ) < alpha && iabs( l1 - l0 ) < beta && iabs( r1 - r0 ) < beta )
    {
      src[-1*step] = (VO_U8)((2*l1 + l0 + r1 + 2) >> 2);   
      src[ 0*step] = (VO_U8)((2*r1 + r0 + l1 + 2) >> 2);   
    }
    src += stride;
  }
}
#endif
static void DeblocIntraChromaH( H264DEC_G *pDecGlobal, VO_U8 *src1, VO_U8 *src2, VO_S32 src_stride, VO_S32 nQP )
{
  VO_S32 alpha = alpha_table[nQP + pDecGlobal->DFAlphaC0Offset+12];
  VO_S32 beta  = beta_table[nQP + pDecGlobal->DFBetaOffset+12];

  if( alpha && beta )
  {
#if defined(VOARMV7)
	DeblockIntraChromaH_ARMV7( src1,src2, src_stride, alpha, beta);
#elif defined(VOARMV6)
	DeblockIntraChromaH_ARMV6( src1, src_stride, alpha, beta);
    DeblockIntraChromaH_ARMV6( src2, src_stride, alpha, beta);
#elif defined(VOSSSE3)
	  vo_deblock_h_chroma_intra_8_mmxext_aligned_16(src1, src_stride, alpha, beta);
	  vo_deblock_h_chroma_intra_8_mmxext_aligned_16(src2, src_stride, alpha, beta);
#else
    DeblocIntraChroma( src1, 1 ,src_stride, alpha, beta );
	DeblocIntraChroma( src2, 1 ,src_stride, alpha, beta );
#endif
  }
}
static void DeblocIntraChromaV( H264DEC_G *pDecGlobal, VO_U8 *src1, VO_U8 *src2, VO_S32 src_stride, VO_S32 nQP )
{
  VO_S32 alpha = alpha_table[nQP + pDecGlobal->DFAlphaC0Offset+12];
  VO_S32 beta  = beta_table[nQP + pDecGlobal->DFBetaOffset+12];
  if( alpha && beta )
  {
#if defined(VOARMV7)
	DeblockIntraChromaV_ARMV7( src1,src2, src_stride, alpha, beta);
#elif defined(VOARMV6)
	DeblockIntraChromaV_ARMV6( src1, src_stride, alpha, beta);
    DeblockIntraChromaV_ARMV6( src2, src_stride, alpha, beta);
#elif defined(VOSSSE3)
	  vo_deblock_v_chroma_intra_8_mmxext_aligned_16(src1, src_stride, alpha, beta);
	  vo_deblock_v_chroma_intra_8_mmxext_aligned_16(src2, src_stride, alpha, beta);

	  //DeblocIntraChroma( src1, src_stride,1 , alpha, beta );
	  //DeblocIntraChroma( src2, src_stride,1 , alpha, beta );
#else
    DeblocIntraChroma( src1, src_stride,1 , alpha, beta );
	DeblocIntraChroma( src2, src_stride,1 , alpha, beta );
#endif
  }
}

static void DeblockLumaV_mbaff( H264DEC_G *pDecGlobal,  VO_U8 *src, VO_S32 src_stride, VO_S16 strength[4], int step, int qp ) 
{
  int i;
  int index_alpha = qp + pDecGlobal->DFAlphaC0Offset+12;
  int alpha = alpha_table[index_alpha];
  int beta  = beta_table[qp + pDecGlobal->DFBetaOffset+12];
  const VO_S8* st = skip_table[index_alpha];
  for( i = 0; i < 8; i++, src += src_stride) 
  {
    const int bS_index = (i >> 1) * step;

    if( strength[bS_index] == 0 )
      continue;

    if( strength[bS_index] < 4 ) 
	{
      VO_S32 skip0 = st[strength[bS_index]];
      VO_S32 p0 = src[-1];VO_S32 p1 = src[-2];VO_S32 p2 = src[-3];
      VO_S32 q0 = src[0];VO_S32 q1 = src[1];VO_S32 q2 = src[2];

      if( iabs( p0 - q0 ) < alpha && iabs( p1 - p0 ) < beta && iabs( q1 - q0 ) < beta ) 
	  {
        VO_S32 threshold = skip0;
        VO_S32 delta_luma;

        if( iabs( p2 - p0 ) < beta ) 
		{
          if(skip0)
            src[-2] = (VO_U8)(p1 + iClip3( -skip0, skip0,( p2 + ( ( p0 + q0 + 1 ) >> 1 ) - ( p1 << 1 ) ) >> 1  ));
          threshold++;
        }
        if( iabs( q2 - q0 ) < beta ) 
		{
          if(skip0)
            src[1] = (VO_U8)(q1 + iClip3( -skip0, skip0,( q2 + ( ( p0 + q0 + 1 ) >> 1 ) - ( q1 << 1 ) ) >> 1 ));
          threshold++;
        }

        delta_luma = iClip3( -threshold, threshold,(((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3 );
        src[-1] = Clip255( p0 + delta_luma );    
        src[0]  = Clip255( q0 - delta_luma );  
      }
    }
	else
	{
      VO_S32 p0 = src[-1];VO_S32 p1 = src[-2]; VO_S32 p2 = src[-3];
	  VO_S32 q0 = src[0];VO_S32 q1 = src[1];VO_S32 q2 = src[2];

      if( iabs( p0 - q0 ) < alpha && iabs( p1 - p0 ) < beta && iabs( q1 - q0 ) < beta ) 
	  {
        if(iabs( p0 - q0 ) < (( alpha >> 2 ) + 2 ))
		{
          if( iabs( p2 - p0 ) < beta)
          {
            VO_S32 p3 = src[-4];
            src[-1] = (VO_U8)(( p2 + 2*p1 + 2*p0 + 2*q0 + q1 + 4 ) >> 3);
            src[-2] = (VO_U8)(( p2 + p1 + p0 + q0 + 2 ) >> 2);
            src[-3] = (VO_U8)(( 2*p3 + 3*p2 + p1 + p0 + q0 + 4 ) >> 3);
          } 
		  else 
		  {
            src[-1] = (VO_U8)(( 2*p1 + p0 + q1 + 2 ) >> 2);
          }
          if( iabs( q2 - q0 ) < beta)
          {
            VO_S32 q3 = src[3];
            src[0] = (VO_U8)(( p1 + 2*p0 + 2*q0 + 2*q1 + q2 + 4 ) >> 3);
            src[1] = (VO_U8)(( p0 + q0 + q1 + q2 + 2 ) >> 2);
            src[2] = (VO_U8)(( 2*q3 + 3*q2 + q1 + q0 + p0 + 4 ) >> 3);
          } 
		  else 
		  {
            src[0] = (VO_U8)(( 2*q1 + q0 + p1 + 2 ) >> 2);
          }
        }
		else
		{
          src[-1] = (VO_U8)(( 2*p1 + p0 + q1 + 2 ) >> 2);
          src[ 0] = (VO_U8)(( 2*q1 + q0 + p1 + 2 ) >> 2);
        }
      }
    }
  }
}

static void DeblockChromaV_mbaff( H264DEC_G *pDecGlobal,  VO_U8 *src, VO_S32 src_stride, VO_S16 strength[4], int step, int qp ) 
{
  int i;
  int index_alpha = qp + pDecGlobal->DFAlphaC0Offset+12;
  int alpha = alpha_table[index_alpha];
  int beta  = beta_table[qp + pDecGlobal->DFBetaOffset+12];
  const VO_S8* st = skip_table[index_alpha];
  for( i = 0; i < 4; i++, src += src_stride) 
  {
    const int strength_index = i*step;

    if( strength[strength_index] == 0 )
      continue;

    if( strength[strength_index] < 4 ) 
	{
      VO_S32 skip0 = st[strength[strength_index]]+1;
      VO_S32 p0 = src[-1];VO_S32 p1 = src[-2];
      VO_S32 q0 = src[0];VO_S32 q1 = src[1];

      if( iabs( p0 - q0 ) < alpha && iabs( p1 - p0 ) < beta && iabs( q1 - q0 ) < beta ) 
	  {
        VO_S32 delta_chroma = iClip3( -skip0, skip0, (((q0 - p0 ) << 2) + (p1 - q1) + 4) >> 3 );
	    src[-1] = Clip255( p0 + delta_chroma );   
         src[0]  = Clip255( q0 - delta_chroma );    
      }
    }
	else
	{
      VO_S32 p0 = src[-1];VO_S32 p1 = src[-2];
      VO_S32 q0 = src[0];VO_S32 q1 = src[1];

      if( iabs( p0 - q0 ) < alpha && iabs( p1 - p0 ) < beta && iabs( q1 - q0 ) < beta ) 
	  {
        src[-1] = (VO_U8)(( 2*p1 + p0 + q1 + 2 ) >> 2);  
        src[0]  = (VO_U8)(( 2*q1 + q0 + p1 + 2 ) >> 2);  
      }
    }
  }
}

static int strength_mv(H264DEC_L *pDecLocal,int idx_cur,int idx_nei,int mvlimit,int b_slice)
{
  int StrValue;
  VO_S32 ref_p0 = (VO_S32)pDecLocal->pic_cache[LIST_0][idx_cur];            
  VO_S32 ref_q0 = (VO_S32)pDecLocal->pic_cache[LIST_0][idx_nei];            
  short* mv_p0 = (short*)&pDecLocal->mv_cache[LIST_0][idx_cur];			
  short* mv_q0 = (short*)&pDecLocal->mv_cache[LIST_0][idx_nei];
			
  StrValue = ref_p0 != ref_q0;			
  if(!StrValue )
    StrValue= (mv_p0[0] - mv_q0[0] + 3 >= 7U) | (iabs( mv_p0[1] - mv_q0[1]) >= mvlimit);
  if(b_slice)
  {
	VO_S32 ref_p1 = (VO_S32)pDecLocal->pic_cache[LIST_1][idx_cur];
	VO_S32 ref_q1 = (VO_S32)pDecLocal->pic_cache[LIST_1][idx_nei];
	short* mv_p1 = (short*)&pDecLocal->mv_cache[LIST_1][idx_cur];
	short* mv_q1 = (short*)&pDecLocal->mv_cache[LIST_1][idx_nei];
	if(!StrValue)
	  StrValue = (ref_p1 != ref_q1) |
				 (mv_p1[0] - mv_q1[0] + 3 >= 7U) | (iabs( mv_p1[1] - mv_q1[1]) >= mvlimit);
			
	if(StrValue)
	{
	  if((ref_p0 != ref_q1) | (ref_p1 != ref_q0))
		StrValue =  1;
	  else
		StrValue = ((mv_p0[0] - mv_q1[0] + 3 >= 7U) | (iabs( mv_p0[1] - mv_q1[1]) >= mvlimit)) |
                   ((mv_p1[0] - mv_q0[0] + 3 >= 7U) | (iabs( mv_p1[1] - mv_q0[1]) >= mvlimit));
	}
  }
  return StrValue;
}
static const VO_U8 offset[2][2][8]=
{
  {
    {3+4*0, 3+4*0, 3+4*0, 3+4*0, 3+4*1, 3+4*1, 3+4*1, 3+4*1},
    {3+4*2, 3+4*2, 3+4*2, 3+4*2, 3+4*3, 3+4*3, 3+4*3, 3+4*3},
  },
  {
    {3+4*0, 3+4*1, 3+4*2, 3+4*3, 3+4*0, 3+4*1, 3+4*2, 3+4*3},
    {3+4*0, 3+4*1, 3+4*2, 3+4*3, 3+4*0, 3+4*1, 3+4*2, 3+4*3},
  }
};


	
void DeblockMb_MBAFF(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *p)
{  
    VO_S32           edge,i;
    byte Strength[4];
    VO_S32         *p_Strength32 = (VO_S32*) Strength;
    VO_S32           filterLeftMbEdgeFlag;
    VO_S32           filterTopMbEdgeFlag;
	VO_S32 mb_xy = pDecLocal->mb_xy;
	VO_S32 mb_field = pDecLocal->mb_field;
    VO_S32 mvlimit = (mb_field) ? 2 : 4;
    VO_S32 nQP = pDecGlobal->qp_frame_buffer[mb_xy];
	VO_S32 nQPC = pDecGlobal->qpc_frame_buffer[mb_xy*2];
	VO_S32 luma_stride;
    VO_S32 chroma_stride;
	VO_S32 nMBX  = pDecLocal->mb.x;
    VO_S32 nMBY	 = pDecLocal->mb.y;
	VO_U8 *pixy = p->imgY + (nMBY*pDecGlobal->iLumaStride+nMBX)*16;
    VO_U8 *pixu = p->imgUV[0] +  (nMBY*pDecGlobal->iChromaStride+nMBX)*8;
    VO_U8 *pixv = p->imgUV[1] +  (nMBY*pDecGlobal->iChromaStride+nMBX)*8;
	VO_U8* nz_coeff = pDecLocal->nz_coeff;
    seq_parameter_set_rbsp_t *active_sps = pDecGlobal->pCurSlice->active_sps;  
	int b_slice = pDecGlobal->type == B_SLICE;
	int mb_type0 = pDecLocal->mb_type0;
	int is_intra = pDecLocal->is_intra_block;
    int is_skip = (IS_SKIP(mb_type0)&& pDecGlobal->type == P_SLICE)||(mb_type0&0x18);//((mb_type == PSKIP && pDecGlobal->type == P_SLICE) || (mb_type == P16x16) || (mb_type == P16x8));
	int filterNon8x8LumaEdgesFlag[4] = {1,1,1,1};
	VO_S32 top_xy = pDecLocal->top_mb_xy;
	VO_S32 left_xy[2];
    filterLeftMbEdgeFlag = (nMBX != 0);
    filterTopMbEdgeFlag  = (nMBY != 0);
	if(mb_field)
	{
	  luma_stride = pDecGlobal->iLumaStride<<1;
	  chroma_stride = pDecGlobal->iChromaStride<<1;
	  if(nMBY&1)
	  {
	    pixy -= 15*pDecGlobal->iLumaStride;
		pixu -= 7*pDecGlobal->iChromaStride;
		pixv -= 7*pDecGlobal->iChromaStride;
	  }
	  if(nMBY==1)
	  	filterTopMbEdgeFlag = 0;
    }
	else
	{
	  luma_stride = pDecGlobal->iLumaStride;
	  chroma_stride = pDecGlobal->iChromaStride;
	}
	
	left_xy[0] = pDecLocal->left_mb_xy[0];
	left_xy[1] = pDecLocal->left_mb_xy[1];
	
	filterNon8x8LumaEdgesFlag[1] =filterNon8x8LumaEdgesFlag[3] = !(IS_8x8DCT(mb_type0));
  
    if(filterLeftMbEdgeFlag&&mb_field!=!!IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[left_xy[0]]))
    {
      ALIGNED_16(VO_S16 Strength[8]);
	  VO_S32 qp_lt0= pDecGlobal->qp_frame_buffer[left_xy[0]];
	  VO_S32 qp_lt1= pDecGlobal->qp_frame_buffer[left_xy[1]];
	  VO_S32 qpc0 = (nQPC + pDecGlobal->qpc_frame_buffer[left_xy[0]*2] + 1) >> 1;
	  VO_S32 qpc1 = (nQPC + pDecGlobal->qpc_frame_buffer[left_xy[1]*2] + 1) >> 1;
      filterLeftMbEdgeFlag=0;
	  if(is_intra) 
	  {
        M64(&Strength[0]) = 0x0004000400040004ll;
        M64(&Strength[4]) = 0x0004000400040004ll;
      }
	  else
	  {
	    const VO_U8 *off= offset[mb_field][nMBY&1];
        for( i = 0; i < 8; i++ ) 
		{
          int j= mb_field ? i>>2 : i&1;
          int left_xy = pDecLocal->left_mb_xy[j];
          int left_intra = pDecLocal->left_type[j];

          if(left_intra)
          {
            Strength[i] = 4;
          }
          else
		  {
            Strength[i] = 1 + !!(nz_coeff[12+8*(i>>1)] |
                         ((!pDecGlobal->pCurSlice->active_pps->entropy_coding_mode_flag && IS_8x8DCT(pDecGlobal->mb_type0_frame_buffer[left_xy])) ?
                            (pDecGlobal->cbp_frame_buffer[left_xy] & ((mb_field ? (i&2) : (nMBY&1)) ? 8 : 2))
                                                                       :
                            pDecGlobal->nz_coeff_frame[left_xy*24+off[i] ]));
          }
        }
	  }

	  if(mb_field)
	  {
        DeblockLumaV_mbaff ( pDecGlobal, pixy,luma_stride, Strength, 1, (nQP+qp_lt0+1) >> 1 );
        DeblockLumaV_mbaff ( pDecGlobal, pixy + 8*luma_stride, luma_stride, Strength+4, 1, (nQP+qp_lt1+1) >> 1 );
        DeblockChromaV_mbaff( pDecGlobal, pixu, chroma_stride, Strength  , 1, qpc0 );
        DeblockChromaV_mbaff( pDecGlobal, pixu + 4*chroma_stride, chroma_stride, Strength+4, 1, qpc1 );
        DeblockChromaV_mbaff( pDecGlobal, pixv, chroma_stride, Strength  , 1, qpc0 );
        DeblockChromaV_mbaff( pDecGlobal, pixv + 4*chroma_stride, chroma_stride, Strength+4, 1, qpc1 );
      }
	  else
	  {
        DeblockLumaV_mbaff ( pDecGlobal, pixy, 2*luma_stride, Strength, 2, (nQP+qp_lt0+1) >> 1 );
        DeblockLumaV_mbaff ( pDecGlobal, pixy + luma_stride, 2*luma_stride, Strength+1, 2, (nQP+qp_lt1+1) >> 1 );
        DeblockChromaV_mbaff( pDecGlobal, pixu, 2*chroma_stride, Strength  , 2, qpc0 );
        DeblockChromaV_mbaff( pDecGlobal, pixu + chroma_stride, 2*chroma_stride, Strength+1, 2, qpc1 );
        DeblockChromaV_mbaff( pDecGlobal, pixv, 2*chroma_stride, Strength  , 2, qpc0 );
        DeblockChromaV_mbaff( pDecGlobal, pixv + chroma_stride, 2*chroma_stride, Strength+1, 2, qpc1 );
      }
    }
	// Vertical deblocking
    if(filterLeftMbEdgeFlag)
    {
      int qp_lt= pDecGlobal->qp_frame_buffer[left_xy[0]];
	  VO_S32 qpc = (nQPC + pDecGlobal->qpc_frame_buffer[left_xy[0]*2] + 1) >> 1;
      if (is_intra || pDecLocal->left_type[0])
      {
        DeblocIntraLumaH( pDecGlobal, pixy ,luma_stride,(nQP+qp_lt+1) >> 1);
        DeblocIntraChromaH( pDecGlobal, pixu, pixv, chroma_stride,qpc);
      }
	  else
	  {
        for(i = 0; i < 4; i++)
        {
          if (nz_coeff[12+(i<<3)] | nz_coeff[11+(i<<3)])	
            Strength[i] = 2;
		  else
		  	Strength[i] = strength_mv(pDecLocal,12+(i<<3),11+(i<<3),mvlimit,b_slice);
        }
		if (p_Strength32[0]) // only if one of the 16 Strength bytes is != 0
        {
          DeblockLumaH ( pDecGlobal, pixy,luma_stride, Strength, (nQP+qp_lt+1) >> 1);
          DeblockChromaH( pDecGlobal, pixu, pixv,chroma_stride, Strength, qpc);
        }   
	  }
    }
    for (edge = 1; edge < 4 ; ++edge )    
    {
      // If cbp == 0 then deblocking for some macroblock types could be skipped
      if ((pDecLocal->cbp&15) == 0)
      { 
        if (filterNon8x8LumaEdgesFlag[edge] == 0)
          continue;
        if (is_skip)
          continue;
        else if ((edge & 0x01) && ((mb_type0&VO_8x16) || (b_slice && IS_SKIP(mb_type0)&& active_sps->direct_8x8_inference_flag)))
          continue;
      }      
      // Strength for 4 blks in 1 stripe
      //GetStrengthVer(pDecGlobal,b_slice,is_intra,Strength, edge, mvlimit, p, MbQAddr);
      if(is_intra) 
	  {
        M32(Strength) = 0x03030303;
      } 
	  else 
	  {
        int idx;
		int StrValue;
        int in_skip = mb_type0&0x18;
        for( idx = 0 ; idx < MB_BLOCK_SIZE ; idx += BLOCK_SIZE )
        {
          if (nz_coeff[12+(idx<<1)+edge] | nz_coeff[11+(idx<<1)+edge])	
            StrValue = 2;
          else if (in_skip)
            StrValue = 0; // if internal edge of certain types, we already know StrValue should be 0
          else 
          {
            StrValue = strength_mv(pDecLocal,12+(idx<<1)+edge,11+(idx<<1)+edge,mvlimit,b_slice);
          }
		  Strength[idx>>2] = StrValue;
        }
	  }
      if (p_Strength32[0]) // only if one of the 16 Strength bytes is != 0
      {
        if (filterNon8x8LumaEdgesFlag[edge])
          DeblockLumaH ( pDecGlobal, pixy + 4*edge,luma_stride, Strength, nQP);
        if( !(edge & 1) )
	    {
	      DeblockChromaH( pDecGlobal, pixu + 2*edge, pixv + 2*edge,chroma_stride, Strength, nQPC);
	    }
      }        
    }//end edge

    
    // horizontal deblocking  
    if(filterTopMbEdgeFlag)
    {      
      int qp_lt= pDecGlobal->qp_frame_buffer[top_xy];
	  if(((nMBY&1)==0) && (!!IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[top_xy])&~mb_field))
	  {
	    VO_U32 tmp_luma_stride   = 2*luma_stride;
        VO_U32 tmp_chroma_stride = 2*chroma_stride;
        VO_S32 top2_xy = mb_xy - 2*pDecGlobal->PicWidthInMbs;
        VO_S32 j;

        for(j=0; j<2; j++, top2_xy += pDecGlobal->PicWidthInMbs)
		{
		  VO_S32 qpl = (nQP+pDecGlobal->qp_frame_buffer[top2_xy]+1) >> 1;
		  VO_S32 qpc = (nQPC + pDecGlobal->qpc_frame_buffer[(top2_xy)*2] + 1) >> 1;
		  if(is_intra||IS_INTRA0(pDecGlobal->mb_type0_frame_buffer[top2_xy]))
		  {
		    M32(Strength) = 0x03030303;
		  }
		  else
		  {
		    if(!pDecGlobal->pCurSlice->active_pps->entropy_coding_mode_flag && IS_8x8DCT(pDecGlobal->mb_type0_frame_buffer[top2_xy]))
			{
			  VO_S32 cbp = pDecGlobal->cbp_frame_buffer[top2_xy];
              Strength[0]= 1+((cbp & 4)||nz_coeff[12]);
              Strength[1]= 1+((cbp & 4)||nz_coeff[13]);
              Strength[2]= 1+((cbp & 8)||nz_coeff[14]);
              Strength[3]= 1+((cbp & 8)||nz_coeff[15]);
            }
			else
			{
              VO_U8 *top_nz = &pDecGlobal->nz_coeff_frame[top2_xy*24]+3*4;
              int i;
              for( i = 0; i < 4; i++ ) 
			  {
                Strength[i] = 1 + !!(nz_coeff[12+i] | top_nz[i]);
              }
            }
		  }
		  
		  DeblockLumaV ( pDecGlobal, pixy+j*luma_stride,tmp_luma_stride, Strength, qpl);
          DeblockChromaV( pDecGlobal, pixu+j*chroma_stride, pixv+j*chroma_stride,tmp_chroma_stride, Strength, qpc);
		}
	  }
	  else
	  {
        if (is_intra || pDecLocal->top_type)
        {
          VO_S32 qpc = (nQPC + pDecGlobal->qpc_frame_buffer[(top_xy)*2] + 1) >> 1;
          if(mb_field||IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[top_xy]))
          {
            M32(Strength) = 0x03030303;
            DeblockLumaV ( pDecGlobal, pixy,luma_stride, Strength, (nQP+qp_lt+1) >> 1);
            DeblockChromaV( pDecGlobal, pixu, pixv,chroma_stride, Strength, qpc);
          }
  		  else
  		  {
            DeblocIntraLumaV( pDecGlobal, pixy ,luma_stride,(nQP+qp_lt+1) >> 1);
            DeblocIntraChromaV( pDecGlobal, pixu, pixv, chroma_stride,qpc);
  		  }
        }
  	    else
  	    {
  	      if(mb_field!=!!IS_INTERLACED(pDecGlobal->mb_type0_frame_buffer[top_xy]))
  	      {
  	        for(i = 0; i < 4; i++)
            {
              if (nz_coeff[12+i] | nz_coeff[4+i] )	
                Strength[i] = 2;
  		      else
  		  	    Strength[i] = 1;
            }
  	      }
		  else
		  {
            for(i = 0; i < 4; i++)
            {
              if (nz_coeff[12+i] | nz_coeff[4+i] )	
                Strength[i] = 2;
  		      else
  		  	    Strength[i] = strength_mv(pDecLocal,12+i,4+i,mvlimit,b_slice);
            }
		  }
  		  if (p_Strength32[0]) // only if one of the 16 Strength bytes is != 0
          {
            VO_S32 qpc = (nQPC + pDecGlobal->qpc_frame_buffer[top_xy*2] + 1) >> 1;
            DeblockLumaV ( pDecGlobal, pixy,luma_stride, Strength, (nQP+qp_lt+1) >> 1);
            DeblockChromaV( pDecGlobal, pixu, pixv,chroma_stride, Strength, qpc);
          }  
  	    } 
	  }
    }
	is_skip = (IS_SKIP(mb_type0)&& pDecGlobal->type == P_SLICE)||(mb_type0&0x28);//((mb_type == PSKIP && pDecGlobal->type == P_SLICE) || (mb_type == P16x16) || (mb_type == P8x16));
    for( edge = 1; edge < 4 ; ++edge )
    {
      // If cbp == 0 then deblocking for some macroblock types could be skipped
      if ((pDecLocal->cbp&15) == 0)
      {      
        if (filterNon8x8LumaEdgesFlag[edge] == 0)
          continue;
        if (is_skip)
          continue;
        else if ((edge & 0x01) && ((mb_type0&VO_16x8) || (b_slice && IS_SKIP(mb_type0)&& active_sps->direct_8x8_inference_flag)))
          continue;
      }
      // Strength for 4 blks in 1 stripe
      //GetStrengthHor(pDecGlobal,b_slice,is_intra,Strength, edge << 2, mvlimit, p,MbQAddr);
      if(is_intra) 
	  {
        M32(Strength) = 0x03030303;
      } 
	  else 
	  {
        int idx;
		int StrValue;
        int in_skip = mb_type0&0x28;
        for( idx = 0 ; idx < 4 ; idx++ )
        {
          if (nz_coeff[12+(edge<<3)+idx] | nz_coeff[4+(edge<<3)+idx])	
            StrValue = 2;
          else if (in_skip)
            StrValue = 0; // if internal edge of certain types, we already know StrValue should be 0
          else 
          {
            StrValue = strength_mv(pDecLocal,12+(edge<<3)+idx,4+(edge<<3)+idx,mvlimit,b_slice);
          }
		  Strength[idx] = StrValue;
        }
	  }
      if (p_Strength32[0]) // only if one of the 16 Strength bytes is != 0
      {
        if (filterNon8x8LumaEdgesFlag[edge])
          DeblockLumaV( pDecGlobal, pixy + 4*edge*luma_stride,luma_stride, Strength, nQP);
        if( !(edge & 1) )
	    {
	      DeblockChromaV( pDecGlobal, pixu + 2*edge*chroma_stride, pixv + 2*edge*chroma_stride,chroma_stride, Strength, nQPC);
	    }
      }
    }//end edge  
}

void DeblockMb(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, StorablePicture *p)
{
  
    int           edge,i;
    byte Strength[4];
    VO_S32         *p_Strength32 = (VO_S32*) Strength;
    int           filterLeftMbEdgeFlag;
    int           filterTopMbEdgeFlag;
    int mvlimit = (p->structure!=FRAME) ? 2 : 4;
    VO_S32 nQP = pDecGlobal->qp_frame_buffer[pDecLocal->mb_xy];
	VO_S32 nQPC = pDecGlobal->qpc_frame_buffer[pDecLocal->mb_xy*2];
	VO_S32 luma_stride   = (p->structure!=FRAME)?pDecGlobal->iLumaStride<<1:pDecGlobal->iLumaStride;
    VO_S32 chroma_stride  = (p->structure!=FRAME)?pDecGlobal->iChromaStride<<1:pDecGlobal->iChromaStride;
	//VO_S32 nMBX  = pDecLocal->mb.x;
    //VO_S32 nMBY	 = pDecLocal->mb.y;
	VO_U8 *pixy = p->imgY+pDecLocal->pix_y*pDecGlobal->iLumaStride+pDecLocal->pix_x;
    VO_U8 *pixu = p->imgUV[0]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
    VO_U8 *pixv = p->imgUV[1]+pDecLocal->pix_c_y*pDecGlobal->iChromaStride+pDecLocal->pix_c_x;
	VO_U8* nz_coeff = pDecLocal->nz_coeff;
    seq_parameter_set_rbsp_t *active_sps = pDecGlobal->pCurSlice->active_sps;  
	int b_slice = pDecGlobal->type == B_SLICE;
	int mb_type0 = pDecLocal->mb_type0;
	int is_intra = pDecLocal->is_intra_block;
    int is_skip = (IS_SKIP(mb_type0)&& pDecGlobal->type == P_SLICE)||(mb_type0&0x18);//((mb_type == PSKIP && pDecGlobal->type == P_SLICE) || (mb_type == P16x16) || (mb_type == P16x8));
	int filterNon8x8LumaEdgesFlag[4] = {1,1,1,1};
	VO_S32 top_xy = pDecLocal->top_mb_xy;
	VO_S32 left_xy[2];
	left_xy[0] = pDecLocal->left_mb_xy[0];
	left_xy[1] = pDecLocal->left_mb_xy[1];
	filterLeftMbEdgeFlag = (pDecLocal->mb.x != 0);
    filterTopMbEdgeFlag  = (pDecLocal->top_mb_xy >= 0);
	filterNon8x8LumaEdgesFlag[1] =filterNon8x8LumaEdgesFlag[3] = !(IS_8x8DCT(mb_type0));
    // Vertical deblocking
    if(filterLeftMbEdgeFlag)
    {
      int qp_lt= pDecGlobal->qp_frame_buffer[left_xy[0]];
	  VO_S32 qpc = (nQPC + pDecGlobal->qpc_frame_buffer[left_xy[0]*2] + 1) >> 1;
      if (is_intra || pDecLocal->left_type[0])
      {
          DeblocIntraLumaH( pDecGlobal, pixy ,luma_stride,(nQP+qp_lt+1) >> 1);
          DeblocIntraChromaH( pDecGlobal, pixu, pixv, chroma_stride,qpc);
      }
	  else
	  {
        for(i = 0; i < 4; i++)
        {
          if (nz_coeff[12+(i<<3)] | nz_coeff[11+(i<<3)])	
            Strength[i] = 2;
		  else
		  	Strength[i] = strength_mv(pDecLocal,12+(i<<3),11+(i<<3),mvlimit,b_slice);
        }
		if (p_Strength32[0]) // only if one of the 16 Strength bytes is != 0
        {
          DeblockLumaH ( pDecGlobal, pixy,luma_stride, Strength, (nQP+qp_lt+1) >> 1);
          DeblockChromaH( pDecGlobal, pixu, pixv,chroma_stride, Strength, qpc);
        }   
	  }
    }
    for (edge = 1; edge < 4 ; ++edge )    
    {
      // If cbp == 0 then deblocking for some macroblock types could be skipped
      if ((pDecLocal->cbp&15) == 0)
      { 
        if (filterNon8x8LumaEdgesFlag[edge] == 0)
          continue;
        if (is_skip)
          continue;
        else if ((edge & 0x01) && ((mb_type0&VO_8x16) || (b_slice && IS_SKIP(mb_type0) && active_sps->direct_8x8_inference_flag)))
          continue;
      }      
      // Strength for 4 blks in 1 stripe
      //GetStrengthVer(pDecGlobal,b_slice,is_intra,Strength, edge, mvlimit, p, MbQAddr);
      if(is_intra) 
	  {
        M32(Strength) = 0x03030303;
      } 
	  else 
	  {
        int idx;
		int StrValue;
        int in_skip = mb_type0&0x18;
        for( idx = 0 ; idx < MB_BLOCK_SIZE ; idx += BLOCK_SIZE )
        {
          if (nz_coeff[12+(idx<<1)+edge] | nz_coeff[11+(idx<<1)+edge])	
            StrValue = 2;
          else if (in_skip)
            StrValue = 0; // if internal edge of certain types, we already know StrValue should be 0
          else 
          {
            StrValue = strength_mv(pDecLocal,12+(idx<<1)+edge,11+(idx<<1)+edge,mvlimit,b_slice);
          }
		  Strength[idx>>2] = StrValue;
        }
	  }
      if (p_Strength32[0]) // only if one of the 16 Strength bytes is != 0
      {
        if (filterNon8x8LumaEdgesFlag[edge])
        DeblockLumaH ( pDecGlobal, pixy + 4*edge,luma_stride, Strength, nQP);
        if( !(edge & 1) )
	    {
	      DeblockChromaH( pDecGlobal, pixu + 2*edge, pixv + 2*edge,chroma_stride, Strength, nQPC);
	    }
      }        
    }//end edge

    // horizontal deblocking  
    if(filterTopMbEdgeFlag)
    {
      int qp_lt= pDecGlobal->qp_frame_buffer[top_xy];
      if (is_intra || pDecLocal->top_type)
      {
        VO_S32 qpc = (nQPC + pDecGlobal->qpc_frame_buffer[(top_xy)*2] + 1) >> 1;
		if(p->structure!=FRAME)
        {
          M32(Strength) = 0x03030303;
          DeblockLumaV ( pDecGlobal, pixy,luma_stride, Strength, (nQP+qp_lt+1) >> 1);
          DeblockChromaV( pDecGlobal, pixu, pixv,chroma_stride, Strength, qpc);
        }
		else
		{
          DeblocIntraLumaV( pDecGlobal, pixy ,luma_stride,(nQP+qp_lt+1) >> 1);
          DeblocIntraChromaV( pDecGlobal, pixu, pixv, chroma_stride,qpc);
		}
      }
	  else
	  {
        //GetStrengthHor(pDecGlobal,pDecLocal,Strength, 0, mvlimit, p, MbQAddr);
        for(i = 0; i < 4; i++)
        {
          if (nz_coeff[12+i] | nz_coeff[4+i] )	
            Strength[i] = 2;
		  else
		  	Strength[i] = strength_mv(pDecLocal,12+i,4+i,mvlimit,b_slice);
        }
		if (p_Strength32[0]) // only if one of the 16 Strength bytes is != 0
        {
          VO_S32 qpc = (nQPC + pDecGlobal->qpc_frame_buffer[top_xy*2] + 1) >> 1;
          DeblockLumaV ( pDecGlobal, pixy,luma_stride, Strength, (nQP+qp_lt+1) >> 1);
          DeblockChromaV( pDecGlobal, pixu, pixv,chroma_stride, Strength, qpc);
        }  
	  } 
    }
	is_skip = (IS_SKIP(mb_type0)&& pDecGlobal->type == P_SLICE)||(mb_type0&0x28);//((mb_type == PSKIP && pDecGlobal->type == P_SLICE) || (mb_type == P16x16) || (mb_type == P8x16));
    for( edge = 1; edge < 4 ; ++edge )
    {
      // If cbp == 0 then deblocking for some macroblock types could be skipped
      if ((pDecLocal->cbp&15) == 0)
      {      
        if (filterNon8x8LumaEdgesFlag[edge] == 0)
          continue;
        if (is_skip)
          continue;
        else if ((edge & 0x01) && ((mb_type0&VO_16x8) || (b_slice && IS_SKIP(mb_type0)&& active_sps->direct_8x8_inference_flag)))
          continue;
      }
      // Strength for 4 blks in 1 stripe
      //GetStrengthHor(pDecGlobal,b_slice,is_intra,Strength, edge << 2, mvlimit, p,MbQAddr);
      if(is_intra) 
	  {
        M32(Strength) = 0x03030303;
      } 
	  else 
	  {
        int idx;
		int StrValue;
        int in_skip = mb_type0&0x28;
        for( idx = 0 ; idx < 4 ; idx++ )
        {
          if (nz_coeff[12+(edge<<3)+idx] | nz_coeff[4+(edge<<3)+idx])	
            StrValue = 2;
          else if (in_skip)
            StrValue = 0; // if internal edge of certain types, we already know StrValue should be 0
          else 
          {
            StrValue = strength_mv(pDecLocal,12+(edge<<3)+idx,4+(edge<<3)+idx,mvlimit,b_slice);
          }
		  Strength[idx] = StrValue;
        }
	  }
      if (p_Strength32[0]) // only if one of the 16 Strength bytes is != 0
      {
        if (filterNon8x8LumaEdgesFlag[edge])
        DeblockLumaV( pDecGlobal, pixy + 4*edge*luma_stride,luma_stride, Strength, nQP);
        if( !(edge & 1) )
	    {
	      DeblockChromaV( pDecGlobal, pixu + 2*edge*chroma_stride, pixv + 2*edge*chroma_stride,chroma_stride, Strength, nQPC);
	    }
      }
    }//end edge  
}


