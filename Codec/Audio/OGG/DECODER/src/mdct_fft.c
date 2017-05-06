//*@@@+++@@@@******************************************************************
//
// VisualOn voPlayer
// Copyright (C) VisualOn SoftWare Co., Ltd. All rights reserved.
//
//*@@@---@@@@******************************************************************
/*************************************************************************

Copyright(c) 2003-2009 VisualOn SoftWare Co., Ltd.

Module Name:

    mdct_fft.c

Abstract:

    mdct and fft transform c file. 

	Normalized modified discrete cosine transform power of two length transform only [64 <= n ]

Author:

    Witten Wen 10-October-2009

Revision History:

*************************************************************************/

#include "macros.h"
#include "tables.h"
#include "mdct_fft.h"
#include "vorbismemory.h"

#if !OPT_IMDCT_ARM || !defined(ARMV6)
/* N/stage point generic N stage butterfly (in place, 2 register) */
void mdct_butterfly_generic(DATA_TYPE *x,int points,int step){

	LOOKUP_T *T   = sincos_lookup0;
	DATA_TYPE *x1        = x + points      - 8;
	DATA_TYPE *x2        = x + (points>>1) - 8;
	REG_TYPE   r0;
	REG_TYPE   r1;

	do{

		X_ADD_SUB(x1[6],x2[6],r0,x1[6]);
		X_ADD_SUB(x2[7],x1[7],r1,x1[7]);		
		XPROD31( r1, r0, T[0], T[1], &x2[6], &x2[7] ); T+=step;

		X_ADD_SUB(x1[4],x2[4],r0,x1[4]);
		X_ADD_SUB(x2[5],x1[5],r1,x1[5]);
		XPROD31( r1, r0, T[0], T[1], &x2[4], &x2[5] ); T+=step;

		X_ADD_SUB(x1[2],x2[2],r0,x1[2]);
		X_ADD_SUB(x2[3],x1[3],r1,x1[3]);
		XPROD31( r1, r0, T[0], T[1], &x2[2], &x2[3] ); T+=step;

		X_ADD_SUB(x1[0],x2[0],r0,x1[0]);
		X_ADD_SUB(x2[1],x1[1],r1,x1[1]);
		XPROD31( r1, r0, T[0], T[1], &x2[0], &x2[1] ); T+=step;

		x1-=8; x2-=8;
	}while(T<sincos_lookup0+1024);
	do{
	
		X_ADD_SUB(x1[6],x2[6],r0,x1[6]);
		X_ADD_SUB(x1[7],x2[7],r1,x1[7]);	
		XNPROD31( r0, r1, T[0], T[1], &x2[6], &x2[7] ); T-=step;

		X_ADD_SUB(x1[4],x2[4],r0,x1[4]);
		X_ADD_SUB(x1[5],x2[5],r1,x1[5]);
		XNPROD31( r0, r1, T[0], T[1], &x2[4], &x2[5] ); T-=step;

		X_ADD_SUB(x1[2],x2[2],r0,x1[2]);
		X_ADD_SUB(x1[3],x2[3],r1,x1[3]);
		XNPROD31( r0, r1, T[0], T[1], &x2[2], &x2[3] ); T-=step;

		X_ADD_SUB(x1[0],x2[0],r0,x1[0]);
		X_ADD_SUB(x1[1],x2[1],r1,x1[1]);
		XNPROD31( r0, r1, T[0], T[1], &x2[0], &x2[1] ); T-=step;

		x1-=8; x2-=8;
	}while(T>sincos_lookup0);
	do{
	
		X_ADD_SUB(x2[6],x1[6],r0,x1[6]);
		X_ADD_SUB(x2[7],x1[7],r1,x1[7]);	
		XPROD31( r0, r1, T[0], T[1], &x2[6], &x2[7] ); T+=step;

		X_ADD_SUB(x2[4],x1[4],r0,x1[4]);
		X_ADD_SUB(x2[5],x1[5],r1,x1[5]);
		XPROD31( r0, r1, T[0], T[1], &x2[4], &x2[5] ); T+=step;
	
		X_ADD_SUB(x2[2],x1[2],r0,x1[2]);
		X_ADD_SUB(x2[3],x1[3],r1,x1[3]);
		XPROD31( r0, r1, T[0], T[1], &x2[2], &x2[3] ); T+=step;

		X_ADD_SUB(x2[0],x1[0],r0,x1[0]);
		X_ADD_SUB(x2[1],x1[1],r1,x1[1]);
		XPROD31( r0, r1, T[0], T[1], &x2[0], &x2[1] ); T+=step;

		x1-=8; x2-=8;
	}while(T<sincos_lookup0+1024);
	do{
		X_ADD_SUB(x1[6],x2[6],r0,x1[6]);
		X_ADD_SUB(x2[7],x1[7],r1,x1[7]);
		XNPROD31( r1, r0, T[0], T[1], &x2[6], &x2[7] ); T-=step;

		X_ADD_SUB(x1[4],x2[4],r0,x1[4]);
		X_ADD_SUB(x2[5],x1[5],r1,x1[5]);
		XNPROD31( r1, r0, T[0], T[1], &x2[4], &x2[5] ); T-=step;

		X_ADD_SUB(x1[2],x2[2],r0,x1[2]);
		X_ADD_SUB(x2[3],x1[3],r1,x1[3]);
		XNPROD31( r1, r0, T[0], T[1], &x2[2], &x2[3] ); T-=step;

		X_ADD_SUB(x1[0],x2[0],r0,x1[0]);
		X_ADD_SUB(x2[1],x1[1],r1,x1[1]);
		XNPROD31( r1, r0, T[0], T[1], &x2[0], &x2[1] ); T-=step;

		x1-=8; x2-=8;
	}while(T>sincos_lookup0);
}

