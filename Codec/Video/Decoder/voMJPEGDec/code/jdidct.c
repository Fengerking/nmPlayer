/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "jcommon.h"
#include "jdecoder.h"

#define W1 2841                 // 2048*sqrt(2)*cos(1*pi/16) 
#define W2 2676                 // 2048*sqrt(2)*cos(2*pi/16) 
#define W3 2408                 // 2048*sqrt(2)*cos(3*pi/16) 
#define W5 1609                 // 2048*sqrt(2)*cos(5*pi/16) 
#define W6 1108                 // 2048*sqrt(2)*cos(6*pi/16) 
#define W7 565                  // 2048*sqrt(2)*cos(7*pi/16) 

#define W1_minus_W7	2276
#define W1_plus_W7	3406
#define W3_minus_W5	799
#define W3_plus_W5	4017
#define W2_minus_W6	1568
#define W2_plus_W6	3784

#define ADDSAT32(a,Dst,Add32)		\
	b = a + Add32;					\
	c = a & Add32;					\
	a ^= Add32;						\
	a &= ~b;						\
	a |= c;							\
	a &= MaskCarry;					\
	c = a << 1;						\
	b -= c;	/* adjust neighbour */	\
	b |= c - (a >> 7); /* mask */	\
	Dst = b;						

#define SUBSAT32(a,Dst,Add32)		\
	a = ~a;							\
	b = a + Add32;					\
	c = a & Add32;					\
	a ^= Add32;						\
	a &= ~b;						\
	a |= c;							\
	a &= MaskCarry;					\
	c = a << 1;						\
	b -= c;	/* adjust neighbour */	\
	b |= c - (a >> 7); /* mask */	\
	Dst = ~b;						

void IDCT_Col1(INT16 *Blk)
{
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;
	int x567,x123;

	x0 = Blk[0];
	x4 = Blk[8];
	x3 = Blk[16];
	x7 = Blk[24];
	x1 = Blk[32] << 11;
	x6 = Blk[40];
	x2 = Blk[48];	
	x5 = Blk[56];
	
	x123=x1|x2|x3;
	x567=x5|x6|x7;

	if (!(x123|x567))
	{
		if (!x4) { // x0

			if (x0) 
				Blk[0] = (INT16)(x0 << 3);
		}
		else { // x0,x4

			x0 = (x0 << 11) + 128;    
//			x5 = W7 * x4;
			x1 = W1 * x4;
//			x2 = ((181 * W1_plus_W7 + 128) >> 8) * x4;
//			x4 = ((181 * W1_minus_W7 + 128) >> 8) * x4;

			Blk[0] = (INT16)((x0 + x1) >> 8);
//			Blk[8] = (INT16)((x0 + x2) >> 8);
//			Blk[16] = (INT16)((x0 + x4) >> 8);
//			Blk[24] = (INT16)((x0 + x5) >> 8);
//			Blk[32] = (INT16)((x0 - x5) >> 8);
//			Blk[40] = (INT16)((x0 - x4) >> 8);
//			Blk[48] = (INT16)((x0 - x2) >> 8);
//			Blk[56] = (INT16)((x0 - x1) >> 8);
		}
	}
	else if (!(x4|x567)) { // x0,x1,x2,x3
	
		x0 = (x0 << 11) + 128;    
		
		x8 = x0 + x1;
		x0 -= x1;
		x1 = W6 * (x3 + x2);
//		x2 = x1 - (W2_plus_W6) * x2;
		x3 = x1 + (W2_minus_W6) * x3;
		
		x7 = x8 + x3;
//		x8 -= x3;
//		x3 = x0 + x2;
//		x0 -= x2;
		
		Blk[0]  = (INT16)(x7 >> 8);
//		Blk[8]  = (INT16)(x3 >> 8);
//		Blk[16] = (INT16)(x0 >> 8);
//		Blk[24] = (INT16)(x8 >> 8);
//		Blk[32] = (INT16)(x8 >> 8);
//		Blk[40] = (INT16)(x0 >> 8);
//		Blk[48] = (INT16)(x3 >> 8);
//		Blk[56] = (INT16)(x7 >> 8);

		return;
	}
	else { //x0,x1,x2,x3,x4,x5,x6,x7

		x0 = (x0 << 11) + 128;    
		x8 = W7 * (x4 + x5);
		x4 = x8 + (W1_minus_W7) * x4;
//		x5 = x8 - (W1_plus_W7) * x5;
		x8 = W3 * (x6 + x7);
		x6 = x8 - (W3_minus_W5) * x6;
//		x7 = x8 - (W3_plus_W5) * x7;

		x8 = x0 + x1;
		x0 -= x1;
		x1 = W6 * (x3 + x2);
//		x2 = x1 - (W2_plus_W6) * x2;
		x3 = x1 + (W2_minus_W6) * x3;
		x1 = x4 + x6;
//		x4 -= x6;
//		x6 = x5 + x7;
//		x5 -= x7;

		x7 = x8 + x3;
//		x8 -= x3;
//		x3 = x0 + x2;
//		x0 -= x2;
//		x2 = (181 * (x4 + x5) + 128) >> 8;
//		x4 = (181 * (x4 - x5) + 128) >> 8;

		Blk[0]  = (INT16)((x7 + x1) >> 8);
//		Blk[8]  = (INT16)((x3 + x2) >> 8);
//		Blk[16] = (INT16)((x0 + x4) >> 8);
//		Blk[24] = (INT16)((x8 + x6) >> 8);
//		Blk[32] = (INT16)((x8 - x6) >> 8);
//		Blk[40] = (INT16)((x0 - x4) >> 8);
//		Blk[48] = (INT16)((x3 - x2) >> 8);
//		Blk[56] = (INT16)((x7 - x1) >> 8);
	}
}	

