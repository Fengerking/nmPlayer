/***********************************************************************
*                                                                      *
*        VisualOn, Inc. Confidential and Proprietary, 2003-2010        *
*                                                                      *
************************************************************************/
/***********************************************************************
*       File: syn_filt.c                                               *
*                                                                      *
*       Description: Do the synthesis filtering 1/A(z)                 *
*                                                                      *
************************************************************************/

#include "typedef.h"
#include "basic_op.h"
#include "math_op.h"
#include "cnst.h"

void Syn_filt(
			  Word16 a[],                           /* (i) Q12 : a[m+1] prediction coefficients           */
			  Word16 x[],                           /* (i)     : input signal                             */
			  Word16 y[],                           /* (o)     : output signal                            */
			  Word16 lg,                            /* (i)     : size of filtering                        */
			  Word16 mem[],                         /* (i/o)   : memory associated with this filtering.   */
			  Word16 y_buf[],
			  Word16 update                         /* (i)     : 0=no update, 1=update of memory.         */
			  )
{
	Word32 i, a0;
	//Word16 y_buf[L_SUBFR16k + M16k];
	Word32 L_tmp;
	Word16 *yy, *p1, *p2;
	yy = (Word16*)y_buf;                        
	/* copy initial filter states into synthesis buffer */
	for (i = 0; i < 16; i++)
	{
		*yy++ = mem[i];                    
	}
	//s = norm_s(a[0]) - 2;
	a0 = (a[0] >> 1);                     /* input / 2 */
	/* Do the filtering. */
	for (i = 0; i < lg; i++)
	{
		p1 = &a[1];
		p2 = &yy[i-1];
        L_tmp  = a0 * x[i];
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1++ * *p2--;
		L_tmp -= *p1 * *p2;

		L_tmp = L_shl2(L_tmp, 4);
		y[i] = yy[i] = extract_h(L_add(L_tmp, 0x8000));  
	}

	/* Update memory if required */
	if (update)
		for (i = 0; i < 16; i++)
		{
			mem[i] = yy[lg - 16 + i];
		}
		return;
}


void Syn_filt_32(
				 Word16 a[],                           /* (i) Q12 : a[m+1] prediction coefficients */
				 Word16 m,                             /* (i)     : order of LP filter             */
				 Word16 exc[],                         /* (i) Qnew: excitation (exc[i] >> Qnew)    */
				 Word16 Qnew,                          /* (i)     : exc scaling = 0(min) to 8(max) */
				 Word16 sig_hi[],                      /* (o) /16 : synthesis high                 */
				 Word16 sig_lo[],                      /* (o) /16 : synthesis low                  */
				 Word16 lg                             /* (i)     : size of filtering              */
				 )
{
	Word32 i,a0;
	Word32 L_tmp, L_tmp1;
	Word16 *p1, *p2, *p3;
	a0 = a[0] >> (4 + Qnew);          /* input / 16 and >>Qnew */
	/* Do the filtering. */
	for (i = 0; i < lg; i++)
	{
		L_tmp  = 0; 
		L_tmp1 = 0;
        p1 = a;
		p2 = &sig_lo[i - 1];
        p3 = &sig_hi[i - 1];

		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
        L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;
		L_tmp  -= *p2-- * *p1;
		L_tmp1 -= *p3-- * *p1++;

		//for (j = 1; j <= m; j++)
		//{
		//	L_tmp  -= (sig_lo[i - j] * a[j]);
		//	L_tmp1 += (sig_hi[i - j] * a[j]);
		//}

		L_tmp = L_tmp >> 11;      /* -4 : sig_lo[i] << 4 */
		L_tmp += (exc[i] * a0)<<1;

		/* sig_hi = bit16 to bit31 of synthesis */
		L_tmp = L_tmp - (L_tmp1<<1);

		L_tmp = L_tmp >> 3;           /* ai in Q12 */
		sig_hi[i] = extract_h(L_tmp);      

		/* sig_lo = bit4 to bit15 of synthesis */
		L_tmp >>= 4;           /* 4 : sig_lo[i] >> 4 */
		sig_lo[i] = (Word16)((L_tmp - (sig_hi[i] << 13)));  
	}

	return;
}
