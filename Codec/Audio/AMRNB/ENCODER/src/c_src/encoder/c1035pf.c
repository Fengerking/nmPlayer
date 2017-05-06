/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : c1035pf.c
*      Purpose          : Searches a 35 bit algebraic codebook containing 
*                       : 10 pulses in a frame of 40 samples.
*
********************************************************************************
*/
/*
********************************************************************************
*                         MODULE INCLUDE FILE AND VERSION ID
********************************************************************************
*/
#include "c1035pf.h"
const char c1035pf_id[] = "@(#)$Id $" c1035pf_h;
/*
********************************************************************************
*                         INCLUDE FILES
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"
#include "inv_sqrt.h"
#include "set_sign.h"
#include "cor_h.h"
#include "s10_8pf.h"
/*
********************************************************************************
*                         LOCAL VARIABLES AND TABLES
********************************************************************************
*/
#define NB_PULSE  10
#include "gray.tab"
/*
********************************************************************************
*                         LOCAL PROGRAM CODE
********************************************************************************
*/
#if 0
static void q_p (
		  Word16 *ind,        /* Pulse position */
		  Word16 n            /* Pulse number   */
		  )
{
	int  tmp = *ind;                            
	if(n<5)
	{
		*ind = (tmp & 0x8) | gray[tmp & 0x7];    
	}
	else
	{
		*ind = gray[tmp & 0x7];  
	}
}
#endif

/*************************************************************************
*
*  FUNCTION:  build_code()
*
*  PURPOSE: Builds the codeword, the filtered codeword and index of the
*           codevector, based on the signs and positions of 10 pulses.
*
*************************************************************************/
static void build_code (
						Word16 codvec[], /* (i)  : position of pulses                           */
						Word16 sign[],   /* (i)  : sign of d[n]                                 */
						Word16 cod[],    /* (o)  : innovative code vector                       */
						Word16 h[],      /* (i)  : impulse response of weighted synthesis filter*/
						Word16 y[],      /* (o)  : filtered innovative code                     */
						Word16 indx[]    /* (o)  : index of 10 pulses (sign+position)           */
)
{
	Word16  _sign[NB_PULSE];
	Word16  *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7, *p8, *p9;
	Word32  s;
	Word32  k,i, j, track, index;
	p0 = cod;
	for (i = L_CODE; i != 0; i -=8)
	{
		*p0++ =0;  *p0++ =0;
		*p0++ =0;  *p0++ =0;
		*p0++ =0;  *p0++ =0;
		*p0++ =0;  *p0++ =0;
	}
	indx[0] = -1;
	indx[1] = -1;
	indx[2] = -1;
	indx[3] = -1;
	indx[4] = -1;

	for (k = 0; k < NB_PULSE; k++)
	{
		/* read pulse position */            
		i = codvec[k];                          
		/* read sign           */        
		j = sign[i];                             
		index = i* 6554>>15;//mult (i, 6554);                  /* index = pos/5       */
		/* track = pos%5 */
		track = i - extract_l (index*5);
		if (j > 0)
		{
			cod[i] = (cod[i] + 4096);         
			_sign[k] = 8192;                     
		}
		else
		{
			cod[i] =  (cod[i] - 4096);         
			_sign[k] = -8192;                    
			index = (index+ 8);
		}
		if (indx[track] < 0)
		{
			indx[track] = index;                 
		}
		else
		{
			if (((index ^ indx[track]) & 8) == 0)
			{
				/* sign of 1st pulse == sign of 2nd pulse */
				if(indx[track] <= index)
				{
					indx[track + 5] = index;     
				}
				else
				{
					indx[track + 5] = indx[track];
					indx[track] = index;         
				}
			}
			else
			{
				/* sign of 1st pulse != sign of 2nd pulse */
				if((indx[track] & 7) <= (index & 7))
				{
					indx[track + 5] = indx[track];
					indx[track] = index;         
				}
				else
				{
					indx[track + 5] = index;     
				}
			}
		}
	}
	p0 = h - codvec[0];                          
	p1 = h - codvec[1];                          
	p2 = h - codvec[2];                          
	p3 = h - codvec[3];                          
	p4 = h - codvec[4];                          
	p5 = h - codvec[5];                         
	p6 = h - codvec[6];                          
	p7 = h - codvec[7];                         
	p8 = h - codvec[8];                         
	p9 = h - codvec[9];    

#ifdef C_OPT  //be bitmatch
	for (i = 0; i < L_CODE; i++)
	{
		s =  (*p0++ * _sign[0]);
		s += (*p1++ * _sign[1]);
		s += (*p2++ * _sign[2]);
		s += (*p3++ * _sign[3]);
		s += (*p4++ * _sign[4]);
		s += (*p5++ * _sign[5]);
		s += (*p6++ * _sign[6]);
		s += (*p7++ * _sign[7]);
		s += (*p8++ * _sign[8]);
		s += (*p9++ * _sign[9]);
		y[i] = (s + 0x4000) >>15; 
	}   
#else
	for (i = 0; i < L_CODE; i++)
	{
		s = 0;  
		s = L_mac (s, *p0++, _sign[0]);
		s = L_mac (s, *p1++, _sign[1]);
		s = L_mac (s, *p2++, _sign[2]);
		s = L_mac (s, *p3++, _sign[3]);
		s = L_mac (s, *p4++, _sign[4]);
		s = L_mac (s, *p5++, _sign[5]);
		s = L_mac (s, *p6++, _sign[6]);
		s = L_mac (s, *p7++, _sign[7]);
		s = L_mac (s, *p8++, _sign[8]);
		s = L_mac (s, *p9++, _sign[9]);
		y[i] = (s + 0x8000) >>16; 
	}
#endif
}