/* 8 point butterfly (in place) */
void mdct_butterfly_8(DATA_TYPE *x){
	
	REG_TYPE r0,r1,r2,r3,r4,r5,r6,r7;

	X_ADD_SUB(x[4],x[0],r1,r0);
	X_ADD_SUB(x[5],x[1],r3,r2);
	X_ADD_SUB(x[6],x[2],r5,r4);
	X_ADD_SUB(x[7],x[3],r7,r6);
	
	//keep access of x[i] sequential
	x[0] = r5   + r3;
	x[1] = r7   - r1;
	x[2] = r5   - r3;
	x[3] = r7   + r1;
	x[4] = r4   - r0;
	x[5] = r6   - r2;
	x[6] = r4   + r0;
	x[7] = r6   + r2;
	MB();
}

/* 16 point butterfly (in place, 4 register) */
void mdct_butterfly_16(DATA_TYPE *x)
{

	REG_TYPE r0, r1;

	X_ADD_SUB(x[0],x[8],r0,x[ 8]);
	X_ADD_SUB(x[1],x[9],r1,x[ 9]);
	x[ 0] = MULT31((r0 + r1) , cPI2_8);
	x[ 1] = MULT31((r1 - r0) , cPI2_8);
	MB();

	X_ADD_SUB(x[10],x[2],r0,x[10]);
	X_ADD_SUB(x[3],x[11],x[ 2],x[11]);
	x[ 3] = r0;
	MB();

	X_ADD_SUB(x[12],x[4],r0,x[12]);
	X_ADD_SUB(x[13],x[5],r1,x[13]);
	x[ 4] = MULT31((r0 - r1) , cPI2_8);
	x[ 5] = MULT31((r0 + r1) , cPI2_8);
	MB();

	X_ADD_SUB(x[14],x[6],r0,x[14]);
	X_ADD_SUB(x[15],x[7],r1,x[15]);
	x[ 6] = r0; x[ 7] = r1;
	MB();

	mdct_butterfly_8(x);
	mdct_butterfly_8(x+8);
}


