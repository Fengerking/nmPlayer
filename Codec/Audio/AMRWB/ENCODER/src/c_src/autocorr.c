/*------------------------------------------------------------------------*
*                         AUTOCORR.C                                     *
*------------------------------------------------------------------------*
*   Compute autocorrelations of signal with windowing                    *
*                                                                        *
*------------------------------------------------------------------------*/

#include "typedef.h"
#include "basic_op.h"
#include "oper_32b.h"
#include "acelp.h"
#include "ham_wind.tab"

void Autocorr(
			  Word16 x[],                           /* (i)    : Input signal                      */
			  Word16 m,                             /* (i)    : LPC order                         */
			  Word16 r_h[],                         /* (o) Q15: Autocorrelations  (msb)           */
			  Word16 r_l[],                          /* (o)    : Autocorrelations  (lsb)           */
			  Word32 yy[]
)
{
	Word32 i, norm, shift;
	Word16 *y;
	Word32 L_sum, L_sum1, L_tmp, F_LEN;
    Word16 *p1,*p3;
	const Word16 *p2;
	/* Windowing of signal */
	y = (Word16*)yy;
	p1 = x;
	p2 = voAMRWBEncWindow;
	p3 = y;

#if 0
	L_sum = vo_L_deposit_h(16);  
	for (i = 0; i < L_WINDOW; i+=4)
	{
		*p3 =  ((*p1++ * *p2++) + 0x4000)>>15;
		L_tmp =  (*p3 * *p3)<<1;
		L_tmp =  (L_tmp >> 8);
		L_sum += L_tmp;
        p3++;
		*p3 = ((*p1++ * *p2++) + 0x4000)>>15;
		L_tmp =  (*p3 * *p3)<<1;
		L_tmp =  (L_tmp >> 8);
		L_sum += L_tmp;
        p3++;
		*p3 = ((*p1++ * *p2++) + 0x4000)>>15;
		L_tmp =  (*p3 * *p3)<<1;
		L_tmp =  (L_tmp >> 8);
		L_sum += L_tmp;
        p3++;
		*p3 = ((*p1++ * *p2++) + 0x4000)>>15;
		L_tmp =  (*p3 * *p3)<<1;
		L_tmp =  (L_tmp >> 8);
		L_sum += L_tmp;
        p3++;
	}
#else
	for (i = 0; i < L_WINDOW; i+=4)
	{
		//y[i] = mult_r(x[i], window[i]);
		*p3++=  ((*p1++ * *p2++) + 0x4000)>>15;
		*p3++ = ((*p1++ * *p2++) + 0x4000)>>15;
		*p3++ = ((*p1++ * *p2++) + 0x4000)>>15;
		*p3++ = ((*p1++ * *p2++) + 0x4000)>>15;
	}
	/* calculate energy of signal */
	L_sum = vo_L_deposit_h(16);               /* sqrt(256), avoid overflow after rounding */
	for (i = 0; i < L_WINDOW; i++)
	{
		L_tmp = (y[i] * y[i])<<1;
		L_tmp = (L_tmp >> 8);
		L_sum += L_tmp;
	}
#endif
	/* scale signal to avoid overflow in autocorrelation */

	norm = norm_l(L_sum);
	shift = 4 - (norm >> 1);

	if(shift > 0)
	{
		p1 = y;
		for (i = 0; i < L_WINDOW; i+=4)
		{
			*p1 = vo_shr_r(*p1, shift);
			p1++;
			*p1 = vo_shr_r(*p1, shift); 
			p1++;
			*p1 = vo_shr_r(*p1, shift);
			p1++;
			*p1 = vo_shr_r(*p1, shift); 
			p1++;
		}
	}

	/* Compute and normalize r[0] */

	L_sum = 1; 
	for (i = 0; i < L_WINDOW; i+=4)
	{
		L_sum += (y[i] * y[i])<<1;
		L_sum += (y[i+1] * y[i+1])<<1;
		L_sum += (y[i+2] * y[i+2])<<1;
		L_sum += (y[i+3] * y[i+3])<<1;
	}
		//L_sum = L_mac(L_sum, y[i], y[i]);

	norm = norm_l(L_sum);
	L_sum = (L_sum << norm);

	//L_Extract(L_sum, &r_h[0], &r_l[0]);    /* Put in DPF format (see oper_32b) */
    r_h[0] = L_sum >> 16;
	r_l[0] = (L_sum & 0xffff)>>1;

	/* Compute r[1] to r[m] */
#if 1
    for (i = 1; i <= 8; i++)
    {
        L_sum1 = 0;
		L_sum = 0;
        F_LEN = (Word32)(L_WINDOW - 2*i);
		p1 = y;
		p2 = y + (2*i)-1;
        do{
			L_sum1 += *p1 * *p2++;
			L_sum += *p1++ * *p2;
		}while(--F_LEN!=0);
		L_sum1 += *p1 * *p2++;

        L_sum1 = L_sum1<<norm;
        L_sum = L_sum<<norm;

        r_h[(2*i)-1] = L_sum1 >> 15;
        r_l[(2*i)-1] = L_sum1 & 0x00007fff;
		r_h[(2*i)] = L_sum >> 15;
        r_l[(2*i)] = L_sum & 0x00007fff;
    }
#else
	for (i = 1; i <= m; i++)
	{
		L_sum = 0; 
		for (j = 0; j < L_WINDOW - i; j++)
			L_sum += (y[j] * y[j+i])<<1;
			//L_sum = L_mac(L_sum, y[j], y[j + i]);

		L_sum = L_sum << norm;
		r_h[i] = L_sum >> 16;
		r_l[i] = (L_sum & 0xffff)>>1;
		//L_Extract(L_sum, &r_h[i], &r_l[i]);
	}
#endif
	return;
}
