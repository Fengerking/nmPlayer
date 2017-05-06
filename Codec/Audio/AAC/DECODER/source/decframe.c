#include "struct.h"
#include "decoder.h"
#include "sbr_dec.h"
#include "global.h"
#include "voLog.h"
#ifdef PS_DEC
#include "ps_dec.h"
#endif//PS_DEC
#ifdef BSAC_DEC
#include "decode_bsac.h"
#endif

static void ResetDecStatus(AACDecoder *decoder)
{
	decoder->old_id_syn_ele = -1;
	decoder->id_syn_ele = -1;
	decoder->sbrEnabled = 0;
	decoder->intensity_used = 0;
	decoder->ms_mask_present = 0;
	decoder->decodedChans = 0;
	decoder->decodedSBRChans = 0;
	decoder->seletedChDecoded = 0;
	decoder->seletedSBRChDecoded = 0;
}

static void PostProcess(AACDecoder *decoder, VO_S16* outbuf)
{
	VO_AUDIO_FORMAT * pAudioFormat;
	int channels = decoder->channelNum;
#ifdef SBR_DEC
	sbr_info *sbrDec;
	sbrDec = (sbr_info *)(decoder->sbr);
#endif

	if(channels < decoder->decodedChans)
		channels = decoder->decodedChans;

#ifdef PS_DEC	
	if(sbrDec&&sbrDec->ps_used)
	{
		channels = 2;
	}
#endif

	if(decoder->decodedChans > 2 && decoder->seletedChs != VO_CHANNEL_ALL)
		channels = decoder->seletedChDecoded;

	pAudioFormat = &decoder->outAudioFormat;

	switch(decoder->chSpec) {
	case VO_AUDIO_CHAN_DUALONE:
		if(channels==1)
		{
			Mono2Stereo(decoder,outbuf,MAX_SAMPLES * (decoder->sbrEnabled ? 2 : 1));
			channels = 2;
		}
		break;

	case VO_AUDIO_CHAN_MULDOWNMIX2:
#if DOWNTO2CHS
		if(decoder->decodedChans == 6 && decoder->seletedChs == VO_CHANNEL_ALL)
		{
			DownMixto2Chs(decoder,decoder->decodedChans,outbuf);
			channels = 2;
		}
		else if(decoder->decodedChans > 2 && decoder->decodedChans != 6 && decoder->seletedChs == VO_CHANNEL_ALL)
		{
			Selectto2Chs(decoder,decoder->decodedChans,outbuf);
			channels = 2;
		}
		else
		{
			if(decoder->decodedChans > 2 && decoder->seletedChDecoded && decoder->seletedChs != VO_CHANNEL_ALL)
				channels = decoder->seletedChDecoded;
		}
#endif//DOWNTO2CHS
		break;

	case VO_AUDIO_CHAN_STE2MONO:
		if(decoder->decodedChans == 2)
		{
			Stereo2Mono(decoder,outbuf, MAX_SAMPLES * (decoder->sbrEnabled ? 2 : 1));
			channels = 1;
		}
		break;

	case VO_AUDIO_CHAN_DUALLEFT:
	case VO_AUDIO_CHAN_DUALRIGHT:
		if(decoder->decodedChans == 2)
		{
			PostChannelProcess(decoder,outbuf, MAX_SAMPLES * (decoder->sbrEnabled ? 2 : 1));
			channels = 2;
		}
		break;
	}


#ifdef PS_DEC
	if((decoder->decodedChans == 1 && decoder->channelNum == 2 && (!sbrDec||!sbrDec->ps_used)))
#else
	if(decoder->decodedChans == 1 && decoder->channelNum == 2)
#endif
	{
		Mono2Stereo(decoder,outbuf, MAX_SAMPLES * (decoder->sbrEnabled ? 2 : 1));
		channels = 2;
	}

	pAudioFormat->SampleBits = 16;
	pAudioFormat->SampleRate = decoder->sampleRate * (decoder->sbrEnabled ? 2 : 1);
	pAudioFormat->Channels = channels;

	return;
}