/* 32 point butterfly (in place, 4 register) */
void mdct_butterfly_32(DATA_TYPE *x)
{

	REG_TYPE r0, r1;

	X_ADD_SUB(x[30],x[14],r0,x[30]);
	X_ADD_SUB(x[31],x[15],r1,x[31]);
	x[14] = r0; x[15] = r1;
	MB();

	X_ADD_SUB(x[28],x[12],r0,x[28]);
	X_ADD_SUB(x[29],x[13],r1,x[29]);
	XNPROD31( r0, r1, cPI1_8, cPI3_8, &x[12], &x[13] );
	MB();

	X_ADD_SUB(x[26],x[10],r0,x[26]);
	X_ADD_SUB(x[27],x[11],r1,x[27]);
	x[10] = MULT31((r0 - r1) , cPI2_8);
	x[11] = MULT31((r0 + r1) , cPI2_8);
	MB();

	X_ADD_SUB(x[24],x[8],r0,x[24]);
	X_ADD_SUB(x[25],x[9],r1,x[25]);
	XNPROD31( r0, r1, cPI3_8, cPI1_8, &x[ 8], &x[ 9] );
	MB();

	X_ADD_SUB(x[22],x[6],r0,x[22]);
	X_ADD_SUB(x[7],x[23],x[6],x[23]);
	x[ 7] = r0;
	MB();

	X_ADD_SUB(x[4],x[20],r0,x[20]);
	X_ADD_SUB(x[5],x[21],r1,x[21]);
	XPROD31 ( r0, r1, cPI3_8, cPI1_8, &x[ 4], &x[ 5] );
	MB();

	X_ADD_SUB(x[2],x[18],r0,x[18]);
	X_ADD_SUB(x[3],x[19],r1,x[19]);
	x[ 2] = MULT31((r1 + r0) , cPI2_8);
	x[ 3] = MULT31((r1 - r0) , cPI2_8);
	MB();

	X_ADD_SUB(x[0],x[16],r0,x[16]);
	X_ADD_SUB(x[1],x[17],r1,x[17]);
	XPROD31 ( r0, r1, cPI1_8, cPI3_8, &x[ 0], &x[ 1] );
	MB();

	mdct_butterfly_16(x);
	mdct_butterfly_16(x+16);
}
#endif
STIN void mdct_butterflies(DATA_TYPE *x, int points, int shift)
{
	
	int i, j;
	int stages = 8 - shift;

	for(i=0; --stages>0; i++){
		int jPoints = points>>i;
		int jStep = 4<<(i+shift);
		for(j=0; j<(1<<i); j++){
			mdct_butterfly_generic(x+jPoints*j, jPoints, jStep);//STEP 2
		}
	}

	//do mdct_butterfly_32
	for(j=0; j<points/32; j++){
		mdct_butterfly_32(x+j*32);
	}

}


unsigned char bitrev[16] = {0x0,0x8,0x4,0xc,0x2,0xa,0x6,0xe,0x1,0x9,0x5,0xd,0x3,0xb,0x7,0xf};
#if !OPT_IMDCT_ARM || !defined(ARMV6)
STIN int bitrev12(int x)
{
  return bitrev[x>>8]|(bitrev[(x&0x00f0)>>4]<<4)|(((int)bitrev[x&0x000f])<<8);
}

