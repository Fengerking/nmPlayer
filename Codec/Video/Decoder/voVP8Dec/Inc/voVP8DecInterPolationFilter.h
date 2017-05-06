
/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
 
#ifndef __VP8_DEC_INTERPF_H
#define __VP8_DEC_INTERPF_H
#include "voVP8DecID.h"


#ifdef VOARMV7
extern   void vp8_sixtap_predict16x16_neon(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
extern   void vp8_sixtap_predict8x8_neon(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
extern   void vp8_sixtap_predict8x4_neon(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
extern   void vp8_sixtap_predict_neon(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

extern   void vp8_bilinear_predict16x16_neon(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
extern   void vp8_bilinear_predict8x8_neon(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
extern   void vp8_bilinear_predict8x4_neon(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
extern   void vp8_bilinear_predict4x4_neon(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

#define vp8_sixtap_predict16x16		vp8_sixtap_predict16x16_neon
#define vp8_sixtap_predict8x8		vp8_sixtap_predict8x8_neon
#define vp8_sixtap_predict8x4		vp8_sixtap_predict8x4_neon
#define vp8_sixtap_predict		    vp8_sixtap_predict_neon

#define vp8_bilinear_predict16x16	vp8_bilinear_predict16x16_neon
#define vp8_bilinear_predict8x8		vp8_bilinear_predict8x8_neon
#define vp8_bilinear_predict8x4		vp8_bilinear_predict8x4_neon
#define vp8_bilinear_predict4x4		vp8_bilinear_predict4x4_neon
#elif VOARMV6
void vp8_sixtap_predict16x16_armv6(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_sixtap_predict8x8_armv6(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_sixtap_predict8x4_armv6(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_sixtap_predict_armv6(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

void vp8_bilinear_predict16x16_armv6(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_bilinear_predict8x8_armv6(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_bilinear_predict8x4_armv6(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_bilinear_predict4x4_armv6(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

#define vp8_sixtap_predict16x16		vp8_sixtap_predict16x16_armv6
#define vp8_sixtap_predict8x8		vp8_sixtap_predict8x8_armv6
#define vp8_sixtap_predict8x4		vp8_sixtap_predict8x4_armv6
#define vp8_sixtap_predict		    vp8_sixtap_predict_armv6

#define vp8_bilinear_predict16x16	vp8_bilinear_predict16x16_armv6
#define vp8_bilinear_predict8x8		vp8_bilinear_predict8x8_armv6
#define vp8_bilinear_predict8x4		vp8_bilinear_predict8x4_armv6
#define vp8_bilinear_predict4x4		vp8_bilinear_predict4x4_armv6
#else
void vp8_sixtap_predict16x16_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_sixtap_predict8x8_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_sixtap_predict8x4_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_sixtap_predict_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

void vp8_bilinear_predict16x16_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_bilinear_predict8x8_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_bilinear_predict8x4_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);
void vp8_bilinear_predict4x4_c(unsigned char *src, int src_pitch, int xofst, int yofst, unsigned char *dst, int dst_pitch);

#define vp8_sixtap_predict16x16		vp8_sixtap_predict16x16_c
#define vp8_sixtap_predict8x8		vp8_sixtap_predict8x8_c
#define vp8_sixtap_predict8x4		vp8_sixtap_predict8x4_c
#define vp8_sixtap_predict		    vp8_sixtap_predict_c

#define vp8_bilinear_predict16x16	vp8_bilinear_predict16x16_c
#define vp8_bilinear_predict8x8		vp8_bilinear_predict8x8_c
#define vp8_bilinear_predict8x4		vp8_bilinear_predict8x4_c
#define vp8_bilinear_predict4x4		vp8_bilinear_predict4x4_c
#endif


#ifdef VOARMV7
extern void vp8_copy_mem16x16_neon(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride);
extern void vp8_copy_mem8x8_neon(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride);
extern void vp8_copy_mem8x4_neon(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride);

#define vp8_copy_mem16x16		vp8_copy_mem16x16_neon
#define vp8_copy_mem8x8		vp8_copy_mem8x8_neon
#define vp8_copy_mem8x4	    vp8_copy_mem8x4_neon
#elif VOARMV6
extern void vp8_copy_mem16x16_v6(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride);
extern void vp8_copy_mem8x8_v6(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride);
extern void vp8_copy_mem8x4_v6(unsigned char *src, int src_stride, unsigned char *dst, int dst_stride);

#define vp8_copy_mem16x16		vp8_copy_mem16x16_v6
#define vp8_copy_mem8x8		vp8_copy_mem8x8_v6
#define vp8_copy_mem8x4	    vp8_copy_mem8x4_v6
#else
extern void vp8_copy_mem16x16_c(unsigned char *src,int src_stride,unsigned char *dst,int dst_stride);
extern void vp8_copy_mem8x8_c(unsigned char *src,int src_stride,unsigned char *dst,int dst_stride);
extern void vp8_copy_mem8x4_c(unsigned char *src,int src_stride,unsigned char *dst,int dst_stride);

#define vp8_copy_mem16x16		vp8_copy_mem16x16_c
#define vp8_copy_mem8x8		vp8_copy_mem8x8_c
#define vp8_copy_mem8x4	    vp8_copy_mem8x4_c
#endif

#endif