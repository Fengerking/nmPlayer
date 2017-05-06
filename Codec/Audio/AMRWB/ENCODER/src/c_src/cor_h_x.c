/***********************************************************************
*                                                                      *
*         VisualOn, Inc. Confidential and Proprietary, 2003-2010       *
*                                                                      *
************************************************************************/
/***********************************************************************
*       File: cor_h_x.c                                                *
*                                                                      *
*	   Description:Compute correlation between target "x[]" and "h[]"  *
*	               Designed for codebook search (24 pulses, 4 tracks,  * 
*				   4 pulses per track, 16 positions in each track) to  *
*				   avoid saturation.                                   *
*                                                                      *
************************************************************************/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"

#define L_SUBFR   64
#define NB_TRACK  4
#define STEP      4

#define vo_max(a,b) if(b > a) {a = b;}
#if 1
void voAMRWBEnccor_h_x(
			 Word16 h[],                           /* (i) Q12 : impulse response of weighted synthesis filter */
			 Word16 x[],                           /* (i) Q0  : target vector                                 */
			 Word16 dn[]                           /* (o) <12bit : correlation between target and h[]         */
)
{
	Word32 L_max1,L_max2,L_max3,L_max4,L_tmp1,L_tmp2,L_tmp3,L_tmp4,temp1,temp2,temp3;
	Word32 y[L_SUBFR],L_tot;
	Word32 *p1,*p2,*y32 = y;
	Word32 nIntraLens,nOutLens;

	/* first keep the result on 32 bits and find absolute maximum */
	L_tot = 1L;
	nOutLens =15;

	L_max1 = L_max2 = L_max3 = L_max4 = 0L;
	do{
		L_tmp1 = L_tmp2 = L_tmp3 = L_tmp4 = 0L;							/* 1 -> to avoid null dn[] */
		p1 = (Word32 *)h,p2 = (Word32 *)x;
		x += STEP;
		temp2 = *p2++;
		nIntraLens = nOutLens << 1;
		do{
			temp1 = *p1++;
			L_tmp1 += extract_l(temp1) * extract_l(temp2);
			L_tmp2 += extract_l(temp1) * extract_h(temp2);
			L_tmp1 += extract_h(temp1) * extract_h(temp2);

			temp2 = *p2++;
			L_tmp2 += extract_h(temp1) * extract_l(temp2); 
			L_tmp3 += extract_l(temp1) * extract_l(temp2);
			L_tmp4 += extract_l(temp1) * extract_h(temp2);
			L_tmp3 += extract_h(temp1) * extract_h(temp2);

			temp3 = *p2;
			L_tmp4 += extract_h(temp1) * extract_l(temp3);
		}while(--nIntraLens!=0);

		temp1 = *p1++;
		L_tmp1 += extract_l(temp1) * extract_l(temp2);
		L_tmp2 += extract_l(temp1) * extract_h(temp2);
		L_tmp1 += extract_h(temp1) * extract_h(temp2);

		temp2 = *p2++;
		L_tmp2 += extract_h(temp1) * extract_l(temp2); 
		L_tmp3 += extract_l(temp1) * extract_l(temp2);
		L_tmp4 += extract_l(temp1) * extract_h(temp2);
		L_tmp3 += extract_h(temp1) * extract_h(temp2);

		temp1 = *p1++;
		L_tmp1 += extract_l(temp1) * extract_l(temp2);
		L_tmp2 += extract_l(temp1) * extract_h(temp2);
		L_tmp1 += extract_h(temp1) * extract_h(temp2);

		L_tmp1 = (L_tmp1<<1) +(Word32)1;
		L_tmp2 = (L_tmp2<<1) +(Word32)1;
		L_tmp3 = (L_tmp3<<1) +(Word32)1;
		L_tmp4 = (L_tmp4<<1) +(Word32)1;

		*y32++ = L_tmp1;
		*y32++ = L_tmp2;
		*y32++ = L_tmp3;
		*y32++ = L_tmp4;

		L_tmp1 = L_abs(L_tmp1);
		L_tmp2 = L_abs(L_tmp2);
		L_tmp3 = L_abs(L_tmp3);
		L_tmp4 = L_abs(L_tmp4);

		vo_max(L_max1,L_tmp1);
		vo_max(L_max2,L_tmp2);
		vo_max(L_max3,L_tmp3);
		vo_max(L_max4,L_tmp4);
	}while(--nOutLens!=0);

	L_tmp1 = L_tmp2 = L_tmp3 = L_tmp4 = 0;							/* 1 -> to avoid null dn[] */
	p1 = (Word32 *)h;
	p2 = (Word32 *)x;

	temp2 = *p2++;
	temp1 = *p1++;
	L_tmp1 += extract_l(temp1) * extract_l(temp2);
	L_tmp2 += extract_l(temp1) * extract_h(temp2);
	L_tmp1 += extract_h(temp1) * extract_h(temp2);

	temp2 = *p2++;
	L_tmp2 += extract_h(temp1) * extract_l(temp2); 
	L_tmp3 += extract_l(temp1) * extract_l(temp2);
	L_tmp4 += extract_l(temp1) * extract_h(temp2);
	L_tmp3 += extract_h(temp1) * extract_h(temp2);

	temp1 = *p1++;
	L_tmp1 += extract_l(temp1) * extract_l(temp2);
	L_tmp2 += extract_l(temp1) * extract_h(temp2);
	L_tmp1 += extract_h(temp1) * extract_h(temp2);

	L_tmp1 = (L_tmp1<<1) +(Word32)1;
	L_tmp2 = (L_tmp2<<1) +(Word32)1;
	L_tmp3 = (L_tmp3<<1) +(Word32)1;
	L_tmp4 = (L_tmp4<<1) +(Word32)1;

	*y32++ = L_tmp1;
	*y32++ = L_tmp2;
	*y32++ = L_tmp3;
	*y32++ = L_tmp4;

	L_tmp1 = L_abs(L_tmp1);
	L_tmp2 = L_abs(L_tmp2);
	L_tmp3 = L_abs(L_tmp3);
	L_tmp4 = L_abs(L_tmp4);

	vo_max(L_max1,L_tmp1);
	vo_max(L_max2,L_tmp2);
	vo_max(L_max3,L_tmp3);
	vo_max(L_max4,L_tmp4);

	/* tot += 3*max / 8 */
	L_max1 >>= 2;
	L_tot += L_max1; 
	L_tot += (L_max1 >> 1);

	L_max2 >>= 2;
	L_tot += L_max2; 
	L_tot += L_max2 >> 1; 

	L_max3 >>= 2;
	L_tot += L_max3; 
	L_tot += L_max3 >> 1; 

	L_max4 >>= 2;
	L_tot += L_max4; 
	L_tot += L_max4 >> 1;  

	/* Find the number of right shifts to do on y32[] so that    */
	/* 6.0 x sumation of max of dn[] in each track not saturate. */
	y32 = y;
	temp1 = norm_l(L_tot) - 4;             /* 4 -> 16 x tot */
	nOutLens = 16;
	do{
		*dn++ = vo_round(vo_L_shl(*y32++, temp1));
		*dn++ = vo_round(vo_L_shl(*y32++, temp1));
		*dn++ = vo_round(vo_L_shl(*y32++, temp1));
		*dn++ = vo_round(vo_L_shl(*y32++, temp1));
	}while(--nOutLens!=0);

	return;
}
#else
void voAMRWBEnccor_h_x(
			 Word16 h[],                           /* (i) Q12 : impulse response of weighted synthesis filter */
			 Word16 x[],                           /* (i) Q0  : target vector                                 */
			 Word16 dn[]                           /* (o) <12bit : correlation between target and h[]         */
)
{
	Word16 i, j, k;
	Word32 L_tmp, y32[L_SUBFR], L_max, L_tot;
	Word16 *p1, *p2;
	Word32 *p3;
	/* first keep the result on 32 bits and find absolute maximum */
	L_tot = 1L;                            
	for (k = 0; k < NB_TRACK; k++)
	{
		L_max = 0;                         
		for (i = k; i < L_SUBFR; i += STEP)
		{
			L_tmp = 1L;                                    /* 1 -> to avoid null dn[] */
		    p1 = &x[i];
		    p2 = &h[0];
			for (j = i; j < L_SUBFR; j++)
				L_tmp += (*p1++ * *p2++)<<1;

			y32[i] = L_tmp;               
			L_tmp = (L_tmp > 0)? L_tmp:-L_tmp;
			if(L_tmp > L_max)
			{
				L_max = L_tmp;             
			}
		}
		/* tot += 3*max / 8 */
		L_max = (L_max >> 2);
		L_tot = vo_L_add(L_tot, L_max);       /* +max/4 */
		L_tot = vo_L_add(L_tot, (L_max >> 1));  /* +max/8 */
	}
	/* Find the number of right shifts to do on y32[] so that    */
	/* 6.0 x sumation of max of dn[] in each track not saturate. */
	j = norm_l(L_tot) - 4;             /* 4 -> 16 x tot */
	p1 = dn;
	p3 = y32;
	for (i = 0; i < L_SUBFR; i+=4)
	{
		*p1++ = vo_round(L_shl(*p3++, j));
        *p1++ = vo_round(L_shl(*p3++, j));
		*p1++ = vo_round(L_shl(*p3++, j));
		*p1++ = vo_round(L_shl(*p3++, j));

		//dn[i] = vo_round(L_shl(y32[i], j));   
	}
	return;
}
#endif
