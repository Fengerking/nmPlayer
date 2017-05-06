

#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

extern void ihadamard4x4 (short *tblock);
extern void ihadamard2x2 (short tblock[4], VO_S16* cof,VO_S32 mul,VO_S32 qp);
void Idct4x4Add_ARMV7(short *tblock, unsigned char  *imgBuf1, int dst_stride); 
void Idct8x8Add_ARMV7(short *tblock, unsigned char  *imgBuf1, int dst_stride); 

void Idct4x4DCAdd_ARMV7(int dc, unsigned char  *imgBuf1, int dst_stride); 

void Idct4x4Add_C(short *tblock, imgpel  *imgBuf1, int dst_stride);
void Idct4x4DCAdd_C(int dc, imgpel  *imgBuf1, int dst_stride);
void Idct8x8Add_C(short *tblock, imgpel  *imgBuf1, int dst_stride);

#if defined(VOARMV6)
extern void Idct4x4Add_ARMV6(short *tblock, unsigned char  *imgBuf1, int dst_stride); 
extern void Idct4x4DCAdd_ARMV6(int dc, unsigned char  *imgBuf1, int dst_stride); 
extern void Idct8x8Add_ARMV6(short *tblock, unsigned char  *imgBuf1, int dst_stride); 
#endif



#if defined(VOARMV7)
#define Idct4x4Add 	Idct4x4Add_ARMV7
#define Idct4x4DCAdd 	Idct4x4DCAdd_ARMV7
#define Idct8x8Add 	Idct8x8Add_ARMV7
#elif defined(VOARMV6)
#define Idct4x4Add 	Idct4x4Add_ARMV6
#define Idct4x4DCAdd 	Idct4x4DCAdd_ARMV6
#define Idct8x8Add 	Idct8x8Add_ARMV6
#elif defined(VOSSSE3)
#define Idct4x4Add 	vo_Idct4x4Add_mmx
#define Idct4x4DCAdd 	vo_Idct4x4DCAdd_mmx
#define Idct8x8Add 	vo_Idct8x8Add_sse2_ex
#else
#define Idct4x4Add 	Idct4x4Add_C
#define Idct4x4DCAdd 	Idct4x4DCAdd_C
#define Idct8x8Add 	Idct8x8Add_C
#endif

//end of add

#endif //_TRANSFORM_H_
