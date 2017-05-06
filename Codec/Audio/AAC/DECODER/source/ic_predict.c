#include "struct.h"
#include "decoder.h"

#ifdef MAIN_DEC

/* A*Q30 */
#define A 0x3d000000  
/* B*Q30 */
#define B 0x3d000000
/* ALPHA*Q30 */
#define ALPHA 0x7400

static void ic_predict(PRED_State *state, int input, int *output, unsigned char pred)
{
	int dr1;
	int predictedvalue;
	int e0, e1;
	int k1, k2;
	int r[2];
	VO_S64 COR[2], VAR[2];

	r[0] = state->r[0];
	r[1] = state->r[1];

	COR[0] = state->COR[0];
	VAR[0] = state->VAR[0];
	COR[1] = state->COR[1];
	VAR[1] = state->VAR[1];

	if (COR[0] == 0 || VAR[0] <= 1)
	{
		k1 = 0;
	} else {
		k1 = (int)(COR[0]*(1 << 15)/VAR[0]);
		k1 = MUL_30(k1, B);
	}

    if (pred)
    {
		if (COR[1] == 0 || VAR[1] <= 1)
		{
			k2 = 0;
		} else {
			k2 = (int)(COR[1]*(1 << 15)/VAR[1]);
			k2 = MUL_30(k2, B);
		}

		predictedvalue = MUL_15(k1, r[0]) + MUL_15(k2, r[1]);
		*output = input + predictedvalue;
    }

    /* calculate new state data */
    e0 = *output;
    e1 = e0 - MUL_15(k1, r[0]);
    dr1 = MUL_15(k1, e0);

	{
		int r0 = r[0] >> 11;
		int r1 = r[1] >> 11;
		int ee0 = e0 >> 11;
		int ee1 = e1 >> 11;

		state->VAR[0] = ((ALPHA*VAR[0]) >> 15) + (((VO_S64)r0*(VO_S64)r0 + (VO_S64)ee0*(VO_S64)ee0) >> 1);
		state->COR[0] = ((ALPHA*COR[0]) >> 15) + ((VO_S64)r0*(VO_S64)ee0);
		state->VAR[1] = ((ALPHA*VAR[1]) >> 15) + (((VO_S64)r1*(VO_S64)r1 + (VO_S64)ee1*(VO_S64)ee1) >> 1);
		state->COR[1] = ((ALPHA*COR[1]) >> 15) + ((VO_S64)r1*(VO_S64)ee1);
	}

    state->r[1] = MUL_30(A, (r[0]-dr1));
    state->r[0] = MUL_30(A, e0);

}

static void reset_pred_state(PRED_State *state)
{
    state->r[0]   = 0;
    state->r[1]   = 0;
    state->COR[0] = 0;
    state->COR[1] = 0;
    state->VAR[0] = 1;
    state->VAR[1] = 1;
}

void reset_all_predictors(PRED_State *state, int frame_len)
{
    int i;

    for (i = 0; i < frame_len; i++)
        reset_pred_state(&state[i]);
}

void pns_reset_pred_state(AACDecoder* decoder, ICS_Data *ics, PRED_State *state, int ch)
{
	int sfb, g, b;
	int i, begin, end;
	short *sfb_offset;

	if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
		return;

	sfb_offset = (short*)(sfBandTabLong + sfBandTabLongOffset[decoder->sampRateIdx]);

	for (g = 0; g < ics->num_window_groups; g++)
	{
		for (b = 0; b < ics->window_group_length[g]; b++)
		{			
			unsigned char *sfb_cb			  = decoder->sfb_cb[ch];
			unsigned char *pns_sfb_flag      = decoder->pns_sfb_flag[ch];
			for (sfb = 0; sfb < ics->max_sfb; sfb++)
			{
				int cb = sfb_cb[sfb];
				if(cb == NOISE_HCB || pns_sfb_flag[cb])
				{
					begin = sfb_offset[sfb];
					end = MIN(sfb_offset[sfb+1], MAX_SAMPLES);

					for (i = begin; i < end; i++)
						reset_pred_state(&state[i]);
				}
			}
		}
	}
}

int ic_prediction(AACDecoder* decoder, int channels)
{
	int ch,sfb,bin;
	ICS_Data *ics;
	PRED_State  *pred_stat;
	int chOut, ChanIndex;

	chOut = decoder->decodedChans;

	if(decoder->profile!=VOAAC_AAC_MAIN)
		return 0;

	for(ch=0; ch<channels; ch++)
	{
		if(!EnableDecodeCurrChannel(decoder,ch))
			continue;

#if SUPPORT_MUL_CHANNEL
		if(decoder->channelNum > 2)//multi channel
		{
			if(decoder->seletedChs==VO_CHANNEL_ALL)
				ChanIndex = chOut + ch;
			else
			{
				ChanIndex = decoder->seletedSBRChDecoded+ch;
			}
		}
		else
#endif//SUPPORT_MUL_CHANNEL
			ChanIndex = chOut + ch;

		if(decoder->pred_stat[ChanIndex] == NULL)
		{
			decoder->pred_stat[ChanIndex] = (PRED_State *)voAACDecAlignedMalloc(decoder->voMemop, MAX_SAMPLES * sizeof(PRED_State));
			if(decoder->pred_stat[ChanIndex] == NULL)
			{
				return VO_ERR_OUTOF_MEMORY;
			}
			reset_all_predictors(decoder->pred_stat[ChanIndex], MAX_SAMPLES);
		}

		pred_stat = decoder->pred_stat[ChanIndex];
		
		if(decoder->common_window)
			ics = &decoder->ICS_Data[0];
		else
			ics = &decoder->ICS_Data[ch];

		if(ics->window_sequence == EIGHT_SHORT_SEQUENCE)
		{
			reset_all_predictors(pred_stat, MAX_SAMPLES);
		}
		else
		{
			int		pred_max_sfb = predSFBMax[decoder->sampRateIdx];
			short   *sfb_offset = (short*)(sfBandTabLong + sfBandTabLongOffset[decoder->sampRateIdx]);
			int		*coef = decoder->coef[ch];
			
			for (sfb = 0; sfb < pred_max_sfb; sfb++)
			{
				int low  = sfb_offset[sfb];
				int high = MIN(sfb_offset[sfb+1], MAX_SAMPLES);					
				unsigned char pred = ics->predictor_data_present && ics->prediction_used[sfb];

				for (bin = low; bin < high; bin++)
				{
					ic_predict(&pred_stat[bin], coef[bin], &coef[bin], pred);
				}
			}

			if (ics->predictor_data_present)
			{
				if (ics->predictor_reset)
				{
					for (bin = ics->predictor_reset_group_number - 1; bin < MAX_SAMPLES; bin += 30)
					{
						reset_pred_state(&pred_stat[bin]);
					}
				}
			}
		}

		pns_reset_pred_state(decoder, ics, pred_stat, ch);
	}

	return 0;
}

#endif