STIN void mdct_bitreverse(DATA_TYPE *x,int n,int step,int shift)
{

	int          bit   = 0;
	DATA_TYPE   *w0    = x;
	DATA_TYPE   *w1    = x = w0+(n>>1);
	LOOKUP_T    *T = (step>=4)?(sincos_lookup0+(step>>1)):sincos_lookup1;
	LOOKUP_T    *Ttop  = T+1024;
	DATA_TYPE    r2;
	DATA_TYPE	 r3;  
	REG_TYPE  r0, r1, r4, r5;
	DATA_TYPE 	 x01 =  (n>>1) - 2;  //x01 = x0 + x1

	w1    -= 4;

	do{

		DATA_TYPE bitRev = (bitrev12(bit))>>shift;
	
		DATA_TYPE *x0    = x + (x01 - bitRev);
		DATA_TYPE *x1    = x + bitRev;	

		r0     = x0[0]  + x1[0];
		r5     = (x0[0] - x1[0])>>1;
		r1     = x1[1]  - x0[1];
		r4     = (x0[1] + x1[1])>>1;	

		XPROD32( r0, r1, T[1], T[0], &r2, &r3 ); T+=step;

		w0[0]  = r4     + r2;
		w0[1]  = r5     + r3;
		w1[2]  = r4     - r2;
		w1[3]  = r3     - r5;

		//second group	
		bitRev     = (bitRev + (1<<(11-shift)));
		x0     = x + (x01 - bitRev);
		x1     = x + bitRev;

		r0     = x0[0]  + x1[0];
		r5     = (x0[0] - x1[0])>>1;
		r1     = x1[1]  - x0[1];
		r4     = (x0[1] + x1[1])>>1;	

		XPROD32( r0, r1, T[1], T[0], &r2, &r3 ); T+=step;

		w0[2]  = r4     + r2;
		w0[3]  = r5     + r3;
		w1[0]  = r4     - r2;
		w1[1]  = r3     - r5;
		
		bit		+=2;
		w0		+= 4;
		w1		-= 4;
	}while(T<Ttop);

	do{

		DATA_TYPE bitRev = (bitrev12(bit))>>shift;
		DATA_TYPE *x0    =  x + (x01 - bitRev);
		DATA_TYPE *x1    =  x + bitRev;	

		r0     = x0[0]  + x1[0];
		r5     = (x0[0] - x1[0])>>1;
		r1     = x1[1]  - x0[1];
		r4     = (x0[1] + x1[1])>>1;		

		T-=step; XPROD32( r0, r1, T[0], T[1], &r2, &r3 );
	
		w0[0]  = r4     + r2;
		w0[1]  = r5     + r3;
		w1[2]  = r4     - r2;
		w1[3]  = r3     - r5;

		//second group	
		bitRev     = (bitRev + (1<<(11-shift)));
		x0     = x + (x01 - bitRev);
		x1     = x + bitRev;

		r0     = x0[0]  + x1[0];
		r5     = (x0[0] - x1[0])>>1;
		r1     = x1[1]  - x0[1];
		r4     = (x0[1] + x1[1])>>1;	

		T-=step; XPROD32( r0, r1, T[0], T[1], &r2, &r3 );
	
		w0[2]  = r4     + r2;
		w0[3]  = r5     + r3;
		w1[0]  = r4     - r2;
		w1[1]  = r3     - r5;
		
		bit		+=2;
		w0    += 4;
		w1    -= 4;
	}while(w0<w1);
}
#endif
#if 1   //enabled forever, why?
#if !OPT_IMDCT_ARM || !defined(ARMV6)
void mdct_preprocess(DATA_TYPE *in, DATA_TYPE *out, int n4, int step)
{
	/* rotate */
	//v1, v2, ..., vn-1 -> out+n2 (oX(0), oX(1), ..., oX(n/2-1))
	int n2 = n4<<1;
	DATA_TYPE *pIn	= in + n2 - 1;
	DATA_TYPE *pOut	= out + n2 + n4 - 1;
	LOOKUP_T *pA = sincos_lookup0;
	DATA_TYPE temp1=0;
	REG_TYPE t0, t1;

	while(pIn >= in+n4)
	{
		temp1 = *pIn;
		pIn -= 2;
		t0 = *pA++;
		t1 = *pA--;
		*pOut-- = MULT31(temp1, t0) - MULT31(*pIn, t1);
		*pOut-- = MULT31(*pIn, t0) + MULT31(temp1, t1);
		pIn -= 2, pA += step;
		temp1 = *pIn;
		pIn -= 2;
		t0 = *pA++;
		t1 = *pA--;
		*pOut-- = MULT31(temp1, t0) - MULT31(*pIn, t1);
		*pOut-- = MULT31(*pIn, t0) + MULT31(temp1, t1);
		pIn -= 2, pA += step;
	}
	pA++;
	while(pIn>=in)
	{
		temp1 = *pIn;
		pIn -= 2;
		t0 = *pA--;
		t1 = *pA++;
		*pOut-- = MULT31(temp1, t0) - MULT31(*pIn, t1);
		*pOut-- = MULT31(*pIn, t0) + MULT31(temp1, t1);
		pIn -= 2, pA -= step;
		temp1 = *pIn;
		pIn -= 2;
		t0 = *pA--;
		t1 = *pA++;
		*pOut-- = MULT31(temp1, t0) - MULT31(*pIn, t1);
		*pOut-- = MULT31(*pIn, t0) + MULT31(temp1, t1);
		pIn -= 2, pA -= step;
	}

	pIn		= in + n2 - 2;
	pOut	= out + n2 + n4;
	pA		= sincos_lookup0;

	while(pIn>=in+n4)
	{
		pA += step;
		temp1 = *pIn;
		pIn -= 2;
		t0 = *pA++;
		t1 = *pA--;
		*pOut++	= MULT31(temp1, t0) - MULT31(*pIn, t1);
		*pOut++ = MULT31(*pIn, t0) + MULT31(temp1, t1);
		pA += step, pIn -= 2;
		temp1 = *pIn;
		pIn -= 2;
		t0 = *pA++;
		t1 = *pA--;
		*pOut++	= MULT31(temp1, t0) - MULT31(*pIn, t1);
		*pOut++ = MULT31(*pIn, t0) + MULT31(temp1, t1);
		pIn -= 2;
	}
	pA++;
	while(pIn>=in)
	{
		pA -= step;
		temp1 = *pIn;
		pIn -= 2;
		t0 = *pA--;
		t1 = *pA++;
		*pOut++	= MULT31(temp1, t0) - MULT31(*pIn, t1);
		*pOut++ = MULT31(*pIn, t0) + MULT31(temp1, t1);
		pA -= step, pIn -= 2;
		temp1 = *pIn;
		pIn -= 2;
		t0 = *pA--;
		t1 = *pA++;
		*pOut++	= MULT31(temp1, t0) - MULT31(*pIn, t1);
		*pOut++ = MULT31(*pIn, t0) + MULT31(temp1, t1);
		pIn -= 2;
	}
}
#endif	//#if !OPT_IMDCT_ARM || !defined(ARMV6)

