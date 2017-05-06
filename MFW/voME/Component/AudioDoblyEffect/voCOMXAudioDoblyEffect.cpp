	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioDec.cpp

	Contains:	voCOMXAudioDec class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>

#include "voOMXBase.h"
#include "voOMXMemory.h"
#include "voOMXOSFun.h"
#include "OMX_DolbyExt.h"

#include "voCOMXAudioDoblyEffect.h"
#include "cmnMemory.h"

#include "compiler.h"

#include "target_client.h"
#include "control.h"
#include "tdas_client.h" 
#include "tdas_resampler_helper.h"

#define LOG_TAG "voCOMXAudioDoblyEffect"

#include "voLog.h"

voCOMXAudioDobyEffect::voCOMXAudioDobyEffect(OMX_COMPONENTTYPE * pComponent)
	: voCOMXCompFilter (pComponent)
	, m_pFormatData (NULL)
	, m_nExtSize (0)
	, m_pExtData (NULL)
	, m_nSampleRate (44100)
	, m_nChannels (2)
	, m_nInSampleRate (44100)
	, m_nInChannels (2)
	, m_nBits (16)
	, m_bSetThreadPriority (OMX_FALSE)
	, m_nStartTime (0)
	, m_nStepSize (4410 * 2)
	, m_nBuffSize (0)
	, pCAudioDoblyEffect (NULL)
{
	strcpy (m_pObjName, __FILE__);
	strcpy (m_pName, "OMX.VisualOn.Audio.DoblyEffect.XXX");

	m_inBuffer.Length = 0;

	pCAudioDoblyEffect = new CAudioDEffect;
	if (pCAudioDoblyEffect == NULL)
	{
		VOLOGE ("Create the audio decoder failed.");
	}

	VOLOGF ("Name: %s", m_pName);
}

voCOMXAudioDobyEffect::~voCOMXAudioDobyEffect(void)
{
	VOLOGF ("Name: %s", m_pName);
	
	if(pCAudioDoblyEffect)
	{
		delete pCAudioDoblyEffect;
		pCAudioDoblyEffect = NULL;
	}

//	if (m_pCallBack != NULL)
//		m_pCallBack->EventHandler (m_pComponent, m_pAppData, (OMX_EVENTTYPE)OMX_EventComponentLog, 0, 0, (OMX_PTR)__FUNCTION__);

}

