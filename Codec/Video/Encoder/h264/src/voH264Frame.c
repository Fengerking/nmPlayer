/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2011						*
*																		*
************************************************************************/

#include "voH264EncGlobal.h"
#include <string.h>

extern void CopyPlaneMod16_ARMV7(VO_U8 *dst, VO_S32 i_dst,VO_U8 *src, VO_S32 i_src, VO_S32 w, VO_S32 h);
extern void UV420pack_to_UV420_mod16_ARMV7(VO_U8 * u_dst, VO_U8 * v_dst,
		   VO_S32 uv_dst_stride, VO_U8 * uv_src,VO_S32 uv_src_stride,
		   VO_S32 width, VO_S32 height);

AVC_FRAME *AVCCrateFrame( H264ENC *pEncGlobal )
{
  AVC_FRAME *frame;
  VO_S32 i;

  VO_S32 tmp_size;
  VO_S32 i_padv = PADV;
  VO_S32 luma_plane_size;
  VO_S32 chroma_plane_size;

  //CHECKED_MALLOCZERO( frame, sizeof(AVC_FRAME) );
  tmp_size = sizeof(AVC_FRAME);
  CHECK_SIZE(tmp_size);
  MEMORY_ALLOC( frame, pEncGlobal->buffer_total , tmp_size, pEncGlobal->buffer_used,AVC_FRAME *);

  luma_plane_size = (pEncGlobal->i_stride[0] * (pEncGlobal->height[0] + 2*i_padv));
  chroma_plane_size = (pEncGlobal->i_stride[1] * (pEncGlobal->height[1] + 2*i_padv));
  for( i = 1; i < 3; i++ )
  {
    //CHECKED_MALLOC( frame->buffer[i], chroma_plane_size );
    MEMORY_ALLOC( frame->buffer[i], pEncGlobal->buffer_total , chroma_plane_size, pEncGlobal->buffer_used,VO_U8 *);
    frame->plane[i] = frame->buffer[i] + (pEncGlobal->i_stride[i] * i_padv + PADH)/2;
  }

  //CHECKED_MALLOC( frame->buffer[0], luma_plane_size );
  MEMORY_ALLOC( frame->buffer[0], pEncGlobal->buffer_total , luma_plane_size, pEncGlobal->buffer_used,VO_U8 *);
  frame->plane[0] = frame->buffer[0] + pEncGlobal->i_stride[0] * i_padv + PADH;

  return frame;

    //voFree( frame );
    return NULL;
}
/*
void AVCDeleteFrame( AVC_FRAME *frame )
{
    VO_S32 i, j;

	for( i = 0; i < 4; i++ )
		voFree( frame->buffer[i] );
	

	voFree( frame );
}*/

static void CopyPlane( VO_U8 *dst, VO_S32 i_dst,
                        VO_U8 *src, VO_S32 i_src, VO_S32 w, VO_S32 h)
{
  while( h-- )
  {
    memcpy( dst, src, w );
    dst += i_dst;
    src += i_src;
  }
}

void yuv420pack_to_yuv420_c(VO_U8 * y_dst, VO_U8 * u_dst, VO_U8 * v_dst,
				   VO_S32 y_dst_stride, VO_S32 uv_dst_stride,
				   VO_U8 * y_src, VO_U8 * uv_src,
				   VO_S32 y_src_stride, VO_S32 uv_src_stride,
				   VO_S32 width, VO_S32 height)
{
	VO_S32 width2 = width / 2;
	VO_S32 height2 = height / 2;
	VO_S32 y, x;
	
	for (y = height; y; y--){
		memcpy(y_dst, y_src, width);
		y_src += y_src_stride;
		y_dst += y_dst_stride;
	}
	
	for (y = height2; y; y--){
		for(x = width2; x; x--){ 
			*u_dst++ = *uv_src++;
			*v_dst++ = *uv_src++;
		}
		uv_src -= width;
		uv_src += uv_src_stride;
		u_dst -= width2;
		u_dst += uv_dst_stride;
		v_dst -= width2;
		v_dst += uv_dst_stride;
	}
}

void uv420pack_to_uv420_c(VO_U8 * u_dst, VO_U8 * v_dst,
				   VO_S32 uv_dst_stride, VO_U8 * uv_src,VO_S32 uv_src_stride,
				   VO_S32 width, VO_S32 height)
{
	VO_S32 width2 = width / 2;
	VO_S32 height2 = height / 2;
	VO_S32 y, x;
	
	for (y = height2; y; y--){
		for(x = width2; x; x--){ 
			*u_dst++ = *uv_src++;
			*v_dst++ = *uv_src++;
		}
		uv_src -= width;
		uv_src += uv_src_stride;
		u_dst -= width2;
		u_dst += uv_dst_stride;
		v_dst -= width2;
		v_dst += uv_dst_stride;
	}
}

