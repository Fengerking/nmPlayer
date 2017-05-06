	/************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#include "struct.h"
#include "volc_huff.h"
#include "sbr_dec.h"
#include "decoder.h"

const int sfBandTabLongNum[NUM_SAMPLE_RATES] = {
	42, 42, 48, 50, 50, 52, 
	48, 48, 44, 44, 44, 41
};

const int vosfBandTabShortNum[NUM_SAMPLE_RATES] = {
	13, 13, 13, 15, 15, 15, 
	16, 16, 16, 16, 16, 16
};

int ltp_data(AACDecoder* decoder,ICS_Data *ics,LTP_Data* ltp)
{
	int sfb, w;
	BitStream *bs = &decoder->bs;
	ltp->lag =  (VO_U16)BitStreamGetBits(bs, 11);


	/* Check length of lag */
	if (ltp->lag > MAX_SAMPLES*2)
	{
		ltp->lag = MAX_SAMPLES*2;
	}

	ltp->coef = (VO_U8)BitStreamGetBits(bs, 3);

	if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
	{
		for (w = 0; w < ics->num_window_groups; w++)
		{
			ltp->short_used[w] = (VO_U8)BitStreamGetBits(bs, 1);
			if (ltp->short_used[w])
			{
				ltp->short_lag_present[w] = (VO_U8)BitStreamGetBits(bs, 1);
				if (ltp->short_lag_present[w])
				{
					ltp->short_lag[w] = (VO_U8)BitStreamGetBits(bs, 4);
				}
			}
		}
	} 
	else 
	{
		ltp->last_band = (ics->max_sfb < MAX_LTP_SFB ? ics->max_sfb : MAX_LTP_SFB);

		for (sfb = 0; sfb < ltp->last_band; sfb++)
		{
			ltp->long_used[sfb] = (VO_U8)BitStreamGetBits(bs, 1);
		}
	}

	return 0;
}

