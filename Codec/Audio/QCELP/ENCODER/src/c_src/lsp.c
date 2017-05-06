/**********************************************************************
Each of the companies; Qualcomm, and Lucent (hereinafter 
referred to individually as "Source" or collectively as "Sources") do 
hereby state:

To the extent to which the Source(s) may legally and freely do so, the 
Source(s), upon submission of a Contribution, grant(s) a free, 
irrevocable, non-exclusive, license to the Third Generation Partnership 
Project 2 (3GPP2) and its Organizational Partners: ARIB, CCSA, TIA, TTA, 
and TTC, under the Source's copyright or copyright license rights in the 
Contribution, to, in whole or in part, copy, make derivative works, 
perform, display and distribute the Contribution and derivative works 
thereof consistent with 3GPP2's and each Organizational Partner's 
policies and procedures, with the right to (i) sublicense the foregoing 
rights consistent with 3GPP2's and each Organizational Partner's  policies 
and procedures and (ii) copyright and sell, if applicable) in 3GPP2's name 
or each Organizational Partner's name any 3GPP2 or transposed Publication 
even though this Publication may contain the Contribution or a derivative 
work thereof.  The Contribution shall disclose any known limitations on 
the Source's rights to license as herein provided.

When a Contribution is submitted by the Source(s) to assist the 
formulating groups of 3GPP2 or any of its Organizational Partners, it 
is proposed to the Committee as a basis for discussion and is not to 
be construed as a binding proposal on the Source(s).  The Source(s) 
specifically reserve(s) the right to amend or modify the material 
contained in the Contribution. Nothing contained in the Contribution 
shall, except as herein expressly provided, be construed as conferring 
by implication, estoppel or otherwise, any license or right under (i) 
any existing or later issuing patent, whether or not the use of 
information in the document necessarily employs an invention of any 
existing or later issued patent, (ii) any copyright, (iii) any 
trademark, or (iv) any other intellectual property right.

With respect to the Software necessary for the practice of any or 
all Normative portions of the QCELP-13 Variable Rate Speech Codec as 
it exists on the date of submittal of this form, should the QCELP-13 be 
approved as a Specification or Report by 3GPP2, or as a transposed 
Standard by any of the 3GPP2's Organizational Partners, the Source(s) 
state(s) that a worldwide license to reproduce, use and distribute the 
Software, the license rights to which are held by the Source(s), will 
be made available to applicants under terms and conditions that are 
reasonable and non-discriminatory, which may include monetary compensation, 
and only to the extent necessary for the practice of any or all of the 
Normative portions of the QCELP-13 or the field of use of practice of the 
QCELP-13 Specification, Report, or Standard.  The statement contained above 
is irrevocable and shall be binding upon the Source(s).  In the event 
the rights of the Source(s) in and to copyright or copyright license 
rights subject to such commitment are assigned or transferred, the 
Source(s) shall notify the assignee or transferee of the existence of 
such commitments.
*******************************************************************/
/* lsp.c - lsp to lpc and vice versa routines                         */

#include "basic_op.h"
#include "celp.h"
#include "lsp.h"
#include "coderate.h"