//add by Really Yang 20110418
void uyvy422pack_to_uv420_c(VO_U8 * y_dst,VO_S32 y_dst_stride,VO_U8 * u_dst, VO_U8 * v_dst,
				   VO_S32 uv_dst_stride, VO_U8 * src,VO_S32 src_stride,
				   VO_S32 width, VO_S32 height)
{
	VO_U8 * src0 = src;
	VO_U8 * src1 = src0 + width*2;
	VO_U8 *dst_y1 = y_dst + y_dst_stride;
	
	VO_S32 i, j;
	
	for(j = 0; j < height; j += 2){
		for(i = 0; i < width; i += 2){
			VO_S32 u0, u1, v0, v1;
			u0 = *(src0++);
			*(y_dst++) = *(src0++);
			v0 = *(src0++);
			*(y_dst++) = *(src0++);
			u1 = *(src1++);
			*(dst_y1++) = *(src1++);
			v1 = *(src1++);
			*(dst_y1++) = *(src1++);
			*(u_dst++) = (VO_U8)(u0 + u1 + 1)/2;
			*(v_dst++) = (VO_U8)(v0 + v1 + 1)/2;
		}
		src0 += width*2;
		src1 = src0 + width*2;
		y_dst -= width;
		y_dst += y_dst_stride*2;
		dst_y1 = y_dst + y_dst_stride;
		
		u_dst -= width/2;
		u_dst += uv_dst_stride;
		v_dst -= width/2;
		v_dst += uv_dst_stride;
	}
}
//end of add