OMX_ERRORTYPE voCOMXAudioDobyEffect::GetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nParamIndex,
											OMX_INOUT OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name: %s, The Param Index 0X%08X.", m_pName, nParamIndex);

	OMX_ERRORTYPE errType = OMX_ErrorNone;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nParamIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			pAudioFormat->eEncoding = OMX_AUDIO_CodingPCM;
			errType = OMX_ErrorNone;
		}
		break;

	case OMX_IndexParamAudioPcm:
		{
			OMX_AUDIO_PARAM_PCMMODETYPE * pAudioPcmMode = (OMX_AUDIO_PARAM_PCMMODETYPE *) pComponentParameterStructure;
			if (pAudioPcmMode->nPortIndex == 1)  
			{
				VOLOGI ("Get Audio Format. S %d, C %d", m_pcmType.nSamplingRate, m_pcmType.nChannels);
				m_pcmType.nChannels = 2;
				m_nOutSampleRate = m_pcmType.nSamplingRate;
				m_nOutChannels = m_pcmType.nChannels;
				voOMXMemCopy (pAudioPcmMode, &m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
				errType = OMX_ErrorNone;
			}
			break;
		}

	default:
		errType = voCOMXCompFilter::GetParameter (hComponent, nParamIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}


OMX_ERRORTYPE voCOMXAudioDobyEffect::SetParameter (OMX_IN  OMX_HANDLETYPE hComponent,
											OMX_IN  OMX_INDEXTYPE nIndex,
											OMX_IN  OMX_PTR pComponentParameterStructure)
{
	VOLOGF ("Name: %s, The Param Index 0X%08X.", m_pName, nIndex);

	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentParameterStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex)
	{
	case OMX_IndexParamAudioPortFormat:
		{
			OMX_AUDIO_PARAM_PORTFORMATTYPE * pAudioFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE *) pComponentParameterStructure;
			errType = CheckParam(pComp, pAudioFormat->nPortIndex, pAudioFormat, sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
			if (errType != OMX_ErrorNone)
			{
				VOLOGE ("Check Param error. 0X%08X", errType);
				return errType;
			}

			if (pAudioFormat->nPortIndex == 0)
			{
				if (pAudioFormat->eEncoding == OMX_AUDIO_CodingPCM)
					errType = OMX_ErrorNone;
				else
				{
					//VOLOGW ("The codeing is " );
					errType = OMX_ErrorComponentNotFound;
				}
			}
		}
		break;

    case OMX_IndexParamAudioPcm:
        {
           OMX_AUDIO_PARAM_PCMMODETYPE* pData = (OMX_AUDIO_PARAM_PCMMODETYPE*)pComponentParameterStructure;
		   errType = CheckParam(pComp, pData->nPortIndex, pData, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
		   if (errType != OMX_ErrorNone)
		   {
			   VOLOGE ("Check Param error. 0X%08X", errType);
			   return errType;
		   }

		   m_nInChannels = pData->nChannels;
		   m_nInSampleRate = pData->nSamplingRate;

		   memcpy(&m_pcmType, pData, sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));

           errType = OMX_ErrorNone;
        }
        break;

	default:
		errType = voCOMXCompFilter::SetParameter (hComponent, nIndex, pComponentParameterStructure);
		break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXAudioDobyEffect::GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_INOUT OMX_PTR pComponentConfigStructure)
{
	VOLOGF ("Name: %s, The Param Index 0X%08X.", m_pName, nIndex);
	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentConfigStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex) {

	  case OMX_IndexConfigDAK_BUNDLE:
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }
		  ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nDAK_Version = 
			 pCAudioDoblyEffect->GetParameter(TDAS_BUNDLE, TDAS_VERSION);
		  ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nDAK_Revision = 
			 pCAudioDoblyEffect->GetParameter(TDAS_BUNDLE, TDAS_REVISION);
		  ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nSampleRateIndex = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_BUNDLE, TDAS_SAMPLERATE);
		  ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nInChans = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_BUNDLE, TDAS_IN_CHANS);
		  ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nInMatrix = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_BUNDLE, TDAS_IN_MATRIX);
		  ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nBypass = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_BUNDLE, TDAS_BYPASS);
		  ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nRateMultipier = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_BUNDLE, TDAS_RATE_MULTIPLIER);
		  ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nInChanFormat = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_BUNDLE, TDAS_IN_CHAN_FORMAT);
		  ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nLfeMixLevel = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_BUNDLE, TDAS_LFE_MIX_LEVEL);
		  ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nMsrMaxProfile = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_BUNDLE, TDAS_MSR_MAX_PROFILE);
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_M2S:
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }
		  ((OMX_DAK_CONFIG_M2STYPE *)pComponentConfigStructure)-> bEnable = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_M2S, M2S_ENABLE);
		  ((OMX_DAK_CONFIG_M2STYPE *)pComponentConfigStructure)-> bDetector = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_M2S, M2S_DETECTOR);
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_SSE:
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }
		  ((OMX_DAK_CONFIG_SSETYPE *)pComponentConfigStructure)-> bEnable = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_SSE, SSE_ENABLE);

		  ((OMX_DAK_CONFIG_SSETYPE *)pComponentConfigStructure)-> nWidth = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_SSE, SSE_WIDTH);

		  ((OMX_DAK_CONFIG_SSETYPE *)pComponentConfigStructure)-> nSpkMode = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_SSE, SSE_SPK_MODE);
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_SLC:
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }
		  ((OMX_DAK_CONFIG_SLCTYPE *)pComponentConfigStructure)-> bEnable = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_SLC, SLC_ENABLE);

		  ((OMX_DAK_CONFIG_SLCTYPE *)pComponentConfigStructure)-> nLevel = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_SLC, SLC_LEVEL);

		  ((OMX_DAK_CONFIG_SLCTYPE *)pComponentConfigStructure)-> nDepth = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_SLC, SLC_DEPTH);
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_VOL:
		  if(pCAudioDoblyEffect == NULL) {
			  break;
		  }
		  ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> bEnable = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_VOL, VOL_ENABLE);

		  ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> nGainType = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_VOL, VOL_GAIN_TYPE);

		  ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> nInternalGain = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_VOL, VOL_GAIN_INTERNAL);

		  ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> nExternalGain = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_VOL, VOL_GAIN_EXTERNAL);

		  ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> nBalance = 
			  (OMX_S8)pCAudioDoblyEffect->GetParameter(TDAS_VOL, VOL_BALANCE);

		  ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> bMute = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_VOL, VOL_MUTE);
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_NB:
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }
		  ((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> bEnable = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_NB, NB_ENABLE);

		  ((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> nCutoff = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_NB, NB_CUTOFF);

		  ((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> nBoost = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_NB, NB_BOOST);

		  ((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> nLevel = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_NB, NB_LEVEL);

		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_GEQ:
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }
		  ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> bEnable = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_GEQ, GEQ_ENABLE);

		  ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nNbands = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_GEQ, GEQ_NBANDS);

		  ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nPreamp = 
			  (OMX_S8)pCAudioDoblyEffect->GetParameter(TDAS_GEQ, GEQ_PREAMP);

		  ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nMaxboost = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_GEQ, GEQ_MAXBOOST);

		  ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand1 = 
			  (OMX_S8)pCAudioDoblyEffect->GetParameter(TDAS_GEQ, GEQ_BAND1);

		  ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand2 = 
			  (OMX_S8)pCAudioDoblyEffect->GetParameter(TDAS_GEQ, GEQ_BAND2);

		  ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand3 = 
			  (OMX_S8)pCAudioDoblyEffect->GetParameter(TDAS_GEQ, GEQ_BAND3);

		  ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand4 = 
			  (OMX_S8)pCAudioDoblyEffect->GetParameter(TDAS_GEQ, GEQ_BAND4);

		  ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand5 = 
			  (OMX_S8)pCAudioDoblyEffect->GetParameter(TDAS_GEQ, GEQ_BAND5);

		  ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand6 = 
			  (OMX_S8)pCAudioDoblyEffect->GetParameter(TDAS_GEQ, GEQ_BAND6);

		  ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand7 = 
			  (OMX_S8)pCAudioDoblyEffect->GetParameter(TDAS_GEQ, GEQ_BAND7);

		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_MSR:
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }
		  ((OMX_DAK_CONFIG_MSRTYPE *)pComponentConfigStructure)-> bMsrEnable = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_MSR, MSR_ENABLE);

		  ((OMX_DAK_CONFIG_MSRTYPE *)pComponentConfigStructure)-> nMsrRoom = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_MSR, MSR_ROOM);

		  ((OMX_DAK_CONFIG_MSRTYPE *)pComponentConfigStructure)-> nMsrBright = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_MSR, MSR_BRIGHT);

		  ((OMX_DAK_CONFIG_MSRTYPE *)pComponentConfigStructure)-> bMupEnable = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_MUP, MUP_ENABLE);

		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_HFE:
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }
		  ((OMX_DAK_CONFIG_HFETYPE *)pComponentConfigStructure)-> bEnable = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_HFE, HFE_ENABLE);

		  ((OMX_DAK_CONFIG_HFETYPE *)pComponentConfigStructure)-> nDepth = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_HFE, HFE_DEPTH);
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_FADE:
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }
		  ((OMX_DAK_CONFIG_FADETYPE *)pComponentConfigStructure)-> bEnable = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_FADE, FADE_ENABLE);

		  ((OMX_DAK_CONFIG_FADETYPE *)pComponentConfigStructure)-> nTarget = 
			  (OMX_U8)pCAudioDoblyEffect->GetParameter(TDAS_FADE, FADE_TARGET);

		  ((OMX_DAK_CONFIG_FADETYPE *)pComponentConfigStructure)-> nTime = 
			  (OMX_U16)pCAudioDoblyEffect->GetParameter(TDAS_FADE, FADE_TIME);
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_SEQ:
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }
		  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> bEnable = 
			  (OMX_BOOL)pCAudioDoblyEffect->GetParameter(TDAS_SEQ, SPKEQ_ENABLE);

		  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nLeftGainDB = 
			 (OMX_S8)pCAudioDoblyEffect->GetParameter(TDAS_SEQ, SPKEQ_CHGAIN_DB1);

		  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nRightGainDB = 
			  (OMX_S8)pCAudioDoblyEffect->GetParameter(TDAS_SEQ, SPKEQ_CHGAIN_DB2);

		  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef48000Size = 0;
		  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef48000 = NULL;
		  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef44100Size = 0;
		  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef44100 = NULL;
		  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef32000Size = 0;
		  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef32000 = NULL;
		  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef24000Size = 0;
		  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef24000 = NULL;
		  errType = OMX_ErrorNone;
		  break;
	case OMX_IndexConfigDAK_DEFAULT:
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }  
		  pCAudioDoblyEffect->GetParam(VO_PID_AUDIO_CHANNELCONFIG, (VO_PTR)pComponentConfigStructure);
		  errType = OMX_ErrorNone;
		  break;

	  default :
		   errType = voCOMXCompFilter::GetConfig (hComponent, nIndex, pComponentConfigStructure);
		   break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXAudioDobyEffect::SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
												OMX_IN  OMX_INDEXTYPE nIndex,
												OMX_IN  OMX_PTR pComponentConfigStructure)
{
	VOLOGF ("Name: %s, The Param Index 0X%08X.", m_pName, nIndex);
	OMX_ERRORTYPE errType = OMX_ErrorUnsupportedIndex;
	OMX_COMPONENTTYPE * pComp = (OMX_COMPONENTTYPE *)hComponent;

	if(pComponentConfigStructure == NULL)
		return OMX_ErrorBadParameter;

	switch (nIndex) {
	  case OMX_IndexConfigDAK_BUNDLE:
		  if (  ( ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nSampleRateIndex >= N_SAMPLERATES)
			  || ( ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nInChans >= TDAS_N_IN_CHANS)
			  || ( ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nInChans == CHANS_RESERVED)
			  || ( ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nInChans == CHANS_RESERVED_2)
			  || ( ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nInMatrix >= TDAS_N_IN_MATRICES)
			  || ( ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nBypass >= TDAS_N_BYPASSES-1) /* Bypass mode 2 is not supported */
			  || ( ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nRateMultipier >= TDAS_N_SAMPLERATE_MULTIPLIERS)
			  || ( ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nInChanFormat >= TDAS_N_CHAN_FORMATS) 
			  || ( ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nLfeMixLevel > DMX_LFE_MIX_MIN) 
			  || ( ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nMsrMaxProfile >= TDAS_MSR_N_MAX_PROFILES) )
		  {
			  errType = OMX_ErrorBadParameter;
			  break;
		  }

		  if(pCAudioDoblyEffect) {
			pCAudioDoblyEffect->SetParameter(TDAS_BUNDLE, TDAS_SAMPLERATE, ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nSampleRateIndex); 
			pCAudioDoblyEffect->SetParameter(TDAS_BUNDLE, TDAS_IN_CHANS, ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nInChans);
			pCAudioDoblyEffect->SetParameter(TDAS_BUNDLE, TDAS_IN_MATRIX, ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nInMatrix);
			pCAudioDoblyEffect->SetParameter(TDAS_BUNDLE, TDAS_BYPASS, ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nBypass);
			pCAudioDoblyEffect->SetParameter(TDAS_BUNDLE, TDAS_RATE_MULTIPLIER, ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nRateMultipier);
			pCAudioDoblyEffect->SetParameter(TDAS_BUNDLE, TDAS_IN_CHAN_FORMAT, ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nInChanFormat);
			pCAudioDoblyEffect->SetParameter(TDAS_BUNDLE, TDAS_LFE_MIX_LEVEL, ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nLfeMixLevel);
			pCAudioDoblyEffect->SetParameter(TDAS_BUNDLE, TDAS_MSR_MAX_PROFILE, ((OMX_DAK_CONFIG_BUNDLETYPE *)pComponentConfigStructure)-> nMsrMaxProfile);
		  }
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_M2S:
		  if ( ((OMX_DAK_CONFIG_M2STYPE *)pComponentConfigStructure)-> bEnable >= M2S_N_ENABLES)
		  {
			  errType = OMX_ErrorBadParameter;
			  break;
		  }
		  if(pCAudioDoblyEffect) {
			pCAudioDoblyEffect->SetParameter(TDAS_M2S, M2S_ENABLE, ((OMX_DAK_CONFIG_M2STYPE *)pComponentConfigStructure)-> bEnable);
		  }
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_SSE:
		  if (  ( ((OMX_DAK_CONFIG_SSETYPE *)pComponentConfigStructure)-> bEnable >= SSE_N_ENABLES)
			  || ( ((OMX_DAK_CONFIG_SSETYPE *)pComponentConfigStructure)-> nWidth >= SSE_N_WIDTHS)
			  || ( ((OMX_DAK_CONFIG_SSETYPE *)pComponentConfigStructure)-> nSpkMode >= SSE_N_SPK_MODES) )
		  {
			  errType = OMX_ErrorBadParameter;
			  break;
		  }
		  if(pCAudioDoblyEffect) {
			pCAudioDoblyEffect->SetParameter(TDAS_SSE, SSE_ENABLE, ((OMX_DAK_CONFIG_SSETYPE *)pComponentConfigStructure)-> bEnable);
			pCAudioDoblyEffect->SetParameter(TDAS_SSE, SSE_WIDTH, ((OMX_DAK_CONFIG_SSETYPE *)pComponentConfigStructure)-> nWidth);
			pCAudioDoblyEffect->SetParameter(TDAS_SSE, SSE_SPK_MODE, ((OMX_DAK_CONFIG_SSETYPE *)pComponentConfigStructure)-> nSpkMode);
		  }
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_SLC:
		  if (  ( ((OMX_DAK_CONFIG_SLCTYPE *)pComponentConfigStructure)-> bEnable >= SLC_N_ENABLES)
			  || ( ((OMX_DAK_CONFIG_SLCTYPE *)pComponentConfigStructure)-> nLevel >= SLC_N_LEVELS)
			  || ( ((OMX_DAK_CONFIG_SLCTYPE *)pComponentConfigStructure)-> nDepth >= SLC_N_DEPTHS) )
		  {
			  errType = OMX_ErrorBadParameter;
			  break;
		  }
		  if(pCAudioDoblyEffect) {
			pCAudioDoblyEffect->SetParameter(TDAS_SLC, SLC_ENABLE, ((OMX_DAK_CONFIG_SLCTYPE *)pComponentConfigStructure)-> bEnable);
			pCAudioDoblyEffect->SetParameter(TDAS_SLC, SLC_LEVEL, ((OMX_DAK_CONFIG_SLCTYPE *)pComponentConfigStructure)-> nLevel);
			pCAudioDoblyEffect->SetParameter(TDAS_SLC, SLC_DEPTH, ((OMX_DAK_CONFIG_SLCTYPE *)pComponentConfigStructure)-> nDepth);
		  }
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_VOL:
		  if (  ( ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> bEnable >= VOL_N_ENABLES)
			  || ( ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> nGainType >= VOL_N_GAIN_TYPES)
			  /* can't check nInternalGain, nExternalGain, nBalance boundaries as the value is always in range of 8 bits */
			  || ( ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> nExternalGain && ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> nInternalGain) /* internal volume should be set to 0 when external volume is used */
			  || ( ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> bMute >= VOL_N_MUTES) )
		  {
			  errType = OMX_ErrorBadParameter;
			  break;
		  }
		  if(pCAudioDoblyEffect) {
			pCAudioDoblyEffect->SetParameter(TDAS_VOL, VOL_ENABLE, ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> bEnable);
			pCAudioDoblyEffect->SetParameter(TDAS_VOL, VOL_GAIN_TYPE, ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> nGainType);
			pCAudioDoblyEffect->SetParameter(TDAS_VOL, VOL_GAIN_INTERNAL, ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> nInternalGain);
			pCAudioDoblyEffect->SetParameter(TDAS_VOL, VOL_GAIN_EXTERNAL, ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> nExternalGain);
			pCAudioDoblyEffect->SetParameter(TDAS_VOL, VOL_BALANCE, ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> nBalance);
			pCAudioDoblyEffect->SetParameter(TDAS_VOL, VOL_MUTE, ((OMX_DAK_CONFIG_VOLTYPE *)pComponentConfigStructure)-> bMute);
		  }
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_NB:
		  if (  ( ((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> bEnable >= NB_N_ENABLES)
			  || ( ((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> nCutoff >= NB_N_CUTOFFS)
			  || ( ((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> nBoost >= NB_N_BOOSTS)
			  || ( ((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> nLevel >= NB_N_LEVELS) )
		  {
			  errType = OMX_ErrorBadParameter;
			  break;
		  }
		  if(pCAudioDoblyEffect) {
			pCAudioDoblyEffect->SetParameter(TDAS_NB, NB_ENABLE, ((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> bEnable);
			pCAudioDoblyEffect->SetParameter(TDAS_NB, NB_CUTOFF, ((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> nCutoff);
			pCAudioDoblyEffect->SetParameter(TDAS_NB, NB_BOOST,((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> nBoost);
			pCAudioDoblyEffect->SetParameter(TDAS_NB, NB_LEVEL, ((OMX_DAK_CONFIG_NBTYPE *)pComponentConfigStructure)-> nLevel);
		  }
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_GEQ:
		  if (  ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> bEnable >= GEQ_N_ENABLES)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nNbands < GEQ_N_BANDS_MIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nNbands > GEQ_N_BANDS_MAX)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nPreamp < GEQ_MIN_PREAMP)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nPreamp > GEQ_MAX_PREAMP)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nMaxboost > GEQ_MAX_MAXBOOST) 
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand1 < GEQ_MINGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand1 > GEQ_MAXGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand2 < GEQ_MINGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand2 > GEQ_MAXGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand3 < GEQ_MINGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand3 > GEQ_MAXGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand4 < GEQ_MINGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand4 > GEQ_MAXGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand5 < GEQ_MINGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand5 > GEQ_MAXGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand6 < GEQ_MINGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand6 > GEQ_MAXGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand7 < GEQ_MINGAIN)
			  || ( ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand7 > GEQ_MAXGAIN) )
		  {
			  errType = OMX_ErrorBadParameter;
			  break;
		  }
		  if(pCAudioDoblyEffect) {
			pCAudioDoblyEffect->SetParameter(TDAS_GEQ, GEQ_ENABLE, ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> bEnable);
			pCAudioDoblyEffect->SetParameter(TDAS_GEQ, GEQ_NBANDS, ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nNbands);
			pCAudioDoblyEffect->SetParameter(TDAS_GEQ, GEQ_PREAMP, ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nPreamp);
			pCAudioDoblyEffect->SetParameter(TDAS_GEQ, GEQ_MAXBOOST, ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nMaxboost);
			pCAudioDoblyEffect->SetParameter(TDAS_GEQ, GEQ_BAND1, ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand1);
			pCAudioDoblyEffect->SetParameter(TDAS_GEQ, GEQ_BAND2, ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand2);
			pCAudioDoblyEffect->SetParameter(TDAS_GEQ, GEQ_BAND3, ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand3);
			pCAudioDoblyEffect->SetParameter(TDAS_GEQ, GEQ_BAND4, ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand4);
			pCAudioDoblyEffect->SetParameter(TDAS_GEQ, GEQ_BAND5, ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand5);
			pCAudioDoblyEffect->SetParameter(TDAS_GEQ, GEQ_BAND6, ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand6);
			pCAudioDoblyEffect->SetParameter(TDAS_GEQ, GEQ_BAND7, ((OMX_DAK_CONFIG_GEQTYPE *)pComponentConfigStructure)-> nBand7);
		  }
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_MSR:
		  if ( ( ((OMX_DAK_CONFIG_MSRTYPE *)pComponentConfigStructure)-> bMsrEnable >= MSR_N_ENABLES)
			  || ( ((OMX_DAK_CONFIG_MSRTYPE *)pComponentConfigStructure)-> nMsrRoom >= MSR_N_ROOMS)
			  || ( ((OMX_DAK_CONFIG_MSRTYPE *)pComponentConfigStructure)-> nMsrBright >= MSR_N_BRIGHTS) )
		  {
			  errType = OMX_ErrorBadParameter;
			  break;
		  }
		  if(pCAudioDoblyEffect) {
			pCAudioDoblyEffect->SetParameter(TDAS_MSR, MSR_ENABLE, ((OMX_DAK_CONFIG_MSRTYPE *)pComponentConfigStructure)-> bMsrEnable);
			pCAudioDoblyEffect->SetParameter(TDAS_MSR, MSR_ROOM, ((OMX_DAK_CONFIG_MSRTYPE *)pComponentConfigStructure)-> nMsrRoom);
			pCAudioDoblyEffect->SetParameter(TDAS_MSR, MSR_BRIGHT, ((OMX_DAK_CONFIG_MSRTYPE *)pComponentConfigStructure)-> nMsrBright);
		  }
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_HFE:
		  if (  ( ((OMX_DAK_CONFIG_HFETYPE *)pComponentConfigStructure)-> bEnable >= HFE_N_ENABLES)
			  || ( ((OMX_DAK_CONFIG_HFETYPE *)pComponentConfigStructure)-> nDepth >= HFE_N_DEPTHS) )
		  {
			  errType = OMX_ErrorBadParameter;
			  break;
		  }
		  if(pCAudioDoblyEffect) {
			pCAudioDoblyEffect->SetParameter(TDAS_HFE, HFE_ENABLE, ((OMX_DAK_CONFIG_HFETYPE *)pComponentConfigStructure)-> bEnable);
			pCAudioDoblyEffect->SetParameter(TDAS_HFE, HFE_DEPTH, ((OMX_DAK_CONFIG_HFETYPE *)pComponentConfigStructure)-> nDepth);
		  }
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_FADE:
		  if (  ( ((OMX_DAK_CONFIG_FADETYPE *)pComponentConfigStructure)-> bEnable >= FADE_N_ENABLES)
			  /* can't check nTarget as value is always in range of 8 bits */
			  || ( ((OMX_DAK_CONFIG_FADETYPE *)pComponentConfigStructure)-> nTime < FADE_TIME_MIN)
			  || ( ((OMX_DAK_CONFIG_FADETYPE *)pComponentConfigStructure)-> nTime > FADE_TIME_MAX) )
		  {
			  errType = OMX_ErrorBadParameter;
			  break;
		  }
		  if(pCAudioDoblyEffect) {
			pCAudioDoblyEffect->SetParameter(TDAS_FADE, FADE_ENABLE, ((OMX_DAK_CONFIG_FADETYPE *)pComponentConfigStructure)-> bEnable);
			pCAudioDoblyEffect->SetParameter(TDAS_FADE, FADE_TARGET, ((OMX_DAK_CONFIG_FADETYPE *)pComponentConfigStructure)-> nTarget);
			pCAudioDoblyEffect->SetParameter(TDAS_FADE, FADE_TIME, ((OMX_DAK_CONFIG_FADETYPE *)pComponentConfigStructure)-> nTime);
		  }
		  errType = OMX_ErrorNone;
		  break;

	  case OMX_IndexConfigDAK_SEQ:
		  if (  ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> bEnable >= SPKEQ_N_ENABLES)
			  || ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nLeftGainDB < SPKEQ_MIN_CHGAIN_DB)
			  || ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nLeftGainDB > SPKEQ_MAX_CHGAIN_DB) 
			  || ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nRightGainDB < SPKEQ_MIN_CHGAIN_DB)
			  || ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nRightGainDB > SPKEQ_MAX_CHGAIN_DB) 
			  || ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef48000Size > SPKEQ_COEF_TABLE_SZ*sizeof(COEF)) 
			  || ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef44100Size > SPKEQ_COEF_TABLE_SZ*sizeof(COEF)) 
			  || ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef32000Size > SPKEQ_COEF_TABLE_SZ*sizeof(COEF)) 
			  || ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef24000Size > SPKEQ_COEF_TABLE_SZ*sizeof(COEF)) )
		  {
			  errType = OMX_ErrorBadParameter;
			  break;
		  }
		  if(pCAudioDoblyEffect == NULL) {
			  errType = OMX_ErrorNone;
			  break;
		  }
		  pCAudioDoblyEffect->SetParameter(TDAS_SEQ, SPKEQ_ENABLE, ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> bEnable);
		  pCAudioDoblyEffect->SetParameter(TDAS_SEQ, SPKEQ_CHGAIN_DB1, ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nLeftGainDB);
		  pCAudioDoblyEffect->SetParameter(TDAS_SEQ, SPKEQ_CHGAIN_DB2, ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nRightGainDB);
		  if ( ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef48000Size > 0) &&  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef48000 )
		  {
			  pCAudioDoblyEffect->SetCoef(TDAS_SEQ, SAMPLERATE_48000, 
				  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef48000Size, ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef48000);
		  }	
		  if ( ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef44100Size > 0) &&  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef44100 )
		  {
			  pCAudioDoblyEffect->SetCoef(TDAS_SEQ, SAMPLERATE_44100, 
				  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef44100Size, ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef44100);
		  }	
		  if ( ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef32000Size > 0) &&  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef32000 )
		  {
			  pCAudioDoblyEffect->SetCoef(TDAS_SEQ, SAMPLERATE_32000, 
				  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef32000Size, ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef32000);
		  }	
		  if ( ( ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef24000Size > 0) &&  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef24000 )
		  {
			  pCAudioDoblyEffect->SetCoef(TDAS_SEQ, SAMPLERATE_24000, 
				  ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> nCoef24000Size, ((OMX_DAK_CONFIG_SEQTYPE *)pComponentConfigStructure)-> pCoef24000);
		  }	

		  errType = OMX_ErrorNone;
		  break;
	  case OMX_IndexConfigDAK_DEFAULT:
		  if(pCAudioDoblyEffect)
			  pCAudioDoblyEffect->SetParam(VO_PID_AUDIO_CHANNELCONFIG, (VO_PTR)pComponentConfigStructure);
		  errType = OMX_ErrorNone;
		  break;
		
	  default: 
		  errType = voCOMXCompFilter::SetConfig (hComponent, nIndex, pComponentConfigStructure);
		  break;
	}

	return errType;
}

