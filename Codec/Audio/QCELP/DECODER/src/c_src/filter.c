/**********************************************************************/
/* QCELP Variable Rate Speech Codec - Simulation of TIA IS96-A, service */
/*     option one for TIA IS95, North American Wideband CDMA Digital  */
/*     Cellular Telephony.                                            */
/*                                                                    */
/* (C) Copyright 1993, QUALCOMM Incorporated                          */
/* QUALCOMM Incorporated                                              */
/* 10555 Sorrento Valley Road                                         */
/* San Diego, CA 92121                                                */
/*                                                                    */
/* Note:  Reproduction and use of this software for the design and    */
/*     development of North American Wideband CDMA Digital            */
/*     Cellular Telephony Standards is authorized by                  */
/*     QUALCOMM Incorporated.  QUALCOMM Incorporated does not         */
/*     authorize the use of this software for any other purpose.      */
/*                                                                    */
/*     The availability of this software does not provide any license */
/*     by implication, estoppel, or otherwise under any patent rights */
/*     of QUALCOMM Incorporated or others covering any use of the     */
/*     contents herein.                                               */
/*                                                                    */
/*     Any copies of this software or derivative works must include   */
/*     this and all other proprietary notices.                        */
/**********************************************************************/

/*****************************************************************************
* File:           filter.cpp
*
* Version:        DSPSE Release    2.0
*
* Description:    all initialization, filtering, and impulse response 
*                 filtering routines.                                         
*
* Revisions:      27 AUG 97         Sidd Gupta
*
* Copyright:      (c) Copyright 1997 DSP Software Engineering, Inc.
*                 All Rights Reserved. Duplication Strictly Prohibited.
*****************************************************************************/

#include"qcelp.h"
#include"qcelp13.h"

#include<stdlib.h>

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  fir_filt(I16 *data, I16 * coeffs, I16 * hist, Int16 len, Int16 order)
Void  fir_filt(Int16 *data, Int16 * coeffs, Int16 * hist, Int16 len, Int16 order)
{
    Int32 acc;
    Int16 i, j;
	
    for (i = 0; i < len; i++)
    {
        acc=(Int32)0l;
        for (j = (Int16)(order - 1) ; j > 0; j--)
        {
            //acc += (Int32)hist[j]*(Int32)coeffs[j+1];
			acc = L_add(acc, (Int32)hist[j]*(Int32)coeffs[j+1]);
            hist[j] = hist[j - 1];
        }
        //acc += (Int32)hist[j]*(Int32)coeffs[j+1];
        //acc += (Int32)data[i]*(Int32)coeffs[0];
		acc = L_add(acc, (Int32)hist[j]*(Int32)coeffs[j+1]);
		acc = L_add(acc, (Int32)data[i]*(Int32)coeffs[0]);
        hist[0]=data[i];
        acc = L_add(acc, (Int32)0x00004000l);
        data[i] = (Int16)saturate(acc >> 15);
    }
}

/****************************************************************************
Integer Subroutines
****************************************************************************/
//Void  polefilt(I16 *data, I16 *coeffs, I16 *hist, Int16 len)
Void  polefilt(Int16 *data, Int16 *coeffs, Int16 *hist, Int16 len)
{
    Int16 i,j;
    Int32 ar;
	
    for (i = 0; i < len; i++)
    {
        ar = ((Int32)data[i]) << 13;
        for (j = LPCORDER-1; j > 0; j--)
        {
            //ar += (Int32)coeffs[j] * (Int32)hist[j];
			ar = L_add(ar, (Int32)coeffs[j] * (Int32)hist[j]);
            hist[j] = hist[j - 1];
        }
        //ar += (Int32)coeffs[0] * (Int32)hist[0];
		ar = L_add(ar, (Int32)coeffs[0] * (Int32)hist[0]);
		
        hist[0] = (Int16)saturate(ar >> 13);
        data[i] = hist[0];
    }
}


