/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

//#include "fdct.h"
#define  _BIT_C

#ifndef  _BIT_C
#include <math.h>
//#include "../common/portab.h"

typedef int16_t TYPE;

//////////////////////////////////////////////////////////

#define LOAD_BUTF(m1, m2, a, b, tmp, S) \
  (m1) = (S)[(a)] + (S)[(b)];           \
  (m2) = (S)[(a)] - (S)[(b)]

#define BUTF(a, b, tmp) \
  (tmp) = (a)+(b);      \
  (b) = (a)-(b);        \
  (a) = (tmp)

#define ROTATE(m1,m2,c,k1,k2,tmp,Fix,Rnd) \
  (tmp) = ( (m1) + (m2) )*(c);            \
  (m1) *= k1;                             \
  (m2) *= k2;                             \
  (tmp) += (Rnd);                         \
  (m1) = ((m1)+(tmp))>>(Fix);             \
  (m2) = ((m2)+(tmp))>>(Fix);

#define ROTATE2(m1,m2,c,k1,k2,tmp) \
  (tmp) = ( (m1) + (m2) )*(c);     \
  (m1) *= k1;                      \
  (m2) *= k2;                      \
  (m1) = (m1)+(tmp);               \
  (m2) = (m2)+(tmp);

#define ROTATE0(m1,m2,c,k1,k2,tmp) \
  (m1) = ( (m2) )*(c);             \
  (m2) = (m2)*k2+(m1);

#define SHIFTL(x,n)   ((x)<<(n))
#define SHIFTR(x, n)  ((x)>>(n))
#define HALF(n)       (1<<((n)-1))

#define IPASS 3
#define FPASS 2
#define FIX  16


#define ROT6_C     35468
#define ROT6_SmC   50159
#define ROT6_SpC  121095
#define ROT17_C    77062
#define ROT17_SmC  25571
#define ROT17_SpC 128553
#define ROT37_C    58981
#define ROT37_SmC  98391
#define ROT37_SpC  19571
#define ROT13_C   167963
#define ROT13_SmC 134553
#define ROT13_SpC 201373


//////////////////////////////////////////////////////////