//#if !OPT_IMDCT_ARM || !defined(ARMV6)
void mdct_postprocess(DATA_TYPE *in, DATA_TYPE *out, int n4, int step)
{
	int n2 = n4<<1;
	LOOKUP_T *pA, *pB;
	DATA_TYPE *pOut1 = out + n2 + n4 - 1;
	DATA_TYPE *pOut2 = out + n2 + n4;
	DATA_TYPE *pIn  = out;
	DATA_TYPE temp1=0, temp2=0;
	REG_TYPE t0, t1;
	switch(step)
	{
	default: 
		{				
			//				REG_TYPE t0, t1;
			pA = (step>=4)?(sincos_lookup0+(step>>1)):sincos_lookup1;
			while(pIn<pOut1)
			{
				temp1 = *pIn++;
				temp2 = -*pIn++;
				t0 = *pA++;
				t1 = *pA--;
				*pOut1-- = MULT31(temp1, t0) + MULT31(temp2, t1);
				*pOut2++ = MULT31(temp2, t0) - MULT31(temp1, t1);

				pA += step;
				temp1 = *pIn++;
				temp2 =- *pIn++;
				t0 = *pA++;
				t1 = *pA--;
				*pOut1-- = MULT31(temp1, t0) + MULT31(temp2, t1);
				*pOut2++ = MULT31(temp2, t0) - MULT31(temp1, t1);

				pA += step;
				temp1 = *pIn++;
				temp2 = -*pIn++;
				t0 = *pA++;
				t1 = *pA--;
				*pOut1-- = MULT31(temp1, t0) + MULT31(temp2, t1);
				*pOut2++ = MULT31(temp2, t0) - MULT31(temp1, t1);

				pA += step;
				temp1 = *pIn++;
				temp2 = -*pIn++;
				t0 = *pA++;
				t1 = *pA--;
				*pOut1-- = MULT31(temp1, t0) + MULT31(temp2, t1);
				*pOut2++ = MULT31(temp2, t0) - MULT31(temp1, t1);

				pA += step;
			};
			break;
		}
	case 1:
		{
			REG_TYPE  v0,v1;
			pA		= sincos_lookup0;
			pB		= sincos_lookup1;
			t0 = (*pA++)>>1;
			t1 = (*pA++)>>1;
			while(pIn<pOut1)
			{
				temp1 = *pIn++;
				temp2 = -*pIn++;
				t0 += (v0 = (*pB++)>>1);
				t1 += (v1 = (*pB++)>>1);
				*pOut1-- = MULT31(temp1, t0) + MULT31(temp2, t1);
				*pOut2++ = MULT31(temp2, t0) - MULT31(temp1, t1);

				temp1 = *pIn++;
				temp2 = -*pIn++;
				v0 += (t0 = (*pA++)>>1);
				v1 += (t1 = (*pA++)>>1);
				*pOut1-- = MULT31(temp1, v0) + MULT31(temp2, v1);
				*pOut2++ = MULT31(temp2, v0) - MULT31(temp1, v1);

				temp1 = *pIn++;
				temp2 = -*pIn++;
				t0 += (v0 = (*pB++)>>1);
				t1 += (v1 = (*pB++)>>1);
				*pOut1-- = MULT31(temp1, t0) + MULT31(temp2, t1);
				*pOut2++ = MULT31(temp2, t0) - MULT31(temp1, t1);

				temp1 = *pIn++;
				temp2 = -*pIn++;
				v0 += (t0 = (*pA++)>>1);
				v1 += (t1 = (*pA++)>>1);
				*pOut1-- = MULT31(temp1, v0) + MULT31(temp2, v1);
				*pOut2++ = MULT31(temp2, v0) - MULT31(temp1, v1);
			}
			break;
		}
	case 0:
		{
			REG_TYPE  v0,v1,q0,q1;
			pA         = sincos_lookup0;
			pB         = sincos_lookup1;
			t0        = *pA++;
			t1        = *pA++;
			while(pIn<pOut1)
			{
				temp1 = *pIn++;
				temp2 = -*pIn++;
				v0  = *pB++;
				v1  = *pB++;
				t0 +=  (q0 = (v0-t0)>>2);
				t1 +=  (q1 = (v1-t1)>>2);
				*pOut1-- = MULT31(temp1, t0) + MULT31(temp2, t1);
				*pOut2++ = MULT31(temp2, t0) - MULT31(temp1, t1);

				temp1 = *pIn++;
				temp2 = -*pIn++;
				t0  = v0-q0;
				t1  = v1-q1;
				*pOut1-- = MULT31(temp1, t0) + MULT31(temp2, t1);
				*pOut2++ = MULT31(temp2, t0) - MULT31(temp1, t1);

				temp1 = *pIn++;
				temp2 = -*pIn++;
				t0  = *pA++;
				t1  = *pA++;
				v0 += (q0 = (t0-v0)>>2);
				v1 += (q1 = (t1-v1)>>2);
				*pOut1-- = MULT31(temp1, v0) + MULT31(temp2, v1);
				*pOut2++ = MULT31(temp2, v0) - MULT31(temp1, v1);

				temp1 = *pIn++;
				temp2 = -*pIn++;
				v0  = t0-q0;
				v1  = t1-q1;
				*pOut1-- = MULT31(temp1, v0) + MULT31(temp2, v1);
				*pOut2++ = MULT31(temp2, v0) - MULT31(temp1, v1);
			}
			break;
		}
	}
}
//#endif	//#if !OPT_IMDCT_ARM || !defined(ARMV6)

