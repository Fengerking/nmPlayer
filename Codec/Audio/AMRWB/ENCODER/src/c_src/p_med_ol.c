/*------------------------------------------------------------------------*
*                         P_MED_OL.C									  *
*------------------------------------------------------------------------*
* Compute the open loop pitch lag.										  *
*------------------------------------------------------------------------*/
#include "typedef.h"
#include "basic_op.h"
#include "acelp.h"
#include "oper_32b.h"
#include "math_op.h"
#include "p_med_ol.tab"


#define max_eq_index(dist_min,dist,index,i)						\
	if (dist >= dist_min){dist_min = dist; index=i;}

static Word16 max_ol_index_2(Word16 *wsp,
							 Word32 *wsp_a,
							 Word16 *ww,
							 Word16 *we,
							 Word16 L_frame)
{
	Word32 *p1,*p2,temp1,temp2,temp3;
	Word32 L_temp1,L_temp2,L_temp3,L_temp4,i,counter;
	Word32 	max = MIN_32,index=0;
	Word16 hi,lo;

	for(i=0;i<96;i+=4)
	{
		/* Compute the correlation */
		p1 = (Word32 *)wsp;
		p2 = wsp_a;
		wsp_a+=2;
		counter = L_frame>>2;
		temp3 = *p2++;
		temp2 = *p2++;
		L_temp1=L_temp2=L_temp3=L_temp4=0L;
		do{
			temp1 = *p1++;
			L_temp1 += extract_l(temp1) * extract_l(temp3);
			L_temp2 += extract_l(temp1) * extract_h(temp3);
			L_temp1 += extract_h(temp1) * extract_h(temp3);
			L_temp2 += extract_h(temp1) * extract_l(temp2);
			L_temp3 += extract_l(temp1) * extract_l(temp2);
			L_temp4 += extract_l(temp1) * extract_h(temp2);
			L_temp3 += extract_h(temp1) * extract_h(temp2);

			temp3 = *p2++;
			L_temp4 += extract_h(temp1) * extract_l(temp3);
			temp1 = *p1++;

			L_temp1 += extract_l(temp1) * extract_l(temp2);
			L_temp2 += extract_l(temp1) * extract_h(temp2);
			L_temp1 += extract_h(temp1) * extract_h(temp2);
			L_temp2 += extract_h(temp1) * extract_l(temp3);
			L_temp3 += extract_l(temp1) * extract_l(temp3);
			L_temp4 += extract_l(temp1) * extract_h(temp3);
			L_temp3 += extract_h(temp1) * extract_h(temp3);

			temp2 = *p2++;

			L_temp4 += extract_h(temp1) * extract_l(temp2);

		}while(--counter!=0);

		hi = L_temp1>>15;
		lo = L_temp1&0x7fff;
		L_temp1 = *ww * hi;
		L_temp1 += (*ww-- *lo)>>15;

		hi = L_temp2>>15;
		lo = L_temp2&0x7fff;
		L_temp2 = *ww * hi;
		L_temp2 += (*ww-- * lo)>>15;
		hi = L_temp3>>15;
		lo = L_temp3&0x7fff;
		L_temp3 = *ww * hi;
		L_temp3 += (*ww-- *lo)>>15;

		hi = L_temp4>>15;
		lo = L_temp4&0x7fff;
		L_temp4 = *ww * hi;
		L_temp4 += (*ww-- * lo)>>15;

		max_eq_index(max,L_temp1,index,(i));
		max_eq_index(max,L_temp2,index,(i+1));
		max_eq_index(max,L_temp3,index,(i+2));
		max_eq_index(max,L_temp4,index,(i+3));
	}
	p1 = (Word32 *)wsp;
	p2 = wsp_a;
	counter = L_frame>>2;
	temp3 = *p2++;
	temp2 = *p2++;
	L_temp1=L_temp2=L_temp3=L_temp4=0L;
	do{
		temp1 = *p1++;

		L_temp1 += extract_l(temp1) * extract_l(temp3);
		L_temp2 += extract_l(temp1) * extract_h(temp3);
		L_temp1 += extract_h(temp1) * extract_h(temp3);
		temp3 = *p2++;
		L_temp2 += extract_h(temp1) * extract_l(temp2);

		temp1 = *p1++;

		L_temp1 += extract_l(temp1) * extract_l(temp2);
		L_temp2 += extract_l(temp1) * extract_h(temp2);
		L_temp1 += extract_h(temp1) * extract_h(temp2);
		temp2 = *p2++;
		L_temp2 += extract_h(temp1) * extract_l(temp3);

	}while(--counter!=0);

	hi = L_temp1>>15;
	lo = L_temp1&0x7fff;
	L_temp1 = *ww * hi;
	L_temp1 += (*ww-- *lo)>>15;

	hi = L_temp2>>15;
	lo = L_temp2&0x7fff;
	L_temp2 = *ww * hi;
	L_temp2 += (*ww-- * lo)>>15;

	max_eq_index(max,L_temp1,index,(i));
	max_eq_index(max,L_temp2,index,(i+1));

	return (Word16)index;
}

