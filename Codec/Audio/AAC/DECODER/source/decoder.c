//############################################################################################################

	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2004				*
	*																		*
	************************************************************************/
//############################################################################################################


#include "cmnAudioMemory.h"
#include "struct.h"
#include "decoder.h"
#include "voAAC.h"

#include "voChHdle.h"
#include "voCheck.h"
#include "sbr_dec.h"
#include "voLog.h"
//#define LOGDUMP

#ifdef LOGDUMP
#include "voLog.h"

#define LOG_TAG "decoder"
#endif

#if defined(LCHECK)
VO_PTR	g_hAACDecInst = NULL;
#endif

#ifdef LOGDUMP
static FILE* dump =NULL;
#endif

VO_U32 VO_API voAACDecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA *pUserData )
{
	int 	ch;
	VO_U32 nRet = 0;
	AACDecoder *decoder = NULL;
	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *voMemop = NULL;
	int* pcoef = NULL;
	int* overlap = NULL;
	int* tmpbuf = NULL;
	unsigned char* start = NULL;
#if LCHECK	
  VO_PTR p_lic_handle				= NULL;
  VO_LIB_OPERATOR *p_lib_op		= NULL;
#endif

  if(pUserData!=NULL){
    VOLOGINIT((VO_CHAR *)pUserData->reserved1);
  }  
  VOLOGE("VOLOGE");
  VOLOGW("VOLOGW");
  VOLOGI("VOLOGI");
  VOLOGS("VOLOGS");
  VOLOGR("VOLOGR");

  voMemoprator.Alloc = cmnMemAlloc;
  voMemoprator.Copy = cmnMemCopy;
  voMemoprator.Free = cmnMemFree;
  voMemoprator.Set = cmnMemSet;
  voMemoprator.Check = cmnMemCheck;
  voMemop = &voMemoprator;

	if(pUserData != NULL && (pUserData->memflag&0x0f) == VO_IMF_USERMEMOPERATOR && pUserData->memData != NULL )
	{   
    //not default case, caller passes memory pointer
		voMemop = (VO_MEM_OPERATOR *)pUserData->memData;
	}

#if LCHECK
  if ( pUserData ) {
    if ( pUserData->memflag & 0XF0 ) {//valid libOperator flag
      p_lib_op = pUserData->libOperator;
    }
  }
  nRet = voCheckLibInit( &p_lic_handle, VO_INDEX_DEC_AAC, p_lib_op ? pUserData->memflag : 0, g_hAACDecInst, p_lib_op );

  if ( nRet != VO_ERR_NONE ) {
    voCheckLibUninit( p_lic_handle );
    p_lic_handle = NULL;
    return nRet;
  }
#endif

	decoder = (AACDecoder*)voAACDecAlignedMalloc(voMemop, sizeof(AACDecoder));
	if(decoder==NULL)
	{
		goto INIT_END;
	}

	tmpbuf = (int*)voAACDecAlignedMalloc(voMemop, 4*MAX_SAMPLES*sizeof(VO_U32));
	if(tmpbuf == NULL) 
	{
		goto INIT_END;
	}

	decoder->tmpBuffer = (VO_U32 *)tmpbuf;

	pcoef = (int *)voAACDecAlignedMalloc(voMemop, MAX_SYNTAX_ELEMENTS*MAX_SAMPLES*sizeof(VO_S32));
	if(pcoef == NULL)
	{
		goto INIT_END;
	}
	for(ch = 0; ch < MAX_SYNTAX_ELEMENTS; ch++)
	{
		decoder->coef[ch] = pcoef + MAX_SAMPLES*ch;
	}

	start = (VO_U8 *)voAACDecAlignedMalloc(voMemop, BUFFER_DATA);
	if(start == NULL)
	{
		goto INIT_END;
	}
	decoder->Fstream.start = start;

#ifdef _SYMBIAN_
	overlap = (VO_S32 *)voAACDecAlignedMalloc(voMemop, MAX_SYNTAX_ELEMENTS*MAX_SAMPLES*sizeof(VO_S32));
	if(overlap == NULL)
	{
		goto INIT_END;
	}

	for(ch = 0; ch < MAX_SYNTAX_ELEMENTS; ch++)
	{
		decoder->overlap[ch] = overlap + MAX_SAMPLES*ch;
	}

	decoder->latm = (latm_header *)voAACDecAlignedMalloc(voMemop,sizeof(latm_header));
	if(decoder->latm == NULL)
		goto INIT_END;
#endif
	
	decoder->Fstream.maxLength = BUFFER_DATA;
	decoder->Fstream.storelength = 0;
	decoder->Fstream.length = 0;

	decoder->channelNum	= 2;
	decoder->sampleBits = 16;
	decoder->sampleRate	= 44100;
	decoder->profile	= VOAAC_AAC_LC;
	decoder->chSpec	= 0;
	decoder->seletedChs = VO_CHANNEL_ALL;//default,decode all channels if the sample is multichannels
	decoder->nFlushFlag = 0;
	decoder->decoderNum = 0;


  decoder->voMemop = &decoder->voMemoprator;
  decoder->voMemop->Alloc = voMemop->Alloc;
  decoder->voMemop->Copy = voMemop->Copy;
  decoder->voMemop->Free = voMemop->Free;
  decoder->voMemop->Set = voMemop->Set;
  decoder->voMemop->Check = voMemop->Check;


#if LCHECK
  decoder->hCheck = p_lic_handle;
#endif


	*phCodec = decoder;

#ifdef LOGDUMP
	dump = fopen("D:/dump.aac", "wb");
#endif

	return VO_ERR_NONE;

INIT_END:
	if(decoder) SafeAlignedFree(decoder);
	if(tmpbuf)  SafeAlignedFree(tmpbuf);
	if(pcoef)   SafeAlignedFree(pcoef);
	if(overlap)   SafeAlignedFree(overlap);
	if(start)   SafeAlignedFree(start);

	return VO_ERR_OUTOF_MEMORY;
}