/* dct4(y1, (0,...N/2-1)) to imdct(y0, (0,..., N-1)):
	y0[n] = y1[n+N/4],									n = 0,...,N/4
	y0[n] = y1[n+N/4] = -y1[N-1-n-N/4] = -y1[3N/4-1-n],	n = N/4,...,3N/4
	y0[n] = -y0[n-N] = -y1[n-N+N/4] = -y1[n-3N/4],		n = 3N/4,...,N
*/
void dctiv2imdct(DATA_TYPE *out, int n2, int n4)
{
	DATA_TYPE *pIn = out + n2 + n4 - 1, *pOut2 = out + n4;
	DATA_TYPE *pOut1 = pOut2 - 1;

	while(pOut2<pIn)
	{
		*pOut2++ = -(*pOut1-- = *pIn--);
		*pOut2++ = -(*pOut1-- = *pIn--);
		*pOut2++ = -(*pOut1-- = *pIn--);
		*pOut2++ = -(*pOut1-- = *pIn--);
	}
	pIn = out + n2 + n4;
	pOut1 = pIn - 1;
	pOut2 = out + n2;
	while(pOut1>pOut2)
	{
		*pOut1-- = *pIn++;
		*pOut1-- = *pIn++;
		*pOut1-- = *pIn++;
		*pOut1-- = *pIn++;
	}
}

void MDCTBackward(int n, DATA_TYPE *in, DATA_TYPE *out)
{
	int n2 = n>>1;
	int n4 = n>>2;
	
//	LOOKUP_T *pA=sincos_lookup0;
//	LOOKUP_T *pB=sincos_lookup1;
	int shift=0, step=0;
//	DATA_TYPE temp1=0, temp2=0;
//	REG_TYPE t0, t1;
//	int ai = sizeof(double);
//	float a = 0.236, b = 0.418, c;
//	int ai;
//	ai = sizeof(a+b);
	for (shift = 6; !(n&(1<<shift)); shift++);
	shift = 13 - shift;
	step = 2<<shift;
	
	mdct_preprocess(in, out, n4, step);

	mdct_butterflies(out+n2, n2, shift);//STEP2, STEP3,
	mdct_bitreverse(out, n, step, shift);

	/* rotate + window */
	step >>= 2;
	
	mdct_postprocess(in, out, n4, step);
	dctiv2imdct(out, n2, n4);

}
#else
	//delete unused code

#endif