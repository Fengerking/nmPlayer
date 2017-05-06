	/************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#include "struct.h"

static const int pow05[] = {
    Q28(1.68179283050743), /* 0.5^(-3/4) */
	Q28(1.41421356237310), /* 0.5^(-2/4) */
	Q28(1.18920711500272), /* 0.5^(-1/4) */
	Q28(1.0),              /* 0.5^( 0/4) */
	Q28(0.84089641525371), /* 0.5^(+1/4) */
	Q28(0.70710678118655), /* 0.5^(+2/4) */
	Q28(0.59460355750136)  /* 0.5^(+3/4) */
};

int mi_decode(AACDecoder* decoder,int channels)
{
	ICS_Data *ics;
	int g, b,sfb,num_window_groups,max_sfb,mask_present,window_group_length;
	int *l_spec, *r_spec;
	int tmp;
	unsigned char *sfb_cb,*ms_used, *is_used;
	short *sfb_offset,*scaleFactor;
	if(channels!=2||decoder->common_window ==0||!(decoder->ms_mask_present || decoder->intensity_used))
		return 0;

	ics = &(decoder->ICS_Data[0]);
	if (ics->window_sequence == EIGHT_SHORT_SEQUENCE) {
		sfb_offset = (short*)(sfBandTabShort + sfBandTabShortOffset[decoder->sampRateIdx]);
	} else {
		sfb_offset = (short*)(sfBandTabLong + sfBandTabLongOffset[decoder->sampRateIdx]);
	}
	num_window_groups = ics->num_window_groups;
	max_sfb			  = ics->max_sfb;
	sfb_cb			  = decoder->sfb_cb[1];
	scaleFactor		  = decoder->scaleFactors[1];
	mask_present	  = decoder->ms_mask_present;
	l_spec = decoder->coef[0];
	r_spec = decoder->coef[1];
	for (g = 0; g < num_window_groups; g++) 
	{
		ms_used = decoder->ms_used[g];
		is_used = decoder->is_used[g];
		window_group_length = ics->window_group_length[g];
		for (b = 0; b < window_group_length; b++)
		{
			int* left;
			int* right;

			if(decoder->profile != 22) //VOAAC_ER_BSAC
			{
				for (sfb = 0; sfb <max_sfb; sfb++)
				{

					int cb =  sfb_cb[sfb];
					int size   = sfb_offset[sfb+1] - sfb_offset[sfb];
					int offset = sfb_offset[sfb];
					left = l_spec + offset;
					right = r_spec + offset;
					if(cb==INTENSITY_HCB||cb==INTENSITY_HCB2)//intensity decode
					{
						int   exp =  scaleFactor[sfb] >> 2;
						int   frac = scaleFactor[sfb] & 3;
						int   pow = pow05[frac + 3];
						int   reverse=1;

						if(mask_present==1)
							reverse = 1 - 2*ms_used[sfb];

						if(exp < 0)
						{
							for (; size>0; size--)
							{							
								*right = *left << -exp;
								*right = MUL_28(*right, pow);
								if (reverse<0)
									*right = -*right;

								left++;
								right++;
							}			
						}
						else
						{
							for (; size>0; size--)
							{							
								*right = *left >> exp;
								*right = MUL_28(*right, pow);
								if (reverse<0)
									*right = -*right;

								left++;
								right++;
							}			
						}
					}
					else if((ms_used[sfb]||mask_present == 2)&&(cb!=NOISE_HCB))//ms decode 4.6.8.1.3
					{

						for (; size>0; size--)
						{						
							tmp = *left - *right;
							*left = *left + *right;
							*right = tmp;
							left++;
							right++;
						}
					}
				}
			}
#ifdef BSAC_DEC
			else
			{
				for (sfb = 0; sfb <max_sfb; sfb++)
				{
					int size   = sfb_offset[sfb+1] - sfb_offset[sfb];
					int offset = sfb_offset[sfb];
					left = l_spec + offset;
					right = r_spec + offset;
					
					if((ms_used[sfb]||mask_present == 2))
					{

						for (; size>0; size--)
						{						
							tmp = *left - *right;
							*left = *left + *right;
							*right = tmp;
							left++;
							right++;
						}
					}

					if(mask_present == 3 && is_used[sfb])//intensity decode
					{
						int   exp =  scaleFactor[sfb] >> 2;
						int   frac = scaleFactor[sfb] & 3;
						int   pow = pow05[frac + 3];
						int   reverse=1;

						if(is_used[sfb] == 2)
							reverse = -1;

						if(exp < 0)
						{
							for (; size>0; size--)
							{							
								*right = *left << -exp;
								*right = MUL_28(*right, pow);
								if (reverse<0)
									*right = -*right;

								left++;
								right++;
							}			
						}
						else
						{
							for (; size>0; size--)
							{							
								*right = *left >> exp;
								*right = MUL_28(*right, pow);
								if (reverse<0)
									*right = -*right;

								left++;
								right++;
							}			
						}
					}
				}
			}
#endif			
			l_spec += SIZE_SHORT_WIN;
			r_spec += SIZE_SHORT_WIN;
		}
		sfb_cb	   +=max_sfb;	
		scaleFactor+=max_sfb;
	}

	return 0;
}
