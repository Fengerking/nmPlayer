/************************************************************************
*									*									*
*		VisualOn, Inc. Confidential and Proprietary, 2004	*
*									*									*
************************************************************************/

#include "struct.h"
#include "decoder.h"

static int adif_frame(BitStream* bs,adif_header* adif)
{
	int i;

	adif->adif_id = (unsigned char)BitStreamGetBits(bs,32);
	adif->copyright_id_present = (unsigned char)BitStreamGetBit(bs);
	if(adif->copyright_id_present)
	{
		for (i = 0; i < 9; i++)
		{
			adif->copyright_id[i] = (unsigned char)BitStreamGetBits(bs, 8);
		}
	}
	adif->original_copy  = (unsigned char)BitStreamGetBit(bs);
	adif->home = (unsigned char)BitStreamGetBit(bs);
	adif->bitstream_type = (unsigned char)BitStreamGetBit(bs);
	adif->bitrate = (unsigned char)BitStreamGetBits(bs, 23);
	adif->num_program_config_elements = (unsigned char)BitStreamGetBits(bs, 4);

	for (i = 0; i < adif->num_program_config_elements + 1; i++)
	{
		if(adif->bitstream_type == 0)
		{
			adif->adif_buffer_fullness = BitStreamGetBits(bs, 20);
		} else {
			adif->adif_buffer_fullness = 0;
		}

		program_config_element(bs,&adif->pce[i]);
	}
	return 0;
}

#define CHECK_LEN_ADTSHEADER 3500