/*
********************************************************************************
*                         LOCAL PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Chebps
*  Purpose     : Evaluates the Chebyshev polynomial series
*  Description : - The polynomial order is   n = m/2 = 5
*                - The polynomial F(z) (F1(z) or F2(z)) is given by
*                   F(w) = 2 exp(-j5w) C(x)
*                  where
*                   C(x) = T_n(x) + f(1)T_n-1(x) + ... +f(n-1)T_1(x) + f(n)/2
*                  and T_m(x) = cos(mw) is the mth order Chebyshev
*                  polynomial ( x=cos(w) )
*  Returns     : C(x) for the input x.
*
**************************************************************************
*/
static __inline int Chebps(int x,
						   int f[], /* (n) */
						   int n)
{
	int cheb;
	int b0_h, b0_l, b1_h, b1_l, b2_h, b2_l;
	int t0;
	b2_h = 256;                   
	b2_l = 0;                      

	t0 = x<<10;//L_mult (x, 512);          /* 2*x                 */
	t0 += (f[1]<<14);//L_mac (t0, f[1], 8192);   /* + f[1]              */

	b1_h = t0>>16;
	b1_l  = (t0 - (b1_h<<16))>>1;

	t0 = (b1_h*x<<1) + ((b1_l*x>>15)<<1) ; //t0 = Mpy_32_16 (b1_h, b1_l, x);         /* t0 = 2.0*x*b1        */
	t0 = (t0<< 1);
	t0 += (b2_h*((short) 0x8000)<<1);//L_mac (t0, b2_h, (short) 0x8000); /* t0 = 2.0*x*b1 - b2  

	t0 -= (b2_l<<1);//L_msu (t0, b2_l, 1);//t0 = (t0 - (b2_l<<2));
	t0 += (f[2]<<14);//t0 = L_mac (t0, f[i], 8192); //t0 =  (t0 +  (f[i]<<14));            /* t0 = 2.0*x*b1 - b2 + f[i] */

	b0_h = t0>>16;
	b0_l  = (t0 - (b0_h<<16))>>1;

	t0 = (b0_h*x<<1) + ((b0_l*x>>15)<<1) ; //t0 = Mpy_32_16 (b1_h, b1_l, x);         /* t0 = 2.0*x*b1        */
	t0 = (t0<< 1);
	t0 += (b1_h*((short) 0x8000)<<1);//L_mac (t0, b2_h, (short) 0x8000); /* t0 = 2.0*x*b1 - b2   
	t0 -= (b1_l<<1);//L_msu (t0, b2_l, 1);//t0 = (t0 - (b2_l<<2));
	t0 += (f[3]<<14);//t0 = L_mac (t0, f[i], 8192); //t0 =  (t0 +  (f[i]<<14));            /* t0 = 2.0*x*b1 - b2 + f[i] */

	b1_h = t0>>16;
	b1_l  = (t0 - (b1_h<<16))>>1;
	// L_Extract (t0, &b0_h, &b0_l);           /* b0 = 2.0*x*b1 - b2 + f[i]*/

	t0 = (b1_h*x<<1) + ((b1_l*x>>15)<<1) ; //t0 = Mpy_32_16 (b1_h, b1_l, x);         /* t0 = 2.0*x*b1        */
	t0 = (t0<< 1);
	t0 += (b0_h*((short) 0x8000)<<1);//L_mac (t0, b2_h, (short) 0x8000); /* t0 = 2.0*x*b1 - b2  

	t0 -= (b0_l<<1);//L_msu (t0, b2_l, 1);//t0 = (t0 - (b2_l<<2));
	t0 += (f[4]<<14);//t0 = L_mac (t0, f[i], 8192); //t0 =  (t0 +  (f[i]<<14));            /* t0 = 2.0*x*b1 - b2 + f[i] */

	b0_h = t0>>16;
	b0_l  = (t0 - (b0_h<<16))>>1;
	// L_Extract (t0, &b0_h, &b0_l);           /* b0 = 2.0*x*b1 - b2 + f[i]*/

	t0 = (b0_h*x<<1) + ((b0_l*x>>15)<<1) ;	 // t0 = Mpy_32_16 (b1_h, b1_l, x);             /* t0 = x*b1; */

	t0 += (b1_h*((short) 0x8000)<<1);//L_mac (t0, b2_h, (short) 0x8000);//(t0 + (b2_h*((short) 0x8000)<<1))
	t0 -= (b1_l<<1);//L_msu (t0, b2_l, 1);//t0 = (t0 - (b2_l<<2));
	t0 += (f[5]<<13);//L_mac (t0, f[i], 4096);                /* t0 = x*b1 - b2 + f[i]/2 */

	t0 = L_shl2(t0, 6);

	cheb = extract_h (t0);

	return (cheb);
}

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*
**************************************************************************
*
*  Function    : Az_lsp
*  Purpose     : Compute the LSPs from  the LP coefficients
*
**************************************************************************
*/
void lpc2lsp(
			short* a,         /* (i)  : predictor coefficients (MP1)               */
			short* lsp,       /* (o)  : line spectral pairs (M)                    */
			short* old_lsp,    /* (i)  : old lsp[] (in case not found 10 roots) (M) */
			short  order
			)
{
    int  i, j, nf, ip;
    int  xlow, ylow, xhigh, yhigh, xmid, ymid, xint;
    int x, y, sign, exp, order2;
    int *coef;
    int f1[6], f2[6];
    int t0,t1,t2;

	order2 = order >> 1;
 	
    f1[0] = 1024;                  
    f2[0] = 1024;    
	
	for (i=1; i<=order2; i++) {
		t1 = -a[i - 1]<<14;
        t2 = a[LPCORDER - i]<<14;
        t0 = t1 - t2;
        x = extract_h (t0);
		f1[i] = x - f1[i-1];

		t0 = t1 + t2;
		x = extract_h (t0);
		f2[i] = x + f2[i-1];		
    }
    /*-------------------------------------------------------------*
     * find the LSPs using the Chebychev pol. evaluation           *
     *-------------------------------------------------------------*/
    nf = 0;                        
    ip = 0;                        
    coef = f1;                    
    xlow = grid[0];               
    ylow = Chebps(xlow, coef, order2);
    j = 0;
    /* while ( (nf < M) && (j < grid_points) ) */
    while (((nf - order) < 0) && ((j - GRID_POINT) < 0))
    {
        j++;
        xhigh = xlow;              
        yhigh = ylow;             
        xlow = grid[j];            
        ylow = Chebps(xlow, coef, order2);	
        if ((ylow * yhigh) <= 0)
        {
            /* divide 4 times the interval */
            for (i = 4; i ; i--)
            {
                /* xmid = (xlow + xhigh)/2 */
                xmid = (xlow +  xhigh) >> 1;
                ymid = Chebps(xmid, coef, order2);                                  
                if ((ylow* ymid) <= 0)
                {
                    yhigh = ymid;  
                    xhigh = xmid;  
                }
                else
                {
                    ylow = ymid;   
                    xlow = xmid;   
                }
            }
            /*-------------------------------------------------------------*
             * Linear interpolation                                        *
             *    xint = xlow - ylow*(xhigh-xlow)/(yhigh-ylow);            *
             *-------------------------------------------------------------*/
            x = (xhigh -xlow);
            y = (yhigh- ylow);
            if (y == 0)
            {
                xint = xlow;       
            }
            else
            {
                sign = y;  
                if(y<0)
                   y = -(y);
                exp = norm_s (y);
                y = (y << exp);
                y = divide_s((short) 16383, y);
                t0 = (x*y) << 1;
                t0 = (t0 >> (20 - exp));
                y = extract_l (t0);     /* y= (xhigh-xlow)/(yhigh-ylow) */
                if (sign < 0)
                    y = - (y);
				t0 = L_mult(ylow, y);
                //t0 = (ylow * y) << 1;
                t0 = (t0 >> 11);
                xint =  (xlow - extract_l (t0)); /* xint = xlow - ylow*y */
            }
            lsp[nf] = xint;      
            xlow = xint;           
            nf++;
            if (ip == 0)
            {
                ip = 1;           
                coef = f2;         
            }
            else
            {
                ip = 0;          
                coef = f1;        
            }
            ylow = Chebps(xlow, coef, order2);                                 
        }       
    }
    /* Check if order roots found */
    if ((nf - order) < 0)
    {
        for (i = 0; i < order; i++)
        {
            lsp[i] = old_lsp[i];   
        }
    }
	else
	{
		for (i = 0; i < order; i++)
        {
            lsp[i] = anticosvalue(lsp[i], 15);   
        }			
	}

    return;
}

