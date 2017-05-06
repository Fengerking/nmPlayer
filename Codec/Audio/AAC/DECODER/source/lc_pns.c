        /************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#include "struct.h"
#include "decoder.h"
//static __inline 
int random2(VO_U32 *last)
{
	unsigned int r = *last;

	r = (1664525U * r) + 1013904223U;
	*last = r;

	return (int)r;
}

/* pow(2, i/4.0) for i = [0,1,2,3], format = Q30 */
static const int pow14[4] = { 
	//0x40000000, 0x4c1bf829, 0x5a82799a, 0x6ba27e65
	Q30(1.000000000000),
	Q30(1.189207115003),
	Q30(1.414213562373),
	Q30(1.681792830507),
};

#define NUM_ITER_INVSQRT	4

#define X0_COEF_2	0xc0000000	/* Q29: -2.0 */
#define X0_OFF_2	0x60000000	/* Q29:  3.0 */
#define Q26_3		0x0c000000	/* Q26:  3.0 */

int pns_decode(AACDecoder* decoder,int channels)
{
	int g, b,sfb,i,num_window_groups,max_sfb,mask_present,window_group_length,ch;
	int energy, sf, scalef, scalei, invSqrtEnergy, zero,r;
	ICS_Data *ics;
	int *spec;
	VO_U32 tmpseed;
	int tmp;
	unsigned char *sfb_cb,*ms_used;
	short *sfb_offset,*scaleFactor;
	unsigned char * pns_sfb_flag, *pns_sfb_mode;
	mask_present = decoder->ms_mask_present;
	for(ch=0;ch<channels;ch++)
	{
		if (!decoder->pns_data_present[ch])
			continue;
		if(!EnableDecodeCurrChannel(decoder,ch))
			continue;
		
		if(decoder->common_window)
			ics = &decoder->ICS_Data[0];
		else
			ics = &decoder->ICS_Data[ch];
		
		
		if (ics->window_sequence == EIGHT_SHORT_SEQUENCE) {
			sfb_offset = (short*)(sfBandTabShort + sfBandTabShortOffset[decoder->sampRateIdx]);
		} else {
			sfb_offset = (short*)(sfBandTabLong + sfBandTabLongOffset[decoder->sampRateIdx]);
		}
		num_window_groups = ics->num_window_groups;
		max_sfb			  = ics->max_sfb;
		sfb_cb			  = decoder->sfb_cb[ch];
		scaleFactor		  = decoder->scaleFactors[ch];
		pns_sfb_flag      = decoder->pns_sfb_flag[ch];
		pns_sfb_mode	  = decoder->pns_sfb_mode;

		spec = decoder->coef[ch];
		for (g = 0; g < num_window_groups; g++) 
		{
			ms_used = decoder->ms_used[g];
			window_group_length = ics->window_group_length[g];
			for (b = 0; b < window_group_length; b++)
			{
				int* currSpec;
				for (sfb = 0; sfb <max_sfb; sfb++)
				{					
					int cb =  sfb_cb[sfb];
				
					if(cb==NOISE_HCB || pns_sfb_flag[cb])
					{
						int size   = sfb_offset[sfb+1] - sfb_offset[sfb];
						int offset = sfb_offset[sfb];
#ifdef LTP_DEC
						decoder->Ltp_Data[0].long_used[sfb] = 0;
						decoder->Ltp_Data[1].long_used[sfb] = 0;
#endif//LTP_DEC	

#ifdef MAIN_DEC
						ics->prediction_used[sfb] = 0;
#endif
						currSpec = spec+offset;
						

						if(ch==0)
						{						
							/* Generate random vector */
							tmpseed = decoder->pns_seed;
							for (i = 0; i < size; i++)
								currSpec[i] = random2(&tmpseed)>> 16;
							
							decoder->pns_seed = tmpseed;

							if(channels==2&&decoder->sfb_cb[1][g*max_sfb+sfb]==NOISE_HCB)
							{
								decoder->voMemop->Copy(VO_INDEX_DEC_AAC, currSpec+MAX_SAMPLES,currSpec,size*sizeof(int));
							}								
						}
						else//ch==1
						{
							
							if(decoder->sfb_cb[0][g*max_sfb+sfb]!=NOISE_HCB
							  || !(((mask_present==1&&ms_used[sfb])||mask_present == 2))
							  )
							{					
								tmpseed = decoder->pns_seed;
								for (i = 0; i < size; i++)
									currSpec[i] = random2(&tmpseed) >> 16;
								decoder->pns_seed = tmpseed;
							}
						}
				
						sf = scaleFactor[sfb];
						energy = 0;
						for (i = 0; i < size; i++) {
							tmp = currSpec[i];

							energy += (tmp * tmp) >> 8;		
						}

						if(energy == 0)
							return 0;

						scalef = pow14[sf & 0x3];
						scalei = (sf >> 2) + FBITS_OUT_DQ_OFF;

						zero = CLZ(energy) - 2;		
						zero &= 0xfffffffe;	
						r = energy << zero;
						
						invSqrtEnergy = (MULHIGH(r, X0_COEF_2) << 2) + X0_OFF_2;

						for (i = 0; i < NUM_ITER_INVSQRT; i++) {
							tmp = MULHIGH(invSqrtEnergy, invSqrtEnergy);					
							tmp = Q26_3 - (MULHIGH(r, tmp) << 2);	
							invSqrtEnergy = MULHIGH(invSqrtEnergy, tmp) << 5;		
						}

						if (invSqrtEnergy >> 30)
							invSqrtEnergy = (1 << 30) - 1;

						scalei -= (15 - zero/2 + 4);				

						zero = CLZ(invSqrtEnergy) - 1;
						invSqrtEnergy <<= zero;
						scalei -= (zero - 5);	
						scalef = MULHIGH(scalef, invSqrtEnergy);	

						if (scalei < 0) {
							scalei = -scalei;
							if (scalei > 31)
								scalei = 31;
							for (i = 0; i < size; i++) {
								currSpec[i] = MULHIGH(currSpec[i], scalef) >> scalei;
							}
						} else {
							if (scalei > 16)
								scalei = 16;						
							for (i = 0; i < size; i++) {
								currSpec[i] = MULHIGH(currSpec[i] << scalei, scalef);
							}
						}						
					}				
				}
				spec += SIZE_SHORT_WIN;
			}
			sfb_cb	   +=max_sfb;	
			scaleFactor+=max_sfb;
		}
	}
	return 0;
}