void IDCT_Col8(INT16 *Blk)
{
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;
	int x567,x123;

	x0 = Blk[0];
	x4 = Blk[8];
	x3 = Blk[16];
	x7 = Blk[24];
	x1 = Blk[32] << 11;
	x6 = Blk[40];
	x2 = Blk[48];	
	x5 = Blk[56];
	
	x123=x1|x2|x3;
	x567=x5|x6|x7;

	if (!(x123|x567))
	{
		if (!x4) { // x0

			if (x0) 
				Blk[0] = Blk[8] = Blk[16] = Blk[24] = Blk[32] = Blk[40] = Blk[48] = Blk[56] = (INT16)(x0 << 3);
		}
		else { // x0,x4

			x0 = (x0 << 11) + 128;    
			x5 = W7 * x4;
			x1 = W1 * x4;
			x2 = ((181 * W1_plus_W7 + 128) >> 8) * x4;
			x4 = ((181 * W1_minus_W7 + 128) >> 8) * x4;

			Blk[0] = (INT16)((x0 + x1) >> 8);
			Blk[8] = (INT16)((x0 + x2) >> 8);
			Blk[16] = (INT16)((x0 + x4) >> 8);
			Blk[24] = (INT16)((x0 + x5) >> 8);
			Blk[32] = (INT16)((x0 - x5) >> 8);
			Blk[40] = (INT16)((x0 - x4) >> 8);
			Blk[48] = (INT16)((x0 - x2) >> 8);
			Blk[56] = (INT16)((x0 - x1) >> 8);
		}
	}
	else if (!(x4|x567)) { // x0,x1,x2,x3
	
		x0 = (x0 << 11) + 128;    
		
		x8 = x0 + x1;
		x0 -= x1;
		x1 = W6 * (x3 + x2);
		x2 = x1 - (W2_plus_W6) * x2;
		x3 = x1 + (W2_minus_W6) * x3;
		
		x7 = x8 + x3;
		x8 -= x3;
		x3 = x0 + x2;
		x0 -= x2;
		
		Blk[0]  = (INT16)(x7 >> 8);
		Blk[8]  = (INT16)(x3 >> 8);
		Blk[16] = (INT16)(x0 >> 8);
		Blk[24] = (INT16)(x8 >> 8);
		Blk[32] = (INT16)(x8 >> 8);
		Blk[40] = (INT16)(x0 >> 8);
		Blk[48] = (INT16)(x3 >> 8);
		Blk[56] = (INT16)(x7 >> 8);

		return;
	}
	else { //x0,x1,x2,x3,x4,x5,x6,x7

		x0 = (x0 << 11) + 128;    
		x8 = W7 * (x4 + x5);
		x4 = x8 + (W1_minus_W7) * x4;
		x5 = x8 - (W1_plus_W7) * x5;
		x8 = W3 * (x6 + x7);
		x6 = x8 - (W3_minus_W5) * x6;
		x7 = x8 - (W3_plus_W5) * x7;

		x8 = x0 + x1;
		x0 -= x1;
		x1 = W6 * (x3 + x2);
		x2 = x1 - (W2_plus_W6) * x2;
		x3 = x1 + (W2_minus_W6) * x3;
		x1 = x4 + x6;
		x4 -= x6;
		x6 = x5 + x7;
		x5 -= x7;

		x7 = x8 + x3;
		x8 -= x3;
		x3 = x0 + x2;
		x0 -= x2;
		x2 = (181 * (x4 + x5) + 128) >> 8;
		x4 = (181 * (x4 - x5) + 128) >> 8;

		Blk[0]  = (INT16)((x7 + x1) >> 8);
		Blk[8]  = (INT16)((x3 + x2) >> 8);
		Blk[16] = (INT16)((x0 + x4) >> 8);
		Blk[24] = (INT16)((x8 + x6) >> 8);
		Blk[32] = (INT16)((x8 - x6) >> 8);
		Blk[40] = (INT16)((x0 - x4) >> 8);
		Blk[48] = (INT16)((x3 - x2) >> 8);
		Blk[56] = (INT16)((x7 - x1) >> 8);
	}
}

