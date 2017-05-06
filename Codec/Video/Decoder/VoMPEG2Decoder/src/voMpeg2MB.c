/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2007		            *
*								 	                                    *
************************************************************************/

#include "voMpegPort.h"
#include "voMpeg2Decoder.h"
#include "voMpeg2MB.h"
#include "voMpegMC.h"
#include "voMpegBuf.h"
#include "voMpeg2DecGlobal.h"

/* 16*16 or 16*8 */
VO_VOID MotionComp(MpegDecode *dec, Mp2DecLocal *mp2_dec_local, int field_based,
				int isMBPat, int dmv_flage) 
{
	VO_U8	*src[3], *dst[3];
	VO_S32 src_stride = dec->img_stride;
	VO_S32 *MVBack = mp2_dec_local->MVBack;
	VO_S32 *MVFwd = mp2_dec_local->MVFwd;
	VO_S32 MV = 0, MV1;
	VO_S32 height = 16;
	VO_S32 dst_stride, dst_strideUV;
	VO_S32 bottom_field = (field_based>>1);
	VO_S32 stride_temp;
	VO_S32 src_stride_uv;
	VO_S32 stride_uv_temp = dec->img_stride_uv;

	VO_S32 cur_x, cur_y;
	VO_S32 mv_x, mv_y;

	if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
	{
		height = 8;
		stride_temp = (src_stride<<1);

		stride_uv_temp <<= 1;
	}
	else
	{
		stride_temp = src_stride;
	}

	if(1==dmv_flage)
	{
		if((PICT_FRAME==dec->picture_structure)&&(3==field_based))
		{
			MVBack = &dec->fmv[2][1];
		}
		else
		{
			MVBack = &dec->fmv[3][1];
		}
	}

	if (isMBPat){
		dst[0] = dec->mc_dst[0];
		dst_stride = 16>>DOWNSAMPLE_FLAG(dec);
		
		if (isMBPat & (1<<(MB_PAT+1))){
			// chroma coeff are all zeros; get MC to frame directly;
			dst[1] = mp2_dec_local->frame_dst[1];
			dst[2] = mp2_dec_local->frame_dst[2];

			dst_strideUV = dec->img_stride_uv;
		}
		else {
			dst[1] = dec->mc_dst[1];
			dst[2] = dec->mc_dst[2];

			dst_strideUV = (dec->uv_interlace_flage)?16:8;
			dst_strideUV = dst_strideUV>>DOWNSAMPLE_FLAG(dec);
		}
	}
	else {
		dst[0] = mp2_dec_local->frame_dst[0];
		dst[1] = mp2_dec_local->frame_dst[1];
		dst[2] = mp2_dec_local->frame_dst[2];
		dst_stride = dec->img_stride;
		dst_strideUV = dec->img_stride_uv;
	}

	if(field_based){
		if(bottom_field){ 
			if(!dmv_flage)
			{
				if(MVBack)
					MVBack++;
				if(MVFwd)
					MVFwd++;
				dst[0] += dst_stride;
				dst[1] += dst_strideUV;
				dst[2] += dst_strideUV;
			}
		}
		else if(1==dmv_flage)
		{
			dst[0] += dst_stride;
			dst[1] += dst_strideUV;
			dst[2] += dst_strideUV;
		}
		if((1== dec->ds_Bframe)&&(B_VOP==dec->prediction_type))
		{
			height = 8;
			src_stride <<= 1;
		}
		else
		{
			height = 8;
			src_stride <<= 1;
			dst_stride <<= 1;
			dst_strideUV <<= 1;

			stride_temp = src_stride;
			stride_uv_temp <<= 1;
		}
	}

	if (MVBack){
		src[0] = mp2_dec_local->ref[0];
		src[1] = mp2_dec_local->ref[1];
		src[2] = mp2_dec_local->ref[2];
		if(field_based&&dec->field_select[0][bottom_field]){
			src[0] += dec->img_stride;
			src[1] += dec->img_stride_uv;
			src[2] += dec->img_stride_uv;
		}

		MV = MVBack[0];

		cur_x = dec->xmb_pos*16;
		cur_y = dec->ymb_pos*16;	
		mv_x = (MV<<16)>>16;
		mv_y = MV>>16;

		if(mv_x<0)
		{
			mv_x = max(-cur_x*2, mv_x);
		}
		else
		{
			mv_x = min((dec->img_width - 16 -cur_x)*2,mv_x);
		}

		if(mv_y<0)
		{
			mv_y = max(-cur_y*2, mv_y);
			if(field_based)
			{
				mv_y = max(-cur_y, mv_y);
			}
		}
		else
		{
			mv_y = min((dec->img_height - 16 - cur_y)*2,mv_y);
			if(field_based)
			{
				mv_y = min((dec->img_height - 16 - cur_y),mv_y);
			}
		}
		
		MV = MAKEMV(mv_x, mv_y);
		
		src[0] += MV_X(MV, dec) + src_stride * MV_Y(MV);

		//MV = MV_SUB(MV, dec);
		if(DOWNSAMPLE_FLAG(dec))
		{
			MV = MVDS_SUB(MV, dec);
		}
		else
		{
			MV = MV_SUB(MV, dec);
		}
		
		if(!dmv_flage)
		{
#ifndef IPP_EDIT
			dec->all_copy_block[MV](src[0], dst[0], stride_temp, dst_stride, height);
			dec->all_copy_block[MV](src[0]+(8>>DOWNSAMPLE_FLAG(dec)), dst[0]+(8>>DOWNSAMPLE_FLAG(dec)), stride_temp, dst_stride, height);
#else //IPP_EDIT
			if(8 == height){
				ippiCopy16x8HP_8u_C1R(src[0], src_stride, dst[0], dst_stride,MV,0);
			}else{
				ippiCopy16x16HP_8u_C1R(src[0], src_stride, dst[0], dst_stride,MV,0);
			}
#endif //IPP_EDIT


		}
		else
		{
#ifndef IPP_EDIT
			dec->all_add_block[MV](src[0], dst[0], stride_temp, dst_stride, height);
			dec->all_add_block[MV](src[0]+(8>>DOWNSAMPLE_FLAG(dec)), dst[0]+(8>>DOWNSAMPLE_FLAG(dec)), stride_temp, dst_stride, height);					
#else //IPP_EDIT
			if(8 == height){
				ippiInterpolateAverage16x8_8u_C1IR(src[0], src_stride, dst[0], dst_stride,MV,0);
			}else{
				ippiInterpolateAverage16x16_8u_C1IR(src[0], src_stride, dst[0], dst_stride,MV,0);
			}
#endif //IPP_EDIT


		}

//		MV = MVBack[4];
		MV = MAKEMV(mv_x/2,mv_y/2);
		
		src_stride_uv = (dec->uv_interlace_flage)?src_stride:(src_stride>>1);
		//stride_uv_temp = (dec->uv_interlace_flage)?stride_temp:(stride_temp>>1);
		src[1] += MV_X(MV, dec) + src_stride_uv * MV_Y(MV);
		src[2] += MV_X(MV, dec) + src_stride_uv * MV_Y(MV);
		
		//MV = MV_SUB(MV, dec);
		if(DOWNSAMPLE_FLAG(dec))
		{
			MV = MVDS_SUB(MV, dec);
		}
		else
		{
			MV = MV_SUB(MV, dec);
		}
		
		if(!dmv_flage)
		{
#ifndef IPP_EDIT
			dec->all_copy_block[MV](src[1], dst[1], stride_uv_temp,dst_strideUV, (height>>1));
			dec->all_copy_block[MV](src[2], dst[2], stride_uv_temp,dst_strideUV, (height>>1));					
#else //IPP_EDIT
			if(8 == height){
				ippiCopy8x4HP_8u_C1R(src[1], src_stride_uv, dst[1], dst_strideUV,MV,0);
				ippiCopy8x4HP_8u_C1R(src[2], src_stride_uv, dst[2], dst_strideUV,MV,0);
			}else{
				ippiCopy8x8HP_8u_C1R(src[1], src_stride_uv, dst[1], dst_strideUV,MV,0);
				ippiCopy8x8HP_8u_C1R(src[2], src_stride_uv, dst[2], dst_strideUV,MV,0);

			}
#endif //IPP_EDIT

		}
		else
		{
#ifndef IPP_EDIT
			dec->all_add_block[MV](src[1], dst[1], stride_uv_temp, dst_strideUV, (height>>1));
			dec->all_add_block[MV](src[2], dst[2], stride_uv_temp, dst_strideUV, (height>>1));					
#else //IPP_EDIT
			if(8 == height){
				ippiInterpolateAverage8x4_8u_C1IR(src[1], src_stride_uv, dst[1], dst_strideUV,MV,0);
				ippiInterpolateAverage8x4_8u_C1IR(src[2], src_stride_uv, dst[2], dst_strideUV,MV,0);
			}else{
				ippiInterpolateAverage8x8_8u_C1IR(src[1], src_stride_uv, dst[1], dst_strideUV,MV,0);
				ippiInterpolateAverage8x8_8u_C1IR(src[2], src_stride_uv, dst[2], dst_strideUV,MV,0);
			}
#endif //IPP_EDIT

		}
	} 

	if (MVFwd){
		src[0] = mp2_dec_local->cur[0];
		src[1] = mp2_dec_local->cur[1];
		src[2] = mp2_dec_local->cur[2];
		if(field_based&&dec->field_select[1][bottom_field]){
			src[0] += dec->img_stride;
			src[1] += dec->img_stride_uv;
			src[2] += dec->img_stride_uv;
		}
		MV = MVFwd[0];

		cur_x = dec->xmb_pos*16; 
		cur_y = dec->ymb_pos*16;	
		mv_x = (MV<<16)>>16;
		mv_y = MV>>16;

		if(mv_x<0)
		{
			mv_x = max(-cur_x*2, mv_x);
		}
		else
		{
			mv_x = min((dec->img_width - 16 - cur_x)*2,mv_x);
		}

		if(mv_y<0)
		{
			mv_y = max(-cur_y*2, mv_y);
			if(field_based)
			{
				mv_y = max(-cur_y, mv_y);
			}
		}
		else
		{
			mv_y = min((dec->img_height - 16 - cur_y)*2, mv_y);
			if(field_based)
			{
				mv_y = min((dec->img_height - 16 - cur_y), mv_y);
			}
		}
		
		MV = MAKEMV(mv_x, mv_y);
		
		src[0] += MV_X(MV, dec) + src_stride * MV_Y(MV);
		//MV1 = MV_SUB(MV, dec);
		if(DOWNSAMPLE_FLAG(dec))
		{
			MV1 = MVDS_SUB(MV, dec);
		}
		else
		{
			MV1 = MV_SUB(MV, dec);
		}
		MV = MAKEMV(mv_x/2,mv_y/2);

		src_stride_uv = (dec->uv_interlace_flage)?src_stride:(src_stride>>1);
		//stride_uv_temp = (dec->uv_interlace_flage)?stride_temp:(stride_temp>>1);
		src[1] += MV_X(MV, dec) + src_stride_uv * MV_Y(MV);
		src[2] += MV_X(MV, dec) + src_stride_uv * MV_Y(MV);

		//MV = MV_SUB(MV, dec);
		if(DOWNSAMPLE_FLAG(dec))
		{
			MV = MVDS_SUB(MV, dec);
		}
		else
		{
			MV = MV_SUB(MV, dec);
		}
		
		if (!MVBack){
#ifndef IPP_EDIT
			dec->all_copy_block[MV1](src[0], dst[0], stride_temp, dst_stride, height);
			dec->all_copy_block[MV1](src[0]+(8>>DOWNSAMPLE_FLAG(dec)), dst[0]+(8>>DOWNSAMPLE_FLAG(dec)), stride_temp, dst_stride, height);
			dec->all_copy_block[MV](src[1],dst[1],stride_uv_temp,dst_strideUV, (height>>1));
			dec->all_copy_block[MV](src[2],dst[2],stride_uv_temp,dst_strideUV, (height>>1));
#else //IPP_EDIT
			if(8==height){
				ippiCopy16x8HP_8u_C1R(src[0], src_stride, dst[0], dst_stride,MV1,0);
				ippiCopy8x4HP_8u_C1R(src[1], src_stride_uv, dst[1], dst_strideUV,MV,0);
				ippiCopy8x4HP_8u_C1R(src[2], src_stride_uv, dst[2], dst_strideUV,MV,0);
			}else{
				ippiCopy16x16HP_8u_C1R(src[0], src_stride, dst[0], dst_stride,MV1,0);
				ippiCopy8x8HP_8u_C1R(src[1], src_stride_uv, dst[1], dst_strideUV,MV,0);
				ippiCopy8x8HP_8u_C1R(src[2], src_stride_uv, dst[2], dst_strideUV,MV,0);
			}
#endif //IPP_EDIT


		}
		else{
			// TBD: another version to add dec->mc_dst to mp2_dec_local->frame_dst to avoid
			// data cache miss;
#ifndef IPP_EDIT
			dec->all_add_block[MV1](src[0], dst[0], stride_temp, dst_stride, height);
			dec->all_add_block[MV1](src[0]+(8>>DOWNSAMPLE_FLAG(dec)), dst[0]+(8>>DOWNSAMPLE_FLAG(dec)), stride_temp, dst_stride, height);
			dec->all_add_block[MV](src[1],dst[1],stride_uv_temp,dst_strideUV, (height>>1));
			dec->all_add_block[MV](src[2],dst[2],stride_uv_temp,dst_strideUV, (height>>1));
#else //IPP_EDIT
			if(8==height){
				ippiInterpolateAverage16x8_8u_C1IR(src[0], src_stride, dst[0], dst_stride,MV1,0);
				ippiInterpolateAverage8x4_8u_C1IR(src[1], src_stride_uv, dst[1], dst_strideUV,MV,0);
				ippiInterpolateAverage8x4_8u_C1IR(src[2], src_stride_uv, dst[2], dst_strideUV,MV,0);
			}else{
				ippiInterpolateAverage16x16_8u_C1IR(src[0], src_stride, dst[0], dst_stride,MV1,0);
				ippiInterpolateAverage8x8_8u_C1IR(src[1], src_stride_uv, dst[1], dst_strideUV,MV,0);
				ippiInterpolateAverage8x8_8u_C1IR(src[2], src_stride_uv, dst[2], dst_strideUV,MV,0);
			}
#endif //IPP_EDIT


		}
	}
}



#if 0
#define CheckBound(src, stride, begin, end)	\
{											\
	if(src<begin) {							\
		src = begin;						\
	}										\
	if((src+16*stride+16)>end) {			\
		src = end-16-16*stride ;			\
	}										\
}
#else
#define CheckBound(src, stride, begin, end)	\
{											\
									\
}

#endif