static int ics_info(AACDecoder* decoder,ICS_Data *ics, int sampRateIdx)
{
	int sfb, g, bitset, limit;

	BitStream *bs = &decoder->bs;
	ics->ics_reserved_bit =   (VO_U8)BitStreamGetBits(bs, 1);
	ics->window_sequence =    (VO_U8)BitStreamGetBits(bs, 2);
	ics->window_shape =       (VO_U8)BitStreamGetBits(bs, 1);
	if (ics->window_sequence == EIGHT_SHORT_SEQUENCE) {

		ics->max_sfb =		  (VO_U8)BitStreamGetBits(bs, 4);
		if(ics->max_sfb > vosfBandTabShortNum[decoder->sampRateIdx] - 1)
			ics->max_sfb = vosfBandTabShortNum[decoder->sampRateIdx] - 1;
		ics->scale_factor_grouping =    (VO_U8)BitStreamGetBits(bs, 7);
		ics->num_window_groups =    1;
		ics->window_group_length[0] = 1;
		bitset = 0x40;	
		for (g = 0; g < 7; g++) {
			if (ics->scale_factor_grouping & bitset)	{ 
				ics->window_group_length[ics->num_window_groups - 1]++;
			} else { 
				ics->num_window_groups++; 
				ics->window_group_length[ics->num_window_groups - 1] = 1; 
			}
			bitset >>= 1;
		}
	} else {
#ifdef LTP_DEC
		LTP_Data* left	=decoder->Ltp_Data;
		LTP_Data* right	=decoder->Ltp_Data+1;
		left->data_present  = 0;
		right->data_present = 0;
#endif
		ics->max_sfb =               (VO_U8)BitStreamGetBits(bs, 6);
		if(ics->max_sfb > sfBandTabLongNum[decoder->sampRateIdx] - 1)
			ics->max_sfb = sfBandTabLongNum[decoder->sampRateIdx] - 1;
		ics->predictor_data_present = (VO_U8)BitStreamGetBits(bs, 1);

		if(ics->predictor_data_present) 
		{			
			if(decoder->profile==VOAAC_AAC_MAIN)
			{
				limit =  MIN(ics->max_sfb, predSFBMax[sampRateIdx]);
				
				ics->predictor_reset = (VO_U8)BitStreamGetBits(bs, 1);
				if(ics->predictor_reset)
				{
					ics->predictor_reset_group_number = (VO_U8)BitStreamGetBits(bs, 5);
				}
				
				for (sfb = 0; sfb < limit; sfb++)
				{
					ics->prediction_used[sfb] = (VO_U8)BitStreamGetBits(bs, 1);
				}
			}
			else			
			{
#ifdef LTP_DEC
				if(decoder->t_est_buf == NULL)
				{
					decoder->t_est_buf = (int *)voAACDecAlignedMalloc(decoder->voMemop, MAX_SAMPLES*2*sizeof(int));
					if(decoder->t_est_buf == NULL)
					{
						return VO_ERR_OUTOF_MEMORY;
					}
				}
				
				if(decoder->f_est_buf == NULL)
				{
					decoder->f_est_buf = (int *)voAACDecAlignedMalloc(decoder->voMemop, MAX_SAMPLES*2*sizeof(int));
					if(decoder->f_est_buf == NULL)
					{
						return VO_ERR_OUTOF_MEMORY;
					}
				}

				left->data_present = (VO_U8)BitStreamGetBits(bs, 1);
				if(left->data_present)
				{
					ltp_data(decoder,ics,left);
				}
				
				if (decoder->common_window&&(right->data_present=(VO_U8)BitStreamGetBits(bs, 1)))
				{
					ltp_data(decoder,ics,right);
				}
#else
				return VO_ERR_AUDIO_UNSFEATURE;
#endif
			}

		}

		ics->num_window_groups = 1;
		ics->window_group_length[0] = 1;
	}

	return 0;
}
//Table 4.46 ¨C Syntax of section_data()
static int section_data(AACDecoder* decoder,ICS_Data *ics, int ch)
{
	int	g, sect_cb, sect_esc_val,sect_len, sect_len_bits,sfb;
	int  num_window_groups	= ics->num_window_groups;
	int	max_sfb	= ics->max_sfb;
	unsigned char *sfb_cb = decoder->sfb_cb[ch];
	BitStream *bs = &decoder->bs;
	sect_len_bits = (ics->window_sequence == EIGHT_SHORT_SEQUENCE ? 3 : 5);
	sect_esc_val = (1 << sect_len_bits) - 1;

	for (g = 0; g <  num_window_groups; g++) {
		int sect_len_incr;
		int k = 0;
		int i = 0;
		while (k < max_sfb) {
			sect_cb = BitStreamGetBits(bs, 4);	
			sect_len = 0;
			sect_len_incr = BitStreamGetBits(bs, sect_len_bits);
			while (sect_len_incr == sect_esc_val)
			{
				sect_len += sect_esc_val;
				sect_len_incr = BitStreamGetBits(bs, sect_len_bits);
			}
			sect_len += sect_len_incr;	
			
			if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
            {
                if (k + sect_len > 8*15)
                    return -1;
                if (i >= 8*15)
                    return -1;
            } else {
                if (k + sect_len > 51)
                    return -1;
                if (i >= 51)
                    return -1;
            }
			
			for ( sfb = 0; sfb < sect_len; sfb++ ) 
				sfb_cb[sfb] = sect_cb;
 			sfb_cb += sect_len;
			k += sect_len;
			i++;
		}
#if ERROR_CHECK
		if(k != max_sfb)
		{	
			return -1;
		}			
#endif
	}

	return 0;
}