/* Special double-precision synthesis filter for decoder */
//Void  dp_polefilt(I16 *data, I16 *coeffs, I32 *hist, Int16 len)
#if 1  //C_OPT
Void  dp_polefilt(Int16 *data, Int16 *coeffs, Int32 *hist, Int16 len)
{
    Int16        i;
    Int32        tmp_acca;
    Int32        tmp_accb;
    Int32        tmp_hist_hi[LPCORDER];
	Int32        tmp_hist_lo[LPCORDER];
    /* Copy coeffs[] and hist[] into temp. floating point arrays */
    for (i = 0; i < LPCORDER; i++)
    {
        tmp_hist_hi[i] = (Int32)hist[i] >> 16;
        tmp_hist_lo[i] = (Int32)hist[i] & 0xffff;
    }
    for (i = 0; i < len; i++)
    {
		tmp_accb  = ((Int32)coeffs[9] * tmp_hist_hi[9]) << 1;
		tmp_accb += ((Int32)coeffs[9] * tmp_hist_lo[9]) >> 15;
		tmp_hist_hi[9] = tmp_hist_hi[8];
		tmp_hist_lo[9] = tmp_hist_lo[8]; 

		tmp_accb += ((Int32)coeffs[8] * tmp_hist_hi[8]) << 1;
		tmp_accb += ((Int32)coeffs[8] * tmp_hist_lo[8]) >> 15;
		tmp_hist_hi[8] = tmp_hist_hi[7];
		tmp_hist_lo[8] = tmp_hist_lo[7];

		tmp_accb += ((Int32)coeffs[7] * tmp_hist_hi[7]) << 1;
		tmp_accb += ((Int32)coeffs[7] * tmp_hist_lo[7]) >> 15;
		tmp_hist_hi[7] = tmp_hist_hi[6];
		tmp_hist_lo[7] = tmp_hist_lo[6];

		tmp_accb += ((Int32)coeffs[6] * tmp_hist_hi[6]) << 1;
		tmp_accb += ((Int32)coeffs[6] * tmp_hist_lo[6]) >> 15;
		tmp_hist_hi[6] = tmp_hist_hi[5];
		tmp_hist_lo[6] = tmp_hist_lo[5];

		tmp_accb += ((Int32)coeffs[5] * tmp_hist_hi[5]) << 1;
		tmp_accb += ((Int32)coeffs[5] * tmp_hist_lo[5]) >> 15;
		tmp_hist_hi[5] = tmp_hist_hi[4];
		tmp_hist_lo[5] = tmp_hist_lo[4];

		tmp_accb += ((Int32)coeffs[4] * tmp_hist_hi[4]) << 1;
		tmp_accb += ((Int32)coeffs[4] * tmp_hist_lo[4]) >> 15;
		tmp_hist_hi[4] = tmp_hist_hi[3];
		tmp_hist_lo[4] = tmp_hist_lo[3];

		tmp_accb += ((Int32)coeffs[3] * tmp_hist_hi[3]) << 1;
		tmp_accb += ((Int32)coeffs[3] * tmp_hist_lo[3]) >> 15;
		tmp_hist_hi[3] = tmp_hist_hi[2];
		tmp_hist_lo[3] = tmp_hist_lo[2];

		tmp_accb += ((Int32)coeffs[2] * tmp_hist_hi[2]) << 1;
		tmp_accb += ((Int32)coeffs[2] * tmp_hist_lo[2]) >> 15;
		tmp_hist_hi[2] = tmp_hist_hi[1];
		tmp_hist_lo[2] = tmp_hist_lo[1];

		tmp_accb += ((Int32)coeffs[1] * tmp_hist_hi[1]) << 1;
		tmp_accb += ((Int32)coeffs[1] * tmp_hist_lo[1]) >> 15;
		tmp_hist_hi[1] = tmp_hist_hi[0];
		tmp_hist_lo[1] = tmp_hist_lo[0];

        /* simulate DSP MPY_LS */
        tmp_accb += ((Int32)coeffs[0] * tmp_hist_hi[0]) << 1;
        tmp_accb += ((Int32)coeffs[0] * tmp_hist_lo[0]) >> 15;
		
		tmp_acca = L_shl2(tmp_accb, 3);
		tmp_acca = L_add(tmp_acca, (Int32)data[i] << 10);
        
        /* update history */
        tmp_hist_hi[0] = tmp_acca >> 16;
        tmp_hist_lo[0] = tmp_acca & 0xffff;
		
		tmp_acca = L_shl2(tmp_acca, 6);
		
        data[i] = (Int16) (tmp_acca >> 16);
    }	
    /* Update hist[] with tmp_hist[] */
    for (i = 0; i < LPCORDER; i++)
    {
        hist[i] = (Int32)(((tmp_hist_hi[i] << 16) & 0xffff0000) | (tmp_hist_lo[i] & 0x0000ffff));
    }
}
#else
Void  dp_polefilt(Int16 *data, Int16 *coeffs, Int32 *hist, Int16 len)
{
    /*..(local) variables and arrays..*/
    Int16        i,j;
    Int32        tmp_acca;
    Int32        tmp_accb;
    Int32        tmp_hist_hi[LPCORDER], tmp_hist_lo[LPCORDER];
    Int32        tmp_coeffs[LPCORDER];
    //Int32        tmp_chen;
	
    /*..constants..*/
    const Int16  DP_HEADROOM   = 6;
	
    /* Copy coeffs[] and hist[] into temp. floating point arrays */
    for (j = 0; j < LPCORDER; j++)
    {
        tmp_hist_hi[j] = (Int32)hist[j] >> 16;
        tmp_hist_lo[j] = (Int32)hist[j] & 0xffff;
        tmp_coeffs[j] = (Int32)coeffs[j];
    }
	
    for (i = 0; i < len; i++)
    {
        tmp_acca = 0;
        for (j = LPCORDER-1; j > 0; j--)
        {
            //tmp_accb_hi, tmp_accb_lo;
            tmp_accb  = (tmp_coeffs[j] * tmp_hist_hi[j]) << 1;
            tmp_accb += (tmp_coeffs[j] * tmp_hist_lo[j]) >> 15;
            //tmp_acca += tmp_accb;
			tmp_acca = L_add(tmp_acca, tmp_accb);
            tmp_hist_hi[j] = tmp_hist_hi[j - 1]; // shift filter history 
            tmp_hist_lo[j] = tmp_hist_lo[j - 1]; // shift filter history 
        }
        /* simulate DSP MPY_LS */
        tmp_accb  = (tmp_coeffs[0] * tmp_hist_hi[0]) << 1;
        tmp_accb += (tmp_coeffs[0] * tmp_hist_lo[0]) >> 15;
        //tmp_acca += tmp_accb;
		tmp_acca = L_add(tmp_acca, tmp_accb);
		
        /* accA = accA << 3 */
		tmp_acca = L_shl2(tmp_acca, 3);
		tmp_acca = L_add(tmp_acca, (Int32)data[i] << (16-DP_HEADROOM));
        
		
        /* update history */
        tmp_hist_hi[0] = tmp_acca >> 16;
        tmp_hist_lo[0] = tmp_acca & 0xffff;
		
        /* accA = accA << DP_HEADROOM */
		tmp_acca = L_shl2(tmp_acca, DP_HEADROOM);
		
        data[i] = (Int16) (tmp_acca >> 16);
    }
	
    /* Update hist[] with tmp_hist[] */
    for (j = 0; j < LPCORDER; j++)
    {
        hist[j] = (Int32)(((tmp_hist_hi[j] << 16) & 0xffff0000) | (tmp_hist_lo[j] & 0x0000ffff));
    }
}
#endif