static __inline void IDCT_RowConst(int v, UINT8 *Dst, const UINT8 *Src)
{
	if (Src) {
		
		UINT32 MaskCarry = 0x80808080;
		UINT32 a,b,c,d;

		a = ((UINT32*)Src)[0];
		d = ((UINT32*)Src)[1];

		if (v>0)
		{
			v |= v << 8;
			v |= v << 16;

			ADDSAT32(a,((UINT32*)Dst)[0],v);
			ADDSAT32(d,((UINT32*)Dst)[1],v);
		}
		else
		if (v<0)
		{
			v = -v;
			v |= v << 8;
			v |= v << 16;

			SUBSAT32(a,((UINT32*)Dst)[0],v);
			SUBSAT32(d,((UINT32*)Dst)[1],v);
		}
		else
		{
			((UINT32*)Dst)[0] = a;
			((UINT32*)Dst)[1] = d;
		}
	}
	else	
	{
		SAT(v);

		v &= 255;
		v |= v << 8;
		v |= v << 16;

		((UINT32*)Dst)[1] = ((UINT32*)Dst)[0] = v;
	}
}   

static __inline void IDCT_RowConst2(int v, UINT8 *Dst, const UINT8 *Src)
{
	SAT(v);
	
	v &= 255;
	v |= v << 8;
	v |= v << 16;
	
	((UINT32*)Dst)[0] = v;	
}   

static __inline void IDCT_RowConst4(int v, UINT8 *Dst, const UINT8 *Src)
{
	SAT(v);
	
	v &= 255;
	v |= v << 8;
	
	((UINT16*)Dst)[0] = v;	
}   

void IDCT_Row8(INT16 *Blk, UINT8 *Dst, const UINT8 *Src)
{
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;

	x4 = Blk[1];
  	x3 = Blk[2];
  	x7 = Blk[3];
	x1 = Blk[4];
	x6 = Blk[5];
	x2 = Blk[6];
	x5 = Blk[7];
	
	if (!(x1|x2|x3|x4|x5|x6|x7))
	{
		IDCT_RowConst((Blk[0] + 32) >> 6,Dst,Src);
		return;
	}

	x1 <<= 8;
	x0 = (Blk[0] << 8) + 8192;

	x8 = W7 * (x4 + x5) + 4;
	x4 = (x8 + (W1_minus_W7) * x4) >> 3;
	x5 = (x8 - (W1_plus_W7) * x5) >> 3;
	x8 = W3 * (x6 + x7) + 4;
	x6 = (x8 - (W3_minus_W5) * x6) >> 3;
	x7 = (x8 - (W3_plus_W5) * x7) >> 3;

	x8 = x0 + x1;
	x0 -= x1;
	x1 = W6 * (x3 + x2) + 4;
	x2 = (x1 - (W2_plus_W6) * x2) >> 3;
	x3 = (x1 + (W2_minus_W6) * x3) >> 3;
	x1 = x4 + x6;
	x4 -= x6;
	x6 = x5 + x7;
	x5 -= x7;

	x7 = x8 + x3;
	x8 -= x3;
	x3 = x0 + x2;
	x0 -= x2;
	x2 = (181 * (x4 + x5) + 128) >> 8;
	x4 = (181 * (x4 - x5) + 128) >> 8;

	x5 = (x7 + x1) >> 14;
	x1 = (x7 - x1) >> 14;
	x7 = (x3 + x2) >> 14;
	x2 = (x3 - x2) >> 14;
	x3 = (x0 + x4) >> 14;
	x4 = (x0 - x4) >> 14;
	x0 = (x8 + x6) >> 14;
	x6 = (x8 - x6) >> 14;
	
	x8 = (x5|x1|x7|x2|x3|x4|x0|x6)>>8;

	if (x8)
	{
		SAT(x5);
		SAT(x7);
		SAT(x3);
		SAT(x0);
		SAT(x6);
		SAT(x4);
		SAT(x2);
		SAT(x1);
	}

	Dst[0] = (UINT8)x5;
	Dst[1] = (UINT8)x7;
	Dst[2] = (UINT8)x3;
	Dst[3] = (UINT8)x0;
	Dst[4] = (UINT8)x6;
	Dst[5] = (UINT8)x4;
	Dst[6] = (UINT8)x2;
	Dst[7] = (UINT8)x1;
}