#define NOISE_OFFSET (90+256) //for PNS
static int scale_factor_data(AACDecoder* decoder,ICS_Data *ics, int ch)
{
	int g, cb, noise_energy, noise_pcm_flag, val, scalefactor, is_position,total;
	int max_sfb    = ics->max_sfb;
	BitStream *bs = &decoder->bs;
	int global_gain = decoder->global_gain;
	unsigned char *sfb_cb = decoder->sfb_cb[ch];
	short *scaleFactors = decoder->scaleFactors[ch];
	g = 0;
	is_position = 0;
	noise_pcm_flag = 1;
	scalefactor = global_gain;	
	total = ics->num_window_groups * max_sfb;
	noise_energy = global_gain - NOISE_OFFSET;

	while(g++ < total)
	{
		cb = *sfb_cb++;
		switch (cb)
		{
		case ZERO_HCB:
			*scaleFactors++ = 0;
			break;

		case NOISE_HCB:			
			if (noise_pcm_flag) {
				val = BitStreamGetBits(bs, 9);
				noise_pcm_flag = 0;
			} else {
				val = DecodeHuffmanData(&huffTabScaleInfo, bs);
			}
			noise_energy += val;
			*scaleFactors++ = (short)noise_energy;			
			break;

		case INTENSITY_HCB: 
		case INTENSITY_HCB2:
			val = DecodeHuffmanData(&huffTabScaleInfo, bs);
			is_position += val;
			*scaleFactors++ = is_position;			
			break;
		
		default: /* spectral books */
			val = DecodeHuffmanData(&huffTabScaleInfo, bs);
			scalefactor += val;

#if ERROR_CHECK
			if (scalefactor < 0)
			{
				//error(decoder,"ERR_INVALID_LC_BITSTREAM sf error\n",ERR_INVALID_LC_BITSTREAM);
				//scalefactor = 140;
				return -1;
			}				
#endif			
			*scaleFactors++ = scalefactor;
			break;
		}
	}

	return 0;
}


static void pulse_data(AACDecoder* decoder,Pulse_Data *pi)
{
	int i;
	BitStream *bs = &decoder->bs;
	pi->number_pulse = (VO_U8)BitStreamGetBits(bs, 2);		
	pi->pulse_start_sfb = (VO_U8)BitStreamGetBits(bs, 6);
	//if(pi->pulse_start_sfb>)
	for (i = 0; i < pi->number_pulse+1; i++) {
		pi->pulse_offset[i] = (VO_U8)BitStreamGetBits(bs, 5);
		pi->pulse_amp[i] = (VO_U8)BitStreamGetBits(bs, 4);
	}
}

// tns, tns->coef
void tns_data(AACDecoder* decoder,BitStream *bs, int window_sequence, int ch)
{
	int i, w, filt, n_filt,coef_compress,start_coef_bits,coef_bits,order;
	int n_filt_bits = 2;
    int length_bits = 6;
    int order_bits = 5;
	int	num_windows = 1;
	unsigned char *coef;
	TNS_Data *tns = decoder->tns_data[ch];
    if (window_sequence == EIGHT_SHORT_SEQUENCE)
    {
        n_filt_bits = 1;
        length_bits = 4;
        order_bits = 3;
		num_windows = 8;
    }
	for (w = 0; w < num_windows; w++)
    {
        n_filt = tns->n_filt = (VO_U8)BitStreamGetBits(bs, n_filt_bits);
		
        if (n_filt)
        {
            tns->coef_res = (VO_U8)BitStreamGetBits(bs,1);
            if (tns->coef_res)
            {
                start_coef_bits = 4;
            } 
			else 
			{
                start_coef_bits = 3;
            }
        }
		
        for (filt = 0; filt < n_filt; filt++)
        {
            tns->length[filt] = (VO_U8)BitStreamGetBits(bs, length_bits);
            order = tns->order[filt]  = (VO_U8)BitStreamGetBits(bs, order_bits);
            if (tns->order[filt])
            {
                tns->direction[filt] = (VO_U8)BitStreamGetBits(bs,1);
                coef_compress = tns->coef_compress[filt] =(VO_U8)BitStreamGetBits(bs,1);
				
                coef_bits = start_coef_bits - coef_compress;
                coef = tns->coef[filt];
				for (i = 0; i < order; i++)
                {
                    *coef++ = (unsigned char)BitStreamGetBits(bs, coef_bits);
                }
            }
        }

		tns++;
    }	
}


