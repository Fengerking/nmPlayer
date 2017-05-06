#include "struct.h"

int error(AACDecoder *decoder,char *text, int code)
{
	return 0;
}

int voSignedDivide(AACDecoder* decoder,int divisor,int dividend)
{
	if(dividend==0)
	{	
		return 0x7FFFFFFF;
	}
#ifdef ARMv4Comipler
	return  signed_divid_arm(divisor,dividend);
#else
	return  divisor/dividend;
#endif
}
unsigned int voUnsignedDivide(AACDecoder* decoder,unsigned int divisor,unsigned int dividend)
{
	if(dividend==0)
	{	
		return 0x7FFFFFFF;
	}
#ifdef ARMv4Comipler
	return  unsigned_divid_arm(divisor,dividend);
#else
	return  divisor/dividend;
#endif
}

int updateSampleRate(AACDecoder *decoder,int sampleRate)
{
	int idx;
	for (idx = 0; idx < NUM_SAMPLE_RATES; idx++) {
		if (sampleRate == sampRateTab[idx]) {
			decoder->sampRateIdx = idx;
			break;
		}
	}

	if (idx == NUM_SAMPLE_RATES)
		return VO_ERR_AUDIO_UNSSAMPLERATE;

	decoder->sampleRate = sampleRate;
	return VO_ERR_NONE;
}


int updateProfile(AACDecoder *decoder, int profile)
{
	if(!(profile == VOAAC_AAC_LC 
		|| profile == VOAAC_ER_AAC_LC 
#ifdef LTP_DEC
		|| profile == VOAAC_AAC_LTP 
		|| profile == VOAAC_ER_AAC_LTP
#endif
#ifdef MAIN_DEC
		|| profile == VOAAC_AAC_MAIN 
#endif 
#ifdef SBR_DEC
		|| profile == VOAAC_SBR
#endif
#ifdef PS_DEC
		|| profile == VOAAC_HE_PS
#endif
#ifdef BSAC_DEC
		|| profile == VOAAC_ER_BSAC
#endif
		))
		return VO_ERR_AAC_UNSPROFILE; 

	decoder->profile = profile;
	return VO_ERR_NONE;
}

int EnableDecodeCurrChannel(AACDecoder *decoder,int ch)
{
	int nch;
	int *channel_position = decoder->channel_position;
	int *channel_offsize = decoder->channel_offsize;

	nch = channel_offsize[ch + decoder->decodedChans];

	if(decoder->seletedChs&channel_position[nch])
	{
		return 1;
	}
	else
	{	
		return 0;
	}
}

int EnableDecodeCurrSBRChannel(AACDecoder *decoder,int ch)
{
	int nch;
	int *channel_position = decoder->channel_position;
	int *channel_offsize = decoder->channel_offsize;

	nch = channel_offsize[ch + decoder->decodedSBRChans];

	if(decoder->seletedChs&channel_position[nch])
	{
		return 1;
	}
	else
	{	
		return 0;
	}
}

void UpdateSeletedChDecoded(AACDecoder *decoder,int channels)
{
	int ch;
	if(decoder->channelNum>2 || decoder->ChansMode == VO_AUDIO_CHANNEL_DUALMONO)
	{
		for(ch=0;ch<channels;ch++)
		{
			decoder->seletedChDecoded+=EnableDecodeCurrChannel(decoder,ch);
		}
	}
}