static Word16 max_ol_index_1(Word16 *wsp,
							 Word32 *wsp_a,
							 Word16 *ww,
							 Word16 *we,
							 Word16 L_frame)
{
	Word32 *p1,*p2,temp1,temp2,temp3;
	Word32 L_temp1,L_temp2,L_temp3,L_temp4,i,counter;
	Word32 	max = MIN_32,index=0;
	Word16 hi,lo;

	for(i=0;i<96;i+=4)
	{
		/* Compute the correlation */
		p1 = (Word32 *)wsp;
		p2 = wsp_a;
		wsp_a+=2;
		counter = L_frame>>2;
		temp3 = *p2++;
		temp2 = *p2++;
		L_temp1=L_temp2=L_temp3=L_temp4=0L;
		do{
			temp1 = *p1++;

			L_temp1 += extract_l(temp1) * extract_l(temp3);
			L_temp2 += extract_l(temp1) * extract_h(temp3);
			L_temp1 += extract_h(temp1) * extract_h(temp3);
			L_temp2 += extract_h(temp1) * extract_l(temp2);
			L_temp3 += extract_l(temp1) * extract_l(temp2);
			L_temp4 += extract_l(temp1) * extract_h(temp2);
			L_temp3 += extract_h(temp1) * extract_h(temp2);

			temp3 = *p2++;

			L_temp4 += extract_h(temp1) * extract_l(temp3);

			temp1 = *p1++;

			L_temp1 += extract_l(temp1) * extract_l(temp2);
			L_temp2 += extract_l(temp1) * extract_h(temp2);
			L_temp1 += extract_h(temp1) * extract_h(temp2);
			L_temp2 += extract_h(temp1) * extract_l(temp3);
			L_temp3 += extract_l(temp1) * extract_l(temp3);
			L_temp4 += extract_l(temp1) * extract_h(temp3);
			L_temp3 += extract_h(temp1) * extract_h(temp3);

			temp2 = *p2++;

			L_temp4 += extract_h(temp1) * extract_l(temp2);

		}while(--counter!=0);

		hi = L_temp1>>15;
		lo = L_temp1&0x7fff;
		L_temp1 = *ww * hi;
		L_temp1 += (*ww-- *lo)>>15;

		hi = L_temp2>>15;
		lo = L_temp2&0x7fff;
		L_temp2 = *ww * hi;
		L_temp2 += (*ww-- * lo)>>15;

		hi = L_temp3>>15;
		lo = L_temp3&0x7fff;
		L_temp3 = *ww * hi;
		L_temp3 += (*ww-- *lo)>>15;

		hi = L_temp4>>15;
		lo = L_temp4&0x7fff;
		L_temp4 = *ww * hi;
		L_temp4 += (*ww-- * lo)>>15;


		/* Weight the neighbourhood of the old lag. */
		hi = L_temp1>>15;
		lo = L_temp1&0x7fff;
		L_temp1 = *we * hi;
		L_temp1 += (*we-- *lo)>>15;

		hi = L_temp2>>15;
		lo = L_temp2&0x7fff;
		L_temp2 = *we * hi;
		L_temp2 += (*we-- * lo)>>15;

		hi = L_temp3>>15;
		lo = L_temp3&0x7fff;
		L_temp3 = *we * hi;
		L_temp3 += (*we-- *lo)>>15;

		hi = L_temp4>>15;
		lo = L_temp4&0x7fff;
		L_temp4 = *we * hi;
		L_temp4 += (*we-- * lo)>>15;

		max_eq_index(max,L_temp1,index,(i));
		max_eq_index(max,L_temp2,index,(i+1));
		max_eq_index(max,L_temp3,index,(i+2));
		max_eq_index(max,L_temp4,index,(i+3));
	}

	p1 = (Word32 *)wsp;
	p2 = wsp_a;
	counter = L_frame>>2;
	temp3 = *p2++;
	temp2 = *p2++;
	L_temp1=L_temp2=L_temp3=L_temp4=0L;
	do{
		temp1 = *p1++;

		L_temp1 += extract_l(temp1) * extract_l(temp3);
		L_temp2 += extract_l(temp1) * extract_h(temp3);
		L_temp1 += extract_h(temp1) * extract_h(temp3);
		temp3 = *p2++;
		L_temp2 += extract_h(temp1) * extract_l(temp2);

		temp1 = *p1++;

		L_temp1 += extract_l(temp1) * extract_l(temp2);
		L_temp2 += extract_l(temp1) * extract_h(temp2);
		L_temp1 += extract_h(temp1) * extract_h(temp2);
		temp2 = *p2++;
		L_temp2 += extract_h(temp1) * extract_l(temp3);

	}while(--counter!=0);

	hi = L_temp1>>15;
	lo = L_temp1&0x7fff;
	L_temp1 = *ww * hi;
	L_temp1 += (*ww-- *lo)>>15;

	hi = L_temp2>>15;
	lo = L_temp2&0x7fff;
	L_temp2 = *ww * hi;
	L_temp2 += (*ww-- * lo)>>15;

	hi = L_temp1>>15;
	lo = L_temp1&0x7fff;
	L_temp1 = *we * hi;
	L_temp1 += (*we-- *lo)>>15;

	hi = L_temp2>>15;
	lo = L_temp2&0x7fff;
	L_temp2 = *we * hi;
	L_temp2 += (*we-- * lo)>>15;

	max_eq_index(max,L_temp1,index,(i));
	max_eq_index(max,L_temp2,index,(i+1));

	return(index);
}

