	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voOMXPortAudioConnect.cpp

	Contains:	voOMXPortAudioConnect class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined(_WIN32)
#  include <tchar.h>
#endif

#include <voOMXMemory.h>
#include <voOMX_Index.h>
#include <voOMX_Types.h>

#include "voOMXBase.h"
#include "voOMXOSFun.h"

#include "voOMXPortAudioConnect.h"

OMX_ERRORTYPE voOMXConnectAudioPort (OMX_COMPONENTTYPE * pPrev, OMX_U32 nOutputPort,
										OMX_COMPONENTTYPE * pNext, OMX_U32 nInputPort)
{
	if (pPrev == NULL || pNext == NULL)
		return OMX_ErrorBadParameter;

	OMX_ERRORTYPE		errType = OMX_ErrorNone;
	OMX_S8				szName[128];
	OMX_VERSIONTYPE		verMainPrev;
	OMX_VERSIONTYPE		verMainNext;
	OMX_VERSIONTYPE		verSpec;
	OMX_UUIDTYPE		uuidType;

	verMainPrev.s.nVersionMajor = SPEC_VERSION_MAJOR;
	verMainPrev.s.nVersionMinor = SPEC_VERSION_MINOR;
	verMainPrev.s.nRevision = SPEC_REVISION;
	verMainPrev.s.nStep = SPEC_STEP;
	verMainNext.s.nVersionMajor = SPEC_VERSION_MAJOR;
	verMainNext.s.nVersionMinor = SPEC_VERSION_MINOR;
	verMainNext.s.nRevision = SPEC_REVISION;
	verMainNext.s.nStep = SPEC_STEP;

	if (pPrev->GetComponentVersion != NULL)
	{
		errType = pPrev->GetComponentVersion (pPrev, (OMX_STRING) szName, &verMainPrev, &verSpec, &uuidType);
		if (errType != OMX_ErrorNone)
			return errType;
	}

	if (pNext->GetComponentVersion != NULL)
	{
		errType = pNext->GetComponentVersion (pNext, (OMX_STRING) szName, &verMainNext, &verSpec, &uuidType);
		if (errType != OMX_ErrorNone)
			return errType;
	}

	OMX_PARAM_PORTDEFINITIONTYPE portType;
	voOMXBase_SetHeader (&portType, sizeof (OMX_PARAM_PORTDEFINITIONTYPE), &verMainPrev);
	portType.nPortIndex = nOutputPort;

	errType = pPrev->GetParameter (pPrev, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
		return errType;

	if (portType.eDomain != OMX_PortDomainAudio)
		return OMX_ErrorPortsNotCompatible;

	portType.nPortIndex = nInputPort;
	errType = pNext->SetParameter (pNext, OMX_IndexParamPortDefinition, &portType);
	if (errType != OMX_ErrorNone)
		return errType;

	OMX_AUDIO_PARAM_PORTFORMATTYPE tpAudioFormat;
	voOMXBase_SetHeader (&tpAudioFormat, sizeof (OMX_AUDIO_PARAM_PORTFORMATTYPE));
	tpAudioFormat.nPortIndex = nOutputPort;

	errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioPortFormat, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
		return errType;

	tpAudioFormat.nPortIndex = nInputPort;
	errType = pNext->SetParameter (pNext, OMX_IndexParamAudioPortFormat, &tpAudioFormat);
	if (errType != OMX_ErrorNone)
		return errType;

	if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingPCM)
	{
		OMX_AUDIO_PARAM_PCMMODETYPE tpPCMFormat;
		voOMXBase_SetHeader (&tpPCMFormat, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
		tpPCMFormat.nPortIndex = nOutputPort;

		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioPcm, &tpPCMFormat);
		if (errType != OMX_ErrorNone)
			return errType;

		tpPCMFormat.nPortIndex = nInputPort;
		errType = pNext->SetParameter (pNext, OMX_IndexParamAudioPcm, &tpPCMFormat);
		if (errType != OMX_ErrorNone)
		{
			voOMXBase_SetHeader (&tpPCMFormat, sizeof (OMX_AUDIO_PARAM_PCMMODETYPE));
			tpPCMFormat.nPortIndex = nInputPort;
			errType = pNext->GetParameter (pNext, OMX_IndexParamAudioPcm, &tpPCMFormat);
			if (errType != OMX_ErrorNone)
				return errType;

			tpPCMFormat.nPortIndex = nOutputPort;
			errType = pPrev->SetParameter (pPrev, OMX_IndexParamAudioPcm, &tpPCMFormat);
			if (errType != OMX_ErrorNone)
				return errType;
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingEVRC)
	{
		OMX_AUDIO_PARAM_EVRCTYPE tpEVRCFormat;
		voOMXBase_SetHeader (&tpEVRCFormat, sizeof (OMX_AUDIO_PARAM_EVRCTYPE));
		tpEVRCFormat.nPortIndex = nOutputPort;

		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioEvrc, &tpEVRCFormat);
		if (errType == OMX_ErrorNone)
		{
			tpEVRCFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioEvrc, &tpEVRCFormat);
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingMP3)
	{
		OMX_AUDIO_PARAM_MP3TYPE tpMP3Format;
		voOMXBase_SetHeader (&tpMP3Format, sizeof (OMX_AUDIO_PARAM_MP3TYPE));
		tpMP3Format.nPortIndex = nOutputPort;

		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioMp3, &tpMP3Format);
		if (errType == OMX_ErrorNone)
		{
			tpMP3Format.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioMp3, &tpMP3Format);
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingRA)
	{
		OMX_AUDIO_PARAM_RATYPE tpRAFormat;
		voOMXBase_SetHeader (&tpRAFormat, sizeof (OMX_AUDIO_PARAM_RATYPE));
		tpRAFormat.nPortIndex = nOutputPort;

		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioRa, &tpRAFormat);
		if (errType == OMX_ErrorNone)
		{
			tpRAFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioRa, &tpRAFormat);
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingAAC)
	{
		OMX_AUDIO_PARAM_AACPROFILETYPE tpAACFormat;
		voOMXBase_SetHeader (&tpAACFormat, sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE));
		tpAACFormat.nPortIndex = nOutputPort;

		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioAac, &tpAACFormat);
		if (errType == OMX_ErrorNone)
		{
			tpAACFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioAac, &tpAACFormat);
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingAMR)
	{
		OMX_AUDIO_PARAM_AMRTYPE tpAmrFormat;
		voOMXBase_SetHeader (&tpAmrFormat, sizeof (OMX_AUDIO_PARAM_AMRTYPE));
		tpAmrFormat.nPortIndex = nOutputPort;

		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioAmr, &tpAmrFormat);
		if (errType == OMX_ErrorNone)
		{
			tpAmrFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioAmr, &tpAmrFormat);
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingWMA)
	{
		OMX_AUDIO_PARAM_WMATYPE tpWMAFormat;
		voOMXBase_SetHeader (&tpWMAFormat, sizeof (OMX_AUDIO_PARAM_WMATYPE));
		tpWMAFormat.nPortIndex = nOutputPort;

		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioWma, &tpWMAFormat);
		if (errType == OMX_ErrorNone)
		{
			tpWMAFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioWma, &tpWMAFormat);
		}
	}
	else if (tpAudioFormat.eEncoding == OMX_AUDIO_CodingADPCM)
	{
		OMX_AUDIO_PARAM_ADPCMTYPE tpADPCMFormat;
		voOMXBase_SetHeader (&tpADPCMFormat, sizeof (OMX_AUDIO_PARAM_ADPCMTYPE));
		tpADPCMFormat.nPortIndex = nOutputPort;

		errType = pPrev->GetParameter (pPrev, OMX_IndexParamAudioAdpcm, &tpADPCMFormat);
		if (errType == OMX_ErrorNone)
		{
			tpADPCMFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, OMX_IndexParamAudioAdpcm, &tpADPCMFormat);
		}
	}
	else if(tpAudioFormat.eEncoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingDTS)
	{
		OMX_VO_AUDIO_PARAM_DTSTYPE tpDTSFormat;

		errType = pPrev->GetParameter (pPrev, (OMX_INDEXTYPE)OMX_VO_IndexAudioParamDTS, &tpDTSFormat);
		if (errType == OMX_ErrorNone)
		{
			tpDTSFormat.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, (OMX_INDEXTYPE)OMX_VO_IndexAudioParamDTS, &tpDTSFormat);
		}
	}
	else if(tpAudioFormat.eEncoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingAC3 || tpAudioFormat.eEncoding == (OMX_AUDIO_CODINGTYPE)OMX_VO_AUDIO_CodingEAC3)
	{
		OMX_VO_AUDIO_PARAM_AC3TYPE tpAC3Format;

		errType = pPrev->GetParameter (pPrev, (OMX_INDEXTYPE)OMX_VO_IndexAudioParamAC3, &tpAC3Format);
		if (errType == OMX_ErrorNone)
		{
			tpAC3Format.nPortIndex = nInputPort;
			errType = pNext->SetParameter (pNext, (OMX_INDEXTYPE)OMX_VO_IndexAudioParamAC3, &tpAC3Format);
		}
	}

	OMX_U32 uDataSize = 256;
	OMX_OTHER_EXTRADATATYPE * pExtData = (OMX_OTHER_EXTRADATATYPE *)voOMXMemAlloc (sizeof (OMX_OTHER_EXTRADATATYPE) + uDataSize);
	if (pExtData == NULL)
		return OMX_ErrorNone;

	voOMXBase_SetHeader (pExtData, sizeof (OMX_OTHER_EXTRADATATYPE) + uDataSize);
	pExtData->nPortIndex = nOutputPort;
	pExtData->nDataSize = uDataSize;

	errType = pPrev->GetParameter (pPrev, OMX_IndexParamCommonExtraQuantData, pExtData);
	if (errType == OMX_ErrorNone && pExtData->nDataSize > 0)
	{
		pExtData->nPortIndex = nInputPort;
		errType = pNext->SetParameter (pNext, OMX_IndexParamCommonExtraQuantData, pExtData);
	}
	voOMXMemFree (pExtData);

	return OMX_ErrorNone;
}