/****************************************************************************
Integer Subroutines
****************************************************************************/
//void  wghtfilt(I16 *data, I16 * pcoeffs,I16 * zcoeffs, I16 * hist, Int16 order, Int16 len)
#if 1  //C_OPT
void  wghtfilt(Int16 *data, Int16 * pcoeffs,Int16 * zcoeffs, Int16 * hist, Int16 order, Int16 len)
{
    Int16 i;
    Int32 arA, arB;
    Int16 extra_shift = 0;

    for (i = 0; i < len; i++)
    {
        arA = (data[i] << (9 - extra_shift));

        arA += (Int32)pcoeffs[9] * (Int32)hist[9];
        arB = ((Int32)hist[9] * (Int32)zcoeffs[9]);
		hist[9] = hist[8];

        arA += ((Int32)pcoeffs[8] * (Int32)hist[8]);
        arB += ((Int32)hist[8] * (Int32)zcoeffs[8]);
		hist[8] = hist[7];

        arA += ((Int32)pcoeffs[7] * (Int32)hist[7]);
        arB += ((Int32)hist[7] * (Int32)zcoeffs[7]);
		hist[7] = hist[6];

		arA += ((Int32)pcoeffs[6] * (Int32)hist[6]);
        arB += ((Int32)hist[6] * (Int32)zcoeffs[6]);
		hist[6] = hist[5];

		arA += ((Int32)pcoeffs[5] * (Int32)hist[5]);
        arB += ((Int32)hist[5] * (Int32)zcoeffs[5]);
		hist[5] = hist[4];

		arA += ((Int32)pcoeffs[4] * (Int32)hist[4]);
        arB += ((Int32)hist[4] * (Int32)zcoeffs[4]);
		hist[4] = hist[3];

		arA += ((Int32)pcoeffs[3] * (Int32)hist[3]);
        arB += ((Int32)hist[3] * (Int32)zcoeffs[3]);
		hist[3] = hist[2];

		arA += ((Int32)pcoeffs[2] * (Int32)hist[2]);
        arB += ((Int32)hist[2] * (Int32)zcoeffs[2]);
		hist[2] = hist[1];

		arA += ((Int32)pcoeffs[1] * (Int32)hist[1]);
        arB += ((Int32)hist[1] * (Int32)zcoeffs[1]);
		hist[1] = hist[0];

        arA += ((Int32)pcoeffs[0] * (Int32)hist[0]);
        arB +=(((Int32)hist[0] * (Int32)zcoeffs[0]));

		arB = L_shl2((arA-arB),(7+extra_shift));		
		//arB = ((arA-arB) << (7+extra_shift));
		arA = L_shl2(arA ,4);
		//arA = (arA << 4);
        arA = L_add(arA, 0x00008000l);
        hist[0] = (Int16)(arA >> 16);
		
        /* check for potential saturation */
        arA = (arA >= 0) ? arA : (-arA);	       	

        arB = L_add(arB, 0x00008000l);
        data[i] = (Int16)(arB >> 16);
		
        /* if the write to filter memory either saturated or came within */
        /* 1 dB of saturation, add extra headroom. */
        if (arA > 0x70000000L)
        {
            extra_shift += 2;
            /* limit extra_shift */
            if (extra_shift > 6)
            {
                extra_shift = 6;
            }
        }
    }
}
#else
void  wghtfilt(Int16 *data, Int16 * pcoeffs,Int16 * zcoeffs, Int16 * hist, Int16 order, Int16 len)
{
    /*..(local) variables and arrays..*/
    Int16 i, j;
    Int32 arA, arB;
    Int16 extra_shift;
    Int32 L_potential_saturation;
	
    /*..constants..*/
    const Int16 HEADROOM        = 3;
    const Int16 EXTRA_HEADROOM  = 2;
    const Int16 MAX_EXTRA_SHIFT = 6;
	
    /*..execute..*/
    extra_shift = 0;
	
    for (i = 0; i < len; i++)
    {
		//        arA = ((Int32)data[i]) << (13-HEADROOM-extra_shift);
        arA = L_shl(((Int32)data[i]), (12-HEADROOM-extra_shift));
		
        for (j = (Int16)(order-1); j > 0; j--)
        {
            //arA += (Int32)pcoeffs[j] * (Int32)hist[j]; // Q12 * Q14 = Q27 (with left shift)
			arA = L_add(arA, (Int32)pcoeffs[j] * (Int32)hist[j]); // Q12 * Q14 = Q27 (with left shift)
        }
        arA = L_add(arA, (Int32)pcoeffs[0] * (Int32)hist[0]);
        //arA <<= 1;
		arA = L_shl2(arA, 1);
		
        arB = 0l;
        for (j = (Int16)(order-1); j > 0; j--)
        {
            arB = L_sub(arB, (Int32)hist[j] * (Int32)zcoeffs[j]);
            hist[j] = hist[j -1];
        }
        //arB <<= 1;
		arB = L_shl2(arB, 1);
		
        arB = L_sub(arB, ((Int32)hist[0] * (Int32)zcoeffs[0]) << 1);
        arB = L_add(arB, arA);
		
        //arA <<= 3;
		arA = L_shl2(arA, 3);
        arA = L_add(arA, 0x00008000l);
        hist[0] = (Int16)(arA >> 16);
		
        /* check for potential saturation */
        arA = (arA >= 0) ? arA : (-arA);
		
		//cast a varible instead of a constant. yhp. 10/26/01
		arA = arA - ((Int32)0x7000 << 16);
        
		L_potential_saturation = arA;
		
        /* simulate arB << 3+HEADROOM+extra_shift */
        for (j = 0; j < (3+HEADROOM+extra_shift); j++)
        {
            //arB <<= 1;
			arB = L_shl2(arB, 1);
        }
        arB = L_add(arB, 0x00008000l);
        data[i] = (Int16)(arB >> 16);
		
        /* if the write to filter memory either saturated or came within */
        /* 1 dB of saturation, add extra headroom. */
        if (L_potential_saturation > 0)
        {
            extra_shift += EXTRA_HEADROOM;
            /* limit extra_shift */
            if (extra_shift > MAX_EXTRA_SHIFT)
            {
                extra_shift = MAX_EXTRA_SHIFT;
            }
        }
    }
}
#endif
/****************************************************************************
Integer Subroutines
****************************************************************************/
//I16  postfilt(Int16 mode, I16 * data, I16 * lpc, I16 * pf_hist, I16 bright, I16 history, Int16 length)
Int16  postfilt(Int16 mode, Int16 * data, Int16 * lpc, Int16 * pf_hist, Int16 bright, Int16 history, Int16 length)
{
    Int16 zlpc[LPCORDER];
    Int16 plpc[LPCORDER];
    Int16 zw, pw;
    Int16 zero_wght_factor;
    Int16 pole_wght_factor;
    Int32 arA,arB,temp;
    Int16 i;
	
    if (mode==QC13)
    {
        zw=QC13_PF_ZERO_WGHT_FACTOR_16;
        pw=QC13_PF_POLE_WGHT_FACTOR_16;
        bright = (Int16)9830;
    }

#if 1  //C_OPT
    zero_wght_factor = (Int16)32767;
    pole_wght_factor = (Int16)32767;
	for(i = 0; i < LPCORDER; i++)
	{
        arA = ((Int32)zero_wght_factor * (Int32)zw) << 1; /* Q15 * Q15 = Q31 (with left shift) */
        zero_wght_factor = (Int16)(arA >> 16);
        arB = ((Int32)pole_wght_factor * (Int32)pw) << 1;/* Q15 * Q15 = Q31 (with left shift) */
        pole_wght_factor = (Int16)(arB >> 16);
        arA = ((Int32)zero_wght_factor * (Int32)lpc[i]) << 1; /* Q15 * Q12 = Q28 (with left shift) */;
        zlpc[i] = (Int16)(arA >> 16);
        arB = ((Int32)pole_wght_factor * (Int32)lpc[i]) << 1; /* Q15 * Q12 = Q28 (with left shift) */
        plpc[i] = (Int16)(arB >> 16);
	}
#else
    zero_wght_factor = (Int16)32767;	
    for (i = 0; i < LPCORDER; i++)
    {
        arA = ((Int32)zero_wght_factor * (Int32)zw) << 1; /* Q15 * Q15 = Q31 (with left shift) */
        zero_wght_factor = (Int16)(arA >> 16);
        arA = ((Int32)zero_wght_factor * (Int32)lpc[i]) << 1; /* Q15 * Q12 = Q28 (with left shift) */;
        zlpc[i] = (Int16)(arA >> 16);
    }
	
    pole_wght_factor = (Int16)32767;
    for (i = 0; i < LPCORDER; i++)
    {
        arA = ((Int32)pole_wght_factor * (Int32)pw) << 1;/* Q15 * Q15 = Q31 (with left shift) */
        pole_wght_factor = (Int16)(arA >> 16);
        arA = ((Int32)pole_wght_factor * (Int32)lpc[i]) << 1; /* Q15 * Q12 = Q28 (with left shift) */
        plpc[i] = (Int16)(arA >> 16);
    }
#endif	

#ifdef ASM_OPT //asm_opt
    wghtfilt_asm(data, plpc, zlpc, pf_hist, LPCORDER, length);
#else
    wghtfilt(data, plpc, zlpc, pf_hist, LPCORDER, length);
#endif
	
    if (mode==QC13)
    {
        for (i = 0; i < length; i++)
        {
            arA =  (Int32)data[i] << 17; // Q31
            arA -= ((Int32)bright * (Int32)history); /* Q15 * Q15 = Q30 (with left shift) */
            arA += 0x00004000l;
            arA = L_shl2(arA, 1);
            data[i] = (Int16)(arA >> 16);
            history = data[i];
        }
    }
    else
    {
        for (i = 0; i < length; i++)
        {
            arB = 0l;
            arA = (Int32)data[i] << 17;
            arA -= (Int32)bright * (Int32)history;
			
            arB -= (Int32)bright * (Int32)history ;   /* Q15 * Q14 = Q30 (with left shift) */
            arB += arA;
            temp = (arA >= 0) ? arA : (-arA);
            if (temp & 0x40000000)
            {
                if (arA >= 0) arA = 0x7fffffff;
                else arA = 0x80000000;
            }
            else
            {
                arA <<= 1;
            }
            history=(Int16)(arA >> 16);
			
            arB = L_shl2(arB, 1);
            data[i]=(Int16)(arB >> 16);
        }
    }
	
    return(history);
}