/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
*
*  FUNCTION:  code_10i40_35bits()
*
*  PURPOSE:  Searches a 35 bit algebraic codebook containing 10 pulses
*            in a frame of 40 samples.
*
*  DESCRIPTION:
*    The code contains 10 nonzero pulses: i0...i9.
*    All pulses can have two possible amplitudes: +1 or -1.
*    The 40 positions in a subframe are divided into 5 tracks of
*    interleaved positions. Each track contains two pulses.
*    The pulses can have the following possible positions:
*
*       i0, i5 :  0, 5, 10, 15, 20, 25, 30, 35.
*       i1, i6 :  1, 6, 11, 16, 21, 26, 31, 36.
*       i2, i7 :  2, 7, 12, 17, 22, 27, 32, 37.
*       i3, i8 :  3, 8, 13, 18, 23, 28, 33, 38.
*       i4, i9 :  4, 9, 14, 19, 24, 29, 34, 39.
*
*    Each pair of pulses require 1 bit for their signs and 6 bits for their
*    positions (3 bits + 3 bits). This results in a 35 bit codebook.
*    The function determines the optimal pulse signs and positions, builds
*    the codevector, and computes the filtered codevector.
*
*************************************************************************/

void voAMRNBEnc_code_10i40_35bits (
						Word16 x[],   /* (i)   : target vector                                 */
						Word16 cn[],  /* (i)   : residual after long term prediction           */
						Word16 h[],   /* (i)   : impulse response of weighted synthesis filter h[-L_subfr..-1] must be set to zero*/
						Word16 cod[], /* (o)   : algebraic (fixed) codebook excitation         */
						Word16 y[],   /* (o)   : filtered fixed codebook excitation            */
						Word16 indx[] /* (o)   : index of 10 pulses (sign + position)          */
)
{
    Word16 rr[L_CODE][L_CODE];
	Word32 tmp;
	Word16 ipos[NB_PULSE];
	Word16 pos_max[NB_TRACK];
	Word16 codvec[NB_PULSE];
	Word16 dn[L_CODE];
	Word16 sign[L_CODE];

#ifdef ARMv6_OPT  //have error
        voAMRNBEnccor_h_x2_asm(h, x, dn, 2, NB_TRACK, STEP);
#else
	voAMRNBEnccor_h_x (h, x, dn, 2);
#endif

#if 1
	set_sign12k2 (dn, cn, sign, pos_max, ipos);
#else
	set_sign12k2 (dn, cn, sign, pos_max, NB_TRACK, ipos, STEP);
#endif 

#ifdef ARMv6_OPT   //have some error close the optimzation branch
	cor_h_asm(h, sign, rr);
#else
	cor_h (h, sign, rr);
#endif

	voAMRNBEnc_search_10and8i40 (NB_PULSE, STEP, NB_TRACK, dn, rr, ipos, pos_max, codvec);   
	build_code(codvec, sign, cod, h, y, indx);	

	tmp = indx[0] ; indx[0]= (tmp & 0x8) | gray[tmp & 0x7]; 
	tmp = indx[1] ; indx[1]= (tmp & 0x8) | gray[tmp & 0x7]; 
	tmp = indx[2] ; indx[2]= (tmp & 0x8) | gray[tmp & 0x7]; 
	tmp = indx[3] ; indx[3]= (tmp & 0x8) | gray[tmp & 0x7]; 
	tmp = indx[4] ; indx[4]= (tmp & 0x8) | gray[tmp & 0x7]; 

	tmp = indx[5] ; indx[5]= gray[tmp & 0x7]; 
	tmp = indx[6] ; indx[6]= gray[tmp & 0x7]; 
	tmp = indx[7] ; indx[7]= gray[tmp & 0x7]; 
	tmp = indx[8] ; indx[8]= gray[tmp & 0x7]; 
	tmp = indx[9] ; indx[9]= gray[tmp & 0x7]; 
	/*
	for (i = 0; i < 10; i++)
	{
	q_p (&indx[i], i);
	}*/
	return;
}