VO_U32 VO_API voAACDecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	int lenth;
	AACDecoder *decoder;
	FrameStream *fstream;

	decoder = (AACDecoder*)hCodec;
	if(decoder == NULL)
		return VO_ERR_INVALID_ARG;

	if(pInput == NULL || pInput->Buffer == NULL)
		return VO_ERR_INVALID_ARG;

	fstream = &decoder->Fstream;
	
	fstream->input = pInput->Buffer;
	fstream->inlen = pInput->Length;

	fstream->this_frame = fstream->input;
	fstream->length = fstream->inlen;
	fstream->uselength = 0;




#ifdef LOGDUMP
	VOLOGI ("AAC decoder Input buffer. %d", pInput->Length);

	if(dump)
	{
		fwrite(&pInput->Length, 1, 4, dump);
		fwrite(pInput->Buffer, 1, pInput->Length, dump);
	}
	else
		VOLOGI ("Open file error");
#endif

	if(fstream->storelength)
	{
		lenth = MIN(fstream->maxLength - fstream->storelength, fstream->inlen);
		decoder->voMemop->Copy(VO_INDEX_DEC_AAC, fstream->start + fstream->storelength, 
			fstream->input, lenth);

		fstream->this_frame = fstream->start;
		fstream->length = fstream->storelength + lenth;
		fstream->storelength = fstream->length;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voAACDecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO *pOutInfo)
{
	int err=0;
	unsigned int length;
	unsigned char* inbuf;
	BitStream *bs;
	AACDecoder *decoder;
	FrameStream *fstream;
	VO_AUDIO_FORMAT *poutAudioFormat;

	decoder = (AACDecoder*)hCodec;
	if(decoder == NULL || pOutput == NULL || pOutput->Buffer == NULL)
		return VO_ERR_INVALID_ARG;

	length = MAX(decoder->seletedChDecoded, 2);
	if(decoder->chSpec == VO_AUDIO_CHAN_MULDOWNMIX2)
		length = 2;
	length = length * (decoder->sbrEnabled ? 2 : 1);
	fstream = &decoder->Fstream;
	bs = &(decoder->bs);

	if(pOutput->Length < length*MAX_SAMPLES*2)
	{
		pOutput->Length = 0;
		if(pOutInfo)
			pOutInfo->InputUsed = fstream->uselength;
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	}

	if(fstream->length == 0)
	{
		return VO_ERR_INPUT_BUFFER_SMALL;
	}
	
	inbuf = fstream->this_frame;
	err = DecodeOneFrame(decoder, (VO_S16*)pOutput->Buffer);

	//Add for UnitTest
	if (err == (VO_ERR_AUDIO_UNSFEATURE))
	{
		err = VO_ERR_AAC_INVSTREAM;
	}

	if(err == VO_ERR_AAC_INVADTS) 
	{
		if(fstream->storelength) {
			fstream->this_frame = fstream->input;
			fstream->length = fstream->inlen;
			fstream->storelength = 0;
			fstream->uselength = 0;

			err =  DecodeOneFrame(decoder, (VO_S16*)pOutput->Buffer);
		}

		if(err == VO_ERR_AAC_INVADTS)
		{
			err = VO_ERR_INPUT_BUFFER_SMALL;
		}
	}

	if(decoder->frametype != VOAAC_LATM)
	{
		if(err == VO_ERR_INPUT_BUFFER_SMALL)
		{
			length = fstream->length;		
			if(fstream->storelength == 0)
			{	
				decoder->voMemop->Copy(VO_INDEX_DEC_AAC, fstream->start, 
					fstream->this_frame, length);
				fstream->this_frame = fstream->start;
			}
			fstream->storelength = length;
			fstream->uselength += length;
			pOutput->Length = 0;
			if(pOutInfo)
				pOutInfo->InputUsed = fstream->uselength;

			if(fstream->this_frame != fstream->start)
			{
				decoder->voMemop->Copy(VO_INDEX_DEC_AAC, fstream->start, fstream->this_frame, fstream->length);
			}
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
	}		
	
	if(decoder->frametype != VOAAC_LOAS)
		length =  (CalcBitsUsed(bs, fstream->this_frame, 0) + 7) >> 3;
	else
		length = decoder->frame_length;

	if(fstream->length < length)
	{
		fstream->this_frame += fstream->length;
		fstream->length = 0;
		fstream->uselength += fstream->length;
		
		return VO_ERR_FAILED;
	}

	if(decoder->frametype == VOAAC_RAWDATA && err)
	{
		length = fstream->length;
	}

	fstream->this_frame += length;
	fstream->length -= length;
	fstream->uselength += length;

	if(fstream->storelength)
	{
		int lenth;
		if(decoder->frametype != VOAAC_LOAS)
			length =  (CalcBitsUsed(bs, inbuf, 0) + 7) >> 3;

		lenth = length - (fstream->storelength - fstream->inlen);
		if(lenth >= 0)
		{
			fstream->this_frame = fstream->input + lenth;
			fstream->length = fstream->inlen - lenth;
			fstream->uselength -= (fstream->storelength  - fstream->inlen);
			fstream->storelength = 0;
		}
		else
		{
			lenth = fstream->storelength - length;
			decoder->voMemop->Copy(VO_INDEX_DEC_AAC, fstream->start, 
					fstream->start + length, lenth);

			fstream->uselength -= length;
			fstream->storelength = lenth;
			fstream->this_frame = fstream->start;
			fstream->length = lenth;			
		}
	}

	if(err) 
	{
		pOutput->Length = 0;
		if(pOutInfo)
			pOutInfo->InputUsed = fstream->uselength;
		return err;
	}

	poutAudioFormat = &decoder->outAudioFormat;		 		
	pOutput->Length =  poutAudioFormat->Channels * MAX_SAMPLES * (decoder->sbrEnabled ? 2 : 1)*2;

	if(pOutInfo)
	{
		pOutInfo->Format.SampleRate = poutAudioFormat->SampleRate; 
		pOutInfo->Format.Channels = poutAudioFormat->Channels;    
		pOutInfo->Format.SampleBits = 16;			
		pOutInfo->InputUsed = fstream->uselength;
	}

	if (decoder->decoderNum == 32767)
	{
		decoder->decoderNum = 256;
	}
	else
	{
		decoder->decoderNum++;
	}
	//#8710 bug fixed, skip the first frame after flush.
	if (decoder->nFlushFlag == 1)
	{
		pOutput->Length = 0;
		decoder->nFlushFlag = 0;
	}

#if defined (LCHECK)
	voCheckLibCheckAudio(decoder->hCheck, pOutput, &(pOutInfo->Format));
#endif

	return VO_ERR_NONE;
}

VO_U32 VO_API voAACDecSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	AACDecoder*	decoder = (AACDecoder*)hCodec;
	int i, err, lValue;
	if (decoder==NULL || pData == NULL )//avoid invalid pointer 
		return VO_ERR_INVALID_ARG;

	switch(uParamID)
	{
	case VO_PID_COMMON_HEADDATA:
		{	
			VO_CODECBUFFER* params = (VO_CODECBUFFER*)pData;
			//BitStream bs2;
			BitStream *bs = &decoder->bs;
			int profile,sampIdx,chanNum,sampFreq;
			int length=params->Length;
			if(params->Length>1024)
				length = 1024;
#ifdef LOGDUMP
			VOLOGI ("AAC decoder Input buffer. %d", params->Length);

			if(dump)
			{
				fwrite(&params->Length, 1, 4, dump);
				fwrite(params->Buffer, 1, params->Length, dump);
			}
			else
				VOLOGI ("Open file error");
#endif
			BitStreamInit(bs, length, params->Buffer);

			if(params->Length > 4 && IS_ADIFHEADER(params->Buffer))
			{
				err = ParseADIFHeader(decoder, bs);
				if(err)
					return err;
				break;
			}

			//if(decoder->frametype != VOAAC_RAWDATA)
			//	break; 			

			profile = BitStreamGetBits(bs,5);
			if(profile==31)
			{
				profile = BitStreamGetBits(bs,6);
				profile +=32;
			}

			sampIdx = BitStreamGetBits(bs,4);
			if(sampIdx==0x0f)
			{
				sampFreq = BitStreamGetBits(bs,24);
			}
			else
			{
				if(sampIdx < NUM_SAMPLE_RATES) {
					sampFreq = sampRateTab[sampIdx];
				}
				else
				{
					if(decoder->sampleRate)			
						sampFreq = decoder->sampleRate;
					else
						return VO_ERR_AUDIO_UNSSAMPLERATE;
				}
			}

			chanNum = BitStreamGetBits(bs,4);

			//LG #8606 support 3~5 channels
			if(chanNum > 2 && chanNum <= 6)
				chanNum = 6;

			if(chanNum <= 0 || chanNum > MAX_CHANNELS)
			{
				if(decoder->channelNum)			
					chanNum = decoder->channelNum;
				else
					return VO_ERR_AUDIO_UNSCHANNEL;
			}
			
			decoder->sampleRate = sampFreq;
			decoder->profile = profile;
			decoder->channelNum = chanNum;

			Channelconfig(decoder);
			break;
		}
		
	case VO_PID_AAC_PROFILE:
		lValue = *((int *)pData);
		err = updateProfile(decoder, lValue);
		if (err)
		{
			return err;
		}
		break;
	case VO_PID_AAC_FRAMETYPE:
		lValue = *((int *)pData);
		if(lValue < VOAAC_RAWDATA&& lValue > VOAAC_LOAS)
			return VO_ERR_AAC_UNSFILEFORMAT;
		decoder->frametype = lValue;
		break;
	case VO_PID_AUDIO_CHANNELCONFIG:
	case VO_PID_AAC_CHANNELSPEC:
		lValue = *((int *)pData);
		decoder->chSpec = lValue;
		if(lValue == VO_AUDIO_CHAN_DUALLEFT)
			decoder->seletedChs = VO_CHANNEL_FRONT_LEFT;
		else if(lValue == VO_AUDIO_CHAN_DUALRIGHT)
			decoder->seletedChs = VO_CHANNEL_FRONT_RIGHT;
		else
			decoder->seletedChs = VO_CHANNEL_ALL;
		break;
	case VO_PID_AUDIO_CHANNELS:
		lValue = *((int *)pData);
		if(lValue<=0||lValue>MAX_CHANNELS)
			return VO_ERR_AUDIO_UNSCHANNEL;
		decoder->channelNum = lValue;
		break;
	case VO_PID_AUDIO_SAMPLEREATE:
		lValue = *((int *)pData);
        err = updateSampleRate(decoder, lValue);
		if(err)
		{
			return err;
		}
		break;
	case VO_PID_AAC_DISABLEAACPLUSV1:
		lValue = *((int *)pData);
		decoder->disableSBR = lValue;
		break;
	case VO_PID_AAC_DISABLEAACPLUSV2:
		lValue = *((int *)pData);
		decoder->disablePS = lValue;
		break;
	case VO_PID_AAC_SELECTCHS:
		lValue = *((int *)pData);
		decoder->seletedChs = lValue;
		break;
	case VO_PID_COMMON_FLUSH:
		lValue = *((int *)pData);
		for(i=0;i<decoder->channelNum;i++)
		{
			if(i>=MAX_CHANNELS)//in error bitstream,it may appear
				break;
			if(decoder->overlap[i])
			{
				decoder->voMemop->Set(VO_INDEX_DEC_AAC, decoder->overlap[i], 0, MAX_SAMPLES*sizeof(int));
			}
		}	

		decoder->Fstream.storelength = 0;
		decoder->Fstream.inlen = 0;
		decoder->Fstream.length = 0;
		decoder->decoderNum = 0;

		decoder->nFlushFlag = 1;  //#8710 bug fixed

#ifdef SBR_DEC		
		if(decoder->sbr)
			ReSetSBRDate(decoder->sbr, decoder->voMemop);
#endif

		break;
	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT* fmt = (VO_AUDIO_FORMAT*)pData;
			if(fmt->SampleRate!=0)
			{
				decoder->sampleRate = fmt->SampleRate;
			}

			if(fmt->Channels > 0 && fmt->Channels <= MAX_CHANNELS)
			{
				decoder->channelNum = fmt->Channels;
			}

			if(fmt->SampleBits == 16 || fmt->SampleBits == 32)
			{
				decoder->sampleBits = fmt->SampleBits;
			}

			break;
		}
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voAACDecGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	AACDecoder* decoder = (AACDecoder*)hCodec;
#ifdef WIN32
	int err = 0;
#endif
	if(decoder==NULL || pData == NULL)
		return VO_ERR_INVALID_ARG;

	switch(uParamID)
	{
	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT* fmt = (VO_AUDIO_FORMAT*)pData;
			if(decoder->sampleRate!=0)
			{
				fmt->Channels = decoder->channelNum;
				fmt->SampleRate = decoder->sampleRate * (decoder->sbrEnabled ? 2 : 1);
				fmt->SampleBits = 16;
			}
			else
			{
				return VO_ERR_WRONG_STATUS;
			}
			break;
		}	
	case VO_PID_AAC_PROFILE:	
		*((int *)pData) =  decoder->profile;
		break;
	case VO_PID_AAC_FRAMETYPE:	
		*((int *)pData) =  decoder->frametype;
		break;
	case VO_PID_AAC_CHANNELSPEC:
	case VO_PID_AUDIO_CHANNELCONFIG:
		*((int *)pData) = decoder->chSpec;
		break;
	case VO_PID_AUDIO_CHANNELS:
		*((int *)pData) = decoder->channelNum;
		break;
	case VO_PID_AUDIO_SAMPLEREATE:
		*((int *)pData) = decoder->sampleRate * (decoder->sbrEnabled ? 2 : 1);
		break;
	case VO_PID_AUDIO_CHANNELMODE:
	case VO_PID_AAC_CHANNELMODE:
		*((int *)pData) = decoder->ChansMode;
		break;
	case VO_PID_AAC_CHANNELPOSTION:
		pData = (VO_PTR)decoder->channel_position;
		break;
#ifdef WIN32
	case VO_PID_COMMON_HeadInfo:
		{	
			VO_HEAD_INFO* params = (VO_HEAD_INFO*)pData;
			//BitStream bs2;
			BitStream *bs = &decoder->bs;
			int profile,sampIdx,chanNum,sampFreq;
			int length=params->Length;
			if(params->Length>1024)
				length = 1024;

			if(params->Description == NULL || params->Size < 256)
				break;

			BitStreamInit(bs, length, params->Buffer);

			if(params->Length > 4 && IS_ADIFHEADER(params->Buffer))
			{
				err = ParseADIFHeader(decoder, bs);
				if(err)
				{
					return err;
				}
				
				//sprintf(params->Description, 
				//	"ADIF aac file.\r\n"
				//	"SampleRate: %d \r\n"
				//	"Channel: %d \r\n"
				//	"Profile: %d \r\n",
				//	decoder->sampleRate, 
				//	decoder->channelNum, 
				//	decoder->profile
				//	);

				break;
			}

			if(decoder->frametype != VOAAC_RAWDATA)
			{
/*				sprintf(params->Description, 
					"ADTS aac file.\r\n"
					);	*/		
				break; 			
			}

			profile = BitStreamGetBits(bs,5);
			if(profile==31)
			{
				profile = BitStreamGetBits(bs,6);
				profile +=32;
			}

			sampIdx = BitStreamGetBits(bs,4);
			if(sampIdx==0x0f)
			{
				sampFreq = BitStreamGetBits(bs,24);
			}
			else
			{
				if(sampIdx < NUM_SAMPLE_RATES)
					sampFreq = sampRateTab[sampIdx];
				else
					return VO_ERR_AUDIO_UNSSAMPLERATE;
			}

			chanNum = BitStreamGetBits(bs,4);
			if(chanNum <= 0 || chanNum > MAX_CHANNELS)
				return VO_ERR_AUDIO_UNSCHANNEL;
			
			decoder->sampleRate = sampFreq;
			decoder->profile = profile;
			decoder->channelNum = chanNum;

			//sprintf(params->Description, 
			//		"Rawdata aac file.\r\n" 
			//		"SampleRate: %d \r\n"
			//		"Channel: %d\r\n"
			//		"Profile: %d\r\n",
			//		sampFreq, chanNum, profile
			//		);

			break;
		}
#endif
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API voAACDecUninit(VO_HANDLE hCodec)
{
	AACDecoder* decoder = (AACDecoder*)hCodec;
	VO_MEM_OPERATOR *voMemop;
	int i;

	if (decoder == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
	
	voMemop = decoder->voMemop;

	if(decoder)
	{
#ifdef SBR_DEC
		sbr_free(decoder);
#endif

#ifndef _SYMBIAN_
		for(i=0;i<MAX_CHANNELS;i++)
		{
			if(decoder->overlap[i])
			{
				SafeAlignedFree(decoder->overlap[i]);
			}
		}
#else
		if(decoder->overlap[0])
		{
			SafeAlignedFree(decoder->overlap[0]);
		}
		decoder->overlap[1] = NULL;
#endif

		if(decoder->latm)
		{
			SafeAlignedFree(decoder->latm);
		}

#ifdef LTP_DEC
		for(i = 0; i  < MAX_CHANNELS; i++)
		{
			if(decoder->ltp_coef[i])
			{
				SafeAlignedFree(decoder->ltp_coef[i]);
			}
		}

		if(decoder->t_est_buf)
			SafeAlignedFree(decoder->t_est_buf);
		if(decoder->f_est_buf)
			SafeAlignedFree(decoder->f_est_buf);
#endif

#ifdef MAIN_DEC
		for(i = 0; i  < MAX_CHANNELS; i++)
		{
			if(decoder->pred_stat[i])
			{
				SafeAlignedFree(decoder->pred_stat[i]);
			}
		}
#endif


		SafeAlignedFree(decoder->coef[0]);
		for(i = 0; i  < MAX_SYNTAX_ELEMENTS; i++)
		{
			decoder->coef[i] = NULL;
		}

		if(decoder->tmpBuffer)
			SafeAlignedFree(decoder->tmpBuffer);
		if(decoder->bsac)
			SafeAlignedFree(decoder->bsac);
		if(decoder->Fstream.start)
		{
			SafeAlignedFree(decoder->Fstream.start);
			decoder->Fstream.length = 0;
			decoder->Fstream.maxLength = 0;
		}

#if defined (LCHECK)
		voCheckLibUninit(decoder->hCheck);
#endif

		SafeAlignedFree(decoder);
	}

#ifdef LOGDUMP
	if(dump)
		fclose(dump);
#endif

  VOLOGUNINIT();

	return VO_ERR_NONE;
}

VO_S32 VO_API yyGetAACDecFunc (VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return VO_ERR_INVALID_ARG;
		
	pDecHandle->Init = voAACDecInit;
	pDecHandle->SetInputData = voAACDecSetInputData;
	pDecHandle->GetOutputData = voAACDecGetOutputData;
	pDecHandle->SetParam = voAACDecSetParam;
	pDecHandle->GetParam = voAACDecGetParam;
	pDecHandle->Uninit = voAACDecUninit;

	return VO_ERR_NONE;
}


//#ifdef _LINUX
//#define MODULE_NAME "voAACDec"  //your module name
//#define BUILD_NO 1130  // your build number of your module
//#include "voVersion.c"
//#endif