static void fdct_int32( TYPE * const In )
{
  TYPE *pIn;
  uint32_t i;

  pIn = In;
  for(i=8; i>0; --i)
  {
    int32_t mm0, mm1, mm2, mm3, mm4, mm5, mm6, mm7, Spill;

      // odd

    LOAD_BUTF(mm1,mm6, 1, 6, mm0, pIn);
    LOAD_BUTF(mm2,mm5, 2, 5, mm0, pIn);
    LOAD_BUTF(mm3,mm4, 3, 4, mm0, pIn);
    LOAD_BUTF(mm0,mm7, 0, 7, Spill, pIn);

    BUTF(mm1, mm2, Spill);
    BUTF(mm0, mm3, Spill);

    ROTATE(mm3, mm2, ROT6_C, ROT6_SmC, -ROT6_SpC, Spill, FIX-FPASS, HALF(FIX-FPASS));
    pIn[2] = mm3;
    pIn[6] = mm2;

    BUTF(mm0, mm1, Spill);
    pIn[0] = SHIFTL(mm0, FPASS);
    pIn[4] = SHIFTL(mm1, FPASS);


      // even

    mm3 = mm5 + mm7;
    mm2 = mm4 + mm6;
    ROTATE(mm2, mm3,  ROT17_C, -ROT17_SpC, -ROT17_SmC, mm0, FIX-FPASS, HALF(FIX-FPASS));
    ROTATE(mm4, mm7, -ROT37_C,  ROT37_SpC,  ROT37_SmC, mm0, FIX-FPASS, HALF(FIX-FPASS));
    mm7 += mm3;
    mm4 += mm2;
    pIn[1] = mm7;
    pIn[7] = mm4;

    ROTATE(mm5, mm6, -ROT13_C,  ROT13_SmC,  ROT13_SpC, mm0, FIX-FPASS, HALF(FIX-FPASS));
    mm5 += mm3;
    mm6 += mm2;
    pIn[3] = mm6;
    pIn[5] = mm5;

    pIn  += 8;
  }

  pIn = In;
  for(i=8; i>0; --i)
  {
    int32_t mm0, mm1, mm2, mm3, mm4, mm5, mm6, mm7, Spill;

      // odd

    LOAD_BUTF(mm1,mm6, 1*8, 6*8, mm0, pIn);
    LOAD_BUTF(mm2,mm5, 2*8, 5*8, mm0, pIn);
    BUTF(mm1, mm2, mm0);

    LOAD_BUTF(mm3,mm4, 3*8, 4*8, mm0, pIn);
    LOAD_BUTF(mm0,mm7, 0*8, 7*8, Spill, pIn);
    BUTF(mm0, mm3, Spill);

    ROTATE(mm3, mm2, ROT6_C, ROT6_SmC, -ROT6_SpC, Spill, 0,  HALF(FIX+FPASS+3));
    pIn[2*8] = (TYPE)SHIFTR(mm3,FIX+FPASS+3);
    pIn[6*8] = (TYPE)SHIFTR(mm2,FIX+FPASS+3);

    mm0 += HALF(FPASS+3) - 1;
    BUTF(mm0, mm1, Spill);
    pIn[0*8] = (TYPE)SHIFTR(mm0, FPASS+3);
    pIn[4*8] = (TYPE)SHIFTR(mm1, FPASS+3);

      // even

    mm3 = mm5 + mm7;
    mm2 = mm4 + mm6;

    ROTATE(mm2, mm3,  ROT17_C, -ROT17_SpC, -ROT17_SmC, mm0, 0, HALF(FIX+FPASS+3));
    ROTATE2(mm4, mm7, -ROT37_C,  ROT37_SpC,  ROT37_SmC, mm0);
    mm7 += mm3;
    mm4 += mm2;
    pIn[7*8] = (TYPE)SHIFTR(mm4,FIX+FPASS+3);
    pIn[1*8] = (TYPE)SHIFTR(mm7,FIX+FPASS+3);

    ROTATE2(mm5, mm6, -ROT13_C,  ROT13_SmC,  ROT13_SpC, mm0);
    mm5 += mm3;
    mm6 += mm2;
    pIn[5*8] = (TYPE)SHIFTR(mm5,FIX+FPASS+3);
    pIn[3*8] = (TYPE)SHIFTR(mm6,FIX+FPASS+3);

    pIn++;
  }
}

#else
#define USE_ACCURATE_ROUNDING

#define RIGHT_SHIFT(x, shft)  ((x) >> (shft))

#ifdef USE_ACCURATE_ROUNDING
#define ONE ((int) 1)
#define DESCALE(x, n)  RIGHT_SHIFT((x) + (ONE << ((n) - 1)), n)
#else
#define DESCALE(x, n)  RIGHT_SHIFT(x, n)
#endif

//#define CONST_BITS  13
//#define PASS1_BITS  2

#define CONST_BITS  13
#define PASS1_BITS  2


#define FIX_0_298631336  ((int)  2446)	/* FIX(0.298631336) */
#define FIX_0_390180644  ((int)  3196)	/* FIX(0.390180644) */
#define FIX_0_541196100  ((int)  4433)	/* FIX(0.541196100) */
#define FIX_0_765366865  ((int)  6270)	/* FIX(0.765366865) */ //2*cos(pi*3/8)
#define FIX_0_899976223  ((int)  7373)	/* FIX(0.899976223) */
#define FIX_1_175875602  ((int)  9633)	/* FIX(1.175875602) */
#define FIX_1_501321110  ((int) 12299)	/* FIX(1.501321110) */
#define FIX_1_847759065  ((int) 15137)	/* FIX(1.847759065) */ //2*cos(pi/8)
#define FIX_1_961570560  ((int) 16069)	/* FIX(1.961570560) */ //2*cos(pi/16)
#define FIX_2_053119869  ((int) 16819)	/* FIX(2.053119869) */
#define FIX_2_562915447  ((int) 20995)	/* FIX(2.562915447) */
#define FIX_3_072711026  ((int) 25172)	/* FIX(3.072711026) */

/* Use C code to simulate ARMv6 SIMD instruction. Video quality will be reduced a little with ARMv6 version.*/
/* wshao 2009.09.02 */
//#define	FDCT_ARMv6_C_SIMULATION	