static int adts_frame(BitStream* bs,adts_header* adts, FrameStream* fStream, int nFrameNum, adts_header* adts_new)
{
	int i;
	unsigned char *buf = fStream->this_frame; 
	int sync_err = 1;
	int limit = fStream->length;
	unsigned char* bufk;

	if(limit <= 5)
		return VO_ERR_AAC_INVADTS;

	/* try to recover from sync errors */
	//if ((buf[0] & 0xFF) != 0xFF || (buf[1] & 0xF0) != 0xF0 )
	{
		int framelen = 0;
		int sampleIndex, profile, channel;
		int framebufferlen;

		bufk = buf;

		framebufferlen = 2048*(adts->channel_configuration?adts->channel_configuration:2);

		do {

			//buf += 1; 
			//limit -= 1;

			for (i = 0; i < limit - 1; i++) {
				if ( (buf[0] & 0xFF) == 0xFF && (buf[1] & 0xF0) == 0xF0 )
					break;

				buf++; 
				limit--;
				if (limit <= 5)
				{
					fStream->uselength += buf - fStream->this_frame;
					fStream->this_frame = buf;
					fStream->length = limit;
					return VO_ERR_AAC_INVADTS;
				}
			}


			if(limit <= 5)
				return VO_ERR_AAC_INVADTS;

			framelen = ((buf[3] & 0x3) << 11) + (buf[4] << 3) + (buf[5] >> 5);
			sampleIndex = (buf[2] >> 2) &0xF;
			profile = (buf[2] >> 6) + 1;
			channel = ((buf[2]&0x01) << 2) | (buf[3] >> 6);

			if(framelen > framebufferlen || sampleIndex >= NUM_SAMPLE_RATES)
			{
				buf++; 
				limit--;
				if (limit < 2)
				{
					fStream->uselength += buf - fStream->this_frame;
					fStream->this_frame = buf;
					fStream->length = limit;
					return VO_ERR_AAC_INVADTS;
				}
				continue;
			}
			else if(framelen == 0)
			{
				//update for live stream 20111024
				return VO_ERR_AAC_INVADTS;
			}

			if(framelen + 2 < limit)
			{
				if(buf[framelen] == 0xFF && (buf[framelen + 1] & 0xF0) == 0xF0)
				{
					sync_err = 0;
				}				
			}

			if (nFrameNum != 0)
			{
				if(channel == adts->channel_configuration && profile == adts->profile && sampleIndex == adts->sampling_frequency_index)
				{
					sync_err = 0;
					adts_new->channel_configuration = channel;
					adts_new->profile = profile;
					adts_new->sampling_frequency_index = sampleIndex;
				}
				else
				{		
					if(channel == adts_new->channel_configuration && profile == adts_new->profile && sampleIndex == adts_new->sampling_frequency_index)
					{
						//for changed bitstream, like channel changed, samplerate changed etc.
						sync_err = 0;					
					}
					else 
					{
						buf++; 
						limit--;
						sync_err = 1;

						adts_new->channel_configuration = channel;
						adts_new->profile = profile;
						adts_new->sampling_frequency_index = sampleIndex;

						if (limit < 2)
						{
							fStream->uselength += buf - fStream->this_frame;
							fStream->this_frame = buf;
							fStream->length = limit;
							return VO_ERR_AAC_INVADTS;
						}
					}
				}
			}
			else
			{
				sync_err = 0;
				adts_new->channel_configuration = channel;
				adts_new->profile = profile;
				adts_new->sampling_frequency_index = sampleIndex;
			}
		}while(sync_err);

		fStream->uselength += buf - fStream->this_frame;
		fStream->this_frame = buf;
		fStream->length = limit;

		BitStreamInit(bs, limit, buf);
	}

	adts->syncword = (unsigned short)BitStreamGetBits(bs, 12);	
	adts->ID = (unsigned char)BitStreamGetBit(bs);
	adts->layer = (unsigned char)BitStreamGetBits(bs, 2);
	adts->protection_absent = (unsigned char)BitStreamGetBit(bs);
	adts->profile = (unsigned char)BitStreamGetBits(bs, 2) + 1;
	adts->sampling_frequency_index = (unsigned char)BitStreamGetBits(bs, 4);
	adts->private_bit = (unsigned char)BitStreamGetBit(bs);
	adts->channel_configuration = (unsigned char)BitStreamGetBits(bs, 3);
	adts->original = (unsigned char)BitStreamGetBit(bs);
	adts->home = (unsigned char)BitStreamGetBit(bs);

	adts->copyright_identification_bit = (unsigned char)BitStreamGetBit(bs);
	adts->copyright_identification_start = (unsigned char)BitStreamGetBit(bs);
	adts->frame_length = (unsigned short)BitStreamGetBits(bs, 13);
	adts->adts_buffer_fullness = (unsigned short)BitStreamGetBits(bs, 11);
	adts->number_of_raw_data_blocks_in_frame = (unsigned char)BitStreamGetBits(bs, 2);

	if(adts->protection_absent == 0)
	{
		adts->crc_check = (unsigned short)BitStreamGetBits(bs, 16);
	}

	BitStreamByteAlign(bs);



	return 0;
}

int ParseADIFHeader(AACDecoder* decoder, BitStream*	bs)
{
	int err,channels,sampleRateIdx,profile;
	adif_header *adif = &decoder->adif;
	program_config* pce;

	err = adif_frame(bs,adif);
	if (err)
		return err;

	decoder->pce_set = 1;
	pce = &adif->pce[0];
	profile = pce->object_type + 1;
	sampleRateIdx = pce->sampling_frequency_index;
	channels = pce->channels;

	if(channels <= 0 || channels > MAX_CHANNELS)
		return VO_ERR_AUDIO_UNSCHANNEL;

	decoder->ChansMode = VO_AUDIO_CHANNEL_STEREO;
	if(channels == 1)
		decoder->ChansMode = VO_AUDIO_CHANNEL_MONO;
	else if(channels > 2)
		decoder->ChansMode = VO_AUDIO_CHANNEL_MULTICH;

	//LG #8606 support 3~5 channels
	if (channels > 2 && channels <= 6)
	{
		channels = 6;
	}

	if(pce->num_front_channel_elements == 2 && !pce->front_element_is_cpe[0]
	&& !pce->front_element_is_cpe[1] && pce->channels == 2)
	{
		decoder->ChansMode = VO_AUDIO_CHANNEL_DUALMONO;
	}

	err = updateProfile(decoder, profile);
	if (err) return err;		

	if(sampleRateIdx >= NUM_SAMPLE_RATES)
		return VO_ERR_AUDIO_UNSSAMPLERATE;

	decoder->sampleRate = sampRateTab[sampleRateIdx];
	decoder->sampRateIdx = sampleRateIdx;
	decoder->channelNum = channels;
	decoder->frametype = VOAAC_ADIF;

	return 0;
}