//Table 4.44 ¨C Syntax of individual_channel_stream()
static int individual_channel_stream(AACDecoder* decoder,int ch)
{
	int err;
	ICS_Data *ics;
	Pulse_Data *pulse;
	BitStream *bs = &decoder->bs;
	
	decoder->global_gain = BitStreamGetBits(bs, 8);
	
	//if(decoder->sampleBits == 32)
	//	decoder->global_gain -= 32;

	if (!decoder->common_window)
	{
		ics = &(decoder->ICS_Data[ch]);
		if(ics_info(decoder,ics, decoder->sampRateIdx) < 0)
			return VO_ERR_AAC_INVSTREAM;
	}
	else
	{
		ics = &(decoder->ICS_Data[0]);
	}
	
	if(section_data(decoder,ics, ch) < 0)
		return VO_ERR_AAC_INVSTREAM;
	
	if(scale_factor_data(decoder,ics, ch) < 0) 
		return VO_ERR_AAC_INVSTREAM;
		
	
	pulse = &decoder->pulse_data[ch];
	decoder->pulse_data_present[ch] = BitStreamGetBits(bs, 1);
	if (decoder->pulse_data_present[ch])
		pulse_data(decoder,pulse);
	
	decoder->tns_data_present[ch] = BitStreamGetBits(bs, 1);
	if (decoder->tns_data_present[ch])
	{
		tns_data(decoder,bs, ics->window_sequence,ch);
	}
	//gain control tool is used for the scalable sampling rate (SSR) audio object type only
	if (BitStreamGetBits(bs, 1))
	{
		//MESSAGE("gain control tool is used for the scalable sampling rate (SSR) audio object type only\n");	
		return VO_ERR_AUDIO_UNSFEATURE;
	}
	
	err = spectral_data(decoder,ics,ch);
	if(err)
		return err;	

	return 0;

}

static int single_channel_element(AACDecoder* decoder)
{
	int err;
	BitStream *bs = &decoder->bs;
	BitStreamGetBits(bs, 4);//element_instance_tag
	err = individual_channel_stream(decoder,0);
	return err;
}


static int channel_pair_element(AACDecoder* decoder)
{
	int err;
	int sfb, g;
	unsigned char* ms_used;
	ICS_Data *ics;

	BitStream *bs = &decoder->bs;
	BitStreamGetBits(bs, 4);//element_instance_tag
	decoder->common_window = BitStreamGetBits(bs, 1);
	if (decoder->common_window) {
		ics = &decoder->ICS_Data[0];
		if(ics_info(decoder,ics, decoder->sampRateIdx) < 0)
			return VO_ERR_AAC_INVSTREAM;
		decoder->ms_mask_present = BitStreamGetBits(bs, 2);		
		
		if(decoder->ms_mask_present == 3)
			return VO_ERR_AUDIO_UNSFEATURE;

		if (decoder->ms_mask_present == 1) {			
			for (g = 0; g < ics->num_window_groups; g++) {
				ms_used = decoder->ms_used[g];
				for (sfb = 0; sfb < ics->max_sfb; sfb++) {
					ms_used[sfb] = (unsigned char)BitStreamGetBits(bs, 1);					
				}		
			}
		}		
	}
	err = individual_channel_stream(decoder,0);
	if (err)
	{
		return err;
	}
	err = individual_channel_stream(decoder,1);
	if (err) 
	{
		return err;
	}

	return 0;
}

static int data_stream_element(AACDecoder* decoder)
{
	unsigned int data_byte_align_flag, count,i;
	unsigned char data_stream_byte[560];

	BitStream *bs = &decoder->bs;

	BitStreamGetBits(bs, 4);//element_instance_tag
	data_byte_align_flag = BitStreamGetBits(bs, 1);
	count = BitStreamGetBits(bs, 8);
	if (count == 255)
		count += BitStreamGetBits(bs, 8);

	if (data_byte_align_flag)
		BitStreamByteAlign(bs);

	for (i = 0; i < count; i++)
		data_stream_byte[i] = (unsigned char)BitStreamGetBits(bs, 8);

	return 0;
}