#ifdef FDCT_ARMv6_C_SIMULATION
#define CONST_4176	  ((int)4176)
#define CONST_4433	  ((int)4433)
#define CONST_4926	  ((int)4926)
#define CONST_6436	  ((int)6436)
#define CONST_7373	  ((int)7373)
#define CONST_9633	  ((int)9633)
#define CONST_10703	  ((int)10703)
#define CONST_20995	  ((int)20995)
#endif				  
					  
/* function pointer   */
					  
/*
 * Perform an integer forward DCT on one block of samples.
 */

// static void fdct_int32(short *const block)
// {
// 	short tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
// 	short tmp10, tmp11, tmp12, tmp13;
// 	int tmp44, tmp55, tmp66, tmp77;
// 	int z1, z2, z3, z4, z5;
// 	short *blkptr;
// //	int *dataptr;
// 	int i;
// 
// 
// 	/* Pass 1: process rows. */
// 	/* Note results are scaled up by sqrt(8) compared to a true DCT; */
// 	/* furthermore, we scale the results by 2**PASS1_BITS. */
// 
// //	dataptr = data;
// 	blkptr = block;
// 	for (i = 0; i < 8; i++) {
// 		tmp0 = blkptr[0] + blkptr[7];
// 		tmp7 = blkptr[0] - blkptr[7];
// 		tmp1 = blkptr[1] + blkptr[6];
// 		tmp6 = blkptr[1] - blkptr[6];
// 		tmp2 = blkptr[2] + blkptr[5];
// 		tmp5 = blkptr[2] - blkptr[5];
// 		tmp3 = blkptr[3] + blkptr[4];
// 		tmp4 = blkptr[3] - blkptr[4];
// 
// 		/* Even part per LL&M figure 1 --- note that published figure is faulty;
// 		 * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
// 		 */
// 
// 		tmp10 = tmp0 + tmp3;
// 		tmp13 = tmp0 - tmp3;
// 		tmp11 = tmp1 + tmp2;
// 		tmp12 = tmp1 - tmp2;
// 
// 		blkptr[0] = (short)(tmp10 + tmp11) << PASS1_BITS;
// 		blkptr[4] = (short)(tmp10 - tmp11) << PASS1_BITS;
// 
// 
// #ifdef FDCT_ARMv6_C_SIMULATION
// 		blkptr[2] =	(short)DESCALE(tmp12 * CONST_4433     + tmp13 * CONST_10703, CONST_BITS - PASS1_BITS);
// 		blkptr[6] =	(short)DESCALE(tmp12 * (-CONST_10703) + tmp13 * CONST_4433 , CONST_BITS - PASS1_BITS);
// #else
// 		z1 = (tmp12 + tmp13) * FIX_0_541196100;
// 		blkptr[2] =	(short)DESCALE(z1 + tmp13 * FIX_0_765366865,    CONST_BITS - PASS1_BITS);
// 		blkptr[6] =	(short)DESCALE(z1 + tmp12 * (-FIX_1_847759065), CONST_BITS - PASS1_BITS);
// #endif
// 
// 		/* Odd part per figure 8 --- note paper omits factor of sqrt(2).
// 		 * cK represents cos(K*pi/16).
// 		 * i0..i3 in the paper are tmp4..tmp7 here.
// 		 */
// 
// #ifdef FDCT_ARMv6_C_SIMULATION
// 		{
// 			int z5, z6;
// 
// 			z5 = (tmp4 + tmp6) *(-CONST_6436) + (tmp5 + tmp7) * CONST_9633;
// 			z6 = (tmp4 + tmp6) *  CONST_9633  + (tmp5 + tmp7) * CONST_6436;
// 
// 			z4 = tmp4 * (-CONST_4926)  + tmp7 * (-CONST_7373);
// 			z1 = tmp4 * (-CONST_7373)  + tmp7 *   CONST_4926;
// 			z3 = tmp5 * (-CONST_4176)  + tmp6 * (-CONST_20995);
// 			z2 = tmp5 * (-CONST_20995) + tmp6 *   CONST_4176;
// 
// 			z4 += z5;
// 			z3 += z6;
// 			z2 += z5;
// 			z1 += z6;
// 
// 			blkptr[7] = (short)DESCALE(z4, CONST_BITS - PASS1_BITS);
// 			blkptr[5] = (short)DESCALE(z3, CONST_BITS - PASS1_BITS);
// 			blkptr[3] = (short)DESCALE(z2, CONST_BITS - PASS1_BITS);
// 			blkptr[1] = (short)DESCALE(z1, CONST_BITS - PASS1_BITS);
// 		}
// #else
// 		z1 = tmp4 + tmp7;
// 		z2 = tmp5 + tmp6;
// 		z3 = tmp4 + tmp6;
// 		z4 = tmp5 + tmp7;
// 
// 		z5 = (z3 + z4) * FIX_1_175875602;	/* sqrt(2) * c3 */
// 
// 		tmp44 = tmp4 * FIX_0_298631336;	/* sqrt(2) * (-c1+c3+c5-c7) */
// 		tmp55 = tmp5 * FIX_2_053119869;	/* sqrt(2) * ( c1+c3-c5+c7) */
// 		tmp66 = tmp6 * FIX_3_072711026;	/* sqrt(2) * ( c1+c3+c5-c7) */
// 		tmp77 = tmp7 * FIX_1_501321110;	/* sqrt(2) * ( c1+c3-c5-c7) */
// 		z1 *= -FIX_0_899976223;	/* sqrt(2) * (c7-c3) */
// 		z2 *= -FIX_2_562915447;	/* sqrt(2) * (-c1-c3) */
// 		z3 *= -FIX_1_961570560;	/* sqrt(2) * (-c3-c5) */
// 		z4 *= -FIX_0_390180644;	/* sqrt(2) * (c5-c3) */
// 
// 		z3 += z5;
// 		z4 += z5;
// 
// 		blkptr[7] = (short)DESCALE(tmp44 + z1 + z3, CONST_BITS - PASS1_BITS);
// 		blkptr[5] = (short)DESCALE(tmp55 + z2 + z4, CONST_BITS - PASS1_BITS);
// 		blkptr[3] = (short)DESCALE(tmp66 + z2 + z3, CONST_BITS - PASS1_BITS);
// 		blkptr[1] = (short)DESCALE(tmp77 + z1 + z4, CONST_BITS - PASS1_BITS);
// #endif
// //		dataptr += 8;			/* advance pointer to next row */
// 		blkptr += 8;
// 	}
// 
// 	/* Pass 2: process columns.
// 	 * We remove the PASS1_BITS scaling, but leave the results scaled up
// 	 * by an overall factor of 8.
// 	 */
// 
// 	blkptr = block;
// 	for (i = 0; i < 8; i++) {
// 		tmp0 = blkptr[0]  + blkptr[56];
// 		tmp7 = blkptr[0]  - blkptr[56];
// 		tmp1 = blkptr[8]  + blkptr[48];
// 		tmp6 = blkptr[8]  - blkptr[48];
// 		tmp2 = blkptr[16] + blkptr[40];
// 		tmp5 = blkptr[16] - blkptr[40];
// 		tmp3 = blkptr[24] + blkptr[32];
// 		tmp4 = blkptr[24] - blkptr[32];
// 
// 		/* Even part per LL&M figure 1 --- note that published figure is faulty;
// 		 * rotator "sqrt(2)*c1" should be "sqrt(2)*c6".
// 		 */
// 
// 		tmp10 = tmp0 + tmp3;
// 		tmp13 = tmp0 - tmp3;
// 		tmp11 = tmp1 + tmp2;
// 		tmp12 = tmp1 - tmp2;
// 
// 		blkptr[0]  = (short)DESCALE(tmp10 + tmp11, PASS1_BITS+3);
// 		blkptr[32] = (short)DESCALE(tmp10 - tmp11, PASS1_BITS+3)/3;
// 
// #ifdef FDCT_ARMv6_C_SIMULATION
// 		blkptr[16] = (short)DESCALE(tmp12 * CONST_4433     + tmp13 * CONST_10703, CONST_BITS + PASS1_BITS+3);
// 		blkptr[48] = (short)DESCALE(tmp12 * (-CONST_10703) + tmp13 * CONST_4433 , CONST_BITS + PASS1_BITS+3);
// #else
// 		z1 = (tmp12 + tmp13) * FIX_0_541196100;
// 		blkptr[16] = DESCALE(z1 + tmp13 * FIX_0_765366865,    CONST_BITS + PASS1_BITS+3)/3;
// 		blkptr[48] = DESCALE(z1 + tmp12 * (-FIX_1_847759065), CONST_BITS + PASS1_BITS+3)/3;
// #endif
// 		/* Odd part per figure 8 --- note paper omits factor of sqrt(2).
// 		 * cK represents cos(K*pi/16).
// 		 * i0..i3 in the paper are tmp4..tmp7 here.
// 		 */
// 
// #ifdef FDCT_ARMv6_C_SIMULATION
// 		{
// 			int z5, z6;
// 
// 			z5 = (tmp4 + tmp6) *(-CONST_6436) + (tmp5 + tmp7) * CONST_9633;
// 			z6 = (tmp4 + tmp6) *  CONST_9633  + (tmp5 + tmp7) * CONST_6436;
// 
// 			z4 = tmp4 * (-CONST_4926)  + tmp7 * (-CONST_7373);
// 			z1 = tmp4 * (-CONST_7373)  + tmp7 *   CONST_4926;
// 			z3 = tmp5 * (-CONST_4176)  + tmp6 * (-CONST_20995);
// 			z2 = tmp5 * (-CONST_20995) + tmp6 *   CONST_4176;
// 
// 			z4 += z5;
// 			z3 += z6;
// 			z2 += z5;
// 			z1 += z6;
// 
// 			blkptr[56] = (short)DESCALE(z4, CONST_BITS + PASS1_BITS+3);
// 			blkptr[40] = (short)DESCALE(z3, CONST_BITS + PASS1_BITS+3);
// 			blkptr[24] = (short)DESCALE(z2, CONST_BITS + PASS1_BITS+3);
// 			blkptr[8]  = (short)DESCALE(z1, CONST_BITS + PASS1_BITS+3);
// 		}
// #else
// 		z1 = tmp4 + tmp7;
// 		z2 = tmp5 + tmp6;
// 		z3 = tmp4 + tmp6;
// 		z4 = tmp5 + tmp7;
// 
// 		z5 = (z3 + z4) * FIX_1_175875602;	/* sqrt(2) * c3 */
// 
// 		tmp44 = tmp4 * FIX_0_298631336;	/* sqrt(2) * (-c1+c3+c5-c7) */
// 		tmp55 = tmp5 * FIX_2_053119869;	/* sqrt(2) * ( c1+c3-c5+c7) */
// 		tmp66 = tmp6 * FIX_3_072711026;	/* sqrt(2) * ( c1+c3+c5-c7) */
// 		tmp77 = tmp7 * FIX_1_501321110;	/* sqrt(2) * ( c1+c3-c5-c7) */
// 		z1 *= -FIX_0_899976223;	/* sqrt(2) * (c7-c3) */
// 		z2 *= -FIX_2_562915447;	/* sqrt(2) * (-c1-c3) */
// 		z3 *= -FIX_1_961570560;	/* sqrt(2) * (-c3-c5) */
// 		z4 *= -FIX_0_390180644;	/* sqrt(2) * (c5-c3) */
// 
// 		z3 += z5;
// 		z4 += z5;
// 
// 		blkptr[56] = (short)DESCALE(tmp44 + z1 + z3, CONST_BITS + PASS1_BITS+3)/3;
// 		blkptr[40] = (short)DESCALE(tmp55 + z2 + z4, CONST_BITS + PASS1_BITS+3)/3;
// 		blkptr[24] = (short)DESCALE(tmp66 + z2 + z3, CONST_BITS + PASS1_BITS+3)/3;
// 		blkptr[8]  = (short)DESCALE(tmp77 + z1 + z4, CONST_BITS + PASS1_BITS+3)/3;
// #endif
// 		blkptr++;				/* advance pointer to next column */
// 	}
// 
// }
#endif