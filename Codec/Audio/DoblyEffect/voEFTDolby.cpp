/*******************************************************************************
	File:		voEFTDolby.c

	Content:	aac encoder interface functions

*******************************************************************************/

#include "voEFTDolby.h"
#include "cmnMemory.h"
#include "struct.h"

#include "compiler.h"
#include "target_client.h"
#include "control.h"
#include "tdas_client.h" 
#include "tdas_resampler_helper.h"

/**
* Init the audio codec module and return codec handle
* \param phCodec [OUT] Return the video codec handle
* \param vType	[IN] The codec type if the module support multi codec.
* \param pUserData	[IN] The init param. It is memory operator or alloced memory
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voEFTDOLBYInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA *pUserData)
{
	DOLBYEffectInfo	*deffect = NULL;
	VO_MEM_OPERATOR voMemoprator;
	VO_MEM_OPERATOR *pMemOP;
	VO_MEM_INFO MemInfo;
	int ret;

	int interMem = 0;
	if(pUserData == NULL || pUserData->memflag != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
		voMemoprator.Alloc = cmnMemAlloc;
		voMemoprator.Copy = cmnMemCopy;
		voMemoprator.Free = cmnMemFree;
		voMemoprator.Set = cmnMemSet;
		voMemoprator.Check = cmnMemCheck;
		voMemoprator.Move = cmnMemMove;
		interMem = 1;
		pMemOP = &voMemoprator;
	}
	else
	{
		pMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	}

	MemInfo.Flag = 0;
	MemInfo.Size = sizeof(DOLBYEffectInfo);

	ret = pMemOP->Alloc(VO_INDEX_EFT_DOLBY, &MemInfo);
	if(ret != 0)
	{
		*phCodec = NULL;	
		return VO_ERR_OUTOF_MEMORY;
	}

	deffect = (DOLBYEffectInfo *)MemInfo.VBuffer;

	pMemOP->Set(VO_INDEX_EFT_DOLBY, deffect, 0, sizeof(DOLBYEffectInfo));

	deffect->hDEffect = tdas_open();
	if(deffect->hDEffect == NULL)
	{
		pMemOP->Free(VO_INDEX_EFT_DOLBY, deffect);
		*phCodec = NULL;	
		return VO_ERR_OUTOF_MEMORY;	
	}

	deffect->nInMatrix = MATRIX_PL2;
	deffect->nChanFmt = CHAN_FORMAT_INTERLACED;
	deffect->nBypass = 1;

	deffect->nLfeMixLevel = DMX_LFE_MIX_DEFAULT;

	deffect->nHFEEnable = 0;
	deffect->nHFEDepth = HFE_DEPTH_DEFAULT;

	deffect->nM2SEnable = 0;

	deffect->nMSREnable = 0;
	deffect->nMSRMaxProfile = TDAS_MSR_DEF_MAX_PROFILE;		
	deffect->nMSRBright = MSR_BRIGHT_DEFAULT;	
	deffect->nMSRRoom = MSR_ROOM_DEFAULT;	

	deffect->nNBEnable = 0;
	deffect->nNBCutoff = NB_CUTOFF_DEFAULT;
	deffect->nNBBoost = NB_BOOST_DEFAULT;
	deffect->nNBLevel = NB_LEVEL_DEFAULT;

	deffect->nUPSampleEnable = 0;

	deffect->nMupEnable = 0;

	deffect->nGEQEnable = 0;
	deffect->nGEQNband = GEQ_NBANDS_DEFAULT;
	deffect->nGEQPreamp = GEQ_PREAMP_DEFAULT;
	deffect->nGEQMaxBoost = GEQ_MAXBOOST_DEFAULT;

	deffect->nGEQBand1 = GEQ_BAND1_DEFAULT;
	deffect->nGEQBand2 = GEQ_BAND2_DEFAULT;
	deffect->nGEQBand3 = GEQ_BAND3_DEFAULT;
	deffect->nGEQBand4 = GEQ_BAND4_DEFAULT;
	deffect->nGEQBand5 = GEQ_BAND5_DEFAULT;
	deffect->nGEQBand6 = GEQ_BAND6_DEFAULT;
	deffect->nGEQBand7 = GEQ_BAND7_DEFAULT;

	deffect->nInChannel = 2;
	deffect->nInChanIndex = CHANS_L_R;
	deffect->nInSampleRate = 44100;
	deffect->nInSampleIndex = SAMPLERATE_44100;

	if(deffect->nUPSampleEnable)
	{
		deffect->nOutSampleIndex = (int) tdas_resampler_get_output_samplerate((int) deffect->nInSampleIndex);
		deffect->nUpsampleParam = (int) tdas_resampler_get_multiplier((int) deffect->nInSampleIndex);
	}
	else
	{
		deffect->nOutSampleIndex = deffect->nInSampleIndex;
		deffect->nUpsampleParam = SAMPLERATE_MULTIPLIER_1;
	}

	deffect->nUpsampleNum = tdas_resampler_real_multiplier((int) deffect->nUpsampleParam);

	deffect->nOutSampleRate = deffect->nUpsampleNum*deffect->nInSampleRate;

	deffect->nOutChannel = 2;
	deffect->nOutChanIndex = CHANS_L_R;

	deffect->nPortableModeEnable = 0;
	deffect->nPortableModeGain = TDAS_PORTABLEMODE_GAIN_DEF;
	deffect->nPortableModeOrl = TDAS_PORTABLEMODE_ORL_DEF;

	deffect->nPulseDownMix = 0;
	deffect->nPulseDownMixAtten = TDAS_PULSEDOWNMIX_ATTEN_DEF;

	tdas_reset(deffect->hDEffect);

	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_SAMPLERATE, deffect->nOutSampleIndex);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_CHANS, deffect->nInChanIndex);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_MATRIX, deffect->nInMatrix);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_RATE_MULTIPLIER, deffect->nUpsampleParam);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_CHAN_FORMAT, deffect->nChanFmt);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_LFE_MIX_LEVEL, deffect->nLfeMixLevel);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_MSR_MAX_PROFILE, deffect->nMSRMaxProfile);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_BYPASS, deffect->nBypass);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE, deffect->nPortableModeEnable);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE_GAIN, deffect->nPortableModeGain);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE_ORL, deffect->nPortableModeOrl);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PULSEDOWNMIX, deffect->nPulseDownMix);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PULSEDOWNMIX_ATTEN, deffect->nPulseDownMixAtten);
	tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_OUT_CHANS, deffect->nOutChanIndex);


	if(interMem)
	{
		deffect->voMemoprator.Alloc = cmnMemAlloc;
		deffect->voMemoprator.Copy = cmnMemCopy;
		deffect->voMemoprator.Free = cmnMemFree;
		deffect->voMemoprator.Set = cmnMemSet;
		deffect->voMemoprator.Check = cmnMemCheck;
		deffect->voMemoprator.Move = cmnMemMove;
		pMemOP = &deffect->voMemoprator;
	}
	deffect->pvoMemop = pMemOP;

	*phCodec = (void *)deffect;

	return VO_ERR_NONE;
}

/**
* Set input audio data.
* \param hCodec [IN]] The Codec Handle which was created by Init function.
* \param pInput [IN] The input buffer param.
* \param pOutBuffer [OUT] The output buffer info.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voEFTDOLBYSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	DOLBYEffectInfo     *deffect;
	
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	if(NULL == pInput || NULL == pInput->Buffer || 0 > pInput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	deffect = (DOLBYEffectInfo *)hCodec;

	deffect->buf_ptr    = pInput->Buffer;
	deffect->buf_len    = pInput->Length;
	deffect->used_len   = 0;

	return VO_ERR_NONE;
}

/**
* Get the outut audio data
* \param hCodec [IN]] The Codec Handle which was created by Init function.
* \param pOutBuffer [OUT] The output audio data
* \param pOutInfo [OUT] The dec module filled audio format and used the input size.
*						 pOutInfo->InputUsed is total used the input size.
* \retval  VO_ERR_NONE Succeeded.
*			VO_ERR_INPUT_BUFFER_SMALL. The input was finished or the input data was not enought.
*/
VO_U32 VO_API voEFTDOLBYGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO * pOutInfo)
{
	DOLBYEffectInfo     *deffect;
	VO_S16 *in, *out;
	VO_S32 n_samples;
	VO_S32 out_rate_multiplier;
	VO_S32 nInChannel;
	VO_S32 nOutChannel;
	VO_S32 ret;
	
	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	if(NULL == pOutput || NULL == pOutput->Buffer || 0 > pOutput->Length)
	{
		return VO_ERR_INVALID_ARG;
	}

	deffect = (DOLBYEffectInfo *)hCodec;

	if(deffect->buf_len <= 0)
	{
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	
	in = (VO_S16 *)deffect->buf_ptr;
	n_samples = deffect->buf_len;

	n_samples = n_samples / (deffect->nInChannel * sizeof(VO_S16));

	nInChannel = deffect->nInChannel;
	nOutChannel = deffect->nOutChannel;

	out = (VO_S16 *)pOutput->Buffer;
//======Add on 2013/09/12 by Jonathon======//
//====when disable dolby effect, output directly====//

	if ((nInChannel<=2)&&(deffect->nEffect==0))
	{
		int i=0,n=0;
		for (i=0;i<(deffect->buf_len/sizeof(VO_S16));i++)
		{
			out[n++]=in[i];
		}
		pOutput->Length = deffect->buf_len;
		deffect->buf_len = 0;
		if(pOutInfo)
		{
			pOutInfo->Format.Channels   = deffect->nOutChannel;
			pOutInfo->Format.SampleBits = 16;
			pOutInfo->Format.SampleRate = deffect->nOutSampleRate;
			pOutInfo->InputUsed        = deffect->buf_len;
		}
		return VO_ERR_NONE;
	}
//==========End==================//
	out_rate_multiplier = tdas_resampler_real_multiplier(tdas_get(deffect->hDEffect, TDAS_BUNDLE, TDAS_RATE_MULTIPLIER));

	if(pOutput->Length < (VO_U32)(deffect->buf_len * out_rate_multiplier * nOutChannel / nInChannel))
		return VO_ERR_OUTPUT_BUFFER_SMALL;

	if(!(n_samples & 63))
	{
		ret = tdas_process(deffect->hDEffect, in, out, n_samples);
		if(ret)
		{
			pOutput->Length = 0;
			deffect->buf_len = 0;
			return ret;
		}
	}
	else
	{
		int i, j, n = 0;
		if(nInChannel == 1) {
			for(i = 0; i < n_samples; i++)
			{
				for(j = 0; j < out_rate_multiplier; j++) {
					out[n++] = in[i];
					out[n++] = in[i];
				}
			}
		}
		else if(nInChannel == 2)
		{
			for(i = 0; i < n_samples*nInChannel; i += nInChannel)
			{
				for(j = 0; j < out_rate_multiplier; j++) {
					out[n++] = in[i];
					out[n++] = in[i+1];
				}
			}
		}
		else
		{
			for(i = 0; i < n_samples*nInChannel; i += nInChannel)
			{
				for(j = 0; j < out_rate_multiplier; j++) {
					out[n++] = in[i];
					out[n++] = in[i+2];
				}
			}
		}
	}

	pOutput->Length = deffect->buf_len * out_rate_multiplier * nOutChannel / nInChannel;
	deffect->buf_len = 0;

	if(pOutInfo)
	{
		pOutInfo->Format.Channels   = deffect->nOutChannel;
		pOutInfo->Format.SampleBits = 16;
		pOutInfo->Format.SampleRate = deffect->nOutSampleRate;
		pOutInfo->InputUsed        = deffect->buf_len;
	}

	return VO_ERR_NONE;
}

/**
* Uninit the Codec.
* \param hCodec [IN]] The Codec Handle which was created by Init function.
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voEFTDOLBYUninit(VO_HANDLE hCodec)
{
	DOLBYEffectInfo     *deffect;
	VO_MEM_OPERATOR *pMemOP;

	if(NULL == hCodec)
	{
		return VO_ERR_INVALID_ARG;
	}

	deffect = (DOLBYEffectInfo *)hCodec;
	pMemOP  = (VO_MEM_OPERATOR *)deffect->pvoMemop;

	tdas_close(deffect->hDEffect);

	pMemOP->Free(VO_INDEX_EFT_DOLBY, deffect);


	return VO_ERR_NONE;	
}

/**
* Set the param for special target.
* \param hCodec [IN]] The Codec Handle which was created by Init function.
* \param uParamID [IN] The param ID.
* \param pData [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voEFTDOLBYSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	DOLBYEffectInfo     *deffect;
	VO_AUDIO_FORMAT     *pFormat;
	EFFECT_DOLBY_PARAM  *pDolbyParam;
	int nEnable;

	if(NULL == hCodec || NULL == pData)
	{
		return VO_ERR_WRONG_STATUS;
	}

	deffect = (DOLBYEffectInfo *)hCodec;

	switch(uParamID)
	{
	case VO_PID_AUDIO_FORMAT:
		pFormat = (VO_AUDIO_FORMAT *)pData;

		deffect->nInChannel = pFormat->Channels;
		deffect->nInSampleRate = pFormat->SampleRate;

		switch(deffect->nInChannel)
		{
		case 1:
			deffect->nInChanIndex = CHANS_C;
			break;
		case 2:
			deffect->nInChanIndex = CHANS_L_R;
			break;
		case 3:
			deffect->nInChanIndex = CHANS_L_C_R;
			break;
		case 4:
			deffect->nInChanIndex = CHANS_L_R_Ls_Rs;
			break;
		case 5:
			deffect->nInChanIndex = CHANS_L_C_R_Ls_Rs;
			break;
		case 6:
			deffect->nInChanIndex = CHANS_L_C_R_Ls_Rs_LFE;
			break;
		}

		switch(deffect->nInSampleRate)
		{
		case 48000:
			deffect->nInSampleIndex = SAMPLERATE_48000;
			break;
		case 44100:
			deffect->nInSampleIndex = SAMPLERATE_44100;
			break;
		case 32000:
			deffect->nInSampleIndex = SAMPLERATE_32000;
			break;
		case 24000:
			deffect->nInSampleIndex = SAMPLERATE_24000;
			break;
		case 22050:
			deffect->nInSampleIndex = SAMPLERATE_22050;
			break;
		case 16000:
			deffect->nInSampleIndex = SAMPLERATE_16000;
			break;
		case 12000:
			deffect->nInSampleIndex = SAMPLERATE_12000;
			break;
		case 11025:
			deffect->nInSampleIndex = SAMPLERATE_11025;
			break;
		case 8000:
			deffect->nInSampleIndex = SAMPLERATE_8000;
			break;
		}

		if(deffect->nUPSampleEnable)
		{
			deffect->nOutSampleIndex = (int) tdas_resampler_get_output_samplerate((int) deffect->nInSampleIndex);
			deffect->nUpsampleParam = (int) tdas_resampler_get_multiplier((int) deffect->nInSampleIndex);
		}
		else
		{
			deffect->nOutSampleIndex = deffect->nInSampleIndex;
			deffect->nUpsampleParam = SAMPLERATE_MULTIPLIER_1;
		}

		deffect->nUpsampleNum = tdas_resampler_real_multiplier((int) deffect->nUpsampleParam);

		deffect->nOutSampleRate = deffect->nUpsampleNum*deffect->nInSampleRate;

		deffect->nOutChannel = 2;
		deffect->nOutChanIndex = CHANS_L_R;

		deffect->nPortableModeEnable = 0;
		deffect->nPortableModeGain = TDAS_PORTABLEMODE_GAIN_DEF;
		deffect->nPortableModeOrl = TDAS_PORTABLEMODE_ORL_DEF;

		deffect->nPulseDownMix = 0;
		deffect->nPulseDownMixAtten = TDAS_PULSEDOWNMIX_ATTEN_DEF;

		tdas_reset(deffect->hDEffect);

		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_SAMPLERATE, deffect->nOutSampleIndex);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_CHANS, deffect->nInChanIndex);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_MATRIX, deffect->nInMatrix);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_RATE_MULTIPLIER, deffect->nUpsampleParam);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_CHAN_FORMAT, deffect->nChanFmt);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_LFE_MIX_LEVEL, deffect->nLfeMixLevel);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_MSR_MAX_PROFILE, deffect->nMSRMaxProfile);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_BYPASS, deffect->nBypass);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE, deffect->nPortableModeEnable);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE_GAIN, deffect->nPortableModeGain);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE_ORL, deffect->nPortableModeOrl);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PULSEDOWNMIX, deffect->nPulseDownMix);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PULSEDOWNMIX_ATTEN, deffect->nPulseDownMixAtten);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_OUT_CHANS, deffect->nOutChanIndex);
		
		break;
	case VO_PID_COMMON_FLUSH:
		break;
	case VO_PID_EFFDOLBY_DEFAULT:
		nEnable = *((int *)pData);
		if(nEnable == 0)
		{
			deffect->nEffect = 0;
			deffect->nBypass = 1;

			deffect->nHFEEnable = 0;
			deffect->nHFEDepth = HFE_DEPTH_DEFAULT;

			deffect->nM2SEnable = 0;

			deffect->nMSREnable = 0;
			deffect->nMSRMaxProfile = TDAS_MSR_DEF_MAX_PROFILE;		
			deffect->nMSRBright = MSR_BRIGHT_DEFAULT;	
			deffect->nMSRRoom = MSR_ROOM_DEFAULT;	

			deffect->nNBEnable = 0;
			deffect->nNBCutoff = NB_CUTOFF_DEFAULT;
			deffect->nNBBoost = NB_BOOST_DEFAULT;
			deffect->nNBLevel = NB_LEVEL_DEFAULT;

			deffect->nUPSampleEnable = 0;

			deffect->nMupEnable = 0;

			deffect->nGEQEnable = 0;
			deffect->nGEQNband = GEQ_NBANDS_DEFAULT;
			deffect->nGEQPreamp = GEQ_PREAMP_DEFAULT;
			deffect->nGEQMaxBoost = GEQ_MAXBOOST_DEFAULT;

			deffect->nGEQBand1 = GEQ_BAND1_DEFAULT;
			deffect->nGEQBand2 = GEQ_BAND2_DEFAULT;
			deffect->nGEQBand3 = GEQ_BAND3_DEFAULT;
			deffect->nGEQBand4 = GEQ_BAND4_DEFAULT;
			deffect->nGEQBand5 = GEQ_BAND5_DEFAULT;
			deffect->nGEQBand6 = GEQ_BAND6_DEFAULT;
			deffect->nGEQBand7 = GEQ_BAND7_DEFAULT;

			if(deffect->nUPSampleEnable)
			{
				deffect->nOutSampleIndex = (int) tdas_resampler_get_output_samplerate((int) deffect->nInSampleIndex);
				deffect->nUpsampleParam = (int) tdas_resampler_get_multiplier((int) deffect->nInSampleIndex);
			}
			else
			{
				deffect->nOutSampleIndex = deffect->nInSampleIndex;
				deffect->nUpsampleParam = SAMPLERATE_MULTIPLIER_1;
			}

			deffect->nUpsampleNum = tdas_resampler_real_multiplier((int) deffect->nUpsampleParam);

			deffect->nOutSampleRate = deffect->nUpsampleNum*deffect->nInSampleRate;
//=======Modified by Jonathon on 2013/09/18==============//
#if 0
			deffect->nOutChannel = 2;
#else
			if (deffect->nInChannel>=2)
			{
				deffect->nOutChannel = 2;
			}
			else		
				deffect->nOutChannel=deffect->nInChannel;
#endif
			deffect->nOutChanIndex = CHANS_L_R;

			deffect->nPortableModeEnable = 0;
			deffect->nPortableModeGain = TDAS_PORTABLEMODE_GAIN_DEF;
			deffect->nPortableModeOrl = TDAS_PORTABLEMODE_ORL_DEF;

			deffect->nPulseDownMix = 0;
			deffect->nPulseDownMixAtten = TDAS_PULSEDOWNMIX_ATTEN_DEF;

			tdas_reset(deffect->hDEffect);

			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_SAMPLERATE, deffect->nOutSampleIndex);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_CHANS, deffect->nInChanIndex);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_MATRIX, deffect->nInMatrix);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_RATE_MULTIPLIER, deffect->nUpsampleParam);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_CHAN_FORMAT, deffect->nChanFmt);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_LFE_MIX_LEVEL, deffect->nLfeMixLevel);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_MSR_MAX_PROFILE, deffect->nMSRMaxProfile);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_BYPASS, deffect->nBypass);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE, deffect->nPortableModeEnable);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE_GAIN, deffect->nPortableModeGain);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE_ORL, deffect->nPortableModeOrl);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PULSEDOWNMIX, deffect->nPulseDownMix);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PULSEDOWNMIX_ATTEN, deffect->nPulseDownMixAtten);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_OUT_CHANS, deffect->nOutChanIndex);
		}
		else
		{
			deffect->nEffect = 1;
			deffect->nBypass = 0;

			deffect->nHFEEnable = 1;
			deffect->nHFEDepth = HFE_DEPTH_DEFAULT;

			deffect->nM2SEnable = 1;

			deffect->nMSREnable = 1;
			deffect->nMSRMaxProfile = TDAS_MSR_DEF_MAX_PROFILE;		
			deffect->nMSRBright = MSR_BRIGHT_DEFAULT;	
			deffect->nMSRRoom = MSR_ROOM_DEFAULT;	

			deffect->nNBEnable = 1;
			deffect->nNBCutoff = NB_CUTOFF_DEFAULT;
			deffect->nNBBoost = NB_BOOST_DEFAULT;
			deffect->nNBLevel = NB_LEVEL_DEFAULT;

			deffect->nUPSampleEnable = 1;

			deffect->nMupEnable = 1;

			deffect->nGEQEnable = 1;
			deffect->nGEQNband = GEQ_NBANDS_DEFAULT;
			deffect->nGEQPreamp = GEQ_PREAMP_DEFAULT;
			deffect->nGEQMaxBoost = GEQ_MAXBOOST_DEFAULT;

			deffect->nGEQBand1 = GEQ_BAND1_DEFAULT;
			deffect->nGEQBand2 = GEQ_BAND2_DEFAULT;
			deffect->nGEQBand3 = GEQ_BAND3_DEFAULT;
			deffect->nGEQBand4 = GEQ_BAND4_DEFAULT;
			deffect->nGEQBand5 = GEQ_BAND5_DEFAULT;
			deffect->nGEQBand6 = GEQ_BAND6_DEFAULT;
			deffect->nGEQBand7 = GEQ_BAND7_DEFAULT;

			if(deffect->nUPSampleEnable)
			{
				deffect->nOutSampleIndex = (int) tdas_resampler_get_output_samplerate((int) deffect->nInSampleIndex);
				deffect->nUpsampleParam = (int) tdas_resampler_get_multiplier((int) deffect->nInSampleIndex);
			}
			else
			{
				deffect->nOutSampleIndex = deffect->nInSampleIndex;
				deffect->nUpsampleParam = SAMPLERATE_MULTIPLIER_1;
			}

			deffect->nUpsampleNum = tdas_resampler_real_multiplier((int) deffect->nUpsampleParam);

			deffect->nOutSampleRate = deffect->nUpsampleNum*deffect->nInSampleRate;

			deffect->nOutChannel = 2;
			deffect->nOutChanIndex = CHANS_L_R;

			deffect->nPortableModeEnable = 1;
			deffect->nPortableModeGain = TDAS_PORTABLEMODE_GAIN_DEF;
			deffect->nPortableModeOrl = TDAS_PORTABLEMODE_ORL_DEF;

			deffect->nPulseDownMix = 1;
			deffect->nPulseDownMixAtten = TDAS_PULSEDOWNMIX_ATTEN_DEF;

			tdas_reset(deffect->hDEffect);

			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_SAMPLERATE, deffect->nOutSampleIndex);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_CHANS, deffect->nInChanIndex);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_MATRIX, deffect->nInMatrix);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_RATE_MULTIPLIER, deffect->nUpsampleParam);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_CHAN_FORMAT, deffect->nChanFmt);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_LFE_MIX_LEVEL, deffect->nLfeMixLevel);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_MSR_MAX_PROFILE, deffect->nMSRMaxProfile);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_BYPASS, deffect->nBypass);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE, deffect->nPortableModeEnable);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE_GAIN, deffect->nPortableModeGain);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE_ORL, deffect->nPortableModeOrl);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PULSEDOWNMIX, deffect->nPulseDownMix);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PULSEDOWNMIX_ATTEN, deffect->nPulseDownMixAtten);
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_OUT_CHANS, deffect->nOutChanIndex);

			//Mobile Surround
			tdas_set (deffect->hDEffect, TDAS_MSR,	   MSR_ENABLE, deffect->nMSREnable);
			if(deffect->nMSREnable) {
				tdas_set (deffect->hDEffect, TDAS_MSR,	   MSR_ROOM, deffect->nMSRRoom);
				tdas_set (deffect->hDEffect, TDAS_MSR,	   MSR_BRIGHT, deffect->nMSRBright);
			}

			//Mobile Surround Upmixer
			tdas_set (deffect->hDEffect, TDAS_MUP,	   MUP_ENABLE, deffect->nMupEnable);

			//High Frequency Enhancer
			tdas_set (deffect->hDEffect, TDAS_HFE, HFE_ENABLE, deffect->nHFEEnable);
			if(deffect->nHFEEnable) {
				tdas_set (deffect->hDEffect, TDAS_HFE, HFE_DEPTH, deffect->nHFEDepth);
			}

			//Mono-to-Stereo Creator 
			//tdas_set (deffect->hDEffect, TDAS_M2S, M2S_ENABLE, deffect->nM2SEnable);

			//Natural Bass
			tdas_set (deffect->hDEffect, TDAS_NB, NB_ENABLE, deffect->nNBEnable);
			if(deffect->nNBEnable) {
				tdas_set (deffect->hDEffect, TDAS_NB, NB_CUTOFF, deffect->nNBCutoff);
				tdas_set (deffect->hDEffect, TDAS_NB, NB_BOOST,deffect->nNBBoost);
				tdas_set (deffect->hDEffect, TDAS_NB, NB_LEVEL, deffect->nMSRRoom);
			}

			// Graphic EQ		
			tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_ENABLE, deffect->nGEQEnable);
			if(deffect->nGEQEnable)
			{
				tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_NBANDS, deffect->nGEQNband);
				tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_PREAMP, deffect->nGEQPreamp);
				tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_MAXBOOST, deffect->nGEQMaxBoost);

				tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND1, deffect->nGEQBand1);
				tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND2, deffect->nGEQBand2);
				tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND3, deffect->nGEQBand3);
				tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND4, deffect->nGEQBand4);
				tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND5, deffect->nGEQBand5);
				tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND6, deffect->nGEQBand6);
				tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND7, deffect->nGEQBand7);
			}
		}
		break;
	case VO_PID_AUDIO_EFFECTCONFIG:
		pDolbyParam = (EFFECT_DOLBY_PARAM  *)pData;

		deffect->nHFEEnable = pDolbyParam->HFEEnable;
		deffect->nHFEDepth = pDolbyParam->HFEDepth;

		deffect->nM2SEnable = pDolbyParam->M2SEnable;

		deffect->nMSREnable = pDolbyParam->MSREnable;
		deffect->nMSRBright = pDolbyParam->MSRBright;	
		deffect->nMSRRoom = pDolbyParam->MSRRoom;	

		deffect->nNBEnable = pDolbyParam->NBEnable;
		deffect->nNBCutoff = pDolbyParam->NBCutoff;
		deffect->nNBBoost = pDolbyParam->NBBoost;
		deffect->nNBLevel = pDolbyParam->NBLevel;

		deffect->nUPSampleEnable = pDolbyParam->UPSampleEnable;

		deffect->nMupEnable = pDolbyParam->MUPEnable;

		deffect->nGEQEnable = pDolbyParam->GEQEnable;
		deffect->nGEQNband = pDolbyParam->GEQNband;
		deffect->nGEQPreamp = pDolbyParam->GEQPreamp;
		deffect->nGEQMaxBoost = pDolbyParam->GEQMaxBoost;

		deffect->nGEQBand1 = pDolbyParam->GEQBand1;
		deffect->nGEQBand2 = pDolbyParam->GEQBand2;
		deffect->nGEQBand3 = pDolbyParam->GEQBand3;
		deffect->nGEQBand4 = pDolbyParam->GEQBand4;
		deffect->nGEQBand5 = pDolbyParam->GEQBand5;
		deffect->nGEQBand6 = pDolbyParam->GEQBand6;
		deffect->nGEQBand7 = pDolbyParam->GEQBand7;

		deffect->nPortableModeEnable = pDolbyParam->PortableModeEnable;
		deffect->nPortableModeGain = pDolbyParam->PortableModeGain;
		deffect->nPortableModeOrl = pDolbyParam->PortableModeOrl;

		deffect->nPulseDownMix = pDolbyParam->PulseDownMix;
		deffect->nPulseDownMixAtten = pDolbyParam->PulseDownMixAtten;

		if(deffect->nUPSampleEnable)
		{
			deffect->nOutSampleIndex = (int) tdas_resampler_get_output_samplerate((int) deffect->nInSampleIndex);
			deffect->nUpsampleParam = (int) tdas_resampler_get_multiplier((int) deffect->nInSampleIndex);
		}
		else
		{
			deffect->nOutSampleIndex = deffect->nInSampleIndex;
			deffect->nUpsampleParam = SAMPLERATE_MULTIPLIER_1;
		}

		deffect->nUpsampleNum = tdas_resampler_real_multiplier((int) deffect->nUpsampleParam);

		deffect->nOutSampleRate = deffect->nUpsampleNum*deffect->nInSampleRate;

		deffect->nOutChannel = 2;
		deffect->nOutChanIndex = CHANS_L_R;

		tdas_reset(deffect->hDEffect);

		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_SAMPLERATE, deffect->nOutSampleIndex);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_CHANS, deffect->nInChanIndex);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_MATRIX, deffect->nInMatrix);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_RATE_MULTIPLIER, deffect->nUpsampleParam);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_IN_CHAN_FORMAT, deffect->nChanFmt);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_LFE_MIX_LEVEL, deffect->nLfeMixLevel);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_MSR_MAX_PROFILE, deffect->nMSRMaxProfile);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE, deffect->nPortableModeEnable);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE_GAIN, deffect->nPortableModeGain);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PORTABLEMODE_ORL, deffect->nPortableModeOrl);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PULSEDOWNMIX, deffect->nPulseDownMix);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_PULSEDOWNMIX_ATTEN, deffect->nPulseDownMixAtten);
		tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_OUT_CHANS, deffect->nOutChanIndex);


		deffect->nEffect = deffect->nHFEEnable | deffect->nM2SEnable | deffect->nMSREnable | deffect->nUPSampleEnable | deffect->nMupEnable 
			| deffect->nGEQEnable | deffect->nMSREnable;

		if(deffect->nEffect == 0)
		{
			deffect->nBypass = 1;
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_BYPASS, deffect->nBypass);	
			return VO_ERR_NONE;
		}
		else
		{
			deffect->nBypass = 0;
			tdas_set (deffect->hDEffect, TDAS_BUNDLE, TDAS_BYPASS, deffect->nBypass);	
		}

		//Mobile Surround
		tdas_set (deffect->hDEffect, TDAS_MSR,	   MSR_ENABLE, deffect->nMSREnable);
		if(deffect->nMSREnable) {
			tdas_set (deffect->hDEffect, TDAS_MSR,	   MSR_ROOM, deffect->nMSRRoom);
			tdas_set (deffect->hDEffect, TDAS_MSR,	   MSR_BRIGHT, deffect->nMSRBright);
		}

		//Mobile Surround Upmixer
		tdas_set (deffect->hDEffect, TDAS_MUP,	   MUP_ENABLE, deffect->nMupEnable);

		//High Frequency Enhancer
		tdas_set (deffect->hDEffect, TDAS_HFE, HFE_ENABLE, deffect->nHFEEnable);
		if(deffect->nHFEEnable) {
			tdas_set (deffect->hDEffect, TDAS_HFE, HFE_DEPTH, deffect->nHFEDepth);
		}

		//Mono-to-Stereo Creator 
		//tdas_set (deffect->hDEffect, TDAS_M2S, M2S_ENABLE, deffect->nM2SEnable);

		//Natural Bass
		tdas_set (deffect->hDEffect, TDAS_NB, NB_ENABLE, deffect->nNBEnable);
		if(deffect->nNBEnable) {
			tdas_set (deffect->hDEffect, TDAS_NB, NB_CUTOFF, deffect->nNBCutoff);
			tdas_set (deffect->hDEffect, TDAS_NB, NB_BOOST,deffect->nNBBoost);
			tdas_set (deffect->hDEffect, TDAS_NB, NB_LEVEL, deffect->nMSRRoom);
		}

		// Graphic EQ		
		tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_ENABLE, deffect->nGEQEnable);
		if(deffect->nGEQEnable)
		{
			tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_NBANDS, deffect->nGEQNband);
			tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_PREAMP, deffect->nGEQPreamp);
			tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_MAXBOOST, deffect->nGEQMaxBoost);

			tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND1, deffect->nGEQBand1);
			tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND2, deffect->nGEQBand2);
			tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND3, deffect->nGEQBand3);
			tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND4, deffect->nGEQBand4);
			tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND5, deffect->nGEQBand5);
			tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND6, deffect->nGEQBand6);
			tdas_set (deffect->hDEffect, TDAS_GEQ, GEQ_BAND7, deffect->nGEQBand7);
		}

		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

/**
* Get the param for special target.
* \param hCodec [IN]] The Codec Handle which was created by Init function.
* \param uParamID [IN] The param ID.
* \param pData [IN] The param value depend on the ID>
* \retval VO_ERR_NONE Succeeded.
*/
VO_U32 VO_API voEFTDOLBYGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)
{
	DOLBYEffectInfo     *deffect;
	VO_AUDIO_FORMAT     *pFormat;
	EFFECT_DOLBY_PARAM  *pDolbyParam;

	if(NULL == hCodec || NULL == pData)
	{
		return VO_ERR_WRONG_STATUS;
	}

	deffect = (DOLBYEffectInfo *)hCodec;

	switch(uParamID)
	{
	case VO_PID_AUDIO_FORMAT:
		pFormat = (VO_AUDIO_FORMAT *)pData;
		pFormat->Channels = deffect->nOutChannel;
		pFormat->SampleRate = deffect->nOutSampleRate;
		pFormat->SampleBits = 16;
		break;
	case VO_PID_AUDIO_EFFECTMODE:
		*((VO_AUDIO_EFFECTTYPE *)pData) = VO_AUDIO_DolbyMoblie;
		break;
	case VO_PID_AUDIO_EFFECTCONFIG:
		pDolbyParam = (EFFECT_DOLBY_PARAM  *)pData;

		pDolbyParam->HFEEnable = deffect->nHFEEnable;
		pDolbyParam->HFEDepth = deffect->nHFEDepth;

		pDolbyParam->M2SEnable = deffect->nM2SEnable;

		pDolbyParam->MSREnable = deffect->nMSREnable;
		pDolbyParam->MSRBright = deffect->nMSRBright ;	
		pDolbyParam->MSRRoom = deffect->nMSRRoom;	

		pDolbyParam->NBEnable = deffect->nNBEnable;
		pDolbyParam->NBCutoff = deffect->nNBCutoff ;
		pDolbyParam->NBBoost =deffect->nNBBoost;
		pDolbyParam->NBLevel = deffect->nNBLevel;

		pDolbyParam->UPSampleEnable = deffect->nUPSampleEnable;

		pDolbyParam->MUPEnable = deffect->nMupEnable ;

		pDolbyParam->GEQEnable = deffect->nGEQEnable;
		pDolbyParam->GEQNband = deffect->nGEQNband;
		pDolbyParam->GEQPreamp = deffect->nGEQPreamp;
		pDolbyParam->GEQMaxBoost = deffect->nGEQMaxBoost;

		pDolbyParam->GEQBand1 = deffect->nGEQBand1;
		pDolbyParam->GEQBand2 = deffect->nGEQBand2;
		pDolbyParam->GEQBand3 = deffect->nGEQBand3;
		pDolbyParam->GEQBand4 = deffect->nGEQBand4;
		pDolbyParam->GEQBand5 = deffect->nGEQBand5;
		pDolbyParam->GEQBand6 = deffect->nGEQBand6;
		pDolbyParam->GEQBand7 = deffect->nGEQBand7;
		
		pDolbyParam->PortableModeEnable = deffect->nPortableModeEnable;
		pDolbyParam->PortableModeGain = deffect->nPortableModeGain;
		pDolbyParam->PortableModeOrl = deffect->nPortableModeOrl;

		pDolbyParam->PulseDownMix = deffect->nPulseDownMix;
		pDolbyParam->PulseDownMixAtten = deffect->nPulseDownMixAtten;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

/**
 * Get audio effect API interface
 * \param pEncHandle [out] Return the dolby effect handle.
 * \retval VO_ERR_OK Succeeded.
 */
VO_S32 VO_API voGetDolbyEffectAPI(VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return VO_ERR_INVALID_ARG;
		
	pDecHandle->Init = voEFTDOLBYInit;
	pDecHandle->SetInputData = voEFTDOLBYSetInputData;
	pDecHandle->GetOutputData = voEFTDOLBYGetOutputData;
	pDecHandle->SetParam = voEFTDOLBYSetParam;
	pDecHandle->GetParam = voEFTDOLBYGetParam;
	pDecHandle->Uninit = voEFTDOLBYUninit;

	return VO_ERR_NONE;
}


extern "C"   {
	void __assert_fail(const char* c1, const char*c2, unsigned int n, const char* c3)
	{

		//printf("---->>>> ASSERT");

	}
}