static int fill_element(AACDecoder* decoder)
{
	unsigned int fillCount;
	unsigned char *fillBuf;

	BitStream *bs = &decoder->bs;

	fillCount = BitStreamGetBits(bs, 4);
	if (fillCount == 15)
		fillCount += (BitStreamGetBits(bs, 8) - 1);

	decoder->fillCount = fillCount;
	fillBuf = decoder->fillBuf;
	while (fillCount-->0)
		*fillBuf++ = (unsigned char)BitStreamGetBits(bs, 8);

	decoder->fillExtType = 0;

#ifdef SBR_DEC
	/* check for SBR 
	 * decoder->sbrEnabled is sticky (reset each raw_data_block), so for multichannel 
	 *    need to verify that all SCE/CPE/ICCE have valid SBR fill element following, and 
	 *    must upsample by 2 for LFE
	 */
	if (decoder->fillCount > 0&&decoder->disableSBR==0) {
		decoder->fillExtType = (int)((decoder->fillBuf[0] >> 4) & 0x0f);
		if (decoder->fillExtType == SBR_EXTENSION || decoder->fillExtType == SBR_EXTENSION_CRC)
		{
			decoder->sbrEnabled = 1;
			if(decoder->sbr==0&&sbr_init(decoder))
			{
				//error(decoder,"sbr is not initialized !",ERR_FAIL_DECODE_SBR);
				decoder->disableSBR = 1;
				return VO_ERR_AAC_FAILDECSBR;
			}
		}
	}

#endif

	return 0;
}

//Table 4.3 ¨C Syntax of top level payload for audio object types AAC Main, SSR, LC, and LTP
int raw_data_block(AACDecoder* decoder)
{
	int err=0;
	BitStream *bs = &decoder->bs;

	decoder->old_id_syn_ele = decoder->id_syn_ele;
	decoder->id_syn_ele = BitStreamGetBits(bs, 3);

	if(decoder->old_id_syn_ele == -1)
		decoder->first_id_syn_ele = decoder->id_syn_ele;

	decoder->common_window = 0;
    decoder->elementChans = elementNumChans[decoder->id_syn_ele];
	
	switch (decoder->id_syn_ele) {
	case ID_SCE:
		err = single_channel_element(decoder);
		break;
	case ID_CPE:
		err = channel_pair_element(decoder);
		if(err == 0)
		{
			if(decoder->channelNum < 2)
				decoder->channelNum = 2;
		}
		break;
	case ID_CCE:
		//error(decoder,"this decoder does not support ID_CCE",ERR_INVALID_LC_BITSTREAM);
		err = VO_ERR_AUDIO_UNSFEATURE;
		break;
	case ID_LFE:
		err = single_channel_element(decoder);
		break;
	case ID_DSE:
		err = data_stream_element(decoder);
		break;
	case ID_PCE:
		{
			program_config *pce = &decoder->pce;
			if(decoder->old_id_syn_ele != -1)
				return VO_ERR_AAC_INVSTREAM;

			decoder->pce_set = 1;
			program_config_element(bs,pce);

			if(pce->num_front_channel_elements == 2 && !pce->front_element_is_cpe[0]
			&& !pce->front_element_is_cpe[1] && pce->channels == 2)
			{
				decoder->channelNum = pce->channels;
				decoder->ChansMode = VO_AUDIO_CHANNEL_DUALMONO;
			}
			
			if((decoder->channelNum!=0&&pce->channels!=decoder->channelNum)
				||(decoder->sampRateIdx!=0&&pce->sampling_frequency_index!=decoder->sampRateIdx))
			{
				//error(decoder,"bitstream error,not correct PCE",ERR_INVALID_LC_BITSTREAM);
				pce->channels = decoder->channelNum;
				return VO_ERR_AAC_INVSTREAM;
			}

			if(pce->sampling_frequency_index >= NUM_SAMPLE_RATES)
				return VO_ERR_AAC_INVSTREAM;

			decoder->sampleRate = sampRateTab[pce->sampling_frequency_index];
			decoder->sampRateIdx = pce->sampling_frequency_index;
			decoder->channelNum = pce->channels;
			decoder->profile = pce->object_type + 1;

			Channelconfig(decoder);

			break;
		}
		
	case ID_FIL:
		err = fill_element(decoder);
		break;
	case ID_END:
		break;
	default:
		//error(decoder,"Invalid id_syn_ele in raw_data_block()",ERR_INVALID_LC_BITSTREAM);
		break;
	}
	if (err)
		return err;

	return 0;
}