OMX_ERRORTYPE voCOMXAudioDobyEffect::ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
													OMX_OUT OMX_U8 *cRole,
													OMX_IN OMX_U32 nIndex)
{
	strcat((char*)cRole, "\0");

	strcpy ((char *)cRole, "audio_effect.dobly");

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioDobyEffect::SetNewState (OMX_STATETYPE newState)
{
	VOLOGF ("Name: %s. New State %d", m_pName, newState);

	if (newState == OMX_StateIdle && m_sTrans == COMP_TRANSSTATE_LoadedToIdle)
	{
		if (m_nInSampleRate < 8000)
			m_nInSampleRate = 44100;
		if (m_nInChannels == 0)
			m_nInChannels = 2;

		if (pCAudioDoblyEffect == NULL)
		{
			pCAudioDoblyEffect = new CAudioDEffect;
			if (pCAudioDoblyEffect == NULL)
			{
				VOLOGE ("Create the audio decoder failed.");
				return OMX_ErrorInsufficientResources;
			}
		}

		pCAudioDoblyEffect->SetOMXComp (VO_TRUE);

		VO_AUDIO_FORMAT	fmtAudio;
		fmtAudio.Channels = m_nInChannels;
		fmtAudio.SampleRate = m_nInSampleRate;
		fmtAudio.SampleBits = 16;

		m_nStepSize = m_nInSampleRate * m_nInChannels * 2 / 5;

		VO_U32 nRC = pCAudioDoblyEffect->Init (&fmtAudio);
		if (nRC != VO_ERR_NONE)
		{
			VOLOGE ("pCAudioDoblyEffect->Init was failed. 0X%08X", nRC);
			return OMX_ErrorResourcesLost;
		}

		m_nChannels = m_nInChannels;
		m_nSampleRate = m_nInSampleRate;
	}
	else if (newState == OMX_StateExecuting && m_sTrans == COMP_TRANSSTATE_IdleToExecute)
	{
		if (pCAudioDoblyEffect != NULL)
			pCAudioDoblyEffect->Flush ();

		m_inBuffer.Length = 0;
	}

	return voCOMXCompFilter::SetNewState (newState);
}

OMX_ERRORTYPE voCOMXAudioDobyEffect::Flush (OMX_U32	nPort)
{
	VOLOGF ("Name: %s", m_pName);

	if (pCAudioDoblyEffect != NULL)
		pCAudioDoblyEffect->Flush ();

	m_inBuffer.Length = 0;
	m_nStartTime = 0;
	m_nBuffSize = 0;

	return voCOMXCompFilter::Flush (nPort);
}

OMX_ERRORTYPE voCOMXAudioDobyEffect::FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled)
{
	VOLOGR ("Name: %s. Size: %d, Time %d", m_pName, pInput->nFilledLen, (int)pInput->nTimeStamp);
	VO_AUDIO_FORMAT	fmtAudio;

	if (!m_bSetThreadPriority)
	{
		voOMXThreadSetPriority (m_hBuffThread, VOOMX_THREAD_PRIORITY_ABOVE_NORMAL);
		m_bSetThreadPriority = OMX_TRUE;
	}

	OMX_U32 nRC = 0;

	if(m_nInChannels != m_nChannels || m_nInSampleRate != m_nSampleRate)
	{
		fmtAudio.Channels = m_nInChannels;
		fmtAudio.SampleRate = m_nInSampleRate;
		fmtAudio.SampleBits = 16;

		pCAudioDoblyEffect->SetParam(VO_PID_AUDIO_FORMAT, &fmtAudio);

		m_nChannels = m_nInChannels;
		m_nSampleRate = m_nInSampleRate;
	}

	m_inBuffer.Buffer = pInput->pBuffer + pInput->nOffset;
	m_inBuffer.Length = pInput->nFilledLen;

	if (m_nBuffSize == 0)
		m_nStartTime = pInput->nTimeStamp;

	m_outBuffer.Buffer = pOutput->pBuffer + m_nBuffSize;
	m_outBuffer.Length = pOutput->nAllocLen -  m_nBuffSize;

	nRC = pCAudioDoblyEffect->Process(&m_inBuffer, &m_outBuffer); 

	pCAudioDoblyEffect->GetParam(VO_PID_AUDIO_FORMAT, &fmtAudio);
	if (((int)fmtAudio.Channels != (int)m_pcmType.nChannels || (int)fmtAudio.SampleRate != (int)m_pcmType.nSamplingRate ||
		(int)fmtAudio.Channels != (int)m_nOutChannels || (int)fmtAudio.SampleRate != (int)m_nOutSampleRate))
	{
		pOutput->nFilledLen = 0;
		m_pOutput->ReturnBuffer (pOutput);
		m_pOutputBuffer = NULL;
		*pFilled = OMX_FALSE;

		m_pcmType.nSamplingRate = fmtAudio.SampleRate;
		m_pcmType.nChannels = fmtAudio.Channels;

		m_nStepSize = fmtAudio.SampleRate * fmtAudio.Channels * 2 / 5;

		if (m_pCallBack != NULL)
		{
			m_pCallBack->EventHandler (m_pComponent, m_pAppData, OMX_EventPortSettingsChanged, 1, 0, NULL);
		}

		return OMX_ErrorNone;
	}

	VOLOGR ("Name: %s. GetOutputData Size: %d, Result %d", m_pName, m_outBuffer.Length, nRC);

    if(pOutput->nFlags & OMX_BUFFERFLAG_EOS)
	{
		if(nRC == VO_ERR_NONE && m_outBuffer.Length > 0)
		{
			m_nBuffSize = m_nBuffSize + m_outBuffer.Length;
		}

		pOutput->nFilledLen  = m_nBuffSize;
		pOutput->nTimeStamp = m_nStartTime;

		m_nStartTime = m_nStartTime + (m_nOutBuffSize * 1000) / (m_nSampleRate * m_nChannels * 2);
		m_nBuffSize = 0;

		*pFilled = OMX_TRUE;

		*pEmptied = OMX_TRUE;
		m_inBuffer.Length = 0;	
	}
	else if (nRC == VO_ERR_NONE && m_outBuffer.Length > 0)
	{
		pOutput->nFilledLen = m_outBuffer.Length;
		m_nBuffSize = m_nBuffSize + m_outBuffer.Length;

		if (m_nBuffSize >= m_nStepSize)
		{
			pOutput->nFilledLen  = m_nBuffSize;
			pOutput->nTimeStamp = m_nStartTime;

			m_nStartTime = m_nStartTime + (m_nBuffSize * 1000) / (m_nSampleRate * m_nChannels * 2);
			m_nBuffSize = 0;

			*pFilled = OMX_TRUE;
		}
		else
		{
			*pFilled = OMX_FALSE;
		}
	}
	else
	{
		pOutput->nFilledLen = 0;
		*pFilled = OMX_FALSE;
	}

	*pEmptied = OMX_TRUE;

	return OMX_ErrorNone;
}