Word16 Pitch_med_ol(                       /* output: open loop pitch lag                             */
					Word16 wsp[],                         /* input : signal used to compute the open loop pitch      */
					/*         wsp[-pit_max] to wsp[-1] should be known        */
					Word16 L_min,                         /* input : minimum pitch lag                               */
					Word16 L_max,                         /* input : maximum pitch lag                               */
					Word16 L_frame,                       /* input : length of frame to compute pitch                */
					Word16 L_0,                           /* input : old_ open-loop pitch                            */
					Word16 * gain,                        /* output: normalize correlation of hp_wsp for the Lag     */
					Word16 * hp_wsp_mem,                  /* i:o   : memory of the hypass filter for hp_wsp[] (lg=9) */
					Word16 * old_hp_wsp,                  /* i:o   : hypass wsp[]                                    */
					Word16 wght_flg                       /* input : is weighting function used                      */
					)
{
	Word16 i,Tm;
	Word16 tmp1,tmp2;
	Word16 *ww, *we, *hp_wsp,*ptr1,*ptr2;
	Word16 exp_L_temp1, exp_L_temp2, exp_L_temp3;
	Word32 L_temp1, L_temp2, L_temp3,counter;
	Word32 *wsp_a,wsp_a1[113] = {0};

	wsp_a = wsp_a1;

	ww = &corrweight[198];

	we = &corrweight[98 + L_max - L_0];

	counter = L_max-L_min + L_frame;
	counter = counter>>1;
	ptr1 = wsp-L_max;
	ptr2 = (Word16 *)wsp_a;

	do{
		tmp1 = *ptr1++;
		tmp2 = *ptr1++;
		*ptr2++ = tmp1;
		*ptr2++ = tmp2;
	}while(--counter!=0);

	if((L_0 > 0) && (wght_flg > 0))
		Tm = max_ol_index_1(wsp,wsp_a,ww,we,L_frame);
	else
		Tm = max_ol_index_2(wsp,wsp_a,ww,we,L_frame);

	Tm = L_max-Tm;
	/* Hypass the wsp[] vector */
	hp_wsp = old_hp_wsp + L_max;
	Hp_wsp(wsp, hp_wsp, L_frame, hp_wsp_mem);

	/* Compute normalize correlation at delay Tm */

	L_temp1 = 0L;
	L_temp2 = 0L;
	L_temp3 = 0L;
	ptr1 = hp_wsp;
	ptr2 = hp_wsp-Tm;
	counter= (Word32)L_frame/2;
	do{
		L_temp3 +=  *ptr1 * *ptr1;
		L_temp2 +=  *ptr2 * *ptr2;
		L_temp1 +=  *ptr1++ * *ptr2++;
		L_temp3 +=  *ptr1 * *ptr1;
		L_temp2 +=  *ptr2 * *ptr2;
		L_temp1 +=  *ptr1++ * *ptr2++;
	}while(--counter!=0);

	L_temp1 = (L_temp1<<1);
	L_temp2 = (L_temp2<<1)+1;
	L_temp3 = (L_temp3<<1)+1;

	/* gain = L_temp1/ sqrt(L_temp2*L_temp3) */

	exp_L_temp1 = norm_l(L_temp1);
	L_temp1 = L_temp1 << exp_L_temp1;

	exp_L_temp2 = norm_l(L_temp2);
	L_temp2 = L_temp2 << exp_L_temp2;

	exp_L_temp3 = norm_l(L_temp3);
	L_temp3 = L_temp3 << exp_L_temp3;


	L_temp2 = vo_L_mult(vo_round(L_temp2), vo_round(L_temp3));

	i = norm_l(L_temp2);
	L_temp2 = L_temp2 << i;

	exp_L_temp2 = add1(exp_L_temp2, exp_L_temp3);
	exp_L_temp2 = add1(exp_L_temp2, i);
	exp_L_temp2 = 62 - exp_L_temp2;

	Isqrt_n(&L_temp2, &exp_L_temp2);

	L_temp1 = vo_L_mult(vo_round(L_temp1), vo_round(L_temp2));
	exp_L_temp1 = 31 - exp_L_temp1;
	exp_L_temp1 = add1(exp_L_temp1, exp_L_temp2);

	*gain = vo_round(vo_L_shl(L_temp1, exp_L_temp1));

	/* Shitf hp_wsp[] for next frame */
	for (i = 0; i < L_max; i++)
	{
		old_hp_wsp[i] = old_hp_wsp[i + L_frame];

	}
	return (Tm);
}

