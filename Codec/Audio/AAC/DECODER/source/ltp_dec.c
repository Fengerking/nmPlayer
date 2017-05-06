#include "struct.h"
#include "decoder.h"

#ifdef LTP_DEC

static const int ltp_coef_C[8] =
{
    Q14(0.570829),
    Q14(0.696616),
    Q14(0.813004),
    Q14(0.911304),
    Q14(0.984900),
    Q14(1.067894),
    Q14(1.194601),
    Q14(1.369533)
};

static int ltp_mdct(ICS_Data *ics,int* in_data,int* out_mdct,int winTypePrev)
{

	int i, in0, in1, f0, f1, w0, w1;
	const int *window_long = NULL;
	const int *window_long_prev = NULL;
	const int *window_short = NULL;
	const int *window_short_prev = NULL;

	int nlong = MAX_SAMPLES;
	int nshort = MAX_SAMPLES/8;
	int nflat_ls = (nlong-nshort)/2;
	int	 winTypeCurr = ics->window_shape;
	int	 *indata0, *indata1, *indata2, *indata3;
	int  *out_mdct0, *out_mdct1;

	indata0 = in_data;
	indata1 = in_data + nlong - 1;
	indata2 = in_data + nlong;
	indata3 = in_data + nlong + nlong - 1;

	out_mdct0 = out_mdct + nlong/2;
	out_mdct1 = out_mdct0 - 1;


	window_long       = (winTypeCurr == 1 ? kbdWindow + kbdWindowOffset[1] : sinWindow + sinWindowOffset[1]);
	window_long_prev  = (winTypePrev == 1 ? kbdWindow + kbdWindowOffset[1] : sinWindow + sinWindowOffset[1]);
	window_short      = (winTypeCurr == 1 ? kbdWindow + kbdWindowOffset[0] : sinWindow + sinWindowOffset[0]);
	window_short_prev = (winTypePrev == 1 ? kbdWindow + kbdWindowOffset[0] : sinWindow + sinWindowOffset[0]);


	switch(ics->window_sequence)
	{
	case ONLY_LONG_SEQUENCE:
		if(window_long == window_long_prev)
		{
			for(i=0; i<nlong >> 1; i++) {
				w0 = *window_long++;
				w1 = *window_long++;
				in0 = *indata0++;
				in1 = *indata1--;

				f0 = MUL_32(w0, in0);
				f1 = MUL_32(w1, in1);

				*out_mdct0++ = f0 - f1;

				in0 = *indata2++;
				in1 = *indata3--;

				f0 = MUL_32(w1, in0);
				f1 = MUL_32(w0, in1);

				*out_mdct1-- = -(f0 + f1);
			}
		}
		else
		{
			for(i=0; i<nlong >> 1; i++) {
				w0 = *window_long_prev++;
				w1 = *window_long_prev++;
				in0 = *indata0++;
				in1 = *indata1--;

				f0 = MUL_32(w0, in0);
				f1 = MUL_32(w1, in1);

				*out_mdct0++ = f0 - f1;

				w0 = *window_long++;
				w1 = *window_long++;

				in0 = *indata2++;
				in1 = *indata3--;

				f0 = MUL_32(w1, in0);
				f1 = MUL_32(w0, in1);

				*out_mdct1-- = -(f0 + f1);
			}
		}
		break;

	case LONG_START_SEQUENCE:
		for(i=0; i<nlong >> 1; i++) {
			w0 = *window_long_prev++;
			w1 = *window_long_prev++;
			in0 = *indata0++;
			in1 = *indata1--;

			f0 = MUL_32(w0, in0);
			f1 = MUL_32(w1, in1);

			*out_mdct0++ = f0 - f1;
		}

		for(i=0; i<nflat_ls >> 1; i++){
			in0 = *indata2++;
			*out_mdct1-- =  -(in0 >> 1);  

			in0 = *indata2++;
			*out_mdct1-- =  -(in0 >> 1); 
		}

		indata3 -= nflat_ls;

		for(i=0; i< nshort>>1; i++){
			w0 = *window_short++;
			w1 = *window_short++;
			in0 = *indata2++;
			in1 = *indata3--;

			f0 = MUL_32(w1, in0);
			f1 = MUL_32(w0, in1);


			*out_mdct1-- =  -(f0 + f1);  
		}
		break;

	case LONG_STOP_SEQUENCE:
		for(i=0; i<nflat_ls >> 1; i++){
			in1 = *indata1--;
			*out_mdct0++ =  -(in1 >> 1); 

			in1 = *indata1--;
			*out_mdct0++ =  -(in1 >> 1);
		} 

		indata0 += nflat_ls;
		for(i=0; i<nshort >> 1; i++){
			w0 = *window_short_prev++;
			w1 = *window_short_prev++;
			in0 = *indata0++;
			in1 = *indata1--;

			f0 = MUL_32(w1, in0);
			f1 = MUL_32(w0, in1);

			*out_mdct0++ = f0 - f1;  
		}

		for(i=0; i<nlong >> 1; i++) {
			w0 = *window_long++;
			w1 = *window_long++;

			in0 = *indata2++;
			in1 = *indata3--;

			f0 = MUL_32(w1, in0);
			f1 = MUL_32(w0, in1);

			*out_mdct1-- = -(f0 + f1);
		}
		break;
	default:
		//error(decoder,"error:window_sequence is not long_win!\n",ERR_INVALID_PROFILE);
		return 0;
	}

	MDCT(1, out_mdct);

	return 0;
}	

