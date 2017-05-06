#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "global.h"


extern void intrapred_chroma      (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);

extern int  intrapred   (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal, int ioff,int joff);
//edit by Really Yang 20110321
extern void itrans_2    (H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
//end of edit

//extern int CheckVertMV(Macroblock *currMB, int vec1_y, int block_size_y);
//add by Really Yang 20110303
extern void MCCopy16x16_c( VO_U8 *src, VO_S32 i_src_stride, VO_U8 *dst, VO_S32 i_dst_stride );
extern void MCCopy8x8_c( VO_U8 *src, VO_S32 i_src_stride, VO_U8 *dst, VO_S32 i_dst_stride );
extern void MCCopy4x4_c( VO_U8 *src, VO_S32 i_src_stride, VO_U8 *dst, VO_S32 i_dst_stride );
extern void iTransformLuma(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);
extern void iTransformChroma(H264DEC_G *pDecGlobal,H264DEC_L *pDecLocal);

//end of edit

#if defined(VOARMV7)
void MCCopy16x16_ARMV7(VO_U8 *src, VO_S32 src_stride,VO_U8 *dst, VO_S32 dst_stride);
void MCCopy16x8_ARMV7(VO_U8 *src, VO_S32 src_stride,VO_U8 *dst, VO_S32 dst_stride);
void MCCopy8x16_ARMV7(VO_U8 *src, VO_S32 src_stride,VO_U8 *dst, VO_S32 dst_stride);
void MCCopy8x8_ARMV7(VO_U8 *src, VO_S32 src_stride,VO_U8 *dst, VO_S32 dst_stride);
void MCCopy8x4_ARMV7(VO_U8 *src, VO_S32 src_stride,VO_U8 *dst, VO_S32 dst_stride);
void MCCopy4x8_ARMV7(VO_U8 *src, VO_S32 src_stride,VO_U8 *dst, VO_S32 dst_stride);
void MCCopy4x4_ARMV7(VO_U8 *src, VO_S32 src_stride,VO_U8 *dst, VO_S32 dst_stride);
void MCCopy4x2_ARMV7(VO_U8 *src, VO_S32 src_stride,VO_U8 *dst, VO_S32 dst_stride);
void MCCopy2x4_ARMV7(VO_U8 *src, VO_S32 src_stride,VO_U8 *dst, VO_S32 dst_stride);
void MCCopy2x2_ARMV7(VO_U8 *src, VO_S32 src_stride,VO_U8 *dst, VO_S32 dst_stride);



#define MCCopy16x16 MCCopy16x16_ARMV7
#define MCCopy16x8 MCCopy16x8_ARMV7
#define MCCopy8x16 MCCopy8x16_ARMV7
#define MCCopy8x8 MCCopy8x8_ARMV7
#define MCCopy8x4 MCCopy8x4_ARMV7
#define MCCopy4x8 MCCopy4x8_ARMV7
#define MCCopy4x4 MCCopy4x4_ARMV7
#define MCCopy4x2 MCCopy4x2_ARMV7
#define MCCopy2x4 MCCopy2x4_ARMV7
#define MCCopy2x2 MCCopy2x2_ARMV7
#else
#define MCCopy16x16 MCCopy16x16_c
#define MCCopy8x8 MCCopy8x8_c
#define MCCopy4x4 MCCopy4x4_c

#endif

#endif