int DecodeOneFrame(AACDecoder *decoder, VO_S16* outbuf)
{
	int err=0;
	BitStream *bs;
	int length, muxlength;
	unsigned char *inbuf;
	FrameStream *fstream;
	
	fstream = &(decoder->Fstream);
	bs = &(decoder->bs);
	decoder->frame_length = 0;

	inbuf = fstream->this_frame;
	length = fstream->length;

	BitStreamInit(bs, length, inbuf);

	if(decoder->framenumber==0)
	{		
		if(decoder->frametype == VOAAC_LOAS)
		{
			BitStream bsi;

#ifndef _SYMBIAN_
			if(decoder->latm == NULL)
			{
				decoder->latm = (latm_header *)voAACDecAlignedMalloc(decoder->voMemop,sizeof(latm_header));
				if(decoder->latm == NULL)
					return VO_ERR_OUTOF_MEMORY; 
			}
#endif

			BitStreamInit(&bsi, length, inbuf);

			if(BitStreamGetBits(&bsi, 11) != 0x2b7) 
				return VO_ERR_AAC_UNSPROFILE;		// just support AudioSyncStream() now

			muxlength = BitStreamGetBits(&bsi, 13);

			decoder->frame_length = muxlength + 3;

			if (3+muxlength > length) 
			{
				return VO_ERR_INPUT_BUFFER_SMALL;			// not enough data
			}

			if(ReadMUXConfig(decoder, &bsi) < 0)
				return VO_ERR_AAC_UNSPROFILE;

			if(updateProfile(decoder, decoder->profile))
				return VO_ERR_AAC_UNSPROFILE;

			if(decoder->channelNum<=0 || decoder->channelNum>MAX_CHANNELS)
				return VO_ERR_AUDIO_UNSCHANNEL;

			if(updateSampleRate(decoder, decoder->sampleRate))
				return VO_ERR_AUDIO_UNSSAMPLERATE;
		}
		
		if(decoder->frametype == VOAAC_RAWDATA)//guarantee the params are updated
		{
			if(updateProfile(decoder, decoder->profile))
				return VO_ERR_AAC_UNSPROFILE;
			
			if(decoder->channelNum<=0 || decoder->channelNum>MAX_CHANNELS)
				return VO_ERR_AUDIO_UNSCHANNEL;

			if(updateSampleRate(decoder, decoder->sampleRate))
				return VO_ERR_AUDIO_UNSSAMPLERATE;

			if (length > 7 && ((inbuf[0] & 0xFF) == 0xFF && (inbuf[1] & 0xF0) == 0xF0))
			{
				decoder->Checknumber++;
				if(decoder->Checknumber >= 3)
				{
					//return VO_ERR_AAC_UNSPROFILE;
					decoder->frametype = VOAAC_ADTS;
				}
			}
		}
		
		if(length > 4 && IS_ADIFHEADER(inbuf))
		{
			err = ParseADIFHeader(decoder, bs);
			if(err)
			{
				return err;
			}
		}
		
#ifdef BSAC_DEC	
		if(decoder->profile==VOAAC_ER_BSAC)
		{
			err = sam_decode_init(decoder,decoder->sampleRate,1024);
			if(err)//only support 1024 sample
			{
				return err;
			}

		}
#endif//BSAC_DEC
	}
	
	//parse ADTS header if the frame is not rawdata
	if(decoder->frametype == VOAAC_ADTS)
	{
		err = ParseADTSHeader(decoder);
		if(err)
		{
			return err;
		}
	}
	else if(decoder->frametype == VOAAC_LOAS)
	{
		err = ParserLatm(decoder);
		if(err)
		{
			return err;
		}
	}
	else if(decoder->frametype == VOAAC_LATM)
	{
		int tmp = 0; 
		int framelength = 0;
		int skiplength = 0;
		do {
			tmp = BitStreamGetBits(bs, 8);
			framelength += tmp;
			skiplength++;
		} while (tmp == 255);

		if(framelength + skiplength > length)
		{
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
	}

	Channelconfig(decoder);

	ResetDecStatus(decoder);

	do {
		if(decoder->profile != VOAAC_ER_BSAC)
		{
			err = raw_data_block(decoder);
			if(err)
			{
				if(bs->noBytes)
				{
					return VO_ERR_INPUT_BUFFER_SMALL;
				}			
				return err;
			}

			if(bs->noBytes)
			{
				return VO_ERR_INPUT_BUFFER_SMALL;
			}
		}
#ifdef BSAC_DEC
		else//BSAC
		{
			decoder->elementChans = decoder->channelNum;
			if(decoder->elementChans==2)
				decoder->id_syn_ele = ID_CPE;
			else
				decoder->id_syn_ele = ID_SCE;

			err = bsac_raw_data_block(decoder, inbuf, length);
			if(err)
			{
				return err;
			}
		}
#endif//#ifdef BSAC_DEC	
	
		if(decoder->decodedChans>=decoder->channelNum&&decoder->elementChans>0)
		{
			return VO_ERR_AAC_INVSTREAM;
		}
		
		if (decoder->elementChans>0) 
		{
			//dequant and applying scalar
			if(err != dequant_rescale(decoder,decoder->elementChans))
				return VO_ERR_AAC_INVSTREAM;

			//mid-side and intensity stereo
			mi_decode(decoder,decoder->elementChans);

			//pns decode
			pns_decode(decoder,decoder->elementChans);

#ifdef MAIN_DEC
			err = ic_prediction(decoder,decoder->elementChans);
			if(err)
				return err;
#endif//MAIN_DEC

#ifdef LTP_DEC
			//pns decode
			err = ltp_decode(decoder,decoder->elementChans);
			if(err)
				return err;
#endif//LTP_DEC
			//tns decode
			tns_decode(decoder,decoder->elementChans);

			//filter bank
			err = filter_bank(decoder,outbuf);
			if (err)
			{
				return err;
			}
		
#ifdef LTP_DEC
			ltp_update(decoder,decoder->elementChans);
#endif//LTP_DEC		
		}

#ifdef SBR_DEC
		if (decoder->sbrEnabled&&elementNumChans[decoder->old_id_syn_ele])// && (decoder->id_syn_ele == ID_FIL || decoder->id_syn_ele == ID_LFE)) 
		{
			decoder->elementSBRChans = elementNumChans[decoder->old_id_syn_ele];	
		 
			if (decoder->decodedSBRChans>= decoder->channelNum&& decoder->elementSBRChans >0 )
			{
				return VO_ERR_AAC_INVSBRSTREAM;
			}

			/* parse SBR extension data if present (contained in a fill element) */
			if (voSBRExtData(decoder, decoder->decodedSBRChans))
			{
				return VO_ERR_AAC_INVSBRSTREAM;
			}

			/* apply SBR */
			if (DecodeSBRData(decoder, decoder->decodedSBRChans, outbuf))
			{
				return VO_ERR_AAC_INVSBRSTREAM;
			}
	
			UpdateSeletedSBRChDecoded(decoder,decoder->elementSBRChans);
			decoder->decodedSBRChans += decoder->elementSBRChans;
			{
				sbr_info *sbrDec = (sbr_info *)(decoder->sbr);
				if(sbrDec&&sbrDec->ps_used)
				{
					decoder->decodedChans++;//if ps is used,then the decodedChans double
					if(decoder->decodedChans>2)
					{
						return VO_ERR_AAC_INVSBRSTREAM;
					}
				}
			}
		}
#endif

		UpdateSeletedChDecoded(decoder,decoder->elementChans);
		decoder->decodedChans += decoder->elementChans;
		if(decoder->channelNum>MAX_CHANNELS)
		{
			return VO_ERR_AAC_INVSTREAM;
		}

	} while (decoder->id_syn_ele != ID_END&& decoder->profile != VOAAC_ER_BSAC);

	if(decoder->decodedChans <= 0)
		return VO_ERR_AAC_INVSTREAM;

	PostProcess(decoder, outbuf);

	decoder->framenumber++;

	return err ;
}