int ltp_decode(AACDecoder* decoder,int channels)
{
#define LTP_NUM_SAMPLE 2048
	int ch,i,sfb,bin, coef;
	ICS_Data *ics;
	int *t_est; 
    int *f_est;
	int chOut, ChanIndex;

	chOut = decoder->decodedChans;

	for(ch=0;ch<channels;ch++)
	{
		LTP_Data*	ltp	= &(decoder->Ltp_Data[ch]);

		if(!EnableDecodeCurrChannel(decoder,ch))
			continue;

		if(decoder->common_window)
			ics = &decoder->ICS_Data[0];
		else
			ics = &decoder->ICS_Data[ch];

#if SUPPORT_MUL_CHANNEL
		if(decoder->channelNum > 2)//multi channel
		{
			if(decoder->seletedChs==VO_CHANNEL_ALL)
			{
				ChanIndex = chOut + ch;
			}
			else
			{
				ChanIndex = decoder->seletedSBRChDecoded+ch;
			}
		}
		else
#endif//SUPPORT_MUL_CHANNEL
			ChanIndex = chOut + ch;

		if(decoder->ltp_coef[ChanIndex] == NULL)
		{
			decoder->ltp_coef[ChanIndex] = (int *)voAACDecAlignedMalloc(decoder->voMemop, MAX_SAMPLES*4*sizeof(int));
			if(decoder->ltp_coef[ChanIndex] == NULL)
			{
				return VO_ERR_OUTOF_MEMORY;
			}
		}
		
		if(ics->window_sequence!=EIGHT_SHORT_SEQUENCE&&ltp->data_present)
		{
			short*		sfb_offset;
			int*		lt_pred_stat	= (int*)decoder->ltp_coef[ChanIndex];
			int*		spec			=  decoder->coef[ch];

			t_est = decoder->t_est_buf;
			f_est = decoder->f_est_buf;
			
			sfb_offset = (short*)(sfBandTabLong + sfBandTabLongOffset[decoder->sampRateIdx]);
			
			coef = ltp_coef_C[ltp->coef];
			for(i=0;i<LTP_NUM_SAMPLE;i++)
			{
				t_est[i] =lt_pred_stat[LTP_NUM_SAMPLE + i - ltp->lag]*coef;
			}

			ltp_mdct(ics,t_est,f_est,decoder->prevWinShape[ChanIndex]);
			
			if(decoder->tns_data_present[ch])
			{
				tns_analysis_filter(decoder,ics,decoder->tns_data[ch],f_est);
			}
			
			for (sfb = 0; sfb < ltp->last_band; sfb++)
			{
				if (ltp->long_used[sfb])
				{
					int low  = sfb_offset[sfb];
					int high = sfb_offset[sfb+1];
					for (bin = low; bin < high; )
					{
						spec[bin] += f_est[bin]; bin++;
						spec[bin] += f_est[bin]; bin++;
						spec[bin] += f_est[bin]; bin++;
						spec[bin] += f_est[bin]; bin++;
					}
				}
			}
		}
		
	}
	return 0;
}

int ltp_update(AACDecoder* decoder,int channels)
{
	int ch,i;
	int a,b,c;
	int chOut, ChanIndex;

	chOut = decoder->decodedChans;

	if(decoder->profile==VOAAC_AAC_LTP)
	{
		for(ch=0;ch<channels;ch++)
		{
			int*		lt_pred_stat;
			int*		spec;
			int*		overlap;
			
			if(!EnableDecodeCurrChannel(decoder,ch))
				continue;

#if SUPPORT_MUL_CHANNEL
			if(decoder->channelNum > 2)//multi channel
			{
				if(decoder->seletedChs==VO_CHANNEL_ALL)
				{
					ChanIndex = chOut + ch;
				}
				else
				{
					ChanIndex = decoder->seletedSBRChDecoded+ch;
				}
			}
			else
#endif//SUPPORT_MUL_CHANNEL
				ChanIndex = chOut + ch;

			if(decoder->ltp_coef[ChanIndex] == NULL)
			{
				decoder->ltp_coef[ChanIndex] = (int *)voAACDecAlignedMalloc(decoder->voMemop, MAX_SAMPLES*4*sizeof(int));
				if(decoder->ltp_coef[ChanIndex] == NULL)
				{
					return VO_ERR_OUTOF_MEMORY;
				}
			}

			lt_pred_stat	=  (int*)decoder->ltp_coef[ChanIndex];
			spec			= (int*)( decoder->tmpBuffer+MAX_SAMPLES*ch);
			overlap			=  decoder->overlap[ChanIndex];

			if(lt_pred_stat == NULL || overlap == NULL)
				continue;

			for (i = 0; i < MAX_SAMPLES; i++)
			{
				a = lt_pred_stat[i + MAX_SAMPLES];
				b = (spec[i]>>SCLAE_IMDCT);
				c = (overlap[i]>>SCLAE_IMDCT);
				lt_pred_stat[i]						= a;
				lt_pred_stat[MAX_SAMPLES + i]      = b;
				lt_pred_stat[(MAX_SAMPLES* 2) + i]	= c;
			}			
		}
	}

	return 0;
}
#endif//LTP_DEC
