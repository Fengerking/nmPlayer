///******************************************************************************
//**
//**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
//**   > Software Release 2.1 (2008-06)
//**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
//**
//**   ?2004 Polycom, Inc.
//**
//**	 All rights reserved.
//**
//******************************************************************************/
//
///******************************************************************************
//* Filename: samples_to_rmlt_coefs.c
//*
//* Purpose:  Convert Samples to Reversed MLT (Modulated Lapped Transform) 
//*           Coefficients
//*
//*     The "Reversed MLT" is an overlapped block transform which uses
//*     even symmetry * on the left, odd symmetry on the right and a
//*     Type IV DCT as the block transform.  * It is thus similar to a
//*     MLT which uses odd symmetry on the left, even symmetry * on the
//*     right and a Type IV DST as the block transform.  In fact, it is
//*     equivalent * to reversing the order of the samples, performing
//*     an MLT and then negating all * the even-numbered coefficients.
//*
//******************************************************************************/
//
///***************************************************************************
// Include files                                                           
//***************************************************************************/
#include "defs.h"
#include "tables.h"
#include "basop32.h"

/***************************************************************************
Function:    samples_to_rmlt_coefs 

Syntax:      Word16 samples_to_rmlt_coefs(new_samples, 
old_samples,
coefs,
dct_length)
Word16 *new_samples;           
Word16 *old_samples;           
Word16 *coefs;                 
Word16 dct_length;

Description: Convert samples to MLT coefficients

Design Notes:

WMOPS:     7kHz |    24kbit    |     32kbit
-------|--------------|----------------
AVG  |    1.40      |     1.40
-------|--------------|----------------  
MAX  |    1.40      |     1.40
-------|--------------|---------------- 

14kHz |    24kbit    |     32kbit     |     48kbit
-------|--------------|----------------|----------------
AVG  |    3.07      |     3.07       |     3.07
-------|--------------|----------------|----------------
MAX  |    3.10      |     3.10       |     3.10
-------|--------------|----------------|----------------

***************************************************************************/

Word16 samples_to_rmlt_coefs(G722EncState *st, Word16 *coefs)
{
    Word16  *new_samples = (Word16 *)st->input;
    Word16  *old_samples = (Word16 *)st->history;
    Word16  dct_length = st->frame_size;
	Word16	index, vals_left,mag_shift,n;
	Word16	windowed_data[MAX_DCT_LENGTH];
	Word16	*new_ptr, *old_ptr, *sam_low, *sam_high;
	Word16	*win_low, *win_high;
	Word16	*dst_ptr;
	Word16  neg_win_low;
	Word16  samp_high;
	Word16  half_dct_size;

	Word32	acca;
	Word32	accb;
	Word16	temp;
	Word16	temp1;
	Word16	temp2;
	Word16	temp5;

	half_dct_size = dct_length >> 1;

	/*++++++++++++++++++++++++++++++++++++++++++++*/
	/* Get the first half of the windowed samples */
	/*++++++++++++++++++++++++++++++++++++++++++++*/   
	dst_ptr  = windowed_data;  
	/* address arithmetic */
	if (dct_length==DCT_LENGTH)
	{
		win_high = samples_to_rmlt_window + half_dct_size;
	}
	else
	{
		win_high = max_samples_to_rmlt_window + half_dct_size;
	}   
	win_low  = win_high;   
	/* address arithmetic */
	sam_high = old_samples + half_dct_size;   
	sam_low  = sam_high;    
	for (vals_left = half_dct_size;vals_left > 0;vals_left--)
	{
		acca = 0L;       
		acca = L_mac(acca,*--win_low, *--sam_low);
		acca = L_mac(acca,*win_high++, *sam_high++);
		temp = voRound(acca);      
		*dst_ptr++ = temp;
	}           

	/*+++++++++++++++++++++++++++++++++++++++++++++*/
	/* Get the second half of the windowed samples */
	/*+++++++++++++++++++++++++++++++++++++++++++++*/  
	sam_low  = new_samples;

	/* address arithmetic */
	sam_high = new_samples + dct_length;

	for (vals_left = half_dct_size; vals_left > 0; vals_left--)
	{
		acca = 0L;
		acca = L_mac(acca,*--win_high, *sam_low++);
		neg_win_low = negate(*win_low++);
		samp_high = *--sam_high;
		acca = L_mac(acca, neg_win_low, samp_high);
		temp = voRound(acca);       
		*dst_ptr++=temp;
	}

	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	/* Save the new samples for next time, when they will be the old samples */
	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/    
	new_ptr = new_samples;
	old_ptr = old_samples;

	for (vals_left = dct_length;vals_left > 0;vals_left--)
	{
		*old_ptr++ = *new_ptr++;
	}

	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	/* Calculate how many bits to shift up the input to the DCT.             */
	/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/   
	temp1=0;
	for(index=0;index<dct_length;index++)
	{
		temp2 = abs_s(windowed_data[index]);
		if(temp2 > temp1)
		{
			temp1 = temp2;
		}
	}

	mag_shift=0;
	if (temp1 >= 14000)
	{
		mag_shift = 0;
	}
	else
	{
		if(temp1 < 438)
			temp = temp1 + 1;
		else 
		{
			temp = temp1;
		}
		accb = L_mult(temp,9587);
		acca = (accb >> 20);
		temp5 = extract_l(acca);
		temp = norm_s(temp5);
		if (temp == 0)
		{
			mag_shift = 9;
		}
		else
			mag_shift = temp - 6;

	}
	acca = 0L;
	for(index=0; index<dct_length; index++)
	{
		temp = abs_s( windowed_data[index]);
		acca = L_add(acca,temp);
	}

	acca >>= 7;

	if (temp1 < acca)
	{
		mag_shift -=1;
	}

	if (mag_shift > 0) 
	{
		for(index=0;index<dct_length;index++)
		{
			windowed_data[index] = shl(windowed_data[index],mag_shift);
		}
	}
	else 
	{
		if (mag_shift < 0) 
		{
			n = negate(mag_shift);
			for(index=0;index<dct_length;index++)
			{
				windowed_data[index] = shr(windowed_data[index],n);
			}
		}
	}

	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
	/* Perform a Type IV DCT on the windowed data to get the coefficients */
	/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

	dct_type_iv_a(windowed_data, coefs, dct_length);

	return(mag_shift);
}
