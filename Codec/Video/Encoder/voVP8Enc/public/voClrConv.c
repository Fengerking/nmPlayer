#include "voVP8.h"

void CopyPlane( 
    VO_U8 *dst, 
    VO_S32 i_dst,
    VO_U8 *src, 
    VO_S32 i_src, 
    VO_S32 w, 
    VO_S32 h
    )
{
    while( h-- )
    {
        memcpy( dst, src, w );
        dst += i_dst;
        src += i_src;
    }
}

void yuv420pack_to_yuv420_c(
    VO_U8 * y_dst, 
    VO_U8 * u_dst, 
    VO_U8 * v_dst,
    VO_S32 y_dst_stride, 
    VO_S32 uv_dst_stride,
    VO_U8 * y_src, 
    VO_U8 * uv_src,
    VO_S32 y_src_stride, 
    VO_S32 uv_src_stride,
    VO_S32 width, 
    VO_S32 height
    )
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

void uv420pack_to_uv420_c(
    VO_U8 * u_dst, 
    VO_U8 * v_dst,
    VO_S32 uv_dst_stride, 
    VO_U8 * uv_src,
    VO_S32 uv_src_stride,
    VO_S32 width, 
    VO_S32 height
    )
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
void uyvy422pack_to_uv420_c(
    VO_U8 * y_dst,
    VO_S32 y_dst_stride,
    VO_U8 * u_dst, 
    VO_U8 * v_dst,
    VO_S32 uv_dst_stride, 
    VO_U8 * src,
    VO_S32 src_stride,
    VO_S32 width, 
    VO_S32 height
    )
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
            *(u_dst++) = (u0 + u1 + 1)/2;
            *(v_dst++) = (v0 + v1 + 1)/2;
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
