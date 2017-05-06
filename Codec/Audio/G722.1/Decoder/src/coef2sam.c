/*****************************************************************************
**
**   ITU-T G.722.1 (2005-05) - Fixed point implementation for main body and Annex C
**   > Software Release 2.1 (2008-06)
**     (Simple repackaging; no change from 2005-05 Release 2.0 code)
**
**   ?2004 Polycom, Inc.
**
**   All rights reserved.
**
*****************************************************************************/

/*****************************************************************************
* Filename: rmlt_coefs_to_samples.c
*
* Purpose:  Convert Reversed MLT (Modulated Lapped Transform) 
*           Coefficients to Samples
*
*     The "Reversed MLT" is an overlapped block transform which uses
*     even symmetry * on the left, odd symmetry on the right and a
*     Type IV DCT as the block transform.  * It is thus similar to a
*     MLT which uses odd symmetry on the left, even symmetry * on the
*     right and a Type IV DST as the block transform.  In fact, it is
*     equivalent * to reversing the order of the samples, performing
*     an MLT and then negating all * the even-numbered coefficients.
*
*****************************************************************************/

/***************************************************************************
Include files                                                           
***************************************************************************/
#include "defs.h"
#include "tables.h"

/***************************************************************************
Function:     rmlt_coefs_to_samples 

Syntax:       void rmlt_coefs_to_samples(Word16 *coefs,       
Word16 *old_samples, 
Word16 *out_samples, 
Word16 dct_length,
Word16 mag_shift)    

inputs:    Word16 *coefs
Word16 *old_samples
Word16 dct_length
Word16 mag_shift


outputs:   Word16 *out_samples

Description:  Converts the mlt_coefs to samples

Design Notes:

WMOPS:     7kHz |    24kbit    |    32kbit
-------|--------------|----------------
AVG  |     1.91     |    1.91
-------|--------------|----------------  
MAX  |     1.91     |    1.91
-------|--------------|---------------- 

14kHz |    24kbit    |    32kbit      |     48kbit
-------|--------------|----------------|----------------
AVG  |     3.97     |    3.97        |     3.97   
-------|--------------|----------------|----------------
MAX  |     3.97     |    3.97        |     3.97   
-------|--------------|----------------|----------------

***************************************************************************/

/*void rmlt_coefs_to_samples(Word16 *coefs,     
						   Word16 *old_samples,
						   Word16 *out_samples,           
						   Word16 dct_length,
						   Word16 mag_shift)    */   
void  rmlt_coefs_to_samples(Word16 *coefs, G722DecState *st, Word16 mag_shift)
{
    Word16  *old_samples = (Word16 *)st->old_samples;
	Word16  *out_samples = (Word16 *)st->output;
    Word16  dct_length = st->frame_size;
	Word16	index, vals_left;
	Word16	new_samples[MAX_DCT_LENGTH];
	Word16	*new_ptr, *old_ptr;
	Word16	*win_new, *win_old;
	Word16	*out_ptr;
	Word16  half_dct_size;
	Word32  sum;
	half_dct_size = (dct_length >> 1);

	/* Perform a Type IV (inverse) DCT on the coefficients */
	dct_type_iv_s(coefs, new_samples, dct_length);
	if (mag_shift > 0) 
	{
		for(index=0;index<dct_length;index++)
		{
			new_samples[index] = (new_samples[index] >> mag_shift);
		}
	}
	else 
	{
		if (mag_shift < 0) 
		{
			mag_shift = -mag_shift;
			for(index=0;index<dct_length;index++)
			{
				new_samples[index] = shl(new_samples[index],mag_shift);
			}
		}

	}

	/* Get the first half of the windowed samples */
	out_ptr = out_samples;
	if (dct_length==DCT_LENGTH)
	{
		win_new = rmlt_to_samples_window;
		win_old = rmlt_to_samples_window + dct_length;
	}
	else
	{
		win_new = max_rmlt_to_samples_window;
		win_old = max_rmlt_to_samples_window + dct_length;
	}
	old_ptr = old_samples;
	new_ptr = new_samples + half_dct_size;

	for (vals_left = half_dct_size;vals_left > 0;vals_left--)
	{
		sum = 0L;
		sum = L_mac(sum,*win_new++, *--new_ptr);
		sum = L_mac(sum,*--win_old, *old_ptr++);
		*out_ptr++ = voRound(L_shl(sum,2));
	}

	/* Get the second half of the windowed samples */

	for (vals_left = half_dct_size;vals_left > 0;vals_left--)
	{
		sum = 0L;
		sum = L_mac(sum,*win_new++, *new_ptr++);
		sum = L_mac(sum,-(*--win_old), *--old_ptr);
		*out_ptr++ = voRound(L_shl(sum,2));
	}

	/* Save the second half of the new samples for   */
	/* next time, when they will be the old samples. */
	/* pointer arithmetic */
	new_ptr = new_samples + half_dct_size;
	old_ptr = old_samples;
	for (vals_left = half_dct_size;vals_left > 0;vals_left--)
	{
		*old_ptr++ = *new_ptr++;
	}
}