void lsp2lpc(
			 short *a,         /* (i)  : line spectral frequencies            */
			 short *Lpc,         /* (o)  : predictor coefficients (order = 10)  */
			 short order
			 )
{
    int i;
    int j;
    int unsgn_k;
    int temp1,temp2;
    int Fac[(LPCORDER>>1)-2] = {4096, 2048, 1024};
    int P[(LPCORDER>>1) +1];
    int Q[(LPCORDER>>1) +1];
	int	Lsp[LPCORDER];
    int arA,arB;
    int Lpc_Overflow = 0;
    int raw_factor, factor;

    for (i=0; i < order; i++)
    {
        arA = (int)a[i];
        j = a[i] >> (15 - COSTABLE_SCALE);
        temp1 = (arA & 0x3f) << 9;
		temp2 = CosineTable_16[j+1] - CosineTable_16[j];

        //can't modify following 4
        arA = -((int)CosineTable_16[j] << 16);
        arA = L_sub(arA, (int)temp2 * (int)temp1 << 1);
        arA = arA + 0x00008000; // rounding
        Lsp[i] = (arA >> 16);
    }

/* Init P and Q, All entries in Q13 format */
    P[0] = 134217728;                // 0.5 Q28
    arA = Lsp[0] << 13;
    arA += Lsp[2] << 13;
    P[1] = arA;

    arA = Lsp[0] * Lsp[2];
    arA >>= 1;
    arA += 4096 << 16;
    P[2] = arA;

    Q[0] = 134217728;
    arA = Lsp[1] << 13;
    arA += Lsp[3] << 13;
    Q[1] = arA;

    arA = Lsp[1] * Lsp[3];
    arA >>= 1;
    arA += 4096 << 16; /* 1.0 in Q28 format */
    Q[2] = arA;

//    for (i=2; i < order/2; i++)
    for (i=2; i < (order>>1); i++)
    {
       arA = P[i-1];
       temp1 = (P[i] >> 16);  // TCMC_PPA 14.07.97
       arA += (int)temp1 * (int)Lsp[2*i+0] << 1; /* Q12 * Q15 = Q28 (with left shift) */
       unsgn_k = (int)(P[i] & (0x0000ffffl));	// TCMC_PPA 14.07.97
	   arB = (int)(unsgn_k * Lsp[2*i+0] + 0x8000);
       arB >>= 15;
       arA += arB;
       P[i+1] = arA;
       arA = Q[i-1];
       temp1 = (Q[i] >> 16);  // TCMC_PPA 14.07.97
       arA += (int)temp1 * (int)Lsp[2*i+1] << 1; /* Q12 * Q15 = Q28 (with left shift) */
       unsgn_k = (Q[i] & (0x0000ffffl)); // TCMC_PPA 14.07.97
	   arB = (int)(unsgn_k * Lsp[2*i+1] + 0x8000); 
	   arB >>= 15;
	   arA += arB;
       Q[i+1] = arA;

/* Compute coefficients i, i-1, ..., 2 */
       for (j=i; j >= 2; j--)
       {
           arA = P[j];
           arA += P[j-2];
           arA >>= 1; /* 0.5 * (P[j] + P[j-2]) */
    	   temp1 = (P[j-1] >> 16);    // TCMC_PPA 14.07.97
           arA += (int)temp1 * (int)Lsp[2*i+0] << 1; /* Q12 * Q15 = Q28 (with left shift) */
    	   unsgn_k = (int)(P[j-1] & (0x0000ffffl)); // TCMC_PPA 14.07.97
    	   arB = (int)(unsgn_k * Lsp[2*i+0] + 0x8000);  
		   arB >>= (short)15;
		   arA += arB;
           P[j] = arA;

           arA = Q[j];
           arA += Q[j-2];
           arA >>= 1; /* 0.5 * (Q[j] + Q[j-2]) */
    	   temp1 = (Q[j-1] >> 16);  // TCMC_PPA 14.07.97
           arA += (int)temp1 * (int)Lsp[2*i+1] << 1; /* Q12 * Q15 = Q28 (with left shift) */
    	   unsgn_k = (int)(Q[j-1] & (0x0000ffffl)); // TCMC_PPA 14.07.97
    	   arB = (int)(unsgn_k * Lsp[2*i+1] + 0x8000);
           arB >>= 15;
           arA += arB;
           Q[j] = arA;
       }

       P[0] = P[0] >> 1;
       Q[0] = Q[0] >> 1;
       arA = P[1];
       arA += (int)Lsp[2*i+0] * (int)Fac[i-2] << 1; /* Q15 * Q12 = Q28 (with left shift) */
       P[1] = arA >> 1;
       arA = Q[1];
       arA += (int)Lsp[2*i+1] * (int)Fac[i-2] << 1; /* Q15 * Q12 = Q28 (with left shift) */
       Q[1] = arA >> 1;
    }

//    for (i=0; i < order/2; i++)
    for (i=0; i < (order>>1); i++)
    {
      arA = Q[i] - Q[i+1] - P[i] - P[i+1];
      arB = arA;
      arA += 0x00001000l;
      Lpc[i] = (short)(arA >> 13);
      if (arB < 0) arB = -arB;
      if (arB > 0xfffffffl)
          Lpc_Overflow = 1;

      arA = Q[i + 1] - Q[i] - P[i] - P[i+1];
      arB = arA;
      arA += 0x00001000l;
      Lpc[order-1-i] = (short)(arA >> 13);
      if (arB < 0) arB = -arB;
      if (arB > 0xfffffffl)
          Lpc_Overflow = 1;
    }

    if (Lpc_Overflow == 1)
    {
        arB = 0x10000000l;
        for (i=0; i < (order>>1); i++)
        {
            arA = Q[i] - Q[i+1] - P[i] - P[i+1];
            Lpc[i] = (short)(arA >> 16);
            if (arA < 0) arA = -arA;
            if (arA > arB)
                arB = arA;

            arA = Q[i + 1] - Q[i] - P[i] - P[i+1];
            Lpc[order-1-i] = (short)(arA >> 16);
            if (arA < 0) arA = -arA;
            if (arA > arB)
                arB = arA;
        }

        arB -= 0x10000000l;
        arB = -arB;
        arB <<= 3;
        arB += 0x7fffffffl;
        raw_factor = (arB > 16);

        factor = raw_factor;

        for (i=0; i < LPCORDER; i++)
        {
            arA = (int)Lpc[i] * (int)factor;
            Lpc[i] = (short)(arA >> 12);
            arA = (int)raw_factor * (int)factor;
            factor = (arA >> 15);
        }
    }
}