int ParseADTSHeader(AACDecoder* decoder)
{
	int nFrameNum;
	int err,channels,sampleRateIdx,profile, chconfig;
	unsigned char* buf;
	BitStream*	bs = &decoder->bs;
	adts_header *adts = &decoder->adts;
	FrameStream* fStream = &decoder->Fstream;

	buf = decoder->Fstream.this_frame;
	nFrameNum = decoder->decoderNum;

	//ISO/IEC 14496-3:2005(E) 1.A.4.3 Audio Data Transport Stream (ADTS)
	err = adts_frame(bs,adts, fStream, nFrameNum, &decoder->adts_new);
	if (err)
	{
		if(fStream->length > 5)
		{
			fStream->uselength += fStream->length - 2;
			fStream->this_frame	= fStream->this_frame + fStream->length - 2;
			fStream->length = 2;
		}

		return err;
	}

	if(adts->frame_length > 2048*(adts->channel_configuration?adts->channel_configuration:2))
	{
		if(fStream->length >= 1)
		{
			fStream->uselength += 1;
			fStream->this_frame	= fStream->this_frame + 1;
			fStream->length -= 1;
		}

		return VO_ERR_AAC_INVADTS;
	}

	profile = adts->profile;
	sampleRateIdx = adts->sampling_frequency_index;
	chconfig = adts->channel_configuration;

	//in raw_data_block() or implicitly (see ISO/IEC13818-7)
	channels = chconfig;
	if(chconfig == 7)
		channels = 8;

	decoder->ChansMode = VO_AUDIO_CHANNEL_STEREO;
	if(chconfig==0)
	{
		decoder->ChansMode = VO_AUDIO_CHANNEL_DUALMONO;
		channels = 2;
	}
	else if(chconfig == 1) 
		decoder->ChansMode = VO_AUDIO_CHANNEL_MONO;
	else if(chconfig > 2)
		decoder->ChansMode = VO_AUDIO_CHANNEL_MULTICH;

	//LG #8606 support 3~5 channels
	if (chconfig > 2 && chconfig <= 6)
	{
		channels = 6;
	}

	if(sampleRateIdx >= NUM_SAMPLE_RATES)
		return VO_ERR_AUDIO_UNSSAMPLERATE;

	decoder->sampleRate = sampRateTab[sampleRateIdx];
	decoder->sampRateIdx = sampleRateIdx;
	decoder->channelNum = channels;
	decoder->frametype = VOAAC_ADTS;

	if(adts->frame_length != 0)
	{
		int length = bs->nBytes + 7 + (bs->cachedBits >>3);
		if(adts->protection_absent == 0)
			length += 2;
		decoder->frame_length = adts->frame_length;
		if(length < adts->frame_length)
		{
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
	}

	if(profile != VOAAC_AAC_LC 
#ifdef LTP_DEC
		&& profile != VOAAC_AAC_LTP 
#endif
#ifdef MAIN_DEC
		&& profile != VOAAC_AAC_MAIN
#endif
		)
	{
		decoder->profilenumber++;
		if(decoder->profilenumber > CHECK_PROFILE)
			return VO_ERR_AAC_UNSPROFILE; 
	}
	else
	{
		decoder->profilenumber = 0;
	}

	decoder->profile = profile;

	Channelconfig(decoder);

	return 0;
}

int program_config_element(BitStream *bs,program_config* pce)
{
	int i,channels;

	channels = 0;
	pce->element_instance_tag = (unsigned char)BitStreamGetBits(bs, 4);

	pce->object_type = (unsigned char)BitStreamGetBits(bs, 2);
	pce->sampling_frequency_index = (unsigned char)BitStreamGetBits(bs, 4);
	pce->num_front_channel_elements = (unsigned char)BitStreamGetBits(bs, 4);
	pce->num_side_channel_elements = (unsigned char)BitStreamGetBits(bs, 4);
	pce->num_back_channel_elements = (unsigned char)BitStreamGetBits(bs, 4);
	pce->num_lfe_channel_elements = (unsigned char)BitStreamGetBits(bs, 2);
	pce->num_assoc_data_elements = (unsigned char)BitStreamGetBits(bs, 3);
	pce->num_valid_cc_elements = (unsigned char)BitStreamGetBits(bs, 4);

	pce->mono_mixdown_present = (unsigned char)BitStreamGetBit(bs);
	if (pce->mono_mixdown_present == 1)
	{
		pce->mono_mixdown_element_number = (unsigned char)BitStreamGetBits(bs, 4);
	}

	pce->stereo_mixdown_present = (unsigned char)BitStreamGetBit(bs);
	if (pce->stereo_mixdown_present == 1)
	{
		pce->stereo_mixdown_element_number = (unsigned char)BitStreamGetBits(bs, 4);
	}

	pce->matrix_mixdown_idx_present = (unsigned char)BitStreamGetBit(bs);
	if (pce->matrix_mixdown_idx_present == 1)
	{
		pce->matrix_mixdown_idx = (unsigned char)BitStreamGetBits(bs, 2);
		pce->pseudo_surround_enable = (unsigned char)BitStreamGetBit(bs);
	}

	pce->num_front_channels = 0;
	for (i = 0; i < pce->num_front_channel_elements; i++)
	{
		pce->front_element_is_cpe[i] = (unsigned char)BitStreamGetBit(bs);
		pce->front_element_tag_select[i] = (unsigned char)BitStreamGetBits(bs, 4);

		if (pce->front_element_is_cpe[i] & 1)
		{            
			pce->num_front_channels += 2;
			channels += 2;
		} 
		else 
		{          
			pce->num_front_channels += 1;
			channels++;
		}
	}

	pce->num_side_channels = 0;
	for (i = 0; i < pce->num_side_channel_elements; i++)
	{
		pce->side_element_is_cpe[i] = (unsigned char)BitStreamGetBit(bs);
		pce->side_element_tag_select[i] = (unsigned char)BitStreamGetBits(bs, 4);

		if (pce->side_element_is_cpe[i] & 1)
		{            
			pce->num_side_channels += 2;
			channels += 2;
		} 
		else 
		{			
			pce->num_side_channels += 1;
			channels++;
		}
	}

	pce->num_back_channels = 0;
	for (i = 0; i < pce->num_back_channel_elements; i++)
	{
		pce->back_element_is_cpe[i] = (unsigned char)BitStreamGetBit(bs);
		pce->back_element_tag_select[i] = (unsigned char)BitStreamGetBits(bs, 4);
		if (pce->back_element_is_cpe[i] & 1)
		{            
			pce->num_back_channels += 2;
			channels += 2;
		} 
		else 
		{			
			pce->num_back_channels += 1;
			channels++;
		}
	}

	pce->num_lfe_channels = 0;
	for (i = 0; i < pce->num_lfe_channel_elements; i++)
	{
		pce->lfe_element_tag_select[i] = (unsigned char)BitStreamGetBits(bs, 4);
		pce->num_lfe_channels += 1;
		channels++;
	}

	for (i = 0; i < pce->num_assoc_data_elements; i++)
		pce->assoc_data_element_tag_select[i] = (unsigned char)BitStreamGetBits(bs, 4);

	for (i = 0; i < pce->num_valid_cc_elements; i++)
	{
		pce->cc_element_is_ind_sw[i] = (unsigned char)BitStreamGetBit(bs);
		pce->valid_cc_element_tag_select[i] = (unsigned char)BitStreamGetBits(bs, 4);
	}

	pce->channels = channels;
	BitStreamByteAlign(bs);

	//comment_field_bytes
	i = BitStreamGetBits(bs, 8);
	while (i--)
		BitStreamGetBits(bs, 8);

	return 0 ;
}