void IDCT_Row82(INT16 *Blk, UINT8 *Dst, const UINT8 *Src)
{
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;

	x4 = Blk[1];
  	x3 = Blk[2];
  	x7 = Blk[3];
	x1 = Blk[4];
	x6 = Blk[5];
	x2 = Blk[6];
	x5 = Blk[7];
	
	if (!(x1|x2|x3|x4|x5|x6|x7))
	{
		IDCT_RowConst2((Blk[0] + 32) >> 6,Dst,Src);
		return;
	}

	x1 <<= 8;
	x0 = (Blk[0] << 8) + 8192;

	x8 = W7 * (x4 + x5) + 4;
	x4 = (x8 + (W1_minus_W7) * x4) >> 3;
	x5 = (x8 - (W1_plus_W7) * x5) >> 3;
	x8 = W3 * (x6 + x7) + 4;
	x6 = (x8 - (W3_minus_W5) * x6) >> 3;
	x7 = (x8 - (W3_plus_W5) * x7) >> 3;

	x8 = x0 + x1;
	x0 -= x1;
	x1 = W6 * (x3 + x2) + 4;
	x2 = (x1 - (W2_plus_W6) * x2) >> 3;
	x3 = (x1 + (W2_minus_W6) * x3) >> 3;
	x1 = x4 + x6;
	x4 -= x6;
	x6 = x5 + x7;
	x5 -= x7;

	x7 = x8 + x3;
	x8 -= x3;
	x3 = x0 + x2;
	x0 -= x2;
	x2 = (181 * (x4 + x5) + 128) >> 8;
	x4 = (181 * (x4 - x5) + 128) >> 8;

	x5 = (x7 + x1) >> 14;
	x2 = (x3 - x2) >> 14;
	x3 = (x0 + x4) >> 14;
	x6 = (x8 - x6) >> 14;
	
	x8 = (x5|x2|x3|x6)>>8;
	if (x8)
	{
		SAT(x5);
		SAT(x3);
		SAT(x6);
		SAT(x2);
	}

	Dst[0] = (UINT8)x5;
	Dst[1] = (UINT8)x3;
	Dst[2] = (UINT8)x6;
	Dst[3] = (UINT8)x2;
}

void IDCT_Row84(INT16 *Blk, UINT8 *Dst, const UINT8 *Src)
{
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;

	x4 = Blk[1];
  	x3 = Blk[2];
  	x7 = Blk[3];
	x1 = Blk[4];
	x6 = Blk[5];
	x2 = Blk[6];
	x5 = Blk[7];
	
	if (!(x1|x2|x3|x4|x5|x6|x7))
	{
		IDCT_RowConst4((Blk[0] + 32) >> 6,Dst,Src);
		return;
	}

	x1 <<= 8;
	x0 = (Blk[0] << 8) + 8192;

	x8 = W7 * (x4 + x5) + 4;
	x4 = (x8 + (W1_minus_W7) * x4) >> 3;
	x5 = (x8 - (W1_plus_W7) * x5) >> 3;
	x8 = W3 * (x6 + x7) + 4;
	x6 = (x8 - (W3_minus_W5) * x6) >> 3;
	x7 = (x8 - (W3_plus_W5) * x7) >> 3;

	x8 = x0 + x1;
	x0 -= x1;
	x1 = W6 * (x3 + x2) + 4;
//	x2 = (x1 - (W2_plus_W6) * x2) >> 3;
	x3 = (x1 + (W2_minus_W6) * x3) >> 3;
	x1 = x4 + x6;
//	x4 -= x6;
	x6 = x5 + x7;
//	x5 -= x7;

	x7 = x8 + x3;
	x8 -= x3;
//	x3 = x0 + x2;
//	x0 -= x2;
//	x2 = (181 * (x4 + x5) + 128) >> 8;
//	x4 = (181 * (x4 - x5) + 128) >> 8;

	x5 = (x7 + x1) >> 14;
	x6 = (x8 - x6) >> 14;
	
	x8 = (x5|x6)>>8;
	if (x8)
	{
		SAT(x5);
		SAT(x6);
	}

	Dst[0] = (UINT8)x5;
	Dst[1] = (UINT8)x6;
}