VO_S32 PreprocessPic( H264ENC *pEncGlobal, AVC_FRAME *dst, VO_VIDEO_BUFFER *src )
{

    VO_S32 i;
    /*{
		FILE *fp = fopen("/sdcard/dump.yuv", "ab+");
		VO_U8* src1;
		src1 = src->Buffer[0];
        for(i = 0;i<pEncGlobal->OutParam.height;i++,src1+=src->Stride[0])
        {
		  fwrite(src1, 1, src->Stride[0], fp);
        }
		src1 = src->Buffer[1];
        for(i = 0;i<pEncGlobal->OutParam.height/2;i++,src1+=src->Stride[1])
        {
		  fwrite(src1, 1, src->Stride[1], fp);
        }
		src1 = src->Buffer[2];
        for(i = 0;i<pEncGlobal->OutParam.height/2;i++,src1+=src->Stride[2])
        {
		  fwrite(src1, 1, src->Stride[2], fp);
        }

		fclose(fp);

	}*/
	//YU_TBD, support other input type, refer to IMGPreprocess
	switch(src->ColorType)
	{
		case VO_COLOR_YUV_PLANAR420:
		    for( i=0; i<3; i++ ){
		        VO_U8 *plane = src->Buffer[i];
		        VO_S32 stride = src->Stride[i];
		        VO_S32 width = pEncGlobal->OutParam.width >> !!i;
		        VO_S32 height = pEncGlobal->OutParam.height >> !!i;

		        CopyPlane( dst->plane[i], pEncGlobal->i_stride[i], plane, stride, width, height );
		    }
			break;
		case VO_COLOR_YUV_420_PACK:
			{
				VO_U8 *plane = src->Buffer[0];
		        VO_S32 stride = src->Stride[0];
		        VO_S32 width = pEncGlobal->OutParam.width;
		        VO_S32 height = pEncGlobal->OutParam.height;
#if defined(VOARMV7)
				if(width%16==0 && height%16==0)
				{
		        	CopyPlaneMod16_ARMV7( dst->plane[0], pEncGlobal->i_stride[0], plane, stride, width, height );
					UV420pack_to_UV420_mod16_ARMV7(dst->plane[1],dst->plane[2],pEncGlobal->i_stride[1],src->Buffer[1],
											src->Stride[1],pEncGlobal->OutParam.width,pEncGlobal->OutParam.height);
				}
				else
				{
					CopyPlane( dst->plane[0], pEncGlobal->i_stride[0], plane, stride, width, height );
					uv420pack_to_uv420_c(dst->plane[1],dst->plane[2],pEncGlobal->i_stride[1],src->Buffer[1],
											src->Stride[1],pEncGlobal->OutParam.width,pEncGlobal->OutParam.height);
				}
#else
				CopyPlane( dst->plane[0], pEncGlobal->i_stride[0], plane, stride, width, height );
				uv420pack_to_uv420_c(dst->plane[1],dst->plane[2],pEncGlobal->i_stride[1],src->Buffer[1],
											src->Stride[1],width,height);
#endif
			}
			break;
		case VO_COLOR_UYVY422_PACKED:
			{
		        VO_S32 width = pEncGlobal->OutParam.width;
		        VO_S32 height = pEncGlobal->OutParam.height;
#if defined(VOARMV7)
				if(width%16==0 && height%16==0)
				{
		        	uyvy422pack_to_uv420_c(dst->plane[0], pEncGlobal->i_stride[0],dst->plane[1],dst->plane[2],pEncGlobal->i_stride[1],src->Buffer[0],
											src->Stride[0],pEncGlobal->OutParam.width,pEncGlobal->OutParam.height);
				}
				else
				{
					uyvy422pack_to_uv420_c(dst->plane[0], pEncGlobal->i_stride[0],dst->plane[1],dst->plane[2],pEncGlobal->i_stride[1],src->Buffer[0],
											src->Stride[0],pEncGlobal->OutParam.width,pEncGlobal->OutParam.height);
				}
#else
				uyvy422pack_to_uv420_c(dst->plane[0], pEncGlobal->i_stride[0],dst->plane[1],dst->plane[2],pEncGlobal->i_stride[1],src->Buffer[0],
											src->Stride[0],width,height);
#endif
			}
		    break;
		case VO_COLOR_YUV_420_PACK_2:
			{
				VO_U8 *plane = src->Buffer[0];
		        VO_S32 stride = src->Stride[0];
		        VO_S32 width = pEncGlobal->OutParam.width;
		        VO_S32 height = pEncGlobal->OutParam.height;
#if defined(VOARMV7)
				if(width%16==0 && height%16==0)
				{
		        	CopyPlaneMod16_ARMV7( dst->plane[0], pEncGlobal->i_stride[0], plane, stride, width, height );
					UV420pack_to_UV420_mod16_ARMV7(dst->plane[2],dst->plane[1],pEncGlobal->i_stride[1],src->Buffer[1],
											src->Stride[1],pEncGlobal->OutParam.width,pEncGlobal->OutParam.height);
				}
				else
				{
					CopyPlane( dst->plane[0], pEncGlobal->i_stride[0], plane, stride, width, height );
					uv420pack_to_uv420_c(dst->plane[2],dst->plane[1],pEncGlobal->i_stride[1],src->Buffer[1],
											src->Stride[1],pEncGlobal->OutParam.width,pEncGlobal->OutParam.height);
				}
#else
				CopyPlane( dst->plane[0], pEncGlobal->i_stride[0], plane, stride, width, height );
				uv420pack_to_uv420_c(dst->plane[2],dst->plane[1],pEncGlobal->i_stride[1],src->Buffer[1],
											src->Stride[1],pEncGlobal->OutParam.width,pEncGlobal->OutParam.height);
#endif
			}
			break;
		default:
			break;
	}
    return 0;
}

static void ExpandPlane( VO_U8 *pix, VO_S32 stride, VO_S32 width, VO_S32 height, VO_S32 padh, VO_S32 padv)
{
  VO_S32 j;
  for( j = 0; j < height; j++ )
  {
    /* left band */
    memset( pix-padh+j*stride, (pix+j*stride)[0], padh );
    /* right band */
    memset( pix+width+j*stride, (pix+width-1+j*stride)[0], padh );
  }
  for( j = 0; j < padv; j++ )
    memcpy( pix-padh-(j+1)*stride,pix-padh, width+2*padh );
  for( j = 0; j < padv; j++ )
    memcpy( pix-padh+(j+height)*stride, pix-padh+(height-1)*stride, width+2*padh );
}

void FillBorder( H264ENC *pEncGlobal, AVC_FRAME *frame )
{
  //luma
  ExpandPlane( frame->plane[0], pEncGlobal->i_stride[0], 16*pEncGlobal->mb_width, 16*pEncGlobal->mb_height, 32, 32);
  //u
  ExpandPlane( frame->plane[1], pEncGlobal->i_stride[1], 8*pEncGlobal->mb_width, 8*pEncGlobal->mb_height, 16, 16);
  //v
  ExpandPlane( frame->plane[2], pEncGlobal->i_stride[2], 8*pEncGlobal->mb_width, 8*pEncGlobal->mb_height, 16, 16);
}