void UpdateSeletedSBRChDecoded(AACDecoder *decoder,int channels)
{
	if(decoder->channelNum>2 || decoder->ChansMode == VO_AUDIO_CHANNEL_DUALMONO)
	{
		int ch;
		for(ch=0;ch<channels;ch++)
		{
			decoder->seletedSBRChDecoded+=EnableDecodeCurrSBRChannel(decoder,ch);
		}
	}
}

 void Channelconfig(AACDecoder *decoder)
{
	int *channel_position;
	int *channel_offsize;
	
	channel_position = decoder->channel_position;
	channel_offsize = decoder->channel_offsize;

	if(decoder->channelNum <= 2)
	{
		channel_position[0] = VO_CHANNEL_FRONT_LEFT;
		channel_position[1] = VO_CHANNEL_FRONT_RIGHT;

		channel_offsize[0] = 0;
		channel_offsize[1] = 1;

		return;
	}
#ifdef MSORDER
	    /* check if there is a PCE */
    if (decoder->pce_set)
    {
        int i, chpos = 0;
        int chdir, back_center = 0;
		program_config *pce = &decoder->pce;

		chdir = pce->num_front_channels;	

        if (chdir == 3)
        {
			if(pce->front_element_is_cpe[0] & 1)
			{
				channel_offsize[chpos] = 0;            
				channel_position[chpos++] = VO_CHANNEL_FRONT_LEFT;			
				channel_offsize[chpos] = 1;
				channel_position[chpos++] = VO_CHANNEL_FRONT_RIGHT;
				channel_offsize[chpos] = 2;
				channel_position[chpos++] = VO_CHANNEL_CENTER;
			}
			else
			{
				channel_offsize[chpos] = 2;            
				channel_position[chpos++] = VO_CHANNEL_FRONT_LEFT;			
				channel_offsize[chpos] = 0;
				channel_position[chpos++] = VO_CHANNEL_FRONT_RIGHT;
				channel_offsize[chpos] = 1;
				channel_position[chpos++] = VO_CHANNEL_CENTER;
			}			
        }
		else if(chdir == 2)
		{
			channel_offsize[chpos] = chpos;            
			channel_position[chpos++] = VO_CHANNEL_FRONT_LEFT;			
			channel_offsize[chpos] = chpos;
            channel_position[chpos++] = VO_CHANNEL_FRONT_RIGHT;
		}
		else if(chdir == 1)
		{
			channel_offsize[chpos] = chpos;
			channel_position[chpos++] = VO_CHANNEL_CENTER;
		}

		for (i = 0; i < pce->num_lfe_channels; i++)
        {
			channel_offsize[chpos] = chpos;            
			channel_position[chpos++] = VO_CHANNEL_LFE_BASS;
        } 

		for (i = 0; i < pce->num_side_channels; i += 2)
        {
			channel_offsize[chpos] = chpos;            
			channel_position[chpos++] = VO_CHANNEL_SIDE_LEFT;
			channel_offsize[chpos] = chpos;
            channel_position[chpos++] = VO_CHANNEL_SIDE_RIGHT;
        }

		chdir = pce->num_back_channels;
        if (chdir & 1)
        {
            back_center = 1;
            chdir--;
        }
        for (i = 0; i < chdir; i += 2)
        {
			channel_offsize[chpos] = chpos;            
			channel_position[chpos++] = VO_CHANNEL_BACK_LEFT;
			channel_offsize[chpos] = chpos;
            channel_position[chpos++] = VO_CHANNEL_BACK_RIGHT;
        }

        if (back_center)
        {
			channel_offsize[chpos] = chpos;
            channel_position[chpos++] = VO_CHANNEL_BACK_CENTER;
        }
    } 
	else 
	{
		int config;
		
		if(decoder->frametype == VOAAC_ADTS)
			config = decoder->adts.channel_configuration;
		else
			config = decoder->channelNum;

		switch (config)
        {
		case 0:
		case 1:
        case 2:
            channel_position[0] = VO_CHANNEL_FRONT_LEFT;
            channel_position[1] = VO_CHANNEL_FRONT_RIGHT;
			channel_offsize[0] = 0;
			channel_offsize[1] = 1;
            break;
        case 3:
            channel_position[0] = VO_CHANNEL_FRONT_LEFT;
            channel_position[1] = VO_CHANNEL_FRONT_RIGHT;
			channel_position[2] = VO_CHANNEL_CENTER;
			if(decoder->first_id_syn_ele == ID_SCE)
			{
				channel_offsize[0] = 2;
				channel_offsize[1] = 0;
				channel_offsize[2] = 1;
			}
			else
			{
				channel_offsize[0] = 0;
				channel_offsize[1] = 1;
				channel_offsize[2] = 2;
			}
            break;
        case 4:
            channel_position[0] = VO_CHANNEL_FRONT_LEFT;
            channel_position[1] = VO_CHANNEL_FRONT_RIGHT; 
            channel_position[2] = VO_CHANNEL_CENTER;
            channel_position[3] = VO_CHANNEL_BACK_CENTER;
			if(decoder->first_id_syn_ele == ID_SCE)
			{
				channel_offsize[0] = 2;
				channel_offsize[1] = 0;
				channel_offsize[2] = 1;
			}
			else
			{
				channel_offsize[0] = 0;
				channel_offsize[1] = 1;
				channel_offsize[2] = 2;
			}
			channel_offsize[3] = 3;
            break;
        case 5:
            channel_position[0] = VO_CHANNEL_FRONT_LEFT;
            channel_position[1] = VO_CHANNEL_FRONT_RIGHT;
            channel_position[2] = VO_CHANNEL_CENTER;
            channel_position[3] = VO_CHANNEL_BACK_LEFT;
			channel_position[4] = VO_CHANNEL_BACK_RIGHT;
			if(decoder->first_id_syn_ele == ID_SCE)
			{
				channel_offsize[0] = 2;
				channel_offsize[1] = 0;
				channel_offsize[2] = 1;
			}
			else
			{
				channel_offsize[0] = 0;
				channel_offsize[1] = 1;
				channel_offsize[2] = 2;
			}
			channel_offsize[3] = 3;
			channel_offsize[4] = 4;
            break;
        case 6:
			channel_position[0] = VO_CHANNEL_FRONT_LEFT;
			channel_position[1] = VO_CHANNEL_FRONT_RIGHT;
			channel_position[2] = VO_CHANNEL_CENTER;
			channel_position[3] = VO_CHANNEL_LFE_BASS; 
			channel_position[4] = VO_CHANNEL_BACK_LEFT;
			channel_position[5] = VO_CHANNEL_BACK_RIGHT;
			if(decoder->first_id_syn_ele == ID_SCE)
			{
				channel_offsize[0] = 2;
				channel_offsize[1] = 0;
				channel_offsize[2] = 1;
			}
			else
			{
				channel_offsize[0] = 0;
				channel_offsize[1] = 1;
				channel_offsize[2] = 2;
			}
			channel_offsize[3] = 4;
			channel_offsize[4] = 5;
			channel_offsize[5] = 3;
            break;
        case 7:
            channel_position[0] = VO_CHANNEL_FRONT_LEFT;
            channel_position[1] = VO_CHANNEL_FRONT_RIGHT;
			channel_position[2] = VO_CHANNEL_CENTER; ;
			channel_position[3] = VO_CHANNEL_LFE_BASS; 
            channel_position[4] = VO_CHANNEL_SIDE_LEFT; 
            channel_position[5] = VO_CHANNEL_SIDE_RIGHT; 
            channel_position[6] = VO_CHANNEL_BACK_LEFT; 
            channel_position[7] = VO_CHANNEL_BACK_RIGHT; 
			if(decoder->first_id_syn_ele == ID_SCE)
			{
				channel_offsize[0] = 2;
				channel_offsize[1] = 0;
				channel_offsize[2] = 1;
			}
			else
			{
				channel_offsize[0] = 0;
				channel_offsize[1] = 1;
				channel_offsize[2] = 2;
			}
			channel_offsize[3] = 4;
			channel_offsize[4] = 5;
			channel_offsize[5] = 6;
			channel_offsize[6] = 7;
			channel_offsize[7] = 3;
            break;
        }
    }
#else
    /* check if there is a PCE */
    if (decoder->pce_set)
    {
        int i, chpos = 0;
        int chdir, back_center = 0;
		program_config *pce = &decoder->pce;

		chdir = pce->num_front_channels;	

        if (chdir == 3)
        {
			if(pce->front_element_is_cpe[0] & 1)
			{
				channel_offsize[chpos] = 0;            
				channel_position[chpos++] = VO_CHANNEL_FRONT_LEFT;			
				channel_offsize[chpos] = 2;
				channel_position[chpos++] = VO_CHANNEL_CENTER;
				channel_offsize[chpos] = 1;
				channel_position[chpos++] = VO_CHANNEL_FRONT_RIGHT;
			}
			else
			{
				channel_offsize[chpos] = 1;            
				channel_position[chpos++] = VO_CHANNEL_FRONT_LEFT;			
				channel_offsize[chpos] = 0;
				channel_position[chpos++] = VO_CHANNEL_CENTER;
				channel_offsize[chpos] = 2;
				channel_position[chpos++] = VO_CHANNEL_FRONT_RIGHT;
			}			
        }
		else if(chdir == 2)
		{
			channel_offsize[chpos] = chpos;            
			channel_position[chpos++] = VO_CHANNEL_FRONT_LEFT;			
			channel_offsize[chpos] = chpos;
            channel_position[chpos++] = VO_CHANNEL_FRONT_RIGHT;
		}
		else if(chdir == 1)
		{
			channel_offsize[chpos] = chpos;
			channel_position[chpos++] = VO_CHANNEL_CENTER;
		}


		for (i = 0; i < pce->num_side_channels; i += 2)
        {
			channel_offsize[chpos] = chpos;            
			channel_position[chpos++] = VO_CHANNEL_SIDE_LEFT;
			channel_offsize[chpos] = chpos;
            channel_position[chpos++] = VO_CHANNEL_SIDE_RIGHT;
        }

		chdir = pce->num_back_channels;
        if (chdir & 1)
        {
            back_center = 1;
            chdir--;
        }
        for (i = 0; i < chdir; i += 2)
        {
			channel_offsize[chpos] = chpos;            
			channel_position[chpos++] = VO_CHANNEL_BACK_LEFT;
			channel_offsize[chpos] = chpos;
            channel_position[chpos++] = VO_CHANNEL_BACK_RIGHT;
        }

        if (back_center)
        {
			channel_offsize[chpos] = chpos;
            channel_position[chpos++] = VO_CHANNEL_BACK_CENTER;
        }

		for (i = 0; i < pce->num_lfe_channels; i++)
        {
			channel_offsize[chpos] = chpos;            
			channel_position[chpos++] = VO_CHANNEL_LFE_BASS;
        } 
    } 
	else 
	{
		int config;
		
		if(decoder->frametype == VOAAC_ADTS)
			config = decoder->adts.channel_configuration;
		else
			config = decoder->channelNum;

		switch (config)
        {
		case 0:
		case 1:
        case 2:
            channel_position[0] = VO_CHANNEL_FRONT_LEFT;
            channel_position[1] = VO_CHANNEL_FRONT_RIGHT;
			channel_offsize[0] = 0;
			channel_offsize[1] = 1;
            break;
        case 3:
            channel_position[0] = VO_CHANNEL_FRONT_LEFT;
            channel_position[1] = VO_CHANNEL_CENTER;
			channel_position[2] = VO_CHANNEL_FRONT_RIGHT;
			if(decoder->first_id_syn_ele == ID_SCE)
			{
				channel_offsize[0] = 1;
				channel_offsize[1] = 0;
				channel_offsize[2] = 2;
			}
			else
			{
				channel_offsize[0] = 0;
				channel_offsize[1] = 2;
				channel_offsize[2] = 1;
			}
            break;
        case 4:
            channel_position[0] = VO_CHANNEL_FRONT_LEFT;
            channel_position[1] = VO_CHANNEL_CENTER;
            channel_position[2] = VO_CHANNEL_FRONT_RIGHT;
            channel_position[3] = VO_CHANNEL_BACK_CENTER;
			if(decoder->first_id_syn_ele == ID_SCE)
			{
				channel_offsize[0] = 1;
				channel_offsize[1] = 0;
				channel_offsize[2] = 2;
			}
			else
			{
				channel_offsize[0] = 0;
				channel_offsize[1] = 2;
				channel_offsize[2] = 1;
			}
			channel_offsize[3] = 3;
            break;
        case 5:
            channel_position[0] = VO_CHANNEL_FRONT_LEFT;
            channel_position[1] = VO_CHANNEL_CENTER;
            channel_position[2] = VO_CHANNEL_FRONT_RIGHT; 
            channel_position[3] = VO_CHANNEL_BACK_LEFT;
			channel_position[4] = VO_CHANNEL_BACK_RIGHT;
			if(decoder->first_id_syn_ele == ID_SCE)
			{
				channel_offsize[0] = 1;
				channel_offsize[1] = 0;
				channel_offsize[2] = 2;
			}
			else
			{
				channel_offsize[0] = 0;
				channel_offsize[1] = 2;
				channel_offsize[2] = 1;
			}
			channel_offsize[3] = 3;
			channel_offsize[4] = 4;
            break;
        case 6:
            channel_position[0] = VO_CHANNEL_FRONT_LEFT;
            channel_position[1] = VO_CHANNEL_CENTER;
			channel_position[2] = VO_CHANNEL_FRONT_RIGHT;
			channel_position[3] = VO_CHANNEL_BACK_LEFT;
            channel_position[4] = VO_CHANNEL_BACK_RIGHT;
            channel_position[5] = VO_CHANNEL_LFE_BASS;
			if(decoder->first_id_syn_ele == ID_SCE)
			{
				channel_offsize[0] = 1;
				channel_offsize[1] = 0;
				channel_offsize[2] = 2;
			}
			else
			{
				channel_offsize[0] = 0;
				channel_offsize[1] = 2;
				channel_offsize[2] = 1;
			}
			channel_offsize[3] = 3;
			channel_offsize[4] = 4;
			channel_offsize[5] = 5;
            break;
        case 7:
            channel_position[0] = VO_CHANNEL_FRONT_LEFT;
            channel_position[1] = VO_CHANNEL_CENTER;
			channel_position[2] = VO_CHANNEL_FRONT_RIGHT;
			channel_position[3] = VO_CHANNEL_SIDE_LEFT;
            channel_position[4] = VO_CHANNEL_SIDE_RIGHT;
            channel_position[5] = VO_CHANNEL_BACK_LEFT;
            channel_position[6] = VO_CHANNEL_BACK_RIGHT;
            channel_position[7] = VO_CHANNEL_LFE_BASS;
			if(decoder->first_id_syn_ele == ID_SCE)
			{
				channel_offsize[0] = 1;
				channel_offsize[1] = 0;
				channel_offsize[2] = 2;
			}
			else
			{
				channel_offsize[0] = 0;
				channel_offsize[1] = 2;
				channel_offsize[2] = 1;
			}
			channel_offsize[3] = 3;
			channel_offsize[4] = 4;
			channel_offsize[5] = 5;
			channel_offsize[6] = 6;
			channel_offsize[7] = 7;
            break;
        }
    }
#endif

	return;
}