/*____________________________________________________________________
|
|
|  FUNCTION NAME median5
|
|      Returns the median of the set {X[-2], X[-1],..., X[2]},
|      whose elements are 16-bit integers.
|
|  INPUT
|      X[-2:2]   16-bit integers.
|
|  RETURN VALUE
|      The median of {X[-2], X[-1],..., X[2]}.
|_____________________________________________________________________
*/

Word16 median5(Word16 x[])
{
	Word16 x1, x2, x3, x4, x5;
	Word16 tmp;

	x1 = x[-2];                            
	x2 = x[-1];                            
	x3 = x[0];                             
	x4 = x[1];                             
	x5 = x[2];                             

	if (x2 < x1)
	{
		tmp = x1;
		x1 = x2;
		x2 = tmp;                          
	}
	if (x3 < x1)
	{
		tmp = x1;
		x1 = x3;
		x3 = tmp;                          
	}
	if (x4 < x1)
	{
		tmp = x1;
		x1 = x4;
		x4 = tmp;                          
	}
	if (x5 < x1)
	{
		x5 = x1;                           
	}
	if (x3 < x2)
	{
		tmp = x2;
		x2 = x3;
		x3 = tmp;                          
	}
	if (x4 < x2)
	{
		tmp = x2;
		x2 = x4;
		x4 = tmp;                          
	}
	if (x5 < x2)
	{
		x5 = x2;                           
	}
	if (x4 < x3)
	{
		x3 = x4;                           
	}
	if (x5 < x3)
	{
		x3 = x5;                           
	}
	return (x3);
}

/*____________________________________________________________________
|
|
|  FUNCTION NAME med_olag
|
|
|_____________________________________________________________________
*/


Word16 Med_olag(                           /* output : median of  5 previous open-loop lags       */
				Word16 prev_ol_lag,                   /* input  : previous open-loop lag                     */
				Word16 old_ol_lag[5]
)
{
	Word32 i;

	/* Use median of 5 previous open-loop lags as old lag */

	for (i = 4; i > 0; i--)
	{
		old_ol_lag[i] = old_ol_lag[i - 1]; 
	}

	old_ol_lag[0] = prev_ol_lag;           

	i = median5(&old_ol_lag[2]);

	return i;

}