void IDCT_Row88(INT16 *Blk, UINT8 *Dst, const UINT8 *Src)
{
	int x0, x1, x2, x3, x4, x5, x6, x7, x8;

	x4 = Blk[1];
  	x3 = Blk[2];
  	x7 = Blk[3];
	x1 = Blk[4];
	x6 = Blk[5];
	x2 = Blk[6];
	x5 = Blk[7];
	
	if (!(x1|x2|x3|x4|x5|x6|x7))
	{
		x1 = (Blk[0] + 32) >> 6;
		*Dst = SAT(x1);
		return;
	}

	x1 <<= 8;
	x0 = (Blk[0] << 8) + 8192;

	x8 = W7 * (x4 + x5) + 4;
	x4 = (x8 + (W1_minus_W7) * x4) >> 3;
//	x5 = (x8 - (W1_plus_W7) * x5) >> 3;
	x8 = W3 * (x6 + x7) + 4;
	x6 = (x8 - (W3_minus_W5) * x6) >> 3;
//	x7 = (x8 - (W3_plus_W5) * x7) >> 3;

	x8 = x0 + x1;
	x0 -= x1;
	x1 = W6 * (x3 + x2) + 4;
// 	x2 = (x1 - (W2_plus_W6) * x2) >> 3;
	x3 = (x1 + (W2_minus_W6) * x3) >> 3;
	x1 = x4 + x6;
//	x4 -= x6;
//	x6 = x5 + x7;
// 	x5 -= x7;

	x7 = x8 + x3;
//	x8 -= x3;
//	x3 = x0 + x2;
//	x0 -= x2;
//	x2 = (181 * (x4 + x5) + 128) >> 8;
// 	x4 = (181 * (x4 - x5) + 128) >> 8;

	x5 = (x7 + x1) >> 14;
	
	SAT(x5);

	Dst[0] = (UINT8)x5;
}


void  IDCT_Block8x8_c(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride)
{
	IDCT_Col8(Block);
	IDCT_Col8(Block+1);
	IDCT_Col8(Block+2);
	IDCT_Col8(Block+3);
	IDCT_Col8(Block+4);
	IDCT_Col8(Block+5);
	IDCT_Col8(Block+6);
	IDCT_Col8(Block+7);

	IDCT_Row8(Block,Dest,Src);
	Dest+=DestStride;
	IDCT_Row8(Block+8,Dest,Src);
	Dest+=DestStride;
	IDCT_Row8(Block+16,Dest,Src);
	Dest+=DestStride;
	IDCT_Row8(Block+24,Dest,Src);
	Dest+=DestStride;
	IDCT_Row8(Block+32,Dest,Src);
	Dest+=DestStride;
	IDCT_Row8(Block+40,Dest,Src);
	Dest+=DestStride;
	IDCT_Row8(Block+48,Dest,Src);
	Dest+=DestStride;
	IDCT_Row8(Block+56,Dest,Src);
}

void  IDCT_Block8x8_c2(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride)
{
	IDCT_Col8(Block);
	IDCT_Col8(Block+1);
	IDCT_Col8(Block+2);
	IDCT_Col8(Block+3);
	IDCT_Col8(Block+4);
	IDCT_Col8(Block+5);
	IDCT_Col8(Block+6);
	IDCT_Col8(Block+7);

	IDCT_Row82(Block,Dest,Src);
	Dest+=DestStride;
	IDCT_Row82(Block+16,Dest,Src);
	Dest+=DestStride;
	IDCT_Row82(Block+32,Dest,Src);
	Dest+=DestStride;
	IDCT_Row82(Block+48,Dest,Src);
	Dest+=DestStride;
}

void  IDCT_Block8x8_c4(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride)
{
	IDCT_Col8(Block);
	IDCT_Col8(Block+1);
	IDCT_Col8(Block+2);
	IDCT_Col8(Block+3);
	IDCT_Col8(Block+4);
	IDCT_Col8(Block+5);
	IDCT_Col8(Block+6);
	IDCT_Col8(Block+7);

	IDCT_Row84(Block,Dest,Src);
	Dest+=DestStride;
	IDCT_Row84(Block+32,Dest,Src);
	Dest+=DestStride;
}

void  IDCT_Block8x8_c8(INT16 *Block, UINT8 *Dest, INT32 DestStride, const UINT8 *Src, INT32 srcStride)
{
	IDCT_Col1(Block);
	IDCT_Col1(Block+1);
	IDCT_Col1(Block+2);
	IDCT_Col1(Block+3);
	IDCT_Col1(Block+4);
	IDCT_Col1(Block+5);
	IDCT_Col1(Block+6);
	IDCT_Col1(Block+7);

	IDCT_Row88(Block,Dest,Src);
}