OMX_ERRORTYPE voCOMXAudioDobyEffect::InitPortType (void)
{
	VOLOGF ("Name: %s", m_pName);

	m_portParam[OMX_PortDomainAudio].nPorts = 2;
	m_portParam[OMX_PortDomainAudio].nStartPortNumber = 0;

	OMX_PARAM_PORTDEFINITIONTYPE * pPortType = NULL;
	m_pInput->GetPortType (&pPortType);

	pPortType->eDomain = OMX_PortDomainAudio;
	pPortType->nBufferCountActual = 2;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 44100 * 4;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;
	pPortType->format.audio.pNativeRender = NULL;
	pPortType->format.audio.bFlagErrorConcealment = OMX_FALSE;
	pPortType->format.audio.eEncoding = OMX_AUDIO_CodingPCM;

	m_pOutput->GetPortType (&pPortType);
	pPortType->eDomain = OMX_PortDomainAudio;
	pPortType->nBufferCountActual = 2;
	pPortType->nBufferCountMin = 1;
	pPortType->nBufferSize = 44100 * 4;
	pPortType->bBuffersContiguous = OMX_FALSE;
	pPortType->nBufferAlignment = 1;

	pPortType->format.audio.pNativeRender = NULL;
	pPortType->format.audio.bFlagErrorConcealment = OMX_FALSE;

	pPortType->format.audio.eEncoding = OMX_AUDIO_CodingPCM;

	voOMXBase_SetHeader (&m_pcmType, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
	m_pcmType.nPortIndex = 1;
	m_pcmType.nChannels = 2;
	m_pcmType.eNumData = OMX_NumericalDataSigned;
	m_pcmType.eEndian = OMX_EndianBig;
	m_pcmType.bInterleaved = OMX_TRUE;
	m_pcmType.nBitPerSample = 16;
	m_pcmType.nSamplingRate = 44100;
	m_pcmType.ePCMMode = OMX_AUDIO_PCMModeLinear;

	return OMX_ErrorNone;
}
